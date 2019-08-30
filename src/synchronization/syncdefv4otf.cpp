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

//! \file	syncdefv4otf.cpp
//! \brief	Definition of the default model (version 3) with on the fly system request generation.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <bitset>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <queue>

#include "synchronization/syncdefv4otf.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

using namespace std;

/*----------------------------------------------------------------------------*/
/* MEMBER STRUCTURES ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static OGSS_Ushort						_TIER_ {0};
static OGSS_Ushort						_VOLM_ {1};
static OGSS_Ushort						_DVIN_ {2};
static OGSS_Ushort						_DVOT_ {3};

typedef pair <OGSS_RequestIdx, OGSS_Real> __Stamp;
typedef OGSS_Bool (* __StampCmpFunc) (__Stamp, __Stamp);

struct SyncDefV4OTF::OrderedWaitQueue {
	queue <__Stamp>						_primary;
	set <__Stamp, __StampCmpFunc>		_secondary {SyncDefV4OTF::__stampCompare};

	OGSS_Real							_minValue {OGSS_REAL_MAX};
															// min between primary & secondary
	OGSS_Real							_lastValue {.0};	// last value of primary

	OGSS_Real probe ();
	OGSS_RequestIdx fetch ();
	void insert (
		const __Stamp					s);
	OGSS_Ulong size ();

private:
	void _reinject ();
};

OGSS_Real
SyncDefV4OTF::OrderedWaitQueue::probe ()
	{ return _minValue; }

OGSS_RequestIdx
SyncDefV4OTF::OrderedWaitQueue::fetch () {
	OGSS_RequestIdx						r;
	if (_primary.size () && _primary.front () .second <= _minValue) {
		r = _primary.front () .first;
		_primary.pop ();
	} else if (_secondary.size () ) {
		r = _secondary.begin () ->first;
		_secondary.erase (_secondary.begin () );
	} else return OGSS_REQUESTIDX_UND;

	if (_primary.size () &&
		( (! _secondary.size () )
			|| (_secondary.size () && _primary.front () .second <= _secondary.begin () ->second) ) )
		_minValue = _primary.front () .second;
	else if (_secondary.size () )
		_minValue = _secondary.begin () ->second;
	else
		_minValue = OGSS_REAL_MAX;

	if (! _primary.size () ) {
		_lastValue = .0;
	}

	return r;
}

void
SyncDefV4OTF::OrderedWaitQueue::insert (
	const __Stamp						s) {
	if (s.second < _minValue) {
		_minValue = s.second;
	}

	if (_secondary.size () >= 2500) {
		_reinject ();
	}

	if (_lastValue <= s.second) {
		_lastValue = s.second;
		_primary.push (s);
	} else {
		_secondary.insert (s);
	}
}

OGSS_Ulong
SyncDefV4OTF::OrderedWaitQueue::size ()
	{ return _primary.size () + _secondary.size (); }

void
SyncDefV4OTF::OrderedWaitQueue::_reinject () {
	queue <__Stamp>						tmp {};
	auto								i {0};
	auto								setIter {_secondary.begin () };

	auto								a {_primary.size () + _secondary.size () };

	tmp.swap (_primary);

	while (tmp.size () || setIter != _secondary.end () ) {
		if (setIter == _secondary.end ()
		 || (tmp.size () && tmp.front () .second <= setIter->second) ) {
			_primary.push (tmp.front () );
			DLOG_IF(INFO, tmp.front () .first == OGSS_REQUESTIDX_UND) << "Err[3] -- insert UND from primary";
			tmp.pop ();
		} else {
			_primary.push (* setIter);
			DLOG_IF(INFO, setIter->first == OGSS_REQUESTIDX_UND) << "Err[4] -- insert UND from primary";
			++ setIter;
		}
	}

	_secondary.clear ();

	DLOG_IF(INFO, a != _primary.size () ) << "Err[1] -- size difference " << a << " /= " << _primary.size ();
	DLOG_IF(INFO, _secondary.size () ) << "Err[2] -- secondary is not empty" << _secondary.size ();
}

OGSS_Bool
SyncDefV4OTF::__stampCompare (
	__Stamp								lhs,
	__Stamp								rhs) {
	return lhs.second < rhs.second
	 || (lhs.second == rhs.second && lhs.first < rhs.first); }

/*----------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define IDCLK(a)		((((a) < TABTOT / 2) ? (a) + 2 : TABTOT - (a) + 2))
#define IDCHECK(b)		((b == OGSS_ULONG_MAX) ? 0 : b)

static OGSS_Bool __requestCompare (pair <Request, OGSS_Bool> lhs, pair <Request, OGSS_Bool> rhs) {
	return (lhs.first._type == RQT_READ && rhs.first._type == RQT_WRITE)
		|| (lhs.first._type == rhs.first._type && lhs.first._date < rhs.first._date)
		|| (lhs.first._type == rhs.first._type && lhs.first._date == rhs.first._date && lhs.first._minrIdx < rhs.first._minrIdx);
}

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

SyncDefV4OTF::SyncDefV4OTF (
	std::shared_ptr <CommunicationInterface>	ci,
	HardwareParameters		& params,
	vector <Tier>			& tiers,
	vector <Volume>			& vols,
	vector <Device>			& devs,
	OGSS_DataUnit			globalDU):
	SynchronizationModel (ci),
	_hardParam (params), _tiers (tiers), _volumes (vols),
	_devices (devs), _globalDU (globalDU) {

	for (auto i = 0; i < _hardParam._numInterfaces; ++i)
		_busClocks [i] = .0;

	for (auto i = 0; i < _hardParam._numDevices; ++i)
		_devClocks [i] = .0;

	_minOTFRequestSize = max ((OGSS_Ulong) 1, _minOTFRequestSize / _globalDU._memory);

	for (auto i = 0; i < 4; ++i)
		_busWaitQueue.push_back (OrderedWaitQueue () );
}
SyncDefV4OTF::~SyncDefV4OTF () {  }

void
SyncDefV4OTF::addEntry (
	Request  					& req) {

	req._index = OGSS_RequestIdx (req._mainIdx, req._majrIdx, req._minrIdx);

	if (req._type == RQT_EVFLT || req._type == RQT_EVRPL) {
		_evByVolIdx [req._idxVolume] .push_back (req._index);
		_events [req._index] = req;
		_eventRequests [req._index] = make_pair (true, vector <Request> () );
		_eventDurations [req._index] = req._date;
		_evCntMax += req._size;

		_firstEventDate = min (_firstEventDate, req._date);

		if (req._type == RQT_EVFLT) {
			if (_volumes [req._idxVolume] ._type == VTP_DECRAID) {
				_sparedBlocks [req._idxDevice] = vector <OGSS_Bool> (_devices [req._idxDevice] ._physicalCapacity / _volumes [req._idxVolume] ._suSize, false);
				DLOG(INFO) << "Create for " << _devices [req._idxDevice] ._physicalCapacity / _volumes [req._idxVolume] ._suSize << " blocks: "
					<< _devices [req._idxDevice] ._physicalCapacity << " / "
					<< _volumes [req._idxVolume] ._suSize;
			} else _sparedBlocks [req._idxDevice] = vector <OGSS_Bool> (0);
			_OTFEvPrefetch [req._index] = vector <OGSS_Ulong> (_volumes [req._idxVolume] ._numDevices, OGSS_ULONG_MAX);
			_evByDev [req._idxDevice] .first = req._index;
			_evByDev [req._idxDevice] .second = OGSS_REQUESTIDX_UND;
			_deviceState [req._idxDevice] ._failureDate = req._date;
		} else { // RQT_EVRPL
			_rebuiltBlocks [req._idxDevice] = vector <OGSS_Bool> (_devices [req._idxDevice] ._physicalCapacity / _volumes [req._idxVolume] ._suSize, false);
			_OTFEvPrefetch [req._index] = vector <OGSS_Ulong> (_volumes [req._idxVolume] ._numDevices, OGSS_ULONG_MAX);
			_evByDev [req._idxDevice] .second = req._index;
			_deviceState [req._idxDevice] ._renewalDate = req._date;
		}
	} else	_userRequests [req._index] = req;

	_data [req._index][ARRIVL] =		req._date;

	if (! req._majrIdx) {							// Logical request
		if (! req._system) {

			_cntr [req._index][IDSTEP] = 	ARRIVL;
			_cntr [req._index][IDBUS] =	_hardParam._hostInterface;
			_cntr [req._index][START] =	true;
			_cntr [req._index][REMOVE] =	false;

			_userRslt [req._index][ARRIVL] =	req._date;
			_userRslt [req._index][IN] =	.0;
			_userRslt [req._index][OUT] =	.0;

			_requestClock [req._index._main] = make_pair (req._date, req._date);

			_busWaitQueue [_TIER_] .insert (make_pair (req._index, req._date) );

			++ _nbMainRequests;
		}
	} else if (! req._minrIdx) {					// Intermediate request

			_cntr [req._index][IDSTEP] = 	UND;
			_cntr [req._index][IDBUS] =	_tiers [_volumes [req._idxVolume] ._parent] ._interface;
			_cntr [req._index][START] =	false;
			_cntr [req._index][REMOVE] =	false;

			_userRslt [req._index][ARRIVL] =	.0;
			_userRslt [req._index][IN] =	.0;
			_userRslt [req._index][OUT] =	.0;
	} else {										// Physical request
		_data [req._index][IN]	=		req._transferTimeA3;
		_data [req._index][OUT] =		req._transferTimeB3;

		OGSS_RequestIdx					main {req._index._main, 0, 0};
		OGSS_RequestIdx					majr {req._index._main, req._index._major, 0};

		_data [main][IN] =				req._transferTimeA1;
		_data [main][OUT] =				req._transferTimeB1;
		_data [majr][IN] =				req._transferTimeA2;
		_data [majr][OUT] =				req._transferTimeB2;

		_userRequests [main] ._idxDevice = req._idxDevice;
		_userRequests [majr] ._idxDevice = req._idxDevice;

		_cntr [req._index][IDSTEP] = 	UND;
		_cntr [req._index][IDBUS] =	_volumes [req._idxVolume] ._interface;
		_cntr [req._index][START] =	false;
		_cntr [req._index][REMOVE] =	false;

		_userRslt [req._index][ARRIVL] =	.0;
		_userRslt [req._index][IN] =	.0;
		_userRslt [req._index][OUT] =	.0;
	}

	_cntr [req._index][SYSTEM] =		req._system;

//	_failedReqs [req._index] = req._failed;
}

RequestStat
SyncDefV4OTF::prepareStat (
	const OGSS_RequestIdx			idx) {
	RequestStat						stat;
	Request & r = _userRequests.at (idx);

	stat._mainIdx = r._mainIdx;
	stat._majrIdx = r._majrIdx;
	stat._minrIdx = r._minrIdx;

	stat._type = r._type;
	stat._size = r._size;

	stat._serviceTime = .0;

	stat._idxTier = 0;
	stat._idxVolume = r._idxVolume;
	stat._idxDevice = r._idxDevice;

	stat._arrivalDate = r._date;
	stat._system = r._system;

	stat._failed = r._failed;

	if (idx.isLogical () ) {
		stat._idxBus = _cntr [idx][IDBUS];
		stat._transferTime = r._transferTimeA1 + r._transferTimeA2;
		stat._waitingTime = r._waitingTime;
		stat._serviceTime = r._responseTime - stat._transferTime - stat._waitingTime;
	}
	else if (idx.isIntermediate () ) {
		stat._idxBus = _cntr [idx][IDBUS];
		stat._transferTime = r._transferTimeA2 + r._transferTimeB2;
		stat._waitingTime = r._waitingTime;
		stat._serviceTime = 0;
	}
	else {
		stat._idxBus = _cntr [idx][IDBUS];
		stat._serviceTime = r._serviceTime;
		stat._transferTime = r._transferTimeA3 + r._transferTimeB3;
		stat._waitingTime = r._waitingTime;
	}

	return stat;
}

void
SyncDefV4OTF::process () {
	_resume = Resume (_nbMainRequests, _globalDU, _tiers.size(), _volumes.size(), _devices.size() );
	_resume.setFirstDateEvent (_firstEventDate);

	LOG(INFO) << "Number of requests to process: " << _nbMainRequests << endl;

	while (_nbMainRequestsDone != _nbMainRequests) {
		auto e {_getNextRequest () };

		LOG_IF(FATAL, e == OGSS_REQUESTIDX_UND) << "No more request to process, but the whole request set is not processed yet";

		if (e.isLogical () && _cntr [e][IDSTEP] == ARRIVL)			_processToTier (e);
		else if (e.isLogical ())									_processFromTier (e);
		else if (e.isIntermediate () && _cntr [e][IDSTEP] == ARRIVL)_processToVolume (e);
		else if (e.isIntermediate ())								_processFromVolume (e);
		else {
			if (e.isPhysical () && _cntr [e][IDSTEP] == ARRIVL)		_processUserToDevice (e);
			else													_processUserFromDevice (e);
		}
	}

	cout << "-\tNumber of events to resolve: " << _events.size () << endl;

	if (_events.size () ) {
		cout << "-\tMaximum number of blocks to reconstruct: " << _evCntMax << endl;
		cout << "-\t" << (OGSS_Real) 100*_evCounter/_evCntMax << "% Blocks done [" << _evCounter << "/" << _evCntMax << "] during user requests processing" << endl;

		_resume.setPartReconstructedBlocks (100. * _evCounter / _evCntMax);

		cout << "\r-\tBlocks done [" << _evCounter << "/" << _evCntMax << "]" << flush;

		for (auto & elt: _events) {
			LOG(INFO) << "Now processing event #" << elt.first._major;
			while (_eventRequests [elt.first] .first || ! _eventRequests [elt.first] .second.empty () ) {
				_processEvent (elt.first, OGSS_REAL_MAX);
			}

			LOG(INFO) << "[SC] Event #" << elt.first._major << " is done.";
			_resume.updateEvent (elt.second, _eventDurations [elt.first]);
		}

		cout << "\r-\tNumber of blocks reconstructed: " << _evCounter << endl;
	}

	LOG(INFO) << "[SC] Process done!";
}

void
SyncDefV4OTF::createOutputFile (
	const OGSS_String		outputFile) {  }

void
SyncDefV4OTF::createResumeFile (
	const OGSS_String		resumeFile) {
	
	_resume.save (resumeFile);
}

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

OGSS_RequestIdx
SyncDefV4OTF::_getNextRequest () {
	auto								minValue {OGSS_REAL_MAX};
	auto								minIndex {OGSS_REQUESTIDX_UND};
	OrderedWaitQueue					* tmp;

	for (auto & elt: _busWaitQueue) {
		if (elt.probe () < minValue) {
			minValue = elt.probe ();
			tmp = & elt;
		}
	}

	if (minValue == OGSS_REAL_MAX)
		return OGSS_REQUESTIDX_UND;

	minIndex = tmp->fetch ();

	/*** Event management when the disk will be in idle step ***/
	if (minIndex._minor && _cntr [minIndex][IDSTEP] == ARRIVL) {
		if (_userRslt [minIndex][_cntr [minIndex][IDSTEP]] > _busClocks [_cntr [minIndex][IDBUS]]
			&& ! _evByVolIdx [_userRequests [minIndex] ._idxVolume] .empty ()) {

			for (auto elt: _evByVolIdx [_userRequests [minIndex] ._idxVolume]) {
				if (_events [elt] ._date >= _userRslt [minIndex][_cntr [minIndex][IDSTEP]]) continue;
				if (! _eventRequests [elt] .first) continue;

				_processEvent (elt, _userRslt [minIndex][_cntr [minIndex][IDSTEP]]);
			}
		}
	}
	/*** End of event management ***/

	return minIndex;
}

