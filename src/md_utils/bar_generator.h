#ifndef _BAR_GENERATOR_H_
#define _BAR_GENERATOR_H_


#include "md_utils.h"

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


#define BG_PAUSE_TIMES_SIZE     10
#define BG_INVAL_PAUSE_TIME     -1

#define BG_ENABLE_CTP_MD        1

#if _DEBUG
#define BG_BAR_DEBUG            1
#else
#define BG_BAR_DEBUG            0
#endif//_DEBUG


typedef struct field_names_s
{
    const char* InstrumentID;
    const char* Exchange;
    const char* Open;
    const char* High;
    const char* Low;
    const char* Close;
    const char* Volume;
    const char* Turnover;
    const char* OpenInterest;
    const char* Date;
    const char* Time;
    const char* Period;

    const char* DateTime;
    const char* AdjustFactor;
}field_names_t;


typedef struct 
{
    char    InstrumentID[32];
    char    ExchangeID[8];
    double  Open;
    double  High;
    double  Low;
    double  Close;
    int64_t Volume;
    double  Turnover;
    int32_t OpenInterest;
    char    Date[12];
    char    Time[12];
    char    Period[4];

#if BG_BAR_DEBUG
    // below just for easy debug
    int32_t StartTime;
    int32_t EndTime;
#endif//BG_BAR_DEBUG
}bar_data_t;

struct bar_generator_s
{
    char        instrument[16];
    char        code[8];
    uint64_t    hash_code;
    uint32_t    tick_count;
    uint32_t    bar_count;
    int32_t     generated;
    int32_t     first_update_flag;
    int32_t     have_end_auction;

    int64_t     begin_volume;
    double      begin_turnover;
    int32_t     begin_open_interest;
    int32_t     begin_update_time;

    int64_t     prev_volume;
    double      prev_turnover;
    int32_t     prev_open_interest;
    int32_t     prev_update_time;
    sim_time_t  prev_tm;

    double      cur_high;
    double      cur_low;

    bar_data_t  cur_bar;
    bar_data_t  fin_bar;

    // 
    int32_t     pause_times[BG_PAUSE_TIMES_SIZE];
    int32_t     pack;

    bool      (*filter_data_by_updatetime)(const char* instrument, int update_time);
    void*       udata;
};

typedef struct bar_generator_s bar_generator_t;


/* init a bar generator,
 * code: the product code, like a,rb,IF,CF,000001,600004
 */
MD_UTILS_API int MD_UTILS_STDCALL bar_generator_init(bar_generator_t* bargen,
    const char* exchange, const char* instrument);

/* set the new product code pause times,
 * pause_times: [230000, 101500, 113000, 150000, BG_INVAL_PAUSE_TIME]
 * size: the pause_times array size
 */
MD_UTILS_API void MD_UTILS_STDCALL bar_generator_set_pause_times(bar_generator_t* bargen, int pause_times[], int size);

/* set have end acution for this instrument or not, default have for cn stocks
 */
MD_UTILS_API void MD_UTILS_STDCALL bar_generator_set_end_auction(bar_generator_t* bargen, int end_auction_flag);

/* try generate a new bar by the new updated tick data,
 * return: the bar data if generated
 */
MD_UTILS_API bar_data_t* MD_UTILS_STDCALL bar_generator_update_data(
    bar_generator_t* bargen, const char* date, int update_time,
    double last_price, int64_t volume, double turnover, int open_interest);

/* try generate a new bar by the existed updated tick data,
 * return: the bar data (maybe not a finished bar)
 */
MD_UTILS_API bar_data_t* MD_UTILS_STDCALL bar_generator_update_without_data(
    bar_generator_t* bargen, const char* date, int update_time);

/* try return the current bar data
 */
MD_UTILS_API bar_data_t* MD_UTILS_STDCALL bar_generator_current_bar(bar_generator_t* bargen);

#if BG_ENABLE_CTP_MD
/* try generate a new bar by the CTP data (struct CThostFtdcDepthMarketDataField),
 * return: the bar data if generated
 */
MD_UTILS_API bar_data_t* MD_UTILS_STDCALL bar_generator_update_by_ctp(
    bar_generator_t* bargen, void* apmd, int size);
#endif//BG_ENABLE_CTP_MD

/* reset the bar generator, such as the trading day changed
 */
MD_UTILS_API void MD_UTILS_STDCALL bar_generator_reset(bar_generator_t* bargen);

/* get the bar generator version like "1.0.1"
 */
MD_UTILS_API const char* MD_UTILS_STDCALL bar_generator_version();


//////////////////////////////////////////////////////////////////////////
/* format the bar data into a json buffer
 */
MD_UTILS_API int MD_UTILS_STDCALL bar_data_as_json(
    char json_buffer[], int size,
    const bar_data_t* bar,
    const field_names_t* fieldnames,
    int indent);

/* set the default json names
 */
MD_UTILS_API void MD_UTILS_STDCALL bar_data_fieldnames_init(field_names_t* fieldnames);



#ifdef __cplusplus
}
#endif//__cplusplus

#endif//_BAR_GENERATOR_H_
