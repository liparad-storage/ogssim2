/*
 * Copyright UVSQ - CEA/DAM/DIF (2018)
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

//! \file	sd2sscheme.cpp
//! \brief	Describes the SD2S (Symmetric Difference of Source Sets) scheme
//!			for declustered RAID.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "scheme/sd2sscheme.hpp"

#include <set>

#include "util/math.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

#define _half_up(i) ((i/2)+1-((i+1)%2))

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC MEMBER FUNCTIONS ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

SD2SScheme::SD2SScheme (
	const Volume			& vol,
	const vector <Volume>	& subVols,
	const Device			& dev):
	DecRAIDScheme (vol) {
	_lambda = 0;

	_numDevices = vol._numDevices;
	_numSpareDevices = vol._numRedundancyDevices;
	_numDataDevices = vol._numDevices - vol._numRedundancyDevices;
	_numBytesBySU = vol._suSize;
	_numBytesByDev = dev._physicalCapacity;

	for (auto i = 0; i < subVols.size (); ++i)
		for (auto j = 0; j < subVols [i] ._numDevices; ++j)
			_belongs.push_back (i);

	for (auto i = 0; i < _numSpareDevices; ++i)
		_belongs.push_back (OGSS_ULONG_MAX);
}
SD2SScheme::~SD2SScheme () {  }

void
SD2SScheme::build () { findFirstGoodLambda (); }

/*void
SD2SScheme::realloc (
	Request					& request) {
	OGSS_Ulong				stripe;

	stripe = request._deviceAddress / _numBytesBySU;
	stripe %= _numDataDevices;

	request._idxDevice = (request._idxDevice + stripe * _lambda) % _numDevices;
	request._nativeIdxDevice = request._idxDevice;
	request._nativeDeviceAddress = request._deviceAddress;

	if (_eventTimes.find (request._idxDevice) != _eventTimes.end () ) {
		if (request._operation == ROP_DBGNATIVE) {
		} else if (request._operation == ROP_DBGREDIRECTED) {
			request._idxDevice = _redirection [request._idxDevice][stripe];
		} else if (request._operation == ROP_COPYBACK) {
			if (request._type == RQT_READ)
				request._idxDevice = _redirection [request._idxDevice][stripe];
		} else if (! request._multiple && _eventTimes [request._idxDevice] .first <= request._date) {
			request._idxDevice = _redirection [request._idxDevice][stripe];
		} else {
			if (_eventTimes [request._idxDevice] .second <= request._date) {
				if (request._operation == ROP_REDIRECTED) {
					// Do nothing
				}
				else if (request._operation == ROP_COPY) {
					if (request._type == RQT_READ)
						request._idxDevice = _redirection [request._idxDevice][stripe];
					// else
						// Do nothing
				} // else if (request._operation == ROP_RECOVER)
					// Do nothing
			} else {
				if (request._operation == ROP_REDIRECTED || request._operation == ROP_RECOVER)
					request._idxDevice = _redirection [request._idxDevice][stripe];
				// else
					// Do nothing
			}
		}
	}
}*/

void
SD2SScheme::realloc (
	Request								& request) {
	auto								stripe { (request._deviceAddress / _numBytesBySU) % _numDataDevices};

	request._idxDevice = (request._idxDevice + stripe * _lambda) % _numDevices;
	request._nativeIdxDevice = request._idxDevice;
	request._nativeDeviceAddress = request._deviceAddress;

	if (request._type == RQT_READ) {
		if (request._operation == ROP_COPY)
			request._idxDevice = _redirection [request._idxDevice][stripe];
		else if (request._operation == ROP_NATIVE || request._operation == ROP_RECOVERY) {
			while (_deviceState [request._idxDevice] .isFailed (request._date) )
				request._idxDevice = _redirection [request._idxDevice][stripe];
		}
	} else if (request._type == RQT_WRITE) {
		if (request._operation == ROP_UPDATE)
			request._idxDevice = _redirection [request._idxDevice][stripe];
		else if (request._operation == ROP_NATIVE) {
			while (_deviceState [request._idxDevice] .isFailed (request._date) )
				request._idxDevice = _redirection [request._idxDevice][stripe];
		}
	}
}

