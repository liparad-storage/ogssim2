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

//! \file	synchronization.cpp
//! \brief	Definition of the synchronization module class.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <algorithm>
#include <iomanip>

#include "module/synchronization.hpp"

#include "synchronization/syncdefv2.hpp"
#include "synchronization/syncdefv4otf.hpp"
#include "synchronization/syncsingledisk.hpp"

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

Synchronization::Synchronization (
	const OGSS_String		configurationFile):
	Module (configurationFile, make_pair (MTP_SYNCHRONIZATION, 0) ) {

	_outputFile = XMLParser::getFilePath (_cfg, FTP_RESULT, false);
	_resumeFile = XMLParser::getFilePath (_cfg, FTP_RESUME, false);

	_globalDU = XMLParser::getDataUnit(_cfg, PTP_GLOBAL);
}

Synchronization::~Synchronization () {
	_ci->request (make_pair (MTP_TOTAL, 0) );
}

void
Synchronization::processExtraction () {
	receiveData ();

	_numRealVolumes = 0;
	for (auto i = 0; i < _hardParam._numVolumes; ++i) {
		i += _volumes [i] ._numSubVolumes;
		++ _numRealVolumes;
	}

	initSyncModel ();
}

void
Synchronization::processDecomposition () {
	void					* arg;
	Request					req;
	RequestStat				stat;
	OGSS_Ushort				unfinished = _numRealVolumes + 2;

	OGSS_Ulong				_nbRequests = 0;
	OGSS_Ulong				_nbLogicalRequests = 0;
	OGSS_Ulong				_nbIntermediateRequests = 0;
	OGSS_Ulong				_nbPhysicalRequests = 0;

	bool					_myFirstTime = false;

	OGSS_Ulong				printStep = max (_numLogicalRequests/20, (OGSS_Ulong) 1);

	while (unfinished) {
		_ci->receive (arg);
		req = * static_cast <Request *> (arg); free (arg);
//		DLOG (INFO) << "Received [" << req._mainIdx << "/" << req._majrIdx << "/" << req._minrIdx << "]"
//			<< " (" << req._numChild << ")";

		if(!_myFirstTime){
			cout << "-\tTotal number of logical requests to compute: " << _numLogicalRequests << endl;
			cout << "-\tMinimum number of requests to compute:  " << _numLogicalRequests * 3 << endl;

			cout << /*setw(35) << left << */"-\tLogical Requests computed: " << /*right << */_nbLogicalRequests
			<< /*setw(35) << left << */" - Intermediate Requests computed: " << /*right << */_nbIntermediateRequests
			<< /*setw(35) << left << */" - Physical Requests computed: " << /*right << */_nbPhysicalRequests << flush;

			_myFirstTime = true;
		}

		if (req._type == RQT_END) {
			--unfinished;
//			DLOG(INFO) << "[SC] Received an end request, wait for " << unfinished << " more!";
		} else if (req._type == RQT_EVFLT || req._type == RQT_EVRPL) {
			stat._type = req._type;
			stat._arrivalDate = req._date;
			stat._idxDevice = req._idxDevice;

			req._system = true;

			_sync->addEntry (req);

			_evtStats.push_back (stat);
		} else {
			_sync->addEntry (req);
			_nbRequests++;
			if(req._majrIdx == 0){ //Logical Request
				_nbLogicalRequests++;
			}
			else if(req._minrIdx == 0){ //Intermediate Request
				_nbIntermediateRequests++;
			}
			else{ //Physical Request
				_nbPhysicalRequests++;
			}
			if(!(_nbRequests%(printStep))){
				cout << /*setw(35) << left << */"\r-\tLogical Requests computed: " << /*right << */_nbLogicalRequests
				<< /*setw(35) << left << */" - Intermediate Requests computed: " << /*right << */_nbIntermediateRequests
				<< /*setw(35) << left << */" - Physical Requests computed: " << /*right << */_nbPhysicalRequests << flush;
			}
		}
	}

	cout << /*setw(35) << left << */"\r-\tLogical Requests computed: " << /*right << */_nbLogicalRequests
		<< /*setw(35) << left << */" - Intermediate Requests computed: " << /*right << */_nbIntermediateRequests
		<< /*setw(35) << left << */" - Physical Requests computed: " << /*right << */_nbPhysicalRequests << endl;
	cout <<"-\tTotal Requests computed: "<< _nbRequests << endl;
	cout << "-\tEnd" << endl;

//	_ci->releaseBarrier (numMonoProcessSynchBarrier
//		+ numMultiProcessSynchBarrier * _numRealVolumes);
}

void
Synchronization::processSynchronization () {
	void					* arg;
	RequestStat				stat;

	cout << "STEP 3 - Synchronization" << endl;
	cout << "-\tStart" << endl;

	for (auto & elt: _evtStats)
		_ci->send (make_pair (MTP_EVALUATION, 0), &elt, sizeof (RequestStat) );

	_sync->process ();

	stat._type = RQT_END;
	_ci->send (make_pair (MTP_EVALUATION, 0), &stat, sizeof (RequestStat) );
	_ci->receive (arg); free (arg);

	cout << "\tEnd" << endl;

	cout << "STEP 4 - Output" << endl;
	cout << "-\tStart" << endl;
	cout << "-\tStart writing Output file: " << _outputFile << endl;
	_sync->createOutputFile (_outputFile);
	cout << "-\tEnd writing Output file" << endl;
	cout << "-\tStart writing Resume file: " << _resumeFile << endl;
	_sync->createResumeFile (_resumeFile);
	cout << "-\tEnd writing Resume file" << endl;
	cout << "-\tEnd" << endl;
}

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
Synchronization::initSyncModel () {
	OGSS_String				modelType;
	OGSS_SynchronizationType	syncType;

	modelType = XMLParser::getComputationModel (_cfg, PTP_SYNC);

	auto findRes = find_if (SynchronizationNameMap.begin (), SynchronizationNameMap.end (),
		[&] (const pair <OGSS_SynchronizationType, OGSS_String> & elt)
		{ return ! elt.second.compare (modelType); } );
	if (findRes != SynchronizationNameMap.end () )
		syncType = findRes->first;
	else {
		DLOG (INFO) << "Default synchronization model is chosen (" << modelType << ")";
		syncType = SNC_DEFV2;
	}

	switch (syncType) {
		case SNC_DEFV4OTF:
			_sync = make_unique <SyncDefV4OTF> (_ci, _hardParam, _tiers, _volumes,
				_devices, _globalDU);
			break;
		case SNC_SINGLEDISK: 
			_sync = make_unique <SyncSingleDisk> (_ci, _hardParam, _globalDU);
			break;
		case SNC_TOTAL: case SNC_DEFAULT: case SNC_PARALLEL: case SNC_QUEUE:
		case SNC_DEFV2:
			_sync = make_unique <SyncDefV2> (_ci, _hardParam, _tiers, _volumes,
				_devices, _interfaces, _globalDU);
	}
}
