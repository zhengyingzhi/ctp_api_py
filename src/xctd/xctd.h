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
#include <XcTradeApi/XcTradeApi.h>
#include <XcTradeApi/XcTrade_Define.h>

// #include <nlohmann/json.hpp>
// using json = nlohmann::json;

// using std::string;
// using std::map;
// using namespace std;
// using namespace pybind11;


#define XC_TDAPI_VERSION        "1.2.2"

#define XC_FUNC_QRY_SECINFO     330300      // ֤ȯ������Ϣ��ѯ
#define XC_FUNC_QRY_CASH_FAST   332254      // �ͻ��ʽ���ٲ�ѯ
#define XC_FUNC_QRY_CASH        332255      // �ͻ��ʽ�ȷ��ѯ
#define XC_FUNC_PLACE_ORDER     333002      // ��ͨί��
#define XC_FUNC_CANCEL_ORDER    333017      // ί�г���
#define XC_FUNC_QRY_PURCHASE    333030      // �¹��깺��ѯ
#define XC_FUNC_QRY_ORDER       333101      // ֤ȯί�в�ѯ
#define XC_FUNC_QRY_TRADE       333102      // ֤ȯ�ɽ���ѯ
#define XC_FUNC_QRY_POS_FAST    333103      // ֤ȯ�ֲֿ��ٲ�ѯ
#define XC_FUNC_QRY_POSITION    333104      // ֤ȯ�ֲֲ�ѯ
#define XC_FUNC_QRY_MD          400         // ֤ȯ�����ѯ
#define XC_FUNC_LOGIN           331100      // �ͻ���¼
#define XC_FUNC_QRY_ACCOUNTINFO 331155      // �˻���Ϣ��ȡ
#define XC_FUNC_SUB_ORDER       620001      // ���Ļر�����
#define XC_FUNC_RTN_ORDER       620003      // �ر�����

#define XC_FUNC_OPT_QRY_INFO        338000  // ��Ȩ�����ѯ
#define XC_FUNC_OPT_QRY_UNDERLYING  338001  // ��Ȩ��Ĳ�ѯ
#define XC_FUNC_OPT_PLACE_ORDER     338011  // ��Ȩί��
#define XC_FUNC_OPT_CANCEL_ORDER    338012  // ��Ȩ����
#define XC_FUNC_OPT_QRY_ORDER       338020  // ��Ȩί�в�ѯ
#define XC_FUNC_OPT_QRY_TRADE       338021  // ��Ȩ�ɽ���ѯ
#define XC_FUNC_OPT_QRY_POSITION    338023  // ��Ȩ�ֲֲ�ѯ
#define XC_FUNC_OPT_QRY_ASSET       338022  // ��Ȩ�ʲ���ѯ
#define XC_FUNC_OPT_QRY_MD          395     // ��Ȩ�����ѯ



#define ONCLOSE         1
#define ONDISCONNECT    2
#define ONCONNECTED     3
#define ONRECVJSONMSG   4

//����ṹ��
struct Task
{
    int task_name;      //�ص��������ƶ�Ӧ�ĳ���
    void *task_data;    //����ָ��
    void *task_error;   //����ָ��
    int task_id;        //����id
    bool task_last;     //�Ƿ�Ϊ��󷵻�
    std::string task_msg;
};

class TerminatedError : std::exception
{};

class TaskQueue
{
private:
    std::queue<Task> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;

    bool _terminate = false;

public:
    void push(const Task &task)
    {
        std::unique_lock<std::mutex > mlock(mutex_);
        queue_.push(task);
        mlock.unlock();
        cond_.notify_one();
    }

    //ȡ���ϵ�����
    Task pop()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        cond_.wait(mlock, [&]() {
            return !queue_.empty() || _terminate;
        });
        if (_terminate)
            throw TerminatedError();
        Task task = queue_.front();
        queue_.pop();
        return task;
    }

    void terminate()
    {
        _terminate = true;
        cond_.notify_all();
    }
};


class XcTdApi :public CXcTradeSpi
{
public:
    XcTdApi();
    ~XcTdApi();

    CXcTradeApi*    m_api;
    FILE*           m_fp;
    int             m_log_level;

