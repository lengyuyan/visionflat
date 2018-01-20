#include "GlobalDefine.h"
#include "GolobalError.h" 
#include "sys_internal.h"
#include "sysapi.h"

#define PING_TIMES 3
#define LOCAL_IP "127.0.0.1"

bool    g_bInit = false;        //是否已经初始化网络 这种全局变量要加锁!!!!

int WINAPI sys_InitNet()
{
    int ret = 0;
    WSADATA wsaData;

    RETURN_CHK_NOPRT(!g_bInit, RETURN_OK);

    if (ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) // 加载Ws2_32.dll
    {
        TRACEF("WSAStartup return error:%d", ret);
        WSACleanup();
        return ERR_INIT_NET;
    }

    if (2 != LOBYTE(wsaData.wVersion) || 2 != HIBYTE(wsaData.wVersion)) 
    {
        WSACleanup();
        return ERR_VERSION;
    }

    g_bInit = true;
    return RETURN_OK;
}

void WINAPI sys_UninitNet()
{
    ZERO_CHK(g_bInit);

    g_bInit = false;
    WSACleanup();
}

int WINAPI sys_ChkNet(const char* lpClient)
{
    int PingTimes = 0;
    Handle hndlFile = NULL;     // Handle for IcmpCreateFile()

    RETURN_CHK_NOPRT(g_bInit, ERR_NOINIT_NET);
    RETURN_CHK_NOPRT(lpClient, ERR_INPUT_PARAM);

    IPAddr pAddr = (IPAddr)inet_addr(lpClient);
    icmp_echo_reply pData;

    hndlFile = IcmpCreateFile();    // Get an ICMP echo request handle
    while(PingTimes < PING_TIMES)
    {
        PingTimes++;
        IcmpSendEcho(hndlFile, pAddr, NULL, 0, NULL, &pData, sizeof(pData), 200);
        if (IP_SUCCESS != pData.Status)
        {
            continue;
        }
        IcmpCloseHandle(hndlFile);
        return RETURN_OK;    //ping suucess
    }

    IcmpCloseHandle(hndlFile);
    return ERR_CLIENT_NORESPONSE;   //ping fail.
}

int WINAPI sys_Socket(int af, int type, int protocol)
{
    SOCKET skt;

    RETURN_CHK_NOPRT(g_bInit, INVALID_SKT);

    skt = socket(af, type, protocol);
    if (INVALID_SOCKET == skt)
    {
        TRACEF("socket() return error:%d", WSAGetLastError());
        return INVALID_SKT;
    }

    u_long iMode = 1; //设置为非阻塞
    if (ioctlsocket(skt, FIONBIO, &iMode) < 0)
    {
        //perror("ioctlsocket error\n");
        return INVALID_SKT;
    }

    return (int)skt;
}

int WINAPI sys_Bind(int s, const char* lpIP, ushort uPort)
{
    SOCKADDR_IN addr;

    RETURN_CHK_NOPRT(g_bInit, ERR_NOINIT_NET);
    RETURN_CHK_NOPRT(INVALID_SKT != s && uPort, ERR_INPUT_PARAM);

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
    if (ioctlsocket(s, FIONBIO, &iMode))
    {
        TRACEF("ioctlsocket() return error:%d", WSAGetLastError());
        return ERR_BIND_SOCKET;
    }

    int opt = 1; //设置可重用端口(server,sport,client,cport)4者有一个不同,即可用SO_REUSEADDR
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt,sizeof(opt)))
    {
        TRACEF("setsockopt return error:%d", WSAGetLastError());
        return ERR_BIND_SOCKET;
    }

    if (bind(s,(SOCKADDR *)&addr, sizeof(addr)))
    {
        TRACEF("bind return error:%d", WSAGetLastError());
        return ERR_BIND_SOCKET;
    }

    iMode = 1; //设置为非阻塞
    if (ioctlsocket(s, FIONBIO, &iMode) < 0)
    {
        //perror("ioctlsocket error\n");
        return ERR_BIND_SOCKET;
    }

    return RETURN_OK;
}

