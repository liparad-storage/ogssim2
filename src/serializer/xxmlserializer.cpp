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

//! \file	xxmlserializer.cpp
//! \brief	Serializer for XML files. This is used by Unitary Test modules when
//! 		gathering the diverse scenario results.
//!
//! 		The serializer is made using Xercesc library.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <map>
#include <sstream>

#include "serializer/xxmlserializer.hpp"

#if ! USE_TINYXML

#include <xercesc/framework/LocalFileFormatTarget.hpp>

using namespace std;
using namespace xercesc;

/*----------------------------------------------------------------------------*/
/* MACROS --------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Xercesc string getter.
#define _DOM_GET_STRING(elt)	XMLString::transcode(elt)
//! \brief	Xercesc string destructor.
#define _DOM_FREE_STRING(elt)	XMLString::release(&elt)

/*----------------------------------------------------------------------------*/
/* NAMESPACE FUNCTIONS -------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

DOMDocument *
XMLSerializer::createXMLDocument (
	const OGSS_String			& rootName) {
	XMLCh						* tmp;
	DOMImplementation			* impl;
	DOMLSSerializer				* writer;
	DOMLSOutput					* output;
	DOMDocument					* document;
	XMLFormatTarget				* file;

	tmp = _DOM_GET_STRING ("core");
	impl = DOMImplementationRegistry::getDOMImplementation (tmp);
	_DOM_FREE_STRING (tmp);

	tmp = _DOM_GET_STRING (rootName.c_str () );
	document = impl->createDocument (0, tmp, NULL);
	_DOM_FREE_STRING (tmp);

	return document;
}

void
XMLSerializer::addTestResult (
	DOMDocument					* document,
	const OGSS_ModuleType		& moduleType,
	const OGSS_String			& testDescription,
	const OGSS_Bool				& result,
	const OGSS_Real				& time) {
	OGSS_Bool					found = false;
	ostringstream				oss ("");
	XMLCh						* name;
	XMLCh						* value;
	DOMElement					* cursor;
	DOMElement					* elt;
	DOMNamedNodeMap				* attributes;
	DOMNodeList					* tmp;
	char						* src;
	cursor = document->getDocumentElement ();

	name = _DOM_GET_STRING ("testsuite");

	tmp = cursor->getElementsByTagName (name);

	for (auto idx = 0; idx < tmp->getLength (); ++idx) {
		attributes = tmp->item (idx) -> getAttributes ();
		src = _DOM_GET_STRING (attributes->item (0) ->getNodeValue () );
		if (! ModuleNameMap.at (moduleType) .compare (src) ) {
			cursor = (DOMElement *) tmp->item (idx);
			found = true;
		}
		_DOM_FREE_STRING (src);
	}

	if (!found) {
		elt = document->createElement (name);
		_DOM_FREE_STRING (name);

		name = _DOM_GET_STRING ("classname");
		value = _DOM_GET_STRING (ModuleNameMap.at (moduleType) .c_str () );
		elt->setAttribute (name, value);
		_DOM_FREE_STRING (value);

		cursor->appendChild (elt);
		cursor = elt;
	}

	_DOM_FREE_STRING (name);

	name = _DOM_GET_STRING ("testcase");
	elt = document->createElement (name);
	_DOM_FREE_STRING (name);

	name = _DOM_GET_STRING ("name");
	value = _DOM_GET_STRING (testDescription.c_str () );
	elt->setAttribute (name, value);
	_DOM_FREE_STRING (name);
	_DOM_FREE_STRING (value);

	oss << time;
	name = _DOM_GET_STRING ("time");
	value = _DOM_GET_STRING (oss.str () .c_str () );
	elt->setAttribute (name, value);
	_DOM_FREE_STRING (name);
	_DOM_FREE_STRING (value);

	cursor->appendChild (elt);

	if (! result) {
		cursor = elt;

		name = _DOM_GET_STRING ("failure");
		elt = document->createElement (name);
		_DOM_FREE_STRING (name);

		cursor->appendChild (elt);
	}
}

void
XMLSerializer::writeXMLFile (
	DOMDocument					* document,
	const OGSS_String			& filePath) {
	XMLCh						* tmp;
	DOMImplementation			* impl;
	DOMLSSerializer				* writer;
	DOMLSOutput					* output;
	XMLFormatTarget				* file;

	tmp = _DOM_GET_STRING ("LS");
	impl = DOMImplementationRegistry::getDOMImplementation (tmp);
	_DOM_FREE_STRING (tmp);

	writer = impl->createLSSerializer ();

	if (writer->getDomConfig () ->canSetParameter (
		XMLUni::fgDOMWRTFormatPrettyPrint, true) )
		writer->getDomConfig () ->setParameter(
			XMLUni::fgDOMWRTFormatPrettyPrint, true);

	file = new LocalFileFormatTarget (filePath.c_str () );
	output = impl->createLSOutput ();
	output->setByteStream (file);
	writer->write (document, output);

	document->release ();
	output->release ();
	writer->release ();

	delete file;
}

#endif
