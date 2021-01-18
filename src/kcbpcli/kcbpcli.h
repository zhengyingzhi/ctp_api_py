#pragma warning(disable:4200)
#pragma once

#include <stdio.h>
#include <string.h>

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>

#include <pybind11/pybind11.h>
#include <KCBPCLI3/KCBPCLI.h>
#include <KCBPCLI3/KDEncodeCli.h>

// #include <nlohmann/json.hpp>
// using json = nlohmann::json;

// using std::string;
// using std::map;
// using namespace std;
// using namespace pybind11;


#define KCBPCLI_API_VERSION     "1.0.0"

#define KCBP_ERR_REPLY_TIMEOUT  2001    // 接收应答超时
#define KCBP_ERR_TRANS_FAULT    2082    // 数据通讯错误


class TerminatedError : std::exception
{};


class KCBPCli
{
public:
    KCBPCli();
    ~KCBPCli();

    KCBPCLIHANDLE   m_handle;
    FILE*           m_fp;
    int             m_log_level;
    int             m_last_rv;

    std::string     m_SSLKeyFileName;
    std::string     m_SSLPassword;

#if 0
    std::thread     m_task_thread;
    TaskQueue       m_task_queue;
    bool            m_active = false;

private:
    void processTask();
    void _processGetReply();
#endif

public:
    virtual void on_front_connected() {}
    virtual void on_front_disconnected(int reason) {}
    virtual void on_recv_msg(const std::string& msg) {}

public:
    // KCBPCLI_Init
    int Init();

    int Exit();

    int GetVersion();

    // deprecated
    int SetConnectOption(const pybind11::dict& option);
    pybind11::dict GetConnectOption();

    int ConnectServer(const std::string& server_name, const std::string& user_name, const std::string& password);
    int DisConnect();
    int DisConnectForce();

    int BeginWrite();

    /*synchronize call*/
    int CallProgramAndCommit(const std::string& ProgramName);
    int CallProgram(const std::string& ProgramName);
    int Commit();
    int RollBack();

    /*asynchronize call*/
    int ACallProgramAndCommit(const std::string& ProgramName, pybind11::dict& dctCallCtrl);
    int ACallProgram(const std::string& ProgramName, pybind11::dict& dctCallCtrl);
    int GetReply(pybind11::dict& dctCallCtrl);
    int Cancel(pybind11::dict& dctCallCtrl);

    std::string GetValue(const std::string& KeyName);
    int SetValue(const std::string& KeyName, const std::string& Value);

    /*rs*/
    int RsCreate(const std::string& Name, int ColNum, const std::string& ColInfo);
    int RsNewTable(const std::string& Name, int ColNum, const std::string& ColInfo);
    int RsAddRow();
    int RsSaveRow();
    int RsSetCol(int Col, const std::string& Vlu);
    int RsSetColByName(const std::string& Name, const std::string& Vlu);
    int RsSetVal(int nColumnIndex, const std::string& Value, int nSize);
    int RsSetValByName(const std::string& ColumnName, const std::string& Value, int nSize);
    int RsOpen();
    int RsMore();
    int RsClose();
    std::string RsGetCursorName();
    std::string RsGetColNames();
    std::string RsGetColName(int nCol);
    int RsFetchRow();
    int RsFetchRowScroll(int nOrientation, int nOffset);
    std::string RsGetCol(int Col);
    std::string RsGetColByName(const std::string& KeyName);
    std::string RsGetVal(int nColumnIndex);
    std::string RsGetValByName(const std::string& ColumnName);
    int RsGetRowNum();
    int RsGetColNum();
    int RsGetTableRowNum(int nt);
    int RsGetTableColNum(int nt);

    // TODO: only provide json apis ?

    /*misc*/
    // int GetErr(int *pErrCode, char *ErrMsg);
    int GetErrorCode();
    std::string GetErrorMsg();
    int GetCommLen();

    int SetCliTimeOut(int TimeOut);
#if 0
    int SetOptions(int nIndex, void *pValue, int nLen);
    int GetOptions(int nIndex, void *pValue, int *nLen);
#else
    int SetOptionsInt(int nIndex, int Value);
    int SetOptionsStr(int nIndex, const std::string& Value);
    int SetOptionsDict(int nIndex, const pybind11::dict& Value);
    int GetOptionsInt(int nIndex);
    std::string GetOptionsStr(int nIndex);
    pybind11::dict GetOptionsDict(int nIndex);
#endif

    int SetSystemParam(int nIndex, const std::string& Value);
    std::string GetSystemParam(int nIndex);

    /*pub/sub*/
    int Subscribe(pybind11::dict& stPSCtl, const std::string& TopicExpr, const std::string& FilterExpr);
    int Unsubscribe(pybind11::dict& stPSCtl);
    std::string ReceivePublication(pybind11::dict& stPSCtl);
    int RegisterPublisher(pybind11::dict& stPSCtl, const std::string& TopicExpr);
    int DeregisterPublisher(pybind11::dict& stPSCtl);
    int Publish(pybind11::dict& stPSCtl, const std::string& TopicExpr, const std::string& Data, int nDataLen);

    /*broadcast/notify*/
#if 0
    int Notify(const std::string& ConnectionId, const std::string& Data, int nDataLen, int nFlags);
    int Broadcast(const std::string& MachineId, const std::string& UserName, const std::string& ConnectionId, const std::string& Data, int nDataLen, int nFlags);
    int CheckUnsoliciety();
    int SetUnsoliciety();
#endif

    /*ssl*/
    int SSLVerifyCertPasswd(const std::string& KeyFileName, const std::string& Password);
    int SSLModifyCertPasswd(const std::string& KeyFileName, const std::string& OldPassword, const std::string& NewPassword);
#if 0 // TODO:
    int SSLEncrypt(const std::string& KeyFileName, const std::string& Password, unsigned char *pInput, int nInLen, unsigned char **pOutput, int *pnOutLen);
    int SSLDecrypt(const std::string& KeyFileName, const std::string& Password, unsigned char *pInput, int nInLen, unsigned char **pOutput, int *pnOutLen);
    int FreeMemory(void *memblock);
#endif//0
    std::string  SSLGetCertInfo(const std::string& CertFileName, int nInfoType, int nInfoSubType);

    /*wrappered funcs*/
    int get_last_rv();
    std::string get_rs_as_json();
    int set_value_encode(const std::string& KeyName, const std::string& Value, const std::string& EncodeKey, int EncodeLevel);
    int set_ssl_key_password(const std::string& KeyFileName, const std::string& Password);
    int set_value_sslencrypt(const std::string& KeyName, const std::string& Value);
    int rs_set_value_sslencrypt(int ColumnIndex, const std::string& Value);
    int rs_set_value_byname_sslencrypt(const std::string& ColumnName, const std::string& Value);
    int is_disconnected(int code);

    /*wrappered kdencode*/
    std::string kd_encode(int nEncode_Level, const std::string& SrcData, const std::string& EncodeKey);

    std::string get_api_version();

    int open_debug(std::string log_file, int log_level);

    // write msg into log file
    int write_line(int reserve, const std::string& line);

private:
    int write_log(int reserve, const char* fmt, ...);
};
