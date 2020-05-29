#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <t2sdk_interface.h>

#include "hstrade.h"
#include "hstrade_callback.h"


#define HS_TRADE                0x44545348

#define HS_TRADE_VERSION        "1.0.0"


extern void ShowPacket(IF2UnPacker *lpUnPacker);


static int hstrade_is_async(hstrade_t* hstd)
{
    return hstd->is_async;
}

static IBizMessage* _hstrade_make_bizmsg(hstrade_t* hstd, int funcid)
{
    IBizMessage* lpBizMessage;
    lpBizMessage = NewBizMessage();
    lpBizMessage->AddRef();

    lpBizMessage->SetFunction(funcid);
    lpBizMessage->SetPacketType(REQUEST_PACKET);

#if 0
    lpBizMessage->SetBranchNo(hstd->apidata.branch_no);
    lpBizMessage->SetSystemNo(hstd->apidata.system_no);
    lpBizMessage->SetCompanyID(91000);
    lpBizMessage->SetSenderCompanyID(91000);
#endif

    return lpBizMessage;
}

static IF2Packer* _hstrade_make_if2pakcer(hstrade_t* hstd)
{
    hstrade_apidata_t* apidata = &hstd->apidata;

    // ��ȡ�汾Ϊ2���͵�pack�����
    IF2Packer* lpPacker = NewPacker(2);
    if (!lpPacker)
    {
        HSDebugInfo(HSDbgFd, "make_if2pakcer NewPacker() failed!\n");
        return lpPacker;
    }

    lpPacker->AddRef();
    lpPacker->BeginPack();

    // �����ֶ���
    lpPacker->AddField("op_branch_no",  HS_FT_Int, 5);
    lpPacker->AddField("op_entrust_way",HS_FT_Char, 1);
    lpPacker->AddField("op_station",    HS_FT_Str, 255);
    lpPacker->AddField("branch_no",     HS_FT_Int, 5);
    lpPacker->AddField("client_id",     HS_FT_Str, 18);
    lpPacker->AddField("fund_account",  HS_FT_Str, 18);
    lpPacker->AddField("password",      HS_FT_Str, 10);
    lpPacker->AddField("password_type", HS_FT_Char, 1);
    lpPacker->AddField("user_token",    HS_FT_Str, 512);

    // �����Ӧ���ֶ�ֵ
    lpPacker->AddInt(apidata->op_branch_no);    // op_branch_no
    lpPacker->AddChar(apidata->entrust_way);    // op_entrust_way
    lpPacker->AddStr(apidata->op_station);      // op_station
    lpPacker->AddInt(apidata->branch_no);       // branch_no
    lpPacker->AddStr(hstd->client_id);          // client_id
    lpPacker->AddStr(hstd->client_id);          // fund_account
    lpPacker->AddStr(hstd->password);           // password
    lpPacker->AddChar(apidata->password_type);  // password_type
    lpPacker->AddStr(apidata->user_token);      // user_token

    // �������
    lpPacker->EndPack();

    return lpPacker;
}

//////////////////////////////////////////////////////////////////////////
const char* HS_TRADE_STDCALL hstrade_version(int* pver)
{
    if (pver) {
        *pver = GetVersionInfo();
    }
    return HS_TRADE_VERSION;
}

hstrade_t* HS_TRADE_STDCALL hstrade_create(int is_async)
{
    hstrade_t* hstd;
    hstd = (hstrade_t*)malloc(sizeof(hstrade_t));
    memset(hstd, 0, sizeof(hstrade_t));

    hstd->desc      = HS_TRADE;
    hstd->is_async  = is_async;
    hstd->debug_mode= 0;
    hstd->timeoutms = HSTRADE_DEFAULT_TIMEOUT;
    hstd->userdata  = NULL;

    hstd->config = NewConfig();
    hstd->config->AddRef();

    hstd->conn = NULL;

    hstd->callback = new TradeCallback();
    hstd->callback->SetContextData(hstd);

    return hstd;
}

void HS_TRADE_STDCALL hstrade_realese(hstrade_t* hstd)
{
    if (!hstd)
    {
        return;
    }

    // dummy
    int* pdesc = (int*)hstd;
    if (*pdesc != HS_TRADE)
    {
        HSDebugInfo(HSDbgFd, "hstrade_realese unknown desc:%d!\n", *pdesc);
        return;
    }

    if (hstd->config)
    {
        hstd->config->Release();
        hstd->config = NULL;
    }

    if (hstd->callback)
    {
        delete hstd->callback;
        hstd->callback = NULL;
    }

    free(hstd);
}

void  HS_TRADE_STDCALL hstrade_set_userdata(hstrade_t* hstd, void* userdata)
{
    hstd->userdata = userdata;
}

void* HS_TRADE_STDCALL hstrade_get_userdata(hstrade_t* hstd)
{
    return hstd->userdata;
}

void HS_TRADE_STDCALL hstrade_debug_mode(hstrade_t* hstd, int level)
{
    hstd->debug_mode = level;
}

void HS_TRADE_STDCALL hstrade_set_timeout(hstrade_t* hstd, int timeoutms)
{
    if (timeoutms < 0)
    {
        HSDebugInfo(HSDbgFd, "hstrade_set_timeout timeoutms:%d invalid\n", timeoutms);
        return;
    }
    hstd->timeoutms = timeoutms;
}

void HS_TRADE_STDCALL hstrade_register_spi(hstrade_t* hstd, hstrade_spi_t* spi)
{
    hstd->spi = spi;
}

int HS_TRADE_STDCALL hstrade_config_load(hstrade_t* hstd, const char* config_file)
{
    if (!config_file || !config_file[0])
    {
        return -1;
    }

    if (!hstd->config)
    {
        hstd->config = NewConfig();
        hstd->config->AddRef();
    }

    return hstd->config->Load(config_file);
}

