#include <stdarg.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>
#include <iostream>
#include <codecvt>
#include <locale>
// #include <condition_variable>

#ifdef _MSC_VER
#include <Windows.h>
#define sleepms(x)  Sleep((x))
#define get_tid     GetCurrentThreadId
#else
#include <sys/time.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>
#include <iconv.h>
#define sleepms(x)  usleep((x) * 1000)
#define get_tid     pthread_self

int code_convert(char* from, char* to, char* inbuf, size_t inlen, char* outbuf, size_t outlen)
{
    iconv_t cd;
    char** pin = &inbuf;
    char** pout = &outbuf;

    cd = iconv_open(to, from);
    if (cd == 0)
        return -1;
    if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
        return -1;
    iconv_close(cd);
    return 0;
}

#endif

#include "xcmd2.h"


#define XC_MDAPI_VERSION    "2.0.2"
#define XC_SPEC_LOG_LV      11
#define XC_LIMIT_LOG_LV     12


#define MY_DEBUG 0

#if (MY_DEBUG)
#define XcDebugInfo(fd,fmt,...) fprintf(fd,fmt,##__VA_ARGS__)
#else
static  inline void XcPrintNull(FILE* fd, const char* fmt, ...) { (void)fd; (void)fmt; }
#define XcDebugInfo(fd,fmt,...) XcPrintNull(fd,fmt,##__VA_ARGS__)
#endif//MY_DEBUG
#define XcDbgFd stderr


#define PRICE_DIV_STOCK         1000.0
#define PRICE_DIV_OPTION        10000.0
#define DEFAULT_MAX_REQ_SIZE    1000


static string to_utf(const string &gb2312)
{
#ifdef _MSC_VER
    const static locale loc("zh-CN");
    vector<wchar_t> wstr(gb2312.size());
    wchar_t* wstrEnd = nullptr;
    const char* gbEnd = nullptr;
    mbstate_t state = {};
    int res = use_facet<codecvt<wchar_t, char, mbstate_t> >
        (loc).in(state,
            gb2312.data(), gb2312.data() + gb2312.size(), gbEnd,
            wstr.data(), wstr.data() + wstr.size(), wstrEnd);

    if (codecvt_base::ok == res)
    {
        wstring_convert<codecvt_utf8<wchar_t>> cutf8;
        return cutf8.to_bytes(wstring(wstr.data(), wstrEnd));
    }

    return string();
#else
    // const static locale loc("zh_CN.GB18030");
    char outbuf[4080] = "";
    int outlen = sizeof(outbuf) - 1;
    int rv = code_convert("gb2312", "utf-8", (char*)gb2312.c_str(), (int)gb2312.length(), outbuf, outlen);
    if (rv == -1)
        return string();
    return string(outbuf);
#endif
}

static bool get_symbol_market(char instrument[], char market[], const std::string& symbol)
{
    std::size_t lDotIndex = symbol.find('.');
    if (lDotIndex == std::string::npos || lDotIndex < 1 || lDotIndex > 9)
    {
        return false;
    }

    // 000001.SZSE
    strncpy(instrument, symbol.c_str(), lDotIndex);
    strcpy(market, symbol.c_str() + lDotIndex + 1);

    return true;
}

static double get_price_div(const char* market)
{
    if (strcmp(market, "SSEOPT") == 0 || strcmp(market, "SZSEOPT") == 0)
        return PRICE_DIV_OPTION;
    return PRICE_DIV_STOCK;
}

