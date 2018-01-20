#include "ISocket.h"
#include "common.h"
#include "skt.h"
#include "server.h"

#include "GolobalError.h"

ISocket::ISocket(void)
{
    m_usPort = 0;
    m_bRun = false;
    m_pListen = NULL;
    m_emWorkModel = 0;
    m_socket = INVALID_SKT;
    memset(m_szIP, 0, MAX_IP_LEN);
    m_iStatus = SOCKET_DISCONNECTED;
    m_hSection = sys_InitializeCriticalSection();
    m_hQuitSign = sys_CreateEvent(TRUE, TRUE, NULL);
}

ISocket::~ISocket(void)
{
    Close();
    if (INVALID_SKT != m_socket)
    {
        sys_CloseSocket(m_socket);
    }
    sys_EnterCriticalSection(m_hSection);
    sys_DeleteCriticalSection(m_hSection);
    sys_CloseHandle(m_hQuitSign);
}

void ISocket::Close()
{
    ZERO_CHK(m_bRun);

    m_bRun = false;
    sys_WaitForSingleObject(m_hQuitSign);
}

int ISocket::GetStatus()
{
    int ret = 0;
    if (SOCKET_CONNECTED == m_iStatus)
    {
        RecvEx(0, 0, 0);
    }
    return m_iStatus;
}

int ISocket::Send(const char* pData, int len)
{
    int ret = 0;

    RETURN_CHK(pData && len > 0, ERR_INPUT_PARAM);
    RETURN_CHK(INVALID_SKT != m_socket, ERR_INVALID_HANDLE);

    LOG_INF3("socket:%d send len=%d", m_socket, len);
    sys_EnterCriticalSection(m_hSection);
    ret = sys_Send(m_socket, pData, len);
    if (ret > 0)
    {
        if (ret != len)
        {
            LOG_ERR("socket:%d send len=%d ret=%d", m_socket, len, ret);
            ret = ERR_SEND_MSG;
        }
    }
    else if (ERR_NO_CONNECT == ret)
    {//断开
        ChangeStatus(SOCKET_DISCONNECTED);
    }
    sys_LeaveCriticalSection(m_hSection);
    return ret;
}

int ISocket::RecvEx(char* pData, int len, int timeout)
{
    int ret = 0;

    ret = sys_Select(&m_socket, 1, timeout);
    if (ERR_NO_CONNECT == ret)
    {//断开
        ChangeStatus(SOCKET_DISCONNECTED);
    }

    RETURN_CHK_NOPRT(ret >= 0, ret);

    sys_EnterCriticalSection(m_hSection);
    ret = sys_Recv(m_socket, pData, len);
    if (ret > 0)
    {
        LOG_INF3("socket:%d recv len=%d", m_socket, ret);
    }
    else if (ERR_NO_CONNECT == ret)
    {//断开
        ChangeStatus(SOCKET_DISCONNECTED);
    }
    sys_LeaveCriticalSection(m_hSection);
    return ret;
}

int ISocket::Recv(char* pData, int len, int timeout)
{
    int ret = 0;

    RETURN_CHK(pData && len > 0, ERR_INPUT_PARAM);
    RETURN_CHK(INVALID_SKT != m_socket, ERR_INVALID_HANDLE);

    memset(pData, 0, len);
    return RecvEx(pData, len, timeout);
}

int ISocket::ManualConn()
{
    int ret = 0;

    RETURN_CHK_NOPRT(SOCKET_DISCONNECTED == m_iStatus, 0);

    sys_EnterCriticalSection(m_hSection);
    if (INVALID_SKT != m_socket)
    {
        sys_CloseSocket(m_socket);
        m_socket = INVALID_SKT;
    }
    ret = Connect();
    sys_LeaveCriticalSection(m_hSection);
    return ret;
}

int ISocket::Reconn()
{
    sys_EnterCriticalSection(m_hSection);
    if (INVALID_SKT != m_socket)
    {
        sys_CloseSocket(m_socket);
        m_socket = INVALID_SKT;
    }
    ChangeStatus(SOCKET_DISCONNECTED);
    RunThreadConnect();
    sys_LeaveCriticalSection(m_hSection);
    return RETURN_OK;
}

