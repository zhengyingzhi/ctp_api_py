/*
* Copyright (C) Yingzhi Zheng
* Copyright (C) SZKingdom, Inc.
*/

#ifndef _KD_COMMON_DEF_H_
#define _KD_COMMON_DEF_H_


#include <stdint.h>
#include <limits.h>

/// 浮点数小数位数，在KDXxxxProductInfo.DecimalPoint字段中展示
/// 一般情况下行情数据中的价格类均扩大n倍为整数表示，数量未扩大，
/// 例如商品期货若其PriceTick大于等于1元，则DecimalPoint==1，未进行倍数扩大，DecimalPoint==2，则表示扩大了100倍
/// 其他如Volume，Qty，OpenInterest字段则未进行扩大
#define KD_FLOAT_Multiple1          1
#define KD_FLOAT_Multiple10         10
#define KD_FLOAT_Multiple100        100
#define KD_FLOAT_Multiple1000       1000
#define KD_FLOAT_Multiple10000      10000

#define KD_DEFAULT_DP_AStock        2           //A股默认2位小数
#define KD_DEFAULT_DP_BStock        3           //B股默认3位小数
#define KD_DEFAULT_DP_Fund          3           //基金默认3位小数
#define KD_DEFAULT_DP_Bond          3           //债券默认3位小数
#define KD_DEFAULT_DP_Index         4           //指数默认4位小数


/// 无效的价格，若价格可以为负数的价格，则其值>INT_MAX，
/// 可强转成int32并除以对应的倍数，即可得到对应的负数价格
#define KD_INVALID_Price            INT_MAX

/// 合约大小
#define SECURITY_ID_SIZE            8
#define SECURITY_NAME_SIZE          32
#define INSTRUMENT_ID_SIZE          32
#define INSTRUMENT_NAME_SIZE        24


/// 各交易所编码名
#define KD_EXCHANGE_SSE            "SSE"        //上交所
#define KD_EXCHANGE_SHOP           "SHOP"       //上交所期权
#define KD_EXCHANGE_SZSE           "SZSE"       //深交所
#define KD_EXCHANGE_SZOP           "SZOP"       //深交所期权
#define KD_EXCHANGE_SHHK           "SHHK"       //沪港通市场
#define KD_EXCHANGE_SZHK           "SZHK"       //深港通市场
#define KD_EXCHANGE_NEEQ           "NEEQ"       //新三板
#define KD_EXCHANGE_CFFEX          "CFFEX"      //中金所
#define KD_EXCHANGE_SHFE           "SHFE"       //上期所
#define KD_EXCHANGE_DCE            "DCE"        //大商所
#define KD_EXCHANGE_CZCE           "CZCE"       //郑商所
#define KD_EXCHANGE_INE            "INE"        //上能所
#define KD_EXCHANGE_SGE            "SGE"        //上金交所
#define KD_EXCHANGE_OTHER          "OTHER"      //其它市场

/// 各交易所编码简称
#define KD_EX_SH                   "SH"         //上海
#define KD_EX_SZ                   "SZ"         //深圳
#define KD_EX_SHK                  "SHK"        //沪港通
#define KD_EX_ZHK                  "ZHK"        //深港通
#define KD_EX_NQ                   "NQ"         //新三板
#define KD_EX_ZJ                   "ZJ"         //中金
#define KD_EX_SQ                   "SQ"         //上期
#define KD_EX_DL                   "DL"         //大连
#define KD_EX_ZZ                   "ZZ"         //郑州
#define KD_EX_SN                   "SN"         //上能
#define KD_EX_SG                   "SG"         //上金
#define KD_EX_OTHER                "OTHER"      //其它市场


/// 市场代码编号定义 Market ID
#define KD_MI_NONE                  0           //None
#define KD_MI_SSE                   1           //上海市场
#define KD_MI_SZSE                  2           //深圳市场
#define KD_MI_SHOP                  3           //上海期权市场
#define KD_MI_SZOP                  4           //深圳期权市场
#define KD_MI_SHHK                  5           //沪港通
#define KD_MI_SZHK                  6           //深港通
#define KD_MI_NEEQ                  7           //新三板
#define KD_MI_CFFEX                 8           //中金所
#define KD_MI_SHFE                  9           //上期所
#define KD_MI_DCE                   10          //大商所
#define KD_MI_CZCE                  11          //郑商所
#define KD_MI_INE                   12          //上海能源中心
#define KD_MI_SGE                   13          //上海金交所市场
#define KD_MI_INNER1                14          //内部使用市场1
#define KD_MI_INNER2                15          //内部使用市场2
#define KD_MI_OTHER                 16          //其它市场


