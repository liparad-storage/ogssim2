/*
 * Copyright UVSQ - CEA/DAM/DIF (2019)
 * Contributors:	Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *					Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General
 * Public License as published per the Free Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

//! \file			scorecomputation.cpp
//! \brief			Definition of the score class. This class is used to order the metadata nodes by computing
//!					a proximity score to the host nodes and facilitates the right metadata node
//!					when processing a request.

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include "scorecomputation.hpp"

using namespace std;

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

Score::Score (
	const pair <double, int>			lower,
	const pair <double, int>			upper,
	const double						rateLatency):
	_lowerBound (lower), _upperBound (upper), _rateLatency (rateLatency) {  }

Score::~Score () {  }

double
Score::computeScore (
	const pair <double, int>			element) {
	double								res {.0};
	double								latency {.0};
	double								jumps {.0};

	latency = (element.first - _lowerBound.first);
	jumps = (element.second - _lowerBound.second);

	latency /= (_upperBound.first - _lowerBound.first);
	jumps /= (_upperBound.second - _lowerBound.second);

	res = 1 - (_rateLatency * latency + (1 - _rateLatency) * jumps);

	return res;
}
