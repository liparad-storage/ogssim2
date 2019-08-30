/*
 * Copyright UVSQ - CEA/DAM/DIF (2017-2018)
 * Contributors:  Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *                Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published per the Free Software Foundation; either
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

//! \file	devicedriver.hpp
//! \brief	Definition of the device driver.

#ifndef _OGSS_DEVICEDRIVER_HPP_
#define _OGSS_DEVICEDRIVER_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <memory>

#include "communication/communicationinterface.hpp"

#include "controller/devicecontroller.hpp"
#include "module/module.hpp"

#include "structure/devicestate.hpp"
#include "structure/hardware.hpp"
#include "structure/types.hpp"

//! \brief	Device module is composed of two submodules: the driver and the
//! 		controller. The driver is the submodule which handles communication
//! 		with other modules (volume & execution), receiving requests
//! 		to process. When a request is received, it will asks the controller
//! 		to decompose it before sending it to the execution module.
class DeviceDriver: public Module {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//!	\param	configurationFile	Path to the configuration file.
//! \param	idx					Device index.
	DeviceDriver (
		const OGSS_String		configurationFile,
		const OGSS_Ushort		idx);

//! \brief	Destructor.
	~DeviceDriver ();

//! \brief	Extraction process.
	void processExtraction ();

//! \brief	Simulation process of the device driver. Consists in translating the
//!			logical addresses of the requests and performing the device
//!			maintenance routine.
	void processDecomposition ();

//! \brief	Synchronization process.
	void processSynchronization ();

private:

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Data reception (device parameters).
	inline void receiveData ();

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	std::unique_ptr <DeviceController>
								_ctrl;				//!< Device controller.
	Device						_device;			//!< Device parameters.
	std::map <OGSS_Long, OGSS_DeviceState>
								_deviceState;		//!< Devices state.
	OGSS_Bool					_syncOTF;			//!< TRUE if there is a synchronization step
													//!< because of on-the-fly reconstruction.
	OGSS_Bool					_syncProc {false};	//!< Inform if the synchronization step was processed.
};

/*----------------------------------------------------------------------------*/
/* INLINE FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
DeviceDriver::receiveData () {
	void						* arg;
	OGSS_Bool					ack;

	_ci->receive (arg);
	_device = * static_cast <Device *> (arg);
	free (arg);
	ack = true;
	_ci->send (std::make_pair (MTP_HARDWARE, 0), &ack, sizeof (ack) );
}

#endif
