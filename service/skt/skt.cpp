// skt.cpp : 定义 DLL 应用程序的导出函数。
//
#include "skt.h"
#include "socket.h"
#include "common.h"

//#include"GlobalDefine.h"

#include "GolobalError.h"

unsigned int g_initCnt = 0;

EXPORT_C int WINAPI skt_init()
{
    int ret = 0;
    if (0 == g_initCnt)
    {
        ret = sys_InitNet();    //初始化网络协议栈
        if (ret)
        {
            LOG_ERR("skt_init sys_InitNet()=%d.", ret);
            return ret;
        }
    }
    g_initCnt++;
    return RETURN_OK;
}

EXPORT_C void WINAPI skt_uninit()
{
    if (g_initCnt > 0)
    {
        g_initCnt--;
        if (0 == g_initCnt)
        {
            CSSocket::ReleaseInstance();
            sys_UninitNet();    //关闭网络协议栈
        }
    }
}

EXPORT_C int WINAPI create_c(const char* server, unsigned short sport, const char* local, unsigned short lport)
{
    RETURN_CHK(g_initCnt, INVALID_SKT);
    return SKT_INSTANCE->CreateC(server, sport, local, lport);
}

EXPORT_C int WINAPI create_s(const char* server, unsigned short sport, const char* local, unsigned short lport)
{
    RETURN_CHK(g_initCnt, INVALID_SKT);
    return SKT_INSTANCE->CreateS(server, sport, local, lport);
}

EXPORT_C int WINAPI skt_close(int skt)
{
    RETURN_CHK(g_initCnt, ERR_NOINIT_SKT);
    return SKT_INSTANCE->Close(skt);
}

EXPORT_C int WINAPI skt_conn(int skt)
{
    RETURN_CHK(g_initCnt, ERR_NOINIT_SKT);
    return SKT_INSTANCE->Connect(skt);
}

EXPORT_C int WINAPI skt_reconn(int skt)
{
    RETURN_CHK(g_initCnt, ERR_NOINIT_SKT);
    return SKT_INSTANCE->Reconn(skt);
}

EXPORT_C int WINAPI skt_set_model(int skt, int nCode)
{
    RETURN_CHK(g_initCnt, ERR_NOINIT_SKT);
    return SKT_INSTANCE->SetModel(skt, nCode);
}

EXPORT_C int WINAPI skt_get_model(int skt)
{
    RETURN_CHK(g_initCnt, 0);
    return SKT_INSTANCE->GetModel(skt);
}

EXPORT_C int WINAPI skt_send(int skt, const char* pData, int len)
{
    RETURN_CHK(g_initCnt, ERR_NOINIT_SKT);
    return SKT_INSTANCE->Send(skt, pData, len);
}

EXPORT_C int WINAPI skt_recv(int skt, char* pData, int len, int timeout)
{
    RETURN_CHK(g_initCnt, ERR_NOINIT_SKT);
    return SKT_INSTANCE->Recv(skt, pData, len, timeout);
}

EXPORT_C int WINAPI skt_status(int skt)
{
    RETURN_CHK(g_initCnt, ERR_INVALID_HANDLE);
    return SKT_INSTANCE->GetStatus(skt);
}

EXPORT_C int WINAPI get_socket(int skt)
{
    RETURN_CHK(g_initCnt, INVALID_SKT);
    return SKT_INSTANCE->GetSocket(skt);
}

EXPORT_C int WINAPI get_addr(int skt, char* lpRemote, unsigned short* pusport)
{
    RETURN_CHK(g_initCnt, ERR_NOINIT_SKT);
    return SKT_INSTANCE->GetAddr(skt, lpRemote, pusport);
}

EXPORT_C int WINAPI clear_msg(int skt)
{
    RETURN_CHK(g_initCnt, ERR_NOINIT_SKT);
    return SKT_INSTANCE->ClearMsg(skt);
}