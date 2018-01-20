/*********************************************************************
    *文件说明: 提供同进程模块之间的通讯功能
    *作者: luojing
    *日期: 2013/10/20
    *修改日志:  2013/10/26 CW0793 创建
                
***********************************************************************/
#include <vector>
#include <list>
#include "common.h"
#include "sysapi.h"

using std::list;
using std::vector;

//回调函数结构体，发消息用，各模块必须注册了才能接收消息
struct ST_CALLBACK
{
    char dll[DLL_NAME_LEN+1];
    RegistCallBackProc callback;
    ST_CALLBACK()
    {
        memset(dll, 0, DLL_NAME_LEN+1);
        callback = NULL;
    }
};

//消息结构体，用于建立消息队列，post消息时用到
struct ST_MSG
{
    char dll[DLL_NAME_LEN+1];
    unsigned int wparam;
    void *       lparam;  //
    unsigned int isize;   //lparam字节大小
    SYSTEM_MSG_TYPE type;
    ST_MSG()
    {
        memset(dll, 0, DLL_NAME_LEN+1);
        wparam = 0;
        lparam = NULL;
        isize = sizeof(unsigned int);
        type = EVENT_USUALLY;
    }
};

vector<ST_CALLBACK*> g_callBack;    //回调函数合集

BOOL g_ismsgInit = FALSE;  //是否在运行，post消息必须运行时才能用
list<ST_MSG*> g_msgList; //定义全局消息队列
HANDLE g_hThreadPost = INVALID_HANDLEVALUE;   //post消息线程句柄
//CRITICAL_SECTION g_cs;
HANDLE g_cs;

void ClearMsgList()
{
    ST_MSG* pst_msg = NULL;

    sys_EnterCriticalSection(g_cs);
    while(g_msgList.size() > 0)
    {
        pst_msg = g_msgList.front();
        g_msgList.pop_front();
        if (NULL != pst_msg)
        {
            if (NULL != pst_msg->lparam)
            {
                delete pst_msg->lparam;
                pst_msg->lparam = NULL;
            }
            delete pst_msg;
            pst_msg = NULL;
        }
    }
    sys_LeaveCriticalSection(g_cs);
}

int WINAPI MsgPostProc(void* lpThreadParameter)
{
    vector<ST_CALLBACK*>::iterator it;
    ST_MSG* pst_msg = NULL;

    while(g_ismsgInit)
    {
        if (0 == g_msgList.size())
        {
            sys_Sleep(100);
            continue;
        }
        sys_EnterCriticalSection(g_cs);
        pst_msg = g_msgList.front();
        g_msgList.pop_front();  //只发送一次，出消息队列
        sys_LeaveCriticalSection(g_cs);

        for(it = g_callBack.begin(); it!= g_callBack.end(); it++)
        {
            if (0 == strcmp(pst_msg->dll, (*it)->dll) ||
                0 == strcmp(pst_msg->dll, MODULE_ALL))
            {
                ((*it)->callback)(pst_msg->wparam, pst_msg->lparam, pst_msg->type);    //发送消息
            }
        }

        if (NULL != pst_msg)
        {
            if (NULL != pst_msg->lparam)
            {
                delete pst_msg->lparam;
                pst_msg->lparam = NULL;
            }
            delete pst_msg;
            pst_msg = NULL;
        }
		
    }
    return 0;
}

//初始化post消息机制
EXPORT_C bool WINAPI init_msg()
{
    if (g_ismsgInit)
    {
        return true;
    }
    g_cs =  sys_InitializeCriticalSection();
    g_ismsgInit = TRUE;
    g_hThreadPost = sys_CreateThread(MsgPostProc, NULL);
    return true;
}

