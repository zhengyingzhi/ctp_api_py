#include <stdarg.h>
#include <time.h>
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


#define XC_MDAPI_VERSION    "1.2.0"
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


#define PRICE_DIV               1000.0
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

static bool get_symbol_market(char symbol[], char market[], const std::string& instrument)
{
    std::size_t lDotIndex = instrument.find('.');
    if (lDotIndex == std::string::npos || lDotIndex < 1 || lDotIndex > 9)
    {
        return false;
    }

    // 000001.SZSE
    strncpy(symbol, instrument.c_str(), lDotIndex);

    if (instrument.find("SSE") != string::npos ||
        instrument.find("SH") != string::npos ||
        instrument.find("SHA") != string::npos)
    {
        strcpy(market, Scdm_SSE);
    }
    else if (instrument.find("SZSE") != string::npos ||
        instrument.find("SZ") != string::npos ||
        instrument.find("SZA") != string::npos)
    {
        strcpy(market, Scdm_SZSE);
    }
    else
    {
#if 0
        // unknown market
        if (strncmp(symbol, "60", 2) == 0 ||
            strncmp(symbol, "68", 2) == 0)
        {
            strcpy(market, Scdm_SSE);
        }
        else if (strncmp(symbol, "00", 2) == 0 ||
            strncmp(symbol, "30", 2) == 0)
        {
            strcpy(market, Scdm_SZSE);
        }
        else {
            return false;
        }
#else
        return false;
#endif
    }
    return true;
}

static int get_ratio_index(double aUpdownRatio)
{
    return int32_t(abs(aUpdownRatio) * 100);
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

    time_t now = time(NULL);
    struct tm* ptm = localtime(&now);
    sprintf(trading_day, "%04d%02d%02d", (ptm->tm_year + 1900), (ptm->tm_mon + 1), ptm->tm_mday);

    refid = 1;
    flush_count = 0;
    have_level10 = 0;
    statistic_mode = 0;
    log_level = 0;
    fp = NULL;

    memset(m_UpStatistics, 0, sizeof(m_UpStatistics));
    memset(m_DownStatistics, 0, sizeof(m_DownStatistics));
    m_UpCount = 0;
    m_DownCount = 0;
    m_UpLimitCount = 0;
    m_DownLimitCount = 0;
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

    char buf[64] = "";
    sprintf(buf, "%s.%s", pSecurity->SecCode, pSecurity->MarketCode);
    string lXcSymbol(buf);

    XcSecurityInfo lSecInfo;
    memset(&lSecInfo, 0, sizeof(lSecInfo));
    strcpy(lSecInfo.ExchangeID, pSecurity->MarketCode);
    strcpy(lSecInfo.InstrumentID, pSecurity->SecCode);
    strcpy(lSecInfo.SecName, pSecurity->SecName);
    lSecInfo.PreClosePrice = pSecurity->SecurityClosePx;
    lSecInfo.UpperLimitPrice = pSecurity->DailyPriceUpLimit;
    lSecInfo.LowerLimitPrice = pSecurity->DailyPriceDownLimit;
    secmap[lXcSymbol] = lSecInfo;

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
    if (statistic_mode && lSecInfo.InstrumentID[0] == '6')
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
    socket_struct_Security_Opt* pSecurity = (socket_struct_Security_Opt*)pParam;
    socket_struct_Security_Opt_Extend* pExtend = (socket_struct_Security_Opt_Extend*)pSecurity->Extend_fields;
    (void)pSecurity;
    (void)pExtend;
}

