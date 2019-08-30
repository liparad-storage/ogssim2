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

//! \file	genreresponsetime.hpp
//! \brief	Definition of the graph generator for request response time.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <cmath>

#include "graph/genresponsetime.hpp"

#include "parser/xmlparser.hpp"

#include "serializer/ltxserializer.hpp"
#include "serializer/mplibserializer.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

GenResponseTime::GenResponseTime (
	const Graph				& graph,  OGSS_DataUnit globalDU):
	GraphGeneration (graph, globalDU) {
	int i = 0;

	for (auto & elt: graph._target) {
		_legendLabels.push_back (elt.first);
		_values.push_back (vector <pair <OGSS_Real, OGSS_Real>> () );
		_latencies.push_back (vector <pair <OGSS_Real, OGSS_Real>> () );
		for (auto & flt: elt.second) {
			_targets [flt] .insert (i);
		}

		++i;
	}

	for (auto & elt: graph._rules) {
		_rules.push_back (elt);
	}

	_numPoints = (!graph._arg1) ? 10 : graph._arg1;
	_doubleAxis = (graph._arg2 == 2);
	_maxDate = .0; _maxDate2 = .0; _systStart = OGSS_REAL_MAX;
	_blockSize = 0;

	if (_doubleAxis) {
		i = 0;
		
		for (auto & elt: graph._target) {
			_values2.push_back (vector <pair <OGSS_Real, OGSS_Real>> () );
			_latencies2.push_back (vector <pair <OGSS_Real, OGSS_Real>> () );
		}

		++i;
	}
}

GenResponseTime::~GenResponseTime () {  }

void
GenResponseTime::updateData (
	const RequestStat		& stat) {

	if (_targets.find (stat._idxDevice) == _targets.end () ) return;

	if (_doubleAxis && stat._system) {
		_maxDate2 = max (_maxDate2,
			stat._arrivalDate + stat._serviceTime + stat._waitingTime
			+ stat._transferTime);
		_systStart = min (_systStart, stat._arrivalDate);
	} else if (! stat._majrIdx && (stat._type == RQT_READ || stat._type == RQT_WRITE) ) {
		_maxDate = max (_maxDate,
			stat._arrivalDate + stat._serviceTime + stat._waitingTime
			+ stat._transferTime);
	}

	if (_doubleAxis && stat._system) {
		_blockSize = stat._size;
		for (auto & elt: _targets [stat._idxDevice])
			_latencies2 [elt] .push_back (make_pair (stat._arrivalDate + stat._waitingTime,
				stat._serviceTime + stat._transferTime) );
	} else if (! stat._majrIdx && ! stat._failed
	 && (stat._type == RQT_READ || stat._type == RQT_WRITE) ) {
	 	for (auto & elt: _targets [stat._idxDevice])
			_latencies [elt] .push_back (make_pair (stat._arrivalDate,
				stat._serviceTime + stat._transferTime + stat._waitingTime) );
	}
}

