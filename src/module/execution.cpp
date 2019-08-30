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

//! \file	execution.cpp
//! \brief	Definition of the execution module class.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <algorithm>

#include "computation/comphdd.hpp"
#include "computation/compinterface.hpp"
#include "computation/compssd.hpp"
#include "computation/compnvram.hpp"

#include "module/execution.hpp"

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

Execution::Execution (
	const OGSS_String		configurationFile):
	Module (configurationFile, make_pair (MTP_EXECUTION, 0) ) {
	OGSS_String				sncModel;
	OGSS_SynchronizationType	syncType = SNC_TOTAL;

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

Execution::~Execution () {  }

void
Execution::processExtraction () {
	receiveData ();

	_numRealVolumes = 0;
	for (auto i = 0; i < _hardParam._numVolumes; ++i) {
		i += _volumes [i] ._numSubVolumes;
		++ _numRealVolumes;
	}

	initComputationModels (_cfg);
}

void
Execution::processDecomposition () {
	void					* arg;
	Request					req;
	OGSS_Ushort				counter {_numRealVolumes};

	DLOG(INFO) << "[EX] Number of required thread terminations: " << counter;

	while (counter) {
		_ci->receive (arg);
		req = * static_cast <Request *> (arg); free (arg);

		switch (req._type) {
			case RQT_END:
				-- counter;
//				DLOG(INFO) << "[EX] Received an end request, wait for " << counter << " more!";
				continue;
			case RQT_EVEND:
//				LOG(INFO) << "[EX] #" << _id.second << " Reception of end event (" << req._date << ", " << req._idxDevice << ")";
				break;
			case RQT_EVSTP:
				break;
			case RQT_EVFLT: case RQT_EVRPL:
//				DLOG(INFO) << "[EX] Received an event " << req._majrIdx << "/" << req._minrIdx;
				break;
			default:
				treatRequest (req);
		}

		_ci->send (make_pair (MTP_SYNCHRONIZATION, 0), &req, sizeof (Request) );
	}

	DLOG (INFO) << "[EX] Send ending to SC";

	if (! _syncProc) {
		req._type = RQT_END;
		_ci->send (make_pair (MTP_SYNCHRONIZATION, 0), &req, sizeof (Request) );
	}
}

void
Execution::processSynchronization () {
	if (! _syncOTF) return;

	_syncProc = true;

	processDecomposition ();
}

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
Execution::treatRequest (
	Request					& req) {

	LOG_IF (FATAL, req._idxDevice >= _devices.size () ) << "Bad device index on ["
		<< req._mainIdx << "/" << req._majrIdx << "/" << req._minrIdx << "] -- " << req._idxDevice;

	if (req._failed)
		req._serviceTime = .0;
	else {
		if (_devices [req._idxDevice] ._type == DTP_HDD)
			_hdd->compute (req);
		else if(_devices [req._idxDevice]._type == DTP_SSD){
			_ssd->compute (req);
		}else{
			_nvram->compute (req);
		}
	}

	_interface->compute (req);
}

void
Execution::initComputationModels (
	const OGSS_String		configurationFile) {
	OGSS_String				modelType;
	OGSS_HDDComputationType	hddType;
	OGSS_SSDComputationType	ssdType;
	OGSS_NVRAMComputationType nvramType;
	OGSS_InterfaceComputationType	interfaceType;

	modelType = XMLParser::getComputationModel (configurationFile, PTP_HDD);

	auto findRes1 = find_if (HDDComputationNameMap.begin (), HDDComputationNameMap.end (),
		[&] (const pair <OGSS_HDDComputationType, OGSS_String> & elt)
		{ return ! elt.second.compare (modelType); } );
	if (findRes1 != HDDComputationNameMap.end () )
		hddType = findRes1->first;
	else {
		DLOG (INFO) << "Default HDD computation model is chosen";
		hddType = HCP_DEFAULT;
	}

	switch (hddType) {
		case HCP_TOTAL:
		case HCP_DEFAULT:
		_hdd = make_unique <CompHDD> (_devices);
	}

	modelType = XMLParser::getComputationModel (configurationFile, PTP_SSD);

	auto findRes2 = find_if (SSDComputationNameMap.begin (), SSDComputationNameMap.end (),
		[&] (const pair <OGSS_SSDComputationType, OGSS_String> & elt)
		{ return ! elt.second.compare (modelType); } );
	if (findRes2 != SSDComputationNameMap.end () )
		ssdType = findRes2->first;
	else {
		DLOG (INFO) << "Default SSD computation model is chosen";
		ssdType = SCP_DEFAULT;
	}

	switch (ssdType) {
		case SCP_TOTAL:
		case SCP_DEFAULT:
		_ssd = make_unique <CompSSD> (_devices);
	}

	modelType = XMLParser::getComputationModel (configurationFile, PTP_NVRAM);

	auto findRes4 = find_if (NVRAMComputationNameMap.begin (), NVRAMComputationNameMap.end (),
		[&] (const pair <OGSS_NVRAMComputationType, OGSS_String> & elt)
		{ return ! elt.second.compare (modelType); } );
	if (findRes4 != NVRAMComputationNameMap.end () )
		nvramType = findRes4->first;
	else {
		DLOG (INFO) << "Default NVRAM computation model is chosen";
		nvramType = NCP_DEFAULT;
	}

	switch (nvramType) {
		case NCP_TOTAL:
		case NCP_DEFAULT:
		_nvram = make_unique <CompNVRAM> (_devices);
	}

	modelType = XMLParser::getComputationModel (configurationFile, PTP_INTERFACE);
	
	auto findRes3 = find_if (InterfaceComputationNameMap.begin (), InterfaceComputationNameMap.end (),
		[&] (const pair <OGSS_InterfaceComputationType, OGSS_String> & elt)
		{ return ! elt.second.compare (modelType); } );
	if (findRes3 != InterfaceComputationNameMap.end () )
		interfaceType = findRes3->first;
	else {
		DLOG (INFO) << "Default bus computation model is chosen";
		interfaceType = ICP_DEFAULT;
	}

	switch (interfaceType) {
		case ICP_TOTAL:
		case ICP_DEFAULT:
		_interface = make_unique <CompInterface> (_hardParam, _tiers, _volumes,
			_devices, _interfaces);
	}
}
