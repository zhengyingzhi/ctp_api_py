#include <time.h>
#include <codecvt>
#include <locale>
#include <iostream>
// #include <condition_variable>

#include "xctd.h"

#ifdef _MSC_VER
#include <Windows.h>

static long get_tid()
{
    return GetCurrentThreadId();
}
#else

#include <pthread.h>
#include <unistd.h>
#include <iconv.h>
#define sleepms(x)  usleep((x) * 1000)

static long get_tid()
{
    return (long)pthread_self();
}

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


#define MY_DEBUG 0

#if (MY_DEBUG)
#define XcDebugInfo(fd,fmt,...) fprintf(fd,fmt,##__VA_ARGS__)
#else
static  inline void XcPrintNull(FILE* fd, const char* fmt, ...) { (void)fd; (void)fmt; }
#define XcDebugInfo(fd,fmt,...) XcPrintNull(fd,fmt,##__VA_ARGS__)
#endif//MY_DEBUG
#define XcDbgFd stdout

#define DEBUG_VIEW_LEVEL        12
#define IS_DBGVIEW(level)       ((level) == (DEBUG_VIEW_LEVEL))


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

static const char* func_id_desc(int func_id)
{
    switch (func_id)
    {
    case XC_FUNC_QRY_SECINFO:       return "qry_secifo";
    case XC_FUNC_QRY_CASH_FAST:     return "qry_cash_fast";
    case XC_FUNC_QRY_CASH:          return "qry_cash";
    case XC_FUNC_PLACE_ORDER:       return "place_order";
    case XC_FUNC_CANCEL_ORDER:      return "cancel_order";
    case XC_FUNC_QRY_PURCHASE:      return "qry_purchase";
    case XC_FUNC_QRY_ORDER:         return "qry_order";
    case XC_FUNC_QRY_TRADE:         return "qry_trade";
    case XC_FUNC_QRY_POSITION:      return "qry_position";
    case XC_FUNC_QRY_MD:            return "qry_md";
    case XC_FUNC_LOGIN:             return "client_login";
    case XC_FUNC_QRY_ACCOUNTINFO:   return "qry_account_info";
    case XC_FUNC_SUB_ORDER:         return "subscribe_order";
    case XC_FUNC_OPT_QRY_INFO:      return "opt_qry_info";
    case XC_FUNC_OPT_QRY_UNDERLYING:return "opt_qry_underlying";
    case XC_FUNC_OPT_PLACE_ORDER:   return "opt_place_order";
    case XC_FUNC_OPT_CANCEL_ORDER:  return "opt_cancel_order";
    case XC_FUNC_OPT_QRY_ORDER:     return "opt_qry_order";
    case XC_FUNC_OPT_QRY_TRADE:     return "opt_qry_trade";
    case XC_FUNC_OPT_QRY_POSITION:  return "opt_qry_position";
    case XC_FUNC_OPT_QRY_ASSET:     return "opt_qry_asset";
    case XC_FUNC_OPT_QRY_MD:        return "opt_qry_md";
    default:                        return "unknown";
    }
}

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

static void log_debug(const char* fmt, ...)
{
    char buf[16300] = "";
    int  len = 0;

    time_t now = time(NULL);
    struct tm* ptm = localtime(&now);
    len = sprintf(buf + len, "%04d-%02d-%02d %02d:%02d:%02d [%ld] ",
        ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
        ptm->tm_hour, ptm->tm_min, ptm->tm_sec, get_tid());

    va_list args;
    va_start(args, fmt);
    len += vsnprintf(buf + len, sizeof(buf) - len - 2, fmt, args);
    va_end(args);
    // [len++] = '\r';
    buf[len++] = '\n';
    buf[len] = '\0';

#ifdef _MSC_VER
    OutputDebugStringA(buf);
#else
    fprintf(stderr, buf);
#endif//_MSC_VRR
}


XcTdApi::XcTdApi()
    : m_api()
    , m_fp()
{
}

XcTdApi::~XcTdApi()
{
    if (m_fp)
    {
        fclose(m_fp);
        m_fp = NULL;
    }
}

void XcTdApi::OnClose()
{
    if (IS_DBGVIEW(m_log_level)) {
        log_debug("xctd OnClose");
    }
    this->on_front_disconnected(-1);
}

void XcTdApi::OnDisConnect(void)
{
    if (IS_DBGVIEW(m_log_level)) {
        log_debug("xctd OnDisConnect");
    }
    this->on_front_disconnected(-1);
}

