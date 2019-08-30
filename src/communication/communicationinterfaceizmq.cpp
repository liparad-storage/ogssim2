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

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#ifdef OGSSMPI

#include <sstream>

#include "communication/communicationinterfaceizmq.hpp"

#if USE_MPI_BOOST
#include <boost/mpi.hpp>
#include <boost/serialization/utility.hpp>

namespace mpi = boost::mpi;
#else
#include <mpi.h>
#endif

using namespace std;
using namespace zmq;

/*----------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#if ! USE_MPI_BOOST
static MPI_Datatype createInterlocutorDatatype () {
	OGSS_Interlocutor		to;
	MPI_Datatype			interlocutorRawtype;
	MPI_Datatype			interlocutorDatatype;
	int						lengths [2] = {1, 1};
	MPI_Aint				addrs [2];
	MPI_Datatype			types [2] = {MPI_INT, MPI_UNSIGNED_SHORT};

	MPI_Get_address (&to, addrs);
	MPI_Get_address (&(to.second), addrs + 1);

	addrs [1] = MPI_Aint_diff (addrs [1], addrs [0]);
	addrs [0] = MPI_Aint_diff (addrs [0], addrs [0]);

	MPI_Type_create_struct (2, lengths, addrs, types, &interlocutorRawtype);
	MPI_Type_create_resized (interlocutorRawtype, 0, sizeof (to), &interlocutorDatatype);
	MPI_Type_commit (&interlocutorDatatype);

	return interlocutorDatatype;
}
#endif

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

CI_IZMQ::CI_IZMQ (
	const OGSS_String		configurationFile,
	const OGSS_Interlocutor	myself) {
	
	_myself = myself;

	_zmqContext = new context_t (1);

	request (_myself);
}

CI_IZMQ::~CI_IZMQ () {
	for (auto & elt: _mapping) {
		elt.second->close ();
		delete elt.second;
	}

	_mapping.clear ();

	delete _zmqContext;
}

OGSS_Bool
CI_IZMQ::request (
	const OGSS_Interlocutor	to) {
	ostringstream			oss ("");
	OGSS_Ulong				port;
	socket_t				* mailbox;
#if USE_MPI_BOOST
	mpi::communicator		world;
#else
	MPI_Datatype			interlocutorType {createInterlocutorDatatype () };
#endif

	if (_mapping.find (to) != _mapping.end () )
		return false;

#if USE_MPI_BOOST
	world.send (0, 0, to);
	world.recv (0, 0, port);
#else
	MPI_Send (&to, 1, interlocutorType, 0, 0, MPI_COMM_WORLD);
	MPI_Recv (&port, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
#endif

	if (to.first == MTP_TOTAL) return true;

	oss << "tcp://127.0.0.1:" << port;

	if (to == _myself) {
		mailbox = new socket_t (*_zmqContext, ZMQ_PULL);
		mailbox->bind (oss.str () .c_str () );
	} else {
		mailbox = new socket_t (*_zmqContext, ZMQ_PUSH);
		mailbox->connect (oss.str () .c_str () );
	}

	_mapping.insert (make_pair (to, mailbox) );

	return true;
}

void
CI_IZMQ::requestBarrier () {
	OGSS_Ulong				ack;
#if USE_MPI_BOOST
	mpi::communicator		world;
#else
	MPI_Datatype			interlocutorType {createInterlocutorDatatype () };
#endif

#if USE_MPI_BOOST
	world.send (0, 10, _myself);
	world.recv (0, 10, ack);
#else
	MPI_Send (&_myself, 1, interlocutorType, 0, 10, MPI_COMM_WORLD);
	MPI_Recv (&ack, 1, MPI_UNSIGNED_LONG, 0, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
#endif
}

void
CI_IZMQ::requestFullBarrier () {
	OGSS_Interlocutor		to;
	to.first = MTP_TOTAL;	to.second = OGSS_USHORT_MAX;
	OGSS_Ulong				ack;
#if USE_MPI_BOOST
	mpi::communicator		world;
#else
	MPI_Datatype			interlocutorType {createInterlocutorDatatype () };
#endif

#if USE_MPI_BOOST
	world.send (0, 0, to);
	world.recv (0, 0, ack);
	world.barrier ();
#else
	MPI_Send (&to, 1, interlocutorType, 0, 0, MPI_COMM_WORLD);
	MPI_Recv (&ack, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Barrier (MPI_COMM_WORLD);
#endif
}

void
CI_IZMQ::releaseBarrier (
	const OGSS_Ushort		numThreads) {
	OGSS_Ulong				ack;
	OGSS_Interlocutor		to;
	to.first = MTP_TOTAL;	to.second = numThreads;
#if USE_MPI_BOOST
	mpi::communicator		world;
#else
	MPI_Datatype			interlocutorType {createInterlocutorDatatype () };
#endif

#if USE_MPI_BOOST
	world.send (0, 0, to);
	world.recv (0, 0, ack);
#else
	MPI_Send (&to, 1, interlocutorType, 0, 0, MPI_COMM_WORLD);
	MPI_Recv (&ack, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
#endif
}

#endif