void
SyncDefV4OTF::_updateRequestClock (
	const OGSS_Ulong				mainIdx,
	const OGSS_Real					clock) {
	OGSS_RequestIdx				main {mainIdx, 0, 0};

	for (auto elt = _userRslt.find (main); elt != _userRslt.end (); ++elt) {
		if (elt->first._main != mainIdx) break;
		if (! _cntr [elt->first][START]) continue;
		_requestClock [mainIdx] .second = min (_requestClock [mainIdx] .second, elt->second [ARRIVL]);
	}
}

void
SyncDefV4OTF::_processToTier (
	const OGSS_RequestIdx			idx) {
	Request & r = _userRequests.at (idx);

	_userRslt [idx][IN] = max (_busClocks [_cntr [idx][IDBUS]], _userRslt [idx][ARRIVL]) + _data [idx][IN];
	r._waitingTime += max (.0,
		_busClocks [_cntr [idx][IDBUS]] - _userRslt [idx][ARRIVL]);
	_busClocks [_cntr [idx][IDBUS]] = _userRslt [idx][IN];

	for (auto elt = ++ _userRslt.find (idx); elt != _userRslt.end (); ++elt) {
		if (elt->first._main != idx._main) break;
		if (elt->first._minor) continue;
		elt->second [ARRIVL] = _userRslt [idx][IN];
		_cntr [elt->first][IDSTEP] = ARRIVL;
		_cntr [elt->first][START] = true;
		_busWaitQueue [_VOLM_] .insert (make_pair (elt->first, elt->second [ARRIVL] ) );
	}

	_updateRequestClock (idx._main, _userRslt [idx][IN]);

	_cntr [idx][IDSTEP] = IN;
	_cntr [idx][START] = false;
}

