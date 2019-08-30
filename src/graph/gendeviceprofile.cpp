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

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \file	gendeviceprofile.cpp
//! \brief	Definition of the graph generator for device profile.

#include "graph/gendeviceprofile.hpp"

#include "serializer/ltxserializer.hpp"
#include "serializer/mplibserializer.hpp"

#include <algorithm>

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

GenDeviceProfile::GenDeviceProfile (
	const Graph				& graph,  OGSS_DataUnit globalDU):
	GraphGeneration (graph, globalDU) {
	int i = 0;

	_legendLabels.push_back ("idle");
	_legendLabels.push_back ("work");
	_legendLabels.push_back ("off");

	for (auto i = 0; i < 3; ++i)
		_values.push_back (vector <pair <OGSS_Real, OGSS_Real>> () );

	for (auto & elt: graph._target) {
		for (auto & flt: elt.second) {
			_values [0] .push_back (make_pair (flt, .0) );
			_values [1] .push_back (make_pair (flt, .0) );
			_values [2] .push_back (make_pair (flt, .0) );
			_redirects [flt] = _targets.size ();
			_targets.insert (flt);
		}
	}

	_maxDate = .0;
}

GenDeviceProfile::~GenDeviceProfile () {  }

void
GenDeviceProfile::updateData (
	const RequestStat		& stat) {

	if (stat._type == RQT_EVFLT && _targets.find (stat._idxDevice) != _targets.end () ) {
		for (auto & elt: _values [2])
			if (elt.first == stat._idxDevice)
				elt.second = stat._arrivalDate;
	}
	else if (stat._type == RQT_READ || stat._type == RQT_WRITE) {
		if (! stat._majrIdx && _targets.find (stat._idxDevice) != _targets.end () )
			_maxDate = max (_maxDate,
				stat._arrivalDate + stat._serviceTime + stat._waitingTime
				+ stat._transferTime);

		if (_targets.find (stat._idxDevice) != _targets.end () && ! stat._failed)
			_values [1][_redirects [stat._idxDevice] ] .second += stat._serviceTime;
	}
}

void GenDeviceProfile::makeGraph () {
	ostringstream oss {""};
	for (auto i = 0; i < _values [0] .size (); ++i)
		oss << "[" << i << "] " << _values [0][i] .second
			<< " - " << _values [1][i] .second
			<< " - " << _values [2][i] .second << endl;

	DLOG (INFO) << "[GG] Max date: " << _maxDate << endl << oss.str ();

	for (auto i = 0; i < _values [0] .size (); ++i) {
		if (_maxDate == .0) {
			_values [0][i] .second = 100;
		}else {

			if (_values [2][i] .second == 0)
				_values [2][i] .second = _maxDate;
			_values [1][i] .second /= _maxDate;
			_values [2][i] .second = 1 - (_values [2][i] .second / _maxDate);
			_values [0][i] .second =
				1 - _values [1][i] .second - _values [2][i] .second;

			_values [0][i] .second *= 100;
			_values [1][i] .second *= 100;
			_values [2][i] .second *= 100;
		}
	}

	switch (_format) {
		case GFM_LATEX:
			LTXSerializer::createStackedHistogram (_filename,
				_values,
				_legendLabels,
				"disk",
				"simulation time (\\\%)");
		break;
#if ! USE_PYTHON_BINDING
		default: break;
#else
		case GFM_PDF:
			MPLibSerializer::createStackedHistogram (_filename,
				_values,
				_legendLabels,
				"disk",
				"simulation time (%)",
				"pdf");
		break;
		case GFM_PNG:
		default:
			MPLibSerializer::createStackedHistogram (_filename,
				_values,
				_legendLabels,
				"disk",
				"simulation time (%)");
		break;
#endif
	}
}
