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

//! \file			xmlextract.cpp
//! \brief			Definition of the XML parser class. This class is used to read the system and OGMDSim configuration
//!					files.

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include "parser/xmlextract.hpp"

using namespace std;
using namespace tinyxml2;

/*--------------------------------------------------------------------------------------------------------------------*/
/* STATIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

queue <string>
splitString (
	const string						path,
	const char							del) {
	queue <string>						q;
	int									a {0}, b;

	if (path.empty () ) return q;

	while ( (b = path.find (del, a) ) != string::npos) {
		q.push (path.substr (a, b-a) );
		a = b + 1;
	}

	q.push (path.substr (a) );

	return q;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

OGXML::OGXML (
	const string						cfgPath):
	_cfgPath (cfgPath) {
	_cfgDoc.LoadFile (_cfgPath.c_str () );
}

OGXML::~OGXML () {
	
}

template <> bool
OGXML::getXMLItem (
	bool								& value,
	const OGFileType					file,
	const string						path,
	const bool							attribute) {
	XMLElement							* elt {nullptr};
	XMLAttribute						* att {nullptr};
	auto								q {splitString (path, '/')};

	if (! (elt = goToLastNode (q, file, path, attribute) ) ) return false;

	if (attribute)
		return elt->QueryBoolAttribute (q.front () .c_str (), &value) == XML_SUCCESS;
	else
		return elt->QueryBoolText (&value) == XML_SUCCESS;
}

template <> bool
OGXML::getXMLItem (
	double								& value,
	const OGFileType					file,
	const string						path,
	const bool							attribute) {
	XMLElement							* elt {nullptr};
	XMLAttribute						* att {nullptr};
	auto								q {splitString (path, '/')};

	if (! (elt = goToLastNode (q, file, path, attribute) ) ) return false;

	if (attribute)
		return elt->QueryDoubleAttribute (q.front () .c_str (), &value) == XML_SUCCESS;
	else
		return elt->QueryDoubleText (&value) == XML_SUCCESS;
}

template <> bool
OGXML::getXMLItem (
	int									& value,
	const OGFileType					file,
	const string						path,
	const bool							attribute) {
	XMLElement							* elt {nullptr};
	XMLAttribute						* att {nullptr};
	auto								q {splitString (path, '/')};

	if (! (elt = goToLastNode (q, file, path, attribute) ) ) return false;

	if (attribute)
		return elt->QueryIntAttribute (q.front () .c_str (), &value) == XML_SUCCESS;
	else
		return elt->QueryIntText (&value) == XML_SUCCESS;
}

template <> bool
OGXML::getXMLItem (
	int64_t								& value,
	const OGFileType					file,
	const string						path,
	const bool							attribute) {
	XMLElement							* elt {nullptr};
	XMLAttribute						* att {nullptr};
	auto								q {splitString (path, '/')};

	if (! (elt = goToLastNode (q, file, path, attribute) ) ) return false;

	if (attribute)
		return elt->QueryInt64Attribute (q.front () .c_str (), &value) == XML_SUCCESS;
	else
		return elt->QueryInt64Text (&value) == XML_SUCCESS;
}

template <> bool
OGXML::getXMLItem (
	string								& value,
	const OGFileType					file,
	const string						path,
	const bool							attribute) {
	XMLElement							* elt {nullptr};
	XMLAttribute						* att {nullptr};
	auto								q {splitString (path, '/')};

	if (! (elt = goToLastNode (q, file, path, attribute) ) ) return false;

	if (attribute) {
		auto							t {elt->Attribute (q.front () .c_str () ) };
		if (! t) return false;
		value = t;
	}
	else
		value = elt->GetText ();

	return true;
}

template <> bool
OGXML::getXMLItem (
	unsigned							& value,
	const OGFileType					file,
	const string						path,
	const bool							attribute) {
	XMLElement							* elt {nullptr};
	XMLAttribute						* att {nullptr};
	auto								q {splitString (path, '/')};

	if (! (elt = goToLastNode (q, file, path, attribute) ) ) return false;

	if (attribute)
		return elt->QueryUnsignedAttribute (q.front () .c_str (), &value) == XML_SUCCESS;
	else
		return elt->QueryUnsignedText (&value) == XML_SUCCESS;
}

template <> bool
OGXML::getXMLItem (
	uint64_t							& value,
	const OGFileType					file,
	const string						path,
	const bool							attribute) {
	XMLElement							* elt {nullptr};
	XMLAttribute						* att {nullptr};
	auto								q {splitString (path, '/')};
	int64_t								v;

	if (! (elt = goToLastNode (q, file, path, attribute) ) ) return false;

	if (attribute) {
		auto t {elt->QueryInt64Attribute (q.front () .c_str (), &v) == XML_SUCCESS};
		if (!t) return t;
		value = static_cast <uint64_t> (v);
		return t;
	}
	else {
		auto t {elt->QueryInt64Text (&v) == XML_SUCCESS};
		if (!t) return t;
		value = static_cast <uint64_t> (v);
		return t;
	}
}

template <> bool
OGXML::getXMLList (
	vector <bool>						& vec,
	const OGFileType					file,
	const string						path) {
	XMLElement							* elt {nullptr};
	auto								q {splitString (path, '/')};
	queue <string>						qprim;
	bool								value;

	while (q.size () != 1) {
		qprim.push (q.front () );
		q.pop ();
	}

	if (! (elt = goToLastNode (qprim, file, path, false) ) ) return false;

	elt = elt->FirstChildElement (q.front () .c_str () );
	while (elt) {
		if (elt->QueryBoolText (&value) == XML_SUCCESS)
			vec.push_back (value);
		elt = elt->NextSiblingElement (q.front () .c_str () );
	}

	return true;
}

template <> bool
OGXML::getXMLList (
	vector <double>						& vec,
	const OGFileType					file,
	const string						path) {
	XMLElement							* elt {nullptr};
	auto								q {splitString (path, '/')};
	queue <string>						qprim;
	double								value;

	while (q.size () != 1) {
		qprim.push (q.front () );
		q.pop ();
	}

	if (! (elt = goToLastNode (qprim, file, path, false) ) ) return false;

	elt = elt->FirstChildElement (q.front () .c_str () );
	while (elt) {
		if (elt->QueryDoubleText (&value) == XML_SUCCESS)
			vec.push_back (value);
		elt = elt->NextSiblingElement (q.front () .c_str () );
	}

	return true;
}

template <> bool
OGXML::getXMLList (
	vector <int>						& vec,
	const OGFileType					file,
	const string						path) {
	XMLElement							* elt {nullptr};
	auto								q {splitString (path, '/')};
	queue <string>						qprim;
	int									value;

	while (q.size () != 1) {
		qprim.push (q.front () );
		q.pop ();
	}

	if (! (elt = goToLastNode (qprim, file, path, false) ) ) return false;

	elt = elt->FirstChildElement (q.front () .c_str () );
	while (elt) {
		if (elt->QueryIntText (&value) == XML_SUCCESS)
			vec.push_back (value);
		elt = elt->NextSiblingElement (q.front () .c_str () );
	}

	return true;
}

template <> bool
OGXML::getXMLList (
	vector <int64_t>					& vec,
	const OGFileType					file,
	const string						path) {
	XMLElement							* elt {nullptr};
	auto								q {splitString (path, '/')};
	queue <string>						qprim;
	int64_t								value;

	while (q.size () != 1) {
		qprim.push (q.front () );
		q.pop ();
	}

	if (! (elt = goToLastNode (qprim, file, path, false) ) ) return false;

	elt = elt->FirstChildElement (q.front () .c_str () );
	while (elt) {
		if (elt->QueryInt64Text (&value) == XML_SUCCESS)
			vec.push_back (value);
		elt = elt->NextSiblingElement (q.front () .c_str () );
	}

	return true;
}

template <> bool
OGXML::getXMLList (
	vector <string>						& vec,
	const OGFileType					file,
	const string						path) {
	XMLElement							* elt {nullptr};
	auto								q {splitString (path, '/')};
	queue <string>						qprim;

	while (q.size () != 1) {
		qprim.push (q.front () );
		q.pop ();
	}

	if (! (elt = goToLastNode (qprim, file, path, false) ) ) return false;

	elt = elt->FirstChildElement (q.front () .c_str () );
	while (elt) {
		vec.push_back (elt->GetText () );
		elt = elt->NextSiblingElement (q.front () .c_str () );
	}

	return true;
}

template <> bool
OGXML::getXMLList (
	vector <unsigned>					& vec,
	const OGFileType					file,
	const string						path) {
	XMLElement							* elt {nullptr};
	auto								q {splitString (path, '/')};
	queue <string>						qprim;
	unsigned							value;

	while (q.size () != 1) {
		qprim.push (q.front () );
		q.pop ();
	}

	if (! (elt = goToLastNode (qprim, file, path, false) ) ) return false;

	elt = elt->FirstChildElement (q.front () .c_str () );
	while (elt) {
		if (elt->QueryUnsignedText (&value) == XML_SUCCESS)
			vec.push_back (value);
		elt = elt->NextSiblingElement (q.front () .c_str () );
	}

	return true;
}

template <> bool
OGXML::getXMLList (
	vector <uint64_t>					& vec,
	const OGFileType					file,
	const string						path) {
	XMLElement							* elt {nullptr};
	auto								q {splitString (path, '/')};
	queue <string>						qprim;
	uint64_t							value;
	int64_t								v;

	while (q.size () != 1) {
		qprim.push (q.front () );
		q.pop ();
	}

	if (! (elt = goToLastNode (qprim, file, path, false) ) ) return false;

	elt = elt->FirstChildElement (q.front () .c_str () );
	while (elt) {
		if (elt->QueryInt64Text (&v) == XML_SUCCESS) {
			value = static_cast <uint64_t> (v);
			vec.push_back (value);
		}
		elt = elt->NextSiblingElement (q.front () .c_str () );
	}

	return true;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS -------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

XMLElement *
OGXML::goToLastNode (
	queue <string>						& q,
	const OGFileType					file,
	const string						path,
	const bool							attribute) {
	XMLElement							* elt {nullptr};
	switch (file) {
		case OGFT_CFGFILE: elt = _cfgDoc.FirstChildElement (); break;
		default: DLOG(WARNING) << "File type is not recognized (using gXI)"; return nullptr;
	}

	while (! q.empty () ) {
		if (attribute && q.size () == 1)
			break;

		auto							s {q.front () };

		if (s.find ('@') != string::npos) {
			auto						s1 {s.substr (0, s.find ('@') ) };
			auto						s2 {s.substr (s.find ('@') + 1) };
			elt = elt->FirstChildElement (s1.c_str () );
			while (elt) {
				auto t {elt->Attribute ("id") };
				if (t && ! s2.compare (t) )
					break;

				elt = elt->NextSiblingElement (s1.c_str () );
			}
		} else
			elt = elt->FirstChildElement (q.front () .c_str () );
		
		q.pop ();

		if (! elt) {
			DLOG(WARNING) << "The XML tag '" << path << "' was not found in the requested file";
			return nullptr;
		}
	}

	return elt;
}