void
SyncDefV4OTF::_processToVolume (
	const OGSS_RequestIdx			idx) {
	Request & r = _userRequests.at (idx);

	_userRslt [idx][IN] = max (_busClocks [_cntr [idx][IDBUS]], _userRslt [idx][ARRIVL]) + _data [idx][IN];
	r._waitingTime += max (.0,
		_busClocks [_cntr [idx][IDBUS]] - _userRslt [idx][ARRIVL]);
	_busClocks [_cntr [idx][IDBUS]] = _userRslt [idx][IN];

	_selectUserRequestOperation (idx, _userRslt [idx][IN]);

	for (auto elt = ++ _userRslt.find (idx); elt != _userRslt.end (); ++elt) {
		if (elt->first._main != idx._main || elt->first._major != idx._major) break;
		if (_cntr [elt->first][REMOVE]) continue;
		Request & c = _userRequests.at (elt->first);
		if (c._prio || ! r._numPrioChild) {
			elt->second [ARRIVL] = _userRslt [idx][IN];
			_cntr [elt->first][IDSTEP] = ARRIVL;
			_cntr [elt->first][START] = true;
			_busWaitQueue [_DVIN_] .insert (make_pair (elt->first, elt->second [ARRIVL] ) );
		}
	}

	_updateRequestClock (idx._main, _userRslt [idx][IN]);

	_cntr [idx][IDSTEP] = IN;
	_cntr [idx][START] = false;
}

