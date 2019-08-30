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

//! \file	execution.hpp
//! \brief	Definition of the execution module class.

#ifndef _OGSS_EXECUTION_HPP_
#define _OGSS_EXECUTION_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "communication/communicationinterface.hpp"
#include "computation/computationmodel.hpp"
#include "module/module.hpp"
#include "structure/hardware.hpp"
#include "structure/types.hpp"

//! \brief	The execution module computes for each request received from the
//!			the device drivers, their service and transfer times according to
//!			the hardware components used. Then, the requests are sent to the
//!			synchronization module.
class Execution: public Module {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//!	\param	configurationFile	Path to the configuration file.
	Execution (
		const OGSS_String		configurationFile);

//! \brief	Destructor.
	~Execution ();

//! \brief	Process during the extraction step.
	void processExtraction ();

//! \brief	Simulation process of the execution module. In charge of computing
//!			the service and transfer times of each received request.
	void processDecomposition ();

//! \brief	Process during the synchronization step.
	void processSynchronization ();

private:

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Data reception during the initialization step, for the hardware parameters.
	inline void receiveData ();

//! \brief	Request processing.
//!	\param	req					Request to process.
	void treatRequest (
		Request					& req);

//! \brief	Initialization of the computation models, following the information
//!			given in OGSSim configuration file.
//! \param	configurationFile	Path to the configuration file.
	void initComputationModels (
		const OGSS_String		configurationFile);

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	HardwareParameters			_hardParam;			//!< Hardware parameters.

	std::vector <Tier>			_tiers;				//!< Tiers.
	std::vector <Volume>		_volumes;			//!< Volumes.
	std::vector <Device>		_devices;			//!< Devices.
	std::vector <Interface>		_interfaces;		//!< Interfaces.

	OGSS_Ushort					_numRealVolumes;	//!< Number of real volumes.

	std::unique_ptr <ComputationModel>	_hdd;		//!< HDD computation model.
	std::unique_ptr <ComputationModel>	_ssd;		//!< SSD computation model.
	std::unique_ptr <ComputationModel>	_nvram;		//!< NVRAM computation model.
	std::unique_ptr <ComputationModel>	_interface;	//!< Interface computation model.

	OGSS_Bool					_syncOTF;			//!< TRUE if there is an on-the-fly synchronization step.
	OGSS_Bool					_syncProc {false};	//!< Indicate if the synchronization was processed.
};

/*----------------------------------------------------------------------------*/
/* INLINE FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
Execution::receiveData () {
	void						* arg;
	OGSS_Bool					ack;

	_ci->receive (arg);
	_hardParam = * static_cast <HardwareParameters *> (arg); free (arg);

	for (auto idx = 0; idx < _hardParam._numTiers; ++idx) {
		_ci->receive (arg);
		_tiers.push_back (* static_cast <Tier *> (arg) ); free (arg);
	}

	for (auto idx = 0; idx < _hardParam._numVolumes; ++idx) {
		_ci->receive (arg);
		_volumes.push_back (* static_cast <Volume *> (arg) ); free (arg);
	}

	for (auto idx = 0; idx < _hardParam._numVolumes; ++idx) {
		if (! _volumes [idx] ._isSubVolume) {
			_ci->receive (arg);
			for (auto idy = 0; idy < _volumes [idx] ._numDevices; ++idy)
				_devices.push_back (* static_cast <Device *> (arg) );
			free (arg);
		}
	}

	for (auto idx = 0; idx < _hardParam._numInterfaces; ++idx) {
		_ci->receive (arg);
		_interfaces.push_back (* static_cast <Interface *> (arg) ); free (arg);
	}

	ack = true;
	_ci->send (std::make_pair (MTP_HARDWARE, 0), &ack, sizeof (ack) );
}

#endif
