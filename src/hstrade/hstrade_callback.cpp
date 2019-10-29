#include <string>

#include "hstrade_struct.h"
#include "hstrade_callback.h"

using namespace std;


// 从 unpacker 中获取字符串字段
#define extract_str_field(d, k)     \
    do { \
        const char* val = lpUnPacker->GetStr((k)); \
        if (val) \
            strncpy((d), val, sizeof((d)) - 1); \
    } while (0);


int convert_int_time(char buf[], int int_time)
{
    // 112456
    int hour = int_time / 100 % 100;
    return sprintf(buf, "%02d:%02d:%02d", int_time / 10000, hour, int_time % 100);
}


void ShowPacket(IF2UnPacker *lpUnPacker);


TradeCallback::TradeCallback()
{
    m_client_data = NULL;
    m_on_recv_msg = NULL;
}

TradeCallback::~TradeCallback()
{}

unsigned long TradeCallback::QueryInterface(const char *iid, IKnown **ppv)
{
    return 0;
}

unsigned long TradeCallback::AddRef()
{
    return 0;
}

unsigned long TradeCallback::Release()
{
    return 0;
}



void TradeCallback::OnConnect(CConnectionInterface *lpConnection)
{
    puts("TradeCallback::OnConnect");
}

void TradeCallback::OnSafeConnect(CConnectionInterface *lpConnection)
{
    puts("TradeCallback::OnSafeConnect");
}

void TradeCallback::OnRegister(CConnectionInterface *lpConnection)
{
    puts("TradeCallback::OnRegister");
}

//断开连接后会调用Onclose函数，提示连接断开
void TradeCallback::OnClose(CConnectionInterface *lpConnection)
{
    puts("TradeCallback::OnClose");
}

void TradeCallback::OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData)
{
    puts("TradeCallback::Onsent");
}

void TradeCallback::Reserved1(void *a, void *b, void *c, void *d)
{
    puts("TradeCallback::Reserved1");
}


void TradeCallback::Reserved2(void *a, void *b, void *c, void *d)
{
    puts("TradeCallback::Reserved2");
}

void TradeCallback::OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult)
{
    puts("TradeCallback::OnReceivedBizEx");
}

void TradeCallback::OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg)
{
    printf("TradeCallback::OnReceivedBizMsg hSend:%d", hSend);
    if (lpMsg == NULL)
    {
        return;
    }

    int iLen = 0;
    const void * lpBuffer = lpMsg->GetContent(iLen);
    IF2UnPacker * lpUnPacker = NewUnPacker((void *)lpBuffer, iLen);
    if (!lpUnPacker)
    {
        int error_no = lpMsg->GetErrorNo();
        const char* error_info = lpMsg->GetErrorInfo();
        printf("业务包是空包，功能号：%d，错误代码：%d，错误信息:%s\n", lpMsg->GetFunction(), error_no, error_info);
        return;
    }

    lpUnPacker->AddRef();
    ShowPacket(lpUnPacker);

    //成功,应用程序不能释放lpBizMessageRecv消息
    if (lpMsg->GetReturnCode() == 0)
    {
        //如果要把消息放到其他线程处理，必须自行拷贝，操作如下：
        //int iMsgLen = 0;
        //void * lpMsgBuffer = lpBizMessageRecv->GetBuff(iMsgLen);
        //将lpMsgBuffer拷贝走，然后在其他线程中恢复成消息可进行如下操作：
        //lpBizMessageRecv->SetBuff(lpMsgBuffer,iMsgLen);
        //没有错误信息

        switch (lpMsg->GetFunction())
        {
        case UFX_FUNC_LOGIN:
            OnResponseUserLogin(lpUnPacker);
            break;
        case UFX_FUNC_QRY_CASH:
            OnResponseQryTradingAccount(lpUnPacker);
            break;
        case UFX_FUNC_QRY_POSITION:
            OnResponseQryPosition(lpUnPacker);
            break;
        case UFX_FUNC_QRY_ORDER:
            TradeCallback::UnpackBizMessage(lpUnPacker, m_hstd, TradeCallback::UnpackQryOrderData);
            // OnResponseQryOrder(lpUnPacker);
            break;
        case UFX_FUNC_QRY_TRADE:
            TradeCallback::UnpackBizMessage(lpUnPacker, m_hstd, TradeCallback::UnpackQryTradeData);
            // OnResponseQryTrade(lpUnPacker);
            break;
        case UFX_FUNC_RTN_DATA:
        {
            if (lpMsg->GetIssueType() == UFX_ISSUE_TYPE_TRADE)
            {
                OnRtnTrade(lpUnPacker);
            }
            else if (lpMsg->GetIssueType() == UFX_ISSUE_TYPE_ORDER)
            {
                OnRtnOrder(lpUnPacker);
            }
        }
        default:
            break;
        }

    }
    else
    {
    }

    lpUnPacker->Release();
}

