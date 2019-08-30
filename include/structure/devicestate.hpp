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

//! \file	devicestate.hpp
//! \brief	Define the device state structure, used during the synchronization step
//!			to determine which reconstruction requests need to be generated.

#ifndef _OGSS_DEVICESTATE_HPP_
#define _OGSS_DEVICESTATE_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/types.hpp"

/*----------------------------------------------------------------------------*/
/* STRUCTURE -----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Structure defining a device state, which can be on/off, failed or
//!			renewed depending on when failure and/or renewal events occur.
struct OGSS_DeviceState {

//! \brief	Check if the device is on.
//! \param	clock						Checking date.
//! \return								TRUE if the device is on.
	inline OGSS_Bool isOn (OGSS_Real clock);

//! \brief	Check if the device is off.
//! \param	clock						Checking date.
//! \return								TRUE if the device is off.
	inline OGSS_Bool isOff (OGSS_Real clock);

//! \brief	Check if the device is failed.
//! \param	clock						Checking date.
//! \return								TRUE if the device is failed.
	inline OGSS_Bool isFailed (OGSS_Real clock);

//! \brief	Check if the device is renewed.
//! \param	clock						Checking date.
//! \return								TRUE if the device is renewed.
	inline OGSS_Bool isRenewed (OGSS_Real clock);

	OGSS_Real							_failureDate {OGSS_REAL_MAX};	//!< Device failure date (max if no failure).
	OGSS_Real							_renewalDate {OGSS_REAL_MAX};	//!< Device renewal date (max if no renewal.
};

/*----------------------------------------------------------------------------*/
/* INLINE FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

OGSS_Bool
OGSS_DeviceState::isOn (
	OGSS_Real							clock) {
	return (_failureDate != OGSS_REAL_MAX)
		|| (_renewalDate <= clock);
}

OGSS_Bool
OGSS_DeviceState::isOff (
	OGSS_Real							clock) {
	return (_failureDate <= clock)
		&& (_renewalDate > clock);
}

OGSS_Bool
OGSS_DeviceState::isFailed (
	OGSS_Real							clock)
	{ return _failureDate <= clock && _renewalDate > clock; }

OGSS_Bool
OGSS_DeviceState::isRenewed (
	OGSS_Real							clock)
	{ return _renewalDate <= clock; }

#endif
