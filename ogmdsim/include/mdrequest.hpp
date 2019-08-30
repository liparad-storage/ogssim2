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

//! \file			mdrequest.hpp
//! \brief			Definition of the metadata request structure which is one of the two main data structures of
//!					OGMDSim. It contains the request parameters (object ID, date, host ID and request type) and the
//!					generated informations (end date, data logical address, data size, metadata node ID).

#ifndef _OGMDS_MDREQUEST_HPP_
#define _OGMDS_MDREQUEST_HPP_

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include <cstdint>
#include <limits>

/*--------------------------------------------------------------------------------------------------------------------*/
/* CONSTANTS ---------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Metadata request types recognized by OGMDSim.
enum RequestType {
	OGRT_READ,																	//!< Read request.
	OGRT_WRITE																	//!< Write request.
};

//! \brief			Metadata request structure.
struct MDRequest {

/*--------------------------------------------------------------------------------------------------------------------*/
/* ATTRIBUTES --------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

	uint64_t							objID;									//!< Object ID.
	double								date;									//!< Request date of launch.
	unsigned							host;									//!< Host node ID.
	RequestType							type;									//!< Request type.

	double								endDate {std::numeric_limits <double> ::max () };
																				//!< End date ie. date when the metadata
																				//!< process is done.
	uint64_t							logAddress {std::numeric_limits <uint64_t> ::max () };
																				//!< Data logical address.
	unsigned							size {std::numeric_limits <unsigned> ::max () };
																				//!< Data size.
	unsigned							mdServ;									//!< Metadata node ID.
};

#endif
