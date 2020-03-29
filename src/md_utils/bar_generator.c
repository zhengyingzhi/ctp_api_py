#include <string.h>

#include "bar_generator.h"


#define BAR_GENERATOR_VERSION   "1.0.0"

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))


typedef struct sim_json_s
{
    char* buf;
    int   len;
    int   capacity;
    int   indent;
    char  indent_buf[8];
    char  line_feed;
}sim_json_t;

void sjson_json_init(sim_json_t* json, char* buf, int size, int indent);
int sjson_add_head(sim_json_t* json);
int sjson_add_str(sim_json_t* json, const char* key, const char* val, int islast);
int sjson_add_int32(sim_json_t* json, const char* key, int32_t val, int islast);
int sjson_add_int64(sim_json_t* json, const char* key, const int64_t val, int islast);
int sjson_add_float(sim_json_t* json, const char* key, double val, int islast);
int sjson_add_bool(sim_json_t* json, const char* key, bool val, int islast);
int sjson_add_null(sim_json_t* json, const char* key, int islast);


static field_names_t default_fieldnames = {
    "instrument",
    "exchange",
    "open",
    "high",
    "low",
    "close",
    "volume",
    "turnover",
    "open_interest",
    "date",
    "time",
    "period",
    NULL,       // datetime
    NULL        // adjust_factor
};


//////////////////////////////////////////////////////////////////////////
int bar_generator_init(bar_generator_t* bargen)
{
    memset(bargen, 0, sizeof(bar_generator_t));
    return 0;
}


bar_data_t* MD_UTILS_STDCALL bar_generator_update(
    bar_generator_t* bargen, const char* instrument, const char* exchange,
    const char* date, int update_time, double last_price,
    int64_t volume, double turnover, int open_interest)
{
    int generated, new_minute;
    int64_t volume_delta;
    double  turnover_delta;
    bar_data_t* bar;
    sim_time_t tick_tm = { 0 };
    int_to_ptime(&tick_tm, update_time, 0);

    generated = 0;
    new_minute = 0;
    volume_delta = 0;
    turnover_delta = 0;
    bar = (bar_data_t*)&bargen->cur_bar;

    if (!bargen->fin_bar.InstrumentID[0])
    {
        new_minute = 1;
    }
    else if (bargen->bar_tm.tm_min != tick_tm.tm_min ||
             bargen->bar_tm.tm_hour != tick_tm.tm_hour)
    {
        new_minute = 1;
        generated = 1;
        memcpy(&bargen->fin_bar, &bargen->cur_bar, sizeof(bar_data_t));
    }

    if (new_minute)
    {
        // new bar
        strncpy(bar->InstrumentID, instrument, sizeof(bar->InstrumentID) - 1);
        strncpy(bar->ExchangeID, instrument, sizeof(bar->ExchangeID) - 1);
        strncpy(bar->Period, "1m", 2);
        strncpy(bargen->fin_bar.InstrumentID, instrument, sizeof(bar->InstrumentID) - 1);
        strncpy(bargen->fin_bar.ExchangeID, instrument, sizeof(bar->ExchangeID) - 1);
        strncpy(bargen->fin_bar.Period, "1m", 2);
        bargen->bar_tm = tick_tm;

        bar->Open = last_price;
        bar->High = last_price;
        bar->Low = last_price;
    }

    bar->High = MAX(bar->High, last_price);
    bar->Low = MIN(bar->Low, last_price);
    bar->Close = last_price;
    bar->OpenInterest = (int)open_interest;
    bar->Volume += MAX(0, volume - bargen->prev_volume);
    bar->Turnover += MAX(0, turnover - bargen->prev_turnover);

    bargen->prev_volume = volume;
    bargen->prev_turnover = turnover;
    bargen->tick_count += 1;
    bargen->bar_tm = tick_tm;

    if (generated)
    {
        bargen->bar_count += 1;

        bargen->bar_tm.tm_sec = 0;
        bargen->bar_tm.tm_msec = 0;

        strncpy(bargen->fin_bar.Date, date, 8);
        sprintf(bargen->fin_bar.Time, "%02d:%02d:00",
            bargen->bar_tm.tm_hour, bargen->bar_tm.tm_min);
        return &bargen->fin_bar;
    }

    return NULL;
}

const char* MD_UTILS_STDCALL bar_generator_version()
{
    return BAR_GENERATOR_VERSION;
}


//////////////////////////////////////////////////////////////////////////
int MD_UTILS_STDCALL bar_data_as_json(
    char json_buffer[], int size,
    const bar_data_t* bar,
    const field_names_t* fieldnames,
    int indent)
{
    if (!fieldnames)
        fieldnames = &default_fieldnames;

    sim_json_t json;
    sjson_json_init(&json, json_buffer, size, indent);
    sjson_add_head(&json);
    sjson_add_str(&json, fieldnames->Instrument, bar->InstrumentID, 0);
    sjson_add_str(&json, fieldnames->Exchange, bar->ExchangeID, 0);
    sjson_add_float(&json, fieldnames->Open, bar->Open, 0);
    sjson_add_float(&json, fieldnames->High, bar->High, 0);
    sjson_add_float(&json, fieldnames->Low, bar->Low, 0);
    sjson_add_float(&json, fieldnames->Close, bar->Close, 0);
    sjson_add_float(&json, fieldnames->Turnover, bar->Turnover, 0);
    sjson_add_float(&json, fieldnames->OpenInterest, bar->OpenInterest, 0);
    sjson_add_int64(&json, fieldnames->Volume, bar->Volume, 0);

    int end = 0;
    if (!bar->Period)
        end = 1;

    if (fieldnames->DateTime)
    {
        char dt[64] = "";
        snprintf(dt, 63, "%s %s", bar->Date, bar->Time);
        sjson_add_str(&json, fieldnames->DateTime, dt, end);
    }
    else
    {
        sjson_add_str(&json, fieldnames->Date, bar->Date, 0);
        sjson_add_str(&json, fieldnames->Time, bar->Time, end);
    }

    if (bar->Period)
        sjson_add_str(&json, fieldnames->Period, bar->Period, 1);

    return json.len;
}

