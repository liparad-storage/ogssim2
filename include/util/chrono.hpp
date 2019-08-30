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

//! \file	chrono.hpp
//! \brief	Simple chronometer implementation.

#ifndef _OGSS_CHRONO_HPP_
#define _OGSS_CHRONO_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <chrono>

#include "structure/types.hpp"

//! \brief	Chronometer implementation. Measure the execution time between two
//! 		ticks in microseconds.
class Chrono {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC MEMBER FUNCTIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Default constructor.
	Chrono ();

//! \brief	Copy constructor.
//! \param	copy				Copy element.
	Chrono (
		const Chrono			& copy);

//! \brief	Destructor.
	~Chrono ();

//! \brief	Copy operator.
//! \param	copy				Copy element.
//! \return						Current element.
	Chrono & operator= (
		const Chrono			& copy);

//! \brief	Act as a button to launch or stop the chronometer.
	void tick ();

//! \brief	Restart the time counter to zero.
	void restart ();

//! \brief	Getter for the measured time in microseconds.
//! \return						Measured time.
	OGSS_Long get ();

private:

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	OGSS_Bool					_restart;
		//!< Indicates if a restart was requested.
	std::chrono::time_point <std::chrono::system_clock>
								_start;
		//!< First time the tick button was used.
	std::chrono::time_point <std::chrono::system_clock>
								_end;
		//!< Last time the tick button was used.
};

#endif
