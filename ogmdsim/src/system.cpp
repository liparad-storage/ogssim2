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

//! \file			system.cpp
//! \brief			Definition of the system class which is one of the two main data structures of OGMDSim. It contains
//!					the information about the servers used in the system  
//!					a proximity score to the host nodes and facilitates the right metadata node
//!					when processing a request.

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include "system.hpp"

#include <iostream>
#include <limits>
#include <set>

#include <chrono>

#include "rawextract.hpp"

using namespace std;

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

System::System (
	OGXML								& parser):
	_parser (parser) {
	string								a {""};
	uint64_t							mssize {1};
	uint64_t							coef {1};
	double								d {1.};
	bool								chr {false};
	chrono::time_point <chrono::high_resolution_clock>
										start, end;

	parser.getXMLItem <bool> (chr, OGFT_CFGFILE, "global/chrono");

	LOG_IF (FATAL, ! parser.getXMLItem <int> (_nbHostServers, OGFT_SYSFILE, "global/nbhost") )
		<< "Tag: 'global/nbhost' must be provided";
	LOG_IF (FATAL, ! parser.getXMLItem <int> (_nbMetadataServers, OGFT_SYSFILE, "global/nbmetadata") )
		<< "Tag: 'global/nbmetadata' must be provided";
	_nbServers = _nbHostServers + _nbMetadataServers;

	if (parser.getXMLItem <string> (a, OGFT_SYSFILE, "global/mssize/unit", true) && ! a.compare ("object") ) {
		LOG_IF (FATAL, ! parser.getXMLItem <string> (a, OGFT_SYSFILE, "global/mssize") )
			<< "Tag: 'global/mssize' must be provided";
		_mssize = static_cast <uint64_t> (OGUtils::convertPrefix (a) );
	} else {
		LOG_IF (FATAL, ! parser.getXMLItem <string> (a, OGFT_SYSFILE, "global/mssize") )
			<< "Tag: 'global/mssize' must be provided";
		_mssize = static_cast <uint64_t> (OGUtils::convertPrefix (a) );

		LOG_IF (FATAL, ! parser.getXMLItem <string>	(a, OGFT_SYSFILE, "global/mosize") )
			<< "Tag: 'global/mosize' must be provided";
		_mssize /= static_cast <uint64_t> (OGUtils::convertPrefix (a) );
	}

	parser.getXMLItem <double> (d, OGFT_SYSFILE, "global/loadbalancing");
	_limit = static_cast <uint64_t> (d * _mssize);

	_matrix = vector <vector <double>> (
		_nbServers, vector <double> (
			_nbServers, 0) );
	_paths = vector <vector <pair <double, int>>> (
		_nbHostServers, vector <pair <double, int>> (
			_nbMetadataServers, make_pair (numeric_limits <double> ::max (), 0) ) );

	_nodeLoad = vector <queue <uint64_t>> (_nbServers);

	LOG_IF (FATAL, ! parser.getXMLItem <string> (a, OGFT_SYSFILE, "path/matrix") )
		<< "Tag: 'path/matrix' must be provided";
	RawParser::extractMatrix (_matrix, a);

	if (chr) start = chrono::high_resolution_clock::now ();
	updatePaths ();
	buildScore ();
	updateScores ();
	if (chr) end = chrono::high_resolution_clock::now ();

	LOG_IF (INFO, chr) << "Computation done in "
		<< chrono::duration_cast <chrono::microseconds> (end - start) .count ()
		<< " us" << endl;
}

System::~System () {
}

int
System::getNextAvailableMDServer (
	const int							server) {
	for (auto e: _orderedServers [server]) {
		if (_nodeLoad [e.second] .size () < _limit)
			return e.second;
	}

	return -1;
}

int
System::getNextOverloadedMDServer (
	const int							server) {
	for (auto e: _orderedServers [server]) {
		if (_nodeLoad [e.second] .size () < _mssize)
			return e.second;
	}
	
	return -1;
}

void
System::addObject (
	const int							server,
	const uint64_t						objID) {
	_nodeLoad [server] .push (objID);
}

