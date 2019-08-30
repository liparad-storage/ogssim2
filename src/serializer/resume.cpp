/*
 * Copyright UVSQ - CEA/DAM/DIF (2017-2018)
 * Contributors:  Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *                Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *                Maxence JOULIN
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

//! \file	resume.hpp
//! \brief	Generation of the resume file once the simulation is complete.
//!			The resume file consists in global to local results on the system
//!			(whole system, tiers, volumes and devices).

#include "serializer/resume.hpp"

#include <glog/logging.h>

using namespace std;

Resume::Resume (OGSS_Ulong nbRequests, OGSS_DataUnit globalDU, OGSS_Ulong nbTiers, OGSS_Ulong nbVols, OGSS_Ulong nbDevs){
    _nbRequests = nbRequests;
    _globalDU = globalDU;
    _nbTiers = nbTiers;
    _nbVols = nbVols;
    _nbDevs = nbDevs;

    //Init Stat vector
	for(auto i = 0; i<_nbDevs; i++){
		devices_stats.push_back(Stat());
		devices_stats[i].id = i;
	}

	//Init volume_stats vector
	for(auto i = 0; i <_nbVols; i++){
		volumes_stats.push_back(Stat());
		volumes_stats[i].id = i;
	}
}

void
Resume::updateStats(RequestStat stats){
    if(stats._minrIdx){

    	if (devices_stats [stats._idxDevice] .parent == OGSS_USHORT_MAX)
    		devices_stats [stats._idxDevice] .parent = stats._idxVolume;
    	else
    		DLOG_IF (ERROR, devices_stats [stats._idxDevice] .parent != stats._idxVolume) << "Unknwon behavior (one device on two volumes)";

    	devices_stats[stats._idxDevice].nbRequests ++;

    	if (!stats._failed) {
        	devices_stats[stats._idxDevice].workTime += stats._serviceTime;
	        devices_stats[stats._idxDevice].waitTime += stats._waitingTime;
        
	        devices_stats[stats._idxDevice].averageRequestSize += stats._size;

	        if(stats._type == RQT_READ)
    	        devices_stats[stats._idxDevice].nbReadRequests ++;

	        devices_stats[stats._idxDevice].totalTime = max (stats._arrivalDate + stats._serviceTime + stats._waitingTime + stats._transferTime,
    	    	devices_stats [stats._idxDevice] .totalTime);

	        if (stats._arrivalDate < firstEventDate) {
    	    	waitTimeBeforeFirstEvent += stats._waitingTime;
        		++ counterBeforeFirstEvent;
	        } else {
    	    	waitTimeAfterFirstEvent += stats._waitingTime;
        		++ counterAfterFirstEvent;
	        }
	    } else
	    	++ devices_stats [stats._idxDevice] .nbFailedRequests;

        nbRequestsDone ++;

        OGSS_Ulong printStep {max (tier.nbRequests / 100, (OGSS_Ulong) 1) };
    } else if (stats._majrIdx) {
    	volumes_stats[stats._idxVolume].nbRequests ++;
    	volumes_stats[stats._idxVolume].averageRequestSize += stats._size;
    	if (stats._type == RQT_READ)
    		volumes_stats[stats._idxVolume].nbReadRequests ++;
    	volumes_stats [stats._idxVolume] .totalTime = max (stats._arrivalDate + stats._serviceTime + stats._waitingTime + stats._transferTime,
    		volumes_stats [stats._idxDevice] .totalTime);
    } else {
    	tier.nbRequests ++;
    	tier.averageRequestSize += stats._size;
    	if (stats._type == RQT_READ)
    		tier.nbReadRequests ++;
    	tier.totalTime = max (tier.totalTime, stats._arrivalDate + stats._serviceTime + stats._waitingTime + stats._transferTime);
    }
}

void
Resume::updateEvent (
	const Request						& event,
	OGSS_Real							duration)
	{ _eventRes [event._majrIdx] = duration - event._date; }

void
Resume::save (OGSS_String resumeFile) {
	if (! resumeFile.compare ("") ){
		cout << "-\tResume file not requested by the configuration" << endl;
		return;
	} 

	ofstream				output (resumeFile);

	OGSS_Real totm = .0;
	for (auto & d: devices_stats)		totm = max (totm, d.totalTime);
	for (auto & v: volumes_stats)		totm = max (totm, v.totalTime);
										totm = max (totm, tier.totalTime);

	//Write all Data Units of the simulator
	output << "Global Data Unit:" << endl 
	<< "Time unit (TU): " << _globalDU._time << "s" << endl 
	<< "Memory unit (MU): " << _globalDU._memory << " bytes (" << _globalDU._memory*8 << " bits)" << endl << endl;

	if (_eventRes.size () ) {
		output << "Event results: " << endl;
		output << "\t\tFirst event happening at " << firstEventDate << endl;
		output << "\t\tPart of blocks reconstructed during the user requests process: " << partReconstructedBlocks << "%" << endl;
		for (auto & e: _eventRes)
			output << "\t\tEvent #" << e.first << " resolves in " << e.second << " TUs" << endl;
		if (counterBeforeFirstEvent > 0)
			output << "\t\tMean waiting time before the first event: " << waitTimeBeforeFirstEvent / counterBeforeFirstEvent << endl;
		if (counterAfterFirstEvent > 0)
			output << "\t\tMean waiting time after the first event: " << waitTimeAfterFirstEvent / counterAfterFirstEvent << endl;
		output << endl;

	}

	output << setw(50) << right << "Local Unit" 
	<< setw(18) << right << "SI" << endl << endl;


	//Write all stats gathered for the Tier level
	output << endl << "=== Tier ===" << endl;

	tier.totalTime = totm;

	output << setw(35) << left << "Number of requests: " 				
	<< setw(15) << right << tier.nbRequests << endl;

	output << setw(35) << left << "Read requests percentage: " 			
	<< setw(15) << right << (OGSS_Real)tier.nbReadRequests * 100 / tier.nbRequests  << " %" << endl;
	
	output << setw(35) << left << "Average request size: " 			
	<< setw(15) << right << floor(tier.averageRequestSize / tier.nbRequests + 0.5) << " MU"
	<< setw(15) << right << floor(tier.averageRequestSize / tier.nbRequests + 0.5) * _globalDU._memory << " bytes" << endl; // Multiply the size by the Memory unit to get bytes

	output << setw(35) << left << "Bandwidth: " 						
	<< setw(15) << right << tier.nbRequests  / (tier.totalTime * _globalDU._time) << " IOps" << endl; // Multiply the totalTime by the Time Unit to get time in seconds.

	output << endl;

	output << "=========================" << endl << endl;

	//Write all stats gathered for the volume level
	output << endl << "=== Volumes ===" << endl;

	for(auto i = 0; i<_nbVols; i++){
		volumes_stats[i].totalTime = totm;

		if (! volumes_stats [i] .nbRequests) continue;

		output << "Volume ID: " << volumes_stats[i].id << endl;
		output << setw(35) << left << "Number of requests: " 				
		<< setw(15) << right << volumes_stats[i].nbRequests << endl;

		output << setw(35) << left << "Read requests percentage: " 			
		<< setw(15) << right << (OGSS_Real)volumes_stats[i].nbReadRequests * 100 / volumes_stats[i].nbRequests  << " %" << endl;
	
		output << setw(35) << left << "Average request size: " 			
		<< setw(15) << right << floor(volumes_stats[i].averageRequestSize / volumes_stats[i].nbRequests + 0.5) << " MU"
		<< setw(15) << right << floor(volumes_stats[i].averageRequestSize / volumes_stats[i].nbRequests + 0.5) * _globalDU._memory << " bytes" << endl;

		output << setw(35) << left << "Bandwidth: " 						
		<< setw(15) << right << volumes_stats[i].nbRequests  / (volumes_stats[i].totalTime * _globalDU._time) << " IOps" << endl;

		output << endl;
	}


	output << "=========================" << endl << endl;

	//Write all stats gathered for the deivce level
	for(auto i = 0; i<_nbDevs; i++){
		devices_stats [i] .totalTime = totm;
		devices_stats[i].idleTime =  devices_stats[i].totalTime - devices_stats[i].workTime;

		if (! devices_stats [i] .nbRequests) continue;

		output << "Device ID: " << devices_stats[i].id << " on volume #" << devices_stats [i] .parent << endl;

		//Print the gathered informations to the output
		output << setw(35) << left << "Total time: " 						
		<< setw(15) << right <<  devices_stats[i].totalTime << " TU" 
		<< setw(15) << right <<  devices_stats[i].totalTime * _globalDU._time << " s" << endl;

		output << setw(35) << left << "Total work time: "  					
		<< setw(15) << right <<  devices_stats[i].workTime << " TU" 
		<< setw(15) << right <<  devices_stats[i].workTime * _globalDU._time << " s" << endl;

		output << setw(35) << left << "Work percentage: " 					
		<< setw(15) << right << ( devices_stats[i].workTime) / ( devices_stats[i].totalTime) * 100 << " %" << endl;

		output << setw(35) << left << "Total idle time: " 					
		<< setw(15) << right <<  devices_stats[i].idleTime << " TU"
		<< setw(15) << right <<  devices_stats[i].idleTime * _globalDU._time << " s" << endl;

		output << setw(35) << left << "Total wait time: " 					
		<< setw(15) << right <<  devices_stats[i].waitTime << " TU"
		<< setw(15) << right <<  devices_stats[i].waitTime * _globalDU._time << " s" << endl;

		output << setw(35) << left << "Average Wait time per phys. req: " 	
		<< setw(15) << right <<  devices_stats[i].waitTime /  devices_stats[i].nbRequests << " TU"
		<< setw(15) << right <<  devices_stats[i].waitTime /  devices_stats[i].nbRequests * _globalDU._time << " s" << endl;

		output << setw(35) << left << "Number of phys. requests: " 				
		<< setw(15) << right <<  devices_stats[i].nbRequests << endl;

		output << setw(35) << left << "Read requests percentage: " 			
		<< setw(15) << right << (OGSS_Real) devices_stats[i].nbReadRequests * 100 /  devices_stats[i].nbRequests  << " %" << endl;
	
		output << setw(35) << left << "Failed requests percentage: "
		<< setw(15) << right << (OGSS_Real) devices_stats[i].nbFailedRequests * 100 / devices_stats [i].nbRequests << " %" << endl;

		output << setw(35) << left << "Average phys. request size: " 			
		<< setw(15) << right << floor( devices_stats[i].averageRequestSize /  devices_stats[i].nbRequests + 0.5) << " MU"
		<< setw(15) << right << floor( devices_stats[i].averageRequestSize /  devices_stats[i].nbRequests + 0.5) * _globalDU._memory << " bytes" << endl;

		output << setw(35) << left << "Bandwidth: " 						
		<< setw(15) << right <<  devices_stats[i].nbRequests  / ( devices_stats[i].totalTime * _globalDU._time) << " IOps" << endl;

		output << endl;

	}

	cout << "-\t\tResume written" << endl;
}
