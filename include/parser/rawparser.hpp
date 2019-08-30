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

//! \file	rawparser.hpp
//! \brief	Parser for workload files (in RAW format).

#ifndef _OGSS_RAWPARSER_HPP_
#define _OGSS_RAWPARSER_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <tuple>
#include <vector>

#include "structure/types.hpp"
#include "util/unitarytest.hpp"

/*----------------------------------------------------------------------------*/
/* MAIN NAMESPACE ------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Namespace for the RAW parser.
namespace RawParser {
//! \brief	Getter for the number of fields in the workload file.
//! \param	path				Workload file path.
//! \return						Number of fields.
	OGSS_Ushort getNumField (
		const OGSS_String		path);
//! \brief	Extraction of the requests from the workload file.
//! \param	path				Workload file path.
//! \param	requests			Extracted requests.
	void extractRequests (
		const OGSS_String		path,
		std::vector <std::tuple <
			OGSS_Real, OGSS_RequestType, OGSS_Ulong,
			OGSS_Ulong, OGSS_Ushort, OGSS_Ushort> >
								& requests);
};

/*----------------------------------------------------------------------------*/
/* UNITARY TEST --------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Unitary tests for RAW parser.
class UT_RawParser:
public UnitaryTest <UT_RawParser> {
public:
//! \brief	Default constructor.
//! \param	configurationFile	Configuration file.
	UT_RawParser (
		const OGSS_String		& configurationFile);

//! \brief	Destructor.
	~UT_RawParser ();

private:
//! \brief	Extraction on an empty file.
//! \return						TRUE on success.
	OGSS_Bool emptyFile ();
//! \brief	Extraction on a filled file.
//! \return						TRUE on success.
	OGSS_Bool filledFile ();
//! \brief	Extraction on a commented file.
//! \return						TRUE on success.
	OGSS_Bool commentedFile ();
//! \brief	Extraction on a bad-ended file.
//! \return						TRUE on success.
	OGSS_Bool badEndingFile ();
};

#endif
