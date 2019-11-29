# encoding: UTF-8

import json
import sys
from datetime import datetime
from time import sleep

from hstrade import HSTdApi
# import hstrade as hstd
# print(help(hstd))


# 以下为一些BT类型和SEC类型的映射字典
HS_FUNC_QRY_SECINFO     = 330300        # 证券代码信息查询
HS_FUNC_QRY_CASH_FAST   = 332254        # 客户资金快速查询
HS_FUNC_QRY_CASH        = 332255        # 客户资金精准查询
HS_FUNC_PLACE_ORDER     = 333002        # 普通委托
HS_FUNC_CANCEL_ORDER    = 333017        # 委托撤单
HS_FUNC_QRY_PURCHASE    = 333030        # 新股申购查询
HS_FUNC_QRY_ORDER       = 333101        # 证券委托查询
HS_FUNC_QRY_TRADE       = 333102        # 证券成交查询
HS_FUNC_QRY_POS_FAST    = 333103        # 证券持仓快速查询
HS_FUNC_QRY_POSITION    = 333104        # 证券持仓查询
HS_FUNC_QRY_MD          = 400           # 证券行情查询
HS_FUNC_LOGIN           = 331100        # 客户登录
HS_FUNC_QRY_ACCOUNTINFO = 331155        # 账户信息获取
HS_FUNC_SUB_ORDER       = 620001        # 订阅回报主推
HS_FUNC_UNSUB_ORDER     = 620002        # 取消订阅回报主推
HS_FUNC_RTN_ORDER       = 620003        # 回报主推

HS_OP_STATION           = ""
HS_OP_BRANCH_NO         = "0"
HS_OP_ENTRUST_WAY       = "7"           # 网上委托
HS_PASSWORD_TYPE        = "2"           # 1-资金密码 2-交易密码

HS_EXCHANGE_TYPE_CASH   = "0"           # 0-资金 1-上海 2-深圳
HS_EXCHANGE_TYPE_SSE    = "1"
HS_EXCHANGE_TYPE_SZSE   = "2"

HS_ENTRUST_TYPE_BUY     = "1"           # 1-买入 2-卖出
HS_ENTRUST_TYPE_SELL    = "2"
HS_ENTRUST_PROP         = "0"           # 0-买卖


#----------------------------------------------------------------------
def print_dict(d):
    """按照键值打印一个字典"""
    for key,value in d.items():
        print(key + ':' + str(value))

#----------------------------------------------------------------------
def simple_log(func):
    """简单装饰器用于输出函数名"""
    def wrapper(*args, **kw):
        print("")
        print(str(func.__name__), "at", datetime.now())
        return func(*args, **kw)
    return wrapper

func_id_desc_dict = {
    HS_FUNC_QRY_SECINFO: "qry_secinfo",
    HS_FUNC_QRY_CASH_FAST: "qry_cash_fast",
    HS_FUNC_QRY_CASH: "qry_cash",
    HS_FUNC_PLACE_ORDER: "place_order",
    HS_FUNC_CANCEL_ORDER: "cancel_order",
    HS_FUNC_QRY_PURCHASE: "qry_purchase",
    HS_FUNC_QRY_ORDER: "qry_order",
    HS_FUNC_QRY_TRADE: "qry_trade",
    HS_FUNC_QRY_POSITION: "qry_position",
    HS_FUNC_QRY_MD: "qrt_md",
    HS_FUNC_LOGIN: "client_login",
    HS_FUNC_QRY_ACCOUNTINFO: "qry_account_info",
    HS_FUNC_SUB_ORDER: "subscribe_order",
    HS_FUNC_RTN_ORDER: "rtn_msg"
}

def func_id_desc(func_id):
    return func_id_desc_dict.get(func_id, "unknown")


