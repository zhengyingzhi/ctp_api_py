// kdmd.cpp : 定义 DLL 应用程序的导出函数。
//

#include <stdlib.h>
#include <string.h>

#include "kdmd.h"
#include "KDCommonDef.h"
#include "KDMDApiStruct.h"
#include "KDStockMarket.h"

#define DEFAULT_MAX_REQ_SIZE    256

///-------------------------------------------------------------------------------------
///从Python对象到C++类型转换用的函数
///-------------------------------------------------------------------------------------

void get_int(dict d, string key, int *value)
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

void get_double(dict d, string key, double *value)
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

void get_str(dict d, string key, char *value)
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

void get_char(dict d, string key, char *value)
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

typedef struct {
    char* ptr;
    int   len;
}zditem_t;

int str_delimiter_ex(const char* src, int length, zditem_t* retArr, int arrSize, const char* sep)
{
    int lenSep = (int)strlen(sep);
    int index = 0;
    const char* sentinel = src + length;
    while (index < arrSize)
    {
        const char* end;
        if (lenSep == 1)
            end = strchr(src, *sep);
        else
            end = strstr(src, sep);

        if (end == NULL || end >= sentinel)
        {
            // last item
            if (sentinel - src > 0)
            {
                retArr[index].ptr = (char*)src;
                retArr[index].len = (int)(sentinel - src);
                index++;
            }
            break;
        }

        retArr[index].ptr = (char*)src;
        retArr[index].len = (int)(end - src);
        index++;

        // next pos
        src = end + lenSep;
    }

    return index;
}

int get_instrument_key(dict req, kd_md_instrument_key_t* apInstruKey, int aSize)
{
    int lMarketId = 0, lServiceId = 0;
    char lInstrumentIDs[4080] = "";
    get_str(req, "InstrumentID", lInstrumentIDs);
    get_int(req, "MarketId", &lMarketId);
    get_int(req, "ServiceId", &lServiceId);

        zditem_t lItems[1000];
        int lCount = str_delimiter_ex(lInstrumentIDs, (int)strlen(lInstrumentIDs), lItems, 1000, ",");
        int lCount2 = 0;
        for (int i = 0; i < lCount && i < aSize; ++i)
        {
            if (lItems[i].len <= 0 || lItems[i].len > 31)
                continue;
            ++lCount2;
            strncpy(apInstruKey[i].m_InstrumentID, lItems[i].ptr, lItems[i].len);
            apInstruKey[i].m_MarketId = (uint16_t)lMarketId;
            apInstruKey[i].m_ServiceId = (uint16_t)lServiceId;
        }
        return lCount2;
}

void extract_array_data(dict out_data, const char* apKey, uint32_t src_data[], uint32_t aSize)
{
    char lKey[64] = "";
    for (uint32_t i = 1; i <= aSize; ++i)
    {
        sprintf(lKey, apKey, i);
        out_data[lKey] = src_data[i - 1];
    }
}

