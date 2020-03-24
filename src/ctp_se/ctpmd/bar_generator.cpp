#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bar_generator.h"

#include "ThostFtdcUserApiStruct.h"


#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))


int my_strptime(const char* buf, int len, sim_time_t* ptm)
{
    // 14:50:50.500
    ptm->tm_hour = atoi(buf);
    ptm->tm_min = atoi(buf + 3);
    ptm->tm_sec = atoi(buf + 6);
    // ptm->ms = atoi(buf + 9);
    return 0;
}


int bar_generator_init(bar_generator_t* bargen)
{
    memset(bargen, 0, sizeof(bar_generator_t));
    return 0;
}

bar_data_t* bar_generator_update_tick(bar_generator_t* bargen, void* apMD)
{
    int generated, new_minute;
    int64_t volume_delta;
    double  turnover_delta;
    bar_data_t* bar;
    CThostFtdcDepthMarketDataField* lpMD;
    sim_time_t tick_tm = { 0 };

    generated = 0;
    new_minute = 0;
    volume_delta = 0;
    turnover_delta = 0;
    bar = (bar_data_t*)&bargen->cur_bar;
    lpMD = (CThostFtdcDepthMarketDataField*)apMD;

    my_strptime(lpMD->UpdateTime, sizeof(lpMD->UpdateTime) - 1, &tick_tm);

    if (!bargen->fin_bar.InstrumentID[0])
    {
        new_minute = 1;
    }
    else if (bargen->bar_tm.tm_min != tick_tm.tm_min || bargen->bar_tm.tm_hour != tick_tm.tm_hour)
    {
        new_minute = 1;
        generated = 1;
        memcpy(&bargen->fin_bar, &bargen->cur_bar, sizeof(bar_data_t));
    }

    if (new_minute)
    {
        // new bar
        strncpy(bar->InstrumentID, lpMD->InstrumentID, sizeof(bar->InstrumentID) - 1);
        strncpy(bar->Period, "1m", 2);
        strncpy(bargen->fin_bar.InstrumentID, lpMD->InstrumentID, sizeof(bar->InstrumentID) - 1);
        strncpy(bargen->fin_bar.Period, "1m", 2);
        bargen->bar_tm = tick_tm;

        bar->Open = lpMD->LastPrice;
        bar->High = lpMD->LastPrice;
        bar->Low = lpMD->LastPrice;
    }

    bar->High = MAX(bar->High, lpMD->LastPrice);
    bar->Low = MIN(bar->Low, lpMD->LastPrice);
    bar->Close = lpMD->LastPrice;
    bar->OpenInterest = (int)lpMD->OpenInterest;
    bar->Volume += MAX(0, lpMD->Volume - bargen->prev_volume);
    bar->Turnover += MAX(0, lpMD->Turnover - bargen->prev_turnover);

    bargen->prev_volume = lpMD->Volume;
    bargen->prev_turnover = lpMD->Turnover;
    bargen->tick_count += 1;
    bargen->bar_tm = tick_tm;

    if (generated)
    {
        bargen->bar_count += 1;

        // bargen->bar_tm = tick_tm;
        bargen->bar_tm.tm_sec = 0;
        bargen->bar_tm.tm_msec = 0;

        strncpy(bargen->fin_bar.Date, lpMD->ActionDay, 8);
        sprintf(bargen->fin_bar.Time, "%02d:%02d:00", bargen->bar_tm.tm_hour, bargen->bar_tm.tm_min);
        return &bargen->fin_bar;
    }
    return NULL;
}

