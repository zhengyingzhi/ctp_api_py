#ifndef _MD_DICT_H_
#define _MD_DICT_H_


#include <stdbool.h>
#include <stdint.h>

#include "md_utils.h"


#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


#if 0
struct md_dict_value_s
{
    int64_t     update_dt;
    int64_t     volume;
    double      last_price;
};
typedef struct md_dict_value_s md_dict_value_t;
#else
typedef int64_t md_dict_value_t;
#endif//0

typedef struct md_dict_s md_dict_t;


/* creat a md dict, which key is hashid(exchange&instrument)
 */
MD_UTILS_API md_dict_t* MD_UTILS_STDCALL md_dict_create();

MD_UTILS_API int MD_UTILS_STDCALL md_dict_init(md_dict_t* mdd, uint32_t size);

MD_UTILS_API void MD_UTILS_STDCALL md_dict_release(md_dict_t* mdd);

MD_UTILS_API void MD_UTILS_STDCALL md_dict_reset(md_dict_t* mdd);

MD_UTILS_API md_dict_value_t MD_UTILS_STDCALL md_dict_find(md_dict_t* mdd, uint64_t hashid);
MD_UTILS_API md_dict_value_t MD_UTILS_STDCALL md_dict_find2(md_dict_t* mdd, const char* exchange, const char* instruent);

MD_UTILS_API int MD_UTILS_STDCALL md_dict_add(md_dict_t* mdd, uint64_t hashid, md_dict_value_t dt);
MD_UTILS_API int MD_UTILS_STDCALL md_dict_add2(md_dict_t* mdd, uint64_t hashid, int date, int update_time);

// mainly for c programs
MD_UTILS_API int MD_UTILS_STDCALL md_dict_add_ptr(md_dict_t* mdd, uint64_t hashid, void* data_ptr);
MD_UTILS_API void* MD_UTILS_STDCALL md_dict_find_ptr(md_dict_t* mdd, uint64_t hashid);

MD_UTILS_API uint32_t MD_UTILS_STDCALL md_dict_size(md_dict_t* mdd);

MD_UTILS_API void MD_UTILS_STDCALL md_dict_lock(md_dict_t* mdd);

MD_UTILS_API bool MD_UTILS_STDCALL md_dict_trylock(md_dict_t* mdd);

MD_UTILS_API void MD_UTILS_STDCALL md_dict_unlock(md_dict_t* mdd);

MD_UTILS_API uint64_t MD_UTILS_STDCALL md_dict_hash(const char* exchange, const char* instruent, uint32_t len);

MD_UTILS_API const char* MD_UTILS_STDCALL md_dict_version();


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//_MD_DICT_H_
