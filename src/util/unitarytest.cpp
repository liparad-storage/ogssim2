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

//! \file	unitarytest.cpp
//! \brief	Interface of unitary test add-ons. The goal of this module add-on is
//! 		to validate the behavior of a module by executing a series of
//! 		scenario. Each scenario will return <b>TRUE</b> in case of success,
//! 		and <b>FALSE</b> if the test failed.
//! 
//! 		The result of each scenario is written in the log file. It can also
//! 		be output in XML JUnit format by providing the <b>unitarytest</b>
//! 		tag in the <b>path</b> section of the configuration file.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "util/unitarytest.hpp"

/*----------------------------------------------------------------------------*/
/* STATIC ATTRIBUTES ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

OGSS_Bool UnitaryInterface::_launched = false;
OGSS_String UnitaryInterface::_filePath = "";

#if USE_TINYXML
tinyxml2::XMLDocument UnitaryInterface::_document;
#else
xercesc::DOMDocument * UnitaryInterface::_document = NULL;
#endif

/*----------------------------------------------------------------------------*/
/* MEMBER FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
UnitaryInterface::init (
	const OGSS_String			& filePath) {
	if (filePath.compare ("") ) {
		_launched = true;
		_filePath = filePath;
#if USE_TINYXML
		TXMLSerializer::createXMLDocument (_document, "testsuites");
#else
		_document = XMLSerializer::createXMLDocument ("testsuites");
#endif
	}
	else _launched = false;
}

void
UnitaryInterface::finalize () {
	if (_launched)
#if USE_TINYXML
		TXMLSerializer::writeXMLFile (_document, _filePath);
#else
		XMLSerializer::writeXMLFile (_document, _filePath);
#endif
	_launched = false;
}
