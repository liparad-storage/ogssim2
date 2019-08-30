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

//! \file	communicationmanagerizmq.hpp
//! \brief	MPI-ZMQ communication manager.

#ifndef _OGSS_CMMIZMQ_HPP_
#define _OGSS_CMMIZMQ_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#ifdef OGSSMPI

#include <map>

#include "communication/communicationmanager.hpp"

//! \brief	MPI-ZMQ communication manager which stores the communication ports
//!			in a mapping array.
class CM_IZMQ:
public CommunicationManager {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	configurationFile	Path to the configuration file.
	CM_IZMQ (
		const OGSS_String		configurationFile = "");

//! \brief	Destructor.
	~CM_IZMQ ();

protected:

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Search for the requested communication port, and send it.
//! \return 					FALSE if the manager needs to stop, TRUE else.
	OGSS_Bool provide ();

//! \brief	Release a partial barrier where a given number of threads are waiting.
//! \param	numThreads			Number of threads which are waiting.
	void releaseBarrier (
		OGSS_Ushort				numThreads);

//! \brief	Enter into a full barrier.
	void fullBarrier ();

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	std::map <std::pair <OGSS_ModuleType, OGSS_Ushort>, OGSS_Ulong>
								_mapping;			//!< Mapping between IDs and communication ports.
	OGSS_Ulong					_fullBarrierCapacity;	//!< Capacity of the full barrier.
	OGSS_Ulong					_fullBarrierCounter;	//!< Counter for the full barrier.
};

#endif
#endif
