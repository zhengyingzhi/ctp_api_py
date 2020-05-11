#include <assert.h>
#include <string.h>

#include <Python.h>

#include "md_dict.h"
#include "md_utils_py.h"


#define MD_DICT_PY_VERSION    "1.0.0"


#if HAVE_PY_WRAPPER

PyObject* mdd_create(PyObject* self, PyObject* args)
{
    (void)self;
    (void)args;
    md_union_dtype_t v;
    md_dict_t* mdd;
    PyObject* res;

    mdd = md_dict_create();
    md_dict_init(mdd, 0);

    v.ptr = mdd;
    res = Py_BuildValue("L", v.i64);
    return res;
}

PyObject* mdd_reset(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    md_dict_t* mdd;

    if (!PyArg_ParseTuple(args, "L", &v.i64)) {
        return Py_BuildValue("i", -1);
    }

    mdd = (md_dict_t*)v.ptr;
    md_dict_reset(mdd);

    return Py_None;
}

PyObject* mdd_release(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    md_dict_t* mdd;

    if (!PyArg_ParseTuple(args, "L", &v.i64)) {
        return Py_BuildValue("i", -1);
    }

    mdd = (md_dict_t*)v.ptr;
    if (mdd) {
        md_dict_release(mdd);
    }

    return Py_BuildValue("i", 0);
}

PyObject* mdd_hash(PyObject* self, PyObject* args)
{
    (void)self;
    char* exchange;
    char* instrument;
    int64_t hashid;

    if (!PyArg_ParseTuple(args, "ss", &exchange, &instrument)) {
        return Py_BuildValue("i", -1);
    }

    hashid = md_dict_hash(exchange, instrument, (uint32_t)strlen(instrument));

    return Py_BuildValue("L", hashid);
}

PyObject* mdd_add(PyObject* self, PyObject* args)
{
    (void)self;
    int rv;
    md_union_dtype_t v;
    md_dict_t* mdd;
    int64_t hashid;
    int64_t dt;

    if (!PyArg_ParseTuple(args, "LLL", &v.i64, &hashid, &dt)) {
        return Py_BuildValue("i", -1);
    }

    mdd = (md_dict_t*)v.ptr;
    if (!mdd) {
        return Py_BuildValue("i", -2);
    }

    rv = md_dict_add(mdd, hashid, dt);

    return Py_BuildValue("i", rv);
}

PyObject* mdd_add2(PyObject* self, PyObject* args)
{
    (void)self;
    int rv;
    md_union_dtype_t v;
    md_dict_t* mdd;
    uint64_t hashid;
    int date;
    int update_time;

    if (!PyArg_ParseTuple(args, "LLii", &v.i64, &hashid, &date, &update_time)) {
        return Py_BuildValue("i", -1);
    }

    mdd = (md_dict_t*)v.ptr;
    if (!mdd) {
        return Py_BuildValue("i", -2);
    }

    rv = md_dict_add2(mdd, hashid, date, update_time);

    return Py_BuildValue("i", rv);
}

PyObject* mdd_find(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    md_dict_t* mdd;
    int64_t hashid;
    md_dict_value_t dt;

    if (!PyArg_ParseTuple(args, "LL", &v.i64, &hashid)) {
        return Py_BuildValue("i", -1);
    }

    mdd = (md_dict_t*)v.ptr;
    if (mdd) {
        dt = md_dict_find(mdd, hashid);
    }
    else {
        dt = -1;
    }

    return Py_BuildValue("L", dt);
}

PyObject* mdd_find2(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    md_dict_t* mdd;
    char* exchange;
    char* instrument;
    md_dict_value_t dt;

    if (!PyArg_ParseTuple(args, "Lss", &v.i64, &exchange, &instrument)) {
        return Py_BuildValue("i", -1);
    }

    mdd = (md_dict_t*)v.ptr;
    if (mdd) {
        dt = md_dict_find2(mdd, exchange, instrument);
    }
    else {
        dt = -1;
    }

    return Py_BuildValue("L", dt);
}

PyObject* mdd_size(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    md_dict_t* mdd;

    if (!PyArg_ParseTuple(args, "L", &v.i64)) {
        return Py_BuildValue("i", -1);
    }

    mdd = (md_dict_t*)v.ptr;
    return Py_BuildValue("i", md_dict_size(mdd));
}

PyObject* mdd_version(PyObject* self, PyObject* args)
{
    (void)self;
    (void)args;
    return Py_BuildValue("s", md_dict_version());
}

#endif//HAVE_PY_WRAPPER
