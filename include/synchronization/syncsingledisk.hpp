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

//! \file	syncsingledisk.hpp
//! \brief	Definition of the single disk model.

#ifndef _OGSS_SYNCSINGLEDISK_HPP_
#define _OGSS_SYNCSINGLEDISK_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <vector>

#include "structure/hardware.hpp"
#include "synchronization/synchronizationmodel.hpp"
#include "structure/requeststat.hpp"

//! \brief	Synchronization model for a single disk system. This model is only
//!			used to calibrate a device model or validate the simulation against
//!			a real disk system. It does not take into account the full hardware
//!			topology to only consider a single disk.
class SyncSingleDisk: public SynchronizationModel {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	ci					Communication interface.
//! \param	params				Hardware parameters.
//! \param	globalDU			Global data unit.
	SyncSingleDisk (
		std::shared_ptr <CommunicationInterface>	ci,
		HardwareParameters		& params,
		OGSS_DataUnit			globalDU);

//! \brief	Destructor.
	~SyncSingleDisk ();

//! \brief	Add a request in the model, following its reception from the
//!			execution module.
	void addEntry (
		Request	    			&req);

//! \brief	Construct a request stat structure.
//! \param	idx					Request index.
//! \return						Request stat.
	RequestStat prepareStat (
		Request					& idx);

//! \brief	Once all the requests are received, start processing the
//!			synchronization which computes request waiting times by determining
//!			the correct request scheduling.
	void process ();

//! \brief	Generation of the detailed output file.
//! \param	outputFile			Path to the detailed output file.
	void createOutputFile (
		const OGSS_String		outputFile);

//! \brief	Generation of the resume output file.
//! \param	resumeFile			Path to the resume file.
	void createResumeFile (
		const OGSS_String		resumeFile);

private:

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	std::vector <Request>		_reqs;				//!< Requests.
	HardwareParameters			& _hardParam;		//!< Hardware parameters.
	OGSS_DataUnit				_globalDU;			//!< Global data unit.
};

#endif
