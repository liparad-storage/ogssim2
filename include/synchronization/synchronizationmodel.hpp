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

//! \file	synchronizationmodel.hpp
//! \brief	Definition of the synchronization model.

#ifndef _OGSS_SYNCHRONIZATIONMODEL_HPP_
#define _OGSS_SYNCHRONIZATIONMODEL_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <memory>

#include "communication/communicationinterface.hpp"

#include "structure/request.hpp"
#include "structure/requeststat.hpp"

//! \brief	Synchronization model interface.
class SynchronizationModel {
public:
	
/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Destructor.
	virtual ~SynchronizationModel () = default;

//! \brief	Add a request in the model, following its reception from the
//!			execution module.
	virtual void addEntry (
		Request					&req) = 0;

//! \brief	Once all the requests are received, start processing the
//!			synchronization which computes request waiting times by determining
//!			the correct request scheduling.
	virtual void process () = 0;

//! \brief	Send the request stats to the evaluation module.
//! \param	stat				Request stats.
	virtual void sendStat (
		RequestStat				stat);

//! \brief	Generation of the detailed output file.
//! \param	outputFile			Path to the detailed output file.
	virtual void createOutputFile (
		const OGSS_String		outputFile) = 0;

//! \brief	Generation of the resume output file.
//! \param	resumeFile			Path to the resume file.
	virtual void createResumeFile (
		const OGSS_String		resumeFile) = 0;

protected:

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	ci					Communication interface from the synchronization module.
	SynchronizationModel (
		std::shared_ptr <CommunicationInterface> ci);

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	std::shared_ptr <CommunicationInterface>
								_ci;				//!< Communication interface.
};

#endif
