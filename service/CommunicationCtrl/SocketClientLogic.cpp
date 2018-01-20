#include "SocketClientLogic.h"

#ifndef NULL
#define NULL 0
#endif
#pragma comment(lib,"ws2_32.lib")
enum SocketNetStatus
{
    SOCKET_FD_INVALIDE,//无效socket
    SOCKET_FD_VALIDE,//有效socket
    SOCKET_DISCONNECT, //失去连接
    SOCKET_CONNECTING,//连接中
    SOCKET_CONNECTED, //连接成功
    SOCKET_CLOSE,//关闭
};

SocketClientLogic * SocketClientLogic::m_pInstance = NULL;



SocketClientLogic::SocketClientLogic(void)
{
    m_SocketStatus = SOCKET_FD_INVALIDE;
    m_SocketClientFd = INVALID_SOCKET;  
    //WSADATA wsaData;       //不在客户端初始化 ,网络栈只初始化一次                        
    //WSAStartup(MAKEWORD(2,2), &wsaData);
	m_isinit = false;
	memset(m_SenToSktID,-1,sizeof(m_SenToSktID)); 

	m_TimeOut = 1000;

}


SocketClientLogic::~SocketClientLogic(void)
{
    UninitClientSocket();
  //  WSACleanup(); //不在客户端反初始化
}

SocketClientLogic* SocketClientLogic::GetSocketClientInstance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new SocketClientLogic();
    }
    return m_pInstance;
}

void SocketClientLogic::DestroySocketClientInstance()
{
    if (NULL != m_pInstance)
    {
        delete m_pInstance;
    }
}


bool SocketClientLogic::InitClientSocket(int NetType,int SocketType,int protocol )
{
    m_SocketClientFd = socket(NetType, SocketType, protocol);
    if (m_SocketClientFd == INVALID_SOCKET)
    {
        WSACleanup();
        return false;
    }
    struct timeval timeout={1,0};//3s

    setsockopt(m_SocketClientFd,SOL_SOCKET,SO_REUSEADDR,(char *)&timeout,sizeof(timeout));
    m_SocketStatus = SOCKET_FD_VALIDE;
    return true;
}

bool SocketClientLogic::ClientSocketConnect(const char* pServerIP,unsigned int port, const char* pClientIP,unsigned int iClientPort)
{

    SOCKADDR_IN ServerInfo;
    SOCKADDR_IN ClientInfo;
    memset(&ServerInfo, 0, sizeof(SOCKADDR_IN));
    memset(&ClientInfo, 0, sizeof(SOCKADDR_IN));
    ServerInfo.sin_family = AF_INET;
    ClientInfo.sin_family = AF_INET;
    // 连接到服务端
    m_SocketStatus = SOCKET_CONNECTING;
    
    m_strServerIP = pServerIP;
    m_ServerPort = port;
	if (!m_isinit)
	{
		m_strClientIP = pClientIP;//ip字符串不能为空
		m_ClientPort = iClientPort;
		m_isinit = true;
	}

    int iError = 0;
    if (NULL != pClientIP && 0 != iClientPort)
    {
        ClientInfo.sin_addr.s_addr = inet_addr(pClientIP);
        ClientInfo.sin_port = htons(iClientPort);
        iError = bind(m_SocketClientFd,(SOCKADDR*) &ClientInfo, sizeof(ClientInfo));
        if (0 > iError)
        {
            closesocket(m_SocketClientFd);
            m_SocketClientFd = INVALID_SOCKET;
            m_SocketStatus = SOCKET_DISCONNECT;
            return false;
        }
    }
    ServerInfo.sin_addr.s_addr = inet_addr(pServerIP);
    ServerInfo.sin_port = htons(port);
    if (connect(m_SocketClientFd, (SOCKADDR*) &ServerInfo, sizeof(ServerInfo) ) == SOCKET_ERROR)
    {
        closesocket(m_SocketClientFd);
        m_SocketClientFd = INVALID_SOCKET;
        m_SocketStatus = SOCKET_DISCONNECT;
        return false;
    }
    m_SocketStatus = SOCKET_CONNECTED;
    return true;
}

