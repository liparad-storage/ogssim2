#include <algorithm>

#include "txmlparser-base.cpp"

#if USE_TINYXML

namespace {
	pair <OGSS_String, OGSS_String>
	retrieveZMQInformation (
		XMLElement				* node) {
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
		XMLElement				* node) {
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
		XMLElement				* node) {
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
		XMLElement				* node) {
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
	XMLDocument				doc;
	XMLElement				* node {_getRootNode (filename, doc) };

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
	XMLDocument				doc;
	XMLElement				* root {_getRootNode (filename, doc) };
	XMLElement				* node {root};

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
	
	node = _getNode (root, ParamNameMap.at (PTP_DATAUNITS), true);

	node = node->FirstChildElement (ParamNameMap.at (PTP_DATAUNIT) .c_str () );
	while (node) {
		if (node->Attribute (ParamNameMap.at (PTP_NAME) .c_str (), s.c_str () ) ) {
			u._time =  _getRealPrefix (node, PTP_TIME);
			u._memory = _getRealPrefix (node,  PTP_MEMORY);
			break;
		}

		node = node->NextSiblingElement (ParamNameMap.at (PTP_DATAUNIT) .c_str () );
	}

	return u;
}

OGSS_String
XMLParser::getConfigParameter (
	const OGSS_String		& filename,
	const OGSS_ParamType	& paramType) {
	XMLDocument				doc;
	XMLElement				* root {_getRootNode (filename, doc) };
	root = _getNode (root, ParamNameMap.at (PTP_GLOBAL), true);

	return _getString (root, ParamNameMap.at (paramType), false);
}

vector <Graph>
XMLParser::getGraphs (
	const OGSS_String		& filename) {
	XMLDocument				doc;
	XMLElement				* root {_getRootNode (filename, doc) };
	vector <Graph>			graphs;
	Graph					elt;
	OGSS_String				str;

	root = _getNode (root, ParamNameMap.at (PTP_OUTPUT), true);

	root = root->FirstChildElement (ParamNameMap.at (PTP_GRAPH) .c_str () );
	while (root) {
		elt._filename = _getText (root);

		str = _getString (root, ParamNameMap.at (PTP_TYPE), true);
		auto findResult = find_if (GraphTypeNameMap.begin (), GraphTypeNameMap.end (),
			[&] (const pair <OGSS_GraphType, OGSS_String> & elt)
			{ return ! elt.second.compare (str); } );
		if (findResult != GraphTypeNameMap.end () )
			elt._type = findResult->first;
		else elt._type = GPT_TOTAL;

		str = _getString (root, ParamNameMap.at (PTP_FORMAT), true);
		auto findRes2 = find_if (GraphFormatNameMap.begin (), GraphFormatNameMap.end (),
			[&] (const pair <OGSS_GraphFormatType, OGSS_String> & elt)
			{ return ! elt.second.compare (str); } );
		if (findRes2 != GraphFormatNameMap.end () )
			elt._format = findRes2->first;
		else elt._format = GFM_TOTAL;

		switch (elt._type) {
			case GPT_REQRESP: elt._target = retrieveGroupTarget (root);
				elt._arg1 = _getLong (root, ParamNameMap.at (PTP_ARG1), true);
				elt._rules = retrieveRules (root);
				elt._arg2 = _getLong (root, ParamNameMap.at (PTP_ARG2), true);
				break;
			case GPT_DEVPROF: elt._target = retrieveTarget (root); break;
			case GPT_WAITTM:  elt._target = retrieveTarget (root);
				elt._arg1 = _getLong (root, ParamNameMap.at (PTP_ARG1), true);
				break;
			case GPT_REQCPLT: elt._target = retrieveTarget (root); break;
			case GPT_DEVTHRP: elt._target = retrieveGroupTarget (root); 
				elt._arg1 = _getLong (root, ParamNameMap.at (PTP_ARG1), true);
				break;
			default:
			break;
		};

		graphs.push_back (elt);
		root = root->NextSiblingElement (ParamNameMap.at (PTP_GRAPH) .c_str () );
	}

	return graphs;
}

vector <Event>
XMLParser::getEvents (
	const OGSS_String		& filename) {
	XMLDocument				doc;
	XMLElement				* root {_getRootNode (filename, doc) };
	vector <Event>			events;
	Event					elt;
	OGSS_String				str;

	root = _getNode (root, ParamNameMap.at (PTP_EVENT), true);

	root = root->FirstChildElement (ParamNameMap.at (PTP_ENTRY) .c_str () );
	while (root) {
		str = _getString (root, ParamNameMap.at (PTP_TYPE), true);
		auto findResult = find_if (EventTypeNameMap.begin (), EventTypeNameMap.end (),
			[&] (const pair <OGSS_EventType, OGSS_String> & elt)
			{ return ! elt.second.compare (str); } );
		if (findResult != EventTypeNameMap.end () )
			elt._type = findResult->first;
		else elt._type = EVT_TOTAL;

		elt._target = _getLong (root, ParamNameMap.at (PTP_TARGET), true);
		elt._date = _getReal (root, ParamNameMap.at (PTP_DATE), true);

		events.push_back (elt);
		root = root->NextSiblingElement (ParamNameMap.at (PTP_ENTRY) .c_str () );
	}

	return events;
}

OGSS_String
XMLParser::getComputationModel (
	const OGSS_String		& filename,
	const OGSS_ParamType	& paramType) {
	XMLDocument				doc;
	XMLElement				* root {_getRootNode (filename, doc) };

	root = _getNode (root, ParamNameMap.at (PTP_COMPUTATION), true);

	return _getString (root, ParamNameMap.at (paramType), false);
}

OGSS_Ulong
XMLParser::getCommunicationPort (
	const OGSS_String		& filename) {
	XMLDocument				doc;
	XMLElement				* root {_getRootNode (filename, doc) };

	root = _getNode (root, ParamNameMap.at (PTP_COMM), true);

	return _getLong (root, ParamNameMap.at (PTP_PORT) );
}

OGSS_CommType
XMLParser::getCommunicationType (
	const OGSS_String		& filename) {
	XMLDocument				doc;
	XMLElement				* root {_getRootNode (filename, doc) };
	OGSS_String				type;

	root = _getNode (root, ParamNameMap.at (PTP_COMM), true);

	type = _getString (root, ParamNameMap.at (PTP_TYPE), true);

	auto findResult = find_if (CommNameMap.begin (), CommNameMap.end (),
		[&] (const pair <OGSS_CommType, OGSS_String> & elt)
		{ return ! elt.second.compare (type); } );
	if (findResult != CommNameMap.end () )
		return findResult->first;

	return CTP_ZMQ;
}

tuple <OGSS_Bool, OGSS_String, OGSS_String>
XMLParser::getOGMDSInformation (
	const OGSS_String 		& configurationFile) {
	XMLDocument				doc;
	XMLElement				* root {_getRootNode (configurationFile, doc) };
	OGSS_Bool				on;
	OGSS_String				file;
	OGSS_String				path;

	root = _getNode (root, ParamNameMap.at (PTP_OGMD), true);

	on = _getBool (root, ParamNameMap.at (PTP_ON), true);
	path = _getString (root, ParamNameMap.at (PTP_PATH) );
	file = _getString (root, ParamNameMap.at (PTP_CONFIG) );

	return make_tuple (on, path, file);
}

OGSS_Bool
XMLParser::getListOfRequestedUnitaryInstances (
	const OGSS_String		& configurationFile,
	set <OGSS_ModuleType>	& uiNames) {
	XMLDocument				doc;
	XMLElement				* root {_getRootNode (configurationFile, doc) };

	root = _getNode (root, ParamNameMap.at (PTP_UTEST), true);

	root = root->FirstChildElement ();
	while (root) {
		if (root->Value () [0] != '#') {
			auto findResult = find_if (ModuleNameMap.begin (), ModuleNameMap.end (),
				[&] (const pair <OGSS_ModuleType, OGSS_String> & elt)
				{ return ! elt.second.compare (root->Value ()); } );
			if (findResult != ModuleNameMap.end () )
				uiNames.insert (findResult->first);
			else
				LOG (INFO) << "The name '" << root->Value () << "' does not match a known "
					<< "module name!";
		}

		root = root->NextSiblingElement ();
	}

	return true;
}

OGSS_Bool
XMLParser::getListOfRequestedUnitaryTests (
	const OGSS_String		& configurationFile,
	const OGSS_ModuleType	& uiName,
	set <OGSS_String>		& testNames) {
	XMLDocument				doc;
	XMLElement				* root {_getRootNode (configurationFile, doc) };

	root = _getNode (root, ParamNameMap.at (PTP_UTEST), true);
	root = _getNode (root, ModuleNameMap.at (uiName) );

	root = root->FirstChildElement ();

	// Check for <Module /> syntax
	if (! root)
		testNames.insert ("all");
	// Check for <Module><test>...</test></Module> syntax
	else {
		while (root) {
			if (root->Value () [0] != '#')
				testNames.insert (_getText (root) );
			root = root->NextSiblingElement ();
		}
	// Check for <Module>all</Module> syntax
		if (testNames.empty () )
			testNames.insert ("all");
	}

	return true;
}

#endif
