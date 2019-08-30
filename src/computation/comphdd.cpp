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

//! \file	comphdd.cpp
//! \brief	Definition of the HDD computation model.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <cmath>

#include "computation/comphdd.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

CompHDD::CompHDD (
	vector <Device>			& devs):
	_devices (devs) {  }
CompHDD::~CompHDD () {  }

void
CompHDD::compute (
	Request					& req) {

	OGSS_Real				st, rt, tt;

	st = seekTime (req, _devices [req._idxDevice]);
	rt = rotTime (req, _devices [req._idxDevice]);
	tt = tsfTime (req, _devices [req._idxDevice]);

	req._serviceTime = st + rt + tt;
}

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

OGSS_Real
CompHDD::seekTime (
	Request					& req,
	Device					& dev) {
	OGSS_Real				a, b, dist;
	OGSS_Real				seekTime, minSeek, avgSeek, maxSeek;
	OGSS_Ulong				previous, next, start;

	minSeek = (req._type & RQT_WRITE) ? dev._param.h._minWriteSeek
									  : dev._param.h._minReadSeek;
	avgSeek = (req._type & RQT_WRITE) ? dev._param.h._avgWriteSeek
									  : dev._param.h._avgReadSeek;
	maxSeek = (req._type & RQT_WRITE) ? dev._param.h._maxWriteSeek
									  : dev._param.h._maxReadSeek;

	previous = dev._param.h._lastTrack;
	start = req._deviceAddress / dev._param.h._sectorSize;

	// Distance computation
	next = (start / dev._param.h._sectorsPerTrack)
		% dev._param.h._tracksPerPlatter;

	dist = (previous >= next) ? previous - next : next - previous;

	// Track position setter
	previous = (start + req._size / dev._param.h._sectorSize)
		/ dev._param.h._sectorsPerTrack;
	previous = previous % dev._param.h._tracksPerPlatter;
	dev._param.h._lastTrack = previous;

	// Seek time computation
	if (dist == 0) seekTime = 0;
	else {
		a = (-10 * minSeek + 15 * avgSeek - 5 * maxSeek)
			/ (3 * sqrt (dev._param.h._tracksPerPlatter) );
		b = (7 * minSeek - 15 * avgSeek + 8 * maxSeek)
			/ (3 * dev._param.h._tracksPerPlatter);
		seekTime = a * sqrt (dist) + b * (dist - 1) + minSeek;
	}

	return seekTime;
}

OGSS_Real
CompHDD::rotTime (
	Request					& req,
	Device					& dev) {

	return dev._param.h._rotationSpeed / 2;

}

OGSS_Real
CompHDD::tsfTime (
	Request					& req,
	Device					& dev) {

	return req._size * (dev._param.h._transferRate);
}
