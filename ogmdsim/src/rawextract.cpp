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

//! \file			rawextract.cpp
//! \brief			Definition of the raw parser namespace. This namespace is used to read the trace
//!					and the matrix files.

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include "rawextract.hpp"

#include <glog/logging.h>
#include <iostream>
#include <fstream>
#include <sstream>

/*--------------------------------------------------------------------------------------------------------------------*/
/* CONSTANTS ---------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#define									BUFFERSIZE			16384

using namespace std;

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

void
RawParser::extractMatrix (
	vector <vector <double>>			& matrix,
	const string						path) {
	char								buffer [BUFFERSIZE];
	ifstream							filestream (path.c_str () );
	istringstream						bufferstream;
	int									i {0}, j;
	LOG_IF (FATAL, ! filestream.good () )
		<< "The matrix file '" << path << "' does not exist";

	for (filestream.getline (buffer, BUFFERSIZE); filestream.good ();
		filestream.getline (buffer, BUFFERSIZE), ++i) {
		bufferstream.str (buffer);
		for (j = 0; j < matrix [i] .size (); ++j)
			bufferstream >> matrix [i][j];
		bufferstream.clear ();
	}
	LOG(INFO) << i << " " << j;
}

void
RawParser::extractRequests (
	vector <MDRequest>					& req,
	const string						path) {
	char								buffer [BUFFERSIZE];
	ifstream							filestream (path.c_str () );
	istringstream						bufferstream;
	MDRequest							tmp;
	int									a;
	LOG_IF (FATAL, ! filestream.good () )
		<< "The request file '" << path << "' does not exist";

	for (filestream.getline (buffer, BUFFERSIZE); filestream.good ();
		filestream.getline (buffer, BUFFERSIZE) ) {
		bufferstream.str (buffer);
		bufferstream >> tmp.date >> a >> tmp.host >> tmp.objID;
		tmp.type = a ? OGRT_WRITE : OGRT_READ;
		req.push_back (tmp);
		bufferstream.clear ();
	}
	
	LOG (INFO) << "Extraction of " << req.size () << " requests completed";
}
