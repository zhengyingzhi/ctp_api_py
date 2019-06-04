# encoding: UTF-8

import sys
from time import sleep

print('importing ctptd')
from ctptd import *

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
        print(str(func.__name__))
        return func(*args, **kw)
    return wrapper


########################################################################
class TestTdApi(TdApi):
    """测试用实例"""
    def __init__(self):
        """Constructor"""
        super(TestTdApi, self).__init__()
        self.max_order_ref = 1

    def next_order_ref(self):
        self.max_order_ref += 1
        return str(self.max_order_ref)

    @simple_log
    def onFrontConnected(self):
        """服务器连接"""
        print('td connected')
        pass

    @simple_log
    def onFrontDisconnected(self, n):
        """服务器断开"""
        print('td disconnected', n)

    @simple_log
    def onRspError(self, error, n, last):
        """错误"""
        print_dict(error)

    @simple_log
    def onRspAuthenticate(self, data, error, n, last):
        print_dict(data)
        print_dict(error)

    @simple_log
    def onRspUserLogin(self, data, error, n, last):
        """登陆回报"""
        print_dict(data)
        print_dict(error)
        self.max_order_ref = int(data['MaxOrderRef'])

    @simple_log
    def onRspUserLogout(self, data, error, n, last):
        """登出回报"""
        print_dict(data)
        print_dict(error)

    @simple_log
    def onRspQrySettlementInfo(self, data, error, n, last):
        """查询结算信息回报"""
        print_dict(data)
        print_dict(error)

    @simple_log
    def onRspSettlementInfoConfirm(self, data, error, n, last):
        """确认结算信息回报"""
        print_dict(data)
        print_dict(error)

    @simple_log
    def onRspQryInstrument(self, data, error, n, last):
        """查询合约回报"""
        print_dict(data)
        print_dict(error)
        print(n)
        print(last)

    @simple_log
    def onRspQryInstrumentMarginRate(self, data, error, n, last):
        """查询保证金率回报"""
        print_dict(data)
        print_dict(error)
        print(n)
        print(last)

    @simple_log
    def onRspQryInvestorPosition(self, data, error, n, last):
        """查询持仓"""
        print_dict(data)
        print_dict(error)
        print(n)
        print(last)

    @simple_log
    def onRspQryInvestorPositionDetail(self, data, error, n, last):
        """查询持仓明细"""
        print_dict(data)
        print_dict(error)
        print(n)
        print(last)

    @simple_log
    def onRspQryOrder(self, data, error, n, last):
        """查询报单"""
        print_dict(data)
        print_dict(error)
        print(n)
        print(last)

    @simple_log
    def onRspQryTrade(self, data, error, n, last):
        """查询成交"""
        print_dict(data)
        print_dict(error)
        print(n)
        print(last)

    @simple_log
    def onRspQryTradingAccount(self, data, error, n, last):
        """查询资金账户"""
        print_dict(data)
        print_dict(error)
        print(n)
        print(last)

    @simple_log
    def onRspQryInvestor(self, data, error, n, last):
        """查询投资者信息账户"""
        print_dict(data)
        print_dict(error)
        print(n)
        print(last)

    @simple_log
    def onRspOrderInsert(self, data, error, n, last):
        """报单错误响应"""
        print_dict(data)
        print_dict(error)

    @simple_log
    def onRtnOrder(self, data):
        """报单通知"""
        print_dict(data)

    @simple_log
    def onRtnTrade(self, data):
        """成交通知"""
        print_dict(data)

