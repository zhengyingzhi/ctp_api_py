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
#include <boost/python/module.hpp>	//python封装
#include <boost/python/def.hpp>		//python封装
#include <boost/python/dict.hpp>	//python封装
#include <boost/python/object.hpp>	//python封装
#include <boost/python.hpp>			//python封装
// #include <boost/thread.hpp>			//任务队列的线程功能
// #include <boost/bind.hpp>			//任务队列的线程功能
// #include <boost/any.hpp>			//任务队列的任务实现
// #include <boost/locale.hpp>			//字符集转换
//API
#include "KDMdUserApi.h"

//命名空间
using namespace std;
using namespace boost::python;
using namespace boost;

//常量
#define ONFRONTCONNECTED 1
#define ONFRONTDISCONNECTED 2
#define ONHEARTBEATWARNING 3
#define ONRSPUSERLOGIN 4
#define ONRSPUSERLOGOUT 5
#define ONRSPERROR 6
#define ONRSPSUBMARKETDATA 7
#define ONRSPUNSUBMARKETDATA 8
#define ONRSPSUBFORQUOTERSP 9
#define ONRSPUNSUBFORQUOTERSP 10
#define ONRTNDEPTHMARKETDATA 11
#define ONRTNFORQUOTERSP 12



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
    kd_md_api_t* api;           // API对象

public:
    KDMdApi() : api()
    {
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

    void registerFront(string pszFrontAddress, uint16_t port);

    int subscribeMarketData(string instrumentID, uint16_t aMarketId, uint16_t aServiceId);

    int unSubscribeMarketData(string instrumentID, uint16_t aMarketId, uint16_t aServiceId);

    int unSubscribeAll();

    int reqQryData(dict req);

    int reqGetData(dict req, dict outData, int aTimeoutMS);
    
    int isConnected();

    int isLogined();

    int openDebug(string aLogFile, int aLogLevel);

    int reqUserLogin(dict req, int aTimeoutMS = 0);

    int reqUserLogout(dict req);
};
