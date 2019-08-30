/*
 * Copyright UVSQ - CEA/DAM/DIF (2018)
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

//! \file	sd2sscheme.hpp
//! \brief	Describes the SD2S (Symmetric Difference of Source Sets) scheme
//!			for declustered RAID.

#ifndef _OGSS_SD2SSCHEME_HPP_
#define _OGSS_SD2SSCHEME_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "scheme/decraidscheme.hpp"

#include "structure/devicestate.hpp"

//! \brief	Class implementing the SD2S algorithm. It consists in selecting the
//!			first offset that satisfy four constraints: (1) parallel access on
//!			devices, (2) parallel access on stripes, (3) source sets differences
//!			and (4) load balancing.
class SD2SScheme: public DecRAIDScheme {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC MEMBER FUNCTIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	Declustered RAID parameters.
//! \param	Subvolumes.
//! \param	Device used in the declustered RAID.
	SD2SScheme (
		const Volume			& vol,
		const std::vector <Volume>	& subVols,
		const Device			& dev);

//! \brief	Destructor.
	~SD2SScheme ();

//! \brief	Scheme construction.
	void build ();

//! \brief	Request translation from logical volume to physical volume.
//! \param	request				Request to translate.
	void realloc (
		Request					& request);

//! \brief	Update the scheme with the given failure event.
//! \param	event				Received event.
	void updateFailureScheme (
		const Request			& event);

//! \brief	Update the scheme with the given renewal event.
//! \param	event				Received event.
	void updateRenewalScheme (
		const Request			& event);

//! \brief	Request generation following a failure event.
//! \brief	event				Failure.
//! \brief	subrequests			Created requests.
	void generateFailureRequests (
		const Request			& block,
		std::vector <Request>	& subrequests);

//! \brief	Request generation following a renewal/replacement event.
//! \brief	event				Renewal.
//! \brief	subrequests			Created requests.
	void generateRenewalRequests (
		const Request			& block,
		std::vector <Request>	& subrequests);

//! \brief	Compute the logical block of a given physical block.
//! \param	block				Physical block.
//! \return						Logical block.
	Request getLogicalBlock (
		const Request			& block);

//! \brief	Request generation in case an event occured:
//!			- Native generation -- access on target device
//!			- Spare generation -- access on redundancy device
//!			- Renewal generation -- access on spare area
//! \param	request				Request.
//! \param	nativeSubrequests	Requests from the decomposition.
//! \param	multipSubrequests	Requests from the multiplier.
	void requestMultiplier (
		const Request			& request,
		std::vector <Request>	& nativeSubrequests,
		std::vector <Request>	& multipSubrequests);

private:

/*----------------------------------------------------------------------------*/
/* PRIVATE MEMBER FUNCTION ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Search for each offset prime with the number of devices, the first one
//!			which satisfies all the constraints.
	void findFirstGoodLambda () ;

//! \brief	Compute the class of the degree matrix for a given offset.
//! \param	lambda				Offset.
//! \return						2 if it satisfies all the constraints, 1 if the
//!								third constraint is half satisfied, 0 if not
//!								satisfied at all.
	OGSS_Ushort computeMatrixRank (
		const OGSS_Ulong		lambda);

/*----------------------------------------------------------------------------*/
/* PRIVATE ATTRIBUTES --------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	OGSS_Ulong					_lambda;			//!< Scheme offset.
	std::vector <OGSS_Ulong>	_belongs;			//!< Vector which informs in which subvolume is a device.

	OGSS_Ushort					_numDevices;		//!< Number of devices.
	OGSS_Ushort					_numSpareDevices;	//!< Number of spare devices.
	OGSS_Ushort					_numDataDevices;	//!< Number of data devices.
	OGSS_Ulong					_numBytesBySU;		//!< Number of bytes by stripe unit.
	OGSS_Ulong					_numBytesByDev;		//!< Number of bytes by device.

	std::map <OGSS_Ushort, std::vector <OGSS_Ushort>>
								_redirection;		//!< Redirection vector.
	std::map <OGSS_Ushort, std::pair <OGSS_Real, OGSS_Real>>
								_eventTimes;		//!< Event arrival dates.
	std::map <OGSS_Ulong, OGSS_DeviceState>
								_deviceState;		//!< Devices state.
};

#endif