void
GenResponseTime::makeGraph () {	
	vector <OGSS_Ulong> sizes;
	vector <OGSS_Ulong> sizes2;

	for (auto i = 0; i < _numPoints; ++i) sizes.push_back (0);
		
	for (auto & elt: _values)
		for (auto i = 0; i < _numPoints; ++i)
			elt.push_back (make_pair (_maxDate * i / _numPoints, .0) );

	for (auto k = 0; k < _values.size (); ++k) {
		auto s = _latencies [k] .size ();
		for (auto i = 0; i < _numPoints; ++i)
			sizes [i] = 0;

		for (OGSS_Long i = 0; i < s; ++i) {
			_values [k][OGSS_Long (floor (_latencies [k][i] .first * _numPoints / _maxDate) ) ] .second += _latencies [k][i] .second;
			++ sizes [OGSS_Long (floor (_latencies [k][i] .first * _numPoints / _maxDate) ) ];
		}

		for (auto i = 0; i < _numPoints; ++i)
			if (sizes [i])
				_values [k][i] .second /= sizes [i];
	}

	for (auto & elt: _values)
		for (auto i = 0; i < _numPoints; ++i){
			elt[i].first *= _globalDU._time;
			elt[i].second *= _globalDU._time;
		}

	if (_doubleAxis) {
		for (auto i = 0; i < _numPoints; ++i) sizes2.push_back (0);
		
		for (auto & elt: _values2)
			for (auto i = 0; i < _numPoints; ++i)
				elt.push_back (make_pair (_systStart + (_maxDate2 - _systStart) * i / _numPoints, .0) );

		for (auto k = 0; k < _values2.size (); ++k) {
			auto s = _latencies2 [k] .size ();
			for (auto i = 0; i < _numPoints; ++i)
				sizes2 [i] = 0;

			for (OGSS_Long i = 0; i < s; ++i) {
				_values2 [k][OGSS_Long (floor (_latencies2 [k][i] .first * _numPoints / (_maxDate2 - _systStart) ) ) ] .second += _latencies2 [k][i] .second;
				++ sizes2 [OGSS_Long (floor (_latencies2 [k][i] .first * _numPoints / (_maxDate2 - _systStart) ) ) ];
			}

			for (auto i = 0; i < _numPoints; ++i)
				if (sizes2 [i]) {
					_values2 [k][i] .second = _blockSize * sizes2 [i] / ( (_maxDate2 - _systStart) * (i+1) / _numPoints - (_maxDate2 - _systStart) * i / _numPoints);
					_values2 [k][i] .second /= MEGA;
					_values2 [k][i] .second *= 4*KILO;
				}
		}
	}



	OGSS_Real maximum_time_1 = 0.;
	OGSS_Real maximum_time_2 = 0.;

	for(auto & elt: _values){
		for(auto i=0; i < _numPoints; i++){
			maximum_time_1 = max(elt[i].first, maximum_time_1);
			maximum_time_2 = max(elt[i].second, maximum_time_2);
		}
	}


	string timelabel_1 = "";
	string timelabel_2 = "";

	for(auto &elt: _values){
		if(maximum_time_1 < MICRO){
			timelabel_1 = "simulation time (ns)";
			applyNewTimeUnit_1(elt, NANO);
		}else if(maximum_time_1 < MILLI){
			timelabel_1 = "simulation time (us)";
			applyNewTimeUnit_1(elt, MICRO);
		}else if(maximum_time_1 < 1){
			timelabel_1 = "simulation time (ms)";
			applyNewTimeUnit_1(elt, MILLI);
		}else if(maximum_time_1 < MINUTE){
			timelabel_1 = "simulation time (s)";
		}else{
			timelabel_1 = "simulation time (minutes)";
			applyNewTimeUnit_1(elt, MINUTE);
		}

		if(maximum_time_2 < MICRO){
			timelabel_2 = "mean user response time (ns)";
			applyNewTimeUnit_2(elt, NANO);
		}else if(maximum_time_2 < MILLI){
			timelabel_2 = "mean user response time (us)";
			applyNewTimeUnit_2(elt, MICRO);
		}else if(maximum_time_2 < 1){
			timelabel_2 = "mean user response time (ms)";
			applyNewTimeUnit_2(elt, MILLI);
		}else if(maximum_time_2 < MINUTE){
			timelabel_2 = "mean user response time (s)";
		}else{
			timelabel_2 = "mean user response time (minutes)";
			applyNewTimeUnit_2(elt, MINUTE);
		}
	}


	if (_doubleAxis) {
		switch (_format) {
			case GFM_LATEX:
				LOG(WARNING) << "The response time graph can not be made in latex";
			break;
#if ! USE_PYTHON_BINDING
			default: break;
#else
			case GFM_PDF:
				MPLibSerializer::createLineDoubleAxisGraphWithRules (_filename,
					_values, _values2,
					_legendLabels, _legendLabels,
					_rules, _maxDate2,
					timelabel_1,
					timelabel_2, "mean syst. bandwidth (MB/s)",
					"pdf");
			break;
			case GFM_PNG:
			default:
				MPLibSerializer::createLineDoubleAxisGraphWithRules (_filename,
					_values, _values2,
					_legendLabels, _legendLabels,
					_rules, _maxDate2,
					timelabel_1,
					timelabel_2, "mean syst. bandwith (MB/s)");
			break;
#endif
		}

	} else if (! _rules.empty ()) {
		switch (_format) {
			case GFM_LATEX:
				LOG(WARNING) << "The response time graph can not be made in latex";
			break;
#if ! USE_PYTHON_BINDING
			default: break;
#else
			case GFM_PDF:
				MPLibSerializer::createLineGraphWithRules (_filename,
					_values,
					_legendLabels,
					_rules,
					timelabel_1,
					timelabel_2,
					"pdf");
			break;
			case GFM_PNG:
			default:
				MPLibSerializer::createLineGraphWithRules (_filename,
					_values,
					_legendLabels,
					_rules,
					timelabel_1,
					timelabel_2);
			break;
#endif
		}
	} else {
		switch (_format) {
			case GFM_LATEX:
				LTXSerializer::createLineGraph (_filename,
					_values,
					_legendLabels,
					timelabel_1,
					timelabel_2);
			break;
#if ! USE_PYTHON_BINDING
			default: break;
#else
			case GFM_PDF:
				MPLibSerializer::createLineGraph (_filename,
					_values,
					_legendLabels,
					timelabel_1,
					timelabel_2,
					"pdf");
			break;
			case GFM_PNG:
			default:
				MPLibSerializer::createLineGraph (_filename,
					_values,
					_legendLabels,
					timelabel_1,
					timelabel_2);
			break;
#endif
		}
	}
}
