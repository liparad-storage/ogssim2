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

//! \file			xmlextract.hpp
//! \brief			Definition of the XML parser class. This class is used to read the system and OGMDSim configuration
//!					files.

#ifndef _OGMDS_HPP_XMLEXTRACT_
#define _OGMDS_HPP_XMLEXTRACT_

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include <tinyxml2.h>

#include <queue>
#include <string>
#include <vector>

/*--------------------------------------------------------------------------------------------------------------------*/
/* CONSTANTS ---------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Definition of the XML files compliant with the parser.
enum OGFileType {
	OGFT_CFGFILE,																//!< Configuration file type.
	OGFT_SYSFILE																//!< System file type.
};

//! \brief			XML parser class which is used by two functions: getXMLItem() and getXMLList(). During the
//!					initialization step, the files are opened and loaded in memory, to allow any module to access them
//!					at any time by provided the path to the requested information.
class OGXML {
public:

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Constructor.
//! \param			cfgPath				Path to the configuration file.
	OGXML (
		const std::string				cfgPath);

//! \brief			Destructor.
	~OGXML ();

//! \brief			Getter for the first requested item of the given type.
//! \param			value				Requested value.
//! \param			file				File which contains the value.
//! \param			path				XML tag which contains the value.
//! \param			attribute			TRUE if the tag is an attribute.
//! \return								TRUE if the requested value was found.	
	template <typename T>
	bool getXMLItem (
		T								& value,
		const OGFileType				file,
		const std::string				path,
		const bool						attribute = false);

//! \brief			Getter for all the requested items of the given type.
//! \param			vec					Vector of requested values.
//! \param			file				File which contains the values.
//! \param			path				XML tag which contains the values.
//! \return								TRUE if at least one requested value was found.
	template <typename T>
	bool getXMLList (
		std::vector <T>					& vec,
		const OGFileType				file,
		const std::string				path);

private:

/*--------------------------------------------------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS -------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Browse the file following the path to go to the last tag.
//! \param			q					Split parts of the path.
//! \param			file				File to browse.
//! \param			path				Path to browse.
//! \param			attribute			TRUE if the last node to find is an attribute.
	tinyxml2::XMLElement * goToLastNode (
		std::queue <std::string>		& q,
		const OGFileType				file,
		const std::string				path,	
		const bool						attribute = false);

/*--------------------------------------------------------------------------------------------------------------------*/
/* ATTRIBUTES --------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

	std::string							_cfgPath {""};							//!< Path to the configuration file.
	std::string							_sysPath {""};							//!< Path to the system file.

	tinyxml2::XMLDocument				_cfgDoc;								//!< Configuration file document.
	tinyxml2::XMLDocument				_sysDoc;								//!< System file document.
};

#include "xmlextract.tpp"

#endif
