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

//! \file			locator.hpp
//! \brief			Definition of the locator class. The locator is used to determine the location of the targeted
//!					metadata on the metadata servers (address & node ID) and the location and the size of the data on
//!					the logical storage space.

#ifndef _OGMDS_LOCATOR_HPP_
#define _OGMDS_LOCATOR_HPP_

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include <random>
#include <tuple>

#include "maintenance.hpp"
#include "mdrequest.hpp"
#include "system.hpp"

/*--------------------------------------------------------------------------------------------------------------------*/
/* CONSTANTS ---------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Strategies available for the generation of data location and size.
enum Strategy {
	OGST_CONSTANT,																//!< Constant (size only)
	OGST_SEQUENTIAL,															//!< Sequential (location only)
	OGST_UNIFORM																//!< Uniform random
};

//! \brief			The locator can be used by calling the locate() function, which determines the metadata and data
//!					information.
class Locator {
public:

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Constructor.
//! \param			sys					System structure, which gets the server load and the metadata placement.
//! \param			parser				Parser for getting the parameters of the data location and size generation.
	Locator (
		System							& sys,
		OGXML							& parser);

//! \brief			Destructor.
	~Locator ();

//! \brief			Generate the location of the requested metadata and data or search in the system if the location
//!					already exists.
//! \param			req					Request to process.
	void locate (
		MDRequest						& req);

private:

/*--------------------------------------------------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS -------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Initialize the maintenance module after the parsing of its parameters.
	void launchMaintenanceModule ();

//! \brief			Update the system structure with the newly generated data location.
//! \param			start				Data start address.
//! \param			end					Data end address.	
	void dataUpdate (
		uint64_t						start,
		uint64_t						end);

//! \brief			Generate the data location and size.
//!	\param			req					Request to process.
	void dataLocate (
		MDRequest						& req);

//! \brief			Check if the generated location can be used ie. the space is available.
//! \param			start				Data start address.
//! \param			end					Data end address.
//! \return								TRUE if the space is not available.
	bool dataCheck (
		uint64_t						start,
		uint64_t						end);

/*--------------------------------------------------------------------------------------------------------------------*/
/* ATTRIBUTES --------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

	System								& _system;								//!< Metadata system.
	OGXML								& _parser;								//!< XML parser.

	std::unique_ptr <Maintenance>		_maint {nullptr};						//!< Maintenance module.
	double								_maintPeriod {1.};						//!< Maintenance call period.
	double								_lastMaintCall {.0};					//!< Last maintenance call.

	Strategy							_sAddress {OGST_SEQUENTIAL};			//!< Data address generation strategy.
	Strategy							_sSize {OGST_CONSTANT};					//!< Data size generation strategy.

	std::default_random_engine			_gen;									//!< Random engine.

	uint64_t							_constSize {1};							//!< Constant size value.
	uint64_t							_seqNextAddress {0};					//!< Next address to select.
	std::uniform_int_distribution <int>	_unifDistAddress;						//!< Data address generator.
	std::uniform_int_distribution <int>	_unifDistSize;							//!< Data size generator.

	std::map <uint64_t, std::tuple <uint64_t, uint64_t, uint64_t> >
										_translationMap;						//!< Translation map between an object
																				//!< ID and the targeted metadata.

	std::map <uint64_t, uint64_t>		_checkA;								//!< Internal use in dataUpdate() to
																				//!< optimize the algorithm execution
																				//!< time.
	std::map <uint64_t, uint64_t>		_checkR;								//!< Internal use in dataUpdate() to
																				//!< optimize the algorithm execution
																				//!< time.
	std::vector <bool>					_checkFinder;							//!< TRUE if the data unit is used.

	uint64_t							_dataFactor;							//!< Data factor ie. size of the
																				//!< smallest data unit.
	uint64_t							_maxSize;								//!< Logical data space size.
};

#endif
