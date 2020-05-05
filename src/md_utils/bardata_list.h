#ifndef _BARDATA_LIST_H_
#define _BARDATA_LIST_H_


#include <stdbool.h>

#include "bar_generator.h"

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus



struct struct_mem_s
{
    uint32_t    desc;
    uint32_t    count;
    uint32_t    capacity;
    uint32_t    elem_size;
    uint8_t     padding[16];
};
typedef struct struct_mem_s struct_mem_t;

struct bardata_list_s
{
    struct_mem_t*   sm;
    uint8_t*        base_addr;
    bar_data_t**    bar_datas;
    int32_t         index;
    int32_t         alloced;
};
typedef struct bardata_list_s bardata_list_t;


/* init a bar data list
 */
MD_UTILS_API void MD_UTILS_STDCALL bardata_list_init(bardata_list_t* bl, int size, void* addr);

MD_UTILS_API void MD_UTILS_STDCALL bardata_list_release(bardata_list_t* bl);

MD_UTILS_API void MD_UTILS_STDCALL bardata_list_reset(bardata_list_t* bl);

MD_UTILS_API int MD_UTILS_STDCALL bardata_list_push_bar(bardata_list_t* bl,
    const char* exchange, const char* instruent, const char* date, int update_time,
    double open, double high, double low, double close,
    int64_t volume, double turnover, int open_interest);

MD_UTILS_API int MD_UTILS_STDCALL bardata_list_push_bar2(bardata_list_t* bl,
    const bar_data_t* bar);

MD_UTILS_API int MD_UTILS_STDCALL bardata_list_len(bardata_list_t* bl);

MD_UTILS_API bar_data_t* MD_UTILS_STDCALL bardata_list_head(bardata_list_t* bl);
MD_UTILS_API bar_data_t* MD_UTILS_STDCALL bardata_list_tail(bardata_list_t* bl);

MD_UTILS_API bar_data_t* MD_UTILS_STDCALL bardata_list_all(bardata_list_t* bl);

MD_UTILS_API bar_data_t* MD_UTILS_STDCALL bardata_list_prevs(bardata_list_t* bl, int bar_count);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//_BARDATA_LIST_H_
