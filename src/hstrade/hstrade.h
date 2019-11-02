#ifndef _HS_TRADE_INCLUDED_H_
#define _HS_TRADE_INCLUDED_H_


#if defined(_WIN32)
#ifdef HSTRADE_EXPORTS
#define HS_TRADE_API        __declspec(dllexport)
#else
#define HS_TRADE_API        __declspec(dllimport)
#endif
#define HS_TRADE_STDCALL    __stdcall       /* ensure stcall calling convention on NT */

#else
#define HS_TRADE_API
#define HS_TRADE_STDCALL                   /* leave blank for other systems */

#endif//_WIN32


#include "hstrade_struct.h"


#ifdef __cplusplus
extern "C"
{
#endif//__cplusplus


/* exported types */
typedef struct hstrade_s hstrade_t;

typedef struct hstrade_spi_s
{
    void (*on_connected)(hstrade_t*);
    void (*on_disconnected)(hstrade_t*, int reason);
    void (*on_rsp_subscribe)(hstrade_t*, HSRspSubscribeField* rsp_sub);
    void (*on_user_login)(hstrade_t*, HSRspUserLoginField* rsp_login, HSRspInfoField* rsp_info);
    void (*on_order_insert)(hstrade_t*, HSReqOrderInsertField*, HSRspInfoField* rsp_info, int islast);
    void (*on_order_action)(hstrade_t*, HSReqOrderInsertField*, HSRspInfoField* rsp_info, int islast);

    void (*on_qry_trading_account)(hstrade_t*, HSTradingAccountField*, HSRspInfoField* rsp_info, int islast);
    void (*on_qry_position)(hstrade_t*, HSPositionField*, HSRspInfoField* rsp_info, int islast);
    void (*on_qry_order)(hstrade_t*, HSOrderField*, HSRspInfoField* rsp_info, int islast);
    void (*on_qry_trade)(hstrade_t*, HSTradeField*, HSRspInfoField* rsp_info, int islast);
    void (*on_qry_security_info)(hstrade_t*, HSSecurityInfoField*, HSRspInfoField* rsp_info, int islast);
    void (*on_qry_md)(hstrade_t*, HSMarketDataField*, HSRspInfoField* rsp_info, int islast);

    void(*on_qry_cash_hist)(hstrade_t*, HSTradingAccountField*, HSRspInfoField* rsp_info, int islast);
    void(*on_qry_order_hist)(hstrade_t*, HSOrderField*, HSRspInfoField* rsp_info, int islast);
    void(*on_qry_trade_hist)(hstrade_t*, HSTradeField*, HSRspInfoField* rsp_info, int islast);

    void (*on_rtn_order)(hstrade_t*, HSOrderField*);
    void (*on_rtn_trade)(hstrade_t*, HSTradeField*);

    // on json msg only when json mode
    void (*on_json_msg)(hstrade_t*, int func_id, int issue_type, const char* json_msg);

    // on raw api bizmsg IBizMessage* object
    void (*on_raw_bizmsg)(hstrade_t*, void* pmsg);
}hstrade_spi_t;



/* exported apis */
/// 获取版本号
HS_TRADE_API const char* HS_TRADE_STDCALL hstrade_version(int* pver);

/// 创建对象
HS_TRADE_API hstrade_t* HS_TRADE_STDCALL hstrade_create(int is_async);

/// 销毁对象
HS_TRADE_API void HS_TRADE_STDCALL hstrade_realese(hstrade_t* hstd);

/// 用户数据上下文
HS_TRADE_API void  HS_TRADE_STDCALL hstrade_set_userdata(hstrade_t* hstd, void* userdata);
HS_TRADE_API void* HS_TRADE_STDCALL hstrade_get_userdata(hstrade_t* hstd);

/// 调试模式
HS_TRADE_API void HS_TRADE_STDCALL hstrade_debug_mode(hstrade_t* hstd, int level);

/// 注册回调
HS_TRADE_API void HS_TRADE_STDCALL hstrade_register_spi(hstrade_t* hstd, hstrade_spi_t* spi);

/// 设置API选项
HS_TRADE_API int HS_TRADE_STDCALL hstrade_set_option(hstrade_t* hstd, const char* option_name, const void* option_value, int value_size);

/// 发起连接
HS_TRADE_API int HS_TRADE_STDCALL hstrade_init(hstrade_t* hstd, const char* server_addr, const char* license_file, const char* fund_account, int timeoutms);

/// 订阅主推回报
HS_TRADE_API int HS_TRADE_STDCALL hstrade_subscribe(hstrade_t* hstd, int issue_type);

/// 获取交易日 yyyymmdd
HS_TRADE_API const char* HS_TRADE_STDCALL hstrade_get_trading_day(hstrade_t* hstd);

/// 请求登录
HS_TRADE_API int HS_TRADE_STDCALL hstrade_user_login(hstrade_t* hstd, HSReqUserLoginField* login_req);

/// 请求报单
HS_TRADE_API int HS_TRADE_STDCALL hstrade_order_insert(hstrade_t* hstd, HSReqOrderInsertField* order_req);

/// 请求撤单
HS_TRADE_API int HS_TRADE_STDCALL hstrade_order_action(hstrade_t* hstd, HSReqOrderActionField* order_action);

/// 请求查询资金账户
HS_TRADE_API int HS_TRADE_STDCALL hstrade_qry_trading_account(hstrade_t* hstd, HSReqQueryField* qry_field);

/// 请求查询投资者账户
HS_TRADE_API int HS_TRADE_STDCALL hstrade_qry_investor(hstrade_t* hstd, HSReqQueryField* qry_field);

/// 请求查询持仓
HS_TRADE_API int HS_TRADE_STDCALL hstrade_qry_position(hstrade_t* hstd, HSReqQueryField* qry_field);

/// 请求查询成交
HS_TRADE_API int HS_TRADE_STDCALL hstrade_qry_trade(hstrade_t* hstd, HSReqQueryField* qry_field);

/// 请求查询委托
HS_TRADE_API int HS_TRADE_STDCALL hstrade_qry_order(hstrade_t* hstd, HSReqQueryField* qry_field);

/// 请求查询证券信息
HS_TRADE_API int HS_TRADE_STDCALL hstrade_qry_security_info(hstrade_t* hstd, HSReqQueryField* qry_field);

/// 请求查询行情
HS_TRADE_API int HS_TRADE_STDCALL hstrade_qry_md(hstrade_t* hstd, HSReqQueryField* qry_field);


/// 请求查询历史资金流水
HS_TRADE_API int HS_TRADE_STDCALL hstrade_qry_cash_hist(hstrade_t* hstd, HSReqQueryField* qry_field);

/// 请求查询历史成交
HS_TRADE_API int HS_TRADE_STDCALL hstrade_qry_trade_hist(hstrade_t* hstd, HSReqQueryField* qry_field);

/// 请求查询历史委托
HS_TRADE_API int HS_TRADE_STDCALL hstrade_qry_order_hist(hstrade_t* hstd, HSReqQueryField* qry_field);


/// some raw apis for easily use
HS_TRADE_API void* HS_TRADE_STDCALL hstrade_begin_pack(hstrade_t* hstd);
HS_TRADE_API int HS_TRADE_STDCALL hstrade_end_pack(hstrade_t* hstd, void* packer);
HS_TRADE_API int HS_TRADE_STDCALL hstrade_send_bizmsg(hstrade_t* hstd, void* packer, int func_id);

/// add pack field，field_type: I(Integer), S(String), C(Char), F(Double)
HS_TRADE_API int HS_TRADE_STDCALL hstrade_add_field(hstrade_t* hstd, void* packer, const char* key, char field_type, int field_width);

/// add pack values
HS_TRADE_API int HS_TRADE_STDCALL hstrade_pack_char(hstrade_t* hstd, void* packer, const char value);
HS_TRADE_API int HS_TRADE_STDCALL hstrade_pack_string(hstrade_t* hstd, void* packer, const char* value);
HS_TRADE_API int HS_TRADE_STDCALL hstrade_pack_int(hstrade_t* hstd, void* packer, const int value);
HS_TRADE_API int HS_TRADE_STDCALL hstrade_pack_double(hstrade_t* hstd, void* packer, const double value);


#ifdef __cplusplus
}


#endif//__cplusplus


#endif//_HS_TRADE_INCLUDED_H_
