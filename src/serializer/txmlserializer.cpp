/*
 * Copyright UVSQ - CEA/DAM/DIF (2017)
 * contributeur : Sebastien GOUGEAUD	sebastien.gougeaud@uvsq.fr
 *                Soraya ZERTAL			soraya.zertal@uvsq.fr
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

//! \file	txmlserializer.cpp
//! \brief	Serializer for XML files. This is used by Unitary Test modules when
//! 		gathering the diverse scenario results.
//!
//! 		The serializer is made using Xercesc library.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <map>
#include <sstream>

#include "serializer/txmlserializer.hpp"

#if USE_TINYXML

using namespace std;
using namespace tinyxml2;

/*----------------------------------------------------------------------------*/
/* NAMESPACE FUNCTIONS -------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
TXMLSerializer::createXMLDocument (
	XMLDocument					& doc,
	const OGSS_String			& rootName) {
	XMLElement					* elt;

	elt = doc.NewElement ("rootName");
	doc.InsertEndChild (elt);
}

void
TXMLSerializer::addTestResult (
	XMLDocument					& document,
	const OGSS_ModuleType		& moduleType,
	const OGSS_String			& testDescription,
	const OGSS_Bool				& result,
	const OGSS_Real				& time) {
	OGSS_Bool					found = false;
	XMLElement					* cursor;
	XMLElement					* elt;

	cursor = document.FirstChildElement ();

	cursor = cursor->FirstChildElement ("testsuite");
	while (cursor) {
		if (cursor->Attribute ("classname", ModuleNameMap.at (moduleType) .c_str () ) ) {
			found = true;
			break;
		}

		cursor = cursor->NextSiblingElement ("testsuite");
	}

	if (!found) {
		elt = document.NewElement ("testsuite");
		elt->SetAttribute ("classname", ModuleNameMap.at (moduleType) .c_str () );

		cursor->InsertEndChild (elt);
		cursor = elt;
	}

	elt = document.NewElement ("testcase");
	elt->SetAttribute ("name", testDescription.c_str () );
	elt->SetAttribute ("time", time);

	cursor->InsertEndChild (elt);

	if (! result) {
		cursor = elt;
		elt = document.NewElement ("failure");

		cursor->InsertEndChild (elt);
	}
}

void
TXMLSerializer::writeXMLFile (
	XMLDocument					& document,
	const OGSS_String			& filePath) {
	document.SaveFile (filePath.c_str () );
}

#endif
