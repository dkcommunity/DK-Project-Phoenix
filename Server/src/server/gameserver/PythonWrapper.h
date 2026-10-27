#ifndef __PYTHON_WRAPPER_H__
#define __PYTHON_WRAPPER_H__

#include "Types.h"
#include "Exception.h"

class PythonWrapper
{
public:
	PythonWrapper();
	~PythonWrapper();

	int runSimpleString(char* str);
	int runSimpleFile(char* fname);

	static PythonWrapper& Instance()
	{
		static PythonWrapper theInstance;
		return theInstance;
	}
};

#endif
