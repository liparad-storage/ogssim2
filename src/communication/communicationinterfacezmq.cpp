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

//! \file	communicationinterfacezmq.cpp
//! \brief	ZMQ communication interface.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <sstream>

#include <chrono>
#include <thread>

#include "communication/communicationinterfacezmq.hpp"

#include "parser/xmlparser.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

using namespace std;
using namespace zmq;

/*----------------------------------------------------------------------------*/
/* MEMBER FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

CI_ZMQ::CI_ZMQ (
	const OGSS_String		configurationFile,
	const OGSS_Interlocutor	myself) {
	OGSS_Ulong				port;
	ostringstream			oss ("");
	message_t				msgRequest (sizeof (OGSS_Interlocutor));
	message_t				msgPort;

	_myself = myself;

	_zmqContext = new context_t (1);
	_zmqManager = new socket_t (*_zmqContext, ZMQ_REQ);
	_zmqBarrier = new socket_t (*_zmqContext, ZMQ_REQ);

	port = XMLParser::getCommunicationPort (configurationFile);

	oss << "tcp://127.0.0.1:" << port;
	_zmqManager->connect (oss.str () .c_str () );

	oss.str ("");
	oss << "tcp://127.0.0.1:" << port + 1;
	_zmqBarrier->connect (oss.str () .c_str () );

	request (_myself);
}

CI_ZMQ::~CI_ZMQ () {
//	this_thread::sleep_for (chrono::milliseconds (100) );

	for (auto & elt: _mapping) {
		elt.second->close ();
		delete elt.second;
	}

	_mapping.clear ();

	_zmqManager->close ();
	_zmqBarrier->close ();

	delete _zmqManager;
	delete _zmqBarrier;
	delete _zmqContext;
}

OGSS_Bool
CI_ZMQ::request (
	const OGSS_Interlocutor	to) {
	ostringstream			oss ("");
	message_t				msgRequest (sizeof (OGSS_Interlocutor) );
	message_t				msgPort;
	socket_t				* mailbox;
	OGSS_Ulong				port;

	if (_mapping.find (to) != _mapping.end () )
		return false;

	memcpy ( (void*) msgRequest.data (), (void*) &(to), sizeof (to) );

	_zmqManager->send (msgRequest);
	_zmqManager->recv (&msgPort);

	if (to.first == MTP_TOTAL) return true;

	port = * (OGSS_Ulong*) msgPort.data ();
	oss << "tcp://127.0.0.1:" << port;

	if (to == _myself) {
		mailbox = new socket_t (*_zmqContext, ZMQ_PULL);
		mailbox->bind (oss.str () .c_str () );
	}
	else {
		mailbox = new socket_t (*_zmqContext, ZMQ_PUSH);
		mailbox->connect (oss.str () .c_str () );
	}
	
	_mapping.insert (make_pair (to, mailbox) );

	return true;
}

void
CI_ZMQ::requestBarrier () {
	message_t				msgRequest (sizeof (OGSS_Interlocutor) );
	message_t				msgAck;

	memcpy ( (void*) msgRequest.data (), (void*) &(_myself), sizeof (_myself) );

	_zmqBarrier->send (msgRequest);
	_zmqBarrier->recv (&msgAck);
}

void
CI_ZMQ::requestFullBarrier () {
	message_t				msgRequest (sizeof (OGSS_Interlocutor) );
	message_t				msgInfo (sizeof (OGSS_Interlocutor) );
	message_t				msgAck;
	OGSS_Interlocutor		to;

	to.first = MTP_TOTAL;		to.second = OGSS_USHORT_MAX;

	memcpy ( (void*) msgRequest.data (), (void*) &(_myself), sizeof (_myself) );
	memcpy ( (void*) msgInfo.data (), (void*) &(to), sizeof (to) );

	_zmqBarrier->send (msgRequest);
	_zmqManager->send (msgInfo);
	_zmqManager->recv (&msgAck);
	_zmqBarrier->recv (&msgAck);
}

void
CI_ZMQ::releaseBarrier (
	const OGSS_Ushort		numThreads) {
	message_t				msgRequest (sizeof (OGSS_Interlocutor) );
	message_t				msgAck;
	OGSS_Interlocutor		to;
	to.first = MTP_TOTAL;
	to.second = numThreads;

	memcpy ( (void*) msgRequest.data (), (void*) &(to), sizeof (to) );

	_zmqManager->send (msgRequest);
	_zmqManager->recv (&msgAck);
}
