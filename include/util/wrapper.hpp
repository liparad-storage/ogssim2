/*
 * Copyright UVSQ - CEA/DAM/DIF (2017-2019)
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

//! \file	wrapper.hpp
//! \brief	Contains all the functions needed at the initialization by OGSSim.

#ifndef _OGSS_WRAPPER_HPP_
#define _OGSS_WRAPPER_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/types.hpp"

/*----------------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Prepare the launcher for the MPI version of OGSSim.
//! \param	configurationFile	Path to the configuration file.
void prepareMPILauncher (
	const OGSS_String			configurationFile);

//! \brief	Initialize OGSSim when running with MPI.
//! \param	configurationFile	Path to the configuration file.
void MPILauncher (
	const OGSS_String			configurationFile);

//! \brief	Initialize OGSSim when running with C++ standard threads.
//! \param	configurationFile	Path to the configuration file.
void threadLauncher (
	const OGSS_String			configurationFile);

//! \brief  Launch the OGMDSim add-on to retrieve the request file.
//! \param  configurationFile	Path to the configuration file.
void OGMDSimLauncher (
	const OGSS_String			pathToExe,
	const OGSS_String			configurationFile);

#endif
