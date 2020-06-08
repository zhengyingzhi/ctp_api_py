#include <string>
#include <vector>
#include <codecvt>
#include <locale>

#include "ufxtrade_struct.h"
#include "ufxtrade_callback.h"

using namespace std;


// �� unpacker �л�ȡ�ַ����ֶ�
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


static string to_utf(const string &gb2312)
{
#ifdef _MSC_VER
    const static locale loc("zh-CN");
#else
    const static locale loc("zh_CN.GB18030");
#endif

    std::vector<wchar_t> wstr(gb2312.size());
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


void ShowPacket(IF2UnPacker *lpUnPacker);


TradeCallback::TradeCallback()
{
    m_hstd = NULL;
    m_data_proto = 0;
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
    if (m_hstd->debug_mode)
        fprintf(stderr, "TradeCallback::OnConnect\n");

    if (m_hstd->spi && m_hstd->spi->on_connected)
        m_hstd->spi->on_connected(m_hstd);
}

void TradeCallback::OnSafeConnect(CConnectionInterface *lpConnection)
{
    if (m_hstd->debug_mode)
        puts("TradeCallback::OnSafeConnect");
}

void TradeCallback::OnRegister(CConnectionInterface *lpConnection)
{
    if (m_hstd->debug_mode)
        puts("TradeCallback::OnRegister");
}

//�Ͽ����Ӻ�����Onclose��������ʾ���ӶϿ�
void TradeCallback::OnClose(CConnectionInterface *lpConnection)
{
    if (m_hstd->debug_mode)
        fprintf(stderr, "TradeCallback::OnClose\n");

    if (m_hstd->spi && m_hstd->spi->on_disconnected)
        m_hstd->spi->on_disconnected(m_hstd, -0x1001);
}

void TradeCallback::OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData)
{
    if (m_hstd->debug_mode)
        puts("TradeCallback::Onsent");
}

void TradeCallback::Reserved1(void *a, void *b, void *c, void *d)
{
    if (m_hstd->debug_mode)
        puts("TradeCallback::Reserved1");
}


void TradeCallback::Reserved2(void *a, void *b, void *c, void *d)
{
    if (m_hstd->debug_mode)
        puts("TradeCallback::Reserved2");
}

void TradeCallback::OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult)
{
    if (m_hstd->debug_mode)
        puts("TradeCallback::OnReceivedBizEx");
}

