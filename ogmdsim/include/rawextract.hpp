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

//! \file			rawextract.hpp
//! \brief			Definition of the raw parser namespace. This namespace is used to read the trace
//!					and the matrix files.

#ifndef _OGMDS_HPP_RAWEXTRACT_
#define _OGMDS_HPP_RAWEXTRACT_

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include <string>
#include <vector>

#include "mdrequest.hpp"

//! \brief			Raw parser namespace. It only contains the import functions for the trace and the matrix files.
namespace RawParser {

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Extract the matrix information.
//! \param			matrix				Data structure for the matrix. Each link is represented by a real number.
//! \param			path				Input file path.
	void
	extractMatrix (
		std::vector <std::vector <double>>	& matrix,
		const std::string				path);

//! \brief			Extract the request information.
//! \param			req					Request data structure.
//! \param			path				Input file path.
	void
	extractRequests (
		std::vector <MDRequest>			& req,
		const std::string				path);
};

#endif
