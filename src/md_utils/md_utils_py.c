#include <assert.h>
#include <string.h>

#include <Python.h>

#include "md_utils.h"
#include "md_utils_py.h"
#include "ztl_base64.h"


#if HAVE_PY_WRAPPER


/* ---------- bar generate py interfaces begin ---------- */
extern PyObject* bg_create(PyObject* self, PyObject* args);
extern PyObject* bg_set_filter_updatetime(PyObject* self, PyObject* args);
extern PyObject* bg_set_end_auction(PyObject* self, PyObject* args);
extern PyObject* bg_reset(PyObject* self, PyObject* args);
extern PyObject* bg_release(PyObject* self, PyObject* args);
extern PyObject* bg_update_data(PyObject* self, PyObject* args);
extern PyObject* bg_update_without_data(PyObject* self, PyObject* args);
extern PyObject* bg_current_bar(PyObject* self, PyObject* args);
extern PyObject* bg_included_data_flag(PyObject* self, PyObject* args);
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
#define BG_SET_FILTER_UPDATETIME_DOC    "whether filter the the none trading update-time like 113001~125959."
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

static PyObject* mu_data_encode(PyObject* self, PyObject* args)
{
    (void)self;
    char* raw_data = NULL;
    if (!PyArg_ParseTuple(args, "s", &raw_data)) {
        return Py_BuildValue("i", -1);
    }

    if (!raw_data || !raw_data[0])
        return Py_BuildValue("i", -1);

    char lbuf[1000] = "";
    strncpy(lbuf, raw_data, sizeof(lbuf) - 1);
    data_change(lbuf);

    char ldst[1000] = "";
    uint32_t llen = sizeof(ldst) - 1;
    ztl_base64_encode(lbuf, (uint32_t)strlen(lbuf), ldst, &llen);

    return Py_BuildValue("s", ldst);
}

static PyObject* mu_data_decode(PyObject* self, PyObject* args)
{
    (void)self;
    char* raw_data = NULL;
    if (!PyArg_ParseTuple(args, "s", &raw_data)) {
        return Py_BuildValue("i", -1);
    }

    if (!raw_data || !raw_data[0])
        return Py_BuildValue("i", -1);

    char lbuf[1000] = "";
    uint32_t llen = sizeof(lbuf) - 1;
    ztl_base64_decode(raw_data, (uint32_t)strlen(raw_data), lbuf, &llen);

    data_change(lbuf);

    return Py_BuildValue("s", lbuf);
}

/* ---------- md utils py interfaces end   ---------- */

#if 0
typedef struct st_test 
{
    int volume;
    int age;
    char name[8];
    void* ptr;
    int flag;
}test_t;


static PyObject* mu_test(PyObject* self, PyObject* args)
{
    test_t* t = (test_t*)malloc(sizeof(test_t));
    memset(t, 0, sizeof(test_t));
    t->volume = 4;
    t->age = 31;
    strcpy(t->name, "DCE");
    t->ptr = t;
    t->flag = 1;
    fprintf(stderr, "test t:%p\n", t);
    PyObject* ret;
    // ret = PyBytes_FromStringAndSize((const char*)t, sizeof(test_t));
    ret = PyByteArray_FromStringAndSize((const char*)t, sizeof(test_t));

    // we can free memory here, or t is local var ok
    free(t);

    return ret;
    // Py_RETURN_NONE;
}

static PyObject* mu_test2(PyObject* self, PyObject* args)
{
    int rv = 0;
    int sz = 0;
    test_t* t = NULL;
    PyObject* o = NULL;
    PyObject* op = NULL;
    fprintf(stderr, "mu_test2 000 self:%p, args:%p\n", self, args);

    rv = PyArg_ParseTuple(args, "|O", &o);  // o is a new PyOjbect*, then t, sz is new as well
    // rv = PyArg_ParseTuple(args, "s#", &t, &sz);  // t is new bytes object, not bytearray type
    if (o) {
        t = PyByteArray_AsString(o);
        sz = PyByteArray_Size(o);
    }
    fprintf(stderr, "rv:%d, test2 o:%p, sz=%d, t:%p\n", rv, o, sz, t);
    if (t) {
        fprintf(stderr, "test2 t v:%d, age:%d, name:%s, ptr:%p\n", t->volume, t->age, t->name, t->ptr);
        // we need decref count for t, sz
    }

    Py_RETURN_NONE;
}
#endif//0

//////////////////////////////////////////////////////////////////////////
static PyMethodDef MdUtilsModuleMethods[] = {
#if 0
    {"test", mu_test, METH_NOARGS, "test"},
    { "test2", mu_test2, METH_VARARGS, "test2" },
#endif//0
    { "mu_version", mu_version,     METH_NOARGS , "md utils module version" },
    { "mu_to_int_time", mu_to_int_time, METH_VARARGS , "convert time str to int" },
    { "mu_data_encode", mu_data_encode, METH_VARARGS , "simple data encode" },
    { "mu_data_decode", mu_data_decode, METH_VARARGS , "simple data decode" },

    { "bg_create",  bg_create,      METH_VARARGS, BG_CREATE_DOC },
    { "bg_release", bg_release,     METH_VARARGS, BG_RELEASE_DOC },
    { "bg_reset",   bg_reset,       METH_VARARGS, BG_RESET_DOC },
    { "bg_update_data",             bg_update_data,         METH_VARARGS, BG_UPDATE_DATA_DOC },
    { "bg_update_without_data",     bg_update_without_data, METH_VARARGS, BG_UPDATE_WITHOUT_DATA_DOC },
    { "bg_current_bar",             bg_current_bar       ,  METH_VARARGS, BG_CURRENT_BAR_DOC },
    { "bg_included_data_flag",      bg_included_data_flag,  METH_VARARGS, "whether the last updated data included in or not" },
    { "bg_set_filter_updatetime",   bg_set_filter_updatetime,   METH_VARARGS, BG_SET_FILTER_UPDATETIME_DOC },
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
