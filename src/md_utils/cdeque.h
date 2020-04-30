#ifndef _CDEQUE_H_INCLUDED_
#define _CDEQUE_H_INCLUDED_


#include <stdbool.h>
#include "md_utils.h"

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus



typedef union 
{
    int64_t i64;
    void*   ptr;
    double  dv;
    int32_t i32;
}data_object_t;

struct cdeque_s
{
    uint8_t*        values;
    uint8_t*        pbeg;
    uint8_t*        pend;
    uint8_t*        sentinel;
    data_object_t   sum_value;
    int32_t         beg_index;
    int32_t         end_index;
    int32_t         count;
    int32_t         elemsz;
    int32_t         static_sz;
    int32_t         size;
    int32_t         capacity;
    void*           udata;
};

typedef struct cdeque_s cdeque_t;


/* init a deque series,
 * size: your values keep count
 * max_capacity: max capacity for internal array, pass 0 if do not know how to pass-in this parameter
 */
MD_UTILS_API void MD_UTILS_STDCALL cdeque_init(cdeque_t* dq, int32_t size, int max_capacity, int elemsz);

MD_UTILS_API void MD_UTILS_STDCALL cdeque_release(cdeque_t* dq);

MD_UTILS_API int MD_UTILS_STDCALL cdeque_push_double(cdeque_t* dq, double val);
MD_UTILS_API int MD_UTILS_STDCALL cdeque_push_int64(cdeque_t* dq, int64_t val);
MD_UTILS_API int MD_UTILS_STDCALL cdeque_push_int32(cdeque_t* dq, int32_t val);
MD_UTILS_API int MD_UTILS_STDCALL cdeque_push_ptr(cdeque_t* dq, void* val);

MD_UTILS_API bool MD_UTILS_STDCALL cdeque_is_full(cdeque_t* dq);
MD_UTILS_API int  MD_UTILS_STDCALL cdeque_len(cdeque_t* dq);

MD_UTILS_API void*  MD_UTILS_STDCALL cdeque_data(cdeque_t* dq);
MD_UTILS_API void** MD_UTILS_STDCALL cdeque_data_ptr(cdeque_t* dq);

MD_UTILS_API data_object_t MD_UTILS_STDCALL cdeque_head_value(cdeque_t* dq);
MD_UTILS_API data_object_t MD_UTILS_STDCALL cdeque_tail_value(cdeque_t* dq);
MD_UTILS_API data_object_t MD_UTILS_STDCALL cdeque_sum_value(cdeque_t* dq);


typedef enum 
{
    DQ_METH_MaxDouble,
    DQ_METH_MinDouble,
    DQ_METH_MaxInt64,
    DQ_METH_MinInt64,
    DQ_METH_MaxInt32,
    DQ_METH_MinInt32
}CDEQUE_Method;
MD_UTILS_API int MD_UTILS_STDCALL cdeque_get_data(cdeque_t* dq, void* pval, CDEQUE_Method meth);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//_CDEQUE_H_INCLUDED_
