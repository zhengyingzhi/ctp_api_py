#pragma warning(disable:4200)
#pragma once

#include <stdio.h>
#include <string.h>

#include <string>
#include <map>

#include <pybind11/pybind11.h>
#include <XcMarketApi/XcMarketApi.h>

using namespace std;
using namespace pybind11;


#define XC_MDAPI_VERSION    "0.0.1"

// 自定义行情结构体
struct XcDepthMarketData
{
    ///交易日
    char    TradingDay[9];
    ///合约代码
    char    InstrumentID[31];
    ///交易所代码
    char    ExchangeID[8];
    ///最新价
    double  LastPrice;
    ///上次结算价
    double  PreSettlementPrice;
    ///昨收盘
    double  PreClosePrice;
    ///昨持仓量
    double  PreOpenInterest;
    ///今开盘
    double  OpenPrice;
    ///最高价
    double  HighestPrice;
    ///最低价
    double  LowestPrice;
    ///数量
    int     Volume;
    ///成交金额
    double  Turnover;
    ///持仓量
    double  OpenInterest;
    ///今收盘
    double  ClosePrice;
    ///本次结算价
    double  SettlementPrice;
    ///涨停板价
    double  UpperLimitPrice;
    ///跌停板价
    double  LowerLimitPrice;
    ///昨虚实度
    double  PreDelta;
    ///今虚实度
    double  CurrDelta;
    ///最后修改时间
    char    UpdateTime[9];
    ///最后修改毫秒
    int     UpdateMillisec;
    ///申买价一
    double  BidPrice1;
    ///申买量一
    int     BidVolume1;
    ///申卖价一
    double  AskPrice1;
    ///申卖量一
    int     AskVolume1;
    ///申买价二
    double  BidPrice2;
    ///申买量二
    int     BidVolume2;
    ///申卖价二
    double  AskPrice2;
    ///申卖量二
    int     AskVolume2;
    ///申买价三
    double  BidPrice3;
    ///申买量三
    int     BidVolume3;
    ///申卖价三
    double  AskPrice3;
    ///申卖量三
    int     AskVolume3;
    ///申买价四
    double  BidPrice4;
    ///申买量四
    int     BidVolume4;
    ///申卖价四
    double  AskPrice4;
    ///申卖量四
    int     AskVolume4;
    ///申买价五
    double  BidPrice5;
    ///申买量五
    int     BidVolume5;
    ///申卖价五
    double  AskPrice5;
    ///申卖量五
    int     AskVolume5;
    ///当日均价
    double  AveragePrice;
    ///业务日期
    char    ActionDay[9];
};
typedef map<string, XcDepthMarketData>   MDMapType;
typedef map<QWORD, XcDepthMarketData*>   QuoteIDMapType;


class XcMdApi :public CXcMarketSpi
{
public:
    XcMdApi();
    ~XcMdApi();

    CXcMarketApi*   api;
    QuoteIDMapType  qidmap;
    MDMapType       mdmap;

public:
    void OnUserLogin(socket_struct_Msg* pMsg);
    void OnRespMarket(QWORD qQuoteID, socket_struct_Market* pMarket);
    void OnRespSecurity(QWORD qQuoteID, void* pParam);
    void OnRespSecurity_Opt(QWORD qQuoteID, void* pParam);
    void OnRespSecurity_Sz(QWORD qQuoteID, void* pParam);
    void OnRespDyna(QWORD qQuoteID, void* pParam);
    void OnRespDepth(QWORD qQuoteID, char* MarketCode, char* SecCode, socket_struct_DepthDetail* pDepth);
    void OnRespDepthOrder(QWORD qQuoteID, char* MarketCode, char* SecCode, int Grade, DWORD Price, socket_struct_DepthOrderDetail* pDepthOrder);
    void OnRespEachOrder(QWORD qQuoteID, void* pParam);
    void OnRespEachDeal(QWORD qQuoteID, void* pParam);
    void OnHeart();
    void OnIssueEnd(QWORD qQuoteID);
    void OnMsg(QWORD qRefID, socket_struct_Msg* pMsg);
    void OnClose();

public:
    virtual void on_front_connected() {}

    virtual void on_front_disconnected(int reason) {}

    // just notify login result
    virtual void on_rsp_user_login(const dict &data) {}

    // rtn md fields reference to XcDepthMarketData
    virtual void on_rtn_market_data(const dict &data) {}

public:
    void create_md_api(string str = "");

    void release();

    // connect to server
    int init(string user_id, string server_ip, string server_port, string license);

    // subscribe eg. 000001.SZSE or 600000.SSE
    int subscribe_md(string instrument);
    int unsubscribe_md(string instrument);

    int subscribe_md_batch(const vector<std::string>& reqs);
    int unsubscribe_md_batch(const vector<std::string>& reqs);
    int unsubscribe_all();

    // my extend functions
    string get_api_version();

    int open_debug(string log_file, int log_level);
};