void
SyncDefV4OTF::_processUserToDevice (
	const OGSS_RequestIdx			idx) {
	Request & r = _userRequests.at (idx);

	_userRslt [idx][IN] = max (_busClocks [_cntr [idx][IDBUS]], _userRslt [idx][ARRIVL]) + _data [idx][IN];
	r._waitingTime += max (.0, _busClocks [_cntr [idx][IDBUS]] - _userRslt [idx][ARRIVL]);
	_busClocks [_cntr [idx][IDBUS]] = _userRslt [idx][IN];

	_requestClock [idx._main] .second = _userRslt [idx][IN];

	_userRslt [idx][IN] = max (_devClocks [r._idxDevice], _userRslt [idx][IN]) + r._serviceTime;
	r._waitingTime += max (.0, _devClocks [r._idxDevice] - _busClocks [_cntr [idx][IDBUS]]);
	_devClocks [r._idxDevice] = _userRslt [idx][IN];

	_busWaitQueue [_DVOT_] .insert (make_pair (idx, _userRslt [idx][IN] ) );

	_cntr [idx][IDSTEP] = IN;
}

void
SyncDefV4OTF::_processUserFromDevice (
	const OGSS_RequestIdx			idx) {
	Request & r = _userRequests.at (idx);


	_userRslt [idx][OUT] = max (_busClocks [_cntr [idx][IDBUS]], _userRslt [idx][IN]) + _data [idx][OUT];
	r._waitingTime += max (.0,
		_busClocks [_cntr [idx][IDBUS]] - _userRslt [idx][IN]);
	_busClocks [_cntr [idx][IDBUS]] = _userRslt [idx][OUT];

	OGSS_RequestIdx				parent {idx._main, idx._major, 0};
	Request							& p = _userRequests.at (parent);

	-- p._numChild;

	if (r._prio) {
		-- p._numPrioChild;

		for (auto elt = ++ _userRslt.find (parent); elt != _userRslt.end (); ++elt) {
			if (elt->first._main != idx._main || elt->first._major != idx._major) break;
			Request & c = _userRequests.at (elt->first);
			if (c._operation != r._operation) continue;
			if (! c._prio) {
				elt->second [ARRIVL] = _userRslt [idx][OUT];
				_cntr [elt->first][IDSTEP] = ARRIVL;
				if (! p._numPrioChild) {
					_cntr [elt->first][START] = true;
					_busWaitQueue [_DVIN_] .insert (make_pair (elt->first, elt->second [ARRIVL] ) );
				}
			}
		}
	} else {
		_userRslt [parent][IN] = _userRslt [idx][OUT];

		if (! p._numChild) {
			_cntr [parent][START] = true;
			_busWaitQueue [_VOLM_] .insert (make_pair (parent, _userRslt [parent][IN] ) );
		}
	}

	_cntr [idx][IDSTEP] = OUT;
	_cntr [idx][START] = false;

	_requestClock [idx._main] .second = _userRslt [idx][OUT];

	sendStat (prepareStat (idx) );
	_resume.updateStats (prepareStat (idx) );

	_manageBlocks (r);
}

