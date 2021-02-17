#include <time.h>
#include <codecvt>
#include <locale>
#include <iostream>
// #include <condition_variable>

#include "cJSON.h"
#include "ztl_base64.h"
#include "kcbpcli.h"

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
#define KCBPDebugInfo(fd,fmt,...) fprintf(fd,fmt,##__VA_ARGS__)
#else
static  inline void KCBPPrintNull(FILE* fd, const char* fmt, ...) { (void)fd; (void)fmt; }
#define KCBPDebugInfo(fd,fmt,...) KCBPPrintNull(fd,fmt,##__VA_ARGS__)
#endif//MY_DEBUG
#define KCBPDbgFd stdout

#define DEBUG_VIEW_LEVEL        12
#define IS_DBGVIEW(level)       ((level) == (DEBUG_VIEW_LEVEL))


#if 1

using namespace pybind11;

//从字典中获取某个建值对应的整数，并赋值到请求结构体对象的值上
void getInt(const pybind11::dict &d, const char *key, int *value)
{
    if (d.contains(key))		//检查字典中是否存在该键值
    {
        object o = d[key];		//获取该键值
        *value = o.cast<int>();
    }
};


//从字典中获取某个建值对应的浮点数，并赋值到请求结构体对象的值上
void getDouble(const pybind11::dict &d, const char *key, double *value)
{
    if (d.contains(key))
    {
        object o = d[key];
        *value = o.cast<double>();
    }
};


//从字典中获取某个建值对应的字符，并赋值到请求结构体对象的值上
void getChar(const pybind11::dict &d, const char *key, char *value)
{
    if (d.contains(key))
    {
        object o = d[key];
        *value = o.cast<char>();
    }
};


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
};

#endif//0

#if 0
static const char* func_id_desc(int func_id)
{
    switch (func_id)
    {
    default:                        return "unknown";
    }
}
#endif//0

