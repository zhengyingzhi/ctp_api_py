#pragma warning(disable:4200)
#pragma once

#include <stdio.h>
#include <string.h>

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <XcMarketApi/XcMarketApi.h>
#include <XcMarketApi/Xc_Message_Struct.h>


using namespace std;
using namespace pybind11;

#define MAX_UPDOWN_LEVEL 11

#define COLUMNS_NAMES   "qQuoteID,Time,Symbol,Last,Open,High,Low,Volume,UpperLimit,LowerLimit,PreClose,Count"


//常量
#define ONUSERLOGIN 0
#define ONCLOSE 1
#define ONMSG 2
#define ONRSPMARKET 3
#define ONRSPQRYDATA 4
#define ONRTNDEPTHMARKETDATA 5


typedef enum
{
    PS_None = 0,        // 平
    PS_Up = 1,          // 上涨
    PS_UpLimit = 2,     // 涨停
    PS_Down = 3,        // 下跌
    PS_DownLimit = 4    // 跌停
}price_status_t;

// 自定义证券基本信息
struct XcSecurityInfo
{
    int         iSecCode;
    char        cMarketCode;
    char        InstrumentID[31];
    char        ExchangeID[8];
    char        SecType[10];            // STK or OPT
    char        SecName[50];            // 简称
    char        EndDate[20];            // 最后交易日期
    char        StartDate[10];          // 上市日期
    int32_t     BuyUnit;                // 买数量单位
    int32_t     SellUnit;               // 卖数量单位
    double      PreClosePrice;          // 前收盘价 SecurityClosePx
    double      PriceTick;              // 最小价格变动量 MinMovement
    double      UpperLimitPrice;        // 涨幅上限价格 DailyPriceUpLimit
    double      LowerLimitPrice;        // 跌幅下限价格 DailyPriceDownLimit
    double      ExRightRatio;           // 除权比例
    double      DividendAmount;         // 除息金额
    char        SecurityStatusFlag[50]; // 产品状态标志

    // for option
    char        ContractID[50];
    char        Underlying[20];         // UnderlyingSecCode
    char        UnderlyingName[50];     // UnderlyingSymbol
    char        UnderlyingType[10];
    char        OptionType[3];
    char        CallOrPut[3];
    int32_t     Multiplier;             // ContractMultipleUnit
    double      StrikePrice;            // ExercisePrice 行权价
    char        StrikeDate[10];         // ExerciseDate 行权日
    char        DeliveryDate[10];       // 交割日
    char        ExpireDate[10];         // 到期日
    char        UpdateVersion[10];      // 合约版本号
    uint32_t    TotalLongPosition;      // 当前合约未平仓数
    double      PreSettlePrice;         // SettlePricePx 前结算价
    double      UnderlyingPreClose;     // 标的证券前收盘价
    double      MarginUnit;             // 单位保证金
    double      MarginRatioParam1;      // 保证金计算比例参数一
    double      MarginRatioParam2;      // 保证金计算比例参数二
    // char        SecurityStatusFlag[10]; // 期权合约状态信息标签

    char        Bz[200];                // 备注
};

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
    int64_t Volume;
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
    ///申买价六
    double  BidPrice6;
    ///申买量六
    int     BidVolume6;
    ///申卖价六
    double  AskPrice6;
    ///申卖量六
    int     AskVolume6;
    ///申买价七
    double  BidPrice7;
    ///申买量七
    int     BidVolume7;
    ///申卖价七
    double  AskPrice7;
    ///申卖量七
    int     AskVolume7;
    ///申买价八
    double  BidPrice8;
    ///申买量八
    int     BidVolume8;
    ///申卖价八
    double  AskPrice8;
    ///申卖量八
    int     AskVolume8;
    ///申买价九
    double  BidPrice9;
    ///申买量九
    int     BidVolume9;
    ///申卖价九
    double  AskPrice9;
    ///申卖量九
    int     AskVolume9;
    ///申买价十
    double  BidPrice10;
    ///申买量十
    int     BidVolume10;
    ///申卖价十
    double  AskPrice10;
    ///申卖量十
    int     AskVolume10;

    ///当日均价
    double  AveragePrice;
    ///业务日期
    char    ActionDay[9];

    char            TradingPhaseCode[10];   // 产品实时阶段及标志
    uint32_t        TickCount;          // 成交笔数
    uint32_t        Time;               // 原始时间
    price_status_t PriceStatus;        // 涨跌状态
    price_status_t LimitStatus;        // 涨跌停状态
    double      RefPrice;
    double      IOPV;
    double      UpDownRatio;        // 涨跌比例
    double      PriceDiv;
};


