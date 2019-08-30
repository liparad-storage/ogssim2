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

//! \file	compinterface.cpp
//! \brief	Definition of the interface computation model.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "computation/compinterface.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

CompInterface::CompInterface (
	HardwareParameters		& params,
	vector <Tier>			& tiers,
	vector <Volume>			& vols,
	vector <Device>			& devs,
	vector <Interface>		& intfs):
	_hardParam (params), _tiers (tiers), _volumes (vols),
	_devices (devs), _interfaces (intfs) {
	_codSize = 128;
	_ackSize = 128;
}
CompInterface::~CompInterface () {  }

void
CompInterface::compute (
	Request					& req) {

	OGSS_Ushort				idv;
	OGSS_Ulong				size;
	OGSS_Ushort				i1, i2, i3;
	OGSS_Real				date;

	idv = req._idxDevice;
	size = req._size;
	date = req._date;

	i1 = _hardParam._hostInterface;
	i2 = _tiers [_volumes [_devices [idv] ._parent] ._parent] ._interface;
	i3 = _volumes [_devices [idv] ._parent] ._interface;

	req._transferTimeA1 = ((double) _codSize + req._transferTimeA1)
		/ (_interfaces [i1] ._bandwidth);
	req._transferTimeA2 = ((double) _codSize + req._transferTimeA2)
		/ (_interfaces [i2] ._bandwidth);

	req._transferTimeB2 = ((double) _ackSize + req._transferTimeB2)
		/ (_interfaces [i2] ._bandwidth);
	req._transferTimeB1 = ((double) _ackSize + req._transferTimeB1)
		/ (_interfaces [i1] ._bandwidth);

	if (req._type & RQT_WRITE) {
		req._transferTimeA3 = ((double) _codSize + size)
			/ (_interfaces [i3] ._bandwidth);

		req._transferTimeB3 = ((double) _ackSize)
			/ (_interfaces [i3] ._bandwidth);
	} else {
		req._transferTimeA3 = ((double) _codSize)
			/ (_interfaces [i3] ._bandwidth);

		req._transferTimeB3 = ((double) _ackSize + size)
			/ (_interfaces [i3] ._bandwidth);
	}
}
