#ifndef _HS_TRADE_STRUCT_H_
#define _HS_TRADE_STRUCT_H_

#include <stdint.h>


/// some common function id
#define UFX_FUNC_QRY_SECINFO        330300      // 证券代码信息查询
#define UFX_FUNC_QRY_CASH_FAST      332254      // 客户资金快速查询
#define UFX_FUNC_QRY_CASH           332255      // 客户资金精确查询
#define UFX_FUNC_PLACE_ORDER        333002      // 普通委托
#define UFX_FUNC_CANCEL_ORDER       333017      // 委托撤单
#define UFX_FUNC_QRY_PURCHASE       333030      // 新股申购查询
#define UFX_FUNC_QRY_ORDER          333101      // 证券委托查询
#define UFX_FUNC_QRY_TRADE          333102      // 证券成交查询
#define UFX_FUNC_QRY_POS_FAST       333103      // 证券持仓快速查询
#define UFX_FUNC_QRY_POSITION       333104      // 证券持仓查询
#define UFX_FUNC_QRY_SECMD          400         // 证券行情查询
#define UFX_FUNC_LOGIN              331100      // 客户登录
#define UFX_FUNC_QRY_ACCOUNTINFO    331155      // 账户信息获取
#define UFX_FUNC_PWD_UPDATE         331101      // 账户密码更改

#define UFX_FUNC_QRY_MATCHINFO_HIST 339301      // 历史配号信息查询
#define UFX_FUNC_QRY_LUCKINFO_HIST  339302      // 历史中签信息查询
#define UFX_FUNC_QRY_ORDER_HIST     339303      // 历史证券委托查询
#define UFX_FUNC_QRY_TRADE_HIST     339304      // 历史证券成交查询
#define UFX_FUNC_QRY_CASH_HIST      339305      // 历史资金证券流水查询

#define UFX_FUNC_HEART              620000      // 心跳
#define UFX_FUNC_SUBSCRIBE          620001      // 订阅回报
#define UFX_FUNC_SUB_CANCEL         620002      // 取消订阅
#define UFX_FUNC_RTN_DATA           620003      // 回报主推

#define UFX_ISSUE_TYPE_TRADE        12          // 成交推送
#define UFX_ISSUE_TYPE_ORDER        23          // 委托推送


/// 一些常用字段长度定义
#define HS_CLIENT_ID_LEN            18
#define HS_STOCK_CODE_LEN           24
#define HS_STOCK_NAME_LEN           16
#define HS_EXCHANGE_TYPE_LEN        4
#define HS_ENTRUST_PROP_LEN         4
#define HS_STOCK_TYPE_LEN           4
#define HS_ENTRUST_NO_LEN           16
#define HS_BUSINESS_NO_LEN          32


/// 交易类别 exchange_type C4
#define HS_EXCHANGE_CASH    "0"         // 资金
#define HS_EXCHANGE_SSE     "1"         // 上海
#define HS_EXCHANGE_SZSE    "2"         // 深圳

/// 买卖方向 entrust_bs C1
#define HS_D_Buy            '1'         // 买入
#define HS_D_Sell           '2'         // 卖出

/// 委托属性 entrust_prop C3
#define HS_EP_BuySell       "0"         // 买卖
#define HS_EP_R             "R"         // 五档市价转限价
#define HS_EP_U             "U"         // 五档市价即成剩撤
#define HS_EP_d             "d"         // 限价委托

/// 证券类别 stock_type C4
#define HS_ST_Stock         "0"         // 股票
#define HS_ST_Fund          "1"         // 基金
#define HS_ST_Bonus         "2"         // 红利
#define HS_ST_NormSubcri    "4"         // 普通申购
#define HS_ST_FundSubRed    "A"         // 基金申赎
#define HS_ST_FundLOF       "L"         // LOF基金
#define HS_ST_ETFSubscri    "M"         // ETF认购
#define HS_ST_ETFRed        "N"         // ETF申赎
#define HS_ST_FundETF       "T"         // ETF基金
#define HS_ST_StockGEM      "c"         // 创业板

/// 委托状态 entrust_status C1
#define HS_ES_NotPlace          '0'     // 未报
#define HS_ES_ToPlace           '1'     // 待报
#define HS_ES_Placed            '2'     // 已报
#define HS_ES_PlacedToCancel    '3'     // 已报待撤
#define HS_ES_PartTradeToCancel '4'     // 部成待撤
#define HS_ES_PartCancel        '5'     // 部撤
#define HS_ES_Cancelled         '6'     // 已撤
#define HS_ES_PartTrade         '7'     // 部成
#define HS_ES_Trade             '8'     // 已成
#define HS_ES_Placing           'C'     // 正报

