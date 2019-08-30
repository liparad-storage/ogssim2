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

//! \file	raidnpnodecvolctrl.hpp
//! \brief	Definition of the RAID NP controller without declustering.
//!	\deprecated		Use the PerfParityCtrl.

#ifndef _OGSS_RAIDNPNODECVOLCTRL_HPP_
#define _OGSS_RAIDNPNODECVOLCTRL_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <map>
#include <set>

#include "controller/volumecontroller.hpp"

#include "structure/hardware.hpp"

#include "util/unitarytest.hpp"

//! \brief	Controller for a RAID NP.
class RAIDNPNoDecVolCtrl:
public VolumeController {
public:
	friend class UT_RAIDNPNoDecVolCtrl;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	vol					Volume.
//! \param	dev					Device contained in the volume.
	RAIDNPNoDecVolCtrl (
		const Volume			& vol,
		const Device			& dev);

//! \brief	Destructor.
	~RAIDNPNoDecVolCtrl ();

//! \brief	Request decomposition.
//! \param	request				Request to decompose.
//! \param	subrequests			New requests.
	void decompose (
		Request					& request,
		std::vector <Request>	& subrequests);

//! \brief	Update the volume state when receiving an event.
//! \param	event				Event received.
//! \param	subrequests			Requests created due to the event.
	void updateScheme (
		const Request			& event);

//! \brief	Generate the requests issued from a decraid failure of a given block.
//! \param	block				Block to recover.
//! \param	subrequests			Recovery requests.
	void generateDecraidFailureRequests (
		Request					& block,
		std::vector <Request>	& subrequests);

//! \brief	Generate the requests issued from a decraid renewal of a given block.
//! \param	block				Block to recover.
//! \param	subrequests			Recovery requests.
	void generateDecraidRenewalRequests (
		Request					& block,
		std::vector <Request>	& subrequests);

//! \brief	Generate the requests issued from a failure of a given block.
//! \param	block				Block to recover.
//! \param	subrequests			Recovery requests.
	void generateFailureRequests (
		Request					& block,
		std::vector <Request>	& subrequests);

//! \brief	Generate the requests issued from a renewal of a given block.
//! \param	block				Block to recover.
//! \param	subrequests			Recovery requests.
	void generateRenewalRequests (
		Request					& block,
		std::vector <Request>	& subrequests);

//! \brief	Multiply the requests generated by the volume to take into account
//!			the events.
//! \param	block				Targeted block.
//! \param	subrequests			Requests issued from the multiplier.
	void requestMultiplier (
		Request					& block,
		std::vector <Request>	& subrequests);

//! \brief	Getter of the block location from its logical volume address.
//! \param 	block				Targeted block.
	void getBlockLocation (
		Request					& block);

//! \brief	Getter of the number of data blocks in the requested device.
//! \param	idxDevice			Targeted device index.
//! \return						Requested number of data blocks.
	OGSS_Ulong getNumberDataBlocks (
		OGSS_Ulong				idxDevice)
		{ return _numBytesByDev / _numBytesBySU; }

protected:

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Update the decomposition according to a failure if there was one.
//! \param	request				Request to decompose.
//! \param	subrequests			New requests.
	void manageFailure (
		Request					& request,
		std::vector <Request>	& subrequests);

//! \brief	Remove duplicated requests due to multiple generation.
//! \param	request				Parent request.
//! \param	subrequests			Requests to check for duplicates.
	void removeDuplicates (
		Request					& request,
		std::vector <Request>	& subrequests);

//! \brief	Check if the request targets a parity block.
//! \param	request				Request.
//! \return						TRUE if it targets a parity block, FALSE else.
	OGSS_Bool targetsParity (
		Request					& request);
	
/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	OGSS_Ulong					_numBytesByDev;		//!< Number of bytes by device.
	OGSS_Ulong					_numBytesBySU;		//!< Number of bytes by stripe unit.
	OGSS_Ulong					_numDevices;		//!< Number of devices.
	OGSS_Ulong					_numParity;			//!< Number of parity devices.
	std::set <OGSS_Ushort>		_failedDevices;		//!< Failed devices.
	std::map <OGSS_Ushort, OGSS_Real>
								_failureDates;		//!< Failure dates.
};

/*----------------------------------------------------------------------------*/
/* UNITARY TEST --------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

class UT_RAIDNPNoDecVolCtrl:
public UnitaryTest <UT_RAIDNPNoDecVolCtrl> {
public:
	UT_RAIDNPNoDecVolCtrl (
		const OGSS_String		& configurationFile);
	~UT_RAIDNPNoDecVolCtrl ();

protected:
	OGSS_Bool smallStripeR0PRequest ();
	OGSS_Bool largeStripeR0PRequest ();
	OGSS_Bool fullStripeR0PRequest ();
	OGSS_Bool twoStripeR0PRequest ();
	OGSS_Bool moreStripeR0PRequest ();
	OGSS_Bool smallStripeW0PRequest ();
	OGSS_Bool largeStripeW0PRequest ();
	OGSS_Bool fullStripeW0PRequest ();
	OGSS_Bool twoStripeW0PRequest ();
	OGSS_Bool moreStripeW0PRequest ();
	OGSS_Bool smallStripeR1PRequest ();
	OGSS_Bool largeStripeR1PRequest ();
	OGSS_Bool fullStripeR1PRequest ();
	OGSS_Bool twoStripeR1PRequest ();
	OGSS_Bool moreStripeR1PRequest ();
	OGSS_Bool smallStripeW1PRequest ();
	OGSS_Bool largeStripeW1PRequest ();
	OGSS_Bool fullStripeW1PRequest ();
	OGSS_Bool twoStripeW1PRequest ();
	OGSS_Bool moreStripeW1PRequest ();
	OGSS_Bool smallStripeR2PRequest ();
	OGSS_Bool largeStripeR2PRequest ();
	OGSS_Bool fullStripeR2PRequest ();
	OGSS_Bool twoStripeR2PRequest ();
	OGSS_Bool moreStripeR2PRequest ();
	OGSS_Bool smallStripeW2PRequest ();
	OGSS_Bool largeStripeW2PRequest ();
	OGSS_Bool fullStripeW2PRequest ();
	OGSS_Bool twoStripeW2PRequest ();
	OGSS_Bool moreStripeW2PRequest ();
};

#endif
