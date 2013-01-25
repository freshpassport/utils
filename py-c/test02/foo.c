#include <Python.h>

static PyObject* foo_bar(PyObject* self, PyObject* args)
{
	Py_RETURN_NONE;
}

static PyMethodDef foo_methods[] = {
	{"bar",(PyCFunction)foo_bar,METH_NOARGS,NULL},
	{NULL,NULL,0,NULL}
};

PyMODINIT_FUNC initfoo()
{
	Py_InitModule3("foo", foo_methods, "My first extension module.");
}
