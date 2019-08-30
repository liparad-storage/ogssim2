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

//! \file	communicationinterfacempi.hpp
//! \brief	MPI communication interface.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#ifdef OGSSMPI

#include <sstream>

#include "communication/communicationinterfacempi.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

#if USE_MPI_BOOST
#include <boost/serialization/utility.hpp>

namespace mpi = boost::mpi;
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

CI_MPI::CI_MPI (
	const OGSS_String		configurationFile,
	const OGSS_Interlocutor	myself) {
	
#if USE_MPI_BOOST
	mpi::communicator					world;
	_withoutManager = world.split (0);
#else
	MPI_Comm_split (MPI_COMM_WORLD, 0, 0, &_withoutManager);
#endif

	_myself = myself;

	request (_myself);
}

CI_MPI::~CI_MPI () {
	_mapping.clear ();
#if ! USE_MPI_BOOST
	MPI_Comm_free (&_withoutManager);
#endif
}

OGSS_Bool
CI_MPI::request (
	const OGSS_Interlocutor	to) {
	ostringstream			oss ("");
	OGSS_Ulong				rank;
#if USE_MPI_BOOST
	mpi::communicator		world;
#else
	MPI_Status				status;
	MPI_Datatype			interlocutorType {createInterlocutorDatatype () };
#endif

	if (_mapping.find (to) != _mapping.end () )
		return false;

#if USE_MPI_BOOST
	world.send (0, 0, to);
	world.recv (0, 0, rank);
#else
	MPI_Send (&to, 1, interlocutorType, 0, 0, MPI_COMM_WORLD);
	MPI_Recv (&rank, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
#endif

	if (to.first == MTP_TOTAL) return true;

	_mapping.insert (make_pair (to, rank) );

	return true;
}

void
CI_MPI::requestBarrier () {
	OGSS_Ulong				ack;
#if USE_MPI_BOOST
	mpi::communicator		world;
#else
	MPI_Status				status;
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
CI_MPI::requestFullBarrier () {
	OGSS_Interlocutor		to;
	to.first = MTP_TOTAL;	to.second = OGSS_USHORT_MAX;
	OGSS_Ulong				ack;
#if USE_MPI_BOOST
	mpi::communicator		world;
#else
	MPI_Datatype			interlocutorType {createInterlocutorDatatype () };
#endif

#if USE_MPI_BOOST
//	world.send (0, 0, to);
//	world.recv (0, 0, ack);
//	world.barrier ();
	_withoutManager.barrier ();
#else
//	MPI_Send (&to, 1, interlocutorType, 0, 0, MPI_COMM_WORLD);
//	MPI_Recv (&ack, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//	MPI_Barrier (MPI_COMM_WORLD);
	MPI_Barrier (_withoutManager);
#endif
}

void
CI_MPI::releaseBarrier (
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

void
CI_MPI::send (
	const OGSS_Interlocutor	to,
	const void				* arg,
	const size_t			size,
	const OGSS_Bool			multi) {
	auto					p = _mapping.find (to);
	const char				* argc = static_cast <const char *> (arg);
#if USE_MPI_BOOST
	mpi::communicator		world;
#else
	MPI_Datatype			interlocutorType {createInterlocutorDatatype () };
#endif

	if (p == _mapping.end () )
		request (to);

	if (multi)
#if USE_MPI_BOOST
		world.send (_mapping [to], 1, argc, size);
#else
		MPI_Send (argc, size, MPI_CHAR, _mapping [to], 1, MPI_COMM_WORLD);
#endif
	else
#if USE_MPI_BOOST
		world.send (_mapping [to], 0, argc, size);
#else
		MPI_Send (argc, size, MPI_CHAR, _mapping [to], 0, MPI_COMM_WORLD);
#endif
}

void
CI_MPI::receive (
	void					* & arg) {
	static int				prevRank = -1;
	int						size;
	char					* argc;
#if USE_MPI_BOOST
	mpi::communicator		world;
	mpi::status				stat;
#else
	MPI_Status				status;
	MPI_Datatype			interlocutorType {createInterlocutorDatatype () };
#endif

#if USE_MPI_BOOST
	if (prevRank == -1)		stat = world.probe (mpi::any_source, mpi::any_tag);
	else					stat = world.probe (prevRank, mpi::any_tag);

	size = stat.count <char> () .value_or (1);
#else
	if (prevRank == -1)		MPI_Probe (MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	else					MPI_Probe (prevRank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	MPI_Get_count (&status, MPI_CHAR, &size);
#endif

	argc = (char *) malloc (size);

#if USE_MPI_BOOST
	stat = world.recv (stat.source (), mpi::any_tag, argc, size);

	if (stat.tag () == 1)		prevRank = stat.source ();
	else						prevRank = -1;

	if (_myself.first == MTP_WORKLOAD) LOG(INFO) << "Msg source: " << stat.source ();
#else
	MPI_Recv (argc, size, MPI_CHAR, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	if (status.MPI_TAG == 1)	prevRank = status.MPI_SOURCE;
	else						prevRank = -1;
#endif

	arg = argc;
}

#endif
