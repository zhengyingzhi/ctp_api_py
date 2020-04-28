#include <assert.h>
#include <string.h>

#include "bar_generator.h"


#define BAR_GENERATOR_VERSION   "1.0.2"

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))


/* totally copy from ThostFtdcUserApiStruct.h */
typedef char TThostFtdcDateType[9];
typedef char TThostFtdcInstrumentIDType[31];
typedef char TThostFtdcExchangeIDType[9];
typedef char TThostFtdcExchangeInstIDType[31];
typedef double TThostFtdcPriceType;
typedef double TThostFtdcLargeVolumeType;
typedef int TThostFtdcVolumeType;
typedef double TThostFtdcMoneyType;
typedef double TThostFtdcRatioType;
typedef char TThostFtdcTimeType[9];
typedef int TThostFtdcMillisecType;

///深度行情
struct CThostFtdcDepthMarketDataField
{
    ///交易日
    TThostFtdcDateType	TradingDay;
    ///合约代码
    TThostFtdcInstrumentIDType	InstrumentID;
    ///交易所代码
    TThostFtdcExchangeIDType	ExchangeID;
    ///合约在交易所的代码
    TThostFtdcExchangeInstIDType	ExchangeInstID;
    ///最新价
    TThostFtdcPriceType	LastPrice;
    ///上次结算价
    TThostFtdcPriceType	PreSettlementPrice;
    ///昨收盘
    TThostFtdcPriceType	PreClosePrice;
    ///昨持仓量
    TThostFtdcLargeVolumeType	PreOpenInterest;
    ///今开盘
    TThostFtdcPriceType	OpenPrice;
    ///最高价
    TThostFtdcPriceType	HighestPrice;
    ///最低价
    TThostFtdcPriceType	LowestPrice;
    ///数量
    TThostFtdcVolumeType	Volume;
    ///成交金额
    TThostFtdcMoneyType	Turnover;
    ///持仓量
    TThostFtdcLargeVolumeType	OpenInterest;
    ///今收盘
    TThostFtdcPriceType	ClosePrice;
    ///本次结算价
    TThostFtdcPriceType	SettlementPrice;
    ///涨停板价
    TThostFtdcPriceType	UpperLimitPrice;
    ///跌停板价
    TThostFtdcPriceType	LowerLimitPrice;
    ///昨虚实度
    TThostFtdcRatioType	PreDelta;
    ///今虚实度
    TThostFtdcRatioType	CurrDelta;
    ///最后修改时间
    TThostFtdcTimeType	UpdateTime;
    ///最后修改毫秒
    TThostFtdcMillisecType	UpdateMillisec;
    ///申买价一
    TThostFtdcPriceType	BidPrice1;
    ///申买量一
    TThostFtdcVolumeType	BidVolume1;
    ///申卖价一
    TThostFtdcPriceType	AskPrice1;
    ///申卖量一
    TThostFtdcVolumeType	AskVolume1;
    ///申买价二
    TThostFtdcPriceType	BidPrice2;
    ///申买量二
    TThostFtdcVolumeType	BidVolume2;
    ///申卖价二
    TThostFtdcPriceType	AskPrice2;
    ///申卖量二
    TThostFtdcVolumeType	AskVolume2;
    ///申买价三
    TThostFtdcPriceType	BidPrice3;
    ///申买量三
    TThostFtdcVolumeType	BidVolume3;
    ///申卖价三
    TThostFtdcPriceType	AskPrice3;
    ///申卖量三
    TThostFtdcVolumeType	AskVolume3;
    ///申买价四
    TThostFtdcPriceType	BidPrice4;
    ///申买量四
    TThostFtdcVolumeType	BidVolume4;
    ///申卖价四
    TThostFtdcPriceType	AskPrice4;
    ///申卖量四
    TThostFtdcVolumeType	AskVolume4;
    ///申买价五
    TThostFtdcPriceType	BidPrice5;
    ///申买量五
    TThostFtdcVolumeType	BidVolume5;
    ///申卖价五
    TThostFtdcPriceType	AskPrice5;
    ///申卖量五
    TThostFtdcVolumeType	AskVolume5;
    ///当日均价
    TThostFtdcPriceType	AveragePrice;
    ///业务日期
    TThostFtdcDateType	ActionDay;
};

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
int sjson_add_float(sim_json_t* json, const char* key, const double val, int islast);
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


typedef struct
{
    const char* exchange;
    const char* code;
    int32_t pause_times[5];
}code_pause_time_t;

