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

//! \file	event.hpp
//! \brief	Definition of event structure. Used following the extraction by the
//!			event extractor module.

#ifndef _OGSS_EVENT_HPP_
#define _OGSS_EVENT_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/types.hpp"

/*----------------------------------------------------------------------------*/
/* STRUCTURE -----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Event structure.
struct Event {
	OGSS_EventType				_type;				//!< Event type.
	OGSS_Ushort					_target;			//!< Targeted device.
	OGSS_Real					_date;				//!< Arrival date.

//! \brief	Comparison operator. First by comparing the arrival date, and then
//!			the device index.
//! \param	rhs					Comparison member.
//! \return						TRUE if before rhs, FALSE if not.
	inline OGSS_Bool operator< (
		const Event&			rhs) const;
};

/*----------------------------------------------------------------------------*/
/* INLINE MEMBER FUNCTION ----------------------------------------------------*/
/*----------------------------------------------------------------------------*/

inline OGSS_Bool
Event::operator< (
	const Event&				rhs) const {
	return (_date < rhs._date
		 || (_date == rhs._date && _target < rhs._target) );
}

#endif
