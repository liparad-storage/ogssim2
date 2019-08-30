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

//! \file	raid1volctrl.hpp
//! \brief	Definition of the RAID 1 controller.

#ifndef _OGSS_RAID1VOLCTRL_HPP_
#define _OGSS_RAID1VOLCTRL_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <map>
#include <set>

#include "controller/volumecontroller.hpp"

#include "structure/hardware.hpp"

#include "util/unitarytest.hpp"

//! \brief	Controller for a RAID 1.
class RAID1VolCtrl:
public VolumeController {
public:
	friend class UT_RAID1VolCtrl;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	vol					Volume.
//! \param	dev					Device contained in the volume.
	RAID1VolCtrl (
		const Volume			& vol,
		const Device			& dev);

//! \brief	Destructor.
	~RAID1VolCtrl ();

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
		{ return 1; }

protected:

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	OGSS_Ulong					_numBytesByDev;		//!< Number of bytes by device.
	OGSS_Ushort					_numDevices;		//!< Number of devices.
	OGSS_Bool					_mirrorChosen;		//!< TRUE if the mirror was chosen last.
	std::set <OGSS_Ushort>		_failedDevices;		//!< Failed devices.
	std::map <OGSS_Ushort, OGSS_Real>
								_failureDates;		//!< Failure dates.
};

/*----------------------------------------------------------------------------*/
/* UNITARY TEST --------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

class UT_RAID1VolCtrl:
public UnitaryTest <UT_RAID1VolCtrl> {
public:
	UT_RAID1VolCtrl (
		const OGSS_String		& configurationFile);
	~UT_RAID1VolCtrl ();

protected:
	OGSS_Bool middleDeviceRequest ();
	OGSS_Bool startDeviceRequest ();
	OGSS_Bool endDeviceRequest ();
	OGSS_Bool twoDeviceRequest ();
	OGSS_Bool moreDeviceRequest ();
	OGSS_Bool readRequests ();
	OGSS_Bool writeRequest ();
};

#endif
