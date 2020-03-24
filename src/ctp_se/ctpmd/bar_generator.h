#ifndef _BAR_GENERATOR_H_
#define _BAR_GENERATOR_H_

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


typedef struct
{
    int32_t tm_hour;
    int32_t tm_min;
    int32_t tm_sec;
    int32_t tm_msec;
}sim_time_t;

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
    sim_time_t  tick_tm;
};

typedef struct bar_generator_s bar_generator_t;


int bar_generator_init(bar_generator_t* bargen);

bar_data_t* bar_generator_update_tick(bar_generator_t* bargen, void* apMD);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//_BAR_GENERATOR_H_
