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

//! \file	communicationmanager.cpp
//! \brief	Interface for the communication manager. The manager is the main
//! 		module of the communication model. Its goal is to maintain the list
//! 		of existing modules, and share their communication ports if
//! 		requested.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "communication/communicationmanager.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

/*----------------------------------------------------------------------------*/
/* MEMBER FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
CommunicationManager::listen () {
	LOG (INFO) << "[CM] Communication manager will now listen..";
	while (provide () );
}
