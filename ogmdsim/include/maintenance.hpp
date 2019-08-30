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

//! \file			maintenance.hpp
//! \brief			Definition of the maintenance classes. The maintenance classes are used by the locator if a
//!					maintenance routine was asked by the user. This maintenance routine is called periodically and
//!					use a metadata migration process to balance the server loads.


#ifndef _MAINTENANCE_HPP_
#define _MAINTENANCE_HPP_

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include <fstream>

#include "system.hpp"

/*--------------------------------------------------------------------------------------------------------------------*/
/* CONSTANTS ---------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Routines available for the maintenance class.
enum MaintenanceType {
	OGMT_THRESHOLD,																//!< Threshold routine.
	OGMT_AVERAGE																//!< Average routine.
};

//! \brief			Structure which contains the limits for the load balancing, used by the maintenance class.
struct MaintenanceLimit {
	double hard;																//!< Hard limit.
	double soft;																//!< Soft limit.
};

//! \brief			Maintenance parent class. Must be inherited to be used. It allows the implementation of a
//!					maintenance routine class.
class Maintenance {
public:

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Constructor.
//! \param			sys					System structure, which gets the server load and the metadata placement.
//! \param			parser				Parser for getting the parameters of the maintenance routine.
	Maintenance (
		System							& sys,
		OGXML							& parser);

//! \brief			Destructor.
	~Maintenance ();

//! \brief			Launch the maintenance routine. Called periodically by the locator. Must be defined
//!					in each child class.
	virtual void
		launch () = 0;

protected:

/*--------------------------------------------------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS -------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Compute the system average load by asking the system data structure.
//! \return								System average load.
	uint64_t
		getAverageLoad ();

//! \brief			Write the introduction of the load balancing data output file. Must be defined in each child class.
	virtual void
		writeInitToOutput () = 0;

//! \brief			Write the system current state to the load balancing data output file.
	void writeToOutput ();

/*--------------------------------------------------------------------------------------------------------------------*/
/* ATTRIBUTES --------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

	System								& _system;								//!< Metadata system.
	OGXML								& _parser;								//!< XML parser.

	bool								_doOutput;								//!< TRUE if the output file
																				//!< is required by the user.
	std::string							_outPath;								//!< Output file path.
	std::ofstream						_output;								//!< Output file.
};

//! \brief			Maintenance child class for the threshold routine. When called, it first checks if a server load
//!					reaches the hard limit. If it does, all the server loads are reduced to the soft limit, if possible,
//!					by migrating metadatas.
class ThresholdMaintenance:
	public Maintenance {
public:

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Constructor.
//! \param			sys					System structure, which gets the server load and the metadata placement.
//! \param			parser				Parser for getting the parameters of the maintenance routine.
	ThresholdMaintenance (
		System							& sys,
		OGXML							& parser);

//! \brief			Destructor.
	~ThresholdMaintenance ();

//! \brief			Launch the maintenance routine. Called periodically by the locator.
	void launch ();

private:

/*--------------------------------------------------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS -------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Check if a server load reaches the hard limit.
//! \return			TRUE if a server load is high enough, meaning that the maintenance routine needs to be done.
	bool check ();

//! \brief			Check if a metadata migration can occur by verifying that a server load is under the soft limit.
//! \return			TRUE if a server load is low enough, meaning that the maintenance routine can be executed.
	bool checkUnderSoftLimit ();

//! \brief			Update the server load history.
	void updateHist ();

//! \brief			Write the introduction of the load balancing data output file. 
	void writeInitToOutput ();

/*--------------------------------------------------------------------------------------------------------------------*/
/* ATTRIBUTES --------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

	MaintenanceLimit					_loadLimit;								//!< Limits used by the routine.

	std::vector <std::queue <uint64_t>>	_hist;									//!< History of the server loads.
	std::vector <uint64_t>				_sLoads;								//!< Sum of the old server loads.
	int									_histSpanlife {0};						//!< History size.
	double								_oblivRate {.2};						//!< Oblivion rate, used in the average
																				//!< server load computation.
};

//! \brief			Maintenance child class for the average routine. When called, it first checks if all the server
//!					loads are near the average server load. If not, a load balancing process is performed by migrating
//!					metadatas.
class AverageMaintenance:
	public Maintenance {
public:

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Constructor.
//! \param			sys					System structure, which gets the server load and the metadata placement.
//! \param			parser				Parser for getting the parameters of the maintenance routine.
	AverageMaintenance (
		System							& sys,
		OGXML							& parser);

//! \brief			Destructor.
	~AverageMaintenance ();

//! \brief			Launch the maintenance routine. Called periodically by the locator.
	void launch ();

private:

/*--------------------------------------------------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS -------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Check if a server load if near the average server load.
//! \return			TRUE if a server load is far from the average server load, meaning that the maintenance routine
//!					needs to be executed.
	bool check ();

//! \brief			Write the introduction of the load balancing data output file. 
	void writeInitToOutput ();
};

#endif
