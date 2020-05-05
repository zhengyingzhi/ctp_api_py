#include <assert.h>
#include <string.h>


#include "md_dict.h"


#define MD_DICT_VERSION     "1.0.0"


struct md_dict_s
{
    void*       dct;
    uint32_t    count;
};


static uint64_t _int64DictHashFunc(const void* key)
{
    union_dtype_t data;
    return data.i64;
}

static int _intCompareFunc(void* privdata,
    const void* key1, const void* key2)
{
    uint64_t k1 = (uint64_t)key1;
    uint64_t k2 = (uint64_t)key2;
    return k1 == k2;
}

#if 0
static dictType _int64DictType = {
    _int64DictHashFunc,
    NULL,
    NULL,
    _intCompareFunc,
    NULL,
    NULL
};
#endif

//////////////////////////////////////////////////////////////////////////
int64_t MD_UTILS_STDCALL md_dict_instr_hash(const char* exchange, const char* instruent)
{
    return 0;
}

int64_t MD_UTILS_STDCALL md_dict_instr_hash2(int exchange, const char* instruent)
{
    return 0;
}

int32_t MD_UTILS_STDCALL md_dict_exchange_id(const char* exchange)
{
    if (strcmp(exchange, MD_EXCHANGE_SSE) == 0) {
        return 1;
    }
    else if (strcmp(exchange, MD_EXCHANGE_SZSE) == 0) {
        return 2;
    }
    return 0;
}

void MD_UTILS_STDCALL md_dict_init(md_dict_t* mdd, int size)
{
    // mdd->dct = dictCreate(&_int64DictType, mdd);
}

void MD_UTILS_STDCALL md_dict_release(md_dict_t* mdd)
{}

void MD_UTILS_STDCALL md_dict_reset(md_dict_t* mdd)
{}

simple_md_t* MD_UTILS_STDCALL md_dict_find(md_dict_t* mdd, int64_t hashid)
{
#if 0
    dictEntry* entry;
    union_dtype_t data;
    data.i64 = hashid;
    entry = dictFind(mdd->dct, data.ptr);
    if (entry) {
        return entry->v.val;
    }
#endif
    return NULL;
}

simple_md_t* MD_UTILS_STDCALL md_dict_find2(md_dict_t* mdd,
    const char* exchange, const char* instruent)
{
    return NULL;
}

simple_md_t* MD_UTILS_STDCALL md_dict_find_add(md_dict_t* mdd, int64_t hashid)
{
    return NULL;
}

simple_md_t* MD_UTILS_STDCALL md_dict_find_add2(md_dict_t* mdd,
    const char* exchange, const char* instruent, const char* date, int update_time)
{
    return NULL;
}

void MD_UTILS_STDCALL md_dict_lock(md_dict_t* mdd)
{}

bool MD_UTILS_STDCALL md_dict_trylock(md_dict_t* mdd)
{
    return true;
}

void MD_UTILS_STDCALL md_dict_unlock(md_dict_t* mdd)
{}