bool SocketClientLogic::ClientSocketConnect()
{
    if (m_SocketClientFd == INVALID_SOCKET)
    {
        return false;
    }
    if (m_strServerIP.length() >0 && 0 != m_ServerPort)
    {
        return ClientSocketConnect(m_strServerIP.c_str(),m_ServerPort);
    }
    return false;
}


int SocketClientLogic::ClientSocketRecevie(char* buffer,unsigned int len )
{
    int iRet = 0;

    fd_set fd;
    timeval tout;

    FD_ZERO(&fd);
    FD_SET(m_SocketClientFd,&fd);
    tout.tv_sec = 0;
    tout.tv_usec = 100; //微秒

    int iResult = select((int)m_SocketClientFd+1, &fd, NULL, NULL, &tout);
	iRet = WSAGetLastError();
    if (iResult < 0)
    {//disconnected
        m_SocketStatus = SOCKET_DISCONNECT;
        return -1;
    }

    if (0 == iResult)
    {//no msg
        return 0;
    }

    if (0 == FD_ISSET(m_SocketClientFd, &fd))  //判断套接字是否还在链表中
    {//disconnected
        m_SocketStatus = SOCKET_DISCONNECT;
        return -1;
    }
    iRet = recv(m_SocketClientFd,buffer,len,0);
    if (0 > iRet)
    {
        m_SocketStatus = SOCKET_DISCONNECT;
    }
    //select返回有数据但recv不到， 则认为接收异常退出
    if (iResult == 1 && iRet == 0)
    {
        m_SocketStatus = SOCKET_DISCONNECT;
        iRet = -1;
    }
    return iRet;
}

int SocketClientLogic::ClientSocketSend(const char* data,unsigned int len )
{
	int  OutTime = 0; 
    bool bconnected = false;
    bconnected = IsConnected();
    if (bconnected)
    {
        return send(m_SocketClientFd,data,len,0);
    }
    else
    {
        do 
        {
            bconnected = ClientSocketConnect();
            Sleep(50);
			OutTime += 50;
			if (OutTime > m_TimeOut)
			{
				break;
			}
        } while (!bconnected);
        
    }
    return -1;
}

bool SocketClientLogic::UninitClientSocket()
{
    if (INVALID_SOCKET != m_SocketClientFd)
    {
        //setsockopt(m_SocketClientFd,SOL_SOCKET,SO_ERROR,(char *)&optval,optlen);
        closesocket(m_SocketClientFd);
        m_SocketClientFd = INVALID_SOCKET;
        m_SocketStatus = SOCKET_FD_INVALIDE;
    }
    return true;
}

