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

//! \file	txmlparser.hpp
//! \brief	Namespaces used for XML parsing. Current version brings support for
//!			the communication module, the file paths, the unitary tests and the
//!			hardware configuration extraction. This parser uses tiny-xml library.

#ifndef _OGSS_TXMLPARSER_HPP_
#define _OGSS_TXMLPARSER_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/types.hpp"

#if USE_TINYXML

#include <set>

#include "structure/event.hpp"
#include "structure/graph.hpp"
#include "structure/hardware.hpp"

/*----------------------------------------------------------------------------*/
/* NAMESPACE -----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Namespace for XML parsing functions.
namespace XMLParser {

//! \brief	Get the path of the given file type by searching through the
//! configuration file.
//! \param	configurationFile	Configuration file.
//! \param	fileType			Searched file type.
//! \param	input				TRUE if searching an input filepath, FALSE for output.
//! \return						Searched file path.
	OGSS_String getFilePath (
		const OGSS_String		& configurationFile,
		const OGSS_FileType		& fileType,
		const OGSS_Bool			& input = true);

//! \brief	Get the data unit for the given parameter type.
//! \param	configurationFile	Path to the configuration file.
//! \param	paramType			Searched parameter type.
//! \return						Requested data unit.
	OGSS_DataUnit getDataUnit (
		const OGSS_String		& configurationFile,
		const OGSS_ParamType	& paramType);

//! \brief	Get the value of the given parameter type by searching through the
//! configuration file.
//! \param	configurationFile	Configuration file.
//! \param	paramType			Searched parameter type.
//! \return						Searched parameter value.
	OGSS_String getConfigParameter (
		const OGSS_String		& configurationFile,
		const OGSS_ParamType	& paramType);

//! \brief	Get the graphs requested by the user as output files.
//! \param	configurationFile	Path to the configuration file.
//! \return						Requested graphs.
	std::vector <Graph> getGraphs (
		const OGSS_String		& configurationFile);

//! \brief	Get the events that will occur during the simulation.
//! \param	configurationFile	Path to the configuration file.
//! \return 					Event list.
	std::vector <Event> getEvents (
		const OGSS_String		& configurationFile);

//! \brief	Get the computation model for the given parameter type.
//! \param	configurationFile	Path to the configuration file.
//! \param	paramType			Searched parameter type.
//! \return						Requested computation model.
	OGSS_String getComputationModel (
		const OGSS_String		& configurationFile,
		const OGSS_ParamType	& paramType);

//! \brief	Get the value of the port used for the communication by searching
//! through the configuration file.
//! \param	configurationFile	Configuration file.
//! \return						Searched port value.
	OGSS_Ulong getCommunicationPort (
		const OGSS_String		& configurationFile);

//! \brief	Get the type of the communication by searching through the
//! configuration file.
//!	\param	configurationFile	Configuration file.
//!	\return						Communication type.
	OGSS_CommType getCommunicationType (
		const OGSS_String		& configurationFile);

//! \brief	Get the OGMDSim information (plugin) from the configuration file.
//! \param	configurationFile	Configuration file.
//! \return						A tuple consisting of a boolean (TRUE if the call
//!								of OGMDSim is requested), the path of the OGMDSim
//!								plugin and the path of the OGMDSim configuration file.
	std::tuple <OGSS_Bool, OGSS_String, OGSS_String> getOGMDSInformation (
		const OGSS_String		& configurationFile);

//! \brief	Get the list of unitary instances requested by the user in the
//!			configuration file.
//! \param	configurationFile	Configuration file.
//! \param	uiNames				Unitary instance names.
//! \return						TRUE if there is at least one requested instance.
	OGSS_Bool getListOfRequestedUnitaryInstances (
		const OGSS_String		& configurationFile,
		std::set <OGSS_ModuleType>	& uiNames);

//! \brief	Get the list of requested tests for the given instance.
//! \param	configurationFile	Configuration file.
//! \param	uiName				Unitary instance name.
//! \param	testNames			Test names.
//! \return						TRUE if there is at least one requested test.
	OGSS_Bool getListOfRequestedUnitaryTests (
		const OGSS_String		& configurationFile,
		const OGSS_ModuleType	& uiName,
		std::set <OGSS_String>	& testNames);

//! \brief	Extract the hardware configuration from the respective file.
//! \param	hardwareFile		Hardware configuration file.
//! \param	hardware			Hardware data structure.
//! \return						TRUE if the file exists.
	OGSS_Bool getHardwareConfiguration (
		const OGSS_String		& hardwareFile,
		Hardware				& hardware);

//! \brief	Extract the number of volumes in the simulated hardware system.
//! \param	hardwareFile		Hardware configuration file.
//! \return						Number of volumes.
	OGSS_Short getNumberOfVolumes (
		const OGSS_String		& hardwareFile);
};

#endif
#endif
