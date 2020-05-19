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


#ifndef HAVE_BAR_GENERATOR
// #define HAVE_BAR_GENERATOR 1
#endif//HAVE_BAR_GENERATOR

#if HAVE_BAR_GENERATOR
#include "md_utils/bar_generator.h"
#include "md_utils/md_utils.h"
#endif//HAVE_BAR_GENERATOR


using namespace std;
using namespace pybind11;

#define MAX_UPDOWN_LEVEL 11

#define COLUMNS_NAMES   "qQuoteID,Time,Symbol,Last,Open,High,Low,Volume,UpperLimit,LowerLimit,PreClose,Count"


//����
#define ONUSERLOGIN 0
#define ONCLOSE 1
#define ONMSG 2
#define ONRSPMARKET 3
#define ONRSPQRYDATA 4
#define ONRTNDEPTHMARKETDATA 5


typedef enum
{
    PS_None = 0,        // ƽ
    PS_Up = 1,          // ����
    PS_UpLimit = 2,     // ��ͣ
    PS_Down = 3,        // �µ�
    PS_DownLimit = 4    // ��ͣ
}price_status_t;

// �Զ���֤ȯ������Ϣ
struct XcSecurityInfo
{
    char        InstrumentID[31];
    char        ExchangeID[8];
    char        SecType[10];            // STK or OPT
    char        SecName[50];            // ���
    char        EndDate[20];            // ���������
    char        StartDate[10];          // ��������
    int32_t     BuyUnit;                // ��������λ
    int32_t     SellUnit;               // ��������λ
    double      PreClosePrice;          // ǰ���̼� SecurityClosePx
    double      PriceTick;              // ��С�۸�䶯�� MinMovement
    double      UpperLimitPrice;        // �Ƿ����޼۸� DailyPriceUpLimit
    double      LowerLimitPrice;        // �������޼۸� DailyPriceDownLimit
    double      ExRightRatio;           // ��Ȩ����
    double      DividendAmount;         // ��Ϣ���
    char        SecurityStatusFlag[50]; // ��Ʒ״̬��־

    // for option
    char        ContractID[50];
    char        Underlying[20];         // UnderlyingSecCode
    char        UnderlyingName[50];     // UnderlyingSymbol
    char        UnderlyingType[10];
    char        OptionType[3];
    char        CallOrPut[3];
    int32_t     Multiplier;             // ContractMultipleUnit
    double      StrikePrice;            // ExercisePrice ��Ȩ��
    char        StrikeDate[10];         // ExerciseDate ��Ȩ��
    char        DeliveryDate[10];       // ������
    char        ExpireDate[10];         // ������
    char        UpdateVersion[10];      // ��Լ�汾��
    uint32_t    TotalLongPosition;      // ��ǰ��Լδƽ����
    double      PreSettlePrice;         // SettlePricePx ǰ�����
    double      UnderlyingPreClose;     // ���֤ȯǰ���̼�
    double      MarginUnit;             // ��λ��֤��
    double      MarginRatioParam1;      // ��֤������������һ
    double      MarginRatioParam2;      // ��֤��������������
    // char        SecurityStatusFlag[10]; // ��Ȩ��Լ״̬��Ϣ��ǩ

    char        Bz[200];                // ��ע
};

// �Զ�������ṹ��
struct XcDepthMarketData
{
    ///������
    char    TradingDay[9];
    ///��Լ����
    char    InstrumentID[31];
    ///����������
    char    ExchangeID[8];
    ///���¼�
    double  LastPrice;
    ///�ϴν����
    double  PreSettlementPrice;
    ///������
    double  PreClosePrice;
    ///��ֲ���
    double  PreOpenInterest;
    ///����
    double  OpenPrice;
    ///��߼�
    double  HighestPrice;
    ///��ͼ�
    double  LowestPrice;
    ///����
    int     Volume;
    ///�ɽ����
    double  Turnover;
    ///�ֲ���
    double  OpenInterest;
    ///������
    double  ClosePrice;
    ///���ν����
    double  SettlementPrice;
    ///��ͣ���
    double  UpperLimitPrice;
    ///��ͣ���
    double  LowerLimitPrice;
    ///����ʵ��
    double  PreDelta;
    ///����ʵ��
    double  CurrDelta;
    ///����޸�ʱ��
    char    UpdateTime[9];
    ///����޸ĺ���
    int     UpdateMillisec;
    ///�����һ
    double  BidPrice1;
    ///������һ
    int     BidVolume1;
    ///������һ
    double  AskPrice1;
    ///������һ
    int     AskVolume1;
    ///����۶�
    double  BidPrice2;
    ///��������
    int     BidVolume2;
    ///�����۶�
    double  AskPrice2;
    ///��������
    int     AskVolume2;
    ///�������
    double  BidPrice3;
    ///��������
    int     BidVolume3;
    ///��������
    double  AskPrice3;
    ///��������
    int     AskVolume3;
    ///�������
    double  BidPrice4;
    ///��������
    int     BidVolume4;
    ///��������
    double  AskPrice4;
    ///��������
    int     AskVolume4;
    ///�������
    double  BidPrice5;
    ///��������
    int     BidVolume5;
    ///��������
    double  AskPrice5;
    ///��������
    int     AskVolume5;
    ///�������
    double  BidPrice6;
    ///��������
    int     BidVolume6;
    ///��������
    double  AskPrice6;
    ///��������
    int     AskVolume6;
    ///�������
    double  BidPrice7;
    ///��������
    int     BidVolume7;
    ///��������
    double  AskPrice7;
    ///��������
    int     AskVolume7;
    ///����۰�
    double  BidPrice8;
    ///��������
    int     BidVolume8;
    ///�����۰�
    double  AskPrice8;
    ///��������
    int     AskVolume8;
    ///����۾�
    double  BidPrice9;
    ///��������
    int     BidVolume9;
    ///�����۾�
    double  AskPrice9;
    ///��������
    int     AskVolume9;
    ///�����ʮ
    double  BidPrice10;
    ///������ʮ
    int     BidVolume10;
    ///������ʮ
    double  AskPrice10;
    ///������ʮ
    int     AskVolume10;

