//!	\file	maintenanceinterface.cpp
//!	\brief	Interface of device maintenance algorithms (such as HDD
//!			defragmentation of SSD wear levelling / garbage collection).

#include "drivers/maintenance/maintenanceinterface.hpp"

MaintenanceInterface::MaintenanceInterface (
	const OGSS_MaintenanceMode		mode,
	const OGSS_Real					timeStep):
	_mode (mode), _timeStep (timeStep) {  }
