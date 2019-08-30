/*
 * Copyright UVSQ - CEA/DAM/DIF (2018)
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

//! \file	volumedriver.hpp
//! \brief	Definition of the volume driver.

#ifndef _OGSS_VOLUMEDRIVER_HPP_
#define _OGSS_VOLUMEDRIVER_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <memory>

#include "communication/communicationinterface.hpp"

#include "controller/volumecontroller.hpp"
#include "module/module.hpp"
#include "structure/hardware.hpp"
#include "structure/types.hpp"

//! \brief	Volume module is composed of two submodules: the driver and the
//! 		controller. The driver is the submodule which handles communication
//! 		with other modules (preprocessing & device), receiving requests
//! 		to process. When a request is received, it will asks the controller
//! 		to decompose it before sending it to the device module.
class VolumeDriver: public Module {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	configurationFile	Path to the configuration file.
//! \param	idx					Volume index.
	VolumeDriver (
		const OGSS_String		configurationFile,
		const OGSS_Ushort		idx);

//! \brief	Destructor.
	virtual ~VolumeDriver ();

//! \brief			Process during the extraction step.
	void processExtraction ();

//! \brief			Process during the decomposition step.
	void processDecomposition ();

//! \brief			Process during the synchronization step.
	void processSynchronization ();

protected:

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Generation of event requests
//! \param	event				Event to process.
//! \param	numBlocks			Number of blocks to reconstruct.
//! \param	subrequests 		Reconstruction requests.
//! \return						
	OGSS_Bool generateEventRequests (
		Request							& event,
		OGSS_Ulong						numBlocks,
		std::vector <Request>			& subrequests);

//! \brief	Data reception during the initialization process.
	virtual void receiveData ();

/*----------------------------------------------------------------------------*/
/* PRIVATE ATTRIBUTES --------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	std::unique_ptr <VolumeController>	_ctrl;		//!< Volume controller (layout).
	OGSS_Ushort					_firstDevIdx;		//!< Index of the first device
													//!< which belongs to the volume.
	Volume						_vol;				//!< Volume parameters.
	std::vector <Volume>		_subVols;			//!< Subvolumes (in case of
													//!< declustered RAID).
	std::vector <VolumeController>
								_subCtrls;			//!< Subvolume controllers (in
													//!< case of declustered RAID).
	Device						_dev;				//!< Device parameters.
	OGSS_Bool					_syncOTF;			//!< TRUE if there is a synchronization step
													//!< because of on-the-fly reconstruction.
	OGSS_Ushort					_numEvents {0};		//!< Number of events to process.
	OGSS_Ulong					_lastLink {0};		//!< ID used during the generation.

	std::map <OGSS_Ulong, Request>
								_lastEventBlockOTF;	//!< Last block generated for each event.
	std::map <OGSS_Ulong, OGSS_Ulong>
								_evCounter;			//!< Counter for blocks that need to be processed.
	std::map <OGSS_Ulong, std::pair <OGSS_Bool, OGSS_Ulong>>
								_evStarter;			//!< Event starters used during the generation.
	std::map <OGSS_Ulong, OGSS_DeviceState>
								_deviceState;		//!< Devices state.

	OGSS_Real					_fillingRate = .2;	//!< Device filling rate ie. how many
													//!< blocks need to be reconstructed.
};

#endif
