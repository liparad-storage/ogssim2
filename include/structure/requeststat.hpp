/*
 * Copyright UVSQ - CEA/DAM/DIF (2017-2018)
 * Contributors:  Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *                Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published per the Free Software Foundation; either
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

//! \file	requeststat.hpp
//! \brief	Definition of request stat structure, which is used by
//!			synchronization and visualization to transfer the simulation
//!			results.

#ifndef _OGSS_REQUESTAT_HPP_
#define _OGSS_REQUESTAT_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/types.hpp"

/*----------------------------------------------------------------------------*/
/* STRUCTURE -----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Structure which contains all request main information and computed
//!			results.
struct RequestStat {
	OGSS_Ulong					_mainIdx;			//!< Main index.
	OGSS_Ulong					_majrIdx;			//!< Major index.
	OGSS_Ulong					_minrIdx;			//!< Minor index.

	OGSS_RequestType			_type;				//!< Request type.
	OGSS_Ulong					_size;				//!< Request size.

	OGSS_Ushort					_idxTier;			//!< Targeted tier index.
	OGSS_Ushort					_idxVolume;			//!< Targeted volume index.
	OGSS_Ulong					_idxDevice;			//!< Targeted device index.
	OGSS_Ushort					_idxBus;			//!< Targeted bus index.

	OGSS_Real					_arrivalDate;		//!< Arrival date.
	OGSS_Real					_serviceTime;		//!< Service time.
	OGSS_Real					_transferTime;		//!< Transfer time.
	OGSS_Real					_waitingTime;		//!< Waiting time.

	OGSS_Bool					_system;			//!< System priority (user
													//!< if not).
	OGSS_Bool					_failed;			//!< True if the request
													//!< could not succeed.
};

#endif
