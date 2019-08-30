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

//! \file	compssd.cpp
//! \brief	Definition of the SSD computation model.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "computation/compssd.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

CompSSD::CompSSD (
	vector <Device>			& devs):
	_devices (devs) {
	for (auto i = 0; i < _devices.size (); ++i) {
		if (_devices [i] ._type == DTP_SSD)
			// number of elements = number of dies
			_lastPageSeen [i] = vector <OGSS_Ulong> (_devices [i] ._param.s._numDies);
	}
}
CompSSD::~CompSSD () {  }

void
CompSSD::compute (
	Request					& req) {
	OGSS_Real				servTime;

	if (req._type == RQT_ERASE)
		servTime = eraseServTime (req, _devices [req._idxDevice] );
	else if (req._type & RQT_WRITE)
		servTime = writeServTime (req, _devices [req._idxDevice] );
	else
		servTime = readServTime (req, _devices [req._idxDevice] );

	req._serviceTime = servTime;
}

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

OGSS_Real
CompSSD::readServTime (
	Request					& req,
	Device					& dev) {
	OGSS_Ulong	a = req._deviceAddress;
	OGSS_Ulong	b = req._size;

	OGSS_Ulong	pb = dev._param.s._pagesPerBlock;
	OGSS_Ulong	bd = dev._param.s._blocksPerDie;
	OGSS_Ulong	ps = dev._param.s._pageSize;

	std::vector <OGSS_Ulong> & t =
		_lastPageSeen [req._idxDevice];

	OGSS_Real	rt;
	OGSS_Real	st;

	OGSS_Real	r = .0;

	OGSS_Ulong   c, d, e, f;

	rt = dev._param.s._randRead;
	st = dev._param.s._seqRead;

	// Loop on dies
	for (OGSS_Ulong i = a / (pb * bd * ps); i <= (a + b - 1) / (pb * bd * ps); ++i)
	{
		c = std::max (i * pb * bd * ps, a);
		d = std::min ( (i + 1) * pb * bd * ps - 1, a + b - 1);
		// e <- number of rand access
		e = 1 + (d / (pb * ps) ) - (c / (pb * ps) );

		if (t [i] == c % (pb * bd * ps) && (c - 1) / (pb * ps) == c / (pb * ps) )
			e--;
		
		f = (1 + d - c - e) / ps;

		r += rt * e;
		r += st * ( (f < 1) ? 1 : f);

		t [i] = (d + 1) % (pb * bd * ps);
	}

	return r;
}

OGSS_Real
CompSSD::writeServTime (
	Request					& req,
	Device					& dev) {
	OGSS_Ulong   a = req._deviceAddress;
	OGSS_Ulong   b = req._size;

	OGSS_Ulong   pb = dev._param.s._pagesPerBlock;
	OGSS_Ulong   bd = dev._param.s._blocksPerDie;
	OGSS_Ulong   ps = dev._param.s._pageSize;

	std::vector <OGSS_Ulong> & t =
		_lastPageSeen [req._idxDevice];

	OGSS_Real rt;
	OGSS_Real st;

	OGSS_Real r = .0;

	OGSS_Ulong   c, d, e, f;

	rt = dev._param.s._randWrite;
	st = dev._param.s._seqWrite;

	// Loop on dies
	for (OGSS_Ulong i = a / (pb * bd * ps); i <= (a + b - 1) / (pb * bd * ps); ++i)
	{
		c = std::max (i * pb * bd * ps, a);
		d = std::min ( (i + 1) * pb * bd * ps - 1, a + b - 1);
		// e <- number of rand access
		e = 1 + (d / (pb * ps) ) - (c / (pb * ps) );

		if (t [i] == c % (pb * bd * ps) && (c - 1) / (pb * ps)
			== c / (pb * ps) )
			e--;
		
		f = (1 + d - c - e) / ps;

		r += rt * e;
		r += st * ( (f < 1) ? 1 : f);

		t [i] = (d + 1) % (pb * bd * ps);
	}

	return r;
}

OGSS_Real
CompSSD::eraseServTime (
	Request					& req,
	Device					& dev) {
	OGSS_Ulong nbrBlocks = req._size /
		(dev._param.s._pageSize * dev._param.s._pagesPerBlock);

	return nbrBlocks * dev._param.s._erase;
}
