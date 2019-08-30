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

//! \file	decraidscheme.hpp
//! \brief	Describes the interface for declustered RAID schemes.

#ifndef _OGSS_DECRAIDSCHEME_HPP_
#define _OGSS_DECRAIDSCHEME_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/hardware.hpp"
#include "structure/request.hpp"

//! \brief	Interface of declustered RAID scheme.
class DecRAIDScheme {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC MEMBER FUNCTIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Destructor.
	virtual ~DecRAIDScheme ();

//! \brief	Scheme construction.
	virtual void build () = 0;

//! \brief	Request translation from logical volume to physical volume.
//! \param	request				Request to translate.
	virtual void realloc (
		Request					& request) = 0;

//! \brief	Update the scheme with the given failure event.
//! \param	event				Received event.
	virtual void updateFailureScheme (
		const Request			& event) = 0;

//! \brief	Request generation following a failure event.
//! \brief	event				Failure.
//! \brief	subrequests			Created requests.
	virtual void generateFailureRequests (
		const Request			& block,
		std::vector <Request>	& subrequests) = 0;

//! \brief	Update the scheme with the given renewal event.
//! \param	event				Received event.
	virtual void updateRenewalScheme (
		const Request			& event) = 0;

//! \brief	Request generation following a renewal/replacement event.
//! \brief	event				Renewal.
//! \brief	subrequests			Created requests.
	virtual void generateRenewalRequests (
		const Request			& block,
		std::vector <Request>	& subrequests) = 0;

//! \brief	Compute the logical block of a given physical block.
//! \param	block				Physical block.
//! \return						Logical block.
	virtual Request getLogicalBlock (
		const Request			& block) = 0;

//! \brief	Request generation in case an event occured:
//!			- Native generation -- access on target device
//!			- Spare generation -- access on redundancy device
//!			- Renewal generation -- access on spare area
//! \param	request				Request.
//! \param	nativeSubrequests	Requests from the decomposition.
//! \param	multipSubrequests	Requests from the multiplier.
	virtual void requestMultiplier (
		const Request			& request,
		std::vector <Request>	& nativeSubrequests,
		std::vector <Request>	& multipSubrequests) = 0;

protected:

/*----------------------------------------------------------------------------*/
/* PRIVATE MEMBER FUNCTION ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	vol					Declustered RAID parameters.
	DecRAIDScheme (
		const Volume			& vol);

/*----------------------------------------------------------------------------*/
/* PRIVATE ATTRIBUTES --------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	const Volume				& _vol;				//!< Declustered RAID parameters.
};

#endif
