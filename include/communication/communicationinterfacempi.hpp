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

//! \file	communicationinterfacempi.hppz
//! \brief	MPI communication interface.

#ifndef _OGSS_CMMPI_HPP_
#define _OGSS_CMMPI_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#ifdef OGSSMPI

#include <map>

#include <iostream>

#include "communication/communicationinterface.hpp"

#if USE_MPI_BOOST
#include <boost/mpi.hpp>
#else
#include <mpi.h>
#endif

//! \brief	MPI communication interface. The communication ranks are stored in
//! 		a map where one module is bound to one port.
class CI_MPI:
public CommunicationInterface {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC MEMBER FUNCTIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor. Also requests its own communication port.
//! \param	configurationFile	Configuration file.
//! \param	myself				Own identifier.
	CI_MPI (
		const OGSS_String		configurationFile,
		const OGSS_Interlocutor	myself);

//! \brief	Destructor.
	~CI_MPI ();

//! \brief	Request a communication port to the manager. If need of a port for
//! 		itself, send its own identifier.
//! \param	to					Interlocutor identifier.
//! \return						TRUE if success, FALSE else.
	OGSS_Bool request (
		const OGSS_Interlocutor	to);

//! \brief	Request to enter in a partial barrier. One thread needs to release it.
	void requestBarrier ();

//! \brief	Request to enter in a full barrier.
	void requestFullBarrier ();

//! \brief	Request to release a partial barrier when a given number of threads
//!			have joined it.
//! \param	Number of threads waiting in the barrier.
	void releaseBarrier (
		const OGSS_Ushort		numThreads);

//! \brief	Send data.
//! \param	to					Interlocutor.
//! \param	arg					Data.
//! \param	size				Data size.
//! \param	multi				TRUE if multi-part message, FALSE else.
	inline void send (
		const OGSS_Interlocutor	to,
		const void				* arg,
		const size_t			size,
		const OGSS_Bool			multi);

//! \brief	Receive data.
//! \param	arg					Data.
	inline void receive (
		void					* & arg);

protected:

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
	OGSS_Interlocutor			_myself;			//!< Owner identifier.
	std::map <OGSS_Interlocutor, OGSS_Ulong>
								_mapping;			//!< ZMQ map.
#if USE_MPI_BOOST
	mpi::communicator			_withoutManager;	//!< Communicator without the manager.
#else
	MPI_Comm					_withoutManager;	//!< Communicator without the manager.
#endif
};

#endif
#endif
