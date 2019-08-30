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

//! \file	hardwareextractor.hpp
//! \brief	Module which extracts all the parameters of the hardware
//!			configuration.

#ifndef _OGSS_HARDWAREEXTRACTOR_HPP_
#define _OGSS_HARDWAREEXTRACTOR_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <memory>

#include "communication/communicationinterface.hpp"
#include "module/module.hpp"
#include "structure/hardware.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

//!	\brief	Represents the hardware extraction module.
class HardwareExtractor: public Module {

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

public:
//	friend class UT_ArchitectureExtractor;

	//! \brief	Constructor (default).
	//! Initializes the class context, by retrieving from the configuration file
	//! the type of the communication model which will be used during the
	//! simulation, to connect to the manager and reserve a communication pipe.
	//! It also retrieves the path to the trace file, for a further extraction.
	//! \param	configurationFile	Path to the configuration file.
	HardwareExtractor (
		const OGSS_String		configurationFile = "");

	//! \brief	Destructor.
	~HardwareExtractor ();

//! \brief			Process during the extraction step.
	void processExtraction ();

//! \brief			Process during the decomposition step.
	void processDecomposition ();

//! \brief			Process during the synchronization step.
	void processSynchronization ();

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

private:
	//! \brief	Hardware extraction.
	//! Extracts the simulated system contained in the architecture files, and
	//!	stores them in a data structure.
	//! \param	hardwareFile	Path to the trace file.
	void extract (
		const OGSS_String		hardwareFile);

	//! \brief	Application of the data unit on the hardware data
	//!	In case the global data unit is different from the hardware one,
	//!	apply a coefficient equals to local/global to the hardware parameters
	void applyDataUnit ();

	//!	\brief	Data structure transmission.
	//!	Sends the architecture data structure to the pre-processing module once
	//! the extraction done.
	inline void sendData ();

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	Hardware					_hardware;			//!< Hardware data structure.
	OGSS_DataUnit				_globalDU;			//!< Global data unit
};

