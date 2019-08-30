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

//! \file	workloadextractor.cpp
//! \brief	<code>WorkloadExtractor</code> examines a given trace file and
//! 		extracts the user requests contained in it.
//!
//! <code>WorkloadExtractor</code> gets a start-up role and has to send to the
//! <code>PreProcessing</code> all the information related to the trace before
//! the simulation starts.
//!
//! More information are given in the header file.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <algorithm>
#include <fstream>
#include <set>

#include "module/workloadextractor.hpp"

#include "parser/rawparser.hpp"

#include "parser/xmlextract.hpp"
#include "parser/xmlparser.hpp"

#include "util/wrapper.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
/* UTILS ---------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//!	\brief	Sort 6-member tuples.
//!
//!	The sorting is done depending on two criteria: the first one is the request
//! arrival date (the earlier), and the second one the request priority (user
//! requests are executed prior than system requests).
//!
//!	\param	a					First term.
//! \param	b					Second term.
//!	\return	True if the first term is sorted before the second one.
OGSS_Bool
sortTuple (
	const tuple <OGSS_Real, OGSS_RequestType, OGSS_Ulong,
		OGSS_Ulong, OGSS_Ushort, OGSS_Ushort>
							& a,
	const tuple <OGSS_Real, OGSS_RequestType, OGSS_Ulong,
		OGSS_Ulong, OGSS_Ushort, OGSS_Ushort>
							& b) {
	return (get <0> (a) < get <0> (b) )
		|| (get <0> (a) == get <0> (b) && get <2> (a) < get <2> (b) );
}

//!	\brief	Sort 4-member tuples.
//!
//!	The sorting is done depending on two criteria: the first one is the request
//! arrival date (the earlier), and the second one the request priority (user
//! requests are executed prior than system requests).
//!
//!	\param	a					First term.
//! \param	b					Second term.
//!	\return	True if the first term is sorted before the second one.
OGSS_Bool
sortMinTuple (
	const tuple <OGSS_Real, OGSS_RequestType, OGSS_Ulong, OGSS_Ulong>
							& a,
	const tuple <OGSS_Real, OGSS_RequestType, OGSS_Ulong, OGSS_Ulong>
							& b) {
	return (get <0> (a) < get <0> (b) )
		|| (get <0> (a) == get <0> (b) && get <2> (a) < get <2> (b) );
}

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

WorkloadExtractor::WorkloadExtractor (
	const OGSS_String		configurationFile):
	Module (configurationFile, make_pair (MTP_WORKLOAD, 0) ) {

	_localDU = XMLParser::getDataUnit(_cfg, PTP_WORKLOAD);
	_globalDU = XMLParser::getDataUnit(_cfg, PTP_GLOBAL);
  
	DLOG(INFO) << "Local DU: " << _localDU._time << "/" << _localDU._memory;
	DLOG(INFO) << "Globl DU: " << _globalDU._time << "/" << _globalDU._memory;
}

WorkloadExtractor::~WorkloadExtractor () {  }

void
WorkloadExtractor::processExtraction () {
	OGSS_String				filename {""};
	void					* arg;
	OGSS_Ulong				numRequests;

	auto i = XMLParser::getOGMDSInformation (_cfg);

	if (get <0> (i) == true) {
		OGXML				x {get <1> (i) + '/' + get <2> (i) };

		OGMDSimLauncher (get <1> (i), get <2> (i) );

		x.getXMLItem <string> (filename, OGFT_CFGFILE, "path/output");
		filename = get <1> (i) +'/' + filename;

//		LOG(FATAL) << "[WD] DONE on " << get <1> (i) + '/' + filename << "!!";
	} else {
		filename = XMLParser::getFilePath (_cfg, FTP_WORKLOAD);
	}

	extract (filename);
	numRequests = _requests.size ();

#ifndef UTEST
	_ci->send (make_pair (MTP_SYNCHRONIZATION, 0),
		& _id, sizeof (_id), true);
	_ci->send (make_pair (MTP_SYNCHRONIZATION, 0),
		& numRequests, sizeof (numRequests) );

	_ci->receive (arg);	free (arg); // From sync
#endif
}

void
WorkloadExtractor::processDecomposition () {
	LOG (INFO) << "[WD] Starting the simulation process with "
		<< _requests.size () << " requests";

	sendData ();
}

void
WorkloadExtractor::processSynchronization () {  }

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
WorkloadExtractor::extract (
	const OGSS_String		workloadFile) {
	vector <tuple <OGSS_Real, OGSS_RequestType, OGSS_Ulong,
		OGSS_Ulong, OGSS_Ushort, OGSS_Ushort> > requests;
	OGSS_Ulong				cnt {0};

	RawParser::extractRequests (workloadFile, requests);

	DLOG(INFO) << "Extraction size: " << requests.size ();

	sort (requests.begin (), requests.end (), sortTuple);

	for (auto & elt: requests) {
		_requests.push_back (Request (elt) );
		_requests.back () ._mainIdx = cnt++;
	}

	if (_localDU != _globalDU)
		applyDataUnit ();
}

void
WorkloadExtractor::applyDataUnit () {
	for (auto & elt: _requests) {
		elt._date *= (_localDU._time / _globalDU._time);
		elt._address *= (_localDU._memory / _globalDU._memory);
		elt._size *= (_localDU._memory / _globalDU._memory);
	}
}

/*----------------------------------------------------------------------------*/
/* UNITARY TEST --------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

UT_WorkloadExtractor::UT_WorkloadExtractor (
	const OGSS_String		& configurationFile):
	UnitaryTest <UT_WorkloadExtractor> (MTP_WORKLOAD) {
	set <OGSS_String>		testNames;

	XMLParser::getListOfRequestedUnitaryTests (
		configurationFile, _module, testNames);

	for (auto & elt: testNames) {
		if (! elt.compare ("all") ) {
			_tests.push_back (make_pair ("Bad parameter",
				&UT_WorkloadExtractor::badParameter) );
			_tests.push_back (make_pair ("Check file",
				&UT_WorkloadExtractor::checkFile) );
			_tests.push_back (make_pair ("Unordered file",
				&UT_WorkloadExtractor::unorderedFile) );
		}
		else if (! elt.compare ("badParameter") )
			_tests.push_back (make_pair ("Bad parameter",
				&UT_WorkloadExtractor::badParameter) );
		else if (! elt.compare ("checkFile") )
			_tests.push_back (make_pair ("Check file",
				&UT_WorkloadExtractor::checkFile) );
		else if (! elt.compare ("unorderedFile") )
			_tests.push_back (make_pair ("Unordered file",
				&UT_WorkloadExtractor::unorderedFile) );
		else
			LOG (WARNING) << ModuleNameMap.at (_module) << " unitary test "
				<< "named '" << elt << "' does not match!";
	}
}

UT_WorkloadExtractor::~UT_WorkloadExtractor () {  }

OGSS_Bool
UT_WorkloadExtractor::badParameter () {
	WorkloadExtractor		module ("env/conf/_ut_config.xml");

	if (module._requests.size () != 0)
		return false;

	return true;
}

OGSS_Bool
UT_WorkloadExtractor::checkFile () {
	OGSS_Ushort				num = 15;
	ofstream 				filestream ("_ut_test.data");
	set <tuple <OGSS_Real, OGSS_RequestType, OGSS_Ulong, OGSS_Ulong>,
		OGSS_Bool (*) (
			const tuple <OGSS_Real, OGSS_RequestType, OGSS_Ulong, OGSS_Ulong>&,
			const tuple <OGSS_Real, OGSS_RequestType, OGSS_Ulong, OGSS_Ulong>&) >
							requests (sortMinTuple);

	for (auto i = 0; i < num; ++i) {
		filestream << i << " " << i%2 << " " << i*8 << " " << 4 << endl;
		requests.insert (make_tuple (i, (i%2)?RQT_WRITE:RQT_READ, i*8, 4) );
	}

	filestream.close ();

	WorkloadExtractor		module ("env/conf/_ut_config.xml");

	remove ("_ut_test.data");

	for (auto & elt: module._requests) {
		tuple <OGSS_Real, OGSS_RequestType, OGSS_Ulong, OGSS_Ulong>
							tmp;
		
		get <0> (tmp) = elt._date;
		get <1> (tmp) = elt._type;
		get <2> (tmp) = elt._address;
		get <3> (tmp) = elt._size;

		requests.erase (tmp);
	}

	if (! requests.empty () )
		return false;

	return true;
}

OGSS_Bool
UT_WorkloadExtractor::unorderedFile () {
	OGSS_Ushort				num = 15;
	OGSS_Real				prevTime = .0;
	ofstream 				filestream ("_ut_test.data");

	for (auto i = 0; i < num; ++i)
		filestream << i << " 0 0 0" << endl;
	for (auto i = 20; i > 20 - num; --i)
		filestream << i << " 0 0 0" << endl;

	filestream.close ();

	WorkloadExtractor		module ("env/conf/_ut_config.xml");

	remove ("_ut_test.data");

	for (auto & elt: module._requests) {
		if (prevTime > elt._date)
			return false;
		prevTime = elt._date;
	}

	return true;
}
