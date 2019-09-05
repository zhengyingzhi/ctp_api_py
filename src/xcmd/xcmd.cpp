#include <time.h>
#include <iostream>
#include <codecvt>
#include <locale>
// #include <condition_variable>

#include "xcmd.h"

#define PRICE_DIV   1000.0

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

XcMdApi::XcMdApi()
    : api()
    , qidmap()
    , mdmap()
{}

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
    fprintf(stderr, "OnUserLogin0:%s\n", pMsg->Desc);

    gil_scoped_acquire acquire;
    this->on_front_connected();

    dict data;
    data["MsgID"] = pMsg->Msgid;
    data["Desc"] = to_utf(pMsg->Desc);
    fprintf(stderr, "OnUserLogin1:%s\n", pMsg->Desc);

    this->on_rsp_user_login(data);
}

void XcMdApi::OnHeart()
{
    //cout << "out:" << "Heart........." << endl;
}

void XcMdApi::OnClose()
{
    // cout << "out:" << "通道关闭." << endl;
    fprintf(stderr, "OnClose\n");
    this->on_front_disconnected(-1);
}

void XcMdApi::OnIssueEnd(QWORD qQuoteID)
{
    fprintf(stderr, "OnIssueEnd qQuoteID:%ld\n", (long)qQuoteID);

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
    fprintf(stderr, "OnMsg qRefID:%ld, MsgId:%d, Msg:%s\n", (long)qRefID, pMsg->Msgid, pMsg->Desc);
}

void XcMdApi::OnRespMarket(QWORD qQuoteID, socket_struct_Market* pMarket)
{
    fprintf(stderr, "OnRespMarket qQuoteID:%ld, Code:%s, Name:%s\n",
        (long)qQuoteID, pMarket->MarketCode, pMarket->MarketName);
}

void XcMdApi::OnRespSecurity(QWORD qQuoteID, void* pParam)
{
    // 上海A股证券行情响应
    socket_struct_Security* pSecurity = (socket_struct_Security*)pParam;
    socket_struct_Security_Extend* pExtend = (socket_struct_Security_Extend*)pSecurity->Extend_fields;
    fprintf(stderr, "OnRespSecurity Code:%s,Name:%s,PreClose:%.2lf\n",
        pSecurity->SecCode, pSecurity->SecName, pSecurity->SecurityClosePx);
    // cout << "OrdMaxFloor: " << pExtend->OrdMaxFloor << " OrdMinFloor: " << pExtend->OrdMinFloor << endl;
}

void XcMdApi::OnRespSecurity_Opt(QWORD qQuoteID, void* pParam)
{
    // 上海期权证券行情响应
    socket_struct_Security_Opt* pSecurity = (socket_struct_Security_Opt*)pParam;
    socket_struct_Security_Opt_Extend* pExtend = (socket_struct_Security_Opt_Extend*)pSecurity->Extend_fields;
    // cout << "out——Security_Opt:" << pSecurity->SecCode << " Name:" << pSecurity->SecName << endl;
}

