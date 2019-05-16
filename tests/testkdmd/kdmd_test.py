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
    def on_front_connected(self):
        """服务器连接"""
        # we could auto send req_user_login here
        pass

    #----------------------------------------------------------------------
    @simple_log    
    def on_front_disconnected(self, n):
        """服务器断开"""
        print('md disconnected', n)

    @simple_log 
    #----------------------------------------------------------------------
    def on_rsp_user_login(self, data):
        """登陆回报"""
        print_dict(data)

    #----------------------------------------------------------------------
    @simple_log    
    def on_rsp_user_logout(self, data):
        """登出回报"""
        print_dict(data)

    #----------------------------------------------------------------------
    @simple_log    
    def on_rsp_sub_market_data(self, data, last):
        """订阅合约回报"""
        print_dict(data)

    #----------------------------------------------------------------------
    @simple_log    
    def on_rsp_unsub_market_data(self, data, last):
        """退订合约回报"""
        print_dict(data)

    #----------------------------------------------------------------------
    @simple_log    
    def on_rsp_qry_data(self, data, error, last):
        """退订合约回报"""
        print_dict(data)
        print_dict(error)

    #----------------------------------------------------------------------
    @simple_log    
    def on_rtn_market_data(self, market_id, service_id, data):
        """行情推送"""
        print("----", market_id, service_id, "----")
        print_dict(data)

#----------------------------------------------------------------------
def main(server_ip, server_port):
    print('test py35 x64 kd md api server %s:%s' % (server_ip, server_port))

    """主测试函数，出现堵塞时可以考虑使用sleep
    API使用步骤:
    1. 继承实现 KDMdApi 类的相差函数
    2. 创建类对象
    3. 注册服务器地址和端口
    4. 设置连接选项，需要在init()前调用，如心跳时间，加密，压缩，UDP等
    5. 调用 init() 连接服务器，可同步/异步连接
    6. 若连接成功，则可以起登录请求，登录请求也可以在 on_front_connected 回调函数中进行
    7. 登录成功后，可进行订阅/同步异步查询数据等请求
    8. 使用结束后，调用 release() 接口释放接口资源
    """
    reqid = 0

    # 创建API对象
    api = TestKDMdApi()

    print("md version:{}".format(api.get_api_version()))

    # 打开调试模式：日志文件名及日志级别，0-INFO
    api.open_debug("kdmd_test.log", 0)

    # 创建内部MdApi对象
    api.create_md_api('')
    
    # 注册前置机地址
    api.register_front(server_ip, server_port)

    # 设置心跳时间，参考 KDCommonDef.py KD_MD_OPTION_Xxxx 
    api.set_option(0x10, 15000)

    print('md init...')

    # 初始化api，连接前置机，参数为超时时间，0表示异步连接
    timeout_ms = 2000
    rv = api.init(timeout_ms)
    # sleep(1.5)
    print("init rv:%s" % rv)
    
    # 登陆
    login_req = {}
    login_req['UserID'] = '00100001'
    login_req['Password'] = ''
    rv = api.req_user_login(login_req, timeout_ms)
    # sleep(0.5)
    print("req_user_login rv:%s" % rv)

    ## 安全退出，测试通过
    # api.exit()

    # 数据请求，市场ID+服务数据ID+代码
    # 市场ID： 1-SSE 2-SZSE 参考 KD_MI_XXX
    # 服务数据ID：2-idx_info 3-idx_data 4-stk_info 5-stk_data 参考 KD_SI_XXX
    market_id = 1
    service_id = 3
    req = {
        "InstrumentID": "000001",
        "MarketId": market_id,
        "ServiceId": service_id
    }

    ## 订阅行情，测试通过
    # i = api.subscribe_market_data(req)

    ## 退订合约，测试通过
    #i = api.unsubscribe_market_data(req)

    # 异步查询
    # rv = api.req_qry_data(req)

    # 同步查询，查询结果存储在 out_data 中
    timeout_ms = 2500
    out_data = {}
    rv = api.req_get_data(req, out_data, timeout_ms)
    print("req_get_data rv:%s, out_data:%s" % (rv, out_data))

    # 连续运行，用于输出行情
    while (1):
        sleep(1)
    
    api.release()

if __name__ == '__main__':
    print(sys.argv)
    if len(sys.argv) < 3:
        print("command like: python3 kdmd_test.py ip port")
    else:
        main(sys.argv[1], int(sys.argv[2]))
