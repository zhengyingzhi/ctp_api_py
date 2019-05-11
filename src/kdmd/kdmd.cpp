// vnctpmd.cpp : 定义 DLL 应用程序的导出函数。
//

#include "kdmd.h"
#include "KDCommonDef.h"
#include "KDMDApiStruct.h"
#include "KDStockMarket.h"

///-------------------------------------------------------------------------------------
///从Python对象到C++类型转换用的函数
///-------------------------------------------------------------------------------------

void getInt(dict d, string key, int *value)
{
	if (d.has_key(key))		//检查字典中是否存在该键值
	{
		object o = d[key];	//获取该键值
		extract<int> x(o);	//创建提取器
		if (x.check())		//如果可以提取
		{
			*value = x();	//对目标整数指针赋值
		}
	}
};

void getDouble(dict d, string key, double *value)
{
	if (d.has_key(key))
	{
		object o = d[key];
		extract<double> x(o);
		if (x.check())
		{
			*value = x();
		}
	}
};

void getStr(dict d, string key, char *value)
{
	if (d.has_key(key))
	{
		object o = d[key];
		extract<string> x(o);
		if (x.check())
		{
			string s = x();
			const char *buffer = s.c_str();
			//对字符串指针赋值必须使用strcpy_s, vs2013使用strcpy编译通不过
			//+1应该是因为C++字符串的结尾符号？不是特别确定，不加这个1会出错
#ifdef _MSC_VER //WIN32
			strcpy_s(value, strlen(buffer) + 1, buffer);
#elif __GNUC__
			strncpy(value, buffer, strlen(buffer) + 1);
#endif
		}
	}
};

void getChar(dict d, string key, char *value)
{
	if (d.has_key(key))
	{
		object o = d[key];
		extract<string> x(o);
		if (x.check())
		{
			string s = x();
			const char *buffer = s.c_str();
			*value = *buffer;
		}
	}
};


///-------------------------------------------------------------------------------------
///C++的回调函数将数据保存到队列中
///-------------------------------------------------------------------------------------
void KDMdApi::mdApiHandlerStatic(kd_md_api_t* apMdApi, uint32_t aMsgType, kd_md_recved_data_t* apData)
{
    KDMdApi* lpThis;
    lpThis = (KDMdApi*)KDMdGetUserData(apMdApi);
    lpThis->mdApiHandler(apMdApi, aMsgType, apData);
}

void KDMdApi::mdApiHandler(kd_md_api_t* apMdApi, uint32_t aMsgType, kd_md_recved_data_t* apData)
{
    switch (aMsgType)
    {
    case KD_MT_Connected:
        processFrontConnected();
        break;
    case KD_MT_Disconnected:
        processFrontDisconnected(apData);
        break;
    case KD_MT_Login:
        processRspUserLogin(apData);
        break;
    case KD_MT_Logout:
        processRspUserLogout(apData);
        break;
    case KD_MT_Subscribe:
        processRspSubMarketData(apData);
        break;
    case KD_MT_UnSubscribe:
        processRspUnSubMarketData(apData);
        break;
    case KD_MT_Query:
        processRspQueryData(apData);
        break;
    case KD_MT_Publish:
        processRtnData(apData);
        break;
    default:
        break;
    }
}


void KDMdApi::processFrontConnected()
{
    PyLock lock;
    this->onFrontConnected();
};

void KDMdApi::processFrontDisconnected(kd_md_recved_data_t* apData)
{
    PyLock lock;
    this->onFrontDisconnected(apData->m_ErrorID);
};

void KDMdApi::processRspUserLogin(kd_md_recved_data_t* apData)
{
    PyLock lock;
    dict data;
    KDMDLoginRspData* lpLoginData;
    lpLoginData = (KDMDLoginRspData*)apData->m_Data.m_pDataInfo;

    data["UserID"]      = std::string(lpLoginData->m_UserID);
    data["ReqID"]       = lpLoginData->m_ReqID;
    data["Flag"]        = lpLoginData->m_Flag;
    data["ServerFlag"]  = lpLoginData->m_ServerFlag;
    data["ErrorID"]     = apData->m_ErrorID;

    this->onRspUserLogin(data);
};

