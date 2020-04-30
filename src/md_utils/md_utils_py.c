#include <assert.h>
#include <string.h>

#include <Python.h>

#include "md_utils_py.h"


#if HAVE_PY_WRAPPER


/* ---------- bar generate py interfaces begin ---------- */
extern PyObject* bg_create(PyObject* self, PyObject* args);
extern PyObject* bg_set_end_auction(PyObject* self, PyObject* args);
extern PyObject* bg_reset(PyObject* self, PyObject* args);
extern PyObject* bg_release(PyObject* self, PyObject* args);
extern PyObject* bg_update_data(PyObject* self, PyObject* args);
extern PyObject* bg_update_without_data(PyObject* self, PyObject* args);
extern PyObject* bg_version(PyObject* self, PyObject* args);
/* ---------- bar generate py interfaces end   ---------- */


//////////////////////////////////////////////////////////////////////////
static PyMethodDef MdUtilsModuleMethods[] = {
    { "bg_create",  bg_create,  METH_VARARGS, "" },
    { "bg_release", bg_release, METH_VARARGS, "" },
    { "bg_reset",   bg_reset,   METH_VARARGS, "" },
    { "bg_update_data", bg_update_data, METH_VARARGS, "" },
    { "bg_update_without_data", bg_update_without_data, METH_VARARGS, "" },
    { "bg_set_end_auction", bg_set_end_auction, METH_VARARGS, "" },
    { NULL, NULL, 0, NULL }
};

static struct PyModuleDef md_utils = {
    PyModuleDef_HEAD_INIT,
    "md_utils",
    NULL,
    -1,
    MdUtilsModuleMethods
};


PyMODINIT_FUNC PyInit_md_utils(void)
{
    PyObject* m;
    m = PyModule_Create(&md_utils);
    if (!m) {
        return NULL;
    }
    return m;
}

#endif//HAVE_PY_WRAPPER
