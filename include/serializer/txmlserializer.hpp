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

//! \file	txmlserializer.hpp
//! \brief	Serializer for XML files. This is used by Unitary Test modules when
//! 		gathering the diverse scenario results.
//!
//! 		The serializer is made using Xercesc library.

#ifndef _OGSS_TXMLSERIALIZER_HPP_
#define _OGSS_TXMLSERIALIZER_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <memory>

#include "structure/types.hpp"

#if USE_TINYXML
#if USE_TINYXML_STATIC
#include "tinyxml2.h"
#else
#include <tinyxml2.h>
#endif

/*----------------------------------------------------------------------------*/
/* NAMESPACE -----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Namespace of the XML serializer.
namespace TXMLSerializer {

//! \brief	XML document creation.
//! \param	rootName			Root node name.
//! \return						XML document (used by the namespace functions).
	void createXMLDocument (
		tinyxml2::XMLDocument	& document,
		const OGSS_String		& rootName);

//! \brief	Add the results of a given test in an XML document.
//! \param	document			XML document.
//! \param	moduleType			Module which conduct the test.
//! \param	testDescription		Brief test description.
//! \param	result				Test results: success or failure.
//! \param	time				Execution time of the test in microseconds.
	void addTestResult (
		tinyxml2::XMLDocument	& document,
		const OGSS_ModuleType	& moduleType,
		const OGSS_String		& testDescription,
		const OGSS_Bool			& result,
		const OGSS_Real			& time);

//! \brief	XML file creation.
//! \param	document			XML document.
//! \param	filePath			Path of the XML file.
	void writeXMLFile (
		tinyxml2::XMLDocument	& document,
		const OGSS_String		& filePath);
};

#endif
#endif
