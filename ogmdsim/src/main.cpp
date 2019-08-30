/*
 * Copyright UVSQ - CEA/DAM/DIF (2019)
 * Contributors:	Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *					Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General
 * Public License as published per the Free Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

//! \file			main.cpp
//! \brief			Definition of OGMDSim main function.

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <iostream>

#include "computer.hpp"
#include "locator.hpp"
#include "mdrequest.hpp"
#include "outtrace.hpp"
#include "rawextract.hpp"
#include "xmlextract.hpp"

using namespace std;
using namespace google;

/*--------------------------------------------------------------------------------------------------------------------*/
/* MAIN FUNCTION -----------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Main function, which consists of the simulation processing and the glog library initialisation and
//!					finalisation. The simulation processing is done by determining the metadata information and
//!					computing the metadata transfer.
//! \param			argc				Number of arguments.
//! \param			argv				List of arguments.
//! \return								Exit code.
int
main (
	int									argc,
	char								** argv) {

//* Initialisation
	InitGoogleLogging (argv [0]);
	FLAGS_logtostderr = 0;
	FLAGS_minloglevel = 0;
	FLAGS_logbuflevel =-1;

	google::SetLogDestination (google::INFO, "log/tmp.");

//* Processing
	LOG (INFO) << "Launching MDSim..";

	LOG_IF (FATAL, argc != 2) << "Need a configuration file to launch";

	OGXML								parser {argv [1]};
	System								s (parser);
	Locator								l (s, parser);
	Computer							c (s);
	vector <MDRequest>					v;
	string								pt;
	string								po;

	LOG_IF (FATAL, ! parser.getXMLItem <string> (pt, OGFT_CFGFILE, "path/trace") )
		<< "Tag: 'path/trace' must be provided";
	RawParser::extractRequests (v, pt);

	for (auto & e: v) {
		l.locate (e);
		c.compute (e);
	}

	auto								r {parser.getXMLItem <string> (po, OGFT_CFGFILE, "path/output") };
	LOG_IF (WARNING, ! r) << "Tag: 'path/output' is not provided, the output file will not be generated";
	if (r)								OutputTrace::exportDataRequests (v, po);

//* Finalisation
	LOG (INFO) << "Finalising MDSim..";
	ShutdownGoogleLogging ();
	ShutDownCommandLineFlags ();

	return 0;
}
