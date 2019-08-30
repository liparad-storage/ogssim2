/*
 * Copyright UVSQ - CEA/DAM/DIF (2017-2018)
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

//! \file	raid01volctrl.cpp
//! \brief	Definition of the RAID 01 controller.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <set>

#include "controller/raid01volctrl.hpp"

#include "parser/xmlparser.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

using namespace std;

typedef set <Request, OGSS_Bool (*) (Request, Request)>
										__RequestSet;

static OGSS_Bool
__requestCompare (
	Request								lhs,
	Request								rhs) {
	return (lhs._type == RQT_READ && rhs._type == RQT_WRITE)
		|| (lhs._type == rhs._type && lhs._idxDevice < rhs._idxDevice)
		|| (lhs._type == rhs._type && lhs._idxDevice == rhs._idxDevice && lhs._deviceAddress < rhs._deviceAddress);
}

static void
__removeAndMerge (
	Request								& request,
	vector <Request>					& subrequests) {
	__RequestSet						tmp (__requestCompare);

	for (auto elt: subrequests)			tmp.insert (elt);
	subrequests.clear ();

	auto 								c = tmp.begin ();
	auto								p = c;
	OGSS_Bool							building {false};
	Request								flt;
	auto								idx {1};

	request._numChild = request._numPrioChild = 0;

	for (++c; c != tmp.end (); ++c, ++p) {
		if (! building) {
			flt = *p;
			building = true;
		}

		if (flt._type != c->_type
		 || flt._idxDevice != c->_idxDevice
		 || flt._deviceAddress + flt._size != c->_deviceAddress) {
		 	flt._minrIdx = idx ++;
			subrequests.push_back (flt);
			++ request._numChild;
			if (flt._prio) ++ request._numPrioChild;
			building = false;
			continue;
		}

		flt._size += c->_size;
	}

	++ request._numChild;
	if (building) {
		flt._minrIdx = idx;
		subrequests.push_back (flt);
		if (flt._prio) ++ request._numPrioChild;
	} else {
		subrequests.push_back (*tmp.rbegin () );
		subrequests.back () ._minrIdx = idx;
		if (tmp.rbegin () ->_prio) ++ request._numPrioChild;
	}
}


/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

RAID01VolCtrl::RAID01VolCtrl (
	const Volume			& vol,
	const Device			& dev) {
	_numBytesByDev			= dev._physicalCapacity;
	_numBytesBySU			= vol._suSize;
	_numDevices				= vol._numDevices;
	_mirrorChosen			= true;
	_failedDevices			= set <OGSS_Ushort> ();
	_failureDates			= map <OGSS_Ushort, OGSS_Real> ();
}
RAID01VolCtrl::~RAID01VolCtrl () {  }

void
RAID01VolCtrl::decompose (
	Request					& request,
	vector <Request>		& subrequests) {
	OGSS_Ulong				addr;
	OGSS_Ulong				remainingSize;
	OGSS_Ulong				cnt = 0;
	
	addr = request._volumeAddress;
	remainingSize = request._size;

	while (remainingSize > 0) {
		Request				sr {request}, srm {request};

		sr._size = min (_numBytesBySU - addr % _numBytesBySU, remainingSize);
		sr._deviceAddress = (addr / (_numBytesBySU * (_numDevices / 2) )
			* _numBytesBySU) + addr % _numBytesBySU;
		sr._idxDevice = (addr / _numBytesBySU) % (_numDevices / 2);
		
		sr._mainIdx = request._mainIdx;
		sr._majrIdx = request._majrIdx;
		sr._minrIdx = ++cnt;
		sr._type = request._type;
		sr._prio = false; srm._prio = false;

		++ request._numChild;

		if (request._type == RQT_READ) {
			sr._idxDevice = _mirrorChosen ? sr._idxDevice
				: sr._idxDevice + _numDevices / 2;

			if (_failedDevices.find (sr._idxDevice) != _failedDevices.end ()
				&& _failureDates.at (sr._idxDevice) <= sr._date)
				sr._idxDevice = (!_mirrorChosen) ? sr._idxDevice
					: sr._idxDevice + _numDevices / 2;

			request._idxDevice = sr._idxDevice;
			request._deviceAddress = sr._deviceAddress;

			subrequests.push_back (sr);
		} else {
			srm = sr;

			srm._idxDevice = sr._idxDevice + _numDevices / 2;
			srm._minrIdx = ++cnt;

			++ request._numChild;

			if (_failedDevices.find (sr._idxDevice) == _failedDevices.end ()
				|| _failureDates.at (sr._idxDevice) > sr._date) {
				subrequests.push_back (sr);
			} else -- request._numChild;
			if (_failedDevices.find (srm._idxDevice) == _failedDevices.end ()
				|| _failureDates.at (srm._idxDevice) > srm._date) {
				subrequests.push_back (srm);
			} else -- request._numChild;

			request._idxDevice = sr._idxDevice;
			request._deviceAddress = sr._deviceAddress;
		}

		addr += sr._size;
		remainingSize -= sr._size;
	}

	if (request._type == RQT_READ) _mirrorChosen = ! _mirrorChosen;

	__removeAndMerge (request, subrequests);
}

void
RAID01VolCtrl::updateScheme (
	const Request			& event) {
	VolumeController::updateScheme (event);
	
	LOG(INFO) << "RAID1 receives an event on local disk #" << event._idxDevice;

	if (event._type == RQT_EVFLT) {
		_failedDevices.insert (event._idxDevice);
		_failureDates [event._idxDevice] = event._date;
	}
}

void
RAID01VolCtrl::generateDecraidFailureRequests (
	Request								& block,
	vector <Request>					& subrequests) {
	Request								reqR {block};
	Request								reqW {block};

	if (block._idxDevice < _numDevices / 2) {
		reqR._idxDevice += _numDevices / 2;
	} else {
		reqR._idxDevice -= _numDevices / 2;
	}

	reqR._type = RQT_READ;				reqW._type = RQT_WRITE;
	reqR._numPrioChild = 1;				reqW._numPrioChild = 0;
	reqR._operation = ROP_RECOVERY;		reqW._operation = ROP_UPDATE;

	subrequests.push_back (reqR);		subrequests.push_back (reqW);
}

void
RAID01VolCtrl::generateDecraidRenewalRequests (
	Request								& block,
	vector <Request>					& subrequests) {
	Request								reqR {block};
	Request								reqC {block};
	Request								reqW {block};

	if (block._idxDevice < _numDevices / 2) {
		reqR._idxDevice += _numDevices / 2;
	} else {
		reqR._idxDevice -= _numDevices / 2;
	}

	reqR._type = RQT_READ;				reqW._type = RQT_WRITE;
	reqR._numPrioChild = 1;				reqW._numPrioChild = 0;
	reqR._operation = ROP_RECOVERY;		reqW._operation = ROP_RENEW;

	subrequests.push_back (reqR);		subrequests.push_back (reqW);

	reqC._type = RQT_READ;
	reqC._numPrioChild = 1;
	reqC._operation = ROP_COPY;

	subrequests.push_back (reqC);
}

void
RAID01VolCtrl::generateFailureRequests (
	Request								& block,
	vector <Request>					& subrequests) {  }

void
RAID01VolCtrl::generateRenewalRequests (
	Request								& block,
	vector <Request>					& subrequests) {
	Request								reqR {block};
	Request								reqW {block};

	if (block._idxDevice < _numDevices / 2) {
		reqR._idxDevice += _numDevices / 2;
	} else {
		reqR._idxDevice -= _numDevices / 2;
	}

	reqR._type = RQT_READ;				reqW._type = RQT_WRITE;
	reqR._numPrioChild = 1;				reqW._numPrioChild = 0;
	reqR._operation = ROP_RECOVERY;		reqW._operation = ROP_RENEW;

	subrequests.push_back (reqR);		subrequests.push_back (reqW);
}

void
RAID01VolCtrl::requestMultiplier (
	Request								& block,
	vector <Request>					& subrequests) {
	Request								sr {block};

	sr._numPrioChild = 0;
	sr._numChild = 0;
	sr._multiple = true;

	if (block._type == RQT_READ) {
		sr._operation = ROP_NATIVE;		sr._prio = true;
										subrequests.push_back (sr);

		Request							srm {sr};
		srm._operation = ROP_RECOVERY;	srm._idxDevice = (srm._idxDevice + _numDevices / 2) % _numDevices;
										subrequests.push_back (srm);
		sr._operation = ROP_COPY;		subrequests.push_back (sr);

		sr._operation = ROP_UPDATE;		sr._type = RQT_WRITE;
										sr._prio = false;
										subrequests.push_back (sr);
		sr._operation = ROP_RENEW;		subrequests.push_back (sr);

		block._numPrioChild = 3;
		block._numChild = 5;
	} else {
		sr._operation = ROP_NATIVE;		subrequests.push_back (sr);

		// No recovery on write
		// No copy on write

		sr._operation = ROP_UPDATE;		subrequests.push_back (sr);
		sr._operation = ROP_RENEW;		subrequests.push_back (sr);

		block._numPrioChild = 0;
		block._numChild = 3;
	}
}

void
RAID01VolCtrl::getBlockLocation (
	Request					& block) {
	block._idxDevice = (block._volumeAddress / _numBytesBySU) % (_numDevices / 2);
	block._deviceAddress = (block._volumeAddress
		/ (_numBytesBySU * _numDevices / 2) ) * _numBytesBySU;
}

/*----------------------------------------------------------------------------*/
/* UNITARY TEST --------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

UT_RAID01VolCtrl::UT_RAID01VolCtrl (
	const OGSS_String		& configurationFile):
	UnitaryTest <UT_RAID01VolCtrl> (MTP_VOLRAID01) {
	set <OGSS_String>		testNames;

	XMLParser::getListOfRequestedUnitaryTests (
		configurationFile, _module, testNames);

	for (auto & elt: testNames) {
		if (! elt.compare ("all") ) {
			_tests.push_back (make_pair ("Small stripe request",
				&UT_RAID01VolCtrl::smallStripeRequest) );
			_tests.push_back (make_pair ("Large stripe request",
				&UT_RAID01VolCtrl::largeStripeRequest) );
			_tests.push_back (make_pair ("Full stripe request",
				&UT_RAID01VolCtrl::fullStripeRequest) );
			_tests.push_back (make_pair ("Two stripe request",
				&UT_RAID01VolCtrl::twoStripeRequest) );
			_tests.push_back (make_pair ("More stripe request",
				&UT_RAID01VolCtrl::moreStripeRequest) );
			_tests.push_back (make_pair ("Read requests",
				&UT_RAID01VolCtrl::readRequests) );
			_tests.push_back (make_pair ("Write request",
				&UT_RAID01VolCtrl::writeRequest) );
		} else if (! elt.compare ("smallStripe") )
			_tests.push_back (make_pair ("Small stripe request",
				&UT_RAID01VolCtrl::smallStripeRequest) );
		else if (! elt.compare ("largeStripe") )
			_tests.push_back (make_pair ("Large stripe request",
				&UT_RAID01VolCtrl::largeStripeRequest) );
		else if (! elt.compare ("fullStripe") )
			_tests.push_back (make_pair ("Full stripe request",
				&UT_RAID01VolCtrl::fullStripeRequest) );
		else if (! elt.compare ("twoStripe") )
			_tests.push_back (make_pair ("Two stripe request",
				&UT_RAID01VolCtrl::twoStripeRequest) );
		else if (! elt.compare ("moreStripe") )
			_tests.push_back (make_pair ("More stripe request",
				&UT_RAID01VolCtrl::moreStripeRequest) );
		else if (! elt.compare ("readRequests") )
			_tests.push_back (make_pair ("Read requests",
				&UT_RAID01VolCtrl::readRequests) );
		else if (! elt.compare ("writeRequest") )
			_tests.push_back (make_pair ("Write request",
				&UT_RAID01VolCtrl::writeRequest) );
		else
			LOG (WARNING) << ModuleNameMap.at (_module) << " unitary test "
				<< "named '" << elt << "' does not match!";
	}
}

UT_RAID01VolCtrl::~UT_RAID01VolCtrl () {  }

OGSS_Bool
UT_RAID01VolCtrl::smallStripeRequest () {
	RAID01VolCtrl			module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._mirrorChosen = true;

	req._size = 2;
	req._volumeAddress = 12;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 1) return false;

	if (sr.at (0) ._size != 2 || sr.at (0) ._deviceAddress != 4
		|| sr.at (0) ._idxDevice != 1) return false;

	return true;
}

OGSS_Bool
UT_RAID01VolCtrl::largeStripeRequest () {
	RAID01VolCtrl			module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._mirrorChosen = true;

	req._size = 6;
	req._volumeAddress = 16;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 2) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 8
		|| sr.at (0) ._idxDevice != 0) return false;
	if (sr.at (1) ._size != 2 || sr.at (1) ._deviceAddress != 8
		|| sr.at (1) ._idxDevice != 1) return false;

	return true;
}

OGSS_Bool
UT_RAID01VolCtrl::fullStripeRequest () {
	RAID01VolCtrl			module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._mirrorChosen = true;

	req._size = 8;
	req._volumeAddress = 24;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 2) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 12
		|| sr.at (0) ._idxDevice != 0) return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 12
		|| sr.at (1) ._idxDevice != 1) return false;

	return true;
}

OGSS_Bool
UT_RAID01VolCtrl::twoStripeRequest () {
	RAID01VolCtrl			module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._mirrorChosen = true;

	req._size = 10;
	req._volumeAddress = 2;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 3) return false;
	if (sr.at (0) ._size != 2 || sr.at (0) ._deviceAddress != 2
		|| sr.at (0) ._idxDevice != 0) return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 0
		|| sr.at (1) ._idxDevice != 1) return false;
	if (sr.at (2) ._size != 4 || sr.at (2) ._deviceAddress != 4
		|| sr.at (2) ._idxDevice != 0) return false;

	return true;
}

OGSS_Bool
UT_RAID01VolCtrl::moreStripeRequest () {
	RAID01VolCtrl			module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._mirrorChosen = true;

	req._size = 24;
	req._volumeAddress = 4;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 6) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 0
		|| sr.at (0) ._idxDevice != 1) return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 4
		|| sr.at (1) ._idxDevice != 0) return false;
	if (sr.at (2) ._size != 4 || sr.at (2) ._deviceAddress != 4
		|| sr.at (2) ._idxDevice != 1) return false;
	if (sr.at (3) ._size != 4 || sr.at (3) ._deviceAddress != 8
		|| sr.at (3) ._idxDevice != 0) return false;
	if (sr.at (4) ._size != 4 || sr.at (4) ._deviceAddress != 8
		|| sr.at (4) ._idxDevice != 1) return false;
	if (sr.at (5) ._size != 4 || sr.at (5) ._deviceAddress != 12
		|| sr.at (5) ._idxDevice != 0) return false;

	return true;
}

OGSS_Bool
UT_RAID01VolCtrl::readRequests () {
	RAID01VolCtrl			module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._mirrorChosen = true;

	req._size = 4;
	req._volumeAddress = 16;
	req._type = RQT_READ;

	module.decompose (req, sr);

	if (sr.size () != 1) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 8
		|| sr.at (0) ._idxDevice != 0) return false;

	sr.clear ();
	module.decompose (req, sr);

	if (sr.size () != 1) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 8
		|| sr.at (0) ._idxDevice != 2) return false;

	return true;
}

OGSS_Bool
UT_RAID01VolCtrl::writeRequest () {
	RAID01VolCtrl			module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numBytesBySU = 4;
	module._numDevices = 4;
	module._mirrorChosen = true;

	req._size = 4;
	req._volumeAddress = 16;
	req._type = RQT_WRITE;

	module.decompose (req, sr);

	if (sr.size () != 2) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 8
		|| sr.at (0) ._idxDevice != 0) return false;
	if (sr.at (1) ._size != 4 || sr.at (1) ._deviceAddress != 8
		|| sr.at (1) ._idxDevice != 2) return false;

	return true;
}