void XcMdApi::OnRespSecurity_Sz(QWORD qQuoteID, void* pParam)
{
    // 深圳A股证券行情响应
    socket_struct_Security_Sz* pSecurity = (socket_struct_Security_Sz*)pParam;
    socket_struct_Security_Sz_Extend* pExtend = (socket_struct_Security_Sz_Extend*)pSecurity->Extend_fields;
    fprintf(stderr, "OnRespSecurity Code:%s,Name:%s,PreClose:%.2lf\n",
        pSecurity->SecCode, pSecurity->SecName, pSecurity->PrevClosePx);
    // cout << " O_LimitType: " << pExtend->O_LimitType << " O_HasPriceLimit:" << pExtend->O_HasPriceLimit << " O_ReferPriceType: " << pExtend->O_ReferPriceType << " O_PriceTick: " << pExtend->O_PriceTick << " O_LimitUpRate: " << pExtend->O_LimitUpRate << " O_LimitDownRate: " << pExtend->O_LimitDownRate << " O_LimitUpAbsolute: " << pExtend->O_LimitUpAbsolute << " O_LimitDownAbsolute: " << pExtend->O_LimitDownAbsolute << endl;
    // cout << " T_LimitType: " << pExtend->T_LimitType << " T_HasPriceLimit:" << pExtend->T_HasPriceLimit << " T_ReferPriceType: " << pExtend->T_ReferPriceType << " T_PriceTick: " << pExtend->T_PriceTick << " T_LimitUpRate: " << pExtend->O_LimitUpRate << " T_LimitDownRate: " << pExtend->T_LimitDownRate << " T_LimitUpAbsolute: " << pExtend->O_LimitUpAbsolute << " T_LimitDownAbsolute: " << pExtend->T_LimitDownAbsolute << endl;
    // cout << " C_LimitType: " << pExtend->C_LimitType << " C_HasPriceLimit:" << pExtend->C_HasPriceLimit << " C_ReferPriceType: " << pExtend->C_ReferPriceType << " C_PriceTick: " << pExtend->C_PriceTick << " C_LimitUpRate: " << pExtend->O_LimitUpRate << " C_LimitDownRate: " << pExtend->C_LimitDownRate << " C_LimitUpAbsolute: " << pExtend->O_LimitUpAbsolute << " C_LimitDownAbsolute: " << pExtend->C_LimitDownAbsolute << endl;
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
        // fprintf(stderr, "---->>>>%s,%s\n", pDyna->SecCode, pDyna->MarketCode);
        strcpy(lMD.InstrumentID, pDyna->SecCode);
        strcpy(lMD.ExchangeID, pDyna->MarketCode);
        mdmap[xc_symbol] = lMD;
    }
    pMD = &mdmap[xc_symbol];

    qidmap[qQuoteID] = pMD;

    // reset depth field
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
    //if (pDepth->Grade == 1 || pDepth->Grade == -1)
    // cout << "DEPTH-- Market:" << MarketCode << " SecCode: " << SecCode << " Grade: " << pDepth->Grade << " Price: " << pDepth->Price << " Qty: " << pDepth->Quantity << " NumOrders: " << pDepth->NumOrders << " QuoteID: " << qQuoteID << endl;

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
    case 1:
        pMD->AskPrice1 = price;
        pMD->AskVolume1 = quantity;
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
    fprintf(stderr, "init user_id:%s, server_ip:%s, server_port:%s, liscense:%s\n",
        user_id.c_str(), server_ip.c_str(), server_port.c_str(), license.c_str());

    rv = this->api->Connect(user_id.c_str(), server_ip.c_str(), server_port.c_str(), license.c_str());
    if (rv < 0)
    {
        fprintf(stderr, "xcmdapi Connect failed:%d\n", rv);
    }
    return rv;
}


int XcMdApi::subscribe_market_data(string instrumentID)
{
    if (instrumentID.length() < 6) {
        return -1;
    }

    interface_struct_Subscribe sSubscribe1;
    sSubscribe1.Dyna_flag = true;
    sSubscribe1.DepthOrder_flag = false;
    sSubscribe1.Depth_flag = true;
    sSubscribe1.EachDeal_flag = false;
    sSubscribe1.EachOrder_flag = false;

    char market[16] = "";
    char symbol[16] = "";
    strncpy(symbol, instrumentID.c_str(), 6);

    if (instrumentID.find("SSE") != string::npos ||
        instrumentID.find("SH") != string::npos ||
        instrumentID.find("SHA") != string::npos)
    {
        strcpy(market, Scdm_SSE);
    }
    else if (instrumentID.find("SZSE") != string::npos ||
        instrumentID.find("SZ") != string::npos ||
        instrumentID.find("SZA") != string::npos)
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
            return -1;
        }
#else
        return -1;
#endif
    }

    // 订阅
    socket_struct_SubscribeDetail SubList0[MaxSize];
    strcpy(SubList0[0].MarketCode, market);
    strcpy(SubList0[0].SecCode, symbol);
    int SubSize0 = 1;
    if (this->api->Subscribe(1, &sSubscribe1, SubList0, SubSize0) < 0)
    {
        fprintf(stderr, "subscribe failed!\n");
        return -1;
    }

    return 0;
}

int XcMdApi::unsubscribe_market_data(string instrumentID)
{
    return -1;
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
        .def("subscribe_market_data", &XcMdApi::subscribe_market_data)
        .def("unsubscribe_market_data", &XcMdApi::unsubscribe_market_data)
        // .def("reqUserLogin", &XcMdApi::reqUserLogin)

        .def("on_front_connected", &XcMdApi::on_front_connected)
        .def("on_front_disconnected", &XcMdApi::on_front_disconnected)
        .def("on_rsp_user_login", &XcMdApi::on_rsp_user_login)
        .def("on_rtn_market_data", &XcMdApi::on_rtn_market_data)
        ;
}
