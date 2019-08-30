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

//! \file	syncdefv2.hpp
//! \brief	Definition of the default model (version 2).

#ifndef _OGSS_SYNCDEFV2_HPP_
#define _OGSS_SYNCDEFV2_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <array>
#include <tuple>

#include "structure/hardware.hpp"
#include "structure/requeststat.hpp"

#include "synchronization/synchronizationmodel.hpp"
#include "serializer/resume.hpp"

//! \brief	The version 2 of the default model was the mainly used
//!			synchronization model. It consists in extracting the useful request
//!			data from the request structure, and merge them in an array. 4 arrays
//!			are used during the synchronization process: (1) a data array,
//!			containing the arrival date, transfer and service times ; (2) a result
//!			array, containing the waiting times ; (3) a counter array to get which
//!			device/interface the request targets, and other stuff ; (4) and
//!			finally a shortcut pair, to determine faster the request scheduling.
//!			The version 4 was made because implementing the on-the-fly
//!			reconstruction request generation in version 2 would be too
//!			complicated and time consuming.
class SyncDefV2: public SynchronizationModel {
private:
	enum tabid_t {UND = -1, ARRIVL, TO_TIR, TO_VOL, TO_DEV, SERVCE, FM_DEV,
		FM_VOL, FM_TIR, TABTOT};
	enum clkid_t {BUS_HT, BUS_TV, BUS_VD, CLKTOT};
	enum count_t {IDSTEP, NBCHLD, NBPRIO, IDBUST, IDBUSV, IDBUSD, IDTIER,
		IDVOLM, IDDEVC, RQSIZE, RQTYPE, CNTTOT};
	enum pntid_t {IDNEXT, IDPREV, PNTTOT};

	typedef std::tuple <OGSS_Ulong, OGSS_Ulong, OGSS_Ulong>	index_t;
	typedef std::array <double, TABTOT>						value_t;
	typedef std::array <int, CNTTOT>						valct_t;
	typedef std::array <index_t, PNTTOT>					point_t;
	typedef std::map <index_t, value_t>						table_t;
	typedef std::map <index_t, valct_t>						tabct_t;
	typedef	std::map <index_t, point_t>						tabpt_t;

public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	ci					Communication interface.
//! \param	params				Hardware parameters.
//! \param	tiers				Tiers.
//! \param	vols				Volumes.
//! \param	devs				Devices.
//! \param	intfs				Interfaces.
//! \param	globalDU			Global data unit.
	SyncDefV2 (
		std::shared_ptr <CommunicationInterface>	ci,
		HardwareParameters		& params,
		std::vector <Tier>		& tiers,
		std::vector <Volume>	& vols,
		std::vector <Device>	& devs,
		std::vector <Interface>	& intfs,
		OGSS_DataUnit			globalDU);

//! \brief	Destructor.
	~SyncDefV2 ();

//! \brief	Add a request in the model, following its reception from the
//!			execution module.	
	void addEntry (
		Request	    			&req);

//! \brief	Construct a request stat structure.
//! \param	idx					Request index.
//! \return						Request stat.
	RequestStat prepareStat (
		const index_t			idx);

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
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Compute the waiting time from the host to the tier.
//! \param	minCursor			Current index.
//! \param	tmp					Result value of the current index.
//! \param	idx					Counter value of the current index.
//! \param	nbComputations		Remaining number of computations.
	void _processToTier (
		index_t					& minCursor,
		value_t					& tmp,
		valct_t					& idx,
		OGSS_Ulong				& nbComputations);

//! \brief	Compute the waiting time from the tier to the volume.
//! \param	minCursor			Current index.
//! \param	tmp					Result value of the current index.
//! \param	idx					Counter value of the current index.
//! \param	nbComputations		Remaining number of computations.
	void _processToVolume (
		index_t					& minCursor,
		value_t					& tmp,
		valct_t					& idx,
		OGSS_Ulong				& nbComputations);

//! \brief	Compute the waiting time from the volume to the device.
//! \param	minCursor			Current index.
//! \param	tmp					Result value of the current index.
//! \param	idx					Counter value of the current index.
//! \param	nbComputations		Remaining number of computations.
//! \param	devClocks			Device clocks.
	void _processToDevice (
		index_t					& minCursor,
		value_t					& tmp,
		valct_t					& idx,
		OGSS_Ulong				& nbComputations,
		std::vector <double>	& devClocks);

//! \brief	Compute the waiting time from the device to the volume.
//! \param	minCursor			Current index.
//! \param	tmp					Result value of the current index.
//! \param	idx					Counter value of the current index.
//! \param	nbComputations		Remaining number of computations.
	void _processFromDevice (
		index_t					& minCursor,
		value_t					& tmp,
		valct_t					& idx,
		OGSS_Ulong				& nbComputations);

//! \brief	Compute the waiting time from the volume to the tier.
//! \param	minCursor			Current index.
//! \param	tmp					Result value of the current index.
//! \param	idx					Counter value of the current index.
//! \param	nbComputations		Remaining number of computations.
	void _processFromVolume (
		index_t					& minCursor,
		value_t					& tmp,
		valct_t					& idx,
		OGSS_Ulong				& nbComputations);

//! \brief	Compute the waiting time from the tier to the host.
//! \param	minCursor			Current index.
//! \param	tmp					Result value of the current index.
//! \param	idx					Counter value of the current index.
//! \param	nbComputations		Remaining number of computations.
//! \param	starter				First index to compare.
//! \param	lastMainIdx			Last main index done.
	void _processFromTier (
		index_t					& minCursor,
		value_t					& tmp,
		valct_t					& idx,
		OGSS_Ulong				& nbComputations,
		index_t					& starter,
		OGSS_Ulong				& lastMainIdx);

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	HardwareParameters			& _hardParam;		//!< Hardware parameters.
	std::vector <Tier>			& _tiers;			//!< Tiers.
	std::vector <Volume>		& _volumes;			//!< Volumes.
	std::vector <Device>		& _devices;			//!< Devices.
	std::vector <Interface>		& _interfaces;		//!< Interfaces.

	table_t						_data;				//!< Data used during the synchronization.
	table_t						_rslt;				//!< Results computed during the synchronization.
	tabct_t						_cntr;				//!< Counters used during the synchronization.
	tabpt_t						_idpt;				//!< Shortcuts used during comparison.
	std::map <index_t, OGSS_Bool>
								_failedReqs;		//!< Failed requests.

	Resume 						_resume;			//!< Resume file generator.
	OGSS_Ulong					_mainRequestsDone;	//!< Number of done user requests.
	OGSS_Ulong					_nbRequests;		//!< Number of user requests.
	OGSS_DataUnit				_globalDU;			//!< Global data unit.
};

#endif
