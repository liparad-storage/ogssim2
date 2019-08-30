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

//! \file	xxmlparser-dev.cpp
//! \brief	Namespaces used for XML parsing. Contains the functions related to
//! 		the device file.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <algorithm>
#include <fstream>
#include <map>

#include "xxmlparser-base.cpp"

#if ! USE_TINYXML

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

namespace {
OGSS_Ulong
getHDDGeometry (
	DOMNode					* root,
	Device					& device) {
	DOMNode					* node;

	node = _getNode (root, ParamNameMap.at (PTP_GEOMETRY) );

	device._param.h._sectorSize = _getLong (node, ParamNameMap.at (PTP_SECSIZE) );
	device._param.h._sectorsPerTrack = _getLong (node, ParamNameMap.at (PTP_SECTRK) );
	device._param.h._tracksPerPlatter = _getLong (node, ParamNameMap.at (PTP_TRKPLT) );
	device._param.h._numPlatters = _getLong (node, ParamNameMap.at (PTP_NBPLT) );

	return device._param.h._sectorSize
		 * device._param.h._sectorsPerTrack
		 * device._param.h._tracksPerPlatter
		 * device._param.h._numPlatters;
}

OGSS_Ulong
getSSDGeometry (
	DOMNode					* root,
	Device					& device) {
	DOMNode					* node;

	node = _getNode (root, ParamNameMap.at (PTP_GEOMETRY) );

	device._param.s._pageSize = _getLong (node, ParamNameMap.at (PTP_PAGESIZE) );
	device._param.s._pagesPerBlock = _getLong (node, ParamNameMap.at (PTP_PAGBLK) );
	device._param.s._blocksPerDie = _getLong (node, ParamNameMap.at (PTP_BLKDIE) );
	device._param.s._numDies = _getLong (node, ParamNameMap.at (PTP_NBDIE) );

	return device._param.s._pageSize
		 * device._param.s._pagesPerBlock
		 * device._param.s._blocksPerDie
		 * device._param.s._numDies;
}

OGSS_Ulong
getNVRAMGeometry (
	DOMNode					* root,
	Device					& device) {
	DOMNode					* node;

	node = _getNode (root, ParamNameMap.at (PTP_GEOMETRY) );
	device._param.n._nbChips = _getLong (node, ParamNameMap.at (PTP_NBCHIPS) );
	device._param.n._bytesPerCol = _getLong (node, ParamNameMap.at (PTP_BYTESPERCOL) );
	device._param.n._cols = _getLong (node, ParamNameMap.at (PTP_COLS) );
	device._param.n._rows = _getLong (node, ParamNameMap.at (PTP_ROWS) );

	return device._param.n._nbChips 
		* device._param.n._bytesPerCol
		* device._param.n._cols
		* device._param.n._rows;
}


void
getHDDPerformance (
	DOMNode					* root,
	Device					& device) {
	DOMNode					* node;

	node = _getNode (root, ParamNameMap.at (PTP_PERF) );

	device._param.h._minReadSeek = _getRealMeasure (node, PTP_MNRSK);
	device._param.h._avgReadSeek = _getRealMeasure (node, PTP_AGRSK);
	device._param.h._maxReadSeek = _getRealMeasure (node, PTP_MXRSK);
	device._param.h._minWriteSeek = _getRealMeasure (node, PTP_MNWSK);
	device._param.h._avgWriteSeek = _getRealMeasure (node, PTP_AGWSK);
	device._param.h._maxWriteSeek = _getRealMeasure (node, PTP_MXWSK);
	device._param.h._rotationSpeed = 1 / (
		_getReal (node, ParamNameMap.at (PTP_ROTSPD) ) / MINUTE);

	device._param.h._transferRate = _getRealMeasure (node, PTP_TSFRATE);
	device._bufferSize = _getLongPrefix (node, PTP_BUFSIZE);
}

void
getSSDPerformance (
	DOMNode					* root,
	Device					& device) {
	DOMNode					* node;

	node = _getNode (root, ParamNameMap.at (PTP_PERF) );

	device._param.s._randRead = _getRealMeasure (node, PTP_RNDR) * device._param.s._pageSize;
	device._param.s._randWrite = _getRealMeasure (node, PTP_RNDW) * device._param.s._pageSize;
	device._param.s._seqRead = _getRealMeasure (node, PTP_SEQR) * device._param.s._pageSize;
	device._param.s._seqWrite = _getRealMeasure (node, PTP_SEQW) * device._param.s._pageSize;
	device._param.s._erase = _getRealMeasure (node, PTP_ERASE);

	device._bufferSize = _getLongPrefix (node, PTP_BUFSIZE);
}

void
getNVRAMPerformance (
	DOMNode					* root,
	Device					& device) {
	DOMNode					* node;

	node = _getNode (root, ParamNameMap.at (PTP_PERF) );

	device._param.n._read = _getRealMeasure (node, PTP_READ);
	device._param.n._write = _getRealMeasure (node, PTP_WRITE);
}

void
getHDDReliability (
	DOMNode					* root,
	Device					& device) {
	DOMNode					* node;

	node = _getNode (root, ParamNameMap.at (PTP_RELIABILITY) );

	device._param.h._mttf = _getLong (node, ParamNameMap.at (PTP_MTTF) );
}

void
getSSDReliability (
	DOMNode					* root,
	Device					& device) {
	DOMNode					* node;

	node = _getNode (root, ParamNameMap.at (PTP_RELIABILITY) );

	device._param.s._numErase = _getLong (node, ParamNameMap.at (PTP_NBERASE) );
}

void
getHDDController (
	DOMNode					* root,
	Device					& device) {
	DOMNode					* node;
	OGSS_String				trnsType;
	OGSS_String				dfrgType;

	node = _getNode (root, ParamNameMap.at (PTP_CONTROLLER) );

	trnsType = _getString (node, ParamNameMap.at (PTP_TRANSLATION), true);
	dfrgType = _getString (node, ParamNameMap.at (PTP_DEFRAGMENTATION), true);

	auto findRes1 = find_if (TranslationNameMap.begin (), TranslationNameMap.end (),
		[&] (const pair <OGSS_TranslationType, OGSS_String> & elt)
		{ return ! elt.second.compare (trnsType); } );
	if (findRes1 != TranslationNameMap.end () )
		device._param.h._algTrns = findRes1->first;
	else {
		DLOG (INFO) << "Default algorithm is chosen for translation";
		device._param.h._algTrns = TRS_DEFAULT;
	}

	auto findRes2 = find_if (DefragmentationNameMap.begin (), DefragmentationNameMap.end (),
		[&] (const pair <OGSS_DefragmentationType, OGSS_String> & elt)
		{ return ! elt.second.compare (dfrgType); } );
	if (findRes2 != DefragmentationNameMap.end () )
		device._param.h._algDfrg = findRes2->first;
	else {
		DLOG (INFO) << "Default algorithm is chosen for defragmentation";
		device._param.h._algDfrg = DFG_DEFAULT;
	}
}

void
getSSDController (
	DOMNode					* root,
	Device					& device) {
	DOMNode					* node;
	OGSS_String				trnsType;
	OGSS_String				wlType;
	OGSS_String				gcType;

	node = _getNode (root, ParamNameMap.at (PTP_CONTROLLER) );

	trnsType = _getString (node, ParamNameMap.at (PTP_TRANSLATION), true);
	wlType = _getString (node, ParamNameMap.at (PTP_WEARLEVELLING), true);
	gcType = _getString (node, ParamNameMap.at (PTP_GARBAGECOLLECTION), true);

	auto findRes1 = find_if (TranslationNameMap.begin (), TranslationNameMap.end (),
		[&] (const pair <OGSS_TranslationType, OGSS_String> & elt)
		{ return ! elt.second.compare (trnsType); } );
	if (findRes1 != TranslationNameMap.end () )
		device._param.s._algTrns = findRes1->first;
	else {
		DLOG (INFO) << "Default algorithm is chosen for translation";
		device._param.s._algTrns = TRS_DEFAULT;
	}

	auto findRes2 = find_if (WearLevellingNameMap.begin (), WearLevellingNameMap.end (),
		[&] (const pair <OGSS_WearLevellingType, OGSS_String> & elt)
		{ return ! elt.second.compare (wlType); } );
	if (findRes2 != WearLevellingNameMap.end () )
		device._param.s._algWL = findRes2->first;
	else {
		DLOG (INFO) << "Default algorithm is chosen for wear levelling";
		device._param.s._algWL = WRL_DEFAULT;
	}

	auto findRes3 = find_if (GarbageCollectionNameMap.begin (), GarbageCollectionNameMap.end (),
		[&] (const pair <OGSS_GarbageCollectionType, OGSS_String> & elt)
		{ return ! elt.second.compare (gcType); } );
	if (findRes3 != GarbageCollectionNameMap.end () )
		device._param.s._algGC = findRes3->first;
	else {
		DLOG (INFO) << "Default algorithm is chosen for garbage collection";
		device._param.s._algGC = GCL_DEFAULT;
	}

}

OGSS_Bool
getDevice (
	const OGSS_String		deviceFile,
	Device					& device) {
	ifstream				filestream (deviceFile);
	XercesDOMParser			parser;
	DOMNode					* root;
	OGSS_String				type;
	OGSS_Ulong				physicalCapacity;

	if (! filestream.good () ) {
		LOG (FATAL) << "The device file '" << deviceFile
			<< "' does not exist!";
		return false;
	}

	filestream.close ();

	parser.parse (deviceFile.c_str () );

	root = parser.getDocument () ->getDocumentElement ();
	type = _getString (root, ParamNameMap.at (PTP_TYPE), true);

	auto findResult = find_if (DeviceNameMap.begin (), DeviceNameMap.end (),
		[&] (const pair <OGSS_DeviceType, OGSS_String> & elt)
		{ return ! elt.second.compare (type); } );
	if (findResult != DeviceNameMap.end () )
		device._type = findResult->first;
	else
		LOG (FATAL) << "The device type '" << type << "' does not match "
			<< "a known type!";

	switch (device._type) {
		case DTP_HDD:
			device._physicalCapacity = getHDDGeometry (root, device);
			getHDDPerformance (root, device);
			getHDDReliability (root, device);
			getHDDController (root, device);
			device._param.h._lastSector = 0;
			device._param.h._lastTrack = 0;
		break;
		case DTP_SSD:
			device._physicalCapacity = getSSDGeometry (root, device);
			getSSDPerformance (root, device);
			getSSDReliability (root, device);
			getSSDController (root, device);
		break;
		case DTP_NVRAM:
			device._physicalCapacity = getNVRAMGeometry(root, device);
			getNVRAMPerformance(root, device);
		break;
		default: break;
	}

	return true;
}
}

#endif
