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

//! \file	syncsingledisk.cpp
//! \brief	Definition of the single disk model.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "synchronization/syncsingledisk.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>

#include "structure/requeststat.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

using namespace std;

/*----------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

OGSS_Bool requestSort (
	Request					i,
	Request					j) {
	return (i._date < j._date)
		|| (i._date == j._date && i._mainIdx < j._mainIdx)
		|| (i._date == j._date && i._mainIdx == j._mainIdx
			&& i._majrIdx < j._majrIdx)
		|| (i._date == j._date && i._mainIdx == j._mainIdx
			&& i._majrIdx == j._majrIdx && i._minrIdx < j._minrIdx);
}

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

SyncSingleDisk::SyncSingleDisk (
	shared_ptr <CommunicationInterface>	ci,
	HardwareParameters		& params,
	OGSS_DataUnit				globalDU):
	SynchronizationModel (ci),
	_hardParam (params),
	 _globalDU (globalDU) {  }

SyncSingleDisk::~SyncSingleDisk () {
	_reqs.clear ();
}

void
SyncSingleDisk::addEntry (
	Request 				&req) {
	if (req._minrIdx == 0)
		return;

	_reqs.push_back (req);
}

RequestStat
SyncSingleDisk::prepareStat (
	Request					& req) {
	RequestStat				stat;

	stat._mainIdx = req._mainIdx;
	stat._majrIdx = req._majrIdx;
	stat._minrIdx = req._minrIdx;

	if (req._type & RQT_WRITE)
		stat._type = RQT_WRITE;
	else
		stat._type = RQT_READ;

	stat._size = req._size;

	stat._idxTier = 0;
	stat._idxVolume = 0;
	stat._idxDevice = 0;
	stat._idxBus = 2;

	stat._arrivalDate = req._date;

	stat._serviceTime = req._serviceTime;
	stat._transferTime = req._transferTimeA3 + req._transferTimeB3;
	stat._waitingTime = req._waitingTime;

	return stat;
}

void
SyncSingleDisk::process () {
	double					busClock = .0;
	double					devClock = .0;

	sort (_reqs.begin (), _reqs.end (), requestSort);

	for (auto & e: _reqs) {
		busClock = .0;
		e._transferTimeA1 = e._transferTimeA2
			= e._transferTimeB2 = e._transferTimeB1 = .0;

		busClock = e._date;
		busClock += e._transferTimeA3;

		devClock = max (devClock, busClock);
		e._waitingTime += devClock - busClock;
		devClock += e._serviceTime;

		busClock = devClock;
		busClock += e._transferTimeB3;

		sendStat (prepareStat (e) );
	}
}

void
SyncSingleDisk::createOutputFile (
	const OGSS_String		outputFile) {
	ofstream				output (outputFile);

	DLOG(INFO) << "[SC] To write in '" << outputFile << "'!";

	output << left << setfill (' ') << setw (6) << "#Req" << " | "
		 << left << setfill (' ') << setw (17) << "ToDevice" << " | "
		 << left << setfill (' ') << setw (17) << "Service" << " | "
		 << left << setfill (' ') << setw (17) << "FromDevice" << " | "
		 << left << setfill (' ') << setw (17) << "Response" << " | "
		 << endl;

	for (auto & e: _reqs) {
		output << left << setfill (' ') << setw (6) << e._mainIdx << " | "
			<< left << setfill (' ') << setw (8) << e._transferTimeA3 << " | "
			<< left << setfill (' ') << setw (8) << e._serviceTime << " | "
			<< left << setfill (' ') << setw (8) << e._transferTimeB3 << " | "
			<< left << setfill (' ') << setw (8)
			<< e._transferTimeA3 + e._serviceTime + e._transferTimeB3 + e._waitingTime
			<< endl;
	}
}

void
SyncSingleDisk::createResumeFile (
	const OGSS_String		resumeFile) {
	ofstream				output (resumeFile);
	

	// Structure to save stats from each devices
	struct device_stats{
		OGSS_Real workTime = 0;
		OGSS_Real idleTime = 0;
		OGSS_Real waitTime = 0;
		OGSS_Real totalTime = 0;
		OGSS_Ulong nbRequests = 0;
		OGSS_Ulong nbReadRequests = 0;
		OGSS_Real averageRequestSize = 0; 
	};

	// device_stats device[_hardParam._numDevices];
	device_stats volume;

	for (auto & e: _reqs) {
		volume.workTime += e._serviceTime;
		volume.nbRequests ++;
		volume.waitTime += e._waitingTime;
		volume.totalTime = e._date +  e._waitingTime + e._serviceTime + e._transferTimeA3 + e._transferTimeB3;
		if(e._type == RQT_READ){
			volume.nbReadRequests ++;
		}
		volume.averageRequestSize += e._size;
	}
	volume.idleTime =  volume.totalTime - volume.workTime; //Time during the device is not treating data


	output << "Global Data Unit:" << endl 
	<< "Time unit (TU): " << _globalDU._time << "s" << endl 
	<< "Memory unit (MU): " << _globalDU._memory << " bytes (" << _globalDU._memory*8 << " bits)" << endl << endl;

	output << setw(50) << right << "Local Unit" 
	<< setw(18) << right << "SI" << endl << endl;
	//Print the gathered informations to the output
	output << setw(35) << left << "Total time: " 						
	<< setw(15) << right << volume.totalTime << " TU" 
	<< setw(15) << right << volume.totalTime * _globalDU._time << " s" << endl;

	output << setw(35) << left << "Total work time: "  					
	<< setw(15) << right << volume.workTime << " TU" 
	<< setw(15) << right << volume.workTime * _globalDU._time << " s" << endl;

	output << setw(35) << left << "Work percentage: " 					
	<< setw(15) << right << (volume.workTime/_hardParam._numDevices) / (volume.totalTime) * 100 << " %" << endl;

	output << setw(35) << left << "Total idle time: " 					
	<< setw(15) << right << volume.idleTime << " TU"
	<< setw(15) << right << volume.idleTime * _globalDU._time << " s" << endl;

	output << setw(35) << left << "Total wait time: " 					
	<< setw(15) << right << volume.waitTime << " TU"
	<< setw(15) << right << volume.waitTime * _globalDU._time << " s" << endl;

	output << setw(35) << left << "Average Wait time per request: " 	
	<< setw(15) << right << volume.waitTime / volume.nbRequests << " TU"
	<< setw(15) << right << volume.waitTime / volume.nbRequests * _globalDU._time << " s" << endl;

	output << setw(35) << left << "Number of requests: " 				
	<< setw(15) << right << volume.nbRequests << endl;

	output << setw(35) << left << "Read requests percentage: " 			
	<< setw(15) << right << (OGSS_Real)volume.nbReadRequests * 100 / volume.nbRequests  << " %" << endl;
	
	output << setw(35) << left << "Average request size: " 			
	<< setw(15) << right << floor(volume.averageRequestSize / volume.nbRequests + 0.5) << " MU"
	<< setw(15) << right << floor(volume.averageRequestSize / volume.nbRequests + 0.5) * _globalDU._memory << " bytes" << endl;

	output << setw(35) << left << "Bandwidth: " 						
	<< setw(15) << right << volume.nbRequests  / (volume.totalTime * _globalDU._time) << " IOps" << endl;

	output << endl;
}

