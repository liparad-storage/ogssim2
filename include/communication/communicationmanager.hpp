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

//! \file	communicationmanager.hpp
//! \brief	Interface for the communication manager. The manager is the main
//! 		module of the communication model. Its goal is to maintain the list
//! 		of existing modules, and share their communication ports if
//! 		requested.

#ifndef _OGSS_COMMUNICATIONMANAGER_HPP_
#define _OGSS_COMMUNICATIONMANAGER_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/types.hpp"

//! \brief	The manager of OGSSim communications. When a module needs to
//! 		communicate with others, it needs to ask the communication port
//! 		of the desired interlocutor to the communication manager. Then
//! 		the manager retrieves the information and send it to the requester.
class CommunicationManager {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Destructor.
	virtual ~CommunicationManager () = default;

//! \brief	Main function, which puts the manager in a waiting state, listening
//! 		to its communication socket for a provide request.	
	void listen ();

protected:

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Search for the requested communication port, and send it.
//! \return 					FALSE if the manager needs to stop, TRUE else.
	virtual OGSS_Bool provide () = 0;

//! \brief	Release a partial barrier where a given number of threads are waiting.
//! \param	numThreads			Number of threads which are waiting.
	virtual void releaseBarrier (
		OGSS_Ushort				numThreads) = 0;

//! \brief	Enter into a full barrier.
	virtual void fullBarrier () = 0;
};

#endif