void KDMdApi::processRspUserLogout(kd_md_recved_data_t* apData)
{
    PyLock lock;
    dict data;
    KDMDLogoutData* lpLogoutData;
    lpLogoutData = (KDMDLogoutData*)apData->m_Data.m_pDataInfo;

    data["UserID"] = std::string(lpLogoutData->m_UserID);
    data["ReqID"] = lpLogoutData->m_ReqID;
    data["Flag"] = lpLogoutData->m_Flag;

    this->onRspUserLogout(data);
};

void KDMdApi::processRspSubMarketData(kd_md_recved_data_t* apData)
{
    PyLock lock;
    dict data;
    string lInstrumentID = apData->m_Data.m_pDataInfo;

    data["InstrumentID"] = lInstrumentID;
    data["MarketId"] = apData->m_Data.m_MarketId;
    data["ServiceId"] = apData->m_Data.m_ServiceId;
    data["ErrorID"] = apData->m_ErrorID;

    bool lIsLast = apData->m_IsLast ? true : false;
    this->onRspSubMarketData(data, lIsLast);
};

void KDMdApi::processRspUnSubMarketData(kd_md_recved_data_t* apData)
{
    PyLock lock;
    dict data;
    string lInstrumentID = apData->m_Data.m_pDataInfo;

    data["InstrumentID"] = lInstrumentID;
    data["MarketId"] = apData->m_Data.m_MarketId;
    data["ServiceId"] = apData->m_Data.m_ServiceId;
    data["ErrorID"] = apData->m_ErrorID;

    bool lIsLast = apData->m_IsLast ? true : false;
    this->onRspSubMarketData(data, lIsLast);
};

void KDMdApi::processRspQueryData(kd_md_recved_data_t* apData)
{
    PyLock lock;
    dict data;
    dict error;
    bool lIsLast = apData->m_IsLast ? true : false;
    this->onRspQryData(data, error, lIsLast);
}

void KDMdApi::processRtnData(kd_md_recved_data_t* apData)
{
    PyLock lock;
    dict data;
    uint16_t lMarketId = apData->m_Data.m_MarketId;
    uint16_t lServiceId = apData->m_Data.m_ServiceId;
    if (lMarketId == KD_MI_SSE || lMarketId == KD_MI_SZSE)
    {
        if (lServiceId == KD_SI_IDX_ProductInfo)
        {
            KDIndexProductInfo* lpIdxInfo;
            lpIdxInfo = (KDIndexProductInfo*)apData->m_Data.m_pDataInfo;
            data["InstrumentID"] = std::string(lpIdxInfo->InstrumentID);
        }
        else if (lServiceId == KD_SI_IDX_MarketData)
        {
            KDIndexMarketData* lpIdxMD;
            lpIdxMD = (KDIndexMarketData*)apData->m_Data.m_pDataInfo;
            data["InstrumentID"] = std::string(lpIdxMD->InstrumentID);
        }
        else if (lServiceId == KD_SI_STK_ProductInfo)
        {
            KDStockProductInfo* lpStkInfo;
            lpStkInfo = (KDStockProductInfo*)apData->m_Data.m_pDataInfo;
            data["InstrumentID"] = std::string(lpStkInfo->InstrumentID);
        }
        else if (lServiceId == KD_SI_STK_MarketDataL1)
        {
            KDStockMarketDataL1* lpStkMD;
            lpStkMD = (KDStockMarketDataL1*)apData->m_Data.m_pDataInfo;
            data["InstrumentID"] = std::string(lpStkMD->InstrumentID);
        }
    }
    this->onRtnMarketData(lMarketId, lServiceId, data);
};



///-------------------------------------------------------------------------------------
///主动函数
///-------------------------------------------------------------------------------------

void KDMdApi::createMdApi(string pszFlowPath)
{
    (void)pszFlowPath;
    this->api = KDMdCreate();

    KDMdRegisterHandler(this->api, KDMdApi::mdApiHandlerStatic);
};

