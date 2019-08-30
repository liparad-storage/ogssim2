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

//! \file	communicationinterfacezmq.hpp
//! \brief	ZMQ communication interface.

#ifndef _OGSS_CMIZMQ_HPP_
#define _OGSS_CMIZMQ_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <map>

#if USE_STATIC_LIBRARIES
#include "zmq.hpp"
#else
#include <zmq.hpp>
#endif

#include <iostream>

#include "communication/communicationinterface.hpp"

//! \brief	ZMQ communication interface. The communication ports are stored in
//! 		a map where one module is bound to one port.
class CI_ZMQ:
public CommunicationInterface {
public:
	friend class UT_CM_ZMQ;

/*----------------------------------------------------------------------------*/
/* PUBLIC MEMBER FUNCTIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor. Also requests its own communication port.
//! \param	configurationFile	Configuration file.
//! \param	myself				Own identifier.
	CI_ZMQ (
		const OGSS_String		configurationFile,
		const OGSS_Interlocutor	myself);

//! \brief	Destructor.
	~CI_ZMQ ();

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
	zmq::context_t				* _zmqContext;		//!< ZMQ context.
	zmq::socket_t				* _zmqManager;		//!< ZMQ to manager.
	zmq::socket_t				* _zmqBarrier;		//!< ZMQ to manager barrier.
	std::map <OGSS_Interlocutor, zmq::socket_t*>
								_mapping;			//!< ZMQ map.
};

/*----------------------------------------------------------------------------*/
/* INLINE MEMBER FUNCTIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
CI_ZMQ::send (
	const OGSS_Interlocutor	to,
	const void				* arg,
	const size_t			size,
	const OGSS_Bool			multi) {
	auto					p = _mapping.find (to);
	zmq::message_t			msg (size);

	if (p == _mapping.end () )
		request (to);

//	std::cout << "Send (" << ModuleNameMap.at (to.first) << ", " << to.second
//		<< ")" << std::endl;

	memcpy (static_cast <void *> (msg.data () ), arg, size);

	if (multi)
		_mapping [to] ->send (msg, ZMQ_SNDMORE);
	else
		_mapping [to] ->send (msg);
}

void
CI_ZMQ::receive (
	void					* & arg) {
	zmq::message_t			msg;

//	std::cout << "Recv (" << ModuleNameMap.at (_myself.first) << ", "
//		<< _myself.second << ")" << std::endl;

	_mapping [_myself] ->recv (&msg);
	arg = malloc (msg.size () );
	memcpy (arg, msg.data (), msg.size () );
}

#endif
