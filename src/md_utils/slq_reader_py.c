#include <assert.h>
#include <string.h>

#include <Python.h>

#include "md_utils_py.h"

#include "slq_reader.h"



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

///�������
struct CThostFtdcDepthMarketDataField
{
    ///������
    TThostFtdcDateType	TradingDay;
    ///��Լ����
    TThostFtdcInstrumentIDType	InstrumentID;
    ///����������
    TThostFtdcExchangeIDType	ExchangeID;
    ///��Լ�ڽ������Ĵ���
    TThostFtdcExchangeInstIDType	ExchangeInstID;
    ///���¼�
    TThostFtdcPriceType	LastPrice;
    ///�ϴν����
    TThostFtdcPriceType	PreSettlementPrice;
    ///������
    TThostFtdcPriceType	PreClosePrice;
    ///��ֲ���
    TThostFtdcLargeVolumeType	PreOpenInterest;
    ///����
    TThostFtdcPriceType	OpenPrice;
    ///��߼�
    TThostFtdcPriceType	HighestPrice;
    ///��ͼ�
    TThostFtdcPriceType	LowestPrice;
    ///����
    TThostFtdcVolumeType	Volume;
    ///�ɽ����
    TThostFtdcMoneyType	Turnover;
    ///�ֲ���
    TThostFtdcLargeVolumeType	OpenInterest;
    ///������
    TThostFtdcPriceType	ClosePrice;
    ///���ν����
    TThostFtdcPriceType	SettlementPrice;
    ///��ͣ���
    TThostFtdcPriceType	UpperLimitPrice;
    ///��ͣ���
    TThostFtdcPriceType	LowerLimitPrice;
    ///����ʵ��
    TThostFtdcRatioType	PreDelta;
    ///����ʵ��
    TThostFtdcRatioType	CurrDelta;
    ///����޸�ʱ��
    TThostFtdcTimeType	UpdateTime;
    ///����޸ĺ���
    TThostFtdcMillisecType	UpdateMillisec;
    ///�����һ
    TThostFtdcPriceType	BidPrice1;
    ///������һ
    TThostFtdcVolumeType	BidVolume1;
    ///������һ
    TThostFtdcPriceType	AskPrice1;
    ///������һ
    TThostFtdcVolumeType	AskVolume1;
    ///����۶�
    TThostFtdcPriceType	BidPrice2;
    ///��������
    TThostFtdcVolumeType	BidVolume2;
    ///�����۶�
    TThostFtdcPriceType	AskPrice2;
    ///��������
    TThostFtdcVolumeType	AskVolume2;
    ///�������
    TThostFtdcPriceType	BidPrice3;
    ///��������
    TThostFtdcVolumeType	BidVolume3;
    ///��������
    TThostFtdcPriceType	AskPrice3;
    ///��������
    TThostFtdcVolumeType	AskVolume3;
    ///�������
    TThostFtdcPriceType	BidPrice4;
    ///��������
    TThostFtdcVolumeType	BidVolume4;
    ///��������
    TThostFtdcPriceType	AskPrice4;
    ///��������
    TThostFtdcVolumeType	AskVolume4;
    ///�������
    TThostFtdcPriceType	BidPrice5;
    ///��������
    TThostFtdcVolumeType	BidVolume5;
    ///��������
    TThostFtdcPriceType	AskPrice5;
    ///��������
    TThostFtdcVolumeType	AskVolume5;
    ///���վ���
    TThostFtdcPriceType	AveragePrice;
    ///ҵ������
    TThostFtdcDateType	ActionDay;
};

//��ȡ�ļ�����
int64_t MD_UTILS_STDCALL file_length(FILE *fp)
{
    long int num;
    fseek(fp, 0, SEEK_END);
    num = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return num;
}

char* MD_UTILS_STDCALL slq_read_content(const char* slq_name, int64_t* plen)
{
    FILE* fp = fopen(slq_name, "rb");
    if (NULL == fp) {
        return NULL;
    }
    int64_t length = file_length(fp);
    char* ch = (char *)malloc(length);
    if (!ch) {
        return NULL;
    }

    fread(ch, length, 1, fp);
    *(ch + length - 1) = '\0';
    *plen = length;
    fclose(fp);
    return ch;
}


