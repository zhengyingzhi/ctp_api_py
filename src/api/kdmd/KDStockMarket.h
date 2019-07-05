/*
* Copyright (C) Yingzhi Zheng
* Copyright (C) SZKingdom, Inc.
*/

#ifndef _KD_SECURITY_MARKET_H_
#define _KD_SECURITY_MARKET_H_

#include "KDCommonDef.h"

#pragma pack(push,1)

/// 股票市场产品信息
struct KDStockProductInfo
{
    char        InstrumentID[SECURITY_ID_SIZE];
    char        InstrumentName[SECURITY_NAME_SIZE];

    uint32_t    DecimalPoint;       //价格小数位数 一般A股默认为2 B股/债券/基金/港股通默认为3
    uint32_t    PreClosePrice;      //昨收盘(前盘指数)
    uint32_t    UpperLimitPrice;    //涨停价
    uint32_t    LowerLimitPrice;    //跌停价
    uint16_t    MarketId;           //市场代码编号 KD_MI_XXXX
    char        StopFlag;           //停牌标志
    uint8_t     Padding;            //预留
    uint32_t    PriceTick;          //价格变动
    int64_t     PublicFloatShare;   //流通股本
    uint32_t    TradingDay;         //交易日
    uint16_t    ServiceId;          //数据类型 KD_SI_XXX_Yyyy
    uint16_t    Padding2;           //预留
    int32_t     ListDate;           //上市日期
    int32_t     SecurityType;       //证券类别
    uint16_t    CrdUnderlying;      //融资融券标的
    uint16_t    DRFlag;             //除权除息标志
    uint32_t    DivShareRate;       //送股比例
    uint32_t    DivCashRate;        //派息比例
    uint32_t    StatusFlag;         //0非ST,1-ST,2-*ST,4-退市整理
};


/// 股票市场Level2数据
struct KDStockMarketDataL2
{
    char        InstrumentID[SECURITY_ID_SIZE];
    uint32_t    OpenPrice;          //开盘价
    uint32_t    HighestPrice;       //最高价
    uint32_t    LowestPrice;        //最低价
    uint32_t    LastPrice;          //最新价
    uint32_t    BidPrice[10];       //申买价
    uint32_t    BidVol[10];         //申买量
    uint32_t    AskPrice[10];       //申卖价
    uint32_t    AskVol[10];         //申卖量
    uint32_t    TradingCount;       //成交笔数
    int64_t     Volume;             //成交总量
    int64_t     Turnover;           //成交总金额
    uint32_t    AveragePrice;       //当日均价
    uint32_t    ClosePrice;         //今收盘

    int64_t     TotalBidVol;        //委托买入总量
    int64_t     TotalAskVol;        //委托卖出总量
    uint32_t    WeightedAvgBidPrice;//加权平均委买价格
    uint32_t    WeightedAvgAskPrice;//加权平均委卖价格
    uint32_t    YieldToMaturity;    //到期收益率
    uint32_t    IOPV;               //IOPV净值估值
    uint32_t    PrePrice;           //昨收价 基金时为T-1日收盘时刻IOPV(PreIOPV)

    int32_t     TradingDay;         //交易日(YYYYMMDD)
    int32_t     ActionDay;          //业务发生日(自然日)
    int32_t     UpdateTime;         //最后修改时间(HHMMSSmmm)

    uint32_t    PERatio1;           //市盈率1 2位小数 股票：价格/上年每股利润 债券：每百元应计利息
    uint32_t    PERatio2;           //市盈率2 2位小数 股票：价格/本年每股利润 债券：到期收益率 基金：每百份的IOPV 或净值 权证：溢价率
    uint32_t    PriceUpDown1;       //价格升跌1
    uint32_t    PriceUpDown2;       //价格升跌2（对比上一笔）

    char        Status;             //市场状态
    char        StopFlag;           //停牌标志
    char        Padding[2];         //预留
    uint16_t    MarketId;           //
    uint16_t    ServiceId;          //数据类型 KD_SI_XXX_Yyyy
};

/// 股票Level-1行情
struct KDStockMarketDataL1
{
    char        InstrumentID[SECURITY_ID_SIZE];
    uint32_t    OpenPrice;          //开盘价
    uint32_t    HighestPrice;       //最高价
    uint32_t    LowestPrice;        //最低价
    uint32_t    LastPrice;          //最新价
    uint32_t    BidPrice[5];        //申买价
    uint32_t    BidVol[5];          //申买量
    uint32_t    AskPrice[5];        //申卖价
    uint32_t    AskVol[5];          //申卖量
    int64_t     Volume;             //成交总量
    int64_t     Turnover;           //成交总金额
    uint32_t    AveragePrice;       //当日均价
    uint32_t    ClosePrice;         //今收盘
    uint32_t    IOPV;               //IOPV净值估值
    uint32_t    PrePrice;           //昨收价 基金时为T-1日收盘时刻IOPV(PreIOPV)

