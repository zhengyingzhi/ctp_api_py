#ifndef _SLQ_READER_H_
#define _SLQ_READER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "md_utils.h"

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


#define _LARGEFILE_SOURCE
#define n2l(c,l)        (l =((unsigned long)(*((c)++)))<<24L, \
	l|=((unsigned long)(*((c)++)))<<16L, \
	l|=((unsigned long)(*((c)++)))<< 8L, \
	l|=((unsigned long)(*((c)++))))

#define l2n(l,c)        (*((c)++)=(unsigned char)(((l)>>24L)&0xff), \
	*((c)++)=(unsigned char)(((l)>>16L)&0xff), \
	*((c)++)=(unsigned char)(((l)>> 8L)&0xff), \
	*((c)++)=(unsigned char)(((l)     )&0xff))

    //两个行情包之间的间隔
#define PACK_INTERVAL 56
    //包头
#define PACK_HEAD 541 //30 541为跳过了第一个行情包的字节数
    //交易日
#define TRADINGDAY_OFFSET (0) //(PACK_HEAD)
    //合约代码
#define INSTRUMENTID_OFFSET (TRADINGDAY_OFFSET + 9)
    //交易所代码
#define EXGID_OFFSET (INSTRUMENTID_OFFSET + 31)
    //合约在交易所的代码
#define EXGINSTID_OFFSET (EXGID_OFFSET + 9)
    //最新价
#define LASTPRICE_OFFSET (EXGINSTID_OFFSET + 31)
    //上次结算价
#define PRESETPRICE_OFFSET (LASTPRICE_OFFSET + sizeof(double))
    //昨收盘
#define PRECLOSEPRICE_OFFSET (PRESETPRICE_OFFSET + sizeof(double))
    //昨持仓量
#define PREOPENINTEREST_OFFSET (PRECLOSEPRICE_OFFSET + sizeof(double))
    //今开盘
#define OPENPRICE_OFFSET (PREOPENINTEREST_OFFSET + sizeof(double))
    //最高价
#define HIGHESTPRICE_OFFSET (OPENPRICE_OFFSET + sizeof(double))
    //最低价
#define LOWESTPRICE_OFFSET (HIGHESTPRICE_OFFSET + sizeof(double))
    //数量
#define VOLUME_OFFSET (LOWESTPRICE_OFFSET + sizeof(double))
    //成交金额
#define TURNOVER_OFFSET (VOLUME_OFFSET + 8)
    //持仓量
#define OPENINTEREST_OFFSET (TURNOVER_OFFSET + sizeof(double))
    //今收盘
#define CLOSEPRICE_OFFSET (OPENINTEREST_OFFSET + sizeof(double))
    //本次结算价
#define SETPRICE_OFFSET (CLOSEPRICE_OFFSET + sizeof(double))
    //涨停板价
#define UPPERLIMITPRICE_OFFSET (SETPRICE_OFFSET + sizeof(double))
    //跌停板价
#define LOWERLIMITPRICE_OFFSET (UPPERLIMITPRICE_OFFSET + sizeof(double))
    //昨虚实度
#define PREDELTA_OFFSET (LOWERLIMITPRICE_OFFSET + sizeof(double))
    //今虚实度
#define CURRDELTA_OFFSET (PREDELTA_OFFSET + sizeof(double))
    //最后修改时间
#define UPDATETIME_OFFSET (CURRDELTA_OFFSET + sizeof(double))
    //最后修改毫秒 长度为12
#define UPDATEMSEC_OFFSET (UPDATETIME_OFFSET + 12)
    //买一价 unsigned int
#define BID1PRICE_OFFSET (UPDATEMSEC_OFFSET + 4)
    //买一量
#define BID1VOLUME_OFFSET (BID1PRICE_OFFSET + sizeof(double))
    //卖一价
#define ASK1PRICE_OFFSET (BID1VOLUME_OFFSET + 8)
    //卖一量
#define ASK1VOLUME_OFFSET (ASK1PRICE_OFFSET + sizeof(double))



struct slq_reader_s {
    char slq_name[512];
    char* buffer;
    char* pcur;
    int64_t length;
    int64_t offset;
    uint32_t count;
    char trading_day[9];
};
typedef struct slq_reader_s slq_reader_t;


// 获取文件长度
MD_UTILS_API int64_t MD_UTILS_STDCALL file_length(FILE* fp);

// 获取文件内容
MD_UTILS_API char* MD_UTILS_STDCALL slq_read_content(const char* slq_name, int64_t* plen);


// 获取每一条数据
MD_UTILS_API int MD_UTILS_STDCALL slq_reader_init(slq_reader_t* sr, const char* slq_name, const char* trading_day);

MD_UTILS_API int MD_UTILS_STDCALL slq_reader_release(slq_reader_t* sr);

MD_UTILS_API int MD_UTILS_STDCALL slq_reader_next(slq_reader_t* sr, void* dst);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//_SLQ_READER_H_