//将GBK编码的字符串转换为UTF8
static std::string to_utf(const std::string &gb2312)
{
#ifdef _MSC_VER
    const static std::locale loc("zh-CN");

    std::vector<wchar_t> wstr(gb2312.size());
    wchar_t* wstrEnd = nullptr;
    const char* gbEnd = nullptr;
    mbstate_t state = {};
    int res = std::use_facet<std::codecvt<wchar_t, char, mbstate_t> >
        (loc).in(state,
            gb2312.data(), gb2312.data() + gb2312.size(), gbEnd,
            wstr.data(), wstr.data() + wstr.size(), wstrEnd);

    if (std::codecvt_base::ok == res)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> cutf8;
        return cutf8.to_bytes(std::wstring(wstr.data(), wstrEnd));
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

static void extract_callctrl(tagCallCtrl* stControl, const pybind11::dict& dctCallCtrl)
{
    getInt(dctCallCtrl, "nFlags", &stControl->nFlags);
    getString(dctCallCtrl, "szId", stControl->szId);
    getString(dctCallCtrl, "szMsgId", stControl->szMsgId);
    getString(dctCallCtrl, "szCorrId", stControl->szCorrId);
    getInt(dctCallCtrl, "nExpiry", &stControl->nExpiry);
    getInt(dctCallCtrl, "nPriority", &stControl->nPriority);
    // tTimeStamp, lpfnCallback
}

static void extract_psctrl(tagKCBPPSControl* stControl, const pybind11::dict& dctCallCtrl)
{
    getInt(dctCallCtrl, "nFlags", &stControl->nFlags);
    getString(dctCallCtrl, "szId", stControl->szId);
    getString(dctCallCtrl, "szMsgId", stControl->szMsgId);
    getString(dctCallCtrl, "szCorrId", stControl->szCorrId);
    getInt(dctCallCtrl, "nExpiry", &stControl->nExpiry);
    getInt(dctCallCtrl, "nPriority", &stControl->nPriority);
    // tTimeStamp, lpfnCallback
}


//////////////////////////////////////////////////////////////////////////
KCBPCli::KCBPCli()
    : m_handle()
    , m_fp()
    , m_log_level(0)
    , m_last_rv(0)
{
}

KCBPCli::~KCBPCli()
{
    if (m_fp)
    {
        fclose(m_fp);
        m_fp = NULL;
    }

    if (m_handle)
    {
        this->DisConnectForce();
        this->Exit();
    }
}

#if 0
void KCBPCli::processTask()
{
    try
    {
        while (m_active)
        {
            // pass
        } // while
    }
    catch (const TerminatedError&)
    {
    }
}

void KCBPCli::_processGetReply()
{
    pybind11::gil_scoped_acquire acquire;
    // TODO:
    this->on_recv_msg(data);
}
#endif


//////////////////////////////////////////////////////////////////////////
int KCBPCli::Init()
{
    if (m_handle)
    {
        printf("Init() release old.\n");
        DisConnectForce();
        Exit();
    }

    return KCBPCLI_Init(&m_handle);
}

int KCBPCli::Exit()
{
    if (m_handle)
    {
        m_last_rv = KCBPCLI_Exit(m_handle);
        m_handle = NULL;
        return m_last_rv;
    }
    return 0;
}

int KCBPCli::GetVersion()
{
    int ver = 0;
    KCBPCLI_GetVersion(m_handle, &ver);
    return ver;
}

int KCBPCli::SetConnectOption(const pybind11::dict& option)
{
    tagKCBPConnectOption stKCBPConnection;
    memset(&stKCBPConnection, 0, sizeof(stKCBPConnection));

    getString(option, "szServerName", stKCBPConnection.szServerName);
    getInt(option, "nProtocal", &stKCBPConnection.nProtocal);
    getString(option, "szAddress", stKCBPConnection.szAddress);
    getInt(option, "nPort", &stKCBPConnection.nPort);
    getString(option, "szSendQName", stKCBPConnection.szSendQName);
    getString(option, "szReceiveQName", stKCBPConnection.szReceiveQName);
    return KCBPCLI_SetConnectOption(m_handle, stKCBPConnection);
}

pybind11::dict KCBPCli::GetConnectOption()
{
    pybind11::dict d;
    tagKCBPConnectOption stKCBPConnection;
    memset(&stKCBPConnection, 0, sizeof(stKCBPConnection));
    KCBPCLI_GetConnectOption(m_handle, &stKCBPConnection);

    d["szServerName"] = stKCBPConnection.szServerName;
    d["nProtocal"] = stKCBPConnection.nProtocal;
    d["szAddress"] = stKCBPConnection.szAddress;
    d["nPort"] = stKCBPConnection.nPort;
    d["szSendQName"] = stKCBPConnection.szSendQName;
    d["szReceiveQName"] = stKCBPConnection.szReceiveQName;

    return d;
}

int KCBPCli::ConnectServer(const std::string& server_name, const std::string& user_name, const std::string& password)
{
    return KCBPCLI_ConnectServer(m_handle, (char*)server_name.c_str(),
        (char*)user_name.c_str(), (char*)password.c_str());
}

int KCBPCli::DisConnect()
{
    return KCBPCLI_DisConnect(m_handle);
}

int KCBPCli::DisConnectForce()
{
    return KCBPCLI_DisConnectForce(m_handle);
}

int KCBPCli::BeginWrite()
{
    return KCBPCLI_BeginWrite(m_handle);
}

/*synchronize call*/
int KCBPCli::CallProgramAndCommit(const std::string& ProgramName)
{
    return KCBPCLI_CallProgramAndCommit(m_handle, (char*)ProgramName.c_str());
}

int KCBPCli::CallProgram(const std::string& ProgramName)
{
    return KCBPCLI_CallProgram(m_handle, (char*)ProgramName.c_str());
}

int KCBPCli::Commit()
{
    return KCBPCLI_Commit(m_handle);
}

int KCBPCli::RollBack()
{
    return KCBPCLI_RollBack(m_handle);
}

/*asynchronize call*/
int KCBPCli::ACallProgramAndCommit(const std::string& ProgramName, pybind11::dict& dctCallCtrl)
{
    tagCallCtrl stControl = { 0 };
    extract_callctrl(&stControl, dctCallCtrl);
    m_last_rv = KCBPCLI_ACallProgramAndCommit(m_handle, (char*)ProgramName.c_str(), &stControl);

    if (IS_DBGVIEW(m_log_level))
    {
        log_debug("ACallProgramAndCommit(%s) rv:%d, szMsgId:%s,szCorrId:%s\n", ProgramName.c_str(), m_last_rv,
            stControl.szMsgId, stControl.szCorrId);
    }

    dctCallCtrl["szMsgId"] = stControl.szMsgId;
    dctCallCtrl["szCorrId"] = stControl.szCorrId;
    dctCallCtrl["tTimeStamp"] = stControl.tTimeStamp;
    return m_last_rv;
}

int KCBPCli::ACallProgram(const std::string& ProgramName, pybind11::dict& dctCallCtrl)
{
    tagCallCtrl stControl = { 0 };
    extract_callctrl(&stControl, dctCallCtrl);
    m_last_rv = KCBPCLI_ACallProgram(m_handle, (char*)ProgramName.c_str(), &stControl);

    if (IS_DBGVIEW(m_log_level))
    {
        log_debug("ACallProgram(%s) rv:%d, szMsgId:%s,szCorrId:%s\n", ProgramName.c_str(), m_last_rv,
            stControl.szMsgId, stControl.szCorrId);
    }

    dctCallCtrl["szMsgId"] = stControl.szMsgId;
    dctCallCtrl["szCorrId"] = stControl.szCorrId;
    dctCallCtrl["tTimeStamp"] = stControl.tTimeStamp;
    return m_last_rv;
}

int KCBPCli::GetReply(pybind11::dict& dctCallCtrl)
{
    tagCallCtrl stControl = { 0 };
    extract_callctrl(&stControl, dctCallCtrl);
    m_last_rv = KCBPCLI_GetReply(m_handle, &stControl);

    if (IS_DBGVIEW(m_log_level))
    {
        log_debug("GetReply() rv:%d, szId:%s,szMsgId:%s,szCorrId:%s\n", m_last_rv,
            stControl.szId, stControl.szMsgId, stControl.szCorrId);
    }

    dctCallCtrl["szId"] = stControl.szId;
    return m_last_rv;
}

int KCBPCli::Cancel(pybind11::dict& dctCallCtrl)
{
    tagCallCtrl stControl = { 0 };
    extract_callctrl(&stControl, dctCallCtrl);
    m_last_rv = KCBPCLI_Cancel(m_handle, &stControl);

    if (IS_DBGVIEW(m_log_level))
    {
        log_debug("Cancel() rv:%d, szId:%s,szMsgId:%s,szCorrId:%s\n", m_last_rv,
            stControl.szId, stControl.szMsgId, stControl.szCorrId);
    }

    dctCallCtrl["szId"] = stControl.szId;
    return m_last_rv;
}

std::string KCBPCli::GetValue(const std::string& KeyName)
{
    char value[16 * 1024] = { 0 };
    m_last_rv = KCBPCLI_GetValue(m_handle, (char*)KeyName.c_str(), value, sizeof(value) - 1);
    return to_utf(value);
}

int KCBPCli::SetValue(const std::string& KeyName, const std::string& Value)
{
    return KCBPCLI_SetValue(m_handle, (char*)KeyName.c_str(), (char*)Value.c_str());
}

int KCBPCli::SetVal(const std::string& KeyName, const std::string& Val)
{
    return KCBPCLI_SetVal(m_handle, (char*)KeyName.c_str(), (unsigned char*)Val.c_str(), (long)Val.length());
}

int KCBPCli::RsCreate(const std::string& Name, int ColNum, const std::string& ColInfo)
{
    return KCBPCLI_RsCreate(m_handle, (char*)Name.c_str(), ColNum, (char*)ColInfo.c_str());
}

int KCBPCli::RsNewTable(const std::string& Name, int ColNum, const std::string& ColInfo)
{
    return KCBPCLI_RsNewTable(m_handle, (char*)Name.c_str(), ColNum, (char*)ColInfo.c_str());
}

int KCBPCli::RsAddRow()
{
    return KCBPCLI_RsAddRow(m_handle);
}

int KCBPCli::RsSaveRow()
{
    return KCBPCLI_RsSaveRow(m_handle);
}

int KCBPCli::RsSetCol(int Col, const std::string& Vlu)
{
    return KCBPCLI_RsSetCol(m_handle, Col, (char*)Vlu.c_str());
}

int KCBPCli::RsSetColByName(const std::string& Name, const std::string& Vlu)
{
    return KCBPCLI_RsSetColByName(m_handle, (char*)Name.c_str(), (char*)Vlu.c_str());
}

int KCBPCli::RsSetVal(int nColumnIndex, const std::string& Value, int nSize)
{
    // FIXME: use unsigned char* Value
    // RsSetVal(int nColumnIndex, unsigned char *pValue, long nSize)
    return KCBPCLI_RsSetVal(m_handle, nColumnIndex, (unsigned char*)Value.c_str(), nSize);
}

int KCBPCli::RsSetValByName(const std::string& ColumnName, const std::string& Value, int nSize)
{
    // RsSetValByName(char* szColumnName, unsigned char *pValue, long nSize)
    return KCBPCLI_RsSetValByName(m_handle, (char*)ColumnName.c_str(), (unsigned char*)Value.c_str(), nSize);
}

int KCBPCli::RsOpen()
{
    return KCBPCLI_RsOpen(m_handle);
}

int KCBPCli::RsMore()
{
    return KCBPCLI_RsMore(m_handle);
}

int KCBPCli::RsClose()
{
    return KCBPCLI_RsClose(m_handle);
}

std::string KCBPCli::RsGetCursorName()
{
    char lCursorName[256] = "";
    m_last_rv = KCBPCLI_RsGetCursorName(m_handle, lCursorName, sizeof(lCursorName) - 1);
    return to_utf(lCursorName);
}

std::string KCBPCli::RsGetColNames()
{
    char lColNames[256] = "";
    m_last_rv = KCBPCLI_RsGetColNames(m_handle, lColNames, sizeof(lColNames) - 1);
    return to_utf(lColNames);
}

std::string KCBPCli::RsGetColName(int nCol)
{
    char lColName[256] = "";
    m_last_rv = KCBPCLI_RsGetColName(m_handle, nCol, lColName, sizeof(lColName) - 1);
    return to_utf(lColName);
}

int KCBPCli::RsFetchRow()
{
    return KCBPCLI_RsFetchRow(m_handle);
}

int KCBPCli::RsFetchRowScroll(int nOrientation, int nOffset)
{
    return KCBPCLI_RsFetchRowScroll(m_handle, nOrientation, nOffset);
}

std::string KCBPCli::RsGetCol(int Col)
{
    char lColName[256] = "";
    m_last_rv = KCBPCLI_RsGetCol(m_handle, Col, lColName);
    return to_utf(lColName);
}

std::string KCBPCli::RsGetColByName(const std::string& KeyName)
{
    char value[16 * 1024] = "";
    m_last_rv = KCBPCLI_RsGetColByName(m_handle, (char*)KeyName.c_str(), value);
    return to_utf(value);
}

pybind11::bytes KCBPCli::RsGetVal(int nColumnIndex)
{
    // int RsGetVal(int nColumnIndex, unsigned char **pValue, long *pSize);
    unsigned char* pValue = NULL;
    long nSize = 0;
    m_last_rv = KCBPCLI_RsGetVal(m_handle, nColumnIndex, &pValue, &nSize);
    if (pValue) {
        pybind11::bytes ret((char*)pValue, m_last_rv);
        KCBPCLI_FreeMemory(m_handle, pValue);
        return ret;
    }
    return pybind11::bytes("");
}

std::string KCBPCli::RsGetValByName(const std::string& ColumnName)
{
    // int RsGetValByName(char *szColumnName, unsigned char **pValue, long *pSize);
    unsigned char* pValue = NULL;
    long nSize = 0;
    m_last_rv = KCBPCLI_RsGetValByName(m_handle, (char*)ColumnName.c_str(), &pValue, &nSize);
    std::string ret = to_utf((char*)pValue);  // FIXME
    KCBPCLI_FreeMemory(m_handle, pValue);

    return ret;
}

int KCBPCli::RsGetRowNum()
{
    int rows = -1;
    m_last_rv = KCBPCLI_RsGetRowNum(m_handle, &rows);
    return rows;
}

int KCBPCli::RsGetColNum()
{
    int cols = -1;
    m_last_rv = KCBPCLI_RsGetColNum(m_handle, &cols);
    return cols;
}

int KCBPCli::RsGetTableRowNum(int nt)
{
    int rows = -1;
    m_last_rv = KCBPCLI_RsGetTableRowNum(m_handle, nt, &rows);
    return rows;
}

int KCBPCli::RsGetTableColNum(int nt)
{
    int cols = -1;
    m_last_rv = KCBPCLI_RsGetTableColNum(m_handle, nt, &cols);
    return cols;
}


/*misc*/
int KCBPCli::GetErrorCode()
{
    int lErrorCode = -1;
    KCBPCLI_GetErrorCode(m_handle, &lErrorCode);
    return lErrorCode;
}

std::string KCBPCli::GetErrorMsg()
{
    char lErrorMsg[512] = "";
    KCBPCLI_GetErrorMsg(m_handle, lErrorMsg);
    return to_utf(lErrorMsg);
}

int KCBPCli::GetCommLen()
{
    int len = 0;
    m_last_rv = KCBPCLI_GetCommLen(m_handle, &len);
    return len;
}

int KCBPCli::SetCliTimeOut(int TimeOut)
{
    return KCBPCLI_SetCliTimeOut(m_handle, TimeOut);
}

int KCBPCli::SetOptionsInt(int nIndex, int Value)
{
    return KCBPCLI_SetOptions(m_handle, nIndex, &Value, sizeof(Value));
}

int KCBPCli::SetOptionsStr(int nIndex, const std::string& Value)
{
    return KCBPCLI_SetOptions(m_handle, nIndex, (void*)Value.c_str(), (int)Value.length());
}

int KCBPCli::SetOptionsDict(int nIndex, const pybind11::dict& Value)
{
    if (nIndex == KCBP_OPTION_CONNECT)
    {
        tagKCBPConnectOption stKCBPConnection;
        memset(&stKCBPConnection, 0, sizeof(stKCBPConnection));

        getString(Value, "szServerName", stKCBPConnection.szServerName);
        getInt(Value, "nProtocal", &stKCBPConnection.nProtocal);
        getString(Value, "szAddress", stKCBPConnection.szAddress);
        getInt(Value, "nPort", &stKCBPConnection.nPort);
        getString(Value, "szSendQName", stKCBPConnection.szSendQName);
        getString(Value, "szReceiveQName", stKCBPConnection.szReceiveQName);
        getString(Value, "szReserved", stKCBPConnection.szReserved);
        return KCBPCLI_SetOptions(m_handle, nIndex ,&stKCBPConnection, sizeof(stKCBPConnection));
    }
    else if (nIndex == KCBP_OPTION_CONNECT_EX)
    {
        tagKCBPConnectOptionEx stKCBPConnection;
        memset(&stKCBPConnection, 0, sizeof(stKCBPConnection));

        getString(Value, "szServerName", stKCBPConnection.szServerName);
        getInt(Value, "nProtocal", &stKCBPConnection.nProtocal);
        getString(Value, "szAddress", stKCBPConnection.szAddress);
        getInt(Value, "nPort", &stKCBPConnection.nPort);
        getString(Value, "szSendQName", stKCBPConnection.szSendQName);
        getString(Value, "szReceiveQName", stKCBPConnection.szReceiveQName);
        getString(Value, "szReserved", stKCBPConnection.szReserved);
        getString(Value, "szProxy", stKCBPConnection.szProxy);
        getString(Value, "szSSL", stKCBPConnection.szSSL);
        return KCBPCLI_SetOptions(m_handle, nIndex, &stKCBPConnection, sizeof(stKCBPConnection));
    }
    return -1;
}

int KCBPCli::GetOptionsInt(int nIndex)
{
    int val = -1;
    int len = sizeof(val);
    m_last_rv = KCBPCLI_GetOptions(m_handle, nIndex, &val, &len);
    return val;
}

std::string KCBPCli::GetOptionsStr(int nIndex)
{
    char val[16 * 1024] = { 0 };
    int len = sizeof(val) - 1;
    m_last_rv = KCBPCLI_GetOptions(m_handle, nIndex, val, &len);
    return to_utf(val);
}

pybind11::dict KCBPCli::GetOptionsDict(int nIndex)
{
    pybind11::dict d;
    if (nIndex == KCBP_OPTION_CONNECT || nIndex == KCBP_OPTION_CURRENT_CONNECT)
    {
        tagKCBPConnectOption stKCBPConnection;
        memset(&stKCBPConnection, 0, sizeof(stKCBPConnection));
        int len = sizeof(stKCBPConnection);
        m_last_rv = KCBPCLI_GetOptions(m_handle, nIndex, &stKCBPConnection, &len);

        d["szServerName"] = stKCBPConnection.szServerName;
        d["nProtocal"] = stKCBPConnection.nProtocal;
        d["szAddress"] = stKCBPConnection.szAddress;
        d["nPort"] = stKCBPConnection.nPort;
        d["szSendQName"] = stKCBPConnection.szSendQName;
        d["szReceiveQName"] = stKCBPConnection.szReceiveQName;
        d["szReserved"] = stKCBPConnection.szReserved;
    }
    else if (nIndex == KCBP_OPTION_CONNECT_EX)
    {
        tagKCBPConnectOptionEx stKCBPConnection;
        memset(&stKCBPConnection, 0, sizeof(stKCBPConnection));
        int len = sizeof(stKCBPConnection);
        m_last_rv = KCBPCLI_GetOptions(m_handle, nIndex, &stKCBPConnection, &len);

        d["szServerName"] = stKCBPConnection.szServerName;
        d["nProtocal"] = stKCBPConnection.nProtocal;
        d["szAddress"] = stKCBPConnection.szAddress;
        d["nPort"] = stKCBPConnection.nPort;
        d["szSendQName"] = stKCBPConnection.szSendQName;
        d["szReceiveQName"] = stKCBPConnection.szReceiveQName;
        d["szReserved"] = stKCBPConnection.szReserved;
        d["szProxy"] = stKCBPConnection.szProxy;
        d["szSSL"] = stKCBPConnection.szSSL;
    }
    return d;
}

int KCBPCli::SetSystemParam(int nIndex, const std::string& Value)
{
    return KCBPCLI_SetSystemParam(m_handle, nIndex, (char*)Value.c_str());
}

std::string KCBPCli::GetSystemParam(int nIndex)
{
    char val[16 * 1024] = { 0 };
    m_last_rv = KCBPCLI_GetSystemParam(m_handle, nIndex, val, sizeof(val) - 1);
    return to_utf(val);
}

/*pub/sub*/
int KCBPCli::Subscribe(pybind11::dict& stPSCtl, const std::string& TopicExpr, const std::string& FilterExpr)
{
    tagKCBPPSControl stControl;
    memset(&stControl, 0, sizeof(stControl));
    extract_psctrl(&stControl, stPSCtl);
    m_last_rv = KCBPCLI_Subscribe(m_handle, &stControl,
        (char*)TopicExpr.c_str(), (char*)FilterExpr.c_str());

    stPSCtl["szId"] = stControl.szId;
    stPSCtl["szMsgId"] = stControl.szMsgId;
    stPSCtl["szCorrId"] = stControl.szCorrId;
    stPSCtl["tTimeStamp"] = stControl.tTimeStamp;

    return m_last_rv;
}

int KCBPCli::Unsubscribe(pybind11::dict& stPSCtl)
{
    tagKCBPPSControl stControl;
    memset(&stControl, 0, sizeof(stControl));
    extract_psctrl(&stControl, stPSCtl);
    return KCBPCLI_Unsubscribe(m_handle, &stControl);
}

std::string KCBPCli::ReceivePublication(pybind11::dict& stPSCtl)
{
    char szData[16 * 1024] = "";
    tagKCBPPSControl stControl;
    memset(&stControl, 0, sizeof(stControl));
    extract_psctrl(&stControl, stPSCtl);

    m_last_rv = KCBPCLI_ReceivePublication(m_handle, &stControl, szData, sizeof(szData) - 1);
    return to_utf(szData);
}

int KCBPCli::RegisterPublisher(pybind11::dict& stPSCtl, const std::string& TopicExpr)
{
    tagKCBPPSControl stControl;
    memset(&stControl, 0, sizeof(stControl));
    extract_psctrl(&stControl, stPSCtl);
    m_last_rv = KCBPCLI_RegisterPublisher(m_handle, &stControl, (char*)TopicExpr.c_str());

    stPSCtl["szId"] = stControl.szId;
    stPSCtl["tTimeStamp"] = stControl.tTimeStamp;

    return m_last_rv;
}

int KCBPCli::DeregisterPublisher(pybind11::dict& stPSCtl)
{
    tagKCBPPSControl stControl;
    memset(&stControl, 0, sizeof(stControl));
    extract_psctrl(&stControl, stPSCtl);
    return KCBPCLI_DeregisterPublisher(m_handle, &stControl);
}

int KCBPCli::Publish(pybind11::dict& stPSCtl, const std::string& TopicExpr, const std::string& Data, int nDataLen)
{
    tagKCBPPSControl stControl;
    memset(&stControl, 0, sizeof(stControl));
    extract_psctrl(&stControl, stPSCtl);
    return KCBPCLI_Publish(m_handle, &stControl, (char*)TopicExpr.c_str(), (char*)Data.c_str(), nDataLen);
}

/*broadcast/notify*/
#if 0
int KCBPCli::Notify(const std::string& ConnectionId, const std::string& Data, int nDataLen, int nFlags)
{
    return KCBPCLI_Notify(m_handle, (char*)ConnectionId.c_str(), (char*)Data.c_str(), nDataLen, nFlags);
}

int KCBPCli::Broadcast(const std::string& MachineId, const std::string& UserName, const std::string& ConnectionId, const std::string& Data, int nDataLen, int nFlags)
{
    return KCBPCLI_Broadcast(m_handle, (char*)MachineId.c_str(), (char*)UserName.c_str(), (char*)ConnectionId.c_str(),
        (char*)Data.c_str(), nDataLen, nFlags);
}

int KCBPCli::CheckUnsoliciety()
{
    return KCBPCLI_CheckUnsoliciety(m_handle);
}

int KCBPCli::SetUnsoliciety()
{
    return KCBPCLI_SetUnsoliciety(m_handle);
}
#endif

/*ssl*/
int KCBPCli::SSLVerifyCertPasswd(const std::string& KeyFileName, const std::string& Password)
{
    return KCBPCLI_SSLVerifyCertPasswd(m_handle, (char*)KeyFileName.c_str(), (char*)Password.c_str());
}

int KCBPCli::SSLModifyCertPasswd(const std::string& KeyFileName, const std::string& OldPassword, const std::string& NewPassword)
{
    return KCBPCLI_SSLModifyCertPasswd(m_handle, (char*)KeyFileName.c_str(), (char*)OldPassword.c_str(), (char*)NewPassword.c_str());
}

#if 0
int KCBPCli::SSLEncrypt(const std::string& KeyFileName, const std::string& Password, unsigned char *pInput, int nInLen, unsigned char **pOutput, int *pnOutLen)
{}
int KCBPCli::SSLDecrypt(const std::string& KeyFileName, const std::string& Password, unsigned char *pInput, int nInLen, unsigned char **pOutput, int *pnOutLen)
{}
int KCBPCli::FreeMemory(void *memblock)
{}
#endif

std::string KCBPCli::SSLGetCertInfo(const std::string& CertFileName, int nInfoType, int nInfoSubType)
{
    char szBuf[8192] = "";
    m_last_rv = KCBPCLI_SSLGetCertInfo(m_handle, (char*)CertFileName.c_str(), nInfoType, nInfoSubType, szBuf, sizeof(szBuf) - 1);
    return to_utf(szBuf);
}


int KCBPCli::get_last_rv()
{
    return m_last_rv;
}

std::string KCBPCli::get_api_version()
{
    return std::string(KCBPCLI_API_VERSION);
}

std::string KCBPCli::get_rs_as_json()
{
    int nRow, nCol;
    char szCode[13] = { 0 };
    char szMsg[256] = { 0 };
    char szColNames[256], szColName[256], szDat[1024];
    char* json_str;
    std::string ret;
    cJSON* root;
    cJSON* array;
    cJSON* json_data;

    m_last_rv = KCBPCLI_RsOpen(m_handle);
    if (m_last_rv != 0)
    {
        printf("error: KCBPCLI_RsOpen return %d\n", m_last_rv);
        log_debug("get_rs_as_json() error KCBPCLI_RsOpen return %d\n", m_last_rv);
        goto GET_RS_END;
    }

    m_last_rv = KCBPCLI_RsFetchRow(m_handle);
    if (m_last_rv != 0)
    {
        printf("error: KCBPCLI_RsFetchRow return %d\n", m_last_rv);
        log_debug("get_rs_as_json() error KCBPCLI_RsFetchRow return %d\n", m_last_rv);
        goto GET_RS_END;
    }

    szColNames[0] = 0;
    KCBPCLI_RsGetColNames(m_handle, szColNames, sizeof(szColNames) - 1);
    if(strstr(szColNames, "MSG_CODE") != NULL)
    {
        // UNIX风格
        // KCBPCLI_RsGetColByName( m_handle, "MSG_LEVEL", szLevel );
        KCBPCLI_RsGetColByName(m_handle, "MSG_TEXT", szMsg );
        m_last_rv = KCBPCLI_RsGetColByName(m_handle, "MSG_CODE", szCode);
    }
    else
    {
        // Win风格
        // KCBPCLI_RsGetColByName( m_handle, "LEVEL", szLevel );
        KCBPCLI_RsGetColByName(m_handle, "MSG", szMsg);
        m_last_rv = KCBPCLI_RsGetColByName(m_handle, "CODE", szCode);
    }
    if (m_last_rv != 0)
    {
        printf("get code fail, ret=%d\n ", m_last_rv);
        log_debug("get_rs_as_json() get code fail, szColNames:%s, ret=%d\n", szColNames, m_last_rv);
        goto GET_RS_END;
    }
    if (atoi(szCode) != 0)
    {
        printf("LBM Exec error code:%s, msg:%s\n ", szCode, szMsg);
        log_debug("get_rs_as_json() error code:%s, msg:%s\n", szCode, szMsg);
        goto GET_RS_END;
    }

    //查询是否存在后续结果集
    m_last_rv = KCBPCLI_RsMore(m_handle);
    if(m_last_rv != 0)
    {
        log_debug("get_rs_as_json() error KCBPCLI_RsMore return %d\n", m_last_rv);
        goto GET_RS_END;
    }

    root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "CODE", 0);
    cJSON_AddStringToObject(root, "MSG", szMsg);
    array = cJSON_AddArrayToObject(root, "json");

    nRow = 0;
    while(1)
    {
        m_last_rv = KCBPCLI_RsFetchRow(m_handle);
        if(m_last_rv != 0)
        {
            break;
        }

        if (IS_DBGVIEW(m_log_level))
        {
            printf("get_rs_as_json() row %d\n", nRow++);
        }

        nCol = 0;
        json_data = cJSON_CreateObject();
        KCBPCLI_RsGetColNum(m_handle, &nCol);
        for( int j = 1; j <= nCol; j++ )
        {
            KCBPCLI_RsGetColName(m_handle, j, szColName, sizeof(szColName) - 1);
            KCBPCLI_RsGetColByName(m_handle, szColName, szDat);

            if (IS_DBGVIEW(m_log_level))
            {
                printf("get_rs_as_json(): %s = %s\n", szColName, szDat);
            }

            cJSON_AddStringToObject(json_data, szColName, szDat);
        }
        // printf( "\n" );

        cJSON_AddItemToArray(array, json_data);
    }

    json_str = cJSON_Print(root);
    ret = to_utf(json_str);

    cJSON_free(json_str);
    cJSON_Delete(root);

GET_RS_END:
    KCBPCLI_RsClose(m_handle);
    return ret;
}