string KDMdApi::getApiVersion()
{
    std::string lApiVersion = KDMdGetVersion(NULL);
    return lApiVersion;
}

void KDMdApi::release()
{
    KDMdRelease(this->api);
};

void KDMdApi::init(uint32_t aTimeoutMS)
{
    KDMdInit(this->api, aTimeoutMS);
};

int KDMdApi::exit()
{
    //该函数在原生API里没有，用于安全退出API用，原生的join似乎不太稳定
    KDMdRegisterHandler(this->api, NULL);
    KDMdRelease(this->api);
    this->api = NULL;
    return 1;
};

void KDMdApi::registerFront(string pszFrontAddress, uint16_t port)
{
    kd_md_tcp_info_t lTcpInfo = { 0 };
    strncpy(lTcpInfo.m_ServerAddress, pszFrontAddress.c_str(), sizeof(lTcpInfo.m_ServerAddress) - 1);
    lTcpInfo.m_ServerPort = port;
    KDMdRegisterServer(this->api, &lTcpInfo, 1);
};

int KDMdApi::subscribeMarketData(string aInstrument, uint16_t aMarketId, uint16_t aServiceId)
{
    int lMarketId = 0, lServiceId = 0;
    kd_md_instrument_key_t lInstrKey = { 0 };
    strncpy(lInstrKey.m_InstrumentID, aInstrument.c_str(), sizeof(lInstrKey) - 1);
    lInstrKey.m_MarketId = aMarketId;
    lInstrKey.m_ServiceId = aServiceId;
    return KDMdSubscribe(this->api, &lInstrKey, 1);
};

int KDMdApi::unSubscribeMarketData(string aInstrument, uint16_t aMarketId, uint16_t aServiceId)
{
    int lMarketId = 0, lServiceId = 0;
    kd_md_instrument_key_t lInstrKey = { 0 };
    strncpy(lInstrKey.m_InstrumentID, aInstrument.c_str(), sizeof(lInstrKey) - 1);
    lInstrKey.m_MarketId = aMarketId;
    lInstrKey.m_ServiceId = aServiceId;
    return KDMdUnSubscribe(this->api, &lInstrKey, 1);
};


int KDMdApi::unSubscribeAll()
{
    return KDMdUnSubscribeAll(this->api);
}

int KDMdApi::reqQryData(dict req)
{
    int rv;
    int lMarketId = 0, lServiceId = 0;
    kd_md_instrument_key_t lInstrKey = { 0 };
    getStr(req, "InstrumentID", lInstrKey.m_InstrumentID);
    getInt(req, "MarketId", &lMarketId);
    getInt(req, "ServiceId", &lServiceId);

    rv = KDMdReqQryData(this->api, &lInstrKey, 1);
    return rv;
}

int KDMdApi::reqGetData(dict req, dict outData, int aTimeoutMS)
{
    int rv;
    int lMarketId = 0, lServiceId = 0;
    kd_md_instrument_key_t lInstrKey = { 0 };
    getStr(req, "InstrumentID", lInstrKey.m_InstrumentID);
    getInt(req, "MarketId", &lMarketId);
    getInt(req, "ServiceId", &lServiceId);

    kd_md_data_t* lpData = NULL;
    uint32_t lDataSize = 1;
    rv = KDMdReqGetData(this->api, &lInstrKey, 1, &lpData, &lDataSize, aTimeoutMS);
    if (lpData)
    {
        // 
    }
    return rv;
}

int KDMdApi::isConnected()
{
    return KDMdIsConnected(this->api);
}

int KDMdApi::isLogined()
{
    return KDMdIsLogined(this->api);
}

int KDMdApi::openDebug(string aLogFile, int aLogLevel)
{
    return KDMdOpenDebug(this->api, aLogFile.c_str(), aLogLevel);
}


int KDMdApi::reqUserLogin(dict req, int aTimeoutMS)
{
    char lUserID[32] = "";
    char lPassword[32] = "";
    getStr(req, "UserID", lUserID);
    getStr(req, "Password", lPassword);
    return KDMdReqLogin(this->api, lUserID, lPassword, aTimeoutMS);
};

