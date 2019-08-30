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

//! \file	jbodvolctrl.cpp
//! \brief	Definition of the JBOD controller.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <set>

#include "controller/jbodvolctrl.hpp"

#include "parser/xmlparser.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

JBODVolCtrl::JBODVolCtrl (
	const Volume			& vol,
	const Device			& dev) {
	_numBytesByDev			= dev._physicalCapacity;
	_numDevices				= vol._numDevices;
}

JBODVolCtrl::~JBODVolCtrl () {  }

void
JBODVolCtrl::decompose (
	Request					& request,
	vector <Request>		& subrequests) {
	OGSS_Ulong				addr;
	OGSS_Ulong				remainingSize;
	OGSS_Ulong				cnt = 0;

	addr = request._volumeAddress;
	remainingSize = request._size;

	while (remainingSize > 0) {
		Request				sr {request};

		sr._size = min (_numBytesByDev - addr % _numBytesByDev, remainingSize);
		sr._deviceAddress = addr % _numBytesByDev;
		sr._nativeDeviceAddress = sr._deviceAddress;
		sr._idxDevice = addr / _numBytesByDev;
		sr._nativeIdxDevice = sr._idxDevice;
		sr._type = request._type;
		sr._numChild = 0;
		sr._prio = false;

		sr._mainIdx = request._mainIdx;
		sr._majrIdx = request._majrIdx;
		sr._minrIdx = ++cnt;

		++ request._numChild;

		subrequests.push_back (sr);

		addr += sr._size;
		remainingSize -= sr._size;
	}
}

void
JBODVolCtrl::updateScheme (
	const Request			& event) {
	VolumeController::updateScheme (event);
	LOG(INFO) << "JBOD receives an event on local disk #" << event._idxDevice;
}

void
JBODVolCtrl::generateDecraidFailureRequests (
	Request								& block,
	vector <Request>					& subrequests) {  }

void
JBODVolCtrl::generateDecraidRenewalRequests (
	Request								& block,
	vector <Request>					& subrequests) {
	Request								reqR {block},
										reqW {block};

	reqR._type = RQT_READ;				reqW._type = RQT_WRITE;
	reqR._numPrioChild = 1;				reqW._numPrioChild = 0;
	reqR._prio = true;					reqW._prio = false;
	reqR._operation = ROP_COPY;			reqW._operation = ROP_RENEW;

	subrequests.push_back (reqR);		subrequests.push_back (reqW);
}

void
JBODVolCtrl::generateFailureRequests (
	Request								& block,
	std::vector <Request>				& subrequests) {  }

void
JBODVolCtrl::generateRenewalRequests (
	Request								& block,
	std::vector <Request>				& subrequests) {  }

