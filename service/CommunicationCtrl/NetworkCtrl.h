#pragma once
#include "iCommunication.h"
#include "sysapi.h"
#include <map>
#include <string>

#include "vision_api.h"
using namespace std;

struct IMG_BUF
{
	int sence;
	int size;
	void* buf;
	void *network;
	IMG_BUF()
	{
		sence = -1;
		size = 0;
		buf = NULL;
		network =NULL;
	}

	IMG_BUF &operator=( IMG_BUF copy)
	{
		sence = copy.sence;
		size = copy.size;
		if (NULL != buf)
		{
			delete buf;
			buf = NULL;					
		}
		buf = new char[size];	
		memcpy(buf,copy.buf,size);
		network = copy.network;
		return *this;
	}
};
class SocketClientLogic;
class CNetworkCtrl
{
public:
    CNetworkCtrl(void);
    ~CNetworkCtrl(void);

public:
    static CNetworkCtrl *GetInstance(const char* pIp = NULL);
    static void ReleaseInstance();
    static void ReleaseInstance(const char* pIp);


    /**********************************************************************
    函数      : InitConnect
    功能      : 初始化处理
    输入参数  : pCount 初始化配置项
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : N/A
    **********************************************************************/
    int InitConnect(void *pCount);
    /**********************************************************************
    函数      : UnInitConnect
    功能      : 去初始化处理
    输入参数  : pCount 初始化配置项
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : N/A
    **********************************************************************/
    int UnInitConnect(void *pCount);
        
    static DWORD WINAPI ThreadClient(void *pParam);
	static DWORD WINAPI ThreadSendImg(void *pParam) ;
    /**********************************************************************
    函数      : RegesterCallBack
    功能      : 注册触发接口
    输入参数  : 回调函数
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : N/A
    **********************************************************************/
    bool RegesterCallBack(pFunCallback pfun);

    /**********************************************************************
    函数      : RegesterCloseSys
    功能      : 通知进程关闭
    输入参数  : 回调函数
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : N/A
    **********************************************************************/
    bool RegesterCloseSys(pFunCloseSys pfun);


    /**********************************************************************
    函数      : 
    功能      : 图像采集完成
    输入参数  : iSceneID 场景ID
                iErrorCode 错误码
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : N/A
    **********************************************************************/
    int GrabFinished(int iSceneID, CHECK_RESULT *pResult,int iErrorCode);

	/**********************************************************************
    函数      : getImagePathFinished
    功能      : 获取图像路径完成
    输入参数  : iSceneID 场景ID
	            path   结果图片路径
                iErrorCode 错误码
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : N/A
    **********************************************************************/
	int getImagePathFinished(int iSceneID, char* path,int iErrorCode = 0);
    /**********************************************************************
    函数      : ProcessFinished
    功能      : 处理完成
    输入参数  : pResult 处理结果
                iErrorCode 错误码
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
    int ProcessFinished(int iSceneID, CHECK_RESULT *pResult, int iErrorCode = 0);

	int SendData2Board(int iSceneID, CHECK_RESULT *pResult, int iErrorCode = 0);
  /**********************************************************************
    函数      : SendImage
    功能      : 发送图像
    输入参数  : pResult 处理结果
                iErrorCode 错误码
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
	int SendImage(int iSceneID, void* pBuffer,int size,int w,int h, int iErrorCode);
    /**********************************************************************
    函数      : OpenLight
    功能      : 光源操作
    输入参数  : index 光源索引
                type  开关类型, 1为开， 0为关
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 光源通过IO口操作， IO口独占
    **********************************************************************/
    bool OpenLight(int index, int type);


	/**********************************************************************
    函数      : ResetFinished
    功能      : 重置系统返回消息
    输入参数  :  iErrorCode 错误码
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : N/A
    **********************************************************************/
    int ResetFinished(int iErrorCode = 0);

    /**********************************************************************
    函数      : PantCheckSend
    功能      : 心跳检测发送数据函数
    输入参数  :  
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : N/A
    **********************************************************************/
    int PantCheckSend();

	void SetCommuniWebProctocol(const PROCTOCOLDATA& ProctoData);
	void GetCommuniWebProctocol( PROCTOCOLDATA& ProctoData);


	/**********************************************************************
    函数      : SetCalibRevIPHead
    功能      : 设置接收的 标定 IP通信 头标志
    输入参数  :  
    输出参数  : N/A
    返回值    : 
    备注      : N/A
    **********************************************************************/
	void SetCalibRevIPHead(const char* szRvHead);

    /**********************************************************************
    函数      : SetWorkRevIPHead
    功能      : 设置接收的 作业 IP通信 头标志
    输入参数  :  
    输出参数  : N/A
    返回值    : 
    备注      : N/A
    **********************************************************************/
	void SetWorkRevIPHead(const char* szRvHead);

	/**********************************************************************
    函数      : SetCalibSendIPHead
    功能      : 设置发送的 标定 IP通信 头标志
    输入参数  :  
    输出参数  : N/A
    返回值    : 
    备注      : N/A
    **********************************************************************/
	void SetCalibSendIPHead(const char* szSendHead);

    /**********************************************************************
    函数      : SetWorkSendIPHead
    功能      : 设置发送的 作业 IP通信 头标志
    输入参数  :  
    输出参数  : N/A
    返回值    : 
    备注      : N/A
    **********************************************************************/
    void SetWorkSendIPHead(const char* szSendHead);


	 /**********************************************************************
    函数      : SetIPDividLable
    功能      : 设置 IP通信 结尾标志
    输入参数  :  
    输出参数  : N/A
    返回值    : 
    备注      : N/A
    **********************************************************************/
    void SetIPDividLable(const char* szIpDivid);

    /**********************************************************************
    函数      : SetWorkSendIPHead
    功能      : 设置 IP通信 结尾标志
    输入参数  :  
    输出参数  : N/A
    返回值    : 
    备注      : N/A
    **********************************************************************/
    void SetIPEndLable(const char* szIpEnd);




private:
    int RecvQueue();

    bool AssembleProtocol(int index, int iType, int iPercent, char *pData, unsigned int datalength);
    int ResolveProtocol();

private:
    static CNetworkCtrl *m_pNetworkInstance;
    SocketClientLogic *m_pSocketInstance;
    pFunCallback      m_pTriggerFun;       //记录通知处理函数指针
    pFunCloseSys      m_pRemoteSysCommand;       //记录通知系统结束函数指针

    bool              m_bIsRunning;
    HANDLE            m_SocketHandle;

    char              m_szRecvBuf[1024];
    map<int, int>     m_vecOperateResult;
	IMG_BUF			  m_imgb;
    HANDLE  m_Section;


	// 通信协议定义 
	char  m_CalibRevIPHead[64];
	char  m_WorkRevIPHead[64];
	char  m_CalibSendIPHead[64];
	char  m_WorkSendIPHead[64];
	char  m_IPDivid[32];
	char  m_IPEnd[64];
	int  m_SceneIdTemp;
	int m_serverstatus;
	
};

