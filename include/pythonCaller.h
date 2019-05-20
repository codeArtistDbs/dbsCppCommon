#pragma once
#include <string>
#include <vector>
#include <Python.h>  
#include "FileFunction.h"
// typedef struct _object PyObject;
class CPythonCaller
{
public:
	CPythonCaller(const std::string& vPythonFileName)
	{
		Py_Initialize();
		PyRun_SimpleString("import sys");
		std::string path = extractFilePath(vPythonFileName);
		if (path.empty()) path = "./";
		PyRun_SimpleString(("sys.path.append(\"" + path + "\")").c_str());
		PyObject *pName, *pModule, *pDict, *pFunc, *pArgs, *pReturn;
		std::string pythonFile = extractFileName(vPythonFileName);
		pythonFile = deleteExtentionName(pythonFile, false);
		pName = PyUnicode_DecodeFSDefault(pythonFile.c_str());
		pModule = PyImport_Import(pName);
		m_pPythonFunctions = PyModule_GetDict(pModule);
		Py_DECREF(pModule);
		Py_DECREF(pName);
	}
	~CPythonCaller()
	{
		Py_Finalize();
	}
	template<typename T>void appandSampleParameter(const T& v, const std::string& vFormat)
	{
		m_paraSet.push_back(Py_BuildValue(vFormat.c_str(), v));
	}
	void appandArrayParameter(void* vpData, const std::string& vFormat, int vBiytes)
	{
		Py_buffer *buf = (Py_buffer *)malloc(sizeof(*buf));
		int r = PyBuffer_FillInfo(buf, NULL, vpData, vBiytes, 0, PyBUF_CONTIG);
		PyObject *mv = PyMemoryView_FromBuffer(buf);
		m_paraSet.push_back(mv);
		m_paraSet.push_back(Py_BuildValue("s", vFormat.c_str()));
		free(buf);
	}
	int call(const std::string& vFunctionName)
	{
		*pArgs;
		PyObject* pFunc = PyDict_GetItemString(m_pPythonFunctions, vFunctionName.c_str());
		pArgs = PyTuple_New(m_paraSet.size());
		for (int i = 0; i < m_paraSet.size(); ++i)
			PyTuple_SetItem(pArgs, i, m_paraSet[i]);

		PyObject* pReturn = PyObject_CallObject(pFunc, pArgs);
		int result = PyLong_AsLong(pReturn);

		Py_DECREF(pArgs);
		Py_DECREF(pReturn);
		for (auto p : m_paraSet)
			Py_DECREF(p);
		m_paraSet.resize(0);
		return result;
	}
protected:
private:
	PyObject * m_pPythonFunctions, *pArgs;
	std::vector<PyObject *> m_paraSet;
};


/*
CMakeLists.txt:
	find_package(PythonLibs)
	include_directories(${PYTHON_INCLUDE_DIRS})
	target_link_libraries(exeOrDynamiclib  ${PYTHON_LIBRARIES})

example:
python:
import os
import sys
import struct
import numpy as np
def add(a,b,c):
	print ("a = " , a, c, type(c)       )
	b = np.frombuffer(b, dtype=c)
	print ("in python function add"  )
	print ("b = " , b      )
	print ("ret = " , a+b[0] , type(b)     )
	b[0] = 0
	b[1] = 65537
	b[2] = 0

	return  a
cpp:
CPythonCaller pyCaller("../pytest.py");
pyCaller.appandSampleParameter(2, "i");
int data[] = { 256,3,4,4 };
pyCaller.appandArrayParameter(data, "i", sizeof(data));
pyCaller.call("add");
pyCaller.appandSampleParameter(333.3f, "f");
pyCaller.appandArrayParameter(data, "i", sizeof(data));
int r = pyCaller.call("add");
std::cout << r << std::endl;
*/