    int32_t     TradingDay;         //交易日(YYYYMMDD)
    int32_t     ActionDay;          //业务发生日(自然日)
    int32_t     UpdateTime;         //最后修改时间(HHMMSSmmm)
    char        Status;             //市场状态
    char        StopFlag;           //停牌标志
    char        Padding[2];         //预留
    uint16_t    MarketId;           //
    uint16_t    ServiceId;          //数据类型 KD_SI_XXX_Yyyy
};


/// 股票指数产品信息
struct KDIndexProductInfo
{
    char        InstrumentID[SECURITY_ID_SIZE];
    char        InstrumentName[SECURITY_NAME_SIZE];

    uint32_t    DecimalPoint;       //价格小数位数 指数默认为4
    uint32_t    PreCloseIndex;      //前盘指数
    uint16_t    MarketId;           //市场代码编号 KD_MI_XX
    uint16_t    ServiceId;          //数据类型 KD_SI_XXX_Yyyy
    uint32_t    PriceTick;          //价格变动
    uint32_t    TradingDay;         //交易日
};

/// 指数行情数据
struct KDIndexMarketData
{
    char        InstrumentID[SECURITY_ID_SIZE];

    uint32_t    OpenIndex;          //今开盘指数
    uint32_t    HighIndex;          //最高指数
    uint32_t    LowIndex;           //最低指数
    uint32_t    LastIndex;          //最新指数
    uint32_t    CloseIndex;         //今收盘指数
    uint32_t    PreCloseIndex;      //前盘指数
    int64_t     Volume;             //参与计算相应指数的交易数量
    int64_t     Turnover;           //参与计算相应指数的成交金额

    int32_t     TradingDay;         //交易日YYYYMMDD
    int32_t     ActionDay;          //业务发生日(自然日)
    int32_t     UpdateTime;         //最后修改时间(HHMMSSmmm)
    uint16_t    MarketId;           //
    uint16_t    ServiceId;          //数据类型 KD_SI_XXX_Yyyy
};


/// 个股期权产品信息
struct KDStockOptionProductInfo
{
    char        InstrumentID[20];   //期权合约交易代码   C19 510180C1609M02400
    char        SecurityID[9];      //期权合约的合约编码 C8  11000969
    char        InstrumentName[INSTRUMENT_NAME_SIZE];

    char        Underlying[7];      //标的证券代码
    char        UnderlyingSymbol[9];//基础证券证券名称
    char        UnderlyingType[4];  //标的证券类型 C3 EBS=ETF, ASH=A股
    char        StrikeMode;         //KD_OPT_STM_Xxx E=欧式 A=美式 B=百慕大
    char        CallOrPut;          //KD_CP_Xxx C=认购 P=认沽
    char        PriceLimitType;     //涨跌幅限制类型 C1 'N'为有涨跌幅限制类型

    uint16_t    MarketId;           //市场代码 KD_MI_XXX
    uint32_t    DecimalPoint;       //价格小数位数 期权默认为3
    uint32_t    VolumeMultiple;     //合约乘数
    uint32_t    PriceTick;          //最小变动价格
    uint32_t    StrikePrice;        //执行价
    uint32_t    PreClosePrice;      //昨收盘
    uint32_t    PreSettlementPrice; //昨结算
    uint32_t    PreOpenInterest;    //昨持仓量

    uint32_t    UpperLimitPrice;    //涨停价
    uint32_t    LowerLimitPrice;    //跌停价
    uint32_t    TradingDay;         //交易日
    int32_t     StartDate;          //首个交易日(YYYYMMDD) C8
    int32_t     EndDate;            //最后交易日
    int32_t     ExerciseDate;       //期权行权日
    int32_t     DeliveryDate;       //行权交割日
    int32_t     ExpireDate;         //期权到期日
    uint16_t    ServiceId;          //数据类型 KD_SI_XXX_Yyyy
    uint16_t    Padding;
};

struct KDStockOptionMarketData
{
    char        InstrumentID[20];   //期权合约交易代码
    char        SecurityID[9];      //期权合约的合约编码
    char        Reserve;            //预留字段
    char        Status;             //市场状态
    char        StopFlag;           //停牌标志

    uint32_t    OpenPrice;          //开盘价
    uint32_t    HighestPrice;       //最高价
    uint32_t    LowestPrice;        //最低价
    uint32_t    LastPrice;          //最新价
    uint32_t    BidPrice[5];        //申买价
    uint32_t    BidVol[5];          //申买量
    uint32_t    AskPrice[5];        //申卖价
    uint32_t    AskVol[5];          //申卖量
    int64_t     Volume;             //成交总量
    int64_t     Turnover;           //成交总金额
    uint32_t    OpenInterest;       //当前合约未平仓数量
    uint32_t    ClosePrice;         //今收盘
    uint32_t    SettlementPrice;    //结算价
    uint32_t    AuctionPrice;       //波动性中断参考价
    uint32_t    AuctionQty;         //波动性中断集合竞价虚拟匹配量

