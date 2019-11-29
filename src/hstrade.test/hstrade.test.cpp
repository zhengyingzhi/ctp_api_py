#include <stdio.h>

#include <iostream>
using namespace std;

#include "hstrade/hstrade.h"


void on_connected(hstrade_t*);
void on_disconnected(hstrade_t*, int reason);
void on_rsp_subscribe(hstrade_t*, HSRspSubscribeField* rsp_sub, int ref_id);
void on_user_login(hstrade_t*, HSRspUserLoginField* rsp_login, HSRspInfoField* rsp_info, int ref_id);
void on_order_insert(hstrade_t*, HSOrderField*, HSRspInfoField* rsp_info, int ref_id, int islast);
void on_order_action(hstrade_t*, HSOrderField*, HSRspInfoField* rsp_info, int ref_id, int islast);

void on_qry_trading_account(hstrade_t*, HSTradingAccountField*, HSRspInfoField* rsp_info, int ref_id, int islast);
void on_qry_position(hstrade_t*, HSPositionField*, HSRspInfoField* rsp_info, int ref_id, int islast);
void on_qry_order(hstrade_t*, HSOrderField*, HSRspInfoField* rsp_info, int ref_id, int islast);
void on_qry_trade(hstrade_t*, HSTradeField*, HSRspInfoField* rsp_info, int ref_id, int islast);
void on_qry_security_info(hstrade_t*, HSSecurityInfoField*, HSRspInfoField* rsp_info, int ref_id, int islast);
void on_qry_md(hstrade_t*, HSMarketDataField*, HSRspInfoField* rsp_info, int ref_id, int islast);

void on_qry_cash_hist(hstrade_t*, HSTradingAccountField*, HSRspInfoField* rsp_info, int ref_id, int islast);
void on_qry_order_hist(hstrade_t*, HSOrderField*, HSRspInfoField* rsp_info, int ref_id, int islast);
void on_qry_trade_hist(hstrade_t*, HSTradeField*, HSRspInfoField* rsp_info, int ref_id, int islast);

void on_rtn_order(hstrade_t*, HSOrderField*, int ref_id);
void on_rtn_trade(hstrade_t*, HSTradeField*, int ref_id);


int showmenu1()
{
    cerr << "------------------------------------------------" << endl;
    cerr << " [1]  331100 请求登录                           " << endl;
    cerr << " [2]  400 证券行情查询                          " << endl;
    cerr << " [3]  333002 证券普通委托                       " << endl;
    cerr << " [4]  333103 证券持仓查询                       " << endl;
    cerr << " [5]  331300 证券股东信息查询                   " << endl;
    cerr << " [6]  333101 证券委托查询                       " << endl;
    cerr << " [7]  333102 证券成交查询                       " << endl;
    cerr << " [8]  333102 资金账户查询                       " << endl;
    cerr << " [0]  EXIT                                      " << endl;
    cerr << "------------------------------------------------" << endl;
    return 0;

}

