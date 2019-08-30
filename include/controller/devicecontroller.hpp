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

//! \file	devicecontroller.hpp
//! \brief	Definition of the device controller interface.

#ifndef _OGSS_DEVICECONTROLLER_HPP_
#define _OGSS_DEVICECONTROLLER_HPP_

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/request.hpp"

//! \brief	Interface for a device controller. Each controller needs to implement
//!			a translate function to transform an intermediate request into a
//!			physical one. If the function is empty, it means that there is no
//!			additional translation. It will also contain the maintenance functions.
class DeviceController {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Destructor.
	virtual ~DeviceController () = default;

//! \brief	Translate an intermediate request into a physical one.
//! \param	request				Request to translate.
	virtual void translate (
		Request					& request) = 0;

protected:

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
	DeviceController () {  }
};

#endif
