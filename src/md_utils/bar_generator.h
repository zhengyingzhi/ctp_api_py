#ifndef _BAR_GENERATOR_H_
#define _BAR_GENERATOR_H_


#include "md_utils.h"

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


typedef struct field_names_s
{
    const char* Instrument;
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
    char    Date[9];
    char    Time[9];
    char    Period[4];
}bar_data_t;

struct bar_generator_s
{
    uint32_t    tick_count;
    uint32_t    bar_count;

    int64_t     prev_volume;
    double      prev_turnover;
    bar_data_t  cur_bar;
    bar_data_t  fin_bar;
    sim_time_t  bar_tm;
};

typedef struct bar_generator_s bar_generator_t;


MD_UTILS_API int MD_UTILS_STDCALL bar_generator_init(bar_generator_t* bargen);


MD_UTILS_API bar_data_t* MD_UTILS_STDCALL bar_generator_update(
    bar_generator_t* bargen, const char* instrument, const char* exchange,
    const char* date, int update_time, double last_price,
    int64_t volume, double turnover, int open_interest);


MD_UTILS_API const char* MD_UTILS_STDCALL bar_generator_version();


MD_UTILS_API int MD_UTILS_STDCALL bar_data_as_json(
    char json_buffer[], int size,
    const bar_data_t* bar,
    const field_names_t* fieldnames,
    int indent);

MD_UTILS_API void MD_UTILS_STDCALL bar_data_fieldnames_init(field_names_t* fieldnames);



#ifdef __cplusplus
}
#endif//__cplusplus

#endif//_BAR_GENERATOR_H_