/// 委托类别 entrust_type C1
#define HS_ET_Entrust           '0'     // 委托
#define HS_ET_Query             '1'     // 查询
#define HS_ET_EntrustCancel     '2'     // 撤单
#define HS_ET_EntrustAgain      '3'     // 补单


/// 密码类别
#define HS_PWD_TYPE_FUND        '1'     // 资金密码
#define HS_PWD_TYPE_TRADE       '2'     // 交易密码


#pragma pack(push,4)

struct HSRspInfoField
{
    int         error_no;               // 错误代码
    char        error_info[256];        // 错误提示
};


///订阅应答数据 620001
struct HSRspSubscribeField
{
    int         branch_no;              // 营业部号
    char        client_id[18];          // 账号
    int         issue_type;             // 成交-12 委托-23
    char        sub_status;             // 订阅状态
};

///查询请求数据
struct HSReqQueryField
{
    int         branch_no;              // 营业部号
    char        client_id[18];          // 账号
    char        exchange_type[4];       // 交易所
    char        stock_code[24];         // 证券代码
};

///客户登录 331100
struct HSReqUserLoginField
{
    char        client_id[18];          // 账号
    char        password[24];           // 密码
    char        password_type;          // 密码类型
    char        mac_address[16];        // 投资者Mac地址
    char        ip_address[16];         // 投资者IP地址
};

///客户登录应答
struct HSRspUserLoginField
{
    char        client_id[18];          // 账号
    char        client_name[16];        // 投资者姓名
    char        company_name[24];       // 公司ID
    int         branch_no;              // 营业部号
    int         trading_date;           // 交易日
    int         pre_trading_date;       // 上个交易日
    int         bill_confirm_flag;      // 账单确认标志
    int         system_no;              // 系统号
    char        asset_prop;             // 资产属性 0-普通客户 1-信用账户 B-衍生品账户
    char        last_op_station[256];   // 上次登录站点
    char        user_token[512];        // 用户口令
};

///报单录入 333002
struct HSReqOrderInsertField
{
    int         branch_no;          // 营业部号
    char        client_id[18];      // 账号 client_id/fund_account
    char        exchange_type[4];   // 交易类别 0-资金 1-上海 2-深圳
    char        stock_code[24];     // 证券代码 or option_code
    char        entrust_prop[4];    // 委托属性 0-买卖 R-市转限 U-市价即成剩撤
    char        entrust_bs;         // 买卖方向 1-买入 2-卖出
    char        entrust_oc;         // 开平标志 O-开仓 C-平仓 X-行权 A-自动行权
    char        covered_flag;       // 备兑标志 
    int         entrust_amount;     // 委托数量
    double      entrust_price;      // 委托价格
    int         batch_no;           // 批次号
};

///撤单请求 333017
struct HSReqOrderActionField
{
    int         branch_no;          // 营业部号
    char        client_id[18];      // 账号
    char        exchange_type[4];   // 交易所
    char        stock_code[24];     // 证券代码
    char        entrust_no[16];     // 委托号
    int         batch_no;           // 批次号
};

///资金账户数据 332255
struct HSTradingAccountField
{
    int         branch_no;          // 营业部号
    char        client_id[18];      // 账号
    double      asset_balance;      // 资产值
    double      begin_balance;      // 期初资金
    double      frozen_balance;     // 冻结资金
    double      current_balance;    // 当前余额
    double      enable_balance;     // 可用资金
    double      fetch_balance;      // 可取资金
    double      real_buy_balance;   // 回报买入金额
    double      real_sell_balance;  // 回报卖出金额
    double      net_asset;          // 净资产
    double      market_value;       // 持仓市值
};


///持仓数据 333104
struct HSPositionField
{
    int         branch_no;          // 营业部号
    char        client_id[18];      // 账号
    char        exchange_type[4];   // 交易所
    char        stock_code[24];     // 证券代码
    char        stock_name[16];     // 名称
    int         init_date;          // 交易日
    char        stock_type[4];      // 证券类别
    int         current_amount;     // 当前数量
    int         enable_amount;      // 可用数量
    int         frozen_amount;      // 冻结数量
    int         real_buy_amount;    // 回报买入数量
    int         real_sell_amount;   // 回报卖出数量
    double      cost_price;         // 成本价
    double      last_price;         // 最新价
    double      market_value;       // 证券市值
    double      income_balance;     // 盈亏金额
    double      cost_balance;       // 持仓成本
    double      profit_ratio;       // 盈亏比例
    char        delist_flag;        // 退市标志
    int         delist_date;        // 退市日期
};

