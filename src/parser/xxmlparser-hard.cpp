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

//! \file	xxmlparser-hard.cpp
//! \brief	Namespaces used for XML parsing. Contains the functions related to
//! 		the hardware file.

/*----------------------------------------------------------------------------*/
/* HEADERS --------------------------------	-----------------------------------*/
/*----------------------------------------------------------------------------*/

#include "xxmlparser-dev.cpp"

#if ! USE_TINYXML

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

namespace {
void
getInterfaces (
	DOMNode					* root,
	vector <Interface>		& interfaces,
	map <OGSS_String, OGSS_Short>	& busMap) {
	DOMNode					* node;
	DOMNode					* item;
	DOMNodeList				* list;
	OGSS_String				busName;
	OGSS_String				bandwidth;
	OGSS_String				busType;
	OGSS_String				devPath;
	char					* tmp;
	char					unit;
	istringstream			iss;

	node = _getNode (root, ParamNameMap.at (PTP_BUSES), true);

	list = node->getChildNodes ();

	for (auto idx = 0; idx < list->getLength (); ++idx) {
		item = list->item (idx);
		tmp = _DOM_GET_STRING (item->getNodeName () );

		if (ParamNameMap.at (PTP_BUS) .compare (tmp) ) {
			_DOM_FREE_STRING (tmp);
			continue;
		}

		Interface			i;

		busName = _getString (item, ParamNameMap.at (PTP_NAME), true);
		busType = _getString (item, ParamNameMap.at (PTP_TYPE), true);
		i._bandwidth = _getLongPrefix (item, PTP_BANDWIDTH, true);

		auto findResult = find_if (InterfaceNameMap.begin (), InterfaceNameMap.end (),
			[&] (const pair <OGSS_InterfaceType, OGSS_String> & elt)
			{ return ! elt.second.compare (busType); } );
		if (findResult != InterfaceNameMap.end () )
			i._type = findResult->first;
		else
			LOG (INFO) << "The bus type '" << busType << "' does not match "
				<< "a known type!";

		interfaces.push_back (i);
		busMap [busName] = interfaces.size () - 1;

		_DOM_FREE_STRING (tmp);
	}
}

void
getSubVolumes (
	OGSS_Ushort				parent,
	DOMNode					* root,
	Hardware				& hardware,
	OGSS_Ushort				numSub) {
	DOMNodeList				* list;
	DOMNode					* item;
	OGSS_Ushort				cnt = 0;
	OGSS_String				volType;
	OGSS_String				decType;
	char					* tmp;

	list = root->getChildNodes ();

	for (auto idx = 0; idx < list->getLength (); ++idx) {
		item = list->item (idx);
		tmp = _DOM_GET_STRING (item->getNodeName () );

		if (ParamNameMap.at (PTP_SUBVOL) .compare (tmp) ) {
			_DOM_FREE_STRING (tmp);
			continue;
		}

		Volume				v;

		v._parent = parent;
		v._numSubVolumes = 0;
		v._suSize = 0;
		v._isSubVolume = true;

		volType = _getString (item, ParamNameMap.at (PTP_TYPE), true);
		auto findResult = find_if (VolumeNameMap.begin (), VolumeNameMap.end (),
			[&] (const pair <OGSS_VolumeType, OGSS_String> & elt)
			{ return ! elt.second.compare (volType); } );
		if (findResult != VolumeNameMap.end () )
			v._type = findResult->first;
		else
			LOG (FATAL) << "The bus type '" << volType << "' does not match "
				<< "a known type!";

		v._numDevices = _getLong (item, ParamNameMap.at (PTP_NBDEV), true);

		if (v._type == VTP_DECRAID)
			LOG (FATAL) << "A DRD volume can not be a subvolume";
			
		if (v._type != VTP_JBOD && v._type != VTP_RAID1) {
			v._suSize = _getLong (item, ParamNameMap.at (PTP_SUSIZE), true);
			if (v._type == VTP_RAIDNP) {
				v._numRedundancyDevices = _getLong (item,
					ParamNameMap.at (PTP_NBPAR), true);

				decType = _getString (item, ParamNameMap.at (PTP_DECL), true);
				auto fR2 = find_if (DeclusteringNameMap.begin (), DeclusteringNameMap.end (),
					[&] (const pair <OGSS_DeclusteringType, OGSS_String> & elt)
					{ return ! elt.second.compare (decType); } );
				if (fR2 != DeclusteringNameMap.end () )
					v._declustering = fR2->first;
				else {
					LOG (WARNING) << "The declustering '" << decType << "' is "
						<< "not available! 'Off' declustering is chosen instead.";
					v._declustering = DCL_OFF;
				}
			}
		}

		++ cnt;
		hardware._volumes.push_back (v);

		_DOM_FREE_STRING (tmp);
	}

	if (cnt != numSub)
		LOG (FATAL) << "The number of DecRAID subvolumes is not correctly provided";
}

void
getVolumes (
	OGSS_Ushort				parent,
	DOMNode					* root,
	Hardware				& hardware,
	const map <OGSS_String, OGSS_Short> & busMap) {
	DOMNode					* item;
	DOMNode					* cfg;
	DOMNode					* dev;
	DOMNodeList				* list;
	char					* tmp;
	OGSS_String				busName;
	OGSS_String				volType;
	OGSS_String				decType;
	OGSS_String				devPath;

	list = root->getChildNodes ();

	for (auto idx = 0; idx < list->getLength (); ++idx) {
		item = list->item (idx);
		tmp = _DOM_GET_STRING (item->getNodeName () );

		if (ParamNameMap.at (PTP_VOLUME) .compare (tmp) ) {
			_DOM_FREE_STRING (tmp);
			continue;
		}

		Volume				v;

		v._parent = parent;
		v._numSubVolumes = 0;
		v._suSize = 0;
		v._isSubVolume = false;

		busName = _getString (item, ParamNameMap.at (PTP_BUS), true);
		if (busMap.find (busName) == busMap.end () ) {
			LOG (FATAL) << "The bus name '" << busName
				<< "' is not provided!";
		} else
			v._interface = busMap.at (busName);

		cfg = _getNode (item, ParamNameMap.at (PTP_CONFIG) );

		volType = _getString (cfg, ParamNameMap.at (PTP_TYPE), true);
		auto findResult = find_if (VolumeNameMap.begin (), VolumeNameMap.end (),
			[&] (const pair <OGSS_VolumeType, OGSS_String> & elt)
			{ return ! elt.second.compare (volType); } );
		if (findResult != VolumeNameMap.end () )
			v._type = findResult->first;
		else
			LOG (FATAL) << "The bus type '" << volType << "' does not match "
				<< "a known type!";

		v._numDevices = _getLong (cfg, ParamNameMap.at (PTP_NBDEV), true);

		if (v._type == VTP_DECRAID) {
			v._suSize = _getLong (cfg, ParamNameMap.at (PTP_SUSIZE), true);
			v._numRedundancyDevices = _getLong (cfg,
				ParamNameMap.at (PTP_NBSPARE), true);
			v._numSubVolumes = _getLong (cfg, ParamNameMap.at (PTP_NBSUBVOL), true);

			volType = _getString (cfg, ParamNameMap.at (PTP_SCHEME), true);
			auto fR2 = find_if (DRDSchemeNameMap.begin (), DRDSchemeNameMap.end (),
				[&] (const pair <OGSS_DRDType, OGSS_String> & elt)
				{ return ! elt.second.compare (volType); } );
			if (fR2 != DRDSchemeNameMap.end () )
				v._decraidScheme = fR2->first;
			else
				LOG (FATAL) << "The DRD scheme '" << volType << "' does not match "
				<< "a known type!";

			dev = _getNode (item, ParamNameMap.at (PTP_DEVICE) );

			Device				d;
			devPath = _getString (dev, ParamNameMap.at (PTP_PATH), true);

			d._parent = hardware._volumes.size ();

			getDevice (devPath, d);

			hardware._volumes.push_back (v);

			getSubVolumes (parent, item, hardware, v._numSubVolumes);

			for (auto i = 0; i < v._numDevices; ++i)
				hardware._devices.push_back (d);
		} else {
			if (v._type != VTP_JBOD && v._type != VTP_RAID1) {
				v._suSize = _getLong (cfg, ParamNameMap.at (PTP_SUSIZE), true);
				if (v._type == VTP_RAIDNP) {
					v._numRedundancyDevices = _getLong (cfg,
						ParamNameMap.at (PTP_NBPAR), true);

					decType = _getString (cfg, ParamNameMap.at (PTP_DECL), true);
					auto fR2 = find_if (DeclusteringNameMap.begin (), DeclusteringNameMap.end (),
						[&] (const pair <OGSS_DeclusteringType, OGSS_String> & elt)
						{ return ! elt.second.compare (decType); } );
					if (fR2 != DeclusteringNameMap.end () )
						v._declustering = fR2->first;
					else {
						LOG (WARNING) << "The declustering '" << decType << "' is "
							<< "not available! 'Off' declustering is chosen instead.";
						v._declustering = DCL_OFF;
					}
				}
			}

			dev = _getNode (item, ParamNameMap.at (PTP_DEVICE) );

			Device				d;
			devPath = _getString (dev, ParamNameMap.at (PTP_PATH), true);

			d._parent = hardware._volumes.size ();

			getDevice (devPath, d);

			hardware._volumes.push_back (v);

			for (auto i = 0; i < v._numDevices; ++i)
				hardware._devices.push_back (d);
		}

		_DOM_FREE_STRING (tmp);
	}
}

void
getTiers (
	DOMNode					* root,
	Hardware				& hardware,
	const map <OGSS_String, OGSS_Short>	& busMap) {
	DOMNode					* item;
	DOMNodeList				* list;
	OGSS_String				busName;
	OGSS_Ushort				cnt = 0;
	char					* tmp;

	list = root->getChildNodes ();

	for (auto idx = 0; idx < list->getLength (); ++idx) {
		item = list->item (idx);
		tmp = _DOM_GET_STRING (item->getNodeName () );

		if (ParamNameMap.at (PTP_TIER) .compare (tmp) ) {
			_DOM_FREE_STRING (tmp);
			continue;
		}

		Tier				t;
		busName = _getString (item, ParamNameMap.at (PTP_BUS), true);

		if (busMap.find (busName) == busMap.end () ) {
			LOG (WARNING) << "The bus name '" << busName
				<< "' is not provided!";
		} else
			t._interface = busMap.at (busName);

		hardware._tiers.push_back (t);

		getVolumes (cnt++, item, hardware, busMap);

		_DOM_FREE_STRING (tmp);
	}
}

void
getSystem (
	DOMNode					* root,
	Hardware				& hardware,
	map <OGSS_String, OGSS_Short>	busMap) {
	DOMNode					* node;
	OGSS_String				busName;

	node = _getNode (root, ParamNameMap.at (PTP_SYSTEM), true);

	busName = _getString (node, ParamNameMap.at (PTP_BUS), true);

	if (busMap.find (busName) == busMap.end () )
		LOG (WARNING) << "The bus name '" << busName
				<< "' is not provided!";
	else
		hardware._param._hostInterface = busMap.at (busName);

	getTiers (node, hardware, busMap);
}
}

