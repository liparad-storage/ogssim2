/*
 * Copyright UVSQ - CEA/DAM/DIF (2017-2018)
 * Contributors:  Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *                Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *                Maxence JOULIN
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

//! \file	hardware.hpp
//! \brief	The file contains the whole structure set used to store the
//!			simulated storage system information.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/hardware.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

/*----------------------------------------------------------------------------*/
/* FUNCTIONS -----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
Tier::applyDataUnit (
	const OGSS_DataUnit		du) {
	_bufferSize /= du._memory;
}

void
Volume::applyDataUnit (
	const OGSS_DataUnit		du) {
	_bufferSize /= du._memory;
	if (_suSize != 0 && _suSize < du._memory)
		LOG (FATAL) << "The stripe unit size is lower than the "
			<< "requested memory data unit ("
			<< _suSize << " >=? " << du._memory << ")";
	_suSize /= du._memory;
}

void
HDDParameters::applyDataUnit (
	const OGSS_DataUnit		du) {
	_sectorSize /= du._memory;
	_minReadSeek /= du._time;
	_avgReadSeek /= du._time;
	_maxReadSeek /= du._time;
	_minWriteSeek /= du._time;
	_avgWriteSeek /= du._time;
	_maxWriteSeek /= du._time;
	_rotationSpeed /= du._time;

	_transferRate *= du._memory;
	_transferRate /= du._time;
}

void
SSDParameters::applyDataUnit (
	const OGSS_DataUnit		du) {
	_pageSize /= du._memory;
	_randRead /= du._time;
	_randWrite /= du._time;
	_seqRead /= du._time;
	_seqWrite /= du._time;
	_erase /= du._time;
}

void
NVRAMParameters::applyDataUnit (
	const OGSS_DataUnit		du) {
	_bytesPerCol /= du._memory;
	_read /= du._time;
	_write /= du._time;
}

void
Device::applyDataUnit (
	const OGSS_DataUnit		du) {
	_physicalCapacity /= du._memory;
	_bufferSize /= du._memory;

	switch (_type) {
		case DTP_HDD:		_param.h.applyDataUnit (du);
		break;
		case DTP_SSD:		_param.s.applyDataUnit (du);
		break;
		case DTP_NVRAM:		_param.n.applyDataUnit (du);
		break;
		default: break;
	}
}

void
Interface::applyDataUnit (
	const OGSS_DataUnit		du) {
	_bandwidth /= du._memory;
	_bandwidth /= du._time;
}