    int32_t     TradingDay;         //交易日(YYYYMMDD)
    int32_t     ActionDay;          //业务发生日(自然日)
    int32_t     UpdateTime;         //最后修改时间(HHMMSSmmm)
    uint16_t    MarketId;           //
    uint16_t    ServiceId;          //数据类型 KD_SI_XXX_Yyyy
};


/// 逐笔成交
struct KDStockTransaction
{
    char        InstrumentID[SECURITY_ID_SIZE];
    char        SecurityIDSource[4];//证券代码源 102=深圳证券交易所 103=香港交易所
    char        MDStreamID[3];      //行情类别
    char        ExecType;           //成交类别 KD_ET_Cancelled/Traded 撤销='4',成交='F'
    int64_t     ApplSeqNum;         //消息记录号   从1开始计数
    int64_t     BidOrderNo;         //买方委托序号 从1开始计数,0表示无对应委托 
    int64_t     AskOrderNo;         //卖方委托序号 从1开始计数,0表示无对应委托 
    int64_t     TradeVolume;        //成交数量
    uint32_t    TradePrice;         //成交价格 默认扩大10000倍
    int32_t     TradingDay;         //交易日YYYYMMDD
    int32_t     TransactTime;       //委托时间(HHMMSSmmm)
    uint16_t    ChannelNo;          //频道代码
    uint16_t    MarketId;           //
    uint16_t    ServiceId;          //数据类型 KD_SI_XXX_Yyyy
};

/// 逐笔委托
struct KDStockStepOrder
{
    char        InstrumentID[SECURITY_ID_SIZE];
    char        SecurityIDSource[4];//证券代码源 102=深圳证券交易所 103=香港交易所
    char        MDStreamID[3];      //行情类别 KD_MDSTREAMID_011/021/041/051/052/071
    char        Side;               //买卖方向 KD_SIDE_Buy/Sell 买='1', 卖='2',借入='G',借出='F'
    int64_t     ApplSeqNum;         //消息记录号 从1开始计数
    uint32_t    OrderPrice;         //委托价格 默认扩大10000倍
    uint32_t    OrderVolume;        //委托数量
    int32_t     TradingDay;         //交易日YYYYMMDD
    int32_t     OrderTime;          //委托时间(HHMMSSmmm)
    uint16_t    ChannelNo;          //频道代码
    uint16_t    MarketId;           //
    uint16_t    ServiceId;          //数据类型 KD_SI_XXX_Yyyy

    char        ExtendFields[1];    //扩展字段 根据MDStreamID区分 KDStepOrder_ExtendXXX
};

///现货集中竞价MDStreamID=011、质押式回购MDStreamID=021、期权集中竞价MDStreamID=041 逐笔委托行情扩展字段
struct KDStockStepOrder_Extend192
{
    char        OrderType;          //委托类型 KD_OT_Market/LimitPrice 市价='1', 限价='2' 本方最优='U'
};

///协议交易逐笔意向MDStreamID=051、协议交易逐笔定价MDStreamID=052 逐笔委托行情扩展字段
struct KDStockStepOrder_Extend592
{
    char        ConfirmID[8];       //定价行情约定号
    char        Contactor[12];      //联系人
    char        ContactInfo[30];    //联系方式
};

///转融通证券出借业务MDStreamID=071 逐笔委托行情扩展字段
struct KDStockStepOrder_Extend792
{
    uint16_t    ExpirationDays;     //期限,单位为天数
    char        ExpirationType;     //期限类型 1=固定期限
};


/// K线数据
struct KDKLine
{
    char        InstrumentID[SECURITY_ID_SIZE];
    int32_t     Day;                // 日期 YYYYMMDD
    int32_t     Time;               // 时间(北京时间) HHMMSS
    uint32_t    Open;               // 开盘价
    uint32_t    High;               // 最高价
    uint32_t    Low;                // 最低价
    uint32_t    Close;              // 收盘价
    int64_t     Volume;             // 分钟内成交量   单位：该证券的最小交易单位，比如股票为“股”
    int64_t     Turnover;           // 分钟内成交额   单位：元
    uint32_t    ValIncrease;        // 涨跌值         保留
    uint32_t    Reserve;            // 保留字段
    uint16_t    MarketId;           //
    uint16_t    SecurityType;       // 证券类别 KD_SECT_Xxxx
    char        Padding[4];         // 
};

#pragma pack(pop)

#endif//_KD_SECURITY_MARKET_H_