void TradeCallback::OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult)
{

}

int  TradeCallback::Reserved3()
{
    return 0;
}

void TradeCallback::Reserved4()
{
}

void TradeCallback::Reserved5()
{
}

void TradeCallback::Reserved6()
{
}

void TradeCallback::Reserved7()
{
}

void TradeCallback::SetContextData(hstrade_t* hstd)
{
    m_hstd = hstd;
}

void TradeCallback::SetOnRecvMsg(void* clientd, on_recv_msg_pt func)
{
    m_client_data = clientd;
    m_on_recv_msg = func;
}

int TradeCallback::IsJsonMode()
{
    return m_on_recv_msg != NULL;
}

cJSON* TradeCallback::GenJsonData(IF2UnPacker* lpUnPacker)
{
    cJSON* json_data;
    json_data = cJSON_CreateObject();

    int cols = lpUnPacker->GetColCount();

    for (int k = 0; k < cols; ++k)
    {
        char col_type = lpUnPacker->GetColType(k);
        const char* col_name = lpUnPacker->GetColName(k);
        // val = lpUnPacker->GetIntByIndex(k)

        // also, we could add all data as str object

        switch (col_type)
        {
        case 'I':
        {
            int val = lpUnPacker->GetIntByIndex(k);
            cJSON_AddNumberToObject(json_data, col_name, val);
        }
            break;
        case 'C':
        {
            char val = lpUnPacker->GetCharByIndex(k);
            char buf[8] = "";
            sprintf(buf, "%d", val);
            cJSON_AddStringToObject(json_data, col_name, buf);
        }
            break;
        case 'S':
        {
            const char* val = lpUnPacker->GetStrByIndex(k);
            if (val)
                cJSON_AddStringToObject(json_data, col_name, val);
            else
                cJSON_AddNullToObject(json_data, col_name);
        }
            break;
        case 'F':
        {
            double val = lpUnPacker->GetDoubleByIndex(k);
            cJSON_AddNumberToObject(json_data, col_name, val);
        }
            break;
        default:
            break;
        }
    }
    return json_data;
}