########################################################################
class MyHSTdApi(HSTdApi):
    """测试用实例"""

    #----------------------------------------------------------------------
    def __init__(self):
        """API对象的初始化函数"""
        super(MyHSTdApi, self).__init__()

        self.tradingDay = datetime.now().strftime("%Y%m%d")
        self.reqID = 0                      # 操作请求编号
        self.localOrderID = int(datetime.now().strftime("%d%H%M%S")) * 1000

        self.connectionStatus = False       # 连接状态
        self.stockLoginStatus = False       # 股票登录状态

        self.setting = None
        self.serverAddr = ''
        self.serverPort = ''
        self.accountName = ''
        self.accountID = ''                 # 账号
        self.password = ''                  # 账号密码
        self.fundAccount = ''               # 资金账号
        self.licenseFile = ''

        # from api response values
        self.userToken = ''
        self.branchNo = '0'
        self.sysnodeID = '0'

    def add_common_fields(self, data):
        """添加通知请求字段"""
        data["user_token"] = self.userToken
        # data["user_token"] = ""
        data["op_branch_no"] = HS_OP_BRANCH_NO
        data["op_entrust_way"] = HS_OP_ENTRUST_WAY
        data["op_station"] = HS_OP_STATION
        data["branch_no"] = int(self.branchNo)
        data["client_id"] = self.accountID
        data["fund_account"] = self.accountID
        data["password"] = self.password
        data["password_type"] = HS_PASSWORD_TYPE

    def get_order_key(self, orderID, frontID, sessionID):
        """返回订单Key"""
        return (orderID, frontID, sessionID)

    def write_log(self, data, desc):
        """写入消息到xctd api日志文件中"""
        if isinstance(data, dict):
            if "password" in data:
                temp_val = data["password"]
                del data["password"]
                line = json.dumps(data)
                data["password"] = temp_val
            else:
                line = json.dumps(data)
        else:
            line = data
        print("td %s data:\n%s" % (desc, line))
        # self.write_line(0, "td %s data:\n%s" % (desc, line))

    #----------------------------------------------------------------------
    def init(self, setting):
        """初始化连接"""
        self.serverAddr = str(setting["tdAddress"])
        self.serverPort = str(setting["tdPort"])
        self.accountID = setting["accountID"]
        self.password = setting["password"]
        self.accountName = setting.get("accountName", self.accountID)
        self.licenseFile = setting["license"]
        self.fundAccount = self.accountID
        serverIPPort = ":".join([self.serverAddr, self.serverPort])

        print("init serverIPPort:%s, license:%s, fundAccount:%s" \
              % (serverIPPort, self.licenseFile, self.fundAccount))

        if not self.connectionStatus:
            # create td api within async mode
            self.create_api(1, 1)

            self.set_debug_mode(setting.get("debug_mode", 1))

            # 设置API配置
            self.config_load("t2sdk.ini")
            self.config_set_string("t2sdk", "servers", serverIPPort)
            self.config_set_string("t2sdk", "license_file", self.licenseFile)
            self.config_set_string("ufx", "fund_account", self.accountID)

            # do api connect
            # 0: 成功
            # -1: 未注册
            # -2: 链接失败
            # -3: 初始化链接对象失败
            # -4: 异步应答数据反馈线程启动失败
            # -5: 登录检查失败，可调用 get_last_error_msg() 获取错误信息
            # -6: 系统号错误（非支持柜台系统）
            rv = self.connect(3000)
            if rv < 0:
                print("td init error rv:%s" % rv)
            return rv

    def request_data(self, funcID, data, desc=''):
        """发送数据"""
        # >=0: 发送成功，异步时返回ref_id，用于和应答匹配
        # -1: 未注册
        # -2: 链接失败
        # -3: 数据通讯错误（同步模式不存在）
        # -4: 内存不足，用于接收数据的内存不足
        # -5: 数据接收错误（同步模式不存在）
        self.write_log(data, "request " + desc)

        if isinstance(data, dict):
            data = json.dumps(data)

        self.sysnodeID = 0
        self.branchNo = 0

        self.set_json_value(data)
        rv = self.send_msg(funcID, int(self.sysnodeID), int(self.branchNo))
        if rv < 0:
            print("[error] td request_data by %s failed:%s" % (desc, rv))
        return rv

    def subscribe(self):
        """订阅回报"""
        issueTypeTrade = 12
        issueTypeOrder = 23
        sub_data = {
            "function_id": str(HS_FUNC_SUB_ORDER),
            "branch_no": self.branchNo,
            "fund_account": self.accountID,
            "op_station": HS_OP_STATION,
            "password": self.password,
            "sysnode_id": self.sysnodeID
        }

        sub_data["issue_type"] = issueTypeTrade
        self.request_data(HS_FUNC_SUB_ORDER, sub_data, desc="subscribe(trade)")

        sub_data["issue_type"] = issueTypeOrder
        rv = self.request_data(HS_FUNC_SUB_ORDER, sub_data, desc="subscribe(order)")
        return rv

    #----------------------------------------------------------------------
    def login(self):
        """连接服务器"""
        # print("do login within raw api!\n")
        self.begin_pack()
        self.add_field("op_branch_no", 'I', 5)
        self.add_field("op_entrust_way", 'C', 1)
        self.add_field("op_station", 'S', 255)
        self.add_field("branch_no", 'I', 5)
        self.add_field("input_content", 'C', 1)
        self.add_field("account_content", 'S', 30)
        self.add_field("content_type", 'S', 6)
        self.add_field("password", 'S', 10)
        self.add_field("password_type", 'C', 1)

        self.add_int(0)
        self.add_char('7')
        self.add_str("0")
        self.add_int(0)
        self.add_char('1')
        self.add_str(self.accountID);
        self.add_str("0")
        self.add_str(self.password);
        self.add_char(HS_PASSWORD_TYPE);
        self.end_pack()
        rv = self.send_msg(HS_FUNC_LOGIN, 0, 0)
        return rv

        # request client login
        login_data = {
            "op_branch_no":  HS_OP_BRANCH_NO,
            "op_entrust_way": HS_OP_ENTRUST_WAY,
            "op_station": HS_OP_STATION,
            "password": self.password,
            "password_type": HS_PASSWORD_TYPE,
            "input_content": "1",
            "account_content": self.accountID,
            "content_type": "0",
            "asset_prop": "0"
        }
        rv = self.request_data(HS_FUNC_LOGIN, json.dumps(login_data), desc="login")
        return rv

    #----------------------------------------------------------------------
    def qry_account(self):
        """查询账户"""
        data = {}
        self.add_common_fields(data)
        # data["money_type"] = "0"
        rv = self.request_data(HS_FUNC_QRY_CASH, data, desc="qry_account_cash")
        return rv

    #----------------------------------------------------------------------
    def qry_position(self):
        """查询持仓"""
        data = {}
        self.add_common_fields(data)
        rv = self.request_data(HS_FUNC_QRY_POSITION, data, desc="qry_position")
        return rv

    #----------------------------------------------------------------------
    def qry_trade(self):
        """查询成交"""
        data = {}
        self.add_common_fields(data)
        rv = self.request_data(HS_FUNC_QRY_TRADE, data, desc="qry_trade")
        return rv

    #----------------------------------------------------------------------
    def qry_order(self):
        """查询委托"""
        data = {}
        self.add_common_fields(data)
        rv = self.request_data(HS_FUNC_QRY_ORDER, data, desc="qry_order")
        return rv

    #----------------------------------------------------------------------
    def send_order(self, orderReq):
        """发单 no used in test"""
        data = {}
        self.add_common_fields(data)
        data["client_id"] = self.accountID;
        data["fund_account"] = self.fundAccount
        # data["stock_account"]  # it's empty from client login response

        data["exchange_type"] = exchange_inner_2_xc(orderReq.exchange)
        data["stock_code"] = orderReq.symbol[:6]
        data["entrust_amount"] = orderReq.volume
        data["entrust_price"] = round(orderReq.price + 1e-8, 2)
        data["entrust_bs"] = direction_inner_2_xc(orderReq.direction)

        data["entrust_prop"] = HS_ENTRUST_PROP;
        # data["batch_no"] = 1
        rv = self.request_data(HS_FUNC_PLACE_ORDER, data, desc="send_order")
        return rv

    def send_order2(self, symbol, direction, order_qty, order_price):
        """下单
        symbol: 000001.SZSE
        direction: buy/long/1 or sell/short/2
        """
        data = {}
        self.add_common_fields(data)
        data["client_id"] = self.accountID;
        data["fund_account"] = self.fundAccount
        # data["stock_account"]  # it's empty from client login response

        if '.' in symbol:
            symbol, exchange = symbol.split('.')
            exchange = exchange.upper()
        elif symbol.startswith('00') or symbol.startswith("30"):
            exchange = 'SZSE'
        else:
            exchange = 'SSE'
        exchange_type = '1' if exchange == 'SSE' else '2'

        data["exchange_type"] = exchange_type
        data["stock_code"] = symbol
        data["entrust_amount"] = order_qty
        data["entrust_price"] = order_price

        if direction.upper() in ['BUY', 'LONG', '1']:
            data["entrust_bs"] = '1'
        elif direction.upper() in ['SELL', 'SHORT', '2']:
            data["entrust_bs"] = '2'
        else:
            print("invalid direction:%s" % direction)
            return None

        data["entrust_prop"] = HS_ENTRUST_PROP;
        # data["batch_no"] = 1
        rv = self.request_data(HS_FUNC_PLACE_ORDER, data, desc="send_order2")
        return rv

    #----------------------------------------------------------------------
    def cancel_order(self, cancelReq):
        """撤单"""
        data = {}
        self.add_common_fields(data)
        data["client_id"] = self.accountID
        data["batch_flag"] = '0'  # 单笔撤单
        # data["exchange_type"] = ''

        # FIXME
        if cancelReq.orderSysID:
            data["entrust_no"] = cancelReq.orderSysID
        else:
            data["entrust_no"] = cancelReq.orderID

        rv = self.request_data(HS_FUNC_CANCEL_ORDER, data, desc="cancel_order")
        return rv

    def cancel_order2(self, entrust_no):
        """撤单请求"""
        data = {}
        self.add_common_fields(data)
        data["client_id"] = self.accountID
        data["batch_flag"] = '0'  # 单笔撤单
        # data["exchange_type"] = ''
        data["entrust_no"] = entrust_no

        rv = self.request_data(HS_FUNC_CANCEL_ORDER, data, desc="cancel_order2")
        return rv

    #----------------------------------------------------------------------
    def close(self):
        """关闭"""
        self.release()

    def exit(self):
        """退出"""
        pass

    #----------------------------------------------------------------------
    @simple_log
    def on_front_connected(self):
        """服务器连接"""
        # we could auto send req_user_login here
        pass

    #----------------------------------------------------------------------
    @simple_log
    def on_front_disconnected(self, n):
        """服务器断开"""
        pass

    #----------------------------------------------------------------------
    @simple_log
    def on_recv_msg(self, funcID, issueType, refId, msg):
        """消息回调
        issue_type: 主推类型 12-成交 23-委托
        ref_id: 参考序号 用于和请求匹配
        """
        print("on_recv_msg")
        print(msg)
        jsonData = json.loads(msg)
        return 

        function_id = int(jsonData["function_id"])
        ref_id = jsonData["ref_id"]
        issue_type = jsonData["issue_type"]
        print("on_recv_msg function_id:%s(%s) ref_id:%s, issue_type:%s" \
              % (function_id, func_id_desc(function_id), ref_id, issue_type))
        # print(jsonData["json"])

        if function_id != HS_FUNC_SUB_ORDER:
            self.write_log(msg, "on_recv_msg")

        if "json" not in jsonData:
            print("on_recv_msg no 'json' elem in jsonData!")
            return
        data = jsonData["json"]

        if function_id == HS_FUNC_RTN_ORDER:
            # 委托回报
            if issue_type == "12":
                self.on_rtn_trade(data)
            elif issue_type == "23":
                self.on_rtn_order(data)
            else:
                print("on_recv_msg unknown issue_type:%s for function_id:%s(%s)" \
                      % (issue_type, function_id, func_id_desc(function_id)))
                pass
        elif function_id == HS_FUNC_LOGIN:
            # 登录应答
            self.on_rsp_login(data)
        elif function_id == HS_FUNC_QRY_CASH:
            # 客户资金
            self.on_rsp_qry_trading_account(data)
        elif function_id == HS_FUNC_QRY_POSITION:
            # 持仓查询应答
            self.on_rsp_qry_position(data)
        elif function_id == HS_FUNC_QRY_ORDER:
            # 委托查询应答
            self.on_rsp_qry_order(data)
        elif function_id == HS_FUNC_QRY_TRADE:
            # 成交查询应答
            self.on_rsp_qry_trade(data)
        elif function_id == HS_FUNC_QRY_SECINFO:
            # 证券信息查询应答
            pass
        elif function_id == HS_FUNC_SUB_ORDER:
            # 订阅应答
            pass
        else:
            # 
            pass

    #----------------------------------------------------------------------
    # 自定义各个业务处理函数
    #----------------------------------------------------------------------
    def on_trade_error(self, error, funcName=None):
        """交易接口统一错误通知
        error: spi回调的error参数
        """
        err = BtErrorData()
        err.gatewayName = self.gatewayName
        err.accountID = self.accountID
        # err.errorID = error['ErrorID']
        # err.errorMsg = error['ErrorMsg']
        if funcName:
            err.additionalInfo = funcName
        self.gateway.on_error(err)

    #----------------------------------------------------------------------
    def on_rsp_error(self, error):
        """错误回报"""
        self.on_trade_error(error)

    #----------------------------------------------------------------------
    @simple_log
    def on_rsp_login(self, data):
        """登录应答"""
        print(data)
        if isinstance(data, list):
            data = data[0]

        initDate = data["init_date"]
        sysStatus = data["sys_status"]
        errorNo = data["error_no"]
        errorInfo = data["error_info"]
        enableBalance = data["enable_balance"]
        currentBalance = data["current_balance"]
        sessionNo = data["session_no"]

        self.accountName = data["client_name"]
        self.branchNo = data["branch_no"]
        self.sysnodeID = data["sysnode_id"]
        self.userToken = data["user_token"]
        self.fundAccount = data["fund_account"]

    #----------------------------------------------------------------------
    def on_rsp_order_insert(self, data, error):
        """委托应答响应"""
        # TODO: need this ?
        pass

    #----------------------------------------------------------------------
    @simple_log
    def on_rsp_qry_trading_account(self, data):
        """资金账户查询响应"""
        print(data)
        """
        ta = data
        account = BtAccountData()
        account.accountID = self.accountID
        account.btAccountID = account.accountID
        account.accountName = self.accountName
        account.balance = ta["current_balance"]
        account.available = ta["enable_balance"]
        account.frozenCash = ta["frozen_balance"]
        account.tradingDay = datetime.now().strftime("%Y%m%d")
        """

    #----------------------------------------------------------------------
    @simple_log
    def on_rsp_qry_position(self, data):
        """持仓查询响应"""
        print(data)
        """
        for pos_data in data:
            pos = BtPositionData()
            pos.accountID = self.accountID
            pos.exchange = exchange_xc_2_inner(pos_data["exchange_type"])
            pos.symbol = pos_data["stock_code"]
            pos.btSymbol = make_btsymbol(pos.symbol, pos.exchange)

            pos.price = float(pos_data["cost_price"])
            pos.position = int(pos_data["hold_amount"])
            pos.frozen = int(pos_data["frozen_amount"])
            pos.avail = int(pos_data["enable_amount"])
            pos.lastPrice = float(pos_data["last_price"])
            pos.positionValue = float(pos_data["market_value"])
            pos.positionPnl = float(pos_data["income_balance"])

            self.gateway.on_position(pos)
        """

    #----------------------------------------------------------------------
    @simple_log
    def on_rsp_qry_trade(self, data):
        """成交查询响应"""
        print(data)
        """
        for trade_data in data:
            trade = BtTradeData()
            trade.tradeDate = trade_data["date"]
            trade.exchange = exchange_xc_2_inner(trade_data["exchange_type"])
            trade.symbol = trade_data["stock_code"]
            trade.btSymbol = make_btsymbol(trade.symbol, trade.exchange)
            trade.direction = direction_xc_2_inner(trade_data["entrust_bs"])
            trade.price = float(order_data["business_price"])
            trade.volume = int(float(order_data["business_amount"]))
            trade.tradeTime = trade_data["business_time"]
            # real_type: 成交类型
            # real_status: 处理标志
            trade.tradeMoney = trade_data["business_balance"]
            trade.tradeID = trade_data["business_no"]
            trade.orderID = trade_data["order_id"]  # FIXME

            self.gateway.on_trade(trade)
        """

    #----------------------------------------------------------------------
    @simple_log
    def on_rsp_qry_order(self, data):
        """委托查询响应"""
        print(data)
        """
        for order_data in data:
            order = BtOrderData()
            order.tradingDay = order_data["init_date"]
            order.accountID = self.accountID
            order.orderID = order_data["entrust_no"]
            order.exchange = exchange_xc_2_inner(order_data["exchange_type"])
            order.symbol = order_data["stock_code"]
            order.btSymbol = make_btsymbol(order.symbol, order.exchange)
            order.direction = direction_xc_2_inner(order_data["entrust_bs"])
            if order.direction == Direction.NONE:
                continue
            order.price = float(order_data["entrust_price"])
            order.totalVolume = int(float(order_data["entrust_amount"]))
            order.tradedAvgPrice = float(order_data["business_price"])
            order.tradedVolume = int(float(order_data["business_amount"]))
            # order.orderTime = order_data["report_time"][:-3]
            order.status = orderStatusMap.get(order_data["entrust_status"], OrderStatus.UNKNOWN)
            order.orderTime = order_data["entrust_time"]
            order.insertDate = order_data["entrust_date"]

            self.gateway.on_order(order)
        """

    #----------------------------------------------------------------------
    @simple_log
    def on_rtn_order(self, data):
        """回报推送，委托信息
        委托推送：fund_account, branch_no, stock_code, entrust_amount, entrust_price,
                 entrust_bs, entrust_no, entrust_type, entrust_status, report_no,
                 batch_no, exchange_type, stock_account, report_account, report_time
        """
        print(data)
        """
        for order_data in data:
            order = BtOrderData()
            order.tradingDay = order_data["init_date"]
            order.accountID = self.accountID
            # real_type: 0-买卖 1-查询 2-撤单
            # real_status: 0-成交 2-废单 3-补单时成交后废单 4-确认
            order.exchange = exchange_xc_2_inner(order_data["exchange_type"])
            if order.exchange == Exchange.NONE:
                print("[warn] got rtn order data:%s" % order_data)
                continue

            # report_seat: 申报席位
            # report_no: 申报编号
            # report_account: 申报账号

            # stock_account
            order.symbol = order_data["stock_code"]
            order.btSymbol = make_btsymbol(order.symbol, order.exchange)

            # stock_type: 证券类别 0-股票 1-基金 2-红利 3-配股权证 4-申购回款 ... 
            # record_no: 委托库记录号
            # record_bs: 申报方向

            order.direction = direction_xc_2_inner(order_data["entrust_bs"])
            order.status = orderStatusMap.get(order_data["entrust_status"], OrderStatus.UNKNOWN)
            order.orderID = order_data["entrust_no"]

            # business_no: 成交编号
            # business_amount: 成交数量
            # business_price: 成交价格
            # business_time: 成交时间
            # business_balance: 成交金额
            # extern_coe: 外部错误代码

            order.orderTime = order_data["report_time"][:-3]  # FIXME and convert
            order.totalVolume = int(float(order_data["entrust_amount"]))
            order.price = float(order_data["entrust_price"])

            # batch_no: 委托批号
            # entrust_type: 委托类别 0-委托 1-查询 2-撤单 3-补单 4-确认 5-大宗 ...
            # order_id: 客户订单编号 ?
            # stock_name: 证券名称
            # entrust_prop: 委托属性 0-买卖 1-配股 2-转托 3-申购 4-回购 ...
            # withdraw_no: 撤单号
            # clear_balance: 清算金额
        """

    #----------------------------------------------------------------------
    @simple_log
    def on_rtn_trade(self, data):
        """回报推送，成交信息
        成交推送：business_xxx
        """
        print(data)



