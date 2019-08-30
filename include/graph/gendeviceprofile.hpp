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

//! \file	gendeviceprofile.hpp
//! \brief	Definition of the graph generator for device profile.

#ifndef _OGSS_GENDEVICEPROFILE_HPP_
#define _OGSS_GENDEVICEPROFILE_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <map>

#include "graph/graphgeneration.hpp"

//! \brief	Generator of a bar chart to visualize the behavior of targeted
//!			devices.
class GenDeviceProfile: public GraphGeneration {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	graph				Graph data structure.
//! \param	globalDU			Global data unit.
	GenDeviceProfile	(
		const Graph				& graph,
		OGSS_DataUnit			globalDU);

//! \brief	Destructor.
	~GenDeviceProfile ();

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
	std::set <OGSS_Long>		_targets;			//!< Targeted disks.
	std::vector <std::vector <std::pair <OGSS_Real, OGSS_Real>>>
								_values;			//!< Graph values.
	std::map <OGSS_Long, OGSS_Long>	_redirects;
	OGSS_Real					_maxDate;			//!< End of simulation.
};

#endif