int HS_TRADE_STDCALL hstrade_config_set_string(hstrade_t* hstd, const char* section, const char* entry_name, const char* value)
{
    if (!section || !entry_name || !value)
    {
        return -1;
    }

    if (!hstd->config)
    {
        return -2;
    }

    if (strcmp(section, "t2sdk") == 0)
    {
        if (strcmp(entry_name, "servers") == 0)
            strncpy(hstd->server_addr, value, sizeof(hstd->server_addr) - 1);
    }
    if (strcmp(section, "ufx") == 0)
    {
        if (strcmp(entry_name, "fund_account") == 0)
            strncpy(hstd->client_id, value, sizeof(hstd->client_id) - 1);
    }

    // HSDebugInfo(HSDbgFd, "hstrade_config_set_string section:%s, entry:%s, value:%s\n", section, entry_name, value);

    return hstd->config->SetString(section, entry_name, value);
}

int HS_TRADE_STDCALL hstrade_config_set_int(hstrade_t* hstd, const char* section, const char* entry_name, const int value)
{
    if (!section || !entry_name || !value)
    {
        return -1;
    }

    if (!hstd->config)
    {
        return -2;
    }

    // HSDebugInfo(HSDbgFd, "hstrade_config_set_int section:%s, entry:%s, value:%d\n", section, entry_name, value);

    return hstd->config->SetInt(section, entry_name, value);
}

const char* HS_TRADE_STDCALL hstrade_config_get_string(hstrade_t* hstd, const char* section, const char* entry_name, const char* default_value)
{
    if (!hstd->config)
    {
        return NULL;
    }

    return hstd->config->GetString(section, entry_name, default_value);
}

int HS_TRADE_STDCALL hstrade_config_get_int(hstrade_t* hstd, const char* section, const char* entry_name, const int default_value)
{
    if (!hstd->config)
    {
        return default_value;
    }

    return hstd->config->GetInt(section, entry_name, default_value);
}

int HS_TRADE_STDCALL hstrade_init(hstrade_t* hstd, int timeoutms)
{
    int rv = 0;

    CConfigInterface* config;
    CConnectionInterface* conn;

    config = hstd->config;

    hstd->apidata.entrust_way = '7';
    hstd->apidata.op_branch_no = 0;
    hstd->apidata.password_type = HS_PWD_TYPE_TRADE;

    // �������Ӷ���
    conn = NewConnection(config);
    conn->AddRef();

    rv = conn->Create2BizMsg(hstd->callback);
    if (0 != rv)
    {
        HSDebugInfo(HSDbgFd, "hstrade_init Create2BizMsg() failed rv=%d, msg:%s\n",
            rv, conn->GetErrorMsg(rv));

        conn->Release();
        return rv;
    }

    rv = conn->Connect(timeoutms);
    if (0 != rv)
    {
        fprintf(stderr, "hstrade_init Connect(%d) failed rv=%d, msg:%s\n",
            timeoutms, rv, conn->GetErrorMsg(rv));

        conn->Release();
        return rv;
    }

    hstd->conn = conn;
    return 0;
}

int HS_TRADE_STDCALL hstrade_subscribe(hstrade_t* hstd, int issue_type)
{
    hstrade_apidata_t*      apidata;
    CConnectionInterface*   conn;

    apidata = &hstd->apidata;
    conn = hstd->conn;

    // ��ȡ�汾Ϊ2���͵�pack�����
    IF2Packer *lpPacker = NewPacker(2);
    if (!lpPacker)
    {
        HSDebugInfo(HSDbgFd, "hstrade_subscribe NewPacker() failed!\n");
        return -1;
    }
    lpPacker->AddRef();

    // ��ʼ���
    lpPacker->BeginPack();
    lpPacker->AddField("branch_no", HS_FT_Int, 5);
    lpPacker->AddField("fund_account", HS_FT_Str, 18);
#if 1
    lpPacker->AddField("op_branch_no", HS_FT_Int, 5);
    lpPacker->AddField("op_entrust_way", HS_FT_Char, 1);
    lpPacker->AddField("op_station", HS_FT_Str, 255);
    lpPacker->AddField("client_id", HS_FT_Str, 18);
    lpPacker->AddField("password", HS_FT_Str, 10);
    // lpPacker->AddField("password_type", HS_FT_Char, 1);
    lpPacker->AddField("user_token", HS_FT_Str, 40);
    // lpPacker->AddField("sysnode_id", HS_FT_Int, 4);
#endif//0
    lpPacker->AddField("issue_type", HS_FT_Int, 4);

    // �����Ӧ���ֶ�ֵ
    lpPacker->AddInt(apidata->branch_no);
    lpPacker->AddStr(hstd->client_id);
#if 1
    lpPacker->AddInt(apidata->op_branch_no);
    lpPacker->AddChar(apidata->entrust_way);
    lpPacker->AddStr(apidata->op_station);
    lpPacker->AddStr(hstd->client_id);
    lpPacker->AddStr(hstd->password);
    // lpPacker->AddChar(apidata->password_type);
    lpPacker->AddStr(apidata->user_token);
    // lpPacker->AddInt(apidata->sysnode_id);
#endif
    lpPacker->AddInt(issue_type);

    // �������
    lpPacker->EndPack();

    // ���ô������
    IBizMessage* lpBizMessage = NewBizMessage();
    lpBizMessage->AddRef();

    lpBizMessage->SetFunction(UFX_FUNC_SUBSCRIBE);
    lpBizMessage->SetPacketType(REQUEST_PACKET);
    lpBizMessage->SetSequeceNo(hstd->sequence_no++);
    lpBizMessage->SetIssueType(issue_type);     // FIXME
    lpBizMessage->SetKeyInfo(lpPacker->GetPackBuf(), lpPacker->GetPackLen());

    // ������Ϣ
    int rv = conn->SendBizMsg(lpBizMessage, 1);
    // HSDebugInfo(HSDbgFd, "hstrade_subscribe SendBizMsg() rv:%d\n", rv);

    lpPacker->FreeMem(lpPacker->GetPackBuf());
    lpPacker->Release();
    lpBizMessage->Release();

    return rv;
}

const char* HS_TRADE_STDCALL hstrade_get_trading_day(hstrade_t* hstd)
{
    return hstd->trading_day;
}

