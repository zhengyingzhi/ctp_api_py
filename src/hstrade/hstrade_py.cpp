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


// 从原始api获取数据的回调
static void _on_recv_msg(void* clientd, int func_id, int issue_type, const std::string& msg)
{
    HSTdApi* obj;
    obj = (HSTdApi*)clientd;
}

HSTdApi::HSTdApi()
{
    m_hstd = NULL;
    m_packer = NULL;
}

HSTdApi::~HSTdApi()
{}


void HSTdApi::create_api(int async_mode, int data_proto)
{
    m_hstd = hstrade_create(async_mode);

    if (data_proto == HS_DATA_PROTO_JSON)
    {
        // json mode
        m_hstd->callback->SetOnRecvMsg(this, _on_recv_msg);
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

int HSTdApi::connect(std::string server_port, std::string license_path, std::string fund_account)
{
    int rv;
    rv = hstrade_init(m_hstd);
    return rv;
}

int HSTdApi::set_json_value(const std::string& json_str)
{
    m_json_str = json_str;
    return 0;
}

int HSTdApi::send_pack_msg(int func_id)
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
        send_pack_msg(func_id);
    }
    else if (m_json_str.empty())
    {
        return -1;
    }

    // TODO: convert m_json_str data to HSReqOrderField/HSQueryField/...

    int rv;
    cJSON* item;
    cJSON* json;
    json = cJSON_Parse(m_json_str.c_str());

    if (func_id == UFX_FUNC_LOGIN)
    {
        HSReqUserLoginField login_req = { 0 };
        extract_json_str(json, login_req.client_id, "client_id");
        extract_json_str(json, login_req.password, "password");
        rv = hstrade_user_login(m_hstd, &login_req);
    }
    else if (func_id == UFX_FUNC_PLACE_ORDER)
    {
        HSReqOrderInsertField order_req = { 0 };
        extract_json_str(json, order_req.stock_code, "stock_code");

        // item = cJSON_GetObjectItem(json, "entrust_amount");
        // if (item) {
        //     order_req.entrust_amount = item->valueint;
        // }
        extract_json_int(json, order_req.entrust_amount, "entrust_amount");
        extract_json_int(json, order_req.entrust_price, "entrust_price");
        extract_json_str(json, order_req.entrust_prop, "entrust_prop");

        item = cJSON_GetObjectItem(json, "entrust_bs");
        if (item && *item->valuestring) {
            order_req.entrust_bs = item->valuestring[0];
        }

        // TODO: other field

        rv = hstrade_order_insert(m_hstd, &order_req);
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
    else if (func_id == UFX_FUNC_QRY_MD)
    {
        HSReqQueryField qry_field = { 0 };
        extract_json_str(json, qry_field.exchange_type, "exchange_type");
        extract_json_str(json, qry_field.stock_code, "stock_code");
        rv = hstrade_qry_md(m_hstd, &qry_field);
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
        return -1;
    }

    if (field_type.length() > 1)
    {
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


void HSTdApi::on_api_msg(int func_id, int issue_type, const std::string& msg)
{
    // got api msg
    this->on_recv_msg(func_id, issue_type, msg);
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


PYBIND11_MODULE(hstd, m)
{
    class_<HSTdApi, PyHSTdApi> tdapi(m, "HSTdApi", module_local());
    tdapi
        .def(init<>())
        .def("create_td_api", &HSTdApi::create_api)
        .def("release", &HSTdApi::release)
        .def("connect", &HSTdApi::connect)
        .def("set_json_value", &HSTdApi::set_json_value)
        .def("send_msg", &HSTdApi::send_msg)
        // .def("recv_msg", &XcTdApi::recv_msg)

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
        ;
}



#endif//HAVE_WRPPAER_PYTHON