void
SyncDefV4OTF::_processSystem (
	const OGSS_RequestIdx				evIdx,
	Request								& req,
	OGSS_Bool							direction) {
	if (req._serviceTime == -1) return;			// Request targetted a failed device.

	if (! direction) {
		_busClocks [_volumes [req._idxVolume] ._interface]
			+= req._transferTimeA3;
		req._date = _devClocks [req._idxDevice]
			= max (_devClocks [req._idxDevice], _busClocks [_volumes [req._idxVolume] ._interface] )
			+ req._serviceTime;
	} else {
		req._date = _busClocks [_volumes [req._idxVolume] ._interface]
			= _devClocks [req._idxDevice] + req._transferTimeB3;

		_eventDurations [evIdx] = max (_eventDurations [evIdx],
			_busClocks [_volumes [req._idxVolume] ._interface] );

		_manageBlocks (req);
	}
}

void
SyncDefV4OTF::_processFromVolume (
	const OGSS_RequestIdx			idx) {
	Request & r = _userRequests.at (idx);

	_userRslt [idx][OUT] = max (_busClocks [_cntr [idx][IDBUS]], _userRslt [idx][IN]) + _data [idx][OUT];
	r._waitingTime += max (.0,
		_busClocks [_cntr [idx][IDBUS]] - _userRslt [idx][IN]);
	_busClocks [_cntr [idx][IDBUS]] = _userRslt [idx][OUT];

	OGSS_RequestIdx				parent {idx._main, 0, 0};
	Request							& p = _userRequests.at (parent);

	-- p._numChild;

	_userRslt [parent][IN] = _userRslt [idx][OUT];
	if (! p._numChild) {
		_cntr [parent][START] = true;
		_busWaitQueue [_TIER_] .insert (make_pair (parent, _userRslt [parent][IN] ) );
	}

	_cntr [idx][IDSTEP] = OUT;
	_cntr [idx][START] = false;

	sendStat (prepareStat (idx) );
	_resume.updateStats (prepareStat (idx) );
}

