#ifndef _MD_DICT_H_
#define _MD_DICT_H_


#include <stdbool.h>
#include <stdint.h>

#include "md_utils.h"


#ifdef __cplusplus
extern "C" {
#endif//__cplusplus



struct simple_md_s
{
    int64_t     update_dt;
    int64_t     volume;
    double      last_price;
};
typedef struct simple_md_s simple_md_t;

typedef struct md_dict_s md_dict_t;


/* init a md dict
 */
MD_UTILS_API void MD_UTILS_STDCALL md_dict_init(md_dict_t* mdd, int size);

MD_UTILS_API void MD_UTILS_STDCALL md_dict_release(md_dict_t* mdd);

MD_UTILS_API void MD_UTILS_STDCALL md_dict_reset(md_dict_t* mdd);

MD_UTILS_API int64_t MD_UTILS_STDCALL md_dict_hash(const char* exchange, const char* instruent);
MD_UTILS_API int64_t MD_UTILS_STDCALL md_dict_hash2(int exchange, const char* instruent);
MD_UTILS_API int32_t MD_UTILS_STDCALL md_dict_exchange_id(const char* exchange);

MD_UTILS_API simple_md_t* MD_UTILS_STDCALL md_dict_find(md_dict_t* mdd, int64_t hashid);

MD_UTILS_API simple_md_t* MD_UTILS_STDCALL md_dict_find2(md_dict_t* mdd,
    const char* exchange, const char* instruent);

MD_UTILS_API simple_md_t* MD_UTILS_STDCALL md_dict_find_add(md_dict_t* mdd, int64_t hashid);

MD_UTILS_API simple_md_t* MD_UTILS_STDCALL md_dict_find_add2(md_dict_t* mdd,
    const char* exchange, const char* instruent, const char* date, int update_time);


MD_UTILS_API void MD_UTILS_STDCALL md_dict_lock(md_dict_t* mdd);

MD_UTILS_API bool MD_UTILS_STDCALL md_dict_trylock(md_dict_t* mdd);

MD_UTILS_API void MD_UTILS_STDCALL md_dict_unlock(md_dict_t* mdd);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//_MD_DICT_H_
