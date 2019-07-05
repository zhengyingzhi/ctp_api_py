/*
* Copyright (C) Yingzhi Zheng
* Copyright (C) SZKingdom, Inc.
*/

#ifndef _KD_FUTURE_MARKET_H_
#define _KD_FUTURE_MARKET_H_

#include "KDCommonDef.h"


#pragma pack(push,1)

/// 期货产品信息
struct KDFutureProductInfo
{
    char        InstrumentID[INSTRUMENT_ID_SIZE];
    char        InstrumentName[INSTRUMENT_NAME_SIZE];

    uint16_t    MarketId;           //市场代码 KD_MI_XXX
    char        ProductClass;       //产品类型 KD_PC_Xxxx 期货/期权/组合
    char        OptionType;         //期权类型 KD_CP_Call/PutOptions
    uint32_t    DecimalPoint;       //价格小数位数 如大多商品期货为0或1位
    uint32_t    VolumeMultiple;     //合约乘数 N(0)
    uint32_t    PriceTick;          //最小变动价格
    uint32_t    StrikePrice;        //执行价
    uint32_t    PreClosePrice;      //昨收盘
    uint32_t    PreSettlementPrice; //昨结算
    uint32_t    PreOpenInterest;    //昨持仓量

    uint32_t    UpperLimitPrice;    //涨停价
    uint32_t    LowerLimitPrice;    //跌停价
    uint32_t    TradingDay;         //交易日
    uint16_t    ServiceId;          //数据类型 KD_SI_XXX_Yyyy
    uint16_t    Padding;
};

/// 期货市场数据
struct KDFutureMarketData
{
    char        InstrumentID[INSTRUMENT_ID_SIZE];
    uint32_t    OpenPrice;          //开盘价
    uint32_t    HighestPrice;       //最高价
    uint32_t    LowestPrice;        //最低价
    uint32_t    LastPrice;          //最新价
    uint32_t    BidPrice[5];        //申买价
    uint32_t    BidVol[5];          //申买量
    uint32_t    AskPrice[5];        //申卖价
    uint32_t    AskVol[5];          //申卖量
    int64_t     Volume;             //成交总量 N(2)
    int64_t     Turnover;           //成交总金额
    uint32_t    AveragePrice;       //当日均价
    uint32_t    OpenInterest;       //持仓量 N(4)
    uint32_t    ClosePrice;         //今收盘
    uint32_t    SettlementPrice;    //结算价
    uint32_t    PrePrice;           //昨收盘价

    int32_t     TradingDay;         //交易日
    int32_t     ActionDay;          //业务发生日(自然日)
    int32_t     UpdateTime;         //最后修改时间(HHMMSSmmm)

    uint32_t    PreDelta;           //昨虚实度
    uint32_t    CurrDelta;          //今虚实度
    uint16_t    MarketId;           //市场代码 KD_MI_XXX
    uint16_t    ServiceId;          //数据类型 KD_SI_XXX_Yyyy
};


#define KD_SOURCE_Member    0       //会员
#define KD_SOURCE_Exchange  1       //交易所

///期权询价通知
struct KDForQuoteData
{
    char        InstrumentID[32];   //合约代码
    char        ExchangeID[8];      //交易所 KD_EXCHANGE_XXX
    char        QuoteID[22];        //询价编号
    short       Source;             //来源 0-会员 1-交易所
    int32_t     QuoteTime;          //询价时间(HHMMSS)
    int32_t     TradingDay;         //交易日
    int32_t     ActionDay;          //业务发生日(自然日)
    uint16_t    MarketId;           //市场代码 KD_MI_XXX
    uint16_t    ServiceId;          //数据类型 KD_SI_XXX_Yyyy
};

#pragma pack(pop)

#endif//_KD_FUTURE_MARKET_H_
