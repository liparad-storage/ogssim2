/*
 * Copyright UVSQ - CEA/DAM/DIF (2018)
 * Contributors:		Sebastien GOUGEAUD  					----						  sebastien.gougeaud@uvsq.fr
 *              		Soraya ZERTAL       					----							   soraya.zertal@uvsq.fr
 *
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General
 * Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

//! \file			module.cpp
//!	\brief			Definition of the module interface, main thread owner.

/*--------------------------------------------------------------------------------------------------------------------*/
/*--  HEADERS  -------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include "module/module.hpp"

#include "util/launcher.hpp"

/*--------------------------------------------------------------------------------------------------------------------*/
/*--  MAIN CLASS  ----------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

/*--  PUBLIC MEMBER FUNCTIONS  ---------------------------------------------------------------------------------------*/

Module::~Module () {  }

void
Module::barrier () {
#ifndef UTEST
	_ci->requestFullBarrier ();
#endif
}

/*--  PRIVATE MEMBER FUNCTIONS  --------------------------------------------------------------------------------------*/

Module::Module (
	const OGSS_String					configurationFile,
	const OGSS_Interlocutor				id):
	_id (id), _cfg (configurationFile) {
#ifndef UTEST
	_ci = instanciateCommunicationInterface (_cfg, _id);
#endif
}