void
JBODVolCtrl::requestMultiplier (
	Request								& block,
	vector <Request>					& subrequests) {
	Request								sr {block};

	sr._numPrioChild = 0;
	sr._numChild = 0;
	sr._multiple = true;

	if (block._type == RQT_READ) {
		sr._operation = ROP_NATIVE;		sr._prio = true;
										subrequests.push_back (sr);

		// No recovery from JBOD
		sr._operation = ROP_COPY;		subrequests.push_back (sr);

		// No update, read to update means that there is a recovery
		sr._operation = ROP_RENEW;		sr._type = RQT_WRITE;
										sr._prio = false;
										subrequests.push_back (sr);

		block._numPrioChild = 2;
		block._numChild = 3;
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
JBODVolCtrl::getBlockLocation (
	Request					& block) {  }

/*----------------------------------------------------------------------------*/
/* UNITARY TEST --------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

UT_JBODVolCtrl::UT_JBODVolCtrl (
	const OGSS_String		& configurationFile):
	UnitaryTest <UT_JBODVolCtrl> (MTP_VOLJBOD) {
	set <OGSS_String>		testNames;

	XMLParser::getListOfRequestedUnitaryTests (
		configurationFile, _module, testNames);

	for (auto & elt: testNames) {
		if (! elt.compare ("all") ) {
			_tests.push_back (make_pair ("Middle device request",
				&UT_JBODVolCtrl::middleDeviceRequest) );
			_tests.push_back (make_pair ("Start device request",
				&UT_JBODVolCtrl::startDeviceRequest) );
			_tests.push_back (make_pair ("End device request",
				&UT_JBODVolCtrl::endDeviceRequest) );
			_tests.push_back (make_pair ("Two device request",
				&UT_JBODVolCtrl::twoDeviceRequest) );
			_tests.push_back (make_pair ("More device request",
				&UT_JBODVolCtrl::moreDeviceRequest) );
		} else if (! elt.compare ("middleDev") )
			_tests.push_back (make_pair ("Middle device request",
				&UT_JBODVolCtrl::middleDeviceRequest) );
		else if (! elt.compare ("startDev") )
			_tests.push_back (make_pair ("Start device request",
				&UT_JBODVolCtrl::startDeviceRequest) );
		else if (! elt.compare ("endDev") )
			_tests.push_back (make_pair ("End device request",
				&UT_JBODVolCtrl::endDeviceRequest) );
		else if (! elt.compare ("twoDev") )
			_tests.push_back (make_pair ("Two device request",
				&UT_JBODVolCtrl::twoDeviceRequest) );
		else if (! elt.compare ("moreDev") )
			_tests.push_back (make_pair ("More device request",
				&UT_JBODVolCtrl::moreDeviceRequest) );
		else
			LOG (WARNING) << ModuleNameMap.at (_module) << " unitary test "
				<< "named '" << elt << "' does not match!";
	}
}

UT_JBODVolCtrl::~UT_JBODVolCtrl () {  }

OGSS_Bool
UT_JBODVolCtrl::middleDeviceRequest () {
	JBODVolCtrl				module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numDevices = 3;

	req._size = 4;
	req._volumeAddress = 6;

	module.decompose (req, sr);

	if (sr.size () != 1) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 6
		|| sr.at (0) ._idxDevice != 0) return false;

	return true;
}

OGSS_Bool
UT_JBODVolCtrl::startDeviceRequest () {
	JBODVolCtrl				module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numDevices = 3;

	req._size = 12;
	req._volumeAddress = 16;

	module.decompose (req, sr);

	if (sr.size () != 1) return false;
	if (sr.at (0) ._size != 12 || sr.at (0) ._deviceAddress != 0
		|| sr.at (0) ._idxDevice != 1) return false;

	return true;
}

OGSS_Bool
UT_JBODVolCtrl::endDeviceRequest () {
	JBODVolCtrl				module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numDevices = 3;

	req._size = 6;
	req._volumeAddress = 42;

	module.decompose (req, sr);

	if (sr.size () != 1) return false;
	if (sr.at (0) ._size != 6 || sr.at (0) ._deviceAddress != 10
		|| sr.at (0) ._idxDevice != 2) return false;

	return true;
}

OGSS_Bool
UT_JBODVolCtrl::twoDeviceRequest () {
	JBODVolCtrl				module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numDevices = 3;

	req._size = 18;
	req._volumeAddress = 12;

	module.decompose (req, sr);

	if (sr.size () != 2) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 12
		|| sr.at (0) ._idxDevice != 0) return false;
	if (sr.at (1) ._size != 14 || sr.at (1) ._deviceAddress != 0
		|| sr.at (1) ._idxDevice != 1) return false;

	return true;
}

OGSS_Bool
UT_JBODVolCtrl::moreDeviceRequest () {
	JBODVolCtrl				module ( (Volume () ), (Device () ) );
	Request					req;
	vector <Request>		sr;

	module._numBytesByDev = 16;
	module._numDevices = 3;

	req._size = 24;
	req._volumeAddress = 12;

	module.decompose (req, sr);

	if (sr.size () != 3) return false;
	if (sr.at (0) ._size != 4 || sr.at (0) ._deviceAddress != 12
		|| sr.at (0) ._idxDevice != 0) return false;
	if (sr.at (1) ._size != 16 || sr.at (1) ._deviceAddress != 0
		|| sr.at (1) ._idxDevice != 1) return false;
	if (sr.at (2) ._size != 4 || sr.at (2) ._deviceAddress != 0
		|| sr.at (2) ._idxDevice != 2) return false;

	return true;
}
