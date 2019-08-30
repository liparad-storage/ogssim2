/*
 * Copyright UVSQ - CEA/DAM/DIF (2018)
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

//! \file	volumedriver.cpp
//! \brief	Definition of the volume driver.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <algorithm>
#include <bitset>
#include <cmath>

#include "controller/decraidvolctrl.hpp"
#include "controller/jbodvolctrl.hpp"
#include "controller/raid01volctrl.hpp"
#include "controller/raid1volctrl.hpp"
#include "controller/raidnpdatdecvolctrl.hpp"
#include "controller/raidnpnodecvolctrl.hpp"
#include "controller/raidnppardecvolctrl.hpp"

#include "controller/perfparityctrl.hpp"

#include "driver/volumedriver.hpp"

#include "structure/request.hpp"

#include "parser/xmlparser.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC MEMBER FUNCTIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

VolumeDriver::VolumeDriver (
	const OGSS_String		configurationFile,
	const OGSS_Ushort		idx):
	Module (configurationFile, make_pair (MTP_VOLUME, idx) ) {
	OGSS_String				sncModel;
	OGSS_SynchronizationType	syncType = SNC_TOTAL;
//	DLOG(INFO) << "[VD] Constructor called for " << _id.second << "!";

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

VolumeDriver::~VolumeDriver () {  }

void
VolumeDriver::processExtraction () {
	receiveData ();

	switch (_vol._type) {
	case VTP_JBOD:
		_ctrl = make_unique <JBODVolCtrl> (_vol, _dev); break;
	case VTP_RAID01:
		_ctrl = make_unique <RAID01VolCtrl> (_vol, _dev); break;
	case VTP_RAID1:
		_ctrl = make_unique <RAID1VolCtrl> (_vol, _dev); break;
	case VTP_RAIDNP:
/*		switch (_vol._declustering) {
			case DCL_DATA:
				_ctrl = make_unique <RAIDNPDatDecVolCtrl> (_vol, _dev); break;
			case DCL_PARITY:
				_ctrl = make_unique <RAIDNPParDecVolCtrl> (_vol, _dev); break;
			case DCL_OFF: default:
				_ctrl = make_unique <RAIDNPNoDecVolCtrl> (_vol, _dev); break;
		}
*/		_ctrl = make_unique <PerfectParityCtrl> (_vol, _dev); break;
	case VTP_DECRAID:
		_ctrl = make_unique <DecRAIDVolCtrl> (_vol, _dev, _subVols);
		break;
		
	default:
		DLOG(INFO) << "The volume type for #" << _id.second << " does not match a known "
			<< "controller type -- JBOD is selected!";
		_ctrl = make_unique <JBODVolCtrl> (_vol, _dev);
	}
}

void
VolumeDriver::processDecomposition () {
	void					* arg;
	Request					req;
	OGSS_Bool				unfinished = true;
	vector <Request>		subrequests;

	while (unfinished) {
		_ci->receive (arg);
		req = * static_cast <Request *> (arg); free (arg);

		if (req._type == RQT_END)
			{ unfinished = false; continue; }

		if (req._type == RQT_EVFLT || req._type == RQT_EVRPL) {
			LOG(INFO) << "[VD] Reception of event (" << req._date << ", " << req._idxDevice << ")";

			_lastEventBlockOTF [req._majrIdx] ._deviceAddress = 0;
			_lastEventBlockOTF [req._majrIdx] ._size = (_vol._suSize != 0) ? _vol._suSize : _dev._physicalCapacity;
			_evCounter [req._majrIdx] = static_cast <OGSS_Ulong> (floor (_ctrl->getNumberDataBlocks (req._idxDevice - _firstDevIdx) * _fillingRate) );
			req._size = _evCounter [req._majrIdx];

			LOG(INFO) << "[VD] Number of rebuilt blocks: " << _evCounter [req._majrIdx] << "("
					  << _ctrl->getNumberDataBlocks (req._idxDevice - _firstDevIdx) << ", " << req._idxDevice - _firstDevIdx << ")";

			++ _numEvents;
			req._idxVolume = _id.second;

			req._idxDevice -= _firstDevIdx;
			_ctrl->updateScheme (req);

			if (req._type == RQT_EVFLT)
				_deviceState [req._idxDevice] ._failureDate = req._date;
			else if (req._type == RQT_EVRPL)
				_deviceState [req._idxDevice] ._renewalDate = req._date;

			_evStarter [req._majrIdx] = make_pair (false, 0);

			if (! _syncOTF) {
				Request b {req};
				b._size = _vol._suSize;

				if (req._type == RQT_EVFLT)
					for (b._deviceAddress = 0; b._deviceAddress < _dev._physicalCapacity; b._deviceAddress += b._size)
						_ctrl->generateFailureRequests (b, subrequests);
				else if (req._type == RQT_EVRPL)
					for (b._deviceAddress = 0; b._deviceAddress < _dev._physicalCapacity; b._deviceAddress += b._size)
						_ctrl->generateRenewalRequests (b, subrequests);
			}

			req._idxDevice += _firstDevIdx;

			_ci->send (make_pair (MTP_DEVICE, _id.second), &req, sizeof (req) );

			for (auto & elt: subrequests) {
				elt._idxDevice += _firstDevIdx;

				_ci->send (make_pair (MTP_DEVICE, _id.second), &elt, sizeof (req) );
			}

			DLOG (INFO) << "[VD] Event management done";

			subrequests.clear ();
			continue;
		}

		req._idxDevice -= _firstDevIdx;
		_ctrl->decompose (req, subrequests);
		req._idxDevice += _firstDevIdx;
		_ci->send (make_pair (MTP_SYNCHRONIZATION, 0), &req, sizeof (req) );

		for (auto & elt: subrequests) {
			elt._idxDevice += _firstDevIdx;
			elt._nativeIdxDevice += _firstDevIdx;

			if (req._type == RQT_READ)
				elt._transferTimeB2 = req._size;
			else
				elt._transferTimeA2 = req._size;

			_ci->send (make_pair (MTP_DEVICE, _id.second), &elt, sizeof (req) );
		}

		subrequests.clear ();
	}

	_ci->send (make_pair (MTP_DEVICE, _id.second), &req, sizeof (req) );

	_ci->send (make_pair (MTP_SYNCHRONIZATION, 0), &req, sizeof (req) );
}

