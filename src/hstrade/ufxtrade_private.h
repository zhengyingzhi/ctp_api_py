#ifndef _UFX_TRADE_PRIVATE_H_
#define _UFX_TRADE_PRIVATE_H_


#include <t2sdk_interface.h>

#include "ufxtrade.h"


#define MY_DEBUG 1

#if (MY_DEBUG)
#define HSDebugInfo(fd,fmt,...) fprintf(fd,fmt,##__VA_ARGS__)
#else
static  inline void HSPrintNull(FILE* fd, const char* fmt, ...) { (void)fd; (void)fmt; }
#define HSDebugInfo(fd,fmt,...) HSPrintNull(fd,fmt,##__VA_ARGS__)
#endif//MY_DEBUG
#define HSDbgFd stderr


#define HSTRADE_DEFAULT_TIMEOUT     3000

class TradeCallback;

typedef struct ufxtrade_apidata_s
{
    char    user_token[512];
    char    stock_account[16];
    char    op_station[32];
    char    entrust_way;
    char    password_type;
    int     op_branch_no;
    int     branch_no;
    int     sysnode_id;
}ufxtrade_apidata_t;

struct ufxtrade_s
{
    int     desc;
    int     is_async;
    int     debug_mode;
    int     timeoutms;
    int     sequence_no;
    void*   userdata;

    // t2sdk api object
    CConfigInterface*       config;
    CConnectionInterface*   conn;
    TradeCallback*          callback;

    ufxtrade_spi_t*         spi;
    ufxtrade_apidata_t      apidata;

    char    server_addr[32];

    char    trading_day[9];
    char    client_name[24];
    char    client_id[24];
    char    fund_account[24];
    char    password[24];
};


#endif//_UFX_TRADE_PRIVATE_H_