void MD_UTILS_STDCALL bar_data_fieldnames_init(field_names_t* fieldnames)
{
    fieldnames->Instrument = "instrument";
    fieldnames->Exchange = "exchange";
    fieldnames->Open = "open";
    fieldnames->High = "high";
    fieldnames->Low = "low";
    fieldnames->Close = "close";
    fieldnames->Volume = "volume";
    fieldnames->Turnover = "turnover";
    fieldnames->OpenInterest = "open_interest";
    fieldnames->Date = "date";
    fieldnames->Time = "time";

#if 0
    fieldnames->DateTime = "datetime";
    fieldnames->Period = "period";
    fieldnames->AdjustFactor = "adjust_factor";
#else
    fieldnames->DateTime = NULL;
    fieldnames->Period = NULL;
    fieldnames->AdjustFactor = NULL;
#endif
}



//////////////////////////////////////////////////////////////////////////

void sjson_json_init(sim_json_t* json, char* buf, int size, int indent)
{
    json->buf = buf;
    json->len = 0;
    json->capacity = size;
    json->line_feed = '\n';
    if (indent < 0)
    {
        indent = 0;
        json->line_feed = '\0';
    }
    json->indent = indent;
    memset(json->indent_buf, 0, sizeof(json->indent_buf));
    for (int i = 0; i < indent; ++i)
        json->indent_buf[i] = ' ';
}

int sjson_add_head(sim_json_t* json)
{
    json->buf[0] = '{';
    json->buf[1] = json->line_feed;
    if (json->line_feed)
        json->len = 2;
    else
        json->len = 1;
    return json->len;
}

int sjson_add_tail(sim_json_t* json, int islast)
{
    int old_len = json->len;
    if (islast)
    {
        if (json->line_feed)
        {
            json->buf[json->len] = json->line_feed;
            json->len += 1;
        }
        json->buf[json->len] = '}';
        json->buf[json->len + 1] = '\0';
        json->len += 1;
    }
    else
    {
        json->buf[json->len] = ',';
        json->len += 1;
        if (json->line_feed)
        {
            json->buf[json->len] = json->line_feed;
            json->len += 1;
        }
    }
    return json->len - old_len;
}

int sjson_add_str(sim_json_t* json, const char* key, const char* val, int islast)
{
    if (json->capacity - json->len < 8)
    {
        return 0;
    }

    int len = snprintf(json->buf + json->len, json->capacity - json->len - 1,
                       "%s\"%s\": \"%s\"", json->indent_buf, key, val);
    json->len += len;

    int len2 = sjson_add_tail(json, islast);
    return len + len2;
}

int sjson_add_int32(sim_json_t* json, const char* key, int32_t val, int islast)
{
    if (json->capacity - json->len < 8)
    {
        return 0;
    }

    int len = snprintf(json->buf + json->len, json->capacity - json->len - 1,
                       "%s\"%s\": %d", json->indent_buf, key, val);
    json->len += len;

    int len2 = sjson_add_tail(json, islast);
    return len + len2;
}

int sjson_add_int64(sim_json_t* json, const char* key, const int64_t val, int islast)
{
    if (json->capacity - json->len < 8)
    {
        return 0;
    }

    int len = snprintf(json->buf + json->len, json->capacity - json->len - 1,
                       "%s\"%s\": %ld", json->indent_buf, key, (long)val);
    json->len += len;

    int len2 = sjson_add_tail(json, islast);
    return len + len2;
}

int sjson_add_float(sim_json_t* json, const char* key, double val, int islast)
{
    if (json->capacity - json->len < 8)
    {
        return 0;
    }

    int len = snprintf(json->buf + json->len, json->capacity - json->len - 1,
                       "%s\"%s\": %.4lf", json->indent_buf, key, val);
    json->len += len;

    int len2 = sjson_add_tail(json, islast);
    return len + len2;
}

int sjson_add_bool(sim_json_t* json, const char* key, bool val, int islast)
{
    if (json->capacity - json->len < 8)
    {
        return 0;
    }

    int len;
    if (val)
        len = snprintf(json->buf + json->len, json->capacity - json->len - 1,
                       "%s\"%s\": true", json->indent_buf, key);
    else
        len = snprintf(json->buf + json->len, json->capacity - json->len - 1,
                       "%s\"%s\": false", json->indent_buf, key);
    json->len += len;

    int len2 = sjson_add_tail(json, islast);
    return len + len2;
}

int sjson_add_null(sim_json_t* json, const char* key, int islast)
{
    if (json->capacity - json->len < 8)
    {
        return 0;
    }

    int len = snprintf(json->buf + json->len, json->capacity - json->len - 1,
                       "%s\"%s\": null", json->indent_buf, key);
    json->len += len;

    int len2 = sjson_add_tail(json, islast);
    return len + len2;
}
