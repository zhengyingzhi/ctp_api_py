#pragma warning(disable:4200)
#pragma once

#include <stdio.h>
#include <string.h>

#include <string>
#include <map>
#include <vector>
#include <mutex>

#include <pybind11/pybind11.h>
#include <XcMarketApi/XcMarketApi.h>

using namespace std;
using namespace pybind11;

#define MAX_UPDOWN_LEVEL 11

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

    price_status_t PriceStatus;        // �ǵ�״̬
    price_status_t LimitStatus;        // �ǵ�ͣ״̬
    double      UpDownRatio;        // �ǵ�����
};
typedef map<string, XcDepthMarketData>   MDMapType;
typedef map<QWORD, XcDepthMarketData*>   QuoteIDMapType;

typedef map<string, XcSecurityInfo>      SecInfoMapType;


class XcMdApi :public CXcMarketSpi
{
public:
    XcMdApi();
    ~XcMdApi();

    CXcMarketApi*   api;
    QuoteIDMapType  qidmap;
    MDMapType       mdmap;
    SecInfoMapType  secmap;
    QWORD           refid;
    int             have_level10;
    int             statistic_mode;
    int             log_level;
    FILE*           fp;

    uint32_t    m_UpStatistics[MAX_UPDOWN_LEVEL];
    uint32_t    m_DownStatistics[MAX_UPDOWN_LEVEL];
    uint32_t    m_UpCount;
    uint32_t    m_DownCount;
    uint32_t    m_UpLimitCount;
    uint32_t    m_DownLimitCount;
    std::vector<socket_struct_SubscribeDetail> m_SubList;
    std::mutex  m_SubListLock;

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

public:
    void create_md_api(string str = "");

    void release();

    int set_connect_param(int is_reconnect, int reconnect_ms, int reconect_count);

    // connect to server
    int init(string user_id, string server_ip, string server_port, string license);

    // subscribe eg. 000001.SZSE or 600000.SSE
    int subscribe_md(string instrument, int depth_order = 0, int each_flag = 0);
    int unsubscribe_md(string instrument);

    int subscribe_md_batch(const std::vector<std::string>& reqs, int depth_order = 0, int each_flag = 0);
    int unsubscribe_md_batch(const std::vector<std::string>& reqs);
    int unsubscribe_all();

    // request query data
    int req_qry_data(string instrument);
    int req_qry_data_batch(const std::vector<std::string>& reqs);

    // set have level10 md
    void set_have_level10(int on);

    // set statistic mode
    void set_statistic_mode();

    // my extend functions
    string get_api_version();

    int open_debug(string log_file, int log_level);

    // write msg into log file
    int write_line(int reserve, const std::string& line);

    // get statistics
    int get_statistics(dict out);

    void statistic_md(XcDepthMarketData* pMD, int32_t isfirst);
    int get_ratio_count(double ratio, int cond);

private:
    int write_data(int reserve, const char* fmt, ...);
};