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

//! \file	communicationinterfaceizmq.hpp
//! \brief	MPI-ZMQ communication interface.

#ifndef _OGSS_CMIIZMQ_HPP_
#define _OGSS_CMIIZMQ_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#ifdef OGSSMPI

#include <map>

#if USE_STATIC_LIBRARIES
#include "zmq.hpp"
#else
#include <zmq.hpp>
#endif

#include "communication/communicationinterface.hpp"

//! \brief	MPI-ZMQ communication interface. The sockets are stored in a mapping
//!			array.
class CI_IZMQ:
public CommunicationInterface {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	configurationFile	Path to the configuration file.
//! \param	myself				Identifier of the interface owner.
	CI_IZMQ (
		const OGSS_String		configurationFile,
		const OGSS_Interlocutor	myself);

//! \brief	Destructor.
	~CI_IZMQ ();

//! \brief	Request a communication port to the manager. If need of a port for
//! 		itself, send its own identifier.
//! \param	to					Interlocutor identifier.
//! \return						TRUE if success, FALSE else.
	OGSS_Bool request (
		const OGSS_Interlocutor	to);

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

//! \brief	Request to enter in a partial barrier. One thread needs to release it.
	void requestBarrier ();

//! \brief	Request to enter in a full barrier.
	void requestFullBarrier ();

//! \brief	Request to release a partial barrier when a given number of threads
//!			have joined it.
//! \param	Number of threads waiting in the barrier.
	void releaseBarrier (
		const OGSS_Ushort		numThreads);

protected:

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	OGSS_Interlocutor			_myself;			//!< Owner identifier.
	zmq::context_t				* _zmqContext;		//!< ZMQ context.
	std::map <OGSS_Interlocutor, zmq::socket_t *>
								_mapping;			//!< Mapping between IDs and
													//!< ZMQ sockets.
};

/*----------------------------------------------------------------------------*/
/* INLINE FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
CI_IZMQ::send (
	const OGSS_Interlocutor to,
	const void				* arg,
	const size_t			size,
	const OGSS_Bool			multi) {
	auto					p = _mapping.find (to);
	zmq::message_t			msg (size);
	
	if (p == _mapping.end () )
		request (to);

	memcpy (static_cast <void *> (msg.data () ), arg, size);

	if (multi)
		_mapping [to] ->send (msg, ZMQ_SNDMORE);
	else
		_mapping [to] ->send (msg);
}

void
CI_IZMQ::receive (
	void					* & arg) {
	zmq::message_t			msg;

	_mapping [_myself] ->recv (&msg);
	arg = malloc (msg.size () );
	memcpy (arg, msg.data (), msg.size () );
}

#endif
#endif
