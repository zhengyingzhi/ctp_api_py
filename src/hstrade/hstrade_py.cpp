#include <iostream>

#include <t2sdk_interface.h>

#include "cJSON.h"

#include "hstrade_callback.h"
#include "hstrade_py.h"


#ifdef HAVE_WRPPAER_PYTHON

#define extract_json_str(json, d, k)     \
    do { \
        item = cJSON_GetObjectItem((json), (k)); \
        if (item) { \
            strncpy((d), item->valuestring, sizeof((d))); \
        } \
    } while (0);

#define extract_json_int(json, d, k)     \
    do { \
        item = cJSON_GetObjectItem((json), (k)); \
        if (item) { \
            (d) = item->valueint; \
        } \
    } while (0);

#define extract_json_double(json, d, k)     \
    do { \
        item = cJSON_GetObjectItem((json), (k)); \
        if (item) { \
            (d) = item->valuedouble; \
        } \
    } while (0);

#define extract_json_char(json, d, k)     \
    do { \
        item = cJSON_GetObjectItem((json), (k)); \
        if (item) { \
            (d) = item->valuestring[0]; \
        } \
    } while (0);


//从字典中获取某个建值对应的整数，并赋值到请求结构体对象的值上
void getInt(const dict &d, const char *key, int *value)
{
    if (d.contains(key))        //检查字典中是否存在该键值
    {
        object o = d[key];      //获取该键值
        *value = o.cast<int>();
    }
}


//从字典中获取某个建值对应的浮点数，并赋值到请求结构体对象的值上
void getDouble(const dict &d, const char *key, double *value)
{
    if (d.contains(key))
    {
        object o = d[key];
        *value = o.cast<double>();
    }
}


//从字典中获取某个建值对应的字符，并赋值到请求结构体对象的值上
void getChar(const dict &d, const char *key, char *value)
{
    if (d.contains(key))
    {
        object o = d[key];
        *value = o.cast<char>();
    }
}


template <uint32_t size>
using string_literal = char[size];

//从字典中获取某个建值对应的字符串，并赋值到请求结构体对象的值上
template <uint32_t size>
void getString(const pybind11::dict &d, const char *key, string_literal<size> &value)
{
    if (d.contains(key))
    {
        object o = d[key];
        std::string s = o.cast<std::string>();
        const char *buf = s.c_str();
        strcpy(value, buf);
    }
}

extern void ShowPacket(IF2UnPacker *lpUnPacker);

// 从原始api获取数据的回调

static void _on_connected_static(hstrade_t* hstd)
{
    // fprintf(stderr, "<_on_connected_static>\n");

    HSTdApi* obj;
    obj = (HSTdApi*)hstrade_get_userdata(hstd);
    if (!obj)
        return;

    gil_scoped_acquire acquire;
    obj->on_front_connected();
}

static void _on_disconnected_static(hstrade_t* hstd, int reason)
{
    // fprintf(stderr, "<_on_disconnected_static>\n");

    HSTdApi* obj;
    obj = (HSTdApi*)hstrade_get_userdata(hstd);
    if (!obj)
        return;

    gil_scoped_acquire acquire;
    obj->on_front_disconnected(reason);
}

static void _on_msg_error_static(hstrade_t* hstd, int func_id, int issue_type, int error_no, const char* error_info)
{
    // fprintf(stderr, "<_on_msg_error_static>\n");

    HSTdApi* obj;
    obj = (HSTdApi*)hstrade_get_userdata(hstd);
    if (!obj)
        return;

    gil_scoped_acquire acquire;
    obj->on_msg_error(func_id, issue_type, error_no, std::string(error_info));
}

static void _on_recv_msg_static(hstrade_t* hstd, int func_id, int issue_type, const char* msg)
{
    // fprintf(stderr, "#<_on_recv_msg_static> func_id:%d, issue_type:%d\n", func_id, issue_type);
    // fprintf(stderr, msg);

    HSTdApi* obj;
    obj = (HSTdApi*)hstrade_get_userdata(hstd);
    if (!obj)
        return;

    gil_scoped_acquire acquire;
    obj->on_recv_msg(func_id, issue_type, std::string(msg));
}


HSTdApi::HSTdApi()
{
    m_packer = NULL;
    m_hstd = NULL;

    memset(&m_spi, 0, sizeof(m_spi));
    m_spi.on_connected = _on_connected_static;
    m_spi.on_disconnected = _on_disconnected_static;
    m_spi.on_msg_error = _on_msg_error_static;
    m_spi.on_json_msg = _on_recv_msg_static;

    m_hsend = 0;
    m_last_error_code = 0;
    m_async_mode = 0;
    m_data_proto = 0;

    m_sequece_no = 0;
}