void
SyncDefV4OTF::_processFromTier (
	const OGSS_RequestIdx			idx) {
	OGSS_Ulong				printStep {max (_nbMainRequests/100, (OGSS_Ulong) 1) };
	Request & r = _userRequests.at (idx);

	_userRslt [idx][OUT] = max (_busClocks [_cntr [idx][IDBUS]], _userRslt [idx][IN]) + _data [idx][OUT];
	r._waitingTime += max (.0,
		_busClocks [_cntr [idx][IDBUS]] - _userRslt [idx][IN]);
	_busClocks [_cntr [idx][IDBUS]] = _userRslt [idx][OUT];

	_cntr [idx][IDSTEP] = OUT;
	_cntr [idx][START] = false;

	if (idx._main == _starter) {
		int y = _starter;
		do { if (++y > _nbMainRequests) break;
		} while (_cntr [OGSS_RequestIdx (y, 0, 0)][IDSTEP] == OUT);
		_starter = y;
	}

	r._responseTime = _userRslt [idx][OUT] - r._date;

	if(_nbMainRequestsDone == 0){
		cout << "\r-\t" << 0 << "% Requests Done [0/" << _nbMainRequests << "]" << flush;
	}

	++ _nbMainRequestsDone;
//	DLOG(INFO) << "Request " << idx << " done";

	if(!(_nbMainRequestsDone % printStep)){
		if(printStep == 1){
			cout << "\r-\t" << (OGSS_Real)_nbMainRequestsDone/_nbMainRequests*100 << "% Requests Done [" << _nbMainRequestsDone << "/" << _nbMainRequests << "]" << flush;
		}else
			cout << "\r-\t" << (OGSS_Real)_nbMainRequestsDone/printStep << "% Requests Done [" << _nbMainRequestsDone << "/" << _nbMainRequests << "]" << flush;
		if(_nbMainRequestsDone == _nbMainRequests){
			cout << endl;
		}
	}

	_requestClock [idx._main] .second = OGSS_REAL_MAX;

	sendStat (prepareStat (idx) );
	_resume.updateStats (prepareStat (idx) );
}

OGSS_Bool
SyncDefV4OTF::_requestSystemRequests (
	const OGSS_RequestIdx				evIdx,
	const OGSS_Real						clock,
	vector <Request>					& subrequests) {
	void								* arg;
	OGSS_Ulong							link;

	if (_lastOTFEvent != OGSS_REQUESTIDX_UND) {
		if (_lastOTFEvent == evIdx) {	// Targeted requests are in ZMQ
			link = _OTFBuffer._numLink;
		} else {						// Need to store ZMQ requests in event vector
			do {
				_OTFEvRequests [_lastOTFEvent] .second.push_back (_OTFBuffer);
				_ci->receive (arg); _OTFBuffer = * static_cast <Request *> (arg); free (arg);
			} while (_OTFBuffer._type != RQT_END && _OTFBuffer._type != RQT_EVEND && _OTFBuffer._type != RQT_EVSTP);

			if (_OTFBuffer._type == RQT_EVEND)				{ _eventRequests [_lastOTFEvent] .first = false; LOG(INFO) << "Receive[1] END for " << _lastOTFEvent._major; }
			_lastOTFEvent = OGSS_REQUESTIDX_UND;
		}
	}

	if (_lastOTFEvent == OGSS_REQUESTIDX_UND && _OTFEvRequests [evIdx] .second.empty () ) {
		auto							tmp {OGSS_ULONG_MAX};

		if (_events [evIdx] ._type == RQT_EVRPL && _evLastDevAddress [_events [evIdx] ._idxDevice] != OGSS_ULONG_MAX) {
			tmp = _evLastDevAddress [_events [evIdx] ._idxDevice];
			_evLastDevAddress [_events [evIdx] ._idxDevice] = OGSS_ULONG_MAX;
		}

		OGSS_OTFRequest					r {_nbSystemRequestsGenerated, _minOTFRequestSize,
											tmp, clock, _events [evIdx]};

		_ci->send (make_pair (MTP_VOLUME, _events [evIdx] ._idxVolume), &r, sizeof (OGSS_OTFRequest) );
	}

	if (_OTFEvRequests [evIdx] .second.empty () ) {
		if (_lastOTFEvent == OGSS_REQUESTIDX_UND) {
			_lastOTFEvent = evIdx;
			_ci->receive (arg); _OTFBuffer = * static_cast <Request *> (arg); free (arg);
			link = _OTFBuffer._numLink;

			if (_OTFBuffer._type == RQT_END || _OTFBuffer._type == RQT_EVEND || _OTFBuffer._type == RQT_EVSTP) {
				if (_OTFBuffer._type == RQT_EVEND)			{ _eventRequests [evIdx] .first = false; LOG(INFO) << "Receive[2] END for " << evIdx._major; }
				_lastOTFEvent = OGSS_REQUESTIDX_UND;
				return false;
			}
		}

		do {
			subrequests.push_back (_OTFBuffer);

			_ci->receive (arg); _OTFBuffer = * static_cast <Request *> (arg); free (arg);

			if (_OTFBuffer._type == RQT_END || _OTFBuffer._type == RQT_EVEND || _OTFBuffer._type == RQT_EVSTP) {
				if (_OTFBuffer._type == RQT_EVEND)			{ _eventRequests [evIdx] .first = false; LOG(INFO) << "Receive[2] END for " << evIdx._major; }
				_lastOTFEvent = OGSS_REQUESTIDX_UND;
				return false;
			}
		} while (link == _OTFBuffer._numLink);
	} else {
		auto counter {0};
		link = _OTFEvRequests [evIdx] .second.front () ._numLink;
		for (auto elt = _OTFEvRequests [evIdx] .second.begin () + _OTFEvRequests [evIdx] .first;
			elt != _OTFEvRequests [evIdx] .second.end ();
			++elt) {
			if (elt->_numLink != link)						break;
			subrequests.push_back (*elt);
			++counter;
		}

		_OTFEvRequests [evIdx] .first += counter;
		if (_OTFEvRequests [evIdx] .first >= _OTFEvRequests [evIdx] .second.size () )
			_OTFEvRequests [evIdx] .second.clear ();
	}

	return true;
}