#----------------------------------------------------------------------
def main():
    print('test py3 ctp td api')

    """主测试函数，出现堵塞时可以考虑使用sleep"""
    reqid = 0

    # 创建API对象，测试通过
    api = TestTdApi()

    # 在C++环境中创建TdApi对象，传入参数是希望用来保存.con文件的地址，测试通过
    api.createFtdcTraderApi('td')
    print("ctptd api version:{}".format(api.getApiVersion()))

    # 设置数据流重传方式，测试通过
    api.subscribePrivateTopic(1)
    api.subscribePublicTopic(1)

    # simnow
    tdAddr = "tcp://180.168.146.187:10000"
    brokerID = "9999"
    # 评测系统
    tdAddr = "tcp://180.166.213.18:35207"
    brokerID = "3019"
    # 仿真系统
    # tdAddr = "tcp://180.166.213.18:42207"
    # brokerID = "3010"

    # 注册前置机地址，测试通过
    api.registerFront(tdAddr)

    print('td init %s.%s...' % (brokerID, tdAddr))

    investorid = input('input investor id:')
    password = input('input password:')
    if not investorid or not password:
        return
    # investorid = "038313"
    # password = "qwert123"

    print('td init...')

    # 初始化api，连接前置机，测试通过
    api.init()
    sleep(0.5)

    userID = investorid
    req = {
        "UserID": userID,
        "BrokerID": brokerID,
        "AuthCode": "",
        "AppID": ""
    }
    reqid += 1
    api.reqAuthenticate(req, reqid)
    sleep(1.0)

    # 登陆，测试通过
    loginReq = {}                           # 创建一个空字典
    loginReq['UserID'] = investorid         # 参数作为字典键值的方式传入
    loginReq['Password'] = password         # 键名和C++中的结构体成员名对应
    loginReq['BrokerID'] = brokerID
    reqid = reqid + 1                       # 请求数必须保持唯一性
    i = api.reqUserLogin(loginReq, reqid)
    sleep(1.5)

    req = {}
    req['BrokerID'] = brokerID
    req['UserID'] = investorid
    req['InvestorID'] = investorid
    req['HedgeFlag'] = '1'

    cmd_prompt = "input cmd:1(order)|2(qry_order)|3(qry_trade)|4(pos)|5(pos_detail)|6(margin)|7(comm)|8(balance)|9(investor)|99(exit)"
    while True:
        cmd = input(cmd_prompt)
        if not cmd:
            continue

        rv = None
        if cmd == "1":
            # place order
            order_req = {}
            instrument = input("input instrument(a1909,SR909,rb1910):")
            volume = input("input volume:")
            price = input("input price:")
            direction = input("input direction(THOST_FTDC_D_Buy-0,SELL-1)")
            offset = input("input offset(THOST_FTDC_OF_Open-0,Close-1,CloseToday-3,CloseYesterday-4)")
            if not instrument or not volume or not price:
                continue
            try:
                volume = int(volume)
                price = float(price)
            except ValueError:
                continue

            req['InstrumentID'] = instrument
            req['LimitPrice'] = price
            req['VolumeTotalOriginal'] = volume

            # 下面如果由于传入的类型本接口不支持，则会返回空字符串
            req['OrderPriceType'] = '2'         # 1-THOST_FTDC_OPT_AnyPrice 2-THOST_FTDC_OPT_LimitPrice
            req['Direction'] = direction
            req['CombOffsetFlag'] = offset

            req['OrderRef'] = api.next_order_ref()
            req['InvestorID'] = investorid
            req['BrokerID'] = brokerID
            req['UserID'] = investorid

            req['CombHedgeFlag'] = '1'          # THOST_FTDC_HF_Speculation
            req['ContingentCondition'] = '1'    # THOST_FTDC_CC_Immediately立即发单
            req['ForceCloseReason'] = '0'       # THOST_FTDC_FCC_NotForceClose 非强平
            req['IsAutoSuspend'] = 0            # 非自动挂起
            req['TimeCondition'] = '3'          # 1-THOST_FTDC_TC_IOC, 3-THOST_FTDC_TC_GFD
            req['VolumeCondition'] = '1'        # 1-THOST_FTDC_VC_AV, 3-THOST_FTDC_VC_CV
            req['BusinessUnit'] = 'biz'            # how
            req['RequestID'] = 11223
            req['MinVolume'] = 1

            reqid = reqid + 1
            rv  = api.reqOrderInsert(req, reqid)
        elif cmd == "2":
            # query order
            reqid = reqid + 1
            rv = api.reqQryOrder(req, reqid)
        elif cmd == "3":
            # query trade
            req['HedgeFlag'] = '1'
            reqid = reqid + 1
            rv = api.reqQryTrade(req, reqid)
        elif cmd == "4":
            # query position
            instrument = input("input instrument:")
            req['InstrumentID'] = instrument
            reqid = reqid + 1
            rv = api.reqQryInvestorPosition(req, reqid)
        elif cmd == "5":
            # query position detail
            instrument = input("input instrument:")
            req['InstrumentID'] = instrument
            reqid = reqid + 1
            rv = api.reqQryInvestorPositionDetail(req, reqid)
        elif cmd == "6":
            # query margin rate
            instrument = input("input instrument:")
            req['InstrumentID'] = instrument
            req['HedgeFlag'] = '1'
            reqid = reqid + 1
            rv = api.reqQryInstrumentMarginRate(req, reqid)
        elif cmd == "9":
            # query investor info
            rv = api.reqQryInvestor(req, reqid)
        elif cmd == "99":
            break
        else:
            continue
        print("request rv:%s" % rv)


if __name__ == '__main__':
    main()
