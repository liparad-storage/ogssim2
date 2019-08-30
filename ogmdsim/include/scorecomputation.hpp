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

//! \file			scorecomputation.hpp
//! \brief			Definition of the score class. This class is used to order the metadata nodes by computing
//!					a proximity score to the host nodes and facilitates the right metadata node
//!					when processing a request.

#ifndef _OGMDS_HPP_SCORECPT_
#define _OGMDS_HPP_SCORECPT_

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include <utility>

//! \brief			Score computation class. This class is used calling the computeScore() function, to quantify the
//!					distance between a host node and a metadata node.
class Score {
public:

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Constructor.
//! \param			lower				Lower bounds (in latency and number of links).
//! \param			upper				Upper bounds (in latency and number of links).
//! \param			rateLatency			Weight of the latency in the score computation.
	Score (
		const std::pair <double, int>	lower = std::make_pair (.0, 0),
		const std::pair <double, int>	upper = std::make_pair (100., 100),
		const double					rateLatency = .5);

//! \brief			Destructor.
	~Score ();

//! \brief			Score computation function.
//! \param			element				Consists in a latency and a number of links between a host and a metadata node.
//! \return								Computed score.
	double
	computeScore (
		const std::pair <double, int>	element);

private:

/*--------------------------------------------------------------------------------------------------------------------*/
/* ATTRIBUTES --------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

	std::pair <double, int>				_lowerBound;							//!< Lower bound (score = 0).
	std::pair <double, int>				_upperBound;							//!< Upper bound (score = 1)
	double								_rateLatency;							//!< Weight of the latency in score
																				//!< computation.
};

#endif
