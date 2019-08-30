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

//! \file	workloadextractor.hpp
//! \brief	<code>WorkloadExtractor</code> examines a given trace file and
//! extracts the user requests contained in it.
//!
//! <code>WorkloadExtractor</code> gets a start-up role and has to send to the
//! <code>PreProcessing</code> all the information related to the trace before
//! the simulation starts.
//!
//! In a multithreaded context, requests are stored in shared memory. Thus, the
//! communicated information is a pointer to the structure used in shared
//! memory, which is a request vector.
//!
//! In a distributed memory context, requests need to be fully communicated.

#ifndef _OGSS_WORKLOADEXTRACTOR_HPP_
#define _OGSS_WORKLOADEXTRACTOR_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/types.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "communication/communicationinterface.hpp"
#include "module/module.hpp"
#include "structure/request.hpp"
#include "util/unitarytest.hpp"

//!	\brief	Represents the workload extraction module.
class WorkloadExtractor: public Module {

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

public:
	friend class UT_WorkloadExtractor;

	//! \brief	Constructor (default).
	//! Initializes the class context, by retrieving from the configuration file
	//! the type of the communication model which will be used during the
	//! simulation, to connect to the manager and reserve a communication pipe.
	//! It also retrieves the path to the trace file, for a further extraction.
	//! \param	configurationFile	Path to the configuration file.
	WorkloadExtractor (
		const OGSS_String		configurationFile = "");

	//! \brief	Destructor.
	~WorkloadExtractor ();

//! \brief			Process during the extraction step.
	void processExtraction ();

//! \brief			Process during the decomposition step.
	void processDecomposition ();

//! \brief			Process during the synchronization step.
	void processSynchronization ();

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

private:
	//! \brief	Request extraction.
	//! Extracts the user requests contained in the trace file, and stores them
	//! in a data structure. The requests are sorted following their arrival
	//! dates.
	//! \param	workloadFile	Path to the trace file.
	void extract (
		const OGSS_String		workloadFile);

	//! \brief	Application of the data unit on the workload data
	//!	In case the global data unit is different from the workload one,
	//!	apply a coefficient equals to local/global to the date, the address
	//! and the size of the request.
	void applyDataUnit ();

	//!	\brief	Data structure transmission.
	//!			Sends the requests to the pre-processing module once the
	//! 		extraction is done and the others modules ready for the
	//! 		simulation.
	inline void sendData ();

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	std::vector <Request>		_requests;		//!< Request data structure.
	OGSS_DataUnit				_localDU;		//!< Local data unit.
	OGSS_DataUnit				_globalDU;		//!< Global data unit.
};

/*----------------------------------------------------------------------------*/
/* INLINE FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
WorkloadExtractor::sendData () {
	void						* arg;
	Request						req;

	for (auto & elt: _requests)
		_ci->send (std::make_pair (MTP_PREPROCESSING, 0),
			& elt, sizeof (elt) );

	req._type = RQT_END;
	_ci->send (std::make_pair (MTP_PREPROCESSING, 0),
		& req, sizeof (req) );
}

/*----------------------------------------------------------------------------*/
/* UNITARY TEST --------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//!	\brief	Unitary test interface implementation for
//!	<code>WorkloadExtractor</code>.
class UT_WorkloadExtractor:
public UnitaryTest <UT_WorkloadExtractor> {
public:
	//! \brief	Default constructor.
	UT_WorkloadExtractor (
		const OGSS_String		& configurationFile);

	//! \brief	Destructor.
	~UT_WorkloadExtractor ();

protected:
	//!	\brief	Filepath given as a parameter does not exist.
	//! \return					TRUE if the test succeeds.
	OGSS_Bool badParameter ();

	//!	\brief	Requests extracted from the file are correctly retrieved.
	//! \return					TRUE if the test succeeds.
	OGSS_Bool checkFile ();

	//!	\brief	Requests are correctly ordered by their arrival date.
	//! \return					TRUE if the test succeeds.
	OGSS_Bool unorderedFile ();
};

#endif