void extract_data(dict out_data, kd_md_data_t* apData)
{
    uint16_t lMarketId = apData->m_MarketId;
    uint16_t lServiceId = apData->m_ServiceId;
    if (lMarketId == KD_MI_SSE || lMarketId == KD_MI_SZSE)
    {
        if (lServiceId == KD_SI_IDX_ProductInfo)
        {
            KDIndexProductInfo* lpIdxInfo;
            lpIdxInfo = (KDIndexProductInfo*)apData->m_pDataInfo;
            out_data["InstrumentID"]    = boost::locale::conv::to_utf<char>(lpIdxInfo->InstrumentID, std::string("GB2312"));
            out_data["InstrumentName"]  = boost::locale::conv::to_utf<char>(lpIdxInfo->InstrumentName, std::string("GB2312"));
            out_data["DecimalPoint"]    = lpIdxInfo->DecimalPoint;
            out_data["PreCloseIndex"]   = lpIdxInfo->PreCloseIndex;
            out_data["MarketId"]        = lpIdxInfo->MarketId;
        }
        else if (lServiceId == KD_SI_IDX_MarketData)
        {
            KDIndexMarketData* lpIdxMD;
            lpIdxMD = (KDIndexMarketData*)apData->m_pDataInfo;
            out_data["InstrumentID"] = boost::locale::conv::to_utf<char>(lpIdxMD->InstrumentID, std::string("GB2312"));
            out_data["OpenIndex"]    = lpIdxMD->OpenIndex;
            out_data["HighIndex"]    = lpIdxMD->HighIndex;
            out_data["LowIndex"]     = lpIdxMD->LowIndex;
            out_data["LastIndex"]    = lpIdxMD->LastIndex;
            out_data["CloseIndex"]   = lpIdxMD->CloseIndex;
            out_data["PreCloseIndex"] = lpIdxMD->PreCloseIndex;
            out_data["Volume"]       = lpIdxMD->Volume;
            out_data["Turnover"]     = lpIdxMD->Turnover;
            out_data["TradingDay"]   = lpIdxMD->TradingDay;
            out_data["ActionDay"]    = lpIdxMD->ActionDay;
            out_data["UpdateTime"]   = lpIdxMD->UpdateTime;
        }
        else if (lServiceId == KD_SI_STK_ProductInfo)
        {
            KDStockProductInfo* lpStkInfo;
            lpStkInfo = (KDStockProductInfo*)apData->m_pDataInfo;
            out_data["InstrumentID"]     = boost::locale::conv::to_utf<char>(lpStkInfo->InstrumentID, std::string("GB2312"));
            out_data["InstrumentName"]   = boost::locale::conv::to_utf<char>(lpStkInfo->InstrumentName, std::string("GB2312"));
            out_data["DecimalPoint"]     = lpStkInfo->DecimalPoint;
            out_data["PreClosePrice"]    = lpStkInfo->PreClosePrice;
            out_data["UpperLimitPrice"]  = lpStkInfo->UpperLimitPrice;
            out_data["LowerLimitPrice"]  = lpStkInfo->LowerLimitPrice;
            out_data["MarketId"]         = lpStkInfo->MarketId;
        }
        else if (lServiceId == KD_SI_STK_MarketDataL1)
        {
            KDStockMarketDataL1* lpStkMD;
            lpStkMD = (KDStockMarketDataL1*)apData->m_pDataInfo;
            out_data["InstrumentID"] = boost::locale::conv::to_utf<char>(lpStkMD->InstrumentID, std::string("GB2312"));
            out_data["OpenPrice"]    = lpStkMD->OpenPrice;
            out_data["HighestPrice"] = lpStkMD->HighestPrice;
            out_data["LowestPrice"]  = lpStkMD->LowestPrice;
            out_data["LastPrice"]    = lpStkMD->LastPrice;
            extract_array_data(out_data, "BidPrice%u", lpStkMD->BidPrice, 5);
            extract_array_data(out_data, "BidVol%u",   lpStkMD->BidVol, 5);
            extract_array_data(out_data, "AskPrice%u", lpStkMD->AskPrice, 5);
            extract_array_data(out_data, "AskVol%u",   lpStkMD->AskVol, 5);
            out_data["Volume"]       = lpStkMD->Volume;
            out_data["Turnover"]     = lpStkMD->Turnover;
            out_data["AveragePrice"] = lpStkMD->AveragePrice;
            out_data["ClosePrice"]   = lpStkMD->ClosePrice;
            out_data["IOPV"]         = lpStkMD->IOPV;
            out_data["PreIOPV"]      = lpStkMD->PreIOPV;
            out_data["TradingDay"]   = lpStkMD->TradingDay;
            out_data["ActionDay"]    = lpStkMD->ActionDay;
            out_data["UpdateTime"]   = lpStkMD->UpdateTime;
            out_data["Status"]       = lpStkMD->Status;
            out_data["StopFlag"]     = lpStkMD->StopFlag;
        }
        else if (lServiceId == KD_SI_KLineData)
        {
            KDKLine* lpKLine;
            lpKLine = (KDKLine*)apData->m_pDataInfo;
            out_data["InstrumentID"] = boost::locale::conv::to_utf<char>(lpKLine->InstrumentID, std::string("GB2312"));
            out_data["Day"]  = lpKLine->Day;
            out_data["Time"] = lpKLine->Time;
            out_data["Open"] = lpKLine->Open;
            out_data["High"] = lpKLine->High;
            out_data["Low"]  = lpKLine->Low;
            out_data["Close"] = lpKLine->Close;
            out_data["Volume"] = lpKLine->Volume;
            out_data["Turnover"] = lpKLine->Turnover;
        }
    }
}