int KCBPCli::set_value_encode(const std::string& KeyName, const std::string& Value,
    const std::string& EncodeKey, int EncodeLevel)
{
    int rv;
    unsigned char lDestData[1000] = { 0 };

    if (EncodeLevel <= 0) {
        EncodeLevel = KDCOMPLEX_ENCODE;
    }

    if (!EncodeKey.empty())
    {
        rv = KDEncode(EncodeLevel,
            (unsigned char*)Value.c_str(), (int)Value.length(),
            lDestData, sizeof(lDestData) - 1,
            (void*)EncodeKey.c_str(), (int)EncodeKey.length());
    }
    else
    {
        rv = (int)Value.length();
        rv = rv < sizeof(lDestData) ? rv : sizeof(lDestData) - 1;
        memcpy(lDestData, Value.c_str(), rv);
    }

    if (IS_DBGVIEW(m_log_level))
    {
        log_debug("set_value_encode() KDEncode rv:%d\n", rv);
    }

    return KCBPCLI_SetValue(m_handle, (char*)KeyName.c_str(), (char*)lDestData);
}

int KCBPCli::set_ssl_key_password(const std::string& KeyFileName, const std::string& Password)
{
    m_SSLKeyFileName = KeyFileName;
    m_SSLPassword = Password;
    return 0;
}

