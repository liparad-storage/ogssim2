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

//! \file	ltxserializer.cpp
//! \brief	Serializer for LaTeX files. This is used during the graph creation
//!			process. The graphs are created using the tikz package.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "serializer/ltxserializer.hpp"

#include <fstream>
#include <iostream>
#include <set>

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

void
writeIntroduction (
	ofstream				& output,
	const OGSS_String		xLabel,
	const OGSS_String		yLabel) {
	output << "\\documentclass{standalone}" << endl
		<< "\\usepackage[dvipsnames]{xcolor}" << endl
		<< "\\usepackage{pgfplots}" << endl
		<< "\\usepackage{tikz}" << endl
		<< "\\begin{document}" << endl
		<< "\\begin{tikzpicture}" << endl
		<< "\\begin{axis}" << endl;
}

void
writeIntroPieChart (
	ofstream				& output) {
	output << "\\documentclass{standalone}" << endl
		<< "\\usepackage[dvipsnames]{xcolor}" << endl
		<< "\\usepackage{pgfplots}" << endl
		<< "\\usepackage{pgf-pie}" << endl
		<< "\\usepackage{tikz}" << endl
		<< "\\begin{document}" << endl
		<< "\\begin{tikzpicture}" << endl;
}

void
writeConclusion (
	ofstream				& output) {
	output << "\\end{axis}" << endl
		<< "\\end{tikzpicture}" << endl
		<< "\\end{document}" << endl;
}

void
writeConclPieChart (
	ofstream				&output) {
	output << "\\end{tikzpicture}" << endl
		<< "\\end{document}" << endl;
}

void
LTXSerializer::createLineGraph (
	const OGSS_String		filename,
	vector <vector <pair <OGSS_Real, OGSS_Real>>>
							& values,
	vector <OGSS_String>	& legendLabels,
	const OGSS_String		xLabel,
	const OGSS_String		yLabel) {
	ofstream 				output (filename);

	writeIntroduction (output, xLabel, yLabel);

	output << "["
		<< "clip mode=individual," << endl
		<< "width=.8\\textwidth," << endl
		<< "height=6cm," << endl
		<< "xlabel={" << xLabel << "}," << endl
		<< "ylabel={" << yLabel << "}," << endl
		<< "ymin=0," << endl
		<< "legend style={font=\\tiny}," << endl
		<< "legend columns=2," << endl
		<< "legend style={font=\tiny,at={(0.5,1.1)},anchor=north}" << "]" << endl;

	for (auto & e: values) {
		output << "\\addplot plot [smooth] coordinates {";
		for (auto & f: e)
			output << "(" << f.first << ", " << f.second << ") ";
		output << "};" << endl;
	}

	output << "\\legend{";
	auto i = legendLabels.size ();
	for (auto & e: legendLabels) {
		--i;
		if (i) output << e << ", ";
		else output << e;
	}
	output << "}" << endl;

	writeConclusion (output);
}

void
LTXSerializer::createPointGraph (
	const OGSS_String		filename,
	vector <vector <pair <OGSS_Real, OGSS_Real>>>
							& values,
	vector <OGSS_String>	& legendLabels,
	const OGSS_String		xLabel,
	const OGSS_String		yLabel) {
	ofstream 				output (filename);

	writeIntroduction (output, xLabel, yLabel);

	output << "["
		<< "clip mode=individual," << endl
		<< "width=.8\\textwidth," << endl
		<< "height=6cm," << endl
		<< "xlabel={" << xLabel << "}," << endl
		<< "ylabel={" << yLabel << "}," << endl
		<< "ymin=0," << endl
		<< "legend style={font=\\tiny}," << endl
		<< "legend columns=2," << endl
		<< "legend pos=north west" << "]" << endl;

	for (auto & e: values) {
		output << "\\addplot plot [only marks] coordinates {";
		for (auto & f: e)
			output << "(" << f.first << ", " << f.second << ") ";
		output << "};" << endl;
	}

	output << "\\legend{";
	auto i = legendLabels.size ();
	for (auto & e: legendLabels) {
		--i;
		if (i) output << e << ", ";
		else output << e;
	}
	output << "}" << endl;

	writeConclusion (output);
}

void
LTXSerializer::createHistogram (
	const OGSS_String		filename,
	vector <vector <pair <OGSS_Real, OGSS_Real>>>
							& values,
	vector <OGSS_String>	& legendLabels,
	const OGSS_String		xLabel,
	const OGSS_String		yLabel) {
	ofstream 				output (filename);

	writeIntroduction (output, xLabel, yLabel);

	output << "["
		<< "ybar," << endl
		<< "bar width={.4em}," << endl
		<< "clip mode=individual," << endl
		<< "width=.8\\textwidth," << endl
		<< "height=6cm," << endl
		<< "xlabel={" << xLabel << "}," << endl
		<< "ylabel={" << yLabel << "}," << endl
		<< "legend style={font=\\tiny}," << endl
		<< "legend columns=2," << endl
		<< "legend pos=north west" << "]" << endl;

	for (auto & e: values) {
		output << "\\addplot coordinates {";
		for (auto & f: e)
			output << "(" << f.first << ", " << f.second << ") ";
		output << "};" << endl;
	}

	output << "\\legend{";
	auto i = legendLabels.size ();
	for (auto & e: legendLabels) {
		--i;
		if (i) output << e << ", ";
		else output << e;
	}
	output << "}" << endl;

	writeConclusion (output);
}

void
LTXSerializer::createStackedHistogram (
	const OGSS_String		filename,
	vector <vector <pair <OGSS_Real, OGSS_Real>>>
							& values,
	vector <OGSS_String>	& legendLabels,
	const OGSS_String		xLabel,
	const OGSS_String		yLabel) {
	ofstream 				output (filename);

	writeIntroduction (output, xLabel, yLabel);

	output << "["
		<< "ybar stacked," << endl
		<< "clip mode=individual," << endl
		<< "width=.8\\textwidth," << endl
		<< "height=6cm," << endl
		<< "xlabel={" << xLabel << "}," << endl
		<< "ylabel={" << yLabel << "}," << endl
		<< "ymin=0," << endl
		<< "legend style={font=\\tiny}," << endl
		<< "legend columns=2," << endl
		<< "legend pos=north west" << "]" << endl;

	for (auto & e: values) {
		output << "\\addplot coordinates {";
		for (auto & f: e)
			output << "(" << f.first << ", " << f.second << ") ";
		output << "};" << endl;
	}

	output << "\\legend{";
	auto i = legendLabels.size ();
	for (auto & e: legendLabels) {
		--i;
		if (i) output << e << ", ";
		else output << e;
	}
	output << "}" << endl;

	writeConclusion (output);
}

void
LTXSerializer::createPieChart (
	const OGSS_String		filename,
	vector <OGSS_Real>		& values,
	vector <OGSS_String>	& legendLabels) {
	ofstream				output (filename);

	writeIntroPieChart (output);

	output << "\\pie[text=pin]{" << values [0] * 100 << "/completed req.," << values [1] * 100 << "/failed req.}" << endl;

	writeConclPieChart (output);
}