cJSON* TradeCallback::GenJsonDatas(IF2UnPacker* lpUnPacker)
{
    int i = 0, j = 0;

    cJSON* root;
    root = cJSON_CreateArray();

    for (i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
    {
        // 设置当前结果集
        lpUnPacker->SetCurrentDatasetByIndex(i);

        // 多条记录 list
        cJSON* json_data;
        for (j = 0; j < (int)lpUnPacker->GetRowCount(); ++j)
        {
            // 每条记录
            json_data = GenJsonData(lpUnPacker);
            cJSON_AddItemToArray(root, json_data);

            lpUnPacker->Next();
        }
    }

#if 1
    char* json_str;
    json_str = cJSON_Print(root);
    if (m_on_recv_msg)
    {
        m_on_recv_msg(m_client_data, 0, 0, std::string(json_str));
    }

    cJSON_free(json_str);
    cJSON_Delete(root);
    // FIXME: whether need delete each json_data of root
#endif

    return 0;
}

void TradeCallback::GetErrorField(HSRspInfoField* rsp_info, IF2UnPacker* lpUnPacker)
{
    rsp_info->error_no = lpUnPacker->GetInt("error_no");

    const char* error_info = lpUnPacker->GetStr("error_info");
    if (error_info)
        strncpy(rsp_info->error_info, error_info, sizeof(rsp_info->error_info) - 1);
}

void TradeCallback::OnResponseUserLogin(IF2UnPacker *lpUnPacker)
{
    if (IsJsonMode())
    {
        GenJsonDatas(lpUnPacker);
        return;
    }

    int iSystemNo = -1;
    puts("TradeCallBack::331100");

    HSRspUserLoginField rsp_login = { 0 };
    HSRspInfoField rsp_info = { 0 };
    TradeCallback::GetErrorField(&rsp_info, lpUnPacker);

    int branch_no = lpUnPacker->GetInt("branch_no");
    int sysnode_id = lpUnPacker->GetInt("sysnode_id");
    int init_date = lpUnPacker->GetInt("init_date");

    m_hstd->apidata.branch_no = branch_no;
    rsp_login.branch_no = branch_no;
    rsp_login.system_no = sysnode_id;
    m_hstd->apidata.system_no = sysnode_id;

    extract_str_field(rsp_login.company_name, "company_name");
    extract_str_field(rsp_login.client_id, "client_id");
    extract_str_field(rsp_login.user_token, "user_token");

    rsp_login.trading_date = init_date;

    // int op_branch_no = lpUnPacker->GetInt("op_branch_no");

    hstrade_spi_t* spi = m_hstd->spi;
    if (spi && spi->on_user_login)
    {
        spi->on_user_login(m_hstd, &rsp_login, &rsp_info);
    }
}

void TradeCallback::OnResponseQryTradingAccount(IF2UnPacker* lpUnPacker)
{
    if (IsJsonMode())
    {
        GenJsonDatas(lpUnPacker);
        return;
    }

    TradeCallback::UnpackBizMessage(lpUnPacker, m_hstd, TradeCallback::UnpackTradingAccountData);
}

void TradeCallback::OnResponseQryPosition(IF2UnPacker *lpUnPacker)
{
    if (IsJsonMode())
    {
        GenJsonDatas(lpUnPacker);
        return;
    }

    TradeCallback::UnpackBizMessage(lpUnPacker, m_hstd, TradeCallback::UnpackPositionData);
    return;

    if (lpUnPacker->GetInt("error_no") != 0)
    {
        fprintf(stderr, lpUnPacker->GetStr("error_info"));
        return;
    }
    else
    {
        int ct = lpUnPacker->GetRowCount();
        string pos = "";
        while (!lpUnPacker->IsEOF())
        {
            const char* lpStrPos = lpUnPacker->GetStr("position_str");
            if (lpStrPos == 0)
                pos = "";
            else
                pos = lpStrPos;

            lpUnPacker->Next();
        }

        if (pos.length() != 0)
        {
            ShowPacket(lpUnPacker);
            // lpReqMode->ReqFunction333104(pos.c_str());
        }
    }

    hstrade_spi_t* spi = m_hstd->spi;
    HSPositionField rsp_pos = { 0 };
    HSRspInfoField rsp_info = { 0 };
    if (spi && spi->on_qry_position)
    {
        spi->on_qry_position(m_hstd, &rsp_pos, &rsp_info, 1);
    }
}

void TradeCallback::OnResponseQryOrder(IF2UnPacker* lpUnPacker)
{
    if (IsJsonMode())
    {
        GenJsonDatas(lpUnPacker);
        return;
    }

    TradeCallback::UnpackBizMessage(lpUnPacker, m_hstd, TradeCallback::UnpackQryOrderData);
}

void TradeCallback::OnResponseQryTrade(IF2UnPacker* lpUnPacker)
{
    if (IsJsonMode())
    {
        GenJsonDatas(lpUnPacker);
        return;
    }

    TradeCallback::UnpackBizMessage(lpUnPacker, m_hstd, TradeCallback::UnpackQryTradeData);
}

void TradeCallback::OnResponseQrySecurityInfo(IF2UnPacker* lpUnPacker)
{
    if (IsJsonMode())
    {
        GenJsonDatas(lpUnPacker);
        return;
    }
}

void TradeCallback::OnResponseQryMD(IF2UnPacker* lpUnPacker)
{}

void TradeCallback::OnResponseSubscribe(IF2UnPacker* lpUnPacker)
{}


void TradeCallback::OnResponseOrderInsert(IF2UnPacker* lpUnPacker)
{}

void TradeCallback::OnResponseOrderCancel(IF2UnPacker* lpUnPacker)
{}

void TradeCallback::OnRtnOrder(IF2UnPacker* lpUnPacker)
{
    if (IsJsonMode())
    {
        GenJsonDatas(lpUnPacker);
        return;
    }
}

void TradeCallback::OnRtnTrade(IF2UnPacker* lpUnPacker)
{
    if (IsJsonMode())
    {
        GenJsonDatas(lpUnPacker);
        return;
    }
}


int TradeCallback::UnpackRspOrderData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata)
{
    return 0;
}

