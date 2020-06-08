#ifndef _UFX_TRADE_INCLUDED_H_
#define _UFX_TRADE_INCLUDED_H_


#if defined(_WIN32)
#ifdef HSTRADE_EXPORTS
#define UFX_TRADE_API        __declspec(dllexport)
#else
#define UFX_TRADE_API        __declspec(dllimport)
#endif
#define UFX_TRADE_STDCALL    __stdcall       /* ensure stcall calling convention on NT */

#else
#define UFX_TRADE_API
#define UFX_TRADE_STDCALL                   /* leave blank for other systems */

#endif//_WIN32


#include "ufxtrade_struct.h"


#ifdef __cplusplus
extern "C"
{
#endif//__cplusplus


/* exported types */
typedef struct ufxtrade_s ufxtrade_t;

typedef struct ufxtrade_spi_s
{
    void (*on_connected)(ufxtrade_t*);
    void (*on_disconnected)(ufxtrade_t*, int reason);
    void (*on_rsp_subscribe)(ufxtrade_t*, UFXRspSubscribeField* rsp_sub, int ref_id);
    void (*on_user_login)(ufxtrade_t*, UFXRspUserLoginField* rsp_login, UFXRspInfoField* rsp_info, int ref_id);
    void (*on_order_insert)(ufxtrade_t*, HSOrderField*, UFXRspInfoField* rsp_info, int ref_id, int islast);
    void (*on_order_action)(ufxtrade_t*, HSOrderField*, UFXRspInfoField* rsp_info, int ref_id, int islast);

    void (*on_qry_trading_account)(ufxtrade_t*, HSTradingAccountField*, UFXRspInfoField* rsp_info, int ref_id, int islast);
    void (*on_qry_position)(ufxtrade_t*, HSPositionField*, UFXRspInfoField* rsp_info, int ref_id, int islast);
    void (*on_qry_order)(ufxtrade_t*, HSOrderField*, UFXRspInfoField* rsp_info, int ref_id, int islast);
    void (*on_qry_trade)(ufxtrade_t*, HSTradeField*, UFXRspInfoField* rsp_info, int ref_id, int islast);
    void (*on_qry_security_info)(ufxtrade_t*, HSSecurityInfoField*, UFXRspInfoField* rsp_info, int ref_id, int islast);
    void (*on_qry_md)(ufxtrade_t*, HSMarketDataField*, UFXRspInfoField* rsp_info, int ref_id, int islast);

    void(*on_qry_cash_hist)(ufxtrade_t*, HSTradingAccountField*, UFXRspInfoField* rsp_info, int ref_id, int islast);
    void(*on_qry_order_hist)(ufxtrade_t*, HSOrderField*, UFXRspInfoField* rsp_info, int ref_id, int islast);
    void(*on_qry_trade_hist)(ufxtrade_t*, HSTradeField*, UFXRspInfoField* rsp_info, int ref_id, int islast);

    void (*on_rtn_order)(ufxtrade_t*, HSOrderField* order, int ref_id);
    void (*on_rtn_trade)(ufxtrade_t*, HSTradeField* trade, int ref_id);

    // on json msg only when json mode
    void (*on_msg_error)(ufxtrade_t*, int func_id, int issue_type, int ref_id, int error_no, const char* error_info);
    void (*on_json_msg)(ufxtrade_t*, int func_id, int issue_type, int ref_id, const char* json_msg);

    // on raw api bizmsg IBizMessage* object, IF2UnPacker* object
    void (*on_raw_bizmsg)(ufxtrade_t*, int ref_id, void* pmsg, void* punpacker);
}ufxtrade_spi_t;



/* exported apis */
/// ��ȡ�汾��
UFX_TRADE_API const char* UFX_TRADE_STDCALL ufxtrade_version(int* pver);

/// ��������
UFX_TRADE_API ufxtrade_t* UFX_TRADE_STDCALL ufxtrade_create(int is_async);

/// ���ٶ���
UFX_TRADE_API void UFX_TRADE_STDCALL ufxtrade_realese(ufxtrade_t* hstd);

/// �û�����������
UFX_TRADE_API void  UFX_TRADE_STDCALL ufxtrade_set_userdata(ufxtrade_t* hstd, void* userdata);
UFX_TRADE_API void* UFX_TRADE_STDCALL ufxtrade_get_userdata(ufxtrade_t* hstd);

/// ����ģʽ
UFX_TRADE_API void UFX_TRADE_STDCALL ufxtrade_debug_mode(ufxtrade_t* hstd, int level);

/// ͬ��ģʽ��ʱʱ��
UFX_TRADE_API void UFX_TRADE_STDCALL ufxtrade_set_timeout(ufxtrade_t* hstd, int timeoutms);

/// ע��ص�
UFX_TRADE_API void UFX_TRADE_STDCALL ufxtrade_register_spi(ufxtrade_t* hstd, ufxtrade_spi_t* spi);

/// ���������ļ� t2sdk.ini
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_config_load(ufxtrade_t* hstd, const char* config_file);

/// ����API����ѡ��: [t2sdk]servers, [t2sdk]license_file, [ufx]fund_account etc.
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_config_set_string(ufxtrade_t* hstd, const char* section, const char* entry_name, const char* value);
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_config_set_int(ufxtrade_t* hstd, const char* section, const char* entry_name, const int value);

/// ��ȡAPI����ѡ��
UFX_TRADE_API const char* UFX_TRADE_STDCALL ufxtrade_config_get_string(ufxtrade_t* hstd, const char* section, const char* entry_name, const char* default_value);
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_config_get_int(ufxtrade_t* hstd, const char* section, const char* entry_name, const int default_value);

/// ��������
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_init(ufxtrade_t* hstd, int timeoutms);

/// �������ƻر�
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_subscribe(ufxtrade_t* hstd, int issue_type);

/// ��ȡ������ yyyymmdd
UFX_TRADE_API const char* UFX_TRADE_STDCALL ufxtrade_get_trading_day(ufxtrade_t* hstd);

/// �����¼
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_user_login(ufxtrade_t* hstd, UFXReqUserLoginField* login_req);

/// ���󱨵�
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_order_insert(ufxtrade_t* hstd, UFXReqOrderInsertField* order_req);

/// ���󳷵�
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_order_action(ufxtrade_t* hstd, UFXReqOrderActionField* order_action);

/// �����ѯ�ʽ��˻�
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_qry_trading_account(ufxtrade_t* hstd, UFXReqQueryField* qry_field);

/// �����ѯͶ�����˻�
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_qry_investor(ufxtrade_t* hstd, UFXReqQueryField* qry_field);

/// �����ѯ�ֲ�
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_qry_position(ufxtrade_t* hstd, UFXReqQueryField* qry_field);

/// �����ѯ�ɽ�
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_qry_trade(ufxtrade_t* hstd, UFXReqQueryField* qry_field);

/// �����ѯί��
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_qry_order(ufxtrade_t* hstd, UFXReqQueryField* qry_field);

/// �����ѯ֤ȯ��Ϣ
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_qry_security_info(ufxtrade_t* hstd, UFXReqQueryField* qry_field);

/// �����ѯ����
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_qry_md(ufxtrade_t* hstd, UFXReqQueryField* qry_field);


/// �����ѯ��ʷ�ʽ���ˮ
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_qry_cash_hist(ufxtrade_t* hstd, UFXReqQueryField* qry_field);

/// �����ѯ��ʷ�ɽ�
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_qry_trade_hist(ufxtrade_t* hstd, UFXReqQueryField* qry_field);

/// �����ѯ��ʷί��
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_qry_order_hist(ufxtrade_t* hstd, UFXReqQueryField* qry_field);


//////////////////////////////////////////////////////////////////////////
/// some raw apis for easily use
UFX_TRADE_API void* UFX_TRADE_STDCALL ufxtrade_begin_pack(ufxtrade_t* hstd);
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_end_pack(ufxtrade_t* hstd, void* packer);
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_release_pack(ufxtrade_t* hstd, void* packer);
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_send_bizmsg(ufxtrade_t* hstd, void* packer, int func_id);

/// add pack field��field_type(UFX_FT_Xxx): I(Integer), S(String), C(Char), D(Double), R(Raw)
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_add_field(ufxtrade_t* hstd, void* packer, const char* key, char field_type, int field_width, int field_scale);

/// add pack values
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_add_char(ufxtrade_t* hstd, void* packer, const char value);
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_add_str(ufxtrade_t* hstd, void* packer, const char* value);
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_add_int(ufxtrade_t* hstd, void* packer, const int value);
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_add_double(ufxtrade_t* hstd, void* packer, const double value);
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_add_raw(ufxtrade_t* hstd, void* packer, void* buf, const int len);

/// sync recv msg, got IBizMessage* object
UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_recv_msg(ufxtrade_t* hstd, int hsend, int timeoutms, void** ppmsg);

UFX_TRADE_API int UFX_TRADE_STDCALL ufxtrade_show_msg(void* pmsg);


#ifdef __cplusplus
}


#endif//__cplusplus


#endif//_UFX_TRADE_INCLUDED_H_
