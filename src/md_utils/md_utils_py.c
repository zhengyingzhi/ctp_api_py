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
extern PyObject* bg_current_bar(PyObject* self, PyObject* args);
extern PyObject* bg_version(PyObject* self, PyObject* args);

#define BG_CREATE_DOC           "create bar generator object for one instrument.\n\
exchange:   str the exchange code like SSE/SZSE/CFFEX/SHFE/INE/DCE/CZCE\n\
instrument: str the instrument code like 000001/ru2009/IF2006"
#define BG_RELEASE_DOC          "release the the bar generator object created before"
#define BG_RESET_DOC            "reset the the bar generator object created before"
#define BG_UPDATE_DATA_DOC      "update with new md data to try generate a new bar.\n\
date:       str the action day\n\
update_time:int the last update time like 134538\n\
last_price: double the last price\n\
volume:     int the tick volume\n\
turnover:   double the tick turnover\n\
open_interest: int the tick open_interest"
#define BG_UPDATE_WITHOUT_DATA_DOC  "update without new md data to try generate a new bar.\n\
exchange:   str the exchange code like SSE/SZSE/CFFEX/SHFE/INE/DCE/CZCE\n\
instrument: str the instrument code like 000001/ru2009/IF2006\n\
date:       str the action day\n\
update_time:int the last update time like 134520\n"
#define BG_CURRENT_BAR_DOC      "return the current bar data (maybe not finished yet)"
#define BG_SET_END_AUCTION_DOC  "the end auction flag for the instrument (mainly for SSE).\n\
end_auction_flag: int on or off"
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
#define MDD_HASH_DOC            "get hash id by exchange&instrument"
#define MDD_VERSION_DOC         "the md dict module version"
/* ---------- md dict py interfaces end   ---------- */


/* ---------- slq reader py interfaces begin ---------- */
// for Samadhi, no use at other where
PyObject* slqr_create(PyObject* self, PyObject* args);
PyObject* slqr_release(PyObject* self, PyObject* args);
PyObject* slqr_next(PyObject* self, PyObject* args);
PyObject* slqr_count(PyObject* self, PyObject* args);
/* ---------- slq reader py interfaces end   ---------- */

/* ---------- md utils py interfaces begin ---------- */
static PyObject* mu_version(PyObject* self, PyObject* args)
{
    (void)self;
    (void)args;
    return Py_BuildValue("s", md_utils_version());
}

static PyObject* mu_to_int_time(PyObject* self, PyObject* args)
{
    (void)self;
    char* time_str = NULL;
    if (!PyArg_ParseTuple(args, "s", &time_str)) {
        return Py_BuildValue("i", -1);
    }

    if (!time_str || !time_str[0])
        return Py_BuildValue("i", 0);
    return Py_BuildValue("i", to_int_time(time_str));
}

/* ---------- md utils py interfaces end   ---------- */

//////////////////////////////////////////////////////////////////////////
static PyMethodDef MdUtilsModuleMethods[] = {
    { "mu_version", mu_version,     METH_NOARGS , "md utils module version" },
    { "mu_to_int_time", mu_to_int_time, METH_VARARGS , "convert time str to int" },

    { "bg_create",  bg_create,      METH_VARARGS, BG_CREATE_DOC },
    { "bg_release", bg_release,     METH_VARARGS, BG_RELEASE_DOC },
    { "bg_reset",   bg_reset,       METH_VARARGS, BG_RESET_DOC },
    { "bg_update_data",             bg_update_data,         METH_VARARGS, BG_UPDATE_DATA_DOC },
    { "bg_update_without_data",     bg_update_without_data, METH_VARARGS, BG_UPDATE_WITHOUT_DATA_DOC },
    { "bg_current_bar",             bg_current_bar       ,  METH_VARARGS, BG_CURRENT_BAR_DOC },
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

    { "slqr_create", slqr_create,   METH_VARARGS, "" },
    { "slqr_release",slqr_release,  METH_VARARGS, "" },
    { "slqr_next",   slqr_next,     METH_VARARGS, "" },
    { "slqr_count",  slqr_count,    METH_VARARGS, "" },
    { NULL, NULL, 0, NULL }
};

#define MD_UTILS_DOC    "mu utils implemented by Cpython,\n\
    - md_xxx:     some come utils functions,\n\
    - bg_xxx:     bar generator methods,\n\
    - mdd_xxx:    md dict methods, key is hashid(exchange&instrument)\
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
