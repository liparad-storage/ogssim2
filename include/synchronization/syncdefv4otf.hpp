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

//! \file	syncdefv4otf.hpp
//! \brief	Definition of the default model (version 4) with on the fly system request generation.

#ifndef _OGSS_SYNCDEFV4OTF_HPP_
#define _OGSS_SYNCDEFV4OTF_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <array>
#include <tuple>
#include <set>

#include "serializer/resume.hpp"

#include "structure/devicestate.hpp"
#include "structure/hardware.hpp"
#include "structure/requeststat.hpp"

#include "synchronization/synchronizationmodel.hpp"

#include "util/unitarytest.hpp"

typedef std::set <std::pair <Request, OGSS_Bool>, OGSS_Bool (*) (std::pair <Request, OGSS_Bool>, std::pair <Request, OGSS_Bool>)>
										RequestSet;

//! \brief	The version 4 of the default model is the fastest provided model.
//!			It was also made to be simpler to use and implement than the
//!			version 2. The data structures it used are the same as in version 2
//!			but they contain less information. For instance, instead of getting
//!			the waiting times for all the steps of the storage system, they
//!			only stored those of the step they currently matter (logical request
//!			between host and tier or physical request between volume and
//!			device). Also, it takes reconstruction requests on-the-fly to
//!			reduce the memory used and process this step faster.
class SyncDefV4OTF: public SynchronizationModel {
private:

/*----------------------------------------------------------------------------*/
/* PRIVATE STRUCTURES --------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	struct OrderedWaitQueue;

	static OGSS_Bool __stampCompare (
		std::pair <OGSS_RequestIdx, OGSS_Real>				lhs,
		std::pair <OGSS_RequestIdx, OGSS_Real>				rhs);

	enum tabid_t {UND = -1, ARRIVL, IN, OUT, TABTOT};
	enum count_t {IDSTEP, IDBUS, START, SYSTEM, REMOVE, CNTTOT};
	enum pntid_t {IDNEXT, IDPREV, PNTTOT};

	typedef std::array <OGSS_Real, TABTOT>					value_t;
	typedef std::array <int, CNTTOT>						valct_t;
	typedef std::array <OGSS_RequestIdx, PNTTOT>			point_t;
	typedef std::map <OGSS_RequestIdx, value_t>				table_t;
	typedef std::map <OGSS_RequestIdx, valct_t>				tabct_t;
	typedef std::map <OGSS_RequestIdx, point_t>				tabpt_t;

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
	SyncDefV4OTF (
		std::shared_ptr <CommunicationInterface>	ci,
		HardwareParameters		& params,
		std::vector <Tier>		& tiers,
		std::vector <Volume>	& vols,
		std::vector <Device>	& devs,
		OGSS_DataUnit			globalDU);

//! \brief	Destructor.
	~SyncDefV4OTF ();

//! \brief	Add a request in the model, following its reception from the
//!			execution module.
	void addEntry (
		Request	    			& req);

//! \brief	Construct a request stat structure.
//! \param	idx					Request index.
//! \return						Request stat.
	RequestStat prepareStat (
		const OGSS_RequestIdx	idx);

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

//! \brief	Determine the next request to process.
//! \return						Next request index to process.
	OGSS_RequestIdx _getNextRequest ();

//! \brief	Update the clock of each request issued from the same logical parent request.
//! \param	mainIdx				Logical parent request index.
//! \param	clock				New clock.
	void _updateRequestClock (
		const OGSS_Ulong		mainIdx,
		const OGSS_Real			clock);

//! \brief	Compute the waiting time from the host to the tier.
//! \param	idx					Request index.
	void _processToTier (
		const OGSS_RequestIdx	idx);

//! \brief	Compute the waiting time from the tier to the volume.
//! \param	idx					Request index.
	void _processToVolume (
		const OGSS_RequestIdx	idx);

//! \brief	Compute the waiting time from the volume to the device (user request).
//! \param	idx					Request index.
	void _processUserToDevice (
		const OGSS_RequestIdx	idx);

//! \brief	Compute the waiting time from the device to the volume (user request).
//! \param	idx					Request index.
	void _processUserFromDevice (
		const OGSS_RequestIdx	idx);

//! \brief	Compute the waiting time of a system request.-
//! \param	idx					Request index.
	void _processSystem (
		const OGSS_RequestIdx			evIdx,
		Request							& req,
		OGSS_Bool						direction);

//! \brief	Compute the waiting time from the volume to the tier.
//! \param	idx					Request index.
	void _processFromVolume (
		const OGSS_RequestIdx	idx);

//! \brief	Compute the waiting time from the tier to the host.
//! \param	idx					Request index.
	void _processFromTier (
		const OGSS_RequestIdx	idx);

//! \brief	Seek the next system requests to process. If none are found, request
//!			new ones to the volume drivers.
//! \param	idx					Event index.
//! \param	clock				Current clock.
//! \param	subrequests			System requests to process.
//! \return						TRUE if there is still requests for this event.
	OGSS_Bool _requestSystemRequests (
		const OGSS_RequestIdx	idx,
		const OGSS_Real 		clock,
		std::vector <Request>	& subrequests);

//! \brief	Process an event until the deadline is reached.
//! \param	evIdx				Event index.
//! \param	deadline			Deadline clock to reach.
	void _processEvent (
		const OGSS_RequestIdx	evIdx,
		const OGSS_Real			deadline);

//! \brief	Select the system requests to process depending on the request type.
//! \param	evIdx				Event index.
//! \param	subrequests			Subrequests to process.
//! \param	reqs				Selected subrequests.
	void _selectSysRequestOperation (
		const OGSS_RequestIdx	evIdx,
		std::vector <Request>	& subrequests,
		RequestSet				& reqs);

//! \brief	Select the user request operation depending on the current state of
//!			the targeted device.
//! \param	idx					Request index.
//! \param	clock				Current clock.
	void _selectUserRequestOperation (
		const OGSS_RequestIdx	idx,
		const OGSS_Real			clock);

//! \brief	Update the rebuilt and spared blocks data structures depending on
//!			a given processed request.
//! \param	req					Processed request.
	void _manageBlocks (
		const Request			req);

//! \brief	Empty the event request buffer.
//! \param	evIdx				Event index.
	void _emptyBuffer (
		const OGSS_RequestIdx	evIdx);

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	HardwareParameters			& _hardParam;		//!< Hardware parameters.
	std::vector <Tier>			& _tiers;			//!< Tiers.
	std::vector <Volume>		& _volumes;			//!< Volumes.
	std::vector <Device>		& _devices;			//!< Devices.

	table_t						_data;				//!< Data array.
	tabct_t						_cntr;				//!< Counter array.

	table_t						_userRslt;			//!< Result array for user requests.

	std::map <OGSS_RequestIdx, OGSS_Bool>
								_failedReqs;		//!< Failed requests.

	std::map <OGSS_RequestIdx, Request>
								_userRequests;		//!< User requests.

	std::map <OGSS_Ushort, std::vector <OGSS_RequestIdx>>
								_evByVolIdx;		//!< Events sorted by their targeted volume index.
	std::map <OGSS_RequestIdx, Request>
								_events;			//!< Events.
	std::map <OGSS_Ulong, std::pair <OGSS_RequestIdx, OGSS_RequestIdx>>
								_evByDev;			//!< Events sorted by their targeted device index.
	std::map <OGSS_Ulong, OGSS_DeviceState>
								_deviceState;		//!< Devices state.
	std::map <OGSS_Ulong, OGSS_Ulong>
								_evLastDevAddress;	//!< Last address processed by the events.

	OGSS_Ulong					_evCounter {0};		//!< Effective number of reconstructed blocks.
	OGSS_Ulong					_evCntMax {0};		//!< Total number of expected blocks to reconstruct.
	std::map <OGSS_RequestIdx, std::pair <OGSS_Bool, std::vector <Request>>>
								_eventRequests;		//!< Event requests.
	std::map <OGSS_RequestIdx, OGSS_Real>
								_eventDurations;	//!< Event durations.

	OGSS_Ulong					_nbMainRequests {0};	//!< Number of main requests.
	OGSS_Ulong					_nbMainRequestsDone {0};	//!< Number of done main requests.

	OGSS_Ulong					_nbSystemRequestsGenerated {10000};	//!< Number of requests generated on the fly.
	OGSS_Ulong					_minOTFRequestSize {1048576}; //!< Size of the generated request.

	OGSS_Ulong					_starter {0};		//!< Starter used during synchronization to know
													//!< from where we need to start seeking the next
													//!< request to process

	std::map <OGSS_Ulong, std::pair <OGSS_Real, OGSS_Real>>
								_requestClock;		//!< Logical request clocks.

	OGSS_DataUnit				_globalDU;			//!< Global data unit.

	std::map <OGSS_Ulong, OGSS_Real>
								_devClocks;			//!< Device clocks.
	std::map <OGSS_Ulong, OGSS_Real>
								_busClocks;			//!< Interface clocks.

	std::map <OGSS_Ulong, std::vector <OGSS_Bool>>
								_sparedBlocks;		//!< List of spared blocks.
	std::map <OGSS_Ulong, std::vector <OGSS_Bool>>
								_rebuiltBlocks;		//!< List of rebuilt blocks.

	Resume 						_resume;			//!< Resume file generator.

	OGSS_RequestIdx				_lastOTFEvent;		//!< Last processed event.
	Request						_OTFBuffer;			//!< Buffer used when receiving the on-th-fly requests.
	std::map <OGSS_RequestIdx, std::pair <OGSS_Ulong, std::vector <Request>>>
								_OTFEvRequests;		//!< Requests stored for a given event.
	std::map <OGSS_RequestIdx, std::vector <OGSS_Ulong>>
								_OTFEvPrefetch;		//!< Indicate the last device address prefetched for a given event.

	std::vector <OrderedWaitQueue>
								_busWaitQueue;		//!< Bus waiting queues.
	OGSS_Real					_firstEventDate {OGSS_REAL_MAX};
													//!< First event date.

};

#endif