HSTdApi::~HSTdApi()
{
    if (m_packer)
    {
        m_packer->FreeMem(m_packer->GetPackBuf());
        m_packer->Release();
        m_packer = NULL;
    }

    if (m_hstd)
    {
        hstrade_realese(m_hstd);
        m_hstd = NULL;
    }
}

int HSTdApi::get_api_version()
{
    int ver = 0;
    hstrade_version(&ver);
    return ver;
}

std::string HSTdApi::get_hstrade_version()
{
    return std::string(hstrade_version(NULL));
}

std::string HSTdApi::recv_msg(int hsend)
{
    int rv;
    hstrade_t* hstd = m_hstd;
    IBizMessage* lpBizMessageRecv = NULL;

    rv = hstd->conn->RecvBizMsg(hsend, &lpBizMessageRecv, hstd->timeoutms);
    if (rv != 0)
    {
        fprintf(stderr, "recv_msg() recv failed, rv:%d, err:%s!\n", rv, hstd->conn->GetErrorMsg(rv));
        m_last_error_code = rv;
    }
    else
    {
        int iReturnCode = lpBizMessageRecv->GetReturnCode();
        if (iReturnCode != 0)
        {
            fprintf(stderr, "hstrade_order_insert return code error code:%d, msg:%s\n",
                lpBizMessageRecv->GetReturnCode(), lpBizMessageRecv->GetErrorInfo());
        }
        else
        {
            int func_id = lpBizMessageRecv->GetFunction();
            int len = 0;
            const void* lpBuffer;
            if (func_id >= UFX_FUNC_SUBSCRIBE && func_id <= UFX_FUNC_RTN_DATA)
                lpBuffer = lpBizMessageRecv->GetKeyInfo(len);
            else
                lpBuffer = lpBizMessageRecv->GetContent(len);

            if (hstd->debug_mode)
            {
                IF2UnPacker* lpUnPacker2 = NewUnPacker((void *)lpBuffer, len);
                lpUnPacker2->AddRef();
                ShowPacket(lpUnPacker2);
                lpUnPacker2->Release();
            }

            IF2UnPacker* lpUnPacker = NewUnPacker((void *)lpBuffer, len);
            if (lpUnPacker)
            {
                lpUnPacker->AddRef();
                // make json msg
                cJSON* json = TradeCallback::GenJsonDatas(lpUnPacker, lpBizMessageRecv->GetFunction(), lpBizMessageRecv->GetIssueType());
                char* json_str = "";
                if (json)
                {
                    json_str = cJSON_Print(json);
                }
                std::string ret_msg(json_str);
                cJSON_free(json_str);
                cJSON_Delete(json);

                lpUnPacker->Release();
                return ret_msg;
            }
            else
            {
                fprintf(stderr, "empty packet!\n");
            }
        }
    }

    return "";
}

void HSTdApi::create_api(int async_mode, int data_proto)
{
    m_async_mode = async_mode;
    m_data_proto = data_proto;

    m_hstd = hstrade_create(async_mode);

    hstrade_set_userdata(m_hstd, this);
    hstrade_register_spi(m_hstd, &m_spi);

    if (data_proto == HS_DATA_PROTO_JSON)
    {
        // json mode
        // fprintf(stderr, "HSTdApi set json mode!!!\n");
        m_hstd->callback->SetJsonMode(data_proto);
    }
}

void HSTdApi::release()
{
    if (m_hstd)
    {
        hstrade_realese(m_hstd);
        m_hstd = NULL;
    }
}

void HSTdApi::set_debug_mode(int level)
{
    if (m_hstd)
    {
        hstrade_debug_mode(m_hstd, level);
    }
}

void HSTdApi::set_timeout(int timeoutms)
{
    if (m_hstd)
    {
        hstrade_set_timeout(m_hstd, timeoutms);
    }
}

int HSTdApi::config_load(std::string config_file)
{
    return hstrade_config_load(m_hstd, config_file.c_str());
}

int HSTdApi::config_set_string(std::string section, std::string entry_name, std::string value)
{
    return hstrade_config_set_string(m_hstd, section.c_str(), entry_name.c_str(), value.c_str());
}

