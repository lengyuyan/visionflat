/*********************************************************************
    *文件说明: 
    *作者: 
    *日期: 2013/11/25
    *修改日志:  2013/11/25 CW0793 创建
                2016/03/16 EW-0551 修改
                1. 把vector缓冲区换成hash列表，提高查询效率
                2. 返回skt为自动生成的标识ID，之前是返回指针地址
                
***********************************************************************/
#pragma once
#ifndef _INCLUDE_SOCKET_H
#define _INCLUDE_SOCKET_H
#include "sysapi.h"
#include <vector>
#include <hash_map>

using std::vector;
using stdext::hash_map;

#define SKT_INSTANCE CSSocket::GetInstance()

class ISocket;
class CListen;
class CSSocket
{
public:
    CSSocket(void);
    virtual ~CSSocket(void);

    int CreateC(const char* server, unsigned short sport, const char* local = 0, unsigned short lport = 0); //创建一个client

    int CreateS(const char* server, unsigned short sport, const char* local, unsigned short lport);    //创建一个server

    int SetModel(int skt, int nCode);       //设置模式
    int GetModel(int skt);                  //获取模式

    int Close(int skt); //关闭一个skt,关闭主服务端时,所有与其关联客户端都会关闭

    int Connect(int skt);   //手动连接

    int Reconn(int skt);

    int Send(int skt, const char* pData, int len);

    int Recv(int skt, char* pData, int len, int timeout = -1);

    int GetStatus(int skt);

    int GetSocket(int skt);

    int GetAddr(int skt, char* lpRemote, unsigned short* pusport);   //获取远端IP及端口

    int ClearMsg(int skt);   //重置消息缓冲区

    static CSSocket* GetInstance();
    static void ReleaseInstance();

protected:

    ISocket* FindSKT(int skt);      //通过一个skt句柄查找skt结构体

    int GetAutoSktID() { return m_InitSktID++; }   //自动生成一个ID

private:
    static CSSocket*        m_Instance;

    int                     m_InitSktID;        //初始状态时的ID
    Handle                  m_hSection;         //临界区

    vector<CListen*>        m_vecListen;        //所有监听类
    hash_map<int, ISocket*> m_hash_skt;         //所有skt成员
};

#endif  //防止重复包含