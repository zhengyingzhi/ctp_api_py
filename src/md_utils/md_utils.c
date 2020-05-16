#include <ctype.h>
#include <string.h>
#include <time.h>
#include "md_utils.h"

#define MD_UTILS_VERSION    "1.0.1"


typedef struct
{
    const char* code;
    const char* exchange;
    const char* name;
}code_exchange_t;

static code_exchange_t code_exchange_table[] = {
    { "a",  MD_EXCHANGE_DCE, "豆一" },
    { "b",  MD_EXCHANGE_DCE, "豆二" },
    { "bb", MD_EXCHANGE_DCE, "" },
    { "c",  MD_EXCHANGE_DCE, "" },
    { "cs", MD_EXCHANGE_DCE, "" },
    { "eg", MD_EXCHANGE_DCE, "" },
    { "fb", MD_EXCHANGE_DCE, "" },
    { "i",  MD_EXCHANGE_DCE, "" },
    { "j",  MD_EXCHANGE_DCE, "" },
    { "jd", MD_EXCHANGE_DCE, "" },
    { "jm", MD_EXCHANGE_DCE, "" },
    { "l",  MD_EXCHANGE_DCE, "" },
    { "m",  MD_EXCHANGE_DCE, "" },
    { "rr", MD_EXCHANGE_DCE, "" },
    { "p",  MD_EXCHANGE_DCE, "" },
    { "pg", MD_EXCHANGE_DCE, "" },
    { "pp", MD_EXCHANGE_DCE, "" },
    { "v",  MD_EXCHANGE_DCE, "" },
    { "y",  MD_EXCHANGE_DCE, "" },

    { "AP", MD_EXCHANGE_CZCE, "" },
    { "CF", MD_EXCHANGE_CZCE, "" },
    { "CJ", MD_EXCHANGE_CZCE, "" },
    { "CY", MD_EXCHANGE_CZCE, "" },
    { "FG", MD_EXCHANGE_CZCE, "" },
    { "JR", MD_EXCHANGE_CZCE, "" },
    { "LR", MD_EXCHANGE_CZCE, "" },
    { "MA", MD_EXCHANGE_CZCE, "" },
    { "OI", MD_EXCHANGE_CZCE, "" },
    { "PM", MD_EXCHANGE_CZCE, "" },
    { "RI", MD_EXCHANGE_CZCE, "" },
    { "RM", MD_EXCHANGE_CZCE, "" },
    { "RS", MD_EXCHANGE_CZCE, "" },
    { "SF", MD_EXCHANGE_CZCE, "" },
    { "SM", MD_EXCHANGE_CZCE, "" },
    { "SR", MD_EXCHANGE_CZCE, "" },
    { "TA", MD_EXCHANGE_CZCE, "" },
    { "WH", MD_EXCHANGE_CZCE, "" },
    { "UR", MD_EXCHANGE_CZCE, "" },
    { "ZC", MD_EXCHANGE_CZCE, "" },

    { "ag", MD_EXCHANGE_SHFE, "" },
    { "al", MD_EXCHANGE_SHFE, "" },
    { "au", MD_EXCHANGE_SHFE, "" },
    { "bu", MD_EXCHANGE_SHFE, "" },
    { "cu", MD_EXCHANGE_SHFE, "" },
    { "hc", MD_EXCHANGE_SHFE, "" },
    { "fu", MD_EXCHANGE_SHFE, "" },
    { "ni", MD_EXCHANGE_SHFE, "" },
    { "pb", MD_EXCHANGE_SHFE, "" },
    { "rb", MD_EXCHANGE_SHFE, "" },
    { "ru", MD_EXCHANGE_SHFE, "" },
    { "sn", MD_EXCHANGE_SHFE, "" },
    { "wr", MD_EXCHANGE_SHFE, "" },
    { "zn", MD_EXCHANGE_SHFE, "" },
    { "sp", MD_EXCHANGE_SHFE, "" },
    { "ss", MD_EXCHANGE_SHFE, "" },

    { "sc", MD_EXCHANGE_INE, "原油" },
    { "nr", MD_EXCHANGE_INE, "" },

    { "IC", MD_EXCHANGE_CFFEX, "中证500指数" },
    { "IF", MD_EXCHANGE_CFFEX, "沪深300指数" },
    { "IH", MD_EXCHANGE_CFFEX, "上证50指数" },
    { "T",  MD_EXCHANGE_CFFEX, "" },
    { "TS", MD_EXCHANGE_CFFEX, "" },
    { "TF", MD_EXCHANGE_CFFEX, "" },
    { "TT", MD_EXCHANGE_CFFEX, "" },
    { NULL, NULL, NULL }
};


const char* MD_UTILS_STDCALL md_utils_version()
{
    return MD_UTILS_VERSION;
}

int MD_UTILS_STDCALL str_to_ptime(sim_time_t* ptm, const char* buf, int len)
{
    (void)len;
#if 0
    // 14:50:50.500
    ptm->tm_hour = atoi(buf);
    ptm->tm_min = atoi(buf + 3);
    ptm->tm_sec = atoi(buf + 6);
    // ptm->ms = atoi(buf + 9);
#else
    ptm->tm_hour = (buf[0] - '0') * 10 + (buf[1] - '0');
    ptm->tm_min = (buf[3] - '0') * 10 + (buf[4] - '0');
    ptm->tm_sec = (buf[6] - '0') * 10 + (buf[7] - '0');
#endif
    return 0;
}

int MD_UTILS_STDCALL int_to_ptime(sim_time_t* ptm, int time, int have_millisec)
{
    int hhmmss = time;
    if (have_millisec)
        hhmmss /= 1000;

    ptm->tm_hour = time / 10000;
    ptm->tm_min = (time / 100) % 100;
    ptm->tm_sec = time % 100;
    return 0;
}