int HSTdApi::config_set_int(std::string section, std::string entry_name, int value)
{
    return hstrade_config_set_int(m_hstd, section.c_str(), entry_name.c_str(), value);
}

std::string HSTdApi::config_get_string(std::string section, std::string entry_name, std::string default_value)
{
    const char* ret_value = hstrade_config_get_string(m_hstd, section.c_str(), entry_name.c_str(), default_value.c_str());
    if (!ret_value)
        ret_value = "";
    return std::string(ret_value);
}

int HSTdApi::config_get_int(std::string section, std::string entry_name, int default_value)
{
    return hstrade_config_get_int(m_hstd, section.c_str(), entry_name.c_str(), default_value);
}

int HSTdApi::connect(int timeoutms)
{
    int rv;
    rv = hstrade_init(m_hstd, timeoutms);
    return rv;
}

int HSTdApi::set_sequece_no(int sequence_no)
{
    m_sequece_no = sequence_no;
}

int HSTdApi::set_json_value(const std::string& json_str)
{
    m_json_str = json_str;
    return 0;
}

int HSTdApi::send_pack_msg(int func_id, int subsystem_no, int branch_no)
{
    int rv;

    IF2Packer* lpPacker;
    lpPacker = (IF2Packer*)m_packer;

    IBizMessage* lpBizMessage;
    lpBizMessage = NewBizMessage();
    lpBizMessage->AddRef();

    lpBizMessage->SetFunction(func_id);
    lpBizMessage->SetPacketType(REQUEST_PACKET);
    if (m_sequece_no)
        lpBizMessage->SetSequeceNo(m_sequece_no);

    lpBizMessage->SetBranchNo(branch_no);
    lpBizMessage->SetSystemNo(subsystem_no);
    // lpBizMessage->SetCompanyID(91000);
    // lpBizMessage->SetSenderCompanyID(91000);

    // 设置打包数据
    if (func_id == UFX_FUNC_SUBSCRIBE)
        lpBizMessage->SetKeyInfo(lpPacker->GetPackBuf(), lpPacker->GetPackLen());
    else
        lpBizMessage->SetContent(lpPacker->GetPackBuf(), lpPacker->GetPackLen());

    // 发送消息
    rv = m_hstd->conn->SendBizMsg(lpBizMessage, m_hstd->is_async);
    // fprintf(stderr, "send_pack_msg(%d) async:%d rv:%d\n", func_id, m_hstd->is_async, rv);

    // 释放内存
    lpPacker->FreeMem(lpPacker->GetPackBuf());
    lpPacker->Release();

    // set as NULL
    m_packer = NULL;

    lpBizMessage->Release();

    m_hsend = rv;
    return rv;
}

