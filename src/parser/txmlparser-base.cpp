#include <sstream>
#include "parser/txmlparser.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

#if USE_TINYXML
#if USE_TINYXML_STATIC
#include "tinyxml2.h"
#else
#include <tinyxml2.h>
#endif

using namespace tinyxml2;
using namespace std;

namespace {
	XMLElement *
	_getRootNode (
		const OGSS_String				filepath,
		XMLDocument						& doc) {
		XMLElement *					rootNode;
		if (doc.LoadFile (filepath.c_str () ) )
//			throw OGS3_Exception (OGS3_Exception::ERR_Parser, doc.ErrorStr () );
			LOG(FATAL) << doc.ErrorStr ();

		rootNode = doc.FirstChildElement ();

		if (! rootNode)
//			throw OGS3_Exception (OGS3_Exception::ERR_Parser, "No root node found (<ogs3>)");
			LOG(FATAL) << "No root node found";

//		if (OGSS_String ("ogs3") .compare (rootNode->Value () ) )
//			throw OGS3_Exception (OGS3_Exception::ERR_Parser, "The root node is not valid (<ogs3>)");

		return rootNode;
	}

	XMLElement *
	_getNode (
		XMLElement						* start,
		const OGSS_String				nodename,
		const OGSS_Bool					recursive = false) {
		XMLElement						* node {start->FirstChildElement (nodename.c_str () ) };

		if (node) return node;
		if (! recursive) return nullptr;

		node = start->FirstChildElement ();
		while (node) {
			XMLElement					* child {_getNode (node, nodename, true) };
			if (child) return child;
			node = node->NextSiblingElement ();
		}

		return nullptr;
	}

	OGSS_String
	_getValue (
		XMLElement						* node,
		const OGSS_String				attribute) {
		if (! node || ! node->Attribute (attribute.c_str () ) ) return "";
		return node->Attribute (attribute.c_str () );
	}

	OGSS_String
	_getText (
		XMLElement						* node) {
		if (! node || ! node->FirstChild () || ! node->FirstChild () ->ToText () ) return "";
		return node->FirstChild () ->ToText () ->Value ();
	}

	OGSS_Ulong
	_getLong (
		XMLElement						* node,
		const OGSS_String				name,
		const OGSS_Bool					attribute = false) {
		OGSS_String						res {""};

		if (attribute)
			res = _getValue (node, name);
		else {
			node = _getNode (node, name);
			if (node)
				res = _getText (node);
		}

		if (! res.length () ) return 0;
		return atol (res.c_str () );
	}

	OGSS_Real
	_getReal (
		XMLElement						* node,
		const OGSS_String				name,
		const OGSS_Bool					attribute = false) {
		OGSS_String						res {""};

		if (attribute)
			res = _getValue (node, name);
		else {
			node = _getNode (node, name);
			if (node)
				res = _getText (node);
		}

		if (! res.length () ) return 0;
		return atof (res.c_str () );
	}

	OGSS_String
	_getString (
		XMLElement						* node,
		const OGSS_String				name,
		const OGSS_Bool					attribute = false) {
		OGSS_String						res {""};

		if (attribute)
			res = _getValue (node, name);
		else {
			node = _getNode (node, name);
			if (node)
				res = _getText (node);
		}

		return res;
	}

	OGSS_Bool
	_getBool(
		XMLElement						* node,
		const OGSS_String				name,
		const OGSS_Bool					attribute = false) {
		OGSS_String						res {""};

		if (attribute)
			res = _getValue (node, name);
		else {
			node = _getNode (node, name);
			if (node)
				res = _getText (node);
		}

		return res.compare ("on") || res.compare ("yes") || res.compare ("true");
	}

	OGSS_Ulong
	_getLongPrefix (
		XMLElement						* node,
		const OGSS_ParamType			name,
		const OGSS_Bool					attribute = false) {
		OGSS_Ulong						res;
		std::istringstream				iss;
		char							prefix {' '};

		iss.str (_getString (node, ParamNameMap.at (name), attribute) );

		iss >> res >> prefix;

		if (prefix == 'K')		res *= KILO;
		else if (prefix == 'M')	res *= MEGA;
		else if (prefix == 'G')	res *= GIGA;
		else if (prefix == 'T')	res *= TERA;

		return res;
	}

	OGSS_Real
	_getRealPrefix (
		XMLElement						* node,
		const OGSS_ParamType			name,
		const OGSS_Bool					attribute = false) {
		OGSS_Real						res;
		std::istringstream				iss;
		char							prefix = ' ';

		iss.str (_getString (node, ParamNameMap.at (name), attribute) );

		iss >> res >> prefix;

		if (prefix == 'K')		res *= KILO;
		else if (prefix == 'M')	res *= MEGA;
		else if (prefix == 'G')	res *= GIGA;
		else if (prefix == 'T')	res *= TERA;
		else if (prefix == 'm')	res *= MILLI;
		else if (prefix == 'u')	res *= MICRO;
		else if (prefix == 'n')	res *= NANO;

		return res;
	}

	OGSS_Real
	_getRealMeasure (
		XMLElement						* root,
		const OGSS_ParamType			name) {
		XMLElement						* node;
		OGSS_Ulong						size;
		OGSS_String						unit;

		node = _getNode (root, ParamNameMap.at (name));

		unit = _getString (node, "unit", true);

		if (! unit.compare ("bandwidth") ) {
			return 1 / _getRealPrefix (root, name);

		} else if (! unit.compare ("iops") ) {
			size = _getLongPrefix (node, PTP_SIZE, true);
			return 1 / (_getRealPrefix (root, name) * size);

		} else if (! unit.compare ("time") ) {
			return _getRealPrefix (root, name);
		}
//		throw OGS3_Exception (OGS3_Exception::ERR_Parser, "Unit not recognized");
		LOG(FATAL) << "Unit not recognized";

		return .0;
	}
};

#endif