int HS_TRADE_STDCALL hstrade_user_login(hstrade_t* hstd, HSReqUserLoginField* login_req)
{
    int rv = 0, len = 0;
    // cout<<"password"<<m_Password<<endl;
    // EncodeEx(m_Password,pout);
    // cout<<"pout: "<<pout<<endl;

    IF2Packer* lpPacker;
    lpPacker = NewPacker(2);
    if (!lpPacker)
    {
        HSDebugInfo(HSDbgFd, "hstrade_user_login NewPacker() failed!\n");
        return -1;
    }

    lpPacker->AddRef();
    lpPacker->BeginPack();

    lpPacker->AddField("op_branch_no", HS_FT_Int, 5);     // ������֧����
    lpPacker->AddField("op_entrust_way", HS_FT_Char, 1);   // ί�з�ʽ 
    lpPacker->AddField("op_station", HS_FT_Str, 255);     // վ���ַ
    lpPacker->AddField("branch_no", HS_FT_Int, 5);        // Ӫҵ�����
    lpPacker->AddField("input_content", HS_FT_Char, 1);
    lpPacker->AddField("account_content", HS_FT_Str, 30); // �ʽ��˺�
    lpPacker->AddField("content_type", HS_FT_Str, 6);
    lpPacker->AddField("password", HS_FT_Str, 10);
    lpPacker->AddField("password_type", HS_FT_Char, 1);
    //lpPacker->AddField("asset_prop", HS_FT_Char, 1);

    if (strcmp(hstd->client_id, login_req->client_id) != 0)
    {
        fprintf(stderr, "WARN client_id not equal by hstrade_init() pass-in\n");
    }

    // strcpy(hstd->client_id, login_field->client_id);
    strcpy(hstd->password, login_req->password);

    // �����Ӧ���ֶ�ֵ
    lpPacker->AddInt(hstd->apidata.op_branch_no);
    lpPacker->AddChar(hstd->apidata.entrust_way);
    lpPacker->AddStr(hstd->apidata.op_station);
    lpPacker->AddInt(hstd->apidata.branch_no);
    lpPacker->AddChar('1');     // input content
    lpPacker->AddStr(hstd->client_id);
    lpPacker->AddStr("0");      // content type
    lpPacker->AddStr(hstd->password);
    lpPacker->AddChar(hstd->apidata.password_type);
    //lpPacker->AddChar('0');

    // �������
    lpPacker->EndPack();

    // ���ô������
    IBizMessage* lpBizMessage;
    lpBizMessage = _hstrade_make_bizmsg(hstd, UFX_FUNC_LOGIN);
    lpBizMessage->SetContent(lpPacker->GetPackBuf(), lpPacker->GetPackLen());

    // ��������
    rv = hstd->conn->SendBizMsg(lpBizMessage, hstd->is_async);
    if (!hstrade_is_async(hstd))
    {
        if (rv < 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_user_login, rv:%d, err:%s!\n", rv, hstd->conn->GetErrorMsg(rv));
            goto EXIT;
        }

        // ͬ��ģʽ
        IBizMessage* lpBizMessageRecv = NULL;
        rv = hstd->conn->RecvBizMsg(rv, &lpBizMessageRecv, hstd->timeoutms);
        // HSDebugInfo(HSDbgFd, "hstrade_user_login RecvBizMsg ret:%d\n", rv);

        int iReturnCode = lpBizMessageRecv->GetReturnCode();
        if (iReturnCode != 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_user_login return code error code:%d, msg:%s\n",
                lpBizMessageRecv->GetReturnCode(), lpBizMessageRecv->GetErrorInfo());
            rv = iReturnCode;
        }
        else
        {
            const void * lpBuffer = lpBizMessageRecv->GetContent(len);
            IF2UnPacker * lpUnPacker = NewUnPacker((void *)lpBuffer, len);
            lpUnPacker->AddRef();

            if (hstd->debug_mode)
            {
                IF2UnPacker* lpUnPacker2 = NewUnPacker((void*)lpBuffer, len);
                lpUnPacker2->AddRef();
                ShowPacket(lpUnPacker2);
                lpUnPacker2->Release();
            }

            TradeCallback::UnpackBizMessage(lpUnPacker, 0, hstd, TradeCallback::UnpackPositionData);
            lpUnPacker->Release();
        }
    }

EXIT:
    if (lpPacker)
    {
        lpPacker->FreeMem(lpPacker->GetPackBuf());
        lpPacker->Release();
    }
    if (lpBizMessage)
    {
        lpBizMessage->Release();
    }
    return 0;
}