int KCBPCli::set_value_sslencrypt(const std::string& KeyName, const std::string& Value)
{
    if (!m_SSLKeyFileName.empty() && !m_SSLPassword.empty())
    {
        return -1;
    }

    unsigned char* lpOutput;
    int lOutLen;
    m_last_rv = KCBPCLI_SSLEncrypt(m_handle, (char*)m_SSLKeyFileName.c_str(), (char*)m_SSLPassword.c_str(),
        (unsigned char*)Value.c_str(), (int)Value.length(), &lpOutput, &lOutLen);
    if (m_last_rv == 0)
    {
        m_last_rv = KCBPCLI_SetVal(m_handle, (char*)KeyName.c_str(), lpOutput, lOutLen);
        KCBPCLI_FreeMemory(m_handle, lpOutput);
    }
    return m_last_rv;
}

int KCBPCli::rs_set_value_sslencrypt(int ColumnIndex, const std::string& Value)
{
    if (!m_SSLKeyFileName.empty() && !m_SSLPassword.empty())
    {
        return -1;
    }

    unsigned char* lpOutput;
    int lOutLen;
    m_last_rv = KCBPCLI_SSLEncrypt(m_handle, (char*)m_SSLKeyFileName.c_str(), (char*)m_SSLPassword.c_str(),
        (unsigned char*)Value.c_str(), (int)Value.length(), &lpOutput, &lOutLen);

    if (m_last_rv == 0)
    {
        m_last_rv = KCBPCLI_RsSetVal(m_handle, ColumnIndex, lpOutput, lOutLen);
        KCBPCLI_FreeMemory(m_handle, lpOutput);
    }
    return m_last_rv;
}