void XcTdApi::OnConnected(void)
{
    if (IS_DBGVIEW(m_log_level)) {
        log_debug("xctd OnConnected");
    }
    this->on_front_connected();
}

void XcTdApi::OnRecvJsonMsg(char* pJsonMsg)
{
    // XcDebugInfo(XcDbgFd, pJsonMsg);
    // write_data(0, pJsonMsg);

    if (IS_DBGVIEW(m_log_level)) {
        log_debug("xctd OnRecvJsonMsg");
    }

    std::string msg = to_utf(pJsonMsg);
    this->on_recv_msg(msg);
}

void XcTdApi::OnRecvPackMsg(int iFunid, int iRefid, int iIssueType, int iSet, int iRow, int iCol, char* szName, char* szValue)
{
    std::string name = to_utf(szName);
    std::string value = to_utf(szValue);
    this->on_recv_pack_msg(iFunid, iRefid, iIssueType, iSet, iRow, iCol, name, value);
}

void XcTdApi::OnRecvPackEndSet(int iFunid, int iRefid, int iIssueType, int iSet)
{
    this->on_recv_pack_end_set(iFunid, iRefid, iIssueType, iSet);
}

void XcTdApi::OnRecvPackEndRow(int iFunid, int iRefid, int iIssueType, int iSet, int iRow)
{
    this->on_recv_pack_end_row(iFunid, iRefid, iIssueType, iSet, iRow);
}


//////////////////////////////////////////////////////////////////////////
void XcTdApi::create_td_api(int async_mode, int data_proto)
{
    m_api = CXcTradeApi::CreateTradeApi();
    m_api->Register(async_mode, data_proto, this);

    if (IS_DBGVIEW(m_log_level)) {
        log_debug("xctd create_td_api async_mode:%d, data_proto:%d", async_mode, data_proto);
    }
}

void XcTdApi::release()
{
    if (m_api)
    {
        m_api->Release();
        m_api = NULL;
    }
}

int XcTdApi::connect(std::string server_addr, std::string license, std::string fund_account)
{
    int rv;
    XcDebugInfo(XcDbgFd, "xctdapi init user_id:%s, server_addr:%s, liscense:%s\n",
        fund_account.c_str(), server_addr.c_str(), license.c_str());

    if (IS_DBGVIEW(m_log_level)) {
        log_debug("xctd connect server_addr:%s, fund_account:%s", server_addr.c_str(), fund_account.c_str());
    }

    // keep it
    m_server_addr = server_addr;
    m_license = license;
    m_account_id = fund_account;

    rv = m_api->Connect((char*)server_addr.c_str(), (char*)license.c_str(), System_UFX, (char*)fund_account.c_str());
    if (rv < 0)
    {
        XcDebugInfo(XcDbgFd, "xctdapi Connect to %s failed:%d\n", server_addr.c_str(), rv);
        write_data(0, "xctdapi %s connect to %s failed rv:%d", fund_account.c_str(), server_addr.c_str(), rv);
    }

    return rv;
}

void XcTdApi::begin_pack(void)
{
    m_api->BeginPack();
}

void XcTdApi::end_pack(void)
{
    m_api->EndPack();
}

int XcTdApi::set_pack_value(const std::string& key_name, const std::string& value)
{
    return m_api->SetPackValue(key_name.c_str(), value.c_str());
}

int XcTdApi::set_json_value(const std::string& json_str)
{
    return m_api->SetJsonValue(json_str.c_str());
}

int XcTdApi::send_msg(int func_id, int subsystem_no, int branch_no)
{
    int rv = m_api->SendMsg(func_id, subsystem_no, branch_no);
    return rv;
}

int XcTdApi::send_json_data(int func_id, const std::string& data, int subsystem_no, int branch_no)
{
    // write_data(0, "send_data func_id:%d(%s),subsystem_no:%d,branch_no:%d,data:\n%s",
    //     func_id, func_id_desc(func_id), subsystem_no, branch_no, data.c_str());

    if (IS_DBGVIEW(m_log_level)) {
        log_debug("xctd send_json_data func_id:%d(%s), subsystem_no:%d, branch_no:%d, data:%dbytes!",
                   func_id, func_id_desc(func_id), subsystem_no, branch_no, data.length());
    }

    int rv;
    rv = m_api->SetJsonValue(data.c_str());
    if (rv < 0) {
        return rv;
    }

    if (IS_DBGVIEW(m_log_level)) {
        log_debug("xctd SendMsg func_id:%d...", func_id);
    }

    rv = m_api->SendMsg(func_id, subsystem_no, branch_no);

    if (IS_DBGVIEW(m_log_level)) {
        log_debug("xctd SendMsg func_id:%d, rv:%d...", func_id, rv);
    }

    return rv;
}

