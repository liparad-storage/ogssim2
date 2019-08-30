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

//! \file	evaluation.cpp
//! \brief	Definition of the evaluation module class.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "module/evaluation.hpp"

#include "graph/gendeviceprofile.hpp"
#include "graph/gendevicethroughput.hpp"
#include "graph/genresponsetime.hpp"
#include "graph/genreqcompletion.hpp"
#include "graph/genwaitingtime.hpp"

#include "serializer/mplibserializer.hpp"

#include <iostream>

#include "parser/xmlparser.hpp"

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

Evaluation::Evaluation (
	const OGSS_String		& configurationFile):
	Module (configurationFile, make_pair (MTP_EVALUATION, 0) ) {
#if USE_PYTHON_BINDING
	MPLibSerializer::initPythonWrapper ();
#endif

	_globalDU = XMLParser::getDataUnit (configurationFile, PTP_GLOBAL);
	_graphs = XMLParser::getGraphs (configurationFile);
}

Evaluation::~Evaluation () {
#if USE_PYTHON_BINDING
	MPLibSerializer::finiPythonWrapper ();
#endif
}

void
Evaluation::processExtraction () {
	receiveData ();

	for (auto & elt: _graphs) {
		DLOG(INFO) << "[EV] New graph: " << GraphTypeNameMap.at (elt._type) << " - "
			<< elt._filename << " - ";

#if ! USE_PYTHON_BINDING
		if (elt._format == GFM_PNG || elt._format == GFM_PDF) {
			LOG(WARNING) << "Python binding is not available, your graph '" << elt._filename
				<< " cannot be produced";
			continue;
		}
#endif

		switch (elt._type) {
			case GPT_REQRESP: _ggen.push_back (make_unique <GenResponseTime> (elt, _globalDU) ); break;
			case GPT_DEVPROF: _ggen.push_back (make_unique <GenDeviceProfile> (elt, _globalDU) ); break;
			case GPT_DEVTHRP: _ggen.push_back (make_unique <GenDeviceThroughput> (elt, _globalDU) ); break;
			case GPT_WAITTM:  _ggen.push_back (make_unique <GenWaitTime> (elt, _globalDU) ); break;
			case GPT_REQCPLT: _ggen.push_back (make_unique <GenReqCompletion> (elt, _globalDU) ); break;
			default:
			LOG (WARNING) << "The type " << GraphTypeNameMap.at (elt._type)
				<< " is not referenced";
		}
	}
}

void
Evaluation::processDecomposition () {  }

void
Evaluation::processSynchronization () {
	OGSS_Bool				unfinished = true;
	OGSS_Bool				ack;
	RequestStat				stat;

	while (unfinished) {
		stat = receiveRequestStat ();

		if (stat._type == RQT_END) {
			unfinished = false;
			continue;
		}

		for (auto & elt: _ggen)
			elt->updateData (stat);
	}

	_ci->send (std::make_pair (MTP_SYNCHRONIZATION, 0), &ack, sizeof (ack) );

	for (auto & elt: _ggen) {
		elt->makeGraph ();
	}
}

/*----------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

RequestStat
Evaluation::receiveRequestStat () { 
	RequestStat				stat;
	void					* arg;

	_ci->receive (arg);
	stat = * static_cast <RequestStat *> (arg); free (arg);

	return stat;
}