bool SocketClientLogic::IsConnected()
{
    bool bRet = false;
    if (SOCKET_CONNECTED == m_SocketStatus)
    {
        bRet = true;
    }
	//fd_set set;
	//unsigned long ul = 1;

	//timeval tm;
    int optval = -1;
    int optlen = sizeof(int); 
	//tm.tv_sec  = 3;
	//tm.tv_usec = 0;
	//FD_ZERO(&set);
	//int a =-1;
	//FD_SET(m_SocketClientFd, &set);
	//if( a =select(m_SocketClientFd+1, NULL, &set, NULL, &tm) < 0)
	//{
	//	m_SocketStatus = SOCKET_DISCONNECT;
	//	return false;

	//}
	//else if(a== 0)
	//{
	////	return false;
	//}
	if (getsockopt(m_SocketClientFd, SOL_SOCKET, SO_KEEPALIVE,(char*) &optval, &optlen) /*== SOCKET_ERROR */|| m_SocketClientFd == INVALID_SOCKET)
    //if (getsockopt(m_SocketClientFd, SOL_SOCKET, SO_ERROR,(char*) &optval, &optlen) /*== SOCKET_ERROR */|| m_SocketClientFd == INVALID_SOCKET)
    {
		//closesocket(m_SocketClientFd);
		//m_SocketClientFd = INVALID_SOCKET;
		//m_SocketStatus = SOCKET_FD_INVALIDE;
        m_SocketStatus = SOCKET_DISCONNECT;
        return false;
    }
    else if (0 == optval)
    {
        return true;
    }
    return bRet;
}
bool SocketClientLogic::ImgConnect(int sence)
{
	SOCKADDR_IN ServerInfo;
	SOCKADDR_IN ClientInfo;
	int iError = -1;
	memset(&ServerInfo, 0, sizeof(SOCKADDR_IN));
	memset(&ClientInfo, 0, sizeof(SOCKADDR_IN));
	ServerInfo.sin_family = AF_INET;
	ClientInfo.sin_family = AF_INET;

	
	if ((-1 == m_SenToSktID[sence]))//add new sence
	{
		SOCKET newsocket;
		m_SenToSktID[sence] = (int)m_vec_ImgSkt.size();
		m_vec_ImgSkt.push_back(newsocket);
		m_vec_ImgSktStus.push_back((int)INVALID_SOCKET);
		m_vec_ImgSkt[m_SenToSktID[sence]] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_vec_ImgSkt[m_SenToSktID[sence]] == INVALID_SOCKET)
		{
			WSACleanup();
			return false;
		}
		struct timeval timeout={1,0};//3s

		setsockopt(m_vec_ImgSkt[m_SenToSktID[sence]]+1,SOL_SOCKET,SO_REUSEADDR,(char *)&timeout,sizeof(timeout));
		m_vec_ImgSktStus[m_SenToSktID[sence]] = SOCKET_FD_VALIDE;
		ClientInfo.sin_addr.s_addr = inet_addr(m_strClientIP.c_str());
		ClientInfo.sin_port = htons(m_ClientPort+sence+10);
		iError = bind(m_vec_ImgSkt[m_SenToSktID[sence]],(SOCKADDR*) &ClientInfo, sizeof(ClientInfo));
		if (0 > iError)
		{
			closesocket(m_vec_ImgSkt[m_SenToSktID[sence]]);
			m_vec_ImgSkt[m_SenToSktID[sence]] = INVALID_SOCKET;
			m_vec_ImgSktStus[m_SenToSktID[sence]] = SOCKET_DISCONNECT;
			return false;
		}

		ServerInfo.sin_addr.s_addr = inet_addr(m_strServerIP.c_str());
		ServerInfo.sin_port = htons(m_ServerPort);
		if (connect(m_vec_ImgSkt[m_SenToSktID[sence]], (SOCKADDR*) &ServerInfo, sizeof(ServerInfo) ) == SOCKET_ERROR)
		{
			closesocket(m_vec_ImgSkt[m_SenToSktID[sence]]);
			m_vec_ImgSkt[m_SenToSktID[sence]] = INVALID_SOCKET;
			m_vec_ImgSktStus[m_SenToSktID[sence]] = SOCKET_DISCONNECT;
			return false;
		}
		m_vec_ImgSktStus[m_SenToSktID[sence]] = SOCKET_CONNECTED;
	}
	else
	{
		fd_set fd;
		timeval tout;

		FD_ZERO(&fd);
		FD_SET(m_vec_ImgSkt[m_SenToSktID[sence]],&fd);
		tout.tv_sec = 0;
		tout.tv_usec = 100; //微秒
		int iResult = select((int)m_vec_ImgSkt[m_SenToSktID[sence]]+1, &fd, NULL, NULL, &tout);
		if (iResult < 0)
		{//disconnected
			m_vec_ImgSktStus[m_SenToSktID[sence]] = SOCKET_DISCONNECT;
			return false;
		}

		if (0 == iResult)
		{//no msg
			return true;
		}
		if (0 == FD_ISSET(m_vec_ImgSkt[m_SenToSktID[sence]], &fd))  //判断套接字是否还在链表中
		{//disconnected
			m_vec_ImgSktStus[m_SenToSktID[sence]] = SOCKET_DISCONNECT;
			return false;
		}
	}
	
	return true;
}

int SocketClientLogic::ImgeSend(int sence,char* data,unsigned len)
{
	bool bconnected = false;
	bconnected = ImgConnect(sence);
	if (!bconnected)
	{
		return -1;
	}
	bconnected = IsConnected();	
	if (bconnected)
	{
		return send(m_vec_ImgSkt[m_SenToSktID[sence]],data,len,0);
	}
	else
	{
		do 
		{
			bconnected = ClientSocketConnect();
			Sleep(50);
		} while (!bconnected);

	}
	return -1;
}