int KCBPCli::rs_set_value_byname_sslencrypt(const std::string& ColumnName, const std::string& Value)
{
    if (!m_SSLKeyFileName.empty() && !m_SSLPassword.empty())
    {
        return -1;
    }

    unsigned char* lpOutput;
    int lOutLen;
    m_last_rv = KCBPCLI_SSLEncrypt(m_handle, (char*)m_SSLKeyFileName.c_str(), (char*)m_SSLPassword.c_str(),
        (unsigned char*)Value.c_str(), (int)Value.length(), &lpOutput, &lOutLen);

    if (m_last_rv == 0)
    {
        m_last_rv = KCBPCLI_RsSetValByName(m_handle, (char*)ColumnName.c_str(), lpOutput, lOutLen);
        KCBPCLI_FreeMemory(m_handle, lpOutput);
    }
    return m_last_rv;
}

int KCBPCli::is_disconnected(int code)
{
    // 2054、2055、2003、200
    if (code == 2054 || code == 2055 || code == 2003 || code == 200)
    {
        return 1;
    }

    return 0;
}

pybind11::bytes KCBPCli::kd_encode(int nEncode_Level, const std::string& SrcData, const std::string& EncodeKey)
{
    int rv;
    unsigned char lDestData[1000] = { 0 };
    rv = KDEncode(nEncode_Level,
        (unsigned char*)SrcData.c_str(), (int)SrcData.length(),
        lDestData, sizeof(lDestData) - 1,
        (void*)EncodeKey.c_str(), (int)EncodeKey.length());

    return pybind11::bytes((char*)lDestData, rv);
}

