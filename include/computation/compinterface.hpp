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

//! \file	compinterface.hpp
//! \brief	Definition of the interface computation model.

#ifndef _OGSS_COMPINTERFACE_HPP_
#define _OGSS_COMPINTERFACE_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <vector>

#include "computation/computationmodel.hpp"

#include "structure/hardware.hpp"

//! \brief	Interface computation model.
class CompInterface: public ComputationModel {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	params				Hardware parameters.
//! \param	tiers				Tiers.
//! \param	vols				Volumes.
//! \param	devs				Devices.
//! \param	intfs				Interfaces.
	CompInterface (
		HardwareParameters		& params,
		std::vector <Tier>		& tiers,
		std::vector <Volume>	& vols,
		std::vector <Device>	& devs,
		std::vector <Interface>	& intfs);

//! \brief	Destructor.
	~CompInterface ();

//! \brief	Computation function for the request transfer time.
//! \param	req					Request.
	void compute (
		Request					& req);

protected:

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	HardwareParameters			& _hardParam;		//!< Hardware parameters.
	std::vector <Tier>			& _tiers;			//!< Tiers.
	std::vector <Volume>		& _volumes;			//!< Volumes.
	std::vector <Device>		& _devices;			//!< Devices.
	std::vector <Interface>		& _interfaces;		//!< Interfaces.

	OGSS_Ulong					_codSize;			//!< Size of the request code.
	OGSS_Ulong					_ackSize;			//!< Size of the request acknowledgment.
};

#endif
