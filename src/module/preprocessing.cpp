/*
 * Copyright UVSQ - CEA/DAM/DIF (2018)
 * Contributors:  Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *                Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

//! \file	preprocessing.cpp
//! \brief	Module which handles the simulation launching and the request
//!			routing to the Volume Drivers.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "module/preprocessing.hpp"

#include "parser/xmlparser.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC MEMBER FUNCTIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Preprocessing::Preprocessing (
	const OGSS_String		configurationFile):
	Module (configurationFile, make_pair (MTP_PREPROCESSING, 0) ) {
	cout << "STEP 1 - Extraction" << endl;
	cout << "-\tStart extracting " << _cfg << endl;
}

Preprocessing::~Preprocessing () {  }

void
Preprocessing::processExtraction () {
	receiveData ();

	DLOG(INFO) << "[PP] Request for " << _hardParam._numVolumes << " volumes";

	_numRealVolumes = 0;
	for (auto i = 0; i < _hardParam._numVolumes; ++i) {
		i += _volumes [i] .first._numSubVolumes;
		++ _numRealVolumes;
	}

	DLOG(INFO) << "I: " << _hardParam._numInterfaces << " - T:" << _hardParam._numTiers
		<< " - V:" << _hardParam._numVolumes << " - D:" << _hardParam._numDevices;

	updateVolumeMapping ();

	for (auto elt: _redirectionTable)
		DLOG(INFO) << "Redirection table [" << elt.first << "] -> " << elt.second;

	LOG(INFO) << "[PP] Reception of " << _events.size () << " events.";
	
	cout << "-\tEnd" << endl;
}

void
Preprocessing::processDecomposition () {
	OGSS_Bool					unfinished {true};
	Request						req;
	vector <Request>			reqs;
	void						* arg;

	manageEvents ();

	cout << "STEP 2 - Decomposition" << endl;
	cout << "-\tStart" << endl;

	while (unfinished) {
		_ci->receive (arg);
		req = * static_cast <Request *> (arg); free (arg);

		if (req._type == RQT_END)
			{ unfinished = false; continue;	}

		redirectRequest (req, reqs);

		_ci->send (make_pair (MTP_SYNCHRONIZATION, 0),
			&req, sizeof (Request) );

		for (auto & elt: reqs) {
			_ci->send (make_pair (MTP_VOLUME, elt._idxVolume),
				&elt, sizeof (Request) );
		}

		reqs.clear ();
	}

	LOG (INFO) << "[PP] Distribution done";

	endSimulation ();
}

void
Preprocessing::processSynchronization () {  }

void
Preprocessing::endSimulation () {
	Request					endRequest;
	DLOG (INFO) << "Now tell the modules to stop";

	endRequest._type = RQT_END;

	for (auto i = 0; i < _numRealVolumes; ++i) {
//		DLOG(INFO) << "[PP] Send ending to VD" << i;
		_ci->send (make_pair (MTP_VOLUME, i),
			&endRequest, sizeof (endRequest) );
	}

	DLOG(INFO) << "[PP] Send ending to SC";
	_ci->send (make_pair (MTP_SYNCHRONIZATION, 0), &endRequest,
		sizeof (endRequest) ); 
}

void
Preprocessing::updateVolumeMapping () {
	OGSS_Ulong				systemSize = 0;
	OGSS_Ushort				threadCounter = 0;
	OGSS_Ushort				volumeCounter = 0;
	OGSS_Ushort				subvolumeCounter = 0;
	OGSS_Ushort				numDataDevices;

	for (auto & elt: _volumes) {
		switch (elt.first._type) {
		case VTP_JBOD:
			numDataDevices = elt.first._numDevices;
			break;
		case VTP_RAID1: case VTP_RAID01:
			numDataDevices = elt.first._numDevices / 2;
			break;
		case VTP_RAIDNP:
			numDataDevices = elt.first._numDevices
				- elt.first._numRedundancyDevices;
			break;
		case VTP_DECRAID:
			numDataDevices = 0;
			subvolumeCounter = 1 + elt.first._numSubVolumes;
			_redirectionTable [volumeCounter] = 0;
			break;
		default: break;
		}

		systemSize += (numDataDevices * elt.second._physicalCapacity);

		if (!subvolumeCounter) {
			++threadCounter;
			_redirectionTable [volumeCounter] = systemSize;
		}
		else {
			--subvolumeCounter;
			if (!subvolumeCounter) {
				_redirectionTable.rbegin () ->second = systemSize;
			}
		}
		++volumeCounter;
	}
}

void
Preprocessing::redirectRequest (
	Request					& request,
	vector <Request>		& childRequests) {
	OGSS_Ulong				address;
	OGSS_Ulong				previousValue = 0;
	OGSS_Ulong				remainingSize;
	OGSS_Ulong				cnt = 0;

	address = request._address;
	remainingSize = request._size;

	for (auto elt: _redirectionTable) {
		if (address < elt.second) {
#ifndef UTEST
//			_ci->send (make_pair (MTP_VOLUME, elt.second),
//				&idxRequest, sizeof (idxRequest) );
#endif

			Request r (request);
			r._numChild = 0;
			r._size = min (remainingSize, elt.second - address);
			r._volumeAddress = address - previousValue;
			r._idxVolume = elt.first;
			r._mainIdx = request._mainIdx;
			r._majrIdx = ++cnt;
			r._minrIdx = 0;
			if (request._type == RQT_READ)
				r._transferTimeB1 = request._size;
			else
				r._transferTimeA1 = request._size;
			++ request._numChild;
			
			childRequests.push_back (r);

			if (elt.second - address >= remainingSize) {
				break;
			}

			remainingSize -= (elt.second - address);
			address = elt.second;
		}

		previousValue = elt.second;
	}
	if(!childRequests.size()){
		LOG(WARNING) << "Request [" << request._mainIdx << "] is out of the logical address space: "
			<< request._address << "<?" << _redirectionTable.rbegin()->second;
	}
}

void
Preprocessing::manageEvents () {
	Request r;
	OGSS_Long i, n, cnt = 1;

	for (auto elt: _events) {
		if (elt._type == EVT_FAULT)
			r._type = RQT_EVFLT;
		if (elt._type == EVT_REPLC)
			r._type = RQT_EVRPL;
		
		r._date = elt._date;
		r._idxDevice = elt._target;

		r._mainIdx = OGSS_ULONG_MAX;
		r._majrIdx = cnt++;
		r._minrIdx = 0;

		n = i = 0;
		for (auto flt: _volumes) {
			n += flt.first._numDevices;
			if (elt._target < n) {
				r._idxVolume = i;
				break;
			}
			++i;
		}

		_ci->send (make_pair (MTP_VOLUME, i), &r, sizeof (r) );
	}
}

/*----------------------------------------------------------------------------*/
/* UNITARY TEST --------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

UT_Preprocessing::UT_Preprocessing (
	const OGSS_String		& configurationFile):
	UnitaryTest <UT_Preprocessing> (MTP_PREPROCESSING) {
	set <OGSS_String>		testNames;

	XMLParser::getListOfRequestedUnitaryTests (
		configurationFile, _module, testNames);

	for (auto & elt: testNames) {
		if (! elt.compare ("all") ) {
			_tests.push_back (make_pair ("One request - middle of volume",
				&UT_Preprocessing::middleVolRequest) );
			_tests.push_back (make_pair ("One request - all the volume",
				&UT_Preprocessing::fullVolRequest) );
			_tests.push_back (make_pair ("One request - start of volume",
				&UT_Preprocessing::startVolRequest) );
			_tests.push_back (make_pair ("One request - end of volume",
				&UT_Preprocessing::endVolRequest) );
			_tests.push_back (make_pair ("One request - multiple volumes",
				&UT_Preprocessing::multiVolRequest) );
		}
		else if (! elt.compare ("middleVolRequest") )
			_tests.push_back (make_pair ("One request - middle of volume",
				&UT_Preprocessing::middleVolRequest) );
		else if (! elt.compare ("fullVolRequest") )
			_tests.push_back (make_pair ("One request - all the volume",
				&UT_Preprocessing::fullVolRequest) );
		else if (! elt.compare ("startVolRequest") )
			_tests.push_back (make_pair ("One request - start of volume",
				&UT_Preprocessing::startVolRequest) );
		else if (! elt.compare ("endVolRequest") )
			_tests.push_back (make_pair ("One request - end of volume",
				&UT_Preprocessing::endVolRequest) );
		else if (! elt.compare ("multiVolRequest") )
			_tests.push_back (make_pair ("One request - multiple volumes",
				&UT_Preprocessing::multiVolRequest) );
		else
			LOG (WARNING) << ModuleNameMap.at (_module) << " unitary test "
				<< "named '" << elt << "' does not match!" << endl;
	}
}

UT_Preprocessing::~UT_Preprocessing () {  }

//! \brief	Create a hardware data structure for preprocessing unitary tests.
//! \param	h				Created hardware.
void
initTestArchi01 (
	shared_ptr <Hardware>	h) {

	h->_tiers.push_back (Tier () );
	h->_param._numTiers = 1;
	h->_param._numVolumes = 3;

	for (auto i = 0; i < 3; ++i)
		h->_volumes.push_back (Volume () );

	for (auto & elt: h->_volumes) {
		elt._type = VTP_JBOD;
		elt._numDevices = 1;
		elt._numRedundancyDevices = 0;

		h->_devices.push_back (Device () );
		h->_devices [0] ._physicalCapacity = 4;
	}
}

OGSS_Bool
UT_Preprocessing::middleVolRequest () {
	Preprocessing			module ("env/conf/_ut_config.xml");
	shared_ptr <Hardware>	hard = make_shared <Hardware> ();
	vector <Request>		reqs;
	vector <Request>		res;

	initTestArchi01 (hard);
	reqs.push_back (Request (make_tuple (.1, RQT_READ, 25, 10, 0, 0) ) );

	module._hardParam = hard->_param;
	module._volumes.push_back (make_pair (
		hard->_volumes [0], hard->_devices [0] ) );
	module._volumes.push_back (make_pair (
		hard->_volumes [1], hard->_devices [1] ) );
	module._volumes.push_back (make_pair (
		hard->_volumes [2], hard->_devices [2] ) );
	module.updateVolumeMapping ();

	module.redirectRequest (reqs [0], res);

	if (res.size () == 1
		&& res [0] ._volumeAddress == 5 && res [0] ._size == 10)
		return true;

	return false;
}

OGSS_Bool
UT_Preprocessing::fullVolRequest () {
	Preprocessing			module ("env/conf/_ut_config.xml");
	shared_ptr <Hardware>	hard = make_shared <Hardware> ();
	vector <Request>		reqs;
	vector <Request>		res;

	initTestArchi01 (hard);
	reqs.push_back (Request (make_tuple (.1, RQT_READ, 20, 20, 0, 0) ) );

	module._hardParam = hard->_param;
	module._volumes.push_back (make_pair (
		hard->_volumes [0], hard->_devices [0] ) );
	module._volumes.push_back (make_pair (
		hard->_volumes [1], hard->_devices [1] ) );
	module._volumes.push_back (make_pair (
		hard->_volumes [2], hard->_devices [2] ) );
	module.updateVolumeMapping ();

	module.redirectRequest (reqs [0], res);

	if (res.size () == 1
		&& res [0] ._volumeAddress == 0 && res [0] ._size == 20)
		return true;

	return false;
}

OGSS_Bool
UT_Preprocessing::startVolRequest () {
	Preprocessing			module ("env/conf/_ut_config.xml");
	shared_ptr <Hardware>	hard = make_shared <Hardware> ();
	vector <Request>		reqs;
	vector <Request>		res;

	initTestArchi01 (hard);
	reqs.push_back (Request (make_tuple (.1, RQT_READ, 40, 15, 0, 0) ) );

	module._hardParam = hard->_param;
	module._volumes.push_back (make_pair (
		hard->_volumes [0], hard->_devices [0] ) );
	module._volumes.push_back (make_pair (
		hard->_volumes [1], hard->_devices [1] ) );
	module._volumes.push_back (make_pair (
		hard->_volumes [2], hard->_devices [2] ) );
	module.updateVolumeMapping ();

	module.redirectRequest (reqs [0], res);

	if (res.size () == 1
		&& res [0] ._volumeAddress == 0 && res [0] ._size == 15)
		return true;

	return false;
}

OGSS_Bool
UT_Preprocessing::endVolRequest () {
	Preprocessing			module ("env/conf/_ut_config.xml");
	shared_ptr <Hardware>	hard = make_shared <Hardware> ();
	vector <Request>		reqs;
	vector <Request>		res;

	initTestArchi01 (hard);
	reqs.push_back (Request (make_tuple (.1, RQT_READ, 5, 15, 0, 0) ) );

	module._hardParam = hard->_param;
	module._volumes.push_back (make_pair (
		hard->_volumes [0], hard->_devices [0] ) );
	module._volumes.push_back (make_pair (
		hard->_volumes [1], hard->_devices [1] ) );
	module._volumes.push_back (make_pair (
		hard->_volumes [2], hard->_devices [2] ) );
	module.updateVolumeMapping ();

	module.redirectRequest (reqs [0], res);

	if (res.size () == 1
		&& res [0] ._volumeAddress == 5 && res [0] ._size == 15)
		return true;

	return false;
}

OGSS_Bool
UT_Preprocessing::multiVolRequest () {
	Preprocessing			module ("env/conf/_ut_config.xml");
	shared_ptr <Hardware>	hard = make_shared <Hardware> ();
	vector <Request>		reqs;
	vector <Request>		res;

	initTestArchi01 (hard);
	reqs.push_back (Request (make_tuple (.1, RQT_READ, 25, 25, 0, 0) ) );

	module._hardParam = hard->_param;
	module._volumes.push_back (make_pair (
		hard->_volumes [0], hard->_devices [0] ) );
	module._volumes.push_back (make_pair (
		hard->_volumes [1], hard->_devices [1] ) );
	module._volumes.push_back (make_pair (
		hard->_volumes [2], hard->_devices [2] ) );
	module.updateVolumeMapping ();

	module.redirectRequest (reqs [0], res);

	if (res.size () == 2
		&& res [0] ._volumeAddress == 5 && res [0] ._size == 15
		&& res [1] ._volumeAddress == 0 && res [1] ._size == 10)
		return true;

	return false;
}
