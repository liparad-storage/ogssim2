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

//! \file	genreqcompletion.cpp
//! \brief	Definition of the graph generator for request completion.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "graph/genreqcompletion.hpp"

#include "serializer/ltxserializer.hpp"
#include "serializer/mplibserializer.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

GenReqCompletion::GenReqCompletion (
	const Graph				& graph,  OGSS_DataUnit globalDU):
	GraphGeneration (graph, globalDU) {
	_legendLabels.push_back ("completed");
	_legendLabels.push_back ("failed");

	_values.push_back (0);
	_values.push_back (0);

	for (auto & elt: graph._target [0] .second)
		_targets.insert (elt);
}

GenReqCompletion::~GenReqCompletion () {  }

void
GenReqCompletion::updateData (
	const RequestStat		& stat) {
	if (stat._majrIdx) return;
	if (_targets.find (stat._idxDevice) == _targets.end () ) return;

	if (stat._failed)
		++ _values [1];
	else
		++ _values [0];
}

void
GenReqCompletion::makeGraph () {
	auto total = _values [0] + _values [1];

	if (!total)
		_values [0] = 1;
	else {
		_values [0] /= total; _values [1] /= total;
	}

	switch (_format) {
		case GFM_LATEX:
			LTXSerializer::createPieChart (_filename,
				_values,
				_legendLabels);
		break;
#if ! USE_PYTHON_BINDING
		default: break;
#else
		case GFM_PDF:
			MPLibSerializer::createPieChart (_filename,
				_values,
				_legendLabels,
				"pdf");
		break;
		case GFM_PNG:
		default:
			MPLibSerializer::createPieChart (_filename,
				_values,
				_legendLabels);
		break;
#endif
	}
}