#ifdef _MSC_VER
static void gettimeofday(struct timeval *tp, void* reserve)
{
    int64_t intervals;
    FILETIME ft;

    GetSystemTimeAsFileTime(&ft);

    /*
    * A file time is a 64-bit value that represents the number
    * of 100-nanosecond intervals that have elapsed since
    * January 1, 1601 12:00 A.M. UTC.
    *
    * Between January 1, 1970 (Epoch) and January 1, 1601 there were
    * 134744 days,
    * 11644473600 seconds or
    * 11644473600,000,000,0 100-nanosecond intervals.
    *
    * See also MSKB Q167296.
    */

    intervals = ((__int64)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    intervals -= 116444736000000000;

    tp->tv_sec = (long)(intervals / 10000000);
    tp->tv_usec = (long)((intervals % 10000000) / 10);
}

#endif//_MSC_VER

static const char* to_market_code(char exchange_id)
{
    if (exchange_id == Scdm_SSE || exchange_id == Scdm_SSEOPT || exchange_id == Scdm_SSE_Index)
        return "SSE";
    else if (exchange_id == Scdm_SZSE || exchange_id == Scdm_SZSEOPT || exchange_id == Scdm_SZSE_Index)
        return "SZSE";
    else
        return "";
}

static int to_sec_code(char seccode[], int stock_code)
{
    return sprintf(seccode, "%06d", stock_code);
}


XcMdApi::XcMdApi()
    : m_api()
    , m_mdmap()
    , m_secmap()
    , m_pendings()
    , m_Lock()
{
    m_pendings.reserve(1024);

    refid = 1;
    flush_count = 0;
    have_level10 = 0;
    log_level = 0;
    fp = NULL;
}

XcMdApi::~XcMdApi()
{
    if (fp)
    {
        fflush(fp);
        fclose(fp);
        fp = NULL;
    }
    if (m_active)
    {
        this->exit();
    }
}

void XcMdApi::OnUserLogin(void* apMsg, std::size_t Size)
{
    XC_PUBLIC_MSG_T* pMsg = (XC_PUBLIC_MSG_T*)apMsg;
    XcDebugInfo(XcDbgFd, "OnUserLogin:%s\n", pMsg->Desc);

    Task task = Task();
    task.task_name = ONUSERLOGIN;
    XC_PUBLIC_MSG_T* task_data = new XC_PUBLIC_MSG_T();
    *task_data = *pMsg;
    task.task_data = task_data;

    if (m_thread_mode)
    {
        m_task_queue.push(task);
    }
    else
    {
        processOnUserLogin(&task);
    }
}

void XcMdApi::OnHeart()
{
    // fprintf(stderr, "OnHeart\n");
}

void XcMdApi::OnClose()
{
    XcDebugInfo(XcDbgFd, "OnClose\n");

    Task task = Task();
    task.task_name = ONCLOSE;
    task.task_id = -1;

    if (m_thread_mode)
    {
        m_task_queue.push(task);
    }
    else
    {
        processOnClose(&task);
    }
}

void XcMdApi::OnIssueEnd(QWORD qQuoteID)
{
    XcDebugInfo(XcDbgFd, "OnIssueEnd qQuoteID:%ld\n", (long)qQuoteID);

    if (log_level == XC_SPEC_LOG_LV)
    {
        // write_data(log_level, "IssueEnd:%ld, pendings:%d", (long)qQuoteID, (int)m_pendings.size());
    }

    XcDepthMarketData* pMD;

    if (m_thread_mode)
    {
        for (int i = 0; i < (int)m_pendings.size(); ++i)
        {
            pMD = m_pendings[i];

            Task task = Task();
            task.task_name = ONRTNDEPTHMARKETDATA;
            task.task_id = (int)qQuoteID;
            XcDepthMarketData* lpMD = new XcDepthMarketData();
            *lpMD = *pMD;
            task.task_data = lpMD;
            m_task_queue.push(task);
        }
    }
    else
    {
        for (int i = 0; i < (int)m_pendings.size(); ++i)
        {
            pMD = m_pendings[i];
            Task task = Task();
            task.task_name = ONRTNDEPTHMARKETDATA;
            task.task_id = (int)qQuoteID;
            task.task_data = pMD;
            processOnRtnMarketData(&task, 1);
        }
    }

    // clear after notified all
    m_pendings.clear();
}

void XcMdApi::OnMsg(QWORD qRefID, void* apMsg, std::size_t Size)
{
    XC_PUBLIC_MSG_T* pMsg = (XC_PUBLIC_MSG_T*)apMsg;
    XcDebugInfo(XcDbgFd, "OnMsg qRefID:%ld, MsgId:%d, Msg:%s\n",
        (long)qRefID, pMsg->Msgid, pMsg->Desc);

    Task task = Task();
    task.task_name = ONMSG;
    task.task_id = (int)qRefID;
    XC_PUBLIC_MSG_T* task_data = new XC_PUBLIC_MSG_T();
    *task_data = *pMsg;
    task.task_data = task_data;

    if (m_thread_mode)
    {
        m_task_queue.push(task);
    }
    else
    {
        processOnMsg(&task);
    }
}

void XcMdApi::OnRespMarket(char StationId, QWORD qQuoteID, void* apMarket, std::size_t Size)
{
    XC_MARKETINFO_T* pMarket = (XC_MARKETINFO_T*)apMarket;
    XcDebugInfo(XcDbgFd, "OnRespMarket StationId:%c, qQuoteID:%ld, Code:%s, Name:%s\n",
        StationId, (long)qQuoteID, to_market_code(pMarket->ExchangeId), pMarket->MarketName);

    Task task = Task();
    task.task_name = ONRSPMARKET;
    task.task_id = (int)qQuoteID;
    XC_MARKETINFO_T* task_data = new XC_MARKETINFO_T();
    *task_data = *pMarket;
    task.task_data = task_data;

    if (m_thread_mode)
    {
        m_task_queue.push(task);
    }
    else
    {
        processOnRspMarket(&task);
    }
}

void XcMdApi::OnRespSecurity(char StationId, QWORD qQuoteID, char MarketCode, void* apSecurity, std::size_t Size)
{
    XcDebugInfo(XcDbgFd, "OnRespSecurity StationId:%c,qQuoteID:%ld,MarketCode:%c\n",
        StationId, (long)qQuoteID, MarketCode);

    if (MarketCode == Scdm_SZSE || MarketCode == Scdm_SZSEOPT)
    {
        OnRespSecurity_Sz(StationId, qQuoteID, MarketCode, apSecurity, Size);
    }
    else if (MarketCode == Scdm_SSE_Index || MarketCode == Scdm_SZSE_Index)
    {
        OnRespSecurity_Index(StationId, qQuoteID, MarketCode, apSecurity, Size);
    }
    else if (MarketCode == Scdm_SSEOPT)
    {
        OnRespSecurity_Opt(StationId, qQuoteID, MarketCode, apSecurity, Size);
    }
    else if (MarketCode == Scdm_SSE)
    {
        OnRespSecurity_SH(StationId, qQuoteID, MarketCode, apSecurity, Size);
    }
    else
    {
        fprintf(stderr, "OnRespSecurity unknown MarketCode:%c\n", MarketCode);
    }
}

#if 1
void XcMdApi::OnRespSecurity_SH(char StationId, QWORD qQuoteID, char MarketCode, void* apSecurity, std::size_t Size)
{
    // 上海A股证券行情响应
    // socket_struct_Security* pSecurity = (socket_struct_Security*)pParam;
    // socket_struct_Security_Extend* pExtend = (socket_struct_Security_Extend*)pSecurity->Extend_fields;

    XC_SECURITYINFO_SH_T* pSecurity = (XC_SECURITYINFO_SH_T*)apSecurity;
    XcDebugInfo(XcDbgFd, "OnRespSecurity Code:%s,Name:%s,PreClose:%.2lf, UpperLimit:%.2lf, LowerLimit:%.2lf\n",
        pSecurity->StockCode, pSecurity->SecName, pSecurity->SecurityClosePx, pSecurity->DailyPriceUpLimit, pSecurity->DailyPriceDownLimit);

    XcSecurityInfo lSecInfo;
    memset(&lSecInfo, 0, sizeof(lSecInfo));

    lSecInfo.iSecCode = atoi(pSecurity->StockCode);
    lSecInfo.cMarketCode = MarketCode;
    strcpy(lSecInfo.ExchangeID, to_market_code(pSecurity->ExchangeId));
    strcpy(lSecInfo.InstrumentID, pSecurity->StockCode);
    strcpy(lSecInfo.SecName, pSecurity->SecName);
    // strcpy(lSecInfo.SecType, pSecurity->SecType);
    strcpy(lSecInfo.StartDate, pSecurity->StartDate);
    strcpy(lSecInfo.EndDate, pSecurity->EndDate);
    strcpy(lSecInfo.SecurityStatusFlag, pSecurity->SecurityStatusFlag);
    strcpy(lSecInfo.Bz, pSecurity->Bz);
    lSecInfo.BuyUnit = (int)pSecurity->BuyUnit;
    lSecInfo.SellUnit = (int)pSecurity->SellUnit;
    lSecInfo.ExRightRatio = pSecurity->ExRightRatio;
    lSecInfo.DividendAmount = pSecurity->DividendAmount;
    lSecInfo.PreClosePrice = pSecurity->SecurityClosePx;
    lSecInfo.UpperLimitPrice = pSecurity->DailyPriceUpLimit;
    lSecInfo.LowerLimitPrice = pSecurity->DailyPriceDownLimit;

#if MD_MAP_STRING_KEY
    char buf[64] = "";
    sprintf(buf, "%s.%s", pSecurity->SecCode, pSecurity->MarketCode);
    string lXcSymbol(buf);
    set_sec_info(lXcSymbol, lSecInfo);
#else
    int lXcSymbol = atoi(pSecurity->StockCode);
    set_sec_info(lXcSymbol, lSecInfo);
#endif//MD_MAP_STRING_KEY

    int index = 32;
    while (index > 0)
    {
        if (lSecInfo.SecName[index] != ' ')
        {
            break;
        }
        lSecInfo.SecName[index] = '\0';
        --index;
    }

    Task task = Task();
    task.task_name = ONRSPQRYDATA;
    task.task_id = (int)qQuoteID;
    XcSecurityInfo* task_data = new XcSecurityInfo();
    *task_data = lSecInfo;
    task.task_data = task_data;

    if (m_thread_mode)
    {
        m_task_queue.push(task);
    }
    else
    {
        processOnRspQryData(&task);
    }
}

void XcMdApi::OnRespSecurity_Opt(char StationId, QWORD qQuoteID, char MarketCode, void* apSecurity, std::size_t Size)
{
    // 上海期权证券行情响应
    // socket_struct_Security_Opt_Extend* pExtend = (socket_struct_Security_Opt_Extend*)pSecurityOpt->Extend_fields;
    XC_SECURITYINFO_SHOP_T* pSecurityOpt = (XC_SECURITYINFO_SHOP_T*)apSecurity;

#if 0
    std::cout << "OnRespSecurity_Opt:" << std::endl
              << "StartDate:" << pSecurityOpt->StartDate << " EndDate:" << pSecurityOpt->EndDate << std::endl
              << " ContractID:" << pSecurityOpt->ContractID << " CallOrPut:" << pSecurityOpt->CallOrPut << std::endl
              << " ExpireDate:" << pSecurityOpt->ExpireDate << " OptionType:" << pSecurityOpt->OptionType << std::endl
              << " ExerciseDate:" << pSecurityOpt->ExerciseDate << " OptionType:" << pSecurityOpt->OptionType << std::endl
              << " UnderlyingSecCode:" << pSecurityOpt->UnderlyingSecCode << " UnderlyingSymbol:" << pSecurityOpt->UnderlyingSymbol << std::endl
              << std::endl;
#endif

    XcSecurityInfo lSecInfo;
    memset(&lSecInfo, 0, sizeof(lSecInfo));

    lSecInfo.iSecCode = atoi(pSecurityOpt->StockCode);
    lSecInfo.cMarketCode = MarketCode;
    strcpy(lSecInfo.ExchangeID, to_market_code(MarketCode));
    strcpy(lSecInfo.InstrumentID, pSecurityOpt->StockCode);
    strcpy(lSecInfo.SecName, pSecurityOpt->SecName);
    // strcpy(lSecInfo.SecType, pSecurityOpt->SecType);
    strcpy(lSecInfo.StartDate, pSecurityOpt->StartDate);
    strcpy(lSecInfo.EndDate, pSecurityOpt->EndDate);
    strcpy(lSecInfo.SecurityStatusFlag, pSecurityOpt->SecurityStatusFlag);
    lSecInfo.PreClosePrice = pSecurityOpt->SecurityClosePx;
    lSecInfo.UpperLimitPrice = pSecurityOpt->DailyPriceUpLimit;
    lSecInfo.LowerLimitPrice = pSecurityOpt->DailyPriceDownLimit;

    // strcpy(lSecInfo.SecType, pSecurityOpt->SecType);
    strcpy(lSecInfo.ContractID, pSecurityOpt->ContractID);
    strcpy(lSecInfo.Underlying, pSecurityOpt->UnderlyingSecCode);
    strcpy(lSecInfo.UnderlyingName, pSecurityOpt->UnderlyingSymbol);
    strcpy(lSecInfo.UnderlyingType, pSecurityOpt->UnderlyingType);
    strcpy(lSecInfo.OptionType, pSecurityOpt->OptionType);
    strcpy(lSecInfo.CallOrPut, pSecurityOpt->CallOrPut);
    strcpy(lSecInfo.StrikeDate, pSecurityOpt->ExerciseDate);
    strcpy(lSecInfo.DeliveryDate, pSecurityOpt->DeliveryDate);
    strcpy(lSecInfo.ExpireDate, pSecurityOpt->ExpireDate);
    strcpy(lSecInfo.UpdateVersion, pSecurityOpt->UpdateVersion);
    lSecInfo.Multiplier = (int)pSecurityOpt->ContractMultiplierUnit;
    lSecInfo.StrikePrice = pSecurityOpt->ExercisePrice;
    lSecInfo.TotalLongPosition = (uint32_t)pSecurityOpt->TotalLongPosition;
    lSecInfo.UnderlyingPreClose = pSecurityOpt->UnderlyingClosePx;
    lSecInfo.PreSettlePrice = pSecurityOpt->SettlePricePx;
    lSecInfo.MarginUnit = pSecurityOpt->MarginUnit;
    lSecInfo.MarginRatioParam1 = pSecurityOpt->MarginRatioParam1;
    lSecInfo.MarginRatioParam2 = pSecurityOpt->MarginRatioParam2;

#if MD_MAP_STRING_KEY
    char buf[64] = "";
    sprintf(buf, "%s.%s", pSecurityOpt->SecCode, pSecurityOpt->MarketCode);
    string lXcSymbol(buf);
    set_sec_info(lXcSymbol, lSecInfo);
#else
    int lXcSymbol = atoi(pSecurityOpt->StockCode);
    set_sec_info(lXcSymbol, lSecInfo);
#endif//MD_MAP_STRING_KEY

    int index = 32;
    while (index > 0)
    {
        if (lSecInfo.SecName[index] != ' ')
        {
            break;
        }
        lSecInfo.SecName[index] = '\0';
        --index;
    }

    Task task = Task();
    task.task_name = ONRSPQRYDATA;
    task.task_id = (int)qQuoteID;
    XcSecurityInfo* task_data = new XcSecurityInfo();
    *task_data = lSecInfo;
    task.task_data = task_data;

    if (m_thread_mode)
    {
        m_task_queue.push(task);
    }
    else
    {
        processOnRspQryData(&task);
    }
}

void XcMdApi::OnRespSecurity_Sz(char StationId, QWORD qQuoteID, char MarketCode, void* apSecurity, std::size_t Size)
{
    // 深圳A股证券行情响应
    // socket_struct_Security_Sz_Extend* pExtend = (socket_struct_Security_Sz_Extend*)pSecurity->Extend_fields;

    XC_SECURITYINFO_SZ_T* pSecurity = (XC_SECURITYINFO_SZ_T*)apSecurity;
    XcDebugInfo(XcDbgFd, "OnRespSecurity_Sz Code:%s,Name:%s,PreClose:%.2lf,T_LimitUpRate:%.4lf\n",
        pSecurity->StockCode, pSecurity->SecName, pSecurity->PrevClosePx, pSecurity->T_LimitUpRate);

    XcSecurityInfo lSecInfo;
    memset(&lSecInfo, 0, sizeof(lSecInfo));

    lSecInfo.iSecCode = atoi(pSecurity->StockCode);
    lSecInfo.cMarketCode = MarketCode;
    strcpy(lSecInfo.ExchangeID, to_market_code(MarketCode));
    strcpy(lSecInfo.InstrumentID, pSecurity->StockCode);
    strcpy(lSecInfo.SecName, pSecurity->SecName);
    // strcpy(lSecInfo.SecType, pSecurity->SecType);
    strcpy(lSecInfo.StartDate, pSecurity->ListDate);
    strcpy(lSecInfo.EndDate, pSecurity->MaturityDate);      // FIXME
    strcpy(lSecInfo.SecurityStatusFlag, pSecurity->SecurityStatus);

    lSecInfo.PreClosePrice = pSecurity->PrevClosePx;

    if (pSecurity->T_LimitUpRate < 0.0001)
    {
        double lLimitRate = 0.1;
        if (strstr(lSecInfo.SecName, "ST")) {
            lLimitRate = 0.05;
        }
        else if (strncmp(lSecInfo.InstrumentID, "30", 2) == 0) {
            lLimitRate = 0.2;
        }

        char buf[200] = "";
        sprintf(buf, "%.2lf", lSecInfo.PreClosePrice * (1 + lLimitRate));
        lSecInfo.UpperLimitPrice = atof(buf);

        sprintf(buf, "%.2lf", lSecInfo.PreClosePrice * (1 - lLimitRate));
        lSecInfo.LowerLimitPrice = atof(buf);
    }
    else
    {
        char buf[200] = "";
        sprintf(buf, "%.2lf", lSecInfo.PreClosePrice * (1 + pSecurity->T_LimitUpRate));
        lSecInfo.UpperLimitPrice = atof(buf);

        sprintf(buf, "%.2lf", lSecInfo.PreClosePrice * (1 - pSecurity->T_LimitDownRate));
        lSecInfo.LowerLimitPrice = atof(buf);
    }

    if (strcmp(lSecInfo.SecType, "SZOPT") == 0 || strcmp(lSecInfo.SecType, "OPT") == 0)
    {
        strcpy(lSecInfo.ContractID, pSecurity->EnglishName);
        strcpy(lSecInfo.Underlying, pSecurity->UnderlyingSecurityID);
        if (strcmp(lSecInfo.Underlying, "159919") == 0)
            strcpy(lSecInfo.UnderlyingName, "300ETF");
        // strcpy(lSecInfo.UnderlyingType, pSecurity->UnderlyingType);
        lSecInfo.OptionType[0] = pSecurity->ExerciseType;
        lSecInfo.CallOrPut[0] = pSecurity->CallOrPut;
        strcpy(lSecInfo.StrikeDate, pSecurity->ExerciseBeginDate);
        strcpy(lSecInfo.DeliveryDate, pSecurity->DeliveryDay);
        strcpy(lSecInfo.ExpireDate, pSecurity->LastTradeDay);
        lSecInfo.Multiplier = (int32_t)pSecurity->ContractUnit;
        lSecInfo.StrikePrice = pSecurity->ExercisePrice;
        lSecInfo.TotalLongPosition = (uint32_t)pSecurity->ContractPosition;
        // lSecInfo.UnderlyingPreClose = pSecurity->UnderlyingClosePx;
        lSecInfo.PreSettlePrice = pSecurity->PrevClearingPrice;

#if MD_MAP_STRING_KEY
        XcSecurityInfo* lpUnderlyingSecInfo = get_sec_info(atoi(lSecInfo.Underlying));
#else
        int lXcSymbol = atoi(pSecurity->StockCode);
        XcSecurityInfo* lpUnderlyingSecInfo = get_sec_info(lXcSymbol);
#endif//MD_MAP_STRING_KEY
        if (lpUnderlyingSecInfo)
        {
            lSecInfo.UnderlyingPreClose = lpUnderlyingSecInfo->PreClosePrice;
        }

        // lSecInfo.MarginUnit = pSecurity->MarginUnit;
        // lSecInfo.MarginRatioParam1 = pSecurity->MarginRatioParam1;
        // lSecInfo.MarginRatioParam2 = pSecurity->MarginRatioParam2;
    }

#if MD_MAP_STRING_KEY
    char buf[64] = "";
    sprintf(buf, "%s.%s", pSecurity->SecCode, pSecurity->MarketCode);
    string lXcSymbol(buf);
    set_sec_info(lXcSymbol, lSecInfo);
#else
    int lXcSymbol = atoi(pSecurity->StockCode);
    set_sec_info(lXcSymbol, lSecInfo);
#endif//MD_MAP_STRING_KEY

    int index = 20;
    while (index > 0)
    {
        if (lSecInfo.SecName[index] != ' ')
        {
            break;
        }
        lSecInfo.SecName[index] = '\0';
        --index;
    }

    Task task = Task();
    task.task_name = ONRSPQRYDATA;
    task.task_id = (int)qQuoteID;
    XcSecurityInfo* task_data = new XcSecurityInfo();
    *task_data = lSecInfo;
    task.task_data = task_data;

    if (m_thread_mode)
    {
        m_task_queue.push(task);
    }
    else
    {
        processOnRspQryData(&task);
    }
}

void XcMdApi::OnRespSecurity_Index(char StationId, QWORD qQuoteID, char MarketCode, void* apSecurity, std::size_t Size)
{
    XC_SECURITYINFO_INDEX_T* pSecurity = (XC_SECURITYINFO_INDEX_T*)apSecurity;
    XcDebugInfo(XcDbgFd, "OnRespSecurity_Index exchange:%c,code:%s,name:%s,pre_close:%.4lf\n",
        pSecurity->ExchangeId, pSecurity->IndexCode, pSecurity->IndexName, pSecurity->PreClose);

    XcSecurityInfo lSecInfo;    
    memset(&lSecInfo, 0, sizeof(lSecInfo));

    lSecInfo.iSecCode = atoi(pSecurity->IndexCode);
    lSecInfo.cMarketCode = MarketCode;
    strcpy(lSecInfo.ExchangeID, to_market_code(MarketCode));
    strcpy(lSecInfo.InstrumentID, pSecurity->IndexCode);
    strcpy(lSecInfo.SecName, pSecurity->IndexName);

    lSecInfo.PreClosePrice = pSecurity->PreClose;

#if MD_MAP_STRING_KEY
    char buf[64] = "";
    sprintf(buf, "%s.%s", pSecurity->IndexCode, lSecInfo.InstrumentID);
    string lXcSymbol(buf);
    set_sec_info(lXcSymbol, lSecInfo);
#else
    int lXcSymbol = atoi(pSecurity->IndexCode);
    set_sec_info(lXcSymbol, lSecInfo);
#endif//MD_MAP_STRING_KEY

    int index = 20;
    while (index > 0)
    {
        if (lSecInfo.SecName[index] != ' ')
        {
            break;
        }
        lSecInfo.SecName[index] = '\0';
        --index;
    }

    Task task = Task();
    task.task_name = ONRSPQRYDATA;
    task.task_id = (int)qQuoteID;
    XcSecurityInfo* task_data = new XcSecurityInfo();
    *task_data = lSecInfo;
    task.task_data = task_data;

    if (m_thread_mode)
    {
        m_task_queue.push(task);
    }
    else
    {
        processOnRspQryData(&task);
    }
}
#endif//0


void XcMdApi::OnRespDyna(char StationId, QWORD qQuoteID, DWORD SplitId, char MarketCode, void* apDyna, std::size_t Size)
{
    // 动态快照行情响应
    // socket_struct_Dyna* pDyna = (socket_struct_Dyna*)pParam;
    XC_DYNASHOT_T* pDyna = (XC_DYNASHOT_T*)apDyna;
    XcDepthMarketData* pMD;

    if (pDyna->PreClose == 0) {
        return;
    }

    XcDebugInfo(XcDbgFd, "OnRespDyna%ld: %s,%d,%d\n", (long)qQuoteID, pDyna->StockCode, pDyna->New, pDyna->Time);

    int32_t isfirst = 0;
#if MD_MAP_STRING_KEY
    char buf[64] = "";
    sprintf(buf, "%s.%s", pDyna->SecCode, pDyna->MarketCode);
    string lXcSymbol(buf);
    if (m_mdmap.count(lXcSymbol) == 0)
    {
        XcDepthMarketData lMD;
        memset(&lMD, 0, sizeof(lMD));
        // XcDebugInfo(XcDbgFd, "---->>>>%s,%s\n", pDyna->SecCode, pDyna->MarketCode);
        strcpy(lMD.InstrumentID, pDyna->SecCode);
        strcpy(lMD.ExchangeID, pDyna->MarketCode);
        lMD.PriceDiv = get_price_div(lMD.ExchangeID);
        m_mdmap[lXcSymbol] = lMD;

        isfirst = 1;
    }
    pMD = &m_mdmap[lXcSymbol];
#else
    int lXcSymbol = atoi(pDyna->StockCode);
    if (lXcSymbol == 0) {
        return;
    }
    if (m_mdmap.count(lXcSymbol) == 0)
    {
        XcDepthMarketData lMD;
        memset(&lMD, 0, sizeof(lMD));
        strcpy(lMD.InstrumentID, pDyna->StockCode);
        strcpy(lMD.ExchangeID, to_market_code(pDyna->ExchangeId));
        lMD.PriceDiv = get_price_div(lMD.ExchangeID);
        m_mdmap[lXcSymbol] = lMD;

        isfirst = 1;
    }
    pMD = &m_mdmap[lXcSymbol];
#endif//MD_MAP_STRING_KEY

#if 0
    // reset depth field ?
    pMD->BidPrice1 = 0;
    pMD->BidVolume1 = 0;
    pMD->BidPrice2 = 0;
    pMD->BidVolume2 = 0;
    pMD->BidPrice3 = 0;
    pMD->BidVolume3 = 0;
    pMD->BidPrice4 = 0;
    pMD->BidVolume4 = 0;
    pMD->BidPrice5 = 0;
    pMD->BidVolume5 = 0;
    pMD->AskPrice1 = 0;
    pMD->AskVolume1 = 0;
    pMD->AskPrice2 = 0;
    pMD->AskVolume2 = 0;
    pMD->AskPrice3 = 0;
    pMD->AskVolume3 = 0;
    pMD->AskPrice4 = 0;
    pMD->AskVolume4 = 0;
    pMD->AskPrice5 = 0;
    pMD->AskVolume5 = 0;
#endif//0

    double price_div = pMD->PriceDiv;
    pMD->PreClosePrice = pDyna->PreClose / price_div;
    pMD->LastPrice = pDyna->New / price_div;
    pMD->OpenPrice = pDyna->Open / price_div;
    pMD->HighestPrice = pDyna->High / price_div;
    pMD->LowestPrice = pDyna->Low / price_div;
    pMD->ClosePrice = pDyna->Close / price_div;

    pMD->Volume = pDyna->Volume;
    pMD->Turnover = pDyna->Amount / price_div;
    // pMD->OpenInterest = pDyna->OpenInt / price_div;

    int hhmmss, hour, minute, second;
    if (pDyna->Time > 10000000) {
        hhmmss = pDyna->Time / 1000;
        pMD->UpdateMillisec = hhmmss % 1000;
    }
    else {
        hhmmss = pDyna->Time;
        pMD->UpdateMillisec = 0;
    }
    hour = int(hhmmss / 10000);
    minute = int(hhmmss / 100) % 100;
    second = int(hhmmss % 100);
    sprintf(pMD->UpdateTime, "%02d:%02d:%02d", hour, minute, second);
    pMD->Time = pDyna->Time;
    pMD->TickCount = (uint32_t)pDyna->TickCount;
    // pMD->RefPrice = pDyna->RefPrice / price_div;
    pMD->IOPV = pDyna->IOPV / price_div;
    memcpy(pMD->TradingPhaseCode, pDyna->TradingPhaseCode, sizeof(pMD->TradingPhaseCode));

    XcSecurityInfo* psec_info = get_sec_info(lXcSymbol);
    if (psec_info)
    {
        pMD->UpperLimitPrice = psec_info->UpperLimitPrice;
        pMD->LowerLimitPrice = psec_info->LowerLimitPrice;
    }
    else
    {
        pMD->UpperLimitPrice = 0;
        pMD->LowerLimitPrice = 0;
    }

    strcpy(pMD->TradingDay, m_trading_day);
    strcpy(pMD->ActionDay, m_trading_day);

    // keep it firstly, and notify when issue end
    // m_pendings.push_back(pMD);

    if (log_level == XC_SPEC_LOG_LV && (hhmmss >= 91500 && hhmmss <= 153000))
    {
        write_data(log_level, "RespDyna %ld,%s,%s,%.2lf,%.2lf,%.2lf,%.2lf,%ld,%.2lf,%.2lf,%.2lf,bid:%.2lf,%d,ask:%.2lf,%d,n:%d",
            (long)qQuoteID, pMD->UpdateTime, pMD->InstrumentID, pMD->LastPrice, pMD->OpenPrice, pMD->HighestPrice, pMD->LowestPrice,
            (long)pMD->Volume, pMD->UpperLimitPrice, pMD->LowerLimitPrice, pMD->PreClosePrice, pMD->BidPrice1, pMD->BidVolume1,
            pMD->AskPrice1, pMD->AskVolume1, m_pendings.size());
    }
}

void XcMdApi::OnDynaEnd(char StationId, QWORD qQuoteID, DWORD SplitId, char MarketCode, int SecCode)
{
    XcDebugInfo(XcDbgFd, "OnDynaEnd qQuoteID:%ld,MarketCode:%c,SecCode:%d\n", (long)qQuoteID, MarketCode, SecCode);

    XcDepthMarketData* pMD;

    if (!m_mdmap.count(SecCode)) {
        return;
    }
    pMD = &m_mdmap[SecCode];

    if (m_thread_mode)
    {
        Task task = Task();
        task.task_name = ONRTNDEPTHMARKETDATA;
        task.task_id = (int)qQuoteID;
        XcDepthMarketData* lpMD = new XcDepthMarketData();
        *lpMD = *pMD;
        task.task_data = lpMD;
        m_task_queue.push(task);
    }
    else
    {
        Task task = Task();
        task.task_name = ONRTNDEPTHMARKETDATA;
        task.task_id = (int)qQuoteID;
        task.task_data = pMD;
        processOnRtnMarketData(&task, 1);
    }
}

void XcMdApi::OnRespDepth(char StationId, QWORD qQuoteID, DWORD SplitId, char MarketCode, int SecCode, void* apDepth, std::size_t Size)
{
    // 动态深度行情响应
    XC_DEPTH_T* pDepth = (XC_DEPTH_T*)apDepth;

    // XcDebugInfo(XcDbgFd, "Depth symbol:%s qid:%ld, grade:%d, px:%d, vol:%d\n",
    //     SecCode, (long)qQuoteID, pDepth->Grade, pDepth->Price, (int)pDepth->Quantity);

    XcDepthMarketData* pMD;

#if MD_MAP_STRING_KEY
    char buf[64] = "";
    sprintf(buf, "%s.%s", SecCode, MarketCode);
    string lXcSymbol(buf);
    if (m_mdmap.count(lXcSymbol) == 0)
    {
        return;
    }
    pMD = &m_mdmap[lXcSymbol];
#else
    int lXcSymbol = SecCode;
    if (m_mdmap.count(lXcSymbol) == 0)
    {
        return;
    }
    pMD = &m_mdmap[lXcSymbol];
#endif//MD_MAP_STRING_KEY

    if (!pMD) {
        return;
    }

    // double* dstpx;
    // int* dstvol;

    double price = pDepth->Price / pMD->PriceDiv;
    int    quantity = (int)pDepth->Quantity;

    switch (pDepth->PriceLevel)
    {
    case -1:
        pMD->BidPrice1 = price;
        pMD->BidVolume1 = quantity;
        break;
    case -2:
        pMD->BidPrice2 = price;
        pMD->BidVolume2 = quantity;
        break;
    case -3:
        pMD->BidPrice3 = price;
        pMD->BidVolume3 = quantity;
        break;
    case -4:
        pMD->BidPrice4 = price;
        pMD->BidVolume4 = quantity;
        break;
    case -5:
        pMD->BidPrice5 = price;
        pMD->BidVolume5 = quantity;
        break;
    case -6:
        pMD->BidPrice6 = price;
        pMD->BidVolume6 = quantity;
        break;
    case -7:
        pMD->BidPrice7 = price;
        pMD->BidVolume7 = quantity;
        break;
    case -8:
        pMD->BidPrice8 = price;
        pMD->BidVolume8 = quantity;
        break;
    case -9:
        pMD->BidPrice9 = price;
        pMD->BidVolume9 = quantity;
        break;
    case -10:
        pMD->BidPrice10 = price;
        pMD->BidVolume10 = quantity;
        break;
    case 1:
        pMD->AskPrice1 = price;
        pMD->AskVolume1 = quantity;
        break;
    case 2:
        pMD->AskPrice2 = price;
        pMD->AskVolume2 = quantity;
        break;
    case 3:
        pMD->AskPrice3 = price;
        pMD->AskVolume3 = quantity;
        break;
    case 4:
        pMD->AskPrice4 = price;
        pMD->AskVolume4 = quantity;
        break;
    case 5:
        pMD->AskPrice5 = price;
        pMD->AskVolume5 = quantity;
        break;
    case 6:
        pMD->AskPrice6 = price;
        pMD->AskVolume6 = quantity;
        break;
    case 7:
        pMD->AskPrice7 = price;
        pMD->AskVolume7 = quantity;
        break;
    case 8:
        pMD->AskPrice8 = price;
        pMD->AskVolume8 = quantity;
        break;
    case 9:
        pMD->AskPrice9 = price;
        pMD->AskVolume9 = quantity;
        break;
    case 10:
        pMD->AskPrice10 = price;
        pMD->AskVolume10 = quantity;
        break;
    default:
        break;
    }
}

void XcMdApi::OnRespDepthOrder(char StationId, QWORD qQuoteID, DWORD SplitId, char MarketCode, int SecCode, int Grade, DWORD Price, void* apDepthOrder, std::size_t Size)
{
    XC_DEPTHARRAY_T* pDepthOrder = (XC_DEPTHARRAY_T*)apDepthOrder;
    //if (Grade == 1)
    // cout << "out——DepthOrder:" << SecCode << " Grade: " << Grade << " OrderID: " << pDepthOrder->OrderID << " Qty: " << pDepthOrder->Quantity << endl;

    char lStockCode[16] = "";
    to_sec_code(lStockCode, SecCode);

    gil_scoped_acquire acquire;
    dict data;
    data["ExchangeID"] = to_utf(to_market_code(MarketCode));
    data["InstrumentID"] = to_utf(lStockCode);
    data["EntryId"] = pDepthOrder->EntryId;
    data["OrderId"] = pDepthOrder->OrderId;
    data["Quantity"] = pDepthOrder->Quantity;

    // TODO:
    (void)pDepthOrder->Buffer;

    this->on_rtn_depth_order(data);
}

void XcMdApi::OnRespEachOrder(char StationId, QWORD qQuoteID, char MarketCode, int SecCode, void* apEachOrder, std::size_t Size)
{
    // 逐笔委托
    // socket_struct_EachOrder* pEachOrder = (socket_struct_EachOrder*)pParam;
    XC_EACHENTRUST_T* pEachOrder = (XC_EACHENTRUST_T*)apEachOrder;

    char lTemp[8] = "";
    char lStockCode[16] = "";
    to_sec_code(lStockCode, SecCode);

    gil_scoped_acquire acquire;
    dict data;
    data["ExchangeID"] = to_utf(to_market_code(pEachOrder->ExchangeId));
    data["InstrumentID"] = to_utf(lStockCode);
    if (pEachOrder->ExchangeId == Scdm_SZSE || pEachOrder->ExchangeId == Scdm_SSE)
    {
        data["Price"] = pEachOrder->Price / PRICE_DIV_STOCK;
        data["Amount"] = pEachOrder->Amount / PRICE_DIV_STOCK;
        lTemp[0] = pEachOrder->OrderType;
        data["OrderType"] = to_utf(lTemp);
    }
    else
    {
        data["Price"] = pEachOrder->Price / PRICE_DIV_OPTION;
        data["Amount"] = pEachOrder->Amount / PRICE_DIV_OPTION;
    }
    data["Volume"] = pEachOrder->Volume;
    data["OrderNo"] = pEachOrder->OrderNo;

    lTemp[0] = pEachOrder->Side;
    data["Side"] = to_utf(lTemp);
    data["Time"] = pEachOrder->Time;
    this->on_rtn_each_order(data);
}

void XcMdApi::OnRespEachDeal(char StationId, QWORD qQuoteID, char MarketCode, int SecCode, void* apEachDeal, std::size_t Size)
{
    // 逐笔成交
    // socket_struct_EachDeal* pEachDeal = (socket_struct_EachDeal*)pParam;
    XC_EACHBUSINESS_T* pEachDeal = (XC_EACHBUSINESS_T*)apEachDeal;

    char lTemp[8] = "";
    char lStockCode[16] = "";
    to_sec_code(lStockCode, SecCode);

    gil_scoped_acquire acquire;
    dict data;
    data["ExchangeID"] = to_utf(to_market_code(pEachDeal->ExchangeId));
    data["InstrumentID"] = to_utf(lStockCode);

    if (pEachDeal->ExchangeId == Scdm_SZSE || pEachDeal->ExchangeId == Scdm_SSE)
    {
        data["Price"] = pEachDeal->Price / PRICE_DIV_STOCK;
        data["Amount"] = pEachDeal->Amount / PRICE_DIV_STOCK;

        lTemp[0] = pEachDeal->BsFlag;
        data["BsFlag"] = to_utf(lTemp);
    }
    else
    {
        data["Price"] = pEachDeal->Price / PRICE_DIV_OPTION;
        data["Amount"] = pEachDeal->Amount / PRICE_DIV_OPTION;
    }
    data["Volume"] = pEachDeal->Volume;
    data["BuyNo"] = pEachDeal->BuyNo;
    data["SellNo"] = pEachDeal->SellNo;
    data["BuyNo"] = pEachDeal->BuyNo;
    data["DealNo"] = pEachDeal->DealNo;
    lTemp[0] = pEachDeal->Type;     // 成交类型 F：成交 4：撤销
    data["Type"] = to_utf(lTemp);
    data["Time"] = pEachDeal->Time;

    this->on_rtn_each_deal(data);
}


//////////////////////////////////////////////////////////////////////////

int XcMdApi::set_thread_mode(int on)
{
    if (m_active)
    {
        return -1;
    }
    m_thread_mode = on;
    return 0;
}

void XcMdApi::exit()
{
    if (m_thread_mode && m_active)
    {
        m_active = false;
        m_task_queue.terminate();
        m_task_thread.join();
    }
}

int XcMdApi::set_connect_param(int is_reconnect, int reconnect_ms, int reconect_count)
{
    return SetConnectParam(_Protocol_Type::_Xc, is_reconnect, reconnect_ms);
}

int XcMdApi::subscribe_md(const std::string& symbol, int depth_order, int each_flag)
{
    std::vector<std::string> lreqs;
    lreqs.push_back(symbol);
    return Subscribe(12, lreqs, "", "", "");
}

int XcMdApi::req_qry_data(const std::string& symbol)
{
    return Require(14, symbol);
}

void XcMdApi::set_have_level10(int on)
{
    have_level10 = on;
}

string XcMdApi::get_api_version()
{
    return string(XC_MDAPI_VERSION);
}

int XcMdApi::open_debug(std::string log_file, int log_level)
{
    if (log_file.empty())
    {
        return -1;
    }

    if (fp)
    {
        fclose(fp);
        fp = NULL;
    }

    fp = fopen(log_file.c_str(), "a+");
    if (!fp)
    {
        return -2;
    }

    this->log_level = log_level;

    // write header line
    write_data(log_level, "xcmd version: %s\n", XC_MDAPI_VERSION);
    write_data(log_level, "%s\n", COLUMNS_NAMES);

    return 0;
}

int XcMdApi::write_line(int reserve, const std::string& line)
{
    (void)reserve;
    if (!fp)
    {
        return -1;
    }

#if 1
    char buf[1000] = "";
    int  len = 0;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    // time_t now = time(NULL);
    time_t now = tv.tv_sec;
    struct tm* ptm = localtime(&now);
    len = sprintf(buf + len, "[%04d-%02d-%02d %02d:%02d:%02d.%06d] ",
        ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
        ptm->tm_hour, ptm->tm_min, ptm->tm_sec, (int)tv.tv_usec);

    fwrite(buf, len, 1, fp);
    fwrite(line.c_str(), line.length(), 1, fp);
    fwrite("\n", 1, 1, fp);
    fflush(fp);
#endif
    return 0;
}

int XcMdApi::write_data(int reserve, const char* fmt, ...)
{
    (void)reserve;
    if (!fp)
    {
        return -1;
    }

#if 1
    char buf[16300] = "";
    int  len = 0;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    // time_t now = time(NULL);
    time_t now = tv.tv_sec;
    struct tm* ptm = localtime(&now);
    len = sprintf(buf + len, "[%04d-%02d-%02d %02d:%02d:%02d.%06d] ",
        ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
        ptm->tm_hour, ptm->tm_min, ptm->tm_sec, (int)tv.tv_usec);

    va_list args;
    va_start(args, fmt);
    len += vsnprintf(buf + len, sizeof(buf) - len - 2, fmt, args);
    va_end(args);
    // buf[len++] = '\r';
    buf[len++] = '\n';
    buf[len] = '\0';

    fwrite(buf, len, 1, fp);
    flush_count += 1;
    if ((flush_count & 127) == 0)
        fflush(fp);
#endif
    return 0;
}


//////////////////////////////////////////////////////////////////////////
int XcMdApi::CreateMarketApi()
{
    if (!m_api)
    {
#ifdef _MSC_VER
        m_api = CXcMarketApi::CreateMarketApi();
#else
        typedef CXcMarketApi*(*p_Create)();
        p_Create API_Create = NULL;
        void* hdll = dlopen("libXcMarketApi.so", RTLD_LAZY);
        if (!hdll) {
            fprintf(stderr, "xcmd2 dlopen('libXcMarketApi.so') failed!\n");
            return -100;
        }
        API_Create = (p_Create)dlsym(hdll, "CreateMarketApi");
        if (!API_Create) {
            fprintf(stderr, "xcmd2 dlsym('CreateMarketApi') failed!\n");
            return -101;
        }
        m_api = API_Create();
#endif//_MSC_VER
        if (!m_api) {
            return -1;
        }

        int tid = (int)get_tid();
        write_data(log_level, "CreateMarketApi m_api:%p, tid:%d", m_api, tid);
        return 0;
    }
    return 1;
}

int XcMdApi::Register(const std::string& user_id, const std::string& license)
{
    return m_api->Register(user_id.c_str(), this, license.c_str());
}

int XcMdApi::Release()
{
    int tid = (int)get_tid();
    write_data(log_level, "Release m_api:%p, tid:%d", m_api, tid);

    if (m_api)
    {
        m_api->Release();
        m_api = NULL;
    }

    if (fp)
    {
        fflush(fp);
        fclose(fp);
        fp = NULL;
    }
    return 0;
}

int XcMdApi::Connect(std::string server_addr)
{
    if (server_addr.find("://") == server_addr.npos)
    {
        char lbuffer[1024] = "";
        sprintf(lbuffer, "tcp://%s", server_addr.c_str());
        server_addr = lbuffer;
    }


    if (m_thread_mode && !m_active)
    {
        m_active = true;
        m_task_thread = std::thread(&XcMdApi::processTask, this);
    }


    return m_api->Connect(server_addr.c_str());
}

int XcMdApi::SetConnectParam(int aProtocolType, int aReconnect, int aReconnectTime)
{
    if (aProtocolType != _Protocol_Type::_Xc && aProtocolType != _Protocol_Type::_Old && aProtocolType != _Protocol_Type::_Protobuf)
    {
        fprintf(stderr, "XcMdApi::SetConnectParam unknown protocoltype:%d\n", aProtocolType);
        return -100;
    }

    int rv = m_api->SetConnectParam(_Connect_Type::_OutSide, _CallBack_Type::_Noraml, (_Protocol_Type)aProtocolType,
                aReconnect ? true : false, aReconnectTime);
    return rv;
}


static int _make_subscribe_detail(XC_PUBLIC_SUBSCRIBE_DETAIL_T* sub_detail, const char* symbol)
{
    char exchange_id;
    char market[32] = "";
    char instrument[32] = "";
    if (!get_symbol_market(instrument, market, symbol))
    {
        return -11;
    }

    if (strcmp(market, "SSE") == 0 || strcmp(market, "SSEOPT") == 0)
    {
        if (strlen(instrument) == 8) {
            exchange_id = Scdm_SSEOPT;
        }
        if (strncmp(instrument, "00", 2) == 0) {
            exchange_id = Scdm_SSE_Index;
        }
        else {
            exchange_id = Scdm_SSE;
        }
    }
    else
    {
        if (strlen(instrument) == 8) {
            exchange_id = Scdm_SZSEOPT;
        }
        if (strncmp(instrument, "39", 2) == 0) {
            exchange_id = Scdm_SZSE_Index;
        }
        else {
            exchange_id = Scdm_SZSE;
        }
    }

    sub_detail->ExchangeId = exchange_id;
    sub_detail->Stock = atoi(instrument);
    return 0;
}

static int _make_subscribe_flag(XC_INTERFACE_PUBLIC_SUBSCRIBE_T* sub_flag,
    const std::string& types, const std::string& topic, const std::string& station_id, int is_sub)
{
    if (topic.empty()) {
        sub_flag->Topic = Topic_Stock_Level2;
    }
    else {
        sub_flag->Topic = topic[0];
    }

    if (station_id.empty()) {
        sub_flag->Station = '1';
    }
    else {
        sub_flag->Station = station_id[0];
    }

    // 第一位：快照 / 第二位：逐笔委托 / 第三位：逐笔成交 / 第四位：快照拓展, 第十位：全订阅
    if (types.empty())
    {
        if (is_sub)
        {
            sub_flag->Type[0] = true;
            sub_flag->Type[3] = true;
        }
        else
        {
            sub_flag->Type[0] = false;
            sub_flag->Type[3] = false;
        }
    }
    else
    {
        for (std::size_t i = 0; i < types.length() && i < 10; ++i)
        {
            if (types[i] == '1')
                sub_flag->Type[i] = true;
            else
                sub_flag->Type[i] = false;
        }
    }

    return 0;
}

int XcMdApi::Subscribe(int aRefID, const std::vector<std::string>& req_symbols,
    const std::string& types, const std::string& topic, const std::string& station_id)
{
    int rv;
    int32_t count = 0;
    XC_INTERFACE_PUBLIC_SUBSCRIBE_T sub_flag;
    XC_PUBLIC_SUBSCRIBE_DETAIL_T sub_detail[DEFAULT_MAX_REQ_SIZE];

    for (int32_t i = 0; i < (int32_t)req_symbols.size() && i < DEFAULT_MAX_REQ_SIZE; ++i)
    {
        std::string symbol = req_symbols[i];
        rv = _make_subscribe_detail(&sub_detail[i], symbol.c_str());
        if (rv != 0)
        {
            fprintf(stderr, "Subscribe process symbol:%s failed!\n", symbol.c_str());
            continue;
        }

        XcSecurityInfo* lpSecInfo;
#if MD_MAP_STRING_KEY
        lpSecInfo = get_sec_info(symbol);
#else
        lpSecInfo = get_sec_info(atoi(symbol.c_str()));
#endif//0
        if (!lpSecInfo)
        {
            // auto do query security info
            Require(aRefID, symbol);
        }

        ++count;
    }

    rv = _make_subscribe_flag(&sub_flag, types, topic, station_id, 1);
    if (rv != 0)
    {
        fprintf(stderr, "Subscribe process subscribe flag failed!\n");
        return rv;
    }

    if (count)
    {
#if 0
        char lbuf[1024] = "";
        int len = sprintf(lbuf, "%d,%c, topic:%c,station:%c,Type:%d,%d\n",
            sub_detail[0].Stock, sub_detail[0].ExchangeId,
            sub_flag.Topic, sub_flag.Station, sub_flag.Type[0], sub_flag.Type[3]);
        fprintf(stderr, "Subscribe %s\n", lbuf);
#endif//0
        return m_api->Subscribe(refid++, &sub_flag, sub_detail, count);
    }
    return 0;
}

int XcMdApi::UnSubscribe(int aRefID, const std::vector<std::string>& req_symbols,
    const std::string& types, const std::string& topic, const std::string& station_id)
{
    int rv;
    int32_t count = 0;
    XC_INTERFACE_PUBLIC_SUBSCRIBE_T sub_flag;
    XC_PUBLIC_SUBSCRIBE_DETAIL_T sub_detail[DEFAULT_MAX_REQ_SIZE];

    for (int32_t i = 0; i < (int32_t)req_symbols.size() && i < DEFAULT_MAX_REQ_SIZE; ++i)
    {
        std::string symbol = req_symbols[i];
        rv = _make_subscribe_detail(&sub_detail[i], symbol.c_str());
        if (rv != 0)
        {
            fprintf(stderr, "UnSubscribe process symbol:%s failed!\n", symbol.c_str());
            continue;
        }
        ++count;
    }

    rv = _make_subscribe_flag(&sub_flag, types, topic, station_id, 0);
    if (rv != 0)
    {
        fprintf(stderr, "UnSubscribe process subscribe flag failed!\n");
        return rv;
    }

    if (count)
    {
        return m_api->UnSubscribe(refid++, &sub_flag, sub_detail, count);
    }
    return 0;
}

int XcMdApi::Require(int aRefID, const std::string& symbol)
{
    int rv;
    XC_PUBLIC_REQUIRE_DETAIL req_detail[1];
    XC_PUBLIC_SUBSCRIBE_DETAIL_T sub_detail[1];
    rv = _make_subscribe_detail(&sub_detail[0], symbol.c_str());
    if (rv != 0)
    {
        fprintf(stderr, "Require process symbol:%s failed!\n", symbol.c_str());
        return rv;
    }

    req_detail[0].ExchangeId = sub_detail[0].ExchangeId;
    req_detail[0].Stock = sub_detail[0].Stock;
    return m_api->Require(refid, req_detail, 1);
}

int XcMdApi::RequireBatch(int aRefID, const std::vector<std::string>& req_symbols)
{
    int rv;
    int32_t count = 0;
    XC_PUBLIC_REQUIRE_DETAIL req_details[DEFAULT_MAX_REQ_SIZE];

    for (int32_t i = 0; i < (int32_t)req_symbols.size() && i < DEFAULT_MAX_REQ_SIZE; ++i)
    {
        std::string symbol = req_symbols[i];
        XC_PUBLIC_SUBSCRIBE_DETAIL_T sub_detail[1];
        rv = _make_subscribe_detail(&sub_detail[0], symbol.c_str());
        if (rv != 0)
        {
            fprintf(stderr, "RequireBatch process symbol:%s failed!\n", symbol.c_str());
            continue;
        }

        req_details[count].ExchangeId = sub_detail[0].ExchangeId;
        req_details[count].Stock = sub_detail[0].Stock;
        ++count;
    }

    if (count)
    {
        return m_api->RequireBatch(refid++, req_details, count);
    }
    return 0;
}

std::string XcMdApi::GetLastErrorMsg()
{
    if (m_api)
    {
        const char* lpErrorMsg = m_api->GetLastErrorMsg();
        if (lpErrorMsg) {
            return to_utf(std::string(lpErrorMsg));
        }
    }
    return std::string("");
}

//////////////////////////////////////////////////////////////////////////
void XcMdApi::processTask()
{
    int count = 0;
    int tid = (int)get_tid();
    write_data(log_level, "xcmd2 thread task [%d] running", tid);

    try
    {
        while (m_active)
        {
            Task task = m_task_queue.pop();

            switch (task.task_name)
            {
            case ONUSERLOGIN:
            {
                this->processOnUserLogin(&task);
                break;
            }

            case ONCLOSE:
            {
                this->processOnClose(&task);
                break;
            }

            case ONMSG:
            {
                this->processOnMsg(&task);
                break;
            }

            case ONRSPMARKET:
            {
                this->processOnRspMarket(&task);
                break;
            }

            case ONRSPQRYDATA:
            {
                this->processOnRspQryData(&task);
                break;
            }

            case ONRTNDEPTHMARKETDATA:
            {
                this->processOnRtnMarketData(&task);
                break;
            }
            } // switch

            count += 1;
#if 0
            if (count & 62235 == 0)
            {
                write_data(log_level, "");
            }
#endif//0
        } // while
    }
    catch (const TerminatedError&)
    {
    }

    write_data(log_level, "xcmd2 thread task [%d] end", tid);
}

void XcMdApi::processOnUserLogin(Task *task)
{
    time_t now = time(NULL);
    struct tm* ptm = localtime(&now);
    char ltrading_day[16] = "";
    sprintf(ltrading_day, "%04d%02d%02d",
        (ptm->tm_year + 1900), (ptm->tm_mon + 1), ptm->tm_mday);

    if (strcmp(ltrading_day, m_trading_day) != 0)
    {
        m_mdmap.clear();
        m_secmap.clear();
        strcpy(m_trading_day, ltrading_day);
    }

    gil_scoped_acquire acquire;
    this->on_front_connected();

    XC_PUBLIC_MSG_T* pMsg = (XC_PUBLIC_MSG_T*)task->task_data;
    dict data;
    data["MsgID"] = pMsg->Msgid;
    data["Desc"] = to_utf(pMsg->Desc);
    delete pMsg;

    this->on_rsp_user_login(data);
}

void XcMdApi::processOnClose(Task *task)
{
    gil_scoped_acquire acquire;
    this->on_front_disconnected(task->task_id);
}

void XcMdApi::processOnMsg(Task *task)
{
    XC_PUBLIC_MSG_T* pMsg = (XC_PUBLIC_MSG_T*)task->task_data;

    gil_scoped_acquire acquire;
    dict data;
    data["MsgId"] = pMsg->Msgid;
    data["Desc"] = to_utf(pMsg->Desc);
    delete pMsg;
    this->on_msg(task->task_id, data);
}

void XcMdApi::processOnRspMarket(Task *task)
{
    XC_MARKETINFO_T* pMarket = (XC_MARKETINFO_T*)task->task_data;

    gil_scoped_acquire acquire;
    dict data;

    data["MarketCode"] = to_utf(to_market_code(pMarket->ExchangeId));
    data["MarketName"] = to_utf(pMarket->MarketName);
    data["TimeZone"] = pMarket->TimeZone;
    data["OpenTime"] = pMarket->OpenTime;
    data["CloseTime"] = pMarket->CloseTime;
    delete pMarket;
    this->on_rsp_market(task->task_id, data);
}

void XcMdApi::processOnRspQryData(Task *task)
{
    XcSecurityInfo* lpSecInfo = (XcSecurityInfo*)task->task_data;

    // TODO: special proc when index data

    gil_scoped_acquire acquire;
    dict data;
    data["ExchangeID"] = to_utf(lpSecInfo->ExchangeID);
    data["InstrumentID"] = to_utf(lpSecInfo->InstrumentID);
    data["SecName"] = to_utf(lpSecInfo->SecName);
    data["SecType"] = to_utf(lpSecInfo->SecType);
    data["StartDate"] = to_utf(lpSecInfo->StartDate);
    data["EndDate"] = to_utf(lpSecInfo->EndDate);
    data["PreClosePrice"] = lpSecInfo->PreClosePrice;
    data["UpperLimitPrice"] = lpSecInfo->UpperLimitPrice;
    data["LowerLimitPrice"] = lpSecInfo->LowerLimitPrice;
    data["SecurityStatusFlag"] = to_utf(lpSecInfo->SecurityStatusFlag);
    if (strcmp(lpSecInfo->ExchangeID, "SSE") == 0)
    {
        data["BuyUnit"] = lpSecInfo->BuyUnit;
        data["SellUnit"] = lpSecInfo->SellUnit;
        data["ExRightRatio"] = lpSecInfo->ExRightRatio;
        data["DividendAmount"] = lpSecInfo->DividendAmount;
    }
    else if (strcmp(lpSecInfo->ExchangeID, "SZSE") == 0)
    {
    }
    else
    {
        data["ContractID"] = to_utf(lpSecInfo->ContractID);
        data["Underlying"] = to_utf(lpSecInfo->Underlying);
        data["UnderlyingName"] = to_utf(lpSecInfo->UnderlyingName);
        data["UnderlyingType"] = to_utf(lpSecInfo->UnderlyingType);
        data["OptionType"] = to_utf(lpSecInfo->OptionType);
        data["CallOrPut"] = to_utf(lpSecInfo->CallOrPut);
        data["StrikeDate"] = to_utf(lpSecInfo->StrikeDate);
        data["DeliveryDate"] = to_utf(lpSecInfo->DeliveryDate);
        data["ExpireDate"] = to_utf(lpSecInfo->ExpireDate);
        data["UpdateVersion"] = to_utf(lpSecInfo->UpdateVersion);

        data["TotalLongPosition"] = lpSecInfo->TotalLongPosition;
        data["PreSettlePrice"] = lpSecInfo->PreSettlePrice;
        data["UnderlyingPreClose"] = lpSecInfo->UnderlyingPreClose;
        data["MarginUnit"] = lpSecInfo->MarginUnit;
        data["MarginRatioParam1"] = lpSecInfo->MarginRatioParam1;
        data["MarginRatioParam2"] = lpSecInfo->MarginRatioParam2;
    }

    delete lpSecInfo;
    this->on_rsp_qry_data(data);
}

void XcMdApi::processOnRtnMarketData(Task *task, int isIOThread)
{
    XcDepthMarketData* pMD = (XcDepthMarketData*)task->task_data;

    gil_scoped_acquire acquire;
    dict data;
    data["TradingDay"] = to_utf(pMD->TradingDay);
    data["InstrumentID"] = to_utf(pMD->InstrumentID);
    data["ExchangeID"] = to_utf(pMD->ExchangeID);
    data["PreClosePrice"] = pMD->PreClosePrice;
    data["LastPrice"] = pMD->LastPrice;
    data["OpenPrice"] = pMD->OpenPrice;
    data["HighestPrice"] = pMD->HighestPrice;
    data["LowestPrice"] = pMD->LowestPrice;
    data["ClosePrice"] = pMD->ClosePrice;
    data["Volume"] = pMD->Volume;
    data["Turnover"] = pMD->Turnover;
    data["OpenInterest"] = pMD->OpenInterest;
    data["UpdateTime"] = to_utf(pMD->UpdateTime);
    data["UpdateMillisec"] = pMD->UpdateMillisec;
    data["ActionDay"] = to_utf(pMD->ActionDay);
    data["TradingPhaseCode"] = to_utf(pMD->TradingPhaseCode);

    data["Time"] = pMD->Time;
    data["TickCount"] = pMD->TickCount;
    data["RefPrice"] = pMD->RefPrice;
    data["IOPV"] = pMD->IOPV;

    data["UpperLimitPrice"] = pMD->UpperLimitPrice;
    data["LowerLimitPrice"] = pMD->LowerLimitPrice;

    data["BidPrice1"] = pMD->BidPrice1;
    data["BidVolume1"] = pMD->BidVolume1;
    data["BidPrice2"] = pMD->BidPrice2;
    data["BidVolume2"] = pMD->BidVolume2;
    data["BidPrice3"] = pMD->BidPrice3;
    data["BidVolume3"] = pMD->BidVolume3;
    data["BidPrice4"] = pMD->BidPrice4;
    data["BidVolume4"] = pMD->BidVolume4;
    data["BidPrice5"] = pMD->BidPrice5;
    data["BidVolume5"] = pMD->BidVolume5;
    if (have_level10)
    {
        data["BidPrice6"] = pMD->BidPrice6;
        data["BidVolume6"] = pMD->BidVolume6;
        data["BidPrice7"] = pMD->BidPrice7;
        data["BidVolume7"] = pMD->BidVolume7;
        data["BidPrice8"] = pMD->BidPrice8;
        data["BidVolume8"] = pMD->BidVolume8;
        data["BidPrice9"] = pMD->BidPrice9;
        data["BidVolume9"] = pMD->BidVolume9;
        data["BidPrice10"] = pMD->BidPrice10;
        data["BidVolume10"] = pMD->BidVolume10;
    }

    data["AskPrice1"] = pMD->AskPrice1;
    data["AskVolume1"] = pMD->AskVolume1;
    data["AskPrice2"] = pMD->AskPrice2;
    data["AskVolume2"] = pMD->AskVolume2;
    data["AskPrice3"] = pMD->AskPrice3;
    data["AskVolume3"] = pMD->AskVolume3;
    data["AskPrice4"] = pMD->AskPrice4;
    data["AskVolume4"] = pMD->AskVolume4;
    data["AskPrice5"] = pMD->AskPrice5;
    data["AskVolume5"] = pMD->AskVolume5;
    if (have_level10)
    {
        data["AskPrice6"] = pMD->AskPrice6;
        data["AskVolume6"] = pMD->AskVolume6;
        data["AskPrice7"] = pMD->AskPrice7;
        data["AskVolume7"] = pMD->AskVolume7;
        data["AskPrice8"] = pMD->AskPrice8;
        data["AskVolume8"] = pMD->AskVolume8;
        data["AskPrice9"] = pMD->AskPrice9;
        data["AskVolume9"] = pMD->AskVolume9;
        data["AskPrice10"] = pMD->AskPrice10;
        data["AskVolume10"] = pMD->AskVolume10;
    }
    
    this->on_rtn_market_data(data);
    if (!isIOThread)
    {
        delete pMD;
    }
}


#if MD_MAP_STRING_KEY
XcSecurityInfo* XcMdApi::get_sec_info(const std::string& symbol)
{
    std::lock_guard<std::mutex> lk(m_Lock);
    if (m_secmap.count(symbol))
        return &m_secmap[symbol];
    return NULL;
}

void set_sec_info(const std::string& symbol, const XcSecurityInfo& pSecInfo)
{
    std::lock_guard<std::mutex> lk(m_Lock);
    m_secmap[symbol] = aSecInfo;
}

void XcMdApi::del_sec_info(const std::string& symbol)
{
    std::lock_guard<std::mutex> lk(m_Lock);
    m_secmap.erase(symbol);
}
#else
XcSecurityInfo* XcMdApi::get_sec_info(int isymbol)
{
    std::lock_guard<std::mutex> lk(m_Lock);
    if (m_secmap.count(isymbol))
        return &m_secmap[isymbol];
    return NULL;
}

void XcMdApi::set_sec_info(int isymbol, const XcSecurityInfo& aSecInfo)
{
    std::lock_guard<std::mutex> lk(m_Lock);
    m_secmap[isymbol] = aSecInfo;
}

void XcMdApi::del_sec_info(int isymbol)
{
    std::lock_guard<std::mutex> lk(m_Lock);
    m_secmap.erase(isymbol);
}
#endif//MD_MAP_STRING_KEY


///-------------------------------------------------------------------------------------
///Boost.Python封装
///-------------------------------------------------------------------------------------

class PyMdApi : public XcMdApi
{
public:
    using XcMdApi::XcMdApi;

    void on_front_connected() override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcMdApi, on_front_connected);
        }
        catch (const error_already_set &e)
        {
            cout << e.what() << endl;
        }
    };

    void on_front_disconnected(int reqid) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcMdApi, on_front_disconnected, reqid);
        }
        catch (const error_already_set &e)
        {
            cout << e.what() << endl;
        }
    };

    void on_rsp_user_login(const dict &data) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcMdApi, on_rsp_user_login, data);
        }
        catch (const error_already_set &e)
        {
            cout << e.what() << endl;
        }
    };

    void on_rsp_qry_data(const dict &data) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcMdApi, on_rsp_qry_data, data);
        }
        catch (const error_already_set &e)
        {
            cout << e.what() << endl;
        }
    };

    void on_rtn_market_data(const dict &data) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcMdApi, on_rtn_market_data, data);
        }
        catch (const error_already_set &e)
        {
            cout << e.what() << endl;
        }
    };

    void on_rtn_depth_order(const dict &data) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcMdApi, on_rtn_depth_order, data);
        }
        catch (const error_already_set &e)
        {
            cout << e.what() << endl;
        }
    };

    void on_rtn_each_order(const dict &data) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcMdApi, on_rtn_each_order, data);
        }
        catch (const error_already_set &e)
        {
            cout << e.what() << endl;
        }
    };

    void on_rtn_each_deal(const dict &data) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcMdApi, on_rtn_each_deal, data);
        }
        catch (const error_already_set &e)
        {
            cout << e.what() << endl;
        }
    };

    void on_msg(int ref_id, const dict &data) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcMdApi, on_msg, ref_id, data);
        }
        catch (const error_already_set &e)
        {
            cout << e.what() << endl;
        }
    };

    void on_rsp_market(int ref_id, const dict &data) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcMdApi, on_rsp_market, ref_id, data);
        }
        catch (const error_already_set &e)
        {
            cout << e.what() << endl;
        }
    };

};


