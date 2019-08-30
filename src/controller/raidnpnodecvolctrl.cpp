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

//! \file	raidnpnodecvolctrl.cpp
//! \brief	Definition of the RAID NP controller without declustering.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <set>

#include "controller/raidnpnodecvolctrl.hpp"

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

RAIDNPNoDecVolCtrl::RAIDNPNoDecVolCtrl (
	const Volume			& vol,
	const Device			& dev) {
	_numBytesByDev			= dev._physicalCapacity;
	_numBytesBySU			= vol._suSize;
	_numDevices				= vol._numDevices;
	_numParity				= vol._numRedundancyDevices;
}
RAIDNPNoDecVolCtrl::~RAIDNPNoDecVolCtrl () {  }

void
RAIDNPNoDecVolCtrl::decompose (
	Request					& request,
	vector <Request>		& subrequests) {
	OGSS_Ulong				addr;
	OGSS_Ulong				remainingSize;

	OGSS_Ulong				cnt = 0;

	OGSS_Ulong				stripeSize
		= _numBytesBySU * (_numDevices - _numParity);

	OGSS_Ulong				currentStripe;
	Request					srp {request};

	if (request._type == RQT_WRITE && _numParity > 0) {
		addr = request._volumeAddress;
		remainingSize = request._size;

		srp._type = RQT_READ;

		while (remainingSize > 0) {
			auto s = addr / stripeSize;
			auto a = addr % stripeSize;

			srp._size = 0;
			srp._deviceAddress = OGSS_ULONG_MAX;

			if (a == 0 && remainingSize >= stripeSize) {
				remainingSize -= stripeSize;
				addr += stripeSize;

			} else if (a < stripeSize / 2 && remainingSize > stripeSize / 2) {
				for (auto y = 0; y < a;) {
					Request sr {request};
					sr._size = min (_numBytesBySU, a - y);
					sr._deviceAddress = s * _numBytesBySU;
					sr._idxDevice = y / _numBytesBySU;
					sr._type = RQT_READ;

					sr._mainIdx = request._mainIdx;
					sr._majrIdx = request._majrIdx;
					sr._minrIdx = ++cnt;

					++ request._numChild;
					++ request._numPrioChild;
					sr._numPrioChild = 1;

					y += sr._size;

					srp._size = max (srp._size, sr._deviceAddress + sr._size);
					srp._deviceAddress = min (srp._deviceAddress, sr._deviceAddress);

					subrequests.push_back (sr);
				}

				for (auto y = a + remainingSize; y < stripeSize;) {
					Request sr {request};
					sr._size = _numBytesBySU - (y % _numBytesBySU);
					sr._deviceAddress = y % _numBytesBySU + s * _numBytesBySU;
					sr._idxDevice = y / _numBytesBySU;
					sr._type = RQT_READ;

					sr._mainIdx = request._mainIdx;
					sr._majrIdx = request._majrIdx;
					sr._minrIdx = ++cnt;

					++ request._numChild;
					++ request._numPrioChild;
					sr._numPrioChild = 1;

					y += sr._size;

					srp._size = max (srp._size, sr._deviceAddress + sr._size);
					srp._deviceAddress = min (srp._deviceAddress, sr._deviceAddress);

					subrequests.push_back (sr);
				}

				addr += min (remainingSize, stripeSize - a);
				remainingSize -= min (remainingSize, stripeSize - a);

				srp._size = srp._size - srp._deviceAddress;

				for (int i = _numDevices - _numParity; i < _numDevices; ++i) {
					srp._idxDevice = i;

					srp._mainIdx = request._mainIdx;
					srp._majrIdx = request._majrIdx;
					srp._minrIdx = ++cnt;

					++ request._numChild;
					++ request._numPrioChild;
					srp._numPrioChild = 1;

					subrequests.push_back (srp);
				}

			} else {
				do {
					Request	sr {request};
					sr._size = min (_numBytesBySU - addr % _numBytesBySU, remainingSize);
					sr._deviceAddress = (addr / stripeSize) * _numBytesBySU
						+ addr % _numBytesBySU;
					sr._idxDevice = (addr / _numBytesBySU) % (_numDevices - _numParity);
					sr._type = RQT_READ;
		
					sr._mainIdx = request._mainIdx;
					sr._majrIdx = request._majrIdx;
					sr._minrIdx = ++cnt;

					++ request._numChild;
					++ request._numPrioChild;
					sr._numPrioChild = 1;

					srp._size = max (srp._size, sr._deviceAddress + sr._size);
					srp._deviceAddress = min (srp._deviceAddress, sr._deviceAddress);

					addr += sr._size;
					remainingSize -= sr._size;

					subrequests.push_back (sr);
				} while (remainingSize != 0 && s == addr / stripeSize);

				srp._size = srp._size - srp._deviceAddress;
				
				for (int i = _numDevices - _numParity; i < _numDevices; ++i) {
					srp._idxDevice = i;

					srp._mainIdx = request._mainIdx;
					srp._majrIdx = request._majrIdx;
					srp._minrIdx = ++cnt;

					++ request._numChild;
					++ request._numPrioChild;
					srp._numPrioChild = 1;

					subrequests.push_back (srp);
				}
			}
		}

		srp._type = RQT_WRITE;
	}

	addr = request._volumeAddress;
	remainingSize = request._size;

	srp._size = 0;
	srp._deviceAddress = OGSS_ULONG_MAX;

	while (remainingSize > 0) {
		Request				sr {request};

		currentStripe = addr / stripeSize;

		sr._size = min (_numBytesBySU - addr % _numBytesBySU, remainingSize);
		sr._deviceAddress = (addr / stripeSize) * _numBytesBySU + addr % _numBytesBySU;
		sr._idxDevice = (addr / _numBytesBySU) % (_numDevices - _numParity);
		sr._type = request._type;
		
		sr._mainIdx = request._mainIdx;
		sr._majrIdx = request._majrIdx;
		sr._minrIdx = ++cnt;

		++ request._numChild;

		if (request._type == RQT_WRITE) {
			srp._size = max (srp._size, sr._deviceAddress + sr._size);
			srp._deviceAddress = min (srp._deviceAddress, sr._deviceAddress);
		}

		addr += sr._size;
		remainingSize -= sr._size;

		subrequests.push_back (sr);

		if (request._type == RQT_WRITE && (remainingSize == 0 || currentStripe
			!= addr / (_numBytesBySU * (_numDevices - _numParity) ) ) ) {
			srp._size = srp._size - srp._deviceAddress;
			for (int i = _numDevices - _numParity; i < _numDevices; ++i) {
				srp._idxDevice = i;

				srp._mainIdx = request._mainIdx;
				srp._majrIdx = request._majrIdx;
				srp._minrIdx = ++cnt;

				++ request._numChild;

				subrequests.push_back (srp);
			}

			srp._size = 0;
			srp._deviceAddress = OGSS_ULONG_MAX;
		}
	}

	manageFailure (request, subrequests);
}

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
RAIDNPNoDecVolCtrl::manageFailure (
	Request					& request,
	vector <Request>		& subrequests) {
	OGSS_Bool				check = true;

	while (check && _failureDates.size () ) {
		check = false;
		for (auto elt = _failureDates.begin (); elt != _failureDates.end (); ++elt) {
			if (elt->second <= request._date) {
				check = true;
				_failedDevices.insert (elt->first);
				_failureDates.erase (elt);
				break;
			}
		}
	}

	if (! _failedDevices.size () || _numParity < _failedDevices.size () ) return;

	for (OGSS_Ulong i {0}; i < subrequests.size (); ++i) {
		Request & elt = subrequests [i];
		if (_failedDevices.find (elt._idxDevice) != _failedDevices.end ()
		 && _failureDates [elt._idxDevice] <= request._date) {
		 	if (elt._type == RQT_READ) {
		 		for (OGSS_Ushort j = 0; j < _numDevices; ++j) {
		 			if (_failedDevices.find (j) != _failedDevices.end ()
		 			 && _failureDates [j] <= request._date) continue;

		 			Request flt {elt};
		 			flt._idxDevice = j;
		 			++ request._numChild;
		 			if (request._numPrioChild)
		 				++ request._numPrioChild;
		 			subrequests.insert (subrequests.begin () + i + 1, flt);
		 		}

		 		-- request._numChild;
		 		if (request._numPrioChild)
		 			-- request._numPrioChild;
		 		subrequests.erase (subrequests.begin () + i);
		 		--i;
			} else if (elt._type == RQT_WRITE) {
				-- request._numChild;
				subrequests.erase (subrequests.begin () + i);
				--i;
			}
		}
	}

	removeDuplicates (request, subrequests);	
}