void
SyncDefV4OTF::_processEvent (
	const OGSS_RequestIdx				evIdx,
	const OGSS_Real						deadline) {
	OGSS_Real							clock {max (_events [evIdx] ._date, _eventDurations [evIdx] )};
	OGSS_Ulong							lkId;
	Request								tmp;
	RequestSet							reqs (__requestCompare);
	vector <Request>					subrequests;
	OGSS_String							order {""};
	OGSS_String							opcode {""};

	while (clock < deadline) {
		if (! _eventRequests [evIdx] .first && _eventRequests [evIdx] .second.empty () ) return;
		if (_eventRequests [evIdx] .second.empty () )
										_requestSystemRequests (evIdx, clock, subrequests);

		if (! subrequests.size () ) return;

		if (_events [evIdx] ._type == RQT_EVFLT) {
			if (_sparedBlocks [_events [evIdx] ._idxDevice][subrequests.front () ._nativeDeviceAddress / _volumes [_events [evIdx] ._idxVolume] ._suSize]) {
				subrequests.clear ();
				continue;
			}
		} else if (_events [evIdx] ._type == RQT_EVRPL) {
			if (_rebuiltBlocks [_events [evIdx] ._idxDevice][subrequests.front () ._nativeDeviceAddress / _volumes [_events [evIdx] ._idxVolume] ._suSize]) {
				subrequests.clear ();
				continue;
			}
		}

		_selectSysRequestOperation (evIdx, subrequests, reqs);

		subrequests.clear ();

		while (! reqs.empty () ) {
			tmp = reqs.begin () ->first;
			auto direction = reqs.begin () ->second;

			reqs.erase (reqs.begin () );

			_processSystem (evIdx, tmp, direction);

			if (! direction) {
				reqs.insert (make_pair (tmp, true) );

				if (tmp._transferTimeA3 + tmp._transferTimeB3 < 0)
					LOG(INFO) << "Issue on (transfer) -/" << tmp._majrIdx << "/" << tmp._minrIdx << " -> " << tmp._idxDevice << "/" << tmp._size;
				if (tmp._serviceTime < 0)
					LOG(INFO) << "Issue on (service) -/" << tmp._majrIdx << "/" << tmp._minrIdx << " -> " << tmp._idxDevice << "/" << tmp._size << "/" << tmp._serviceTime;

			} else if (tmp._type == RQT_READ) {
				_OTFEvPrefetch [evIdx][tmp._idxDevice] = tmp._deviceAddress;
			} else if (_events [evIdx] ._type == RQT_EVFLT) {
				_evLastDevAddress [tmp._nativeIdxDevice] = tmp._nativeDeviceAddress + tmp._size;
			}

			clock = max (clock, _eventDurations [evIdx]);

			if (_events [evIdx] ._type == RQT_EVFLT
			 && _deviceState [_events [evIdx] ._idxDevice] .isRenewed (clock) ) {
				_emptyBuffer (evIdx);

				if (_eventRequests [_lastOTFEvent] .first)
					_requestSystemRequests (evIdx, clock, subrequests);
			}
		}

		if (_evCntMax > 1000 && ! (_evCounter % (_evCntMax / 100) ) )
			cout << "\r-\t" << (OGSS_Ushort) ((OGSS_Real) 100*_evCounter/_evCntMax) << "% Blocks done [" << _evCounter << "/" << _evCntMax << "]" << flush;
	}
}

void
SyncDefV4OTF::_selectSysRequestOperation (
	const OGSS_RequestIdx				evIdx,
	vector <Request>					& subrequests,
	RequestSet							& reqs) {

	OGSS_Ulong							nID, nDA;

	for (auto elt: subrequests)
		if (elt._type == RQT_WRITE) {
			nID = elt._nativeIdxDevice;
			nDA = elt._nativeDeviceAddress / _volumes [elt._idxVolume] ._suSize;
			break;
		}

	for (auto elt: subrequests)	{
		if (_events [evIdx] ._type == RQT_EVRPL) {
			if (elt._operation == ROP_RECOVERY && _sparedBlocks [nID][nDA])
				continue;
			if (elt._operation == ROP_COPY && ! _sparedBlocks [nID][nDA])
				continue;
		}

		if (elt._type == RQT_READ
		 && _OTFEvPrefetch [evIdx][elt._idxDevice] <= elt._deviceAddress
		 && elt._deviceAddress < _OTFEvPrefetch [evIdx][elt._idxDevice] + _minOTFRequestSize) {
			continue;
		}

		reqs.insert (make_pair (elt, false) );
	}
}

