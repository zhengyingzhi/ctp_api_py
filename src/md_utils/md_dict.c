#include <assert.h>
#include <string.h>

#ifdef _MSC_VER
#include <Windows.h>
#endif//_MSC_VER

#include <ZToolLib/ztl_atomic.h>
#include <ZToolLib/ztl_locks.h>
#include <ZToolLib/ztl_dict.h>
#include "md_dict.h"
#include "md_code_hash.h"


#define MD_DICT_VERSION     "1.0.1"


struct md_dict_s
{
    dict*       dct;
    uint32_t    count;
    volatile uint32_t lock;
};


static uint64_t _int64DictHashFunc(const void* key)
{
    md_union_dtype_t ud;
    ud.ptr = (void*)key;
    return ud.i64;
}

static int _intCompareFunc(void* privdata,
    const void* key1, const void* key2)
{
    (void)privdata;
    uint64_t k1 = (uint64_t)key1;
    uint64_t k2 = (uint64_t)key2;
    return k1 == k2;
}

static dictType _int64DictType = {
    _int64DictHashFunc,
    NULL,
    NULL,
    _intCompareFunc,
    NULL,
    NULL
};

//////////////////////////////////////////////////////////////////////////
static dictEntry* md_dict_find_raw(md_dict_t* mdd, uint64_t hashid)
{
    dictEntry* entry;
    md_union_dtype_t data;
    data.i64 = hashid;
    entry = dictFind(mdd->dct, data.ptr);
    return entry;
}

md_dict_t* MD_UTILS_STDCALL md_dict_create()
{
    md_dict_t* mdd;
    mdd = (md_dict_t*)malloc(sizeof(md_dict_t));
    memset(mdd, 0, sizeof(md_dict_t));
    return mdd;
}

int MD_UTILS_STDCALL md_dict_init(md_dict_t* mdd, uint32_t size)
{
    if (!mdd->dct) {
        mdd->dct = dictCreate(&_int64DictType, mdd);
    }

    if (size > DICT_HT_INITIAL_SIZE) {
        dictExpand(mdd->dct, size);
    }

    return 0;
}

void MD_UTILS_STDCALL md_dict_release(md_dict_t* mdd)
{
    if (mdd->dct) {
        dictRelease(mdd->dct);
        mdd->dct = NULL;
    }

    free(mdd);
}

void MD_UTILS_STDCALL md_dict_reset(md_dict_t* mdd)
{
    if (mdd->dct) {
        dictEmpty(mdd->dct, NULL);
    }
    mdd->count = 0;
}

int64_t MD_UTILS_STDCALL md_dict_find(md_dict_t* mdd, uint64_t hashid)
{
    dictEntry* entry;
    entry = md_dict_find_raw(mdd, hashid);
    if (entry) {
        return entry->v.s64;
    }
    return 0;
}

int64_t MD_UTILS_STDCALL md_dict_find2(md_dict_t* mdd,
    const char* exchange, const char* instruent)
{
    uint64_t hashid;
    hashid = md_dict_hash(exchange, instruent, (uint32_t)strlen(instruent));

    return md_dict_find(mdd, hashid);
}

int MD_UTILS_STDCALL md_dict_add(md_dict_t* mdd, uint64_t hashid, md_dict_value_t dt)
{
    int rv;
    md_union_dtype_t uk, ud;
    uk.i64 = hashid;
    ud.i64 = dt;
    if (!hashid) {
        return -1;
    }

    rv = dictAdd(mdd->dct, uk.ptr, ud.ptr);
    if (rv == ZTL_DICT_OK) {
        mdd->count += 1;
    }
    return rv;
}

int MD_UTILS_STDCALL md_dict_add2(md_dict_t* mdd, uint64_t hashid, int date, int update_time)
{
    md_dict_value_t dt;
    dt = date * 1000000 + update_time;

    return md_dict_add(mdd, hashid, dt);
}

int MD_UTILS_STDCALL md_dict_add_ptr(md_dict_t* mdd, uint64_t hashid, void* data_ptr)
{
    md_union_dtype_t data;
    data.i64 = data_ptr;
    md_dict_value_t val = data.i64;
    return md_dict_add(mdd, hashid, val);
}

void* MD_UTILS_STDCALL md_dict_find_ptr(md_dict_t* mdd, uint64_t hashid)
{
    md_union_dtype_t data;
    md_dict_value_t val;
    val = md_dict_find(mdd, hashid);
    data.i64 = val;
    return data.ptr;
}

uint32_t MD_UTILS_STDCALL md_dict_size(md_dict_t* mdd)
{
    return mdd->count;
}


void MD_UTILS_STDCALL md_dict_lock(md_dict_t* mdd)
{
    ztl_spinlock(&mdd->lock, 1, ZTL_LOCK_SPIN);
}

bool MD_UTILS_STDCALL md_dict_trylock(md_dict_t* mdd)
{
    return ztl_trylock(&mdd->lock, 1);
}

void MD_UTILS_STDCALL md_dict_unlock(md_dict_t* mdd)
{
    ztl_unlock(&mdd->lock);
}


uint64_t MD_UTILS_STDCALL md_dict_hash(const char* exchange, const char* instruent, uint32_t len)
{
    uint64_t hashid;
    if (!md_code_get_hashid(&hashid, exchange, instruent, len)) {
        return 0;
    }
    return hashid;
}

const char* MD_UTILS_STDCALL md_dict_version()
{
    return MD_DICT_VERSION;
}