void XcMdApi::OnRespSecurity_Sz(QWORD qQuoteID, void* pParam)
{
    // 深圳A股证券行情响应
    socket_struct_Security_Sz* pSecurity = (socket_struct_Security_Sz*)pParam;
    socket_struct_Security_Sz_Extend* pExtend = (socket_struct_Security_Sz_Extend*)pSecurity->Extend_fields;
    XcDebugInfo(XcDbgFd, "OnRespSecurity_Sz Code:%s,Name:%s,PreClose:%.2lf\n",
        pSecurity->SecCode, pSecurity->SecName, pSecurity->PrevClosePx);

    char buf[64] = "";
    sprintf(buf, "%s.%s", pSecurity->SecCode, pSecurity->MarketCode);
    string lXcSymbol(buf);

    XcSecurityInfo lSecInfo;
    memset(&lSecInfo, 0, sizeof(lSecInfo));
    strcpy(lSecInfo.ExchangeID, pSecurity->MarketCode);
    strcpy(lSecInfo.InstrumentID, pSecurity->SecCode);
    strcpy(lSecInfo.SecName, pSecurity->SecName);
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

    secmap[lXcSymbol] = lSecInfo;

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
    if (statistic_mode && (strncmp(lSecInfo.InstrumentID, "00", 2) == 0 || strncmp(lSecInfo.InstrumentID, "30", 2) == 0))
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

    // fprintf(stderr, "OnRespDyna%ld: %s\n", (long)qQuoteID, pDyna->SecCode);

    int32_t isfirst = 0;
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
        mdmap[lXcSymbol] = lMD;

        isfirst = 1;
    }
    pMD = &mdmap[lXcSymbol];

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

    pMD->PreClosePrice = pDyna->PreClose / PRICE_DIV;
    pMD->LastPrice = pDyna->New / PRICE_DIV;
    pMD->OpenPrice = pDyna->Open / PRICE_DIV;
    pMD->HighestPrice = pDyna->High / PRICE_DIV;
    pMD->LowestPrice = pDyna->Low / PRICE_DIV;
    pMD->ClosePrice = pDyna->Close / PRICE_DIV;

    pMD->Volume = (int)pDyna->Volume;
    pMD->Turnover = pDyna->Amount / PRICE_DIV;
    pMD->OpenInterest = pDyna->OpenInt / PRICE_DIV;

    int hour = int(pDyna->Time / 10000);
    int minute = int(pDyna->Time / 100) % 100;
    int second = int(pDyna->Time % 100);
    sprintf(pMD->UpdateTime, "%02d:%02d:%02d", hour, minute, second);
    pMD->UpdateMillisec = 0;
    pMD->Time = pDyna->Time;
    pMD->TickCount = pDyna->TickCount;
    memcpy(pMD->TradingPhaseCode, pDyna->TradingPhaseCode, sizeof(pMD->TradingPhaseCode));

    if (memcmp(pDyna->MarketCode, "SZSE", 4) == 0)
    {
        // 注意测试环境 180.169.89.22:2222 没有扩展字段，生产环境有
        socket_struct_Dyna_Extend2* pExtend = (socket_struct_Dyna_Extend2*)pDyna->Extend_fields;
        if (pExtend->DownLimit > 100)
        {
            pMD->UpperLimitPrice = pExtend->UpLimit / PRICE_DIV;
            pMD->LowerLimitPrice = pExtend->DownLimit / PRICE_DIV;
        }
        else if (secmap.count(lXcSymbol))
        {
            XcSecurityInfo* psec_info = &secmap[lXcSymbol];
            pMD->UpperLimitPrice = psec_info->UpperLimitPrice;
            pMD->LowerLimitPrice = psec_info->LowerLimitPrice;
        }
        else
        {
            pMD->UpperLimitPrice = 0;
            pMD->LowerLimitPrice = 0;
        }
    }
    else
    {
        if (secmap.count(lXcSymbol))
        {
            XcSecurityInfo* psec_info = &secmap[lXcSymbol];
            pMD->UpperLimitPrice = psec_info->UpperLimitPrice;
            pMD->LowerLimitPrice = psec_info->LowerLimitPrice;
        }
    }

    if (!statistic_mode)
    {
        strcpy(pMD->TradingDay, trading_day);
        strcpy(pMD->ActionDay, trading_day);

        // keep it firstly, and notify when issue end
        m_pendings.push_back(pMD);
    }
    else
    {
        statistic_md(pMD, isfirst);
    }

    if (log_level == XC_SPEC_LOG_LV && pDyna->Time >= 91500 && pDyna->Time <= 151500)
    {
        write_data(log_level, "RespDyna %ld,%s,%s,%.2lf,%.2lf,%.2lf,%.2lf,%ld,%.2lf,%.2lf,%.2lf,bid:%.2lf,%d,ask:%.2lf,%d,n:%d",
            (long)qQuoteID, pMD->UpdateTime, lXcSymbol, pMD->LastPrice, pMD->OpenPrice, pMD->HighestPrice, pMD->LowestPrice,
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

    char buf[64] = "";
    sprintf(buf, "%s.%s", SecCode, MarketCode);
    string lXcSymbol(buf);
    if (mdmap.count(lXcSymbol) == 0)
    {
        return;
    }
    pMD = &mdmap[lXcSymbol];

    // double* dstpx;
    // int* dstvol;

    double price = pDepth->Price / PRICE_DIV;
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
    data["Price"] = Price / PRICE_DIV;
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
    data["Price"] = pEachOrder->Price / PRICE_DIV;
    data["Volume"] = pEachOrder->Volume;
    data["Amount"] = pEachOrder->Amount / PRICE_DIV;
    data["OrderNo"] = pEachOrder->OrderNo;
    data["Side"] = pEachOrder->Side;
    data["Time"] = pEachOrder->Time;
    if (strcmp(pEachOrder->MarketCode, Scdm_SZSE) == 0)
    {
        socket_struct_EachOrder_Extend2* pExtend2;
        pExtend2 = (socket_struct_EachOrder_Extend2*)pEachOrder->Extend_fields;
        data["OrderType"] = pExtend2->OrderType;
    }
    this->on_rtn_each_order(data);
}

void XcMdApi::OnRespEachDeal(QWORD qQuoteID, void* pParam)
{
    // 逐笔成交
    socket_struct_EachDeal* pEachDeal = (socket_struct_EachDeal*)pParam;

    gil_scoped_acquire acquire;
    dict data;
    data["ExchangeID"] = to_utf(pEachDeal->MarketCode);
    data["InstrumentID"] = to_utf(pEachDeal->SecCode);
    data["Price"] = pEachDeal->Price / PRICE_DIV;
    data["Volume"] = pEachDeal->Volume;
    data["Amount"] = pEachDeal->Amount / PRICE_DIV;
    data["BuyNo"] = pEachDeal->BuyNo;
    data["SellNo"] = pEachDeal->SellNo;
    data["BuyNo"] = pEachDeal->BuyNo;
    data["DealNo"] = pEachDeal->DealNo;
    data["Type"] = pEachDeal->Type;     // 成交类型 F：成交 4：撤销
    data["Time"] = pEachDeal->Time;
    if (strcmp(pEachDeal->MarketCode, Scdm_SSE) == 0)
    {
        socket_struct_EachDeal_Extend1* pExtend2;
        pExtend2 = (socket_struct_EachDeal_Extend1*)pEachDeal->Extend_fields;
        data["BsFlag"] = pExtend2->BsFlag;
    }

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
    sub_flag.Depth_flag = true;
    sub_flag.EachDeal_flag = each_flag ? true : false;
    sub_flag.EachOrder_flag = each_flag ? true : false;

    char market[32] = "";
    char symbol[16] = "";
    if (!get_symbol_market(symbol, market, instrumentID))
    {
        return -11;
    }
    // fprintf(stderr, "subscribe_md symbol:%s, market:%s\n", symbol, market);

    if (strcmp(market, Scdm_SZSE) == 0)
    {
        // do once query for get limit price
        // int rv = req_qry_data(instrumentID);
        // fprintf(stderr, "auto qry rv:%d\n", rv);
        // sleepms(50);
    }

    int rv;
    if (strcmp(symbol, "*") == 0)
    {
        sub_flag.Depth_flag = false;    // FIXME
        sub_flag.DepthOrder_flag = false;
        sub_flag.EachDeal_flag = false;
        sub_flag.EachOrder_flag = false;

        int count = 0;

        std::lock_guard<std::mutex> lk(m_Lock);
        if (m_SubList.size() == 0)
        {
            fprintf(stderr, "[WARN] no security info to subsribe!\n");
            return -1;
        }
        fprintf(stderr, "subscribe_md total count:%d\n", (int)m_SubList.size());

        socket_struct_SubscribeDetail SubList[MaxSize];
        for (int i = 0; i < (int)m_SubList.size(); ++i)
        {
            if (i + 1 == MaxSize)
            {
                rv = this->api->Subscribe(refid++, &sub_flag, SubList, count);
                if (rv < 0)
                {
                    XcDebugInfo(XcDbgFd, "xcmdapi Subscribe %d failed!\n", count);
                    return rv;
                }
                count = 0;
            }
            strcpy(SubList[count].MarketCode, m_SubList[i].MarketCode);
            strcpy(SubList[count].SecCode, m_SubList[i].SecCode);
            ++count;
        }

        if (count > 0)
        {
            rv = this->api->Subscribe(refid++, &sub_flag, SubList, count);
            if (rv < 0)
            {
                XcDebugInfo(XcDbgFd, "xcmdapi Subscribe %d failed!\n", count);
                return rv;
            }
        }

        return 0;
    }

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
        std::string instrument = reqs[i];
        if (!get_symbol_market(SubList[count].SecCode, SubList[count].MarketCode, instrument)) {
            continue;
        }

        if (strcmp(SubList[count].MarketCode, Scdm_SSE) == 0)
        {
            need_qry_instruments.push_back(instrument);
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

    socket_struct_SubscribeDetail SubList[2];
    strcpy(SubList[0].MarketCode, Scdm_SSE);
    strcpy(SubList[0].SecCode, "*");
    strcpy(SubList[1].MarketCode, Scdm_SZSE);
    strcpy(SubList[1].SecCode, "*");
    return api->Cancel(5, &sub_flag, SubList, 2);
}

int XcMdApi::req_qry_data(string instrument)
{
    socket_struct_RequireDetail req[1];
    if (!get_symbol_market(req[0].SecCode, req[0].MarketCode, instrument))
    {
        return -11;
    }

    if (strcmp(req[0].SecCode, "*") == 0)
    {
        std::lock_guard<std::mutex> lk(m_Lock);
        m_SubList.clear();
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

void XcMdApi::set_statistic_mode()
{
    statistic_mode = 1;
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

int XcMdApi::get_statistics(dict out)
{
    out["UpCount"] = m_UpCount;
    out["DownCount"] = m_DownCount;
    out["UpLimitCount"] = m_UpLimitCount;
    out["DownLimitCount"] = m_DownLimitCount;
    return 0;
}

void XcMdApi::statistic_md(XcDepthMarketData* pMD, int32_t isfirst)
{
    // 涨跌额
    double updown = pMD->LastPrice - pMD->PreClosePrice;
    double updown_ratio = updown / pMD->PreClosePrice;
    int32_t updown_ratio_index = get_ratio_index(updown_ratio);
    // fprintf(stderr, "** updown_ratio for %s, %.4lf, %d\n", pMD->InstrumentID, updown_ratio, updown_ratio_index);

    // 涨跌家数
    if (isfirst)
    {
        if (IS_UP(updown_ratio))
        {
            m_UpCount += 1;
            pMD->PriceStatus = PS_Up;
        }
        else if (IS_DOWN(updown_ratio))
        {
            m_DownCount += 1;
            pMD->PriceStatus = PS_Down;
        }
        else
        {
            pMD->PriceStatus = PS_None;
        }
    }
    else
    {
        if (IS_UP(updown_ratio))
        {
            // 当前是上涨，但上一次不是
            if (pMD->UpDownRatio <= 0.000001)
                m_UpCount += 1;
            if (IS_DOWN(pMD->UpDownRatio))
                m_DownCount -= 1;
            pMD->PriceStatus = PS_Up;
        }
        else if (IS_DOWN(updown_ratio))
        {
            // 当前是下跌，但上一次不是
            if (pMD->UpDownRatio >= -0.000001)
                m_DownCount += 1;
            if (IS_UP(pMD->UpDownRatio))
                m_UpCount -= 1;
            pMD->PriceStatus = PS_Down;
        }
        else
        {
            pMD->PriceStatus = PS_None;
        }
    }

    // 涨跌停家数
    if (pMD->UpperLimitPrice > 0.1)
    {
        if (abs(pMD->LastPrice - pMD->UpperLimitPrice) < 0.0001)
        {
            // 涨停
            if (pMD->LimitStatus == PS_UpLimit)
            {
                // 上次已涨停
            }
            else if (pMD->LimitStatus == PS_None)
            {
                // 上次未涨跌停
                m_UpLimitCount += 1;

                // fprintf(stderr, "# %s涨停, upcnt:%d, lowcnt:%d\n", pMD->InstrumentID, m_UpLimitCount, m_DownLimitCount);
                if (log_level == XC_LIMIT_LOG_LV)
                {
                    write_data(log_level, "statistic_md # %s涨停:%.2lf time:%s, upcnt:%d, lowcnt:%d",
                        pMD->InstrumentID, pMD->UpperLimitPrice, pMD->UpdateTime, m_UpLimitCount, m_DownLimitCount);
                }
            }
            else if (pMD->LimitStatus == PS_DownLimit)
            {
                // 上次跌停
                m_UpLimitCount += 1;
                m_DownLimitCount -= 1;
            }
            pMD->LimitStatus = PS_UpLimit;
        }
        else if (abs(pMD->LastPrice - pMD->LowerLimitPrice) < 0.0001)
        {
            // 跌停
            if (pMD->LimitStatus == PS_UpLimit)
            {
                // 上次涨停
                m_UpLimitCount -= 1;
                m_DownLimitCount += 1;
            }
            else if (pMD->LimitStatus == PS_None)
            {
                // 上次未涨跌停
                m_DownLimitCount += 1;
                // fprintf(stderr, "$ %s跌停, upcnt:%d, lowcnt:%d\n", pMD->InstrumentID, m_UpLimitCount, m_DownLimitCount);
                if (log_level == XC_LIMIT_LOG_LV)
                {
                    write_data(log_level, "statistic_md $ %s跌停:%.2lf time:%s, upcnt:%d, lowcnt:%d",
                        pMD->InstrumentID, pMD->UpperLimitPrice, pMD->UpdateTime, m_UpLimitCount, m_DownLimitCount);
                }
            }
            else if (pMD->LimitStatus == PS_DownLimit)
            {
                // 已跌停
            }
            pMD->LimitStatus = PS_DownLimit;
        }
        else
        {
            // 未涨跌停，但上次处于涨跌停
            if (pMD->LimitStatus == PS_UpLimit)
            {
                m_UpLimitCount -= 1;
            }
            else if (pMD->LimitStatus == PS_DownLimit)
            {
                m_DownLimitCount -= 1;
            }
            pMD->LimitStatus = PS_None;
        }
    }

#if 0
    static const char* upper_symbols[] = {
        "300071", "300362", "002591", "000158", "300032", "300795", "300551",
        "300200", "300562", "300552", "300793", "002891", "002291", "300468",
        "300379", "002494", "000851", "000633", NULL
        };
    for (int index = 0; upper_symbols[index]; ++index)
    {
        const char* pcur = upper_symbols[index];
        if (strcmp(pMD->InstrumentID, pcur) == 0)
        {
            fprintf(stderr, "the symbol %s limit status is %d, last:%.2lf, upper:%.2lf!!\n", pMD->InstrumentID, pMD->LimitStatus, pMD->LastPrice, pMD->UpperLimitPrice);
            if (pMD->LimitStatus != PS_UpLimit)
                fprintf(stderr, "error limit status for %s!!\n", pcur);
        }
    }
#endif//0

    // 各股票涨跌幅度
    int32_t old_ratio_index = get_ratio_index(pMD->UpDownRatio);
    if (old_ratio_index != updown_ratio_index || isfirst)
    {
        if (updown_ratio > 0)
            m_UpStatistics[updown_ratio_index] += 1;
        else if (updown_ratio < 0)
            m_DownStatistics[updown_ratio_index] += 1;

        if (!isfirst)
        {
            if (updown_ratio > 0)
                m_UpStatistics[old_ratio_index] -= 1;
            else if (updown_ratio < 0)
                m_DownStatistics[old_ratio_index] -= 1;
        }
    }
    pMD->UpDownRatio = updown_ratio;
}

int XcMdApi::get_ratio_count(double ratio, int cond)
{
    // fprintf(stderr, "now upcount:%d, downcound:%d\n", m_UpCount, m_DownCount);

    int32_t ratio_int = int32_t(ratio * 100);
    if (ratio_int < -10 || ratio_int > 10)
    {
        return 0;
    }

    uint32_t count = 0;
#if 0
    switch (ratio)
    {
        case
    }
#endif//0

    if (ratio_int >= 0)
    {
        for (int32_t i = ratio_int; i < MAX_UPDOWN_LEVEL; ++i)
        {
            count += m_UpStatistics[i];
        }
    }
    else
    {
        for (int32_t i = abs(ratio_int); i < MAX_UPDOWN_LEVEL; ++i)
        {
            count += m_DownStatistics[i];
        }
    }
    return count;
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
    data["PreClosePrice"] = lpSecInfo->PreClosePrice;
    data["UpperLimitPrice"] = lpSecInfo->UpperLimitPrice;
    data["LowerLimitPrice"] = lpSecInfo->LowerLimitPrice;
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
        .def("set_statistic_mode", &XcMdApi::set_statistic_mode)
        .def("get_statistics", &XcMdApi::get_statistics)
        .def("subscribe_md", &XcMdApi::subscribe_md)
        .def("subscribe_md_batch", &XcMdApi::subscribe_md_batch, pybind11::return_value_policy::reference)
        .def("unsubscribe_md_batch", &XcMdApi::unsubscribe_md_batch, pybind11::return_value_policy::reference)
        .def("unsubscribe_md", &XcMdApi::unsubscribe_md)
        .def("unsubscribe_all", &XcMdApi::unsubscribe_all)
        .def("req_qry_data", &XcMdApi::req_qry_data)
        .def("req_qry_data_batch", &XcMdApi::req_qry_data_batch)
        // .def("reqUserLogin", &XcMdApi::reqUserLogin)

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
        ;
}