void
RAIDNPNoDecVolCtrl::removeDuplicates (
	Request					& request,
	vector <Request>		& subrequests) {

	for (OGSS_Ulong i = 0; i < subrequests.size (); ++i) {
		for (OGSS_Ulong j = i + 1; j < subrequests.size (); ++j) {
			if (subrequests [i] ._idxDevice == subrequests [j] ._idxDevice
			 && subrequests [i] ._deviceAddress == subrequests [j] ._deviceAddress
			 && subrequests [i] ._size == subrequests [j] ._size
			 && subrequests [i] ._type == subrequests [j] ._type) {
				--request._numChild;
				if (request._numPrioChild && subrequests [j] ._type == RQT_READ)
					--request._numPrioChild;
				subrequests.erase (subrequests.begin () + j);
				--j;
			}
		}
	}

	OGSS_Ulong cnt = 1;

	for (auto & elt: subrequests)
		elt._minrIdx = cnt++;
}

OGSS_Bool
RAIDNPNoDecVolCtrl::targetsParity (
	Request					& request) {
	return request._idxDevice >= _numParity;
}

void
RAIDNPNoDecVolCtrl::updateScheme (
	const Request			& event) {
	VolumeController::updateScheme (event);
	LOG(INFO) << "RAIDNP receives an event on local disk #" << event._idxDevice;

	if (event._type == RQT_EVFLT)
		_failureDates [event._idxDevice] = event._date;
}

