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

//! \file			maintenance.cpp
//! \brief			Definition of the maintenance classes. The maintenance classes are used by the locator if a
//!					maintenance routine was asked by the user. This maintenance routine is called periodically and
//!					use a metadata migration process to balance the server loads.

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include "maintenance.hpp"

#include <cstdlib>

using namespace std;

/*--------------------------------------------------------------------------------------------------------------------*/
/* PARENT CLASS -- PUBLIC FUNCTIONS ----------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

Maintenance::Maintenance (
	System								& sys,
	OGXML								& parser):
	_system (sys), _parser (parser) {

	parser.getXMLItem <string> (_outPath, OGFT_SYSFILE, "maintenance/output");

	if (_outPath.compare ("") ) {
		_doOutput = true;
		_output.open (_outPath);
	} else _doOutput = false;
}

Maintenance::~Maintenance () {
	_output.close ();
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* PARENT CLASS -- PRIVATE FUNCTIONS ---------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

uint64_t
Maintenance::getAverageLoad () {
	uint64_t avgLoad {0};
	for (auto i = _system.getNbHostServers (); i < _system.getNbServers (); ++i)
		avgLoad += _system.getLoad (i);

	return avgLoad / _system.getNbMetadataServers ();
}

void
Maintenance::writeToOutput () {
	if (! _doOutput) return;

	for (auto i = _system.getNbHostServers (); i < _system.getNbServers (); ++i) {
		if (i < _system.getNbServers () - 1) _output << 100. * _system.getLoad (i) / _system.getMaxServerSize () << " ";
		else _output << 100. * _system.getLoad (i) / _system.getMaxServerSize ();
	}

		_output << endl;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* CHILD CLASSES -----------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

ThresholdMaintenance::ThresholdMaintenance (
	System								& sys,
	OGXML								& parser):
	Maintenance (sys, parser) {

	parser.getXMLItem <double> (_loadLimit.soft, OGFT_SYSFILE, "maintenance/softlim");
	parser.getXMLItem <double> (_loadLimit.hard, OGFT_SYSFILE, "maintenance/hardlim");
	parser.getXMLItem <int> (_histSpanlife, OGFT_SYSFILE, "maintenance/histsize");
	parser.getXMLItem <double> (_oblivRate, OGFT_SYSFILE, "maintenance/oblivrate");

	_hist = vector <queue <uint64_t>> (_system.getNbServers (), queue <uint64_t> () );
	_sLoads = vector <uint64_t> (_system.getNbServers (), 0);
}

ThresholdMaintenance::~ThresholdMaintenance () {
	_hist.clear ();
	_sLoads.clear ();
}

void
ThresholdMaintenance::launch () {
	static bool							firstLaunch = true;
	int									i {_system.getNbHostServers ()}, j {i};

	if (firstLaunch) {
		writeInitToOutput ();
		firstLaunch = false;
	}

	writeToOutput ();

	if (! check () ) { updateHist (); writeToOutput (); return;}
	if (getAverageLoad () >= _loadLimit.soft && ! checkUnderSoftLimit () )
		{ updateHist (); writeToOutput (); return; }

	for (; i < _system.getNbServers (); ++i) {
		while ( ( (double) _system.getLoad (i) / _system.getMaxServerSize () ) > _loadLimit.soft) {
			auto tmp = _system.rmObject (i);
			for (; j < _system.getNbServers () && ( (double) _system.getLoad (j) ) / _system.getMaxServerSize () >= _loadLimit.soft; ++j);
			if (j >= _system.getNbServers () ) break;
			_system.addObject (j, tmp);
		}
	}

	updateHist ();
	writeToOutput ();
}

bool
ThresholdMaintenance::check () {
	for (auto i = _system.getNbHostServers (); i < _system.getNbServers (); ++i) {
		if ( (_system.getLoad (i) * (1 - _oblivRate)
		  + _sLoads [i] * _oblivRate) / _system.getMaxServerSize ()
		 >= _loadLimit.hard)
			return true;
	}

	return false;
}

bool
ThresholdMaintenance::checkUnderSoftLimit () {
	for (auto i = _system.getNbHostServers (); i < _system.getNbServers (); ++i)
		if ( ( (double) _system.getLoad (i) ) / _system.getMaxServerSize () < _loadLimit.soft) return true;

	return false;
}

void
ThresholdMaintenance::updateHist () {
	if (_hist [_system.getNbHostServers ()] .size () < _histSpanlife) {
		for (auto i = _system.getNbHostServers (); i < _system.getNbServers (); ++i) {
			auto tmp {_system.getLoad (i)};
			_hist [i] .push (tmp);
			_sLoads [i] += tmp;
		}
	} else {
		for (auto i = _system.getNbHostServers (); i < _system.getNbServers (); ++i) {
			auto tmp {_hist [i] .front ()};
			_hist [i] .pop ();
			_sLoads [i] -= tmp;
			tmp = _system.getLoad (i);
			_hist [i] .push (tmp);
			_sLoads [i] += tmp;
		}
	}
}

void
ThresholdMaintenance::writeInitToOutput () {
	if (! _doOutput) return;

	_output << _system.getNbMetadataServers () << " " << _loadLimit.soft * 100 << " " << _loadLimit.hard * 100 << endl;
}

AverageMaintenance::AverageMaintenance (
	System								& sys,
	OGXML								& parser):
	Maintenance (sys, parser) {

}

AverageMaintenance::~AverageMaintenance () {  }

void
AverageMaintenance::launch () {
	static bool							firstLaunch = true;
	int									i {_system.getNbHostServers ()}, j {i};

	if (firstLaunch) {
		writeInitToOutput ();
		firstLaunch = false;
	}

	writeToOutput ();
	if (! check () ) { writeToOutput (); return; }

	auto avgLoad {getAverageLoad ()};

	for (; i < _system.getNbServers (); ++i) {
		while (_system.getLoad (i) > avgLoad + 1) {
			auto tmp = _system.rmObject (i);
			for (; _system.getLoad (j) >= avgLoad; ++j);
			_system.addObject (j, tmp);
		}
	}

	writeToOutput ();
}

bool
AverageMaintenance::check () {
	auto avg {getAverageLoad ()};

	for (auto i = _system.getNbHostServers (); i < _system.getNbServers (); ++i)
		if (_system.getLoad (i) > avg + 1) return true;

	return false;
}

void
AverageMaintenance::writeInitToOutput () {
	if (! _doOutput) return;

	_output << _system.getNbMetadataServers () << endl;
}