int TradeCallback::UnpackRspOrderActionData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata)
{
    return 0;
}

int TradeCallback::UnpackTradingAccountData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata)
{
    hstrade_t* hstd;
    hstrade_spi_t* spi;
    hstd = (hstrade_t*)ctxdata;
    spi = hstd->spi;

    HSRspInfoField rsp_info = { 0 };
    TradeCallback::GetErrorField(&rsp_info, lpUnPacker);

    int islast = 0;
    int rows = lpUnPacker->GetRowCount();
    for (int row = 0; row < rows; ++row)
    {
        HSTradingAccountField ta = { 0 };
        extract_str_field(ta.client_id, "fund_account");
        if (!ta.client_id[0])
            extract_str_field(ta.client_id, "client_id");
        ta.asset_balance = lpUnPacker->GetDouble("asset_balance");
        ta.begin_balance = lpUnPacker->GetDouble("begin_balance");
        ta.current_balance = lpUnPacker->GetDouble("current_balance");
        ta.enable_balance = lpUnPacker->GetDouble("enable_balance");
        ta.frozen_balance = lpUnPacker->GetDouble("frozen_balance");
        ta.fetch_balance = lpUnPacker->GetDouble("fetch_balance");
        ta.real_buy_balance = lpUnPacker->GetDouble("real_buy_balance");
        ta.real_sell_balance = lpUnPacker->GetDouble("real_sell_balance");
        ta.net_asset = lpUnPacker->GetDouble("net_asset");
        ta.market_value = lpUnPacker->GetDouble("market_value");

        if (row == rows - 1)
            islast = 1;

        if (spi && spi->on_qry_trading_account)
            spi->on_qry_trading_account(hstd, &ta, &rsp_info, islast);

        // 下一行记录
        lpUnPacker->Next();
    }

    return 0;
}

int TradeCallback::UnpackPositionData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata)
{
    hstrade_t* hstd;
    hstrade_spi_t* spi;
    hstd = (hstrade_t*)ctxdata;
    spi = hstd->spi;

    HSRspInfoField rsp_info = { 0 };
    TradeCallback::GetErrorField(&rsp_info, lpUnPacker);

    int islast = 0;
    int rows = lpUnPacker->GetRowCount();
    for (int row = 0; row < rows; ++row)
    {
        HSPositionField pos = { 0 };
        strncpy(pos.client_id, lpUnPacker->GetStr("fund_account"), sizeof(pos.client_id) - 1);
        strncpy(pos.stock_code, lpUnPacker->GetStr("stock_code"), sizeof(pos.client_id) - 1);
        extract_str_field(pos.client_id, "func_account");
        extract_str_field(pos.stock_code, "stock_code");
        extract_str_field(pos.stock_name, "stock_name");
        extract_str_field(pos.stock_type, "stock_type");

        pos.current_amount = lpUnPacker->GetInt("current_amount");
        pos.enable_amount = lpUnPacker->GetInt("enabled_amount");
        pos.frozen_amount = lpUnPacker->GetInt("frozen_amount");
        pos.real_buy_amount = lpUnPacker->GetInt("real_buy_amount");
        pos.real_sell_amount = lpUnPacker->GetInt("real_sell_amount");
        pos.cost_price = lpUnPacker->GetDouble("cost_price");
        pos.last_price = lpUnPacker->GetDouble("last_price");
        pos.market_value = lpUnPacker->GetDouble("market_value");
        pos.income_balance = lpUnPacker->GetDouble("income_balance");
        pos.cost_balance = lpUnPacker->GetDouble("cost_balance");
        pos.profit_ratio = lpUnPacker->GetDouble("profit_ratio");
        pos.delist_flag = lpUnPacker->GetChar("delist_flag");

        int delist_date = lpUnPacker->GetInt("delist_date");
        pos.delist_date = delist_date;

        if (row == rows - 1)
            islast = 1;

        if (spi && spi->on_qry_position)
            spi->on_qry_position(hstd, &pos, &rsp_info, islast);

        // 下一行记录
        lpUnPacker->Next();
    }

    return 0;
}