void TradeCallback::OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg)
{
    if (m_hstd->debug_mode) {
        fprintf(stderr, "TradeCallback::OnReceivedBizMsg hSend:%d, lpMsg:%p\n", hSend, lpMsg);
    }

    if (!lpMsg)
    {
        HSDebugInfo(HSDbgFd, "TradeCallback::OnReceivedBizMsg lpMsg is null!\n");
        m_return_code = 0;
        m_return_msg = NULL;
        return;
    }

    m_return_code = lpMsg->GetReturnCode();
    m_return_msg = lpMsg->GetErrorInfo();
    int func_id = lpMsg->GetFunction();
    int issue_type = lpMsg->GetIssueType();
    if (!m_return_msg)
        m_return_msg = "";

    if (m_hstd->debug_mode) {
        fprintf(stderr, "TradeCallback::OnReceivedBizMsg func_id:%d, issue_type:%d, code:%d,  msg:%s\n",
            func_id, issue_type, m_return_code, m_return_msg);
    }

    if (m_return_code != 0)
    {
        if (m_hstd->spi && m_hstd->spi->on_msg_error)
        {
            m_hstd->spi->on_msg_error(m_hstd, func_id, issue_type, hSend, m_return_code, m_return_msg);
        }
        return;
    }

    // �ɹ�,Ӧ�ó������ͷ�lpBizMessageRecv��Ϣ

    // ���Ҫ����Ϣ�ŵ������̴߳����������п������������£�
    // int iMsgLen = 0;
    // void * lpMsgBuffer = lpBizMessageRecv->GetBuff(iMsgLen);
    // ��lpMsgBuffer�����ߣ�Ȼ���������߳��лָ�����Ϣ�ɽ������²�����
    // lpBizMessageRecv->SetBuff(lpMsgBuffer,iMsgLen);

    int len = 0;
    const void* lpBuffer;
    IF2UnPacker* lpUnPacker;
    if (func_id >= UFX_FUNC_SUBSCRIBE && func_id <= UFX_FUNC_RTN_DATA)
    {
        lpBuffer = lpMsg->GetKeyInfo(len);
    }
    else
    {
        lpBuffer = lpMsg->GetContent(len);
    }

    lpUnPacker = NewUnPacker((void *)lpBuffer, len);
    if (!lpUnPacker)
    {
        int error_no = lpMsg->GetErrorNo();
        const char* error_info = lpMsg->GetErrorInfo();
        if (func_id != UFX_FUNC_HEART)
        {
            fprintf(stderr, "OnReceivedBizMsg empty packet�����ܺţ�%d��������룺%d��������Ϣ:%s\n",
                func_id, error_no, error_info);
        }
        return;
    }

    lpUnPacker->AddRef();

    if (m_hstd->debug_mode)
    {
        IF2UnPacker* lpUnPacker2 = NewUnPacker((void *)lpBuffer, len);
        if (lpUnPacker2)
        {
            lpUnPacker2->AddRef();
            ShowPacket(lpUnPacker2);
            lpUnPacker2->Release();
        }
    }

    switch (func_id)
    {
    case UFX_FUNC_LOGIN:
        OnResponseUserLogin(hSend, lpMsg, lpUnPacker);
        break;
    case UFX_FUNC_QRY_CASH:
        OnResponseQryTradingAccount(hSend, lpMsg, lpUnPacker);
        break;
    case UFX_FUNC_QRY_POSITION:
        OnResponseQryPosition(hSend, lpMsg, lpUnPacker);
        break;
    case UFX_FUNC_QRY_ORDER:
        OnResponseQryOrder(hSend, lpMsg, lpUnPacker);
        break;
    case UFX_FUNC_QRY_TRADE:
        OnResponseQryTrade(hSend, lpMsg, lpUnPacker);
        break;
    case UFX_FUNC_RTN_DATA:
    {
        if (issue_type == UFX_ISSUE_TYPE_TRADE)
        {
            OnRtnTrade(hSend, lpMsg, lpUnPacker);
        }
        else if (issue_type == UFX_ISSUE_TYPE_ORDER)
        {
            OnRtnOrder(hSend, lpMsg, lpUnPacker);
        }
    }
    case UFX_FUNC_SUBSCRIBE:
    {
        OnResponseSubscribe(hSend, lpMsg, lpUnPacker, issue_type);
        break;
    }
    case UFX_FUNC_SUB_CANCEL:
    {
        OnResponseUnSubscribe(hSend, lpMsg, lpUnPacker, issue_type);
        break;
    }
    case UFX_FUNC_HEART:
    {
        lpMsg->ChangeReq2AnsMessage();
        lpConnection->SendBizMsg(lpMsg, 1);
        break;
    }
    default:
    {
        if (m_hstd->spi && m_hstd->spi->on_raw_bizmsg)
            m_hstd->spi->on_raw_bizmsg(m_hstd, hSend, lpMsg, lpUnPacker);
    }
        break;
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

void TradeCallback::SetContextData(ufxtrade_t* hstd)
{
    m_hstd = hstd;
}

void TradeCallback::SetJsonMode(int data_proto)
{
    m_data_proto = data_proto;
}

int TradeCallback::IsJsonMode()
{
    return m_data_proto == 1;    // UFX_DATA_PROTO_JSON
}

int TradeCallback::GenJsonData(cJSON* json_data, IF2UnPacker* lpUnPacker)
{
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
            sprintf(buf, "%c", val);
            cJSON_AddStringToObject(json_data, col_name, buf);
        }
            break;
        case 'S':
        {
            const char* val = lpUnPacker->GetStrByIndex(k);
            if (val)
            {
                std::string _val = to_utf(val);
                cJSON_AddStringToObject(json_data, col_name, _val.c_str());
            }
            else
            {
                cJSON_AddNullToObject(json_data, col_name);
            }
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
    return 0;
}

cJSON* TradeCallback::GenJsonDatas(IF2UnPacker* lpUnPacker, int func_id, int issue_type, int ref_id)
{
    int i = 0, j = 0;

    cJSON* root;
    root = cJSON_CreateObject();

    cJSON_AddNumberToObject(root, "ref_id", ref_id);
    cJSON_AddNumberToObject(root, "function_id", func_id);
    cJSON_AddNumberToObject(root, "issue_type", issue_type);

    cJSON_AddNumberToObject(root, "error_no", lpUnPacker->GetInt("error_no"));
    const char* error_info = lpUnPacker->GetStr("error_info");
    if (error_info && error_info[0])
    {
        std::string _error_info = to_utf(error_info);
        cJSON_AddStringToObject(root, "error_info", _error_info.c_str());
    }
    else
    {
        cJSON_AddStringToObject(root, "error_info", "");
    }

    cJSON* array = cJSON_AddArrayToObject(root, "json");
    cJSON* json_data;

    for (i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
    {
        // ���õ�ǰ�����
        lpUnPacker->SetCurrentDatasetByIndex(i);

        // ������¼ list
        for (j = 0; j < (int)lpUnPacker->GetRowCount(); ++j)
        {
            // ÿ����¼
            json_data = cJSON_CreateObject();
            GenJsonData(json_data, lpUnPacker);
            cJSON_AddItemToArray(array, json_data);

            lpUnPacker->Next();
        }
    }

    return root;
}

int TradeCallback::NotifyJsonData(cJSON* json, int func_id, int issue_type, int ref_id)
{
    /* json data like:
    {
        "error_no": 0,
        "error_info": "",
        "function_id": 331100,
        "issue_type": 0,
        "ref_id": 0,
        "json": [
            { ... },
            { ... }
        ]
    }
     */

    char* json_str = "";
    if (json)
    {
        json_str = cJSON_Print(json);
    }

    if (m_hstd->spi && m_hstd->spi->on_json_msg)
    {
        m_hstd->spi->on_json_msg(m_hstd, func_id, issue_type, ref_id, json_str);
    }

    cJSON_free(json_str);
    cJSON_Delete(json);
    // FIXME: whether need delete each json_data of root

    return 0;
}

void TradeCallback::GetErrorField(UFXRspInfoField* rsp_info, IF2UnPacker* lpUnPacker)
{
    rsp_info->error_no = lpUnPacker->GetInt("error_no");

    const char* error_info = lpUnPacker->GetStr("error_info");
    if (error_info)
    {
        strncpy(rsp_info->error_info, error_info, sizeof(rsp_info->error_info) - 1);
    }
}

void TradeCallback::OnResponseUserLogin(int hSend, IBizMessage* lpMsg, IF2UnPacker *lpUnPacker)
{
    ufxtrade_apidata_t* apidata = &m_hstd->apidata;

    int branch_no = lpUnPacker->GetInt("branch_no");
    int sysnode_id = lpUnPacker->GetInt("sysnode_id");
    int init_date = lpUnPacker->GetInt("init_date");
    int op_branch_no = lpUnPacker->GetInt("op_branch_no");
    extract_str_field(apidata->user_token, "user_token");

    apidata->branch_no = branch_no;
    apidata->sysnode_id = sysnode_id;
    apidata->op_branch_no = op_branch_no;

    if (IsJsonMode())
    {
        cJSON* json = GenJsonDatas(lpUnPacker, UFX_FUNC_LOGIN, 0, hSend);
        NotifyJsonData(json, UFX_FUNC_LOGIN, 0, hSend);
        return;
    }

    UFXRspUserLoginField rsp_login = { 0 };
    UFXRspInfoField rsp_info = { 0 };
    TradeCallback::GetErrorField(&rsp_info, lpUnPacker);

    extract_str_field(rsp_login.company_name, "company_name");
    extract_str_field(rsp_login.client_id, "client_id");
    extract_str_field(rsp_login.user_token, "user_token");

    rsp_login.branch_no = branch_no;
    rsp_login.system_id = sysnode_id;
    rsp_login.trading_date = init_date;

    ufxtrade_spi_t* spi = m_hstd->spi;
    if (spi && spi->on_user_login)
    {
        spi->on_user_login(m_hstd, &rsp_login, &rsp_info, hSend);
    }
}

void TradeCallback::OnResponseQryTradingAccount(int hSend, IBizMessage* lpMsg, IF2UnPacker* lpUnPacker)
{
    if (IsJsonMode())
    {
        cJSON* json = GenJsonDatas(lpUnPacker, UFX_FUNC_QRY_CASH, UFX_ISSUE_TYPE_DEFAULT, hSend);
        NotifyJsonData(json, UFX_FUNC_QRY_CASH, UFX_ISSUE_TYPE_DEFAULT, hSend);
        return;
    }

    TradeCallback::UnpackBizMessage(lpUnPacker, hSend, m_hstd, TradeCallback::UnpackTradingAccountData);
}

void TradeCallback::OnResponseQryPosition(int hSend, IBizMessage* lpMsg, IF2UnPacker *lpUnPacker)
{
    if (IsJsonMode())
    {
        cJSON* json = GenJsonDatas(lpUnPacker, UFX_FUNC_QRY_POSITION, UFX_ISSUE_TYPE_DEFAULT, hSend);
        NotifyJsonData(json, UFX_FUNC_QRY_POSITION, UFX_ISSUE_TYPE_DEFAULT, hSend);
        return;
    }

    TradeCallback::UnpackBizMessage(lpUnPacker, hSend, m_hstd, TradeCallback::UnpackPositionData);
}

void TradeCallback::OnResponseQryOrder(int hSend, IBizMessage* lpMsg, IF2UnPacker* lpUnPacker)
{
    if (IsJsonMode())
    {
        cJSON* json = GenJsonDatas(lpUnPacker, UFX_FUNC_QRY_ORDER, UFX_ISSUE_TYPE_DEFAULT, hSend);
        NotifyJsonData(json, UFX_FUNC_QRY_ORDER, UFX_ISSUE_TYPE_DEFAULT, hSend);
        return;
    }

    TradeCallback::UnpackBizMessage(lpUnPacker, hSend, m_hstd, TradeCallback::UnpackQryOrderData);
}

void TradeCallback::OnResponseQryTrade(int hSend, IBizMessage* lpMsg, IF2UnPacker* lpUnPacker)
{
    if (IsJsonMode())
    {
        cJSON* json = GenJsonDatas(lpUnPacker, UFX_FUNC_QRY_TRADE, UFX_ISSUE_TYPE_DEFAULT, hSend);
        NotifyJsonData(json, UFX_FUNC_QRY_TRADE, UFX_ISSUE_TYPE_DEFAULT, hSend);
        return;
    }

    TradeCallback::UnpackBizMessage(lpUnPacker, hSend, m_hstd, TradeCallback::UnpackQryTradeData);
}

void TradeCallback::OnResponseQrySecurityInfo(int hSend, IBizMessage* lpMsg, IF2UnPacker* lpUnPacker)
{
    if (IsJsonMode())
    {
        cJSON* json = GenJsonDatas(lpUnPacker, UFX_FUNC_QRY_SECINFO, UFX_ISSUE_TYPE_DEFAULT, hSend);
        NotifyJsonData(json, UFX_FUNC_QRY_SECINFO, UFX_ISSUE_TYPE_DEFAULT, hSend);
        return;
    }
}

void TradeCallback::OnResponseQryMD(int hSend, IBizMessage* lpMsg, IF2UnPacker* lpUnPacker)
{}

void TradeCallback::OnResponseSubscribe(int hSend, IBizMessage* lpMsg, IF2UnPacker* lpUnPacker, int issue_type)
{
    if (IsJsonMode())
    {
        cJSON* json = GenJsonDatas(lpUnPacker, UFX_FUNC_SUBSCRIBE, issue_type, hSend);
        NotifyJsonData(json, UFX_FUNC_SUBSCRIBE, issue_type, hSend);
        return;
    }
}

void TradeCallback::OnResponseUnSubscribe(int hSend, IBizMessage* lpMsg, IF2UnPacker* lpUnPacker, int issue_type)
{
    if (IsJsonMode())
    {
        cJSON* json = GenJsonDatas(lpUnPacker, UFX_FUNC_SUB_CANCEL, issue_type, hSend);
        NotifyJsonData(json, UFX_FUNC_SUB_CANCEL, issue_type, hSend);
        return;
    }
}

void TradeCallback::OnResponseOrderInsert(int hSend, IBizMessage* lpMsg, IF2UnPacker* lpUnPacker)
{}

void TradeCallback::OnResponseOrderCancel(int hSend, IBizMessage* lpMsg, IF2UnPacker* lpUnPacker)
{}

void TradeCallback::OnRtnOrder(int hSend, IBizMessage* lpMsg, IF2UnPacker* lpUnPacker)
{
    if (IsJsonMode())
    {
        cJSON* json = GenJsonDatas(lpUnPacker, UFX_FUNC_RTN_DATA, UFX_ISSUE_TYPE_ORDER, hSend);
        NotifyJsonData(json, UFX_FUNC_RTN_DATA, UFX_ISSUE_TYPE_ORDER, hSend);
        return;
    }

    TradeCallback::UnpackBizMessage(lpUnPacker, hSend, m_hstd, TradeCallback::UnpackRtnOrderData);
}

void TradeCallback::OnRtnTrade(int hSend, IBizMessage* lpMsg, IF2UnPacker* lpUnPacker)
{
    if (IsJsonMode())
    {
        cJSON* json = GenJsonDatas(lpUnPacker, UFX_FUNC_RTN_DATA, UFX_ISSUE_TYPE_TRADE, hSend);
        NotifyJsonData(json, UFX_FUNC_RTN_DATA, UFX_ISSUE_TYPE_TRADE, hSend);
        return;
    }

    TradeCallback::UnpackBizMessage(lpUnPacker, hSend, m_hstd, TradeCallback::UnpackRtnTradeData);
}


//////////////////////////////////////////////////////////////////////////
int TradeCallback::UnpackLoginData(IF2UnPacker* lpUnPacker, int ref_id, int ds_index, void* ctxdata)
{
    ufxtrade_t* hstd;
    ufxtrade_spi_t* spi;
    hstd = (ufxtrade_t*)ctxdata;
    spi = hstd->spi;

    UFXRspInfoField rsp_info = { 0 };
    TradeCallback::GetErrorField(&rsp_info, lpUnPacker);

    int rows = lpUnPacker->GetRowCount();
    if (rows == 0)
    {
        if (spi && spi->on_user_login)
            spi->on_user_login(hstd, NULL, &rsp_info, ref_id);
    }

    for (int row = 0; row < rows; ++row)
    {
        UFXRspUserLoginField login_data = { 0 };
        // TODO:

        if (spi && spi->on_user_login)
            spi->on_user_login(hstd, &login_data, &rsp_info, ref_id);

        // ��һ�м�¼
        lpUnPacker->Next();
    }

    return 0;
}

int TradeCallback::UnpackRspOrderData(IF2UnPacker* lpUnPacker, int ref_id, int ds_index, void* ctxdata)
{
    ufxtrade_t* hstd;
    ufxtrade_spi_t* spi;
    hstd = (ufxtrade_t*)ctxdata;
    spi = hstd->spi;

    UFXRspInfoField rsp_info = { 0 };
    TradeCallback::GetErrorField(&rsp_info, lpUnPacker);

    int islast = 0;
    int rows = lpUnPacker->GetRowCount();
    if (rows == 0)
    {
        if (spi && spi->on_order_insert)
            spi->on_order_insert(hstd, NULL, &rsp_info, ref_id, 1);
    }

    for (int row = 0; row < rows; ++row)
    {
        HSOrderField order = { 0 };
        strncpy(order.client_id, hstd->client_id, sizeof(order.client_id) - 1);
        extract_str_field(order.entrust_no, "entrust_no");

        order.business_price = lpUnPacker->GetDouble("business_price");
        order.batch_no = lpUnPacker->GetInt("batch_no");
        int order_time = lpUnPacker->GetInt("entrust_time");

        order.init_date = lpUnPacker->GetInt("init_date");
        order.entrust_date = order.init_date;
        order.entrust_time = order_time;

        // TODO: loss many order fields

        if (row == rows - 1)
            islast = 1;

        if (spi && spi->on_order_insert)
            spi->on_order_insert(hstd, &order, &rsp_info, ref_id, islast);

        // ��һ�м�¼
        lpUnPacker->Next();
    }

    return 0;
}

int TradeCallback::UnpackRspOrderActionData(IF2UnPacker* lpUnPacker, int ref_id, int ds_index, void* ctxdata)
{
    ufxtrade_t* hstd;
    ufxtrade_spi_t* spi;
    hstd = (ufxtrade_t*)ctxdata;
    spi = hstd->spi;

    UFXRspInfoField rsp_info = { 0 };
    TradeCallback::GetErrorField(&rsp_info, lpUnPacker);

    int islast = 0;
    int rows = lpUnPacker->GetRowCount();
    if (rows == 0)
    {
        if (spi && spi->on_order_action)
            spi->on_order_action(hstd, NULL, &rsp_info, ref_id, 1);
    }

    for (int row = 0; row < rows; ++row)
    {
        HSOrderField order = { 0 };
        strncpy(order.client_id, hstd->client_id, sizeof(order.client_id) - 1);
        extract_str_field(order.entrust_no, "entrust_no_old");
        extract_str_field(order.exchange_type, "exchange_type");
        extract_str_field(order.stock_code, "stock_code");
        order.entrust_status = lpUnPacker->GetChar("entrust_status_old");

        order.business_price = lpUnPacker->GetDouble("business_price");
        order.batch_no = lpUnPacker->GetInt("batch_no");
        int order_time = lpUnPacker->GetInt("entrust_time");

        order.init_date = lpUnPacker->GetInt("init_date");
        order.entrust_date = order.init_date;
        order.entrust_time = order_time;

        // TODO: loss many order fields

        if (row == rows - 1)
            islast = 1;

        if (spi && spi->on_order_action)
            spi->on_order_action(hstd, &order, &rsp_info, ref_id, islast);

        // ��һ�м�¼
        lpUnPacker->Next();
    }

    return 0;
}

int TradeCallback::UnpackTradingAccountData(IF2UnPacker* lpUnPacker, int ref_id, int ds_index, void* ctxdata)
{
    ufxtrade_t* hstd;
    ufxtrade_spi_t* spi;
    hstd = (ufxtrade_t*)ctxdata;
    spi = hstd->spi;

    UFXRspInfoField rsp_info = { 0 };
    TradeCallback::GetErrorField(&rsp_info, lpUnPacker);

    int islast = 0;
    int rows = lpUnPacker->GetRowCount();
    if (rows == 0)
    {
        if (spi && spi->on_qry_trading_account)
            spi->on_qry_trading_account(hstd, NULL, &rsp_info, ref_id, 1);
    }

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
            spi->on_qry_trading_account(hstd, &ta, &rsp_info, ref_id, islast);

        // ��һ�м�¼
        lpUnPacker->Next();
    }

    return 0;
}

int TradeCallback::UnpackPositionData(IF2UnPacker* lpUnPacker, int ref_id, int ds_index, void* ctxdata)
{
    ufxtrade_t* hstd;
    ufxtrade_spi_t* spi;
    hstd = (ufxtrade_t*)ctxdata;
    spi = hstd->spi;

    UFXRspInfoField rsp_info = { 0 };
    TradeCallback::GetErrorField(&rsp_info, lpUnPacker);

    int islast = 0;
    int rows = lpUnPacker->GetRowCount();
    if (rows == 0)
    {
        if (spi && spi->on_qry_position)
            spi->on_qry_position(hstd, NULL, &rsp_info, ref_id, 1);
    }

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
            spi->on_qry_position(hstd, &pos, &rsp_info, ref_id, islast);

        // ��һ�м�¼
        lpUnPacker->Next();
    }

    return 0;
}


int TradeCallback::UnpackQryOrderData(IF2UnPacker* lpUnPacker, int ref_id, int ds_index, void* ctxdata)
{
    ufxtrade_t* hstd;
    ufxtrade_spi_t* spi;
    hstd = (ufxtrade_t*)ctxdata;
    spi = hstd->spi;

    UFXRspInfoField rsp_info = { 0 };
    TradeCallback::GetErrorField(&rsp_info, lpUnPacker);

    int islast = 0;
    int rows = lpUnPacker->GetRowCount();
    if (rows == 0)
    {
        if (spi && spi->on_qry_order)
            spi->on_qry_order(hstd, NULL, &rsp_info, ref_id, 1);
    }

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
            spi->on_qry_order(hstd, &order, &rsp_info, ref_id, islast);

        // ��һ�м�¼
        lpUnPacker->Next();
    }

    return 0;
}

