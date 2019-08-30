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

//! \file	unitarytest.hpp
//! \brief	Interface of unitary test add-ons. The goal of this module add-on is
//! 		to validate the behavior of a module by executing a series of
//! 		scenario. Each scenario will return <b>TRUE</b> in case of success,
//! 		and <b>FALSE</b> if the test failed.
//! 
//! 		The result of each scenario is written in the log file. It can also
//! 		be output in XML JUnit format by providing the <b>unitarytest</b>
//! 		tag in the <b>path</b> section of the configuration file.

#ifndef _OGSS_UNITARYTEST_HPP_
#define _OGSS_UNITARYTEST_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <string>
#include <vector>

#include "structure/types.hpp"

#if USE_TINYXML
#if USE_TINYXML_STATIC
#include "tinyxml2.h"
#else
#include <tinyxml2.h>
#endif
#else
#include <xercesc/dom/DOM.hpp>
#endif

/*----------------------------------------------------------------------------*/
/* UNITARY INTERFACE ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Interface of unitary test, allow to build a container of UnitaryTest
//! 		instances. It also contains initialization and finalization
//! 		processes to generate the unitary test context.
class UnitaryInterface {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC MEMBER FUNCTIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Initalization function. Create the XML document which will be filled
//! 		with unitary test results. Called at the start of the program.
//! \param	filePath			Output file path.
	static void init (
		const OGSS_String		& filePath = "");

//! \brief	Finalization function. Serialize the XML document into the output
//! 		file provided in the initialization function. Called at the end of
//! 		the program.
	static void finalize ();

//! \brief	Destructor.
	virtual ~UnitaryInterface () {  };

//! \brief	Execute all the requested scenarios.
	virtual void executeTests () = 0;

protected:

/*----------------------------------------------------------------------------*/
/* STATIC ATTRIBUTES ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	static OGSS_Bool			_launched;
		//!< Indicate if the initialization function was called.
	static OGSS_String			_filePath;
		//!< Output file path.
#if USE_TINYXML
	static tinyxml2::XMLDocument  _document;
#else
	static xercesc::DOMDocument	* _document;
#endif
		//!< XML document to fill.
};


/*----------------------------------------------------------------------------*/
/* UNITARY TEST --------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Parent class of unitary test instance. It must exist for each module
//! 		a dedicated child class of UnitaryTest. The template T is the child
//! 		class of the instance.
//!
//! 		The way to use it is: first instaciate a child class, then use the
//! 		executeTests function to execute the scenarios.
template <typename T>
class UnitaryTest:
public UnitaryInterface {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC MEMBER FUNCTIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Destructor.
	virtual ~UnitaryTest ();

//! \brief	Execute all the requested scenarios.
	void executeTests ();

protected:

/*----------------------------------------------------------------------------*/
/* PRIVATE MEMBER FUNCTIONS --------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor. Fill the module field.
//! \param	module				Type of the tested module.
	UnitaryTest (
		const OGSS_ModuleType	& module = MTP_TOTAL);

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	OGSS_ModuleType				_module;
		//!< Type of the tested module.
	std::vector <std::pair <OGSS_String, OGSS_Bool (T::*) () > >
								_tests;
		//!< Container for the scenarios. Each scenario is paired with a brief
		//!< description of the test.
};

#include "util/unitarytest.tpp"

#endif
