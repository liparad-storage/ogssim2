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

//! \file	txmlparser-hard.cpp
//! \brief	Namespaces used for XML parsing. Contains the functions related to
//! 		the hardware file.

/*----------------------------------------------------------------------------*/
/* HEADERS --------------------------------	-----------------------------------*/
/*----------------------------------------------------------------------------*/

#include "txmlparser-dev.cpp"

#if USE_TINYXML

/*----------------------------------------------------------------------------*/
/* CONSTANT CONTAINERS -------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

namespace {
void
getInterfaces (
	XMLElement				* root,
	vector <Interface>		& interfaces,
	map <OGSS_String, OGSS_Short>	& busMap) {
	XMLElement				* node;
	OGSS_String				busName;
	OGSS_String				busType;

	node = _getNode (root, ParamNameMap.at (PTP_BUSES), true);

	node = node->FirstChildElement (ParamNameMap.at (PTP_BUS) .c_str () );

	while (node) {
		Interface			i;

		busName = _getString (node, ParamNameMap.at (PTP_NAME), true);
		busType = _getString (node, ParamNameMap.at (PTP_TYPE), true);
		i._bandwidth = _getLongPrefix (node, PTP_BANDWIDTH, true);

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

		node = node->NextSiblingElement (ParamNameMap.at (PTP_BUS) .c_str () );
	}
}

void
getSubVolumes (
	OGSS_Ushort				parent,
	XMLElement				* root,
	Hardware				& hardware,
	OGSS_Ushort				numSub) {
	XMLElement				* item;
	OGSS_Ushort				cnt = 0;
	OGSS_String				volType;
	OGSS_String				decType;

	item = root->FirstChildElement (ParamNameMap.at (PTP_SUBVOL) .c_str () );

	while (item) {
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

		item = item->NextSiblingElement (ParamNameMap.at (PTP_SUBVOL) .c_str () );
	}

	if (cnt != numSub)
		LOG (FATAL) << "The number of DecRAID subvolumes is not correctly provided";
}

void
getVolumes (
	OGSS_Ushort				parent,
	XMLElement				* root,
	Hardware				& hardware,
	const map <OGSS_String, OGSS_Short> & busMap) {
	XMLElement				* item;
	XMLElement				* cfg;
	XMLElement				* dev;
	OGSS_String				busName;
	OGSS_String				volType;
	OGSS_String				decType;
	OGSS_String				devPath;

	item = root->FirstChildElement (ParamNameMap.at (PTP_VOLUME) .c_str () );

	while (item) {
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

		item = item->NextSiblingElement (ParamNameMap.at (PTP_VOLUME) .c_str () );
	}
}

void
getTiers (
	XMLElement				* root,
	Hardware				& hardware,
	const map <OGSS_String, OGSS_Short>	& busMap) {
	XMLElement				* item;
	OGSS_String				busName;
	OGSS_Ushort				cnt = 0;

	item = root->FirstChildElement (ParamNameMap.at (PTP_TIER) .c_str () );

	while (item) {
		Tier				t;
		busName = _getString (item, ParamNameMap.at (PTP_BUS), true);

		if (busMap.find (busName) == busMap.end () ) {
			LOG (WARNING) << "The bus name '" << busName
				<< "' is not provided!";
		} else
			t._interface = busMap.at (busName);

		hardware._tiers.push_back (t);

		getVolumes (cnt++, item, hardware, busMap);

		item = item->NextSiblingElement (ParamNameMap.at (PTP_TIER) .c_str () );
	}
}

void
getSystem (
	XMLElement				* root,
	Hardware				& hardware,
	map <OGSS_String, OGSS_Short>	busMap) {
	XMLElement				* node;
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
	XMLDocument				doc;
	XMLElement				* root {_getRootNode (hardwareFile, doc) };
	map <OGSS_String, OGSS_Short>	busMap;

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
	XMLDocument				doc;
	XMLElement				* root {_getRootNode (hardwareFile, doc) };
	XMLElement				* tier;
	XMLElement				* vol;
	OGSS_Short				counter = 0;

	root = _getNode (root, ParamNameMap.at (PTP_SYSTEM), true);

	tier = root->FirstChildElement (ParamNameMap.at (PTP_TIER) .c_str () );

	while (tier) {
		vol = tier->FirstChildElement (ParamNameMap.at (PTP_VOLUME) .c_str () );

		while (vol) {
			++ counter;
			vol = vol->NextSiblingElement (ParamNameMap.at (PTP_VOLUME) .c_str () );
		}

		tier = tier->NextSiblingElement (ParamNameMap.at (PTP_TIER) .c_str () );
	}

	return counter;
}

#endif
