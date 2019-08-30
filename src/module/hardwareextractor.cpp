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

//! \file	hardwareextractor.cpp
//! \brief	Module which extracts all the parameters of the hardware
//!			configuration.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "module/hardwareextractor.hpp"

#include "parser/xmlparser.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

HardwareExtractor::HardwareExtractor (
	const OGSS_String		configurationFile):
	Module (configurationFile, make_pair (MTP_HARDWARE, 0) ) {

	_globalDU = XMLParser::getDataUnit (_cfg, PTP_GLOBAL);
}

HardwareExtractor::~HardwareExtractor () {  }

void
HardwareExtractor::processExtraction () {
	OGSS_String 			filename {""};

	filename = XMLParser::getFilePath (_cfg, FTP_HARDWARE);

	extract (filename);

	if (_globalDU != OGSS_DataUnit () ) {
		DLOG(INFO) << "[HX] Need to apply the data unit";
		applyDataUnit ();
	}

#ifndef UTEST
	sendData ();
#endif
}

void
HardwareExtractor::processDecomposition () {  }

void
HardwareExtractor::processSynchronization () {  }

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Gives a description of a device parameters.
//! \param	d				Device.
void
describeDevice (
	Device					& d) {
	if (d._type == DTP_HDD) {
		LOG(INFO) << "\n"
		<< "sec. size: " << d._param.h._sectorSize << "\n"
		<< "sec. p track: " << d._param.h._sectorsPerTrack << "\n"
		<< "track p plat.: " << d._param.h._tracksPerPlatter << "\n"
		<< "nb plat.: " << d._param.h._numPlatters << "\n"

		<< "m read seek: " << d._param.h._minReadSeek << "\n"
		<< "a read seek: " << d._param.h._avgReadSeek << "\n"
		<< "M read seek: " << d._param.h._maxReadSeek << "\n"
		<< "m write seek: " << d._param.h._minWriteSeek << "\n"
		<< "a write seek: " << d._param.h._avgWriteSeek << "\n"
		<< "M write seek: " << d._param.h._maxWriteSeek << "\n"
		<< "rot. speed: " << d._param.h._rotationSpeed << "\n"

		<< "tsf rate: " << d._param.h._transferRate << "\n"

		<< "mttf: " << d._param.h._mttf << "\n"
		
		<< "buf. size: " << d._bufferSize;
	} else if (d._type == DTP_SSD) {
		LOG(INFO) << "\n"
		<< "page size: " << d._param.s._pageSize << "\n"
		<< "page p block: " << d._param.s._pagesPerBlock << "\n"
		<< "block p die: " << d._param.s._blocksPerDie << "\n"
		<< "nb die: " << d._param.s._numDies << "\n"

		<< "rand read: " << d._param.s._randRead << "\n"
		<< "rand write: " << d._param.s._randWrite << "\n"
		<< "seq read: " << d._param.s._seqRead << "\n"
		<< "seq write: " << d._param.s._seqWrite << "\n"
		<< "erase: " << d._param.s._erase << "\n"

		<< "nb erase: " << d._param.s._numErase << "\n"
		<< "buf. size: " << d._bufferSize;
	}else if (d._type == DTP_NVRAM) {
		LOG(INFO) << "\n"
		<< "real capacity in bytes: " << d._physicalCapacity << "\n"
		<< "nb of chips: " << d._param.n._nbChips << "\n"
		<< "bytes per columns: " << d._param.n._bytesPerCol << "\n"
		<< "nb of rows: " << d._param.n._rows << "\n"
		<< "nb of columns: " << d._param.n._cols << "\n"
		<< "read: " << d._param.n._read << "\n"
		<< "write: " << d._param.n._write << "\n";
	}
}

void
HardwareExtractor::extract (
	const OGSS_String		hardwareFile) {

	XMLParser::getHardwareConfiguration (hardwareFile, _hardware);

/*	int count = 0;
	int nbsubvol = 0;

	for (auto elt: _hardware._volumes) {
		if (! nbsubvol) {
			LOG(INFO) << VolumeNameMap.at (elt._type) << ": "
				<< elt._numDevices << " "
				<< DeviceNameMap.at (_hardware._devices[count] ._type);
				describeDevice (_hardware._devices[count]);
			count += elt._numDevices;
		} else {
			count += elt._numDevices;
			nbsubvol = elt._numSubVolumes;
		}
	}*/
}

void
HardwareExtractor::applyDataUnit () {
	for (auto & elt: _hardware._interfaces)	elt.applyDataUnit (_globalDU);
	for (auto & elt: _hardware._tiers)		elt.applyDataUnit (_globalDU);
	for (auto & elt: _hardware._volumes)	elt.applyDataUnit (_globalDU);
	for (auto & elt: _hardware._devices)	elt.applyDataUnit (_globalDU);
}
