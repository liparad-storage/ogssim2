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

//! \file	communicationmanagerizmq.cpp
//! \brief	MPI-ZMQ communication manager.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#ifdef OGSSMPI

#include <algorithm>

#include "communication/communicationmanagerizmq.hpp"

#include "parser/xmlparser.hpp"

#if USE_MPI_BOOST
#include <boost/mpi.hpp>
#include <boost/serialization/utility.hpp>

namespace mpi = boost::mpi;
#else
#include <mpi.h>
#endif

using namespace std;

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

CM_IZMQ::CM_IZMQ (
	const OGSS_String		configurationFile) {
	OGSS_Ulong				port;

	port = XMLParser::getCommunicationPort (configurationFile);

	_mapping.insert (make_pair (make_pair (MTP_TOTAL, 0), port - 1) );
}
CM_IZMQ::~CM_IZMQ () {  }

OGSS_Bool
CM_IZMQ::provide () {
	OGSS_Interlocutor		interlocutor;
	OGSS_Ulong				port;
#if USE_MPI_BOOST
	mpi::communicator		world;
	mpi::status				stat;
#else
	MPI_Status				status;
	MPI_Datatype			interlocutorType {createInterlocutorDatatype () };
#endif

#if USE_MPI_BOOST
	stat = world.recv (mpi::any_source, 0, interlocutor);
#else
	MPI_Recv (&interlocutor, 1, interlocutorType, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
#endif

	if (interlocutor.first == MTP_TOTAL) {
#if USE_MPI_BOOST
		world.send (stat.source (), 0, 0);
#else
		MPI_Send (&port, 1, MPI_UNSIGNED_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
#endif

		return false;
	}

	auto p = _mapping.find (interlocutor);

	if (p == _mapping.end () ) {
		port = max_element (_mapping.begin (), _mapping.end (),
		[] (const pair <OGSS_Interlocutor, OGSS_Ulong> &p1,
			const pair <OGSS_Interlocutor, OGSS_Ulong> &p2) {
				return p1.second < p2.second;
			} ) ->second + 1;
		_mapping.insert (make_pair (interlocutor, port) );
	} else {
		port = p->second;
	}

#if USE_MPI_BOOST
	world.send (stat.source (), 0, port);
#else
	MPI_Send (&port, 1, MPI_UNSIGNED_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
#endif

	return true;
}

void
CM_IZMQ::releaseBarrier (
	OGSS_Ushort				numThreads) {
	OGSS_Interlocutor		interlocutor;
#if USE_MPI_BOOST
	mpi::communicator		world;
	mpi::status				stat;
#else
	MPI_Status				status;
	MPI_Datatype			interlocutorType {createInterlocutorDatatype () };
	OGSS_Ulong				ack;
#endif

	while (numThreads) {
#if USE_MPI_BOOST
		stat = world.recv (mpi::any_source, 10, interlocutor);
		world.send (stat.source (), 10, 0);
#else
		MPI_Recv (&interlocutor, 1, interlocutorType, MPI_ANY_SOURCE, 10, MPI_COMM_WORLD, &status);
		MPI_Send (&ack, 1, MPI_UNSIGNED_LONG, status.MPI_SOURCE, 10, MPI_COMM_WORLD);
#endif
		-- numThreads;
	}
}

void
CM_IZMQ::fullBarrier () {
#if USE_MPI_BOOST
	mpi::communicator		world;
	world.barrier ();
#else
	MPI_Barrier (MPI_COMM_WORLD);
#endif

	_fullBarrierCounter = 0;
}

#endif
