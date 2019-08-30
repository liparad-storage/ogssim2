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

//! \file	request.hpp
//! \brief	Define the request structure, used by all the modules of OGSSim.

#ifndef _OGSS_REQUEST_HPP_
#define _OGSS_REQUEST_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/types.hpp"

/*----------------------------------------------------------------------------*/
/* STRUCTURE -----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Structure which stores a request index. This index is composed of 3
//!			fields, one for each decomposition step. The main part refers to the
//!			logical request index (order in the trace file). The major part
//!			refers to the tier decomposition. The minor part refers to the volume
//!			decomposition.
struct OGSS_RequestIdx {
	OGSS_Ulong							_main;		//!< Main part.
	OGSS_Ulong							_major;		//!< Major part.
	OGSS_Ulong							_minor;		//!< Minor part.

//! \brief	Constructor.
//! \param	main				Main part.
//! \param	major				Major part.
//! \param	minor				Minor part.
	OGSS_RequestIdx (
		const OGSS_Ulong				main = OGSS_ULONG_MAX,
		const OGSS_Ulong				major = OGSS_ULONG_MAX,
		const OGSS_Ulong				minor = OGSS_ULONG_MAX)
		{ _main = main; _major = major; _minor = minor; }

//! \brief	Check if the request is a logical one.
//! \return						TRUE if logical.
	inline OGSS_Bool isLogical () const
		{ return ! _major; }

//! \brief	Check if the request is an intermediate one.
//! \return						TRUE if intermediate.
	inline OGSS_Bool isIntermediate () const
		{ return _major && ! _minor; }

//! \brief	Check if the request is a physical one.
//! \return						TRUE if physical.
	inline OGSS_Bool isPhysical () const
		{ return _minor; }
};

//! \brief	Definition of request structure. Some attributes are parameters,
//! 		given by the workload file and others are values that are modified
//!			during the simulation.
struct Request {
//! \brief	Constructor. Initializes the parameters.
//! \param	date				Arrival date.
//! \param	size				Request size.
//! \param	address				Request logical (system) address.
//! \param	type				Request type.
//! \param	system				TRUE if system request, FALSE if user request.
	Request (
		const OGSS_Real			date = .0,
		const OGSS_Ulong		size = 0,
		const OGSS_Ulong		address = 0,
		const OGSS_RequestType	type = RQT_READ,
		const OGSS_Bool			system = false);

//! \brief	Constructor. Using the tuple used by the extractor.
//! \param	req					Request parameters.
	Request (
		const std::tuple <
			OGSS_Real,
			OGSS_RequestType,
			OGSS_Ulong,
			OGSS_Ulong,
			OGSS_Ushort,
			OGSS_Ushort>		req);

//! \brief	Request printer.
//! \return						Request information.
	OGSS_String print ();

	OGSS_Real					_date;				//!< Arrival date.
	OGSS_RequestType			_type;				//!< Request type.
	OGSS_Ulong					_address;			//!< System address.
	OGSS_Ulong					_size;				//!< Request size.

	OGSS_Bool					_system;			//!< TRUE if system
													//!< request.
	
	OGSS_Ulong					_mainIdx;			//!< Main index.
	OGSS_Ulong					_majrIdx;			//!< Major index.
	OGSS_Ulong					_minrIdx;			//!< Minor index.
	OGSS_RequestIdx				_index;				//!< Request index.

	OGSS_Ulong					_numChild;			//!< Number of child
													//!< requests.
	OGSS_Ulong					_numPrioChild;		//!< Number of prior
													//!< child requests.
	OGSS_Ulong					_numLink;			//!< Value used with
													//!< reconstruction
													//!< requests.

	OGSS_Ushort					_idxVolume;			//!< Volume index.
	OGSS_Ulong					_volumeAddress;		//!< Volume address.
	OGSS_Ulong					_idxDevice;			//!< Device index.
	OGSS_Ulong					_nativeIdxDevice;	//!< Intended device index (without event).
	OGSS_Ulong					_deviceAddress;		//!< Device address.
	OGSS_Ulong					_nativeDeviceAddress;	//!< Intended device address (without event).

	OGSS_Real					_serviceTime;		//!< Service time.
	OGSS_Real					_transferTimeA1;	//!< Transfer time from H.
	OGSS_Real					_transferTimeA2;	//!< Transfer time from T.
	OGSS_Real					_transferTimeA3;	//!< Transfer time from V.
	OGSS_Real					_transferTimeB3;	//!< Transfer time to Vol.
	OGSS_Real					_transferTimeB2;	//!< Transfer time to Tier.
	OGSS_Real					_transferTimeB1;	//!< Transfer time to Host.
	OGSS_Real					_waitingTime;		//!< Waiting time.
	OGSS_Real					_responseTime;		//!< Response time.

	OGSS_Bool					_failed;			//!< TRUE if the request
													//!< could not succeed.
	OGSS_Bool					_multiple;			//!< TRUE if the request
													//!< has multiple behavior.
	OGSS_RequestOperation		_operation;			//!< Requested operation
													//!< following an event.
	OGSS_Bool					_prio {false};		//!< TRUE if happened before others.
};

//! \brief	Structure for an on-the-fly request, used during the reconstruction.
struct OGSS_OTFRequest {
	OGSS_Ulong							_nbRequests;		//!< Number of requests to generate.
	OGSS_Ulong							_requestSize;		//!< Request size.
	OGSS_Ulong							_lastDevAddress;	//!< Address of the last generated request.
	OGSS_Real							_currentClock;		//!< Current simulation clock.
	Request								_event;				//!< Event which will generate the requests.
};

//! \brief	Comparator for request index. Comparing in order the main, the major and
//!			finally the minor index.
//! \param	lhs							Left parameter.
//! \param	rhs							Right parameter.
//! \return								TRUE if lhs < rhs.
OGSS_Bool operator< (
	const OGSS_RequestIdx				lhs,
	const OGSS_RequestIdx				rhs);

//! \brief	Comparator for request index.
//! \param	lhs							Left parameter.
//! \param	rhs							Right parameter.
//! \return								TRUE if lhs = rhs.
OGSS_Bool operator== (
	const OGSS_RequestIdx				lhs,
	const OGSS_RequestIdx				rhs);

//! \brief	Comparator for request index.
//! \param	lhs							Left parameter.
//! \param	rhs							Right parameter.
//! \return								TRUE if lhs != rhs.
OGSS_Bool operator!= (
	const OGSS_RequestIdx				lhs,
	const OGSS_RequestIdx				rhs);

//! \brief	Request index printer.
//! \param	stream						Stream where the index will be printed.
//! \param	idx							Index to print.
//! \return 							Stream where the index is printed.
std::ostream & operator<< (
	std::ostream &						stream,
	const OGSS_RequestIdx				& idx);

const OGSS_RequestIdx OGSS_REQUESTIDX_UND {OGSS_ULONG_MAX, OGSS_ULONG_MAX, OGSS_ULONG_MAX};

#endif