int ISocket::SetModel(int nCode)
{
    RETURN_CHK(nCode >= 0 && nCode < (EMSKT_NO_AUTOCONN<<1), ERR_INPUT_PARAM);
    RETURN_CHK_NOPRT(m_emWorkModel != nCode, 0);

    sys_EnterCriticalSection(m_hSection);
    m_emWorkModel = nCode;
    sys_LeaveCriticalSection(m_hSection);
    if (EMSKT_NO_AUTOCONN & nCode && m_bRun)
    {//设置为不自动连接,且正在连接，则退出连接
        m_bRun = false;
        sys_WaitForSingleObject(m_hQuitSign);
    }
    return RETURN_OK;
}

void ISocket::ClearMsgList()
{
    char buff[MAX_BUF_LEN] = {0};
    LOG_INF1("socket:%d ClearMsgList", m_socket);
    while(Recv(buff, MAX_BUF_LEN, 0) > 0);
}

void ISocket::GetAddr(char* lpRemote, unsigned short& usport)
{
    if (lpRemote)
    {
        comm_strcpy(lpRemote, m_szIP);
    }
    usport = m_usPort;
}

int ISocket::ChangeStatus(int status)
{
    RETURN_CHK(SOCKET_DISCONNECTED == status || SOCKET_CONNECTED == status, ERR_INPUT_PARAM);

    sys_EnterCriticalSection(m_hSection);
    if (status != m_iStatus)
    {
        m_iStatus = status;
        if (SOCKET_DISCONNECTED == status &&
            0 == (EMSKT_NO_RECONN & m_emWorkModel) &&
            0 == (EMSKT_NO_AUTOCONN & m_emWorkModel))
        {//从已连接到断开
            RunThreadConnect();
        }
        LOG_INF1("ISocket::ChangeStatus(%d).", status);
    }
    sys_LeaveCriticalSection(m_hSection);
    return RETURN_OK;
}

int ISocket::SetRemoteIP(const char* szIP, int usPort)
{
    RETURN_CHK(szIP && usPort, ERR_INPUT_PARAM);
    sys_EnterCriticalSection(m_hSection);
    strcpy_s(m_szIP, szIP);
    m_usPort = usPort;
    sys_LeaveCriticalSection(m_hSection);
    return RETURN_OK;
}

void ISocket::RunThreadConnect()
{
    ZERO_CHK(!m_bRun);

    m_bRun = true;
    sys_ResetEvent(m_hQuitSign);
    if (INVALID_SKT != m_socket)
    {
        sys_CloseSocket(m_socket);
        m_socket = INVALID_SKT;
    }
    if (pool_thread(Run, this))
    {
        m_bRun = false;
        sys_SetEvent(m_hQuitSign);
        LOG_ERR("ISocket::RunThreadConnect() pool_thread return error.");
    }
}

int ISocket::Connect()
{
    int skt = INVALID_SKT;

    RETURN_CHK_NOPRT(INVALID_SKT != (skt = WaitConn()), ERR_NO_CONNECT);

    //连接成功
    m_socket = skt;
    ChangeStatus(SOCKET_CONNECTED);
    return 0;
}

int WINAPI ISocket::Run(void* lparam)
{
    ISocket* pThis = (ISocket*)lparam;
    LOG_INF1("ISocket::Run AutoConnect..");
    while(pThis->m_bRun && SOCKET_DISCONNECTED == pThis->m_iStatus)
    {
        sys_EnterCriticalSection(pThis->m_hSection);
        pThis->Connect();
        sys_LeaveCriticalSection(pThis->m_hSection);
    }
    pThis->m_bRun = false;
    LOG_INF1("ISocket AutoConnect thread Exit.");
    sys_SetEvent(pThis->m_hQuitSign);  //标识线程已经退出
    return 0;
}
