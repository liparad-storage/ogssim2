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

//! \file			locator.cpp
//! \brief			Definition of the locator class. The locator is used to determine the location of the targeted
//!					metadata on the metadata servers (address & node ID) and the location and the size of the data on
//!					the logical storage space.

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include "locator.hpp"

#include <limits>

using namespace std;

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

Locator::Locator (
	System								& sys,
	OGXML								& parser):
	_system (sys), _parser (parser) {
	string								stratAddress {""};
	string								stratSize {""};
	uint64_t							a {1};
	string								b {""};

	pair <uint64_t, uint64_t>			vAddress {std::make_pair (0, 32) };
	pair <uint64_t, uint64_t>			vSize {std::make_pair (1, 32) };

	parser.getXMLItem <string> (stratAddress, OGFT_CFGFILE, "distribution/address");
	parser.getXMLItem <string> (stratSize, OGFT_CFGFILE, "distribution/size");
	LOG_IF (WARNING, ! parser.getXMLItem <uint64_t> (a, OGFT_CFGFILE, "distribution/datafactor") )
		<< "Tag 'distribution/datafactor' is not provided, default value (" << a << ") is selected";

	_dataFactor = static_cast <uint64_t> (a);

	LOG_IF (FATAL, ! parser.getXMLItem <string> (b, OGFT_CFGFILE, "distribution/address/max", true) )
		<< "Tag: 'distribution/address/max' is not provided";
	_maxSize = static_cast <uint64_t> (OGUtils::convertPrefix (b) );

	if (! stratAddress.compare ("sequential") ) {
		_sAddress = OGST_SEQUENTIAL;
	} else if (! stratAddress.compare ("uniform") ) {
		_sAddress = OGST_UNIFORM;
		LOG_IF (WARNING, ! parser.getXMLItem <uint64_t> (vAddress.first, OGFT_CFGFILE, "distribution/address/min", true) )
			<< "Tag: 'distribution/address/min' is not provided, default value (" << vAddress.first << ") is selected";
		LOG_IF (WARNING, ! parser.getXMLItem <uint64_t> (vAddress.second, OGFT_CFGFILE, "distribution/address/max", true) )
			<< "Tag: 'distribution/address/max' is not provided, default value (" << vAddress.second << ") is selected";

		_unifDistAddress = uniform_int_distribution <int> (vAddress.first, vAddress.second);
	} else {
		LOG (WARNING) << "Tag: 'distribution/address' is not (well?) provided, sequential strategy is chosen";
		_sAddress = OGST_SEQUENTIAL;
	}

	if (! stratSize.compare ("constant") ) {
		_sSize = OGST_CONSTANT;
		LOG_IF (WARNING, ! parser.getXMLItem <uint64_t> (_constSize, OGFT_CFGFILE, "distribution/size/value", true) )
			<< "Tag: 'distribution/size/value' is not provided, default value (" << vSize.first << ") is selected";
	} else if (! stratSize.compare ("uniform") ) {
		_sSize = OGST_UNIFORM;
		LOG_IF (WARNING, ! parser.getXMLItem <uint64_t> (vSize.first, OGFT_CFGFILE, "distribution/size/min", true) )
			<< "Tag: 'distribution/size/min' is not provided, default value (" << vSize.first << ") is selected";
		LOG_IF (WARNING, ! parser.getXMLItem <uint64_t> (vSize.second, OGFT_CFGFILE, "distribution/size/max", true) )
			<< "Tag: 'distribution/size/max' is not provided, default value (" << vSize.second << ") is selected";

		_unifDistSize = uniform_int_distribution <int> (vSize.first, vSize.second);
	} else {
		LOG (WARNING) << "Tag: 'distribution/size' is not (well?) provided, constant strategy is chosen with default value";
		_sSize = OGST_CONSTANT;
	}

	_checkFinder = vector <bool> (_maxSize / _dataFactor, false);

	string mmodel {""};
	parser.getXMLItem <string> (mmodel, OGFT_SYSFILE, "maintenance/model");

	if (! mmodel.compare ("threshold") ) {
		_maint = make_unique <ThresholdMaintenance> (_system, _parser);
	} else if (! mmodel.compare ("average") ) {
		_maint = make_unique <AverageMaintenance> (_system, _parser);
	} else if (! mmodel.compare ("no") ) {
		LOG (INFO) << "The maintenance module if off.";
	} else {
		LOG (INFO) << "The maintenance model is not recognized or was not provided in the system configuration file. "
			<< "The default behavior (no model) is selected.";
	}

	parser.getXMLItem <double> (_maintPeriod, OGFT_SYSFILE, "maintenance/period");
}

