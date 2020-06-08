#include <iostream>
#include <codecvt>
#include <locale>
#include <vector>
using namespace std;

#include "cJSON.h"

#include "ufxtrade_callback.h"
#include "ufxtrade_py.h"


#ifdef _MSC_VER
#else

#include <unistd.h>
#include <iconv.h>
#define sleepms(x)  usleep((x) * 1000)

int code_convert(char* from, char* to, char* inbuf, size_t inlen, char* outbuf, size_t outlen)
{
    iconv_t cd;
    char** pin = &inbuf;
    char** pout = &outbuf;

    cd = iconv_open(to, from);
    if (cd == 0)
        return -1;
    if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
        return -1;
    iconv_close(cd);
    return 0;
}

#endif//_MSC_VER


//将GBK编码的字符串转换为UTF8
static std::string to_utf(const std::string &gb2312)
{
#ifdef _MSC_VER
    const static locale loc("zh-CN");

    vector<wchar_t> wstr(gb2312.size());
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

    return std::string();
#else
    // const static locale loc("zh_CN.GB18030");
    if ((int)gb2312.length() < 4080)
    {
        char outbuf[4080] = "";
        int outlen = sizeof(outbuf) - 1;
        int rv = code_convert("gb2312", "utf-8", (char*)gb2312.c_str(), (int)gb2312.length(), outbuf, outlen);
        if (rv == -1)
            return string();
        return string(outbuf);
    }
    else
    {
        int outlen = gb2312.length() * 2 + 2;
        char* outbuf = (char*)malloc(outlen);
        memset(outbuf, 0, outlen);
        int rv = code_convert("gb2312", "utf-8", (char*)gb2312.c_str(), (int)gb2312.length(), outbuf, outlen);

        string ret(outbuf);
        free(outbuf);

        if (rv == -1)
            return string();
        else
            return ret;
    }
#endif
}


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

static void _on_connected_static(ufxtrade_t* hstd)
{
    // fprintf(stderr, "<_on_connected_static>\n");

    UFXTdApi* obj;
    obj = (UFXTdApi*)ufxtrade_get_userdata(hstd);
    if (!obj)
        return;

    gil_scoped_acquire acquire;
    obj->on_front_connected();
}

static void _on_disconnected_static(ufxtrade_t* hstd, int reason)
{
    // fprintf(stderr, "<_on_disconnected_static>\n");

    UFXTdApi* obj;
    obj = (UFXTdApi*)ufxtrade_get_userdata(hstd);
    if (!obj)
        return;

    gil_scoped_acquire acquire;
    obj->on_front_disconnected(reason);
}

static void _on_msg_error_static(ufxtrade_t* hstd, int func_id, int issue_type, int ref_id, int error_no, const char* error_info)
{
    // fprintf(stderr, "<_on_msg_error_static>\n");

    UFXTdApi* obj;
    obj = (UFXTdApi*)ufxtrade_get_userdata(hstd);
    if (!obj)
        return;

    gil_scoped_acquire acquire;
    obj->on_msg_error(func_id, issue_type, ref_id, error_no, std::string(to_utf(error_info)));
}

static void _on_recv_msg_static(ufxtrade_t* hstd, int func_id, int issue_type, int ref_id, const char* msg)
{
    // fprintf(stderr, "#<_on_recv_msg_static> func_id:%d, issue_type:%d\n", func_id, issue_type);
    // fprintf(stderr, msg);

    UFXTdApi* obj;
    obj = (UFXTdApi*)ufxtrade_get_userdata(hstd);
    if (!obj)
        return;

    gil_scoped_acquire acquire;
    obj->on_recv_msg(func_id, issue_type, ref_id, std::string(msg));
}


UFXTdApi::UFXTdApi()
{
    m_packer = NULL;
    m_hstd = NULL;

    memset(&m_spi, 0, sizeof(m_spi));
    m_spi.on_connected = _on_connected_static;
    m_spi.on_disconnected = _on_disconnected_static;
    m_spi.on_msg_error = _on_msg_error_static;
    m_spi.on_json_msg = _on_recv_msg_static;

    m_pBizMessage = NULL;

    m_hsend = 0;
    m_last_error_code = 0;
    m_async_mode = 0;
    m_data_proto = 0;
    m_debug_level = INT_MAX;
}