int HS_TRADE_STDCALL hstrade_order_insert(hstrade_t* hstd, HSReqOrderInsertField* order_req)
{
    hstrade_apidata_t*      apidata;
    CConnectionInterface*   conn;

    apidata = &hstd->apidata;
    conn = hstd->conn;

    int rv = 0;

    ///��ȡ�汾Ϊ2���͵�pack�����
    IF2Packer *lpPacker = NewPacker(2);
    if (!lpPacker)
    {
        HSDebugInfo(HSDbgFd, "hstrade_order_insert NewPacker() failed!\n");
        return -1;
    }
    lpPacker->AddRef();

    // ��ʼ���
    lpPacker->BeginPack();

    // �����ֶ���
    lpPacker->AddField("op_branch_no", HS_FT_Int, 5);      // ���� ���� ����
    lpPacker->AddField("op_entrust_way", HS_FT_Char, 1);
    lpPacker->AddField("op_station", HS_FT_Str, 255);
    lpPacker->AddField("branch_no", HS_FT_Int, 5);
    lpPacker->AddField("client_id", HS_FT_Str, 18);
    lpPacker->AddField("fund_account", HS_FT_Str, 18);
    lpPacker->AddField("password", HS_FT_Str, 16);
    lpPacker->AddField("password_type", HS_FT_Char, 1);
    lpPacker->AddField("user_token", HS_FT_Str, 512);
    lpPacker->AddField("exchange_type", HS_FT_Str, 4);
    lpPacker->AddField("stock_account", HS_FT_Str, 12);
    lpPacker->AddField("stock_code", HS_FT_Str, 16);
    lpPacker->AddField("entrust_amount", HS_FT_Int, 10);
    lpPacker->AddField("entrust_price", HS_FT_Float, 11, 3);
    lpPacker->AddField("entrust_bs", HS_FT_Char, 1);
    lpPacker->AddField("entrust_prop", HS_FT_Str, 3);
    lpPacker->AddField("batch_no", HS_FT_Int, 8);

    // �����Ӧ���ֶ�ֵ
    lpPacker->AddInt(apidata->op_branch_no);        // op_branch_no
    lpPacker->AddChar(apidata->entrust_way);        // op_entrust_way
    lpPacker->AddStr(apidata->op_station);          // op_station
    lpPacker->AddInt(apidata->branch_no);           // branch_no
    lpPacker->AddStr(hstd->client_id);              // client_id
    lpPacker->AddStr(hstd->client_id);              // fund_account
    lpPacker->AddStr(hstd->password);               // password
    lpPacker->AddChar(apidata->password_type);      // password_type
    lpPacker->AddStr(apidata->user_token);          // user_token
    lpPacker->AddStr(order_req->exchange_type);     // exchange_type
    lpPacker->AddStr("");                            // stock_account
    lpPacker->AddStr(order_req->stock_code);        // stock_code
    lpPacker->AddDouble(order_req->entrust_amount); // entrust_amount
    lpPacker->AddDouble(order_req->entrust_price);  // entrust_price
    lpPacker->AddChar(order_req->entrust_bs);       // entrust_bs
    lpPacker->AddStr(order_req->entrust_prop);      // entrust_prop
    lpPacker->AddInt(order_req->batch_no);          // batch_no

    // �������
    lpPacker->EndPack();

    // ���ô������
    IBizMessage* lpBizMessage;
    lpBizMessage = _hstrade_make_bizmsg(hstd, UFX_FUNC_PLACE_ORDER);
    lpBizMessage->SetContent(lpPacker->GetPackBuf(), lpPacker->GetPackLen());

    // ��������
    rv = conn->SendBizMsg(lpBizMessage, hstd->is_async);
    if (!hstrade_is_async(hstd))
    {
        // ͬ��ģʽ
        if (rv < 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_order_insert, rv:%d, err:%s!\n", rv, conn->GetErrorMsg(rv));
            goto EXIT;
        }

        IBizMessage* lpBizMessageRecv = NULL;
        rv = conn->RecvBizMsg(rv, &lpBizMessageRecv, hstd->timeoutms);
        if (rv != 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_order_insert recvmsg failed, rv:%d, err:%s!\n", rv, conn->GetErrorMsg(rv));
            goto EXIT;
        }
        else
        {
            int iReturnCode = lpBizMessageRecv->GetReturnCode();
            if (iReturnCode != 0)
            {
                HSDebugInfo(HSDbgFd, "hstrade_order_insert return code error code:%d, msg:%s\n",
                    lpBizMessageRecv->GetReturnCode(), lpBizMessageRecv->GetErrorInfo());
                rv = iReturnCode;
            }
            else
            {
                int len = 0;
                const void * lpBuffer = lpBizMessageRecv->GetContent(len);
                IF2UnPacker * lpUnPacker = NewUnPacker((void *)lpBuffer, len);
                lpUnPacker->AddRef();

                if (hstd->debug_mode)
                {
                    IF2UnPacker* lpUnPacker2 = NewUnPacker((void*)lpBuffer, len);
                    lpUnPacker2->AddRef();
                    ShowPacket(lpUnPacker2);
                    lpUnPacker2->Release();
                }

                TradeCallback::UnpackBizMessage(lpUnPacker, 0, hstd, TradeCallback::UnpackRspOrderData);
                lpUnPacker->Release();
            }
        }
    }

EXIT:
    // �ͷ�pack��������ڴ�
    if (lpPacker)
    {
        lpPacker->FreeMem(lpPacker->GetPackBuf());
        lpPacker->Release();
    }
    if (lpBizMessage)
    {
        lpBizMessage->Release();
    }

    return rv;
}

int HS_TRADE_STDCALL hstrade_order_action(hstrade_t* hstd, HSReqOrderActionField* order_action)
{
    hstrade_apidata_t*      apidata;
    CConnectionInterface*   conn;

    apidata = &hstd->apidata;
    conn = hstd->conn;

    int rv = 0;

    // ��ȡ�汾Ϊ2���͵�pack�����
    IF2Packer *lpPacker = NewPacker(2);
    if (!lpPacker)
    {
        HSDebugInfo(HSDbgFd, "hstrade_order_action NewPacker() failed!\n");
        return -1;
    }
    lpPacker->AddRef();

    // ��ʼ���
    lpPacker->BeginPack();

    // �����ֶ���
    lpPacker->AddField("op_branch_no", HS_FT_Int, 5);
    lpPacker->AddField("op_entrust_way", HS_FT_Char, 1);
    lpPacker->AddField("op_station", HS_FT_Str, 255);
    lpPacker->AddField("branch_no", HS_FT_Int, 5);
    lpPacker->AddField("client_id", HS_FT_Str, 18);
    lpPacker->AddField("fund_account", HS_FT_Str, 18);
    lpPacker->AddField("password", HS_FT_Str, 10);
    lpPacker->AddField("password_type", HS_FT_Char, 1);
    lpPacker->AddField("user_token", HS_FT_Str, 512);
    lpPacker->AddField("batch_flag", HS_FT_Str, 4);   // FIXME
    lpPacker->AddField("entrust_no", HS_FT_Int, 8);   // FIXME

    // �����Ӧ���ֶ�ֵ
    lpPacker->AddInt(apidata->op_branch_no);        // op_branch_no	
    lpPacker->AddChar(apidata->entrust_way);        // op_entrust_way	
    lpPacker->AddStr(apidata->op_station);          // op_station
    lpPacker->AddInt(apidata->branch_no);           // branch_no
    lpPacker->AddStr(hstd->client_id);              // client_id
    lpPacker->AddStr(hstd->client_id);              // fund_account
    lpPacker->AddStr(hstd->password);               // password	
    lpPacker->AddChar(apidata->password_type);      // password_type	
    lpPacker->AddStr(apidata->user_token);          // user_token
    lpPacker->AddStr("0");                          // action single order
    lpPacker->AddInt(atoi(order_action->entrust_no));    // entrust_no

    // �������
    lpPacker->EndPack();

    IBizMessage* lpBizMessage;
    lpBizMessage = _hstrade_make_bizmsg(hstd, UFX_FUNC_CANCEL_ORDER);
    lpBizMessage->SetContent(lpPacker->GetPackBuf(), lpPacker->GetPackLen());

    rv = conn->SendBizMsg(lpBizMessage, hstd->is_async);
    if (!hstrade_is_async(hstd))
    {
        // ͬ��ģʽ
        if (rv < 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_order_action failed, rv:%d, err:%s!\n", rv, conn->GetErrorMsg(rv));
            goto EXIT;
        }

        IBizMessage* lpBizMessageRecv = NULL;
        rv = conn->RecvBizMsg(rv, &lpBizMessageRecv, hstd->timeoutms);
        if (rv != 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_order_action recvmsg failed, rv:%d, err:%s!\n", rv, conn->GetErrorMsg(rv));
            goto EXIT;
        }
        else
        {
            int iReturnCode = lpBizMessageRecv->GetReturnCode();
            if (iReturnCode != 0)
            {
                HSDebugInfo(HSDbgFd, "hstrade_order_action return code error code:%d, msg:%s\n",
                    lpBizMessageRecv->GetReturnCode(), lpBizMessageRecv->GetErrorInfo());
                rv = iReturnCode;
            }
            else
            {
                int len = 0;
                const void * lpBuffer = lpBizMessageRecv->GetContent(len);
                IF2UnPacker * lpUnPacker = NewUnPacker((void *)lpBuffer, len);
                lpUnPacker->AddRef();

                if (hstd->debug_mode)
                {
                    IF2UnPacker* lpUnPacker2 = NewUnPacker((void*)lpBuffer, len);
                    lpUnPacker2->AddRef();
                    ShowPacket(lpUnPacker2);
                    lpUnPacker2->Release();
                }

                TradeCallback::UnpackBizMessage(lpUnPacker, 0, hstd, TradeCallback::UnpackRspOrderActionData);
                lpUnPacker->Release();
            }
        }
    }

EXIT:
    // �ͷ�pack��������ڴ�
    if (lpPacker)
    {
        lpPacker->FreeMem(lpPacker->GetPackBuf());
        lpPacker->Release();
    }
    if (lpBizMessage)
    {
        lpBizMessage->Release();
    }

    return rv;
}

