#include <assert.h>
#include <string.h>

#include <Python.h>

#include "md_utils.h"
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

#define BG_CREATE_DOC           "create bar generator object for one instrument"
#define BG_RELEASE_DOC          "release the the bar generator object created before"
#define BG_RESET_DOC            "reset the the bar generator object created before"
#define BG_UPDATE_DATA_DOC      "update with new md data to try generate a new bar.\n\
exchange:   str the exchange code like SSE/SZSE/CFFEX/SHFE/INE/DCE/CZCE\n\
instrument: str the instrument code like 000001/ru2009/IF2006\n\
last_price: double the last price\n\
update_time: int the last update time like 134520"
#define BG_UPDATE_WITHOUT_DATA_DOC  "update without new md data to try generate a new bar"
#define BG_SET_END_AUCTION_DOC  "the end auction flag for the instrument (mainly for SSE)"
#define BG_VERSION_DOC          "the bar generator module version"
/* ---------- bar generate py interfaces end   ---------- */


/* ---------- md dict py interfaces begin ---------- */
extern PyObject* mdd_create(PyObject* self, PyObject* args);
extern PyObject* mdd_reset(PyObject* self, PyObject* args);
extern PyObject* mdd_release(PyObject* self, PyObject* args);
extern PyObject* mdd_hash(PyObject* self, PyObject* args);
extern PyObject* mdd_add(PyObject* self, PyObject* args);
extern PyObject* mdd_add2(PyObject* self, PyObject* args);
extern PyObject* mdd_find(PyObject* self, PyObject* args);
extern PyObject* mdd_find2(PyObject* self, PyObject* args);
extern PyObject* mdd_size(PyObject* self, PyObject* args);
extern PyObject* mdd_version(PyObject* self, PyObject* args);

#define MDD_CREATE_DOC          "create md dict object"
#define MDD_ADD_DOC             "add the current key(hashid) & value(udpate_dt) into the dict"
#define MDD_ADD2_DOC            "add the current key(hashid) & value(date&update_time) into the dict"
#define MDD_FIND_DOC            "find value by key(hashid) from the dict"
#define MDD_FIND2_DOC           "find value by key(exchange&instrument) from the dict"
#define MDD_SIZE_DOC            "get current count in the dict"
#define MDD_VERSION_DOC         "the md dict module version"
/* ---------- md dict py interfaces end   ---------- */


/* ---------- md utils py interfaces begin ---------- */
static PyObject* mu_version(PyObject* self, PyObject* args)
{
    (void)self;
    (void)args;
    return Py_BuildValue("s", md_utils_version());
}


#define MDD_CREATE_DOC          "create md dict object"
#define MDD_ADD_DOC             "add the current key(hashid) & value(udpate_dt) into the dict"
#define MDD_ADD2_DOC            "add the current key(hashid) & value(date&update_time) into the dict"
#define MDD_FIND_DOC            "find value by key(hashid) from the dict"
#define MDD_FIND2_DOC           "find value by key(exchange&instrument) from the dict"
#define MDD_SIZE_DOC            "get current count in the dict"
#define MDD_HASH_DOC            "get the hashid by exchange&instrument (no need pass-in mdd object)"
#define MDD_VERSION_DOC         "the md dict module version"
/* ---------- md utils py interfaces end   ---------- */

//////////////////////////////////////////////////////////////////////////
static PyMethodDef MdUtilsModuleMethods[] = {
    { "mu_version", mu_version,     METH_NOARGS , "md utils module version" },
    { "bg_create",  bg_create,      METH_VARARGS, BG_CREATE_DOC },
    { "bg_release", bg_release,     METH_VARARGS, BG_RELEASE_DOC },
    { "bg_reset",   bg_reset,       METH_VARARGS, BG_RESET_DOC },
    { "bg_update_data",             bg_update_data,         METH_VARARGS, BG_UPDATE_DATA_DOC },
    { "bg_update_without_data",     bg_update_without_data, METH_VARARGS, BG_UPDATE_WITHOUT_DATA_DOC },
    { "bg_set_end_auction",         bg_set_end_auction,     METH_VARARGS, BG_SET_END_AUCTION_DOC },
    { "bg_version", bg_version,     METH_NOARGS,  BG_VERSION_DOC },

    { "mdd_create", mdd_create,     METH_VARARGS, MDD_CREATE_DOC },
    { "mdd_reset",  mdd_reset,      METH_VARARGS, "" },
    { "mdd_release",mdd_release,    METH_VARARGS, "" },
    { "mdd_add",    mdd_add,        METH_VARARGS, MDD_ADD_DOC },
    { "mdd_add2",   mdd_add2,       METH_VARARGS, MDD_ADD2_DOC },
    { "mdd_find",   mdd_find,       METH_VARARGS, MDD_FIND_DOC },
    { "mdd_find2",  mdd_find2,      METH_VARARGS, MDD_FIND2_DOC },
    { "mdd_size",   mdd_size,       METH_VARARGS, MDD_SIZE_DOC },
    { "mdd_hash",   mdd_hash,       METH_VARARGS, MDD_HASH_DOC },
    { "mdd_version",mdd_version,    METH_NOARGS,  MDD_VERSION_DOC },
    { NULL, NULL, 0, NULL }
};

#define MD_UTILS_DOC    "mu utils implemented by C,\n                          \
    - bg_xxx:     bar generator methods,\n                                     \
    - mdd_xxx:    md dict methods, key is hashid(exchange&instrument)          \
"

static struct PyModuleDef md_utils = {
    PyModuleDef_HEAD_INIT,
    "md_utils",
    MD_UTILS_DOC,
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
