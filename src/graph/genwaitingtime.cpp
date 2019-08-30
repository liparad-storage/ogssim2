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

//! \file	genwaitingtime.cpp
//! \brief	Definition of the graph generator for waiting time.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "graph/genwaitingtime.hpp"

#include "parser/xmlparser.hpp"

#include "serializer/ltxserializer.hpp"
#include "serializer/mplibserializer.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

GenWaitTime::GenWaitTime (
	const Graph				& graph,  OGSS_DataUnit globalDU):
	GraphGeneration (graph, globalDU) {

	for (auto & elt: graph._target [0] .second)
		_targets.insert (elt);

	_legendLabels.push_back ("work time");
	_legendLabels.push_back ("wait time");

	_numPoints = (!graph._arg1) ? 10 : graph._arg1;
}

GenWaitTime::~GenWaitTime () {  }

void
GenWaitTime::updateData (
	const RequestStat		& stat) {
	if (! stat._minrIdx) return;
	if (_targets.find (stat._idxDevice) == _targets.end () ) return;

	_waitTime.push_back (stat._waitingTime);
	_workTime.push_back (stat._serviceTime);
	_arrivalDate.push_back (stat._arrivalDate);
}

void
GenWaitTime::makeGraph () {
	_values.push_back (vector <pair <OGSS_Real, OGSS_Real>> ());
	_values.push_back (vector <pair <OGSS_Real, OGSS_Real>> ());

//	for (auto i = 0; i < _waitTime.size (); ++i)
//		cout << i << " -> " << _arrivalDate [i] << " / "
//			<< _waitTime [i] << " / " << _workTime [i] << endl;

	for (auto i = 0; i < _numPoints; ++i) {
		_values [0] .push_back (make_pair (-1., .0) );
		_values [1] .push_back (make_pair (-1., .0) );
	}

	auto s = _workTime.size ();

	for (OGSS_Long i = 0; i < s; ++i) {
		_values [0][i / (s / _numPoints)] .second += _workTime [i];
		_values [1][i / (s / _numPoints)] .second += _waitTime [i];

		if (_values [0][i / (s / _numPoints)] .first < 0) {
			_values [0][i / (s / _numPoints)] .first = _arrivalDate [i];
			_values [1][i / (s / _numPoints)] .first = _arrivalDate [i];
		}
	}

	for (auto & elt: _values [0])
		elt.second /= (s / _numPoints);
	for (auto & elt: _values [1])
		elt.second /= (s / _numPoints);

	for (auto & elt: _values)
		for (auto i = 0; i < _numPoints; ++i){
			elt[i].first *= _globalDU._time;
			elt[i].second *= _globalDU._time;
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
			timelabel_2 = "time (ns)";
			applyNewTimeUnit_2(elt, NANO);
		}else if(maximum_time_2 < MILLI){
			timelabel_2 = "time (us)";
			applyNewTimeUnit_2(elt, MICRO);
		}else if(maximum_time_2 < 1){
			timelabel_2 = "time (ms)";
			applyNewTimeUnit_2(elt, MILLI);
		}else if(maximum_time_2 < MINUTE){
			timelabel_2 = "time (s)";
		}else{
			timelabel_2 = "time (minutes)";
			applyNewTimeUnit_2(elt, MINUTE);
		}
	}

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