int main(int argc, char* argv[])
{
    fprintf(stderr, "hello hstrade v%s test!\n", hstrade_version(NULL));

    hstrade_spi_t spi;
    spi.on_connected = on_connected;
    spi.on_disconnected = on_disconnected;
    spi.on_user_login = on_user_login;
    spi.on_order_insert = on_order_insert;
    spi.on_order_action = on_order_action;
    spi.on_qry_trading_account = on_qry_trading_account;
    spi.on_qry_position = on_qry_position;
    spi.on_qry_order = on_qry_order;
    spi.on_qry_trade = on_qry_trade;
    spi.on_qry_security_info = on_qry_security_info;
    spi.on_qry_md = on_qry_md;
    spi.on_qry_cash_hist = on_qry_cash_hist;
    spi.on_qry_order_hist = on_qry_order_hist;
    spi.on_qry_trade_hist = on_qry_trade_hist;
    spi.on_rtn_order = on_rtn_order;
    spi.on_rtn_trade = on_rtn_trade;

    int is_async = 1;

    int rv;
    hstrade_t* hstd;
    hstd = hstrade_create(is_async);

    // set debug
    hstrade_debug_mode(hstd, 1);

    // set callbacks
    hstrade_register_spi(hstd, &spi);

    // load config and set some items
    hstrade_config_load(hstd, "t2sdk.ini");

    const char* server_addr = "120.55.176.113:9359";
    const char* license_file = "(20130524)HSSBCS-HSTZYJ20-0000_3rd.dat";
    const char* fund_account = "70960013";
    hstrade_config_set_string(hstd, "t2sdk", "servers", server_addr);
    hstrade_config_set_string(hstd, "t2sdk", "license_file", license_file);
    hstrade_config_set_string(hstd, "t2sdk", "license_pwd", "888888");
    hstrade_config_set_string(hstd, "ufx", "fund_account", fund_account);

    // do connect
    rv = hstrade_init(hstd, 3000);
    fprintf(stderr, "hstrade_init rv:%d\n", rv);

    while (1)
    {
        showmenu1();
        char buf[245] = "";
        scanf("%s", buf);
        if (strcmp(buf, "e") == 0 || strcmp(buf, "q") == 0)
        {
            break;
        }
        else if (strcmp(buf, "sub") == 0)
        {
            hstrade_subscribe(hstd, UFX_ISSUE_TYPE_TRADE);
            // hstrade_subscribe(hstd, UFX_ISSUE_TYPE_ORDER);
        }
        else if (strcmp(buf, "1") == 0)
        {
            HSReqUserLoginField login_field = { 0 };
            strcpy(login_field.client_id, fund_account);
            strcpy(login_field.password, "111111");
            rv = hstrade_user_login(hstd, &login_field);
            fprintf(stderr, "hstrade_user_login rv:%d\n", rv);
        }
        else if (strcmp(buf, "3") == 0)
        {
            HSReqOrderInsertField order_field = { 0 };
            strcpy(order_field.client_id, fund_account);
            strcpy(order_field.exchange_type, "1");
            strcpy(order_field.stock_code, "600030");
            strcpy(order_field.entrust_prop, "0");
            order_field.entrust_bs = HS_D_Buy;
            order_field.entrust_amount = 200;
            order_field.entrust_price = 22.40;
            rv = hstrade_order_insert(hstd, &order_field);
            fprintf(stderr, "hstrade_order_insert rv:%d\n", rv);
        }
        else if (strcmp(buf, "4") == 0)
        {
            HSReqQueryField qry_field = { 0 };
            strcpy(qry_field.client_id, fund_account);
            rv = hstrade_qry_position(hstd, &qry_field);
            fprintf(stderr, "hstrade_qry_position rv:%d\n", rv);
        }
        else if (strcmp(buf, "6") == 0)
        {
            HSReqQueryField qry_field = { 0 };
            strcpy(qry_field.client_id, fund_account);
            rv = hstrade_qry_order(hstd, &qry_field);
            fprintf(stderr, "hstrade_qry_order rv:%d\n", rv);
        }
        else if (strcmp(buf, "7") == 0)
        {
            HSReqQueryField qry_field = { 0 };
            strcpy(qry_field.client_id, fund_account);
            rv = hstrade_qry_trade(hstd, &qry_field);
            fprintf(stderr, "hstrade_qry_trade rv:%d\n", rv);
        }
        else if (strcmp(buf, "8") == 0)
        {
            HSReqQueryField qry_field = { 0 };
            strcpy(qry_field.client_id, fund_account);
            rv = hstrade_qry_trading_account(hstd, &qry_field);
            fprintf(stderr, "hstrade_qry_trading_account rv:%d\n", rv);
        }

        getchar();
    }

    hstrade_realese(hstd);

    fprintf(stderr, "hstrade test end!\n");
    return 0;
}


void on_connected(hstrade_t* hstd)
{
    fprintf(stderr, "<on_connected>\n");
}

void on_disconnected(hstrade_t* hstd, int reason)
{
    fprintf(stderr, "<on_disconnected>\n");
}

void on_rsp_subscribe(hstrade_t* hstd, HSRspSubscribeField* rsp_sub, int ref_id)
{
    fprintf(stderr, "<on_rsp_subscribe %d>\n", ref_id);
}

void on_user_login(hstrade_t* hstd, HSRspUserLoginField* rsp_login, HSRspInfoField* rsp_info, int ref_id)
{
    fprintf(stderr, "<on_user_login %d>\n", ref_id);
    fprintf(stderr, "accountid:%s, branch_no:%d, trading_day:%d\n",
        rsp_login->client_id, rsp_login->branch_no, rsp_login->trading_date);
}

