#include <assert.h>
#include <string.h>

#include <Python.h>

#include "bar_generator.h"
#include "md_utils_py.h"


#define BAR_GENERATOR_PY_VERSION    "1.0.1"

#if HAVE_PY_WRAPPER

static PyObject* _bg_convert_bar(bar_data_t* bar)
{
    PyObject* dp;
    dp = PyDict_New();
    PyDict_SetItemString(dp, "instrument", Py_BuildValue("s", bar->InstrumentID));
    PyDict_SetItemString(dp, "exchange", Py_BuildValue("s", bar->ExchangeID));
    PyDict_SetItemString(dp, "date", Py_BuildValue("s", bar->Date));
    PyDict_SetItemString(dp, "time", Py_BuildValue("s", bar->Time));
    PyDict_SetItemString(dp, "open", Py_BuildValue("f", bar->Open));
    PyDict_SetItemString(dp, "high", Py_BuildValue("f", bar->High));
    PyDict_SetItemString(dp, "low", Py_BuildValue("f", bar->Low));
    PyDict_SetItemString(dp, "close", Py_BuildValue("f", bar->Close));
    PyDict_SetItemString(dp, "volume", Py_BuildValue("L", bar->Volume));
    PyDict_SetItemString(dp, "turnover", Py_BuildValue("f", bar->Turnover));
    PyDict_SetItemString(dp, "open_interest", Py_BuildValue("i", bar->OpenInterest));
#if BG_BAR_DEBUG
    PyDict_SetItemString(dp, "start_time", Py_BuildValue("i", bar->StartTime));
    PyDict_SetItemString(dp, "end_time", Py_BuildValue("i", bar->EndTime));
#endif//BG_BAR_DEBUG

    return dp;
}

PyObject* bg_create(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    bar_generator_t* bargen;
    const char* exchange;
    const char* instrument;
    PyObject* res;

    if (!PyArg_ParseTuple(args, "ss", &exchange, &instrument)) {
        Py_RETURN_NONE;
    }

    // fprintf(stderr, "bg_create %s,%s\n", exchange, instrument);

    bargen = (bar_generator_t*)malloc(sizeof(bar_generator_t));
    bar_generator_init(bargen, exchange, instrument);
    // fprintf(stderr, "bg_create %p\n", bargen);

    v.ptr = bargen;
    res = Py_BuildValue("L", v.i64);
    return res;
}

PyObject* bg_set_end_auction(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    bar_generator_t* bargen;
    int have_end_auction;

    if (!PyArg_ParseTuple(args, "Li", &v.i64, &have_end_auction)) {
        return Py_BuildValue("i", -1);
    }

    bargen = (bar_generator_t*)v.ptr;
    if (!bargen) {
        return Py_BuildValue("i", -2);
    }

    bar_generator_set_end_auction(bargen, have_end_auction);
    return Py_BuildValue("i", 0);
}

PyObject* bg_reset(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    bar_generator_t* bargen;

    if (!PyArg_ParseTuple(args, "L", &v.i64)) {
        return Py_BuildValue("i", -1);
    }

    bargen = (bar_generator_t*)v.ptr;
    if (bargen) {
        bar_generator_reset(bargen);
    }

    Py_RETURN_NONE;
}

PyObject* bg_release(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    bar_generator_t* bargen;

    if (!PyArg_ParseTuple(args, "L", &v.i64)) {
        return Py_BuildValue("i", -1);
    }

    bargen = (bar_generator_t*)v.ptr;
    // fprintf(stderr, "bg_release obj:%p\n", bargen);
    if (bargen) {
        free(bargen);
    }

    return Py_BuildValue("i", 0);
}

PyObject* bg_update_data(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    bar_generator_t* bargen;
    bar_data_t* bar;
    const char* date;
    int update_time;
    double last_price;
    double turnover;
    int64_t volume;
    int32_t open_interest;

    if (!PyArg_ParseTuple(args, "LsidLdi", &v.i64, &date, &update_time, &last_price, &volume, &turnover, &open_interest)) {
        return Py_BuildValue("i", -1);
    }
    bargen = (bar_generator_t*)v.ptr;
    // fprintf(stderr, "bg_update_data %p %s,%d,%.2lf,%ld,%.2lf\n", bargen, date, update_time, last_price, (long)volume, turnover);

    if (!bargen) {
        return Py_BuildValue("i", -2);
    }

    bar = bar_generator_update_data(bargen, date, update_time, last_price, volume, turnover, open_interest);
    if (bar)
    {
        PyObject* dp;
        dp = _bg_convert_bar(bar);
        return dp;
    }

    Py_RETURN_NONE;
    // return Py_BuildValue("i", 0);
}

PyObject* bg_update_without_data(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    bar_generator_t* bargen;
    bar_data_t* bar;
    const char* date;
    int update_time;

    if (!PyArg_ParseTuple(args, "Lsi", &v.i64, &date, &update_time)) {
        return Py_BuildValue("i", -1);
    }
    bargen = (bar_generator_t*)v.ptr;
    // fprintf(stderr, "bg_update_without_data %p %s,%d\n", bargen, date, update_time);

    if (!bargen) {
        return Py_BuildValue("i", -2);
    }

    bar = bar_generator_update_without_data(bargen, date, update_time);
    if (bar)
    {
        PyObject* dp;
        dp = _bg_convert_bar(bar);
        return dp;
    }

    Py_RETURN_NONE;
    // return Py_BuildValue("i", 0);
}

PyObject* bg_current_bar(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    bar_generator_t* bargen;
    bar_data_t* bar;

    if (!PyArg_ParseTuple(args, "L", &v.i64)) {
        return Py_BuildValue("i", -1);
    }
    bargen = (bar_generator_t*)v.ptr;

    if (!bargen) {
        return Py_BuildValue("i", -2);
    }

    bar = bar_generator_current_bar(bargen);
    if (bar)
    {
        PyObject* dp;
        dp = _bg_convert_bar(bar);
        return dp;
    }

    Py_RETURN_NONE;
    // return Py_BuildValue("i", 0);
}

PyObject* bg_included_data_flag(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    bar_generator_t* bargen;

    if (!PyArg_ParseTuple(args, "L", &v.i64)) {
        return Py_BuildValue("i", -1);
    }
    bargen = (bar_generator_t*)v.ptr;

    if (!bargen) {
        return Py_BuildValue("i", -2);
    }

    return Py_BuildValue("i", bargen->included_cur_data_flag);
}

PyObject* bg_version(PyObject* self, PyObject* args)
{
    (void)self;
    (void)args;
    return Py_BuildValue("s", bar_generator_version());
}

#endif//HAVE_PY_WRAPPER
