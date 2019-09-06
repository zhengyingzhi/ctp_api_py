#include <time.h>
#include <codecvt>
#include <locale>
#include <iostream>
// #include <condition_variable>

#include "xctd.h"


#define MY_DEBUG 0

#if (MY_DEBUG)
#define XcDebugInfo(fd,fmt,...) fprintf(fd,fmt,##__VA_ARGS__)
#else
static  inline void XcPrintNull(FILE* fd, const char* fmt, ...) { (void)fd; (void)fmt; }
#define XcDebugInfo(fd,fmt,...) XcPrintNull(fd,fmt,##__VA_ARGS__)
#endif//MY_DEBUG
#define XcDbgFd stdout


#define XC_OP_BRANCH_NO         "0"
#define XC_OP_ENTRUST_WAY       "7"     // 网上委托
#define XC_PASSWORD_TYPE        "2"     // 1-资金密码 2-交易密码

#define XC_EXCHANGE_TYPE_CASH   "0"     // 0-资金 1-上海 2-深圳
#define XC_EXCHANGE_TYPE_SSE    "1"
#define XC_EXCHANGE_TYPE_SZSE   "2"

#define XC_ENTRUST_TYPE_BUY     "1"     // 1-买入 2-卖出
#define XC_ENTRUST_TYPE_SELL    "2"
#define XC_ENTRUST_PROP         "0"     // 0-买卖

#if 0
//从字典中获取某个建值对应的字符串，并赋值到请求结构体对象的值上
// template <uint32_t size>
// void get_string(const pybind11::dict &d, const char *key, string_literal<size> &value)
void get_string(const pybind11::dict &d, const char *key, std::string &value)
{
    if (d.contains(key))
    {
        object o = d[key];
        std::string s = o.cast<std::string>();
        // const char *buf = s.c_str();
        // strcpy(value, buf);
        value = s;
    }
};
#endif//0

//将GBK编码的字符串转换为UTF8
static std::string to_utf(const std::string &gb2312)
{
#ifdef _MSC_VER
    const static locale loc("zh-CN");
#else
    const static locale loc("zh_CN.GB18030");
#endif

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
}


XcTdApi::XcTdApi()
    : api()
    , fp()
{
}

XcTdApi::~XcTdApi()
{
    if (fp)
    {
        fclose(fp);
        fp = NULL;
    }
}

void XcTdApi::OnClose()
{
    XcDebugInfo(XcDbgFd, "xctdapi OnClose\n");
    this->on_front_disconnected(-1);
}

void XcTdApi::OnDisConnect(void)
{
    XcDebugInfo(XcDbgFd, "xctdapi OnDisConnect\n");
    this->on_front_disconnected(-1);
}

void XcTdApi::OnConnected(void)
{
    XcDebugInfo(XcDbgFd, "xctdapi OnConnected\n");
    this->on_front_connected();
}

void XcTdApi::OnRecvJsonMsg(char* pJsonMsg)
{
    // XcDebugInfo(XcDbgFd, pJsonMsg);
    write_data(0, pJsonMsg);

    std::string msg = to_utf(pJsonMsg);
    this->on_recv_msg(msg);
}


//////////////////////////////////////////////////////////////////////////
void XcTdApi::create_td_api(std::string str, int reserve)
{
    (void)str;
    (void)reserve;
    this->api = CXcTradeApi::CreateTradeApi();
    this->api->Register(Trans_Mode_ASY, Data_Proto_Json, this);
}

void XcTdApi::release()
{
    if (this->api)
    {
        this->api->Release();
        this->api = NULL;
    }
}

