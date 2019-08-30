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

//! \file	hardware.hpp
//! \brief	The file contains the whole structure set used to store the
//!			simulated storage system information.

#ifndef _OGSS_HARDWARE_HPP_
#define _OGSS_HARDWARE_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <memory>
#include <vector>

#include "structure/types.hpp"

struct Tier;
struct Volume;
struct Device;
struct Interface;

/*----------------------------------------------------------------------------*/
/* STRUCTURES ----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Contains the basic information about the storage system.
struct HardwareParameters {
	OGSS_Real					_simulationTime;	//!< Simulation time.
	OGSS_Ushort					_numInterfaces;		//!< Number of interfaces.
	OGSS_Ushort					_numTiers;			//!< Number of tiers.
	OGSS_Ushort					_numVolumes;		//!< Number of volumes.
	OGSS_Ulong					_numDevices;		//!< Number of devices.
	OGSS_Ulong					_hostInterface;		//!< Interface between the
													//!< host and the tiers.
};

//! \brief	Contains the information of the storage system.
struct Hardware {
	std::vector <Interface>		_interfaces;		//!< Interfaces.
	std::vector <Tier>			_tiers;				//!< Tiers.
	std::vector <Volume>		_volumes;			//!< Volumes.
	std::vector <Device>		_devices;			//!< Devices.
	HardwareParameters			_param;				//!< Parameters.
};

//! \brief	Contains tier parameters.
struct Tier {
	OGSS_Ulong					_interface;			//!< Interface between the tier and the volumes.
	OGSS_Ulong					_bufferSize;		//!< Buffer size.

//! \brief	Apply a data unit (time/memory) to the structure.
//! \param	du					Data unit to apply.
	void applyDataUnit (
		const OGSS_DataUnit		du);
};

//! \brief	Contains volume parameters.
struct Volume {
	OGSS_Ushort					_parent;			//!< Parent tier index.
	OGSS_VolumeType				_type;				//!< Layout type.
	OGSS_Ulong					_interface;			//!< Interface between the volume and the devices.
	OGSS_Ulong					_bufferSize;		//!< Buffer size.
	OGSS_Ulong					_suSize;			//!< Stripe unit size.
	OGSS_Ushort					_numDevices;		//!< Number of devices.
	OGSS_Ushort					_numRedundancyDevices;	//!< Number of devices used for redundancy.
	OGSS_Ushort					_numSubVolumes;		//!< Number of subdevices (for Declustered RAID).
	OGSS_DeclusteringType		_declustering;		//!< Declustering (off, parity, data)
	OGSS_DRDType				_decraidScheme;		//!< Declustered RAID scheme.
	OGSS_Bool					_isSubVolume;		//!< TRUE if a subvolume of a declustered RAID.

//! \brief	Apply a data unit (time/memory) to the structure.
//! \param	du					Data unit to apply.
	void applyDataUnit (
		const OGSS_DataUnit		du);
};

//! \brief	Contains HDD-device parameters.
struct HDDParameters {
	OGSS_Real					_sectorSize;		//!< Sector size.
	OGSS_Ulong					_sectorsPerTrack;	//!< Number of sectors per track.
	OGSS_Ulong					_tracksPerPlatter;	//!< Number of tracks per platter.
	OGSS_Ulong					_numPlatters;		//!< Number of platters.

	OGSS_Real					_minReadSeek;		//!< Minimum read seek time for a sector.
	OGSS_Real					_avgReadSeek;		//!< Average read seek time for a sector.
	OGSS_Real					_maxReadSeek;		//!< Maximum read seek time for a sector.
	OGSS_Real					_minWriteSeek;		//!< Minimum write seek time for a sector.
	OGSS_Real					_avgWriteSeek;		//!< Average write seek time for a sector.
	OGSS_Real					_maxWriteSeek;		//!< Maximum write seek time for a sector.
	OGSS_Real					_rotationSpeed;		//!< Rotation speed.

	OGSS_Real					_transferRate;		//!< Intern ransfer rate for a data unit.

	OGSS_Ulong					_mttf;				//!< Mean time til failure.

	OGSS_Ulong					_lastSector;		//!< Last sector position.
	OGSS_Ulong					_lastTrack;			//!< Last track position.

	OGSS_TranslationType		_algTrns;			//!< Translation algorithm.
	OGSS_DefragmentationType	_algDfrg;			//!< Defragmentation algorithm.

//! \brief	Apply a data unit (time/memory) to the structure.
//! \param	du					Data unit to apply.
	void applyDataUnit (
		const OGSS_DataUnit		du);
};

//! \brief	Contains SSD-device parameters.
struct SSDParameters {
	OGSS_Real					_pageSize;			//!< Page size.
	OGSS_Ulong					_pagesPerBlock;		//!< Number of pages per block.
	OGSS_Ulong					_blocksPerDie;		//!< Number of blocks per die.
	OGSS_Ulong					_numDies;			//!< Number of dies.

	OGSS_Real					_randRead;			//!< Random read access time for a page.
	OGSS_Real					_randWrite;			//!< Random write access time for a page.
	OGSS_Real					_seqRead;			//!< Sequential read access time for a page.
	OGSS_Real					_seqWrite;			//!< Sequential write access time for a page.
	OGSS_Real					_erase;				//!< Erase time for a block.

	OGSS_Ulong					_numErase;			//!< Number of erasure per block.

	OGSS_TranslationType		_algTrns;			//!< Translation algorithm.
	OGSS_WearLevellingType		_algWL;				//!< Wear leveling algorithm.
	OGSS_GarbageCollectionType	_algGC;				//!< Garbage collection algorithm.

//! \brief	Apply a data unit (time/memory) to the structure.
//! \param	du					Data unit to apply.
	void applyDataUnit (
		const OGSS_DataUnit		du);
};

//! \brief	Contains NVRAM-device parameters.
struct NVRAMParameters{
	OGSS_Ulong					_nbChips;			//!< Number of chips.
	OGSS_Real					_bytesPerCol;		//!< Bytes per column.
	OGSS_Ulong					_rows;				//!< Number of rows.
	OGSS_Ulong					_cols;				//!< Number of columns.
	OGSS_Real					_read;				//!< Read access time.
	OGSS_Real					_write;				//!< Write access time.

//! \brief	Apply a data unit (time/memory) to the structure.
//! \param	du					Data unit to apply.
	void applyDataUnit (
		const OGSS_DataUnit		du);
};

//! \brief	Union for device parameters.
union Parameters {
	HDDParameters				h;					//!< HDD parameters.
	SSDParameters				s;					//!< SSD parameters.
	NVRAMParameters				n;					//!< NVRAM parameters.
};

//! \brief	Contains device parameters.
struct Device {
	OGSS_Ushort					_parent;			//!< Parent volume index.
	OGSS_DeviceType				_type;				//!< Device type.
	Parameters					_param;				//!< Type-specific parameters.
	OGSS_Real					_clock;				//!< Device clock.
	OGSS_Real					_busyTime;			//!< Device busy time.
	OGSS_Ulong					_physicalCapacity;	//!< Device capacity.
	OGSS_Ulong					_bufferSize;		//!< Buffer size.

//! \brief	Apply a data unit (time/memory) to the structure.
//! \param	du					Data unit to apply.
	void applyDataUnit (
		const OGSS_DataUnit		du);
};

//! \brief	Contains interface parameters.
struct Interface {
	OGSS_InterfaceType			_type;				//!< Interface type.
	OGSS_Ulong					_bandwidth;			//!< Bandwidth.

//! \brief	Apply a data unit (time/memory) to the structure.
//! \param	du					Data unit to apply.
	void applyDataUnit (
		const OGSS_DataUnit		du);
};

#endif
