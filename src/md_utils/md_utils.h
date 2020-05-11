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


#define MD_EXCHANGE_SSE     "SSE"
#define MD_EXCHANGE_SZSE    "SZSE"
#define MD_EXCHANGE_DCE     "DCE"
#define MD_EXCHANGE_CZCE    "CZCE"
#define MD_EXCHANGE_SHFE    "SHFE"
#define MD_EXCHANGE_INE     "INE"
#define MD_EXCHANGE_CFFEX   "CFFEX"

/// 市场代码编号定义 Market ID
#define MD_MI_NONE                  0           //None
#define MD_MI_SSE                   1           //上海市场
#define MD_MI_SZSE                  2           //深圳市场
#define MD_MI_SHOP                  3           //上海期权市场
#define MD_MI_SZOP                  4           //深圳期权市场
#define MD_MI_SHHK                  5           //沪港通
#define MD_MI_SZHK                  6           //深港通
#define MD_MI_NEEQ                  7           //新三板
#define MD_MI_CFFEX                 8           //中金所
#define MD_MI_SHFE                  9           //上期所
#define MD_MI_DCE                   10          //大商所
#define MD_MI_CZCE                  11          //郑商所
#define MD_MI_INE                   12          //上海能源中心
#define MD_MI_SGE                   13          //上海金交所市场


typedef union
{
    int64_t i64;
    double  f64;
    void*   ptr;
    float   f32;
    int     i32;
}md_union_dtype_t;


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


MD_UTILS_API const char* MD_UTILS_STDCALL md_utils_version();

MD_UTILS_API int MD_UTILS_STDCALL str_to_ptime(sim_time_t* ptm, const char* buf, int len);

MD_UTILS_API int MD_UTILS_STDCALL int_to_ptime(sim_time_t* ptm, int time, int have_millisec);

/* convert CZCE exchange's instrument
 * like: CF008 -> CF2008
 * or  : CF2008 -> CF008
 * cur_year_month: 202003 (without month day)
 * return: result length
 */
MD_UTILS_API int MD_UTILS_STDCALL conv_czce_instrument(char instrument[], int cur_year_month);

MD_UTILS_API int MD_UTILS_STDCALL make_sure_symbol_spec(char instrument[], const char* exchange);

MD_UTILS_API int MD_UTILS_STDCALL get_product_code(char code[], const char* instrument);

MD_UTILS_API const char* MD_UTILS_STDCALL lookup_exchange(const char* instrument);

MD_UTILS_API int MD_UTILS_STDCALL make_symbol(char symbol[], const char* instrument, const char* exchange);

MD_UTILS_API int MD_UTILS_STDCALL extract_symbol(char instrument[], char exchange[], const char* symbol);

MD_UTILS_API int MD_UTILS_STDCALL is_stock(const char* instrument);

MD_UTILS_API int MD_UTILS_STDCALL convert_market_id(const char* exchange);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//_MD_UTILS_H_