/// 市场数据服务号 Service ID
#define KD_SI_None                  0           //无
#define KD_SI_MarketInfo            1           //市场信息
#define KD_SI_IDX_ProductInfo       2           //指数产品信息
#define KD_SI_IDX_MarketData        3           //指数行情数据
#define KD_SI_STK_ProductInfo       4           //股票产品信息
#define KD_SI_STK_MarketDataL1      5           //股票行情数据L1
#define KD_SI_STK_MarketDataL2      6           //股票行情数据L2
#define KD_SI_STK_OptProductInfo    7           //股票期权产品信息
#define KD_SI_STK_OptMarketData     8           //股票期权行情
#define KD_SI_STK_Transaction       9           //深圳逐笔成交
#define KD_SI_STK_StepOrder         10          //深圳逐笔委托
#define KD_SI_KLineData             11          //K线数据
#define KD_SI_BOND_ProductInfo      12          //债券产品信息
#define KD_SI_BOND_MarketData       13          //债券行情L1
#define KD_SI_FUND_ProductInfo      14          //基金产品信息
#define KD_SI_FUND_MarketData       15          //基金行情L1

#define KD_SI_FUT_ProductInfo       18          //期货产品信息
#define KD_SI_FUT_MarketData        19          //期货行情数据
#define KD_SI_FUT_ForQuote          20          //期货期权询价通知


/// 证券类别 Security Type
#define KD_SECT_Index                1          //指数
#define KD_SECT_Stock                2          //股票
#define KD_SECT_Bond                 3          //债券
#define KD_SECT_Fund                 4          //基金
#define KD_SECT_Option               5          //期权


/// 停牌标志 Stop Flag
#define KD_SF_Normal               'F'          //正常
#define KD_SF_NotStart             'N'          //不可恢复交易的熔断时段
#define KD_SF_TempPause            'M'          //临时停牌(可恢复交易的熔断时段)
#define KD_SF_Pause                'T'          //停牌

/// 市场状态 Market Status
#define KD_MS_Unknown              ' '          //未知
#define KD_MS_BeforeTrading        '0'          //开盘前
#define KD_MS_NoTrading            '1'          //非交易(休市)
#define KD_MS_Continous            '2'          //连续交易
#define KD_MS_AuctionOrdering      '3'          //集合竞价报单
#define KD_MS_AuctionBalance       '4'          //集合竞价价格平衡
#define KD_MS_AuctionMatch         '5'          //集合竞价撮合
#define KD_MS_Volatility           '6'          //波动性中断
#define KD_MS_AuctionClose         '7'          //收盘集合竞价
#define KD_MS_Closed               '8'          //收盘
#define KD_MS_AfterTrading         '9'          //盘后交易

/// 产品类型 Product Class
#define KD_PC_Unknown              ' '          //未知
#define KD_PC_Future               '1'          //期货
#define KD_PC_Options              '2'          //期货期权
#define KD_PC_Combination          '3'          //组合
#define KD_PC_Spot                 '4'          //即期
#define KD_PC_EFP                  '5'          //期转现
#define KD_PC_SpotOption           '6'          //现货期权

/// 期权执行方式 Stike Mode
#define KD_OPT_STM_Continental     'E'          //欧式
#define KD_OPT_STM_American        'A'          //美式
#define KD_OPT_STM_Bermuda         'B'          //百慕大

/// 期权类型 Options Type
#define KD_CP_NoneOptions          ' '          //未知期权
#define KD_CP_CallOptions          'C'          //看涨(认购)
#define KD_CP_PutOptions           'P'          //看跌(认沽)


/// API消息类型 Message Type
#define KD_MT_None                  0           // 空类型
#define KD_MT_Connected             1           // 已连接
#define KD_MT_Disconnected          2           // 连接断开
#define KD_MT_Login                 4           // 登录
#define KD_MT_Logout                5           // 登出
#define KD_MT_HeartBeat             6           // 心跳

#define KD_MT_Subscribe             7           // 订阅
#define KD_MT_UnSubscribe           8           // 取消订阅
#define KD_MT_Query                 9           // 查询产品
#define KD_MT_QueryAndSubscribe     10          // 数据推送
#define KD_MT_Publish               11          // 数据推送
#define KD_MT_Notice                12          // 系统公告消息


/// 成交类型 ExecType(TradeType)
#define KD_ET_Unknown               ' '         // 未知
#define KD_ET_Cancelled             '4'         // 撤销
#define KD_ET_Traded                'F'         // 成交

/// 买卖方向 Side
#define KD_SIDE_Unknown             ' '         // 未知
#define KD_SIDE_Buy                 '1'         // 买入
#define KD_SIDE_Sell                '2'         // 卖出
#define KD_SIDE_Borrow              'G'         // 借入
#define KD_SIDE_Lend                'F'         // 借出

/// 订单类型 OrderType
#define KD_OT_Unknown               ' '         // 未知
#define KD_OT_MarketPrice           '1'         // 市价
#define KD_OT_LimitPrice            '2'         // 限价
#define KD_OT_BestPrice             'U'         // 本方最优