    std::string     m_server_addr;
    std::string     m_license;
    std::string     m_account_id;
    int             m_async_mode;
    int             m_data_proto;
    int             m_count;

    std::thread     m_task_thread;
    TaskQueue       m_task_queue;
    bool            m_active = false;

private:
    void processTask();
    void processOnClose(Task *task);
    void processOnDisConnect(Task *task);
    void processOnConnected(Task *task);
    void processOnRecvJsonMsg(Task *task);

public:
    /* �Ͽ���ʾ*/
    virtual void OnClose(void);

    /* �����У����ڳ������� */
    virtual void OnDisConnect(void);

    /* �����ɹ� */
    virtual void OnConnected(void);

    /* ����JSONģʽӦ�����ݣ�JSONģʽ��*/
    virtual void OnRecvJsonMsg(char* pJsonMsg);

    /* �������ݰ�ģʽӦ�����ݣ�Pack��ģʽ��*/
    virtual void OnRecvPackMsg(int iFunid, int iRefid, int iIssueType, int iSet, int iRow, int iCol, char* szName, char* szValue);

    /* �������ݰ�ģʽ���ݼ�������Pack��ģʽ��*/
    virtual void OnRecvPackEndSet(int iFunid, int iRefid, int iIssueType, int iSet);

    /* �������ݰ�ģʽ�����н�����Pack��ģʽ��*/
    virtual void OnRecvPackEndRow(int iFunid, int iRefid, int iIssueType, int iSet, int iRow);

public:
    virtual void on_front_connected() {}
    virtual void on_front_disconnected(int reason) {}
    virtual void on_recv_msg(const std::string& msg) {}

    virtual void on_recv_pack_msg(int iFunid, int iRefid, int iIssueType, int iSet, int iRow, int iCol, const std::string& name, const std::string& value) {}
    virtual void on_recv_pack_end_set(int iFunid, int iRefid, int iIssueType, int iSet) {}
    virtual void on_recv_pack_end_row(int iFunid, int iRefid, int iIssueType, int iSet, int iRow) {}

public:
    // ����API async_mode��1-asyn, data_proto: 1-json
    int create_td_api(int async_mode = Trans_Mode_ASY, int data_proto = Data_Proto_Json);

    int create_trade_api();

    int register_spi(int async_mode = Trans_Mode_ASY, int data_proto = Data_Proto_Json);

    // ����API
    void release();

    // ���ӷ�����
    int connect(std::string server_addr, std::string license, std::string fund_account);

    // ��ʼ���
    void begin_pack(void);
    // �������
    void end_pack(void);

    // д�����ݰ�����
    int set_pack_value(const std::string& key_name, const std::string& value);

    // д��JSON����
    int set_json_value(const std::string& json_str);

    // �������ݣ�ͬ��ģʽ�°����������ݲ�д�����ݶ��У�
    int send_msg(int func_id, int subsystem_no = 0, int branch_no = 0);

    // �������ݣ��������ݳ���
    int recv_msg();

    // ��ȡ���ݼ�����
    int get_data_set_count();
    // ��ȡ����
    int get_cur_row_count();
    // ��ȡ����
    int get_cur_col_count();
    // ��λ��ǰ���ݼ������� true or false
    int get_cur_data_set(int index);
    // ��ȡ����
    std::string get_col_name(int col);
    // ��ȡ��ǰ����ֵ
    std::string get_col_value(int col);
    // ָ����һ��
    void get_next_row();

    // ��ȡJSON��ֵ
    std::string get_json_value(int len);

    // ��ȡ�û����ֵ
    int get_space();
    // ��ȡ�û�Ƶ��ֵ
    int get_frequency();

    // ��ȡ���������Ϣ
    std::string get_last_error_msg();

    // my extend function
    // ��������
    int send_json_data(int func_id, const std::string& data, int subsystem_no = 0, int branch_no = 0);

    // �������� RecvMsg & GetJsonValue
    std::string recv_json_data();

    std::string get_api_version();

    int open_debug(std::string log_file, int log_level);

    // write msg into log file
    int write_line(int reserve, const std::string& line);

private:
    int write_log(int reserve, const char* fmt, ...);
};
