#include <assert.h>
#include <string.h>

#include "cdeque.h"


#define CDEQUE_VERSION              "1.0.0"

#define data_size_align(d,align)    (((d) + (align - 1)) & ~(align - 1))


void MD_UTILS_STDCALL cdeque_init(cdeque_t* dq, int32_t size, int max_capacity, int elemsz)
{
    int capacity;
    memset(dq, 0, sizeof(cdeque_t));

    if (max_capacity < size)
        capacity = data_size_align((size * 2 + 1), 32);
    else
        capacity = max_capacity;

    dq->values = (uint8_t*)malloc(capacity * elemsz);
#ifdef _DEBUG
    memset(dq->values, 0, capacity * elemsz);
#endif//_DEBUG

    dq->pbeg = dq->values;
    dq->pend = dq->values;
    dq->sentinel = dq->pbeg + (capacity * elemsz);

    dq->capacity = capacity;
    dq->size = size;
    dq->elemsz = elemsz;
    dq->static_sz = elemsz * dq->size;
}

void MD_UTILS_STDCALL cdeque_release(cdeque_t* dq)
{
    if (dq->values)
    {
        free(dq->values);
        dq->values = NULL;
    }
}

static data_object_t _cdeque_push_val(cdeque_t* dq, data_object_t val, bool is_int32)
{
    data_object_t oldv;
    if (is_int32)
    {
        int32_t* pdst;
        int32_t* pold;
        pold = (int32_t*)dq->pbeg;
        oldv.i32 = *pold;

        pdst = (int32_t*)dq->pend;
        *pdst = val.i32;
    }
    else
    {
        int64_t* pdst;
        int64_t* pold;
        pold = (int64_t*)dq->pbeg;
        oldv.i64 = *pold;

        pdst = (int64_t*)dq->pend;
        *pdst = val.i64;
    }
    dq->pend += dq->elemsz;

    if (dq->count < dq->size) {
        dq->count += 1;
    }
    else {
        dq->pbeg += dq->elemsz;
    }

    if (dq->pend == dq->sentinel)
    {
        // fprintf(stderr, "pus_value move once!\n");
        memmove(dq->values, dq->pbeg, dq->static_sz);
        dq->pbeg = dq->values;
        dq->pend = dq->values + dq->static_sz;
    }

    return oldv;
}

int MD_UTILS_STDCALL cdeque_push_double(cdeque_t* dq, double val)
{
    data_object_t v;
    v.dv = val;

    v = _cdeque_push_val(dq, v, false);

    // real-time calculate
    dq->sum_value.dv -= v.dv;
    dq->sum_value.dv += val;

    return 0;
}

int MD_UTILS_STDCALL cdeque_push_int64(cdeque_t* dq, int64_t val)
{
    data_object_t v;
    v.i64 = val;

    v = _cdeque_push_val(dq, v, false);

    // real-time calculate
    dq->sum_value.i64 -= v.i64;
    dq->sum_value.i64 += val;

    return 0;
}

int MD_UTILS_STDCALL cdeque_push_int32(cdeque_t* dq, int32_t val)
{
    data_object_t v;
    v.i32 = val;

    v = _cdeque_push_val(dq, v, true);

    // real-time calculate
    dq->sum_value.i32 -= v.i32;
    dq->sum_value.i32 += val;

    return 0;
}

int MD_UTILS_STDCALL cdeque_push_ptr(cdeque_t* dq, void* val)
{
    data_object_t v;
    v.ptr = val;

    v = _cdeque_push_val(dq, v, false);

    return 0;
}

bool MD_UTILS_STDCALL cdeque_is_full(cdeque_t* dq)
{
    return dq->count == dq->size ? true : false;
}

int MD_UTILS_STDCALL cdeque_len(cdeque_t* dq)
{
    return dq->count;
}

void* MD_UTILS_STDCALL cdeque_data(cdeque_t* dq)
{
    return dq->pbeg;
}

void** MD_UTILS_STDCALL cdeque_data_ptr(cdeque_t* dq)
{
    return (void**)dq->pbeg;
}

data_object_t MD_UTILS_STDCALL cdeque_head_value(cdeque_t* dq)
{
    data_object_t val;
    if (dq->elemsz == sizeof(int32_t))
    {
        int32_t* pi = (int32_t*)dq->pbeg;
        val.i32 = *pi;
    }
    else
    {
        int64_t* pi = (int64_t*)dq->pbeg;
        val.i64 = *pi;
    }
    return val;
}

