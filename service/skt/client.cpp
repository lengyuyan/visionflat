#include <string>
#include "skt.h"
#include "socket.h"
#include "Client.h"

#include "GolobalError.h"
#include "common.h"

#define IPPROTO_TCP             6               /* tcp */   

CClient::CClient()
{
    m_lPort = 0;
    m_sktTmp = INVALID_SKT;
    memset(m_szLocal, 0, sizeof(m_szLocal));
}

CClient::~CClient()
{
    Close();
    if (INVALID_SKT != m_sktTmp)
    {
        sys_CloseSocket(m_sktTmp);
    }
    LOG_INF1("CClient free 0x%x", this);
}

int CClient::Init(const char* lpServer, unsigned short sport, const char* lpLocal, unsigned short lport)
{
    int ret = 0;

    LOG_INF1("CClient Init. server=%s, sport=%d, ip=%s,port=%d", lpServer, sport, lpLocal ? lpLocal : "", lport);

    if (lpLocal && *lpLocal)
    {//若传入IP非空, 则需要检查合法性
        if (!chk_ipaddress(lpLocal))
        {
            LOG_ERR("%s must is ip address.", lpLocal);
            return ERR_INVALID_IP;
        }
    }

    m_lPort = lport;
    strcpy_s(m_szLocal, (lpLocal && *lpLocal) ? lpLocal : ANY_IP);

    ret = CreateClientSocket();
    RETURN_CHK(!ret, ret);

    SetRemoteIP(lpServer, sport);
    Connect();
    if (SOCKET_DISCONNECTED == GetStatus())
    {
        RunThreadConnect();
    }
    return RETURN_OK;
}

int CClient::WaitConn()
{//连接服务器
    int ret = 0;
    char szSrv[MAX_IP_LEN] = {0};
    unsigned short usport = 0;

    ret = CreateClientSocket();
    RETURN_CHK(!ret, INVALID_SKT);

    GetAddr(szSrv, usport);
    LOG_INF3("sys_Connect(%s:%d) start.", szSrv, usport);
    ret = sys_Connect(m_sktTmp, szSrv, usport);
    LOG_INF3("sys_Connect(%s:%d) finished.", szSrv, usport);
    if (RETURN_OK == ret)
    {//ok
        LOG_INF1("CClient::WaitConn() success.");
        ret = m_sktTmp;
        m_sktTmp = INVALID_SKT;
        return ret; //返回有效句柄
    }

    if (ERR_TIMEOUT != ret)
    {//非超时，要关闭
        sys_CloseSocket(m_sktTmp);
        m_sktTmp = INVALID_SKT;
    }
    sys_Sleep(50);
    return INVALID_SKT;
}

int CClient::CreateClientSocket()
{
    int ret = 0;

    RETURN_CHK_NOPRT(INVALID_SKT == m_sktTmp, RETURN_OK);

    m_sktTmp = sys_Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SKT == m_sktTmp)
    {
        ret = ERR_CREATE_SOCKET;
        LOG_ERR("CreateSocket sys_Socket return error.%d", ret);
    }

    if (m_lPort && 0 != strcmp(m_szLocal, ANY_IP))
    {
        ret = sys_Bind(m_sktTmp, m_szLocal, m_lPort);
    }

    return ret;
}