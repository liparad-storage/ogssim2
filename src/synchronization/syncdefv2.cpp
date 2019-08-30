/*
 * Copyright UVSQ - CEA/DAM/DIF (2017-2018)
 * Contributors:  Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *                Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *				  Maxence JOULIN
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

//! \file	syncdefv2.cpp
//! \brief	Definition of the default model (version 2).

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <cmath>

#include "synchronization/syncdefv2.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

using namespace std;

/*----------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define IDCLK(a)		((((a) < TABTOT / 2) ? (a) + 2 : TABTOT - (a) + 2))
#define IDCHECK(b)		((b == OGSS_ULONG_MAX) ? 0 : b)
#define IDPRINT(a)		"[" << IDCHECK(get<0>(a)) << "/" << IDCHECK(get<1>(a)) << "/" << IDCHECK(get<2>(a)) << "]"

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

SyncDefV2::SyncDefV2 (
	std::shared_ptr <CommunicationInterface>	ci,
	HardwareParameters		& params,
	vector <Tier>			& tiers,
	vector <Volume>			& vols,
	vector <Device>			& devs,
	vector <Interface>		& intfs,
	OGSS_DataUnit			globalDU):
	SynchronizationModel (ci),
	_hardParam (params), _tiers (tiers), _volumes (vols),
	_devices (devs), _interfaces (intfs), _globalDU (globalDU) {
		_mainRequestsDone = 0;
		_nbRequests = 0;
	}
SyncDefV2::~SyncDefV2 () {  }

void
SyncDefV2::addEntry (
	Request  				&req) {
	index_t					idx
		= make_tuple (req._mainIdx, req._majrIdx, req._minrIdx);
	_data [idx][ARRIVL] = req._date;
	_failedReqs [idx] = req._failed;

	if (req._minrIdx != 0) {
		_data [idx][TO_TIR] = req._transferTimeA1;
		_data [idx][TO_VOL] = req._transferTimeA2;
		_data [idx][TO_DEV] = req._transferTimeA3;
		_data [idx][SERVCE] = req._serviceTime;
		_data [idx][FM_DEV] = req._transferTimeB3;
		_data [idx][FM_VOL] = req._transferTimeB2;
		_data [idx][FM_TIR] = req._transferTimeB1;
	}

	if (req._type & RQT_WRITE)
		_cntr [idx][RQTYPE] = 1;
	else
		_cntr [idx][RQTYPE] = 0;

	_cntr [idx][RQSIZE] = req._size;

	if (req._majrIdx == 0){
		_nbRequests ++;
		_cntr [idx][IDSTEP] = ARRIVL;
	}
	else {
		_cntr [idx][IDSTEP] = UND;
		_data [idx][ARRIVL] = numeric_limits <double> ::max ();
	}
	_cntr [idx][NBCHLD] = req._numChild;
	_cntr [idx][NBPRIO] = req._numPrioChild;

	if (req._minrIdx != 0 || req._numChild == 0) {
		_cntr [idx][IDDEVC] = req._idxDevice;
		_cntr [idx][IDVOLM] = _devices [_cntr [idx][IDDEVC]] ._parent;
		_cntr [idx][IDTIER] = _volumes [_cntr [idx][IDVOLM]] ._parent;

		_cntr [idx][IDBUSD] = _volumes [_cntr [idx][IDVOLM]] ._interface;
		_cntr [idx][IDBUSV] = _tiers   [_cntr [idx][IDTIER]] ._interface;
		_cntr [idx][IDBUST] = _hardParam._hostInterface;

		_cntr [make_tuple (req._mainIdx, req._majrIdx, 0)][IDBUST] = _cntr [idx][IDBUST];
		_cntr [make_tuple (req._mainIdx, req._majrIdx, 0)][IDBUSV] = _cntr [idx][IDBUSV];
		_cntr [make_tuple (req._mainIdx, req._majrIdx, 0)][IDBUSD] = _cntr [idx][IDBUSD];

		_cntr [make_tuple (req._mainIdx, 0, 0)][IDBUST] = _cntr [idx][IDBUST];
		_cntr [make_tuple (req._mainIdx, 0, 0)][IDBUSV] = _cntr [idx][IDBUSV];
		_cntr [make_tuple (req._mainIdx, 0, 0)][IDBUSD] = _cntr [idx][IDBUSD];

		_data [make_tuple (req._mainIdx, req._majrIdx, 0)][TO_VOL] = _data [idx][TO_VOL];
		_data [make_tuple (req._mainIdx, req._majrIdx, 0)][FM_VOL] = _data [idx][FM_VOL];

		_data [make_tuple (req._mainIdx, 0, 0)][TO_TIR] = _data [idx][TO_TIR];
		_data [make_tuple (req._mainIdx, 0, 0)][FM_TIR] = _data [idx][FM_TIR];
	}

	_rslt [idx][ARRIVL] = 0;

	_idpt [idx][IDNEXT]
		= make_tuple (OGSS_ULONG_MAX, OGSS_ULONG_MAX, OGSS_ULONG_MAX);
	_idpt [idx][IDPREV] = _idpt [idx][IDNEXT];

	for (auto &e: _rslt [idx])
		e = - numeric_limits <double> ::max ();

	if (req._majrIdx == 0)
		_rslt [idx][ARRIVL] = _data [idx][ARRIVL];
}

RequestStat
SyncDefV2::prepareStat (
	const index_t			idx) {
	RequestStat				stat;

	stat._mainIdx = get<0> (idx);
	stat._majrIdx = get<1> (idx);
	stat._minrIdx = get<2> (idx);

	if (_cntr [idx][RQTYPE])
		stat._type = RQT_WRITE;
	else
		stat._type = RQT_READ;

	stat._size = _cntr [idx][RQSIZE];

	stat._serviceTime = .0;

	stat._idxTier = _cntr [idx][IDTIER];
	stat._idxVolume = _cntr [idx][IDVOLM];
	stat._idxDevice = _cntr [idx][IDDEVC];

	stat._arrivalDate = _data [idx][ARRIVL];

	stat._failed = _failedReqs [idx];

	if (! get<1> (idx) ) {
		stat._idxBus = _cntr [idx][IDBUST];
		stat._transferTime = _data [idx][FM_TIR] + _data [idx][TO_TIR];
		stat._waitingTime = _rslt [idx][FM_TIR] - _rslt [idx][ARRIVL]
			- stat._transferTime;
		stat._serviceTime = 0;
	}
	else if (! get<2> (idx) ) {
		stat._idxBus = _cntr [idx][IDBUSV];
		stat._transferTime = _data [idx][FM_VOL] + _data [idx][TO_VOL];
		stat._waitingTime = _rslt [idx][FM_VOL] - _rslt [idx][TO_TIR]
			- stat._transferTime;
		stat._serviceTime = 0;
	}
	else {
		stat._idxBus = _cntr [idx][IDBUSD];
		stat._serviceTime = _data [idx][SERVCE];
		stat._transferTime = _data [idx][FM_DEV] + _data [idx][TO_DEV];
		stat._waitingTime = _rslt [idx][FM_DEV] - _rslt [idx][TO_VOL]
			- stat._serviceTime - stat._transferTime;
	}
	
	return stat;
}

void
SyncDefV2::process () {
	vector <double>			busClocks (_hardParam._numInterfaces, .0);
	vector <double>			devClocks (_hardParam._numDevices, .0);
	OGSS_Ulong				nbComputations = _data.size () * (TABTOT - 1);
	OGSS_Ulong				savedNumber;
	index_t					starter = _data.begin () ->first;
	double					minSearch;
	index_t					minCursor;
	OGSS_Ulong				lastMainIdx = get<0> (_data.rbegin () ->first);
	index_t					idxUnd = make_tuple (OGSS_ULONG_MAX, OGSS_ULONG_MAX,
		OGSS_ULONG_MAX);
	index_t					restart = idxUnd;
	index_t					checkpoint = idxUnd;

	DLOG(INFO) << "[SC] Nb computations requested: " << nbComputations;

	_resume = Resume(_nbRequests, _globalDU, _tiers.size(), _volumes.size(), _devices.size());

	while (nbComputations) {
		minSearch = numeric_limits <double> ::max ();
		minCursor = idxUnd;

		table_t::iterator e;

		if (restart != idxUnd) {
			e = _rslt.find (restart);
			if (checkpoint != idxUnd)
				minSearch = _rslt [checkpoint][_cntr [checkpoint][IDSTEP]];
				minCursor = checkpoint;
		}
		else
			e = _rslt.find (starter);

		while (e != _rslt.end () ) {
			if (e->second [ARRIVL] > minSearch) break;

			if (_cntr [e->first][IDSTEP] == FM_TIR
				|| _cntr [e->first][IDSTEP] == UND) {
				++e;
				continue;
			}

			if (e->second [_cntr [e->first][IDSTEP]] < minSearch) {
				if (_idpt [e->first][IDNEXT] != idxUnd) {
					e = _rslt.find (_idpt [e->first][IDNEXT]);
					continue;
				}

				if (minCursor != idxUnd) {
					_idpt [minCursor][IDNEXT] = e->first;
					_idpt [e->first][IDPREV] = minCursor;
				}

				minSearch = e->second [_cntr [e->first][IDSTEP]];
				minCursor = e->first;
			}

			++e;
		}

		LOG_IF(FATAL, minSearch == OGSS_REAL_MAX)
			<< "Issue with the SyncDefV2 model (too much computations requested"
			<< ": " << nbComputations << ")";

		auto & tmp = _rslt [minCursor];
		auto & idx = _cntr [minCursor];

		++ idx [IDSTEP];

		tmp [idx [IDSTEP]]
			= max (busClocks [idx [IDCLK (idx [IDSTEP])]],
			tmp [idx [IDSTEP] - 1]) + _data [minCursor][idx [IDSTEP]];

		busClocks [idx [IDCLK (idx [IDSTEP])]] = tmp [idx [IDSTEP]];

		switch (idx [IDSTEP]) {
			case TO_TIR: _processToTier (minCursor, tmp, idx, nbComputations); break;
			case TO_VOL: _processToVolume (minCursor, tmp, idx, nbComputations); break;
			case TO_DEV: _processToDevice (minCursor, tmp, idx, nbComputations, devClocks); break;
			case FM_DEV: _processFromDevice (minCursor, tmp, idx, nbComputations); break;
			case FM_VOL: _processFromVolume (minCursor, tmp, idx, nbComputations); break;
			case FM_TIR: _processFromTier (minCursor, tmp, idx, nbComputations, starter, lastMainIdx); break;
		}

		if (_idpt [minCursor][IDPREV] != idxUnd) {
			auto elt2 = _idpt [minCursor][IDPREV];
			if (_cntr [minCursor][IDSTEP] == FM_TIR || _cntr [minCursor][IDSTEP] == UND) {
				_idpt [elt2][IDNEXT] = idxUnd;
				restart = elt2;
				checkpoint = idxUnd;
			} else if (_rslt [elt2][_cntr [elt2][IDSTEP]] 
				<= _rslt [minCursor][_cntr[minCursor][IDSTEP]]) {
				_idpt [elt2][IDNEXT] = idxUnd;
				_idpt [minCursor][IDPREV] = idxUnd;
				restart = minCursor;
				checkpoint = elt2;
			} else {
				restart = minCursor;
				checkpoint = idxUnd;
			}
		} else
			restart = idxUnd;

		-- nbComputations;
	}
}

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
SyncDefV2::_processToTier (
	index_t					& minCursor,
	value_t					& tmp,
	valct_t					& idx,
	OGSS_Ulong				& nbComputations) {
	auto savedNumber = nbComputations;

	for (auto elt = ++_rslt.find (minCursor); elt != _rslt.end (); ++elt) {
		if (get<0> (elt->first) != get<0> (minCursor) ) break;
		if (get<2> (elt->first) != 0) continue;
		elt->second [TO_TIR] = tmp [TO_TIR];
		_cntr [elt->first][IDSTEP] = TO_TIR;
		_data [elt->first][ARRIVL] = tmp [TO_TIR];
		-- nbComputations;
		
	}
	if (savedNumber != nbComputations)
		idx [IDSTEP] = UND;
}

void
SyncDefV2::_processToVolume (
	index_t					& minCursor,
	value_t					& tmp,
	valct_t					& idx,
	OGSS_Ulong				& nbComputations) {
	auto savedNumber = nbComputations;
	for (auto elt = ++_rslt.find (minCursor); elt != _rslt.end (); ++elt) {
		if (get<0> (elt->first) != get<0> (minCursor)
			|| get<1> (elt->first) != get<1> (minCursor) ) break;
		elt->second [TO_VOL] = tmp [TO_VOL];
		nbComputations -= 2;
		if (_cntr [elt->first][NBPRIO] || ! idx [NBPRIO]){
			_cntr [elt->first][IDSTEP] = TO_VOL;
			_data [elt->first][ARRIVL] = tmp [TO_VOL];
		}
	}
	if (savedNumber != nbComputations)
		idx [IDSTEP] = UND;
}

void
SyncDefV2::_processToDevice (
	index_t					& minCursor,
	value_t					& tmp,
	valct_t					& idx,
	OGSS_Ulong				& nbComputations,
	vector <double>			& devClocks) {
	++ idx [IDSTEP];

	if (_failedReqs [minCursor]) {
		auto elt = _rslt.find (make_tuple (get <0> (minCursor), 0, 0) );
		if (elt->first != minCursor) {
			_failedReqs [elt->first] = true;
		}
	} else {
		tmp [SERVCE]
			= max (devClocks [idx [IDDEVC]], tmp [TO_DEV])
			+ _data [minCursor][SERVCE];
		devClocks [idx [IDDEVC]] = tmp [SERVCE];
	}

	-- nbComputations;
}

void
SyncDefV2::_processFromDevice (
	index_t					& minCursor,
	value_t					& tmp,
	valct_t					& idx,
	OGSS_Ulong				& nbComputations) {
	auto elt = _rslt.find (make_tuple (get<0> (minCursor), get<1> (minCursor), 0) );

	if (elt->first != minCursor) {
		elt->second [FM_DEV] = max (elt->second [FM_DEV], tmp [FM_DEV]);
		idx [IDSTEP] = FM_TIR;
		nbComputations -= 2;

		if (! -- _cntr [elt->first][NBCHLD]) {
			_cntr [elt->first][IDSTEP] = FM_DEV;
			nbComputations -= 3;
		}

		if (idx [NBPRIO]) {
			-- _cntr [elt->first][NBPRIO];
			for (auto flt = ++_rslt.find (elt->first); flt != _rslt.end (); ++flt) {
				if (get<0> (flt->first) != get<0> (minCursor) ) break;
				if (get<1> (flt->first) != get<1> (minCursor) ) continue;

				if (!_cntr [flt->first][NBPRIO]) {
					if (!_cntr [elt->first][NBPRIO]){
						_cntr [flt->first][IDSTEP] = TO_VOL;
						_data [flt->first][ARRIVL] = tmp[FM_DEV];
					}

					_rslt [flt->first][TO_VOL]
						= max (_rslt [flt->first][TO_VOL], tmp [FM_DEV]);
				}
			}
		}

		_cntr [elt->first][IDTIER] = _cntr [minCursor][IDTIER];
		_cntr [elt->first][IDVOLM] = _cntr [minCursor][IDVOLM];
		_cntr [elt->first][IDDEVC] = _cntr [minCursor][IDDEVC];

		sendStat (prepareStat (minCursor) );
		_resume.updateStats(prepareStat(minCursor));
	}
}

void
SyncDefV2::_processFromVolume (
	index_t					& minCursor,
	value_t					& tmp,
	valct_t					& idx,
	OGSS_Ulong				& nbComputations) {
	auto elt = _rslt.find (make_tuple (get<0> (minCursor), 0, 0) );

	if (elt->first != minCursor) {
		elt->second [FM_VOL] = max (elt->second [FM_VOL], tmp [FM_VOL]);
		idx [IDSTEP] = FM_TIR;
		-- nbComputations;

		if (! --_cntr [elt->first][NBCHLD]) {
			_cntr [elt->first][IDSTEP] = FM_VOL;
			nbComputations -= 5;
		}

		_cntr [elt->first][IDTIER] = _cntr [minCursor][IDTIER];
		_cntr [elt->first][IDVOLM] = _cntr [minCursor][IDVOLM];
		_cntr [elt->first][IDDEVC] = _cntr [minCursor][IDDEVC];

		sendStat (prepareStat (minCursor) );
		_resume.updateStats(prepareStat(minCursor));
	}
}

void
SyncDefV2::_processFromTier (
	index_t					& minCursor,
	value_t					& tmp,
	valct_t					& idx,
	OGSS_Ulong				& nbComputations,
	index_t					& starter,
	OGSS_Ulong				& lastMainIdx) {
	OGSS_Ulong				printStep {max (_nbRequests/100, (OGSS_Ulong) 1) };
	if (get<1> (minCursor) == 0 && get<2> (minCursor) == 0
		&& get<0> (minCursor) == get<0> (starter) ) {
		int y = get<0> (minCursor);
		do {
			if (++y > lastMainIdx) break;
		} while (_cntr [make_tuple (y, 0, 0)][IDSTEP] == FM_TIR);

		starter = make_tuple (y, 0, 0);
	}

	if(_mainRequestsDone == 0){
		cout << "\r\t" << 0 << "% Requests Done [0/" << _nbRequests << "]" << flush;
	}

	++ _mainRequestsDone;
	
	if(!(_mainRequestsDone % printStep)){
		if(printStep == 1){
			cout << "\r\t" << (OGSS_Real)_mainRequestsDone/_nbRequests*100 << "% Requests Done [" << _mainRequestsDone << "/" << _nbRequests << "]" << flush;
		}else
			cout << "\r\t" << (OGSS_Real)_mainRequestsDone/printStep << "% Requests Done [" << _mainRequestsDone << "/" << _nbRequests << "]" << flush;
		if(_mainRequestsDone == _nbRequests){
			cout << endl;
		}
	}

	sendStat (prepareStat (minCursor) );
	_resume.updateStats(prepareStat(minCursor));
}

void
SyncDefV2::createOutputFile (
	const OGSS_String		outputFile) {
	if (! outputFile.compare ("") ){
		cout << "\r\t\tOutput file not requested by the configuration" << endl;
		return;
	} 

	ofstream				output (outputFile);
	OGSS_Ulong				printStep {max (_rslt.size() / 100, (OGSS_Ulong) 1) };
	OGSS_Ulong				printStep2 {max (_data.size() / 100, (OGSS_Ulong) 1) };

	output << left << setfill (' ') << setw (6) << "#M.a.m" << " | "
		 << left << setfill (' ') << setw (8) << "Arrival" << " | "
		 << left << setfill (' ') << setw (17) << "ToTier" << " | "
		 << left << setfill (' ') << setw (17) << "ToVolume" << " | "
		 << left << setfill (' ') << setw (17) << "ToDevice" << " | "
		 << left << setfill (' ') << setw (17) << "Service" << " | "
		 << left << setfill (' ') << setw (17) << "FromDevice" << " | "
		 << left << setfill (' ') << setw (17) << "FromVolume" << " | "
		 << left << setfill (' ') << setw (17) << "FromTier"
		 << endl;

	OGSS_Ulong nbResults = 0;
	for (auto & e: _rslt) {
		output << right << setfill (' ') << setw (2) << get<0> (e.first) << "." << get<1> (e.first) << "."
			 << get<2> (e.first) << " | ";

		if (e.second [ARRIVL] < 0 || _data [e.first][ARRIVL] < 0)
			output << left << setfill (' ') << setw (8) << "-";
		else
			output << left << setfill (' ') << setw (8) << e.second [ARRIVL];
		for (auto j = 1; j < TABTOT; ++j) {
			if (e.second [j] < 0 || _data [e.first][j] < 0)
				output << " | " << setfill (' ') << setw (17) << "-";
			else
				output << " | " << setfill (' ') << setw (17) << e.second [j] - _data [e.first][j];
		}

		output << endl << setfill (' ') << setw (6) << " " << " | " << setfill (' ') << setw (8) << " ";

		for (auto j = 1; j < TABTOT; ++j) {
			if (e.second [j] < 0 || _data [e.first][j] < 0)
				output << " > " << setfill (' ') << setw (17) << "-";
			else
				output << " > " << setfill (' ') << setw (17) << e.second [j];
		}

		output << endl;

		nbResults++;

		if(!( nbResults % printStep)){
			if(printStep == 1){
				cout << "\r\t\t" << floor(((OGSS_Real)nbResults/_rslt.size()) + 0.5) * 100 << "%  results written" << flush;
			}else
			cout << "\r\t\t" << floor(((OGSS_Real)nbResults/printStep) + 0.5) << "%  results written" << flush;
		}
		if(nbResults == _rslt.size()){
			cout << endl;
		}
	}

	output << endl << "----------------------" << endl << endl;

	output << left << setfill (' ') << setw (6) << "#M.a.m" << " | "
		 << left << setfill (' ') << setw (8) << "Arrival" << " | "
		 << left << setfill (' ') << setw (17) << "ToTier" << " | "
		 << left << setfill (' ') << setw (17) << "ToVolume" << " | "
		 << left << setfill (' ') << setw (17) << "ToDevice" << " | "
		 << left << setfill (' ') << setw (17) << "Service" << " | "
		 << left << setfill (' ') << setw (17) << "FromDevice" << " | "
		 << left << setfill (' ') << setw (17) << "FromVolume" << " | "
		 << left << setfill (' ') << setw (17) << "FromTier"
		 << endl;

	OGSS_Ulong nbData = 0;
	for (auto & e: _data) {
		output << right << setfill (' ') << setw (2) << get<0> (e.first) << "." << get<1> (e.first) << "."
			 << get<2> (e.first) << " | ";

		if (e.second [ARRIVL] < 0 || e.second [ARRIVL] == numeric_limits<double> ::max () )
			output << left << setfill (' ') << setw (8) << "-";
		else
			output << left << setfill (' ') << setw (8) << e.second [ARRIVL];
		for (auto j = 1; j < TABTOT; ++j) {
			if (e.second [j] < 0)
				output << " | " << setfill (' ') << setw (17);
			else
				output << " | " << setfill (' ') << setw (17) << e.second [j];
		}

		output << endl;

		nbData++;

		if(!( nbData % printStep2)){
			cout << "\r\t\t" << floor(((OGSS_Real)nbData/printStep2) + 0.5) << "%  data written" << flush;

			if(printStep2 == 1){
				cout << "\r\t\t" << floor(((OGSS_Real)nbData/_rslt.size()) + 0.5) * 100 << "%  data written" << flush;
			}else
			cout << "\r\t\t" << floor(((OGSS_Real)nbData/printStep2) + 0.5) << "%  data written" << flush;
		}

		if(nbData == _data.size()){
				cout << endl;
		}
	}
}

void
SyncDefV2::createResumeFile (
	const OGSS_String		resumeFile) {
	
	_resume.save(resumeFile);
}