/*----------------------------------------------------------------------------*/
/* INLINE FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
HardwareExtractor::sendData () {
	void						* arg;
	OGSS_Interlocutor			myself = std::make_pair (MTP_HARDWARE, 0);
	OGSS_Bool					ack;
	OGSS_Ulong					idy = 0;
	OGSS_Ushort					devCnt = 0;
	OGSS_Ushort					volCnt = 0;
	OGSS_Ushort					numRealVolumes = 0;

	for (auto i = 0; i < _hardware._param._numVolumes; ++i) {
		i += _hardware._volumes [i] ._numSubVolumes;
		++ numRealVolumes;
	}

	// Sendings to Volume drivers
	for (auto i = 0; i < numRealVolumes; ++i, ++volCnt) {
		_ci->send (std::make_pair (MTP_VOLUME, i),
			&devCnt, sizeof (OGSS_Ushort), true);
		_ci->send (std::make_pair (MTP_VOLUME, i),
			&_hardware._volumes [volCnt], sizeof (Volume), true);
		for (auto j = volCnt + 1;
			j <= volCnt + _hardware._volumes [volCnt] ._numSubVolumes; ++j) {
			_ci->send (std::make_pair (MTP_VOLUME, i),
				&_hardware._volumes [j], sizeof (Volume), true);
		}
		_ci->send (std::make_pair (MTP_VOLUME, i),
			&_hardware._devices [devCnt], sizeof (Device) );

		// Sendings to Device drivers
		_ci->send (std::make_pair (MTP_DEVICE, i),
			&_hardware._devices [devCnt], sizeof (Device) );

		devCnt += _hardware._volumes [volCnt] ._numDevices;
		volCnt += _hardware._volumes [volCnt] ._numSubVolumes;
	}

	for (auto i = 0; i < 2 * numRealVolumes; ++i)
		{ _ci->receive (arg); free (arg); }

	// Sendings to Execution module
	_ci->send (std::make_pair (MTP_EXECUTION, 0),
		&_hardware._param, sizeof (HardwareParameters), true);
	for (auto idx = 0; idx < _hardware._param._numTiers; ++idx)
		_ci->send (std::make_pair (MTP_EXECUTION, 0),
			&_hardware._tiers [idx], sizeof (Tier), true);
	for (auto idx = 0; idx < _hardware._param._numVolumes; ++idx)
		_ci->send (std::make_pair (MTP_EXECUTION, 0),
			&_hardware._volumes [idx], sizeof (Volume), true);
	idy = 0;
	for (auto idx = 0; idx < _hardware._param._numVolumes; ++idx) {
		if (! _hardware._volumes [idx] ._isSubVolume) {
			_ci->send (std::make_pair (MTP_EXECUTION, 0),
				&_hardware._devices [idy], sizeof (Device), true);
			idy += _hardware._volumes [idx] ._numDevices;
		}
	}
	for (auto idx = 0; idx < _hardware._param._numInterfaces; ++idx)
		if (idx < _hardware._param._numInterfaces - 1)
			_ci->send (std::make_pair (MTP_EXECUTION, 0),
				&_hardware._interfaces [idx], sizeof (Interface), true);
		else
			_ci->send (std::make_pair (MTP_EXECUTION, 0),
				&_hardware._interfaces [idx], sizeof (Interface) );
	_ci->receive (arg); free (arg);

	_ci->send (std::make_pair (MTP_EVALUATION, 0),
		& _hardware._param, sizeof (HardwareParameters) );
	_ci->receive (arg); free (arg);

	// Sendings to Synchronization module
	_ci->send (std::make_pair (MTP_SYNCHRONIZATION, 0),
		&myself, sizeof (myself), true);
	_ci->send (std::make_pair (MTP_SYNCHRONIZATION, 0),
		&_hardware._param, sizeof (HardwareParameters), true);
	for (auto idx = 0; idx < _hardware._param._numTiers; ++idx)
		_ci->send (std::make_pair (MTP_SYNCHRONIZATION, 0),
			&_hardware._tiers [idx], sizeof (Tier), true);
	for (auto idx = 0; idx < _hardware._param._numVolumes; ++idx)
		_ci->send (std::make_pair (MTP_SYNCHRONIZATION, 0),
			&_hardware._volumes [idx], sizeof (Volume), true);
	idy = 0;
	for (auto idx = 0; idx < _hardware._param._numVolumes; ++idx) {
		if (! _hardware._volumes [idx] ._isSubVolume) {
			_ci->send (std::make_pair (MTP_SYNCHRONIZATION, 0),
				&_hardware._devices [idy], sizeof (Device), true);
			idy += _hardware._volumes [idx] ._numDevices;
		}
	}
	for (auto idx = 0; idx < _hardware._param._numInterfaces; ++idx)
		if (idx < _hardware._param._numInterfaces - 1)
			_ci->send (std::make_pair (MTP_SYNCHRONIZATION, 0),
				&_hardware._interfaces [idx], sizeof (Interface), true);
		else
			_ci->send (std::make_pair (MTP_SYNCHRONIZATION, 0),
				&_hardware._interfaces [idx], sizeof (Interface) );
	_ci->receive (arg); free (arg);

	// Sendings to Preprocessing module
	_ci->send (std::make_pair (MTP_PREPROCESSING, 0),
		&myself, sizeof (myself), true);
	_ci->send (std::make_pair (MTP_PREPROCESSING, 0),
		&_hardware._param, sizeof (HardwareParameters), true);
	idy = 0;
	for (auto idx = 0; idx < _hardware._param._numVolumes; ++idx) {
		_ci->send (std::make_pair (MTP_PREPROCESSING, 0),
			&_hardware._volumes [idx], sizeof (Volume), true);
		if (idx != _hardware._param._numVolumes - 1)
			_ci->send (std::make_pair (MTP_PREPROCESSING, 0),
				&_hardware._devices [idy], sizeof (Device), true);
		else
			_ci->send (std::make_pair (MTP_PREPROCESSING, 0),
				&_hardware._devices [idy], sizeof (Device) );
		if (_hardware._volumes [idx] ._type == VTP_DECRAID)
			idy += _hardware._volumes [idx] ._numRedundancyDevices;
		else
			idy += _hardware._volumes [idx] ._numDevices;
	}
	_ci->receive (arg); free (arg);
}

#endif