#ifndef MD_MAP_STRING_KEY
#define MD_MAP_STRING_KEY   0
#endif

#if MD_MAP_STRING_KEY
typedef map<string, XcSecurityInfo>      SecInfoMapType;
typedef map<string, XcDepthMarketData>   MDMapType;
#else
typedef map<int, XcSecurityInfo>      SecInfoMapType;
typedef map<int, XcDepthMarketData>   MDMapType;
#endif//MD_MAP_STRING_KEY
typedef map<QWORD, XcDepthMarketData*>   QuoteIDMapType;


//任务结构体
struct Task
{
    int task_name;      //回调函数名称对应的常量
    void *task_data;    //数据指针
    void *task_error;   //错误指针
    int task_id;        //请求id
    bool task_last;     //是否为最后返回
};

class TerminatedError : std::exception
{};

class TaskQueue
{
private:
    queue<Task> queue_;
    mutex mutex_;
    condition_variable cond_;

    bool _terminate = false;

public:
    void push(const Task &task)
    {
        unique_lock<mutex > mlock(mutex_);
        queue_.push(task);
        mlock.unlock();
        cond_.notify_one();
    }

    //取出老的任务
    Task pop()
    {
        unique_lock<mutex> mlock(mutex_);
        cond_.wait(mlock, [&]() {
            return !queue_.empty() || _terminate;
        });
        if (_terminate)
            throw TerminatedError();
        Task task = queue_.front();
        queue_.pop();
        return task;
    }

    void terminate()
    {
        _terminate = true;
        cond_.notify_all();
    }
};


class XcMdApi :public CXcMarketSpi
{
public:
    XcMdApi();
    ~XcMdApi();

    CXcMarketApi*   m_api;
    MDMapType       m_mdmap;
    SecInfoMapType  m_secmap;
    std::vector<XcDepthMarketData*> m_pendings;

    char            m_trading_day[9];
    QWORD           refid;
    int             flush_count;
    int             have_level10;
    int             log_level;
    FILE*           fp;

    // std::vector<XC_PUBLIC_SUBSCRIBE_DETAIL_T> m_SubList;
    std::mutex  m_Lock;

    thread m_task_thread;
    TaskQueue m_task_queue;
    bool m_active = false;
    int m_thread_mode = 0;

private:
    void processTask();
    void processOnUserLogin(Task *task);
    void processOnClose(Task *task);
    void processOnMsg(Task *task);
    void processOnRspMarket(Task *task);
    void processOnRspQryData(Task *task);
    void processOnRtnMarketData(Task *task, int isIOThread=0);

public:
#if MD_MAP_STRING_KEY
    XcSecurityInfo* get_sec_info(const std::string& symbol);
    void set_sec_info(const std::string& symbol, const XcSecurityInfo& pSecInfo);
    void del_sec_info(const std::string& symbol);
#else
    XcSecurityInfo* get_sec_info(int isymbol);
    void set_sec_info(int isymbol, const XcSecurityInfo& aSecInfo);
    void del_sec_info(int isymbol);
#endif//MD_MAP_STRING_KEY

public:
    virtual void OnUserLogin(void* apMsg, std::size_t Size);
    virtual void OnRespMarket(char StationId, QWORD qQuoteID, void* apMarket, std::size_t Size);
    virtual void OnRespSecurity(char StationId, QWORD qQuoteID, char MarketCode, void* apSecurity, std::size_t Size);

