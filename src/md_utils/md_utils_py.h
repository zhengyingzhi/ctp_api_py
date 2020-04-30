#ifndef _MD_UTILS_PY_H_
#define _MD_UTILS_PY_H_


#include <stdint.h>


#ifndef _DEBUG
#define HAVE_PY_WRAPPER     1
#endif//_DEBUG

typedef union
{
    int64_t i64;
    void*   ptr;
}union_dtype_t;


#endif//_MD_UTILS_PY_H_
