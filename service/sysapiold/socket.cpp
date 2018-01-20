
#define _NO_INCLUDE_SYSAPI  //此处包含windows头文件,所以不用自定义类型

#include <windows.h>
#include <IPHlpApi.h>
#include <IcmpAPI.h>
#include "sysapi.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")

#define PING_TIMES 3

bool WINAPI sys_ChkNetAsICMP(LPCSTR lpIP)
{
    int PingTimes = 0;
    //unsigned long Time = 0;   // microsecond

    HANDLE hndlFile = NULL;     // HANDLE for IcmpCreateFile()
    //LARGE_INTEGER litmp;

    if(!lpIP)
    {
        return false;
    }

    //QueryPerformanceFrequency(&litmp);
    //LONGLONG dfFreq = litmp.QuadPart; //获得计数器的时钟频率
    IPAddr pAddr = (IPAddr)inet_addr(lpIP);
    icmp_echo_reply pData;

    hndlFile = IcmpCreateFile();    // Get an ICMP echo request handle
    //LONGLONG QPartS, QPartE;
    while(PingTimes < PING_TIMES)
    {
        Sleep(500);

        PingTimes++;

        //QueryPerformanceCounter(&litmp);
        //QPartS = litmp.QuadPart; // start time value

        IcmpSendEcho(hndlFile, pAddr, NULL, 0, NULL, &pData, sizeof(pData), 1000);

        //QueryPerformanceCounter(&litmp);
        //QPartE = litmp.QuadPart; // end time value


        if (IP_SUCCESS != pData.Status)
        {
            continue;
        }

        //Time = (unsigned long)((QPartE - QPartS)*1000000/dfFreq);
        IcmpCloseHandle(hndlFile);

        return true;    //ping suucess
    }

    IcmpCloseHandle(hndlFile);
    return false;       //ping fail.
}

bool WINAPI sys_WSAStartup()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) // 加载Ws2_32.dll
    {
        WSACleanup();
        return false;
    }

	if ( LOBYTE( wsaData.wVersion ) != 2 ||HIBYTE( wsaData.wVersion ) != 2 ) 
	{
		WSACleanup();
		return false;
	}


    return true;
}

void WINAPI sys_WSACleanup()
{
    WSACleanup();
}

SOCKET WINAPI sys_socket(int af, int type, int protocol)
{
    return socket(af, type, protocol);
}

int WINAPI sys_bind(SOCKET s, LPCSTR lpIP, u_short uPort)
{
    SOCKADDR_IN	addr;

    ZeroMemory(&addr, sizeof(addr));
    addr.sin_family = AF_INET;//SOCK_DGRAM
    if (NULL != lpIP)
    {
        addr.sin_addr.s_addr = inet_addr(lpIP);
    }
    else
    {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    // 填充 端口号
    addr.sin_port = htons(uPort);

    u_long iMode = 0; //绑定socket时设置为阻塞
    if (SOCKET_ERROR == ioctlsocket(s, FIONBIO, &iMode))
    {
        return SOCKET_ERROR;
    }

    int opt = 1; //设置可重用端口(server,sport,client,cport)4者有一个不同,即可用SO_REUSEADDR
    if (SOCKET_ERROR == setsockopt(s,SOL_SOCKET ,SO_REUSEADDR, (const char*)&opt,sizeof(opt)))
    {
        return SOCKET_ERROR;
    }

    return bind(s,(SOCKADDR *)&addr, sizeof(addr));
}

int WINAPI sys_listen(SOCKET s, int backlog)
{
    return listen(s, backlog);
}

int WINAPI sys_closesocket(SOCKET s)
{
    return closesocket(s);
}

bool WINAPI sys_accept(SOCKET s, ST_SOCKADDR* pclient, int timeout)
{
	if (!pclient)
	{
		return false;
	}

    fd_set fd;
    timeval tout;
    timeval* ptime = &tout;

    SOCKADDR_IN	addr;
    int length = sizeof(SOCKADDR_IN);

    ZeroMemory(&addr, sizeof(addr));
    FD_ZERO(&fd);
    FD_SET(s,&fd);
    ptime->tv_sec = 0;
    ptime->tv_usec = timeout*1000;//500毫秒

    if (INFINITE == timeout)
    {//阻塞模式, 退出程序前,先关闭socket
        ptime = NULL;
    }

    if (select((int)s + 1, &fd, NULL, NULL, ptime) <= 0)
    {
        return false;
    }

    // 如果是服务器可读，就是有socket连入
    if (FD_ISSET(s, &fd) <= 0)
    {
        return false;
    }

    SOCKET sock = accept(s, (sockaddr *)&addr, &length);
    if (INVALID_SOCKET == sock)
    {
        return false;
    }

    pclient->s = sock;
    pclient->port = ntohs(addr.sin_port);
    strcpy_s(pclient->ip, MAX_IP_LEN, inet_ntoa(addr.sin_addr));
    return true;
}

int WINAPI sys_recv(SOCKET s, LPSTR buf, int len, int timeout)
{
    fd_set fd;
    timeval tout;
    timeval* ptime = &tout;

    FD_ZERO(&fd);
    FD_SET(s,&fd);
    ptime->tv_sec = 0;
    ptime->tv_usec = timeout*1000; //微秒

    if (INFINITE == timeout)
    {//阻塞模式, 退出程序前,先关闭socket
        ptime = NULL;
    }

    int iResult = select((int)s+1, &fd, NULL, NULL, ptime);
    if (iResult < 0)
    {//disconnected
        return 0;
    }

    if (0 == iResult)
    {//no msg
        return WSA_NO_MSG;
    }

    if (0 == FD_ISSET(s, &fd))  //判断套接字是否还在链表中
    {//disconnected
        return 0;
    }
    return recv(s, buf, len, 0);
}

int WINAPI sys_connect(SOCKET s, LPCSTR lpServer, u_short uPort)
{
    SOCKADDR_IN addr;
    if (!lpServer)
    {
        return SOCKET_ERROR;
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(lpServer);
    addr.sin_port = htons(uPort);
    return connect(s, (SOCKADDR*) &addr, sizeof(addr));
}

int WINAPI sys_send(SOCKET s, LPCSTR buf, int len, int flags)
{
    return send(s, buf, len, flags);
}

int WINAPI sys_WSAGetLastError()
{
	return WSAGetLastError();
}

int WINAPI sys_shutdown(SOCKET s, int how)
{
    return shutdown(s, how);
}

int WINAPI sys_getsockname(SOCKET s, ST_SOCKADDR* paddr)
{
    if (!paddr)
    {
        return SOCKET_ERROR;
    }
    SOCKADDR_IN	addr;
    int len = sizeof(SOCKADDR_IN);
    memset(&addr, 0, sizeof(addr));
    int ret = getsockname(s, (sockaddr*)&addr, &len);
    if (SOCKET_ERROR == ret)
    {
        return SOCKET_ERROR;
    }
    paddr->port = ntohs(addr.sin_port);
    strcpy_s(paddr->ip, MAX_IP_LEN, inet_ntoa(addr.sin_addr));
    paddr->s = s;
    return 0;
}