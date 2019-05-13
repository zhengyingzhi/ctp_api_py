//说明部分

//系统
#ifdef WIN32
#include "stdafx.h"
#endif
#include <stdint.h>
#include <string>
#include <queue>

//Boost
#define BOOST_PYTHON_STATIC_LIB
#include <boost/python/module.hpp>  //python封装
#include <boost/python/def.hpp>     //python封装
#include <boost/python/dict.hpp>    //python封装
#include <boost/python/list.hpp>    //python封装
#include <boost/python/object.hpp>  //python封装
#include <boost/python.hpp>         //python封装
#include <boost/thread.hpp>         //任务队列的线程功能
#include <boost/bind.hpp>           //任务队列的线程功能
#include <boost/any.hpp>            //任务队列的任务实现
#include <boost/locale.hpp>         //字符集转换
//API
#include "KDMdUserApi.h"

//命名空间
using namespace std;
using namespace boost::python;
using namespace boost;


#define MD_ENABLE_WORK_THREAD   0

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
    mutable mutex the_mutex;                            //boost互斥锁
    condition_variable the_condition_variable;          //boost条件变量

public:

    //存入新的任务
    void push(Data const& data)
    {
        mutex::scoped_lock lock(the_mutex);             //获取互斥锁
        the_queue.push(data);                           //向队列中存入数据
        lock.unlock();                                  //释放锁
        the_condition_variable.notify_one();            //通知正在阻塞等待的线程
    }

    //检查队列是否为空
    bool empty() const
    {
        mutex::scoped_lock lock(the_mutex);
        return the_queue.empty();
    }

    //取出
    Data wait_and_pop()
    {
        mutex::scoped_lock lock(the_mutex);

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

///-------------------------------------------------------------------------------------
///API中的部分组件
///-------------------------------------------------------------------------------------

//GIL全局锁简化获取用，
//用于帮助C++线程获得GIL锁，从而防止python崩溃
class PyLock
{
private:
    PyGILState_STATE gil_state;

public:
    //在某个函数方法中创建该对象时，获得GIL锁
    PyLock()
    {
        gil_state = PyGILState_Ensure();
    }

    //在某个函数完成后销毁该对象时，解放GIL锁
    ~PyLock()
    {
        PyGILState_Release(gil_state);
    }
};



//从字典中获取某个建值对应的整数，并赋值到请求结构体对象的值上
void getInt(dict d, string key, int* value);


//从字典中获取某个建值对应的浮点数，并赋值到请求结构体对象的值上
void getDouble(dict d, string key, double* value);


//从字典中获取某个建值对应的字符，并赋值到请求结构体对象的值上
void getChar(dict d, string key, char* value);


//从字典中获取某个建值对应的字符串，并赋值到请求结构体对象的值上
void getStr(dict d, string key, char* value);


///-------------------------------------------------------------------------------------
///C++ SPI的回调函数方法实现
///-------------------------------------------------------------------------------------

//API的继承实现
class KDMdApi
{
private:
    kd_md_api_t* api;                   // API对象
#if MD_ENABLE_WORK_THREAD
    thread* task_thread;                //工作线程指针（向python中推送数据）
    ConcurrentQueue<Task> task_queue;   //任务队列
#endif// MD_ENABLE_WORK_THREAD

public:
    KDMdApi() : api()
    {
#if MD_ENABLE_WORK_THREAD
        function0<void> f = boost::bind(&KDMdApi::processTask, this);
        thread t(f);
        this->task_thread = &t;
#endif//MD_ENABLE_WORK_THREAD
    }

    ~KDMdApi()
    {
    }

    //-------------------------------------------------------------------------------------
    //API回调函数
    //-------------------------------------------------------------------------------------
    static void mdApiHandlerStatic(kd_md_api_t* apMdApi, uint32_t aMsgType, kd_md_recved_data_t* apData);
    void mdApiHandler(kd_md_api_t* apMdApi, uint32_t aMsgType, kd_md_recved_data_t* apData);

    //-------------------------------------------------------------------------------------
    //task：任务
    //-------------------------------------------------------------------------------------
    void processTask();

    void processFrontConnected();

    void processFrontDisconnected(kd_md_recved_data_t* apData);

    void processRspUserLogin(kd_md_recved_data_t* apData);

    void processRspUserLogout(kd_md_recved_data_t* apData);

    void processRspSubMarketData(kd_md_recved_data_t* apData);

    void processRspUnSubMarketData(kd_md_recved_data_t* apData);

    void processRspQueryData(kd_md_recved_data_t* apData);

    void processRtnData(kd_md_recved_data_t* apData);

    //-------------------------------------------------------------------------------------
    //data：回调函数的数据字典
    //error：回调函数的错误字典
    //id：请求id
    //last：是否为最后返回
    //i：整数
    //-------------------------------------------------------------------------------------

    virtual void onFrontConnected() {};

    virtual void onFrontDisconnected(int i) {};

    virtual void onRspUserLogin(dict data) {};

    virtual void onRspUserLogout(dict data) {};

    virtual void onRspSubMarketData(dict data, bool last) {};

    virtual void onRspUnSubMarketData(dict data, bool last) {};

    virtual void onRspQryData(dict data, dict error, bool last) {};

    virtual void onRtnMarketData(uint16_t aMarketId, uint16_t aServiceId, dict data) {};

    //-------------------------------------------------------------------------------------
    //req:主动函数的请求字典
    //-------------------------------------------------------------------------------------

    void createMdApi(string pszFlowPath = "");

    string getApiVersion();

    void release();

    void init(uint32_t aTimeoutMS = 0);

    int exit();

    int setOption(int32_t aOptionKey, int32_t aOptionValue);

    int getOption(int32_t aOptionKey, int32_t* aOptionValue);

    void registerFront(string pszFrontAddress, uint16_t port);

    int subscribeMarketData(dict req);

    int unSubscribeMarketData(dict req);

    int unSubscribeAll();

    int reqQryData(dict req);

    int reqGetData(dict req, dict outData, int aTimeoutMS);
    
    int isConnected();

    int isLogined();

    int openDebug(string aLogFile, int aLogLevel);

    int reqUserLogin(dict req, int aTimeoutMS = 0);

    int reqUserLogout(dict req);

    // void  setUserData(void* apUserData);

    // void* getUserData();
};