data_object_t MD_UTILS_STDCALL cdeque_tail_value(cdeque_t* dq)
{
    data_object_t val;
    if (dq->elemsz == sizeof(int32_t))
    {
        int32_t* pi = (int32_t*)(dq->pbeg - dq->elemsz);
        val.i32 = *pi;
    }
    else
    {
        int64_t* pi = (int64_t*)(dq->pbeg - dq->elemsz);
        val.i64 = *pi;
    }
    return val;
}

data_object_t MD_UTILS_STDCALL cdeque_sum_value(cdeque_t* dq)
{
    return dq->sum_value;
}


#if 0
static bool _cdeque_max_double_cmp(cdeque_t* dq, data_object_t max_val, data_object_t val)
{
    if (max_val.dv < val.dv)
        return true;
    return false;
}

static bool _cdeque_min_double_cmp(cdeque_t* dq, data_object_t min_val, data_object_t val)
{
    if (min_val.dv > val.dv)
        return true;
    return false;
}

static bool _cdeque_max_int_cmp(cdeque_t* dq, data_object_t max_val, data_object_t val)
{
    if (max_val.i64 > val.i64)
        return true;
    return false;
}

static bool _cdeque_min_int_cmp(cdeque_t* dq, data_object_t min_val, data_object_t val)
{
    if (min_val.i64 < val.i64)
        return true;
    return false;
}
#else
static bool _cdeque_max_double_cmp(cdeque_t* dq, void* val1, void* val2)
{
    (void)dq;
    double* p1 = (double*)val1;
    double* p2 = (double*)val2;
    return *p1 < *p2 ? true : false;
}

static bool _cdeque_min_double_cmp(cdeque_t* dq, void* val1, void* val2)
{
    (void)dq;
    double* p1 = (double*)val1;
    double* p2 = (double*)val2;
    return *p1 > *p2 ? true : false;
}

static bool _cdeque_max_int64_cmp(cdeque_t* dq, void* val1, void* val2)
{
    (void)dq;
    int64_t* p1 = (int64_t*)val1;
    int64_t* p2 = (int64_t*)val2;
    return *p1 < *p2 ? true : false;
}

static bool _cdeque_min_int64_cmp(cdeque_t* dq, void* val1, void* val2)
{
    (void)dq;
    int64_t* p1 = (int64_t*)val1;
    int64_t* p2 = (int64_t*)val2;
    return *p1 > *p2 ? true : false;
}

static bool _cdeque_max_int32_cmp(cdeque_t* dq, void* val1, void* val2)
{
    (void)dq;
    int32_t* p1 = (int32_t*)val1;
    int32_t* p2 = (int32_t*)val2;
    return *p1 < *p2 ? true : false;
}

static bool _cdeque_min_int32_cmp(cdeque_t* dq, void* val1, void* val2)
{
    (void)dq;
    int32_t* p1 = (int32_t*)val1;
    int32_t* p2 = (int32_t*)val2;
    return *p1 > *p2 ? true : false;
}
#endif

int MD_UTILS_STDCALL cdeque_get_data(cdeque_t* dq, void* pdata, CDEQUE_Method meth)
{
    int32_t  index;
    uint8_t* pval;
    uint8_t* pcur;
    uint8_t* pend;

    typedef bool(*cdeque_cmp_pt)(cdeque_t* dq, void* v1, void* v2);
    cdeque_cmp_pt cmp_func;
    switch (meth)
    {
    case DQ_METH_MaxDouble: cmp_func = _cdeque_max_double_cmp; break;
    case DQ_METH_MinDouble: cmp_func = _cdeque_min_double_cmp; break;
    case DQ_METH_MaxInt64:  cmp_func = _cdeque_max_int64_cmp;  break;
    case DQ_METH_MinInt64:  cmp_func = _cdeque_min_int64_cmp;  break;
    case DQ_METH_MaxInt32:  cmp_func = _cdeque_max_int32_cmp;  break;
    case DQ_METH_MinInt32:  cmp_func = _cdeque_min_int32_cmp;  break;
    default:
        return -1;
    }

    pval = dq->pbeg;
    pcur = dq->pbeg;
    pend = dq->pend;
    index = 0;

    int i;
    for (i = 0; pcur < pend; ++i)
    {
        if (cmp_func(dq, pval, pcur))
        {
            pval = pcur;
            index = i;
        }
    }

    if (pdata)
    {
        if (dq->elemsz == sizeof(int32_t)) {
            int32_t* pi = (int32_t*)pcur;
            (*(int32_t*)pdata) = *pi;
        }
        else {
            data_object_t* po = (data_object_t*)pcur;
            (*(data_object_t*)pdata) = *po;
        }
    }
    return index;
}