static code_pause_time_t default_code_pause_time_table[] = {
    { MD_EXCHANGE_DCE, "a",   { 230000, 101500, 113000, 150000, BG_INVAL_PAUSE_TIME }},
    { MD_EXCHANGE_DCE, "b",   { 230000, 101500, 113000, 150000, BG_INVAL_PAUSE_TIME }},
    { MD_EXCHANGE_DCE, "m",   { 230000, 101500, 113000, 150000, BG_INVAL_PAUSE_TIME }},
    { MD_EXCHANGE_SHFE, "ag", { 23000, 101500, 113000, 150000,  BG_INVAL_PAUSE_TIME }},
    { MD_EXCHANGE_SHFE, "cu", { 10000, 101500, 113000, 150000,  BG_INVAL_PAUSE_TIME } },
    { MD_EXCHANGE_SHFE, "al", { 10000, 101500, 113000, 150000,  BG_INVAL_PAUSE_TIME } },
    { MD_EXCHANGE_SHFE, "zn", { 10000, 101500, 113000, 150000,  BG_INVAL_PAUSE_TIME } },
    { MD_EXCHANGE_SHFE, "pb", { 10000, 101500, 113000, 150000,  BG_INVAL_PAUSE_TIME } },
    { MD_EXCHANGE_SHFE, "sn", { 10000, 101500, 113000, 150000,  BG_INVAL_PAUSE_TIME } },
    { MD_EXCHANGE_SHFE, "rb", { 230000, 101500, 113000, 150000, BG_INVAL_PAUSE_TIME } },
    { MD_EXCHANGE_SHFE, "ru", { 230000, 101500, 113000, 150000, BG_INVAL_PAUSE_TIME } },
    { MD_EXCHANGE_INE,  "sc", { 23000, 101500, 113000, 150000,  BG_INVAL_PAUSE_TIME } },
    { MD_EXCHANGE_INE,  "nr", { 23000, 101500, 113000, 150000,  BG_INVAL_PAUSE_TIME } },
    { MD_EXCHANGE_CFFEX, "IF",{ 113000, 150000, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME } },
    { MD_EXCHANGE_CFFEX, "IC",{ 113000, 150000, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME } },
    { MD_EXCHANGE_CFFEX, "IH",{ 113000, 150000, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME } },
    { MD_EXCHANGE_CFFEX, "T", { 113000, 151500, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME } },
    { MD_EXCHANGE_CFFEX, "TS",{ 113000, 151500, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME } },
    { MD_EXCHANGE_CFFEX, "TF",{ 113000, 151500, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME } },
    { MD_EXCHANGE_CFFEX, "TT",{ 113000, 151500, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME } },
    { NULL, NULL, { BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME } }
};

static const char* codes_2300[] = { "bu", "fu", "hc", "rb", "ru", "sp", "ss", NULL };
static const char* codes_2330[] = { "a", "b", "m", "eb", "y", "i", "jm", "j", "p", "pg",
    "RM", "OI", "CF", "CY", "SR", "FG", "MA", "JR", "SF", "WH", "SM", "RS", "RI", "PM", "LR", "AP", "TA", "ZC", NULL };
static const char* codes_100[] = { "al", "cu", "ni", "pb", "sn", "zn", NULL };
static const char* codes_230[] = { "ag", "au", "sc", "nr", NULL };
static const char* codes_fut[] = { "c", "cs", "jd", "l", "v", "pp", "fb", "bb", "wr" };

static int pause_time_2300[] = { 230000, 101500, 113000, 150000, BG_INVAL_PAUSE_TIME };
static int pause_time_2330[] = { 233000, 101500, 113000, 150000, BG_INVAL_PAUSE_TIME };
static int pause_time_100[]  = { 10000,  101500, 113000, 150000, BG_INVAL_PAUSE_TIME };
static int pause_time_230[]  = { 23000,  101500, 113000, 150000, BG_INVAL_PAUSE_TIME };
static int pause_time_fut[]  = { 101500, 113000, 150000, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME };
static int pause_time_stk[]  = { 113000, 150000, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME, BG_INVAL_PAUSE_TIME };


//////////////////////////////////////////////////////////////////////////
static bool is_code_exist(const char* codes_table[], const char* code)
{
    const char* pcur;
    for (int index = 0; codes_table[index]; ++index)
    {
        pcur = codes_table[index];
        if (strcmp(pcur, code) == 0)
        {
            return true;
        }
    }
    return false;
}

