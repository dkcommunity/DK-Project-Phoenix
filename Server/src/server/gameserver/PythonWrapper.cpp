#include "PythonWrapper.h"
#include "Python.h"
#include <cstdio>

PythonWrapper::PythonWrapper()
{
	Py_Initialize();
}

PythonWrapper::~PythonWrapper()
{
	Py_Exit(0);
}

int PythonWrapper::runSimpleString(char* str)
{
	return PyRun_SimpleString(str);
}

int PythonWrapper::runSimpleFile(char* fname)
{
	FILE* pF = fopen(fname, "r");
	if ( pF == NULL ) return -1;
	int ret = PyRun_SimpleFile(pF, fname);
	fclose(pF);
	return ret;
}
