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

//! \file	unitarytest.tpp
//! \brief	Interface of unitary test add-ons. The goal of this module add-on is
//! 		to validate the behavior of a module by executing a series of
//! 		scenario. Each scenario will return <b>TRUE</b> in case of success,
//! 		and <b>FALSE</b> if the test failed.
//! 
//! 		The result of each scenario is written in the log file. It can also
//! 		be output in XML JUnit format by providing the <b><unitarytest></b>
//! 		tag in the <b><path></b> section of the configuration file.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "util/chrono.hpp"

#if USE_TINYXML
#include "serializer/txmlserializer.hpp"
#else
#include "serializer/xmlserializer.hpp"
#endif

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

/*----------------------------------------------------------------------------*/
/* MEMBER FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template <typename T>
UnitaryTest <T>::UnitaryTest (
	const OGSS_ModuleType		& module)
	{ _module = module; }

template <typename T>
UnitaryTest <T>::~UnitaryTest () {  }

template <typename T>
void
UnitaryTest <T>::executeTests () {
	T*							p = dynamic_cast <T*> (this);
	OGSS_Bool					res;
	Chrono						chr;

	for (auto & func: _tests) {
		chr.tick ();
		res = (*p.*(func.second) ) ();
		chr.tick ();
		chr.restart ();

		if (res)
			LOG (INFO) << "\e[1;32m[" << ModuleNameMap.at (_module) << "] "
				<< func.first << ": OK!\e[0m";
		else
			LOG (ERROR) << "\e[1;31m[" << ModuleNameMap.at (_module) << "] "
				<< func.first << ": NOT OK!\e[0m";

		if (_launched)
#if USE_TINYXML
			TXMLSerializer::addTestResult (_document, _module, func.first, res, chr.get () );
#else
			XMLSerializer::addTestResult (_document, _module, func.first,
				res, chr.get () );
#endif
	}
}
