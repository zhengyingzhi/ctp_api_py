#ifndef _KD_MD_API_STRUCT_H_
#define _KD_MD_API_STRUCT_H_

#include <stdint.h>

#define KD_API_Type_None        0x00        // API类别（可能存在多种实现形式的API连接到行情网关服务）
#define KD_API_Type_Default     0x01

#define KD_API_Cipher_Simple    0x01        // 简单加密

#define KD_API_Compress_None    0x00        // 未压缩

#define KD_FLAG_INVALID         0x00        // 标志无效
#define KD_FLAG_LAST            0x80000000  // 标志是否最后一条

#define KD_SERVER_FLAG_ANDLOW8  0x000000ff  // Server低8位作为压缩方法值使用
#define KD_SERVER_FLAG_NONE     0x00        // 无标志
#define KD_SERVER_FLAG_UDP      (8 << 1)    // 对应TCPServer服务端使用了UDP


/// 登录请求
typedef struct
{
    uint8_t         m_APIType;          // API类别
    uint8_t         m_Cipher;           // 加密类型
    uint8_t         m_Compress;         // 压缩标志
    uint8_t         m_Reserve;          // 预留字段1
    uint32_t        m_APIVersion;       // API版本号
    uint32_t        m_ReqID;            // 请求ID
    uint32_t        m_Flag;             // 请求标志
    uint32_t        m_UdpFlag;          // API使用UDP

    char            m_UserID[16];       // 客户号
    char            m_Password[32];     // 密码
    char            m_MAC[20];          // MAC地址
    char            m_IP[16];           // IP地址
}KDMDLoginData;

/// 登录响应
typedef struct  
{
    char            m_UserID[16];       // 客户号
    uint32_t        m_ReqID;            // 请求ID
    uint32_t        m_Flag;             // 请求标志
    uint32_t        m_ServerFlag;       // Server相关标志
    int32_t         m_ErrorID;          // 错误ID
    int64_t         m_Reserve;          // 预留字段
}KDMDLoginRspData;

/// 登出请求
typedef struct
{
    char            m_UserID[16];       // 客户号
    uint32_t        m_ReqID;            // 请求ID
    uint32_t        m_Flag;             // 请求标志
}KDMDLogoutData;

/// 请求查询/订阅/取消订阅一个或多个产品信息或行情
typedef struct  
{
    uint32_t        m_ReqID;            // 请求ID
    uint32_t        m_Flag;             // 请求标志
    uint32_t        m_Count;            // 产品个数
}KDMDRequestData;

/// 查询订阅/取消订阅应答
typedef struct
{
    uint32_t        m_RelatedReqID;      // 请求ID
    uint32_t        m_Flag;              // 标志
    int32_t         m_ErrorID;           // 错误ID
    uint32_t        m_Count;             // 产品个数
}KDMDRspDataHead;


#endif//_KD_MD_API_STRUCT_H_