void
VolumeDriver::processSynchronization () {
	void								* arg;
	OGSS_Ushort							numUnprocessedEvents {_numEvents};
	Request								req;
	vector <Request>					subrequests;
	OGSS_OTFRequest						otfr;

	if (! _syncOTF) return;

	if (! numUnprocessedEvents)
		DLOG(INFO) << "[VD] #" << _id.second << " Because of no event reception, send END to DD.";
	else {
		DLOG(INFO) << "[VD] #" << _id.second << " Waiting for " << numUnprocessedEvents << " event(s) to be resolved.";

		while (numUnprocessedEvents) {
			_ci->receive (arg);
			otfr = * static_cast <OGSS_OTFRequest *> (arg); free (arg);

			req = otfr._event;

			if (req._type == RQT_EVFLT && _deviceState [req._idxDevice] .isRenewed (otfr._currentClock) ) {
				LOG(INFO) << "STOP on " << _lastEventBlockOTF [req._majrIdx] ._deviceAddress << " for " << req._majrIdx
					<< " because of the clock {" << otfr._currentClock << "}";
				req._type = RQT_EVEND;
				subrequests.push_back (req);
				-- numUnprocessedEvents;
			} else {
				req._size = max (_lastEventBlockOTF [req._majrIdx] ._size, otfr._requestSize);

				if (otfr._lastDevAddress != OGSS_ULONG_MAX) {
					_evStarter [req._majrIdx] .second = otfr._lastDevAddress;
					_lastEventBlockOTF [req._majrIdx] ._deviceAddress = otfr._lastDevAddress;
				}

				if (! generateEventRequests (req, otfr._nbRequests, subrequests) )
					-- numUnprocessedEvents;
			}

			for (auto & elt: subrequests) {
				if (elt._type == RQT_WRITE)
					elt._size = _lastEventBlockOTF [req._majrIdx] ._size;

				_ci->send (make_pair (MTP_DEVICE, _id.second), &elt, sizeof (elt) );
			}

			subrequests.clear ();
		}
	}

	req._type = RQT_END;
	_ci->send (make_pair (MTP_DEVICE, _id.second), &req, sizeof (req) );
}

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

OGSS_Bool
VolumeDriver::generateEventRequests (
	Request						& event,
	OGSS_Ulong					numBlocks,
	std::vector <Request>		& subrequests) {
	Request								req {event};

	OGSS_Ulong					toProcess {min (numBlocks, _evCounter [event._majrIdx] ) };

	while (toProcess) {
		if (_evStarter [event._majrIdx] .first
		 && _lastEventBlockOTF [event._majrIdx] ._deviceAddress  == _evStarter [event._majrIdx] .second) {
			req._type = RQT_EVEND;
			subrequests.push_back (req);
			return false;
		}

		_evStarter [event._majrIdx] .first = true;

		auto i = subrequests.size ();

		event._nativeDeviceAddress = event._deviceAddress = _lastEventBlockOTF [event._majrIdx] ._deviceAddress;

		if (event._type == RQT_EVFLT)
			_ctrl->generateFailureRequests (event, subrequests);
		else if (event._type == RQT_EVRPL)
			_ctrl->generateRenewalRequests (event, subrequests);

		for (; i < subrequests.size (); ++i) {
			subrequests [i] ._numLink = _lastLink;
			subrequests [i] ._minrIdx = i;
			subrequests [i] ._idxDevice += _firstDevIdx;
		}

		_lastEventBlockOTF [event._majrIdx] ._deviceAddress += _lastEventBlockOTF [event._majrIdx] ._size;
		if (_lastEventBlockOTF [event._majrIdx] ._deviceAddress >= _dev._physicalCapacity)
			_lastEventBlockOTF [event._majrIdx] ._deviceAddress = 0;

		-- toProcess;
		++ _lastLink;
	}

	_evCounter [event._majrIdx] -= min (numBlocks, _evCounter [event._majrIdx]);

	if (! _evCounter [event._majrIdx] ) {
		req._type = RQT_EVEND;
		subrequests.push_back (req);
		return false;
	}

	req._type = RQT_EVSTP;
	subrequests.push_back (req);

	return true;
}

void
VolumeDriver::receiveData () {
	void						* arg;
	OGSS_Bool					ack;

	_ci->receive (arg);
	_firstDevIdx = * static_cast <OGSS_Ushort *> (arg); free (arg);

	_ci->receive (arg);
	_vol = * static_cast <Volume *> (arg); free (arg);

	if (_vol._numSubVolumes)
		DLOG (INFO) << "[VD] Reception of " << _vol._numSubVolumes << "subvols";

		for (auto i = 0; i < _vol._numSubVolumes; ++i) {
			_ci->receive (arg);
			_subVols.push_back (* static_cast <Volume *> (arg) );
			free (arg);
			DLOG (INFO) << "[VD] One receive";
		}

	_ci->receive (arg);
	_dev = * static_cast <Device *> (arg); free (arg);

	ack = true;
	_ci->send (std::make_pair (MTP_HARDWARE, 0), &ack, sizeof (ack) );
}