UFXTdApi::~UFXTdApi()
{
    if (m_packer)
    {
        m_packer->FreeMem(m_packer->GetPackBuf());
        m_packer->Release();
        m_packer = NULL;
    }

    if (m_hstd)
    {
        ufxtrade_realese(m_hstd);
        m_hstd = NULL;
    }
}

int UFXTdApi::get_api_version()
{
    int ver = 0;
    ufxtrade_version(&ver);
    return ver;
}

std::string UFXTdApi::get_ufxtrade_version()
{
    return std::string(ufxtrade_version(NULL));
}

std::string UFXTdApi::recv_biz_msg(int hsend)
{
    int rv;
    ufxtrade_t* hstd = m_hstd;
    IBizMessage* lpBizMessageRecv = NULL;

    rv = hstd->conn->RecvBizMsg(hsend, &lpBizMessageRecv, hstd->timeoutms);
    if (rv != 0)
    {
        fprintf(stderr, "recv_biz_msg() recv failed, rv:%d, err:%s!\n", rv, hstd->conn->GetErrorMsg(rv));
        m_last_error_code = rv;
    }
    else
    {
        int iReturnCode = lpBizMessageRecv->GetReturnCode();
        if (iReturnCode != 0)
        {
            fprintf(stderr, "ufxtrade_order_insert return code error code:%d, msg:%s\n",
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
                cJSON* json = TradeCallback::GenJsonDatas(lpUnPacker, lpBizMessageRecv->GetFunction(), lpBizMessageRecv->GetIssueType(), 0);
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

void UFXTdApi::create_api(int async_mode, int data_proto)
{
    m_async_mode = async_mode;
    m_data_proto = data_proto;

    m_hstd = ufxtrade_create(async_mode);
    if (m_debug_level != INT_MAX) {
        ufxtrade_debug_mode(m_hstd, m_debug_level);
    }

    ufxtrade_set_userdata(m_hstd, this);
    ufxtrade_register_spi(m_hstd, &m_spi);

    if (data_proto == UFX_DATA_PROTO_JSON)
    {
        // json mode
        // fprintf(stderr, "UFXTdApi set json mode!!!\n");
        m_hstd->callback->SetJsonMode(data_proto);
    }
}

void UFXTdApi::release()
{
    if (m_hstd)
    {
        ufxtrade_realese(m_hstd);
        m_hstd = NULL;
    }
}

void UFXTdApi::set_debug_mode(int level)
{
    m_debug_level = level;
    if (m_hstd)
    {
        ufxtrade_debug_mode(m_hstd, level);
    }
}

void UFXTdApi::set_timeout(int timeoutms)
{
    if (m_hstd)
    {
        ufxtrade_set_timeout(m_hstd, timeoutms);
    }
}

int UFXTdApi::config_load(std::string config_file)
{
    return ufxtrade_config_load(m_hstd, config_file.c_str());
}

int UFXTdApi::config_set_string(std::string section, std::string entry_name, std::string value)
{
    return ufxtrade_config_set_string(m_hstd, section.c_str(), entry_name.c_str(), value.c_str());
}

int UFXTdApi::config_set_int(std::string section, std::string entry_name, int value)
{
    return ufxtrade_config_set_int(m_hstd, section.c_str(), entry_name.c_str(), value);
}

std::string UFXTdApi::config_get_string(std::string section, std::string entry_name, std::string default_value)
{
    const char* ret_value = ufxtrade_config_get_string(m_hstd, section.c_str(), entry_name.c_str(), default_value.c_str());
    if (!ret_value)
        ret_value = "";
    return std::string(ret_value);
}

int UFXTdApi::config_get_int(std::string section, std::string entry_name, int default_value)
{
    return ufxtrade_config_get_int(m_hstd, section.c_str(), entry_name.c_str(), default_value);
}

int UFXTdApi::connect(int timeoutms)
{
    int rv;
    rv = ufxtrade_init(m_hstd, timeoutms);
    return rv;
}

int UFXTdApi::set_branch_no(int branch_no)
{
    if (m_pBizMessage) {
        m_pBizMessage->SetBranchNo(branch_no);
        return 0;
    }
    return -1;
}

int UFXTdApi::set_system_no(int system_no)
{
    if (m_pBizMessage) {
        m_pBizMessage->SetSystemNo(system_no);
        return 0;
    }
    return -1;
}

int UFXTdApi::set_subsystem_no(int subsystem_no)
{
    if (m_pBizMessage) {
        m_pBizMessage->SetSubSystemNo(subsystem_no);
        return 0;
    }
    return -1;
}

int UFXTdApi::set_sender_id(int sender_id)
{
    if (m_pBizMessage) {
        m_pBizMessage->SetSenderId(sender_id);
        return 0;
    }
    return -1;
}

int UFXTdApi::set_packet_id(int packet_id)
{
    if (m_pBizMessage) {
        m_pBizMessage->SetPacketId(packet_id);
        return 0;
    }
    return -1;
}

int UFXTdApi::set_sequece_no(int sequence_no)
{
    if (m_pBizMessage) {
        m_pBizMessage->SetSequeceNo(sequence_no);
        return 0;
    }
    return -1;
}

int UFXTdApi::set_issue_type(int issue_type)
{
    if (m_pBizMessage) {
        m_pBizMessage->SetIssueType(issue_type);
        return 0;
    }
    return -1;
}

int UFXTdApi::set_company_id(int company_id)
{
    if (m_pBizMessage) {
        m_pBizMessage->SetCompanyID(company_id);
        return 0;
    }
    return -1;
}

int UFXTdApi::set_sender_company_id(int sender_company_id)
{
    if (m_pBizMessage) {
        m_pBizMessage->SetSenderCompanyID(sender_company_id);
        return 0;
    }
    return -1;
}

int UFXTdApi::set_function(int func_id)
{
    if (m_pBizMessage) {
        m_pBizMessage->SetFunction(func_id);
        return 0;
    }
    return -1;
}

int UFXTdApi::set_packet_type(int packet_type)
{
    if (m_pBizMessage) {
        m_pBizMessage->SetPacketType(packet_type);
        return 0;
    }
    return -1;
}

int UFXTdApi::set_content(void)
{
    IBizMessage* lpBizMessage = m_pBizMessage;
    IF2Packer* lpPacker = (IF2Packer*)m_packer;

    if (!lpBizMessage) {
        return -1;
    }
    if (!lpPacker) {
        return -2;
    }

    lpBizMessage->SetContent(lpPacker->GetPackBuf(), lpPacker->GetPackLen());
    return 0;
}

int UFXTdApi::set_key_info(void)
{
    IBizMessage* lpBizMessage = m_pBizMessage;
    IF2Packer* lpPacker = (IF2Packer*)m_packer;

    if (!lpBizMessage) {
        return -1;
    }
    if (!lpPacker) {
        return -2;
    }

    lpBizMessage->SetKeyInfo(lpPacker->GetPackBuf(), lpPacker->GetPackLen());
    return 0;
}

int UFXTdApi::new_biz_message(void)
{
    release_biz_message();

    m_pBizMessage = NewBizMessage();
    m_pBizMessage->AddRef();
    return 0;
}

int UFXTdApi::release_biz_message(void)
{
    if (!m_pBizMessage) {
        return -1;
    }

    m_pBizMessage->Release();
    m_pBizMessage = NULL;
    return 0;
}

int UFXTdApi::send_biz_msg(void)
{
    int rv;
    IBizMessage* lpBizMessage = m_pBizMessage;
    rv = m_hstd->conn->SendBizMsg(lpBizMessage, m_hstd->is_async);
    // fprintf(stderr, "send_pack_msg(%d) async:%d rv:%d\n", func_id, m_hstd->is_async, rv);
    return rv;
}

int UFXTdApi::set_json_value(const std::string& json_str)
{
    m_json_str = json_str;
    return 0;
}

int UFXTdApi::send_pack_msg(int func_id)
{
    int rv;

    IF2Packer* lpPacker = (IF2Packer*)m_packer;
    IBizMessage* lpBizMessage = m_pBizMessage;
    if (!lpBizMessage)
    {
        // no create biz message before
        return -1;
    }

    lpBizMessage->SetFunction(func_id);
    lpBizMessage->SetPacketType(REQUEST_PACKET);

    // 设置打包数据
    if (func_id == UFX_FUNC_SUBSCRIBE)
        lpBizMessage->SetKeyInfo(lpPacker->GetPackBuf(), lpPacker->GetPackLen());
    else
        lpBizMessage->SetContent(lpPacker->GetPackBuf(), lpPacker->GetPackLen());

    // 发送消息
    rv = m_hstd->conn->SendBizMsg(lpBizMessage, m_hstd->is_async);
    // fprintf(stderr, "send_pack_msg(%d) async:%d rv:%d\n", func_id, m_hstd->is_async, rv);

    // 释放内存
    release_pack();
    release_biz_message();

    m_hsend = rv;
    return rv;
}

int UFXTdApi::send_msg(int func_id)
{
    m_hsend = 0;
    m_last_error_code = 0;

    if (m_packer)
    {
        return send_pack_msg(func_id);
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
        UFXReqUserLoginField login_req = { 0 };
        extract_json_str(json, login_req.client_id, "client_id");
        extract_json_str(json, login_req.password, "password");

        char password_type[2] = "";
        extract_json_str(json, password_type, "password_type");
        if (!password_type[0])
            password_type[0] = UFX_PWD_TYPE_TRADE;
        login_req.password_type = password_type[0];
        rv = ufxtrade_user_login(m_hstd, &login_req);
    }
    else if (func_id == UFX_FUNC_PLACE_ORDER)
    {
        UFXReqOrderInsertField order_req = { 0 };

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

        rv = ufxtrade_order_insert(m_hstd, &order_req);
    }
    else if (func_id == UFX_FUNC_CANCEL_ORDER)
    {
        UFXReqOrderActionField order_action = { 0 };
        extract_json_str(json, order_action.entrust_no, "entrust_no");
        rv = ufxtrade_order_action(m_hstd, &order_action);
    }
    else if (func_id == UFX_FUNC_QRY_CASH || func_id == UFX_FUNC_QRY_POSITION ||
        func_id == UFX_FUNC_QRY_ORDER || func_id == UFX_FUNC_QRY_TRADE)
    {
        UFXReqQueryField qry_field = { 0 };
        if (func_id == UFX_FUNC_QRY_CASH)
        {
            rv = ufxtrade_qry_trading_account(m_hstd, &qry_field);
        }
        else if (func_id == UFX_FUNC_QRY_POSITION)
        {
            rv = ufxtrade_qry_position(m_hstd, &qry_field);
        }
        else if (func_id == UFX_FUNC_QRY_TRADE)
        {
            rv = ufxtrade_qry_trade(m_hstd, &qry_field);
        }
        else if (func_id == UFX_FUNC_QRY_ORDER)
        {
            rv = ufxtrade_qry_order(m_hstd, &qry_field);
        }
    }
    else if (func_id == UFX_FUNC_QRY_SECINFO)
    {
        UFXReqQueryField qry_field = { 0 };
        extract_json_str(json, qry_field.exchange_type, "exchange_type");
        extract_json_str(json, qry_field.stock_code, "stock_code");
        rv = ufxtrade_qry_security_info(m_hstd, &qry_field);
    }
    else if (func_id == UFX_FUNC_QRY_SECMD)
    {
        UFXReqQueryField qry_field = { 0 };
        extract_json_str(json, qry_field.exchange_type, "exchange_type");
        extract_json_str(json, qry_field.stock_code, "stock_code");
        rv = ufxtrade_qry_md(m_hstd, &qry_field);
    }
    else if (func_id == UFX_FUNC_SUBSCRIBE)
    {
        int issue_type = 0;
        extract_json_int(json, issue_type, "issue_type");
        rv = ufxtrade_subscribe(m_hstd, issue_type);
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

void UFXTdApi::new_packer(int iver)
{
    release_pack();
    m_packer = NewPacker(iver);
    m_packer->AddRef();
}

void UFXTdApi::begin_pack(void)
{
    m_last_error_code = 0;
    if (m_packer)
    {
        m_packer->BeginPack();
    }
}

void UFXTdApi::new_and_begin_pack()
{
    new_packer(2);
    begin_pack();
}

void UFXTdApi::end_pack(void)
{
    if (m_packer)
    {
        m_packer->EndPack();
    }
}

void UFXTdApi::release_pack(void)
{
    IF2Packer* lpPacker;
    lpPacker = (IF2Packer*)m_packer;
    if (lpPacker)
    {
        lpPacker->FreeMem(lpPacker->GetPackBuf());
        lpPacker->Release();
        m_packer = NULL;
    }
}

int UFXTdApi::add_field(const std::string& key, const std::string& field_type, int field_width, int field_scale)
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

int UFXTdApi::add_char(const std::string& value)
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

int UFXTdApi::add_str(const std::string& value)
{
    if (m_packer)
    {
        // fprintf(stderr, "add_str value:%s\n", value.c_str());
        return m_packer->AddStr(value.c_str());
    }
    return -1;
}

int UFXTdApi::add_int(const int value)
{
    if (m_packer)
    {
        // fprintf(stderr, "add_int value:%d\n", value);
        return m_packer->AddInt(value);
    }
    return -1;
}

int UFXTdApi::add_double(const double value)
{
    if (m_packer)
    {
        // fprintf(stderr, "add_double value:%f\n", value);
        return m_packer->AddDouble(value);
    }
    return -1;
}

//////////////////////////////////////////////////////////////////////////

class PyUFXTdApi : public UFXTdApi
{
public:
    using UFXTdApi::UFXTdApi;

    void on_front_connected() override
    {
        try
        {
            PYBIND11_OVERLOAD(void, UFXTdApi, on_front_connected);
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
            PYBIND11_OVERLOAD(void, UFXTdApi, on_front_disconnected, reqid);
        }
        catch (const error_already_set &e)
        {
            std::cout << "on_front_disconnected: " << e.what() << std::endl;
        }
    };

    void on_msg_error(int func_id, int issue_type, int ref_id, int error_no, const std::string& error_info) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, UFXTdApi, on_msg_error, func_id, issue_type, ref_id, error_no, error_info);
        }
        catch (const error_already_set &e)
        {
            std::cout << "on_msg_error: " << e.what() << std::endl;
        }
    }

    void on_recv_msg(int func_id, int issue_type, int ref_id, const std::string& msg) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, UFXTdApi, on_recv_msg, func_id, issue_type, ref_id, msg);
        }
        catch (const error_already_set &e)
        {
            std::cout << "on_recv_msg: " << e.what() << std::endl;
        }
    }

};


