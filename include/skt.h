/*********************************************************************
    *文件说明: socket 封装类接口文件,所有对外接口都在这里定义
    *作者: 
    *日期: 2013/11/25
    *修改日志:  2013/11/25 CW0793 创建
                
***********************************************************************/
#pragma once
#ifndef DLL_INCLUDE_SOCKET_SKT_H
#define DLL_INCLUDE_SOCKET_SKT_H
#define SKT_VERSION "skt_1.1.1.20170122_beta"

#define WINAPI __stdcall
#define EXPORT_C extern "C"

enum EMSocketStatus
{
    SOCKET_DISCONNECTED = 0,    //未连接
    SOCKET_CONNECTED,           //已连接
};

//SKT模式
enum EMSKT_MODEL
{
    EMSKT_RECONN_CHKIP = 0x01,      //重连验证IP 适用于server
    EMSKT_RECONN_CHKPORT = 0x02,    //重连验证端口 适用于server
    EMSKT_RECONN_NOCLOSE = 0x04,    //重连时不关闭原有的socket
    EMSKT_NO_RECONN = 0x08,         //设置为不自动重连连接
    EMSKT_NO_AUTOCONN = 0x10,       //设置为不自动连接
};

/****************************************************
    *Function: 初始化socket
    *Intput: NULL
    *Output: NULL
    *Return: 成功返回0，失败返回错误码
******************************************************/
EXPORT_C int WINAPI skt_init();

/****************************************************
    *Function: 反初始化socket
    *Intput: NULL
    *Output: NULL
    *Return: NULL
******************************************************/
EXPORT_C void WINAPI skt_uninit();

/****************************************************
    *Function: 创建一个tcp 客户端,自动连接
               释放对象时,自动关闭socket.
    *Intput: server 要连接的服务端ip
             sport  要连接的服务端端口
             local  要绑定的本地ip,不需绑定传NULL即可
             lport  要绑定的本地端口,不使用传0
    *Output: NULL
    *Return: 失败返回INVALID_SKT, 成功返回skt句柄
    *PS： 创建成功后调用skt_status可以查询是否连接ok
******************************************************/
EXPORT_C int WINAPI create_c(const char* server, unsigned short sport, const char* local = 0, unsigned short lport = 0);

/****************************************************
    *Function: 创建一个tcp 服务端,并自动等待指定客户端连接
               释放对象时,会自动关闭socket.
    *Intput: server 要绑定的服务端ip,可以传NULL
             sport  要绑定的服务端端口
             local  指定客户端ip,不指定传NULL即可，即随机IP
             lport  指定客户端port,不指定传NULL即可，即随机port
    *Output: NULL
    *Return: 失败返回INVALID_SKT, 成功返回skt句柄
    *PS： 创建成功后调用skt_status可以查询是否连接ok
******************************************************/
EXPORT_C int WINAPI create_s(const char* server, unsigned short sport, const char* local = 0, unsigned short lport = 0);

/****************************************************
    *Function: 关闭一个skt句柄,适用于客户端/服务端
    *Intput: skt skt句柄
    *Output: NULL
    *Return: 成功返回0,失败返回错误代码
******************************************************/
EXPORT_C int WINAPI skt_close(int skt);

/****************************************************
    *Function: 主动连接一个skt
    *Intput: skt skt句柄
    *Output: NULL
    *Return: 成功返回0,失败返回错误代码
******************************************************/
EXPORT_C int WINAPI skt_conn(int skt);

/****************************************************
    *Function: 设置一个skt重新连接，会关闭原连接
    *Intput: skt  skt句柄
    *Output: NULL
    *Return: 成功返回0,失败返回错误代码
******************************************************/
EXPORT_C int WINAPI skt_reconn(int skt);

/****************************************************
    *Function: 设置一个skt句柄模式，具体参考 EM_SKT_TYPE
    *Intput: skt    skt句柄
             nCode  skt模式 EM_SKT_TYPE
    *Output: NULL
    *Return: 成功返回0,失败返回错误代码
******************************************************/
EXPORT_C int WINAPI skt_set_model(int skt, int nCode);

/****************************************************
    *Function: 获取一个skt句柄当前工作模式 EM_SKT_TYPE
    *Intput: skt    skt句柄
    *Output: NULL
    *Return: 成功返回当前模式,失败返回0
******************************************************/
EXPORT_C int WINAPI skt_get_model(int skt);

/****************************************************
    *Function: 发送消息函数,适用于客户端/服务端.
    *Intput: skt skt句柄
             pData 要发送的数据
             len  要发送的数据长度
    *Output: NULL
    *Return: 成功返回发送成功的数据长度,失败返回错误代码 不会返回0
******************************************************/
EXPORT_C int WINAPI skt_send(int skt, const char* pData, int len);

/****************************************************
    *Function: 接收消息函数,适用于客户端/服务端.
    *Intput: skt skt句柄
             pData 接收数据缓冲区
             len  能接收的最大数据长度
             timeout 超时,默认无限等待,直到有消息
                        或者连接被关闭
    *Output: pData 接收数据缓冲区
    *Return: 成功返回接收到的数据长度,失败返回错误代码,不会返回0
    ps:返回ERR_TIMEOUT表示没有消息可读,函数超时返回
******************************************************/
EXPORT_C int WINAPI skt_recv(int skt, char* pData, int len, int timeout = 0xFFFFFFFF);

/****************************************************
    *Function: 查询当前skt状态,适用于客户端/服务端.
    *Intput: skt skt句柄
    *Output: NULL
    *Return: 参考SocketNetStatus
******************************************************/
EXPORT_C int WINAPI skt_status(int skt);

/****************************************************
    *Function: 获取一个skt句柄对应的底层socket句柄
    *Intput: skt skt句柄
    *Output: NULL
    *Return: 失败返回INVALID_SKT, 成功返回soket句柄
    *PS: 此句柄可以直接使用sysapi接口调用
******************************************************/
EXPORT_C int WINAPI get_socket(int skt);

/****************************************************
    *Function: 获取一个skt对应的对端ip/port
    *Intput: skt skt句柄
    *Output: lpRemote ip地址指针 可以传NULL
             pusport  端口指针 可以传NULL
    *Return: 成功返回0,失败返回错误代码
******************************************************/
EXPORT_C int WINAPI get_addr(int skt, char* lpRemote = 0, unsigned short* pusport = 0);

/****************************************************
    *Function: 重置skt对应的消息缓冲区.
    *Intput: skt skt句柄
    *Output: NULL
    *Return: 成功返回0,失败返回错误代码
******************************************************/
EXPORT_C int WINAPI clear_msg(int skt);

#endif  //防止重复包含