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

//! \file	request.cpp
//! \brief	Define the request structure, used by all the modules of OGSSim.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/request.hpp"

#include <bitset>

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Request::Request (
	const OGSS_Real			date,
	const OGSS_Ulong		size,
	const OGSS_Ulong		address,
	const OGSS_RequestType	type,
	const OGSS_Bool			system) {
	_date = date;
	_size = size;
	_address = address;
	_type = type;

	_system = system;

	_mainIdx = 0;
	_majrIdx = 0;
	_minrIdx = 0;

	_numChild = 0;
	_numPrioChild = 0;
	_numLink = 0;

	_idxVolume = 0;
	_volumeAddress = 0;
	_idxDevice = 0;
	_deviceAddress = 0;

	_serviceTime = .0;
	_transferTimeA1 = .0;
	_transferTimeA2 = .0;
	_transferTimeA3 = .0;
	_transferTimeB3 = .0;
	_transferTimeB2 = .0;
	_transferTimeB1 = .0;
	_waitingTime = .0;
	_responseTime = .0;

	_failed = false;
	_multiple = false;
	_operation = ROP_NATIVE;
}

Request::Request (
	const tuple <
		OGSS_Real,
		OGSS_RequestType,
		OGSS_Ulong,
		OGSS_Ulong,
		OGSS_Ushort,
		OGSS_Ushort>		req): Request (
		get <0> (req), get <3> (req), get <2> (req), get <1> (req) ) {  }

OGSS_String
Request::print () {
	ostringstream oss ("");

	oss << "[";
	if (_mainIdx == OGSS_ULONG_MAX)		oss << "-";
	else								oss << _mainIdx;
	oss << "/";
	if (_majrIdx == OGSS_ULONG_MAX)		oss << "-";
	else								oss << _majrIdx;
	oss << "/";
	if (_minrIdx == OGSS_ULONG_MAX)		oss << "-";
	else								oss << _minrIdx;
	oss << "] {"
		<< _date << "\t" << _size << "\t" << _address << "\t" << bitset <5> (_type)
		<< "}" << " (" << _idxVolume << "-" << _idxDevice << ", " << _deviceAddress << ")" << endl;

	return oss.str ();
}

OGSS_Bool operator< (
	const OGSS_RequestIdx				lhs,
	const OGSS_RequestIdx				rhs) {
	return lhs._main < rhs._main
	   || (lhs._main == rhs._main && lhs._major < rhs._major)
	   || (lhs._main == rhs._main && lhs._major == rhs._major && lhs._minor < rhs._minor);
}

OGSS_Bool operator== (
	const OGSS_RequestIdx				lhs,
	const OGSS_RequestIdx				rhs) {
	return lhs._main == rhs._main && lhs._major == rhs._major && lhs._minor == rhs._minor;
}

OGSS_Bool operator!= (
	const OGSS_RequestIdx				lhs,
	const OGSS_RequestIdx				rhs) {
	return lhs._main != rhs._main || lhs._major != rhs._major || lhs._minor != rhs._minor;
}

std::ostream & operator<< (
	std::ostream &						stream,
	const OGSS_RequestIdx				& idx) {
	stream << "{";
	if (idx._main == OGSS_ULONG_MAX)	stream << "-/";
	else								stream << idx._main << "/";
	if (idx._major == OGSS_ULONG_MAX)	stream << "-/";
	else								stream << idx._major << "/";
	if (idx._minor == OGSS_ULONG_MAX)	stream << "-}";
	else								stream << idx._minor << "}";
	return stream;
}
