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

//! \file	gendevicethroughput.cpp
//! \brief	Definition of the graph generator for device throughput.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "graph/gendevicethroughput.hpp"

#include "serializer/ltxserializer.hpp"
#include "serializer/mplibserializer.hpp"

#include <cmath>

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

GenDeviceThroughput::GenDeviceThroughput (
	const Graph				& graph,  OGSS_DataUnit globalDU):
	GraphGeneration (graph, globalDU) {
	int i = 0;
	
	_numPoints = (!graph._arg1) ? 10 : graph._arg1;
	for (auto & elt: graph._target) {
		_legendLabels.push_back (elt.first);
		_values.push_back (vector <pair <OGSS_Real, OGSS_Real>> () );
		_stats.push_back (vector <pair <OGSS_Real, OGSS_Real>> () );
		for (auto & flt: elt.second) {
			_targets [flt] .insert (i);
		}

		++i;
	}
}

GenDeviceThroughput::~GenDeviceThroughput () {  }

void
GenDeviceThroughput::updateData (
	const RequestStat		& stat) {
	if(stat._minrIdx){
		_nbRequests++;
		for (auto & elt: _targets [stat._idxDevice]){
			_stats [elt] .push_back (
			make_pair (stat._arrivalDate + stat._serviceTime + stat._waitingTime + stat._transferTime, stat._size) );
		}
		_maxDate = max (_maxDate, stat._arrivalDate + stat._serviceTime + stat._waitingTime + stat._transferTime);
	}
}

void
GenDeviceThroughput::makeGraph () {

	for (auto & elt: _values)
		for (auto i = 0; i < _numPoints; ++i)
			elt.push_back (make_pair (_maxDate * i / _numPoints, .0) );

	for (auto k = 0; k < _values.size (); ++k) {
		auto s = _stats [k] .size ();

		for (auto i = 0; i < s; ++i) {
			_values [k][OGSS_Long (floor (_stats [k][i] .first * _numPoints / _maxDate) ) ] .second += _stats [k][i] .second;
		}
	}

	for (auto & elt: _values)
		for (auto i = 0; i < _numPoints; ++i){
			elt[i].first *= _globalDU._time;
			elt[i].second = elt[i].second / (( (_maxDate * (i + 1) / _numPoints) - (_maxDate * i / _numPoints) ) * _globalDU._time);
			elt[i].second *= _globalDU._memory;
		}

	OGSS_Real maximum_time = 0.;
	OGSS_Real maximum_memory = 0.;

	for(auto & elt: _values){
		for(auto i=0; i < _numPoints; i++){
			maximum_time = max(elt[i].first, maximum_time);
			maximum_memory = max(elt[i].second, maximum_memory);
		}
	}

	string bandwidthlabel = "";
	string timelabel = "";

	for(auto &elt: _values){
		if(maximum_memory < KILO){
			bandwidthlabel = "bandiwdth (B/s)";
		}else if(maximum_memory < MEGA){;
			applyNewMemoryUnit(elt, KILO);
			bandwidthlabel = "bandiwdth (KB/s)";
		}else if(maximum_memory < GIGA){
			applyNewMemoryUnit(elt, MEGA);
			bandwidthlabel = "bandiwdth (MB/s)";
		}else if(maximum_memory < TERA){
			applyNewMemoryUnit(elt, GIGA);
			bandwidthlabel = "bandiwdth (GB/s)";
		}else{
			bandwidthlabel = "bandiwdth (TB/s)";
			applyNewMemoryUnit(elt, TERA);
		}

		if(maximum_time < MICRO){
			timelabel = "simulation time (ns)";
			applyNewTimeUnit_1(elt, NANO);
		}else if(maximum_time < MILLI){
			timelabel = "simulation time (us)";
			applyNewTimeUnit_1(elt, MICRO);
		}else if(maximum_time < 1){
			timelabel = "simulation time (ms)";
			applyNewTimeUnit_1(elt, MILLI);
		}else if(maximum_time < MINUTE){
			timelabel = "simulation time (s)";
		}else{
			timelabel = "simulation time (minutes)";
			applyNewTimeUnit_1(elt, MINUTE);
		}
	}

	switch (_format) {
		case GFM_LATEX:
			LTXSerializer::createLineGraph (_filename,
				_values,
				_legendLabels,
				timelabel,
				bandwidthlabel);
		break;
#if ! USE_PYTHON_BINDING
		default: break;
#else
		case GFM_PDF:
			MPLibSerializer::createLineGraph (_filename,
				_values,
				_legendLabels,
				timelabel,
				bandwidthlabel,
				"pdf");
		break;
		case GFM_PNG:
		default:
			MPLibSerializer::createLineGraph (_filename,
				_values,
				_legendLabels,
				timelabel,
				bandwidthlabel);
		break;
#endif
	}
}