kd_md_recved_data_t* clone_recved_data(const kd_md_recved_data_t* apRawData)
{
    kd_md_recved_data_t* lpNewData;
    lpNewData = (kd_md_recved_data_t*)malloc(sizeof(kd_md_recved_data_t) + apRawData->m_Data.m_DataSize);
    memcpy(lpNewData, apRawData, sizeof(kd_md_recved_data_t));
    memcpy(lpNewData->m_Data.m_pDataInfo, apRawData->m_Data.m_pDataInfo, apRawData->m_Data.m_DataSize);
    return lpNewData;
}


///-------------------------------------------------------------------------------------
///C++的回调函数将数据保存到队列中
///-------------------------------------------------------------------------------------
void KDMdApi::md_api_handler_static(kd_md_api_t* apMdApi, uint32_t aMsgType, kd_md_recved_data_t* apData)
{
    // fprintf(stderr, "mdApiHandlerStatic:%d\n", aMsgType);
    // return;
    KDMdApi* lpThis;
    lpThis = (KDMdApi*)KDMdGetUserData(apMdApi);

#if MD_ENABLE_WORK_THREAD
    Task task = Task();
    task.task_name = aMsgType;
    task.task_data = clone_recved_data(apData);
    lpThis->task_queue.push(task);
#else
    lpThis->md_api_handler(lpThis->api, aMsgType, apData);
#endif// MD_ENABLE_WORK_THREAD
}

void KDMdApi::md_api_handler(kd_md_api_t* apMdApi, uint32_t aMsgType, kd_md_recved_data_t* apData)
{
    switch (aMsgType)
    {
    case KD_MT_Connected:
        process_front_connected();
        break;
    case KD_MT_Disconnected:
        process_front_disconnected(apData);
        break;
    case KD_MT_Login:
        process_rsp_user_login(apData);
        break;
    case KD_MT_Logout:
        process_rsp_user_logout(apData);
        break;
    case KD_MT_Subscribe:
        process_rsp_sub_market_data(apData);
        break;
    case KD_MT_UnSubscribe:
        process_rsp_unsub_market_data(apData);
        break;
    case KD_MT_Query:
        process_rsp_qry_data(apData);
        break;
    case KD_MT_Publish:
        process_rtn_data(apData);
        break;
    default:
        break;
    }
}

#if MD_ENABLE_WORK_THREAD
void KDMdApi::process_task()
{
    while (1)
    {
        Task task = this->task_queue.wait_and_pop();
        this->md_api_handler(this->api, task.task_name, task.task_data);
        if (task.task_data)
            free(task.task_data);
    }
}
#endif// MD_ENABLE_WORK_THREAD

void KDMdApi::process_front_connected()
{
    // fprintf(stderr, "processFrontConnected\n");
    PyLock lock;
    this->on_front_connected();
};

void KDMdApi::process_front_disconnected(kd_md_recved_data_t* apData)
{
    PyLock lock;
    this->on_front_disconnected(apData->m_ErrorID);
};

void KDMdApi::process_rsp_user_login(kd_md_recved_data_t* apData)
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

    this->on_rsp_user_login(data);
};

void KDMdApi::process_rsp_user_logout(kd_md_recved_data_t* apData)
{
    PyLock lock;
    dict data;
    KDMDLogoutData* lpLogoutData;
    lpLogoutData = (KDMDLogoutData*)apData->m_Data.m_pDataInfo;

    data["UserID"] = std::string(lpLogoutData->m_UserID);
    data["ReqID"] = lpLogoutData->m_ReqID;
    data["Flag"] = lpLogoutData->m_Flag;

    this->on_rsp_user_logout(data);
};

void KDMdApi::process_rsp_sub_market_data(kd_md_recved_data_t* apData)
{
    PyLock lock;
    dict data;
    string lInstrumentID = apData->m_Data.m_pDataInfo;

    data["InstrumentID"] = lInstrumentID;
    data["MarketId"] = apData->m_Data.m_MarketId;
    data["ServiceId"] = apData->m_Data.m_ServiceId;
    data["ErrorID"] = apData->m_ErrorID;

    bool lIsLast = apData->m_IsLast ? true : false;
    this->on_rsp_sub_market_data(data, lIsLast);
};

void KDMdApi::process_rsp_unsub_market_data(kd_md_recved_data_t* apData)
{
    PyLock lock;
    dict data;
    string lInstrumentID = apData->m_Data.m_pDataInfo;

    data["InstrumentID"] = lInstrumentID;
    data["MarketId"] = apData->m_Data.m_MarketId;
    data["ServiceId"] = apData->m_Data.m_ServiceId;
    data["ErrorID"] = apData->m_ErrorID;

    bool lIsLast = apData->m_IsLast ? true : false;
    this->on_rsp_unsub_market_data(data, lIsLast);
};

