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

//! \file	xmlparser-conf.cpp
//! \brief	Namespaces used for XML parsing. Contains the functions related to
//! 		the configuration file.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <algorithm>
#include <fstream>
#include <map>

#include "xxmlparser-base.cpp"

#if ! USE_TINYXML

/*----------------------------------------------------------------------------*/
/* PRIVATE NAMESPACE ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

namespace {
	pair <OGSS_String, OGSS_String>
	retrieveZMQInformation (
		DOMNode				* node) {
		ostringstream		oss ("");
		ostringstream		ost ("");

		oss << _getString (node, ParamNameMap.at (PTP_PROTOCOL) )
			<< "://" << _getString (node, ParamNameMap.at (PTP_ADDRESS) )
			<< ":" << _getString (node, ParamNameMap.at (PTP_PORT) );
		ost << _getString (node, ParamNameMap.at (PTP_PROTOCOL) )
			<< "://" << _getString (node, ParamNameMap.at (PTP_ADDRESS) )
			<< ":" << _getLong (node, ParamNameMap.at (PTP_PORT) ) + 1;

		return make_pair (oss.str (), ost.str () );
	}

	vector <pair <OGSS_String, set <OGSS_Long>>>
	retrieveGroupTarget (
		DOMNode				* node) {
		vector <pair <OGSS_String, set <OGSS_Long>>> v;
		pair <OGSS_String, set <OGSS_Long>> p;
		istringstream		iss (_getString (node,
			ParamNameMap.at (PTP_TARGET), true) );
		OGSS_String			title;
		char				c;
		int					m, n;

		while (iss.peek () != EOF) {
			title = "";
			while (iss.peek () != ':') {
				iss >> c;
				title += c;
			}

			p = make_pair (title, set <OGSS_Long> () );

			iss >> c;
			iss >> m;

			while (iss.peek () == '-' || iss.peek () == ',') {
				iss >> c >> n;

				if (c == ',')
					p.second.insert (m);
				else if (c == '-')
					for (auto i = m; i < n; ++i)
						p.second.insert (i);
			
				m = n;
			}

			p.second.insert (m);
			v.push_back (p);

			if (iss.peek () == ';') {
				iss >> c;
				continue;
			} else break;
		}

		return v;
	}

	vector <pair <OGSS_String, set <OGSS_Long>>>
	retrieveTarget (
		DOMNode				* node) {
		vector <pair <OGSS_String, set <OGSS_Long>>> v;
		pair <OGSS_String, set <OGSS_Long>> p;
		istringstream iss (_getString (node,
			ParamNameMap.at (PTP_TARGET), true) );
		OGSS_String title;
		char c;
		OGSS_Long m, n;

		while (iss.peek () != EOF) {
			p = make_pair ("all", set <OGSS_Long> () );

			iss >> m;

			while (iss.peek () == '-' || iss.peek () == ',') {
				iss >> c >> n;

				if (c == ',')
					p.second.insert (m);
				else if (c == '-')
					for (auto i = m; i < n; ++i)
						p.second.insert (i);
			
				m = n;
			}

			p.second.insert (m);
			v.push_back (p);
		}

		return v;
	}

	vector <pair <OGSS_String, OGSS_Real>>
	retrieveRules (
		DOMNode				* node) {
		vector <pair <OGSS_String, OGSS_Real>> v;
		pair <OGSS_String, OGSS_Real> p;
		istringstream iss (_getString (node, ParamNameMap.at (PTP_RULES), true) );
		OGSS_String title;
		char c;
		OGSS_Real d;

		while (iss.peek () != EOF) {
			title = "";
			while (iss.peek () != ':') { iss >> c; title += c; }

			iss >> c >> d;

			v.push_back (make_pair (title, d) );

			if (iss.peek () == ';') { iss >> c; continue; }
			else break;
		}

		return v;
	}
}

/*----------------------------------------------------------------------------*/
/* PUBLIC NAMESPACE ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

OGSS_String
XMLParser::getFilePath (
	const OGSS_String		& filename,
	const OGSS_FileType		& fileType,
	const OGSS_Bool			& input) {
	ifstream				filestream (filename.c_str () );
	XercesDOMParser			parser;
	DOMNode					* node;

	if (! filestream.good () ) {
		LOG (FATAL) << "The configuration file '" << filename
			<< "' does not exist!";
		return "";
	}

	filestream.close ();

	parser.parse (filename.c_str () );

	node = parser.getDocument () ->getDocumentElement ();
	if (input)
		node = _getNode (node, ParamNameMap.at (PTP_INPUT), true);
	else
		node = _getNode (node, ParamNameMap.at (PTP_OUTPUT), true);

	OGSS_String s = _getString (node, FileNameMap.at (fileType), false);

	return s;
}

OGSS_DataUnit
XMLParser::getDataUnit (
	const OGSS_String		& filename,
	const OGSS_ParamType	& paramType) {
	ifstream				filestream (filename.c_str () );
	XercesDOMParser			parser;
	DOMNode					* node;

	if (! filestream.good () ) {
		LOG (FATAL) << "The configuration file '" << filename
			<< "' does not exist!";
		return 1;
	}

	filestream.close ();

	parser.parse (filename.c_str () );

	node = parser.getDocument () ->getDocumentElement ();

	OGSS_String s;
	OGSS_DataUnit u;
	
	if(paramType == PTP_WORKLOAD){
		node = _getNode (node, ParamNameMap.at (PTP_WORKLOAD), true);
		s = _getString(node, ParamNameMap.at(PTP_DUNAME), true);
	}

	if(paramType == PTP_GLOBAL){
		node = _getNode (node, ParamNameMap.at (PTP_GLOBAL), true);
		s = _getString(node, ParamNameMap.at(PTP_DUNAME), true);
	}

	if(s.empty()){
		return u;
	}
	
	node = parser.getDocument () ->getDocumentElement ();
	node = _getNode (node, ParamNameMap.at (PTP_DATAUNITS), true);

	DOMNodeList *list = node->getChildNodes ();

	for (auto idx = 0; idx < list->getLength (); ++idx) {
		if(!_getString(list->item (idx), ParamNameMap.at (PTP_NAME), true).compare(s)){
			u._time =  _getRealPrefix (list->item (idx), PTP_TIME);
			u._memory = _getRealPrefix (list->item (idx),  PTP_MEMORY);
			break;
		}
	}

	return u;
}

OGSS_String
XMLParser::getConfigParameter (
	const OGSS_String		& filename,
	const OGSS_ParamType	& paramType) {
	ifstream				filestream (filename.c_str () );
	XercesDOMParser			parser;
	DOMNode					* node;

	if (! filestream.good () ) {
		LOG (FATAL) << "The configuration file '" << filename
			<< "' does not exist!";
		return 0;
	}

	filestream.close ();

	parser.parse (filename.c_str () );

	node = parser.getDocument () ->getDocumentElement ();
	node = _getNode (node, ParamNameMap.at (PTP_GLOBAL), true);

	return _getString (node, ParamNameMap.at (paramType), false);
}

vector <Graph>
XMLParser::getGraphs (
	const OGSS_String		& filename) {
	ifstream				filestream (filename.c_str () );
	XercesDOMParser			parser;
	DOMNode					* node;
	DOMNodeList				* list;
	vector <Graph>			graphs;
	Graph					elt;
	OGSS_String				str;
	char					* tmp;

	if (! filestream.good () ) {
		LOG (FATAL) << "The configuration file '" << filename
			<< "' does not exist!";
		return graphs;
	}

	filestream.close ();
	parser.parse (filename.c_str () );

	node = parser.getDocument () ->getDocumentElement ();
	node = _getNode (node, ParamNameMap.at (PTP_OUTPUT), true);

	list = node->getChildNodes ();
	for (auto idx = 0; idx < list->getLength (); ++idx) {
		tmp = _DOM_GET_STRING (list->item (idx) ->getNodeName () );
		if (! ParamNameMap.at (PTP_GRAPH) .compare (tmp) ) {
			node = list->item (idx);
			elt._filename = _getText (node);

			str = _getString (node, ParamNameMap.at (PTP_TYPE), true);
			auto findResult = find_if (GraphTypeNameMap.begin (), GraphTypeNameMap.end (),
				[&] (const pair <OGSS_GraphType, OGSS_String> & elt)
				{ return ! elt.second.compare (str); } );
			if (findResult != GraphTypeNameMap.end () )
				elt._type = findResult->first;
			else elt._type = GPT_TOTAL;

			str = _getString (node, ParamNameMap.at (PTP_FORMAT), true);
			auto findRes2 = find_if (GraphFormatNameMap.begin (), GraphFormatNameMap.end (),
				[&] (const pair <OGSS_GraphFormatType, OGSS_String> & elt)
				{ return ! elt.second.compare (str); } );
			if (findRes2 != GraphFormatNameMap.end () )
				elt._format = findRes2->first;
			else elt._format = GFM_TOTAL;

			switch (elt._type) {
				case GPT_REQRESP: elt._target = retrieveGroupTarget (node);
					elt._arg1 = _getLong (node, ParamNameMap.at (PTP_ARG1), true);
					elt._rules = retrieveRules (node);
					elt._arg2 = _getLong (node, ParamNameMap.at (PTP_ARG2), true);
					break;
				case GPT_DEVPROF: elt._target = retrieveTarget (node); break;
				case GPT_WAITTM:  elt._target = retrieveTarget (node);
					elt._arg1 = _getLong (node, ParamNameMap.at (PTP_ARG1), true);
					break;
				case GPT_REQCPLT: elt._target = retrieveTarget (node); break;
				case GPT_DEVTHRP: elt._target = retrieveGroupTarget (node); 
					elt._arg1 = _getLong (node, ParamNameMap.at (PTP_ARG1), true);
					break;
				default:
				break;
			};

			graphs.push_back (elt);
		}

		_DOM_FREE_STRING (tmp);
	}

	return graphs;
}

vector <Event>
XMLParser::getEvents (
	const OGSS_String		& filename) {
	ifstream				filestream (filename.c_str () );
	XercesDOMParser			parser;
	DOMNode					* node;
	DOMNodeList				* list;
	vector <Event>			events;
	Event					elt;
	OGSS_String				str;
	char					* tmp;

	if (! filestream.good () ) {
		LOG (FATAL) << "The configuration file '" << filename
			<< "' does not exist!";
		return events;
	}

	filestream.close ();
	parser.parse (filename.c_str () );

	node = parser.getDocument () ->getDocumentElement ();
	node = _getNode (node, ParamNameMap.at (PTP_EVENT), true);

	list = node->getChildNodes ();
	for (auto idx = 0; idx < list->getLength (); ++idx) {
		tmp = _DOM_GET_STRING (list->item (idx) ->getNodeName () );
		if (! ParamNameMap.at (PTP_ENTRY) .compare (tmp) ) {
			node = list->item (idx);

			str = _getString (node, ParamNameMap.at (PTP_TYPE), true);
			auto findResult = find_if (EventTypeNameMap.begin (), EventTypeNameMap.end (),
				[&] (const pair <OGSS_EventType, OGSS_String> & elt)
				{ return ! elt.second.compare (str); } );
			if (findResult != EventTypeNameMap.end () )
				elt._type = findResult->first;
			else elt._type = EVT_TOTAL;

			elt._target = _getLong (node, ParamNameMap.at (PTP_TARGET), true);
			elt._date = _getReal (node, ParamNameMap.at (PTP_DATE), true);

			events.push_back (elt);
		}

		_DOM_FREE_STRING (tmp);
	}

	return events;
}

OGSS_String
XMLParser::getComputationModel (
	const OGSS_String		& filename,
	const OGSS_ParamType	& paramType) {
	ifstream				filestream (filename.c_str () );
	XercesDOMParser			parser;
	DOMNode					* node;

	if (! filestream.good () ) {
		LOG (FATAL) << "The configuration file '" << filename
			<< "' does not exist!";
		return 0;
	}

	filestream.close ();

	parser.parse (filename.c_str () );

	node = parser.getDocument () ->getDocumentElement ();
	node = _getNode (node, ParamNameMap.at (PTP_COMPUTATION), true);

	return _getString (node, ParamNameMap.at (paramType), false);
}

OGSS_Ulong
XMLParser::getCommunicationPort (
	const OGSS_String		& filename) {
	ifstream				filestream (filename.c_str () );
	XercesDOMParser			parser;
	DOMNode					* node;

	if (! filestream.good () ) {
		LOG (FATAL) << "The configuration file '" << filename
			<< "' does not exist!";
		return 5555;
	}

	filestream.close ();

	parser.parse (filename.c_str () );

	node = parser.getDocument () ->getDocumentElement ();
	node = _getNode (node, ParamNameMap.at (PTP_COMM), true);

	return _getLong (node, ParamNameMap.at (PTP_PORT) );
}

OGSS_CommType
XMLParser::getCommunicationType (
	const OGSS_String		& filename) {
	ifstream				filestream (filename.c_str () );
	XercesDOMParser			parser;
	DOMNode					* node;
	OGSS_String				type;

	if (! filestream.good () ) {
		LOG (FATAL) << "The configuration file '" << filename
			<< "' does not exist!";
			return CTP_ZMQ;
	}

	filestream.close ();

	parser.parse (filename.c_str () );

	node = parser.getDocument () ->getDocumentElement ();
	node = _getNode (node, ParamNameMap.at (PTP_COMM), true);

	type = _getString (node, ParamNameMap.at (PTP_TYPE), true);

	auto findResult = find_if (CommNameMap.begin (), CommNameMap.end (),
		[&] (const pair <OGSS_CommType, OGSS_String> & elt)
		{ return ! elt.second.compare (type); } );
	if (findResult != CommNameMap.end () )
		return findResult->first;

	return CTP_ZMQ;
}

tuple <OGSS_Bool, OGSS_String, OGSS_String>
XMLParser::getOGMDSInformation (
	const OGSS_String		& configurationFile) {
	return make_tuple (false, "", "");
}

OGSS_Bool
XMLParser::getListOfRequestedUnitaryInstances (
	const OGSS_String		& configurationFile,
	set <OGSS_ModuleType>	& uiNames) {
	ifstream				filestream (configurationFile.c_str () );
	XercesDOMParser			parser;
	DOMNode					* node;
	DOMNodeList				* list;
	char					* tmp;

	if (! filestream.good () ) {
		LOG (FATAL) << "The configuration file '" << configurationFile
			<< "' does not exist!";
			return false;
	}

	filestream.close ();

	parser.parse (configurationFile.c_str () );

	node = parser.getDocument () ->getDocumentElement ();
	node = _getNode (node, ParamNameMap.at (PTP_UTEST), true);

	list = node->getChildNodes ();
	for (auto idx = 0; idx < list->getLength (); ++idx) {
		tmp = _DOM_GET_STRING (list->item (idx) ->getNodeName () );
		if (tmp [0] != '#') {
			auto findResult = find_if (ModuleNameMap.begin (), ModuleNameMap.end (),
				[&] (const pair <OGSS_ModuleType, OGSS_String> & elt)
				{ return ! elt.second.compare (tmp); } );
			if (findResult != ModuleNameMap.end () )
				uiNames.insert (findResult->first);
			else
				LOG (INFO) << "The name '" << tmp << "' does not match a known "
					<< "module name!";
		}

		_DOM_FREE_STRING (tmp);
	}

	return true;
}

OGSS_Bool
XMLParser::getListOfRequestedUnitaryTests (
	const OGSS_String		& configurationFile,
	const OGSS_ModuleType	& uiName,
	set <OGSS_String>		& testNames) {
	ifstream				filestream (configurationFile.c_str () );
	XercesDOMParser			parser;
	DOMNode					* node;
	DOMNode					* subnode;
	DOMNodeList				* list;
	char					* tmp;

	if (! filestream.good () ) {
		LOG (FATAL) << "The configuration file '" << configurationFile
			<< "' does not exist!";
			return false;
	}

	filestream.close ();

	parser.parse (configurationFile.c_str () );

	node = parser.getDocument () ->getDocumentElement ();
	node = _getNode (node, ParamNameMap.at (PTP_UTEST), true);
	node = _getNode (node, ModuleNameMap.at (uiName) );

	list = node->getChildNodes ();

	// Check for <Module /> syntax
	if (list->getLength () == 0)
		testNames.insert ("all");
	// Check for <Module><test>...</test></Module> syntax
	else {
		for (auto idx = 0; idx < list->getLength (); ++idx) {
			tmp = _DOM_GET_STRING (list->item (idx) ->getNodeName () );
			if (tmp [0] != '#')
				testNames.insert (_getText (list->item (idx) ) );
			_DOM_FREE_STRING (tmp);
		}
	// Check for <Module>all</Module> syntax
		if (testNames.empty () )
			testNames.insert ("all");
	}

	return true;
}

#endif