int HS_TRADE_STDCALL hstrade_qry_trading_account(hstrade_t* hstd, HSReqQueryField* qry_field)
{
    hstrade_apidata_t*      apidata;
    CConnectionInterface*   conn;
    int rv;

    apidata = &hstd->apidata;
    conn = hstd->conn;

    // ��ȡ�汾Ϊ2���͵�pack�����
    IF2Packer* lpPacker = _hstrade_make_if2pakcer(hstd);

    // ���ð�����
    IBizMessage* lpBizMessage = _hstrade_make_bizmsg(hstd, UFX_FUNC_QRY_CASH);
    lpBizMessage->SetContent(lpPacker->GetPackBuf(), lpPacker->GetPackLen());

    // ����ҵ������
    rv = conn->SendBizMsg(lpBizMessage, hstd->is_async);
    if (!hstrade_is_async(hstd))
    {
        // ͬ��ģʽ
        if (rv < 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_qry_trading_account failed, rv:%d, err:%s!\n", rv, conn->GetErrorMsg(rv));
            rv = -2;
            goto EXIT;
        }

        IBizMessage* lpBizMessageRecv = NULL;

        rv = conn->RecvBizMsg(rv, &lpBizMessageRecv, hstd->timeoutms);
        if (rv != 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_qry_trading_account recvmsg failed, rv:%d, err:%s!\n", rv, conn->GetErrorMsg(rv));
            goto EXIT;
        }
        else
        {
            int iReturnCode = lpBizMessageRecv->GetReturnCode();
            if (iReturnCode != 0)
            {
                HSDebugInfo(HSDbgFd, "hstrade_qry_trading_account return code error code:%d, msg:%s\n",
                    lpBizMessageRecv->GetReturnCode(), lpBizMessageRecv->GetErrorInfo());
            }
            else if (iReturnCode == 0)
            {
                int len = 0;
                const void * lpBuffer = lpBizMessageRecv->GetContent(len);
                IF2UnPacker * lpUnPacker = NewUnPacker((void *)lpBuffer, len);
                lpUnPacker->AddRef();

                if (hstd->debug_mode)
                {
                    IF2UnPacker* lpUnPacker2 = NewUnPacker((void*)lpBuffer, len);
                    lpUnPacker2->AddRef();
                    ShowPacket(lpUnPacker2);
                    lpUnPacker2->Release();
                }

                TradeCallback::UnpackBizMessage(lpUnPacker, 0, hstd, TradeCallback::UnpackTradingAccountData);
                lpUnPacker->Release();
            }

        }
    }

EXIT:
    // �ͷ�pack��������ڴ�
    if (lpPacker)
    {
        lpPacker->FreeMem(lpPacker->GetPackBuf());
        lpPacker->Release();
    }
    if (lpBizMessage)
    {
        lpBizMessage->Release();
    }

    return rv;
}

int HS_TRADE_STDCALL hstrade_qry_investor(hstrade_t* hstd, HSReqQueryField* qry_field)
{
    return 0;
}

