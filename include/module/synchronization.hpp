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

//! \file	synchronization.hpp
//! \brief	Definition of the synchronization module class.

#ifndef _OGSS_SYNCHRONIZATION_HPP_
#define _OGSS_SYNCHRONIZATION_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "communication/communicationinterface.hpp"
#include "module/module.hpp"
#include "structure/hardware.hpp"
#include "structure/types.hpp"
	
#include "synchronization/synchronizationmodel.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

//! \brief	The synchronization module computes the request waiting times
//!			according to a given model.
class Synchronization: public Module {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	configurationFile	Path to the configuration file.
	Synchronization (
		const OGSS_String		configurationFile);

//! \brief	Destructor.
	~Synchronization ();

//! \brief			Process during the extraction step.
	void processExtraction ();

//! \brief			Process during the decomposition step.
	void processDecomposition ();

//! \brief			Process during the synchronization step.
	void processSynchronization ();

private:

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Synchronization model initialization. The model is chosen from
//!			OGSSim configuration file.
	void initSyncModel ();

//! \brief	Data reception from the workload and the hardware extraction modules.
	inline void receiveData ();

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	OGSS_String					_outputFile;		//!< Detailled output file.
	OGSS_String					_resumeFile;		//!< Resume output file.

	OGSS_DataUnit				_globalDU;			//!< Global data unit.

	HardwareParameters			_hardParam;			//!< Hardware parameters.

	std::vector <Tier>			_tiers;				//!< Tiers.
	std::vector <Volume>		_volumes;			//!< Volumes.
	std::vector <Device>		_devices;			//!< Devices.
	std::vector <Interface>		_interfaces;		//!< Interfaces.

	OGSS_Ushort					_numRealVolumes;	//!< Number of real volumes.

	OGSS_Ulong					_numLogicalRequests;//!< Number of logical requests.

	std::unique_ptr <SynchronizationModel>
								_sync;				//!< Synchronization model.

	std::vector <RequestStat>	_evtStats;			//!< Event stats.
};

/*----------------------------------------------------------------------------*/
/* INLINE FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
Synchronization::receiveData () {
	void						* arg;
	OGSS_Bool					ack;
	OGSS_Interlocutor			interlocutor;

	for (auto i = 2; i != 0; --i) {
		_ci->receive (arg);
		interlocutor = * static_cast <OGSS_Interlocutor *> (arg); free (arg);

		switch (interlocutor.first) {
		case MTP_WORKLOAD:
			_ci->receive (arg);
			_numLogicalRequests = * static_cast <OGSS_Ulong *> (arg); free (arg);

			_ci->send (std::make_pair (MTP_WORKLOAD, 0), &ack, sizeof (ack) );
			break;

		case MTP_HARDWARE:
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
			break;
		default:
			LOG (INFO) << "Undefined: " << interlocutor.first << "/" << interlocutor.second;
		break;
		}
	}
}

#endif