static int* find_code_pause_time(const char* code)
{
    if (is_stock(code))
    {
        return pause_time_stk;
    }
    else if (is_code_exist(codes_2300, code))
    {
        return pause_time_2300;
    }
    else if (is_code_exist(codes_2330, code))
    {
        return pause_time_2330;
    }
    else if (is_code_exist(codes_100, code))
    {
        return pause_time_100;
    }
    else if (is_code_exist(codes_230, code))
    {
        return pause_time_230;
    }
    else if (is_code_exist(codes_fut, code))
    {
        return pause_time_fut;
    }
    else
    {
        return pause_time_stk;
    }
}

static bool filter_cn_security_by_updatetime(const char* instrument, int update_time)
{
    (void)instrument;
    if ((update_time > 150001 || update_time < 92500) ||
        (update_time > 113001 && update_time < 130000))
    {
        return true;
    }
    return false;
}

static bool filter_cn_future_by_updatetime(const char* instrument, int update_time)
{
    (void)instrument;
    if ((update_time > 150001 && update_time < 205900) ||
        (update_time > 23001 && update_time < 85900))
    {
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////

int MD_UTILS_STDCALL bar_generator_init(bar_generator_t* bargen, const char* exchange, const char* instrument)
{
    // fprintf(stderr, "CTPMD %d\n", sizeof(struct CThostFtdcDepthMarketDataField));

    memset(bargen, 0, sizeof(bar_generator_t));
    bargen->first_update_flag = 1;
    bargen->begin_update_time = BG_INVAL_PAUSE_TIME;
    bargen->prev_update_time = BG_INVAL_PAUSE_TIME;
    bargen->cur_high = 0;
    bargen->cur_low = 999999999;

    if (instrument)
    {
        bargen->have_end_auction = is_stock(instrument);

        strncpy(bargen->instrument, instrument, sizeof(bargen->instrument) - 1);
        get_product_code(bargen->code, instrument);

        bar_data_t* bar;
        bar = &bargen->cur_bar;
        strncpy(bar->InstrumentID, bargen->instrument, sizeof(bar->InstrumentID) - 1);
        strncpy(bar->Period, "1m", 2);

        bar = &bargen->fin_bar;
        strncpy(bar->InstrumentID, bargen->instrument, sizeof(bar->InstrumentID) - 1);
        strncpy(bar->Period, "1m", 2);

        // FIXME: support option or arbitrage
        if (strlen(instrument) <= 8)
            memcpy(&bargen->hash_code, bargen->instrument, 8);
    }

    if (exchange)
    {
        strncpy(bargen->cur_bar.ExchangeID, exchange, sizeof(bargen->cur_bar.ExchangeID) - 1);
        strncpy(bargen->fin_bar.ExchangeID, exchange, sizeof(bargen->fin_bar.ExchangeID) - 1);
    }
    memset(bargen->pause_times, BG_INVAL_PAUSE_TIME, sizeof(bargen->pause_times));

    int* pause_times;
    pause_times = find_code_pause_time(bargen->code);

    int index = 0;
    for (int k = 0; pause_times[k] != BG_INVAL_PAUSE_TIME; ++k)
    {
        bargen->pause_times[index++] = pause_times[k];
        bargen->pause_times[index++] = pause_times[k] + 1;
    }

    if (bargen->pause_times[0] == BG_INVAL_PAUSE_TIME)
    {
        if (is_stock(bargen->code))
        {
            bargen->pause_times[0] = 113000;
            bargen->pause_times[1] = 113001;
            bargen->pause_times[2] = 150000;
            bargen->pause_times[3] = 150001;
        }
        else
        {
            bargen->pause_times[0] = 101500;
            bargen->pause_times[1] = 101501;
            bargen->pause_times[2] = 113000;
            bargen->pause_times[3] = 113001;
            bargen->pause_times[4] = 150000;
            bargen->pause_times[5] = 150001;
        }
    }

    if (instrument[0] >= '0' && instrument[0] <= '9')
    {
        bargen->filter_data_by_updatetime = filter_cn_security_by_updatetime;
    }
    else
    {
        bargen->filter_data_by_updatetime = filter_cn_future_by_updatetime;
    }
    return 0;
}

void MD_UTILS_STDCALL bar_generator_set_pause_times(bar_generator_t* bargen, int pause_times[], int size)
{
    int index = 0;
    for (int k = 0; k < 4 && k < size; ++k)
    {
        bargen->pause_times[index++] = pause_times[k];
        bargen->pause_times[index++] = pause_times[k] + 1;
        if (index == BG_PAUSE_TIMES_SIZE)
            break;
    }
}

void MD_UTILS_STDCALL bar_generator_set_end_auction(bar_generator_t* bargen, int end_auction_flag)
{
    bargen->have_end_auction = end_auction_flag;
}

bar_data_t* MD_UTILS_STDCALL bar_generator_update_data(
    bar_generator_t* bargen, const char* date, int update_time,
    double last_price, int64_t volume, double turnover, int open_interest)
{
    int include_cur_data_flag;
    int market_close_flag;
    int generated;
    bar_data_t* ret_bar;
    bar_data_t* bar;
    sim_time_t tick_tm = { 0 };
    int_to_ptime(&tick_tm, update_time, 0);

    // fprintf(stderr, "bar_update: instr:%s,lastpx:%.2lf, vol:%ld,time:%d\n", 
    //     instrument, last_price, (long)volume, update_time);
    if (bargen->filter_data_by_updatetime(bargen->instrument, update_time))
    {
        return NULL;
    }

    include_cur_data_flag = 0;
    market_close_flag = 0;
    generated = 0;
    ret_bar = NULL;
    bar = (bar_data_t*)&bargen->cur_bar;

    if (bargen->first_update_flag)
    {
        // the first md update, start new bar data

        int pause_time_flag = 0;
        int pause_time;
        for (int i = 0; i < BG_PAUSE_TIMES_SIZE; ++i)
        {
            pause_time = bargen->pause_times[i];
            if (pause_time == update_time)
            {
                pause_time_flag = 1;
                break;
            }
            else if (pause_time == BG_INVAL_PAUSE_TIME)
            {
                break;
            }
        }

        if (!pause_time_flag)
        {
            bar->Open = last_price;
            bar->High = last_price;
            bar->Low = last_price;
            bar->Close = last_price;
#if BG_BAR_DEBUG
            bar->StartTime = bargen->prev_update_time;
            bar->EndTime = update_time;
#endif//BG_BAR_DEBUG

            // !attention! next update is also first data
            bargen->first_update_flag = 0;
        }

        bargen->begin_volume = bargen->prev_volume;
        bargen->begin_turnover = bargen->prev_turnover;
        bargen->begin_open_interest = bargen->prev_open_interest;
        bargen->begin_update_time = bargen->prev_update_time;
    }
    else if (bargen->prev_tm.tm_min != tick_tm.tm_min ||
             bargen->prev_tm.tm_hour != tick_tm.tm_hour)
    {
        // ---- md minute changed now ----

        // previous is auction time or not
        if ((bargen->prev_update_time >= 85900 && bargen->prev_update_time <= 85959) ||
            (bargen->prev_update_time >= 205900 && bargen->prev_update_time <= 205959))
        {
            generated = 0;

            bar->High = MAX(bar->High, last_price);
            bar->Low = MIN(bar->Low, last_price);
            bar->Close = last_price;
            bar->OpenInterest = open_interest;

            // the auction data belongs to first bar
#if BG_BAR_DEBUG
            bar->StartTime = bargen->prev_update_time;
            bar->EndTime = update_time;
#endif//BG_BAR_DEBUG
            bargen->begin_volume = 0;
            bargen->begin_turnover = 0;
            bargen->begin_open_interest = open_interest;
        }
        else
        {
            bargen->bar_count += 1;

            // current update time is pause time or not
            int pause_time;
            for (int i = 0; i < BG_PAUSE_TIMES_SIZE; ++i)
            {
                pause_time = bargen->pause_times[i];
                if (pause_time == update_time)
                {
                    // this tick data need merged into fin_bar
                    include_cur_data_flag = 1;
                    if (bargen->pause_times[i + 1] == BG_INVAL_PAUSE_TIME ||
                        bargen->pause_times[i + 2] == BG_INVAL_PAUSE_TIME)
                        market_close_flag = 1;

                    bar->High = MAX(bar->High, last_price);
                    bar->Low = MIN(bar->Low, last_price);
                    bar->Close = last_price;
                    bar->OpenInterest = open_interest;
                    break;
                }
                else if (pause_time == BG_INVAL_PAUSE_TIME)
                {
                    break;
                }
            }

            // the generated bar
            ret_bar = &bargen->fin_bar;
            ret_bar->Open = bar->Open;
            ret_bar->High = bar->High;
            ret_bar->Low = bar->Low;
            ret_bar->Close = bar->Close;
            ret_bar->OpenInterest = bar->OpenInterest;
            strncpy(ret_bar->Date, date, 8);
            sprintf(ret_bar->Time, "%02d:%02d:00", tick_tm.tm_hour, tick_tm.tm_min);

            if (include_cur_data_flag)
            {
                ret_bar->Volume = volume - bargen->begin_volume;
                ret_bar->Turnover = turnover - bargen->begin_turnover;
                ret_bar->OpenInterest = open_interest;
#if BG_BAR_DEBUG
                ret_bar->StartTime = bar->StartTime;
                ret_bar->EndTime = update_time;
                bar->StartTime = update_time;
#endif//BG_BAR_DEBUG

                // here, next bar must be new start
                bargen->first_update_flag = 1;
                bargen->begin_volume = volume;
                bargen->begin_turnover = turnover;
                bargen->begin_open_interest = open_interest;
                bargen->begin_update_time = update_time;
            }
            else
            {
                ret_bar->Volume = bargen->prev_volume - bargen->begin_volume;
                ret_bar->Turnover = bargen->prev_turnover - bargen->begin_turnover;
                ret_bar->OpenInterest = bargen->prev_open_interest;
#if BG_BAR_DEBUG
                ret_bar->StartTime = bar->StartTime;
                ret_bar->EndTime = bargen->prev_update_time;
                bar->EndTime = update_time;
                bar->StartTime = bargen->prev_update_time;
#endif//BG_BAR_DEBUG

                // current update data belongs to next bar
                bar->Open = last_price;
                bar->High = last_price;
                bar->Low = last_price;
                bar->Close = last_price;
                bar->OpenInterest = open_interest;
                bargen->begin_volume = bargen->prev_volume;
                bargen->begin_turnover = bargen->prev_turnover;
                bargen->begin_open_interest = bargen->prev_open_interest;
                bargen->begin_update_time = bargen->prev_update_time;
            }

            if (market_close_flag && bargen->have_end_auction)
            {
#if BG_BAR_DEBUG
                bar->EndTime = update_time;
                bar->StartTime = bargen->prev_update_time;
#endif//BG_BAR_DEBUG

                bar->Open = last_price;
                bar->High = last_price;
                bar->Low = last_price;
                bar->Close = last_price;
                bar->Volume = volume - bargen->prev_volume;
                bar->Turnover = turnover - bargen->prev_turnover;
                bar->OpenInterest = open_interest;
                memcpy(ret_bar, bar, sizeof(bar_data_t));
                strncpy(ret_bar->Date, date, 8);
                sprintf(ret_bar->Time, "%02d:%02d:00", tick_tm.tm_hour, tick_tm.tm_min);
            }
        }
    }
    else
    {
        // bar->High = MAX(bar->High, last_price);
        // bar->Low = MIN(bar->Low, last_price);
        if (bar->High < last_price)
            bar->High = last_price;
        if (bar->Low > last_price)
            bar->Low = last_price;
        bar->Close = last_price;
        bar->OpenInterest = open_interest;
#if BG_BAR_DEBUG
        bar->EndTime = update_time;
#endif//BG_BAR_DEBUG
    }

    bargen->tick_count += 1;
    bargen->generated = generated;
    bargen->prev_volume = volume;
    bargen->prev_turnover = turnover;
    bargen->prev_open_interest = open_interest;
    bargen->prev_update_time = update_time;
    bargen->prev_tm = tick_tm;

    return ret_bar;
}

bar_data_t* MD_UTILS_STDCALL bar_generator_update_without_data(
    bar_generator_t* bargen, const char* date, int update_time)
{
    bar_data_t* ret_bar;
    bar_data_t* bar;
    sim_time_t tick_tm = { 0 };
    int_to_ptime(&tick_tm, update_time, 0);

    bar = &bargen->cur_bar;

    ret_bar = &bargen->fin_bar;
    ret_bar->Open = bar->Open;
    ret_bar->High = bar->High;
    ret_bar->Low = bar->Low;
    ret_bar->Close = bar->Close;
    ret_bar->OpenInterest = bar->OpenInterest;

    ret_bar->Volume = bargen->prev_volume - bargen->begin_volume;
    ret_bar->Turnover = bargen->prev_turnover- bargen->begin_turnover;

    strncpy(ret_bar->Date, date, 8);
    sprintf(ret_bar->Time, "%02d:%02d:00", tick_tm.tm_hour, tick_tm.tm_min);

#if BG_BAR_DEBUG
    ret_bar->StartTime = bar->StartTime;
    ret_bar->EndTime = bargen->prev_update_time;
    bar->StartTime = update_time;
#endif//BG_BAR_DEBUG

    // prepare for next bar data
    bargen->first_update_flag = 1;
    bargen->begin_volume = bargen->prev_volume;
    bargen->begin_turnover = bargen->prev_turnover;

    return ret_bar;
}

#if BG_ENABLE_CTP_MD
bar_data_t* MD_UTILS_STDCALL bar_generator_update_by_ctp(
    bar_generator_t* bargen, void* apmd, int size)
{
    assert(size == sizeof(struct CThostFtdcDepthMarketDataField));

    int update_time;
    bar_data_t* bar;
    struct CThostFtdcDepthMarketDataField* pmd;
    pmd = (struct CThostFtdcDepthMarketDataField*)apmd;

    if (pmd->LastPrice > 99999999.0)
        pmd->LastPrice = 0.0;

    update_time = atoi(pmd->UpdateTime) * 10000 + \
        atoi(pmd->UpdateTime + 3) * 100 + \
        atoi(pmd->UpdateTime + 6);

    // FIXME: the ActionDay maybe error for DCE night trading
    bar = bar_generator_update_data(bargen, pmd->ActionDay, update_time,
            pmd->LastPrice, pmd->Volume, pmd->Turnover, (int)pmd->OpenInterest);

    return bar;
}
#endif//BG_ENABLE_CTP_MD

void MD_UTILS_STDCALL bar_generator_reset(bar_generator_t* bargen)
{
    bargen->generated = 0;
    bargen->tick_count = 0;
    bargen->bar_count = 0;
    bargen->first_update_flag = 1;

    bargen->begin_volume = 0;
    bargen->begin_turnover = 0.0;
    bargen->begin_open_interest = 0;
    bargen->begin_update_time = BG_INVAL_PAUSE_TIME;

    bargen->prev_volume = 0;
    bargen->prev_turnover = 0.0;
    bargen->prev_open_interest = 0;
    bargen->prev_update_time = BG_INVAL_PAUSE_TIME;
    memset(&bargen->prev_tm, 0, sizeof(bargen->prev_tm));

    bargen->cur_high = 0;
    bargen->cur_low = 999999999;
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

    int islast = 0;
    sim_json_t json;
    sjson_json_init(&json, json_buffer, size, indent);
    sjson_add_head(&json);
    sjson_add_str(&json, fieldnames->InstrumentID, bar->InstrumentID, islast);
    sjson_add_str(&json, fieldnames->Exchange, bar->ExchangeID, islast);
    sjson_add_float(&json, fieldnames->Open, bar->Open, islast);
    sjson_add_float(&json, fieldnames->High, bar->High, islast);
    sjson_add_float(&json, fieldnames->Low, bar->Low, islast);
    sjson_add_float(&json, fieldnames->Close, bar->Close, islast);
    sjson_add_float(&json, fieldnames->Turnover, bar->Turnover, islast);
    sjson_add_int32(&json, fieldnames->OpenInterest, bar->OpenInterest, islast);
    sjson_add_int64(&json, fieldnames->Volume, bar->Volume, islast);

#if BG_BAR_DEBUG
    sjson_add_int32(&json, "StartTime", bar->StartTime, islast);
    sjson_add_int32(&json, "EndTime", bar->EndTime, islast);
#endif//BG_BAR_DEBUG

    if (!bar->Period)
        islast = 1;

    if (fieldnames->DateTime)
    {
        char dt[64] = "";
        snprintf(dt, sizeof(dt) - 1, "%s %s", bar->Date, bar->Time);
        sjson_add_str(&json, fieldnames->DateTime, dt, islast);
    }
    else
    {
        sjson_add_str(&json, fieldnames->Date, bar->Date, 0);
        sjson_add_str(&json, fieldnames->Time, bar->Time, islast);
    }

    if (bar->Period)
        sjson_add_str(&json, fieldnames->Period, bar->Period, 1);

    return json.len;
}

void MD_UTILS_STDCALL bar_data_fieldnames_init(field_names_t* fieldnames)
{
    fieldnames->InstrumentID = "instrument";
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

int sjson_add_float(sim_json_t* json, const char* key, const double val, int islast)
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