PYBIND11_MODULE(xcmd2, m)
{
    class_<XcMdApi, PyMdApi> mdapi(m, "XcMdApi", module_local());
    mdapi
        .def(init<>())
        .def("set_thread_mode", &XcMdApi::set_thread_mode)
        .def("get_api_version", &XcMdApi::get_api_version)
        .def("open_debug", &XcMdApi::open_debug)
        .def("write_line", &XcMdApi::write_line)
        .def("set_have_level10", &XcMdApi::set_have_level10)
        .def("del_sec_info", &XcMdApi::del_sec_info)
        .def("create_md_api", &XcMdApi::CreateMarketApi)
        .def("create_api", &XcMdApi::CreateMarketApi)
        .def("release", &XcMdApi::Release)
        .def("set_connect_param", &XcMdApi::set_connect_param)
        .def("req_qry_data", &XcMdApi::Require)
        .def("req_qry_data_batch", &XcMdApi::RequireBatch)
        .def("get_last_error_msg", &XcMdApi::GetLastErrorMsg)

        // raw api names
        .def("CreateMarketApi", &XcMdApi::CreateMarketApi)
        .def("Release", &XcMdApi::Release)
        .def("Register", &XcMdApi::Register)
        .def("SetConnectParam", &XcMdApi::SetConnectParam)
        .def("Connect", &XcMdApi::Connect)
        .def("Require", &XcMdApi::Require)
        .def("RequireBatch", &XcMdApi::RequireBatch)
        .def("Subscribe", &XcMdApi::Subscribe)
        .def("UnSubscribe", &XcMdApi::UnSubscribe)
        .def("GetLastErrorMsg", &XcMdApi::GetLastErrorMsg)

        .def("OnClose", &XcMdApi::on_front_disconnected)
        .def("OnUserLogin", &XcMdApi::on_rsp_user_login)
        .def("OnRespMarket", &XcMdApi::on_rsp_market)
        .def("OnRespSecurity", &XcMdApi::on_rsp_qry_data)
        .def("OnRespDyna", &XcMdApi::on_rtn_market_data)
        .def("OnRespDepthOrder", &XcMdApi::on_rtn_depth_order)
        .def("OnRespEachOrder", &XcMdApi::on_rtn_each_order)
        .def("OnRespEachDeal", &XcMdApi::on_rtn_each_deal)
        .def("OnMsg", &XcMdApi::on_msg)

        .def("on_front_connected", &XcMdApi::on_front_connected)
        .def("on_front_disconnected", &XcMdApi::on_front_disconnected)
        .def("on_rsp_user_login", &XcMdApi::on_rsp_user_login)
        .def("on_rsp_market", &XcMdApi::on_rsp_market)
        .def("on_rsp_qry_data", &XcMdApi::on_rsp_qry_data)
        .def("on_rtn_market_data", &XcMdApi::on_rtn_market_data)
        .def("on_rtn_depth_order", &XcMdApi::on_rtn_depth_order)
        .def("on_rtn_each_order", &XcMdApi::on_rtn_each_order)
        .def("on_rtn_each_deal", &XcMdApi::on_rtn_each_deal)
        .def("on_msg", &XcMdApi::on_msg)
        ;
}