EXPORT_C void WINAPI uninit_msg()
{
    if (g_ismsgInit)
    {
        g_ismsgInit = FALSE;
        sys_WaitForSingleObject(g_hThreadPost, INFINITE);   //等待线程退出
        sys_CloseHandle(g_hThreadPost);
        ClearMsgList();
    }
    for(vector<ST_CALLBACK*>::iterator it = g_callBack.begin(); it != g_callBack.end(); it++)
    {
        LOG_INFO("callback proc address.0x%x\n", (*it)->callback);
        delete *it;
    }

	sys_DeleteCriticalSection(g_cs);
}

//注册消息接收函数
EXPORT_C bool WINAPI register_notify(LPCSTR dllName, RegistCallBackProc fun)
{
    vector<ST_CALLBACK*>::iterator it;
    ST_CALLBACK* pst_callback = NULL;

    RETURN_NULLPTR(dllName, false);
    RETURN_NULLPTR(fun, false);
    RETURN_NULLPTR(strlen(dllName), false);

    for(it = g_callBack.begin(); it!= g_callBack.end(); it++)
    {
        if (0 == strcmp(dllName, (*it)->dll))
        {//重名或者重复注册了
            return false;
        }
    }
    pst_callback = new ST_CALLBACK;
    strcpy_s(pst_callback->dll, DLL_NAME_LEN, dllName);
    pst_callback->callback = fun;
    g_callBack.push_back(pst_callback);
    return true;
}

EXPORT_C void WINAPI unregister_notify(LPCSTR dllName)
{
    vector<ST_CALLBACK*>::iterator it;
    ST_CALLBACK* pst_callback = NULL;

    RETURN_NULL(dllName);
    RETURN_NULL(strlen(dllName));

    for(it = g_callBack.begin(); it!= g_callBack.end(); it++)
    {
        if (0 == strcmp(dllName, (*it)->dll))
        {
            pst_callback = *it;
            g_callBack.erase(it);
            delete pst_callback;
            pst_callback = NULL;
            break;
        }
    }
}

EXPORT_C int WINAPI msg_send(unsigned int wparam, void *lparam,
    LPCSTR dllName, SYSTEM_MSG_TYPE msgType)
{
    vector<ST_CALLBACK*>::iterator it;
    bool isAll = false;

    if (!dllName)
    {
        isAll = true;
    }
    else if (0 == strcmp(dllName, MODULE_ALL))
    {
        isAll = true;
    }

    for(it = g_callBack.begin(); it!= g_callBack.end(); it++)
    {
        if (isAll)
        {//空，则全部发送
            if ((*it)->callback)
            {
                ((*it)->callback)(wparam, lparam, msgType);
            }
            continue;
        }
        if (0 == strcmp(dllName, (*it)->dll))
        {
            if ((*it)->callback)
            {
                return ((*it)->callback)(wparam, lparam, msgType);
            }
        }
    }

    return isAll ? COMMON_NORMAL : ERR_NOREGIST;
}

EXPORT_C int WINAPI msg_post(unsigned int wparam,void *lparam, unsigned int lsize,
    LPCSTR dllName, SYSTEM_MSG_TYPE msgType)
{
    vector<ST_CALLBACK*>::iterator it;

    if (!g_ismsgInit)
    {
        init_msg();
    }

    ST_MSG* pst_msg = new ST_MSG;
    if (dllName)
    {
        strcpy_s(pst_msg->dll, DLL_NAME_LEN, dllName);
    }
    else
    {
        strcpy_s(pst_msg->dll, DLL_NAME_LEN, MODULE_ALL);
    }
    
    pst_msg->wparam = wparam;
    if (NULL != lparam)
    {
        pst_msg->lparam = new char[lsize];
        memcpy(pst_msg->lparam,lparam, lsize);
    }
    else
    {
        pst_msg->lparam = NULL;
    }
    pst_msg->type = msgType;

    sys_EnterCriticalSection(g_cs);
    g_msgList.push_back(pst_msg);
    sys_LeaveCriticalSection(g_cs);
    return COMMON_NORMAL;
}