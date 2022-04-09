#include <iostream>
#include <set>
#include <vector>

#include "petric.h"

extern "C" {
    #include <Python.h>
}

PyObject*
petric(PyObject* self, PyObject* args);

static PyMethodDef ModuleFunctions [] =
{
	{ "petric", petric, METH_VARARGS,
	  "Documentation or docstring of function exposedFunction1." }
	// Sentinel value used to indicate the end of function listing.
	// All function listing must end with this value.
	,{nullptr, nullptr, 0, nullptr}									
};

/* Module definition */
static struct PyModuleDef ModuleDefinitions = {
	PyModuleDef_HEAD_INIT,
	// Module name as string 
	"native_petric",
	// Module documentation (docstring)
	"A sample C++ native-code module for python3.",
	-1,
	// Functions exposed to the module 
	ModuleFunctions
};

/** Module Initialization function: must have this name schema
 *  PyInit_<ModuleName> where ModuleName is the same base name of the 
 *  shared library ModuleName.so (on Linux) or ModuleName.pyd (on Windows)
 */
PyMODINIT_FUNC PyInit_native_petric(void)
{
	Py_Initialize();
	PyObject* pModule = PyModule_Create(&ModuleDefinitions);
	PyModule_AddObject(pModule, "version", Py_BuildValue("s", "version 0.1-Alpha"));
	return pModule;
}

std::set<int> parseIterableToIntSet(PyObject* obj) {
	PyObject* iterator = PyObject_GetIter(obj);
	PyObject* item;
	std::set<int> s;
	while((item = PyIter_Next(iterator))) {
		if (PyLong_Check(item)) {
			s.insert(PyLong_AsLong(item));
		}
		Py_DECREF(item);
	}
	Py_DECREF(iterator);
	return s;
}

PyObject*
petric(PyObject* self, PyObject* args)
{
	// Extract iterable of sets.
	PyObject* arg;
	if (!PyArg_ParseTuple(args, "O", &arg)) {
		PyErr_SetString(PyExc_TypeError, "petric expect 1 argument (iterable with coverage sets)");
		return nullptr;
	}
	PyObject *iterator = PyObject_GetIter(arg);
	if (iterator == nullptr || !PyIter_Check(iterator)) {
		PyErr_SetString(PyExc_TypeError, "petric argument must be an iterable");
		return nullptr;
	}
	PyObject *item;
	std::vector<std::set<int> > coverageVector;
	while ((item = PyIter_Next(iterator))) {
		coverageVector.push_back(parseIterableToIntSet(item));
		Py_DECREF(item);
	}
	Py_DECREF(iterator);

	/*std::cout << "Petric received vector of " << coverageVector.size() << " sets:" << std::endl;
	for (const auto& s : coverageVector) {
		std::cout << "{";
		for (const int x : s) {
			std:: cout << x << ", ";
		}
		std::cout << "}" << std::endl;
	}*/

	cora::petric::PetricResult petricResult;
	petricResult = cora::petric::petric(coverageVector);


	//std::cout << "ESS : ";
	// for (const auto essImplIdx : petricResult.essential_implicant_idx) {
	// 		std::cout << essImplIdx << ",";
	// }
	//std::cout << std::endl;


	// Construct response tuple.
	PyObject* essImplSet = PyFrozenSet_New(nullptr);
	for (const auto essImplIdx : petricResult.essential_implicant_idx) {
			PySet_Add(essImplSet, PyLong_FromLong(essImplIdx));
		//	std::cout << essImplIdx << ",";
	}
	//std::cout << std::endl;

	PyObject* piSet = PyFrozenSet_New(nullptr);
	for (const auto& sum : petricResult.sums_of_products) {
		PyObject* sumSet = PyFrozenSet_New(nullptr);
		for (const auto implIdx : sum) {
			PySet_Add(sumSet, PyLong_FromLong(implIdx));
		}
		PySet_Add(piSet, sumSet);
	}

	// All python functions that returns anything
	// should end with this macro 
	return PyTuple_Pack(2, essImplSet, piSet);
}