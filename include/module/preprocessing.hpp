/*
 * Copyright UVSQ - CEA/DAM/DIF (2018)
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

//! \file	preprocessing.hpp
//! \brief	Module which handles the simulation launching and the request
//!			routing to the Volume Drivers.

#ifndef _OGSS_PREPROCESSING_HPP_
#define _OGSS_PREPROCESSING_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/types.hpp"

#include <iostream>
#include <memory>
#include <set>

#include "communication/communicationinterface.hpp"
#include "module/module.hpp"
#include "structure/event.hpp"
#include "structure/hardware.hpp"
#include "structure/request.hpp"

#include "util/unitarytest.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

using namespace std;

//! \brief	Module which launches the simulation. Its process consists in
//!			redirecting the whole user request array.
class Preprocessing: public Module {
public:
	friend class UT_Preprocessing;

/*----------------------------------------------------------------------------*/
/* PUBLIC MEMBER FUNCTIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor. Will receive information from the extractor modules.
//! \param	configurationFile	Configuration file.
	Preprocessing (
		const OGSS_String		configurationFile = "");
//! \brief	Destructor.
	~Preprocessing ();

//! \brief			Process during the extraction step.
	void processExtraction ();

//! \brief			Process during the decomposition step.
	void processDecomposition ();

//! \brief			Process during the synchronization step.
	void processSynchronization ();

private:
//! \brief	Manage the events received from the event extraction module. It
//!			converts them into requests and then send them to the targeted
//!			volume drivers.
	void manageEvents ();

	//! \brief	Receive data from the extractor module (workload, hardware).
	inline void receiveData ();

//! \brief	End the simulation by sending to all volumes the end request.
	void endSimulation ();

//! \brief	Make the volume mapping following the receiving of the hardware
//! 		information.
	void updateVolumeMapping ();

//! \brief	Redirect a user request by checking the volume mapping.
//! \param	request				Request.
//! \param	childRequests		Requests coming from the redirection.
	void redirectRequest (
		Request					& request,
		std::vector <Request>	& childRequests);

/*----------------------------------------------------------------------------*/
/* PRIVATE ATTRIBUTES --------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	HardwareParameters			_hardParam;			//!< Hardware parameters.
	std::vector <std::pair <Volume, Device> >
								_volumes;			//!< Volumes/devices.
	OGSS_Ushort					_numRealVolumes;	//!< Number of real volumes (without subvolumes).
	std::vector <Event>			_events;			//!< Vector of events.

	std::map <OGSS_Ulong, OGSS_Ulong>
								_redirectionTable;	//!< Redirection table.
};

/*----------------------------------------------------------------------------*/
/* INLINE MEMBER FUNCTIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
Preprocessing::receiveData () {
	void						* arg;
	void						* arh;
	OGSS_Interlocutor			interlocutor;
	OGSS_Bool					ack;
	OGSS_Ulong					size;

	for (auto i = 2; i != 0; --i) {
		_ci->receive (arg);
		interlocutor = * static_cast <OGSS_Interlocutor*> (arg);
		free (arg);
		
		DLOG(INFO) << "[PP] Reception of data ongoing";

		switch (interlocutor.first) {
		case MTP_HARDWARE:
			DLOG(INFO) << "[PP] Reception from HW module";
			_ci->receive (arg);
			_hardParam = * static_cast <HardwareParameters *> (arg);
			free (arg);
			for (auto idx = 0; idx < _hardParam._numVolumes; ++idx) {
				_ci->receive (arg); _ci->receive (arh);
				_volumes.push_back (make_pair (
					* static_cast <Volume *> (arg),
					* static_cast <Device *> (arh) ) );
				free (arg); free (arh);
			}
			_ci->send (std::make_pair (MTP_HARDWARE, 0), &ack, sizeof (ack) );
			DLOG(INFO) << "[PP] Reception from HW done";
			break;
		case MTP_EVENT:
			DLOG(INFO) << "[PP] Reception from ET module";
			_ci->receive (arg);
			size = * static_cast <OGSS_Ulong *> (arg);
			free (arg);
			for (auto idx = 0; idx < size; ++idx) {
				_ci->receive (arg);
				_events.push_back (* static_cast <Event *> (arg) );
				free (arg);
			}
			_ci->send (std::make_pair (MTP_EVENT, 0), &ack, sizeof (ack) );
			DLOG(INFO) << "[PP] Reception from ET done";
			break;
		default: break;
		}
	}
}

/*----------------------------------------------------------------------------*/
/* UNITARY TEST --------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Unitary tests for Preprocessing.
class UT_Preprocessing:
public UnitaryTest <UT_Preprocessing> {
public:
//! \brief	Default constructor.
//! \param	configurationFile	Configuration file.
	UT_Preprocessing (
		const OGSS_String		& configurationFile);

//! \brief	Destructor.
	~UT_Preprocessing ();

protected:
//! \brief	Targetting the middle of a volume.
//! \return						TRUE on success.
	OGSS_Bool middleVolRequest ();
//! \brief	Targetting a whole volume.
//! \return						TRUE on success.
	OGSS_Bool fullVolRequest ();
//! \brief	Targetting the start of a volume.
//! \return						TRUE on success.
	OGSS_Bool startVolRequest ();
//! \brief	Targetting the end of a volume.
//! \return						TRUE on success.
	OGSS_Bool endVolRequest ();
//! \brief	Targetting multiple volumes.
//! \return						TRUE on success.
	OGSS_Bool multiVolRequest ();
};

#endif
