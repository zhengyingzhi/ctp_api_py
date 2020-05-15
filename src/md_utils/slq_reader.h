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

    //���������֮��ļ��
#define PACK_INTERVAL 56
    //��ͷ
#define PACK_HEAD 541 //30 541Ϊ�����˵�һ����������ֽ���
    //������
#define TRADINGDAY_OFFSET (0) //(PACK_HEAD)
    //��Լ����
#define INSTRUMENTID_OFFSET (TRADINGDAY_OFFSET + 9)
    //����������
#define EXGID_OFFSET (INSTRUMENTID_OFFSET + 31)
    //��Լ�ڽ������Ĵ���
#define EXGINSTID_OFFSET (EXGID_OFFSET + 9)
    //���¼�
#define LASTPRICE_OFFSET (EXGINSTID_OFFSET + 31)
    //�ϴν����
#define PRESETPRICE_OFFSET (LASTPRICE_OFFSET + sizeof(double))
    //������
#define PRECLOSEPRICE_OFFSET (PRESETPRICE_OFFSET + sizeof(double))
    //��ֲ���
#define PREOPENINTEREST_OFFSET (PRECLOSEPRICE_OFFSET + sizeof(double))
    //����
#define OPENPRICE_OFFSET (PREOPENINTEREST_OFFSET + sizeof(double))
    //��߼�
#define HIGHESTPRICE_OFFSET (OPENPRICE_OFFSET + sizeof(double))
    //��ͼ�
#define LOWESTPRICE_OFFSET (HIGHESTPRICE_OFFSET + sizeof(double))
    //����
#define VOLUME_OFFSET (LOWESTPRICE_OFFSET + sizeof(double))
    //�ɽ����
#define TURNOVER_OFFSET (VOLUME_OFFSET + 8)
    //�ֲ���
#define OPENINTEREST_OFFSET (TURNOVER_OFFSET + sizeof(double))
    //������
#define CLOSEPRICE_OFFSET (OPENINTEREST_OFFSET + sizeof(double))
    //���ν����
#define SETPRICE_OFFSET (CLOSEPRICE_OFFSET + sizeof(double))
    //��ͣ���
#define UPPERLIMITPRICE_OFFSET (SETPRICE_OFFSET + sizeof(double))
    //��ͣ���
#define LOWERLIMITPRICE_OFFSET (UPPERLIMITPRICE_OFFSET + sizeof(double))
    //����ʵ��
#define PREDELTA_OFFSET (LOWERLIMITPRICE_OFFSET + sizeof(double))
    //����ʵ��
#define CURRDELTA_OFFSET (PREDELTA_OFFSET + sizeof(double))
    //����޸�ʱ��
#define UPDATETIME_OFFSET (CURRDELTA_OFFSET + sizeof(double))
    //����޸ĺ��� ����Ϊ12
#define UPDATEMSEC_OFFSET (UPDATETIME_OFFSET + 12)
    //��һ�� unsigned int
#define BID1PRICE_OFFSET (UPDATEMSEC_OFFSET + 4)
    //��һ��
#define BID1VOLUME_OFFSET (BID1PRICE_OFFSET + sizeof(double))
    //��һ��
#define ASK1PRICE_OFFSET (BID1VOLUME_OFFSET + 8)
    //��һ��
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


// ��ȡ�ļ�����
MD_UTILS_API int64_t MD_UTILS_STDCALL file_length(FILE* fp);

// ��ȡ�ļ�����
MD_UTILS_API char* MD_UTILS_STDCALL slq_read_content(const char* slq_name, int64_t* plen);


// ��ȡÿһ������
MD_UTILS_API int MD_UTILS_STDCALL slq_reader_init(slq_reader_t* sr, const char* slq_name, const char* trading_day);

MD_UTILS_API int MD_UTILS_STDCALL slq_reader_release(slq_reader_t* sr);

MD_UTILS_API int MD_UTILS_STDCALL slq_reader_next(slq_reader_t* sr, void* dst);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif//_SLQ_READER_H_
