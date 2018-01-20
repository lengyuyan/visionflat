#include "skt.h"
#include "socket.h"
#include "client.h"
#include "server.h"
#include "common.h"

#include "GolobalError.h"

using std::pair;

//CSSocket类
CSSocket* CSSocket::m_Instance = NULL;
CSSocket::CSSocket(void)
{
    srand((unsigned)time(NULL));
    m_InitSktID = rand() % 100 + 1000;
    m_hSection = sys_InitializeCriticalSection();
    LOG_INF1("CSSocket Create.");
}

CSSocket::~CSSocket(void)
{
    hash_map<int, ISocket*>::iterator it;
    sys_EnterCriticalSection(m_hSection);
    for(it = m_hash_skt.begin(); it != m_hash_skt.end(); it++)
    {
        delete it->second;
    }
    m_hash_skt.clear();
    sys_LeaveCriticalSection(m_hSection);
    sys_DeleteCriticalSection(m_hSection);
    //LOG_INF1("CSSocket Destory.");//这句为什么不能加
}

CSSocket* CSSocket::GetInstance()
{
    if (NULL == m_Instance)
    {
        m_Instance = new CSSocket;
    }
    return m_Instance;
}

void CSSocket::ReleaseInstance()
{
    if (m_Instance)
    {
        delete m_Instance;
        m_Instance = NULL;
    }
}

ISocket* CSSocket::FindSKT(int skt)
{
    ISocket* pstSKT = NULL;
    hash_map<int, ISocket*>::iterator it;

    sys_EnterCriticalSection(m_hSection);
    it = m_hash_skt.find(skt);
    if (it != m_hash_skt.end())
    {
        pstSKT = it->second;
    }
    sys_LeaveCriticalSection(m_hSection);
    return pstSKT;
}

//创建一个client
int CSSocket::CreateC(const char* server, unsigned short sport, const char* local, unsigned short lport)
{
    int ret = 0;
    int skt = 0;
    CClient* pSocket = NULL;

    if (!chk_ipaddress(server))
    {
        LOG_ERR("Create Socket Client Error. %s: ip invalid.", server);
        return INVALID_SKT;
    }

    pSocket = new CClient();
    ret = pSocket->Init(server, sport, local, lport);
    if (ret)
    {
        LOG_ERR("Create Init(%s,%d,%s,%d)=%d.", server, sport, local ? local : "", lport, ret);
        FREE_ANY(pSocket);
        return INVALID_SKT;
    }

    sys_EnterCriticalSection(m_hSection);
    skt = GetAutoSktID();
    m_hash_skt.insert(pair<int, ISocket*>(skt, pSocket));
    sys_LeaveCriticalSection(m_hSection);
    return skt;
}

//创建一个server
int CSSocket::CreateS(const char* server, unsigned short sport, const char* local, unsigned short lport)
{
    int ret = 0;
    int skt = 0;
    bool bFind = false;
    CServer* pSocket = NULL;
    CListen* pListen = NULL;
    vector<CListen*>::iterator it;

    if (!server)
    {//保证非空
        server = ANY_IP;
    }

    //先查找此服务端是否存在,若存在,则直接返回即可
    sys_EnterCriticalSection(m_hSection);
    for(it = m_vecListen.begin(); it != m_vecListen.end(); it++)
    {
        pListen = *it;
        if (pListen->IsSelf(server, sport))
        {
            bFind = true;
            break;
        }
    }
    if (!bFind)
    {
        pListen = new CListen();
        ret = pListen->Init(server, sport);
        if (ret)
        {
            LOG_ERR("Init Net Server return error:%d", ret);
            delete pListen;
            sys_LeaveCriticalSection(m_hSection);
            return INVALID_SKT;
        }
        m_vecListen.push_back(pListen);
    }

    pSocket = new CServer();
    ret = pSocket->Init(pListen, local, lport);
    if (ret)
    {
        LOG_ERR("Create Init(%s,%d,%s,%d)=%d.", server, sport, local ? local : "", lport, ret);
        FREE_ANY(pSocket);
        sys_LeaveCriticalSection(m_hSection);
        return INVALID_SKT;
    }

    skt = GetAutoSktID();
    pListen->PushHandle(skt);
    m_hash_skt.insert(pair<int, ISocket*>(skt, pSocket));
    sys_LeaveCriticalSection(m_hSection);
    return skt;
}

int CSSocket::GetModel(int skt)
{
    ISocket* pSocket = FindSKT(skt);

    RETURN_CHK(pSocket, 0);

    return pSocket->GetModel();
}

int CSSocket::SetModel(int skt, int nCode)
{
    ISocket* pSocket = FindSKT(skt);

    RETURN_CHK(pSocket, ERR_INVALID_HANDLE);

    return pSocket->SetModel(nCode);
}

//关闭一个skt
int CSSocket::Close(int skt)
{
    int ret = INVALID_SKT;
    vector<CListen*>::iterator itl;
    hash_map<int, ISocket*>::iterator it;

    sys_EnterCriticalSection(m_hSection);
    it = m_hash_skt.find(skt);
    if (it != m_hash_skt.end())
    {//先查找listen
        it->second->Close();
        for(itl = m_vecListen.begin(); itl != m_vecListen.end(); itl++)
        {
            if (it->second->GetListen() == *itl)
            {//is server
                (*itl)->PopupHandle(skt);
                if (0 == (*itl)->GetHandleCnt())
                {
                    delete *itl;
                    m_vecListen.erase(itl);
                }
                break;
            }
        }
        delete it->second;
        m_hash_skt.erase(it);
        ret = 0;
    }
    sys_LeaveCriticalSection(m_hSection);
    return ret;
}

int CSSocket::Connect(int skt)
{
    ISocket* pSocket = FindSKT(skt);

    RETURN_CHK(pSocket, ERR_INVALID_HANDLE);

    return pSocket->ManualConn();
}

//关闭一个skt
int CSSocket::Reconn(int skt)
{
    ISocket* pSocket = FindSKT(skt);

    RETURN_CHK(pSocket, ERR_INVALID_HANDLE);

    return pSocket->Reconn();
}

int CSSocket::Send(int skt, const char* pData, int len)
{
    ISocket* pSocket = FindSKT(skt);

    RETURN_CHK(pSocket, ERR_INVALID_HANDLE);

    return pSocket->Send(pData, len);
}

int CSSocket::Recv(int skt, char* pData, int len, int timeout)
{
    ISocket* pSocket = FindSKT(skt);

    RETURN_CHK(pSocket, ERR_INVALID_HANDLE);

    return pSocket->Recv(pData, len, timeout);
}

int CSSocket::GetStatus(int skt)
{
    ISocket* pSocket = FindSKT(skt);

    RETURN_CHK(pSocket, ERR_INVALID_HANDLE);

    return pSocket->GetStatus();
}

int CSSocket::GetSocket(int skt)
{
    ISocket* pSocket = FindSKT(skt);

    RETURN_CHK(pSocket, INVALID_SKT);

    return pSocket->GetSocket();
}

int CSSocket::GetAddr(int skt, char* lpRemote, unsigned short* pusport)
{
    ushort port = 0;
    ISocket* pSocket = FindSKT(skt);

    RETURN_CHK(pSocket, ERR_INVALID_HANDLE);

    pSocket->GetAddr(lpRemote, port);
    if (pusport)
    {
        *pusport = port;
    }

    return RETURN_OK;
}

int CSSocket::ClearMsg(int skt)
{
    ISocket* pSocket = FindSKT(skt);

    RETURN_CHK(pSocket, ERR_INVALID_HANDLE);

    pSocket->ClearMsgList();
    return RETURN_OK;
}