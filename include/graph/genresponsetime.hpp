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

#ifndef _OGSS_GENRESPONSETIME_HPP_
#define _OGSS_GENRESPONSETIME_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "graph/graphgeneration.hpp"

//! \brief	Generator of a line graph to visualize the request response time.
class GenResponseTime: public GraphGeneration {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	graph				Graph data structure.
//! \param	globalDU			Global data unit.
	GenResponseTime (
		const Graph				& graph,
		OGSS_DataUnit			globalDU);

//! \brief	Destructor.	
	~GenResponseTime ();

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
	std::map <OGSS_Long, std::set <OGSS_Long>>
								_targets;			//!< Targeted disks.
	std::vector <std::vector <std::pair <OGSS_Real, OGSS_Real>>>
								_values;			//!< Graph values.
	std::vector <std::vector <std::pair <OGSS_Real, OGSS_Real>>>
								_values2;			//!< Graph values (2nd axis).
	OGSS_Long					_numPoints;			//!< Number of points.
	std::vector <std::vector <std::pair <OGSS_Real, OGSS_Real>>>
								_latencies;			//!< Latencies.
	std::vector <std::vector <std::pair <OGSS_Real, OGSS_Real>>>
								_latencies2;		//!< Latencies (2nd axis).
	std::vector <std::pair <OGSS_String, OGSS_Real>>
								_rules;				//!< Rules.

	OGSS_Ulong					_blockSize;			//!< Data block size.

	OGSS_Bool					_doubleAxis;		//!< TRUE if double axis.
	OGSS_Real					_maxDate;			//!< End of simulation (1st axis).
	OGSS_Real					_maxDate2;			//!< End of simulation (2nd axis).
	OGSS_Real					_systStart;			//!< Start of system requests.
};

#endif
