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

//! \file	chrono.cpp
//! \brief	Simple chronometer implementation.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "util/chrono.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
/* MEMBER FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Chrono::Chrono ()
	{ _restart = true; }

Chrono::Chrono (
	const Chrono			& copy) {
	_restart = copy._restart;
	_start = copy._start;
	_end = copy._end;
}

Chrono::~Chrono () {  }

Chrono &
Chrono::operator= (
	const Chrono			& copy) {
	_restart = copy._restart;
	_start = copy._start;
	_end = copy._end;
	return *this;
}

void
Chrono::tick () {
	if (_restart) {
		_restart = false;
		_start = chrono::system_clock::now ();
	} else
		_end = chrono::system_clock::now ();
}

void
Chrono::restart ()
	{ _restart = true; }

OGSS_Long
Chrono::get () {
	return chrono::duration_cast <chrono::microseconds>
		(_end - _start) .count ();
}
