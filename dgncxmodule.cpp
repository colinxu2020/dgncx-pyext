#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "dgncx.cpp"

static PyObject* dgncx_calculator(PyObject* self, PyObject* args)
{
    Calculator();
    Py_RETURN_NONE;
}
static PyObject* dgncx_game(PyObject* self, PyObject* args)
{
    Game();
    Py_RETURN_NONE;
}
static PyObject* dgncx_solution(PyObject* self, PyObject* args)
{
    Solution();
    Py_RETURN_NONE;
}
static PyObject* dgncx_system(PyObject* self, PyObject* args)
{
    System();
    Py_RETURN_NONE;
}


static PyMethodDef dgncx_methods[] = {
    {"calculator",  dgncx_calculator, METH_VARARGS, ""},
    {"game",  dgncx_game, METH_VARARGS, ""},
    {"solution",  dgncx_solution, METH_VARARGS, ""},
    {"system",  dgncx_system, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


static struct PyModuleDef dgncx_module = {
    PyModuleDef_HEAD_INIT,
    "dgncx",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    dgncx_methods
};

PyMODINIT_FUNC PyInit_dgncx(void)
{
    return PyModule_Create(&dgncx_module);
}
