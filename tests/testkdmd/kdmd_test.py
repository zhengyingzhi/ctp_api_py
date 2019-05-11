# encoding: UTF-8

import sys
from time import sleep

from kdmd import KDMdApi


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
class TestKDMdApi(KDMdApi):
    """测试用实例"""

    #----------------------------------------------------------------------
    def __init__(self):
        """Constructor"""
        super(TestKDMdApi, self).__init__()
        
    #----------------------------------------------------------------------
    @simple_log    
    def onFrontConnected(self):
        """服务器连接"""
        print('md connected')
        pass

    #----------------------------------------------------------------------
    @simple_log    
    def onFrontDisconnected(self, n):
        """服务器断开"""
        print('md disconnected', n)

    @simple_log 
    #----------------------------------------------------------------------
    def onRspUserLogin(self, data):
        """登陆回报"""
        print_dict(data)

    #----------------------------------------------------------------------
    @simple_log    
    def onRspUserLogout(self, data):
        """登出回报"""
        print_dict(data)

    #----------------------------------------------------------------------
    @simple_log    
    def onRspSubMarketData(self, data, last):
        """订阅合约回报"""
        print_dict(data)

    #----------------------------------------------------------------------
    @simple_log    
    def onRspUnSubMarketData(self, data, last):
        """退订合约回报"""
        print_dict(data)

    #----------------------------------------------------------------------
    @simple_log    
    def onRspQryData(self, data, error, last):
        """退订合约回报"""
        print_dict(data)
        print_dict(error)

    #----------------------------------------------------------------------
    @simple_log    
    def onRtnMarketData(self, market_id, service_id, data):
        """行情推送"""
        print("----", market_id, service_id, "----")
        print_dict(data)

#----------------------------------------------------------------------
def main():
    print('test py35 x64 kd md api')

    """主测试函数，出现堵塞时可以考虑使用sleep"""
    reqid = 0

    # 创建API对象
    api = TestKDMdApi()

    print("md version:{}".format(api.getApiVersion()))

    api.openDebug("kdmd_test.log", 0)

    # 在C++环境中创建MdApi对象，传入参数是希望用来保存.con文件的地址
    api.createMdApi('')
    
    # 注册前置机地址
    api.registerFront("39.104.95.144", 8500)
    # api.registerFront("192.168.1.174", 8502)

    # 设置心跳时间
    api.setOption(0x10, 15000)

    print('md init...')

    # 初始化api，连接前置机
    api.init(0)
    sleep(0.5)
    
    # 登陆
    loginReq = {}                           # 创建一个空字典
    loginReq['UserID'] = '00100001'         # 参数作为字典键值的方式传入
    loginReq['Password'] = 'bigquant123'    # 键名和C++中的结构体成员名对应
    i = api.reqUserLogin(loginReq, 0)
    sleep(0.5)
    
    ## 登出，测试出错（无此功能）
    #reqid = reqid + 1
    #i = api.reqUserLogout({}, 1)
    #sleep(0.5)

    ## 安全退出，测试通过
    #i = api.exit()

    market_id = 1  # SSE
    service_id = 3 # index md
    req = {
        "InstrumentID": "000001",
        "MarketId": market_id,
        "ServiceId": service_id
    }

    ## 订阅行情，测试通过
    # i = api.subscribeMarketData(req)

    ## 退订合约，测试通过
    #i = api.unSubscribeMarketData(req)

    data = {}
    # rv = api.reqQryData(req)
    rv = api.reqGetData(req, data, 2000)
    print("reqGetData rv:%s, data:%s" % (rv, data))

    # 连续运行，用于输出行情
    while (1):
        sleep(10)
        #data = {}
        #api.reqGetData(req, data, 2000)
        #print("reqGetData rv:%s, data:%s" % (rv, data))
    
    
    
if __name__ == '__main__':
    main()