void
SD2SScheme::updateFailureScheme (
	const Request			& event) {
	OGSS_Ushort				logicalDevice;
	OGSS_Ushort				idxVector;
	OGSS_Ushort				firstIdx;
	OGSS_Ushort				firstIdxVec;
	OGSS_Ushort				redLogicalDevice;

	DLOG (INFO) << "Number of redirection vectors: " << _redirection.size ();

	_eventTimes [event._idxDevice] .first = event._date;
	_eventTimes [event._idxDevice] .second = OGSS_REAL_MAX;
	_deviceState [event._idxDevice] ._failureDate = event._date;

	if (_eventTimes.size () > _numSpareDevices) {
		DLOG (INFO) << "The Declustered RAID cannot be rebuilt!";
		_redirection [event._idxDevice] = vector <OGSS_Ushort> (_numDataDevices, OGSS_USHORT_MAX);
		return;
	}

	for (auto i = 0; i < _numDataDevices; ++i) {
		// First check if there is native data
		logicalDevice
			= (_numDevices * i + event._idxDevice - i * _lambda) % _numDevices;
		idxVector = event._idxDevice;
		if (logicalDevice >= _numDataDevices) { // A spare disk
			// Else check for rebuilt data
			for (OGSS_Ushort j = 0; j < _numDevices; ++j) {
				if (_redirection.find (j) != _redirection.end () ) {
					if (_redirection [j][i] == event._idxDevice) {
						logicalDevice
							= (_numDevices * i + j - i * _lambda)
							% _numDevices;
						idxVector = j;
						break;
					}
				}
			}
		}

		if (logicalDevice >= _numDataDevices) continue;

		if (_redirection.find (event._idxDevice) == _redirection.end () ) {
			_redirection [event._idxDevice]	= vector <OGSS_Ushort> (
				_numDataDevices, _numDevices);
		}
		
		firstIdx = _numDevices;
		for (OGSS_Ushort j = 0; j < _numDevices; ++j) {
			// If spare area
			if ( (_numDevices * i + j - i * _lambda) % _numDevices >= _numDataDevices) {
				redLogicalDevice = 0;

				// If device is faulty and not renewed
				if (_eventTimes.find (j) != _eventTimes.end ()
				 && _eventTimes [j] .first <= event._date
				 && _eventTimes [j] .second > event._date) {
					redLogicalDevice = _numDevices;
					continue;
				}

				// If the spare area is not already used
				for (auto elt: _redirection) {
					if (elt.second [i] == j) {
						redLogicalDevice = _numDevices;
						break;
					}
				}

				// If one condition is not respected
				if (redLogicalDevice == _numDevices)
					continue;

				OGSS_Bool test = true;

				for (OGSS_Ushort k = 0;
				  k < _numDataDevices; ++k) {
					if ( (_numDevices * k + j - k * _lambda) % _numDevices
						== logicalDevice)
					{ test = false; break; }
				}

				for (OGSS_Ushort k = 0; k < _numDevices && test; ++k) {
					if (_redirection.find (k) != _redirection.end ()
					 && _eventTimes [k] .first <= event._date
					 && _eventTimes [k] .second > event._date) {
						for (OGSS_Ushort l = 0; l < _numDataDevices; ++l) {
							if (_redirection [k][l] == j
								&& (_numDevices * l + k - l * _lambda)
								% _numDevices == logicalDevice) {
								test = false; break;
							}
						}
					}
				}

				if (!test) {
					if (firstIdx == _numDevices) {
						firstIdx = j;
						firstIdxVec = idxVector;
					}

					redLogicalDevice = _numDevices;
				}

				// If one condition is not respected
				if (redLogicalDevice == _numDevices)
					continue;

				_redirection [idxVector][i] = j;

				break;
			}
		}

		if (redLogicalDevice == _numDevices)
			_redirection [firstIdxVec][i] = firstIdx;
	}

	DLOG (INFO) << "Number of redirection vectors: " << _redirection.size ();
	DLOG (INFO) << "Redirection vector after event on " << event._idxDevice << ":";

	for (auto i = 0; i < _redirection [event._idxDevice] .size (); ++i) {
		DLOG (INFO) << "[" << i << "]" << " -> " << _redirection [event._idxDevice][i];
	}
}

void
SD2SScheme::generateFailureRequests (
	const Request			& block,
	vector <Request>		& subrequests) {
}

void
SD2SScheme::updateRenewalScheme (
	const Request			& event) {
	_eventTimes [event._idxDevice] .second = event._date;
	_deviceState [event._idxDevice] ._renewalDate = event._date;
}

void
SD2SScheme::generateRenewalRequests (
	const Request			& block,
	vector <Request>		& subrequests) {
	
	if (_redirection [block._idxDevice][(block._deviceAddress / _numBytesBySU) % _numDataDevices] == _numDevices)
		return;

	Request r {block}, s {block};
	r._idxDevice = _redirection [block._idxDevice][(block._deviceAddress / _numBytesBySU) % _numDataDevices];
	r._size = _numBytesBySU;
	r._date = OGSS_REAL_MAX; r._type = RQT_READ; r._numPrioChild = 1;
	r._numChild = r._deviceAddress / _numBytesBySU; r._system = true;

	s._idxDevice = block._idxDevice;
	s._size = _numBytesBySU;
	s._date = OGSS_REAL_MAX; s._type = RQT_WRITE; s._numPrioChild = 0;
	s._numChild = s._deviceAddress / _numBytesBySU; s._system = true;

	subrequests.push_back (r);
	subrequests.push_back (s);
}