int HSTdApi::send_msg(int func_id, int subsystem_no, int branch_no)
{
    m_hsend = 0;
    m_last_error_code = 0;

    if (m_packer)
    {
        return send_pack_msg(func_id, subsystem_no, branch_no);
    }
    else if (m_json_str.empty())
    {
        return -1;
    }

    // fprintf(stderr, "## send_msg json:%s\n", m_json_str.c_str());

    int rv = -1;
    cJSON* item;
    cJSON* json;
    json = cJSON_Parse(m_json_str.c_str());
    if (!json)
    {
        fprintf(stderr, "send_msg(%d) but error json string\n", func_id);
        return rv;
    }

    if (func_id == UFX_FUNC_LOGIN)
    {
        HSReqUserLoginField login_req = { 0 };
        extract_json_str(json, login_req.client_id, "client_id");
        extract_json_str(json, login_req.password, "password");

        char password_type[2] = "";
        extract_json_str(json, password_type, "password_type");
        if (!password_type[0])
            password_type[0] = HS_PWD_TYPE_TRADE;
        login_req.password_type = password_type[0];
        rv = hstrade_user_login(m_hstd, &login_req);
    }
    else if (func_id == UFX_FUNC_PLACE_ORDER)
    {
        HSReqOrderInsertField order_req = { 0 };

        // item = cJSON_GetObjectItem(json, "entrust_amount");
        // if (item) {
        //     order_req.entrust_amount = item->valueint;
        // }

        extract_json_str(json, order_req.stock_code, "stock_code");
        extract_json_str(json, order_req.exchange_type, "exchange_type");
        extract_json_int(json, order_req.entrust_amount, "entrust_amount");
        extract_json_double(json, order_req.entrust_price, "entrust_price");
        extract_json_str(json, order_req.entrust_prop, "entrust_prop");
        extract_json_int(json, order_req.batch_no, "batch_no");

        extract_json_char(json, order_req.entrust_bs, "entrust_bs");

        // maybe option order field
        extract_json_char(json, order_req.entrust_oc, "entrust_oc");
        extract_json_char(json, order_req.covered_flag, "covered_flag");

        rv = hstrade_order_insert(m_hstd, &order_req);
    }
    else if (func_id == UFX_FUNC_CANCEL_ORDER)
    {
        HSReqOrderActionField order_action = { 0 };
        extract_json_str(json, order_action.entrust_no, "entrust_no");
        rv = hstrade_order_action(m_hstd, &order_action);
    }
    else if (func_id == UFX_FUNC_QRY_CASH || func_id == UFX_FUNC_QRY_POSITION ||
        func_id == UFX_FUNC_QRY_ORDER || func_id == UFX_FUNC_QRY_TRADE)
    {
        HSReqQueryField qry_field = { 0 };
        if (func_id == UFX_FUNC_QRY_CASH)
        {
            rv = hstrade_qry_trading_account(m_hstd, &qry_field);
        }
        else if (func_id == UFX_FUNC_QRY_POSITION)
        {
            rv = hstrade_qry_position(m_hstd, &qry_field);
        }
        else if (func_id == UFX_FUNC_QRY_TRADE)
        {
            rv = hstrade_qry_trade(m_hstd, &qry_field);
        }
        else if (func_id == UFX_FUNC_QRY_ORDER)
        {
            rv = hstrade_qry_order(m_hstd, &qry_field);
        }
    }
    else if (func_id == UFX_FUNC_QRY_SECINFO)
    {
        HSReqQueryField qry_field = { 0 };
        extract_json_str(json, qry_field.exchange_type, "exchange_type");
        extract_json_str(json, qry_field.stock_code, "stock_code");
        rv = hstrade_qry_security_info(m_hstd, &qry_field);
    }
    else if (func_id == UFX_FUNC_QRY_SECMD)
    {
        HSReqQueryField qry_field = { 0 };
        extract_json_str(json, qry_field.exchange_type, "exchange_type");
        extract_json_str(json, qry_field.stock_code, "stock_code");
        rv = hstrade_qry_md(m_hstd, &qry_field);
    }
    else if (func_id == UFX_FUNC_SUBSCRIBE)
    {
        int issue_type = 0;
        extract_json_int(json, issue_type, "issue_type");
        rv = hstrade_subscribe(m_hstd, issue_type);
    }
    else
    {
        fprintf(stderr, "send_msg(%d) unsupport func_id yet!\n", func_id);
    }

    // free the json data
    if (json)
    {
        cJSON_Delete(json);
    }

    m_json_str.clear();

    return rv;
}

void HSTdApi::begin_pack(void)
{
    m_last_error_code = 0;

    IF2Packer* lpPacker;
    lpPacker = (IF2Packer*)m_packer;
    if (lpPacker)
    {
        lpPacker->FreeMem(lpPacker->GetPackBuf());
        lpPacker->Release();
    }

    lpPacker = NewPacker(2);
    lpPacker->AddRef();
    lpPacker->BeginPack();
    m_packer = lpPacker;
}

void HSTdApi::end_pack(void)
{
    if (m_packer)
    {
        m_packer->EndPack();
    }
}

int HSTdApi::add_field(const std::string& key, const std::string& field_type, int field_width, int field_scale)
{
    if (!m_packer)
    {
        fprintf(stderr, "add_field not begin_pack() yet!\n");
        return -1;
    }

    if (field_type.length() != 1)
    {
        fprintf(stderr, "add_field unknown field_type:%s\n", field_type.c_str());
        return -2;
    }

    if (field_width == 0)
    {
        field_width = 255;
    }

    char ft = field_type[0];

    // fprintf(stderr, "add_field key:%s, field_type:%c, field_width:%d\n", key.c_str(), ft, field_width);
    return m_packer->AddField(key.c_str(), ft, field_width, field_scale);
}

int HSTdApi::add_char(const std::string& value)
{
    if (m_packer)
    {
        // fprintf(stderr, "add_char value:%s\n", value.c_str());
        if (!value.empty())
            return m_packer->AddChar(value[0]);
        else
            return m_packer->AddChar(0);
    }
    return -1;
}

