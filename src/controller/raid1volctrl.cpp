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

//! \file	raid1volctrl.cpp
//! \brief	Definition of the RAID 1 controller.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <set>

#include "controller/raid1volctrl.hpp"

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

RAID1VolCtrl::RAID1VolCtrl (
	const Volume			& vol,
	const Device			& dev) {
	_numBytesByDev			= dev._physicalCapacity;
	_numDevices				= vol._numDevices;
	_mirrorChosen			= true;
	_failedDevices			= set <OGSS_Ushort> ();
	_failureDates			= map <OGSS_Ushort, OGSS_Real> ();
}

RAID1VolCtrl::~RAID1VolCtrl () {  }

void
RAID1VolCtrl::decompose (
	Request					& request,
	vector <Request>		& subrequests) {
	OGSS_Ulong				addr;
	OGSS_Ulong				remainingSize;
	OGSS_Ulong				cnt = 0;

	addr = request._volumeAddress;
	remainingSize = request._size;

	while (remainingSize > 0) {
		Request				sr {request}, srm {request};

		sr._size = min (_numBytesByDev - addr % _numBytesByDev, remainingSize);
		sr._deviceAddress = addr % _numBytesByDev;
		sr._idxDevice = addr / _numBytesByDev;

		sr._mainIdx = request._mainIdx;
		sr._majrIdx = request._majrIdx;
		sr._minrIdx = ++cnt;
		sr._prio = srm._prio = false;

		++ request._numChild;

		if (request._type == RQT_READ) {
			sr._idxDevice = _mirrorChosen ? sr._idxDevice
				: sr._idxDevice + _numDevices / 2;

			if (_failedDevices.find (sr._idxDevice) != _failedDevices.end ()
				&& _failureDates.at (sr._idxDevice) <= sr._date)
				sr._idxDevice = (!_mirrorChosen) ? sr._idxDevice
					: sr._idxDevice + _numDevices / 2;

			subrequests.push_back (sr);
		} else {
			srm = sr;

			srm._idxDevice = sr._idxDevice + _numDevices / 2;
			srm._minrIdx = ++cnt;

			++ request._numChild;

			if (_failedDevices.find (sr._idxDevice) != _failedDevices.end ()
				&& _failureDates.at (sr._idxDevice) <= sr._date)
				subrequests.push_back (sr);
			else if (_failedDevices.find (srm._idxDevice) != _failedDevices.end ()
				&& _failureDates.at (srm._idxDevice) <= srm._date)
				subrequests.push_back (srm);
		}

		addr += sr._size;
		remainingSize -= sr._size;
	}

	if (request._type == RQT_READ) _mirrorChosen = ! _mirrorChosen;
}

void
RAID1VolCtrl::updateScheme (
	const Request			& event) {
	VolumeController::updateScheme (event);
	LOG(INFO) << "RAID1 receives an event on local disk #" << event._idxDevice;

	if (event._type == RQT_EVFLT) {
		_failedDevices.insert (event._idxDevice);
		_failureDates [event._idxDevice] = event._date;
	}
}

void
RAID1VolCtrl::generateDecraidFailureRequests (
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
RAID1VolCtrl::generateDecraidRenewalRequests (
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
RAID1VolCtrl::generateFailureRequests (
	Request								& block,
	std::vector <Request>				& subrequests) {  }

void
RAID1VolCtrl::generateRenewalRequests (
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
RAID1VolCtrl::requestMultiplier (
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
RAID1VolCtrl::getBlockLocation (
	Request					& block) {  }

/*----------------------------------------------------------------------------*/
/* UNITARY TEST --------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "utest/utraid1.cpp"
