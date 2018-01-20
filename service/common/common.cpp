/*********************************************************************
    *文件说明: 本文件主要封装各模块间通信接口，common资源管理接口
    *作者: luojing
    *日期: 2013/10/20
    *修改日志:  2013/10/22 CW0793 创建
                
***********************************************************************/
#include "GlobalDefine.h"
#include "GolobalError.h"
#include "common.h"
#include "log.h"
#include "thread_pool.h"
#include "msg.h"

typedef struct HINSTANCE__
{
	int unused;
} *HINSTANCE;
#ifndef DLL_PROCESS_ATTACH
#define DLL_PROCESS_ATTACH   1
#endif

#ifndef DLL_THREAD_ATTACH
#define DLL_THREAD_ATTACH    2
#endif

#ifndef DLL_THREAD_DETACH
#define DLL_THREAD_DETACH    3
#endif

#ifndef DLL_PROCESS_DETACH
#define DLL_PROCESS_DETACH   0
#endif


extern BOOL g_ismsgInit;    //msg是否初始化
extern BOOL g_isskInit;     //ssk是否初始化

bool g_bInitComm = 0;
EXPORT_C int APIENTRY
    DllMain(HINSTANCE hInstance, unsigned long dwReason, void* lpReserved)
{
    // 如果使用 lpReserved，请将此移除
    //UNREFERENCED_PARAMETER(lpReserved);

    if (dwReason == DLL_PROCESS_ATTACH)
    {
        // 扩展 DLL 一次性初始化
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        //TRACE0("OpPlugin.DLL 正在终止!\n");
        //printf("detach..");
        // 在调用析构函数之前终止该库

    }
    return 1;
}

EXPORT_C bool WINAPI init_common()
{
    //此处不初始化任何功能
    //有需要的模块自行初始化，msg ssk log在调用相关函数时会自动初始化
	if (false != g_bInitComm)
	{
		return false;
	}
	g_bInitComm = true;

	MSG_INSTANCE;

	init_log();

    return true;
}

EXPORT_C void WINAPI uninit_common()
{
	if (false == g_bInitComm)
	{
		return ;
	}
	 g_bInitComm = false;

    if (g_ismsgInit)
    {//判断msg是否回收了资源
      //  uninit_msg();
    }
	 MSG_INSTANCE->ReleaseInstance();    //释放消息类

    if (g_isskInit)
    {//判断ssk是否已经回收资源
        uninit_ssk();
    }
	uinit_log();
}
EXPORT_C int WINAPI pool_thread(THREAD_PROC lpStartAddress, void* lpParameter)
{
	RETURN_CHK(g_bInitComm, ERR_NOINIT_COMM);
	return POOL_INSTANCE->Run(lpStartAddress, lpParameter);
}
//把消息机制加进来



EXPORT_C int WINAPI register_notify(const char* name, RegistCallBackProc fun, void* lparam)
{//注册消息接收函数
	RETURN_CHK(g_bInitComm, ERR_NOINIT_COMM);
	return MSG_INSTANCE->RegisterNotify(name, fun, lparam);
}
//消息类接口
EXPORT_C int WINAPI unregister_notify(const char* name, RegistCallBackProc fun)
{//注册消息接收函数
	RETURN_CHK(g_bInitComm, ERR_NOINIT_COMM);
	return MSG_INSTANCE->UnRegisterNotify(name);
}


//EXPORT_C int WINAPI msg_recv_str(char* lpstr, int timeout, const char* name)
//{
//	RETURN_CHK(g_bInitComm, ERR_NOINIT_COMM);
//	return MSG_INSTANCE->RecvMsgStr(lpstr, timeout, name);
//}

EXPORT_C int WINAPI msg_send(unsigned int wparam,  void* lparam, unsigned int lsize, const char* name)
{
	RETURN_CHK(g_bInitComm, ERR_NOINIT_COMM);
	return MSG_INSTANCE->SendMsg(wparam, lparam, lsize,name);
}

EXPORT_C int WINAPI msg_post(unsigned int wparam,  void* lparam, unsigned int lsize, const char* name)
{
	RETURN_CHK(g_bInitComm, ERR_NOINIT_COMM);
	return MSG_INSTANCE->PostMsg(wparam, lparam, lsize, name);
}