int TradeCallback::UnpackQryOrderData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata)
{
    hstrade_t* hstd;
    hstrade_spi_t* spi;
    hstd = (hstrade_t*)ctxdata;
    spi = hstd->spi;

    HSRspInfoField rsp_info = { 0 };
    TradeCallback::GetErrorField(&rsp_info, lpUnPacker);

    int islast = 0;
    int rows = lpUnPacker->GetRowCount();
    for (int row = 0; row < rows; ++row)
    {
        HSOrderField order = { 0 };
        // strncpy(order.client_id, lpUnPacker->GetStr("fund_account"), sizeof(order.client_id) - 1);
        extract_str_field(order.client_id, "fund_account");
        extract_str_field(order.stock_code, "stock_code");
        extract_str_field(order.stock_name, "stock_name");
        extract_str_field(order.entrust_no, "entrust_no");
        extract_str_field(order.stock_type, "stock_type");
        extract_str_field(order.entrust_prop, "entrust_prop");

        order.entrust_amount = lpUnPacker->GetInt("entrust_amount");
        order.entrust_price = lpUnPacker->GetDouble("entrust_price");
        order.business_amount = lpUnPacker->GetInt("business_amount");
        order.business_price = lpUnPacker->GetDouble("business_price");
        order.batch_no = lpUnPacker->GetInt("batch_no");
        order.entrust_type = lpUnPacker->GetChar("entrust_type");
        order.entrust_bs = lpUnPacker->GetChar("entrust_bs");
        order.entrust_status = lpUnPacker->GetChar("entrust_status");

        extract_str_field(order.exchange_type, "exchange_type");
        int order_date = lpUnPacker->GetInt("entrust_date");
        int order_time = lpUnPacker->GetInt("entrust_time");

        order.init_date = lpUnPacker->GetInt("init_date");
        order.entrust_date = order_date;
        order.entrust_time = order_time;

        if (row == rows - 1)
            islast = 1;

        if (spi && spi->on_qry_order)
            spi->on_qry_order(hstd, &order, &rsp_info, islast);

        // 下一行记录
        lpUnPacker->Next();
    }

    return 0;
}

int TradeCallback::UnpackQryTradeData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata)
{
    hstrade_t* hstd;
    hstrade_spi_t* spi;
    hstd = (hstrade_t*)ctxdata;
    spi = hstd->spi;

    HSRspInfoField rsp_info = { 0 };
    TradeCallback::GetErrorField(&rsp_info, lpUnPacker);

    int islast = 0;
    int rows = lpUnPacker->GetRowCount();
    for (int row = 0; row < rows; ++row)
    {
        HSTradeField trade = { 0 };
        strncpy(trade.client_id, lpUnPacker->GetStr("fund_account"), sizeof(trade.client_id) - 1);
        extract_str_field(trade.client_id, "fund_account");
        extract_str_field(trade.stock_code, "stock_code");
        extract_str_field(trade.stock_name, "stock_name");
        extract_str_field(trade.entrust_no, "entrust_no");
        extract_str_field(trade.business_no, "business_no");    // business_id ?
        extract_str_field(trade.exchange_type, "exchange_type");
        extract_str_field(trade.stock_type, "stock_type");
        extract_str_field(trade.entrust_prop, "entrust_prop");

        trade.business_amount = lpUnPacker->GetInt("business_amount");
        trade.business_price = lpUnPacker->GetDouble("business_price");
        trade.business_balance = lpUnPacker->GetDouble("business_balance");
        trade.entrust_amount = lpUnPacker->GetInt("entrust_amount");
        trade.entrust_price = lpUnPacker->GetDouble("entrust_price");

        trade.entrust_bs = lpUnPacker->GetChar("entrust_bs");
        trade.real_type = lpUnPacker->GetChar("real_type");
        trade.real_status = lpUnPacker->GetChar("real_status");
        trade.batch_no = lpUnPacker->GetInt("batch_no");

        int trade_date = lpUnPacker->GetInt("date");
        int trade_time = lpUnPacker->GetInt("business_time");

        trade.date = trade_date;
        trade.business_time = trade_time;

        if (row == rows - 1)
            islast = 1;

        if (spi && spi->on_qry_trade)
            spi->on_qry_trade(hstd, &trade, &rsp_info, islast);

        // 下一行记录
        lpUnPacker->Next();
    }

    return 0;
}

