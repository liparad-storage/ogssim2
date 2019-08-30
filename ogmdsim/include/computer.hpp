/*
 * Copyright UVSQ - CEA/DAM/DIF (2019)
 * Contributors:	Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *					Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General
 * Public License as published per the Free Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

//! \file			computer.hpp
//! \brief			Definition of the computer class. The computer is used to compute the metadata request latency,
//!					depending on the nodes visisted to access the one where the targeted metadata are located.

#ifndef _OGMDS_COMPUTER_HPP_
#define _OGMDS_COMPUTER_HPP_

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include "mdrequest.hpp"
#include "system.hpp"

//! \brief			The computer is only composed of a computation function which updates the request structure with
//!					its end date ie. when the metadata information reaches the host node which launched the request.
class Computer {
public:

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Constructor.
//! \param			sys					System structure, which gets the links between the metadata nodes and the
//!										latency to transfer the information from one to another.
	Computer (
		System							& sys);

//! \brief			Destructor.
	~Computer ();

//! \brief			Compute the latency of the metadata request and update its end date.
//! \param			req					Metadata request.
	void compute (
		MDRequest						& req);

private:

/*--------------------------------------------------------------------------------------------------------------------*/
/* ATTRIBUTES --------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

	System								& _system;								//!< Metadata system.
};

#endif
