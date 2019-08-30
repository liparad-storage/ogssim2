/*
 * Copyright UVSQ - CEA/DAM/DIF (2019)
 * Contributors:	Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *					Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General
 * Public License as published per the Free Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

//! \file			system.hpp
//! \brief			Definition of the system class which is one of the two main data structures of OGMDSim. It contains
//!					the information about the servers used in the system  
//!					a proximity score to the host nodes and facilitates the right metadata node
//!					when processing a request.

#ifndef _OGMDS_HPP_SYSTEM_
#define _OGMDS_HPP_SYSTEM_

/*--------------------------------------------------------------------------------------------------------------------*/
/* HEADERS -----------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#include "scorecomputation.hpp"
#include "xmlextract.hpp"

#include <cstdint>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <vector>

#include <glog/logging.h>

//! \brief			Data structure for the metadata system. Besides containing the parameters extracted from the
//!					system configuration file, it also stores the best paths between the host and the metadata nodes.
class System {
public:

/*--------------------------------------------------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS --------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Constructor.
//! \param			parser				Parser for getting the parameters of the metadata system.
	System (
		OGXML							& parser);

//! \brief			Destructor.
	~System ();

//! \brief			Getter for the next available metadata server depending on the host which launches the request.
//!					The next available server is the closest one whose soft limit is not exceeded.
//! \param			server				Host server ID.
//! \param								Computed metadata server ID.
	int getNextAvailableMDServer (
		const int						server);

//! \brief			Getter for the next overloaded metadata server depending on the host which launches the request.
//!					The next overloaded server is the closest one whose hard limit is not exceeded.
//! \param			server				Host server ID.
//! \param								Computed metadata server ID.
	int getNextOverloadedMDServer (
		const int						server);

//! \brief			Add an object in the specified metadata server.
//! \param			server				Host server ID.
//! \param			objID				Object ID.
	void addObject (
		const int						server,
		const uint64_t					objID);

//! \brief			Remove the specified metadata object from the specified server if the object ID is different from
//!					from MAX (not efficient). If not, it removes the first object from the node queue.
//! \param			server				Host server ID (unused if negative).
//! \param			objID				Object ID (used if server is negative).
//! \return								Removed object ID.
	uint64_t rmObject (
		const int						server,
		const uint64_t					objID = std::numeric_limits <uint64_t> ::max () );

//! \brief			Getter for the number of servers in the system.
//! \return								Number of servers.
	inline int getNbServers ()
		{ return _nbServers; }

//! \brief			Getter for the number of host servers in the system.
//! \return								Number of host servers.
	inline int getNbHostServers ()
		{ return _nbHostServers; }

//! \brief			Getter for the number of metadata servers in the system.
//! \return								Number of metadata servers.
	inline int getNbMetadataServers ()
		{ return _nbMetadataServers; }

//! \brief			Getter for a server size (number of stored metadata objects).
//! \return								Requested load.
	inline uint64_t getLoad (
		const int						server)
		{ LOG_IF (FATAL, server >= _nodeLoad.size () ) << "ERROR!!!";
			return _nodeLoad [server] .size (); }

//! \brief			Getter for the metadata server maximum size.
//! \return								Metadata server maximum size.
	inline uint64_t getMaxServerSize ()
		{ return _mssize; }

//! \brief			Getter for the latency between two nodes.
//! \param			hostID				Host node ID.
//! \param			metadataID			Metadata node ID.
//! \return								Requested latency.
	inline double getLatency (
		const unsigned					hostID,
		const unsigned					metadataID)
		{ return _paths [hostID][metadataID - _nbHostServers] .first; }

private:

/*--------------------------------------------------------------------------------------------------------------------*/
/* PRIVATE FUNCTIONS -------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

//! \brief			Construct the paths using a Bellman-Ford algorithm.
	void updatePaths ();

//! \brief			Construct the score computation class by extracting its parameters from the configuration file.
	void buildScore ();

//! \brief			Order the paths by the score.
	void updateScores ();

/*--------------------------------------------------------------------------------------------------------------------*/
/* ATTRIBUTES --------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

	Score								_score;									//!< Score computation class.
	OGXML								& _parser;								//!< XML parser.

	int									_nbServers {0};							//!< Number of servers.
	int									_nbHostServers {0};						//!< Number of host servers.
	int									_nbMetadataServers {0};					//!< Number of metadata servers.
	std::vector <std::vector <double>>	_matrix;								//!< Adjacency matrix.
	std::vector <std::vector <std::pair <double, int>>>
										_paths;									//!< Paths between nodes (latency and
																				//!< number of links).
	std::vector <std::multimap <double, int, std::greater <double>>>
										_orderedServers;						//!< List of servers ordered by their
																				//!< score.

	uint64_t							_mssize {1};							//!< Hard metadata server capacity.
	uint64_t							_limit {1};								//!< Soft metadata server capacity.
	std::vector <std::queue <uint64_t> >
										_nodeLoad;								//!< Stores for each metadata node the
																				//!< existent metadata. Indicates also
																				//!< the node load.
};

#endif
