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

//! \file	xxmlparser-base.cpp
//! \brief	Namespaces used for XML parsing. Contains the binding functions
//! 		between the library and OGSSim.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include <sstream>

#include "parser/xxmlparser.hpp"

#if USE_STATIC_GLOG
#include "glog/logging.h"
#else
#include <glog/logging.h>
#endif

#if ! USE_TINYXML

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>

/*----------------------------------------------------------------------------*/
/* MACROS --------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Xercesc string getter.
#define _DOM_GET_STRING(elt)	XMLString::transcode(elt)
//! \brief	Xercesc string destructor.
#define _DOM_FREE_STRING(elt)	XMLString::release(&elt)

using namespace std;
using namespace xercesc;

/*----------------------------------------------------------------------------*/
/* PRIVATE NAMESPACE ---------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

namespace {
	DOMNode *
	_getNode (
		DOMNode				* node,
		const OGSS_String	& nodename,
		OGSS_Bool			recursive = false) {
		DOMNode				* result;
		DOMNodeList			* list;
		char				* tmp;

		list = node->getChildNodes ();

		for (auto idx = 0; idx < list->getLength (); ++idx) {
			tmp = _DOM_GET_STRING (list->item (idx) ->getNodeName () );

			if (nodename.compare (tmp) == 0) {
				_DOM_FREE_STRING (tmp);
				return list->item (idx);
			}

			_DOM_FREE_STRING (tmp);

			if (recursive) {
				result = _getNode (list->item (idx), nodename, true);
				if (result)
					return result;
			}
		}

		return NULL;
	}

	OGSS_String
	_getValue (
		DOMNode				* node,
		const OGSS_String	& attribute) {
		OGSS_String			result = "";
		DOMNode				* attr;
		DOMNamedNodeMap		* attrMap;
		char				* tmp;

		attrMap = node->getAttributes ();

		if (attrMap != NULL) {
			for (auto idx = 0; idx < attrMap->getLength (); ++idx) {
				attr = attrMap->item (idx);
				tmp = _DOM_GET_STRING (attr->getNodeName () );

				if (attribute.compare (tmp) == 0) {
					_DOM_FREE_STRING (tmp);
					tmp = _DOM_GET_STRING (attr->getNodeValue () );
					result = tmp;
					_DOM_FREE_STRING (tmp);
					break;
				}

				_DOM_FREE_STRING (tmp);
			}
		}

		return result;
	}

	OGSS_String
	_getText (
		DOMNode				* node) {
		OGSS_String			result = "";
		char				* tmp;

		if (node->getTextContent () != NULL) {
			tmp = _DOM_GET_STRING (node->getTextContent () );
			result = tmp;
			_DOM_FREE_STRING (tmp);
		}

		return result;
	}

	OGSS_Ulong
	_getLong (
		DOMNode				* node,
		const OGSS_String	& name,
		const OGSS_Bool		& attribute = false) {
		OGSS_String			result = "";

		if (attribute)
			result = _getValue (node, name);
		else {
			node = _getNode (node, name);
			if (node)
				result = _getText (node);
		}

		if (! result.length () )
			return 0;

		return atol (result.c_str () );
	}

	OGSS_Real
	_getReal (
		DOMNode				* node,
		const OGSS_String	& name,
		const OGSS_Bool		& attribute = false) {
		OGSS_String			result = "";

		if (attribute)
			result = _getValue (node, name);
		else {
			node = _getNode (node, name);
			if (node)
				result = _getText (node);
		}

		if (! result.length () )
			return 0;

		return atof (result.c_str () );
	}

	OGSS_String
	_getString (
		DOMNode				* node,
		const OGSS_String	& name,
		const OGSS_Bool		& attribute = false) {
		OGSS_String			result = "";

		if (attribute)
			result = _getValue (node, name);
		else {
			node = _getNode (node, name);
			if (node)
				result = _getText (node);
		}

		return result;
	}

	OGSS_Bool
	_getBool (
		DOMNode				* node,
		const OGSS_String	& name,
		const OGSS_Bool		& attribute = false) {
		OGSS_String			result = "";

		if (attribute)
			result = _getValue (node, name);
		else {
			node = _getNode (node, name);
			if (node)
				result = _getText (node);
		}

		return result.compare ("on")
			|| result.compare ("yes")
			|| result.compare ("true");
	}

	OGSS_Long
	_getLongPrefix (
		DOMNode					* root,
		OGSS_ParamType			param,
		OGSS_Bool				attribute = false) {
		OGSS_Long				res;
		istringstream			iss;
		char					prefix = ' ';

		iss.str (_getString (root, ParamNameMap.at (param), attribute) );

		iss >> res >> prefix;

		if (prefix == 'K')		res *= KILO;
		else if (prefix == 'M')	res *= MEGA;
		else if (prefix == 'G')	res *= GIGA;
		else if (prefix == 'T')	res *= TERA;

		return res;
	}

	OGSS_Real
	_getRealPrefix (
		DOMNode					* root,
		OGSS_ParamType			param,
		OGSS_Bool				attribute = false) {
		OGSS_Real				res;
		istringstream			iss;
		char					prefix = ' ';

		iss.str (_getString (root, ParamNameMap.at (param), attribute) );

		iss >> res >> prefix;

		if (prefix == 'K')		res *= KILO;
		else if (prefix == 'M')	res *= MEGA;
		else if (prefix == 'G')	res *= GIGA;
		else if (prefix == 'T')	res *= TERA;
		else if (prefix == 'm')	res *= MILLI;
		else if (prefix == 'u')	res *= MICRO;
		else if (prefix == 'n')	res *= NANO;

		return res;
	}

	OGSS_Real
	_getRealMeasure (
		DOMNode					* root,
		OGSS_ParamType			param) {
		DOMNode					* node;
		OGSS_Long				size;
		OGSS_String				unit;

		node = _getNode (root, ParamNameMap.at (param) );

		unit = _getString (node, ParamNameMap.at (PTP_UNIT), true);

		if (! unit.compare (ParamNameMap.at (PTP_BANDWIDTH) ) ) {
			return 1 / _getRealPrefix (root, param);

		} else if (! unit.compare (ParamNameMap.at (PTP_IOPS) ) ) {
			size = _getLongPrefix (node, PTP_SIZE, true);
			return 1 / (_getRealPrefix (root, param) * size);

		} else if (! unit.compare (ParamNameMap.at (PTP_TIME) ) ) {
			return _getRealPrefix (root, param);
		}
		else DLOG(INFO) << "ERROR: " << unit;

		return .0;
	}
}

#endif