void KDMdApi::process_rsp_qry_data(kd_md_recved_data_t* apData)
{
    PyLock lock;
    dict data;
    dict error;
    if (apData->m_ErrorID == 0)
    {
        extract_data(data, &apData->m_Data);
    }
    else
    {
        error["ErrorID"] = apData->m_ErrorID;
    }
    bool lIsLast = apData->m_IsLast ? true : false;
    this->on_rsp_qry_data(data, error, lIsLast);
}

void KDMdApi::process_rtn_data(kd_md_recved_data_t* apData)
{
    PyLock lock;
    dict data;
    extract_data(data, &apData->m_Data);
    this->on_rtn_market_data(apData->m_Data.m_MarketId, apData->m_Data.m_ServiceId, data);
};



///-------------------------------------------------------------------------------------
///主动函数
///-------------------------------------------------------------------------------------

void KDMdApi::create_md_api(string flow_path)
{
    (void)flow_path;
    this->api = KDMdCreate();

    KDMdSetUserData(this->api, this);
    KDMdRegisterHandler(this->api, KDMdApi::md_api_handler_static);
};

string KDMdApi::get_api_version()
{
    std::string lApiVersion = KDMdGetVersion(NULL);
    return lApiVersion;
}

void KDMdApi::release()
{
    KDMdRelease(this->api);
};

void KDMdApi::init(uint32_t timeoutms)
{
    KDMdInit(this->api, timeoutms);
};

int KDMdApi::exit()
{
    //该函数在原生API里没有，用于安全退出API用，原生的join似乎不太稳定
    KDMdRegisterHandler(this->api, NULL);
    KDMdRelease(this->api);
    this->api = NULL;
    return 1;
};

int KDMdApi::set_option(int32_t key, int32_t value)
{
    return KDMdSetOption(this->api, key, &value, sizeof(value));
}

int KDMdApi::get_option(int32_t key, int32_t* pvalue)
{
    uint32_t lValueSize = sizeof(int32_t);
    return KDMdGetOption(this->api, key, pvalue, &lValueSize);
}

void KDMdApi::register_front(string address, uint16_t port)
{
    kd_md_tcp_info_t lTcpInfo;
    memset(&lTcpInfo, 0, sizeof(lTcpInfo));
    strncpy(lTcpInfo.m_ServerAddress, address.c_str(), sizeof(lTcpInfo.m_ServerAddress) - 1);
    lTcpInfo.m_ServerPort = port;
    KDMdRegisterServer(this->api, &lTcpInfo, 1);
};

int KDMdApi::subscribe_market_data(dict req)
{
    kd_md_instrument_key_t lInstrKeys[DEFAULT_MAX_REQ_SIZE];
    memset(&lInstrKeys, 0, sizeof(lInstrKeys));
    uint32_t lCount = get_instrument_key(req, lInstrKeys, DEFAULT_MAX_REQ_SIZE);
    if (lCount == 0) {
        return KD_MD_ERR_RequestMaxCount;
    }
    return KDMdSubscribe(this->api, lInstrKeys, lCount);
};

int KDMdApi::unsubscribe_market_data(dict req)
{
    kd_md_instrument_key_t lInstrKeys[DEFAULT_MAX_REQ_SIZE];
    memset(&lInstrKeys, 0, sizeof(lInstrKeys));
    uint32_t lCount = get_instrument_key(req, lInstrKeys, DEFAULT_MAX_REQ_SIZE);
    if (lCount == 0) {
        return KD_MD_ERR_RequestMaxCount;
    }
    return KDMdUnSubscribe(this->api, lInstrKeys, lCount);
};


int KDMdApi::unsubscribe_all()
{
    return KDMdUnSubscribeAll(this->api);
}

int KDMdApi::req_qry_data(dict req)
{
    int rv;
    kd_md_instrument_key_t lInstrKeys[DEFAULT_MAX_REQ_SIZE];
    memset(&lInstrKeys, 0, sizeof(lInstrKeys));
    uint32_t lCount = get_instrument_key(req, lInstrKeys, DEFAULT_MAX_REQ_SIZE);
    if (lCount == 0) {
        return KD_MD_ERR_RequestMaxCount;
    }
    rv = KDMdReqQryData(this->api, lInstrKeys, lCount);
    return rv;
}

