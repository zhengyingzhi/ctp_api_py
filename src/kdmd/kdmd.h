//说明部分

//系统
#ifdef WIN32
#include "stdafx.h"
#endif
#include <stdint.h>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <codecvt>
#include <locale>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#define MD_ENABLE_WORK_THREAD   0


//API
#include "KDMdUserApi.h"

//命名空间
using namespace std;
using namespace pybind11;


#if MD_ENABLE_WORK_THREAD
//任务结构体
struct Task
{
    int task_name;        //回调函数名称对应的常量
    kd_md_recved_data_t* task_data;
};


///线程安全的队列
template<typename Data>
class ConcurrentQueue
{
private:
    queue<Data> the_queue;                              //标准库队列
    mutable mutex the_mutex;                            //互斥锁
    condition_variable the_condition_variable;          //条件变量

public:

    //存入新的任务
    void push(Data const& data)
    {
        unique_lock<mutex> lock(the_mutex);             //获取互斥锁
        the_queue.push(data);                           //向队列中存入数据
        lock.unlock();                                  //释放锁
        the_condition_variable.notify_one();            //通知正在阻塞等待的线程
    }

    //检查队列是否为空
    bool empty() const
    {
        unique_lock<mutex> lock(the_mutex);
        return the_queue.empty();
    }

    //取出
    Data wait_and_pop()
    {
        unique_lock<mutex> lock(the_mutex);

        while (the_queue.empty())                        //当队列为空时
        {
            the_condition_variable.wait(lock);           //等待条件变量通知
        }

        Data popped_value = the_queue.front();          //获取队列中的最后一个任务
        the_queue.pop();                                //删除该任务
        return popped_value;                            //返回该任务
    }

};

#endif// MD_ENABLE_WORK_THREAD


// Py API的实现
class KDMdApi
{
private:
    kd_md_api_t* api;                   // API对象
    bool active;
#if MD_ENABLE_WORK_THREAD
    thread task_thread;                 //工作线程指针（向python中推送数据）
    ConcurrentQueue<Task> task_queue;   //任务队列
#endif// MD_ENABLE_WORK_THREAD

public:
    KDMdApi() : api()
    {
        this->active = false;
    }

    ~KDMdApi()
    {
        if (this->active)
        {
            this->exit();
        }
    }

    //-------------------------------------------------------------------------------------
    //API回调函数
    //-------------------------------------------------------------------------------------
    static void md_api_handler_static(kd_md_api_t* apMdApi, uint32_t aMsgType, kd_md_recved_data_t* apData);
    void md_api_handler(kd_md_api_t* apMdApi, uint32_t aMsgType, kd_md_recved_data_t* apData);

    //-------------------------------------------------------------------------------------
    //task：任务
    //-------------------------------------------------------------------------------------
    void process_task();

    void process_front_connected();

    void process_front_disconnected(kd_md_recved_data_t* apData);

    void process_rsp_user_login(kd_md_recved_data_t* apData);

    void process_rsp_user_logout(kd_md_recved_data_t* apData);

    void process_rsp_sub_market_data(kd_md_recved_data_t* apData);

    void process_rsp_unsub_market_data(kd_md_recved_data_t* apData);

    void process_rsp_qry_data(kd_md_recved_data_t* apData);

    void process_rtn_data(kd_md_recved_data_t* apData);

    //-------------------------------------------------------------------------------------
    //data：回调函数的数据字典
    //error：回调函数的错误字典
    //id：请求id
    //last：是否为最后返回
    //i：整数
    //-------------------------------------------------------------------------------------

    virtual void on_front_connected() {};

    virtual void on_front_disconnected(int i) {};

    virtual void on_rsp_user_login(dict data) {};

    virtual void on_rsp_user_logout(dict data) {};

    virtual void on_rsp_sub_market_data(dict data, bool last) {};

    virtual void on_rsp_unsub_market_data(dict data, bool last) {};

    virtual void on_rsp_qry_data(dict data, dict error, bool last) {};

    virtual void on_rtn_market_data(uint16_t market_id, uint16_t service_id, dict data) {};

    //-------------------------------------------------------------------------------------
    //req:主动函数的请求字典
    //-------------------------------------------------------------------------------------

    void create_md_api(string flow_path = "");

    string get_api_version();

    void release();

    int init(uint32_t timeoutms = 0);

    int exit();

    int set_option(int32_t key, int32_t value);

    int get_option(int32_t key, int32_t* pvalue);

    void register_front(string pszFrontAddress, uint16_t port);

    int subscribe_market_data(dict req);

    int unsubscribe_market_data(dict req);

    int unsubscribe_all();

    int req_qry_data(dict req);

    int req_get_data(dict req, dict out_data, int timeoutms);

    vector<dict> req_get_data2(const vector<dict>& reqs, int timeoutms);

    int is_connected();

    int is_logined();

    int open_debug(string log_file, int log_level);

    int req_user_login(dict req, int timeoutms= 0);

    int req_user_logout(dict req);

    // void  set_user_data(void* udata);

    // void* get_user_data();
};
