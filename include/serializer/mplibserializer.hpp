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

//! \file	mplibserializer.hpp
//! \brief	Serializer for python files. This is used during the graph creation
//!			process. The graphs are created using the matplotlib package.

#ifndef _OGSS_MPLIBSERIALIZER_HPP_
#define _OGSS_MPLIBSERIALIZER_HPP_

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "structure/types.hpp"

#include "util/unitarytest.hpp"

#if USE_PYTHON_BINDING

/*----------------------------------------------------------------------------*/
/* NAMESPACE -----------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

//! \brief	Namespace which contains LaTeX visualization graph creation functions.
namespace MPLibSerializer {
//!	\brief	Initialization of the python wrapper.
	void initPythonWrapper ();

//! \brief	Destruction of the python wrapper.
	void finiPythonWrapper ();

//! \brief	Create a line plot.
//! \param	filename			Output filename.
//! \param	values				Plot values.
//! \param	legendLabels		Plot legend labels.
//! \param	xLabel				Plot X axis label.
//! \param	yLabel				Plot Y axis label.
//! \param	ext					Output extension.
	void createLineGraph (
		const OGSS_String		filename,
		std::vector <std::vector <std::pair <OGSS_Real, OGSS_Real>>>
								& values,
		std::vector <OGSS_String>
								& legendLabels,
		const OGSS_String		xLabel = "",
		const OGSS_String		yLabel = "",
		const OGSS_String		ext = "png");

//! \brief	Create a line plot.
//! \param	filename			Output filename.
//! \param	values				Plot values.
//! \param	legendLabels		Plot legend labels.
//! \param	rules				Plot rules coordinates.
//! \param	xLabel				Plot X axis label.
//! \param	yLabel				Plot Y axis label.
//! \param	ext					Output extension.
	void createLineGraphWithRules (
		const OGSS_String		filename,
		std::vector <std::vector <std::pair <OGSS_Real, OGSS_Real>>>
								& values,
		std::vector <OGSS_String>
								& legendLabels,
		std::vector <std::pair <OGSS_String, OGSS_Real>>
								& rules,
		const OGSS_String		xLabel = "",
		const OGSS_String		yLabel = "",
		const OGSS_String		ext = "png");

//! \brief	Create a line plot.
//! \param	filename			Output filename.
//! \param	values				Plot values for the first Y axis.
//! \param	values2				Plot values for the second Y axis.
//! \param	legendLabels		Plot legend labels for the first Y axis.
//! \param	legendLabels2		Plot legend labels for the second Y axis.
//! \param	rules				Plot rules coordinates.
//! \param	xLabel				Plot X axis label.
//! \param	yLabel				Plot Y axis label.
//! \param	ext					Output extension.
	void createLineDoubleAxisGraphWithRules (
		const OGSS_String		filename,
		std::vector <std::vector <std::pair <OGSS_Real, OGSS_Real>>>
								& values,
		std::vector <std::vector <std::pair <OGSS_Real, OGSS_Real>>>
								& values2,
		std::vector <OGSS_String>
								& legendLabels,
		std::vector <OGSS_String>
								& legendLabels2,
		std::vector <std::pair <OGSS_String, OGSS_Real>>
								& rules,
		const OGSS_Real			maxVal,
		const OGSS_String		xLabel = "",
		const OGSS_String		yLabel = "",
		const OGSS_String		yLabel2 = "",
		const OGSS_String		ext = "png");

//! \brief	Create a point plot.
//! \param	filename			Output filename.
//! \param	values				Plot values.
//! \param	legendLabels		Plot legend labels.
//! \param	xLabel				Plot X axis label.
//! \param	yLabel				Plot Y axis label.
//! \param	ext					Output extension.
	void createPointGraph (
		const OGSS_String		filename,
		std::vector <std::vector <std::pair <OGSS_Real, OGSS_Real>>>
								& values,
		std::vector <OGSS_String>
								& legendLabels,
		const OGSS_String		xLabel = "",
		const OGSS_String		yLabel = "",
		const OGSS_String		ext = "png");

//! \brief	Create a bar chart.
//! \param	filename			Output filename.
//! \param	values				Plot values.
//! \param	legendLabels		Plot legend labels.
//! \param	xLabel				Plot X axis label.
//! \param	yLabel				Plot Y axis label.
//! \param	ext					Output extension.
	void createHistogram (
		const OGSS_String		filename,
		std::vector <std::vector <std::pair <OGSS_Real, OGSS_Real>>>
								& values,
		std::vector <OGSS_String>
								& legendLabels,
		const OGSS_String		xLabel = "",
		const OGSS_String		yLabel = "",
		const OGSS_String		ext = "png");

//! \brief	Create a stacked bar chart.
//! \param	filename			Output filename.
//! \param	values				Plot values.
//! \param	legendLabels		Plot legend labels.
//! \param	xLabel				Plot X axis label.
//! \param	yLabel				Plot Y axis label.
//! \param	ext					Output extension.
	void createStackedHistogram (
		const OGSS_String		filename,
		std::vector <std::vector <std::pair <OGSS_Real, OGSS_Real>>>
								& values,
		std::vector <OGSS_String>
								& legendLabels,
		const OGSS_String		xLabel = "",
		const OGSS_String		yLabel = "",
		const OGSS_String		ext = "png");

//! \brief	Create a pie chart.
//! \param	filename			Output filename.
//! \param	values				Plot values.
//! \param	legendLabels		Plot legend labels.
//! \param	ext					Output extension.
	void createPieChart (
		const OGSS_String		filename,
		std::vector <OGSS_Real>	& values,
		std::vector <OGSS_String>
								& legendLabels,
		const OGSS_String		ext = "png");
};

#endif
#endif
