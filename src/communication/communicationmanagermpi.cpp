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

//! \file	communicationmanagermpi.cpp
//! \brief	MPI communication manager.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#ifdef OGSSMPI

#include <algorithm>

#include "communication/communicationmanagermpi.hpp"

#include "parser/xmlparser.hpp"

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

CM_MPI::CM_MPI (
	const OGSS_String		configurationFile) {
#if USE_MPI_BOOST
	mpi::communicator		world;
	_withoutManager = world.split (0);

	_mapping.insert (make_pair (make_pair (MTP_COMMUNICATION, 0), world.rank () ) );
#else
	int rank;

	MPI_Comm_split (MPI_COMM_WORLD, MPI_UNDEFINED, 0, &_withoutManager);

	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
	_mapping.insert (make_pair (make_pair (MTP_COMMUNICATION, 0), rank) );
#endif

	_fullBarrierCapacity = numMonoProcessModules + numMultiProcessModules *
		XMLParser::getNumberOfVolumes (XMLParser::getFilePath (configurationFile, FTP_HARDWARE) ) - 1;
	_fullBarrierCounter = 0;
}
CM_MPI::~CM_MPI () {
#if ! USE_MPI_BOOST
	MPI_Comm_free (&_withoutManager);
#endif
}

OGSS_Bool
CM_MPI::provide () {
	OGSS_Interlocutor		interlocutor;
	OGSS_Ulong				port;
#if USE_MPI_BOOST
	mpi::communicator		world;
	mpi::status				stat;
#else
	OGSS_Ulong				ack;
	MPI_Status				status;
	MPI_Datatype			interlocutorType {createInterlocutorDatatype () };
#endif

#if USE_MPI_BOOST
	stat = world.recv (mpi::any_source, 0, interlocutor);
#else
	MPI_Recv (&interlocutor, 1, interlocutorType, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
#endif

	if (interlocutor.first == MTP_TOTAL && interlocutor.second == OGSS_USHORT_MAX) {
		++ _fullBarrierCounter;
#if USE_MPI_BOOST
		world.send (stat.source (), 0, 0);
#else
		MPI_Send (&ack, 1, MPI_UNSIGNED_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
#endif
		if (_fullBarrierCounter == _fullBarrierCapacity)
			fullBarrier ();

		return true;
	}

	if (interlocutor.first == MTP_TOTAL && interlocutor.second != 0) {
		releaseBarrier (interlocutor.second);
#if USE_MPI_BOOST
		world.send (stat.source (), 0, 0);
#else
		MPI_Send (&ack, 1, MPI_UNSIGNED_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
#endif

		return true;
	}

	if (interlocutor.first == MTP_TOTAL) {
#if USE_MPI_BOOST
		world.send (stat.source (), 0, 0);
#else
		MPI_Send (&ack, 1, MPI_UNSIGNED_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
#endif
		return false;
	}

	auto p = _mapping.find (interlocutor);

	if (p == _mapping.end () ) {
		OGSS_Bool			found {false};
		for (auto elt: _mapping) {
#if USE_MPI_BOOST
			if (elt.second == stat.source () )
#else
			if (elt.second == status.MPI_SOURCE)
#endif
				{ found = true; break; }
		}

		if (found) {
#if USE_MPI_BOOST
			_waitingQueue [stat.source ()] = interlocutor;
#else
			_waitingQueue [status.MPI_SOURCE] = interlocutor;
#endif
		} else {
#if USE_MPI_BOOST
			_mapping.insert (make_pair (interlocutor, stat.source () ) );
			world.send (stat.source (), 0, stat.source () );
#else
			_mapping.insert (make_pair (interlocutor, status.MPI_SOURCE) );
			MPI_Send (&status.MPI_SOURCE, 1, MPI_UNSIGNED_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
#endif

			auto elt = _waitingQueue.begin ();
			while (elt != _waitingQueue.end () ) {
				if (elt->second != interlocutor) { ++elt; continue; }
#if USE_MPI_BOOST
				world.send (elt->first, 0, stat.source () );
#else
				MPI_Send (&status.MPI_SOURCE, 1, MPI_UNSIGNED_LONG, elt->first, 0, MPI_COMM_WORLD);
#endif

				auto flt = elt;
				++ elt;
				_waitingQueue.erase (flt);
			}
		}
	} else {
#if USE_MPI_BOOST
		world.send (stat.source (), 0, p->second);
#else
		MPI_Send (&(p->second), 1, MPI_UNSIGNED_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
#endif
	}

	return true;
}

void
CM_MPI::releaseBarrier (
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
CM_MPI::fullBarrier () {
#if USE_MPI_BOOST
	mpi::communicator		world;
	world.barrier ();
#else
	MPI_Barrier (MPI_COMM_WORLD);
#endif

	_fullBarrierCounter = 0;
}

#endif