    ///���վ���
    double  AveragePrice;
    ///ҵ������
    char    ActionDay[9];

    char            TradingPhaseCode[10];   // ��Ʒʵʱ�׶μ���־
    uint32_t        TickCount;          // �ɽ�����
    uint32_t        Time;               // ԭʼʱ��
    price_status_t PriceStatus;        // �ǵ�״̬
    price_status_t LimitStatus;        // �ǵ�ͣ״̬
    double      RefPrice;
    double      UpDownRatio;        // �ǵ�����
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


//����ṹ��
struct Task
{
    int task_name;      //�ص��������ƶ�Ӧ�ĳ���
    void *task_data;    //����ָ��
    void *task_error;   //����ָ��
    int task_id;        //����id
    bool task_last;     //�Ƿ�Ϊ��󷵻�
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

    //ȡ���ϵ�����
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

    CXcMarketApi*   api;
    // QuoteIDMapType  qidmap;
    MDMapType       mdmap;
    SecInfoMapType  secmap;
    std::vector<XcDepthMarketData*> m_pendings;

    char            trading_day[9];
    QWORD           refid;
    int             flush_count;
    int             have_level10;
    int             log_level;
    FILE*           fp;

    std::vector<socket_struct_SubscribeDetail> m_SubList;
    std::mutex  m_Lock;

    thread m_task_thread;
    TaskQueue m_task_queue;
    bool m_active = false;
    int m_thread_mode = 0;

    bool depth_flag = true;
    bool bar_mode = false;
#if HAVE_BAR_GENERATOR
    std::map<int, bar_generator_t*> bar_gen_map;
#endif//HAVE_BAR_GENERATOR

private:
    void processTask();
    void processOnUserLogin(Task *task);
    void processOnClose(Task *task);
    void processOnMsg(Task *task);
    void processOnRspMarket(Task *task);
    void processOnRspQryData(Task *task);
    void processOnRtnMarketData(Task *task, int isIOThread=0);

#if MD_MAP_STRING_KEY
    XcSecurityInfo* get_sec_info(const std::string& symbol);
#else
    XcSecurityInfo* get_sec_info(int isymbol);
#endif//MD_MAP_STRING_KEY

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
    void create_md_api(string str = "");

    void release();

    int set_connect_param(int is_reconnect, int reconnect_ms, int reconect_count);

    int set_thread_mode(int on);

    // connect to server
    int init(string user_id, string server_ip, string server_port, string license);

    void exit();

    // subscribe eg. 000001.SZSE or 600000.SSE
    int subscribe_md(string instrument, int depth_order = 0, int each_flag = 0);
    int unsubscribe_md(string instrument);

    int subscribe_md_batch(const std::vector<std::string>& reqs, int depth_order = 0, int each_flag = 0);
    int unsubscribe_md_batch(const std::vector<std::string>& reqs);
    int unsubscribe_all();

    // request query data
    int req_qry_data(const string& symbol);
    int req_qry_data_batch(const std::vector<std::string>& reqs);

    // set have level10 md
    void set_have_level10(int on);

    // my extend functions
    string get_api_version();

    int open_debug(string log_file, int log_level);

    // write msg into log file
    int write_line(int reserve, const std::string& line);

#if HAVE_BAR_GENERATOR
    // new add
    int subscribe_bar_md(std::string instrument);
    void process_bar_gen(socket_struct_Dyna* apMD);
#endif//HAVE_BAR_GENERATOR

    //////////////////////////////////////////////////////////////////////////
    // some raw api wrapper
    int Subscribe(const std::string& exchange, const std::string& instrument, int dyna_flag, int depth_flag, int depth_order, int each_flag);
    int Require(int ref_id, const std::string& exchange, const std::string& instrument);

private:
    int write_data(int reserve, const char* fmt, ...);
};
