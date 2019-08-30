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

//! \file	communicationmanagerzmq.hpp
//! \brief	ZMQ communication manager.

#ifndef _OGSS_CMMZMQ_HPP_
#define _OGSS_CMMZMQ_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <map>

#if USE_STATIC_LIBRARIES
#include "zmq.hpp"
#else
#include <zmq.hpp>
#endif

#include "communication/communicationmanager.hpp"

#include "util/unitarytest.hpp"

//! \brief	ZMQ communication manager. The communication ports are stored in
//!			a map where one module is bound to one port.
class CM_ZMQ:
public CommunicationManager {
public:
	friend class UT_CM_ZMQ;

/*----------------------------------------------------------------------------*/
/* PUBLIC MEMBER FUNCTIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Default constructor. Get the manager communication port from the
//! 		configuration file.
//! \param	configurationFile	Configuration file.
	CM_ZMQ (
		const OGSS_String		configurationFile = "");

//! \brief	Destructor.
	~CM_ZMQ ();

protected:

/*----------------------------------------------------------------------------*/
/* PRIVATE MEMBER FUNCTIONS --------------------------------------------------*/
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
								_mapping;			//!< Communication port map.
	zmq::context_t				* _zmqContext;		//!< ZMQ context.
	zmq::socket_t				* _zmqMaster;		//!< ZMQ used for the port requests.
	zmq::socket_t				* _zmqBarrier;		//!< ZMQ used for the barrier requests.
	OGSS_Ulong					_fullBarrierCapacity;	//!< Capacity of the full barrier.
	OGSS_Ulong					_fullBarrierCounter;	//!< Counter for the full barrier.
};

/*----------------------------------------------------------------------------*/
/* UNITARY TEST --------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Unitary tests for ZMQ communication manager.
class UT_CM_ZMQ:
public UnitaryTest <UT_CM_ZMQ> {
public:
//! \brief	Default constructor.
//! \param	configurationFile	Configuration file.
	UT_CM_ZMQ (
		const OGSS_String		& configurationFile);

//! \brief	Destructor.
	~UT_CM_ZMQ ();

protected:
//! \brief	1 request is sent to the manager.
//! \return						TRUE on success.
	OGSS_Bool check1Request ();
//! \brief	10 requests are sent to the manager.
//! \return						TRUE on success.
	OGSS_Bool check10Request ();
//! \brief	100 requests are sent to the manager.
//! \return						TRUE on success.
	OGSS_Bool check100Request ();
//! \brief	1000 requests are sent to the manager.
//! \return						TRUE on succes.
	OGSS_Bool check1000Request ();
//! \brief	Data is going through a communication socket.
//! \return						TRUE on success.
	OGSS_Bool checkCommunication ();
};

#endif