/// 证券代码源
#define KD_SECURITY_SOURCEID_SZSE   "102"       // 深圳证券交易所
#define KD_SECURITY_SOURCEID_HKEX   "103"       // 香港交易所

/// 逐笔行情类别 MDStreamID
#define KD_MDSTREAMID_011           "011"       // 现货（股票，基金，债券等）集中竞价交易逐笔行情(192)
#define KD_MDSTREAMID_021           "021"       // 质押式回购交易逐笔行情(192)
#define KD_MDSTREAMID_041           "041"       // 期权集中竞价交易逐笔行情(192)
#define KD_MDSTREAMID_051           "051"       // 协议交易逐笔意向行情(592)
#define KD_MDSTREAMID_052           "052"       // 协议交易逐笔定价行情(592)
#define KD_MDSTREAMID_071           "071"       // 转融通证券出借逐笔行情(792)


/// API错误ID(包括接口返回值和回调函数中的ErrorID)
#define KD_MD_OK                    0           // 正确
#define KD_MD_Error                 -1          // 错误
#define KD_MD_ERR_RequestControl    -2          // 请求流控
#define KD_MD_ERR_NotConnected      -3          // 未连接
#define KD_MD_ERR_AlreadyConnected  -4          // 已连接
#define KD_MD_ERR_NotLogined        -5          // 未登录
#define KD_MD_ERR_AlreadyLogined    -6          // 已登录
#define KD_MD_ERR_UserIDOrPassword  -7          // 用户名或密码错误
#define KD_MD_ERR_InvalidMarketId   -8          // 无效的市场ID
#define KD_MD_ERR_InvalidServiceId  -9          // 无效的数据ID
#define KD_MD_ERR_InvalidInstrument -10         // 无效的代码
#define KD_MD_ERR_RequestMaxCount   -11         // 超过最大请求数
#define KD_MD_ERR_Timedout          -12         // 超时
#define KD_MD_ERR_APIBusy           -13         // API尚有请求未完成
#define KD_MD_ERR_NotSupport        -14         // 暂不支持的功能
#define KD_MD_ERR_InvalidParam      -15         // 无效的参数值
#define KD_MD_ERR_NoData            -16         // 未查询到数据
#define KD_MD_ERR_ServerNoUDP       -17         // 服务器不支持UDP(API只能使用TCP方式订阅行情)
#define KD_MD_ERR_ServerOnlyUDP     -18         // 服务器只支持UDP(API只能使用UDP方式订阅行情)
#define KD_MD_ERR_UdpMulticastInfo  -19         // 错误的UDP组播信息
#define KD_MD_ERR_UdpMulticastStart -20         // 启动UDP组播失败
#define KD_MD_ERR_NoEnoughMemory    -21         // 内存空间不够
#define KD_MD_ERR_NoRight           -22         // 无权限
#define KD_MD_ERR_ZipData           -23         // 无法识别的压缩数据


#define KD_MD_OPTION_TCPInfo            0x01        // TCP连接信息 参数值需为 kd_md_tcp_info_t 类型
#define KD_MD_OPTION_ProxyInfo          0x02        // TCP代理服务信息 参数值需为 kd_md_proxy_info_t 类型
#define KD_MD_OPTION_UDPInfo            0x03        // UDP地址信息 参数值需为 kd_md_udp_info_t 类型
#define KD_MD_OPTION_SSL                0x04        // 启用SSL加密连接 0-禁用 1-单向认证 2-双向认证
#define KD_MD_OPTION_SSL_CAFile         0x05        // 使用SSL加密连接且双向认证时的CA文件路径（默认为kdmd_cacert.pem）
#define KD_MD_OPTION_SSL_CertFile       0x06        // 使用SSL加密连接时的证书文件文件路径（默认为kdmd_api_cert.pem）
#define KD_MD_OPTION_SSL_KeyFile        0x07        // 使用SSL加密连接时的Key文件路径（默认为kdmd_api_key.pem）
#define KD_MD_OPTION_Compress           0x08        // 启用压缩
#define KD_MD_OPTION_ReconnectInterval  0x09        // 重连时间间隔毫秒
#define KD_MD_OPTION_HeartBeatInterval  0x10        // 心跳时间间隔毫秒
#define KD_MD_OPTION_SORcvBufSize       0x11        // SOCKET接收缓冲区大小
#define KD_MD_OPTION_AutoReSubscribe    0x12        // API断开重连后，自动重新订阅，API默认不启用
#define KD_MD_OPTION_SubscribeOnly      0x13        // API仅请求订阅，不自动查询（默认是至少会推送一笔）

