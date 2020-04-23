#include <string.h>
#include <time.h>

#include "contract_generator.h"


#define CONTRACT_GENERATOR_VERSION   "1.0.0"

int generate_contracts_cffex(contract_generator_t* contr_gen,
    char* pinstruments[], int* count, const char* code);


static void get_year_month(int* apYear, int* apMonth);
static int the_next_yymm(int aYYMM);
static void get_cffex_instrument_yymm(int aYYMMArr[], int aYear, int aMonth);


int MD_UTILS_STDCALL generate_contract(char instrument[],
    const char* exchange, const char* code, int year_month)
{
    if (strcmp(exchange, MD_EXCHANGE_CFFEX) == 0)
    {
        year_month %= 1000;
        return sprintf(instrument, "%s%03d", code, year_month);
    }
    else if (strcmp(exchange, MD_EXCHANGE_CZCE) == 0)
    {
        return sprintf(instrument, "%s%03d", code, year_month);
    }
    else
    {
        return sprintf(instrument, "%s%04d", code, year_month);
    }
}

void MD_UTILS_STDCALL contract_generator_init(contract_generator_t* contr_gen)
{
    memset(contr_gen, 0, sizeof(contract_generator_t));
}

int MD_UTILS_STDCALL generate_contracts(contract_generator_t* contr_gen,
    char* pinstruments[], int* count, const char* exchange, const char* code, int months[], int size)
{
    int index = 0;
    if (strcmp(exchange, MD_EXCHANGE_CFFEX) == 0)
    {
        return generate_contracts_cffex(contr_gen, pinstruments, count, code);
    }

    char* pcur = contr_gen->buff;
    char* pend = contr_gen->buff + sizeof(contr_gen->buff);

    time_t now = time(NULL);
    struct tm* ptm = localtime(&now);
    int cur_year = ptm->tm_year + 1900;
    int cur_month = ptm->tm_mon + 1;

    char instrument[8] = "";
    for (int i = 0; i < size; ++i)
    {
        int mon = months[i];
        int year_month;
        if (cur_month > mon) {
            // cur_month==4, mon=1,2,3
            year_month = cur_year + 1;
        }
        else {
            year_month = cur_year;
        }
        year_month = (year_month % 100) * 100 + mon;
        generate_contract(instrument, exchange, code, year_month);

        // vec.push_back(string(instrument));
        // fprintf(stderr, "%s ", instrument);
        strncpy(pcur, instrument, 8);
        pinstruments[index++] = pcur;
        pcur += 8;

        if (index == *count)
            break;

        // make one more contract for next year
        if (cur_month == mon)
        {
            year_month = cur_year + 1;
            year_month = (year_month % 100) * 100 + mon;
            generate_contract(instrument, exchange, code, year_month);

            strncpy(pcur, instrument, 8);
            pinstruments[index++] = pcur;
            pcur += 8;
            // fprintf(stderr, "%s ", instrument);

            if (index == *count)
                break;
        }
    }

    *count = index;

    // fprintf(stderr, "\n");
    return 0;
}

int generate_contracts_cffex(contract_generator_t* contr_gen,
    char* pinstruments[], int* count, const char* code)
{
    int lYear, lMonth;
    int lYearMM[5] = { 0 };
    get_year_month(&lYear, &lMonth);
    get_cffex_instrument_yymm(lYearMM, lYear, lMonth);

    // int lCount = 0;
    int lUsed = 0;
    char lBuffer[64] = "";
    const char* lpVarietyArr[] = { "IF", "IC", "IH", "T", "TF", "TS", "TT", NULL };

    int index = 0;
    char* pcur = contr_gen->buff;

    for (int i = 0; lpVarietyArr[i]; ++i)
    {
        if (code && strcmp(code, lpVarietyArr[i]) != 0) {
            continue;
        }

        for (int k = 0; k < 5 && lYearMM[k]; ++k)
        {
            char* lpInstr = lBuffer;

            lUsed += sprintf(lpInstr, "%s%04d", lpVarietyArr[i], lYearMM[k]);
            lUsed++;

            // lCount++;
            // vec.push_back(string(lpInstr));

            strncpy(pcur, lpInstr, 8);
            pinstruments[index++] = pcur;
            pcur += 8;

            if (index == *count)
                break;
        }
    }

    *count = index;

    return 0;
}



static void get_year_month(int* apYear, int* apMonth)
{
    time_t lNow = time(0);
    struct tm* ptm = localtime(&lNow);

    *apYear = ptm->tm_year + 1900;
    *apMonth = ptm->tm_mon + 1;
}

static int the_next_yymm(int aYYMM)
{
    int lRet;
    int lYear = aYYMM / 100;
    int lMonth = aYYMM % 100;
    if (lMonth == 12) {
        lYear += 1;
        lMonth = 1;
    }
    else {
        lMonth += 1;
    }

    lRet = lYear * 100 + lMonth;
    return lRet;
}

static void get_cffex_instrument_yymm(int aYYMMArr[], int aYear, int aMonth)
{
    static int lSeasonMonth1[] = { 0, 3, 6, 6, 6, 9, 9, 9, 12, 12, 12, 103, 103 };
    static int lSeasonMonth2[] = { 0, 6, 9, 9, 9, 12, 12, 12, 103, 103, 103, 106, 106 };

    int lSeasonMonA = lSeasonMonth1[aMonth];
    int lSeasonMonB = lSeasonMonth2[aMonth];

    aYear %= 100;
    aYear *= 100;

    aYYMMArr[0] = aYear + aMonth;
    aYYMMArr[1] = the_next_yymm(aYYMMArr[0]);
    aYYMMArr[2] = the_next_yymm(aYYMMArr[1]);

    // the next season
    aYYMMArr[3] = aYear + lSeasonMonA;
    // the next next season
    aYYMMArr[4] = aYear + lSeasonMonB;

    if (aYYMMArr[2] == aYYMMArr[3])
    {
        aYYMMArr[3] = aYYMMArr[4];

        // make a more next season month
        if (lSeasonMonB == 12) {
            aYYMMArr[4] = (aYYMMArr[3] / 100 + 1) * 100 + 3;
        }
        else {
            aYYMMArr[4] = aYYMMArr[3] + 3;
        }
    }
}
