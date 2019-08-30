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

//! \file	compssd.hpp
//! \brief	Definition of the SSD computation model.

#ifndef _OGSS_COMPSSD_HPP_
#define _OGSS_COMPSSD_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "computation/computationmodel.hpp"

#include "structure/hardware.hpp"

//! \brief	SSD computation model.
class CompSSD: public ComputationModel {
public:

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Constructor.
//! \param	devs				Devices using the computation model.
	CompSSD (
		std::vector <Device>	& devs);

//! \brief	Destructor.
	~CompSSD ();

//! \brief	Computation function for the request service time.
//! \param	req					Request.
	void compute (
		Request					& req);

protected:

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Computation of the read service time.
//! \param	req					Request.
//! \param	dev					Targeted device.
//! \return						Computed time.
	OGSS_Real readServTime (
		Request					& req,
		Device					& dev);

//! \brief	Computation of the write service time.
//! \param	req					Request.
//! \param	dev					Targeted device.
//! \return						Computed time.
	OGSS_Real writeServTime (
		Request					& req,
		Device					& dev);

//! \brief	Computation of the erase service time.
//! \param	req					Request.
//! \param	dev					Targeted device.
//! \return						Computed time.
	OGSS_Real eraseServTime (
		Request					& req,
		Device					& dev);

/*----------------------------------------------------------------------------*/
/* ATTRIBUTES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

	std::vector <Device>		& _devices;			//!< Device parameters.
	std::map <OGSS_Ulong, std::vector <OGSS_Ulong>>
								_lastPageSeen;		//!< Last page seen on each device.
};

#endif
