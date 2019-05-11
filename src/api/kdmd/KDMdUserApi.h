#ifndef _KD_MD_USERAPI_H_
#define _KD_MD_USERAPI_H_

#if defined(_WIN32)
#ifdef KDMDUSERAPI_EXPORTS
#define KD_MD_API __declspec(dllexport)
#else
#define KD_MD_API __declspec(dllimport)
#endif
#define KD_MD_STDCALL   __stdcall       /* ensure stcall calling convention on NT */

#else
#define KD_MD_API
#define KD_MD_STDCALL                   /* leave blank for other systems */

#endif//_WIN32

#include "KDCommonDef.h"


#ifdef __cplusplus
extern "C"
{
#endif//__cplusplus

/* the exported types */
typedef struct kd_md_api_s kd_md_api_t;

/* api event handler
 * aMsgType: 消息类型 KD_MT_Xxx，如 KD_MT_Publish
 * apData:   应答或推送数据，根据消息类型区分，不为NULL，详见KDCommonDef.h
 */
typedef void(*kd_md_api_handler_t)(kd_md_api_t* apMdApi, uint32_t aMsgType, kd_md_recved_data_t* apData);


/* the exported interfaces */

/// @note  API返回值 KD_MD_OK 表示成功，其它表示错误，错误ID格式 KD_MD_ERR_Xxxx ，详见KDCommonDef.h
KD_MD_API const char*   KD_MD_STDCALL KDMdGetVersion(int* apVersion);

/// @brief  创建一个API对象
KD_MD_API kd_md_api_t*  KD_MD_STDCALL KDMdCreate();

/// @brief  释放一个API对象
KD_MD_API void  KD_MD_STDCALL KDMdRelease(kd_md_api_t* apMdApi);

/// @brief  注册服务器IP和端口
/// @param  apServerInfo    服务器TCP信息地址，一次注册一个或多个信息
/// @param  aServerInfoSize 服务器TCP信息地址个数(最多4个)
KD_MD_API int   KD_MD_STDCALL KDMdRegisterServer(kd_md_api_t* apMdApi, const kd_md_tcp_info_t apServerInfo[], uint32_t aServerInfoSize);

/// @brief  注册代理服务器IP和端口
KD_MD_API int   KD_MD_STDCALL KDMdRegisterProxyServer(kd_md_api_t* apMdApi, const kd_md_proxy_info_t* apProxyInfo);

/// @brief  注册UDP组播IP和端口等
/// @note   注册成功后，则API不再通过TCP通道接收实时推送行情，且需要在KDMdInit前调用
///         注意，一台机器只能有一个API使用UDP接收行情
KD_MD_API int   KD_MD_STDCALL KDMdRegisterUdpInfo(kd_md_api_t* apMdApi, const kd_md_udp_info_t* apUdpInfo);

/// @brief  注册异步消息处理函数
KD_MD_API int   KD_MD_STDCALL KDMdRegisterHandler(kd_md_api_t* apMdApi, kd_md_api_handler_t apHandler);

/// @brief  设置API的一些属性，可选值为 KD_MD_OPTION_Xxx 
KD_MD_API int   KD_MD_STDCALL KDMdSetOption(kd_md_api_t* apMdApi, int32_t aOption, void* apOptionVal, uint32_t aOptionValSize);

/// @brief  获取API的一些属性，可选值为 KD_MD_OPTION_Xxx 
/// @note   apInOutOptionValSize为输入输出参数，输入为apOutOptionVal的有效长度，函数返回0成功时，输出实际数据长度
KD_MD_API int   KD_MD_STDCALL KDMdGetOption(kd_md_api_t* apMdApi, int32_t aOption, void* apOutOptionVal, uint32_t* apInOutOptionValSize);

/// @brief  初始化API连接等操作
/// @param  aConnectTimeoutMS==0则表示异步连接（否则为同步连接），连接成功时 kd_md_api_handler_t 回调通知
/// @return 0成功，其它-失败
/// @note   当为异步连接时，返回0并不一定表示连接成功。调用该接口后，API具有自动重连功能（无论同步还是异步连接）
KD_MD_API int   KD_MD_STDCALL KDMdInit(kd_md_api_t* apMdApi, uint32_t aConnectTimeoutMS);

/// @brief  登录请求
/// @param  aTimeoutMS==0则表示异步登录请求，登录响应由 kd_md_api_handler_t 回调通知，
///         aTimeoutMS >0则为同步登录，返回值0表示登录成功，否则返回对应的错误ID
KD_MD_API int   KD_MD_STDCALL KDMdReqLogin(kd_md_api_t* apMdApi, const char* apUserID, const char* apPasswd, uint32_t aTimeoutMS);

/// @brief  登出请求
/// @note   登出后将不再收到任何推送数据，且重新登录后需要重新订阅行情
KD_MD_API int   KD_MD_STDCALL KDMdReqLogout(kd_md_api_t* apMdApi);

/// @brief  异步查询服务器支持的交易市场和对应支持的行情数据种类
/// @param  aMarketIdAray   查询市场ID的数组，一次可查询一个或多个市场的信息，若数组中的MarketId==KD_MI_NONE表示查询所有市场
/// @param  aArraySize      查询市场ID的数组大小
/// @note   查询条件参数传入空数组和aArraySize==0时，表示查询所有市场信息
KD_MD_API int   KD_MD_STDCALL KDMdReqQryMarketInfo(kd_md_api_t* apMdApi, uint16_t aMarketIdArray[], uint32_t aArraySize);

/// @brief  同步步查询服务器支持的交易市场和对应支持的行情数据种类
/// @param  aMarketIdAray   查询市场ID的数组，一次可指定一个或多个市场的信息，若数组中的MarketId==KD_MI_NONE表示查询所有市场
/// @param  aArraySize      查询市场ID的数组大小
/// @param  appOutDataArray 查询结果的数组首地址
/// @param  apOutArraySize  查询结果的数组元素个数
/// @param  aTimeoutMS      查询超时时间，单位为毫秒
/// @note   查询条件参数传入空数组和aArraySize==0时，表示查询所有市场信息
KD_MD_API int   KD_MD_STDCALL KDMdReqGetMarketInfo(kd_md_api_t* apMdApi, uint16_t aMarketIdArray[], uint32_t aArraySize,
    kd_md_market_info_t** appOutDataArray, uint32_t* apOutArraySize, uint32_t aTimeoutMS);

/// @brief  异步查询产品信息或行情数据
/// @param  apInstruments 请求查询数据的数组
/// @param  aArraySize    apInstruments 数组的大小
/// @note   查询产品信息时 ServiceId可选值为KD_SI_Index/Stock/StockOpt/FutureProductInfo等
///         查询行情数据时 ServiceId可指定为KD_SI_None，默认查询行情网关服务所支持的Level1或Level2行情
///                       也可以指定为KD_SI_Index/Stock/StockOpt/FutureMarketData等行情数据ID
KD_MD_API int   KD_MD_STDCALL KDMdReqQryData(kd_md_api_t* apMdApi, kd_md_instrument_key_t apInstruments[], uint32_t aArraySize);

/// @brief  同步查询产品信息或行情数据
/// @param  apInstruments   请求查询数据的数组
/// @param  aArraySize      apInstruments 数组的大小
/// @param  appOutDataArray 指向查询到的结果数据的数组首地址
/// @param  apOutDataSize   输入为数组大小，输出为查询结果数组大小
/// @note   查询产品信息时   ServiceId可选值为KD_SI_Index/Stock/StockOpt/FutureProductInfo等
///         查询行情数据时   ServiceId可指定为KD_SI_None，默认查询行情网关服务所支持的Level1或Level2行情
///                         也可以指定为KD_SI_Index/Stock/StockOpt/FutureMarketData等行情数据ID
KD_MD_API int   KD_MD_STDCALL KDMdReqGetData(kd_md_api_t* apMdApi,
    kd_md_instrument_key_t apInstruments[], uint32_t aArraySize,
    kd_md_data_t** appOutDataArray, uint32_t* apOutDataSize,
    uint32_t aTimeoutMS);


/// @brief  订阅/取消订阅一个或多个合约的行情
/// @param  apInstruments   请求订阅数据的数组，若其中的m_InstrumentIDs=="*"，表示该市场的所有合约
/// @param  aArraySize      apInstruments 数组的大小
/// @note   ServiceId       可指定为KD_SI_None，默认订阅情网关服务所支持的Level1或Level2行情，也可以指定具体的行情数据ID
KD_MD_API int   KD_MD_STDCALL KDMdSubscribe(kd_md_api_t* apMdApi, kd_md_instrument_key_t apInstruments[], uint32_t aArraySize);
KD_MD_API int   KD_MD_STDCALL KDMdUnSubscribe(kd_md_api_t* apMdApi, kd_md_instrument_key_t apInstruments[], uint32_t aArraySize);

/// @brief  取消所有已订阅的行情
KD_MD_API int   KD_MD_STDCALL KDMdUnSubscribeAll(kd_md_api_t* apMdApi);

/// @brief  是否已连接
/// @return 0-未连接 1-已连接
KD_MD_API int   KD_MD_STDCALL KDMdIsConnected(kd_md_api_t* apMdApi);

/// @brief  是否已登录
/// @return 0-未登录 1-已登录成功
KD_MD_API int   KD_MD_STDCALL KDMdIsLogined(kd_md_api_t* apMdApi);

/// @brief  设置/获取用户指定数据
KD_MD_API void  KD_MD_STDCALL KDMdSetUserData(kd_md_api_t* apMdApi, void* apUserData);
KD_MD_API void* KD_MD_STDCALL KDMdGetUserData(kd_md_api_t* apMdApi);

/// @brief  打开调试日志
/// @param  apLogFile 日志文件名
/// @param  aLogLevel 日志级别：1-debug 3-warn 其它-info
KD_MD_API int   KD_MD_STDCALL KDMdOpenDebug(kd_md_api_t* apMdApi, const char* apLogFile, int aLogLevel);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif//_KD_MD_USER_API_H_