int HS_TRADE_STDCALL hstrade_qry_position(hstrade_t* hstd, HSReqQueryField* qry_field)
{
    hstrade_apidata_t*      apidata;
    CConnectionInterface*   conn;
    int rv;

    apidata = &hstd->apidata;
    conn = hstd->conn;

    // ��ȡ�汾Ϊ2���͵�pack�����
    IF2Packer* lpPacker = _hstrade_make_if2pakcer(hstd);

    // ���ð�����
    IBizMessage* lpBizMessage = _hstrade_make_bizmsg(hstd, UFX_FUNC_QRY_POSITION);
    lpBizMessage->SetContent(lpPacker->GetPackBuf(), lpPacker->GetPackLen());

    // ����ҵ������
    rv = conn->SendBizMsg(lpBizMessage, hstd->is_async);
    if (!hstrade_is_async(hstd))
    {
        // ͬ��ģʽ
        if (rv < 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_qry_position failed, rv:%d, err:%s!\n", rv, conn->GetErrorMsg(rv));
            rv = -2;
            goto EXIT;
        }

        IBizMessage* lpBizMessageRecv = NULL;
        rv = conn->RecvBizMsg(rv, &lpBizMessageRecv, hstd->timeoutms);
        if (rv != 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_qry_position recvmsg failed, rv:%d, err:%s!\n", rv, conn->GetErrorMsg(rv));
            goto EXIT;
        }
        else
        {
            int iReturnCode = lpBizMessageRecv->GetReturnCode();
            if (iReturnCode != 0)
            {
                HSDebugInfo(HSDbgFd, "hstrade_qry_position return code error code:%d, msg:%s\n",
                    lpBizMessageRecv->GetReturnCode(), lpBizMessageRecv->GetErrorInfo());
                rv = iReturnCode;
            }
            else
            {
                int len = 0;
                const void * lpBuffer = lpBizMessageRecv->GetContent(len);
                IF2UnPacker * lpUnPacker = NewUnPacker((void *)lpBuffer, len);
                lpUnPacker->AddRef();

                if (hstd->debug_mode)
                {
                    IF2UnPacker* lpUnPacker2 = NewUnPacker((void*)lpBuffer, len);
                    lpUnPacker2->AddRef();
                    ShowPacket(lpUnPacker2);
                    lpUnPacker2->Release();
                }

                TradeCallback::UnpackBizMessage(lpUnPacker, 0, hstd, TradeCallback::UnpackPositionData);
                lpUnPacker->Release();
            }

        }
    }

EXIT:
    // �ͷ�pack��������ڴ�
    if (lpPacker)
    {
        lpPacker->FreeMem(lpPacker->GetPackBuf());
        lpPacker->Release();
    }
    if (lpBizMessage)
    {
        lpBizMessage->Release();
    }

    return rv;
}

int HS_TRADE_STDCALL hstrade_qry_trade(hstrade_t* hstd, HSReqQueryField* qry_field)
{
    hstrade_apidata_t*      apidata;
    CConnectionInterface*   conn;
    int rv;

    apidata = &hstd->apidata;
    conn = hstd->conn;

    // ����ҵ����Ϣ����
    IBizMessage* lpBizMessage = _hstrade_make_bizmsg(hstd, UFX_FUNC_QRY_TRADE);

    // ��ȡ�汾Ϊ2���͵�pack�����
    IF2Packer* lpPacker = _hstrade_make_if2pakcer(hstd);

    // ���ð�����
    lpBizMessage->SetContent(lpPacker->GetPackBuf(), lpPacker->GetPackLen());

    // ����ҵ������
    rv = conn->SendBizMsg(lpBizMessage, hstd->is_async);
    if (!hstrade_is_async(hstd))
    {
        // ͬ��ģʽ
        if (rv < 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_qry_trade failed, rv:%d, err:%s!\n", rv, conn->GetErrorMsg(rv));
            rv = -2;
            goto EXIT;
        }

        IBizMessage* lpBizMessageRecv = NULL;
        rv = conn->RecvBizMsg(rv, &lpBizMessageRecv, hstd->timeoutms);
        if (rv != 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_qry_trade recvmsg failed, rv:%d, err:%s!\n", rv, conn->GetErrorMsg(rv));
            goto EXIT;
        }
        else
        {
            int iReturnCode = lpBizMessageRecv->GetReturnCode();
            if (iReturnCode != 0)
            {
                fprintf(stderr, "hstrade_qry_trade return code error code:%d, msg:%s\n",
                    lpBizMessageRecv->GetReturnCode(), lpBizMessageRecv->GetErrorInfo());
            }
            else
            {
                int len = 0;
                const void * lpBuffer = lpBizMessageRecv->GetContent(len);
                IF2UnPacker * lpUnPacker = NewUnPacker((void *)lpBuffer, len);
                lpUnPacker->AddRef();

                if (hstd->debug_mode)
                {
                    IF2UnPacker* lpUnPacker2 = NewUnPacker((void*)lpBuffer, len);
                    lpUnPacker2->AddRef();
                    ShowPacket(lpUnPacker2);
                    lpUnPacker2->Release();
                }

                TradeCallback::UnpackBizMessage(lpUnPacker, 0, hstd, TradeCallback::UnpackQryTradeData);
                lpUnPacker->Release();
            }

        }
    }

EXIT:
    // �ͷ�pack��������ڴ�
    if (lpPacker)
    {
        lpPacker->FreeMem(lpPacker->GetPackBuf());
        lpPacker->Release();
    }
    if (lpBizMessage)
    {
        lpBizMessage->Release();
    }

    return rv;
}

int HS_TRADE_STDCALL hstrade_qry_order(hstrade_t* hstd, HSReqQueryField* qry_field)
{
    hstrade_apidata_t*      apidata;
    CConnectionInterface*   conn;
    int rv;

    apidata = &hstd->apidata;
    conn = hstd->conn;

    // ��ȡ�汾Ϊ2���͵�pack�����
    IF2Packer* lpPacker = _hstrade_make_if2pakcer(hstd);

    // ���ð�����
    IBizMessage* lpBizMessage = _hstrade_make_bizmsg(hstd, UFX_FUNC_QRY_ORDER);
    lpBizMessage->SetContent(lpPacker->GetPackBuf(), lpPacker->GetPackLen());

    // ����ҵ������
    rv = conn->SendBizMsg(lpBizMessage, hstd->is_async);
    if (!hstrade_is_async(hstd))
    {
        // ͬ��ģʽ
        if (rv < 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_qry_order failed, rv:%d, err:%s!\n", rv, conn->GetErrorMsg(rv));
            rv = -2;
            goto EXIT;
        }

        IBizMessage* lpBizMessageRecv = NULL;
        rv = conn->RecvBizMsg(rv, &lpBizMessageRecv, hstd->timeoutms);
        if (rv != 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_qry_order recvmsg failed, rv:%d, err:%s!\n", rv, conn->GetErrorMsg(rv));
            goto EXIT;
        }
        else
        {
            int iReturnCode = lpBizMessageRecv->GetReturnCode();
            if (iReturnCode != 0)
            {
                HSDebugInfo(HSDbgFd, "hstrade_qry_order return code error code:%d, msg:%s\n",
                    lpBizMessageRecv->GetReturnCode(), lpBizMessageRecv->GetErrorInfo());
                rv = iReturnCode;
            }
            else
            {
                int len = 0;
                const void * lpBuffer = lpBizMessageRecv->GetContent(len);
                IF2UnPacker * lpUnPacker = NewUnPacker((void *)lpBuffer, len);
                lpUnPacker->AddRef();

                if (hstd->debug_mode)
                {
                    IF2UnPacker* lpUnPacker2 = NewUnPacker((void*)lpBuffer, len);
                    lpUnPacker2->AddRef();
                    ShowPacket(lpUnPacker2);
                    lpUnPacker2->Release();
                }

                TradeCallback::UnpackBizMessage(lpUnPacker, 0, hstd, TradeCallback::UnpackQryOrderData);
                lpUnPacker->Release();
            }

        }
    }

EXIT:
    // �ͷ�pack��������ڴ�
    if (lpPacker)
    {
        lpPacker->FreeMem(lpPacker->GetPackBuf());
        lpPacker->Release();
    }
    if (lpBizMessage)
    {
        lpBizMessage->Release();
    }

    return rv;
}

