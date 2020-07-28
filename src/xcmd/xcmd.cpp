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
#else
#include <sys/time.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <iconv.h>
#define sleepms(x)  usleep((x) * 1000)

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

#include "xcmd.h"


#define XC_MDAPI_VERSION    "1.4.4"
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
#define DEFAULT_MAX_REQ_SIZE    256

#define IS_UP(ratio)            ((ratio) > 0.000001)
#define IS_DOWN(ratio)          ((ratio) < -0.000001)


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
void gettimeofday(struct timeval *tp, void* reserve)
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


XcMdApi::XcMdApi()
    : api()
    , mdmap()
    , secmap()
    , m_pendings()
    , m_SubList()
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

void XcMdApi::OnUserLogin(socket_struct_Msg* pMsg)
{
    /*	if(strstr(pMsg->Desc, "Successed"))
    cout << "用户登录成功:" << pMsg->Desc << endl;
    else
    cout << "用户登录失败:" << pMsg->Desc << endl;
    */
    // cout << "用户登录返回:" << pMsg->Desc << endl;
    XcDebugInfo(XcDbgFd, "OnUserLogin:%s\n", pMsg->Desc);

    Task task = Task();
    task.task_name = ONUSERLOGIN;
    socket_struct_Msg* task_data = new socket_struct_Msg();
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

#if HAVE_BAR_GENERATOR
    if (bar_mode)
    {
        return;
    }
#endif//HAVE_BAR_GENERATOR

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

void XcMdApi::OnMsg(QWORD qRefID, socket_struct_Msg* pMsg)
{
    XcDebugInfo(XcDbgFd, "OnMsg qRefID:%ld, MsgId:%d, Msg:%s\n", (long)qRefID, pMsg->Msgid, pMsg->Desc);

    Task task = Task();
    task.task_name = ONMSG;
    task.task_id = (int)qRefID;
    socket_struct_Msg* task_data = new socket_struct_Msg();
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

void XcMdApi::OnRespMarket(QWORD qQuoteID, socket_struct_Market* pMarket)
{
    XcDebugInfo(XcDbgFd, "OnRespMarket qQuoteID:%ld, Code:%s, Name:%s\n",
        (long)qQuoteID, pMarket->MarketCode, pMarket->MarketName);

    Task task = Task();
    task.task_name = ONRSPMARKET;
    task.task_id = (int)qQuoteID;
    socket_struct_Market* task_data = new socket_struct_Market();
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

void XcMdApi::OnRespSecurity(QWORD qQuoteID, void* pParam)
{
    // 上海A股证券行情响应
    socket_struct_Security* pSecurity = (socket_struct_Security*)pParam;
    // socket_struct_Security_Extend* pExtend = (socket_struct_Security_Extend*)pSecurity->Extend_fields;
    XcDebugInfo(XcDbgFd, "OnRespSecurity Code:%s,Name:%s,PreClose:%.2lf, UpperLimit:%.2lf, LowerLimit:%.2lf\n",
        pSecurity->SecCode, pSecurity->SecName, pSecurity->SecurityClosePx, pSecurity->DailyPriceUpLimit, pSecurity->DailyPriceDownLimit);

    XcSecurityInfo lSecInfo;
    memset(&lSecInfo, 0, sizeof(lSecInfo));
    strcpy(lSecInfo.ExchangeID, pSecurity->MarketCode);
    strcpy(lSecInfo.InstrumentID, pSecurity->SecCode);
    strcpy(lSecInfo.SecName, pSecurity->SecName);
    strcpy(lSecInfo.SecType, pSecurity->SecType);
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
    secmap[lXcSymbol] = lSecInfo;
#else
    int lXcSymbol = atoi(pSecurity->SecCode);
    secmap[lXcSymbol] = lSecInfo;
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

    // only keep the AShares
    if (0 && lSecInfo.InstrumentID[0] == '6')
    {
        socket_struct_SubscribeDetail sub;
        strcpy(sub.MarketCode, lSecInfo.ExchangeID);
        strcpy(sub.SecCode, lSecInfo.InstrumentID);

        std::lock_guard<std::mutex> lk(m_Lock);
        m_SubList.push_back(sub);
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

void XcMdApi::OnRespSecurity_Opt(QWORD qQuoteID, void* pParam)
{
    // 上海期权证券行情响应
    socket_struct_Security_Opt* pSecurityOpt = (socket_struct_Security_Opt*)pParam;
    socket_struct_Security_Opt_Extend* pExtend = (socket_struct_Security_Opt_Extend*)pSecurityOpt->Extend_fields;

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
    strcpy(lSecInfo.ExchangeID, pSecurityOpt->MarketCode);
    strcpy(lSecInfo.InstrumentID, pSecurityOpt->SecCode);
    strcpy(lSecInfo.SecName, pSecurityOpt->SecName);
    strcpy(lSecInfo.SecType, pSecurityOpt->SecType);
    strcpy(lSecInfo.StartDate, pSecurityOpt->StartDate);
    strcpy(lSecInfo.EndDate, pSecurityOpt->EndDate);
    strcpy(lSecInfo.SecurityStatusFlag, pSecurityOpt->SecurityStatusFlag);
    lSecInfo.PreClosePrice = pSecurityOpt->SecurityClosePx;
    lSecInfo.UpperLimitPrice = pSecurityOpt->DailyPriceUpLimit;
    lSecInfo.LowerLimitPrice = pSecurityOpt->DailyPriceDownLimit;

    strcpy(lSecInfo.SecType, pSecurityOpt->SecType);
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
    secmap[lXcSymbol] = lSecInfo;
#else
    int lXcSymbol = atoi(pSecurityOpt->SecCode);
    secmap[lXcSymbol] = lSecInfo;
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

void XcMdApi::OnRespSecurity_Sz(QWORD qQuoteID, void* pParam)
{
    // 深圳A股证券行情响应
    socket_struct_Security_Sz* pSecurity = (socket_struct_Security_Sz*)pParam;
    socket_struct_Security_Sz_Extend* pExtend = (socket_struct_Security_Sz_Extend*)pSecurity->Extend_fields;
    XcDebugInfo(XcDbgFd, "OnRespSecurity_Sz Code:%s,Name:%s,PreClose:%.2lf\n",
        pSecurity->SecCode, pSecurity->SecName, pSecurity->PrevClosePx);

    XcSecurityInfo lSecInfo;
    memset(&lSecInfo, 0, sizeof(lSecInfo));
    strcpy(lSecInfo.ExchangeID, pSecurity->MarketCode);
    strcpy(lSecInfo.InstrumentID, pSecurity->SecCode);
    strcpy(lSecInfo.SecName, pSecurity->SecName);
    strcpy(lSecInfo.SecType, pSecurity->SecType);
    strcpy(lSecInfo.StartDate, pSecurity->ListDate);
    strcpy(lSecInfo.EndDate, pSecurity->MaturityDate);      // FIXME
    strcpy(lSecInfo.SecurityStatusFlag, pSecurity->SecurityStatus);

    lSecInfo.PreClosePrice = pSecurity->PrevClosePx;
    if (pExtend->T_LimitUpRate < 0.0001)
    {
        double lLimitRate = 0.1;
        if (strstr(lSecInfo.SecName, "ST"))
            lLimitRate = 0.05;

        char buf[200] = "";
        sprintf(buf, "%.2lf", lSecInfo.PreClosePrice * (1 + lLimitRate));
        lSecInfo.UpperLimitPrice = atof(buf);

        sprintf(buf, "%.2lf", lSecInfo.PreClosePrice * (1 - lLimitRate));
        lSecInfo.LowerLimitPrice = atof(buf);
    }
    else
    {
        char buf[200] = "";
        sprintf(buf, "%.2lf", lSecInfo.PreClosePrice * (1 + pExtend->T_LimitUpRate));
        lSecInfo.UpperLimitPrice = atof(buf);

        sprintf(buf, "%.2lf", lSecInfo.PreClosePrice * (1 - pExtend->T_LimitDownRate));
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
        int lXcSymbol = atoi(pSecurity->SecCode);
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
    secmap[lXcSymbol] = lSecInfo;
#else
    int lXcSymbol = atoi(pSecurity->SecCode);
    secmap[lXcSymbol] = lSecInfo;
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

    // only keep the AShares
    if (0 && (strncmp(lSecInfo.InstrumentID, "00", 2) == 0 || strncmp(lSecInfo.InstrumentID, "30", 2) == 0))
    {
        socket_struct_SubscribeDetail sub;
        strcpy(sub.MarketCode, lSecInfo.ExchangeID);
        strcpy(sub.SecCode, lSecInfo.InstrumentID);

        std::lock_guard<std::mutex> lk(m_Lock);
        m_SubList.push_back(sub);
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


void XcMdApi::OnRespDyna(QWORD qQuoteID, void* pParam)
{
    // 动态快照行情响应
    socket_struct_Dyna* pDyna = (socket_struct_Dyna*)pParam;
    XcDepthMarketData* pMD;

    if (pDyna->PreClose == 0) {
        return;
    }

    // fprintf(stderr, "OnRespDyna%ld: %s,%d,%d\n", (long)qQuoteID, pDyna->SecCode, pDyna->New, pDyna->Time);

#if HAVE_BAR_GENERATOR
    if (bar_mode)
    {
        process_bar_gen(pDyna);
        return;
    }
#endif//HAVE_BAR_GENERATOR


    int32_t isfirst = 0;
#if MD_MAP_STRING_KEY
    char buf[64] = "";
    sprintf(buf, "%s.%s", pDyna->SecCode, pDyna->MarketCode);
    string lXcSymbol(buf);
    if (mdmap.count(lXcSymbol) == 0)
    {
        XcDepthMarketData lMD;
        memset(&lMD, 0, sizeof(lMD));
        // XcDebugInfo(XcDbgFd, "---->>>>%s,%s\n", pDyna->SecCode, pDyna->MarketCode);
        strcpy(lMD.InstrumentID, pDyna->SecCode);
        strcpy(lMD.ExchangeID, pDyna->MarketCode);
        lMD.PriceDiv = get_price_div(lMD.ExchangeID);
        mdmap[lXcSymbol] = lMD;

        isfirst = 1;
    }
    pMD = &mdmap[lXcSymbol];
#else
    int lXcSymbol = atoi(pDyna->SecCode);
    if (lXcSymbol == 0) {
        return;
    }
    if (mdmap.count(lXcSymbol) == 0)
    {
        XcDepthMarketData lMD;
        memset(&lMD, 0, sizeof(lMD));
        // XcDebugInfo(XcDbgFd, "---->>>>%s,%s\n", pDyna->SecCode, pDyna->MarketCode);
        strcpy(lMD.InstrumentID, pDyna->SecCode);
        strcpy(lMD.ExchangeID, pDyna->MarketCode);
        lMD.PriceDiv = get_price_div(lMD.ExchangeID);
        mdmap[lXcSymbol] = lMD;

        isfirst = 1;
    }
    pMD = &mdmap[lXcSymbol];
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

    pMD->Volume = (int)pDyna->Volume;
    pMD->Turnover = pDyna->Amount / price_div;
    pMD->OpenInterest = pDyna->OpenInt / price_div;

    int hour = int(pDyna->Time / 10000);
    int minute = int(pDyna->Time / 100) % 100;
    int second = int(pDyna->Time % 100);
    sprintf(pMD->UpdateTime, "%02d:%02d:%02d", hour, minute, second);
    pMD->UpdateMillisec = 0;
    pMD->Time = pDyna->Time;
    pMD->TickCount = (uint32_t)pDyna->TickCount;
    pMD->RefPrice = pDyna->RefPrice / price_div;
    memcpy(pMD->TradingPhaseCode, pDyna->TradingPhaseCode, sizeof(pMD->TradingPhaseCode));

    // @202006 可能收盘错误的 pExtend->UpLimit/DownLimit，这里从静态信息中获取
    XcSecurityInfo* psec_info = get_sec_info(lXcSymbol);
    if (psec_info)
    {
        pMD->UpperLimitPrice = psec_info->UpperLimitPrice;
        pMD->LowerLimitPrice = psec_info->LowerLimitPrice;
    }
    else
    {
        // pass
    }

    if (memcmp(pDyna->MarketCode, "SZSE", 4) == 0)
    {
        // here also included SZSEOPT

        // 注意测试环境 180.169.89.22:2222 没有扩展字段，生产环境有
        socket_struct_Dyna_Extend2* pExtend = (socket_struct_Dyna_Extend2*)pDyna->Extend_fields;
        pMD->IOPV = pExtend->IOPV / price_div;
    }
    else // if (strcmp(pDyna->MarketCode, "SSE") == 0)
    {
        socket_struct_Dyna_Extend1* pExtend = (socket_struct_Dyna_Extend1*)pDyna->Extend_fields;
        pMD->IOPV = pExtend->IOPV / price_div;
    }

    strcpy(pMD->TradingDay, trading_day);
    strcpy(pMD->ActionDay, trading_day);

    // keep it firstly, and notify when issue end
    m_pendings.push_back(pMD);

    if (log_level == XC_SPEC_LOG_LV && pDyna->Time >= 91500 && pDyna->Time <= 151500)
    {
        write_data(log_level, "RespDyna %ld,%s,%s,%.2lf,%.2lf,%.2lf,%.2lf,%ld,%.2lf,%.2lf,%.2lf,bid:%.2lf,%d,ask:%.2lf,%d,n:%d",
            (long)qQuoteID, pMD->UpdateTime, pMD->InstrumentID, pMD->LastPrice, pMD->OpenPrice, pMD->HighestPrice, pMD->LowestPrice,
            (long)pMD->Volume, pMD->UpperLimitPrice, pMD->LowerLimitPrice, pMD->PreClosePrice, pMD->BidPrice1, pMD->BidVolume1,
            pMD->AskPrice1, pMD->AskVolume1, m_pendings.size());
    }
}

void XcMdApi::OnRespDepth(QWORD qQuoteID, char* MarketCode, char* SecCode, socket_struct_DepthDetail* pDepth)
{
    // 动态深度行情响应

    // XcDebugInfo(XcDbgFd, "Depth symbol:%s qid:%ld, grade:%d, px:%d, vol:%d\n",
    //     SecCode, (long)qQuoteID, pDepth->Grade, pDepth->Price, (int)pDepth->Quantity);

    XcDepthMarketData* pMD;

#if MD_MAP_STRING_KEY
    char buf[64] = "";
    sprintf(buf, "%s.%s", SecCode, MarketCode);
    string lXcSymbol(buf);
    if (mdmap.count(lXcSymbol) == 0)
    {
        return;
    }
    pMD = &mdmap[lXcSymbol];
#else
    int lXcSymbol = atoi(SecCode);
    if (mdmap.count(lXcSymbol) == 0)
    {
        return;
    }
    pMD = &mdmap[lXcSymbol];
#endif//MD_MAP_STRING_KEY

    if (!pMD) {
        return;
    }

    // double* dstpx;
    // int* dstvol;

    double price = pDepth->Price / pMD->PriceDiv;
    int    quantity = (int)pDepth->Quantity;

    switch (pDepth->Grade)
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

void XcMdApi::OnRespDepthOrder(QWORD qQuoteID, char* MarketCode, char* SecCode, int Grade, DWORD Price, socket_struct_DepthOrderDetail* pDepthOrder)
{
    //if (Grade == 1)
    // cout << "out——DepthOrder:" << SecCode << " Grade: " << Grade << " OrderID: " << pDepthOrder->OrderID << " Qty: " << pDepthOrder->Quantity << endl;

    gil_scoped_acquire acquire;
    dict data;
    data["ExchangeID"] = to_utf(MarketCode);
    data["InstrumentID"] = to_utf(SecCode);
    data["Grade"] = Grade;
    if (strcmp(MarketCode, "SZSE") == 0 || strcmp(MarketCode, "SSE") == 0) {
        data["Price"] = Price / PRICE_DIV_STOCK;
    }
    else {
        data["Price"] = Price / PRICE_DIV_OPTION;
    }
    data["OrderID"] = pDepthOrder->OrderID;
    data["Quantity"] = pDepthOrder->Quantity;
    data["Status"] = pDepthOrder->Status;

    this->on_rtn_depth_order(data);
}


void XcMdApi::OnRespEachOrder(QWORD qQuoteID, void* pParam)
{
    // 逐笔委托
    socket_struct_EachOrder* pEachOrder = (socket_struct_EachOrder*)pParam;

    gil_scoped_acquire acquire;
    dict data;
    data["ExchangeID"] = to_utf(pEachOrder->MarketCode);
    data["InstrumentID"] = to_utf(pEachOrder->SecCode);
    if (strcmp(pEachOrder->MarketCode, "SZSE") == 0 || strcmp(pEachOrder->MarketCode, "SSE") == 0)
    {
        data["Price"] = pEachOrder->Price / PRICE_DIV_STOCK;
        data["Amount"] = pEachOrder->Amount / PRICE_DIV_STOCK;
        if (strcmp(pEachOrder->MarketCode, Scdm_SZSE) == 0)
        {
            socket_struct_EachOrder_Extend2* pExtend2;
            pExtend2 = (socket_struct_EachOrder_Extend2*)pEachOrder->Extend_fields;
            data["OrderType"] = pExtend2->OrderType;
        }
    }
    else
    {
        data["Price"] = pEachOrder->Price / PRICE_DIV_OPTION;
        data["Amount"] = pEachOrder->Amount / PRICE_DIV_OPTION;
    }
    data["Volume"] = pEachOrder->Volume;
    data["OrderNo"] = pEachOrder->OrderNo;
    data["Side"] = pEachOrder->Side;
    data["Time"] = pEachOrder->Time;
    this->on_rtn_each_order(data);
}

void XcMdApi::OnRespEachDeal(QWORD qQuoteID, void* pParam)
{
    // 逐笔成交
    socket_struct_EachDeal* pEachDeal = (socket_struct_EachDeal*)pParam;

    gil_scoped_acquire acquire;
    char temp[8] = "";
    dict data;
    data["ExchangeID"] = to_utf(pEachDeal->MarketCode);
    data["InstrumentID"] = to_utf(pEachDeal->SecCode);

#if 0
    std::cout << "EachDeal:" << pEachDeal->SecCode << "." << pEachDeal->MarketCode << ", "
        << pEachDeal->Price << ", " << pEachDeal->Amount << ", vol:" << pEachDeal->Volume
        << ", time:" << pEachDeal->Time << ", Type:" << pEachDeal->Type << "\n";
#endif//0

    if (strcmp(pEachDeal->MarketCode, "SZSE") == 0 || strcmp(pEachDeal->MarketCode, "SSE") == 0)
    {
        data["Price"] = pEachDeal->Price / PRICE_DIV_STOCK;
        data["Amount"] = pEachDeal->Amount / PRICE_DIV_STOCK;
        if (strcmp(pEachDeal->MarketCode, Scdm_SSE) == 0)
        {
            socket_struct_EachDeal_Extend1* pExtend2;
            pExtend2 = (socket_struct_EachDeal_Extend1*)pEachDeal->Extend_fields;

            temp[0] = pExtend2->BsFlag;
            data["BsFlag"] = to_utf(temp);
        }
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
    data["Time"] = pEachDeal->Time;

    // 成交类型 F：成交 4：撤销;
    temp[0] = pEachDeal->Type;
    data["Type"] = to_utf(temp);

    this->on_rtn_each_deal(data);
}


//////////////////////////////////////////////////////////////////////////
void XcMdApi::create_md_api(string pszFlowPath)
{
    this->api = CXcMarketApi::CreateMarketApi();
    this->api->Register(this);

    // 设置重连参数
    this->api->SetConnectParam(true, 1000, 10000);
}

void XcMdApi::release()
{
    if (this->api)
    {
        write_data(0, "release");

        this->api->Release();
        this->api = NULL;

        if (fp)
        {
            fflush(fp);
            fclose(fp);
            fp = NULL;
        }
    }
}

int XcMdApi::set_connect_param(int is_reconnect, int reconnect_ms, int reconect_count)
{
    return api->SetConnectParam(is_reconnect ? true : false, reconnect_ms, reconect_count);
}

int XcMdApi::set_thread_mode(int on)
{
    if (m_active)
    {
        return -1;
    }
    m_thread_mode = on;
    return 0;
}

int XcMdApi::init(string user_id, string server_ip, string server_port, string license)
{
    int rv;
    XcDebugInfo(XcDbgFd, "init user_id:%s, server_ip:%s, server_port:%s, liscense:%s, thread_mode:%d\n",
        user_id.c_str(), server_ip.c_str(), server_port.c_str(), license.c_str(), m_thread_mode);
    write_data(0, "init user_id:%s, server:%s:%s, liscense:%s, thread_mode:%d\n",
        user_id.c_str(), server_ip.c_str(), server_port.c_str(), license.c_str(), m_thread_mode);

    if (m_thread_mode)
    {
        m_active = true;
        m_task_thread = thread(&XcMdApi::processTask, this);
    }

    rv = this->api->Connect(user_id.c_str(), server_ip.c_str(), server_port.c_str(), license.c_str());
    if (rv < 0)
    {
        XcDebugInfo(XcDbgFd, "xcmdapi Connect failed:%d\n", rv);
    }
    return rv;
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

int XcMdApi::subscribe_md(string instrumentID, int depth_order, int each_flag)
{
    if (instrumentID.length() < 6 && strncmp(instrumentID.c_str(), "*.", 2) != 0) {
        return -1;
    }

    interface_struct_Subscribe sub_flag;
    sub_flag.Dyna_flag = true;
    sub_flag.DepthOrder_flag = depth_order ? true : false;
    sub_flag.Depth_flag = depth_flag;
    sub_flag.EachDeal_flag = each_flag ? true : false;
    sub_flag.EachOrder_flag = each_flag ? true : false;

    char market[32] = "";
    char symbol[16] = "";
    if (!get_symbol_market(symbol, market, instrumentID))
    {
        return -11;
    }
    // fprintf(stderr, "subscribe_md symbol:%s, market:%s\n", symbol, market);

    int rv;

    // 订阅
    socket_struct_SubscribeDetail SubList[MaxSize];
    strcpy(SubList[0].MarketCode, market);
    strcpy(SubList[0].SecCode, symbol);
    int SubSize = 1;
    rv = this->api->Subscribe(refid++, &sub_flag, SubList, SubSize);
    if (rv < 0)
    {
        XcDebugInfo(XcDbgFd, "xcmdapi Subscribe failed!\n");
        return rv;
    }

    return 0;
}

int XcMdApi::unsubscribe_md(string instrument)
{
    interface_struct_Subscribe sub_flag;
    sub_flag.Dyna_flag = false;
    sub_flag.DepthOrder_flag = false;
    sub_flag.Depth_flag = false;
    sub_flag.EachDeal_flag = false;
    sub_flag.EachOrder_flag = false;

    socket_struct_SubscribeDetail SubList[1];
    if (!get_symbol_market(SubList[0].SecCode, SubList[0].MarketCode, instrument))
    {
        return -11;
    }
    return api->Cancel(refid++, &sub_flag, SubList, 2);
}

int XcMdApi::subscribe_md_batch(const std::vector<std::string>& reqs, int depth_order, int each_flag)
{
    interface_struct_Subscribe sub_flag;
    sub_flag.Dyna_flag = true;
    sub_flag.DepthOrder_flag = depth_order ? true : false;
    sub_flag.Depth_flag = true;
    sub_flag.EachDeal_flag = each_flag ? true : false;
    sub_flag.EachOrder_flag = each_flag ? true : false;

    vector<std::string> need_qry_instruments;

    int32_t count = 0;
    socket_struct_SubscribeDetail SubList[DEFAULT_MAX_REQ_SIZE];
    for (int32_t i = 0; i < (int32_t)reqs.size() && i < DEFAULT_MAX_REQ_SIZE; ++i)
    {
        std::string symbol = reqs[i];
        if (!get_symbol_market(SubList[count].SecCode, SubList[count].MarketCode, symbol)) {
            continue;
        }

        if (strcmp(SubList[count].MarketCode, Scdm_SSE) == 0)
        {
            need_qry_instruments.push_back(symbol);
        }

        ++count;
    }

    if (need_qry_instruments.size() > 0)
    {
        req_qry_data_batch(need_qry_instruments);
        sleepms(5);
    }

    if (count)
    {
        return api->Subscribe(refid++, &sub_flag, SubList, count);
    }
    return 0;
}

int XcMdApi::unsubscribe_md_batch(const std::vector<std::string>& reqs)
{
    interface_struct_Subscribe sub_flag;
    sub_flag.Dyna_flag = false;
    sub_flag.DepthOrder_flag = false;
    sub_flag.Depth_flag = false;
    sub_flag.EachDeal_flag = false;
    sub_flag.EachOrder_flag = false;

    int32_t count = 0;
    socket_struct_SubscribeDetail SubList[DEFAULT_MAX_REQ_SIZE];
    for (int32_t i = 0; i < (int32_t)reqs.size() && i < DEFAULT_MAX_REQ_SIZE; ++i)
    {
        std::string instrument = reqs[i];
        if (!get_symbol_market(SubList[count].SecCode, SubList[count].MarketCode, instrument)) {
            continue;
        }
        ++count;
    }

    if (count)
    {
        return api->Cancel(refid++, &sub_flag, SubList, count);
    }
    return 0;
}

int XcMdApi::unsubscribe_all()
{
    interface_struct_Subscribe sub_flag;
    sub_flag.Dyna_flag = false;
    sub_flag.DepthOrder_flag = false;
    sub_flag.Depth_flag = false;
    sub_flag.EachDeal_flag = false;
    sub_flag.EachOrder_flag = false;

    socket_struct_SubscribeDetail SubList[4];
    strcpy(SubList[0].MarketCode, Scdm_SSE);
    strcpy(SubList[0].SecCode, "*");
    strcpy(SubList[1].MarketCode, Scdm_SZSE);
    strcpy(SubList[1].SecCode, "*");
    strcpy(SubList[2].MarketCode, Scdm_SSEOPT);
    strcpy(SubList[2].SecCode, "*");
    strcpy(SubList[3].MarketCode, "SZSEOPT");
    strcpy(SubList[4].SecCode, "*");
    return api->Cancel(5, &sub_flag, SubList, 4);
}

int XcMdApi::req_qry_data(const string& symbol)
{
    socket_struct_RequireDetail req[1];
    if (!get_symbol_market(req[0].SecCode, req[0].MarketCode, symbol))
    {
        return -11;
    }

    return api->Require(refid++, req, 1);
}

int XcMdApi::req_qry_data_batch(const std::vector<std::string>& reqs)
{
    int32_t count = 0;
    socket_struct_RequireDetail ReqList[DEFAULT_MAX_REQ_SIZE];
    for (int32_t i = 0; i < (int32_t)reqs.size() && i < DEFAULT_MAX_REQ_SIZE; ++i)
    {
        std::string instrument = reqs[i];
        if (!get_symbol_market(ReqList[count].SecCode, ReqList[count].MarketCode, instrument)) {
            continue;
        }

        if (strcmp(ReqList[count].SecCode, "*") == 0)
        {
            std::lock_guard<std::mutex> lk(m_Lock);
            m_SubList.clear();
        }

        ++count;
    }

    if (count)
    {
        return api->Require(refid++, ReqList, count);
    }
    return 0;
}

void XcMdApi::set_have_level10(int on)
{
    have_level10 = on;
}

string XcMdApi::get_api_version()
{
    return string(XC_MDAPI_VERSION);
}

int XcMdApi::open_debug(string log_file, int log_level)
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

#if HAVE_BAR_GENERATOR
int XcMdApi::subscribe_bar_md(std::string instrument)
{
    depth_flag = false;
    bar_mode = true;
    return subscribe_md(instrument, 0, 0);
}

void XcMdApi::process_bar_gen(socket_struct_Dyna* apMD)
{
    // fprintf(stderr, "bargen %s,%.2lf,%s\n", apMD->InstrumentID, apMD->LastPrice, apMD->UpdateTime);
    bar_data_t* bar;
    bar_generator_t* bargen;
    // std::string lInstrumentID(apMD->InstrumentID);
    int lInstrumentID = atoi(apMD->SecCode);
    if (bar_gen_map.count(lInstrumentID))
    {
        bargen = bar_gen_map[lInstrumentID];
    }
    else
    {
        bargen = (bar_generator_t*)malloc(sizeof(bar_generator_t));
        bar_generator_init(bargen, apMD->MarketCode, apMD->SecCode);
        bar_gen_map[lInstrumentID] = bargen;
    }

    int update_time = apMD->Time;
    bar = bar_generator_update(bargen, trading_day, update_time,
        apMD->New / apMD->PriceDiv, apMD->Volume, apMD->Amount / apMD->PriceDiv, apMD->OpenInt);
    if (!bar)
    {
        // bar = &bargen->cur_bar;
        return;
    }
    // fprintf(stderr, "generated bar!\n");

    gil_scoped_acquire acquire;
    dict data;

    data["InstrumentID"] = to_utf(bar->InstrumentID);
    data["ExchangeID"] = to_utf(bar->ExchangeID);
    data["Date"] = to_utf(bar->Date);
    data["Time"] = to_utf(bar->Time);
    data["Period"] = to_utf(bar->Period);
    data["Open"] = bar->Open;
    data["High"] = bar->High;
    data["Low"] = bar->Low;
    data["Close"] = bar->Close;
    data["Volume"] = bar->Volume;
    data["Turnover"] = bar->Turnover;

    this->on_rtn_bar_data(data);
}

#endif//HAVE_BAR_GENERATOR


int XcMdApi::Subscribe(const std::string& exchange, const std::string& instrument, int dyna_flag, int depth_flag, int depth_order, int each_flag)
{
    interface_struct_Subscribe sub_flag;
    sub_flag.Dyna_flag = dyna_flag ? true : false;
    sub_flag.DepthOrder_flag = depth_order ? true : false;
    sub_flag.Depth_flag = depth_flag ? true : false;
    sub_flag.EachDeal_flag = each_flag ? true : false;
    sub_flag.EachOrder_flag = each_flag ? true : false;

    // 订阅
    socket_struct_SubscribeDetail SubList[MaxSize];
    strncpy(SubList[0].MarketCode, exchange.c_str(), sizeof(SubList[0].MarketCode) - 1);
    strncpy(SubList[0].SecCode, instrument.c_str(), sizeof(SubList[0].SecCode) - 1);
    int SubSize = 1;
    int rv = this->api->Subscribe(refid++, &sub_flag, SubList, SubSize);
    if (rv < 0)
    {
        XcDebugInfo(XcDbgFd, "xcmdapi Subscribe failed!!\n");
        return rv;
    }

    return rv;
}

int XcMdApi::Cancel(const std::string& exchange, const std::string& instrument, int dyna_flag, int depth_flag, int depth_order, int each_flag)
{
    interface_struct_Subscribe sub_flag;
    sub_flag.Dyna_flag = dyna_flag ? true : false;
    sub_flag.DepthOrder_flag = depth_order ? true : false;
    sub_flag.Depth_flag = depth_flag ? true : false;
    sub_flag.EachDeal_flag = each_flag ? true : false;
    sub_flag.EachOrder_flag = each_flag ? true : false;

    // 订阅
    socket_struct_SubscribeDetail SubList[MaxSize];
    strncpy(SubList[0].MarketCode, exchange.c_str(), sizeof(SubList[0].MarketCode) - 1);
    strncpy(SubList[0].SecCode, instrument.c_str(), sizeof(SubList[0].SecCode) - 1);
    int SubSize = 1;
    int rv = this->api->Cancel(refid++, &sub_flag, SubList, SubSize);
    if (rv < 0)
    {
        XcDebugInfo(XcDbgFd, "xcmdapi Cancel failed!!\n");
        return rv;
    }

    return rv;
}


int XcMdApi::Require(int ref_id, const std::string& exchange, const std::string& instrument)
{
    socket_struct_RequireDetail req[1];
    strncpy(req->MarketCode, exchange.c_str(), sizeof(req->MarketCode) - 1);
    strncpy(req->SecCode, instrument.c_str(), sizeof(req->SecCode) - 1);
    return api->Require(ref_id, req, 1);
}

//////////////////////////////////////////////////////////////////////////
void XcMdApi::processTask()
{
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
        } // while
    }
    catch (const TerminatedError&)
    {
    }
}

void XcMdApi::processOnUserLogin(Task *task)
{
    time_t now = time(NULL);
    struct tm* ptm = localtime(&now);
    sprintf(trading_day, "%04d%02d%02d",
        (ptm->tm_year + 1900), (ptm->tm_mon + 1), ptm->tm_mday);

    gil_scoped_acquire acquire;
    this->on_front_connected();

    socket_struct_Msg* pMsg = (socket_struct_Msg*)task->task_data;
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
    socket_struct_Msg* pMsg = (socket_struct_Msg*)task->task_data;

    gil_scoped_acquire acquire;
    dict data;
    data["MsgId"] = pMsg->Msgid;
    data["Desc"] = to_utf(pMsg->Desc);
    delete pMsg;
    this->on_msg(task->task_id, data);
}

void XcMdApi::processOnRspMarket(Task *task)
{
    socket_struct_Market* pMarket = (socket_struct_Market*)task->task_data;

    gil_scoped_acquire acquire;
    dict data;
    data["MarketCode"] = to_utf(pMarket->MarketCode);
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
    if (secmap.count(symbol))
        return &secmap[symbol];
    return NULL;
}
#else
XcSecurityInfo* XcMdApi::get_sec_info(int isymbol)
{
    if (secmap.count(isymbol))
        return &secmap[isymbol];
    return NULL;
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

    void on_rtn_bar_data(const dict &data) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcMdApi, on_rtn_bar_data, data);
        }
        catch (const error_already_set &e)
        {
            cout << e.what() << endl;
        }
    };
};


PYBIND11_MODULE(xcmd, m)
{
    class_<XcMdApi, PyMdApi> mdapi(m, "XcMdApi", module_local());
    mdapi
        .def(init<>())
        .def("create_md_api", &XcMdApi::create_md_api)
        .def("create_api", &XcMdApi::create_md_api)
        .def("release", &XcMdApi::release)
        .def("set_connect_param", &XcMdApi::set_connect_param)
        .def("set_thread_mode", &XcMdApi::set_thread_mode)
        .def("init", &XcMdApi::init)
        .def("set_have_level10", &XcMdApi::set_have_level10)
        .def("get_api_version", &XcMdApi::get_api_version)
        .def("open_debug", &XcMdApi::open_debug)
        .def("write_line", &XcMdApi::write_line)
        .def("subscribe_md", &XcMdApi::subscribe_md)
        .def("subscribe_md_batch", &XcMdApi::subscribe_md_batch, pybind11::return_value_policy::reference)
        .def("unsubscribe_md_batch", &XcMdApi::unsubscribe_md_batch, pybind11::return_value_policy::reference)
        .def("unsubscribe_md", &XcMdApi::unsubscribe_md)
        .def("unsubscribe_all", &XcMdApi::unsubscribe_all)
        .def("req_qry_data", &XcMdApi::req_qry_data)
        .def("req_qry_data_batch", &XcMdApi::req_qry_data_batch)
        .def("Require", &XcMdApi::Require)
        .def("Subscribe", &XcMdApi::Subscribe)
        .def("UnSubscribe", &XcMdApi::Cancel)
        .def("Cancel", &XcMdApi::Cancel)

#if HAVE_BAR_GENERATOR
        .def("subscribe_bar_md", &XcMdApi::subscribe_bar_md)
#endif//HAVE_BAR_GENERATOR

        .def("on_front_connected", &XcMdApi::on_front_connected)
        .def("on_front_disconnected", &XcMdApi::on_front_disconnected)
        .def("on_rsp_user_login", &XcMdApi::on_rsp_user_login)
        .def("on_rsp_qry_data", &XcMdApi::on_rsp_qry_data)
        .def("on_rtn_market_data", &XcMdApi::on_rtn_market_data)
        .def("on_rtn_depth_order", &XcMdApi::on_rtn_depth_order)
        .def("on_rtn_each_order", &XcMdApi::on_rtn_each_order)
        .def("on_rtn_each_deal", &XcMdApi::on_rtn_each_deal)
        .def("on_msg", &XcMdApi::on_msg)
        .def("on_rsp_market", &XcMdApi::on_rsp_market)
        .def("on_rtn_bar_data", &XcMdApi::on_rtn_bar_data)
        ;
}
