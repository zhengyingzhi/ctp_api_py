#ifndef _MD_UTILS_H_
#define _MD_UTILS_H_


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#if defined(_MSC_VER)
#ifdef MD_UTILS_EXPORTS
#define MD_UTILS_API __declspec(dllexport)
#else
#define MD_UTILS_API __declspec(dllimport)
#endif
#define MD_UTILS_STDCALL    __stdcall   /* ensure stcall calling convention on NT */

#else
#define MD_UTILS_API
#define MD_UTILS_STDCALL                /* leave blank for other systems */

#endif//_MSC_VER


typedef struct
{
    int32_t tm_hour;
    int32_t tm_min;
    int32_t tm_sec;
    int32_t tm_msec;
}sim_time_t;


#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


MD_UTILS_API int MD_UTILS_STDCALL str_to_ptime(sim_time_t* ptm, const char* buf, int len);

MD_UTILS_API int MD_UTILS_STDCALL int_to_ptime(sim_time_t* ptm, int time, int have_millisec);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//_MD_UTILS_H_
