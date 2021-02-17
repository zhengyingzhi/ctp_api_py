#ifndef _BARDATA_SERIES_H_
#define _BARDATA_SERIES_H_


#include <stdbool.h>

#include "cdeque.h"

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus



typedef struct
{
    int32_t date;
    int32_t update_time;
}int_dt_t;

typedef union
{
    int_dt_t sdt;
    int64_t  i64;
}union_dt_t;


struct bardata_series_s
{
    cdeque_t    opens;
    cdeque_t    highs;
    cdeque_t    lows;
    cdeque_t    closes;
    cdeque_t    volumes;
    cdeque_t    turnovers;
    cdeque_t    open_interests;
    cdeque_t    datetimes;

    int32_t     size;
    void*       udata;
};

typedef struct bardata_series_s bardata_series_t;


/* init a bar data series
 */
MD_UTILS_API void MD_UTILS_STDCALL bardata_series_init(bardata_series_t* bards, int32_t size);

MD_UTILS_API void MD_UTILS_STDCALL bardata_series_release(bardata_series_t* bards);

MD_UTILS_API int MD_UTILS_STDCALL bardata_series_push_bar(bardata_series_t* bards,
    int date, int update_time,
    double open, double high, double low, double close,
    int64_t volume, double turnover, double open_interest);

MD_UTILS_API int  MD_UTILS_STDCALL bardata_series_len(bardata_series_t* bards);

MD_UTILS_API double* MD_UTILS_STDCALL bardata_series_open(bardata_series_t* bards);
MD_UTILS_API double* MD_UTILS_STDCALL bardata_series_high(bardata_series_t* bards);
MD_UTILS_API double* MD_UTILS_STDCALL bardata_series_low(bardata_series_t* bards);
MD_UTILS_API double* MD_UTILS_STDCALL bardata_series_close(bardata_series_t* bards);
MD_UTILS_API double* MD_UTILS_STDCALL bardata_series_turnover(bardata_series_t* bards);
MD_UTILS_API double* MD_UTILS_STDCALL bardata_series_open_interest(bardata_series_t* bards);
MD_UTILS_API int64_t* MD_UTILS_STDCALL bardata_series_volume(bardata_series_t* bards);
MD_UTILS_API int64_t* MD_UTILS_STDCALL bardata_series_datetime(bardata_series_t* bards);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//_BARDATA_SERIES_H_