int HSTdApi::add_str(const std::string& value)
{
    if (m_packer)
    {
        // fprintf(stderr, "add_str value:%s\n", value.c_str());
        return m_packer->AddStr(value.c_str());
    }
    return -1;
}

int HSTdApi::add_int(const int value)
{
    if (m_packer)
    {
        // fprintf(stderr, "add_int value:%d\n", value);
        return m_packer->AddInt(value);
    }
    return -1;
}

int HSTdApi::add_double(const double value)
{
    if (m_packer)
    {
        // fprintf(stderr, "add_double value:%f\n", value);
        return m_packer->AddDouble(value);
    }
    return -1;
}


int HSTdApi::req_login(const dict& req)
{
    HSReqUserLoginField myreq = { 0 };
    getString(req, "client_id", myreq.client_id);
    getString(req, "password", myreq.password);
    getChar(req, "password_type", &myreq.password_type);
    getString(req, "mac_address", myreq.mac_address);
    getString(req, "ip_address", myreq.ip_address);

    return hstrade_user_login(m_hstd, &myreq);
}

int HSTdApi::req_qry_account_info(const dict& req)
{
    // 
    fprintf(stderr, "req_qry_account_info not impl yet!\n");
    return -1;
}

int HSTdApi::req_qry_cash(const dict& req)
{
    HSReqQueryField myreq = { 0 };
    getInt(req, "branch_no", &myreq.branch_no);
    getString(req, "client_id", myreq.client_id);
    getString(req, "exchange_type", myreq.exchange_type);
    getString(req, "stock_code", myreq.stock_code);
    return hstrade_qry_trading_account(m_hstd, &myreq);
}

int HSTdApi::req_qry_position(const dict& req)
{
    HSReqQueryField myreq = { 0 };
    getInt(req, "branch_no", &myreq.branch_no);
    getString(req, "client_id", myreq.client_id);
    getString(req, "exchange_type", myreq.exchange_type);
    getString(req, "stock_code", myreq.stock_code);
    return hstrade_qry_position(m_hstd, &myreq);
}

int HSTdApi::req_qry_order(const dict& req)
{
    HSReqQueryField myreq = { 0 };
    getInt(req, "branch_no", &myreq.branch_no);
    getString(req, "client_id", myreq.client_id);
    getString(req, "exchange_type", myreq.exchange_type);
    getString(req, "stock_code", myreq.stock_code);
    return hstrade_qry_order(m_hstd, &myreq);
}

int HSTdApi::req_qry_trade(const dict& req)
{
    HSReqQueryField myreq = { 0 };
    getInt(req, "branch_no", &myreq.branch_no);
    getString(req, "client_id", myreq.client_id);
    getString(req, "exchange_type", myreq.exchange_type);
    getString(req, "stock_code", myreq.stock_code);
    return hstrade_qry_trade(m_hstd, &myreq);
}

int HSTdApi::req_qry_md(const std::string& exchange_type, const std::string& stock_code)
{
    HSReqQueryField myreq = { 0 };
    strncpy(myreq.exchange_type, exchange_type.c_str(), sizeof(myreq.exchange_type) - 1);
    strncpy(myreq.stock_code, stock_code.c_str(), sizeof(myreq.stock_code) - 1);
    return hstrade_qry_md(m_hstd, &myreq);
}

int HSTdApi::req_order_insert(const dict& order_req)
{
    HSReqOrderInsertField myreq = { 0 };
    getInt(order_req, "branch_no", &myreq.branch_no);
    getString(order_req, "client_id", myreq.client_id);
    getString(order_req, "exchange_type", myreq.exchange_type);
    getString(order_req, "stock_code", myreq.stock_code);
    getString(order_req, "entrust_prop", myreq.entrust_prop);
    getChar(order_req, "entrust_bs", &myreq.entrust_bs);
    getChar(order_req, "entrust_oc", &myreq.entrust_oc);
    getChar(order_req, "covered_flag", &myreq.covered_flag);
    getInt(order_req, "entrust_amount", &myreq.entrust_amount);
    getDouble(order_req, "entrust_price", &myreq.entrust_price);
    getInt(order_req, "batch_no", &myreq.batch_no);

    return hstrade_order_insert(m_hstd, &myreq);
}