int KDMdApi::req_get_data(dict req, dict out_data, int aTimeoutMS)
{
    int rv;
    kd_md_instrument_key_t lInstrKeys[DEFAULT_MAX_REQ_SIZE];
    memset(&lInstrKeys, 0, sizeof(lInstrKeys));
    uint32_t lCount = get_instrument_key(req, lInstrKeys, DEFAULT_MAX_REQ_SIZE);
    if (lCount == 0) {
        return KD_MD_ERR_RequestMaxCount;
    }

    kd_md_data_t* lpData = NULL;
    uint32_t lDataSize = 1;
    rv = KDMdReqGetData(this->api, lInstrKeys, lCount, &lpData, &lDataSize, aTimeoutMS);
    if (lpData)
    {
        // fprintf(stderr, "[api]reqGetData MI:%d,SI:%d\n", lpData->m_MarketId, lpData->m_ServiceId);
        extract_data(out_data, lpData);
    }
    return rv;
}

int KDMdApi::is_connected()
{
    return KDMdIsConnected(this->api);
}

int KDMdApi::is_logined()
{
    return KDMdIsLogined(this->api);
}

int KDMdApi::open_debug(string aLogFile, int aLogLevel)
{
    return KDMdOpenDebug(this->api, aLogFile.c_str(), aLogLevel);
}


int KDMdApi::req_user_login(dict req, int aTimeoutMS)
{
    char lUserID[32] = "";
    char lPassword[32] = "";
    get_str(req, "UserID", lUserID);
    get_str(req, "Password", lPassword);
    return KDMdReqLogin(this->api, lUserID, lPassword, aTimeoutMS);
};

int KDMdApi::req_user_logout(dict req)
{
    return KDMdReqLogout(this->api);
};

// void KDMdApi::set_user_data(void* udata)
// {
//     KDMdSetUserData(this->api, udata);
// }
// 
// void* KDMdApi::get_user_data()
// {
//     return KDMdGetUserData(this->api);
// }


///-------------------------------------------------------------------------------------
///Boost.Python封装
///-------------------------------------------------------------------------------------

struct KDMdApiWrap : KDMdApi, wrapper < KDMdApi >
{
    virtual void onFrontConnected()
    {
        // fprintf(stderr, "KDMdApiWrap onFrontConnected\n");
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
        .def("create_md_api", &KDMdApiWrap::create_md_api)
        .def("get_api_version", &KDMdApiWrap::get_api_version)
        .def("release", &KDMdApiWrap::release)
        .def("init", &KDMdApiWrap::init)
        .def("exit", &KDMdApiWrap::exit)
        .def("set_option", &KDMdApiWrap::set_option)
        .def("get_option", &KDMdApiWrap::get_option)
        .def("register_front", &KDMdApiWrap::register_front)
        .def("subscribe_market_data", &KDMdApiWrap::subscribe_market_data)
        .def("unsubscribe_market_data", &KDMdApiWrap::unsubscribe_market_data)
        .def("unsubscribe_all", &KDMdApiWrap::unsubscribe_all)
        .def("req_qry_data", &KDMdApiWrap::req_qry_data)
        .def("req_get_data", &KDMdApiWrap::req_get_data)
        .def("req_user_login", &KDMdApiWrap::req_user_login)
        .def("req_user_logout", &KDMdApiWrap::req_user_logout)
        // .def("set_user_data", &KDMdApiWrap::set_user_data)
        // .def("get_user_data", &KDMdApiWrap::get_user_data)
        .def("open_debug", &KDMdApiWrap::open_debug)

        .def("on_front_connected", pure_virtual(&KDMdApiWrap::on_front_connected))
        .def("on_front_disconnected", pure_virtual(&KDMdApiWrap::on_front_disconnected))
        .def("on_rsp_user_login", pure_virtual(&KDMdApiWrap::on_rsp_user_login))
        .def("on_rsp_user_logout", pure_virtual(&KDMdApiWrap::on_rsp_user_logout))
        .def("on_rsp_sub_market_data", pure_virtual(&KDMdApiWrap::on_rsp_sub_market_data))
        .def("on_rsp_unsub_market_data", pure_virtual(&KDMdApiWrap::on_rsp_unsub_market_data))
        .def("on_rsp_qry_data", pure_virtual(&KDMdApiWrap::on_rsp_qry_data))
        .def("on_rtn_market_data", pure_virtual(&KDMdApiWrap::on_rtn_market_data))
        ;
};
