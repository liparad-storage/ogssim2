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

//! \file	eventextractor.cpp
//! \brief	Definition of the event extraction module class.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <glog/logging.h>

#include "module/eventextractor.hpp"

#include "parser/xmlparser.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

EventExtractor::EventExtractor (
	const OGSS_String		& configurationFile):
	Module (configurationFile, make_pair (MTP_EVENT, 0) ) {  }

EventExtractor::~EventExtractor () {  }

void
EventExtractor::processExtraction () {
	_events = XMLParser::getEvents (_cfg);

	LOG(INFO) << "Number of retrieved events: " << _events.size ();

	sendData ();
}

void
EventExtractor::processDecomposition () {  }

void
EventExtractor::processSynchronization () {  }