int XcTdApi::recv_msg()
{
    return m_api->RecvMsg();
}

int XcTdApi::get_data_set_count()
{
    return m_api->GetDataSetCount();
}

int XcTdApi::get_cur_row_count()
{
    return m_api->GetCurRowCount();
}

int XcTdApi::get_cur_col_count()
{
    return m_api->GetCurColCount();
}

int XcTdApi::get_cur_data_set(int index)
{
    return m_api->GetCurDataSet(index);
}

std::string XcTdApi::get_col_name(int col)
{
    char name[1024] = "";
    if (m_api->GetColName(col, name))
    {
        return std::string(name);
    }
    return to_utf(name);
}

std::string XcTdApi::get_col_value(int col)
{
    char value[4096] = "";
    if (m_api->GetColValue(col, value))
    {
    }
    return to_utf(value);
}

void XcTdApi::get_next_row()
{
    m_api->GetNextRow();
}

std::string XcTdApi::get_json_value(int len)
{
    char* buf = new char[len + 1];
    m_api->GetJsonValue(buf);
    buf[len] = '\0';

    std::string msg = to_utf(buf);
    return msg;
}

std::string XcTdApi::recv_json_data()
{
    int ret;
    ret = m_api->RecvMsg();
    if (ret <= 0)
    {
        // error
        return "";
    }

    return get_json_value(ret);
}

int XcTdApi::get_space()
{
    if (m_api)
    {
        return m_api->GetSpace();
    }
    return -1;
}

int XcTdApi::get_frequency()
{
    if (m_api)
    {
        return m_api->GetFrequency();
    }
    return -1;
}

std::string XcTdApi::get_last_error_msg()
{
    if (m_api)
    {
        std::string errmsg = to_utf(m_api->GetLastErrorMsg());
        return errmsg;
    }
    return std::string("");
}

std::string XcTdApi::get_api_version()
{
    return std::string(XC_TDAPI_VERSION);
}

int XcTdApi::open_debug(std::string log_file, int log_level)
{
    m_log_level = log_level;

    if (m_fp)
    {
        fflush(m_fp);
        fclose(m_fp);
        m_fp = NULL;
    }

    time_t now = time(NULL);
    struct tm* ptm = localtime(&now);
    char filename[512] = "";
    if (log_file.empty())
    {
        sprintf(filename, "xctdapi_%04d%02d%02d.con",
            ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
    }
    else
    {
        strncpy(filename, log_file.c_str(), sizeof(filename) - 1);
    }

    m_fp = fopen(filename, "a+");
    if (!m_fp)
    {
        fprintf(stderr, "open serialize file:%s failed!\n", filename);
        return -1;
    }

    return 0;
}

int XcTdApi::write_line(int reserve, const std::string& line)
{
    (void)reserve;
    if (!m_fp)
    {
        return -1;
    }

#if 1
    char buf[1000] = "";
    int  len = 0;

    time_t now = time(NULL);
    struct tm* ptm = localtime(&now);
    len = sprintf(buf + len, "%04d-%02d-%02d %02d:%02d:%02d \n",
        ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
        ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

    fwrite(buf, len, 1, m_fp);
    fwrite(line.c_str(), line.length(), 1, m_fp);
    fwrite("\n", 1, 1, m_fp);
    fflush(m_fp);
#endif
    return 0;
}

int XcTdApi::write_data(int reserve, const char* fmt, ...)
{
    (void)reserve;
    if (!m_fp)
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
    // [len++] = '\r';
    buf[len++] = '\n';
    buf[len] = '\0';

    fwrite(buf, len, 1, m_fp);
    fflush(m_fp);
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
        catch (const pybind11::error_already_set &e)
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
        catch (const pybind11::error_already_set &e)
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
        catch (const pybind11::error_already_set &e)
        {
            std::cout << "on_recv_msg: " << e.what() << std::endl;
        }
    }

    void on_recv_pack_msg(int iFunid, int iRefid, int iIssueType, int iSet, int iRow, int iCol, const std::string& name, const std::string& value) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcTdApi, on_recv_pack_msg, iFunid, iRefid, iIssueType, iSet, iRow, iCol, name, value);
        }
        catch (const pybind11::error_already_set &e)
        {
            std::cout << "on_recv_pack_msg: " << e.what() << std::endl;
        }
    }

    void on_recv_pack_end_set(int iFunid, int iRefid, int iIssueType, int iSet) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcTdApi, on_recv_pack_end_set, iFunid, iRefid, iIssueType, iSet);
        }
        catch (const pybind11::error_already_set &e)
        {
            std::cout << "on_recv_pack_end_set: " << e.what() << std::endl;
        }
    }

    void on_recv_pack_end_row(int iFunid, int iRefid, int iIssueType, int iSet, int iRow) override
    {
        try
        {
            PYBIND11_OVERLOAD(void, XcTdApi, on_recv_pack_end_row, iFunid, iRefid, iIssueType, iSet, iRow);
        }
        catch (const pybind11::error_already_set &e)
        {
            std::cout << "on_recv_pack_end_row: " << e.what() << std::endl;
        }
    }

};


