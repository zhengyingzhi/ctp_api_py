#ifndef _HS_TRADE_PY_H_
#define _HS_TRADE_PY_H_

#include <stdio.h>
#include <string.h>

#include <string>
#include <map>

#include "hstrade.h"

// using std::string;


#ifdef HAVE_WRPPAER_PYTHON

#include <t2sdk_interface.h>

#include <pybind11/pybind11.h>
using namespace pybind11;


/// 异步模式
#define HS_SYNC_MODE        0
#define HS_ASYNC_MODE       1

/// 数据模式
#define HS_DATA_PROTO_RAW   0
#define HS_DATA_PROTO_JSON  1


/* wrapper api for python */

class HSTdApi
{
public:
    HSTdApi();
    ~HSTdApi();

public:
    // 获取API版本号
    int get_api_version();

    std::string get_hstrade_version();

    // 创建API async_mode：1-asyn, data_proto: 1-json
    void create_api(int async_mode = 0, int data_proto = 0);

    // 销毁API
    void release();

    // 调试模式
    void set_debug_mode(int level);

    // 设置同步模式超时时间
    void set_timeout(int timeoutms);

    // 加载配置文件
    int config_load(std::string config_file);

    // 设置配置选项
    int config_set_string(std::string section, std::string entry_name, std::string value);
    int config_set_int(std::string section, std::string entry_name, int value);

    // 获取配置选项
    std::string config_get_string(std::string section, std::string entry_name, std::string default_value);
    int config_get_int(std::string section, std::string entry_name, int default_value);

    // 连接服务器
    int connect(int timeoutms);

    // 设置序列号
    int set_sequece_no(int sequence_no);
    int set_issue_type(int issue_type);
    int set_company_id(int company_id);
    int set_sender_company_id(int sender_company_id);
    int set_system_no(int system_no);
    int new_biz_message();

    // 写入JSON数据
    int set_json_value(const std::string& json_str);

    // 发送数据
    int send_msg(int func_id, int subsystem_no = 0, int branch_no = 0);

    // 接收数据
    std::string recv_msg(int hsend);

    // 开始打包
    void begin_pack(void);
    // 结束打包
    void end_pack(void);

    // 添加包字段，字段类型：I(Integer), S(String), C(Char), D(Double)
    int add_field(const std::string& key, const std::string& field_type, int field_width, int field_scale=4);

    // 添加包数据
    int add_char(const std::string& value);
    int add_str(const std::string& value);
    int add_int(const int value);
    int add_double(const double value);

    // 一些常用接口
    int req_login(const dict& req);
    int req_qry_account_info(const dict& req);
    int req_qry_cash(const dict& req);
    int req_qry_position(const dict& req);
    int req_qry_order(const dict& req);
    int req_qry_trade(const dict& req);
    int req_qry_md(const std::string& exchange_type, const std::string& stock_code);
    int req_order_insert(const dict& order_req);
    int req_order_action(const dict& order_action);

private:
    // 发送已打好包的数据
    int send_pack_msg(int func_id, int subsystem_no, int branch_no);

public:
    virtual void on_front_connected() {}
    virtual void on_front_disconnected(int reason) {}
    virtual void on_msg_error(int func_id, int issue_type, int ref_id, int error_no, const std::string& error_info) {}
    virtual void on_recv_msg(int func_id, int issue_type, int ref_id, const std::string& msg) {}

private:
    std::string     m_json_str;
    IF2Packer*      m_packer;
    hstrade_t*      m_hstd;
    hstrade_spi_t   m_spi;

    IBizMessage*    m_pBizMessage;

    int             m_hsend;
    int             m_last_error_code;
    int             m_async_mode;
    int             m_data_proto;
    int             m_sequece_no;
    int             m_company_id;
    int             m_sender_company_id;
    int             m_system_no;
};

#endif//HAVE_WRPPAER_PYTHON

#endif//_HS_TRADE_PY_H_
