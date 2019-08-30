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

//! \file			module.hpp
//!	\brief			Definition of the module interface, main thread owner.

#ifndef _OGSS_MODULE_HPP_
#define _OGSS_MODULE_HPP_

/*--------------------------------------------------------------------------------------------------------------------*/
/*--  HEADERS  -------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include "structure/types.hpp"

#include "communication/communicationinterface.hpp"

#include <memory>

/*--------------------------------------------------------------------------------------------------------------------*/
/*--  MAIN CLASS  ----------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Interface for a module class. A module is defined as a thread owner in OGSSim, and will take in
//!					charge a process during the simulation. This process is composed of three steps which can be empty:
//!					an extraction step, where the data are collected from the input files and transmitted to the other
//!					modules ; a decomposition step, where the requests are translated from logical to physical ones ;
//!					and a synchronisation step, where the requests are well-scheduled and their final response time are
//!					computed.
class Module {
public:

/*--  PUBLIC MEMBER FUNCTIONS  ---------------------------------------------------------------------------------------*/

//! \brief			Virtual destructor.
	virtual ~Module ();

//! \brief			Process during the extraction step.
	virtual void processExtraction () = 0;

//! \brief			Process during the decomposition step.
	virtual void processDecomposition () = 0;

//! \brief			Process during the synchronization step.
	virtual void processSynchronization () = 0;

//! \brief			Barrier, where all the modules are waiting each other, used between the different steps.
	void barrier ();

protected:

/*--  PRIVATE MEMBER FUNCTIONS  --------------------------------------------------------------------------------------*/

//! \brief			Protected constructor.
//! \param			configurationFile	Configuration file.
//! \param			id					Module identifier.
	Module (
		const OGSS_String				configurationFile,
		const OGSS_Interlocutor			id);

/*--  PRIVATE ATTRIBUTES  --------------------------------------------------------------------------------------------*/

	OGSS_Interlocutor					_id;				//!< Module identifier.
	OGSS_String							_cfg;				//!< Configuration file.
	std::shared_ptr <CommunicationInterface>
										_ci;				//!< Communication interface.
};

#endif
