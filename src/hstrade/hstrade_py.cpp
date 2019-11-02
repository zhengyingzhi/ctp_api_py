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


// 从原始api获取数据的回调

static void _on_connected_static(hstrade_t* hstd)
{
    fprintf(stderr, "<_on_connected_static>\n");

    HSTdApi* obj;
    obj = (HSTdApi*)hstrade_get_userdata(hstd);

    gil_scoped_acquire acquire;
    obj->on_front_connected();
}

static void _on_disconnected_static(hstrade_t* hstd, int reason)
{
    fprintf(stderr, "<_on_disconnected_static>\n");

    HSTdApi* obj;
    obj = (HSTdApi*)hstrade_get_userdata(hstd);

    gil_scoped_acquire acquire;
    obj->on_front_disconnected(reason);
}

static void _on_recv_msg_static(hstrade_t* hstd, int func_id, int issue_type, const char* msg)
{
    fprintf(stderr, "#<_on_recv_msg_static> func_id:%d, issue_type:%d\n", func_id, issue_type);
    fprintf(stderr, msg);

    HSTdApi* obj;
    obj = (HSTdApi*)hstrade_get_userdata(hstd);

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
    m_spi.on_json_msg = _on_recv_msg_static;

    m_async_mode = 0;
    m_data_proto = 0;
}

HSTdApi::~HSTdApi()
{
    if (m_packer)
    {
        IF2Packer* lpPacker;
        lpPacker = (IF2Packer*)m_packer;
        lpPacker->Release();
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

std::string HSTdApi::recv_msg()
{
    // TODO: 
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
        fprintf(stderr, "HSTdApi set json mode!!!\n");
        m_hstd->callback->SetJsonMode(1);
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

int HSTdApi::connect(std::string server_port, std::string license_file, std::string fund_account, int timeoutms)
{
    int rv;
    rv = hstrade_init(m_hstd, server_port.c_str(), license_file.c_str(), fund_account.c_str(), timeoutms);
    return rv;
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

    lpBizMessage->SetBuff(lpPacker->GetPackBuf(), lpPacker->GetPackLen());

    // 发送消息
    rv = m_hstd->conn->SendBizMsg(lpBizMessage, 1);

    if (lpPacker)
    {
        lpPacker->FreeMem(lpPacker->GetPackBuf());
        lpPacker->Release();

        // set as NULL
        m_packer = NULL;
    }
    if (lpBizMessage)
    {
        lpBizMessage->Release();
    }

    return rv;
}

int HSTdApi::send_msg(int func_id, int subsystem_no, int branch_no)
{
    if (m_packer)
    {
        return send_pack_msg(func_id, subsystem_no, branch_no);
    }
    else if (m_json_str.empty())
    {
        return -1;
    }

    // TODO: convert m_json_str data to HSReqOrderField/HSQueryField/...

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

    return 0;
}

void HSTdApi::begin_pack(void)
{
    if (m_packer)
    {
        IF2Packer* lpPacker;
        lpPacker = (IF2Packer*)m_packer;
        lpPacker->FreeMem(lpPacker->GetPackBuf());
        lpPacker->Release();
    }

    m_packer = NewPacker(2);
}

void HSTdApi::end_pack(void)
{
    if (m_packer)
    {
        IF2Packer* lpPacker;
        lpPacker = (IF2Packer*)m_packer;
        lpPacker->EndPack();
    }
}

int HSTdApi::add_field(const std::string& key, const std::string& field_type, int field_width)
{
    if (!m_packer)
    {
        fprintf(stderr, "add_field not begin_pack() yet!\n");
        return -1;
    }

    if (field_type.length() > 1)
    {
        fprintf(stderr, "add_field unknown field_type:%s\n", field_type.c_str());
        return -2;
    }

    char ft = field_type[0];

    IF2Packer* lpPacker;
    lpPacker = (IF2Packer*)m_packer;
    return lpPacker->AddField(key.c_str(), ft, field_width);
}

int HSTdApi::pack_char(const std::string& value)
{
    if (!m_packer)
    {
        return -1;
    }

    IF2Packer* lpPacker;
    lpPacker = (IF2Packer*)m_packer;
    if (!value.empty())
        lpPacker->AddChar(value[0]);
    else
        lpPacker->AddChar(0);
    return 0;
}

int HSTdApi::pack_string(const std::string& value)
{
    if (!m_packer)
    {
        return -1;
    }

    IF2Packer* lpPacker;
    lpPacker = (IF2Packer*)m_packer;
    return lpPacker->AddStr(value.c_str());
}

int HSTdApi::pack_int(const int value)
{
    if (!m_packer)
    {
        return -1;
    }

    IF2Packer* lpPacker;
    lpPacker = (IF2Packer*)m_packer;
    return lpPacker->AddInt(value);
}

int HSTdApi::pack_double(const double value)
{
    if (!m_packer)
    {
        return -1;
    }

    IF2Packer* lpPacker;
    lpPacker = (IF2Packer*)m_packer;
    return lpPacker->AddDouble(value);
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
        .def("set_debug_mode", &HSTdApi::set_debug_mode)
        .def("set_json_value", &HSTdApi::set_json_value)
        .def("send_msg", &HSTdApi::send_msg)
        .def("recv_msg", &HSTdApi::recv_msg)

        .def("begin_pack", &HSTdApi::begin_pack)
        .def("end_pack", &HSTdApi::end_pack)

        .def("add_field", &HSTdApi::add_field)
        .def("pack_char", &HSTdApi::pack_char)
        .def("pack_string", &HSTdApi::pack_string)
        .def("pack_int", &HSTdApi::pack_int)
        .def("pack_double", &HSTdApi::pack_double)

        .def("on_front_connected", &HSTdApi::on_front_connected)
        .def("on_front_disconnected", &HSTdApi::on_front_disconnected)
        .def("on_recv_msg", &HSTdApi::on_recv_msg)

        .def("get_api_version", &HSTdApi::get_api_version)
        .def("get_hstrade_version", &HSTdApi::get_hstrade_version)
        ;
}



#endif//HAVE_WRPPAER_PYTHON
