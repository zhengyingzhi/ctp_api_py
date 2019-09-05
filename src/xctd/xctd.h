#pragma warning(disable:4200)
#pragma once

#include <stdio.h>
#include <string.h>

#include <string>
#include <map>

#include <pybind11/pybind11.h>
#include <XcTradeApi/XcTradeApi.h>
#include <XcTradeApi/XcTrade_Define.h>

// #include <nlohmann/json.hpp>
// using json = nlohmann::json;

// using std::string;
// using std::map;
// using namespace std;
using namespace pybind11;


#define XC_TDAPI_VERSION        "0.0.1"

#define XC_FUNC_QRY_SECINFO     330300      // 证券代码信息查询
#define XC_FUNC_PLACE_ORDER     333002      // 普通委托
#define XC_FUNC_CANCEL_ORDER    333017      // 委托撤单
#define XC_FUNC_QRY_PRUCHASE    333030      // 新股申购查询
#define XC_FUNC_QRY_ORDER       333101      // 证券委托查询
#define XC_FUNC_QRY_TRADE       333102      // 证券成交查询
#define XC_FUNC_QRY_POS_FAST    333103      // 证券持仓快速查询
#define XC_FUNC_QRY_POSITION    333104      // 证券持仓查询
#define XC_FUNC_QRY_MD          400         // 证券行情查询
#define XC_FUNC_LOGIN           331100      // 客户登录
#define XC_FUNC_TRADING_ACCOUNT 331155      // 资金账户获取
#define XC_FUNC_SUB_ORDER       620001      // 订阅回报主推
#define XC_FUNC_RTN_ORDER       620003      // 回报主推


class XcTdApi :public CXcTradeSpi
{
public:
    XcTdApi();
    ~XcTdApi();

    CXcTradeApi*    api;
    FILE*           fp;

    std::string     account_id;

public:
    /* 断开提示*/
    virtual void OnClose(void);

    /* 断线中，正在尝试重连 */
    virtual void OnDisConnect(void);

    /* 重连成功 */
    virtual void OnConnected(void);

    /* 反馈JSON模式应答数据（JSON模式）*/
    virtual void OnRecvJsonMsg(char* pJsonMsg);

public:
    virtual void on_front_connected() {}
    virtual void on_front_disconnected(int reason) {}
    virtual void on_recv_msg(const std::string& msg) {}

public:
    void create_td_api(std::string str = "", int reserve=0);

    void release();

    int init(std::string user_id, std::string server_ip, std::string server_port, std::string license);

    int send_data(int func_id, const std::string& data, int subsystem_no = 0, int branch_no = 0);

    // 获取用户间隔值
    int get_space();
    // 获取用户频率值
    int get_frequency();
    // 获取最近错误信息
    std::string get_last_error_msg();

    // my extend functions
    std::string get_api_version();

    int open_debug(std::string log_file, int log_level);

    // some api for field map
    std::string get_field_buy();
    std::string get_field_sell();
    std::string get_field_exchange_sse();
    std::string get_field_exchange_szse();

private:
    int write_data(int reserve, const char* fmt, ...);
};
