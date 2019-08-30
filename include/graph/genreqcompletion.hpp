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

//! \file	genreqcompletion.hpp
//! \brief	Definition of the graph generator for request completion.

#ifndef _OGSS_GENREQCOMPLETION_HPP_
#define _OGSS_GENREQCOMPLETION_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "graph/graphgeneration.hpp"

//! \brief	Generator of a pie chart to visualize the parts of completed and
//!			failed requests.
class GenReqCompletion: public GraphGeneration {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	graph				Graph data structure.
//! \param	globalDU			Global data unit.
	GenReqCompletion (
		const Graph				& graph,
		OGSS_DataUnit			globalDU);

//! \brief	Destructor.
	~GenReqCompletion ();

//! \brief	Update the generator data following the reception of request stat.
//! \param	request				Request stats.
	void updateData (
		const RequestStat		& request);

//! \brief	Generate the graph.
	void makeGraph ();

private:

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	std::vector <OGSS_String>	_legendLabels;		//!< Legend labels.
	std::vector <OGSS_Real>		_values;			//!< Graph values.
	std::set <OGSS_Ulong>		_targets;			//!< Targeted disks.
};

#endif
