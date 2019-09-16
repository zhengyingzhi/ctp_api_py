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
#include <unistd.h>
#define sleepms(x)  usleep((x) * 1000)
#endif

#include "xcmd.h"


#define XC_MDAPI_VERSION    "0.0.3"


#define MY_DEBUG 0

#if (MY_DEBUG)
#define XcDebugInfo(fd,fmt,...) fprintf(fd,fmt,##__VA_ARGS__)
#else
static  inline void XcPrintNull(FILE* fd, const char* fmt, ...) { (void)fd; (void)fmt; }
#define XcDebugInfo(fd,fmt,...) XcPrintNull(fd,fmt,##__VA_ARGS__)
#endif//MY_DEBUG
#define XcDbgFd stdout


#define PRICE_DIV               1000.0
#define DEFAULT_MAX_REQ_SIZE    256

static string to_utf(const string &gb2312)
{
#ifdef _MSC_VER
    const static locale loc("zh-CN");
#else
    const static locale loc("zh_CN.GB18030");
#endif

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
}

static bool get_symbol_market(char symbol[], char market[], const std::string& instrument)
{
    std::size_t dot_index = instrument.find('.');
    if (dot_index == std::string::npos || dot_index < 1 || dot_index > 9)
    {
        return false;
    }

    // 000001.SZSE
    strncpy(symbol, instrument.c_str(), dot_index);

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


XcMdApi::XcMdApi()
    : api()
    , qidmap()
    , mdmap()
{
    refid = 1;
}

XcMdApi::~XcMdApi()
{}

void XcMdApi::OnUserLogin(socket_struct_Msg* pMsg)
{
    /*	if(strstr(pMsg->Desc, "Successed"))
    cout << "用户登录成功:" << pMsg->Desc << endl;
    else
    cout << "用户登录失败:" << pMsg->Desc << endl;
    */
    // cout << "用户登录返回:" << pMsg->Desc << endl;
    XcDebugInfo(XcDbgFd, "OnUserLogin:%s\n", pMsg->Desc);

    gil_scoped_acquire acquire;
    this->on_front_connected();

    dict data;
    data["MsgID"] = pMsg->Msgid;
    data["Desc"] = to_utf(pMsg->Desc);

    this->on_rsp_user_login(data);
}

void XcMdApi::OnHeart()
{
    //cout << "out:" << "Heart........." << endl;
}

void XcMdApi::OnClose()
{
    // cout << "out:" << "通道关闭." << endl;
    XcDebugInfo(XcDbgFd, "OnClose\n");
    this->on_front_disconnected(-1);
}

void XcMdApi::OnIssueEnd(QWORD qQuoteID)
{
    XcDebugInfo(XcDbgFd, "OnIssueEnd qQuoteID:%ld\n", (long)qQuoteID);

    XcDepthMarketData* pMD;
    if (qidmap.count(qQuoteID) == 0)
    {
        return;
    }
    pMD = qidmap[qQuoteID];
    qidmap.erase(qQuoteID);

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

    data["UpperLimitPrice"] = pMD->UpperLimitPrice;
    data["LowerLimitPrice"] = pMD->LowerLimitPrice;

    data["BidPrice1"]  = pMD->BidPrice1;
    data["BidVolume1"] = pMD->BidVolume1;
    data["BidPrice2"]  = pMD->BidPrice2;
    data["BidVolume2"] = pMD->BidVolume2;
    data["BidPrice3"]  = pMD->BidPrice3;
    data["BidVolume3"] = pMD->BidVolume3;
    data["BidPrice4"]  = pMD->BidPrice4;
    data["BidVolume4"] = pMD->BidVolume4;
    data["BidPrice5"]  = pMD->BidPrice5;
    data["BidVolume5"] = pMD->BidVolume5;

    data["AskPrice1"]  = pMD->AskPrice1;
    data["AskVolume1"] = pMD->AskVolume1;
    data["AskPrice2"]  = pMD->AskPrice2;
    data["AskVolume2"] = pMD->AskVolume2;
    data["AskPrice3"]  = pMD->AskPrice3;
    data["AskVolume3"] = pMD->AskVolume3;
    data["AskPrice4"]  = pMD->AskPrice4;
    data["AskVolume4"] = pMD->AskVolume4;
    data["AskPrice5"]  = pMD->AskPrice5;
    data["AskVolume5"] = pMD->AskVolume5;

    this->on_rtn_market_data(data);
}

void XcMdApi::OnMsg(QWORD qRefID, socket_struct_Msg* pMsg)
{
    XcDebugInfo(XcDbgFd, "OnMsg qRefID:%ld, MsgId:%d, Msg:%s\n", (long)qRefID, pMsg->Msgid, pMsg->Desc);

    gil_scoped_acquire acquire;
    dict data;
    data["MsgId"] = pMsg->Msgid;
    data["Desc"] = to_utf(pMsg->Desc);
    this->on_msg(data);
}

void XcMdApi::OnRespMarket(QWORD qQuoteID, socket_struct_Market* pMarket)
{
    XcDebugInfo(XcDbgFd, "OnRespMarket qQuoteID:%ld, Code:%s, Name:%s\n",
        (long)qQuoteID, pMarket->MarketCode, pMarket->MarketName);

    gil_scoped_acquire acquire;
    dict data;
    data["MarketCode"] = to_utf(pMarket->MarketCode);
    data["MarketName"] = to_utf(pMarket->MarketName);
    data["TimeZone"] = pMarket->TimeZone;
    data["OpenTime"] = pMarket->OpenTime;
    data["CloseTime"] = pMarket->CloseTime;
    this->on_rsp_market(data);
}

void XcMdApi::OnRespSecurity(QWORD qQuoteID, void* pParam)
{
    // 上海A股证券行情响应
    socket_struct_Security* pSecurity = (socket_struct_Security*)pParam;
    socket_struct_Security_Extend* pExtend = (socket_struct_Security_Extend*)pSecurity->Extend_fields;
    XcDebugInfo(XcDbgFd, "OnRespSecurity Code:%s,Name:%s,PreClose:%.2lf, UpperLimit:%.2lf, LowerLimit:%.2lf\n",
        pSecurity->SecCode, pSecurity->SecName, pSecurity->SecurityClosePx, pSecurity->DailyPriceUpLimit, pSecurity->DailyPriceDownLimit);

    char buf[64] = "";
    sprintf(buf, "%s.%s", pSecurity->SecCode, pSecurity->MarketCode);
    string xc_symbol(buf);

    XcSecurityInfo sec_info = { 0 };
    strcpy(sec_info.ExchangeID, pSecurity->MarketCode);
    strcpy(sec_info.InstrumentID, pSecurity->SecCode);
    strcpy(sec_info.SecName, pSecurity->SecName);
    sec_info.PreClosePrice = pSecurity->SecurityClosePx;
    sec_info.UpperLimitPrice = pSecurity->DailyPriceUpLimit;
    sec_info.LowerLimitPrice = pSecurity->DailyPriceDownLimit;
    secmap[xc_symbol] = sec_info;

    int index = 0;
    while (sec_info.SecName[index])
    {
        if (sec_info.SecName[index] == ' ')
        {
            sec_info.SecName[index] = '\0';
            break;
        }
        ++index;
    }

    gil_scoped_acquire acquire;
    dict data;
    data["ExchangeID"] = to_utf(sec_info.ExchangeID);
    data["InstrumentID"] = to_utf(sec_info.InstrumentID);
    data["SecName"] = to_utf(sec_info.SecName);
    data["PreClosePrice"] = sec_info.PreClosePrice;
    data["UpperLimitPrice"] = sec_info.UpperLimitPrice;
    data["LowerLimitPrice"] = sec_info.LowerLimitPrice;
    this->on_rsp_qry_data(data);
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
    string xc_symbol(buf);

    XcSecurityInfo sec_info = { 0 };
    strcpy(sec_info.ExchangeID, pSecurity->MarketCode);
    strcpy(sec_info.InstrumentID, pSecurity->SecCode);
    strcpy(sec_info.SecName, pSecurity->SecName);
    sec_info.PreClosePrice = pSecurity->PrevClosePx;
    sec_info.UpperLimitPrice = pExtend->T_LimitUpAbsolute;
    sec_info.LowerLimitPrice = pExtend->T_LimitUpAbsolute;
    secmap[xc_symbol] = sec_info;

    int index = 0;
    while (sec_info.SecName[index])
    {
        if (sec_info.SecName[index] == ' ')
        {
            sec_info.SecName[index] = '\0';
            break;
        }
        ++index;
    }

    // for xc test environment
    if (sec_info.LowerLimitPrice < 0.1)
    {
        double limit_rate = 0.1;
        if (strstr(sec_info.SecName, "ST"))
            limit_rate = 0.05;

        char buf[200] = "";
        sprintf(buf, "%.2lf", sec_info.PreClosePrice * (1 + limit_rate));
        sec_info.UpperLimitPrice = atof(buf);

        sprintf(buf, "%.2lf", sec_info.PreClosePrice * (1 - limit_rate));
        sec_info.LowerLimitPrice = atof(buf);
    }

    gil_scoped_acquire acquire;
    dict data;
    data["ExchangeID"] = to_utf(sec_info.ExchangeID);
    data["InstrumentID"] = to_utf(sec_info.InstrumentID);
    data["SecName"] = to_utf(sec_info.SecName);
    data["PreClosePrice"] = sec_info.PreClosePrice;
    data["UpperLimitPrice"] = sec_info.UpperLimitPrice;
    data["LowerLimitPrice"] = sec_info.LowerLimitPrice;
    this->on_rsp_qry_data(data);
}


void XcMdApi::OnRespDyna(QWORD qQuoteID, void* pParam)
{
    // 动态快照行情响应
    socket_struct_Dyna* pDyna = (socket_struct_Dyna*)pParam;
    XcDepthMarketData* pMD;

    char buf[64] = "";
    sprintf(buf, "%s.%s", pDyna->SecCode, pDyna->MarketCode);
    string xc_symbol(buf);
    if (mdmap.count(xc_symbol) == 0)
    {
        XcDepthMarketData lMD = { 0 };
        // XcDebugInfo(XcDbgFd, "---->>>>%s,%s\n", pDyna->SecCode, pDyna->MarketCode);
        strcpy(lMD.InstrumentID, pDyna->SecCode);
        strcpy(lMD.ExchangeID, pDyna->MarketCode);
        mdmap[xc_symbol] = lMD;
    }
    pMD = &mdmap[xc_symbol];

    qidmap[qQuoteID] = pMD;

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

    time_t now = time(NULL);
    struct tm* ptm = localtime(&now);
    sprintf(pMD->TradingDay, "%04d%02d%02d", (ptm->tm_year + 1900), (ptm->tm_mon + 1), ptm->tm_mday);
    strcpy(pMD->ActionDay, pMD->TradingDay);

    if (memcmp(pDyna->MarketCode, "SZSE", 4) == 0)
    {
        // 注意测试环境 180.169.89.22:2222 没有扩展字段，生产环境有
        socket_struct_Dyna_Extend2* pExtend = (socket_struct_Dyna_Extend2*)pDyna->Extend_fields;
        if (pExtend->DownLimit > 100)
        {
            pMD->UpperLimitPrice = pExtend->UpLimit / PRICE_DIV;
            pMD->LowerLimitPrice = pExtend->DownLimit / PRICE_DIV;
        }
        else if (secmap.count(xc_symbol))
        {
            XcSecurityInfo* psec_info = &secmap[xc_symbol];
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
        if (secmap.count(xc_symbol))
        {
            XcSecurityInfo* psec_info = &secmap[xc_symbol];
            pMD->UpperLimitPrice = psec_info->UpperLimitPrice;
            pMD->LowerLimitPrice = psec_info->LowerLimitPrice;
        }
    }
#if 0
    GetLocalTime(&sys_time);
    int Nowtime = sys_time.wHour * 10000 * 1000 + sys_time.wMinute * 100 * 1000 + sys_time.wSecond * 1000 + sys_time.wMilliseconds;
    cout << "OnRespDyna Sec:" << pDyna->SecCode << " open:" << pDyna->Open << " close:" << pDyna->Close << " High:" << pDyna->High << " Low:" << pDyna->Low << " New:" << pDyna->New << " Time:" << pDyna->Time << " LocalTime: " << Nowtime << endl;
    if (memcmp(pDyna->MarketCode, "SSE", 3) == 0)
    {
        socket_struct_Dyna_Extend1* pExtend = (socket_struct_Dyna_Extend1*)pDyna->Extend_fields;
        cout << "SSE Extend: IOPV:" << pExtend->IOPV << " EtfBuyNumber: " << pExtend->EtfBuyNumber << " EtfBuyAmount: " << pExtend->EtfBuyAmount << " EtfBuyMoney: " << pExtend->EtfBuyMoney << endl;
        cout << " EtfSellNumber: " << pExtend->EtfSellNumber << " EtfSellAmount: " << pExtend->EtfSellAmount << " EtfSellMoney: " << pExtend->EtfSellMoney << endl;
        cout << " WithdrawBuyNumber: " << pExtend->WithdrawBuyNumber << " WithdrawBuyAmount: " << pExtend->WithdrawBuyAmount << " WithdrawBuyMoney: " << pExtend->WithdrawBuyMoney << endl;
        cout << " WithdrawSellNumber: " << pExtend->WithdrawSellNumber << " WithdrawSellAmount: " << pExtend->WithdrawSellAmount << " WithdrawSellMoney: " << pExtend->WithdrawSellMoney << endl;
        cout << " TotalBidNumber: " << pExtend->TotalBidNumber << " TotalBidQty: " << pExtend->TotalBidQty << " TotalOfferNumber: " << pExtend->TotalOfferNumber << " TotalOfferQty: " << pExtend->TotalOfferQty << endl;
        cout << " WeightedAvgBidPx: " << pExtend->WeightedAvgBidPx << " WeightedAvgOfferPx: " << pExtend->WeightedAvgOfferPx << endl;
    }
    if (memcmp(pDyna->MarketCode, "SZSE", 4) == 0)
    {
        socket_struct_Dyna_Extend2* pExtend = (socket_struct_Dyna_Extend2*)pDyna->Extend_fields;
        cout << "SZSE Extend: DownLimit:" << pExtend->DownLimit << " UpLimit: " << pExtend->UpLimit << " BidPx: " << pExtend->TotalBidPx << " OfferPx: " << pExtend->TotalOfferPx << " BidQty: " << pExtend->TotalBidQty << " OfferQty: " << pExtend->TotalOfferQty << endl;
        cout << " NAV: " << pExtend->NAV << " IOPV: " << pExtend->IOPV << endl;
    }
#endif
}

void XcMdApi::OnRespDepth(QWORD qQuoteID, char* MarketCode, char* SecCode, socket_struct_DepthDetail* pDepth)
{
    // 动态深度行情响应

    // XcDebugInfo(XcDbgFd, "Depth symbol:%s qid:%ld, grade:%d, px:%d, vol:%d\n",
    //     SecCode, (long)qQuoteID, pDepth->Grade, pDepth->Price, (int)pDepth->Quantity);

    XcDepthMarketData* pMD;
    if (qidmap.count(qQuoteID) == 0)
    {
        return;
    }
    pMD = qidmap[qQuoteID];

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
    default:
        break;
    }
}

void XcMdApi::OnRespDepthOrder(QWORD qQuoteID, char* MarketCode, char* SecCode, int Grade, DWORD Price, socket_struct_DepthOrderDetail* pDepthOrder)
{
    //if (Grade == 1)
    // cout << "out——DepthOrder:" << SecCode << " Grade: " << Grade << " OrderID: " << pDepthOrder->OrderID << " Qty: " << pDepthOrder->Quantity << endl;
}


void XcMdApi::OnRespEachOrder(QWORD qQuoteID, void* pParam)
{
    // 逐笔委托
}

void XcMdApi::OnRespEachDeal(QWORD qQuoteID, void* pParam)
{
    // 逐笔成交
}


//////////////////////////////////////////////////////////////////////////
void XcMdApi::create_md_api(string pszFlowPath)
{
    this->api = CXcMarketApi::CreateMarketApi();
    this->api->Register(this);

    // 设置重连参数
    this->api->SetConnectParam(true, 10, 10000);
}

void XcMdApi::release()
{
    if (this->api)
    {
        this->api->Release();
        this->api = NULL;
    }
}

int XcMdApi::init(string user_id, string server_ip, string server_port, string license)
{
    int rv;
    XcDebugInfo(XcDbgFd, "init user_id:%s, server_ip:%s, server_port:%s, liscense:%s\n",
        user_id.c_str(), server_ip.c_str(), server_port.c_str(), license.c_str());

    rv = this->api->Connect(user_id.c_str(), server_ip.c_str(), server_port.c_str(), license.c_str());
    if (rv < 0)
    {
        XcDebugInfo(XcDbgFd, "xcmdapi Connect failed:%d\n", rv);
    }
    return rv;
}


int XcMdApi::subscribe_md(string instrumentID)
{
    if (instrumentID.length() < 6) {
        return -1;
    }

    interface_struct_Subscribe sub_flag;
    sub_flag.Dyna_flag = true;
    sub_flag.DepthOrder_flag = false;
    sub_flag.Depth_flag = true;
    sub_flag.EachDeal_flag = false;
    sub_flag.EachOrder_flag = false;

    char market[16] = "";
    char symbol[16] = "";
    if (!get_symbol_market(symbol, market, instrumentID))
    {
        return -11;
    }

    if (strcmp(market, Scdm_SZSE) == 0)
    {
        // do once query for get limit price
        // int rv = req_qry_data(instrumentID);
        // fprintf(stderr, "auto qry rv:%d\n", rv);
        // sleepms(50);
    }

    // 订阅
    socket_struct_SubscribeDetail SubList[MaxSize];
    strcpy(SubList[0].MarketCode, market);
    strcpy(SubList[0].SecCode, symbol);
    int SubSize = 1;
    int rv = this->api->Subscribe(refid++, &sub_flag, SubList, SubSize);
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

int XcMdApi::subscribe_md_batch(const std::vector<std::string>& reqs)
{
    interface_struct_Subscribe sub_flag;
    sub_flag.Dyna_flag = true;
    sub_flag.DepthOrder_flag = false;
    sub_flag.Depth_flag = true;
    sub_flag.EachDeal_flag = false;
    sub_flag.EachOrder_flag = false;

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
        ++count;
    }

    if (count)
    {
        return api->Require(refid++, ReqList, count);
    }
    return 0;
}

string XcMdApi::get_api_version()
{
    return string(XC_MDAPI_VERSION);
}

int XcMdApi::open_debug(string log_file, int log_level)
{
    return -1;
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

    void on_msg(const dict &data) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcMdApi, on_msg, data);
        }
        catch (const error_already_set &e)
        {
            cout << e.what() << endl;
        }
    };

    void on_rsp_market(const dict &data) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcMdApi, on_rsp_market, data);
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
};


