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

//! \file	mplibserializer.cpp
//! \brief	Serializer for python files. This is used during the graph creation
//!			process. The graphs are created using the matplotlib package.

/*----------------------------------------------------------------------------*/
/* HEADERS -------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#include "serializer/mplibserializer.hpp"

#if USE_PYTHON_BINDING

#include <Python.h>

using namespace std;

/*----------------------------------------------------------------------------*/
/* PUBLIC FUNCTIONS ----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

PyObject *
getFile () {
	PyObject				* file;

	file = PyUnicode_FromString ("mplinterface");
	return file;
}

void
MPLibSerializer::initPythonWrapper () {
	PyObject				* sysPath, * prog;

	Py_Initialize ();
	sysPath = PySys_GetObject ( (char *) "path");
	prog = PyUnicode_FromString ("./");
	PyList_Append (sysPath, prog);
	prog = PyUnicode_FromString ("./src/graph/");
	PyList_Append (sysPath, prog);
}

void
MPLibSerializer::finiPythonWrapper () {
	Py_Finalize ();
}

void
MPLibSerializer::createLineGraph (
	const OGSS_String		filename,
	vector <vector <pair <OGSS_Real, OGSS_Real>>>
							& values,
	vector <OGSS_String>	& legendLabels,
	const OGSS_String		xLabel,
	const OGSS_String		yLabel,
	const OGSS_String		ext) {
	PyObject				* module, * function, * pyFilename, * pyValues,
							* pyLegendLabels, * pyXLabel, * pyYLabel, * pyExt,
							* tmpLst, * tmpVal, * tmpStr, * args;

	module = PyImport_Import (getFile ());
	if (module == nullptr) PyErr_Print ();
	function = PyObject_GetAttrString (module, "createLineGraph");
	if (function == nullptr) PyErr_Print ();

	pyFilename = PyUnicode_FromString (filename.c_str () );
	pyXLabel = PyUnicode_FromString (xLabel.c_str () );
	pyYLabel = PyUnicode_FromString (yLabel.c_str () );
	pyExt = PyUnicode_FromString (ext.c_str () );

	pyValues = PyList_New (0);
	tmpLst = PyList_New (0);
	for (auto elt: values [0]) {
		tmpVal = PyFloat_FromDouble (elt.first);
		PyList_Append (tmpLst, tmpVal);
	}
	PyList_Append (pyValues, tmpLst);
	for (auto elt: values) {
		tmpLst = PyList_New (0);
		for (auto flt: elt) {
			tmpVal = PyFloat_FromDouble (flt.second);
			PyList_Append (tmpLst, tmpVal);
		}
		PyList_Append (pyValues, tmpLst);
	}

	pyLegendLabels = PyList_New (0);
	for (auto elt: legendLabels) {
		tmpStr = PyUnicode_FromString (elt.c_str () );
		PyList_Append (pyLegendLabels, tmpStr);
	}

	args = PyTuple_New (6);
	PyTuple_SetItem (args, 0, pyFilename);
	PyTuple_SetItem (args, 1, pyValues);
	PyTuple_SetItem (args, 2, pyLegendLabels);
	PyTuple_SetItem (args, 3, pyXLabel);
	PyTuple_SetItem (args, 4, pyYLabel);
	PyTuple_SetItem (args, 5, pyExt);

	if (PyObject_CallObject (function, args) == nullptr)
		PyErr_Print ();

//	Py_DECREF (function);
}

void
MPLibSerializer::createLineGraphWithRules (
	const OGSS_String		filename,
	vector <vector <pair <OGSS_Real, OGSS_Real>>>
							& values,
	vector <OGSS_String>	& legendLabels,
	vector <std::pair <OGSS_String, OGSS_Real>>
							& rules,
	const OGSS_String		xLabel,
	const OGSS_String		yLabel,
	const OGSS_String		ext) {
	PyObject				* module, * function, * pyFilename, * pyValues,
							* pyLegendLabels, * pyXLabel, * pyYLabel, * pyExt,
							* pyRules, * tmpLst, * tmpVal, * tmpStr, * args;

	module = PyImport_Import (getFile ());
	if (module == nullptr) PyErr_Print ();
	function = PyObject_GetAttrString (module, "createLineGraphWithRules");
	if (function == nullptr) PyErr_Print ();

	pyFilename = PyUnicode_FromString (filename.c_str () );
	pyXLabel = PyUnicode_FromString (xLabel.c_str () );
	pyYLabel = PyUnicode_FromString (yLabel.c_str () );
	pyExt = PyUnicode_FromString (ext.c_str () );

	pyValues = PyList_New (0);
	tmpLst = PyList_New (0);
	for (auto elt: values [0]) {
		tmpVal = PyFloat_FromDouble (elt.first);
		PyList_Append (tmpLst, tmpVal);
	}
	PyList_Append (pyValues, tmpLst);
	for (auto elt: values) {
		tmpLst = PyList_New (0);
		for (auto flt: elt) {
			tmpVal = PyFloat_FromDouble (flt.second);
			PyList_Append (tmpLst, tmpVal);
		}
		PyList_Append (pyValues, tmpLst);
	}

	pyLegendLabels = PyList_New (0);
	for (auto elt: legendLabels) {
		tmpStr = PyUnicode_FromString (elt.c_str () );
		PyList_Append (pyLegendLabels, tmpStr);
	}

	pyRules = PyList_New (0);
	for (auto elt: rules) {
		tmpLst = PyList_New (0);
		tmpStr = PyUnicode_FromString (elt.first.c_str () );
		PyList_Append (tmpLst, tmpStr);
		tmpVal = PyFloat_FromDouble (elt.second);
		PyList_Append (tmpLst, tmpVal);
		PyList_Append (pyRules, tmpLst);
	}

	args = PyTuple_New (7);
	PyTuple_SetItem (args, 0, pyFilename);
	PyTuple_SetItem (args, 1, pyValues);
	PyTuple_SetItem (args, 2, pyLegendLabels);
	PyTuple_SetItem (args, 3, pyXLabel);
	PyTuple_SetItem (args, 4, pyYLabel);
	PyTuple_SetItem (args, 5, pyRules);
	PyTuple_SetItem (args, 6, pyExt);

	if (PyObject_CallObject (function, args) == nullptr)
		PyErr_Print ();

//	Py_DECREF (function);
}

void
MPLibSerializer::createLineDoubleAxisGraphWithRules (
	const OGSS_String		filename,
	vector <vector <pair <OGSS_Real, OGSS_Real>>>
							& values,
	vector <vector <pair <OGSS_Real, OGSS_Real>>>
							& values2,
	vector <OGSS_String>	& legendLabels,
	vector <OGSS_String>	& legendLabels2,
	vector <std::pair <OGSS_String, OGSS_Real>>
							& rules,
	const OGSS_Real			maxVal,
	const OGSS_String		xLabel,
	const OGSS_String		yLabel,
	const OGSS_String		yLabel2,
	const OGSS_String		ext) {
	PyObject				* module, * function, * pyFilename, * pyValues,
							* pyLegendLabels, * pyXLabel, * pyYLabel, * pyExt,
							* pyRules, * tmpLst, * tmpVal, * tmpStr, * args,
							* pyValues2, * pyLegendLabels2, * pyYLabel2, * pyMaxVal;

	module = PyImport_Import (getFile ());
	if (module == nullptr) PyErr_Print ();
	function = PyObject_GetAttrString (module, "createLineDoubleAxisGraphWithRules");
	if (function == nullptr) PyErr_Print ();

	pyFilename = PyUnicode_FromString (filename.c_str () );
	pyXLabel = PyUnicode_FromString (xLabel.c_str () );
	pyYLabel = PyUnicode_FromString (yLabel.c_str () );
	pyYLabel2 = PyUnicode_FromString (yLabel2.c_str () );
	pyExt = PyUnicode_FromString (ext.c_str () );
	pyMaxVal = PyFloat_FromDouble (maxVal);

	pyValues = PyList_New (0);
	tmpLst = PyList_New (0);
	for (auto elt: values [0]) {
		tmpVal = PyFloat_FromDouble (elt.first);
		PyList_Append (tmpLst, tmpVal);
	}
	PyList_Append (pyValues, tmpLst);
	for (auto elt: values) {
		tmpLst = PyList_New (0);
		for (auto flt: elt) {
			tmpVal = PyFloat_FromDouble (flt.second);
			PyList_Append (tmpLst, tmpVal);
		}
		PyList_Append (pyValues, tmpLst);
	}

	pyValues2 = PyList_New (0);
	tmpLst = PyList_New (0);
	for (auto elt: values2 [0]) {
		tmpVal = PyFloat_FromDouble (elt.first);
		PyList_Append (tmpLst, tmpVal);
	}
	PyList_Append (pyValues2, tmpLst);
	for (auto elt: values2) {
		tmpLst = PyList_New (0);
		for (auto flt: elt) {
			tmpVal = PyFloat_FromDouble (flt.second);
			PyList_Append (tmpLst, tmpVal);
		}
		PyList_Append (pyValues2, tmpLst);
	}

	pyLegendLabels = PyList_New (0);
	for (auto elt: legendLabels) {
		tmpStr = PyUnicode_FromString (elt.c_str () );
		PyList_Append (pyLegendLabels, tmpStr);
	}

	pyLegendLabels2 = PyList_New (0);
	for (auto elt: legendLabels2) {
		tmpStr = PyUnicode_FromString (elt.c_str () );
		PyList_Append (pyLegendLabels2, tmpStr);
	}	

	pyRules = PyList_New (0);
	for (auto elt: rules) {
		tmpLst = PyList_New (0);
		tmpStr = PyUnicode_FromString (elt.first.c_str () );
		PyList_Append (tmpLst, tmpStr);
		tmpVal = PyFloat_FromDouble (elt.second);
		PyList_Append (tmpLst, tmpVal);
		PyList_Append (pyRules, tmpLst);
	}

	args = PyTuple_New (11);
	PyTuple_SetItem (args, 0, pyFilename);
	PyTuple_SetItem (args, 1, pyValues);
	PyTuple_SetItem (args, 2, pyValues2);
	PyTuple_SetItem (args, 3, pyLegendLabels);
	PyTuple_SetItem (args, 4, pyLegendLabels2);
	PyTuple_SetItem (args, 5, pyXLabel);
	PyTuple_SetItem (args, 6, pyYLabel);
	PyTuple_SetItem (args, 7, pyYLabel2);
	PyTuple_SetItem (args, 8, pyRules);
	PyTuple_SetItem (args, 9, pyMaxVal);
	PyTuple_SetItem (args, 10, pyExt);

	if (PyObject_CallObject (function, args) == nullptr)
		PyErr_Print ();

//	Py_DECREF (function);
}

void
MPLibSerializer::createPointGraph (
	const OGSS_String		filename,
	vector <vector <pair <OGSS_Real, OGSS_Real>>>
							& values,
	vector <OGSS_String>	& legendLabels,
	const OGSS_String		xLabel,
	const OGSS_String		yLabel,
	const OGSS_String		ext) {
	PyObject				* module, * function, * pyFilename, * pyValues,
							* pyLegendLabels, * pyXLabel, * pyYLabel, * pyExt,
							* tmpLst, * tmpVal, * tmpStr, * args;

	module = PyImport_Import (getFile ());
	function = PyObject_GetAttrString (module, "createPointGraph");

	pyFilename = PyUnicode_FromString (filename.c_str () );
	pyXLabel = PyUnicode_FromString (xLabel.c_str () );
	pyYLabel = PyUnicode_FromString (yLabel.c_str () );
	pyExt = PyUnicode_FromString (ext.c_str () );

	pyValues = PyList_New (0);
	tmpLst = PyList_New (0);
	for (auto elt: values [0]) {
		tmpVal = PyFloat_FromDouble (elt.first);
		PyList_Append (tmpLst, tmpVal);
	}
	PyList_Append (pyValues, tmpLst);
	for (auto elt: values) {
		tmpLst = PyList_New (0);
		for (auto flt: elt) {
			tmpVal = PyFloat_FromDouble (flt.second);
			PyList_Append (tmpLst, tmpVal);
		}
		PyList_Append (pyValues, tmpLst);
	}

	pyLegendLabels = PyList_New (0);
	for (auto elt: legendLabels) {
		tmpStr = PyUnicode_FromString (elt.c_str () );
		PyList_Append (pyLegendLabels, tmpStr);
	}

	args = PyTuple_New (6);
	PyTuple_SetItem (args, 0, pyFilename);
	PyTuple_SetItem (args, 1, pyValues);
	PyTuple_SetItem (args, 2, pyLegendLabels);
	PyTuple_SetItem (args, 3, pyXLabel);
	PyTuple_SetItem (args, 4, pyYLabel);
	PyTuple_SetItem (args, 5, pyExt);

	PyObject_CallObject (function, args);
}

void
MPLibSerializer::createHistogram (
	const OGSS_String		filename,
	vector <vector <pair <OGSS_Real, OGSS_Real>>>
							& values,
	vector <OGSS_String>	& legendLabels,
	const OGSS_String		xLabel,
	const OGSS_String		yLabel,
	const OGSS_String		ext) {
	PyObject				* module, * function, * pyFilename, * pyValues,
							* pyLegendLabels, * pyXLabel, * pyYLabel, * pyExt,
							* tmpLst, * tmpVal, * tmpStr, * args;

	module = PyImport_Import (getFile ());
	function = PyObject_GetAttrString (module, "createHistogram");

	pyFilename = PyUnicode_FromString (filename.c_str () );
	pyXLabel = PyUnicode_FromString (xLabel.c_str () );
	pyYLabel = PyUnicode_FromString (yLabel.c_str () );
	pyExt = PyUnicode_FromString (ext.c_str () );

	pyValues = PyList_New (0);
	tmpLst = PyList_New (0);
	for (auto elt: values [0]) {
		tmpVal = PyFloat_FromDouble (elt.first);
		PyList_Append (tmpLst, tmpVal);
	}
	PyList_Append (pyValues, tmpLst);
	for (auto elt: values) {
		tmpLst = PyList_New (0);
		for (auto flt: elt) {
			tmpVal = PyFloat_FromDouble (flt.second);
			PyList_Append (tmpLst, tmpVal);
		}
		PyList_Append (pyValues, tmpLst);
	}

	pyLegendLabels = PyList_New (0);
	for (auto elt: legendLabels) {
		tmpStr = PyUnicode_FromString (elt.c_str () );
		PyList_Append (pyLegendLabels, tmpStr);
	}

	args = PyTuple_New (6);
	PyTuple_SetItem (args, 0, pyFilename);
	PyTuple_SetItem (args, 1, pyValues);
	PyTuple_SetItem (args, 2, pyLegendLabels);
	PyTuple_SetItem (args, 3, pyXLabel);
	PyTuple_SetItem (args, 4, pyYLabel);
	PyTuple_SetItem (args, 5, pyExt);

	PyObject_CallObject (function, args);
}

void
MPLibSerializer::createStackedHistogram (
	const OGSS_String		filename,
	vector <vector <pair <OGSS_Real, OGSS_Real>>>
							& values,
	vector <OGSS_String>	& legendLabels,
	const OGSS_String		xLabel,
	const OGSS_String		yLabel,
	const OGSS_String		ext) {
	PyObject				* module, * function, * pyFilename, * pyValues,
							* pyLegendLabels, * pyXLabel, * pyYLabel, * pyExt,
							* tmpLst, * tmpVal, * tmpStr, * args;

	module = PyImport_Import (getFile ());
	if (module == nullptr)
		PyErr_Print ();
	function = PyObject_GetAttrString (module, "createStackedHistogram");
	if (function == nullptr)
		PyErr_Print ();

	pyFilename = PyUnicode_FromString (filename.c_str () );
	pyXLabel = PyUnicode_FromString (xLabel.c_str () );
	pyYLabel = PyUnicode_FromString (yLabel.c_str () );
	pyExt = PyUnicode_FromString (ext.c_str () );

	pyValues = PyList_New (0);
	tmpLst = PyList_New (0);
	for (auto elt: values [0]) {
		tmpVal = PyFloat_FromDouble (elt.first);
		PyList_Append (tmpLst, tmpVal);
	}
	PyList_Append (pyValues, tmpLst);
	for (auto elt: values) {
		tmpLst = PyList_New (0);
		for (auto flt: elt) {
			tmpVal = PyFloat_FromDouble (flt.second);
			PyList_Append (tmpLst, tmpVal);
		}
		PyList_Append (pyValues, tmpLst);
	}

	pyLegendLabels = PyList_New (0);
	for (auto elt: legendLabels) {
		tmpStr = PyUnicode_FromString (elt.c_str () );
		PyList_Append (pyLegendLabels, tmpStr);
	}

	args = PyTuple_New (6);
	PyTuple_SetItem (args, 0, pyFilename);
	PyTuple_SetItem (args, 1, pyValues);
	PyTuple_SetItem (args, 2, pyLegendLabels);
	PyTuple_SetItem (args, 3, pyXLabel);
	PyTuple_SetItem (args, 4, pyYLabel);
	PyTuple_SetItem (args, 5, pyExt);

	if (PyObject_CallObject (function, args) == nullptr)
		PyErr_Print ();
}

void
MPLibSerializer::createPieChart (
	const OGSS_String		filename,
	std::vector <OGSS_Real>	& values,
	std::vector <OGSS_String>
							& legendLabels,
	const OGSS_String		ext) {
	PyObject				* module, * function, * pyFilename, * pyValues,
							* pyLegendLabels, * pyExt,
							* tmpVal, * tmpStr, * args;

	module = PyImport_Import (getFile ());
	if (module == nullptr)
		PyErr_Print ();
	function = PyObject_GetAttrString (module, "createPieChart");
	if (function == nullptr)
		PyErr_Print ();

	pyFilename = PyUnicode_FromString (filename.c_str () );
	pyExt = PyUnicode_FromString (ext.c_str () );

	pyValues = PyList_New (0);
	for (auto elt: values) {
		tmpVal = PyFloat_FromDouble (elt);
		PyList_Append (pyValues, tmpVal);
	}

	pyLegendLabels = PyList_New (0);
	for (auto elt: legendLabels) {
		tmpStr = PyUnicode_FromString (elt.c_str () );
		PyList_Append (pyLegendLabels, tmpStr);
	}

	args = PyTuple_New (4);
	PyTuple_SetItem (args, 0, pyFilename);
	PyTuple_SetItem (args, 1, pyValues);
	PyTuple_SetItem (args, 2, pyLegendLabels);
	PyTuple_SetItem (args, 3, pyExt);

	if (PyObject_CallObject (function, args) == nullptr)
		PyErr_Print ();
}

#endif
