/*
 * Copyright UVSQ - CEA/DAM/DIF (2017-2018)
 * Contributors:  Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *                Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *				  Maxence JOULIN
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

//! \file	compnvram.cpp
//! \brief	Definition of the NVRAM computation model.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <cmath>

#include "computation/compnvram.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

CompNVRAM::CompNVRAM(
	vector<Device> &devs) : _devices(devs) {}
CompNVRAM::~CompNVRAM() {}

void CompNVRAM::compute(Request &req)
{
	OGSS_Real servTime;
	OGSS_Long nb_sub_requests;
	OGSS_Short bytes_max_in_one_cycle;

	//Compute the max bytes handle in one cycle by the nvram device. Multiples chips inside the device allow a parallel output and so more bytes per cycle.
	bytes_max_in_one_cycle = _devices[req._idxDevice]._param.n._bytesPerCol * _devices[req._idxDevice]._param.n._nbChips;

	//Compute the number of cycle needed to handle the request size.
	nb_sub_requests = (req._size/bytes_max_in_one_cycle) 
					+ ((req._size%bytes_max_in_one_cycle) ? 1 : 0); //Add 1 subrequest if the size of the main request is not a multiple of max bytes per cycle.

	//Get the time associate with request type for one cycle. 
	if (req._type == RQT_READ)
		servTime = _devices[req._idxDevice]._param.n._read;
	else
		servTime = _devices[req._idxDevice]._param.n._write;

	//Compute the total time needed to handle the full request	
	req._serviceTime = servTime * nb_sub_requests;
}