int MD_UTILS_STDCALL to_int_time(const char* time_str)
{
    sim_time_t stime;
    str_to_ptime(&stime, time_str, 8);
    return stime.tm_hour * 10000 + stime.tm_min * 100 + stime.tm_sec;
}

int MD_UTILS_STDCALL conv_czce_instrument(char instrument[], int cur_year_month)
{
    int len;
    len = (int)strlen(instrument);
    if (len == 5)
    {
        if (cur_year_month <= 0)
        {
            time_t now = time(NULL);
            struct tm* ptm = localtime(&now);
            cur_year_month = (ptm->tm_year + 1900) * 100 + ptm->tm_mon;
        }

        // 201904 -> 1
        int cur_year_suffix = (int)(cur_year_month / 100) % 10;
        // MA901 -> 9
        int symbol_year_suffix = (int)(atoi(instrument + 2) / 100);

        int symbol_year;
        if (instrument[2] == '0' && cur_year_suffix > symbol_year_suffix)
        {
            // at the next year : 201901 -> 20
            symbol_year = (int)(cur_year_month / 100 + 1) % 100;
        }
        else
        {
            // at the curr year : 1 * 10 + 9 -> 19
            int cur_year_suffix2 = (int)(cur_year_month / 1000) % 10;
            symbol_year = cur_year_suffix2 * 10 + symbol_year_suffix;
        }

        char instrument_tail[32];
        strcpy(instrument_tail, instrument + 3);
        sprintf(instrument + 2, "%02d%s", symbol_year, instrument_tail);
        instrument[6] = '\0';
        return 6;
    }
    else if (len == 6)
    {
        memmove(instrument + 2, instrument + 3, 3);
        instrument[5] = '\0';
        return 5;
    }
    return 0;
}


int MD_UTILS_STDCALL make_sure_symbol_spec(char instrument[], const char* exchange)
{
    // 根据交易转换合约代码大小写
    if (strcmp(exchange, MD_EXCHANGE_CZCE) == 0 ||
        strcmp(exchange, MD_EXCHANGE_CFFEX) == 0)
    {
        if (instrument[0] >= 'a' && instrument[0] <= 'z')
            instrument[0] = instrument[0] - 32;
        if (instrument[1] >= 'a' && instrument[1] <= 'z')
            instrument[1] = instrument[1] - 32;
    }
    else if (strcmp(exchange, MD_EXCHANGE_DCE) == 0 ||
             strcmp(exchange, MD_EXCHANGE_SHFE) == 0 ||
             strcmp(exchange, MD_EXCHANGE_INE) == 0)
    {
        if (instrument[0] >= 'A' && instrument[0] <= 'A')
            instrument[0] = instrument[0] + 32;
        if (instrument[1] >= 'A' && instrument[1] <= 'Z')
            instrument[1] = instrument[1] - 32;
    }
    return 0;
}

MD_UTILS_API int MD_UTILS_STDCALL get_product_code(char code[], const char* instrument)
{
    if (isdigit(instrument[0]))
    {
        strcpy(code, instrument);
    }
    else if (isdigit(instrument[1]))
    {
        code[0] = instrument[0];
        code[1] = '\0';
    }
    else
    {
        code[0] = instrument[0];
        code[1] = instrument[1];
        code[2] = '\0';
    }
    return 0;
}

const char* MD_UTILS_STDCALL lookup_exchange(const char* instrument)
{
    char code[32] = "";
    get_product_code(code, instrument);

    code_exchange_t* pce;
    int i;
    for (i = 0; code_exchange_table[i].code; ++i)
    {
        pce = &code_exchange_table[i];
        if (strcmp(pce->code, code) == 0)
            return pce->exchange;
    }
    return NULL;
}

int MD_UTILS_STDCALL make_symbol(char symbol[], const char* instrument, const char* exchange)
{
    return sprintf(symbol, "%s.%s", instrument, exchange);
}

int MD_UTILS_STDCALL extract_symbol(char instrument[], char exchange[], const char* symbol)
{
    int index = 0;
    const char* pcur = symbol;
    while (*pcur && *pcur != '.')
        instrument[index++] = *pcur++;
    instrument[index] = '\0';

    if (*pcur)
        pcur++;

    index = 0;
    while (*pcur)
        exchange[index++] = *pcur++;
    exchange[index] = '\0';
    return 0;
}

int MD_UTILS_STDCALL is_stock(const char* instrument)
{
    if (instrument[0] >= '0' && instrument[0] <= '9')
        return 1;
    return 0;
}

int MD_UTILS_STDCALL convert_market_id(const char* exchange)
{
    if (strcmp(exchange, MD_EXCHANGE_SSE) == 0) {
        return MD_MI_SSE;
    }
    else if (strcmp(exchange, MD_EXCHANGE_SZSE) == 0) {
        return MD_MI_SZSE;
    }
    else if (strcmp(exchange, MD_EXCHANGE_CFFEX) == 0) {
        return MD_MI_CFFEX;
    }
    else if (strcmp(exchange, MD_EXCHANGE_SHFE) == 0) {
        return MD_MI_SHFE;
    }
    else if (strcmp(exchange, MD_EXCHANGE_DCE) == 0) {
        return MD_MI_DCE;
    }
    else if (strcmp(exchange, MD_EXCHANGE_CZCE) == 0) {
        return MD_MI_CZCE;
    }
    else if (strcmp(exchange, MD_EXCHANGE_INE) == 0) {
        return MD_MI_INE;
    }

    return MD_MI_NONE;
}
