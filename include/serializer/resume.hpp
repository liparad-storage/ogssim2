/*
 * Copyright UVSQ - CEA/DAM/DIF (2017-2018)
 * Contributors:  Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *                Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *                Maxence JOULIN
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

//! \file	resume.hpp
//! \brief	Generation of the resume file once the simulation is complete.
//!			The resume file consists in global to local results on the system
//!			(whole system, tiers, volumes and devices).

#ifndef _OGSS_RESUME_HPP_
#define _OGSS_RESUME_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <array>
#include <tuple>

#include "structure/hardware.hpp"
#include "structure/requeststat.hpp"

#include "synchronization/synchronizationmodel.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>

#include <glog/logging.h>

/*----------------------------------------------------------------------------*/
/* CLASS ---------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Class in charge of the generation of the resume file.
class Resume {
	public:

/*----------------------------------------------------------------------------*/
/* PUBLIC MEMBER FUNCTIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
		Resume(){};
		
//! \brief	Constructor with initialization.
//! \param	nbRequests			Number of requests.
//! \param	globalDU			Global data units.
//! \param	nbTiers				Number of tiers.
//! \param	nbVols				Number of volumes.
//! \param	nbDevs				Number of devices.
		Resume(
			OGSS_Ulong nbRequests,
			OGSS_DataUnit globalDU,
			OGSS_Ulong nbTiers,
			OGSS_Ulong nbVols,
			OGSS_Ulong nbDevs);

//! \brief	Updates the resume contents with the newly received stats.
//! \param	stats				Stats to used in the update.
		void updateStats (
			RequestStat stats);

//! \brief	Updates the resume contents with event information.
//! \param	event				Processed event.
//! \param	duration			Event duration.
	void updateEvent (
		const Request					& event,
		OGSS_Real						duration);

//! \brief	Generates the resume file.
//! \param	resumeFile			Resume filename.
	void save(
		OGSS_String resumeFile);

//! \brief	Setter for the date of the first event.
//! \param	date				Date of the first event.
	inline void setFirstDateEvent (
		const OGSS_Real					& date) {
		firstEventDate = date;
	}

//! \brief	Setter for the part of reconstructed blocks.
//! \param	part				Part of reconstructed blocks.
	inline void setPartReconstructedBlocks (
		const OGSS_Real					& part) {
		partReconstructedBlocks = part;
	}

	private:

/*----------------------------------------------------------------------------*/
/* PRIVATE ATTRIBUTES --------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

		OGSS_Ulong				_nbRequests;		//!< Number of requests.
		OGSS_DataUnit			_globalDU;			//!< Global data units.
		OGSS_Ulong              _nbTiers;			//!< Number of tiers.
		OGSS_Ulong              _nbVols;			//!< Number of volumes.
		OGSS_Ulong              _nbDevs;			//!< Number of devices.

		OGSS_Ulong              nbRequestsDone = 0;	//!< Number of done requests.
		OGSS_Real				firstEventDate {OGSS_REAL_MAX};	//!< Date of the first event.

		OGSS_Real				waitTimeBeforeFirstEvent {.0};	//!< Waiting time before the first event.
		OGSS_Real				counterBeforeFirstEvent {.0};	//!< Number of requests processed before the first event.
		OGSS_Real				waitTimeAfterFirstEvent {.0};	//!< Waiting time after the first event.
		OGSS_Real				counterAfterFirstEvent {.0};	//!< Number of requests processed after the first event.
		OGSS_Real				partReconstructedBlocks {.0}; 	//!< Part of reconstructed blocks.


//! \brief	Structure used to store device statistics.
	struct Stat {
		int id;										//!< Device index.
		OGSS_Real workTime = 0;						//!< Work time.
		OGSS_Real idleTime = 0;						//!< Idle time.
		OGSS_Real waitTime = 0;						//!< Waiting time.
		OGSS_Real totalTime = 0;					//!< Total time.
		OGSS_Ulong nbRequests = 0;					//!< Number of requests processed.
		OGSS_Ulong nbReadRequests = 0;				//!< Number of read requests processed.
		OGSS_Ulong nbFailedRequests {0};			//!< Number of failed requests.
		OGSS_Real averageRequestSize = 0;			//!< Average request size.

		OGSS_Ushort				parent {OGSS_USHORT_MAX}; //!< Parent index.
	};

	std::vector <Stat> devices_stats;				//!< Vector of each device stats.
	std::vector <Stat> volumes_stats;				//!< Vector of each volume stats.
	std::map <OGSS_Ulong, OGSS_Real>	_eventRes;	//!< Event information.
	Stat tier;										//!< Tier stats.
};

#endif
