#include "skt.h"
#include "server.h"
#include "socket.h"

#include "GolobalError.h"
#include "common.h"


/*
 * Protocols
 */
#define IPPROTO_IP              0               /* dummy for IP */
#define IPPROTO_ICMP            1               /* control message protocol */
#define IPPROTO_IGMP            2               /* group management protocol */
#define IPPROTO_GGP             3               /* gateway^2 (deprecated) */
#define IPPROTO_TCP             6               /* tcp */                     ////需要这个
#define IPPROTO_PUP             12              /* pup */
#define IPPROTO_UDP             17              /* user datagram protocol */
#define IPPROTO_IDP             22              /* xns idp */
#define IPPROTO_ND              77              /* UNOFFICIAL net disk proto */

#define IPPROTO_RAW             255             /* raw IP packet */


CServer::CServer()
{
    m_ucPort = 0;
    memset(m_szClient, 0, MAX_IP_LEN);
}

int CServer::Init(CListen* pListen, const char* local, unsigned short lport)
{//保证pListen非空
    m_pListen = pListen;
    if (local && *local)
    {//若传入IP非空, 则需要检查合法性
        if (!chk_ipaddress(local))
        {
            LOG_ERR("%s must is ip address.", local);
            return ERR_INVALID_IP;
        }
    }

    strcpy_s(m_szClient, (local && *local) ? local : ANY_IP);
    m_ucPort = lport;
    SetRemoteIP(m_szClient, lport);
    Connect();
    if (SOCKET_DISCONNECTED == GetStatus())
    {
        RunThreadConnect();
    }
    LOG_INF1("CServer::Init('%s', %d)", m_szClient, lport);
    return RETURN_OK;
}

int CServer::WaitConn()
{
    int ret = 0;
    ST_SOCKADDR addr;

    RETURN_CHK(m_pListen, INVALID_SKT);

    strcpy_s(addr.ip, m_szClient);
    addr.port = m_ucPort;
    ret = m_pListen->WaitConnect(addr);
    if (!ret)
    {
        SetRemoteIP(addr.ip, addr.port);
        if (strcmp(m_szClient, ANY_IP) && (GetModel() & EMSKT_RECONN_CHKIP))
        {//传入通用IP，且重连检测IP
            strcpy_s(m_szClient, addr.ip);
        }
        if (0 == m_ucPort && (GetModel() & EMSKT_RECONN_CHKPORT))
        {//传入通用端口，且重连检测端口
            m_ucPort = addr.port;
        }
    }
    return ret ? INVALID_SKT : addr.s;
}

CListen::CListen()
{
    m_usPort = 0;
    m_bBind = false;
    m_sktListen = INVALID_SKT;
    memset(m_szServer, 0, MAX_IP_LEN);
    m_hSection = sys_InitializeCriticalSection();
}

CListen::~CListen(void)
{
    vector<ST_SOCKADDR*>::iterator it;

    sys_EnterCriticalSection(m_hSection);
    sys_CloseSocket(m_sktListen);
    m_sktListen = INVALID_SKT;
    for(it = m_vecPTP.begin(); it != m_vecPTP.end(); it++)
    {
        sys_CloseSocket((*it)->s);
        delete *it;
    }
    m_vecPTP.clear();
    sys_LeaveCriticalSection(m_hSection);
    sys_DeleteCriticalSection(m_hSection);
    LOG_INF1("a server is free. 0x%x", this);
}

int CListen::WaitConnect(ST_SOCKADDR& addr)
{//等待连接
    ST_SOCKADDR* ptmp = NULL;

    RETURN_CHK(INVALID_SKT != m_sktListen, ERR_INVALID_HANDLE);

    if (NULL != (ptmp = GetClient(addr.ip, addr.port)))
    {//首先在队列里面查找
        memcpy(&addr, ptmp, sizeof(ST_SOCKADDR));
        return RETURN_OK;
    }

    RETURN_CHK_NOPRT(!AcceptClient(), ERR_TIMEOUT);   //等待连接

    if (NULL != (ptmp = GetClient(addr.ip, addr.port)))
    {//在队列里面查找
        memcpy(&addr, ptmp, sizeof(ST_SOCKADDR));
        return RETURN_OK;
    }

    return ERR_TIMEOUT;
}

ST_SOCKADDR* CListen::GetClient(const char* lpClient, unsigned short uPort)
{
    ST_SOCKADDR* ptmp = NULL;
    vector<ST_SOCKADDR*>::iterator it;

    sys_EnterCriticalSection(m_hSection);
    for(it = m_vecPTP.begin(); it != m_vecPTP.end(); it++)
    {
        if (0 != strcmp(ANY_IP, lpClient))
        {//若传空,则只要有就适合
            if (0 != strcmp(lpClient, (*it)->ip))
            {
                continue;
            }
        }

        if (uPort != (*it)->port && 0 != uPort)
        {//port=0 表示不检测端口
            continue;
        }

        ptmp = (*it);
        LOG_INF1("Find a client. ip=%s,port=%d", (*it)->ip, (*it)->port);
        m_vecPTP.erase(it);     //从等待队列删除
        break;                  //返回通讯指针
    }
    sys_LeaveCriticalSection(m_hSection);
    return ptmp;
}

