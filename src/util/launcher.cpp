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

//! \file	launcher.cpp
//! \brief	Contains all the launchers used by the main process of OGSSim.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "communication/communicationinterfaceizmq.hpp"
#include "communication/communicationinterfacempi.hpp"
#include "communication/communicationinterfacezmq.hpp"
#include "communication/communicationmanagerizmq.hpp"
#include "communication/communicationmanagermpi.hpp"
#include "communication/communicationmanagerzmq.hpp"

#include "util/launcher.hpp"
#include "util/unitarytest.hpp"

#include "parser/xmlparser.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

using namespace std;

/*----------------------------------------------------------------------------*/
/* LAUNCHERS------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

shared_ptr <CommunicationInterface>
instanciateCommunicationInterface (
	const OGSS_String		configurationFile,
	const OGSS_Interlocutor	interlocutor) {
	OGSS_CommType			ctype;

	ctype = XMLParser::getCommunicationType (configurationFile);

	switch (ctype) {
		case CTP_ZMQ:
			return make_shared <CI_ZMQ> (configurationFile, interlocutor);
		case CTP_IZMQ:
#ifdef OGSSMPI
			return make_shared <CI_IZMQ> (configurationFile, interlocutor);		
#endif
		case CTP_MPI:
#ifdef OGSSMPI
			return make_shared <CI_MPI> (configurationFile, interlocutor);
#endif
		default:
			return make_shared <CI_ZMQ> (configurationFile, interlocutor);
	}

	return nullptr;
}

void
launchCommunicationManager (
	const OGSS_String		configurationFile) {
	OGSS_CommType			ctype;
	unique_ptr <CommunicationManager> m;

	DLOG(INFO) << "[CM] COM Manager online!";

	ctype = XMLParser::getCommunicationType (configurationFile);

	switch (ctype) {
		case CTP_ZMQ:
			m = make_unique <CM_ZMQ> (configurationFile);
			break;
		case CTP_IZMQ:
#ifdef OGSSMPI
			m = make_unique <CM_IZMQ> (configurationFile);
			break;
#else
			DLOG(INFO) << "The IZMQ (MPI+ZMQ) model is only available in the "
				<< "MPI version of OGSSim. Replaced here by ZMQ model.";
			m = make_unique <CM_ZMQ> (configurationFile);
			break;
#endif
		case CTP_MPI:
#ifdef OGSSMPI
			m = make_unique <CM_MPI> (configurationFile);
			break;
#else
			DLOG(INFO) << "The MPI model is only available in the MPI version "
				<< "of OGSSim. Replaced here by ZMQ model.";
#endif
		default:
			m = make_unique <CM_ZMQ> (configurationFile);
	}

	m->listen ();

	LOG(INFO) << "[CM] END";
}

void
launchUnitaryTests (
	const OGSS_String		configurationFile) {
}