int WINAPI sys_Listen(int s, int maxCnt)
{
    RETURN_CHK_NOPRT(g_bInit, ERR_NOINIT_NET);
    RETURN_CHK_NOPRT(INVALID_SKT != s && maxCnt > 0, ERR_INPUT_PARAM);

    if (listen(s, maxCnt))
    {
        TRACEF("listen return error:%d", WSAGetLastError());
        return ERR_LISTEN_SKT;
    }
    return RETURN_OK;
}

void WINAPI sys_CloseSocket(int s)
{
    ZERO_CHK(g_bInit);
    ZERO_CHK(INVALID_SKT != s);

    closesocket(s);
}

int WINAPI sys_Accept(int s, ST_SOCKADDR* pclient, int timeout)
{
    fd_set fd;
    timeval tout;
    timeval* ptime = &tout;
    SOCKADDR_IN addr;
    int length = sizeof(SOCKADDR_IN);

    RETURN_CHK_NOPRT(g_bInit, ERR_NOINIT_NET);
    RETURN_CHK_NOPRT(INVALID_SKT != s && pclient, ERR_INPUT_PARAM);

    ZeroMemory(&addr, sizeof(addr));
    FD_ZERO(&fd);
    FD_SET(s, &fd);
    ptime->tv_sec = 0;
    ptime->tv_usec = timeout*1000;//500毫秒

    if (MAX_TIMEOUT == timeout)
    {//阻塞模式, 退出程序前,先关闭socket
        ptime = NULL;
    }

    if (select(s + 1, &fd, NULL, NULL, ptime) <= 0)
    {
        TRACEF("select() return error:%d", WSAGetLastError());
        return ERR_TIMEOUT;
    }

    //如果是服务器可读，就是有socket连入
    if (FD_ISSET(s, &fd) <= 0)
    {
        return ERR_ACCEPT_SKT;
    }

    SOCKET sock = accept(s, (sockaddr *)&addr, &length);
    if (INVALID_SOCKET == sock)
    {
        TRACEF("accept() return error:%d", WSAGetLastError());
        return ERR_ACCEPT_SKT;
    }

    pclient->s = (int)sock;
    pclient->port = ntohs(addr.sin_port);
    strcpy_s(pclient->ip, MAX_IP_LEN, inet_ntoa(addr.sin_addr));
    return RETURN_OK;
}

int WINAPI sys_Connect(int s, const char* lpServer, ushort uPort)
{
    fd_set fd;
    int ret = 0;
    timeval tout;
    SOCKADDR_IN addr;

    RETURN_CHK_NOPRT(g_bInit, ERR_NOINIT_NET);
    RETURN_CHK_NOPRT(INVALID_SKT != s && lpServer && uPort, ERR_INPUT_PARAM);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(lpServer);
    addr.sin_port = htons(uPort);
    if (-1 == connect(s, (SOCKADDR*) &addr, sizeof(addr)))
    {
        //return ERR_CONNECT_SKT;
        FD_ZERO(&fd);
        FD_SET(s, &fd);
        tout.tv_sec = 0;
        tout.tv_usec = 50*1000;
        ret = select(s + 1, NULL, &fd, NULL, &tout);
        if (ret < 0)
        {
            //perror("connect error\n");
            return ERR_CONNECT_SKT;
        }
        else if (0 == ret)
        {
            //perror("timeout error\n");
            return ERR_TIMEOUT;
        }
        else
        {
            int error = -1;
            int optLen = sizeof(int);
            getsockopt(s, SOL_SOCKET, SO_ERROR, (char*)&error, &optLen);
            if (0 != error)
            {
                return ERR_CONNECT_SKT; // 有错误
            }
        }
    }
    return 0;
}

int WINAPI sys_Recv(int s, char* buf, int len)
{
    int recvLen = 0;
    int option = 0; //接收选项
    char tmp = 0;

    RETURN_CHK_NOPRT(g_bInit, ERR_NOINIT_NET);
    RETURN_CHK_NOPRT(INVALID_SKT != s && len >= 0, ERR_INPUT_PARAM);

    if (0 == len)
    {//if equal 0 then check connect status
        option = MSG_PEEK;
        len = 1;
        buf = &tmp;
    }

    RETURN_CHK_NOPRT(buf, ERR_INPUT_PARAM);

    recvLen = recv(s, buf, len, option);
    if (recvLen <= 0)
    {//连接已经断开
        TRACEF("recv() return error:%d", WSAGetLastError());
        return ERR_NO_CONNECT;
    }
    return recvLen;
}

