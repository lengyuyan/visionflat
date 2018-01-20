/*********************************************************************
    *文件说明: socket基类，从此类继承Server/Client类
    *作者: logo
    *日期: 2016/07/25
    *修改日志:  2016/07/25 CW34945 创建
                
***********************************************************************/
#pragma once
#ifndef INCLUDE_ISOCKET_H
#define INCLUDE_ISOCKET_H
#include "sysapi.h"
#include <vector>

using std::vector;

class CListen
{//监听类
public:
    CListen();
    virtual ~CListen(void);

    int Init(const char* lpServer, unsigned short usPort);

    int WaitConnect(ST_SOCKADDR& addr);

    //ip和端口是否和自己相同?
    bool IsSelf(const char* lpServer, unsigned short usPort) { return (usPort == m_usPort && 0 == strcmp(lpServer, m_szServer)) ? true : false; }

    //管理使用包含的句柄
    void PushHandle(int skt);
    void PopupHandle(int skt);
    int GetHandleCnt() { return (int)m_vecskt.size(); }

protected:

    int CreateSocket();  //创建socket
    int ListenSocket();  //绑定端口
    int AcceptClient();  //接收连接 固定等待50ms

    bool ChkAddressValid(const char* ip);   //判断是否可以作为服务端IP

    ST_SOCKADDR* GetClient(const char* lpClient, unsigned short uPort);        //通过ip+port查找客户端

private:

    bool            m_bBind;        //是否已经绑定端口
    int             m_sktListen;    //socket监听句柄
    char            m_szServer[MAX_IP_LEN]; //ip
    unsigned short  m_usPort;

    vector<int>             m_vecskt;   //使用此服务端的skt句柄
    vector<ST_SOCKADDR*>    m_vecPTP;   //已连接skt结构体
    Handle                  m_hSection; //临界区
};

//socket基类
class ISocket
{
public:
    ISocket(void);
    virtual ~ISocket(void);

    void Close();    //关闭

    CListen* GetListen() { return m_pListen;}
    int GetModel() { return m_emWorkModel;} //获取工作模式
    int GetSocket() { return m_socket; }
    int GetStatus();   //获取状态

    int Send(const char* pData, int len);
    int Recv(char* pData, int len, int timeout);

    int ManualConn();   //手动连接

    int Reconn();

    int SetModel(int nCode);    //设置工作模式 EM_SKT_TYPE

    void ClearMsgList();    //清空接收消息队列
    void GetAddr(char* lpRemote, unsigned short& usport);

    virtual int WaitConn() = 0; //成功返回socket句柄，失败返回INVALID_SKT

protected:

    int ChangeStatus(int status);   //更改状态

    int Connect();  //连接

    int SetRemoteIP(const char* szIP, int usPort);

    int RecvEx(char* pData, int len, int timeout);

    void RunThreadConnect();    //线程连接
    static int WINAPI Run(void* lparam);

    CListen*    m_pListen;  //监听类

private:

    bool    m_bRun;         //线程运行标识
    Handle  m_hQuitSign;    //线程退出信号

    int     m_socket;       //socket句柄
    char    m_szIP[MAX_IP_LEN];
    int     m_usPort;
    int     m_iStatus;      //状态
    int     m_emWorkModel;  //工作模式
    Handle  m_hSection;     //临界区
};

#endif  //防止头文件重复包含