//外部加锁
int CListen::CreateSocket()
{
    int ret = 0;

    RETURN_CHK_NOPRT(INVALID_SKT == m_sktListen, RETURN_OK);

    if (0 != strcmp(ANY_IP, m_szServer))
    {
        ret = sys_ChkNet(m_szServer);
    }

    RETURN_CHK(!ret, ret);

    m_bBind = false;    //重新创建socket后需要绑定端口
    m_sktListen = sys_Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SKT == m_sktListen)
    {
        ret = ERR_CREATE_SOCKET;
        LOG_ERR("CreateSocket sys_Socket return error.%d", ret);
    }
    return ret;
}

//外部加锁
int CListen::ListenSocket()
{
    int ret = 0;
    const char* lpServer = NULL;

    RETURN_CHK_NOPRT(!m_bBind, RETURN_OK);

    if (0 != strcmp(m_szServer, ANY_IP))
    {//通用IP，则使用空去绑定
        lpServer = m_szServer;
    }

    if (ret = sys_Bind(m_sktListen, lpServer, m_usPort))
    {
        LOG_ERR("CListen::ListenSocket(sys_Bind) return error.%d", ret);
        return ret;
    }

    if (ret = sys_Listen(m_sktListen, 5))
    {//listen error
        LOG_ERR("CListen::ListenSocket(sys_listen) return error..", ret);
        return ret;
    }

    m_bBind = true;
    return RETURN_OK;
}

int CListen::AcceptClient()
{
    int ret = 0;
    ST_SOCKADDR stAddr;
    ST_SOCKADDR* pAddr = NULL;
    const char* lpServer = NULL;

    if (0 != strcmp(m_szServer, ANY_IP))
    {
        lpServer = m_szServer;
    }

    sys_EnterCriticalSection(m_hSection);
    do
    {
        ret = CreateSocket();
        if (ret)
        {
            break;
        }

        ret = ListenSocket();
        if (ret)
        {
            break;
        }

        ret = sys_Accept(m_sktListen, &stAddr, 50);
        if (0 == ret)
        {
            pAddr = new ST_SOCKADDR;
            memcpy(pAddr, &stAddr, sizeof(ST_SOCKADDR));
            m_vecPTP.push_back(pAddr);
            LOG_INF1("Find a new connection.ip=%s,port=%d,socket=%d", pAddr->ip, pAddr->port, pAddr->s);
        }
        else if (ERR_TIMEOUT != ret)
        {//关闭socket
            sys_CloseSocket(m_sktListen);
            m_sktListen = INVALID_SKT;
        }
    } while (0);
    sys_LeaveCriticalSection(m_hSection);
    return ret;
}

int CListen::Init(const char* lpServer, unsigned short usPort)
{//保证lpServer 非空
    int ret = 0;

    sys_EnterCriticalSection(m_hSection);
    do
    {
        if (!ChkAddressValid(lpServer))
        {
            LOG_ERR("this server ip invalid.");
            ret = ERR_INVALID_IP;
            break;
        }
        m_usPort = usPort;
        strcpy_s(m_szServer, lpServer);

        ret = CreateSocket();
        if (ret)
        {
            break;
        }

        ret = ListenSocket();
        if (ret)
        {
            break;
        }

        LOG_INF1("CListen::Init Success ip=%s, port=%d", m_szServer, m_usPort);
    } while (0);
    sys_LeaveCriticalSection(m_hSection);
    return ret;
}

bool CListen::ChkAddressValid(const char* ip)
{
    int nCnt = 0;
    char ips[MAX_BUF_LEN] = {0};
    const char* ptmp = ips;

    RETURN_CHK(strcmp(ip, ANY_IP), true);
    RETURN_CHK(chk_ipaddress(ip), false);

    nCnt = sys_local_ips(ips);
    if (0 == nCnt)
    {
        LOG_ERR("no valid ip..");
        return false;
    }

    for(int i = 0; i < nCnt; i++)
    {
        if (0 == strcmp(ip, ptmp))
        {
            return true;
        }
        ptmp += strlen(ptmp) + 1;
    }
    LOG_ERR("the ip invalid..");
    return false;
}

void CListen::PushHandle(int skt)
{
    sys_EnterCriticalSection(m_hSection);
    m_vecskt.push_back(skt);
    sys_LeaveCriticalSection(m_hSection);
}

void CListen::PopupHandle(int skt)
{
    vector<int>::iterator it;
    sys_EnterCriticalSection(m_hSection);
    for(it = m_vecskt.begin(); it != m_vecskt.end(); it++)
    {
        if (*it == skt)
        {
            m_vecskt.erase(it);
            break;
        }
    }
    sys_LeaveCriticalSection(m_hSection);
}