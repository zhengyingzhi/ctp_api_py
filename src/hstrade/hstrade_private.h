#ifndef _HS_TRADE_PRIVATE_H_
#define _HS_TRADE_PRIVATE_H_


#include <t2sdk_interface.h>

#include "hstrade.h"


class TradeCallback;

typedef struct hstrade_apidata_s
{
    char    user_token[512];
    char    stock_account[16];
    char    op_station[32];
    char    entrust_way;
    char    password_type;
    int     op_branch_no;
    int     branch_no;
    int     system_no;
}hstrade_apidata_t;

struct hstrade_s
{
    int     desc;
    int     is_async;
    int     topic;
    int     running;
    void*   userdata;

    // t2sdk api object
    CConfigInterface*       config;
    CConnectionInterface*   conn;
    TradeCallback*          callback;

    hstrade_spi_t*          spi;
    hstrade_apidata_t       apidata;

    char    trading_date[9];
    char    client_name[24];
    char    client_id[24];
    char    password[24];
};


#endif//_HS_TRADE_PRIVATE_H_
