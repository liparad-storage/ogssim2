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

//! \file	gendevicethroughput.hpp
//! \brief	Definition of the graph generator for device throughput.

#ifndef _OGSS_GENDEVICETHROUGHPUT_HPP_
#define _OGSS_GENDEVICETHROUGHPUT_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "graph/graphgeneration.hpp"

//! \brief	Generator of a line graph to visualize the throughput of targeted
//!			devices.
class GenDeviceThroughput: public GraphGeneration {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	graph				Graph data structure.
//! \param	globalDU			Global data unit.
	GenDeviceThroughput (
		const Graph				& graph,
		OGSS_DataUnit			globalDU);

//! \brief	Destructor.
	~GenDeviceThroughput ();

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
								_stats;				//!< Received stats.

	OGSS_Real					_maxDate = 0;		//!< End of simulation.
	OGSS_Ulong					_numPoints;			//!< Number of points.
	OGSS_Ulong					_nbRequests = 0;	//!< Number of requests.
};

#endif
