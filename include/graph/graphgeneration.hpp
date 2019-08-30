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

//! \file	graphgeneration.hpp
//! \brief	Definition of the graph generator.

#ifndef _OGSS_GRAPHGENERATION_HPP_
#define _OGSS_GRAPHGENERATION_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/graph.hpp"
#include "structure/requeststat.hpp"

//! \brief	Interface for the graph generator. The implementation consists in
//!			two functions: (1) the update of the data structure following a
//!			request stat reception and (2) the way to create the graph.
class GraphGeneration {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Destructor.
	virtual ~GraphGeneration () {  };

//! \brief	Update the generator data following the reception of request stat.
//! \param	request				Request stats.
	virtual void updateData (
		const RequestStat		& stat) = 0;

//! \brief	Generate the graph.
	virtual void makeGraph () = 0;

protected:

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//!	\param 	graph				Graph data structure.
//! \param	globalDU			Global data unit.
	GraphGeneration (
		const Graph				& graph,
		OGSS_DataUnit			globalDU) {
		_filename = graph._filename;
		_format = graph._format;
		_globalDU = globalDU;
	}

//! \brief	Apply the reading data unit on memory values.
//! \param	values				Graph values.
//! \param	unit				Memory data unit.
	void applyNewMemoryUnit (
		std::vector<std::pair <double,double>> &values,
		OGSS_Ulong unit){
		for(auto &elt: values){
			elt.second /= unit;
		}
	}

//! \brief	Apply the reading data unit on time values.
//! \param	values				Graph values.
//! \param	unit				Time data unit.
	void applyNewTimeUnit_1(
		std::vector<std::pair <double,double>> &values,
		OGSS_Real unit){
		for(auto &elt: values){
			elt.first /= unit;
		}
	}

//! \brief	Apply the reading data unit on time values (2nd axis).
//! \param	values				Graph values.
//! \param	unit				Time data unit.
	void applyNewTimeUnit_2(
		std::vector<std::pair <double,double>> &values,
		OGSS_Real unit){
		for(auto &elt: values){
			elt.second /= unit;
		}
	}

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	OGSS_String					_filename;			//!< Output filename.
	OGSS_GraphFormatType		_format;			//!< File format.
	OGSS_DataUnit				_globalDU;			//!< Global data unit.
};

#endif
