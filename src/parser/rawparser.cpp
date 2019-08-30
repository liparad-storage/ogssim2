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

//! \file	rawparser.cpp
//! \brief	Parser for workload files (in RAW format).

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <cstdio>
#include <fstream>
#include <sstream>

#include "parser/rawparser.hpp"
#include "parser/xmlparser.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
/* CONSTANT VALUES -----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static const OGSS_Ushort	BUFFER_SIZE		= 128;

/*----------------------------------------------------------------------------*/
/* NAMESPACE FUNCTIONS -------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

OGSS_Ushort
RawParser::getNumField (
	const OGSS_String		path) {
	char					buffer [BUFFER_SIZE];
	OGSS_Ushort				numField = 0;
	OGSS_Real				tmp;
	ifstream				filestream (path.c_str () );
	istringstream			bufferstream;

	LOG_IF (FATAL, ! filestream.good () ) << "The workload file '" << path
		<< "' does not exist! The application will exit now!";

	do
		filestream.getline (buffer, BUFFER_SIZE);
	while (buffer [0] == '#' && filestream.good () );

	if (! filestream.good () ) return 0;

	bufferstream.str (buffer);

	bufferstream >> tmp;
	while (! bufferstream.fail () ) {
		numField ++;
		bufferstream >> tmp;
	}

	return numField;
}

void
RawParser::extractRequests (
	const OGSS_String		path,
	vector <tuple <OGSS_Real, OGSS_RequestType, OGSS_Ulong,
		OGSS_Ulong, OGSS_Ushort, OGSS_Ushort> >
							& requests) {
	char					buffer [BUFFER_SIZE];
	OGSS_Ulong				dataUnitSize;
	OGSS_Ushort				numField = getNumField (path);
	ifstream				filestream (path.c_str () );
	istringstream			bufferstream;
	tuple <OGSS_Real, OGSS_RequestType, OGSS_Ulong,
		OGSS_Ulong, OGSS_Ushort, OGSS_Ushort>
							elt;

	if (numField == 0)
		return;

	LOG_IF (FATAL, ! filestream.good () ) << "The workload file '" << path
		<< "' does not exist! Will consider an empty file";

	// Add the dataunitsize research
	dataUnitSize = 1;

	do {
		filestream.getline (buffer, BUFFER_SIZE);

		if (buffer [0] == '#' && filestream.good () ) continue;

		bufferstream.clear ();
		bufferstream.str (buffer);

		bufferstream >> get <0> (elt);

		if (! bufferstream.good () ) break;

		bufferstream >> get <1> (elt) >> get <2> (elt) >> get <3> (elt);

		if (numField > 4)
			bufferstream >> get <4> (elt);

		if (numField > 5)
			bufferstream >> get <5> (elt);

		requests.push_back (elt);

	} while (filestream.good () );
}

/*----------------------------------------------------------------------------*/
/* UNITARY TEST --------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

UT_RawParser::UT_RawParser (
	const OGSS_String		& configurationFile):
	UnitaryTest <UT_RawParser> (MTP_RAWPARSER) {
	set <OGSS_String>		testNames;

	XMLParser::getListOfRequestedUnitaryTests (
		configurationFile, _module, testNames);

	for (auto & elt: testNames) {
		if (! elt.compare ("all") ) {
			_tests.push_back (make_pair ("Empty file",
				&UT_RawParser::emptyFile) );
			_tests.push_back (make_pair ("Filled file",
				&UT_RawParser::filledFile) );
			_tests.push_back (make_pair ("Commented file",
				&UT_RawParser::commentedFile) );
			_tests.push_back (make_pair ("Bad ending file",
				&UT_RawParser::badEndingFile) );
		}
		else if (! elt.compare ("emptyFile") )
			_tests.push_back (make_pair ("Empty file",
				&UT_RawParser::emptyFile) );
		else if (! elt.compare ("filledFile") )
			_tests.push_back (make_pair ("Filled file",
				&UT_RawParser::filledFile) );
		else if (! elt.compare ("commentedFile") )
			_tests.push_back (make_pair ("Commented file",
				&UT_RawParser::commentedFile) );
		else if (! elt.compare ("badEndingFile") )
			_tests.push_back (make_pair ("Bad ending file",
				&UT_RawParser::badEndingFile) );
		else
			LOG (WARNING) << ModuleNameMap.at (_module) << " unitary test "
				<< "named '" << elt << "' does not match!" << endl;
	}
}

UT_RawParser::~UT_RawParser () {  }

OGSS_Bool
UT_RawParser::emptyFile () {
	ofstream 				filestream ("_ut_test.data");
	vector <tuple <OGSS_Real, OGSS_RequestType, OGSS_Ulong,
		OGSS_Ulong, OGSS_Ushort, OGSS_Ushort> >
							requests;

	filestream.close ();
	RawParser::extractRequests ("_ut_test.data", requests);

	remove ("_ut_test.data");

	if (! requests.empty () )
		return false;

	return true;
}

OGSS_Bool
UT_RawParser::filledFile () {
	OGSS_Ushort				num = 15;
	ofstream 				filestream ("_ut_test.data");
	vector <tuple <OGSS_Real, OGSS_RequestType, OGSS_Ulong,
		OGSS_Ulong, OGSS_Ushort, OGSS_Ushort> >
							requests;

	for (auto i = 0; i < num; ++i)
		filestream << i << " 0 0 0" << endl;

	filestream.close ();
	RawParser::extractRequests ("_ut_test.data", requests);

	remove ("_ut_test.data");

	if (requests.size () != num)
		return false;

	return true;
}

OGSS_Bool
UT_RawParser::commentedFile () {
	OGSS_Ushort				num = 20;
	OGSS_Ushort				mod = 4;
	ofstream 				filestream ("_ut_test.data");
	vector <tuple <OGSS_Real, OGSS_RequestType, OGSS_Ulong,
		OGSS_Ulong, OGSS_Ushort, OGSS_Ushort> >
							requests;

	for (auto i = 0; i < num; ++i)
		if (i % mod == 0)
			filestream << "#Commentary" << endl;
		else
			filestream << i << " 0 0 0" << endl;

	filestream.close ();
	RawParser::extractRequests ("_ut_test.data", requests);

	remove ("_ut_test.data");

	if (requests.size () != num - num / mod)
		return false;

	return true;
}

OGSS_Bool
UT_RawParser::badEndingFile () {
	OGSS_Ushort				num = 15;
	ofstream 				filestream ("_ut_test.data");
	vector <tuple <OGSS_Real, OGSS_RequestType, OGSS_Ulong,
		OGSS_Ulong, OGSS_Ushort, OGSS_Ushort> >
							requests;

	for (auto i = 0; i < num - 1; ++i)
		filestream << i << " 0 0 0" << endl;
	filestream << num - 1 << " 0 0 0";

	filestream.close ();
	RawParser::extractRequests ("_ut_test.data", requests);

	remove ("_ut_test.data");

	if (requests.size () != num)
		return false;

	return true;
}