int MD_UTILS_STDCALL slq_reader_init(slq_reader_t* sr, const char* slq_name, const char* trading_day)
{
    memset(sr, 0, sizeof(slq_reader_t));
    strncpy(sr->slq_name, slq_name, sizeof(sr->slq_name) - 1);
    strncpy(sr->trading_day, trading_day, sizeof(sr->trading_day) - 1);
    sr->buffer = slq_read_content(slq_name, &sr->length);
    if (!sr->buffer) {
        return -1;
    }
    sr->pcur = sr->buffer + PACK_HEAD;
    return 0;
}

int MD_UTILS_STDCALL slq_reader_release(slq_reader_t* sr)
{
    if (sr->buffer) {
        free(sr->buffer);
        sr->buffer = NULL;
    }
    return 0;
}


#define EXTRACT_VAL(p, type, offset)    (*(type*)((p) + (offset)))

int MD_UTILS_STDCALL slq_reader_next(slq_reader_t* sr, void* dst)
{
    struct CThostFtdcDepthMarketDataField* ctp_info;
    ctp_info = (struct CThostFtdcDepthMarketDataField*)dst;

    char* p = sr->pcur;
    int next_pack = ASK1VOLUME_OFFSET + 8 + 4 * 4 * sizeof(double) + PACK_INTERVAL;
    if (sr->offset + 2 * next_pack < sr->length)
    {
        strcpy(ctp_info->TradingDay, sr->trading_day);
        strcpy(ctp_info->InstrumentID, (p + INSTRUMENTID_OFFSET));

#if 1
        ctp_info->LastPrice = *(double*)(p + LASTPRICE_OFFSET);
        // 			strcpy(ctp_info->ExchangeID, p+EXGID_OFFSET);
        ctp_info->Volume = *(int*)(p + VOLUME_OFFSET);
        ctp_info->Turnover = *(double*)(p + TURNOVER_OFFSET);
        ctp_info->OpenInterest = *(double*)(p + OPENINTEREST_OFFSET);
        ctp_info->UpperLimitPrice = *(double*)(p + UPPERLIMITPRICE_OFFSET);
        ctp_info->LowerLimitPrice = *(double*)(p + LOWERLIMITPRICE_OFFSET);
        strcpy(ctp_info->UpdateTime, (p + UPDATETIME_OFFSET));
        ctp_info->UpdateMillisec = *(int*)(p + UPDATEMSEC_OFFSET);
        ctp_info->BidPrice1 = *(double*)(p + BID1PRICE_OFFSET);
        ctp_info->BidVolume1 = *(int*)(p + BID1VOLUME_OFFSET);
        ctp_info->AskPrice1 = *(double*)(p + ASK1PRICE_OFFSET);
        ctp_info->AskVolume1 = *(int*)(p + ASK1VOLUME_OFFSET);
#else
        ctp_info->LastPrice = EXTRACT_VAL(p, double, LASTPRICE_OFFSET);
        ctp_info->Volume = EXTRACT_VAL(p, int, VOLUME_OFFSET);
        ctp_info->Turnover = EXTRACT_VAL(p, double, TURNOVER_OFFSET);
        ctp_info->OpenInterest = EXTRACT_VAL(p, double, OPENINTEREST_OFFSET);
        ctp_info->UpperLimitPrice = EXTRACT_VAL(p, double, UPPERLIMITPRICE_OFFSET);
        ctp_info->LowerLimitPrice = EXTRACT_VAL(p, double, LOWERLIMITPRICE_OFFSET);
        strcpy(ctp_info->UpdateTime, (p + UPDATETIME_OFFSET));
        ctp_info->UpdateMillisec = EXTRACT_VAL(p, int, UPDATEMSEC_OFFSET);
        ctp_info->BidPrice1 = EXTRACT_VAL(p, double, BID1PRICE_OFFSET);
        ctp_info->BidVolume1 = EXTRACT_VAL(p, int, BID1VOLUME_OFFSET);
        ctp_info->AskPrice1 = EXTRACT_VAL(p, double, ASK1PRICE_OFFSET);
        ctp_info->AskVolume1 = EXTRACT_VAL(p, int, ASK1VOLUME_OFFSET);
#endif//0

        //printf("%s,%s,%.0f\n",ctp_info->InstrumentID,ctp_info->UpdateTime,ctp_info->LastPrice);

        //pָ���ڵ�ǰ�����ͷ������Ҫָ����һ�������ͷ������Ҫ����
        //(1) �����ĳ��� ASK1VOLUME_OFFSET + 8
        //(2) 4�����������ֽ��� 4 * 4 * 8
        //(3) �����ļ��PACK_INTERVAL
        sr->pcur += next_pack;
        sr->offset += next_pack;
        sr->count += 1;
        return 0;
    }

    return -1;
}