uint64_t
System::rmObject (
	const int							server,
	const uint64_t						objID) {
	uint64_t 							tmp {objID};
	if (objID == numeric_limits <uint64_t> ::max () ) {
		tmp = _nodeLoad [server] .front ();
		_nodeLoad [server] .pop ();
	} else {
		tmp = _nodeLoad [server] .front ();
		_nodeLoad [server] .pop ();
		while (tmp != objID) {
			_nodeLoad [server] .push (tmp);
			tmp = _nodeLoad [server] .front ();
			_nodeLoad [server] .pop ();
		}
	}

	return tmp;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS -------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

void
System::updatePaths () {
	for (auto i = 0; i < _nbHostServers; ++i) {
		auto							k {i};
		pair <double, int> 				old {make_pair (0, 0) };
		set <int>						done;
		for (auto j = _nbHostServers; j < _nbServers; ++j)
			done.insert (j);

		do {
			for (auto j = _nbHostServers; j < _nbServers; ++j) {
				if (k == j)					continue;
				if (_matrix [k][j] < 0)		continue;

				if (old.first + _matrix [k][j] < _paths [i][j - _nbHostServers] .first) {
					_paths [i][j - _nbHostServers] .first = old.first + _matrix [k][j];
					_paths [i][j - _nbHostServers] .second = old.second + 1;
				} else if (old.first + _matrix [k][j] == _paths [i][j - _nbHostServers] .first)
					_paths [i][j - _nbHostServers] .second
						= min (_paths [i][j - _nbHostServers] .second, old.second + 1);
			}

			old.first = numeric_limits <double> ::max ();
			for (auto e: done) {
				if (_paths [i][e - _nbHostServers] .first < old.first) {
					old = _paths [i][e - _nbHostServers];
					k = e;
				}
			}

			done.erase (k);
		} while (! done.empty () );
	}
}

void
System::buildScore () {
	bool								defBounds {false};
	pair <double, int>					lower {make_pair (numeric_limits <double> ::max (), numeric_limits <int> ::max () ) };
	pair <double, int>					upper {make_pair (numeric_limits <double> ::min (), numeric_limits <int> ::min () ) };
	double								rate {.5};

	_parser.getXMLItem <bool> (defBounds, OGFT_CFGFILE, "score/defbounds", true);

	if (defBounds) {
		LOG_IF (FATAL, ! _parser.getXMLItem <double> (lower.first, OGFT_CFGFILE, "score/latency/min", true) )
			<< "Attribute 'score/latency/min' must be provided";
		LOG_IF (FATAL, ! _parser.getXMLItem <double> (upper.first, OGFT_CFGFILE, "score/latency/max", true) )
			<< "Attribute 'score/latency/max' must be provided";

		LOG_IF (FATAL, ! _parser.getXMLItem <int> (lower.second, OGFT_CFGFILE, "score/nbjumps/min", true) )
			<< "Attribute 'score/nbjumps/min' must be provided";
		LOG_IF (FATAL, ! _parser.getXMLItem <int> (upper.second, OGFT_CFGFILE, "score/nbjumps/max", true) )
			<< "Attribute 'score/nbjumps/max' must be provided";
	} else {
		for (auto e: _paths) {
			for (auto f: e) {
				lower.first = min (lower.first, f.first);
				upper.first = max (upper.first, f.first);

				lower.second = min (lower.second, f.second);
				upper.second = max (upper.second, f.second);
			}
		}
	}

	LOG (INFO) << "Score boundaries: (" << lower.first << " > " << upper.first
		<< ") & (" << lower.second << " > " << upper.second << ")";

	LOG_IF (WARNING, ! _parser.getXMLItem <double> (rate, OGFT_CFGFILE, "score/rate") )
		<< "Tag 'score/rate' is not provided, default value is chosen (.5)";
	_score = Score (lower, upper, rate);
}

void
System::updateScores () {
	for (auto i = 0; i < _nbHostServers; ++i) {
		_orderedServers.push_back (multimap <double, int, greater <double>> () );
		for (auto j = 0; j < _paths [i] .size (); ++j) {
			_orderedServers [i] .insert (make_pair (_score.computeScore (_paths [i][j]), _nbHostServers + j) );
		}
	}
}