int TradeCallback::UnpackQryTradeData(IF2UnPacker* lpUnPacker, int ref_id, int ds_index, void* ctxdata)
{
    ufxtrade_t* hstd;
    ufxtrade_spi_t* spi;
    hstd = (ufxtrade_t*)ctxdata;
    spi = hstd->spi;

    UFXRspInfoField rsp_info = { 0 };
    TradeCallback::GetErrorField(&rsp_info, lpUnPacker);

    int islast = 0;
    int rows = lpUnPacker->GetRowCount();
    if (rows == 0)
    {
        if (spi && spi->on_qry_trade)
            spi->on_qry_trade(hstd, NULL, &rsp_info, ref_id, 1);
    }

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
            spi->on_qry_trade(hstd, &trade, &rsp_info, ref_id, islast);

        // ��һ�м�¼
        lpUnPacker->Next();
    }

    return 0;
}

int TradeCallback::UnpackRtnOrderData(IF2UnPacker* lpUnPacker, int ref_id, int ds_index, void* ctxdata)
{
    ufxtrade_t* hstd;
    ufxtrade_spi_t* spi;
    hstd = (ufxtrade_t*)ctxdata;
    spi = hstd->spi;

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

        if (spi && spi->on_rtn_order)
            spi->on_rtn_order(hstd, &order, ref_id);

        // ��һ�м�¼
        lpUnPacker->Next();
    }

    return 0;
}