    // 所有静态信息均通过 OnRespSecurity 返回了
    void OnRespSecurity_SH(char StationId, QWORD qQuoteID, char MarketCode, void* apSecurity, std::size_t Size);
    void OnRespSecurity_Opt(char StationId, QWORD qQuoteID, char MarketCode, void* apSecurity, std::size_t Size);
    void OnRespSecurity_Sz(char StationId, QWORD qQuoteID, char MarketCode, void* apSecurity, std::size_t Size);
    void OnRespSecurity_Index(char StationId, QWORD qQuoteID, char MarketCode, void* apSecurity, std::size_t Size);

    virtual void OnRespDyna(char StationId, QWORD qQuoteID, DWORD SplitId, char MarketCode, void* apDyna, std::size_t Size);
    virtual void OnDynaEnd(char StationId, QWORD qQuoteID, DWORD SplitId, char MarketCode, int SecCode);
    virtual void OnRespDepth(char StationId, QWORD qQuoteID, DWORD SplitId, char MarketCode, int SecCode, void* apDepth, std::size_t Size);
    virtual void OnRespDepthOrder(char StationId, QWORD qQuoteID, DWORD SplitId, char MarketCode, int SecCode, int Grade, DWORD Price, void* apDepthOrder, std::size_t Size);
    virtual void OnRespEachOrder(char StationId, QWORD qQuoteID, char MarketCode, int SecCode, void* pEachOrder, std::size_t Size);
    virtual void OnRespEachDeal(char StationId, QWORD qQuoteID, char MarketCode, int SecCode, void* pEachDeal, std::size_t Size);
    virtual void OnHeart();
    virtual void OnIssueEnd(QWORD qQuoteID);  // deprecated ?
    virtual void OnMsg(QWORD qRefID, void* apMsg, std::size_t Size);
    virtual void OnClose();

public:
    virtual void on_front_connected() {}

    virtual void on_front_disconnected(int reason) {}

    // just notify login result
    virtual void on_rsp_user_login(const dict &data) {}

    virtual void on_rsp_qry_data(const dict& data) {}

    // rtn md fields reference to XcDepthMarketData
    virtual void on_rtn_market_data(const dict &data) {}

    virtual void on_rtn_depth_order(const dict& data) {}
    virtual void on_rtn_each_order(const dict& data) {}
    virtual void on_rtn_each_deal(const dict& data) {}

    virtual void on_msg(int ref_id, const dict& data) {}
    virtual void on_rsp_market(int ref_id, const dict& data) {}

    // new add
    virtual void on_rtn_bar_data(const dict& data) {}

public:
    // for compatible old interface
    int set_thread_mode(int on);
    void exit();

    int set_connect_param(int is_reconnect, int reconnect_ms, int reconect_count);
    int subscribe_md(const std::string& symbol, int depth_order = 0, int each_flag = 0);
    int req_qry_data(const std::string& symbol);

    // set have level10 md
    void set_have_level10(int on);

    // my extend functions
    string get_api_version();

    int open_debug(std::string log_file, int log_level);

    // write msg into log file
    int write_line(int reserve, const std::string& line);

    //////////////////////////////////////////////////////////////////////////
    // some raw api wrapper
    int CreateMarketApi();
    int Register(const std::string& user_id, const std::string& license);
    int Release();
    int Connect(std::string server_addr);
    int SetConnectParam(int aProtocolType, int aReconnect, int aReconnectTime);
    int Subscribe(int aRefID, const std::vector<std::string>& req_symbols, const std::string& types, const std::string& topic, const std::string& station_id);
    int UnSubscribe(int aRefID, const std::vector<std::string>& req_symbols, const std::string& types, const std::string& topic, const std::string& station_id);
    int Require(int aRefID, const std::string& symbol);
    int RequireBatch(int aRefID, const std::vector<std::string>& req_symbols);
    std::string GetLastErrorMsg();

private:
    int write_data(int reserve, const char* fmt, ...);
};