int HS_TRADE_STDCALL hstrade_qry_security_info(hstrade_t* hstd, HSReqQueryField* qry_field)
{
    return 0;
}

int HS_TRADE_STDCALL hstrade_qry_md(hstrade_t* hstd, HSReqQueryField* qry_field)
{
    hstrade_apidata_t*      apidata;
    CConnectionInterface*   conn;
    int rv;

    apidata = &hstd->apidata;
    conn = hstd->conn;

    // ��ȡ�汾Ϊ2���͵�pack�����
    
    // ��ȡ�汾Ϊ2���͵�pack�����
    IF2Packer* lpPacker = NewPacker(2);
    if (!lpPacker)
    {
        HSDebugInfo(HSDbgFd, "hstrade_qry_md NewPacker() failed!\n");
        return -1;
    }

    lpPacker->AddRef();
    lpPacker->BeginPack();

    //�����ֶ���
    lpPacker->AddField("exchange_type", HS_FT_Str);
    lpPacker->AddField("stock_code", HS_FT_Str);

    //�����ֶ�ֵ
    lpPacker->AddStr(qry_field->exchange_type);
    lpPacker->AddStr(qry_field->stock_code);

    // �������
    lpPacker->EndPack();

    // ���ð�����
    IBizMessage* lpBizMessage = _hstrade_make_bizmsg(hstd, UFX_FUNC_QRY_SECMD);
    lpBizMessage->SetContent(lpPacker->GetPackBuf(), lpPacker->GetPackLen());

    // ����ҵ������
    rv = conn->SendBizMsg(lpBizMessage, hstd->is_async);
    if (!hstrade_is_async(hstd))
    {
        // ͬ��ģʽ
        if (rv < 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_qry_md failed, rv:%d, err:%s!\n", rv, conn->GetErrorMsg(rv));
            rv = -2;
            goto EXIT;
        }

        IBizMessage* lpBizMessageRecv = NULL;
        rv = conn->RecvBizMsg(rv, &lpBizMessageRecv, hstd->timeoutms);
        if (rv != 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_qry_md recvmsg failed, rv:%d, err:%s!\n", rv, conn->GetErrorMsg(rv));
            goto EXIT;
        }
        else
        {
            int iReturnCode = lpBizMessageRecv->GetReturnCode();
            if (iReturnCode != 0)
            {
                HSDebugInfo(HSDbgFd, "hstrade_qry_md return code error code:%d, msg:%s\n",
                    lpBizMessageRecv->GetReturnCode(), lpBizMessageRecv->GetErrorInfo());
                rv = iReturnCode;
            }
            else
            {
                int len = 0;
                const void * lpBuffer = lpBizMessageRecv->GetContent(len);
                IF2UnPacker * lpUnPacker = NewUnPacker((void *)lpBuffer, len);
                lpUnPacker->AddRef();

                if (hstd->debug_mode)
                {
                    IF2UnPacker* lpUnPacker2 = NewUnPacker((void*)lpBuffer, len);
                    lpUnPacker2->AddRef();
                    ShowPacket(lpUnPacker2);
                    lpUnPacker2->Release();
                }

                TradeCallback::UnpackBizMessage(lpUnPacker, 0, hstd, TradeCallback::UnpackQryOrderData);
                lpUnPacker->Release();
            }

        }
    }

EXIT:
    // �ͷ�pack��������ڴ�
    if (lpPacker)
    {
        lpPacker->FreeMem(lpPacker->GetPackBuf());
        lpPacker->Release();
    }
    if (lpBizMessage)
    {
        lpBizMessage->Release();
    }

    return rv;
}

int HS_TRADE_STDCALL hstrade_qry_cash_hist(hstrade_t* hstd, HSReqQueryField* qry_field)
{
    return 0;
}

int HS_TRADE_STDCALL hstrade_qry_trade_hist(hstrade_t* hstd, HSReqQueryField* qry_field)
{
    return 0;
}

int HS_TRADE_STDCALL hstrade_qry_order_hist(hstrade_t* hstd, HSReqQueryField* qry_field)
{
    return 0;
}


//////////////////////////////////////////////////////////////////////////
void* HS_TRADE_STDCALL hstrade_begin_pack(hstrade_t* hstd)
{
    IF2Packer* lpPacker;
    lpPacker = NewPacker(2);
    if (!lpPacker)
        return NULL;

    lpPacker->AddRef();
    lpPacker->BeginPack();

    return lpPacker;
}

int HS_TRADE_STDCALL hstrade_end_pack(hstrade_t* hstd, void* packer)
{
    IF2Packer* lpPacker;
    lpPacker = (IF2Packer*)packer;
    lpPacker->EndPack();
    return 0;
}

int HS_TRADE_STDCALL hstrade_release_pack(hstrade_t* hstd, void* packer)
{
    IF2Packer* lpPacker;
    lpPacker = (IF2Packer*)packer;
    lpPacker->FreeMem(lpPacker->GetPackBuf());
    lpPacker->Release();
    return 0;
}

