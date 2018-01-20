/*********************************************************************
    *文件说明: 客户端类头文件，1.自动连接,2 自动重连
    *作者: 
    *日期: 2013/11/25
    *修改日志:  2013/11/25 CW0793 创建
                
***********************************************************************/
#pragma once
#ifndef INCLUDE_SOCKET_CLIENT_H
#define INCLUDE_SOCKET_CLIENT_H
#include "ISocket.h"

class CClient: public ISocket
{
public:
    CClient();
    virtual ~CClient();

    int Init(const char* lpServer, unsigned short sport, const char* local, unsigned short lport);

    int WaitConn();

protected:

    int CreateClientSocket();   //创建socket

private:

    char            m_szLocal[MAX_IP_LEN];
    unsigned short  m_lPort;
    int             m_sktTmp;
};

#endif  //防止重复包含