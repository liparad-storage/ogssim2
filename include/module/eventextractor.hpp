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

//! \file	eventextractor.hpp
//! \brief	Definition of the event extraction module class.

#ifndef _OGSS_EVENTEXTRACTOR_HPP_
#define _OGSS_EVENTEXTRACTOR_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <memory>
#include <utility>
#include <vector>

#include "communication/communicationinterface.hpp"
#include "module/module.hpp"
#include "structure/event.hpp"
#include "structure/types.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

//! \brief	The event extraction module is in charge of extracting the events
//!			from OGSSim configuration file. The events are then sent to the
//!			preprocessing module.
class EventExtractor: public Module {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//!	\param	configurationFile	Path to the configuration file.
	EventExtractor (
		const OGSS_String		& configurationFile);

//! \brief	Destructor.
	~EventExtractor ();

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

//! \brief	Send the events data to the preprocessing module.
	inline void sendData ();

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	std::vector <Event>			_events;			//!< Events.
};

/*----------------------------------------------------------------------------*/
/* INLINE FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
EventExtractor::sendData () {
	void						* arg;
	OGSS_Ulong					size {_events.size ()};

	_ci->send (std::make_pair (MTP_PREPROCESSING, 0),
		&_id, sizeof (_id), true);
	if (size != 0)
		_ci->send (std::make_pair (MTP_PREPROCESSING, 0),
			&size, sizeof (size), true);
	else
		_ci->send (std::make_pair (MTP_PREPROCESSING, 0),
			&size, sizeof (size) );

	DLOG(INFO) << "[ET] Sending " << size << " events to PP module";

	for (auto & elt: _events)
		_ci->send (std::make_pair (MTP_PREPROCESSING, 0),
			&elt, sizeof (elt), --size);

	DLOG(INFO) << "[ET] Sending done";

	_ci->receive (arg); free (arg);
}

#endif
