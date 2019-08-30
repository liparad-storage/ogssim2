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

//! \file	math.hpp
//! \brief	Math function implementation.

#ifndef _OGSS_MATH_HPP_
#define _OGSS_MATH_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/types.hpp"

namespace OGSS_Math {
/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
//! \brief	Primality test function.
//! \param	a					Integer number to test.
//! \return						TRUE if primal, FALSE if not.
	OGSS_Bool isPrime (
		OGSS_Ulong				a);

//! \brief	Computes the Greatest Common Divisor (GCD) of two integers.
//! \param	a					First integer.
//!	\param	b					Second integer.
//! \return						GCD of a and b.
	OGSS_Ulong GCD (
		OGSS_Ulong				a,
		OGSS_Ulong				b);	
}

#endif