PYBIND11_MODULE(xcmd, m)
{
    class_<XcMdApi, PyMdApi> mdapi(m, "XcMdApi", module_local());
    mdapi
        .def(init<>())
        .def("create_md_api", &XcMdApi::create_md_api)
        .def("release", &XcMdApi::release)
        .def("init", &XcMdApi::init)
        .def("get_api_version", &XcMdApi::get_api_version)
        .def("open_debug", &XcMdApi::open_debug)
        .def("subscribe_md", &XcMdApi::subscribe_md)
        .def("unsubscribe_md", &XcMdApi::unsubscribe_md)
        .def("unsubscribe_all", &XcMdApi::unsubscribe_all)
        .def("req_qry_data", &XcMdApi::req_qry_data)
        .def("req_qry_data_batch", &XcMdApi::req_qry_data_batch)
        // .def("reqUserLogin", &XcMdApi::reqUserLogin)

        .def("on_front_connected", &XcMdApi::on_front_connected)
        .def("on_front_disconnected", &XcMdApi::on_front_disconnected)
        .def("on_rsp_user_login", &XcMdApi::on_rsp_user_login)
        .def("on_msg", &XcMdApi::on_msg)
        .def("on_rsp_market", &XcMdApi::on_rsp_market)
        .def("on_rsp_qry_data", &XcMdApi::on_rsp_qry_data)
        .def("on_rtn_market_data", &XcMdApi::on_rtn_market_data)
        ;
}