#define KD_MD_SSL_VAL_AuthDisabled      0x00        // 设置选项值 KD_MD_OPTION_SSL 时的可选值，默认0为禁用SSL
#define KD_MD_SSL_VAL_AuthOneWay        0x01        // SSL启用时 1-单向认证
#define KD_MD_SSL_VAL_AuthTwoWay        0x02        // SSL启用时 2-双向认证

#define KD_MD_COMPRESS_VAL_Disabled      0x00       // 设置选项值 KD_MD_OPTION_Compress 时的可选值
#define KD_MD_COMPRESS_VAL_AggregateOnly 0x01       // 用于低速网络的行情定时延迟推送
#define KD_MD_COMPRESS_VAL_AggregateZip  0x02       // 使用ZIP方法压缩并定时延迟推送数据
#define KD_MD_COMPRESS_VAL_Zip           0x03       // 使用ZIP方法压缩传输数据
#define KD_MD_COMPRESS_VAL_AggregateFast 0x04       // 使用FAST协议方法压缩并定时延迟推送数据
#define KD_MD_COMPRESS_VAL_Fast          0x05       // 使用FAST协议方法压缩传输数据


/// TCP代理类型
#define KD_MD_PROXYTYPE_NONE            0           // 未代理
#define KD_MD_PROXYTYPE_SOCKS4          1           // SOCKS4类型代理
#define KD_MD_PROXYTYPE_SOCKS4A         1           // SOCKS4A类型代理
#define KD_MD_PROXYTYPE_SOCKS5          2           // SOCKS5类型代理


#pragma pack(push,1)


/* tcp connection info */
typedef struct
{
    char        m_ServerAddress[16];    // 服务器地址，如192.168.8.59
    uint16_t    m_ServerPort;           // 服务器端口
}kd_md_tcp_info_t;

/* socket proxy info */
typedef struct
{
    char        m_ProxyAddress[16];     // 代理服务器地址，如192.168.8.59
    uint16_t    m_ProxyPort;            // 代理服务器端口
    uint16_t    m_ProxyType;            // 代理服务类型 KD_MD_PROXYTYPE_XXX
    char        m_UserID[24];           // 用户名
    char        m_Password[24];         // 密码
}kd_md_proxy_info_t;

/* udp multicast info */
typedef struct
{
    char        m_BindAddress[16];      // 绑定网卡地址，如192.168.8.59 0.0.0.0表示INADDR_ANY 
    char        m_GroupAddress[16];     // 总线UDP组播地址，如224.1.1.3
    uint16_t    m_MainPort;             // 主数据总线UDP端口
}kd_md_udp_info_t;

typedef struct
{
    uint16_t    m_MarketId;             // 市场ID KD_MI_Xxx，如KD_MI_SSE
    uint16_t    m_ServiceId;            // 服务数据ID KD_SI_XXX_Yyyy，如 KD_SI_IDX_MarketData
    char        m_InstrumentID[32];     // 一个合约代码 "000001" 或者 "IF1803" 等
}kd_md_instrument_key_t;

typedef struct 
{
    char        m_MarketName[8];        // 交易所名字 KD_EXCHANGE_SSE/SHOP/SZSE/CFFEX 等
    uint16_t    m_MarketId;             // 交易所编号 KD_MI_SSE/SHOP/SZSE 等
    uint16_t    m_ServiceId[11];        // 交易所支持的数据服务ID，如 Index/Level1/Level2/StockTransaction
}kd_md_market_info_t;

typedef struct kd_md_data_s
{
    uint32_t    m_DataSize;             // m_pDataInfo 的数据大小
    uint16_t    m_MarketId;             // 市场ID KD_MI_XX, 如 KD_MI_SSE
    uint16_t    m_ServiceId;            // 数据ID KD_SI_XXX_Yyyy, 如 KD_SI_IDX_MarketData
    char*       m_pDataInfo;            // 指向具体数据
                                        // KD_MT_Login 时，指向登录的UserID
                                        // KD_MT_Query 时，指向具体的产品信息(KDXxxxxProductInfo)或行情数据(KDXxxxxMarketDataX)
                                        //                 但查询失败时（ErrorID!=0），则指向请求时的合约代码
                                        //                 当ServiceId==KD_SI_MarketInfo时指向kd_md_market_info_t信息
                                        // KD_MT_Subscribe/UnSub 时，指向请求时的合约代码，如 000001或IF1712等
                                        // KD_MT_Publish 时，指向行情数据结构
}kd_md_data_t;

/* 服务器向API的应答数据 */
typedef struct kd_md_recved_data_s
{
    int32_t         m_ErrorID;          // 错误ID(KD_MD_ERR_Xxxx)
    int32_t         m_IsLast;           // 是否最后一条数据

    kd_md_data_t    m_Data;             // API回调数据，消息类型为KD_MT_Connect/Disconnect 时为无效数据
}kd_md_recved_data_t;

#pragma pack(pop)

#endif//_KD_COMMON_DEF_H_
