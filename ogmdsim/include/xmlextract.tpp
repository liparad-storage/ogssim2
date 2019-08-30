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

//! \file			xmlextract.tpp
//! \brief			Definition of the XML parser class. This class is used to read the system and OGMDSim configuration
//!					files.

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include <iostream>

#include <glog/logging.h>

#include "utils.hpp"

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

template <typename T> bool
OGXML::getXMLItem (
	T									& value,
	const OGFileType					file,
	const std::string					path,
	const bool							attribute) {
	LOG(WARNING) << "Type not implemented (using gXI with '" << path << "')";
	return false;
}

template <typename T> bool
OGXML::getXMLList (
	std::vector <T>						& vec,
	const OGFileType					file,
	const std::string					path) {
	LOG(WARNING) << "Type not implemented (using gXL with '" << path << "')";
	return false;
}

template <> bool
OGXML::getXMLItem (
	bool 								& value,
	const OGFileType					file,
	const std::string					path,
	const bool							attribute);

template <> bool
OGXML::getXMLItem (
	double 								& value,
	const OGFileType					file,
	const std::string					path,
	const bool							attribute);

template <> bool
OGXML::getXMLItem (
	int 								& value,
	const OGFileType					file,
	const std::string					path,
	const bool							attribute);

template <> bool
OGXML::getXMLItem (
	std::string 						& value,
	const OGFileType					file,
	const std::string					path,
	const bool							attribute);

template <> bool
OGXML::getXMLItem (
	unsigned	 						& value,
	const OGFileType					file,
	const std::string					path,
	const bool							attribute);

template <> bool
OGXML::getXMLItem (
	uint64_t							& value,
	const OGFileType					file,
	const std::string					path,
	const bool							attribute);

template <> bool
OGXML::getXMLList (
	std::vector <bool>					& vec,
	const OGFileType					file,
	const std::string					path);

template <> bool
OGXML::getXMLList (
	std::vector <double>				& vec,
	const OGFileType					file,
	const std::string					path);

template <> bool
OGXML::getXMLList (
	std::vector <int>					& vec,
	const OGFileType					file,
	const std::string					path);

template <> bool
OGXML::getXMLList (
	std::vector <std::string>			& vec,
	const OGFileType					file,
	const std::string					path);

template <> bool
OGXML::getXMLList (
	std::vector <unsigned>				& vec,
	const OGFileType					file,
	const std::string					path);

template <> bool
OGXML::getXMLList (
	std::vector <unsigned>				& vec,
	const OGFileType					file,
	const std::string					path);
