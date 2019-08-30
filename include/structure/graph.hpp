/*
 * Copyright UVSQ - CEA/DAM/DIF (2017-2018)
 * Contributors:  Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *                Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published per the Free Software Foundation; either
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

//! \file	graph.hpp
//! \brief	Definition of visualization graph structure, which is used by
//!			the evaluation module to store the extracted information from
//!			the configuration file.

#ifndef _OGSS_GRAPH_HPP_
#define _OGSS_GRAPH_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <set>
#include <vector>

#include "structure/types.hpp"

/*----------------------------------------------------------------------------*/
/* STRUCTURE -----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Structure that contains the visualization graph parameters
struct Graph {
	OGSS_GraphType				_type;				//!< Graph type.
	OGSS_String					_filename;			//!< Output filename.
	OGSS_GraphFormatType		_format;			//!< Output format.
	std::vector <std::pair <OGSS_String, std::set <OGSS_Long>>>
								_target;			//!< Targeted devices.
	std::vector <std::pair <OGSS_String, OGSS_Real>>
								_rules;				//!< Rule coordinates.
	OGSS_Long					_arg1;				//!< First opt. argument.
	OGSS_Long					_arg2;				//!< Second opt. argument.
};

#endif
