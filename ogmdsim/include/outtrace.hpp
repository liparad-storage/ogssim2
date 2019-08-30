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

//! \file			outtrace.hpp
//! \brief			Definition of the serializer namespace. This namespace is used to write the new trace file once
//!					the metadata requests are processed.

#ifndef _OGMDS_HPP_OUTTRACE_
#define _OGMDS_HPP_OUTTRACE_

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include <string>
#include <vector>

#include "mdrequest.hpp"

//! \brief			Serializer namespace. It only contains the export function to create the new trace file.
namespace OutputTrace {

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Create the output trace file. The format is the one used in OGSSim.
//! \param			req					Metadata request vector.
//! \param			path				Output file path.
	void
	exportDataRequests (
		const std::vector <MDRequest>	& req,
		const std::string				path);
};

#endif