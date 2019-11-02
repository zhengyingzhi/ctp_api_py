#ifndef _HS_TRADE_PY_H_
#define _HS_TRADE_PY_H_

#include <stdio.h>
#include <string.h>

#include <string>
#include <map>

#include "hstrade.h"

// using std::string;

#ifdef HAVE_WRPPAER_PYTHON

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

    // 连接服务器
    int connect(std::string server_port, std::string license_file, std::string fund_account, int timeoutms);

    // 写入JSON数据
    int set_json_value(const std::string& json_str);

    // 发送数据
    int send_msg(int func_id, int subsystem_no = 0, int branch_no = 0);

    // 接收数据
    std::string recv_msg();

    // 开始打包
    void begin_pack(void);
    // 结束打包
    void end_pack(void);

    // 添加包字段，字段类型：I(Integer), S(String), C(Char), F(Double)
    int add_field(const std::string& key, const std::string& field_type, int field_width);

    // 添加包数据
    int pack_char(const std::string& value);
    int pack_string(const std::string& value);
    int pack_int(const int value);
    int pack_double(const double value);

private:
    // 发送已打好包的数据
    int send_pack_msg(int func_id, int subsystem_no, int branch_no);

public:
    virtual void on_front_connected() {}
    virtual void on_front_disconnected(int reason) {}
    virtual void on_recv_msg(int func_id, int issue_type, const std::string& msg) {}

private:
    std::string     m_json_str;
    void*           m_packer;
    hstrade_t*      m_hstd;
    hstrade_spi_t   m_spi;

    int             m_async_mode;
    int             m_data_proto;
};

#endif//HAVE_WRPPAER_PYTHON

#endif//_HS_TRADE_PY_H_