//////////////////////////////////////////////////////////////////////////

#if HAVE_PY_WRAPPER


PyObject* slqr_create(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    slq_reader_t* sr;
    const char* slq_name;
    const char* trading_day;
    PyObject* res;

    if (!PyArg_ParseTuple(args, "ss", &slq_name, &trading_day)) {
        return Py_None;
    }

    // fprintf(stderr, "slq_reader_create %s,%s\n", exchange, instrument);

    sr = (slq_reader_t*)malloc(sizeof(slq_reader_t));
    slq_reader_init(sr, slq_name, trading_day);

    v.ptr = sr;
    res = Py_BuildValue("L", v.i64);
    return res;
}

PyObject* slqr_release(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    slq_reader_t* sr;

    if (!PyArg_ParseTuple(args, "L", &v.i64)) {
        return Py_BuildValue("i", -1);
    }

    sr = (slq_reader_t*)v.ptr;
    // fprintf(stderr, "slq_reader_release obj:%p\n", sr);

    if (sr) {
        slq_reader_release(sr);
        free(sr);
    }

    return Py_BuildValue("i", 0);
}

PyObject* slqr_count(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    slq_reader_t* sr;

    if (!PyArg_ParseTuple(args, "L", &v.i64)) {
        return Py_BuildValue("i", -1);
    }

    sr = (slq_reader_t*)v.ptr;
    if (sr) {
        return Py_BuildValue("i", sr->count);
    }

    return Py_BuildValue("i", 0);
}

PyObject* slqr_next(PyObject* self, PyObject* args)
{
    (void)self;
    md_union_dtype_t v;
    slq_reader_t* sr;
    PyObject* dp;
    dp = PyDict_New();

    if (!PyArg_ParseTuple(args, "L", &v.i64)) {
        return dp;
    }
    sr = (slq_reader_t*)v.ptr;
    if (!sr) {
        return dp;
    }

    struct CThostFtdcDepthMarketDataField ctp_info;
    memset(&ctp_info, 0, sizeof(ctp_info));
    if (slq_reader_next(sr, &ctp_info) == 0)
    {
        PyObject* dp;
        dp = PyDict_New();

        PyDict_SetItemString(dp, "InstrumentID", Py_BuildValue("s", ctp_info.InstrumentID));
        PyDict_SetItemString(dp, "TradingDay", Py_BuildValue("s", ctp_info.TradingDay));
        PyDict_SetItemString(dp, "LastPrice", Py_BuildValue("f", ctp_info.LastPrice));
        PyDict_SetItemString(dp, "Volume", Py_BuildValue("i", ctp_info.Volume));
        PyDict_SetItemString(dp, "Turnover", Py_BuildValue("f", ctp_info.Turnover));
        PyDict_SetItemString(dp, "OpenInterest", Py_BuildValue("i", ctp_info.OpenInterest));
        PyDict_SetItemString(dp, "UpperLimitPrice", Py_BuildValue("f", ctp_info.LowerLimitPrice));
        PyDict_SetItemString(dp, "LowerLimitPrice", Py_BuildValue("f", ctp_info.LowerLimitPrice));
        PyDict_SetItemString(dp, "UpdateTime", Py_BuildValue("s", ctp_info.UpdateTime));
        PyDict_SetItemString(dp, "UpdateMillisec", Py_BuildValue("i", ctp_info.UpdateMillisec));
        PyDict_SetItemString(dp, "BidPrice1", Py_BuildValue("f", ctp_info.BidPrice1));
        PyDict_SetItemString(dp, "BidVolume1", Py_BuildValue("i", ctp_info.BidVolume1));
        PyDict_SetItemString(dp, "AskPrice1", Py_BuildValue("f", ctp_info.AskPrice1));
        PyDict_SetItemString(dp, "AskVolume1", Py_BuildValue("i", ctp_info.AskVolume1));

        return dp;
    }

    return dp;
}

#endif//HAVE_PY_WRAPPER