PYBIND11_MODULE(xctd, m)
{
    pybind11::class_<XcTdApi, PyTdApi> tdapi(m, "XcTdApi", pybind11::module_local());
    tdapi
        .def(pybind11::init<>())
        .def("create_td_api", &XcTdApi::create_td_api)
        .def("create_api", &XcTdApi::create_td_api)
        .def("release", &XcTdApi::release)
        .def("connect", &XcTdApi::connect)
        .def("begin_pack", &XcTdApi::begin_pack)
        .def("end_pack", &XcTdApi::end_pack)
        .def("set_pack_value", &XcTdApi::set_pack_value)
        .def("set_json_value", &XcTdApi::set_json_value)
        .def("send_msg", &XcTdApi::send_msg)
        .def("recv_msg", &XcTdApi::recv_msg)
        .def("get_data_set_count", &XcTdApi::get_data_set_count)
        .def("get_cur_row_count", &XcTdApi::get_cur_row_count)
        .def("get_cur_col_count", &XcTdApi::get_cur_col_count)
        .def("get_cur_data_set", &XcTdApi::get_cur_data_set)
        .def("get_col_name", &XcTdApi::get_col_name)
        .def("get_col_value", &XcTdApi::get_col_value)
        .def("get_next_row", &XcTdApi::get_next_row)
        .def("get_json_value", &XcTdApi::get_json_value)
        .def("get_space", &XcTdApi::get_space)
        .def("get_frequency", &XcTdApi::get_frequency)
        .def("get_last_error_msg", &XcTdApi::get_last_error_msg)

        // some raw api names
        .def("CreateTradeApi", &XcTdApi::create_td_api)
        .def("Release", &XcTdApi::release)
        .def("Connect", &XcTdApi::connect)
        .def("BeginPack", &XcTdApi::begin_pack)
        .def("SetPackValue", &XcTdApi::set_pack_value)
        .def("SetJsonValue", &XcTdApi::set_json_value)
        .def("SendMsg", &XcTdApi::send_msg)
        .def("RecvMsg", &XcTdApi::recv_msg)
        .def("GetDataSetCount", &XcTdApi::get_data_set_count)
        .def("GetCurRowCount", &XcTdApi::get_cur_row_count)
        .def("GetCurColCount", &XcTdApi::get_cur_col_count)
        .def("GetCurDataSet", &XcTdApi::get_cur_data_set)
        .def("GetColName", &XcTdApi::get_col_name)
        .def("GetColValue", &XcTdApi::get_col_value)
        .def("GetNextRow", &XcTdApi::get_next_row)
        .def("GetJsonValue", &XcTdApi::get_json_value)
        .def("GetSpace", &XcTdApi::get_space)
        .def("GetFrequency", &XcTdApi::get_frequency)
        .def("GetLastErrorMsg", &XcTdApi::get_last_error_msg)

        // my wrappered api names
        .def("send_json_data", &XcTdApi::send_json_data)
        .def("recv_json_data", &XcTdApi::recv_json_data)
        .def("get_api_version", &XcTdApi::get_api_version)
        .def("open_debug", &XcTdApi::open_debug)
        .def("write_line", &XcTdApi::write_line)

        // py virtual funcs
        .def("on_front_connected", &XcTdApi::on_front_connected)
        .def("on_front_disconnected", &XcTdApi::on_front_disconnected)
        .def("on_recv_msg", &XcTdApi::on_recv_msg)
        .def("on_recv_pack_msg", &XcTdApi::on_recv_pack_msg)
        .def("on_recv_pack_end_set", &XcTdApi::on_recv_pack_end_set)
        .def("on_recv_pack_end_row", &XcTdApi::on_recv_pack_end_row)
        ;
}