///订单数据 333101
struct HSOrderField
{
    int         branch_no;          // 营业部号
    char        client_id[18];      // 账号
    char        exchange_type[4];   // 交易所
    char        stock_code[24];     // 证券代码
    char        stock_name[16];     // 名称
    char        entrust_no[16];     // 委托号 N10
    char        entrust_prop[4];    // 委托属性
    char        stock_type[4];      // 证券类别
    int         init_date;          // 日期
    int         entrust_date;       // 委托日期
    int         entrust_time;       // 委托时间
    char        entrust_bs;         // 委托方向
    char        entrust_oc;         // 开平标志
    char        covered_flag;       // 备兑标志
    char        entrust_type;       // 委托类别
    char        entrust_status;     // 委托状态
    int         entrust_amount;     // 委托数量
    double      entrust_price;      // 委托价格
    double      business_price;     // 成交价格
    int         business_amount;    // 成交数量
    int         batch_no;           // 批次号
};

///成交数据 333102
struct HSTradeField
{
    int         branch_no;          // 营业部号
    char        client_id[18];      // 账号
    char        stock_code[24];     // 证券代码
    char        stock_name[16];     // 名称
    char        entrust_no[16];     // 委托号
    char        business_no[32];    // 成交编号
    int         date;               // 成交日期
    int         business_time;      // 成交时间
    char        real_type;          // 成交类型 0-买卖 1-查询 2-撤单
    char        real_status;        // 成交状态 0-成交 2-废单 4-确认
    char        exchange_type[4];   // 交易类别 0-资金 1-上海 2-深圳
    char        stock_type[4];      // 证券类别 0-股票 1-基金 2-红利 4-普通申购 A-基金申赎 M/N-ETF认购/申赎 T-ETF基金
    char        entrust_prop[4];    // 委托属性 0-买卖 R-市转限 U-市价即成剩撤
    char        entrust_type;       // 委托类别 0-委托 1-查询 2-撤单 3-补单 4-确认
    char        entrust_status;     // 委托状态 0-未报 1-待报 2-已报 3-已报待撤 4-部成待撤 5-部撤 6-已撤 7-部成 8-已成 9-废单
    char        entrust_bs;         // 委托方向 1-买入 2-卖出
    char        entrust_oc;         // 开平标志 O-开仓 C-平仓 X-行权 A-自动行权
    char        covered_flag;       // 备兑标志 
    char        opt_hold_type;      // 期权持仓类别 0-权利方 1-义务方 2-备兑方
    char        option_type;        // 期权种类 C/P
    int         business_amount;    // 成交数量
    double      business_price;     // 成交价格 or opt_business_price
    double      business_balance;   // 成交金额
    double      entrust_price;      // 委托价格 or opt_entrust_price
    int         entrust_amount;     // 委托数量
    int         batch_no;           // 批次号
};

///证券代码数据
struct HSSecurityInfoField
{
    char        exchange_type[4];   // 交易所
    char        stock_code[24];     // 证券代码
    char        stock_name[16];     // 名称
};

///行情数据
struct HSMarketDataField
{
    char        exchange_type[4];   // 交易所
    char        stock_code[24];     // 证券代码
    int         trading_date;       // 交易日
    int         update_time;        // 更新时间
    double      last_price;
    double      OpenPrice;
    double      HighPrice;
    double      LowPrice;
    double      Turnover;
    int64_t     Volume;

    double      PreClose;
    double      UpperLimit;
    double      LowerLimit;

    double      BidPrice1;
    int         BidVolume1;
    double      AskPrice1;
    int         AskVolume1;
    double      BidPrice2;
    int         BidVolume2;
    double      AskPrice2;
    int         AskVolume2;
    double      BidPrice3;
    int         BidVolume3;
    double      AskPrice3;
    int         AskVolume3;
    double      BidPrice4;
    int         BidVolume4;
    double      AskPrice4;
    int         AskVolume4;
    double      BidPrice5;
    int         BidVolume5;
    double      AskPrice5;
    int         AskVolume5;
};

#pragma pack(pop)

#endif//_HS_TRADE_STRUCT_H_
