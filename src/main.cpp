/*
 * Copyright UVSQ - CEA/DAM/DIF (2018)
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

//! \file	main.cpp
//! \brief	Main function of OGSSim. Two process modes are available:
//! 		- the first	one is the normal mode, where information from input
//!			files are extracted and the simulation is launched, resulting in the
//!			computation of performance metrics;
//! 		- the second one is the test mode, where a selection of scenario
//! 		from unitary test modules are executed, providing 

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <memory>
#include <thread>

#include "structure/types.hpp"

#ifdef OGSSMPI
#if USE_MPI_BOOST
#include <boost/mpi.hpp>
namespace mpi = boost::mpi;
#else
#include <mpi.h>
#endif
#endif

#if USE_STATIC_LIBRARIES
#include "gflags/gflags.h"
#else
#include <gflags/gflags.h>
#endif

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

#include "parser/xmlparser.hpp"

#if ! USE_TINYXML
#include <xercesc/util/PlatformUtils.hpp>
using namespace xercesc;
#endif

#include "util/launcher.hpp"
#include "util/wrapper.hpp"

using namespace std;
using namespace google;

/*----------------------------------------------------------------------------*/
/* MAIN FUNCTION -------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	OGSSim main function. Initializes library contexts (Google Logging,
//! 		Xerces) and launch the software modules.
//! \param	argc			Number of parameters.
//! \param	argv			Parameters.
//! \return					OGSSim error code.
int
main (
	int						argc,
	char **					argv) {

	if (argc != 2 && argc != 3) {
		cout << "OGSSim command usage: " << argv [0]
			<< " configurationFile" << endl;
		return -1;
	}

#ifdef OGSSMPI
	// MPI initialization
#if USE_MPI_BOOST
	mpi::environment		env;
#else
	MPI_Init (&argc, &argv);
#endif
#endif

	// Google glog initialization
	InitGoogleLogging (argv[0]);
	FLAGS_logtostderr = 0;
	FLAGS_minloglevel = 0;
	FLAGS_logbuflevel = -1;

	// Xercesc initialization
#if ! USE_TINYXML
	XMLPlatformUtils::Initialize ();
#endif

	google::SetLogDestination (google::INFO, XMLParser::getFilePath (
		argv [1], FTP_LOGGING, false) .c_str () );

	// Module launcher
#ifdef UTEST
	launchUnitaryTests (argv [1]);
#elif OGSSMPI
	if (! MPI_EXEC.compare (argv [argc - 1] ) )
		MPILauncher (argv [1]);
	else {
			DLOG(INFO) << "MPI version of OGSSim";
			prepareMPILauncher (argv [1]);
	}
#else
	DLOG(INFO) << "Thread version of OGSSim";
	threadLauncher (argv [1]);
#endif

	// Xercesc finalization
#if ! USE_TINYXML
	XMLPlatformUtils::Terminate ();
#endif

	// Google glog finalization
	ShutdownGoogleLogging ();
	ShutDownCommandLineFlags ();

	if (argc != 3)
		cout << "Simulation done" << endl;

	// MPI finalization
#ifdef OGSSMPI
#if ! USE_MPI_BOOST
	MPI_Finalize ();
#endif
#endif

	return 0;
}