int TradeCallback::UnpackRtnTradeData(IF2UnPacker* lpUnPacker, int ref_id, int ds_index, void* ctxdata)
{
    ufxtrade_t* hstd;
    ufxtrade_spi_t* spi;
    hstd = (ufxtrade_t*)ctxdata;
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

        // TODO: �������ر�

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

        if (spi && spi->on_rtn_trade)
            spi->on_rtn_trade(hstd, &trade, ref_id);

        // ��һ�м�¼
        lpUnPacker->Next();
    }

    return 0;
}

int TradeCallback::UnpackBizMessage(IF2UnPacker* lpUnPacker, int ref_id, void* ctxdata,
    int(*data_proc_pt)(IF2UnPacker* lpUnPacker, int ref_id, int ds_index, void* ctxdata))
{
    if (!data_proc_pt) {
        return -1;
    }

    for (int i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
    {
        lpUnPacker->SetCurrentDatasetByIndex(i);
        data_proc_pt(lpUnPacker, ref_id, i, ctxdata);
    }
    return 0;
}

void ShowPacket(IF2UnPacker *lpUnPacker)
{
    int i = 0, t = 0, j = 0, k = 0;

    for (i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
    {
        // ���õ�ǰ�����

        printf("��¼����%d/%d\r\n", i + 1, lpUnPacker->GetDatasetCount());
        lpUnPacker->SetCurrentDatasetByIndex(i);

        // ��ӡ���м�¼
        for (j = 0; j < (int)lpUnPacker->GetRowCount(); ++j)
        {
            printf("\t��%d/%d����¼��\r\n", j + 1, lpUnPacker->GetRowCount());
            // ��ӡÿ����¼
            for (k = 0; k < lpUnPacker->GetColCount(); ++k)
            {
                switch (lpUnPacker->GetColType(k))
                {
                case 'I':
                    printf("\t��������%20s = %35d\r\n", lpUnPacker->GetColName(k), lpUnPacker->GetIntByIndex(k));
                    break;

                case 'C':
                    printf("\t���ַ���%20s = %35c\r\n", lpUnPacker->GetColName(k), lpUnPacker->GetCharByIndex(k));
                    break;

                case 'S':

                    if (NULL != strstr((char *)lpUnPacker->GetColName(k), "password"))
                    {
                        printf("\t���ִ���%20s = %35s\r\n", lpUnPacker->GetColName(k), "******");
                    }
                    else
                        printf("\t���ִ���%20s = %35s\r\n", lpUnPacker->GetColName(k), lpUnPacker->GetStrByIndex(k));
                    break;

                case 'F':
                    printf("\t����ֵ��%20s = %35f\r\n", lpUnPacker->GetColName(k), lpUnPacker->GetDoubleByIndex(k));
                    break;

                case 'R':
                {
                    int nLength = 0;
                    void *lpData = lpUnPacker->GetRawByIndex(k, &nLength);
                    switch (nLength) {
                    case 0:
                        printf("\t�����ݡ�%20s = %35s\r\n", lpUnPacker->GetColName(k), "(N/A)");
                        break;
                    default:
                        printf("\t�����ݡ�%20s = 0x", lpUnPacker->GetColName(k));
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
                    // ��2�������ݽ��д���
                    break;
                }

                default:
                    // δ֪��������
                    printf("δ֪�������͡�\n");
                    break;
                }
            }

            putchar('\n');

            lpUnPacker->Next();
        }

        putchar('\n');
    }
}

