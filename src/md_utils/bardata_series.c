#include <assert.h>
#include <string.h>

#include "bardata_series.h"


#define BARDATA_SERIES_VERSION      "1.0.0"


void MD_UTILS_STDCALL bardata_series_init(bardata_series_t* bards, int32_t size)
{
    memset(bards, 0, sizeof(bardata_series_t));
    cdeque_init(&bards->opens, size, 0, sizeof(double));
    cdeque_init(&bards->highs, size, 0, sizeof(double));
    cdeque_init(&bards->lows,  size, 0, sizeof(double));
    cdeque_init(&bards->closes,size, 0, sizeof(double));
    cdeque_init(&bards->volumes, size, 0, sizeof(int64_t));
    cdeque_init(&bards->turnovers, size, 0, sizeof(double));
    cdeque_init(&bards->datetimes, size, 0, sizeof(int64_t));
}

void MD_UTILS_STDCALL bardata_series_release(bardata_series_t* bards)
{
    cdeque_release(&bards->opens);
    cdeque_release(&bards->highs);
    cdeque_release(&bards->lows);
    cdeque_release(&bards->closes);
    cdeque_release(&bards->volumes);
    cdeque_release(&bards->turnovers);
    cdeque_release(&bards->datetimes);
}

int MD_UTILS_STDCALL bardata_series_push_bar(bardata_series_t* bards,
    int date, int update_time,
    double open, double high, double low, double close,
    int64_t volume, double turnover, double open_interest)
{
    union_dt_t udt;

    cdeque_push_double(&bards->opens, open);
    cdeque_push_double(&bards->highs, high);
    cdeque_push_double(&bards->lows,  low);
    cdeque_push_double(&bards->closes, close);
    cdeque_push_int64(&bards->volumes, volume);
    cdeque_push_double(&bards->turnovers, turnover);
    cdeque_push_double(&bards->open_interests, open_interest);

    udt.sdt.date = date;
    udt.sdt.update_time = update_time;
    cdeque_push_int64(&bards->datetimes, udt.i64);

    return 0;
}

int MD_UTILS_STDCALL bardata_series_len(bardata_series_t* bards)
{
    return cdeque_len(&bards->opens);
}

double* MD_UTILS_STDCALL bardata_series_open(bardata_series_t* bards)
{
    return cdeque_data(&bards->opens);
}

double* MD_UTILS_STDCALL bardata_series_high(bardata_series_t* bards)
{
    return cdeque_data(&bards->highs);
}

double* MD_UTILS_STDCALL bardata_series_low(bardata_series_t* bards)
{
    return cdeque_data(&bards->lows);
}

double* MD_UTILS_STDCALL bardata_series_close(bardata_series_t* bards)
{
    return cdeque_data(&bards->closes);
}

double* MD_UTILS_STDCALL bardata_series_turnover(bardata_series_t* bards)
{
    return cdeque_data(&bards->turnovers);
}

double* MD_UTILS_STDCALL bardata_series_open_interest(bardata_series_t* bards)
{
    return cdeque_data(&bards->open_interests);
}

int64_t* MD_UTILS_STDCALL bardata_series_volume(bardata_series_t* bards)
{
    return cdeque_data(&bards->volumes);
}

int64_t* MD_UTILS_STDCALL bardata_series_datetime(bardata_series_t* bards)
{
    return cdeque_data(&bards->datetimes);
}
