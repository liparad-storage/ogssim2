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

//! \file	communicationmanagerzmq.cpp
//! \brief	ZMQ communication manager.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <algorithm>
#include <chrono>
#include <random>
#include <sstream>
#include <thread>

#include "communication/communicationinterfacezmq.hpp"
#include "communication/communicationmanagerzmq.hpp"

#include "parser/xmlparser.hpp"

using namespace std;
using namespace zmq;

/*----------------------------------------------------------------------------*/
/* MEMBER FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

CM_ZMQ::CM_ZMQ (
	const OGSS_String		configurationFile) {
	OGSS_Ulong				port;
	ostringstream			oss ("");

	_zmqContext = new context_t (1);
	_zmqMaster = new socket_t (*_zmqContext, ZMQ_REP);
	_zmqBarrier = new socket_t (*_zmqContext, ZMQ_REP);

	port = XMLParser::getCommunicationPort (configurationFile);

	_mapping.insert (make_pair (make_pair (MTP_COMMUNICATION, 0), port) );
	_mapping.insert (make_pair (make_pair (MTP_COMMUNICATION, 1), port + 1) );

	oss << "tcp://127.0.0.1:" << port;
	_zmqMaster->bind (oss.str () .c_str () );

	oss.str ("");
	oss << "tcp://127.0.0.1:" << port + 1;
	_zmqBarrier->bind (oss.str () .c_str () );

	_fullBarrierCapacity = numMonoProcessModules + numMultiProcessModules *
		XMLParser::getNumberOfVolumes (XMLParser::getFilePath (configurationFile, FTP_HARDWARE) ) - 1;
	_fullBarrierCounter = 0;
}

CM_ZMQ::~CM_ZMQ () {
	_zmqMaster->close ();
	_zmqBarrier->close ();

	delete _zmqMaster;
	delete _zmqBarrier;
	delete _zmqContext;
}

OGSS_Bool
CM_ZMQ::provide () {
	zmq::message_t			msgRequest;
	zmq::message_t			msgPort (sizeof (OGSS_Ulong));
	OGSS_Interlocutor		interlocutor;
	OGSS_Ulong				port;

	_zmqMaster->recv (&msgRequest);

	interlocutor = * (OGSS_Interlocutor *) msgRequest.data ();

	if (interlocutor.first == MTP_TOTAL && interlocutor.second == OGSS_USHORT_MAX) {
		++ _fullBarrierCounter;
		_zmqMaster->send (msgPort);
		if (_fullBarrierCounter == _fullBarrierCapacity)
			fullBarrier ();
		return true;
	}

	if (interlocutor.first == MTP_TOTAL && interlocutor.second != 0) {
		releaseBarrier (interlocutor.second);
		_zmqMaster->send (msgPort);

		return true;
	}
	
	if (interlocutor.first == MTP_TOTAL) {
		_zmqMaster->send (msgPort);

		return false;
	}

	auto p = _mapping.find (interlocutor);

	if (p == _mapping.end () ) {
		port = max_element (_mapping.begin (), _mapping.end (),
		[] (const pair <OGSS_Interlocutor, OGSS_Ulong> &p1,
			const pair <OGSS_Interlocutor, OGSS_Ulong> &p2) {
				return p1.second < p2.second;
			} ) ->second + 1;
		_mapping.insert (make_pair (interlocutor, port) );
	} else {
		port = p->second;
	}

	memcpy ( (void*) msgPort.data (), (void*) &port, sizeof (port) );

	_zmqMaster->send (msgPort);

	return true;
}

void
CM_ZMQ::releaseBarrier (
	OGSS_Ushort				numThreads) {
	zmq::message_t			msgRequest;
	zmq::message_t			msgAck (sizeof (OGSS_Ushort) );
	OGSS_Interlocutor		interlocutor;

	while (numThreads) {
		_zmqBarrier->recv (&msgRequest);

		interlocutor = * (OGSS_Interlocutor *) msgRequest.data ();

		_zmqBarrier->send (msgAck);

		-- numThreads;
	}
}

void
CM_ZMQ::fullBarrier () {
	zmq::message_t			msgRequest;
	zmq::message_t			msgAck (sizeof (OGSS_Ushort) );
	OGSS_Interlocutor		interlocutor;

	while (_fullBarrierCounter) {
		_zmqBarrier->recv (&msgRequest);

		interlocutor = * (OGSS_Interlocutor *) msgRequest.data ();

		_zmqBarrier->send (msgAck);

		-- _fullBarrierCounter;
	}
}

/*----------------------------------------------------------------------------*/
/* UNITARY TEST --------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

UT_CM_ZMQ::UT_CM_ZMQ (
	const OGSS_String		& configurationFile):
	UnitaryTest <UT_CM_ZMQ> (MTP_COMMUNICATIONZMQ) {
	set <OGSS_String>		testNames;

	XMLParser::getListOfRequestedUnitaryTests (
		configurationFile, _module, testNames);

	for (auto & elt: testNames) {
		if (! elt.compare ("all") ) {
			_tests.push_back (make_pair ("request1() check",
				&UT_CM_ZMQ::check1Request) );
			_tests.push_back (make_pair ("request10() check",
				&UT_CM_ZMQ::check10Request) );
			_tests.push_back (make_pair ("request100() check",
				&UT_CM_ZMQ::check100Request) );
			_tests.push_back (make_pair ("request1000() check",
				&UT_CM_ZMQ::check1000Request) );
			_tests.push_back (make_pair ("communication() check",
				&UT_CM_ZMQ::checkCommunication) );
		}
		else if (! elt.compare ("check1Request") )
			_tests.push_back (make_pair ("request1() check",
				&UT_CM_ZMQ::check1Request) );
		else if (! elt.compare ("check10Request") )
			_tests.push_back (make_pair ("request10() check",
				&UT_CM_ZMQ::check10Request) );
		else if (! elt.compare ("check100Request") )
			_tests.push_back (make_pair ("request100() check",
				&UT_CM_ZMQ::check100Request) );
		else if (! elt.compare ("check1000Request") )
			_tests.push_back (make_pair ("request1000() check",
				&UT_CM_ZMQ::check1000Request) );
		else if (! elt.compare ("checkCommunication") )
			_tests.push_back (make_pair ("communication() check",
				&UT_CM_ZMQ::checkCommunication) );
		else
			LOG (WARNING) << ModuleNameMap.at (_module) << " unitary test "
				<< "named '" << elt << "' does not match!" << endl;
	}
}

UT_CM_ZMQ::~UT_CM_ZMQ () {  }

//! \brief	Function used with checkRequest tests, to instanciate the
//! 		communication interface.
//! \param	id				Module identifier.
void _thread_01 (
	OGSS_Ulong				id) {
	CommunicationInterface	* cmi = new CI_ZMQ ("env/conf/_ut_config.xml",
		make_pair (MTP_WORKLOAD, id) );
	CI_ZMQ					* pointer;

	default_random_engine generator;
  	uniform_int_distribution <int> distribution (0,999);

	this_thread::sleep_for (chrono::milliseconds (distribution (generator) ) );

	delete cmi;
}

OGSS_Bool
UT_CM_ZMQ::check1Request () {
	const OGSS_Ulong		N = 1;
	CommunicationManager	* cmm = new CM_ZMQ ("env/conf/_ut_config.xml");
	CM_ZMQ					* pointer;
	thread					* t [N];
	OGSS_Bool				result;

	pointer = dynamic_cast <CM_ZMQ *> (cmm);

	for (auto i = 0; i < N; ++i)
		t [i] = new thread (_thread_01, i);

	for (auto i = 0; i < N; ++i)
		pointer->provide ();

	for (auto i = 0; i < N; ++i) {
		t [i] ->join ();
		delete t [i];
	}

	result = (pointer->_mapping.size () != N+1) ? false : true;

	delete cmm;

	return result;
}

OGSS_Bool
UT_CM_ZMQ::check10Request () {
	const OGSS_Ulong		N = 10;
	CommunicationManager	* cmm = new CM_ZMQ ("env/conf/_ut_config.xml");
	CM_ZMQ					* pointer;
	thread					* t [N];
	OGSS_Bool				result;

	pointer = dynamic_cast <CM_ZMQ *> (cmm);

	for (auto i = 0; i < N; ++i)
		t [i] = new thread (_thread_01, i);

	for (auto i = 0; i < N; ++i)
		pointer->provide ();

	for (auto i = 0; i < N; ++i) {
		t [i] ->join ();
		delete t [i];
	}

	result = (pointer->_mapping.size () != N+1) ? false : true;

	delete cmm;

	return result;
}

OGSS_Bool
UT_CM_ZMQ::check100Request () {
	const OGSS_Ulong		N = 100;
	CommunicationManager	* cmm = new CM_ZMQ ("env/conf/_ut_config.xml");
	CM_ZMQ					* pointer;
	thread					* t [N];
	OGSS_Bool				result;

	pointer = dynamic_cast <CM_ZMQ *> (cmm);

	for (auto i = 0; i < N; ++i)
		t [i] = new thread (_thread_01, i);

	for (auto i = 0; i < N; ++i)
		pointer->provide ();

	for (auto i = 0; i < N; ++i) {
		t [i] ->join ();
		delete t [i];
	}

	result = (pointer->_mapping.size () != N+1) ? false : true;

	delete cmm;

	return result;
}

OGSS_Bool
UT_CM_ZMQ::check1000Request () {
	const OGSS_Ulong		N = 1000;
	CommunicationManager	* cmm = new CM_ZMQ ("env/conf/_ut_config.xml");
	CM_ZMQ					* pointer;
	thread					* t [N];
	OGSS_Bool				result;

	pointer = dynamic_cast <CM_ZMQ *> (cmm);

	for (auto i = 0; i < N; ++i)
		t [i] = new thread (_thread_01, i);

	for (auto i = 0; i < N; ++i)
		pointer->provide ();

	for (auto i = 0; i < N; ++i) {
		t [i] ->join ();
		delete t [i];
	}

	result = (pointer->_mapping.size () != N+1) ? false : true;

	delete cmm;

	return result;
}

//! \brief	Function used with checkCommunication test, to instanciate the
//! 		communication interface.
//! \param	value			Communicated value.
void _thread_11 (
	OGSS_Ulong				& value) {
	CommunicationInterface	* cmi = new CI_ZMQ ("env/conf/_ut_config.xml",
		make_pair (MTP_WORKLOAD, 1) );
	CI_ZMQ					* pointer;

	random_device			randDevice;
	default_random_engine	generator (randDevice () );
	uniform_int_distribution <int> distribution (0,9999);

	cmi->request (make_pair (MTP_HARDWARE, 1) );

	pointer = dynamic_cast <CI_ZMQ *> (cmi);
	value = distribution (generator);

	cmi->send (make_pair (MTP_HARDWARE, 1), &value, sizeof (value) );

	delete cmi;
}

//! \brief	Function used with checkCommunication test, to instanciate the
//! 		communication interface.
//! \param	value			Communicated value.
void _thread_12 (
	OGSS_Ulong				& value) {
	CommunicationInterface	* cmi = new CI_ZMQ ("env/conf/_ut_config.xml",
		make_pair (MTP_HARDWARE, 1) );
	void					* arg;

	cmi->request (make_pair (MTP_WORKLOAD, 1) );

	cmi->receive (arg);

	value = * static_cast <OGSS_Ulong *> (arg);
	free (arg);

	delete cmi;
}

OGSS_Bool
UT_CM_ZMQ::checkCommunication () {
	OGSS_Ulong				value1, value2;
	CommunicationManager	* cmm = new CM_ZMQ ("env/conf/_ut_config.xml");
	thread					t1 (_thread_11, ref (value1) );
	thread					t2 (_thread_12, ref (value2) );
	CM_ZMQ					* pointer;

	pointer = dynamic_cast <CM_ZMQ *> (cmm);

	pointer->provide ();
	pointer->provide ();
	pointer->provide ();
	pointer->provide ();

	t1.join ();
	t2.join ();

	delete cmm;

	return (value1 == value2);
}
