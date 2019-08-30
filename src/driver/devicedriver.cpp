/*
 * Copyright UVSQ - CEA/DAM/DIF (2017-2018)
 * Contributors:  Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *                Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published per the Free Software Foundation; either
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

//! \file	devicedriver.cpp
//! \brief	Definition of the device driver.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <algorithm>

#include "controller/hddctrl.hpp"
#include "controller/ssdctrl.hpp"

#include "driver/devicedriver.hpp"

#include "structure/request.hpp"

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

DeviceDriver::DeviceDriver (
	const OGSS_String		configurationFile,
	const OGSS_Ushort		idx):
	Module (configurationFile, make_pair (MTP_DEVICE, idx) ) {
	OGSS_String				sncModel;
	OGSS_SynchronizationType	syncType = SNC_TOTAL;
//	DLOG(INFO) << "[DD] Constructor called for " << idx << "!";

	sncModel = XMLParser::getComputationModel (_cfg, PTP_SYNC);

	auto findRes = find_if (SynchronizationNameMap.begin (), SynchronizationNameMap.end (),
		[&] (const pair <OGSS_SynchronizationType, OGSS_String> & elt)
		{ return ! elt.second.compare (sncModel); } );
	if (findRes != SynchronizationNameMap.end () )
		syncType = findRes->first;

	if (find (OTFModels.begin (), OTFModels.end (), syncType) != OTFModels.end () )
					_syncOTF = true;
	else			_syncOTF = false;
}

DeviceDriver::~DeviceDriver () {  }

void
DeviceDriver::processExtraction () {
	receiveData ();

	switch (_device._type) {
	case DTP_HDD:
		_ctrl = make_unique <HDDCtrl> (); break;
	case DTP_SSD:
		_ctrl = make_unique <SSDCtrl> (); break;
	case DTP_NVRAM:
		DLOG(INFO) << "TODO: NVRAM controller is selected"; break;
	default:
		DLOG(INFO) << "The device type does not match a known "
			<< "controller type -- HDD is selected!";
		_ctrl = make_unique <HDDCtrl> ();
	}
}

void
DeviceDriver::processDecomposition () {
	void					* arg;
	Request					req;
	OGSS_Bool				unfinished = true;

	while (unfinished) {
		_ci->receive (arg);
		req = * static_cast <Request *> (arg); free (arg);

		switch (req._type) {
			case RQT_END:
				unfinished = false;
//				DLOG(INFO) << "[DD] #" << _id.second << " Send ending to EX";
				break;
			case RQT_EVFLT:
				_deviceState [req._idxDevice] ._failureDate = req._date;
				break;
			case RQT_EVRPL:
				_deviceState [req._idxDevice] ._renewalDate = req._date;
				break;
			case RQT_EVEND:
				LOG(INFO) << "[DD] #" << _id.second << " Reception of end event (" << req._date << ", " << req._idxDevice << ")";
				break;
			case RQT_EVSTP:
				break;
			default:
// If bug, ensure that the state is valid for all devices (need to know numDev)
				if (_deviceState [req._idxDevice] .isFailed (req._date) )
					req._failed = true;
		}

		_ci->send (make_pair (MTP_EXECUTION, 0), &req, sizeof (req) );
	}
}

void
DeviceDriver::processSynchronization () {
	if (! _syncOTF) return;
	
	_syncProc = true;
	DLOG(INFO) << "[DD] #" << _id.second << " Because of OTF synchronization model, synchronization step.";
	
	processDecomposition ();

	DLOG(INFO) << "[DD] #" << _id.second << " End of sync.";
}
