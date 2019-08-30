/*
 * Copyright UVSQ - CEA/DAM/DIF (2017-2018)
 * Contributors:  Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *                Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published per the Free Software Foundation; either
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

//! \file	types.hpp
//! \brief	Definition of all types and basic structures used in OGSSim. It
//! 		also defines constant containers.

#ifndef _OGSG_TYPES_HPP_
#define _OGSG_TYPES_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <cstdint>
#include <iostream>
#include <limits>
#include <map>
#include <vector>

#include "config.h"

/*----------------------------------------------------------------------------*/
/* BASIC TYPES ---------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

typedef	bool					OGSS_Bool;			//!< Boolean.
typedef	int16_t					OGSS_Short;			//!< Short integer.
typedef	int64_t					OGSS_Long;			//!< Long integer.
typedef	uint16_t				OGSS_Ushort;		//!< Short unsigned.
typedef	uint64_t				OGSS_Ulong;			//!< Long unsigned.
typedef	double					OGSS_Real;			//!< Long real.
typedef	std::string				OGSS_String;		//!< String.

/*----------------------------------------------------------------------------*/
/* BASIC STRUCTURES ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Module type.
enum OGSS_ModuleType {
	MTP_COMMUNICATION,
	MTP_COMMUNICATIONZMQ,
	MTP_COMPUTATIONBUSADV,
	MTP_DECRAIDCTRL,
	MTP_DEVICE,
	MTP_EVALUATION,
	MTP_EVENT,
	MTP_EXECUTION,
	MTP_GENLATENCY,
	MTP_GENWAITTIME,
	MTP_HARDWARE,
	MTP_LTXSERIALIZER,
	MTP_MPLIBSERIALIZER,
	MTP_PREPROCESSING,
	MTP_RAWPARSER,
	MTP_SSDALLOCDEFAULT,
	MTP_SYNCDEFV2,
	MTP_SYNCDEFV3,
	MTP_SYNCHRONIZATION,
	MTP_SYNCPAR,
	MTP_SYNCQUEUE,
	MTP_VOLJBOD,
	MTP_VOLRAID01,
	MTP_VOLRAID1,
	MTP_VOLRAIDNPDATDEC,
	MTP_VOLRAIDNPNODEC,
	MTP_VOLRAIDNPPARDEC,
	MTP_VOLUME,
	MTP_WORKLOAD,
	MTP_TOTAL
};

typedef std::pair <OGSS_ModuleType, OGSS_Ushort>	OGSS_Interlocutor;
	//!< An interlocutor is the combination of a module type and an id.

//! \brief	File type, which can be an input or an output file.
enum OGSS_FileType {
	FTP_CONFIGURATION,
	FTP_HARDWARE,
	FTP_LOGGING,
	FTP_RESULT,
	FTP_RESUME,
	FTP_SUBRESULT,
	FTP_UNITARYTEST,
	FTP_WORKLOAD,
	FTP_TOTAL
};

//! \brief	Communication type.
enum OGSS_CommType {
	CTP_ZMQ,
	CTP_IZMQ,
	CTP_MPI,
	CTP_TOTAL
};

//! \brief	Parameter type, used for XML file parsing.
enum OGSS_ParamType {
	PTP_ADDRESS, PTP_AGRSK, PTP_AGWSK, PTP_ARG1, PTP_ARG2, PTP_ARG3,
	PTP_BANDWIDTH, PTP_BLKDIE, PTP_BUFSIZE, PTP_BUS, PTP_BUSES, PTP_BYTESPERCOL,
	PTP_COLS, PTP_COMM, PTP_COMPUTATION, PTP_CONFIG, PTP_CONTROLLER,
	PTP_DATAUNIT, PTP_DATAUNITS, PTP_DATE, PTP_DECL, PTP_DEFRAGMENTATION,
	PTP_DEVICE, PTP_DUNAME,
	PTP_ENTRY, PTP_ERASE, PTP_EVENT,
	PTP_FORMAT,
	PTP_GARBAGECOLLECTION, PTP_GEOMETRY, PTP_GLOBAL, PTP_GRAPH,
	PTP_HDD,
	PTP_INPUT, PTP_INTERFACE, PTP_IOPS,
	PTP_MEMORY, PTP_MNRSK, PTP_MNWSK, PTP_MTTF, PTP_MXRSK, PTP_MXWSK,
	PTP_NAME, PTP_NBCHIPS, PTP_NBDEV, PTP_NBDIE, PTP_NBERASE, PTP_NBPAR,
	PTP_NBPLT, PTP_NBSPARE, PTP_NBSUBVOL, PTP_NVRAM,
	PTP_OGMD, PTP_ON, PTP_OUTPUT,
	PTP_PAGBLK, PTP_PAGESIZE, PTP_PATH, PTP_PERF, PTP_PORT, PTP_PROTOCOL,
	PTP_READ, PTP_RELIABILITY, PTP_RNDR, PTP_RNDW, PTP_ROTSPD, PTP_ROWS,
	PTP_RULES,
	PTP_SCHEME, PTP_SECSIZE, PTP_SECTRK, PTP_SEQR, PTP_SEQW, PTP_SIZE, PTP_SSD,
	PTP_SUBVOL, PTP_SUSIZE, PTP_SYSTEM, PTP_SYNC,
	PTP_TARGET, PTP_TIER, PTP_TIME, PTP_TRANSLATION,
	PTP_TRKPLT, PTP_TSFRATE, PTP_TYPE,
	PTP_UNIT, PTP_UTEST,
	PTP_VOLUME,
	PTP_WAIT, PTP_WEARLEVELLING, PTP_WORKLOAD, PTP_WRITE,
	PTP_TOTAL,
};

//! \brief	Volume types which are handled per OGSSim.
enum OGSS_VolumeType {
	VTP_JBOD,
	VTP_RAID1,
	VTP_RAID01,
	VTP_RAIDNP,
	VTP_DECRAID,
	VTP_TOTAL
};

//! \brief	Declustering type used for RAID NP:
//!			- Off - without declustering
//!			- Parity
enum OGSS_DeclusteringType {
	DCL_OFF,
	DCL_PARITY,
	DCL_DATA,
	DCL_TOTAL
};

//! \brief	Device types which are handled per OGSSim.
enum OGSS_DeviceType {
	DTP_HDD,
	DTP_SSD,
	DTP_NVRAM,
	DTP_TOTAL
};

//! \brief	Interface type.
enum OGSS_InterfaceType {
	ITP_ETHERNET,
	ITP_SCSI,
	ITP_INFINIBAND,
	ITP_TOTAL
};

typedef OGSS_Ushort OGSS_MaintenanceMode;
const OGSS_MaintenanceMode MTM_SYNC = 0b001;
const OGSS_MaintenanceMode MTM_ASYC = 0b010;
const OGSS_MaintenanceMode MTM_EVNT = 0b100;

//! \brief	Maintenance return code.
enum OGSS_MaintenanceOutcode {
	MOC_NORMAL,
	MOC_RETRY,
	MOC_GC,
	MOC_TOTAL
};

//! \brief	Translation algorithm type.
enum OGSS_TranslationType {
	TRS_DEFAULT,
	TRS_SSDNAIVE,
	TRS_TOTAL
};

//! \brief	Defragmentation algorithm type.
enum OGSS_DefragmentationType {
	DFG_DEFAULT,
	DFG_TOTAL
};

//! \brief	Wear levelling algorithm type.
enum OGSS_WearLevellingType {
	WRL_DEFAULT,
	WRL_TOTAL
};

//! \brief	Garbage collection algorithm type.
enum OGSS_GarbageCollectionType {
	GCL_DEFAULT,
	GCL_TOTAL
};

//! \brief	HDD computation model type.
enum OGSS_HDDComputationType {
	HCP_DEFAULT,
	HCP_TOTAL
};

//! \brief	SSD computation model type.
enum OGSS_SSDComputationType {
	SCP_DEFAULT,
	SCP_TOTAL
};

//! \brief	NVRAM computation model type.
enum OGSS_NVRAMComputationType {
	NCP_DEFAULT,
	NCP_TOTAL
};

//! \brief	Interface computation model type.
enum OGSS_InterfaceComputationType {
	ICP_DEFAULT,
	ICP_TOTAL
};

//! \brief	Synchronization model type.
enum OGSS_SynchronizationType {
	SNC_DEFAULT,
	SNC_DEFV2,
	SNC_DEFV3,
	SNC_DEFV3OTF,
	SNC_DEFV4OTF,
	SNC_PARALLEL,
	SNC_QUEUE,
	SNC_SINGLEDISK,
	SNC_TOTAL
};

//! \brief	Visualization graph format type.
enum OGSS_GraphFormatType {
	GFM_LATEX,
	GFM_PNG,
	GFM_PDF,
	GFM_JS,
	GFM_TOTAL
};

//! \brief	Visualization graph type.
enum OGSS_GraphType {
	GPT_BUSPROF,
	GPT_DEVPROF,
	GPT_DEVTHRP,
	GPT_REQRESP,
	GPT_LATPER,
	GPT_MAINCLL,
	GPT_REQCPLT,
	GPT_WAITTM,
	GPT_WEARLVL,
	GPT_TOTAL
};

//! \brief	Event type.
enum OGSS_EventType {
	EVT_FAULT,
	EVT_REPLC,
	EVT_TOTAL
};

//! \brief	Declustered RAID type.
enum OGSS_DRDType {
	DRD_SD2S,
	DRD_SD2SOPT,
	DRD_TOTAL
};

//! \brief	Request types used in OGSSim. Ghost requests are ones which are
//! 		decomposed into child subrequests. Faker requests are parent
//! 		requests which are not requested by the user (not present in the
//! 		trace file), they are generated when a failure occurs, during the
//! 		reconstruction process or per maintenance routines.
enum OGSS_RequestType {
	RQT_READ					= 0b00000,
	RQT_WRITE					= 0b00001,
	RQT_ERASE					= 0b00111,
	RQT_FKFLT					= 0b10000,
	RQT_FKRPL					= 0b10001,
	RQT_EVFLT					= 0b11000,
	RQT_EVRPL					= 0b11001,
	RQT_EVSTP					= 0b11100,
	RQT_EVEND					= 0b11101,
	RQT_END						= 0b11111
};

//! \brief	Synchronization level: from/to host, from/to tier, from/to volume.
enum OGSS_SyncLevel {
	SNC_HOST, SNC_TIER, SNC_VOLM, SNC_TOTL
};

//! \brief	Request operation type.
enum OGSS_RequestOperation {
	ROP_NATIVE					= 0b00001,
	ROP_RECOVERY				= 0b00010,
	ROP_UPDATE					= 0b00100,
	ROP_RENEW					= 0b01000,
	ROP_COPY					= 0b10000,
	ROP_COPYBACK,				// used in decraid event management
	ROP_RECOVER,
	ROP_REDIRECTED,
	ROP_DBGNATIVE,
	ROP_DBGREDIRECTED,
	ROP_TOTAL
};


//! \brief	Dataunit structure, determine what memory and time unit to use
//!			during the simulation.
struct OGSS_DataUnit {
	OGSS_Real					_time;					//!< Time unit.
	OGSS_Ulong					_memory;				//!< Memory unit.

	OGSS_DataUnit (
		const OGSS_Real			t = 1,
		const OGSS_Ulong		m = 1):
		_time (t), _memory (m) {  }

	OGSS_Bool operator!= (const OGSS_DataUnit & a) {
		return _time != a._time || _memory != a._memory;
	}
};

/*----------------------------------------------------------------------------*/
/* OPERATOR OVERLOADS --------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//!	\brief	Output stream operator for request type.
//! \param	out					Output stream.
//! \param	type				Request type.
inline std::ostream & operator<< (
	std::ostream				& out,
	OGSS_RequestType			& type) {
	out << static_cast <OGSS_Short> (type);
	return out;
}

//! \brief	Input stream operator for request type.
//! \param	in					Input stream.
//! \param	type				Request type.
inline std::istream & operator>> (
	std::istream				& in,
	OGSS_RequestType			& type) {
	OGSS_Ushort					tmp;

	in >> tmp;

	if (tmp > 0b0101)
		type = RQT_READ;
	else
		type = static_cast <OGSS_RequestType> (tmp);

	return in;
}

/*----------------------------------------------------------------------------*/
/* CONSTANT VALUES -----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

const OGSS_Ushort				OGSS_USHORT_MAX		= UINT16_MAX;
	//!< Maximum value of a short unsigned.
const OGSS_Ulong				OGSS_ULONG_MAX		= UINT64_MAX;
	//!< Maximum value of a long unsigned.
const OGSS_Real					OGSS_REAL_MAX		= std::numeric_limits <OGSS_Real> ::max ();

const OGSS_Ulong				KILO				= 1024;
	//!< Kilo (K) factor.
const OGSS_Ulong				MEGA				= KILO*KILO;
	//!< Mega (M) factor.
const OGSS_Ulong				GIGA				= KILO*MEGA;
	//!< Giga (G) factor.
const OGSS_Ulong				TERA				= KILO*GIGA;
	//!< Tera (T) factor.

const OGSS_Ulong				MINUTE				= 60;
	//!< Minute factor.

const OGSS_Real					MILLI				= .001;
	//!< Milli (m) factor.
const OGSS_Real					MICRO				= MILLI * MILLI;
	//!< Micro (u) factor.
const OGSS_Real					NANO				= MILLI * MICRO;
	//!< Nano (n) factor.

const OGSS_String				MPI_EXEC			= "EX";
	//!< String used to determine if OGSSim is launched with mpirun.

const OGSS_Short numMonoProcessModules = 0
	+ 1							// communication manager
	+ 1							// workload extractor
	+ 1							// hardware extractor
	+ 1							// event extractor
	+ 1							// preproccessing
	+ 1							// execution
	+ 1							// evaluation
	+ 1							// synchronization
	;
	//!< Number of modules which need only one process.

const OGSS_Short numMultiProcessModules = 0
	+ 1							// volume driver
	+ 1							// device driver
	;
	//!< Number of modules which need n processes, with n the number of volumes.

const OGSS_Short numMonoProcessSynchBarrier = 0
	+ 1							// workload extractor
	+ 1							// hardware extractor
	+ 1							// event extractor
	+ 1							// preproccessing
	+ 1							// execution
	;
	//!< Number of monoprocess modules which will wait at the barrier.

const OGSS_Short numMultiProcessSynchBarrier = 0
	+ 1							// volume driver
	+ 1							// device driver
	;
	//!< Number of multiprocess modules which will wait at the barrier.

#define OGMP_STR(a,b) a.at (b)
#define OGMP_CST(a,b) a.at (b) .c_str ()

/*----------------------------------------------------------------------------*/
/* CONSTANT CONTAINERS -------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Synchronization models that used the on-the-fly reconstruction generation.
const std::vector <OGSS_SynchronizationType>
								OTFModels = {
	SNC_DEFV4OTF
};

//! \brief	Map between a module type and its name.
const std::map <OGSS_ModuleType, OGSS_String>
								ModuleNameMap = {
	{MTP_COMMUNICATION,			"CommunicationManager"},
	{MTP_COMMUNICATIONZMQ,		"CommunicationManagerZMQ"},
	{MTP_COMPUTATIONBUSADV,		"ComputationModelBusAdvanced"},
	{MTP_DECRAIDCTRL,			"DecRAIDVolCtrl"},
	{MTP_DEVICE,				"Device"},
	{MTP_EVALUATION,			"Evaluation"},
	{MTP_EVENT,					"Event"},
	{MTP_EXECUTION,				"Execution"},
	{MTP_GENLATENCY,			"GenLatency"},
	{MTP_GENWAITTIME,			"GenWaitTime"},
	{MTP_HARDWARE,				"Hardware"},
	{MTP_MPLIBSERIALIZER,		"MPLibSerializer"},
	{MTP_LTXSERIALIZER,			"LTXSerializer"},
	{MTP_PREPROCESSING,			"Preprocessing"},
	{MTP_RAWPARSER,				"RawParser"},
	{MTP_SSDALLOCDEFAULT,		"SSDAllocDefault"},
	{MTP_SYNCDEFV2,				"SyncDefV2"},
	{MTP_SYNCDEFV3,				"SyncDefV3"},
	{MTP_SYNCHRONIZATION,		"Synchronization"},
	{MTP_SYNCPAR,				"SyncParallel"},
	{MTP_SYNCQUEUE,				"SyncQueue"},
	{MTP_VOLJBOD,				"JBODController"},
	{MTP_VOLRAID01,				"RAID01Controller"},
	{MTP_VOLRAID1,				"RAID1Controller"},
	{MTP_VOLRAIDNPDATDEC,		"RAIDNPDatDecController"},
	{MTP_VOLRAIDNPNODEC,		"RAIDNPNoDecController"},
	{MTP_VOLRAIDNPPARDEC,		"RAIDNPParDecController"},
	{MTP_VOLUME,				"Volume"},
	{MTP_WORKLOAD,				"Workload"},
	{MTP_TOTAL,					"und."}
};

//! \brief	Map between a file type and its name.
const std::map <OGSS_FileType, OGSS_String>
								FileNameMap = {
	{FTP_CONFIGURATION,			"configuration"},
	{FTP_HARDWARE,				"hardware"},
	{FTP_LOGGING,				"logging"},
	{FTP_RESULT,				"result"},
	{FTP_RESUME,				"resume"},
	{FTP_SUBRESULT,				"subresult"},
	{FTP_UNITARYTEST,			"unitarytest"},
	{FTP_WORKLOAD,				"workload"},
	{FTP_TOTAL,					"und."}
};

//! \brief	Map between a communication type and its name.
const std::map <OGSS_CommType, OGSS_String>
								CommNameMap = {
	{CTP_ZMQ,					"zmq"},
	{CTP_IZMQ,					"izmq"},
	{CTP_MPI,					"mpi"},
	{CTP_TOTAL,					"und."}
};

//! \brief	Map between a parameter type and its name.
const std::map <OGSS_ParamType, OGSS_String>
								ParamNameMap = {
	{PTP_ADDRESS,				"address"},
	{PTP_AGRSK,					"avgrseek"},
	{PTP_AGWSK,					"avgwseek"},
	{PTP_ARG1,					"arg1"},
	{PTP_ARG2,					"arg2"},
	{PTP_ARG3,					"arg3"},
	{PTP_BANDWIDTH,				"bandwidth"},
	{PTP_BLKDIE,				"blocksperdie"},
	{PTP_BUFSIZE,				"buffersize"},
	{PTP_BUS,					"bus"},
	{PTP_BUSES,					"buses"},
	{PTP_BYTESPERCOL,			"bytespercol"},
	{PTP_NBCHIPS,				"nbchips"},
	{PTP_COLS,					"columns"},
	{PTP_COMM,					"communication"},
	{PTP_COMPUTATION,			"computation"},
	{PTP_CONFIG,				"config"},
	{PTP_CONTROLLER,			"controller"},
	{PTP_DATAUNIT,				"dataunit"},
	{PTP_DATAUNITS,				"dataunits"},
	{PTP_DATE,					"date"},
	{PTP_DECL,					"decl"},
	{PTP_DEFRAGMENTATION,		"defragmentation"},
	{PTP_DEVICE,				"device"},
	{PTP_DUNAME,				"duname"},
	{PTP_ENTRY,					"entry"},
	{PTP_ERASE,					"erase"},
	{PTP_EVENT,					"event"},
	{PTP_FORMAT,				"format"},
	{PTP_GARBAGECOLLECTION,		"garbagecollection"},
	{PTP_GEOMETRY,				"geometry"},
	{PTP_GLOBAL,				"global"},
	{PTP_GRAPH,					"graph"},
	{PTP_HDD,					"hdd"},
	{PTP_INPUT,					"input"},
	{PTP_INTERFACE,				"interface"},
	{PTP_IOPS,					"iops"},
	{PTP_MEMORY,				"memory"},
	{PTP_MNRSK,					"minrseek"},
	{PTP_MNWSK,					"minwseek"},
	{PTP_MTTF,					"mttf"},
	{PTP_MXRSK,					"maxrseek"},
	{PTP_MXWSK,					"maxwseek"},
	{PTP_NAME,					"name"},
	{PTP_NBDEV,					"nbdev"},
	{PTP_NBDIE,					"nbdies"},
	{PTP_NBERASE,				"nberase"},
	{PTP_NBPAR,					"nbpar"},
	{PTP_NBPLT,					"nbplatters"},
	{PTP_NBSPARE,				"nbspare"},
	{PTP_NBSUBVOL,				"nbsubvol"},
	{PTP_NVRAM,					"nvram"},
	{PTP_OGMD,					"ogmdsim"},
	{PTP_ON,					"on"},
	{PTP_OUTPUT,				"output"},
	{PTP_PAGBLK,				"pagesperblock"},
	{PTP_PAGESIZE,				"pagesize"},
	{PTP_PATH,					"path"},
	{PTP_PERF,					"performance"},
	{PTP_PORT,					"port"},
	{PTP_PROTOCOL,				"protocol"},
	{PTP_READ,					"read"},
	{PTP_RELIABILITY,			"reliability"},
	{PTP_RNDR,					"randread"},
	{PTP_RNDW,					"randwrite"},
	{PTP_ROWS,					"rows"},
	{PTP_ROTSPD,				"rotspeed"},
	{PTP_RULES,					"rules"},
	{PTP_SCHEME,				"scheme"},
	{PTP_SECSIZE,				"sectorsize"},
	{PTP_SECTRK,				"sectorspertrack"},
	{PTP_SEQR,					"seqread"},
	{PTP_SEQW,					"seqwrite"},
	{PTP_SIZE,					"size"},
	{PTP_SSD,					"ssd"},
	{PTP_SUBVOL,				"subvol"},
	{PTP_SUSIZE,				"susize"},
	{PTP_SYSTEM,				"system"},
	{PTP_SYNC,					"synchronization"},
	{PTP_TARGET,				"target"},
	{PTP_TIER,					"tier"},
	{PTP_TIME,					"time"},
	{PTP_TRANSLATION,			"translation"},
	{PTP_TRKPLT,				"tracksperplatter"},
	{PTP_TSFRATE,				"mediatransferrate"},
	{PTP_TYPE,					"type"},
	{PTP_UNIT,					"unit"},
	{PTP_UTEST,					"utest"},
	{PTP_VOLUME,				"volume"},
	{PTP_WAIT,					"wait"},
	{PTP_WEARLEVELLING,			"wearlevelling"},
	{PTP_WORKLOAD,				"workload"},
	{PTP_WRITE,					"write"},
	{PTP_TOTAL,					"und."}
};

//! \brief	Map between a volume type and its name.
const std::map <OGSS_VolumeType, OGSS_String>
								VolumeNameMap = {
	{VTP_JBOD,					"jbod"},
	{VTP_RAID1,					"raid1"},
	{VTP_RAID01,				"raid01"},
	{VTP_RAIDNP,				"raidnp"},
	{VTP_DECRAID,				"decraid"},
	{VTP_TOTAL,					"und."}
};

//! \brief	Map between a declustering type and its name.
const std::map <OGSS_DeclusteringType, OGSS_String>
								DeclusteringNameMap = {
	{DCL_OFF,					"off"},
	{DCL_PARITY,				"parity"},
	{DCL_DATA,					"data"},
	{DCL_TOTAL,					"und."}
};


//! \brief	Map between a device type and its name.
const std::map <OGSS_DeviceType, OGSS_String>
								DeviceNameMap = {
	{DTP_HDD,					"hdd"},
	{DTP_SSD,					"ssd"},
	{DTP_NVRAM,					"nvram"},
	{DTP_TOTAL,					"und."}
};

//! \brief	Map between an interface type and its name.
const std::map <OGSS_InterfaceType, OGSS_String>
								InterfaceNameMap = {
	{ITP_ETHERNET,				"eth"},
	{ITP_SCSI,					"scsi"},
	{ITP_INFINIBAND,			"ifb"},
	{ITP_TOTAL,					"und."}
};

//! \brief	Map between a translation algorithm and its name.
const std::map <OGSS_TranslationType, OGSS_String>
								TranslationNameMap = {
	{TRS_DEFAULT,				"default"},
	{TRS_SSDNAIVE,				"ssdnaive"},
	{TRS_TOTAL,					"und."}
};

//! \brief	Map between a defragmentation algorithm and its name.
const std::map <OGSS_DefragmentationType, OGSS_String>
								DefragmentationNameMap = {
	{DFG_DEFAULT,				"default"},
	{DFG_TOTAL,					"und."}
};

//! \brief	Map between a wear levelling algorithm and its name.
const std::map <OGSS_WearLevellingType, OGSS_String>
								WearLevellingNameMap = {
	{WRL_DEFAULT,				"default"},
	{WRL_TOTAL,					"und."}
};

//! \brief	Map between a garbage collection type and its name.
const std::map <OGSS_GarbageCollectionType, OGSS_String>
								GarbageCollectionNameMap = {
	{GCL_DEFAULT,				"default"},
	{GCL_TOTAL,					"und."}
};

//! \brief	Map between an HDD computation model and its name.
const std::map <OGSS_HDDComputationType, OGSS_String>
								HDDComputationNameMap = {
	{HCP_DEFAULT,				"default"},
	{HCP_TOTAL,					"und."}
};

//! \brief	Map between an SSD computation model and its name.
const std::map <OGSS_SSDComputationType, OGSS_String>
								SSDComputationNameMap = {
	{SCP_DEFAULT,				"default"},
	{SCP_TOTAL,					"und."}
};

//! \brief	Map between an NVRAM computation model and its name.
const std::map <OGSS_NVRAMComputationType, OGSS_String>
								NVRAMComputationNameMap = {
	{NCP_DEFAULT,				"default"},
	{NCP_TOTAL,					"und."}
};

//! \brief	Map between an interface computation model and its name.
const std::map <OGSS_InterfaceComputationType, OGSS_String>
								InterfaceComputationNameMap = {
	{ICP_DEFAULT,				"default"},
	{ICP_TOTAL,					"und."}
};

//! \brief	Map between a synchronization model and its name.
const std::map <OGSS_SynchronizationType, OGSS_String>
								SynchronizationNameMap = {
	{SNC_DEFAULT,				"default"},
	{SNC_DEFV2,					"defv2"},
	{SNC_DEFV3,					"defv3"},
	{SNC_DEFV3OTF,				"defv3otf"},
	{SNC_DEFV4OTF,				"defv4otf"},
	{SNC_PARALLEL,				"parallel"},
	{SNC_QUEUE,					"queue"},
	{SNC_SINGLEDISK,			"singledisk"},
	{SNC_TOTAL,					"und."}
};

//! \brief	Map between a visualization graph format and its name.
const std::map <OGSS_GraphFormatType, OGSS_String>
								GraphFormatNameMap = {
	{GFM_LATEX,					"tex"},
	{GFM_PNG,					"png"},
	{GFM_PDF,					"pdf"},
	{GFM_JS,					"js"},
	{GFM_TOTAL,					"und."}
};

//! \brief	Map between a visualization graph type and its name.
const std::map <OGSS_GraphType, OGSS_String>
								GraphTypeNameMap = {
	{GPT_BUSPROF,				"busprofile"},
	{GPT_DEVPROF,				"deviceprofile"},
	{GPT_DEVTHRP,				"devicethroughput"},
	{GPT_LATPER,				"latencyper"},
	{GPT_MAINCLL,				"maintenancecall"},
	{GPT_REQCPLT,				"requestcompletion"},
	{GPT_REQRESP,				"responsetime"},
	{GPT_WAITTM,				"waitingtime"},
	{GPT_WEARLVL,				"wearlevelling"},
	{GPT_TOTAL,					"und."}
};

//! \brief	Map between an event type and its name.
const std::map <OGSS_EventType, OGSS_String>
								EventTypeNameMap = {
	{EVT_FAULT,					"failure"},
	{EVT_REPLC,					"replace"},
	{EVT_TOTAL,					"und."}
};

//! \brief	Map between a declustered RAID scheme and its name.
const std::map <OGSS_DRDType, OGSS_String>
								DRDSchemeNameMap = {
	{DRD_SD2S,					"sd2s"},
	{DRD_SD2SOPT,				"sd2s-opt"},
	{DRD_TOTAL,					"und."}
};

//! \brief	Map between an event type and its request type.
const std::map <OGSS_RequestType, OGSS_EventType>
								EventTypeTranslation = {
	{RQT_EVFLT,					EVT_FAULT},
	{RQT_EVRPL,					EVT_REPLC}
};

#endif