PYBIND11_MODULE(ufxtd, m)
{
    class_<UFXTdApi, PyUFXTdApi> tdapi(m, "UFXTdApi", module_local());
    tdapi
        .def(init<>())
        .def("create_api", &UFXTdApi::create_api)
        .def("release", &UFXTdApi::release)
        .def("connect", &UFXTdApi::connect)
        .def("config_load", &UFXTdApi::config_load)
        .def("config_set_string", &UFXTdApi::config_set_string)
        .def("config_set_int", &UFXTdApi::config_set_int)
        .def("config_get_string", &UFXTdApi::config_get_string)
        .def("config_get_int", &UFXTdApi::config_get_int)
        .def("set_debug_mode", &UFXTdApi::set_debug_mode)
        .def("set_json_value", &UFXTdApi::set_json_value)
        .def("SetJsonValue", &UFXTdApi::set_json_value)

        // some interfaces for biz message
        .def("set_branch_no", &UFXTdApi::set_branch_no)
        .def("set_system_no", &UFXTdApi::set_system_no)
        .def("set_subsystem_no", &UFXTdApi::set_subsystem_no)
        .def("set_sender_id", &UFXTdApi::set_sender_id)
        .def("set_packet_id", &UFXTdApi::set_packet_id)
        .def("set_sequece_no", &UFXTdApi::set_sequece_no)
        .def("set_issue_type", &UFXTdApi::set_issue_type)
        .def("set_company_id", &UFXTdApi::set_company_id)
        .def("set_sender_company_id", &UFXTdApi::set_sender_company_id)
        .def("set_function", &UFXTdApi::set_function)
        .def("set_packet_type", &UFXTdApi::set_packet_type)
        .def("set_content", &UFXTdApi::set_content)
        .def("set_key_info", &UFXTdApi::set_key_info)
        .def("new_biz_message", &UFXTdApi::new_biz_message)
        .def("release_biz_message", &UFXTdApi::release_biz_message)
        .def("send_biz_msg", &UFXTdApi::send_biz_msg)
        .def("recv_biz_msg", &UFXTdApi::recv_biz_msg)
        .def("send_msg", &UFXTdApi::send_msg)  // local wrapper

        .def("new_and_begin_pack", &UFXTdApi::new_and_begin_pack)
        .def("new_packer", &UFXTdApi::new_packer)
        .def("begin_pack", &UFXTdApi::begin_pack)
        .def("end_pack", &UFXTdApi::end_pack)
        .def("release_pack", &UFXTdApi::release_pack)
        .def("add_field", &UFXTdApi::add_field)
        .def("add_char", &UFXTdApi::add_char)
        .def("add_str", &UFXTdApi::add_str)
        .def("add_int", &UFXTdApi::add_int)
        .def("add_double", &UFXTdApi::add_double)

        // some raw t2sdk api names
        .def("SetBranchNo", &UFXTdApi::set_branch_no)
        .def("SetSystemNo", &UFXTdApi::set_system_no)
        .def("SetSubSystemNo", &UFXTdApi::set_subsystem_no)
        .def("SetSenderId", &UFXTdApi::set_sender_id)
        .def("SetPacketId", &UFXTdApi::set_packet_id)
        .def("SetSequenceNo", &UFXTdApi::set_sequece_no)
        .def("SetIssueType", &UFXTdApi::set_issue_type)
        .def("SetCompanyId", &UFXTdApi::set_company_id)
        .def("SetSenderCompanyId", &UFXTdApi::set_sender_company_id)
        .def("SetFunction", &UFXTdApi::set_function)
        .def("SetPacketType", &UFXTdApi::set_packet_type)
        .def("SetContent", &UFXTdApi::set_content)
        .def("SetKeyInfo", &UFXTdApi::set_key_info)
        .def("NewBizMessage", &UFXTdApi::new_biz_message)
        .def("ReleaseBizMessage", &UFXTdApi::release_biz_message)
        .def("SendBizMsg", &UFXTdApi::send_biz_msg)

        .def("NewPacker", &UFXTdApi::new_packer)
        .def("BeginPack", &UFXTdApi::begin_pack)
        .def("EndPack", &UFXTdApi::end_pack)
        .def("ReleasePack", &UFXTdApi::end_pack)
        .def("AddField", &UFXTdApi::add_field)
        .def("AddChar", &UFXTdApi::add_char)
        .def("AddStr", &UFXTdApi::add_str)
        .def("AddInt", &UFXTdApi::add_int)
        .def("AddDouble", &UFXTdApi::add_double)
        .def("SendMsg", &UFXTdApi::send_msg)
        .def("RecvBizMsg", &UFXTdApi::recv_biz_msg)

        // the callbacks
        .def("on_front_connected", &UFXTdApi::on_front_connected)
        .def("on_front_disconnected", &UFXTdApi::on_front_disconnected)
        .def("on_msg_error", &UFXTdApi::on_msg_error)
        .def("on_recv_msg", &UFXTdApi::on_recv_msg)

        // other
        .def("get_api_version", &UFXTdApi::get_api_version)
        .def("get_ufxtrade_version", &UFXTdApi::get_ufxtrade_version)
        ;
}



#endif//HAVE_WRPPAER_PYTHON
