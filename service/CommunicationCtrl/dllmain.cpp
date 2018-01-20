// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "iCommunication.h"
#include "NetworkCtrl.h"

//BOOL APIENTRY DllMain( HMODULE hModule,
//                       DWORD  ul_reason_for_call,
//                       LPVOID lpReserved
//					 )
//{
//	switch (ul_reason_for_call)
//	{
//	case DLL_PROCESS_ATTACH:
//	case DLL_THREAD_ATTACH:
//	case DLL_THREAD_DETACH:
//	case DLL_PROCESS_DETACH:
//		break;
//	}
//	return TRUE;
//}

/**********************************************************************
函数      : InitConnect
功能      : 初始化处理
输入参数  : pCount 初始化配置项
输出参数  : N/A
返回值    : 成功或失败
备注      : SDK入口函数
**********************************************************************/
int InitConnect(void *pCount)
{
    int iRet = 0;
    SOCKET_PARAM *pParam = NULL;
    CNetworkCtrl *pInstance = NULL;
    if (NULL == pCount)
    {
        return -1;
    }
    pParam = (SOCKET_PARAM *)pCount;
    pInstance = CNetworkCtrl::GetInstance(pParam->strServerIPAdd.c_str());
    if (NULL != pInstance)
    {
        iRet = pInstance->InitConnect(pParam);
    }
    return iRet;
}
void ReleaseConnectInstance()
{
    CNetworkCtrl::ReleaseInstance(NULL);
}

/**********************************************************************
函数      : UnInitConnect
功能      : 初始化处理
输入参数  : pCount 初始化配置项
输出参数  : N/A
返回值    : 成功或失败
备注      : N/A
**********************************************************************/
int UnInitConnect(void)
{
    int iRet = 0;
    return iRet;
}
/**********************************************************************
函数      : RegesterCallBack
功能      : 注册触发接口
输入参数  : 回调函数
输出参数  : N/A
返回值    : 成功或失败
备注      : N/A
**********************************************************************/
bool RegesterCallBack(pFunCallback pfun)
{
    CNetworkCtrl *pInstance = NULL;
    pInstance = CNetworkCtrl::GetInstance();
    if (NULL != pInstance)
    {
        pInstance->RegesterCallBack(pfun);
    }
    return true;
}
/**********************************************************************
函数      : RegesterCloseSys
功能      : 通知进程关闭
输入参数  : 回调函数
输出参数  : N/A
返回值    : 成功或失败
备注      : N/A
**********************************************************************/
bool RegesterCloseSys(pFunCloseSys pfun)
{
    CNetworkCtrl *pInstance = NULL;
    pInstance = CNetworkCtrl::GetInstance();
    if (NULL != pInstance)
    {
        pInstance->RegesterCloseSys(pfun);
    }
    return true;
}

int GrabFinished(int iSceneID,CHECK_RESULT *pResult, int iErrorCode)
{
    int iRet = 0;
    CNetworkCtrl *pInstance = NULL;
    pInstance = CNetworkCtrl::GetInstance();
    if (NULL != pInstance)
    {
        iRet = pInstance->GrabFinished(iSceneID, pResult,iErrorCode);
    }
    return iRet;
}

int getImagePathFinished(int iSceneID, char* path,int iErrorCode)
{
    int iRet = 0;
    CNetworkCtrl *pInstance = NULL;
    pInstance = CNetworkCtrl::GetInstance();
    if (NULL != pInstance)
    {
        iRet = pInstance->getImagePathFinished(iSceneID, path,iErrorCode);
    }
    return iRet;
}
int CommunicationProcessFinished(int iSceneID, CHECK_RESULT *pResult, int iErrorCode)
{
    int iRet = 0;
    CNetworkCtrl *pInstance = NULL;
    pInstance = CNetworkCtrl::GetInstance();
    if (NULL != pInstance)
    {
        iRet = pInstance->ProcessFinished(iSceneID, pResult, iErrorCode);
    }
    return iRet;
}
  /**********************************************************************
    函数      : SendImage
    功能      : 发送图像
    输入参数  : pResult 处理结果
                iErrorCode 错误码
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
  **********************************************************************/

EXPORT_C DLL_EXPORT int SendImage( int iSceneID, void *pBuffer, int size,int w,int h,int iErrorCode /*= 0*/ )
{
	int iRet = 0;
	CNetworkCtrl *pInstance = NULL;
	pInstance = CNetworkCtrl::GetInstance();
	if (NULL != pInstance)
	{
		iRet = pInstance->SendImage(iSceneID,pBuffer,size,w,h,iErrorCode);
	}
	return iRet;
}

/**********************************************************************
函数      : OpenLight
功能      : 光源操作
输入参数  : index 光源索引
            type  开关类型
输出参数  : N/A
返回值    : 成功或失败
备注      : 光源通过IO口操作， IO口独占
**********************************************************************/
bool OpenLight(int index, int type)
{
    bool bRet = false;
    CNetworkCtrl *pInstance = NULL;
    pInstance = CNetworkCtrl::GetInstance();
    if (NULL != pInstance)
    {
        bRet = pInstance->OpenLight(index, type);
    }
    return bRet;
}
/**********************************************************************
函数      : ResetFinished
功能      : 重置系统返回消息
输入参数  :  iErrorCode 错误码
输出参数  : N/A
返回值    : 成功或失败
备注      : N/A
**********************************************************************/
int ResetFinished(int iErrorCode)
{
    int iRet = 0;
    CNetworkCtrl *pInstance = NULL;
    pInstance = CNetworkCtrl::GetInstance();
    if (NULL != pInstance)
    {
        iRet = pInstance->ResetFinished( iErrorCode);
    }
    return iRet;
}
/**********************************************************************
函数      : PantCheckSend
功能      : 心跳检测发送数据函数
输入参数  :  
输出参数  : N/A
返回值    : 成功或失败
备注      : N/A
**********************************************************************/
int PantCheckSend()
{
    int iRet = 0;
    CNetworkCtrl *pInstance = NULL;
    pInstance = CNetworkCtrl::GetInstance();
    if (NULL != pInstance)
    {
        iRet = pInstance->PantCheckSend();
    }
    else
    {
        iRet = 1;  //系统已销毁
    }
    return iRet;
}

void SetCommuniWebProctocol(const PROCTOCOLDATA& ProctoData)
{
	CNetworkCtrl *pInstance = NULL;
	pInstance = CNetworkCtrl::GetInstance();
	if (NULL != pInstance)
	{
		pInstance->SetCommuniWebProctocol(ProctoData);
	}
}

void GetCommuniWebProctocol( PROCTOCOLDATA& ProctoData)
{
	CNetworkCtrl *pInstance = NULL;
	pInstance = CNetworkCtrl::GetInstance();
	if (NULL != pInstance)
	{
		pInstance->GetCommuniWebProctocol(ProctoData);
	}
}




