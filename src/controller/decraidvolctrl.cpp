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

//! \file	decraidvolctrl.cpp
//! \brief	Definition of the declustered RAID controller.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "controller/decraidvolctrl.hpp"

#include "controller/jbodvolctrl.hpp"
#include "controller/raid01volctrl.hpp"
#include "controller/raid1volctrl.hpp"
#include "controller/raidnpdatdecvolctrl.hpp"
#include "controller/raidnpnodecvolctrl.hpp"
#include "controller/raidnppardecvolctrl.hpp"

#include "controller/perfparityctrl.hpp"

#include "scheme/sd2sscheme.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

DecRAIDVolCtrl::DecRAIDVolCtrl (
	const Volume			& vol,
	const Device			& dev,
	const vector <Volume>	& subVols) {
	_numBytesByDev			= dev._physicalCapacity;
	_numBytesBySU			= vol._suSize;
	_numDevices 			= vol._numDevices;
	_numSpare				= vol._numRedundancyDevices;

	DLOG (INFO) << "[DRD] We got " << _numBytesByDev << " MU by device";
	DLOG (INFO) << "[DRD] For a total of " << 12 * _numBytesByDev << " MUs";

	updateMapping (vol, dev, subVols);
	instanciateScheme (vol, subVols, dev);
}

DecRAIDVolCtrl::~DecRAIDVolCtrl () {  }

void
DecRAIDVolCtrl::decompose (
	Request					& request,
	vector <Request>		& subrequests) {
	auto idxLDev = request._volumeAddress / _numBytesByDev;

	OGSS_Ushort previousIdx = 0;
	OGSS_Ulong previousOff = 0;

	for (auto elt: _redirectionTable) {
		if (request._volumeAddress < elt.second) {
			Request			block {request};

			block._volumeAddress -= previousOff;
			request._volumeAddress -= previousOff;

			_volCtrls [elt.first] ->getBlockLocation (block);

			block._volumeAddress += previousOff;
			block._idxDevice += previousIdx;
			_scheme->realloc (block);

			_volCtrls [elt.first] ->decompose (request, subrequests);

			for (auto & flt: subrequests) {
				flt._volumeAddress += previousOff;
				flt._idxDevice += previousIdx;
				_scheme->realloc (flt);
			}

			request._idxDevice = block._idxDevice;

			break;
		}

		previousIdx = elt.first;
		previousOff = elt.second;
	}
}

void
DecRAIDVolCtrl::updateScheme (
	const Request			& event) {
	if (event._type == RQT_EVFLT)
		_scheme->updateFailureScheme (event);

	else if (event._type == RQT_EVRPL)
		_scheme->updateRenewalScheme (event);
}

void
DecRAIDVolCtrl::generateDecraidFailureRequests (
	Request			& block,
	vector <Request>		& subrequests) {  }

void
DecRAIDVolCtrl::generateDecraidRenewalRequests (
	Request			& block,
	vector <Request>		& subrequests) {  }

void
DecRAIDVolCtrl::generateFailureRequests (
	Request								& block,
	vector <Request>					& subrequests) {
	Request								tmp;
	OGSS_Ushort							previous {0};

	tmp = _scheme->getLogicalBlock (block);

	if (tmp._idxVolume == OGSS_USHORT_MAX) return;

	for (auto elt: _redirectionTable) {
		if (tmp._idxDevice < elt.first) {
			tmp._idxDevice -= previous;

			auto idx = subrequests.size ();
			auto base = idx;

			_volCtrls [elt.first] ->generateDecraidFailureRequests (tmp, subrequests);

			for (; idx < subrequests.size (); ++idx) {
				subrequests [idx] ._idxDevice += previous;

				if (subrequests [idx] ._type == RQT_READ)
					subrequests [idx] ._operation = ROP_RECOVERY;
				else
					subrequests [idx] ._operation = ROP_UPDATE;

				_scheme->realloc (subrequests [idx] );

				if (subrequests [idx] ._idxDevice == OGSS_USHORT_MAX) {
					subrequests.erase (subrequests.begin () + base, subrequests.begin () + subrequests.size () );
					break;
				}
			}

			break;
		}

		previous = elt.first;
	}

}