int WINAPI sys_Select(int* s, int count, int timeout)
{
    fd_set fd;
    timeval tout;
    int index = 0;

    RETURN_CHK_NOPRT(g_bInit, ERR_NOINIT_NET);
    RETURN_CHK_NOPRT(s && count > 0 && count <= FD_SETSIZE, ERR_INPUT_PARAM);

    FD_ZERO(&fd);
    for(index = 0; index < count; index++)
    {//添加到队列
        FD_SET(s[index], &fd);
    }

    tout.tv_sec = 0;
    tout.tv_usec = timeout * 1000; //微秒
    int iResult = select(s[count - 1] + 1, &fd, NULL, NULL, 
        (MAX_TIMEOUT == timeout) ? NULL : &tout);
    //无限等待时,先关闭socket才能退出
    if (iResult < 0)
    {//disconnected
        return ERR_NO_CONNECT;
    }
    else if (0 == iResult)
    {//超时
        return ERR_TIMEOUT;
    }

    for(index = 0; index < count; index++)
    {
        if (FD_ISSET(s[index], &fd))
        {
            return index;
        }
    }
    return ERR_NO_CONNECT;
}

int WINAPI sys_Send(int s, const char* buf, int len)
{
    int sendLen = 0;
    fd_set fd;
    timeval tout;

    RETURN_CHK_NOPRT(g_bInit, ERR_NOINIT_NET);
    RETURN_CHK_NOPRT(INVALID_SKT != s && buf && len > 0, ERR_INPUT_PARAM);

    FD_ZERO(&fd);
    FD_SET(s, &fd);
    tout.tv_sec = 0;
    tout.tv_usec = 500 * 1000; //微秒

    int iResult = select(s + 1, NULL, &fd, NULL, &tout);
    if (iResult < 0)
    {//disconnected
        return ERR_NO_CONNECT;
    }
    else if (0 == iResult)
    {//超时
        return ERR_TIMEOUT;
    }

    sendLen = send(s, buf, len, 0);
    if (sendLen < 0 || (len > 0 && 0 == sendLen))
    {//连接已经断开
        TRACEF("send() return error:%d", WSAGetLastError());
        return ERR_NO_CONNECT;
    }
    return sendLen;
}

int WINAPI sys_GetSockName(int s, ST_SOCKADDR* paddr)
{
    SOCKADDR_IN addr;
    int len = sizeof(SOCKADDR_IN);

    RETURN_CHK_NOPRT(g_bInit, ERR_NOINIT_NET);
    RETURN_CHK_NOPRT(INVALID_SKT != s && paddr, ERR_INPUT_PARAM);

    memset(&addr, 0, sizeof(addr));
    if (SOCKET_ERROR == getsockname(s, (sockaddr*)&addr, &len))
    {
        TRACEF("getsockname() return error:%d", WSAGetLastError());
        return ERR_GET_SKTINFO;
    }
    paddr->port = ntohs(addr.sin_port);
    strcpy_s(paddr->ip, MAX_IP_LEN, inet_ntoa(addr.sin_addr));
    paddr->s = s;
    return RETURN_OK;
}

int WINAPI sys_local_ips(char* ips)
{
    char hostname[256] = {0};
    char* ptmp = ips;
    int nCnt = 1;

    RETURN_CHK_NOPRT(g_bInit, ERR_NOINIT_NET);
    RETURN_CHK_NOPRT(ips, ERR_INPUT_PARAM);

    strcpy_s(ips, MAX_IP_LEN, LOCAL_IP);
    ips += strlen(ips) + 1;

    if (SOCKET_ERROR == gethostname(hostname, sizeof(hostname)))
    {
        TRACEF("gethostname() return error:%d", WSAGetLastError());
        return nCnt;
    }

    HOSTENT* pHost = gethostbyname(hostname);
    if (!pHost)
    {
        TRACEF("gethostbyname() return error:%d", WSAGetLastError());
        return nCnt;
    }

    for (int i = 0; i < pHost->h_length; i++)
    {
        in_addr* addr = (in_addr*)*pHost->h_addr_list;
        strcpy_s(ips, MAX_IP_LEN, inet_ntoa(addr[i]));
        ips += strlen(ips) + 1;
    }
    return pHost->h_length + 1;
}