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

#define XC_FUNC_QRY_SECINFO     330300      // 证券代码信息查询
#define XC_FUNC_QRY_CASH_FAST   332254      // 客户资金快速查询
#define XC_FUNC_QRY_CASH        332255      // 客户资金精确查询
#define XC_FUNC_PLACE_ORDER     333002      // 普通委托
#define XC_FUNC_CANCEL_ORDER    333017      // 委托撤单
#define XC_FUNC_QRY_PURCHASE    333030      // 新股申购查询
#define XC_FUNC_QRY_ORDER       333101      // 证券委托查询
#define XC_FUNC_QRY_TRADE       333102      // 证券成交查询
#define XC_FUNC_QRY_POS_FAST    333103      // 证券持仓快速查询
#define XC_FUNC_QRY_POSITION    333104      // 证券持仓查询
#define XC_FUNC_QRY_MD          400         // 证券行情查询
#define XC_FUNC_LOGIN           331100      // 客户登录
#define XC_FUNC_QRY_ACCOUNTINFO 331155      // 账户信息获取
#define XC_FUNC_SUB_ORDER       620001      // 订阅回报主推
#define XC_FUNC_RTN_ORDER       620003      // 回报主推

#define XC_FUNC_OPT_QRY_INFO        338000  // 期权代码查询
#define XC_FUNC_OPT_QRY_UNDERLYING  338001  // 期权标的查询
#define XC_FUNC_OPT_PLACE_ORDER     338011  // 期权委托
#define XC_FUNC_OPT_CANCEL_ORDER    338012  // 期权撤单
#define XC_FUNC_OPT_QRY_ORDER       338020  // 期权委托查询
#define XC_FUNC_OPT_QRY_TRADE       338021  // 期权成交查询
#define XC_FUNC_OPT_QRY_POSITION    338023  // 期权持仓查询
#define XC_FUNC_OPT_QRY_ASSET       338022  // 期权资产查询
#define XC_FUNC_OPT_QRY_MD          395     // 期权行情查询



#define ONCLOSE         1
#define ONDISCONNECT    2
#define ONCONNECTED     3
#define ONRECVJSONMSG   4

//任务结构体
struct Task
{
    int task_name;      //回调函数名称对应的常量
    void *task_data;    //数据指针
    void *task_error;   //错误指针
    int task_id;        //请求id
    bool task_last;     //是否为最后返回
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

    //取出老的任务
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
    /* 断开提示*/
    virtual void OnClose(void);

    /* 断线中，正在尝试重连 */
    virtual void OnDisConnect(void);

    /* 重连成功 */
    virtual void OnConnected(void);

    /* 反馈JSON模式应答数据（JSON模式）*/
    virtual void OnRecvJsonMsg(char* pJsonMsg);

    /* 反馈数据包模式应答数据（Pack包模式）*/
    virtual void OnRecvPackMsg(int iFunid, int iRefid, int iIssueType, int iSet, int iRow, int iCol, char* szName, char* szValue);

    /* 反馈数据包模式数据集结束（Pack包模式）*/
    virtual void OnRecvPackEndSet(int iFunid, int iRefid, int iIssueType, int iSet);

    /* 反馈数据包模式数据行结束（Pack包模式）*/
    virtual void OnRecvPackEndRow(int iFunid, int iRefid, int iIssueType, int iSet, int iRow);

public:
    virtual void on_front_connected() {}
    virtual void on_front_disconnected(int reason) {}
    virtual void on_recv_msg(const std::string& msg) {}

    virtual void on_recv_pack_msg(int iFunid, int iRefid, int iIssueType, int iSet, int iRow, int iCol, const std::string& name, const std::string& value) {}
    virtual void on_recv_pack_end_set(int iFunid, int iRefid, int iIssueType, int iSet) {}
    virtual void on_recv_pack_end_row(int iFunid, int iRefid, int iIssueType, int iSet, int iRow) {}

public:
    // 创建API async_mode：1-asyn, data_proto: 1-json
    int create_td_api(int async_mode = Trans_Mode_ASY, int data_proto = Data_Proto_Json);

    int create_trade_api();

    int register_spi(int async_mode = Trans_Mode_ASY, int data_proto = Data_Proto_Json);

    // 销毁API
    void release();

    // 连接服务器
    int connect(std::string server_addr, std::string license, std::string fund_account);

    // 开始打包
    void begin_pack(void);
    // 结束打包
    void end_pack(void);

    // 写入数据包数据
    int set_pack_value(const std::string& key_name, const std::string& value);

    // 写入JSON数据
    int set_json_value(const std::string& json_str);

    // 发送数据（同步模式下包含接收数据并写入数据队列）
    int send_msg(int func_id, int subsystem_no = 0, int branch_no = 0);

    // 接收数据，返回数据长度
    int recv_msg();

    // 获取数据集数量
    int get_data_set_count();
    // 获取行数
    int get_cur_row_count();
    // 获取列数
    int get_cur_col_count();
    // 定位当前数据集，返回 true or false
    int get_cur_data_set(int index);
    // 获取列名
    std::string get_col_name(int col);
    // 获取当前行列值
    std::string get_col_value(int col);
    // 指向下一行
    void get_next_row();

    // 获取JSON数值
    std::string get_json_value(int len);

    // 获取用户间隔值
    int get_space();
    // 获取用户频率值
    int get_frequency();

    // 获取最近错误信息
    std::string get_last_error_msg();

    // my extend function
    // 发送数据
    int send_json_data(int func_id, const std::string& data, int subsystem_no = 0, int branch_no = 0);

    // 接收数据 RecvMsg & GetJsonValue
    std::string recv_json_data();

    std::string get_api_version();

    int open_debug(std::string log_file, int log_level);

    // write msg into log file
    int write_line(int reserve, const std::string& line);

private:
    int write_log(int reserve, const char* fmt, ...);
};
