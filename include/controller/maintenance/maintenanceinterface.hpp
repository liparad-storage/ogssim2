//!	\file	maintenanceinterface.hpp
//!	\brief	Interface of device maintenance algorithms (such as HDD
//!			defragmentation of SSD wear levelling / garbage collection).

#ifndef _OGSS_MAINTENANCEINTERFACE_HPP_
#define _OGSS_MAINTENANCEINTERFACE_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <vector>

#include "structures/types.hpp"

//!	\brief	This class can possess three different kinds of process:
//!			- SYNC -- synchronous (following a given time step);
//!			- ASYC -- asynchronous (after all request processing);
//!			- EVNT -- event (at a given date).
//!			Each class can be of any combination of those three, by indicating
//!			the corresponding maintenance mode.

class MaintenanceInterface {

public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//!	\brief	Constructor. Default parameters are asynchronous mode without
//!			time step.
//! \param	mode		Maintenance mode.
//!	\param	timeStep	Time between two different launches (for synchronous
//!						mode only).
	MaintenanceInterface (
		const OGSS_MaintenanceMode		mode = MTM_ASYC,
		const OGSS_Real					timeStep = 0);

	virtual ~MaintenanceInterface () = default;

//!	\brief	Synchronous process.
//!	\param	date		Launching date.
//!	\param	requests	Generated requests during the process.
//!	\return				Output code.
	virtual OGSS_MaintenanceOutcode syncLaunch (
		const OGSS_Real					date,
		std::vector <OGSS_Ulong> &		requests) = 0;

//!	\brief	Asynchronous process.
//!	\param	idxRequest	Index of the processed request by the driver.
//!	\param	requests	Generated requests during the process.
//!	\return				Output code.
	virtual OGSS_MaintenanceOutcode asycLaunch (
		const OGSS_Ulong				idxRequest,
		std::vector <OGSS_Ulong> &		requests) = 0;

//!	\brief	Event process.
//!	\param	date		Launching date.
//!	\param	requests	Generated requests during the process.
//!	\return				Output code.
	virtual OGSS_MaintenanceOutcode evntLaunch (
		const OGSS_Real					date,
		std::vector <OGSS_Ulong> &		requests) = 0;

//!	\brief	Data structure update.
//!	\param	type		Update type requested.
//!	\param	data		Data to update.
	virtual void update (
		const OGSS_UpdateType			type,
		const OGSS_UpdateData			data) = 0;

//!	\brief	Getter for maintenance mode.
//!	\return				Maintenance mode.
	inline const OGSS_MaintenanceMode & getMode ()
		{ return _mode; }

//!	\brief	Getter for time step.
//!	\return				Time step.
	inline const OGSS_Real & getTimeStep ()
		{ return _timeStep; }

protected:

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	OGSS_MaintenanceMode _mode;			//!< Maintenance mode.
	OGSS_Real _timeStep;				//!< Time step.
};

#endif