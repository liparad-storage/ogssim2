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

//! \file	communicationinterface.hpp
//! \brief	Communication interface used by the modules to transfer data.

#ifndef _OGSS_COMMUNICATIONINTERFACE_HPP_
#define _OGSS_COMMUNICATIONINTERFACE_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/types.hpp"

//! \brief	Communication interface for OGSSim modules. The interface is
//! 		composed of three functions. The request one is used to ask a
//!			communication port to the manager (itself or another). Send and
//!			receive ones are used for data transfer.
class CommunicationInterface {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Destructor.
	virtual ~CommunicationInterface () = default;

//! \brief	Request a communication port to the manager. If need of a port for
//! 		itself, send its own identifier.
//! \param	to					Interlocutor identifier.
//! \return						TRUE if success, FALSE else.
	virtual OGSS_Bool request (
		const OGSS_Interlocutor	to) = 0;

//! \brief	Request to enter in a partial barrier. One thread needs to release it.
	virtual void requestBarrier () = 0;

//! \brief	Request to enter in a full barrier.
	virtual void requestFullBarrier () = 0;

//! \brief	Request to release a partial barrier when a given number of threads
//!			have joined it.
//! \param	Number of threads waiting in the barrier.
	virtual void releaseBarrier (
		const OGSS_Ushort		numThreads) = 0;

//! \brief	Send data.
//! \param	to					Interlocutor.
//! \param	arg					Data.
//! \param	size				Data size.
//! \param	multi				TRUE if multi-part message, FALSE else.
	virtual void send (
		const OGSS_Interlocutor	to,
		const void				* arg,
		const size_t			size,
		const OGSS_Bool			multi = false) = 0;

//! \brief	Receive data.
//! \param	arg					Data.
	virtual void receive (
		void					* & arg) = 0;
};

#endif