int KDMdApi::reqUserLogout(dict req)
{
    return KDMdReqLogout(this->api);
};



///-------------------------------------------------------------------------------------
///Boost.Python封装
///-------------------------------------------------------------------------------------

struct KDMdApiWrap : KDMdApi, wrapper < KDMdApi >
{
    virtual void onFrontConnected()
    {
        //以下的try...catch...可以实现捕捉python环境中错误的功能，防止C++直接出现原因未知的崩溃
        try
        {
            this->get_override("onFrontConnected")();
        }
        catch (error_already_set const &)
        {
            PyErr_Print();
        }
    };

    virtual void onFrontDisconnected(int i)
    {
        try
        {
            this->get_override("onFrontDisconnected")(i);
        }
        catch (error_already_set const &)
        {
            PyErr_Print();
        }
    };

    virtual void onRspUserLogin(dict data)
    {
        try
        {
            this->get_override("onRspUserLogin")(data);
        }
        catch (error_already_set const &)
        {
            PyErr_Print();
        }
    };

    virtual void onRspUserLogout(dict data)
    {
        try
        {
            this->get_override("onRspUserLogout")(data);
        }
        catch (error_already_set const &)
        {
            PyErr_Print();
        }
    };

    virtual void onRspSubMarketData(dict data, bool last)
    {
        try
        {
            this->get_override("onRspSubMarketData")(data, last);
        }
        catch (error_already_set const &)
        {
            PyErr_Print();
        }
    };

    virtual void onRspUnSubMarketData(dict data, bool last)
    {
        try
        {
            this->get_override("onRspUnSubMarketData")(data, last);
        }
        catch (error_already_set const &)
        {
            PyErr_Print();
        }
    };

    virtual void onRspQryData(dict data, dict error, bool last)
    {
        try
        {
            this->get_override("onRspQryData")(data, error, last);
        }
        catch (error_already_set const &)
        {
            PyErr_Print();
        }
    };

    virtual void onRtnMarketData(uint16_t aMarketId, uint16_t aServiceId, dict data)
    {
        try
        {
            this->get_override("onRtnMarketData")(aMarketId, aServiceId, data);
        }
        catch (error_already_set const &)
        {
            PyErr_Print();
        }
    };

};


BOOST_PYTHON_MODULE(kdmd)
{
    PyEval_InitThreads();   //导入时运行，保证先创建GIL

    class_<KDMdApiWrap, boost::noncopyable>("KDMdApi")
        .def("createMdApi", &KDMdApiWrap::createMdApi)
        .def("getApiVersion", &KDMdApiWrap::getApiVersion)
        .def("release", &KDMdApiWrap::release)
        .def("init", &KDMdApiWrap::init)
        .def("exit", &KDMdApiWrap::exit)
        .def("registerFront", &KDMdApiWrap::registerFront)
        .def("subscribeMarketData", &KDMdApiWrap::subscribeMarketData)
        .def("unSubscribeMarketData", &KDMdApiWrap::unSubscribeMarketData)
        .def("reqQryData", &KDMdApiWrap::reqQryData)
        .def("reqGetData", &KDMdApiWrap::reqGetData)
        .def("reqUserLogin", &KDMdApiWrap::reqUserLogin)
        .def("reqUserLogout", &KDMdApiWrap::reqUserLogout)

        .def("onFrontConnected", pure_virtual(&KDMdApiWrap::onFrontConnected))
        .def("onFrontDisconnected", pure_virtual(&KDMdApiWrap::onFrontDisconnected))
        .def("onRspUserLogin", pure_virtual(&KDMdApiWrap::onRspUserLogin))
        .def("onRspUserLogout", pure_virtual(&KDMdApiWrap::onRspUserLogout))
        .def("onRspSubMarketData", pure_virtual(&KDMdApiWrap::onRspSubMarketData))
        .def("onRspUnSubMarketData", pure_virtual(&KDMdApiWrap::onRspUnSubMarketData))
        .def("onRspQryData", pure_virtual(&KDMdApiWrap::onRspQryData))
        .def("onRtnMarketData", pure_virtual(&KDMdApiWrap::onRtnMarketData))
        ;
};
