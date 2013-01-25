#include <Python.h>

int my_c_function(const char *arg)
{
	int n = atoi(arg);
	return n+1;
}

static PyObject *warp_my_c_function(PyObject *self, PyObject *args)
{
	const char *command;
	int n;

	if (!PyArg_ParseTuple(args, "s", &command))  // 把python的arg变量转换为c的变量command
		return NULL;
	n = my_c_function(command);
	return Py_BuildValue("i", n);
}

#if 1
// 方法列表
static PyMethodDev MyCppMethods[] = {
	{"MyCppFunc1", warp_my_c_function, METH_VARARGS, "get string ++1"},
	{NULL, NULL, 0, NULL}
};

// 模块初始化
PyMODINIT_FUNC initMyCppModule(void)
{
	PyObject *m = Py_InitModule("MyCppModule", MyCppMethods);
	if (m==NULL)
		return;
}
#endif
