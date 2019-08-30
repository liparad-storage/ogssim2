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

//! \file			utils.hpp
//! \brief			Definition of the utils namespace. This namespace is used to define global constants
//!					and utility functions.

#ifndef _OGMDS_HPP_UTILS_
#define _OGMDS_HPP_UTILS_

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include <queue>
#include <string>

//! \brief			Utility namespace. It contains constants and utility functions.
namespace OGUtils {

/*--------------------------------------------------------------------------------------------------------------------*/
/* CONSTANTS ---------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

	const double						KILO {1024};							//!< Kilo factor (10^3).
	const double						MEGA {KILO * KILO};						//!< Mega factor (10^6).
	const double						GIGA {KILO * MEGA};						//!< Giga factor (10^9).
	const double						TERA {MEGA * MEGA};						//!< Tera factor (10^12).
	const double						MILLI {.001};							//!< Milli factor (10^-3).
	const double						MICRO {MILLI * MILLI};					//!< Micro factor (10^-6).
	const double						NANO {MILLI * MICRO};					//!< Nano factor (10^-9).

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Split a string depending on a separator character.
//! \param			path				String to split.
//! \param			del					Separator character.
//! \return								Substring vector.
	std::queue <std::string>
	splitString (
		const std::string				path,
		const char						del);

//! \brief			Convert a string with or without prefix into a real value.
//! \param			a					String to convert.
//! \return								Converted value.
	double
	convertPrefix (
		const std::string				a);
};

#endif