#----------------------------------------------------------------------
def main(server_ip='', server_port=''):
    print('test py35 x64 hstrade api')

    """主测试函数，出现堵塞时可以考虑使用sleep
    API使用步骤:
    1. 继承实现 XcTdApi 类的相关函数
    2. 创建类对象
    3. 初始化连接 init
    4. 登录获取账户相关信息
    5. 订阅回报
    6. 发送请求
    7. release
    """
    reqid = 0

    # 创建API对象
    api = MyHSTdApi()

    server_ip = "120.55.176.113"
    server_port = "9359"
    accountID = ""
    password = ""

    setting = {
        "tdAddress": server_ip,
        "tdPort": server_port,
        "accountID": accountID,
        "password": password,
        "license": "(20130524)HSSBCS-HSTZYJ20-0000_3rd.dat"
    }

    print("td version:{}".format(api.get_api_version()))

    # 打开调试模式：日志文件名及日志级别，0-INFO
    # api.open_debug("", 0)

    print('td connect to %s:%s user_id:%s ...' % (server_ip, server_port, accountID))

    # 初始化api，连接服务器
    rv = api.init(setting)
    print("td connect rv:%s" % rv)

    while 1:
        cmd = input("input cmd:1-login, 2-subscribe, 3-qry_cash, 4-qry_pos, 5-qry_order, 6-qry_trade, 11-order, 12-cancel\n")
        if cmd in ['e', 'exit', 'q', 'quit']:
            break

        if cmd == '1':
            api.login()
        elif cmd == '2':
            api.subscribe()
        elif cmd == '3':
            api.qry_account()
        elif cmd == '4':
            api.qry_position()
        elif cmd == '5':
            api.qry_order()
        elif cmd == '6':
            api.qry_trade()
        elif cmd == '11':
            symbol = "510050"
            direction = "buy"
            order_qty = 200
            order_price = 2.41
            rv = api.send_order2(symbol, direction, order_qty, order_price)
            print("send_order2 rv:%s" % rv)
        elif cmd == '12':
            order_no = input("input entrust_no: ")
            rv = api.cancel_order2(order_no)
            print("cancel_order2 rv:%s" % rv)
        else:
            pass

    api.release()

if __name__ == '__main__':
    print(sys.argv)
    # if len(sys.argv) < 3:
    #     print("command like: python3 xctd_test.py ip port")
    # else:
    #     main(sys.argv[1], int(sys.argv[2]))
    main()