Request
SD2SScheme::getLogicalBlock (
	const Request			& block) {
	Request					nBlock {block};
	OGSS_Ushort				logicalDevice;
	OGSS_Ulong				idx {(block._deviceAddress / _numBytesBySU) % _numDataDevices};
//	OGSS_Ushort				redLogicalDevice;

	// First check if there is native data
	logicalDevice
		= (_numDevices * _numDataDevices + block._idxDevice - idx * _lambda) % _numDevices;

	if (logicalDevice >= _numDataDevices) { // A spare space
		// Else check for rebuilt data
		auto				found {false};
		for (OGSS_Ushort j = 0; j < _numDevices; ++j) {
			if (_redirection.find (j) != _redirection.end () ) {
				if (_redirection [j][idx] == block._idxDevice) {
					logicalDevice
						= (_numDevices * _numDataDevices + j - idx * _lambda) % _numDevices;
					found = true;
					break;
				}
			}
		}

		if (! found) logicalDevice = _numDataDevices;
	}

	if (logicalDevice >= _numDataDevices) {
		nBlock._idxVolume = OGSS_USHORT_MAX;
		nBlock._idxDevice = _numDataDevices;
		return nBlock;
	}

	nBlock._idxDevice = logicalDevice;
	nBlock._idxVolume = _belongs [logicalDevice];

	return nBlock;
}

void
SD2SScheme::requestMultiplier (
	const Request			& event,
	vector <Request>		& nativeSubrequests,
	vector <Request>		& multipSubrequests) {
	
}

/*----------------------------------------------------------------------------*/
/* PRIVATE MEMBER FUNCTION ---------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
SD2SScheme::findFirstGoodLambda () {
	OGSS_Bool				found {false};

	for (int i = 1; i < _half_up (_numDevices); ++i) {
		if (OGSS_Math::GCD (i, _numDevices) != 1)
			continue;
		if (computeMatrixRank (i) == 2) {
			_lambda = i;
			found = true;
			break;
		}	
	}

	if (! found) {
		for (int i = 1; i < _half_up (_numDevices); ++i) {
			if (OGSS_Math::GCD (i, _numDevices) == 1)
				continue;
			if (computeMatrixRank (i) == 2) {
				_lambda = i;
				found = true;
				break;
			}
		}
	}

	if (! found) {
		for (int i = 1; i < _half_up (_numDevices); ++i) {
			if (OGSS_Math::GCD (i, _numDevices) != 1)
				continue;
			if (computeMatrixRank (i) == 1) {
				_lambda = i;
				found = true;
				break;
			}	
		}
	}

	if (! found) {
		for (int i = 1; i < _half_up (_numDevices); ++i) {
			if (OGSS_Math::GCD (i, _numDevices) == 1)
				continue;
			if (computeMatrixRank (i) == 1) {
				_lambda = i;
				found = true;
				break;
			}
		}
	}

	LOG(INFO) << "SD2S scheme lambda: " << _lambda;
}

OGSS_Ushort
SD2SScheme::computeMatrixRank (
	OGSS_Ulong				lambda) {
	OGSS_Ushort				physicalDisk;
	OGSS_Ushort				logical_Volume;
	OGSS_Bool				optimizable;
	vector <pair <OGSS_Ushort, OGSS_Ushort> >
							degreeMatrix (_numDevices * _numDevices,
								make_pair (0, 0) );
	vector <int>			deviceLoad (_numDevices, 0);
	vector <set <OGSS_Ushort> >	originSets (_numDevices);

	for (auto i = originSets.begin (); i != originSets.end (); ++i)
		i->clear ();

	for (auto i = 0; i < _numDataDevices; ++i) {
		for (auto j = 0; j < _numDataDevices; ++j) {
			physicalDisk = (i + j * lambda) % _numDevices;
			deviceLoad [physicalDisk] ++;

			if (i < _numDataDevices) {
				originSets [physicalDisk] .insert (
					j + _belongs [i] * _numDataDevices);

				logical_Volume = _belongs [i];

				for (auto k = 0; k < _numDataDevices; ++k) {
					if (physicalDisk != (k + j * lambda) %_numDevices
						&& _belongs [k] == logical_Volume) {
						degreeMatrix [physicalDisk * _numDevices
							+ ( (k + j * lambda) % _numDevices) ] .first ++;
					}
				}
			}
		}
	}

	for (auto i = 0; i < _numDevices; ++i) {
		for (auto j = i + 1; j < _numDevices; ++j) {
			degreeMatrix [i * _numDevices + j] .first = min (deviceLoad [i],
				deviceLoad [j])	- degreeMatrix [i * _numDevices + j] .first;
			degreeMatrix [i * _numDevices + j] .second
				= abs (deviceLoad [i] - deviceLoad [j]);
			degreeMatrix [j * _numDevices + i]
				= degreeMatrix [i * _numDevices + j];
		}
	}

	optimizable = true;
	for (auto i = 0; i < _numDevices; ++i) {
		for (auto j = i + 1; j < _numDevices; ++j) {
			if (degreeMatrix [i * _numDevices + j] .first == 0)
			{
				if (degreeMatrix [i * _numDevices + j] .second == 0)
				{
					DLOG(INFO) << "Rank for computed lambda (" << lambda << ") is 0";
					return 0;
				}
				optimizable = false;
			}
		}
	}

	if (!optimizable)
	{
		DLOG(INFO) << "Rank for computed lambda (" << lambda << ") is 1";
		return 1;
	}

	DLOG(INFO) << "Rank for computed lambda (" << lambda << ") is 2";
	return 2;
}