/*----------------------------------------------------------------------------*/
/* PUBLIC NAMESPACE ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

OGSS_Bool
XMLParser::getHardwareConfiguration (
	const OGSS_String		& hardwareFile,
	Hardware				& hardware) {
	ifstream				filestream (hardwareFile);
	XercesDOMParser			parser;
	DOMNode					* root;
	map <OGSS_String, OGSS_Short>	busMap;

	LOG_IF (FATAL, !filestream.good () ) << "The hardware file '"
		<< hardwareFile << "'does not exist!";

	filestream.close ();

	parser.parse (hardwareFile.c_str () );

	root = parser.getDocument () ->getDocumentElement ();

	getInterfaces (root, hardware._interfaces, busMap);
	getSystem (root, hardware, busMap);

	hardware._param._numInterfaces = hardware._interfaces.size ();
	hardware._param._numTiers = hardware._tiers.size ();
	hardware._param._numVolumes = hardware._volumes.size ();
	hardware._param._numDevices = hardware._devices.size ();

	DLOG(INFO) << "The system is composed of "
		<< hardware._param._numInterfaces << " interfaces, "
		<< hardware._param._numTiers << " tiers, "
		<< hardware._param._numVolumes << " volumes, "
		<< hardware._param._numDevices << " devices";

	return true;
}

OGSS_Short
XMLParser::getNumberOfVolumes (
	const OGSS_String		& hardwareFile) {
	ifstream				filestream (hardwareFile);
	XercesDOMParser			parser;
	DOMNode					* node;
	DOMNode					* tier;
	DOMNode					* vol;
	DOMNodeList				* tierList;
	DOMNodeList				* volList;
	OGSS_Short				counter = 0;
	char					* tmp;

	LOG_IF (FATAL, !filestream.good () ) << "The hardware file '"
		<< hardwareFile << "'does not exist!";


	filestream.close ();

	parser.parse (hardwareFile.c_str () );

	node = parser.getDocument () ->getDocumentElement ();
	node = _getNode (node, ParamNameMap.at (PTP_SYSTEM), true);

	tierList = node->getChildNodes ();

	for (auto idx = 0; idx < tierList->getLength (); ++idx) {
		tier = tierList->item (idx);
		tmp = _DOM_GET_STRING (tier->getNodeName () );

		if (ParamNameMap.at (PTP_TIER) .compare (tmp) ) {
			_DOM_FREE_STRING (tmp);
			continue;
		}

		_DOM_FREE_STRING (tmp);
		volList = tier->getChildNodes ();

		for (auto idy = 0; idy < volList->getLength (); ++idy) {
			vol = volList->item (idy);
			tmp = _DOM_GET_STRING (vol->getNodeName () );

			if (ParamNameMap.at (PTP_VOLUME) .compare (tmp) ) {
				_DOM_FREE_STRING (tmp);
				continue;
			}

			_DOM_FREE_STRING (tmp);
			++ counter;
		}
	}

	return counter;
}

#endif