void
RAIDNPNoDecVolCtrl::generateDecraidFailureRequests (
	Request								& block,
	vector <Request>					& subrequests) {
	for (auto i = 0; i < _numDevices; ++i) {
		Request req {block};

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
RAIDNPNoDecVolCtrl::generateDecraidRenewalRequests (
	Request								& block,
	vector <Request>					& subrequests) {
	for (auto i = 0; i < _numDevices; ++i) {
		Request req {block};

		req._idxDevice = i;
		if (i != block._idxDevice) {
			req._numPrioChild = 1;
			req._type = RQT_READ;
			req._operation = ROP_RECOVERY;
		} else {
			Request reqC {req};

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
RAIDNPNoDecVolCtrl::generateFailureRequests (
	Request			& block,
	vector <Request>		& subrequests) {  }

void
RAIDNPNoDecVolCtrl::generateRenewalRequests (
	Request								& block,
	vector <Request>					& subrequests) {
	for (auto i = 0; i < _numDevices; ++i) {
		Request req {block};

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
RAIDNPNoDecVolCtrl::requestMultiplier (
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
RAIDNPNoDecVolCtrl::getBlockLocation (
	Request					& block) {  }

/*----------------------------------------------------------------------------*/
/* UNITARY TEST --------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

UT_RAIDNPNoDecVolCtrl::UT_RAIDNPNoDecVolCtrl (
	const OGSS_String		& configurationFile):
	UnitaryTest <UT_RAIDNPNoDecVolCtrl> (MTP_VOLRAIDNPNODEC) {
	set <OGSS_String>		testNames;

	XMLParser::getListOfRequestedUnitaryTests (
		configurationFile, _module, testNames);

	for (auto & elt: testNames) {
		if (! elt.compare ("all") ) {
			_tests.push_back (make_pair ("Small stripe read 0par request",
				&UT_RAIDNPNoDecVolCtrl::smallStripeR0PRequest) );
			_tests.push_back (make_pair ("Large stripe read 0par request",
				&UT_RAIDNPNoDecVolCtrl::largeStripeR0PRequest) );
			_tests.push_back (make_pair ("Full stripe read 0par request",
				&UT_RAIDNPNoDecVolCtrl::fullStripeR0PRequest) );
			_tests.push_back (make_pair ("Two stripe read 0par request",
				&UT_RAIDNPNoDecVolCtrl::twoStripeR0PRequest) );
			_tests.push_back (make_pair ("More stripe read 0par request",
				&UT_RAIDNPNoDecVolCtrl::moreStripeR0PRequest) );
			_tests.push_back (make_pair ("Small stripe write 0par request",
				&UT_RAIDNPNoDecVolCtrl::smallStripeW0PRequest) );
			_tests.push_back (make_pair ("Large stripe write 0par request",
				&UT_RAIDNPNoDecVolCtrl::largeStripeW0PRequest) );
			_tests.push_back (make_pair ("Full stripe write 0par request",
				&UT_RAIDNPNoDecVolCtrl::fullStripeW0PRequest) );
			_tests.push_back (make_pair ("Two stripe write 0par request",
				&UT_RAIDNPNoDecVolCtrl::twoStripeW0PRequest) );
			_tests.push_back (make_pair ("More stripe write 0par request",
				&UT_RAIDNPNoDecVolCtrl::moreStripeW0PRequest) );

			_tests.push_back (make_pair ("Small stripe read 1par request",
				&UT_RAIDNPNoDecVolCtrl::smallStripeR1PRequest) );
			_tests.push_back (make_pair ("Large stripe read 1par request",
				&UT_RAIDNPNoDecVolCtrl::largeStripeR1PRequest) );
			_tests.push_back (make_pair ("Full stripe read 1par request",
				&UT_RAIDNPNoDecVolCtrl::fullStripeR1PRequest) );
			_tests.push_back (make_pair ("Two stripe read 1par request",
				&UT_RAIDNPNoDecVolCtrl::twoStripeR1PRequest) );
			_tests.push_back (make_pair ("More stripe read 1par request",
				&UT_RAIDNPNoDecVolCtrl::moreStripeR1PRequest) );
			_tests.push_back (make_pair ("Small stripe write 1par request",
				&UT_RAIDNPNoDecVolCtrl::smallStripeW1PRequest) );
			_tests.push_back (make_pair ("Large stripe write 1par request",
				&UT_RAIDNPNoDecVolCtrl::largeStripeW1PRequest) );
			_tests.push_back (make_pair ("Full stripe write 1par request",
				&UT_RAIDNPNoDecVolCtrl::fullStripeW1PRequest) );
			_tests.push_back (make_pair ("Two stripe write 1par request",
				&UT_RAIDNPNoDecVolCtrl::twoStripeW1PRequest) );
			_tests.push_back (make_pair ("More stripe write 1par request",
				&UT_RAIDNPNoDecVolCtrl::moreStripeW1PRequest) );

			_tests.push_back (make_pair ("Small stripe read 2par request",
				&UT_RAIDNPNoDecVolCtrl::smallStripeR2PRequest) );
			_tests.push_back (make_pair ("Large stripe read 2par request",
				&UT_RAIDNPNoDecVolCtrl::largeStripeR2PRequest) );
			_tests.push_back (make_pair ("Full stripe read 2par request",
				&UT_RAIDNPNoDecVolCtrl::fullStripeR2PRequest) );
			_tests.push_back (make_pair ("Two stripe read 2par request",
				&UT_RAIDNPNoDecVolCtrl::twoStripeR2PRequest) );
			_tests.push_back (make_pair ("More stripe read 2par request",
				&UT_RAIDNPNoDecVolCtrl::moreStripeR2PRequest) );
			_tests.push_back (make_pair ("Small stripe write 2par request",
				&UT_RAIDNPNoDecVolCtrl::smallStripeW2PRequest) );
			_tests.push_back (make_pair ("Large stripe write 2par request",
				&UT_RAIDNPNoDecVolCtrl::largeStripeW2PRequest) );
			_tests.push_back (make_pair ("Full stripe write 2par request",
				&UT_RAIDNPNoDecVolCtrl::fullStripeW2PRequest) );
			_tests.push_back (make_pair ("Two stripe write 2par request",
				&UT_RAIDNPNoDecVolCtrl::twoStripeW2PRequest) );
			_tests.push_back (make_pair ("More stripe write 2par request",
				&UT_RAIDNPNoDecVolCtrl::moreStripeW2PRequest) );
		} else if (! elt.compare ("smallStripeR0P") )
			_tests.push_back (make_pair ("Small stripe read 0par request",
				&UT_RAIDNPNoDecVolCtrl::smallStripeR0PRequest) );
		else if (! elt.compare ("largeStripeR0P") )
			_tests.push_back (make_pair ("Large stripe read 0par request",
				&UT_RAIDNPNoDecVolCtrl::largeStripeR0PRequest) );
		else if (! elt.compare ("fullStripeR0P") )
			_tests.push_back (make_pair ("Full stripe read 0par request",
				&UT_RAIDNPNoDecVolCtrl::fullStripeR0PRequest) );
		else if (! elt.compare ("twoStripeR0P") )
			_tests.push_back (make_pair ("Two stripe read 0par request",
				&UT_RAIDNPNoDecVolCtrl::twoStripeR0PRequest) );
		else if (! elt.compare ("moreStripeR0P") )
			_tests.push_back (make_pair ("More stripe read 0par request",
				&UT_RAIDNPNoDecVolCtrl::moreStripeR0PRequest) );
		else if (! elt.compare ("smallStripeW0P") )
			_tests.push_back (make_pair ("Small stripe write 0par request",
				&UT_RAIDNPNoDecVolCtrl::smallStripeW0PRequest) );
		else if (! elt.compare ("largeStripeW0P") )
			_tests.push_back (make_pair ("Large stripe write 0par request",
				&UT_RAIDNPNoDecVolCtrl::largeStripeW0PRequest) );
		else if (! elt.compare ("fullStripeW0P") )
			_tests.push_back (make_pair ("Full stripe write 0par request",
				&UT_RAIDNPNoDecVolCtrl::fullStripeW0PRequest) );
		else if (! elt.compare ("twoStripeW0P") )
			_tests.push_back (make_pair ("Two stripe write 0par request",
				&UT_RAIDNPNoDecVolCtrl::twoStripeW0PRequest) );
		else if (! elt.compare ("moreStripeW0P") )
			_tests.push_back (make_pair ("More stripe write 0par request",
				&UT_RAIDNPNoDecVolCtrl::moreStripeW0PRequest) );
		else
			LOG (WARNING) << ModuleNameMap.at (_module) << " unitary test "
				<< "named '" << elt << "' does not match!";
	}
}

UT_RAIDNPNoDecVolCtrl::~UT_RAIDNPNoDecVolCtrl () {  }

#include "utest/utnodecp0.cpp"
#include "utest/utnodecp1.cpp"
#include "utest/utnodecp2.cpp"