void
SyncDefV4OTF::_selectUserRequestOperation (
	const OGSS_RequestIdx				idx,
	const OGSS_Real						clock) {
	auto								jdx {idx};
	auto								& parent {_userRequests.at (idx)};

	if (! parent._multiple) return;

	for (jdx._minor = parent._numChild; jdx._minor > 0; --jdx._minor) {
		auto							& elt {_userRequests.at (jdx)};
		auto							stp {elt._nativeDeviceAddress / _volumes [elt._idxVolume] ._suSize};
		auto							idx {elt._nativeIdxDevice};

		if (elt._type == RQT_READ) {
			if (elt._operation == ROP_NATIVE) {
				if ((_deviceState [idx] .isFailed (clock) && ! _sparedBlocks [idx][stp])
				 || (_deviceState [idx] .isRenewed (clock) && ! _rebuiltBlocks [idx][stp]))
					_cntr [jdx][REMOVE] = true;
			} else if (elt._operation == ROP_RECOVERY) {
				if ((! _deviceState [idx] .isFailed (clock) && ! _deviceState [idx] .isRenewed (clock))
				 || _sparedBlocks [idx][stp] || _rebuiltBlocks [idx][stp])
					_cntr [jdx][REMOVE] = true;
			} else if (elt._operation == ROP_COPY) {
				if (! _deviceState [idx] .isRenewed (clock)
				 || ! _sparedBlocks [idx][stp] || _rebuiltBlocks [idx][stp])
					_cntr [jdx][REMOVE] = true;
			} else if (elt._operation == ROP_UPDATE) {
				if (! _deviceState [idx] .isFailed (clock) || _sparedBlocks [idx][stp])
					_cntr [jdx][REMOVE] = true;
			} else if (elt._operation == ROP_RENEW) {
				if (! _deviceState [idx] .isRenewed (clock) || _rebuiltBlocks [idx][stp])
					_cntr [jdx][REMOVE] = true;
			}
		} else {
			if (elt._operation == ROP_NATIVE) {
				if (_deviceState [idx] .isFailed (clock) || _deviceState [idx] .isRenewed (clock) )
					_cntr [jdx][REMOVE] = true;
			} else if (elt._operation == ROP_UPDATE) {
				if (_deviceState [idx] .isRenewed (clock) || ! _deviceState [idx] .isFailed (clock) )
					_cntr [jdx][REMOVE] = true;
			} else if (elt._operation == ROP_RENEW) {
				if (! _deviceState [idx] .isRenewed (clock) )
					_cntr [jdx][REMOVE] = true;
			}
		}
	}
}

void
SyncDefV4OTF::_manageBlocks (
	const Request						req) {

	if (req._type != RQT_WRITE) return;
	if (req._failed) return;

	if (req._mainIdx == OGSS_ULONG_MAX) {
		if (req._idxDevice != req._nativeIdxDevice) {
			_sparedBlocks [req._nativeIdxDevice][req._deviceAddress / _volumes [req._idxVolume] ._suSize] = true;
			++ _evCounter;
		} else {
			_rebuiltBlocks [req._idxDevice][req._deviceAddress / _volumes [req._idxVolume] ._suSize] = true;
			++ _evCounter;
		}
	} else {
		if (_evByDev.find (req._nativeIdxDevice) == _evByDev.end () ) return;
		if (req._date < _events [_evByDev [req._nativeIdxDevice] .first] ._date) return;
		if (req._nativeIdxDevice != req._idxDevice) {
			_sparedBlocks [req._nativeIdxDevice][req._nativeDeviceAddress / _volumes [req._idxVolume] ._suSize] = true;
			++ _evCounter;
		} else {
			if (_evByDev [req._idxDevice] .second != OGSS_REQUESTIDX_UND
				|| req._date < _events [_evByDev [req._idxDevice] .second] ._date) return;
			_rebuiltBlocks [req._idxDevice][req._deviceAddress / _volumes [req._idxVolume] ._suSize] = true;
			++ _evCounter;
		}
	}
}

void
SyncDefV4OTF::_emptyBuffer (
	const OGSS_RequestIdx				evIdx) {
	void *								arg;
	if (_lastOTFEvent == evIdx) {
		do {
			_OTFEvRequests [_lastOTFEvent] .second.push_back (_OTFBuffer);
			_ci->receive (arg); _OTFBuffer = * static_cast <Request *> (arg); free (arg);
		} while (_OTFBuffer._type != RQT_END && _OTFBuffer._type != RQT_EVEND && _OTFBuffer._type != RQT_EVSTP);

		if (_OTFBuffer._type == RQT_EVEND)				{ _eventRequests [_lastOTFEvent] .first = false; LOG(INFO) << "Receive[1] END for " << _lastOTFEvent._major; }
		_lastOTFEvent = OGSS_REQUESTIDX_UND;
	} else {
		_OTFEvRequests [evIdx] .second.clear ();
		_OTFEvRequests [evIdx] .first = 0;
	}
}