int HSTdApi::req_order_action(const dict& order_action)
{
    HSReqOrderActionField myreq = { 0 };
    getInt(order_action, "branch_no", &myreq.branch_no);
    getString(order_action, "client_id", myreq.client_id);
    getString(order_action, "exchange_type", myreq.exchange_type);
    getString(order_action, "stock_code", myreq.stock_code);

    int entrust_no = 0;
    getInt(order_action, "entrust_no", &entrust_no);
    if (entrust_no != 0)
        sprintf(myreq.entrust_no, "%d", entrust_no);
    else
        getString(order_action, "entrust_no", myreq.entrust_no);

    return hstrade_order_action(m_hstd, &myreq);
}

//////////////////////////////////////////////////////////////////////////

class PyHSTdApi : public HSTdApi
{
public:
    using HSTdApi::HSTdApi;

    void on_front_connected() override
    {
        try
        {
            PYBIND11_OVERLOAD(void, HSTdApi, on_front_connected);
        }
        catch (const error_already_set &e)
        {
            std::cout << "on_front_connected: " << e.what() << std::endl;
        }
    };

    void on_front_disconnected(int reqid) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, HSTdApi, on_front_disconnected, reqid);
        }
        catch (const error_already_set &e)
        {
            std::cout << "on_front_disconnected: " << e.what() << std::endl;
        }
    };

    void on_msg_error(int func_id, int issue_type, int error_no, const std::string& error_info) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, HSTdApi, on_msg_error, func_id, issue_type, error_no, error_info);
        }
        catch (const error_already_set &e)
        {
            std::cout << "on_msg_error: " << e.what() << std::endl;
        }
    }

    void on_recv_msg(int func_id, int issue_type, const std::string& msg) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, HSTdApi, on_recv_msg, func_id, issue_type, msg);
        }
        catch (const error_already_set &e)
        {
            std::cout << "on_recv_msg: " << e.what() << std::endl;
        }
    }


};


PYBIND11_MODULE(hstrade, m)
{
    class_<HSTdApi, PyHSTdApi> tdapi(m, "HSTdApi", module_local());
    tdapi
        .def(init<>())
        .def("create_api", &HSTdApi::create_api)
        .def("release", &HSTdApi::release)
        .def("connect", &HSTdApi::connect)
        .def("config_load", &HSTdApi::config_load)
        .def("config_set_string", &HSTdApi::config_set_string)
        .def("config_set_int", &HSTdApi::config_set_int)
        .def("config_get_string", &HSTdApi::config_get_string)
        .def("config_get_int", &HSTdApi::config_get_int)
        .def("set_debug_mode", &HSTdApi::set_debug_mode)
        .def("set_json_value", &HSTdApi::set_json_value)
        .def("send_msg", &HSTdApi::send_msg)
        .def("recv_msg", &HSTdApi::recv_msg)

        .def("begin_pack", &HSTdApi::begin_pack)
        .def("end_pack", &HSTdApi::end_pack)
        .def("add_field", &HSTdApi::add_field)
        .def("add_char", &HSTdApi::add_char)
        .def("add_str", &HSTdApi::add_str)
        .def("add_int", &HSTdApi::add_int)
        .def("add_double", &HSTdApi::add_double)

        // some raw t2sdk api names
        .def("BeginPack", &HSTdApi::begin_pack)
        .def("EndPack", &HSTdApi::end_pack)
        .def("AddField", &HSTdApi::add_field)
        .def("AddChar", &HSTdApi::add_char)
        .def("AddStr", &HSTdApi::add_str)
        .def("AddInt", &HSTdApi::add_int)
        .def("AddDouble", &HSTdApi::add_double)

        .def("req_login", &HSTdApi::req_login)
        .def("req_qry_account_info", &HSTdApi::req_qry_account_info)
        .def("req_qry_cash", &HSTdApi::req_qry_cash)
        .def("req_qry_position", &HSTdApi::req_qry_position)
        .def("req_qry_order", &HSTdApi::req_qry_order)
        .def("req_qry_trade", &HSTdApi::req_qry_trade)
        .def("req_qry_md", &HSTdApi::req_qry_md)
        .def("req_order_insert", &HSTdApi::req_order_insert)
        .def("req_order_action", &HSTdApi::req_order_action)

        .def("on_front_connected", &HSTdApi::on_front_connected)
        .def("on_front_disconnected", &HSTdApi::on_front_disconnected)
        .def("on_msg_error", &HSTdApi::on_msg_error)
        .def("on_recv_msg", &HSTdApi::on_recv_msg)

        .def("get_api_version", &HSTdApi::get_api_version)
        .def("get_hstrade_version", &HSTdApi::get_hstrade_version)
        ;
}



#endif//HAVE_WRPPAER_PYTHON