int KCBPCli::open_debug(std::string log_file, int log_level)
{
    m_log_level = log_level;
    if (IS_DBGVIEW(m_log_level)) {
        log_debug("kcbpcli open_debug log_file:%s, log_level:%d, ver:%s",
            log_file.c_str(), log_level, KCBPCLI_API_VERSION);
    }

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
        sprintf(filename, "kcbpcli_%04d%02d%02d.con",
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

int KCBPCli::write_line(int reserve, const std::string& line)
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

int KCBPCli::write_log(int reserve, const char* fmt, ...)
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

class PyKCBPCli : public KCBPCli
{
public:
    using KCBPCli::KCBPCli;

    void on_front_connected() override
    {
        try
        {
            PYBIND11_OVERLOAD(void, KCBPCli, on_front_connected);
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
            PYBIND11_OVERLOAD(void, KCBPCli, on_front_disconnected, reqid);
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
            PYBIND11_OVERLOAD(void, KCBPCli, on_recv_msg, msg);
        }
        catch (const pybind11::error_already_set &e)
        {
            std::cout << "on_recv_msg: " << e.what() << std::endl;
        }
    }

};


PYBIND11_MODULE(kcbpcli, m)
{
    pybind11::class_<KCBPCli, PyKCBPCli> tdapi(m, "KCBPCli", pybind11::module_local());
    tdapi
        .def(pybind11::init<>())

        // some raw api names
        .def("Init", &KCBPCli::Init)
        .def("Exit", &KCBPCli::Exit)
        .def("GetVersion", &KCBPCli::GetVersion)
        .def("SetConnectOption", &KCBPCli::SetConnectOption)
        .def("GetConnectOption", &KCBPCli::GetConnectOption)
        .def("ConnectServer", &KCBPCli::ConnectServer)
        .def("DisConnect", &KCBPCli::DisConnect)
        .def("DisConnectForce", &KCBPCli::DisConnectForce)
        .def("BeginWrite", &KCBPCli::BeginWrite)
        .def("CallProgramAndCommit", &KCBPCli::CallProgramAndCommit)
        .def("CallProgram", &KCBPCli::CallProgram)
        .def("Commit", &KCBPCli::Commit)
        .def("RollBack", &KCBPCli::RollBack)
        .def("ACallProgramAndCommit", &KCBPCli::ACallProgramAndCommit)
        .def("ACallProgram", &KCBPCli::ACallProgram)
        .def("GetReply", &KCBPCli::GetReply)
        .def("Cancel", &KCBPCli::Cancel)
        .def("GetValue", &KCBPCli::GetValue)
        .def("SetValue", &KCBPCli::SetValue)
        .def("GetErrorCode", &KCBPCli::GetErrorCode)
        .def("GetErrorMsg", &KCBPCli::GetErrorMsg)
        .def("GetCommLen", &KCBPCli::GetCommLen)
        .def("SetCliTimeOut", &KCBPCli::SetCliTimeOut)
        .def("SetSystemParam", &KCBPCli::SetSystemParam)
        .def("GetSystemParam", &KCBPCli::GetSystemParam)

        /*rs*/
        .def("RsCreate", &KCBPCli::RsCreate)
        .def("RsNewTable", &KCBPCli::RsNewTable)
        .def("RsAddRow", &KCBPCli::RsAddRow)
        .def("RsSaveRow", &KCBPCli::RsSaveRow)
        .def("RsSetCol", &KCBPCli::RsSetCol)
        .def("RsSetVal", &KCBPCli::RsSetVal)
        .def("RsSetValByName", &KCBPCli::RsSetValByName)
        .def("RsSetColByName", &KCBPCli::RsSetColByName)
        .def("RsOpen", &KCBPCli::RsOpen)
        .def("RsMore", &KCBPCli::RsMore)
        .def("RsClose", &KCBPCli::RsClose)
        .def("RsGetCursorName", &KCBPCli::RsGetCursorName)
        .def("RsGetColNames", &KCBPCli::RsGetColNames)
        .def("RsGetColName", &KCBPCli::RsGetColName)
        .def("RsFetchRow", &KCBPCli::RsFetchRow)
        .def("RsFetchRowScroll", &KCBPCli::RsFetchRowScroll)
        .def("RsGetCol", &KCBPCli::RsGetCol)
        .def("RsGetColByName", &KCBPCli::RsGetColByName)
        .def("RsGetVal", &KCBPCli::RsGetVal)
        .def("RsGetValByName", &KCBPCli::RsGetValByName)
        .def("RsGetRowNum", &KCBPCli::RsGetRowNum)
        .def("RsGetColNum", &KCBPCli::RsGetColNum)
        .def("RsGetTableRowNum", &KCBPCli::RsGetTableRowNum)
        .def("RsGetTableColNum", &KCBPCli::RsGetTableColNum)

        /*pub/sub*/
        .def("Subscribe", &KCBPCli::Subscribe)
        .def("Unsubscribe", &KCBPCli::Unsubscribe)
        .def("ReceivePublication", &KCBPCli::ReceivePublication)
        .def("RegisterPublisher", &KCBPCli::RegisterPublisher)
        .def("DeregisterPublisher", &KCBPCli::DeregisterPublisher)
        .def("Publish", &KCBPCli::Publish)

        /*broadcast/notify*/
#if 0
        .def("Notify", &KCBPCli::Notify)
        .def("Broadcast", &KCBPCli::Broadcast)
        .def("CheckUnsoliciety", &KCBPCli::CheckUnsoliciety)
        .def("SetUnsoliciety", &KCBPCli::SetUnsoliciety)
#endif

        /*ssl*/
        .def("SSLVerifyCertPasswd", &KCBPCli::SSLVerifyCertPasswd)
        .def("SSLModifyCertPasswd", &KCBPCli::SSLModifyCertPasswd)
        // .def("SSLEncrypt", &KCBPCli::SSLEncrypt)
        // .def("SSLDecrypt", &KCBPCli::SSLDecrypt)
        // .def("FreeMemory", &KCBPCli::FreeMemory)
        .def("SSLGetCertInfo", &KCBPCli::SSLGetCertInfo)

        .def("SetOptionsInt", &KCBPCli::SetOptionsInt)
        .def("SetOptionsStr", &KCBPCli::SetOptionsStr)
        .def("SetOptionsDict", &KCBPCli::SetOptionsDict)
        .def("GetOptionsInt", &KCBPCli::GetOptionsInt)
        .def("GetOptionsStr", &KCBPCli::GetOptionsStr)
        .def("GetOptionsDict", &KCBPCli::GetOptionsDict)

        // my wrappered api names
        .def("get_last_rv", &KCBPCli::get_last_rv)
        .def("get_rs_as_json", &KCBPCli::get_rs_as_json)
        .def("set_value_encode", &KCBPCli::set_value_encode)
        .def("set_ssl_key_password", &KCBPCli::set_ssl_key_password)
        .def("set_value_sslencrypt", &KCBPCli::set_value_sslencrypt)
        .def("rs_set_value_sslencrypt", &KCBPCli::rs_set_value_sslencrypt)
        .def("rs_set_value_byname_sslencrypt", &KCBPCli::rs_set_value_byname_sslencrypt)
        .def("is_disconnected", &KCBPCli::is_disconnected)
        .def("kd_encode", &KCBPCli::kd_encode)

        .def("get_api_version", &KCBPCli::get_api_version)
        .def("open_debug", &KCBPCli::open_debug)
        .def("write_line", &KCBPCli::write_line)

        // py virtual funcs
#if 0
        .def("on_front_connected", &KCBPCli::on_front_connected)
        .def("on_front_disconnected", &KCBPCli::on_front_disconnected)
        .def("on_recv_msg", &KCBPCli::on_recv_msg)
#endif
        ;
}