int HS_TRADE_STDCALL hstrade_send_bizmsg(hstrade_t* hstd, void* packer, int func_id)
{
    int rv;
    IF2Packer* lpPacker;
    CConnectionInterface* conn;

    rv = 0;
    lpPacker = (IF2Packer*)packer;
    conn = (CConnectionInterface*)hstd->conn;

    IBizMessage* lpBizMessage;
    lpBizMessage = NewBizMessage();
    lpBizMessage->AddRef();
    lpBizMessage->SetFunction(func_id);
    lpBizMessage->SetPacketType(REQUEST_PACKET);

    lpBizMessage->SetContent(lpPacker->GetPackBuf(), lpPacker->GetPackBufSize());

    // ��������, FIXME: maybe we should not support sync mode here.
    rv = conn->SendBizMsg(lpBizMessage, hstd->is_async);
    if (!hstrade_is_async(hstd))
    {
        // ͬ��ģʽ
        if (rv < 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_send_bizmsg failed, rv:%d, err:%s!\n", rv, conn->GetErrorMsg(rv));
            goto EXIT;
        }

        IBizMessage* lpBizMessageRecv = NULL;
        rv = conn->RecvBizMsg(rv, &lpBizMessageRecv, hstd->timeoutms);
        if (rv != 0)
        {
            HSDebugInfo(HSDbgFd, "hstrade_send_bizmsg recvmsg failed, rv:%d, err:%s!\n", rv, conn->GetErrorMsg(rv));
            goto EXIT;
        }
        else
        {
            int iReturnCode = lpBizMessageRecv->GetReturnCode();
            if (iReturnCode != 0)
            {
                HSDebugInfo(HSDbgFd, "hstrade_send_bizmsg return code error code:%d, msg:%s\n",
                    lpBizMessageRecv->GetReturnCode(), lpBizMessageRecv->GetErrorInfo());
                rv = iReturnCode;
            }
            else
            {
                int len = 0;
                const void * lpBuffer = lpBizMessageRecv->GetContent(len);
                IF2UnPacker * lpUnPacker = NewUnPacker((void *)lpBuffer, len);
                lpUnPacker->AddRef();

                if (hstd->debug_mode)
                {
                    IF2UnPacker* lpUnPacker2 = NewUnPacker((void*)lpBuffer, len);
                    lpUnPacker2->AddRef();
                    ShowPacket(lpUnPacker2);
                    lpUnPacker2->Release();
                }

                // TradeCallback::UnpackBizMessage(lpUnPacker, hstd, TradeCallback::UnpackXXX);
                lpUnPacker->Release();
            }

        }
    }

EXIT:
    if (lpPacker)
    {
        lpPacker->FreeMem(lpPacker->GetPackBuf());
        lpPacker->Release();
    }
    if (lpBizMessage)
    {
        lpBizMessage->Release();
    }
    return rv;
}

/// add pack fields
int HS_TRADE_STDCALL hstrade_add_field(hstrade_t* hstd, void* packer, const char* key, char field_type, int field_width, int field_scale)
{
    IF2Packer* lpPacker;
    lpPacker = (IF2Packer*)packer;
    return lpPacker->AddField(key, field_type, field_width, field_scale);
}


/// add pack values
int HS_TRADE_STDCALL hstrade_add_char(hstrade_t* hstd, void* packer, const char value)
{
    IF2Packer* lpPacker;
    lpPacker = (IF2Packer*)packer;
    lpPacker->AddChar(value);
    return 0;
}

int HS_TRADE_STDCALL hstrade_add_str(hstrade_t* hstd, void* packer, const char* value)
{
    IF2Packer* lpPacker;
    lpPacker = (IF2Packer*)packer;
    lpPacker->AddStr(value);
    return 0;
}

int HS_TRADE_STDCALL hstrade_add_int(hstrade_t* hstd, void* packer, const int value)
{
    IF2Packer* lpPacker;
    lpPacker = (IF2Packer*)packer;
    lpPacker->AddInt(value);
    return 0;
}

int HS_TRADE_STDCALL hstrade_add_double(hstrade_t* hstd, void* packer, const double value)
{
    IF2Packer* lpPacker;
    lpPacker = (IF2Packer*)packer;
    lpPacker->AddDouble(value);
    return 0;
}

int HS_TRADE_STDCALL hstrade_add_raw(hstrade_t* hstd, void* packer, void* buf, const int len)
{
    IF2Packer* lpPacker;
    lpPacker = (IF2Packer*)packer;
    lpPacker->AddRaw(buf, len);
    return 0;
}

int HS_TRADE_STDCALL hstrade_recv_msg(hstrade_t* hstd, int hsend, int timeoutms, void** ppmsg)
{
    IBizMessage* lpBizMessageRecv = NULL;
    int rv = hstd->conn->RecvBizMsg(hsend, &lpBizMessageRecv, timeoutms);

    if (rv == 0)
    {
        *ppmsg = lpBizMessageRecv;
    }

    return rv;
}

int HS_TRADE_STDCALL hstrade_show_msg(void* pmsg)
{
    if (!pmsg)
    {
        return 0;
    }

    IBizMessage* lpBizMessageRecv = NULL;
    lpBizMessageRecv = (IBizMessage*)pmsg;
    int func_id = lpBizMessageRecv->GetFunction();

    if (lpBizMessageRecv->GetReturnCode() != 0)
    {
        fprintf(stderr, "function_id: %d\n", func_id);
        fprintf(stderr, "issue_type: %d\n", lpBizMessageRecv->GetIssueType());
        fprintf(stderr, "return_code: %d\n", lpBizMessageRecv->GetReturnCode());
        fprintf(stderr, "error_no: %d\n", lpBizMessageRecv->GetErrorNo());
        fprintf(stderr, "error_info: %s\n", lpBizMessageRecv->GetErrorInfo());
        return 0;
    }

    int len;
    const void* lpBuffer;
    if (func_id >= UFX_FUNC_SUBSCRIBE && func_id <= UFX_FUNC_RTN_DATA)
    {
        lpBuffer = lpBizMessageRecv->GetKeyInfo(len);
    }
    else
    {
        lpBuffer = lpBizMessageRecv->GetContent(len);
    }

    IF2UnPacker* lpUnpacker = NewUnPacker((void*)lpBuffer, len);
    if (!lpUnpacker)
    {
        fprintf(stderr, "empty packet for function_id:%d\n", func_id);
    }
    else
    {
        lpUnpacker->AddRef();
        ShowPacket(lpUnpacker);
        lpUnpacker->Release();
    }

    return 0;
}