void on_order_insert(hstrade_t* hstd, HSOrderField* order, HSRspInfoField* rsp_info, int ref_id, int islast)
{
    fprintf(stderr, "<on_order_insert %d>\n", ref_id);
}

void on_order_action(hstrade_t* hstd, HSOrderField* order, HSRspInfoField* rsp_info, int ref_id, int islast)
{
    fprintf(stderr, "<on_order_action %d>\n", ref_id);
}


void on_qry_trading_account(hstrade_t* hstd, HSTradingAccountField* ta, HSRspInfoField* rsp_info, int ref_id, int islast)
{
    fprintf(stderr, "<on_qry_trading_account %d>\n", ref_id);
    if (!ta)
    {
        return;
    }

    fprintf(stderr, "asset:%.2lf, balance:%.2lf, avail:%.2lf, frozen:%.2lf, market_value:%.2lf\n",
        ta->asset_balance, ta->current_balance, ta->enable_balance, ta->frozen_balance, ta->market_value);
}

void on_qry_position(hstrade_t* hstd, HSPositionField* pos, HSRspInfoField* rsp_info, int ref_id, int islast)
{
    fprintf(stderr, "<on_qry_position %d>\n", ref_id);
    if (!pos)
    {
        return;
    }

    fprintf(stderr, "symbol:%s,cur_qty:%d,avail_qty:%d,frozen_qty:%d,cost_price:%.3lf,market_value:%.2lf,profit:%.2lf\n",
        pos->stock_code, pos->current_amount, pos->enable_amount, pos->frozen_amount, pos->cost_price, pos->market_value, pos->income_balance);
}

void on_qry_order(hstrade_t* hstd, HSOrderField* order, HSRspInfoField* rsp_info, int ref_id, int islast)
{
    fprintf(stderr, "<on_qry_order %d>\n", ref_id);
    if (!order)
    {
        return;
    }

    fprintf(stderr, "symbol:%s,qty:%d,price:%.2lf,orderid:%s,status:%c\n",
        order->stock_code, order->entrust_amount, order->entrust_price, order->entrust_no, order->entrust_status);
}

void on_qry_trade(hstrade_t* hstd, HSTradeField* trade, HSRspInfoField* rsp_info, int ref_id, int islast)
{
    fprintf(stderr, "<on_qry_trade %d>\n", ref_id);
    if (!trade)
    {
        return;
    }

    fprintf(stderr, "symbol:%s,qty:%d,price:%.2lf,orderid:%s,tradeid:%s,tradetime:%d\n",
        trade->stock_code, trade->business_amount, trade->business_price, trade->entrust_no, trade->business_no, trade->business_time);
}

void on_qry_security_info(hstrade_t* hstd, HSSecurityInfoField* secinfo, HSRspInfoField* rsp_info, int ref_id, int islast)
{}

void on_qry_md(hstrade_t* hstd, HSMarketDataField* md, HSRspInfoField* rsp_info, int ref_id, int islast)
{}


void on_qry_cash_hist(hstrade_t* hstd, HSTradingAccountField*, HSRspInfoField* rsp_info, int ref_id, int islast)
{}

void on_qry_order_hist(hstrade_t* hstd, HSOrderField* order, HSRspInfoField* rsp_info, int ref_id, int islast)
{}

void on_qry_trade_hist(hstrade_t* hstd, HSTradeField* order, HSRspInfoField* rsp_info, int ref_id, int islast)
{}

void on_rtn_order(hstrade_t* hstd, HSOrderField* order, int ref_id)
{
    fprintf(stderr, "<on_rtn_order>\n");
    fprintf(stderr, "symbol:%s,qty:%d,price:%.2lf,orderid:%s,status:%c\n",
        order->stock_code, order->entrust_amount, order->entrust_price, order->entrust_no, order->entrust_status);
}

void on_rtn_trade(hstrade_t* hstd, HSTradeField* trade, int ref_id)
{
    fprintf(stderr, "<on_rtn_trade>\n");
    fprintf(stderr, "symbol:%s,qty:%d,price:%.2lf,orderid:%s,tradeid:%s,tradetime:%d\n",
        trade->stock_code, trade->business_amount, trade->business_price, trade->entrust_no, trade->business_no, trade->business_time);
}

