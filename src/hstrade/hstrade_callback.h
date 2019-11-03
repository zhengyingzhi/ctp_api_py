#ifndef _HS_TRADE_CALLBACK_H_
#define _HS_TRADE_CALLBACK_H_

#include <string>

#include <t2sdk_interface.h>

#include "cJSON.h"

#include "hstrade_private.h"


typedef void(*on_recv_msg_pt)(void* clientd, int func_id, int issue_type, const std::string& msg);

class TradeCallback : public CCallbackInterface
{
public:
    TradeCallback();
    ~TradeCallback();

    // 因为CCallbackInterface的最终纯虚基类是IKnown，所以需要实现一下这3个方法
    unsigned long  FUNCTION_CALL_MODE QueryInterface(const char *iid, IKnown **ppv);
    unsigned long  FUNCTION_CALL_MODE AddRef();
    unsigned long  FUNCTION_CALL_MODE Release();

    // 各种事件发生时的回调方法，实际使用时可以根据需要来选择实现，对于不需要的事件回调方法，可直接return
    // Reserved?为保留方法，为以后扩展做准备，实现时可直接return或return 0。
    void FUNCTION_CALL_MODE OnConnect(CConnectionInterface *lpConnection);
    void FUNCTION_CALL_MODE OnSafeConnect(CConnectionInterface *lpConnection);
    void FUNCTION_CALL_MODE OnRegister(CConnectionInterface *lpConnection);
    void FUNCTION_CALL_MODE OnClose(CConnectionInterface *lpConnection);
    void FUNCTION_CALL_MODE OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData);
    void FUNCTION_CALL_MODE Reserved1(void *a, void *b, void *c, void *d);
    void FUNCTION_CALL_MODE Reserved2(void *a, void *b, void *c, void *d);
    int  FUNCTION_CALL_MODE Reserved3();
    void FUNCTION_CALL_MODE Reserved4();
    void FUNCTION_CALL_MODE Reserved5();
    void FUNCTION_CALL_MODE Reserved6();
    void FUNCTION_CALL_MODE Reserved7();
    void FUNCTION_CALL_MODE OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult);
    void FUNCTION_CALL_MODE OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult);
    void FUNCTION_CALL_MODE OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg);
    //int FUNCTION_CALL_MODE EncodeEx(const char *pIn, char *pOut);

public:
    static int UnpackBizMessage(IF2UnPacker* lpUnPacker, void* ctxdata, int(*data_proc_pt)(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata));

    static int UnpackLoginData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata);
    static int UnpackRspOrderData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata);
    static int UnpackRspOrderActionData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata);
    static int UnpackTradingAccountData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata);
    static int UnpackPositionData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata);
    static int UnpackQryOrderData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata);
    static int UnpackQryTradeData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata);
    static int UnpackRtnOrderData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata);
    static int UnpackRtnTradeData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata);

    static void GetErrorField(HSRspInfoField* rsp_info, IF2UnPacker* lpUnPacker);

    static int GenJsonData(cJSON* json_data, IF2UnPacker* lpUnPacker);
    static cJSON* GenJsonDatas(IF2UnPacker* lpUnPacker, int func_id, int issue_type);

public:
    void SetContextData(hstrade_t* hstd);

    void SetJsonMode(int data_proto);

    int  IsJsonMode();

    int NotifyJsonData(cJSON* json, int func_id, int issue_type);

    // 331100 登入
    void OnResponseUserLogin(IF2UnPacker* lpUnPacker);
    // 332255 资金账户查询
    void OnResponseQryTradingAccount(IF2UnPacker* lpUnPacker);
    // 333104 持仓查询
    void OnResponseQryPosition(IF2UnPacker* lpUnPacker);
    // 333101 委托查询
    void OnResponseQryOrder(IF2UnPacker* lpUnPacker);
    // 333102 成交查询
    void OnResponseQryTrade(IF2UnPacker* lpUnPacker);
    // 330300 证券代码信息查询
    void OnResponseQrySecurityInfo(IF2UnPacker* lpUnPacker);
    // 400 行情查询
    void OnResponseQryMD(IF2UnPacker* lpUnPacker);
    // 620001 订阅回报应答
    void OnResponseSubscribe(IF2UnPacker* lpUnPacker);

    // 委托应答
    void OnResponseOrderInsert(IF2UnPacker* lpUnPacker);
    void OnResponseOrderCancel(IF2UnPacker* lpUnPacker);

    // 620003 回报主推
    void OnRtnOrder(IF2UnPacker* lpUnPacker);
    void OnRtnTrade(IF2UnPacker* lpUnPacker);

private:
    hstrade_t*  m_hstd;
    int         m_data_proto;

    int         m_return_code;
    const char* m_return_msg;
};



#endif//_HS_TRADE_CALLBACK_H_
