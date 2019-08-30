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

//! \file	perfparityctrl.cpp
//! \brief	Definition of the parity controller. RAID NP controllers are now
//!			deprecated as we suppose that request sizes are multiples of a
//!			stripe unit size.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/


#include "controller/perfparityctrl.hpp"

#include <set>

#include <glog/logging.h>

using namespace std;

typedef set <Request, OGSS_Bool (*) (Request, Request)>
										__RequestSet;

/*----------------------------------------------------------------------------*/
/* STATIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

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
/* MEMBER FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

PerfectParityCtrl::PerfectParityCtrl (
	const Volume						& vol,
	const Device						& dev) {
	_numMUByDev							= dev._physicalCapacity;
	_numMUBySU							= vol._suSize;
	_numDevices							= vol._numDevices;
	_numParity							= vol._numRedundancyDevices;
	_numData							= _numDevices - _numParity;
	_parity								= vol._declustering;
}

PerfectParityCtrl::~PerfectParityCtrl () {  }

void
PerfectParityCtrl::decompose (
	Request								& request,
	vector <Request>					& subrequests) {
	OGSS_Ulong							addr {request._volumeAddress};
	OGSS_Ulong							remainingSize {request._size};

	while (remainingSize) {
		auto							sIdx {addr / (_numData * _numMUBySU)};
		auto							sStart {addr % (_numData * _numMUBySU)};
		auto							sEnd {min (sStart + remainingSize, _numData * _numMUBySU)};

		if (request._type == RQT_WRITE)
			_getPrereadStripe (request, subrequests, sIdx, sStart, sEnd);
		_getDataStripe (request, subrequests, sIdx, sStart, sEnd);

		++ sIdx;
		addr += (sEnd - sStart);
		remainingSize -= (sEnd - sStart);
	}

	_realloc (subrequests);
	__removeAndMerge (request, subrequests);
}

void
PerfectParityCtrl::updateScheme (
	const Request						& event) {
	VolumeController::updateScheme (event);
	LOG(INFO) << "Parity RAID receives an event on local disk #" << event._idxDevice
		<< " which will happen at " << event._date;
}

void
PerfectParityCtrl::manageFailure (
	Request								& request,
	vector <Request>					& subrequests) {

}

void
PerfectParityCtrl::generateDecraidFailureRequests (
	Request								& block,
	vector <Request>					& subrequests) {
	for (auto i = 0; i < _numDevices; ++i) {
		Request							req {block};

		req._idxDevice = i;
		if (i != block._idxDevice) {
			req._numPrioChild = 1;
			req._type = RQT_READ;
			req._operation = ROP_RECOVERY;
		} else {
			req._numPrioChild = 0;
			req._type = RQT_WRITE;
			req._operation = ROP_UPDATE;
		}

		subrequests.push_back (req);
	}
}

void
PerfectParityCtrl::generateDecraidRenewalRequests (
	Request								& block,
	vector <Request>					& subrequests) {
	for (auto i = 0; i < _numDevices; ++i) {
		Request							req {block};

		req._idxDevice = i;
		if (i != block._idxDevice) {
			req._numPrioChild = 1;
			req._type = RQT_READ;
			req._operation = ROP_RECOVERY;
		} else {
			Request						reqC {req};

			reqC._numPrioChild = 1;
			reqC._type = RQT_READ;
			reqC._operation = ROP_COPY;

			subrequests.push_back (reqC);

			req._numPrioChild = 0;
			req._type = RQT_WRITE;
			req._operation = ROP_RENEW;
		}

		subrequests.push_back (req);
	}
}

void
PerfectParityCtrl::generateFailureRequests (
	Request								& block,
	vector <Request>					& subrequests) {  }

void
PerfectParityCtrl::generateRenewalRequests (
	Request								& block,
	vector <Request>					& subrequests) {
	for (auto i = 0; i < _numDevices; ++i) {
		Request							req {block};

		req._idxDevice = i;
		if (i != block._idxDevice) {
			req._numPrioChild = 1;
			req._type = RQT_READ;
			req._operation = ROP_RECOVERY;
		} else {
			req._numPrioChild = 0;
			req._type = RQT_WRITE;
			req._operation = ROP_RENEW;
		}

		subrequests.push_back (req);
	}
}

void
PerfectParityCtrl::requestMultiplier (
	Request								& block,
	vector <Request>					& subrequests) {
	Request								sr {block};

	sr._numPrioChild = 0;
	sr._numChild = 0;
	sr._multiple = true;

	if (block._type == RQT_READ) {
		sr._operation = ROP_NATIVE;		sr._prio = true;
										subrequests.push_back (sr);

		for (auto i = 0; i < _numDevices; ++i) {
			if (i == block._idxDevice)	continue;
			Request						srp {sr};
			srp._operation = ROP_RECOVERY;	srp._idxDevice = i;
										subrequests.push_back (srp);
		}

		sr._operation = ROP_COPY;		subrequests.push_back (sr);

		sr._operation = ROP_UPDATE;		sr._type = RQT_WRITE;
										sr._prio = false;
										subrequests.push_back (sr);
		sr._operation = ROP_RENEW;		subrequests.push_back (sr);

		block._numPrioChild = _numDevices + 1;
		block._numChild = _numDevices + 3;
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
PerfectParityCtrl::getBlockLocation (
	Request								& block) {
	auto								stripe {block._volumeAddress / _numMUBySU * _numData};
	block._deviceAddress = (block._volumeAddress / (_numMUBySU * _numData) ) * _numMUBySU;

	switch (_parity) {
	case DCL_PARITY:
		block._idxDevice = (stripe + ( (block._volumeAddress / _numMUBySU) % _numData) ) % _numDevices;
		break;
	case DCL_DATA:

		break;
	case DCL_OFF: default:

		break;
	}
}

void
PerfectParityCtrl::_getPrereadStripe (
	Request						& parent,
	vector <Request>			& children,
	OGSS_Ulong					idx,
	OGSS_Ulong					start,
	OGSS_Ulong					end) {
	Request						child {parent};
	OGSS_Ulong					stripeSize = _numMUBySU * _numData;
	OGSS_Ulong					fullStripeSize = _numMUBySU * _numDevices;
	OGSS_Ulong					firstDevice = start % stripeSize / _numMUBySU;
	OGSS_Ulong					lastDevice = (end - 1) % stripeSize / _numMUBySU;

	child._deviceAddress = idx * _numMUBySU;
	child._size = _numMUBySU;
	child._numPrioChild = 1;
	child._prio = true;
	child._numLink = idx;

	child._type = RQT_READ;

	if (end - start == stripeSize) return;			// If whole stripe, do not need a preread
	else if (end - start + _numParity * _numMUBySU > fullStripeSize / 2) {
		OGSS_Ulong				idx {0};
		while (idx < firstDevice) {					// If large stripe, do on not written data
			child._idxDevice = idx;					// BEFORE written data

			++ parent._numChild;
			++ parent._numPrioChild;
			children.push_back (child);

			++ idx;
		}

		idx = lastDevice + 1;
		while (idx < _numData) {					// AFTER written data
			child._idxDevice = idx;

			++ parent._numChild;
			++ parent._numPrioChild;
			children.push_back (child);

			++ idx;
		}
	} else {
		while (firstDevice <= lastDevice) {			// If small stripe, do on written data
			child._idxDevice = firstDevice;			

			++ parent._numChild;
			++ parent._numPrioChild;
			children.push_back (child);

			++ firstDevice;
		}

		OGSS_Ulong				idx {_numData};
		while (idx < _numDevices) {	// And parity
			child._idxDevice = idx;

			++ parent._numChild;
			++ parent._numPrioChild;
			children.push_back (child);

			++ idx;
		}
	}
}

void
PerfectParityCtrl::_getDataStripe (
	Request						& parent,
	vector <Request>			& children,
	OGSS_Ulong					idx,
	OGSS_Ulong					start,
	OGSS_Ulong					end) {
	Request						child {parent};
	OGSS_Ulong					stripeSize = _numMUBySU * _numData;

	child._deviceAddress = idx * _numMUBySU;
	child._size = _numMUBySU;
	child._numPrioChild = 0;
	child._prio = false;
	child._numLink = idx;

	while (start != end) {							// Operation on data
		child._idxDevice = (start % stripeSize) / _numMUBySU;

		++ parent._numChild;
		children.push_back (child);

		start += child._size;
	}

	if (parent._type == RQT_WRITE) {				// Write on parity if needed
		for (auto i = _numData; i < _numDevices; ++i) {
			child._idxDevice = i;

			++ parent._numChild;
			children.push_back (child);
		}
	}
}

void
PerfectParityCtrl::_realloc (
	vector <Request>					& requests) {
	OGSS_Ulong							stripe;
	OGSS_Ulong							idx;

	if (_parity == DCL_OFF) return;

	if (_parity == DCL_PARITY) {
		for (auto & elt: requests) {
			stripe = (elt._deviceAddress / _numMUBySU) % _numDevices;
			if (elt._idxDevice >= _numData)
				elt._idxDevice = (_numDevices + elt._idxDevice - stripe) % _numDevices;
			else if ( (_numDevices + _numData - (stripe % _numDevices) ) % _numDevices <= elt._idxDevice ||
				(_numDevices - (stripe % _numDevices) - 1) % _numDevices <= elt._idxDevice)
				elt._idxDevice += min ( (OGSS_Ulong) _numParity,
					1 + ( (_numDevices - (stripe % _numDevices) - 1) % _numDevices) );
		}
	} else { // DCL_DATA
		for (auto & elt: requests) {
			stripe = (elt._deviceAddress / _numMUBySU) % _numDevices;
			elt._idxDevice = (_numDevices + elt._idxDevice - (stripe % _numDevices) ) % _numDevices;
		}
	}
}