int TradeCallback::UnpackRtnOrderData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata)
{
    hstrade_t* hstd;
    hstrade_spi_t* spi;
    hstd = (hstrade_t*)ctxdata;
    spi = hstd->spi;

    HSRspInfoField rsp_info = { 0 };
    TradeCallback::GetErrorField(&rsp_info, lpUnPacker);

    int islast = 0;
    int rows = lpUnPacker->GetRowCount();
    for (int row = 0; row < rows; ++row)
    {
        HSOrderField order = { 0 };
        // strncpy(order.client_id, lpUnPacker->GetStr("fund_account"), sizeof(order.client_id) - 1);
        extract_str_field(order.client_id, "fund_account");
        extract_str_field(order.stock_code, "stock_code");
        extract_str_field(order.stock_name, "stock_name");
        extract_str_field(order.entrust_no, "entrust_no");
        extract_str_field(order.stock_type, "stock_type");
        extract_str_field(order.entrust_prop, "entrust_prop");
        extract_str_field(order.exchange_type, "exchange_type");

        order.entrust_amount = lpUnPacker->GetInt("entrust_amount");
        order.entrust_price = lpUnPacker->GetDouble("entrust_price");
        order.business_amount = lpUnPacker->GetInt("business_amount");
        order.business_price = lpUnPacker->GetDouble("business_price");
        order.batch_no = lpUnPacker->GetInt("batch_no");
        order.entrust_type = lpUnPacker->GetChar("entrust_type");
        order.entrust_bs = lpUnPacker->GetChar("entrust_bs");
        order.entrust_status = lpUnPacker->GetChar("entrust_status");

        int order_date = lpUnPacker->GetInt("entrust_date");
        int order_time = lpUnPacker->GetInt("entrust_time");

        // order.entrust_date = order_date;
        order.entrust_time = order_time;

        if (row == rows - 1)
            islast = 1;

        if (spi && spi->on_qry_order)
            spi->on_qry_order(hstd, &order, &rsp_info, islast);

        // 下一行记录
        lpUnPacker->Next();
    }

    return 0;
}

int TradeCallback::UnpackRtnTradeData(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata)
{
    hstrade_t* hstd;
    hstrade_spi_t* spi;
    hstd = (hstrade_t*)ctxdata;
    spi = hstd->spi;

    int islast = 0;
    int rows = lpUnPacker->GetRowCount();
    for (int row = 0; row < rows; ++row)
    {
        HSTradeField trade = { 0 };
        strncpy(trade.client_id, lpUnPacker->GetStr("fund_account"), sizeof(trade.client_id) - 1);
        extract_str_field(trade.client_id, "fund_account");
        extract_str_field(trade.stock_code, "stock_code");
        extract_str_field(trade.stock_name, "stock_name");
        extract_str_field(trade.entrust_no, "entrust_no");
        extract_str_field(trade.business_no, "business_no");    // business_id ?
        extract_str_field(trade.exchange_type, "exchange_type");
        extract_str_field(trade.stock_type, "stock_type");
        extract_str_field(trade.entrust_prop, "entrust_prop");

        // TODO: 处理撤单回报

        trade.business_amount = lpUnPacker->GetInt("business_amount");
        trade.business_price = lpUnPacker->GetDouble("business_price");
        trade.business_balance = lpUnPacker->GetDouble("business_balance");
        trade.entrust_amount = lpUnPacker->GetInt("entrust_amount");
        trade.entrust_price = lpUnPacker->GetDouble("entrust_price");

        trade.entrust_bs = lpUnPacker->GetChar("entrust_bs");
        trade.real_type = lpUnPacker->GetChar("real_type");
        trade.real_status = lpUnPacker->GetChar("real_status");
        trade.batch_no = lpUnPacker->GetInt("batch_no");

        int trade_date = lpUnPacker->GetInt("date");
        int trade_time = lpUnPacker->GetInt("business_time");

        // sprintf(trade.TradeDate, "%d", trade_date);
        trade.date = trade_date;
        trade.business_time = trade_time;

        if (row == rows - 1)
            islast = 1;

        if (spi && spi->on_qry_trade)
            spi->on_qry_trade(hstd, &trade, NULL, islast);

        // 下一行记录
        lpUnPacker->Next();
    }

    return 0;
}