void
DecRAIDVolCtrl::generateRenewalRequests (
	Request								& block,
	vector <Request>					& subrequests) {
	Request								tmp;
	OGSS_Ushort							previous {0};

	tmp = _scheme->getLogicalBlock (block);

	if (tmp._idxVolume == OGSS_USHORT_MAX) return;

	for (auto elt: _redirectionTable) {
		if (tmp._idxDevice < elt.first) {
			tmp._idxDevice -= previous;

			auto idx = subrequests.size ();
			auto base = idx;

			_volCtrls [elt.first] ->generateDecraidRenewalRequests (tmp, subrequests);

			for (; idx < subrequests.size (); ++idx) {
				subrequests [idx] ._idxDevice += previous;

				_scheme->realloc (subrequests [idx]);

				if (subrequests [idx] ._idxDevice == OGSS_USHORT_MAX) {
					subrequests.erase (subrequests.begin () + base, subrequests.begin () + subrequests.size () );
					break;
				}
			}

			break;
		}

		previous = elt.first;
	}
}

void
DecRAIDVolCtrl::requestMultiplier (
	Request					& block,
	vector <Request>		& subrequests) {  }

void
DecRAIDVolCtrl::getBlockLocation (
	Request					& block) {  }

OGSS_Ulong
DecRAIDVolCtrl::getNumberDataBlocks (
	OGSS_Ulong				idxDevice) {
	OGSS_Ulong				counter {0};
	for (auto i = 0; i < _numDevices - _numSpare; ++i) {
		Request				r;
		r._idxDevice = idxDevice;		r._deviceAddress = i * _numBytesBySU;
		auto				tmp {_scheme->getLogicalBlock (r) };
		LOG(INFO) << i << " -> " << tmp._idxDevice;
		if (tmp._idxDevice < _numDevices - _numSpare) ++ counter;
		else LOG(INFO) << "Spare block";
	}

	counter *= ( (_numBytesByDev / _numBytesBySU) / (_numDevices - _numSpare) );

	auto s { (_numBytesByDev / _numBytesBySU) % (_numDevices - _numSpare) };
	for (auto i = 0; i < s; ++i) {
		Request				r;
		r._idxDevice = idxDevice;		r._deviceAddress = i * _numBytesBySU;
		auto				tmp {_scheme->getLogicalBlock (r) };
		if (tmp._idxDevice < _numDevices - _numSpare) ++ counter;
		else LOG(INFO) << "Spare block";
	}

	return counter;
}

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
DecRAIDVolCtrl::updateMapping (
	const Volume			& vol,
	const Device			& dev,
	const vector <Volume>	& subVols) {
	OGSS_Ulong				counter = 0;
	OGSS_Ulong				devCnt = 0;

	_volCtrls [0] = nullptr;
	_redirectionTable [0] = 0;

	for (auto elt: subVols) {
		switch (elt._type) {
		case VTP_JBOD:
			devCnt += elt._numDevices;
			counter += elt._numDevices * _numBytesByDev;
			_redirectionTable [devCnt] = counter;
			_volCtrls [devCnt] = make_unique <JBODVolCtrl> (elt, dev);
			DLOG (INFO) << "Controller JBOD on " << devCnt;
			break;
		case VTP_RAID1: case VTP_RAID01:
			devCnt += elt._numDevices;
			counter += (elt._numDevices / 2) * _numBytesByDev;
			_redirectionTable [devCnt] = counter;
			if (elt._type == VTP_RAID1) 
				_volCtrls [devCnt] = make_unique <RAID1VolCtrl> (elt, dev);
			else
				_volCtrls [devCnt] = make_unique <RAID01VolCtrl> (elt, dev);
			DLOG (INFO) << "Controller Mirror on " << devCnt;
			break;
		case VTP_RAIDNP:
			devCnt += elt._numDevices;
			counter += (elt._numDevices - elt._numRedundancyDevices) * _numBytesByDev;
			_redirectionTable [devCnt] = counter;
			DLOG (INFO) << "Controller Parity on " << devCnt;
/*			switch (elt._declustering) {
			case DCL_OFF:
				_volCtrls [devCnt] = make_unique <RAIDNPNoDecVolCtrl> (elt, dev);
				break;
			case DCL_PARITY:
				_volCtrls [devCnt] = make_unique <RAIDNPParDecVolCtrl> (elt, dev);
				break;
			case DCL_DATA:
				_volCtrls [devCnt] = make_unique <RAIDNPDatDecVolCtrl> (elt, dev);
				break;
			default: break;
			}
*/			_volCtrls [devCnt] = make_unique <PerfectParityCtrl> (elt, dev);
			break;
		default: break;
		}
	}
}

void
DecRAIDVolCtrl::instanciateScheme (
	const Volume			& vol,
	const vector <Volume>	& subVols,
	const Device			& dev) {
	switch (vol._decraidScheme) {
	case DRD_SD2S:
		_scheme = make_unique <SD2SScheme> (vol, subVols, dev);
		break;
	default: break;
	}

	_scheme->build ();
}
