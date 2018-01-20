#pragma once

#include <ws2tcpip.h>
#include <string>
#include <vector>
using namespace  std;
class SocketClientLogic
{
public:
    SocketClientLogic(void);
    ~SocketClientLogic(void);

    static SocketClientLogic* GetSocketClientInstance();
    static SocketClientLogic *m_pInstance;
    static void DestroySocketClientInstance();


    virtual bool InitClientSocket(int NetType = AF_INET,int SocketType= SOCK_STREAM,int protocol = IPPROTO_TCP);

    virtual bool ClientSocketConnect(const char* pServerIP,unsigned int port, const char* pClientIP = NULL,unsigned int iClientPort = 0);

    virtual int ClientSocketRecevie(char* buffer,unsigned int len);
    
    virtual int ClientSocketSend(const char* data,unsigned int len);

    virtual bool UninitClientSocket();
    
    virtual bool IsConnected();
	virtual bool ImgConnect(int sence);
	
	virtual int ImgeSend(int sence,char* data,unsigned len);
private:
    int m_SocketStatus;
    SOCKET m_SocketClientFd;
    string m_strServerIP;
	string m_strClientIP;
	unsigned int m_ClientPort;
    unsigned int m_ServerPort;

	vector<SOCKET> m_vec_ImgSkt;
	int m_SenToSktID[36];
	vector<int> m_vec_ImgSktStus;
	bool m_isinit;
    bool ClientSocketConnect();

public:
	// 通信超时时间
	int m_TimeOut;



};