int TradeCallback::UnpackBizMessage(IF2UnPacker* lpUnPacker, void* ctxdata,
    int(*data_proc_pt)(IF2UnPacker* lpUnPacker, int ds_index, void* ctxdata))
{
    if (!data_proc_pt) {
        return -1;
    }

    for (int i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
    {
        lpUnPacker->SetCurrentDatasetByIndex(i);
        data_proc_pt(lpUnPacker, i, ctxdata);
    }
    return 0;
}

void ShowPacket(IF2UnPacker *lpUnPacker)
{
    int i = 0, t = 0, j = 0, k = 0;

    for (i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
    {
        // 设置当前结果集

        printf("记录集：%d/%d\r\n", i + 1, lpUnPacker->GetDatasetCount());
        lpUnPacker->SetCurrentDatasetByIndex(i);

        // 打印所有记录
        for (j = 0; j < (int)lpUnPacker->GetRowCount(); ++j)
        {
            printf("\t第%d/%d条记录：\r\n", j + 1, lpUnPacker->GetRowCount());
            // 打印每条记录
            for (k = 0; k < lpUnPacker->GetColCount(); ++k)
            {
                switch (lpUnPacker->GetColType(k))
                {
                case 'I':
                    printf("\t【整数】%20s = %35d\r\n", lpUnPacker->GetColName(k), lpUnPacker->GetIntByIndex(k));
                    break;

                case 'C':
                    printf("\t【字符】%20s = %35c\r\n", lpUnPacker->GetColName(k), lpUnPacker->GetCharByIndex(k));
                    break;

                case 'S':

                    if (NULL != strstr((char *)lpUnPacker->GetColName(k), "password"))
                    {
                        printf("\t【字串】%20s = %35s\r\n", lpUnPacker->GetColName(k), "******");
                    }
                    else
                        printf("\t【字串】%20s = %35s\r\n", lpUnPacker->GetColName(k), lpUnPacker->GetStrByIndex(k));
                    break;

                case 'F':
                    printf("\t【数值】%20s = %35f\r\n", lpUnPacker->GetColName(k), lpUnPacker->GetDoubleByIndex(k));
                    break;

                case 'R':
                {
                    int nLength = 0;
                    void *lpData = lpUnPacker->GetRawByIndex(k, &nLength);
                    switch (nLength) {
                    case 0:
                        printf("\t【数据】%20s = %35s\r\n", lpUnPacker->GetColName(k), "(N/A)");
                        break;
                    default:
                        printf("\t【数据】%20s = 0x", lpUnPacker->GetColName(k));
                        for (t = nLength; t < 11; t++) {
                            printf("   ");
                        }
                        unsigned char *p = (unsigned char *)lpData;
                        for (t = 0; t < nLength; t++) {
                            printf("%3x", *p++);
                        }
                        printf("\r\n");
                        break;
                    }
                    // 对2进制数据进行处理
                    break;
                }

                default:
                    // 未知数据类型
                    printf("未知数据类型。\n");
                    break;
                }
            }

            putchar('\n');

            lpUnPacker->Next();
        }

        putchar('\n');
    }
}