int XcTdApi::init(std::string user_id, std::string server_ip, std::string server_port, std::string license)
{
    int rv;
    XcDebugInfo(XcDbgFd, "xctdapi init user_id:%s, server_ip:%s, server_port:%s, liscense:%s\n",
        user_id.c_str(), server_ip.c_str(), server_port.c_str(), license.c_str());

    char server_addr[256] = "";
    sprintf(server_addr, "%s:%s", server_ip.c_str(), server_port.c_str());

    // we serialize all data for easy debug
    if (!fp)
    {
        time_t now = time(NULL);
        struct tm* ptm = localtime(&now);
        char filename[256] = "";
        sprintf(filename, "xctdapi_%04d%02d%02d.con",
            ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
        fp = fopen(filename, "a+");
        if (!fp)
        {
            XcDebugInfo(XcDbgFd, "open serialize file:%s failed!\n", filename);
        }
    }

    // keep it
    account_id = user_id;

    write_data(0, "xctdapi connecting to server:%s, liscense:%s, user_id:%s",
        server_addr, license.c_str(), user_id.c_str());
    rv = this->api->Connect(server_addr, (char*)license.c_str(), System_UFX, (char*)user_id.c_str());
    if (rv < 0)
    {
        XcDebugInfo(XcDbgFd, "xctdapi Connect failed:%d\n", rv);
        write_data(0, "xctdapi %s connect failed rv:%d", user_id.c_str(), rv);
    }

    return rv;
}

int XcTdApi::send_data(int func_id, const std::string& data, int subsystem_no, int branch_no)
{
    write_data(0, "send_data func_id:%d,subsystem_no:%d,branch_no:%d,data:\n%s",
        func_id, subsystem_no, branch_no, data.c_str());

    int rv;
    rv = api->SetJsonValue(data.c_str());
    if (rv < 0) {
        return rv;
    }

    rv = api->SendMsg(func_id);
    return rv;
}

int XcTdApi::get_space()
{
    if (api)
    {
        return api->GetSpace();
    }
    return -1;
}

int XcTdApi::get_frequency()
{
    if (api)
    {
        return api->GetFrequency();
    }
    return -1;
}

std::string XcTdApi::get_last_error_msg()
{
    if (api)
    {
        return api->GetLastErrorMsg();
    }
    return "";
}

std::string XcTdApi::get_api_version()
{
    return std::string(XC_TDAPI_VERSION);
}

int XcTdApi::open_debug(std::string log_file, int log_level)
{
    return -1;
}

std::string XcTdApi::get_field_buy()
{
    return XC_ENTRUST_TYPE_BUY;
}
std::string XcTdApi::get_field_sell()
{
    return XC_ENTRUST_TYPE_SELL;
}

std::string XcTdApi::get_field_exchange_sse()
{
    return XC_EXCHANGE_TYPE_SSE;
}
std::string XcTdApi::get_field_exchange_szse()
{
    return XC_EXCHANGE_TYPE_SZSE;
}

int XcTdApi::write_data(int reserve, const char* fmt, ...)
{
    (void)reserve;
    if (!fp)
    {
        return -1;
    }

#if 1
    char buf[16300] = "";
    int  len = 0;

    time_t now = time(NULL);
    struct tm* ptm = localtime(&now);
    len = sprintf(buf + len, "%04d-%02d-%02d %02d:%02d:%02d \n",
        ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
        ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

    va_list args;
    va_start(args, fmt);
    len += vsnprintf(buf + len, sizeof(buf) - len - 2, fmt, args);
    va_end(args);
    buf[len] = '\r';
    buf[len + 1] = '\n';
    len += 2;
    buf[len] = '\0';

    fwrite(buf, len, 1, fp);
    fflush(fp);
#endif
    return 0;
}

///-------------------------------------------------------------------------------------
///Boost.Python封装
///-------------------------------------------------------------------------------------

class PyTdApi : public XcTdApi
{
public:
    using XcTdApi::XcTdApi;

    void on_front_connected() override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcTdApi, on_front_connected);
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
            PYBIND11_OVERLOAD(void, XcTdApi, on_front_disconnected, reqid);
        }
        catch (const error_already_set &e)
        {
            std::cout << "on_front_disconnected: " << e.what() << std::endl;
        }
    };

    void on_recv_msg(const std::string& msg) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcTdApi, on_recv_msg, msg);
        }
        catch (const error_already_set &e)
        {
            std::cout << "on_recv_msg: " << e.what() << std::endl;
        }
    }

};


PYBIND11_MODULE(xctd, m)
{
    class_<XcTdApi, PyTdApi> tdapi(m, "XcTdApi", module_local());
    tdapi
        .def(init<>())
        .def("create_td_api", &XcTdApi::create_td_api)
        .def("release", &XcTdApi::release)
        .def("init", &XcTdApi::init)
        .def("get_api_version", &XcTdApi::get_api_version)
        .def("open_debug", &XcTdApi::open_debug)
        .def("send_data", &XcTdApi::send_data)
        .def("get_space", &XcTdApi::get_space)
        .def("get_frequency", &XcTdApi::get_frequency)
        .def("get_last_error_msg", &XcTdApi::get_last_error_msg)
        .def("get_field_buy", &XcTdApi::get_field_buy)
        .def("get_field_sell", &XcTdApi::get_field_sell)
        .def("get_field_exchange_sse", &XcTdApi::get_field_exchange_sse)
        .def("get_field_exchange_szse", &XcTdApi::get_field_exchange_szse)

        .def("on_front_connected", &XcTdApi::on_front_connected)
        .def("on_front_disconnected", &XcTdApi::on_front_disconnected)
        .def("on_recv_msg", &XcTdApi::on_recv_msg)
        ;
}
