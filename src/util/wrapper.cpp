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

//! \file	wrapper.cpp
//! \brief	Contains all the functions needed at the initialization by OGSSim.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <errno.h>
#include <string>
#include <thread>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "util/launcher.hpp"
#include "util/wrapper.hpp"

#include "driver/devicedriver.hpp"
#include "driver/volumedriver.hpp"

#include "module/evaluation.hpp"
#include "module/eventextractor.hpp"
#include "module/execution.hpp"
#include "module/hardwareextractor.hpp"
#include "module/preprocessing.hpp"
#include "module/synchronization.hpp"
#include "module/workloadextractor.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

#include "parser/xmlparser.hpp"

#ifdef OGSSMPI
#if USE_MPI_BOOST
#include <boost/mpi.hpp>
namespace mpi = boost::mpi;
#else
#include <mpi.h>
#endif
#endif

using namespace std;

/*----------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Extract the number of volumes from the hardware configuration file
//! 		given in the OGSSim configuration file.
//! \param	configurationFile	Path to the configuration file.
//! \return						Number of volumes.
OGSS_Short
extractNumberOfVolumes (
	const OGSS_String		configurationFile) {
	return XMLParser::getNumberOfVolumes (
		XMLParser::getFilePath (configurationFile, FTP_HARDWARE) );
}

/*----------------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
prepareMPILauncher (
	const OGSS_String		configurationFile) {
	OGSS_Short				numProcesses = numMonoProcessModules
		+ numMultiProcessModules * extractNumberOfVolumes (configurationFile);

	if (! fork () ) {
		DLOG(INFO) << "mpirun" << " " << "-n" << " " << to_string (numProcesses) .c_str ()
			<< " " << "./OGSSim" << " " << configurationFile.c_str () << " "
			<< MPI_EXEC.c_str ();

		execlp ("mpirun", "mpirun", "-n", to_string (numProcesses) .c_str (),
			"./OGSSim", configurationFile.c_str (), MPI_EXEC.c_str (), (char *) NULL);
	}

	wait (NULL);
}

void
OGMDSimLauncher (
	const OGSS_String		pathToExe,
	const OGSS_String		configurationFile) {

	OGSS_String				execString {pathToExe + "/OGMDSim"};

	if (! fork () ) {
		LOG_IF (FATAL, chdir (pathToExe.c_str () ) ) << "OGMDSim directory was not found";

		execlp ("./OGMDSim", "./OGMDSim", configurationFile.c_str (), (char *) NULL);
		LOG (FATAL) << "OGMDSim execution ERR: " << strerror (errno);
	}

	wait (NULL);
}

void
MPILauncher (
	const OGSS_String		configurationFile) {
#ifdef OGSSMPI
#if USE_MPI_BOOST
	mpi::communicator		world;
	world.barrier ();
		switch (world.rank () ) {
#else
	int rank;
	MPI_Barrier (MPI_COMM_WORLD);
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);
		switch (rank) {
#endif
		case 0: launchCommunicationManager (configurationFile); break;
		case 1: launchModule <WorkloadExtractor> (configurationFile); break;
		case 2: launchModule <HardwareExtractor> (configurationFile); break;
		case 3: launchModule <EventExtractor> (configurationFile); break;
		case 4: launchModule <Preprocessing> (configurationFile); break;
		case 5: launchModule <Execution> (configurationFile); break;
		case 6: launchModule <Synchronization> (configurationFile); break;
		case 7: launchModule <Evaluation> (configurationFile); break;
		default:
#if USE_MPI_BOOST
			auto i = world.rank () - numMonoProcessModules;
#else
			auto i = rank - numMonoProcessModules;
#endif
			auto n = extractNumberOfVolumes (configurationFile);
			if (i < n)
				launchModule <VolumeDriver> (configurationFile, i % n);
			else
				launchModule <DeviceDriver> (configurationFile, i % n);
			break;
		}
#endif
}

void
threadLauncher (
	const OGSS_String		configurationFile) {
	vector <unique_ptr <thread> >	threads;
	auto					n = extractNumberOfVolumes (configurationFile);
	OGSS_Short				numProcesses = numMonoProcessModules
		+ numMultiProcessModules * n;

	for (auto i = 1; i < numProcesses; ++i) {
		switch (i) {
		case 1: threads.push_back (make_unique <thread> (
				[=]{ launchModule <WorkloadExtractor> (configurationFile); } ) ); break;
		case 2: threads.push_back (make_unique <thread> (
				[=]{ launchModule <HardwareExtractor> (configurationFile); } ) ); break;
		case 3: threads.push_back (make_unique <thread> (
				[=]{ launchModule <EventExtractor> (configurationFile); } ) ); break;
		case 4: threads.push_back (make_unique <thread> (
				[=]{ launchModule <Preprocessing> (configurationFile); } ) ); break;
		case 5: threads.push_back (make_unique <thread> (
				[=]{ launchModule <Execution> (configurationFile); } ) ); break;
		case 6: threads.push_back (make_unique <thread> (
				[=]{ launchModule <Synchronization> (configurationFile); } ) ); break;
		case 7: threads.push_back (make_unique <thread> (
				[=]{ launchModule <Evaluation> (configurationFile); } ) ); break;
		default:
			if (i - numMonoProcessModules < n)
				threads.push_back (make_unique <thread> (
					[=]{ launchModule <VolumeDriver> (configurationFile,
					(i - numMonoProcessModules) % n); } ) );
			else
				threads.push_back (make_unique <thread> (
					[=]{ launchModule <DeviceDriver> (configurationFile,
					(i - numMonoProcessModules) % n); } ) );
			break;
		}
	}

	launchCommunicationManager (configurationFile);

	for (auto & elt: threads) elt->join ();
}
