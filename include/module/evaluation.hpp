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

//! \file	evaluation.hpp
//! \brief	Definition of the evaluation module class.

#ifndef _OGSS_EVALUATION_HPP_
#define _OGSS_EVALUATION_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "graph/graphgeneration.hpp"
#include "module/module.hpp"
#include "structure/graph.hpp"
#include "structure/hardware.hpp"
#include "structure/requeststat.hpp"
#include "structure/types.hpp"

//! \brief	The evaluation module generates the visualization graphs requested
//!			by the user in OGSSim configuration file.
class Evaluation: public Module {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	configurationFile	Path to OGSSim configuration file.
	Evaluation (
		const OGSS_String		& configurationFile);

//! \brief	Destructor.
	~Evaluation ();

//!	\brief	Reception of data during the initialization process. The data
//!			contains the hardware parameters.
	inline void receiveData ();

//! \brief	Process during the extraction step.
	void processExtraction ();

//! \brief	Process during the decomposition step.
	void processDecomposition ();

//! \brief	Process during the synchronization step.
	void processSynchronization ();

private:

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Request stat reception.
//! \return						Request stat.
	RequestStat receiveRequestStat ();

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	std::vector <Graph>			_graphs;			//!< Requested graphs.
	std::vector <std::unique_ptr <GraphGeneration>>
								_ggen;				//!< Graph generators.
	HardwareParameters			_hardParam;			//!< Hardware parameters.
	OGSS_DataUnit				_globalDU;			//!< Global data unit.
};

/*----------------------------------------------------------------------------*/
/* INLINE FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
Evaluation::receiveData () {
	void						* arg;
	OGSS_Bool					ack;

	_ci->receive (arg);
	_hardParam = * static_cast <HardwareParameters *> (arg); free (arg);

	ack = true;
	_ci->send (std::make_pair (MTP_HARDWARE, 0), &ack, sizeof (ack) );
}

#endif