Locator::~Locator () {  }

void
Locator::locate (
	MDRequest							& req) {

	if (_maint != nullptr && req.date >= _lastMaintCall + _maintPeriod) {
		VLOG(1) << "Launching maintenance (" << req.date << " - " << _lastMaintCall << " - " << _maintPeriod << ")";
		_maint->launch ();
		_lastMaintCall = req.date;
	}

	if (_translationMap.find (req.objID) != _translationMap.end () ) {
		VLOG (1) << "Locating obj #" << req.objID << " on (" << get <0> (_translationMap [req.objID]) << ", " << get <1> (_translationMap [req.objID]) << ")";
		req.mdServ = get <0> (_translationMap [req.objID]);
		req.logAddress = get <1> (_translationMap [req.objID]);
		req.size = get <2> (_translationMap [req.objID]);
		return;
	}

	req.mdServ = _system.getNextAvailableMDServer (req.host);
	if (req.mdServ == -1)
		req.mdServ = _system.getNextOverloadedMDServer (req.host);
	LOG_IF (FATAL, req.mdServ == -1) << "All the servers are filled, can not continue the simulation";

	VLOG (1) << "Put obj.#" << req.objID << " on srv.#" << req.mdServ;

	_system.addObject (req.mdServ, req.objID);

	dataLocate (req);
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS -------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

void
Locator::dataUpdate (
	uint64_t							start,
	uint64_t							end) {
	auto								e {_checkA.end () };

	if ( (e = _checkA.find (end + 1) ) != _checkA.end () ) {
		_checkR [e->second] = start;

		_checkA [start] = e->second;
		_checkA.erase (e);
	} else if ( (e = _checkR.find (start - 1) ) != _checkR.end () ) {
		_checkA [e->second] = end;

		_checkR [end] = e->second;
		_checkR.erase (e);
	} else {
		_checkA [start] = end;
		_checkR [end] = start;
	}

	for (auto i = start; i <= end; ++i)
		_checkFinder [i] = true;
}

void
Locator::dataLocate (
	MDRequest							& req) {

	do {
		switch (_sSize) {
			case OGST_CONSTANT:
				req.size = _constSize;
				break;
			case OGST_UNIFORM:
				req.size = _unifDistSize (_gen);
				break;
			default: break;
		}

		switch (_sAddress) {
			case OGST_SEQUENTIAL:
				req.logAddress = _seqNextAddress;
				_seqNextAddress += req.size;
				break;
			case OGST_UNIFORM:
				req.logAddress = _unifDistAddress (_gen);
				break;
			default: break;
		}
	} while (! dataCheck (req.logAddress, req.logAddress + req.size - 1) );

	dataUpdate (req.logAddress, req.logAddress + req.size - 1);

	VLOG (1) << "Locating obj #" << req.objID << " on (" << req.mdServ << ", " << req.logAddress << ")";
	_translationMap [req.objID] = make_tuple (req.mdServ, req.logAddress, req.size);
}

bool
Locator::dataCheck (
	uint64_t							start,
	uint64_t							end) {
	bool								t {false};
	
	for (auto i = start; i <= end; ++i, t |= _checkFinder [i]);

	return ! t;
}
