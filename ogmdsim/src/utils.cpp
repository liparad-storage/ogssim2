/*
 * Copyright UVSQ - CEA/DAM/DIF (2019)
 * Contributors:	Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *					Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General
 * Public License as published per the Free Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

//! \file			utils.hpp
//! \brief			Definition of the utils namespace. This namespace is used to define global constants
//!					and utility functions.

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include "utils.hpp"

#include <iostream>
#include <sstream>

using namespace std;

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

queue <string>
OGUtils::splitString (
	const string						path,
	const char							del) {
	queue <string>						q;
	int									a {0}, b;

	if (path.empty () ) return q;

	while ( (b = path.find (del, a) ) != string::npos) {
		q.push (path.substr (a, b-a) );
		a = b + 1;
	}

	q.push (path.substr (a) );

	return q;
}

double
OGUtils::convertPrefix (
	const string						a) {
	double								val {.0};
	istringstream						iss {a};
	char								prefix {' '};

	iss >> val >> prefix;

	if (prefix == 'K')					val *= KILO;
	else if (prefix == 'M')				val *= MEGA;
	else if (prefix == 'G')				val *= GIGA;
	else if (prefix == 'T')				val *= TERA;
	else if (prefix == 'm')				val *= MILLI;
	else if (prefix == 'u')				val *= MICRO;
	else if (prefix == 'n')				val *= NANO;

	return val;
}
