#include <Python.h>

int test_call_py_func()
{
	PyObject *pModule, *pFunc, *pArg, *pRetVal, *pName;

	Py_Initialize();
	if (!Py_IsInitialized())
	{
		fprintf(stderr, "初始化错误!\n");
		return -1;
	}

	pModule = pFunc = pArg = pRetVal = NULL;

	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");

	pName = PyString_FromString("wrap");
	pModule = PyImport_ImportModule("wrap");
	//pModule = PyImport_Import(pName);

	pFunc = PyObject_GetAttrString(pModule, "wrap");
	if(!PyCallable_Check(pFunc))
	{
		printf("func not callable!\n");
		return -1;
	}

	pArg = Py_BuildValue("(s)", "abc");
	pRetVal = PyObject_CallObject(pFunc, pArg);

	return printf("py ret: %s\n", PyString_AsString(pRetVal));
}

int main()
{
	return test_call_py_func();
}
