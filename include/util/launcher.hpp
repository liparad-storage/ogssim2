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

//! \file	launcher.hpp
//! \brief	Contains all the launchers used by the main process of OGSSim.

#ifndef _OGSS_LAUNCHER_HPP_
#define _OGSS_LAUNCHER_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/types.hpp"

#include "communication/communicationinterface.hpp"
#include "module/module.hpp"

#include <memory>

/*----------------------------------------------------------------------------*/
/* LAUNCHERS -----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Instanciate the communication interface for the requested module.
//! 		This will bind the module with the communication manager.
//! \param	configurationFile	Path to the configuration file.
//! \param	interlocutor		Module which needs the interface.
//! \return						Communication interface.
std::shared_ptr <CommunicationInterface> instanciateCommunicationInterface (
	const OGSS_String			configurationFile,
	const OGSS_Interlocutor		interlocutor);

//! \brief	Launch the communication manager process, depending on which
//! 		library the user requests.
//! \param	configurationFile	Path to the configuration file.
void launchCommunicationManager (
	const OGSS_String			configurationFile);

//! \brief	Launch a given module, providing the main configuration file and an
//!			argument. This argument is an ID needed by the drivers to
//!			differenciate them.
//! \param	configurationFile	Path to the configuration file.
//! \param	arg					Argument.
template <class M>
void launchModule (
	const OGSS_String			configurationFile,
	const OGSS_Ushort			arg);

//! \brief	Launch a given module, providing the main configuration file.
//! \param	configurationFile	Path to the configuration file.
template <class M>
void launchModule (
	const OGSS_String			configurationFile);

//! \brief	Launch the unitary tests requested in the configuration file.
//! \param	configurationFile	Path to the configuration file.
void launchUnitaryTests (
	const OGSS_String			configurationFile);

/*----------------------------------------------------------------------------*/
/* TEMPLATE FUNCTIONS --------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

template <class M>
void
launchModule (
	const OGSS_String					configurationFile,
	const OGSS_Ushort					arg) {
	std::unique_ptr <Module>			m {std::make_unique <M> (configurationFile, arg) };

	m->barrier ();
	m->processExtraction ();
	m->barrier ();
	m->processDecomposition ();
	m->barrier ();
	m->processSynchronization ();
	m->barrier ();
}

template <class M>
void
launchModule (
	const OGSS_String					configurationFile) {
	std::unique_ptr <Module>			m {std::make_unique <M> (configurationFile) };

	m->barrier ();
	m->processExtraction ();
	m->barrier ();
	m->processDecomposition ();
	m->barrier ();
	m->processSynchronization ();
	m->barrier ();
}

#endif
