

// FlatFishManager.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ManagerCtrl.h"
#include "VisionServer.h"
#include "iCommunication.h"
#include "iConfigManager.h"
#include "iLightCtrl.h"
#include "iImageCtrl.h"
#include <process.h>
#include  <io.h>
#include <assert.h>
#include "FuncDefine.h"
#include "sysapi.h"
#include "common.h"

#define OPEN_OP           1;   //打开
#define CLOSE_OP          0;   //关闭

unsigned long         m_CurrentError;      //记录系统初始化的异常

#define					HEART_BEAT_TIME		2000
#define					MAN_OK				0
#define					MAN_ERROR			-1

CHECK_RESULT  SceneProcResult;

vector<SCENE_PKG *> CManagerCtrl::m_vecSceneParam;

class ManageDataInvalib
{
public:
	ManageDataInvalib (bool *bIsValib)
	{
		if (NULL != bIsValib)
		{
			*bIsValib = true;
		}
		m_bIsValib = bIsValib;
	}

	~ManageDataInvalib ()
	{
		if (NULL != m_bIsValib)
		{
			*m_bIsValib = false;
		}
	}
private:
	bool *m_bIsValib;
};

class LockStock 
{
public:
	LockStock (CRITICAL_SECTION *LockSection)
	{
		if (NULL != LockSection)
		{
			EnterCriticalSection(LockSection);
		}
		m_LockSection = LockSection;
	}

	~LockStock ()
	{
		if (NULL != m_LockSection)
		{
			LeaveCriticalSection(m_LockSection);          
		}
	}
private:
	CRITICAL_SECTION *m_LockSection;
};

CManagerCtrl *CManagerCtrl::m_pInstance = NULL;
CRITICAL_SECTION *CManagerCtrl::m_pSection = NULL;


CManagerCtrl::CManagerCtrl(void)
{
	m_pVisionInstance = NULL;
	m_pVisionInstance = new CVisionServer;
	m_CalibWindID = -1;
	memset(m_chSN,0,64);
	memset(m_SceneWidth, 0, SCENE_SIZE);
	memset(m_SceneHeight, 0, SCENE_SIZE);
	memset(m_bIsWorking,0, SCENE_SIZE);
	m_CurrentError = INIT_NULL;
	m_iExitThread = true;
	InitReadConfig();
	register_notify(MODULE_SCENE_MAIN_SDK, RemoteManualMsg,this);
	m_RecvEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_WorkingStatus = 0;
	if (NULL == m_pSection)
	{
		m_pSection = new CRITICAL_SECTION;
		InitializeCriticalSection(m_pSection);
	}

	//初始化存放场景结果的map
	m_mapSceneResultSection = sys_InitializeCriticalSection();
	ClearSceneProcResultMap();
	//跟据系统参数来生成结果map
	InitSceneProcResultMap();


}

CManagerCtrl::~CManagerCtrl(void)
{
	WORKING_THREAD_DATA *pInfo = NULL;
	map<int, WORKING_THREAD_DATA *>::iterator itTh = m_ThSceneData.begin();
	while (itTh != m_ThSceneData.end())
	{
		pInfo = itTh->second;
		if (NULL != pInfo)
		{
			SetEvent(pInfo->thExit);
			delete pInfo;
			pInfo = NULL;
			//句柄在析构函数中关闭
		}
		m_pInstance->m_ThSceneData.erase(itTh);
		itTh = m_ThSceneData.begin();
	}

	CVisionServer::ReleaseInstance();
	if (NULL != m_pVisionInstance)
	{
		delete m_pVisionInstance;
		m_pVisionInstance = NULL;
	}
	//unregister_notify(MODULE_SCENE_MAIN_SDK);
	vector<SCENE_PKG *>::iterator it;
	SCENE_PKG *pConfig = NULL;
	for (it = m_vecSceneParam.begin(); it != m_vecSceneParam.end(); it++)
	{
		pConfig = (*it);
		//delete 
		if (!pConfig->vecBody.empty())
		{
			vector<VIRSION_DATA_PKG *>::iterator it;
			it = pConfig->vecBody.begin();
			for (; it != pConfig->vecBody.end(); )
			{
				delete *it;
				pConfig->vecBody.erase(it);
				it = pConfig->vecBody.begin();
			}
		}
		if (!pConfig->vecDataType.empty())
		{
			vector<unsigned int *>::iterator it;
			for (it = pConfig->vecDataType.begin(); it != pConfig->vecDataType.end();)
			{
				delete *it;
				pConfig->vecDataType.erase(it);
				it = pConfig->vecDataType.begin();
			}
		}
		if (!pConfig->InterfaceSetData.empty())
		{
			vector<INTERFACE_SET_DATA *>::iterator it;
			for (it = pConfig->InterfaceSetData.begin(); it != pConfig->InterfaceSetData.end(); )
			{
				delete *it;
				pConfig->InterfaceSetData.erase(it);
				it = pConfig->InterfaceSetData.begin();
			}
		}
		delete pConfig;
		pConfig = NULL;
	}
	//释放结果map锁
	ClearSceneProcResultMap();
	sys_DeleteCriticalSection(m_mapSceneResultSection);


	ReleaseConnectInstance();
	m_iExitThread=false;
	//释放光源
	destroyLightInstance();
	deinitImage();
	CloseHandle(m_RecvEvent);
	ReleaseConfigInstance();
	unregister_notify(MODULE_SCENE_MAIN_SDK,RemoteManualMsg);
}

CManagerCtrl *CManagerCtrl::GetInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CManagerCtrl;
	}
	return m_pInstance;
}

void CManagerCtrl::ReleaseInstance()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

/**********************************************************************
函数      : InitReadConfig
功能      : 初始化读取配置文档处理
输入参数  : N/A
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
void  CManagerCtrl::InitReadConfig()
{
	SCENE_PKG *pSceneConfig;
	if (InitConfig(&m_stSysParam))//读取系统参数
	{
		m_vecSceneParam.clear();
		for (int i = 0; i < m_stSysParam.iSceneNum; i++)//根据系统参数里的场景个数来读取场景参数
		{
			pSceneConfig = new SCENE_PKG;
			pSceneConfig->sceneID = i;//场景ID为在系统文件中的排列顺序
			strncpy(pSceneConfig->szSceneName, m_stSysParam.pstrSceneName[i].c_str(), SCENE_SIZE - 1);
			GetSceneConfig(m_stSysParam.pstrSceneName[i].c_str(), (void *)pSceneConfig);
			m_vecSceneParam.push_back(pSceneConfig);  
			//在这里生成visionprocess，产生处理类，不要动态生成，动态生成的好处是可以少点内存,这里做的好处是可以在创建场景里的模板时可以读取模板然后保存模板id
			//无需每次检测都去读取模板

			CVisionServer::GetInstance(i);

		}
		SetCurrentStatus(INIT_CONFIG_OK);
	}
	else
	{
		msg_post(MSG_DISP_WORKING_INFO, "初始化配置失败", (unsigned int)strlen("初始化配置失败") + 1, MODULE_SCENE_MAIN_UI); 
	}
	AlterMainWndTitle(m_stSysParam.CurProductName);
}
/**********************************************************************
函数      : InitManager
功能      : 读取系统配置文件,场景参数，初始化相机，光源
输入参数  : N/A
输出参数  : 无
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::InitManager()
{
	bool bRet = true;
	if (CAMERA_SUCCESS != imageInstance(m_stSysParam.iCameraDllNum,m_stSysParam.pstrCameraDll))  //待改为可配置
	{
		//发送异常给UI;
		msg_post(MSG_DISP_WORKING_INFO, "相机初始化失败",(unsigned int)strlen("相机初始化失败")+1, MODULE_SCENE_MAIN_UI); 
		for (int in = 0; in < m_stSysParam.iCameraNum; in++)
		{
			msg_post(MSG_CONNECT_CAMERA_FAILED, &in,sizeof(int), MODULE_SCENE_MAIN_UI); 
		}
		bRet = false;
	}
	else
	{
		int iResult = 0;
		int width = 0;
		int height = 0;
		for (int i = 0; i < m_stSysParam.iCameraNum; i++)
		{
			if (CAMERA_SUCCESS != openCamera(m_stSysParam.pstrCamareGUID[i]))   
			{
				msg_post(MSG_DISP_WORKING_INFO, "相机打开失败",(unsigned int)strlen("相机初打开失败")+1, MODULE_SCENE_MAIN_UI); 
				msg_post(MSG_CONNECT_CAMERA_FAILED, &i,sizeof(int), MODULE_SCENE_MAIN_UI); 
			}
			else
			{
				msg_post(MSG_CONNECT_CAMERA_SUCCESS, &i,sizeof(int), MODULE_SCENE_MAIN_UI); 
				string camerasuccess = m_stSysParam.pstrCamareGUID[i]+ "相机打开成功";
				const char *cameraID = (camerasuccess).c_str();
				msg_post(MSG_DISP_WORKING_INFO, (void*)cameraID,(unsigned int)strlen(camerasuccess.c_str())+1, MODULE_SCENE_MAIN_UI);
			}
		}
		SetCurrentStatus(INIT_CAMERA_OK);
	}
	Sleep(200); //TBD
	//初始化相机机械旋转矩阵
	for (int i = 0;i<m_stSysParam.iCameraNum;i++)
	{
		for (int AxeIndex = 0;AxeIndex<m_stSysParam.iAxisNum;AxeIndex++)
		{
			for (int j = 0;j<m_stSysParam.iToolNum;j++)
			{
				m_pVisionInstance->SetCamInMecRot(i,AxeIndex,j,m_stSysParam.CamInMecRotMetix[i][AxeIndex][j],(int)m_stSysParam.CamInMecRotMetix[i][AxeIndex][j][0]);
				m_pVisionInstance->SetCamRot(i,AxeIndex,j,m_stSysParam.CamRotMetix[i][AxeIndex][j],(int)m_stSysParam.CamRotMetix[i][AxeIndex][j][0]);
				m_pVisionInstance->SetImageMetrix(i,AxeIndex,j,m_stSysParam.ImageRelative[i][AxeIndex][j],m_stSysParam.ImageAbsolute[i][AxeIndex][j],
					(int)m_stSysParam.ImageRelative[i][AxeIndex][j][0]);

			}
		}
	}
	//初始化光源
	if (LIGHTCTL_SUCCESS != iLightInstance(m_stSysParam.strLightInfo, m_stSysParam.iLightNum, m_stSysParam.strLightType,4))
	{
		//发送异常给UI;
		msg_post(MSG_DISP_WORKING_INFO, "光源初始化失败",(unsigned int)strlen("光源初始化失败") + 1, MODULE_SCENE_MAIN_UI); 
		bRet = false;
	}
	else
	{
		//初始化光源大小
		for (int iL = 0; iL < m_stSysParam.iLightNum; iL++)
		{
			for (int iC = 0; iC < m_stSysParam.strLightInfo[iL].Channel; iC++)
			{
				setLightValue(m_stSysParam.strLightInfo[iL].PortName, iC, m_stSysParam.strLightInfo[iL].DefaultValue);
				closeLight(m_stSysParam.strLightInfo[iL].PortName, iC);
			}
		}
		SetCurrentStatus(INIT_LIGHT_OK);
	}
	//初始化图像处理
	if(m_pVisionInstance->InitVision())
	{
		SetCurrentStatus(INIT_VISION_OK);
	}
	else 
	{
		msg_post(MSG_DISP_WORKING_INFO, "初始化图像处理失败",(unsigned int)strlen("初始化图像处理失败") + 1, MODULE_SCENE_MAIN_UI); 
		bRet = false;
	}
	//初始化通讯处理
	if (0 <= InitConnect((void *)&m_stSysParam.stSocketParam))
	{
		SetCurrentStatus(INIT_CONNECT_OK);
		msg_post(MSG_DISP_WORKING_INFO, "初始化连接成功",(unsigned int)strlen("初始化连接成功") + 1, MODULE_SCENE_MAIN_UI);  
	}
	else
	{
		//向list中添加运行log
		msg_post(MSG_DISP_WORKING_INFO, "初始化连接失败",(unsigned int)strlen("初始化连接失败") + 1, MODULE_SCENE_MAIN_UI);  
		bRet = false;
	}
	//向socket注册触发检测回调接口
	// RegesterCallBack(CManagerCtrl::TriggerVisionProcess);
	// RegesterCloseSys(CManagerCtrl::RemoteSystemMsg); //处理其它远程通知消息
	return bRet;
}

bool CManagerCtrl::UnInitManager()
{
	bool bRet = false;
	deinitImage();
	//释放串口
	for (int iL = 0; iL < m_stSysParam.iLightNum; iL++)
	{
		deInitLightCtrl(m_stSysParam.strLightInfo[iL].PortName);
	}
	UnInitConnect();
	//ReleaseInstance();///为啥要注释掉这句？
	return bRet;
}

void  CManagerCtrl::GetSceneProcess(pSceneProcess* pProcess)
{

	m_SceneProcess = pProcess;

}

/**********************************************************************
函数      : TriggerVisionProcess
功能      : 触发视觉处理处理
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::TriggerVisionProcess(int iSceneID, int iPercent)
{
	bool bRet = false;
	//启动检测(线程执行);
	map<int, WORKING_THREAD_DATA*>::iterator itTh;
	WORKING_THREAD_DATA *pthInfo = NULL;
	itTh = m_ThSceneData.find(iSceneID);  
	LockStock Stock(m_pSection);
	if(itTh != m_ThSceneData.end())
	{
		pthInfo = itTh->second;
		//当前场景的处理线程已创建
		if (WAIT_OBJECT_0 != WaitForSingleObject(pthInfo->thRunning, 0))  //检测当前场景处理是否在运行，当事件为有信号时 WAIT_OBJECT_0，
		{
			//无信号，表示场景没有执行
			ResetEvent(pthInfo->thBegin);  //清空已有状态
			SetEvent(pthInfo->thBegin);    //启动场景处理
		}
		else  //有信号，表示场景正在执行。
		{
			int SceneLocal = iSceneID;
			msg_post(MSG_SCENE_BUSY, &SceneLocal, sizeof(int), MODULE_SCENE_MAIN_SDK);//通知当前场景正忙 ，
		}
	}
	else
	{
		WORKING_THREAD_DATA *pthInfo = new WORKING_THREAD_DATA;
		if (NULL != pthInfo)
		{
			pthInfo->iSceneID = iSceneID;
			pthInfo->thHandle = (HANDLE)_beginthreadex(NULL,0,(unsigned (__stdcall *)(void*))CManagerCtrl::ThreadVisionProcess,(void *)pthInfo,0,NULL);
			SetEvent(pthInfo->thBegin);
			m_ThSceneData.insert(make_pair(iSceneID, pthInfo));
		}
	}

	return bRet;
}

/**********************************************************************
函数      : ThreadVisionProcess
功能      : 动态创建的图像处理线程
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
void CManagerCtrl::ThreadVisionProcess(void *pParam)
{
	WORKING_THREAD_DATA *thInfo = (WORKING_THREAD_DATA *)pParam;
	int iSceneID = (int)thInfo->iSceneID;	
	bool bIsFinde = false;
	int  iErrorCode = 0;
	int  iTimeCircle = 0;
	int  pThInfo = (int)thInfo;
	char szErr[126] = {0};
	HANDLE hThEvnets[2] = {thInfo->thBegin, thInfo->thExit};
	DWORD  hEventID = -1;
	SCENE_PKG *pConfig = m_pInstance->GetSceneHandle(iSceneID);
	CHECK_RESULT  *g_SceneProcResult = NULL;
	string sname = pConfig->szSceneName;
	if (NULL == pConfig || 0 == strlen(pConfig->szSceneName))
	{
		return;
	}
	do 
	{
		hEventID = WaitForMultipleObjects(2, hThEvnets, false, INFINITE);
		g_SceneProcResult = m_pInstance->GetSceneProcResult(sname);		
		SetEvent(thInfo->thRunning);    //启动场景处理
		if (WAIT_OBJECT_0 == hEventID)
		{
			LARGE_INTEGER  start,end,litmp;
			QueryPerformanceFrequency(&litmp);//获取系统时钟频率
			QueryPerformanceCounter(&start); 
			if (NULL != m_pInstance)
			{
				if (0 == m_pInstance->m_WorkingStatus)
				{		
					//memset(g_SceneProcResult->szValue[RESULT_NUMBER],0,sizeof(char)*RESULT_NUMBER*BUF_SIZE/2);
					iErrorCode = m_pInstance->SingleVisionProcess(iSceneID, *g_SceneProcResult,WORKING_CAMERA);
					thInfo = (WORKING_THREAD_DATA*)pThInfo;
					//是否需要进行仿射转换 
				}
				else
				{
					sprintf_s(szErr, "请退出登陆");
					msg_post(MSG_DISP_WORKING_INFO, szErr, (unsigned int)strlen(szErr) + 1, MODULE_SCENE_MAIN_UI); 
					g_SceneProcResult->err = -2;
				}
			}  
			if (1000 == iErrorCode)//视觉处理OK
			{
				int TotalIndex = g_SceneProcResult->iCount;
				//memcpy(g_SceneProcResult->szValue[0],"1",sizeof(g_SceneProcResult->szValue[0]));
				//memcpy(g_SceneProcResult->szValue[1],g_SceneProcResult->szValue[TotalIndex-3],sizeof(g_SceneProcResult->szValue[TotalIndex-3]));
				//memcpy(g_SceneProcResult->szValue[2],g_SceneProcResult->szValue[TotalIndex-2],sizeof(g_SceneProcResult->szValue[TotalIndex-2]));
				//memcpy(g_SceneProcResult->szValue[3],g_SceneProcResult->szValue[TotalIndex-1],sizeof(g_SceneProcResult->szValue[TotalIndex-1]));

			}else
			{
				//这里不会填充那些数据
				//memcpy(g_SceneProcResult->szValue[0],"0",sizeof(g_SceneProcResult->szValue[0]));
			}
			g_SceneProcResult->SeneTpye = WORKING_CAMERA;
			int LocalSceneID =iSceneID;

			QueryPerformanceCounter(&end) ; 
			double dTime = (double)(end.QuadPart-start.QuadPart) / (double)litmp.QuadPart;    //获取时间单位为秒	
			char szBuf[125] = {0};
			g_SceneProcResult->time = dTime - g_SceneProcResult->caturetime;
			m_pInstance->ManagerProcFinish(LocalSceneID, g_SceneProcResult, iErrorCode);  //网络传送处理结果
			if ( 0<=iErrorCode )
			{	//通知UI显示结果,显示在工作界面上
				msg_post(MSG_DISP_IMAGE_DUMP, (void *)&iSceneID, sizeof(int), MODULE_SCENE_MAIN_UI);  //gpz不需要发送图片,这里发图片也有问题只能发1024个字节
			}
			m_pInstance->ClearProcResultByID(iSceneID);
			sprintf_s(szBuf, "场景 %d 用时 %0.3lfs",iSceneID, dTime);
			msg_post(MSG_DISP_WORKING_INFO, (void *)szBuf, (unsigned int)strlen(szBuf)+1, MODULE_SCENE_MAIN_UI);
			ResetEvent(thInfo->thRunning);  //清空已有状态
		}
		//退出线程， 并清除数据
		else if ((WAIT_OBJECT_0 + 1) == hEventID)
		{
			break;  //退出线程
		}
		ResetEvent(thInfo->thBegin);  //清空已有状态
	} while (true);
}

int CManagerCtrl::SceneGrabImage(int iSceneID,int ImageIndex,int CamIndex,int shutterTime,int Gain,int Interval,int LightIndex,int LightValue)
{
	int  iErrorCode = 0;
	char szErr[128] = {0};
	sprintf_s(szErr, "Grab start,cameraindex:%d,sceneID:%d,imageidx:%d.", CamIndex,iSceneID,ImageIndex);
	LOG_INFO(szErr);
	if (!m_pInstance->GrabWay2Sceneimage(iSceneID, ImageIndex,CamIndex,shutterTime,Gain,Interval,LightIndex,LightValue))
	{	
		iErrorCode = -1;	
		sprintf_s(szErr, "Grab fail,cameraindex:%d,sceneID:%d,imageidx:%d.", CamIndex,iSceneID,ImageIndex);
		LOG_ERR(szErr);
		msg_post(MSG_DISP_WORKING_INFO, szErr, (unsigned int)strlen(szErr) + 1, MODULE_SCENE_MAIN_UI); 
		return iErrorCode;
	}
	sprintf_s(szErr, "Grab end,cameraindex:%d,sceneID:%d,imageidx:%d.", CamIndex,iSceneID,ImageIndex);
	LOG_INFO(szErr);
	return iErrorCode;
}

/**********************************************************************
函数      : SingleVisionProcess
功能      : 抓图处理
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
result   返回结果
type     处理类型
输出参数  : N/A
返回值    : 返回结果
备注      : 
修改记录
日期            修改人            修改内容
2016-08-16     guopengzhou        这里主要是标定和处理用到此函数，根据协议返回客户端指令必须不同，
这里加type来区分，处理时需返回拍照完成指令
**********************************************************************/
int CManagerCtrl::SingleVisionProcess(int iSceneID, CHECK_RESULT &result,unsigned int type)
{
	int  iErrorCode = 0;
	SCENE_PKG *pscene;
	int LocalSceneId = iSceneID;
	char szErr[126] = {0};
	bool bIsWorking = m_bIsWorking[iSceneID];
	if (bIsWorking)
	{
		iErrorCode = -1;
		sprintf_s(szErr, "当前场景(%d)正在处理中", iSceneID);
		msg_post(MSG_DISP_WORKING_INFO, szErr, (unsigned int)strlen(szErr) + 1, MODULE_SCENE_MAIN_UI); 
		result.err = -2;
		return iErrorCode;
	}
	ManageDataInvalib Invalib(&m_bIsWorking[iSceneID]);
	pscene = m_pInstance->GetSceneHandle(iSceneID);
	if (!pscene)
	{
		return -1;
	}

	if ( 0 == pscene->ProcProcess)  //新方法不单独拍照
	{
		iErrorCode = 1000;
	}

	if (-1 != iErrorCode)
	{    //step4 调用图像处理过程 //SYSTEMTIME st;已把过程计算时间加到结果结构体中
		sprintf_s(szErr, "T_当前场景(%d)Procee_start", iSceneID);
		LOG_INFO(szErr);
		if (!m_pInstance->VisionGetResult(iSceneID, 0,result))////第二个参数不应该为定值，上面可能采集了几张相片
		{
			LOG_ERR("图像处理失败");
			sprintf_s(szErr, "图像处理失败(%d)", iSceneID);
			msg_post(MSG_DISP_WORKING_INFO, szErr, (unsigned int)strlen(szErr) + 1, MODULE_SCENE_MAIN_UI); 
			iErrorCode = -2;
		}
		sprintf_s(szErr, "T_当前场景(%d)Procee_end", iSceneID);
		LOG_INFO(szErr);
	}
	return iErrorCode;
}

void CManagerCtrl::ThreadCalibProcess(void *pParam)
{
	bool bRet = true;
	CManagerCtrl *pInstance = NULL;
	pInstance = GetInstance();
	CALIB_DATA *pData = (CALIB_DATA *)pParam;
	//没登陆不允许进行标定操作
	if (NULL == pInstance || NULL == pData)
	{
		return;
	}
	bRet = pInstance->VisionCalibProcess(pParam);
	if (bRet)
	{
		msg_post(MSG_DISP_IMAGE_DUMP, (void *)&pData->iSceneID, sizeof(int), MODULE_SCENE_MAIN_UI);  
	}
}

/**********************************************************************
函数      : ThreadCalibProcess
功能      : 标定图像处理线程
输入参数  : 标定参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::VisionCalibProcess(void *pParam)
{
	double PixSize = 0.0;
	int CameraID = -1;
	int iError = 0;
	int AixsIndex = -1;
	CALIB_DATA *pData = (CALIB_DATA *)pParam;
	char szBuf[256] = {0};
	char *pTmp = NULL;
	CManagerCtrl *pInstance = NULL;
	int index = 0;
	pInstance = GetInstance();
	//没登陆不允许进行标定操作
	if (NULL == pInstance || (m_WorkingStatus == 0 && pData->bIsInteractive))
	{
		return false;
	}

	SCENE_PKG *pConfig = NULL;
	string sname ;
	CHECK_RESULT  *g_SceneProcResult = NULL;
	//获取保存路径--需要用到该关系的话， 只能是单个相机
	pConfig = pInstance->GetSceneHandle(pData->iSceneID);
	pConfig->halconID = pInstance->m_CalibWindID;
	sname = pConfig->szSceneName;

	//index = pData->index;  //标定点索引
	g_SceneProcResult = m_pInstance->GetSceneProcResult(sname);


	//调用标定处理（场景处理)
	iError = pInstance->SingleVisionProcess(pData->iSceneID, *g_SceneProcResult,CALIB_CAMERA);
	index = pConfig->CalPointIndex;//记忆已经标定的点的数量,开始是0,出错不会清零，记住要清零，最好搞一个清零按钮
	int CamIndex = g_SceneProcResult->CameraIndex[0]; //因为标定场景只有一个相机，所以数组下标为0
	AixsIndex = pConfig->AxisIndex;
	int ToolIndex = pConfig->MotorToolIndex;
	if (AixsIndex<0)
	{
		msg_post(MSG_DISP_CLIB_INFO, "轴序号错误或未设定", (unsigned int)strlen("轴序号错误或未设定") + 1, MODULE_SCENE_MAIN_UI); 
		return false;
	}
	if (CamIndex<0)
	{
		msg_post(MSG_DISP_CLIB_INFO, "相机序号错误或未设定", (unsigned int)strlen("相机序号错误或未设定") + 1, MODULE_SCENE_MAIN_UI); 
		return false;
	}
	if (ToolIndex<0)
	{
		msg_post(MSG_DISP_CLIB_INFO, "夹具序号错误或未设定", (unsigned int)strlen("夹具序号错误或未设定") + 1, MODULE_SCENE_MAIN_UI); 
		return false;
	}
	if ( m_stSysParam.CalPointNum[CamIndex][AixsIndex][ToolIndex]<3)
	{
		msg_post(MSG_DISP_CLIB_INFO, "标定点数量少于3个点", (unsigned int)strlen("标定点数量少于3个点") + 1, MODULE_SCENE_MAIN_UI); 
		return false;
	}
	MyFunClass::ParshRelativeToObsolute(m_stSysParam.CalMotorX[CamIndex][AixsIndex][pConfig->MotorToolIndex],
		m_stSysParam.CameraCalMachineX[CamIndex], m_stSysParam.CalPointNum[CamIndex][AixsIndex][pConfig->MotorToolIndex]);
	MyFunClass::ParshRelativeToObsolute(m_stSysParam.CalMotorY[CamIndex][AixsIndex][pConfig->MotorToolIndex], 
		m_stSysParam.CameraCalMachineY[CamIndex], m_stSysParam.CalPointNum[CamIndex][AixsIndex][pConfig->MotorToolIndex]);

	if (0 == pConfig->ProcProcess && 0 <= iError)
	{
		//1，标定 相机的 机械坐标和像素坐标 放入系统参数中，且保存到系统配置文件中 模板匹配的值放在1,2,3
		m_stSysParam.CameraCalPixRow[CamIndex][index] = atof(g_SceneProcResult->szValue[1]);
		m_stSysParam.CameraCalPixCol[CamIndex][index]  = atof(g_SceneProcResult->szValue[2]);
		m_stSysParam.CameraCalPixPhi[CamIndex][index]  = atof(g_SceneProcResult->szValue[3]);
		double CamDisPhi=m_stSysParam.CameraCalPixPhi[CamIndex][index]-m_stSysParam.CameraCalPixPhi[CamIndex][index-1];//角度查index=0会失败
		//double DisToolPhi = m_stSysParam.CalMotorPhi[CamIndex][AixsIndex][ToolIndex][index] - m_stSysParam.CalMotorPhi[CamIndex][AixsIndex][ToolIndex][index-1] ;
		//保存原始数据到配置文件
		//SaveSysCamCalibrate(CamIndex,pConfig->MotorToolIndex,index+1,m_stSysParam.CameraCalMachineX[CamIndex], m_stSysParam.CameraCalMachineY[CamIndex],
		//m_stSysParam.CameraCalPixRow[CamIndex], m_stSysParam.CameraCalPixCol[CamIndex]);
		//2，如果 索引数量超过3个就可以进行计算了，计算的数据保存在视觉库的静态成员中。
		if (index >= pConfig->CalPointNum-1)
		{
			double CamRow[16]={0.0};
			double CamCol[16]={0.0};
			double MechAndCamRot[6]={0};
			double MechAndCam[6]={0};
			double RelaImageToMotor[6] ={0};
			double AbsImageToMotor[6] ={0};
			PixSize = MyFunClass::CalPixSize(m_stSysParam.CameraCalMachineX[CamIndex],m_stSysParam.CameraCalMachineY[CamIndex],
				m_stSysParam.CameraCalPixCol[CamIndex], m_stSysParam.CameraCalPixRow[CamIndex],index+1);
			m_stSysParam.CameraPixSize[CamIndex][AixsIndex][pConfig->MotorToolIndex] = PixSize;
			//保存像素大小;
			SaveSysCamPixSize(CamIndex,AixsIndex,ToolIndex,PixSize);	
			for (int i = 0;i<=index;i++ )
			{
				CamRow[i] = m_stSysParam.CameraCalPixRow[CamIndex][i];
				CamCol[i] = m_stSysParam.CameraCalPixCol[CamIndex][i];

			}
			//获取相机是移动还是固定；获取相机序号和轴序号；获取相机在第一个标定点坐标；获取夹具在第一个标定点坐标			   
			double CalFirstAngle=0.0;			  
			switch ( m_stSysParam.CamFixedType[CamIndex])
			{
			case SYSTEM_PARAM::FIXED://固定相机为0
				{
					CalFirstAngle = 0.0;  
				}
				break;
			case SYSTEM_PARAM::UNFIXED:
				{
					CalFirstAngle = 3.1416;//移动相机为180
				}
				break;
			}
			double ToolFirstX = m_stSysParam.ToolCalOriX[CamIndex][AixsIndex][ToolIndex];
			double ToolFirstY = m_stSysParam.ToolCalOriY[CamIndex][AixsIndex][ToolIndex];
			double CalFirstX = m_stSysParam.CamCalOriX[CamIndex][AixsIndex][ToolIndex];
			double CalFirstY = m_stSysParam.CamCalOriY[CamIndex][AixsIndex][ToolIndex];

			double RotToCamX = m_stSysParam.RotpointToCamX[CamIndex][AixsIndex][ToolIndex];//旋转中心到相机的距离
			double RotToCamY = m_stSysParam.RotpointToCamY[CamIndex][AixsIndex][ToolIndex];
			double CamRotAngle = m_stSysParam.ToolCalOriPhi[CamIndex][AixsIndex][ToolIndex]-
				m_stSysParam.CamCalOriPhi[CamIndex][AixsIndex][ToolIndex]; //这里的Phi应该是Angle
			if (CamRotAngle>-0.1||CamRotAngle<0.1)//没用到
			{
				CamRotAngle = 0.0;
			}
			CamRotAngle = CamRotAngle*3.1416/180;  //用于机器人计算绝对坐标的标定

			//如果使用的是相机参数，则像素大小设定为1,无量纲;
			double CalPixSize = PixSize;
			if (m_stSysParam.CalWithCamParam[CamIndex][AixsIndex][ToolIndex])
			{
				CalPixSize = 1;
			}

			//参数为:相机索引,标定点数量,相对第一个点的绝对坐标x，y,模板像素坐标x,y,像素大小(可能无单位),旋转矩阵,矩阵,
			//相对矩阵,绝对矩阵,相机内参(12),内参数量,相机外参（7),工件索引,轴索引,相机第一个点x,y,相机固定(0)不固定(3.14),工件第一个点x,y,
			//相机第一个点跟工具第一个的角度,旋转中心到相机的距离x，y,标定班厚度
			m_pVisionInstance->CalCamMetrix(CamIndex,index+1,m_stSysParam.CameraCalMachineX[CamIndex], m_stSysParam.CameraCalMachineY[CamIndex],
				CamRow,CamCol,CalPixSize,MechAndCamRot,MechAndCam,RelaImageToMotor,AbsImageToMotor,
				m_stSysParam.CamParam[CamIndex][AixsIndex][ToolIndex],12,
				m_stSysParam.CamPose[CamIndex][AixsIndex][ToolIndex],ToolIndex,AixsIndex,CalFirstX,CalFirstY,CalFirstAngle,ToolFirstX,
				ToolFirstY,CamRotAngle,RotToCamX,RotToCamY,m_stSysParam.CalThickNess[CamIndex][AixsIndex][ToolIndex]);
			//int BeginIndex = result.iCount;
			//保存机械系到相机系的角度变换矩阵，（平移到原点重合，不含移动）			   
			SaveMecAndCamRot(CamIndex,AixsIndex,ToolIndex,MechAndCamRot,MechAndCam,RelaImageToMotor,
				AbsImageToMotor,sizeof(MechAndCamRot)/sizeof(double));
			memcpy(m_stSysParam.CamInMecRotMetix[CamIndex][AixsIndex][ToolIndex],MechAndCam,sizeof(MechAndCam));
			memcpy(m_stSysParam.CamRotMetix[CamIndex][AixsIndex][ToolIndex],MechAndCamRot,sizeof(MechAndCamRot));
			memcpy(m_stSysParam.ImageRelative[CamIndex][AixsIndex][ToolIndex],RelaImageToMotor,sizeof(RelaImageToMotor));
			memcpy(m_stSysParam.ImageAbsolute[CamIndex][AixsIndex][ToolIndex],AbsImageToMotor,sizeof(AbsImageToMotor));
			//保存图像中心像素点
			m_stSysParam.PicCenterPointX[CamIndex][AixsIndex][ToolIndex]= CamRow[0];
			m_stSysParam.PicCenterPointY[CamIndex][AixsIndex][ToolIndex]= CamCol[0];
			SavePicCenter(CamIndex,AixsIndex,ToolIndex,CamRow[0],CamCol[0]);
		}
		//如果3个标定点，那么就是第2个点计算，此分支没用,先注释
		//if ( m_stSysParam.CalPointNum[CamIndex][AixsIndex][pConfig->MotorToolIndex]-2 == index)  //pConfig->CalPointNum-1
		if(0)
		{
			// 最后一个标定点，计算转轴此时在相机坐标系中的位置坐标; 不管如何，都需要计算转轴与夹具（产品）间的位姿。
			double CenX,CenY;
			if (CamDisPhi>0.017||CamDisPhi<-0.017)//后一个点跟前一个点像素点之间的角度
			{
				CenX = m_stSysParam.CameraCalPixCol[CamIndex][index-1] * PixSize;//产品在相机中的位置,Match value
				CenY = m_stSysParam.CameraCalPixRow[CamIndex][index-1] * PixSize;//
				MyFunClass::CalRotCenterTool(CenX,CenY,m_stSysParam.CameraCalPixCol[CamIndex][index]*PixSize,
					m_stSysParam.CameraCalPixRow[CamIndex][index] * PixSize,-CamDisPhi); //CamDisPhi取反是由于图像中的方向与坐标系中相反。

				m_stSysParam.RotpointToCamX[CamIndex][AixsIndex][ToolIndex] = CenX;
				m_stSysParam.RotpointToCamY[CamIndex][AixsIndex][ToolIndex] = CenY;
				SaveSysRotAxleToCam(CamIndex,AixsIndex,ToolIndex,CenX,CenY);
				//计算转轴与夹具（产品）间的位姿
				//2,产品（夹具）在相机中的位姿

				double ToolX = m_stSysParam.CameraCalPixCol[CamIndex][index] * PixSize;
				double ToolY = m_stSysParam.CameraCalPixRow[CamIndex][index] * PixSize;
				double ToolPhi = -atof(g_SceneProcResult->szValue[2]);
				m_pVisionInstance->AffineCoorPoint(ToolX,ToolY,ToolPhi,CenX,CenY);

			}else
			{
				CenX = 0.0;
				CenY = 0.0;
			}
			m_stSysParam.RotX[AixsIndex][pConfig->MotorToolIndex] = CenX;
			m_stSysParam.RotY[AixsIndex][pConfig->MotorToolIndex] = CenY;
			SaveSysRotAxleToTool(AixsIndex,pConfig->MotorToolIndex,CenX,CenY);

			sprintf_s(g_SceneProcResult->szValue[4],"%d",0);//home
			sprintf_s(g_SceneProcResult->szValue[5],"%d",0); // finish

		}else if(m_stSysParam.CalPointNum[CamIndex][AixsIndex][pConfig->MotorToolIndex] == index)
		{
			sprintf_s(g_SceneProcResult->szValue[4],"%d",1);
			sprintf_s(g_SceneProcResult->szValue[5],"%d",1);
		}else
		{
			sprintf_s(g_SceneProcResult->szValue[4],"%d",0);//  home
			sprintf_s(g_SceneProcResult->szValue[5],"%d",0); // finish
		}

		if (pConfig->CalPointIndex == m_stSysParam.CalPointNum[CamIndex][AixsIndex][pConfig->MotorToolIndex] )
		{
			pConfig->CalPointIndex = 0;
		}
		pConfig->CalPointIndex++;
		//step5 发送检测完成信号	
		sprintf_s(g_SceneProcResult->szValue[0],"%d",1);
		sprintf_s(g_SceneProcResult->szValue[1],"%.4lf",m_stSysParam.CalMotorX[CamIndex][AixsIndex][pConfig->MotorToolIndex][index]);
		sprintf_s(g_SceneProcResult->szValue[2],"%.4lf",m_stSysParam.CalMotorY[CamIndex][AixsIndex][pConfig->MotorToolIndex][index]);
		sprintf_s(g_SceneProcResult->szValue[3],"%.4lf",m_stSysParam.CalMotorPhi[CamIndex][AixsIndex][pConfig->MotorToolIndex][index]);
	}
	else
	{
		sprintf_s(g_SceneProcResult->szValue[1],"%.4lf",0.0);
		sprintf_s(g_SceneProcResult->szValue[2],"%.4lf",0.0);
		sprintf_s(g_SceneProcResult->szValue[3],"%.4lf",0.0);
		sprintf_s(g_SceneProcResult->szValue[0],"%d",0);
		if (m_stSysParam.IsSimulate)
		{
			sprintf_s(g_SceneProcResult->szValue[0],"%d",1);		 
			if(m_stSysParam.CalPointNum[CamIndex][AixsIndex][pConfig->MotorToolIndex]-1 == index)
			{
				sprintf_s(g_SceneProcResult->szValue[4],"%d",1);
				sprintf_s(g_SceneProcResult->szValue[5],"%d",1);
			}else
			{
				sprintf_s(g_SceneProcResult->szValue[4],"%d",0);
				sprintf_s(g_SceneProcResult->szValue[5],"%d",0);
			}

		}else
		{
			index = pConfig->CalPointIndex = 0;
			sprintf_s(g_SceneProcResult->szValue[4],"%d",1);
			sprintf_s(g_SceneProcResult->szValue[5],"%d",1);
		}		
	}
	g_SceneProcResult->iCount= 6;
	g_SceneProcResult->SeneTpye = CALIB_CAMERA;
	g_SceneProcResult->iCount = 6;
	Sleep(100);
	int LocalSceneID = pData->iSceneID;
	ManagerProcFinish(LocalSceneID, g_SceneProcResult, iError);
	return g_SceneProcResult->bIsDisp;
}
/**********************************************************************
函数      : OpenWindow
功能      : 绑定halcon窗口与UI窗口
输入参数  : iWinID 界面窗口ID
x,y,width,hight 界面窗口位置大小
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::BindWindow(long iWinID, int x, int y, int width, int hight, bool bDebug)
{
	long halconID = -1;
	map<long ,long>::iterator it;; 
	it=m_mapWinID.find(iWinID);
	if(it!=m_mapWinID.end())
	{
		halconID = it->second;
		if (-1 != halconID)
		{
			m_pVisionInstance->CloseHalconWindow(halconID);
		}
	}
	m_pVisionInstance->OpenWindow(iWinID, x, y, width, hight, halconID);
	m_mapWinID.insert(make_pair(iWinID,halconID));
	if (bDebug)
	{
		m_CalibWindID = halconID;
	}
	return true;
}

bool CManagerCtrl::SetDebugWindowID(int iSceneID, long iWinID)
{
	bool bRet = false;
	SCENE_PKG *pConfig = NULL;
	map<long ,long>::iterator it;
	long halconID = -1;
	//获取保存路径--需要用到该关系的话， 只能是单个相机
	pConfig = GetSceneHandle(iSceneID);

	it=m_mapWinID.find(iWinID);
	//将窗口ID转换成halcon窗口ID
	if(it!=m_mapWinID.end())
	{
		halconID = it->second;
		if (-1 != halconID)
		{
			m_CalibWindID = halconID;
			pConfig->halconID = m_CalibWindID;
			bRet = true;
		}
	}
	return bRet;
}

/**********************************************************************
函数      : SetSysParam
功能      : 更新视觉配置
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
pData    要设置值的结构体指针
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::SetSysParam(void *pData)
{
	SYSTEM_PARAM *pSys = (SYSTEM_PARAM *)pData;
	if (NULL == pSys)
	{
		return false;
	}

	if (NULL != pSys)
	{
		m_stSysParam.SystemParamCopy(pSys);
	}
	return true;
}


void CManagerCtrl::SetProctocolData(const PROCTOCOLDATA& ProctoData)
{
	m_stSysParam.stSocketParam.CalRevHead = ProctoData.CalRevHead;
	m_stSysParam.stSocketParam.WorkRevHead = ProctoData.WorkRevHead ;
	m_stSysParam.stSocketParam.CalSendHead = ProctoData.CalSendHead;
	m_stSysParam.stSocketParam.WorkSendHead = ProctoData.WorkSendHead;
	m_stSysParam.stSocketParam.DividLabel = ProctoData.IPDivid;
	m_stSysParam.stSocketParam.EndLabel = ProctoData.IPEnd;
	//保存到配置文件
	SaveProctocolData(ProctoData);


}

void CManagerCtrl::SetProductMap(void* pMapData)
{
	//保存场景映射 
	ConfigSaveProductMap(pMapData);


	//保存产品名称及其ID
	//LPPMAPDATA lpData = (LPPMAPDATA)pMapData;
	//PMAPDATA::iterator it;
	//ConfigSaveProductId();
	//for (it = lpData->begin();it!=lpData->end();it++)
	//{
	//     ConfigSaveProductId(GenProductId(it->first),it->first);
	//}



}

void CManagerCtrl::GetProductMap(void* pMapData)
{
	ConfigReadProductMap(pMapData);


}




/**********************************************************************
函数      : SaveSysParam
功能      : 更新并保存视觉配置
输入参数  : N/A 
pData    要设置值的结构体指针
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::SaveSysParam(void *pData)
{
	if (NULL != pData)
	{
		SetSysParam(pData);
	}
	SaveSysConfig(pData);
	return true;
}

/**********************************************************************
函数      : GetSysParam
功能      : 获取系统参数
输入参数  : pData要获取的结构体指针
输出参数  : pData要获取的结构体指针
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::GetSysParam(void *pData)
{
	SYSTEM_PARAM *pSys = (SYSTEM_PARAM *)pData;
	if (NULL != pSys)
	{
		pSys->SystemParamCopy(&m_stSysParam);
	}
	return true;
}


void* CManagerCtrl::GetSysParamPoint()
{
	return  &m_stSysParam;
}


char* CManagerCtrl::GetSysconfigPath()
{
	return ConfigGetsysPath();

}


char* CManagerCtrl::GetSceneName()
{
	return ConfigGetSceneName();
}

/**********************************************************************
函数      : SetSceneParam
功能      : 更新视觉配置
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
pData    要设置值的结构体指针
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::SetSceneParam(void *pData)
{
	bool bRet = true;
	int iSceneID;
	if (NULL == pData)
	{
		return false;
	}
	SCENE_PKG *pconfig = (SCENE_PKG *)pData;
	vector<SCENE_PKG *>::iterator it;
	bool bIsFinde = false;
	iSceneID = pconfig->sceneID;

	//如果列表中有该场景参数，则更新，没有则添加
	for (it = m_vecSceneParam.begin(); it != m_vecSceneParam.end(); it++)
	{
		if (iSceneID == (*it)->sceneID)
		{
			(*it)->SceneCopy(pconfig);
			bIsFinde = true;
			break;
		}
	}
	if (!bIsFinde)
	{
		m_stSysParam.iSceneNum++;
		m_stSysParam.pstrSceneName[iSceneID] = pconfig->szSceneName;
		SCENE_PKG *pNewConfig = new SCENE_PKG;
		*pNewConfig = *pconfig;
		pNewConfig->vecBody = pconfig->vecBody;
		// pNewConfig->pReserve = pconfig->pReserve;
		pNewConfig->vecDataType = pconfig->vecDataType;
		pNewConfig->vecInfo = pconfig->vecInfo;
		m_vecSceneParam.push_back(pNewConfig);
	}
	return bRet;
}





/**********************************************************************
函数      : SetSceneParam
功能      : 更新视觉配置
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
pData    要设置值的结构体指针
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::SaveSceneParam(int iSceneID, void *pData)
{
	bool bRet = false;
	SCENE_PKG *pConfig = NULL;
	if (NULL != pData)
	{
		SetSceneParam(pData);
		pConfig = (SCENE_PKG *)pData;
	}
	else
	{
		pConfig = GetSceneHandle(iSceneID);

	}
	if (NULL != pConfig)
	{
		SetSceneConfig(m_stSysParam.pstrSceneName[iSceneID].c_str(), pConfig);
		bRet = true;
	}
	return bRet;
}

/**********************************************************************
函数      : GetSceneParam
功能      : 更新视觉配置
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : pData    要获取的结构体指针
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::GetSceneParam(int iSceneID, void *pData)
{
	bool bRet = true;
	SCENE_PKG *pConfig = NULL;
	pConfig = GetSceneHandle(iSceneID);
	if (NULL == pConfig)
	{
		bRet = false;
	}
	else
	{
		if (NULL != pData)
		{
			((SCENE_PKG *)pData)->SceneCopy(pConfig);
			//(*(SCENE_PKG *)pData) = *pConfig;
		}
	}
	return bRet;
}

char* CManagerCtrl::GetSceneconfigPath(int SceneID)
{
	//char SceneFolderName[256]={0};

	for (int i = 0;i<m_vecSceneParam.size();i++)
	{
		if (SceneID == m_vecSceneParam.at(i)->sceneID)
		{
			return  ConfigGetScenePath( m_vecSceneParam.at(i)->szSceneName);

		}
	}
	return NULL;

	//sprintf(SceneFolderName,"%s",m_stSysParam.pstrSceneName[SceneIndex]);
	//return ConfigGetScenePath(SceneFolderName);
}

char* CManagerCtrl::GetScenePath(int SceneID)
{
	for (int i = 0;i<m_vecSceneParam.size();i++)
	{
		if (SceneID == m_vecSceneParam.at(i)->sceneID)
		{
			char szBuf[64] = {0};
			char szName[64] = {0};
			memset(m_ScenePath,0,sizeof(m_ScenePath));   
			get_module_path(m_ScenePath);  //取得config.ini的地址
			strcat_s(m_ScenePath,m_vecSceneParam.at(i)->szSceneName);
			if(-1 == access(m_ScenePath, 0))//ini文件不存在
			{
				return NULL;
			}else
			{
				return m_ScenePath;//szCurrentPath
			}		
		}
	}
	return NULL;
}




int CManagerCtrl::GetSceneNum()
{
	return  m_stSysParam.iSceneNum;

}

int CManagerCtrl::GetSceneProcType(int SceneIndex)
{
	if (m_vecSceneParam.size()<= SceneIndex)
	{
		return -1;
	}
	return  m_vecSceneParam.at(SceneIndex)->ProcProcess;

}

int	CManagerCtrl::GetSceneId(int SceneIndex)
{
	if (m_vecSceneParam.size()<= SceneIndex)
	{
		return -1;
	}
	//读取场景的时候,是按照在文件里的顺序来生成id的,这里用向量
	return  m_vecSceneParam.at(SceneIndex)->sceneID;

}


/**********************************************************************
函数      : GrabWay2Sceneimage
功能      : 采用新方法获取图片数据
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
index 当前场景中的第几张图像（存在多张图像则可能根据index区分不同光源或采图参数）
.....光源、曝光 等参数。

输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
void CManagerCtrl::SetCamParam(int CamIndex,int shutterTime,int Gain)
{
	string strCameraGuid;
	strCameraGuid = m_pInstance->m_stSysParam.pstrCamareGUID[CamIndex];  
	// 设置相机shutter
	if (getExposureTime(strCameraGuid) != shutterTime)
	{	
		setExposureTime(strCameraGuid, shutterTime);
		Sleep(200);
	}
	//设置相机增益
	if (getGain(strCameraGuid) != Gain)
	{		
		setGain(strCameraGuid, Gain);
		Sleep(200);
	}
}

bool CManagerCtrl::GrabWay2Sceneimage(int SceneID,int ImageIndex,int CamIndex,int shutterTime,int Gain,int Interval,int LightIndex,int LightValue)
{
	//ImageIndex标识当前场景中的第几个图像采集祥光的处理项
	bool bRet = false;
	SCENE_PKG *pscene;
	int  iErrorCode = 0;
	int  iTimeCircle = 0;
	char szBuf[126] = {0};
	char *pBuf = NULL;
	string strCameraGuid;
	int    width = 0;
	int    height = 0;
	int    iCameraIndex = 0;
	int    iLightIndex = 0;
	int    iShutter = 0;
	pscene = GetSceneHandle(SceneID);	
	strCameraGuid = m_pInstance->m_stSysParam.pstrCamareGUID[CamIndex];  
	// 设置相机shutter
	if (getExposureTime(strCameraGuid) != shutterTime)
	{		
		setExposureTime(strCameraGuid, shutterTime);
		Sleep(Interval);//视频流设置参数进去要有时间
	}
	//设置相机增益
	if (getGain(strCameraGuid) != Gain)
	{		
		setGain(strCameraGuid, Gain);
		Sleep(Interval);
	}
	//获取图像处理缓存
	if (NULL != pscene && 0 != pscene->bIsSimulate) //若为模拟就不抓图
	{
		char szBuf[256] = {0};
		void *pBuf = NULL;
		pBuf = m_pInstance->m_pVisionInstance->GetImageBuf(SceneID, ImageIndex,(int)pscene->defcamwidth, (int)pscene->defcamheight);
		size_t  buffSize = strlen((char*)pBuf);
		//填充buf
		if (pBuf == NULL)
		{
			return false;
		}
		sprintf_s(szBuf, ".\\%s\\demo%d.jpg", m_pInstance->m_stSysParam.pstrSceneName[SceneID].c_str(), CamIndex);
		if (0 == access(szBuf, 0))
		{
			m_pInstance->m_pVisionInstance->ReadImage(szBuf, SceneID, CamIndex, false);
		}
		return true;
	}

	if (CAMERA_SUCCESS == getImageSize(strCameraGuid, width, height))
	{
		void *pBuf = NULL;
		//获取保存图像数据的内存
		pBuf = m_pInstance->m_pVisionInstance->GetImageBuf(SceneID, ImageIndex,width, height);		
		if (pBuf == NULL)
		{
			return false;
		}
		size_t bufferSize =  strlen((char*)pBuf);
		if (0 != getImage(strCameraGuid, pBuf))
		{
			LOG_ERR("抓图失败");
			sprintf_s(szBuf, "抓图失败(%d)", SceneID);
			msg_post(MSG_DISP_WORKING_INFO, szBuf,(unsigned int)strlen(szBuf)+1, MODULE_SCENE_MAIN_UI); 
		}
		else
		{
			bufferSize =  strlen((char*)pBuf);
			bRet = true;

		}
	}
	return bRet;
}


/**********************************************************************
函数      : LightCtrl
功能      : 开关光源
输入参数  : index 光源编号
iType 0为关， 1为开
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::LightCtrl(int index, int iType)
{
	bool bRet = false;
	int iRet = -1;
	if (0 < m_stSysParam.iAutoOL)
	{
		string strComm;
		int    iChannel = -1;
		for (int i = 0; i < m_stSysParam.iLightNum; i++)
		{
			if (index < m_stSysParam.strLightInfo[i].Channel)
			{
				iChannel = index;
				strComm = m_stSysParam.strLightInfo[i].PortName;
				break;
			}
			else
			{
				index = index - m_stSysParam.strLightInfo[i].Channel;
			}
		}
		if (-1 != iChannel)
		{
			if (GetCurrentStatus(INIT_LIGHT_OK))
			{
				if (1 == iType)
				{
					iRet = openLight(strComm,iChannel);
				}
				else
				{
					iRet = closeLight(strComm, iChannel);
				}
				if (0 == iRet)
				{
					bRet = true;
				}
				else
				{
					bRet = false;
				}
			}
		}
		else
		{
			bRet = false;
		}
	}
	else
	{
		//连接失败则不发送
		if (GetCurrentStatus(INIT_CONNECT_OK))
		{
			bRet = OpenLight(index, iType);
		}
	}
	return bRet;
}

//旧方法
bool CManagerCtrl::InitGrabSceneimage(int iSceneID, int index)
{
	bool bRet = false;
	SCENE_PKG *pscene;
	int  iErrorCode = 0;
	int  iTimeCircle = 0;

	//step1 根据场景ID找到对应配置
	pscene = GetSceneHandle(iSceneID);

	int iPos = m_pVisionInstance->FindParamData(pscene, MAIN_PRO, index);
	if (NULL == pscene || iPos < 0 || index >= pscene->vecInfo.size())
	{
		return false;
	}

	//step2 采集图像，结束后通知采集完毕
	do
	{
		char *pBuf = NULL;
		string strCameraGuid;
		int    width = 0;
		int    height = 0;
		int    iCameraIndex = pscene->vecInfo[index].CameraIndex;
		strCameraGuid = m_pInstance->m_stSysParam.pstrCamareGUID[iCameraIndex];  

		if (NULL != pscene && 0 != pscene->bIsSimulate) //若为模拟就不抓图
		{
			char szBuf[256] = {0};
			void *pBuf = NULL;
			pBuf = m_pInstance->m_pVisionInstance->GetImageBuf(iSceneID, index,(int)pscene->defcamwidth, (int)pscene->defcamheight);
			size_t  buffSize = strlen((char*)pBuf);
			//填充buf
			if (pBuf == NULL)
			{
				return false;
			}
			sprintf_s(szBuf, ".\\%s\\demo%d.jpg", m_pInstance->m_stSysParam.pstrSceneName[iSceneID].c_str(), index);
			if (0 == access(szBuf, 0))
			{
				m_pInstance->m_pVisionInstance->ReadImage(szBuf, iSceneID, index, false);
			}
			return true;
		}
		//获取图像处理缓存，获取相机像素
		if (CAMERA_SUCCESS == getImageSize(strCameraGuid, width, height))
		{
			void *pBuf = NULL;
			//获取保存图像数据的内存
			pBuf = m_pInstance->m_pVisionInstance->GetImageBuf(iSceneID, index,width, height);
			size_t  buffSize = strlen((char*)pBuf);
			//填充buf
			if (pBuf == NULL)
			{
				return false;
			}
			if(0 != getImage(strCameraGuid, pBuf))
			{
				bRet = FALSE;
			}
			buffSize = strlen((char*)pBuf);
		}
		else
		{
			bRet = FALSE;
		}

		bRet = true;
	}while(0);

	return bRet;
}



bool CManagerCtrl::InitGrabSceneimage1(int CamIndex ,int iSceneID,int ImageIndex)
{
	bool bRet = false;
	int    width = 0;
	int    height = 0;
	string strCameraGuid;
	strCameraGuid = m_pInstance->m_stSysParam.pstrCamareGUID[CamIndex];  

	SCENE_PKG *pscene = GetSceneHandle(iSceneID); //获取场景包

	if (NULL != pscene && 0 != pscene->bIsSimulate) //若为模拟就不抓图
	{
		int index = 0;//假设每个场景里有一张图片名字为demo0.jpg
		char szBuf[256] = {0};
		void *pBuf = NULL;
		//获取保存图像数据的内存
		pBuf = m_pInstance->m_pVisionInstance->GetImageBuf(iSceneID, ImageIndex,(int)pscene->defcamwidth, (int)pscene->defcamheight);
		size_t  buffSize = strlen((char*)pBuf);
		//填充buf
		if (pBuf == NULL)
		{
			return false;
		}
		sprintf_s(szBuf, ".\\%s\\demo%d.jpg", m_pInstance->m_stSysParam.pstrSceneName[iSceneID].c_str(), index);
		if (0 == access(szBuf, 0))
		{
			m_pInstance->m_pVisionInstance->ReadImage(szBuf, iSceneID, index, false);
		}
		return true;
	}


	if (CAMERA_SUCCESS == getImageSize(strCameraGuid, width, height))
	{
		void *pBuf = NULL;
		//获取保存图像数据的内存
		pBuf = m_pInstance->m_pVisionInstance->GetImageBuf(iSceneID, ImageIndex,width, height);
		size_t  buffSize = strlen((char*)pBuf);
		//填充buf
		if (pBuf == NULL)
		{
			return false;
		}




		if(0 != getImage(strCameraGuid, pBuf))
		{
			bRet = FALSE;
		}		
		bRet = true;
	}
	else
	{
		bRet = FALSE;
	}
	return bRet;
}


/**********************************************************************
函数      : SetGain
功能      : 设置相机采图增益
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
iValue 设置的值
index 当前场景中的第几张图像（存在多张图像则可能根据index区分不同光源或采图参数）
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::SetGain(int iSceneID, int iValue, int index)
{
	string strCameraGuid;
	SCENE_PKG *pscene;

	//step1 根据场景ID找到对应配置
	pscene = GetSceneHandle(iSceneID);
	int iPos = m_pVisionInstance->FindParamData(pscene, MAIN_PRO, index);
	if (NULL == pscene || iPos < 0 || index >= pscene->vecInfo.size())
	{
		return false;
	}
	strCameraGuid = m_pInstance->m_stSysParam.pstrCamareGUID[pscene->vecInfo[index].CameraIndex];  
	setGain(strCameraGuid, iValue);
	return true;
}

/**********************************************************************
函数      : GetGain
功能      : 获取相机采图增益
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
index 当前场景中的第几张图像（存在多张图像则可能根据index区分不同光源或采图参数）
输出参数  : N/A
返回值    : 获取的增益值
备注      : 
**********************************************************************/
int CManagerCtrl::GetGain(int iSceneID, int index)
{
	string strCameraGuid;
	SCENE_PKG *pscene;
	int   iGainValue = 0;

	//step1 根据场景ID找到对应配置
	pscene = GetSceneHandle(iSceneID);
	int iPos = m_pVisionInstance->FindParamData(pscene, MAIN_PRO, index);
	if (NULL == pscene || iPos >= 0 || index >= pscene->vecInfo.size())
	{
		return false;
	}
	strCameraGuid = m_pInstance->m_stSysParam.pstrCamareGUID[pscene->vecInfo[index].CameraIndex];  
	GetGain(iGainValue);
	return iGainValue;
}
/**********************************************************************
函数      : ReadImage
功能      : 加载本地图片
输入参数  : iSceneID  
pFileName 文件名称
index 当前场景中的第几张图像（存在多张图像则可能根据index区分不同光源或采图参数）
输出参数  : 
返回值    : 成功或失败 
备注      : 
**********************************************************************/
bool CManagerCtrl::ReadImage(int iSceneID, const char *pFileName, int index)
{
	m_pVisionInstance->ReadImage(pFileName, iSceneID, index);
	return true;
}

bool CManagerCtrl::SaveImage(int iSceneID,  int index, string SNstr)
{
	bool bRet = false;
	char szPath[MAX_PATH] = {0};
	//路径为:    /配置路径/RawImage/时间(日期)/场景名/
	SYSTEMTIME st;
	GetLocalTime(&st);
	SCENE_PKG  Getsceneparam;
	SYSTEM_PARAM   Sparam;
	GetSysParam(&Sparam);
	GetSceneParam(iSceneID, &Getsceneparam);//获取结构体内容
	strcpy_s(szPath,256, Sparam.ImageSavePath);
	if (create_path(szPath))
	{
		LOG_ERR("access %s failed when save pic",szPath);
		return false;
	}

	sprintf_s(szPath, "%s/%s", szPath,"RawImage");
	if (create_path(szPath))
	{
		LOG_ERR("access %s failed when save pic",szPath);
		return false;
	}
	sprintf_s(szPath, "%s/%04d-%02d-%02d", szPath, st.wYear, st.wMonth, st.wDay);
	if (create_path(szPath))
	{
		LOG_ERR("access %s failed when save pic",szPath);
		return false;
	}
	sprintf_s(szPath, "%s/%s", szPath, Getsceneparam.szSceneName);
	if (create_path(szPath))
	{
		LOG_ERR("access %s failed when save pic",szPath);
		return false;
	}
	SYSTEMTIME lt;
	char temPath[_MAX_PATH*2] = {0};
	GetLocalTime(&lt);
	if (SNstr.empty())
	{
		sprintf_s(temPath,"%02d_%02d_%02d_%02d.jpg",lt.wHour,lt.wMinute,lt.wSecond,lt.wMilliseconds);
		SNstr = temPath;
	}
	sprintf_s(temPath,"%s/%s",szPath,SNstr.c_str());
	CHECK_RESULT *res = GetSceneProcResultById(iSceneID);
	if (NULL == res)
	{
		LOG_ERR("access %s failed when save pic",szPath);
		return false;
	}
	res->rawimg = temPath;
	bRet = m_pVisionInstance->saveImage(iSceneID, index, szPath, SNstr);
	return bRet;
}
/**********************************************************************
函数      : SaveImageDump
功能      : 保存窗口图像
输入参数  : winID 当前窗口ID
iSceneID 场景ID
输出参数  : N/A
返回值    : 成功或失败
备注      : 
修改记录：
日期        修改人                  修改内容
2016-08-16  guopengzhou            visionProcess实例增加图片结果保存路径,以便通讯协议获取
**********************************************************************/
bool CManagerCtrl::SaveImageDump( const long winID,int iSceneID,char* outpath )
{
	bool bRet = NULL;
	char szPath[256] = {0};
	long halconID = -1;
	map<long ,long>::iterator it;; 
	//路径为:    /配置路径/ResultImage/时间(日期)/场景名/
	SCENE_PKG  *sceneparam = NULL;
	SYSTEM_PARAM   *Sparam = NULL;
	SYSTEMTIME st;
	char temPath[_MAX_PATH*2] = {0};
	GetLocalTime(&st);
	Sparam = (SYSTEM_PARAM   *)GetSysParamPoint();
	sceneparam = (SCENE_PKG  *)GetSceneParamHandle(iSceneID);
	strcpy_s(szPath, 256, Sparam->ImageSavePath);
	if (create_path(szPath))
	{
		LOG_ERR("access result pic dir%s failed when save pic",szPath);
		return false;
	}
	sprintf_s(szPath, "%s\\%s", szPath,"ResultImage");
	if (create_path(szPath))
	{
		LOG_ERR("access result pic dir%s failed when save pic",szPath);
		return false;
	}	
	sprintf_s(szPath, "%s\\%04d-%02d-%02d", szPath, st.wYear, st.wMonth, st.wDay);
	if (create_path(szPath))
	{
		LOG_ERR("access result pic dir%s failed when save pic",szPath);
		return false;
	}
	sprintf_s(szPath, "%s\\%s", szPath, sceneparam->szSceneName);
	if (create_path(szPath))
	{
		LOG_ERR("access result pic dir%s failed when save pic",szPath);
		return false;
	}

	//if (SNstr.empty())
	//{
	//	sprintf_s(temPath,"%02d_%02d_%02d_%02d.jpg",lt.wHour,lt.wMinute,lt.wSecond,lt.wMilliseconds);
	//	SNstr = temPath;
	//}
	//sprintf_s(temPath,"%s/%s",szPath,SNstr.c_str());
	//CHECK_RESULT *res = GetSceneProcResultById(iSceneID);
	//if (NULL == res)
	//{
	//	LOG_ERR("access %s failed when save pic",szPath);
	//	return false;
	//}

	it=m_mapWinID.find(winID);
	//将窗口ID转换成halcon窗口ID
	if(it!=m_mapWinID.end())
	{
		halconID = it->second;
		if (-1 != halconID)
		{
			bRet = m_pVisionInstance->saveWinImage(halconID, szPath,outpath,m_chSN);
			if (true == bRet)
			{
				bRet = m_pVisionInstance->saveImagePath(iSceneID, 0, outpath, m_chSN);

			}

		}
	}
	return bRet;
}

/**********************************************************************
函数      : SaveImageDump
功能      : 保存窗口图像
输入参数  : winID 当前窗口ID
iSceneID  场景ID
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
void* CManagerCtrl::GetImageDump( const long winID,int iSceneID,double &width,double &heigh )
{
	bool bRet = false;
	long halconID = -1;
	map<long ,long>::iterator it;
	it=m_mapWinID.find(winID);
	char* buff = NULL;

	//将窗口ID转换成halcon窗口ID
	if(it!=m_mapWinID.end())
	{
		halconID = it->second;
		if (-1 != halconID)
		{
			buff = (char*)m_pVisionInstance->getWinImage(halconID,width,heigh);
		}
	}
	return buff;
}

char* CManagerCtrl::getImagePath(int iSceneID, int index,unsigned int type)
{
	return m_pVisionInstance->getImagePath(iSceneID, index, type);
}

/**********************************************************************
函数      : DispImage
功能      : 显示指定图片到指定窗口上
输入参数  : iWinID 界面窗口ID
iSceneID  窗口序号
index     图像序号，调试使用
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::DispImage(long iWinID, int iSceneID, int index)
{
	bool bRet = false;
	long halconID = -1;
	map<long ,long>::iterator it;; 
	it=m_mapWinID.find(iWinID);
	//将窗口ID转换成halcon窗口ID
	if(it!=m_mapWinID.end())
	{
		halconID = it->second;
		if (-1 != halconID)
		{
			bRet = m_pVisionInstance->DispImage(halconID,iSceneID, index);
		}
	}
	return bRet;
}

bool  CManagerCtrl::FindHalconWinID(int iWinID, long& HalconWinID)
{

	bool bRet = false;
	long halconID = -1;
	map<long ,long>::iterator it;; 
	it=m_mapWinID.find(iWinID);
	//将窗口ID转换成halcon窗口ID
	if(it!=m_mapWinID.end())
	{
		halconID = it->second;
		if (-1 != halconID)
		{
			HalconWinID = halconID;
			bRet = true;
		}
	}
	return bRet;
}

bool CManagerCtrl::ShowVisionString(long winID,char* StrContent,int topRow ,int topCol)
{

	return   m_pVisionInstance->ShowWindString(winID,StrContent,topRow,topCol);

}



bool CManagerCtrl::SetVisionColorHeight(long hwinID,char* strColor,int FontHeight)
{
	return	m_pVisionInstance->SetDispColorHeight(hwinID,strColor,FontHeight);
}





/**********************************************************************
函数      : DispObj
功能      : 显示处理结果
输入参数  : iWinID 界面窗口ID
iSceneID  窗口序号
bIsShowTmp 调试参数，用于显示中间过程
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::DispObj(int iWinID, int iSceneID, bool bIsShowTmp)
{
	bool bRet = false;
	long halconID = -1;
	map<long ,long>::iterator it;; 
	it=m_mapWinID.find(iWinID);
	//将窗口ID转换成halcon窗口ID
	if(it!=m_mapWinID.end())
	{
		halconID = it->second;
		if (-1 != halconID)
		{
			bRet = m_pVisionInstance->DispObj(halconID,iSceneID, bIsShowTmp);
		}
	}
	return bRet;
}
/**********************************************************************
函数      : DispROI
功能      : 置为绘制ROI模式
输入参数  : iWinID 窗口ID
iSceneID  场景ID，根据该ID获取参数
index 当前场景中的第几个ROI
iType, 0,为处理图像区域, 1为模板区域，2为画直线， 3为画圆， 待改为枚举类型
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::DispROI(int iWinID, int iSceneID, int index, PROCESS_TYPE iType)
{
	bool bRet = false;
	ST_ROI_INFO stInRoiInfo;
	long halconID = -1;
	SCENE_PKG *pConfig;
	bool bIsFinde = false;
	map<long ,long>::iterator it;
	it=m_mapWinID.find(iWinID);
	bool bIsRect = true;
	//将窗口ID转换成halcon窗口ID
	if(it!=m_mapWinID.end())
	{
		halconID = it->second;
		pConfig = GetSceneHandle(iSceneID);
		if (-1 != halconID && NULL != pConfig)
		{
			int iPos = m_pVisionInstance->FindParamData(pConfig, iType, index);
			if (iPos < 0 || iPos >= pConfig->vecBody.size() || iPos >= pConfig->vecDataType.size())
			{
				return bRet;
			}
			VIRSION_DATA_PKG *pData = pConfig->vecBody[iPos];

			iPos = FindSpecTypePos(pConfig->vecBody[iPos], pConfig->vecDataType[iPos], ROI_TYPE);
			switch (iType)
			{
			case MAIN_PRO:
			case MODEL_PRO:
			case LINE_PRO:
				{
					stInRoiInfo.strect.row = pData[iPos + 0].dValue;    //ROI  row
					stInRoiInfo.strect.col = pData[iPos + 1].dValue;    //ROI  col
					stInRoiInfo.strect.angle = pData[iPos + 2].dValue;  //ROI  angle
					stInRoiInfo.strect.length1 = pData[iPos + 3].dValue;  //ROI l1
					stInRoiInfo.strect.length2 = pData[iPos + 4].dValue;  //ROI l2
					bIsFinde = true;
				}
				break;
			case CIRCLE_PRO:
				{
					stInRoiInfo.stcircle.row = pData[iPos + 0].dValue;   //ROI  row
					stInRoiInfo.stcircle.col = pData[iPos + 1].dValue;   //ROI  col
					stInRoiInfo.stcircle.Radius = pData[iPos + 2].dValue;  //ROI R 
					stInRoiInfo.stcircle.width = pData[iPos + 3].dValue;   //ROI w
					stInRoiInfo.stcircle.angleB = pData[iPos + 4].dValue;  //ROI b 
					stInRoiInfo.stcircle.angleE = pData[iPos + 5].dValue;   //ROI e
					bIsRect = false;
					bIsFinde = true;
				}
				break;
			default:
				break;
			}
			if (bIsFinde)
			{
				bRet = m_pVisionInstance->dispRoi(iSceneID,halconID , stInRoiInfo, bIsRect);
			}
			else
			{
				bRet = bIsFinde;
			}

		}
	}
	return bRet;
}
/**********************************************************************
函数      : DrawROI
功能      : 直接绘制ROI
输入参数  : iWinID 窗口ID
iSceneID  场景ID，根据该ID获取参数
iType, 画直线， 画圆，
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::OnDrawROI(int iWinID, int iSceneID, PROCESS_TYPE iType,RoiCombType CombType,vector<int>* pRoiIdList)
{
	bool bRet = false;
	ST_ROI_INFO stInRoiInfo;
	long halconID = -1;
	SCENE_PKG *pConfig;
	map<long ,long>::iterator it;
	it=m_mapWinID.find(iWinID);	 
	int  iSceneindex = iSceneID;
	//将窗口ID转换成halcon窗口ID
	if(it!=m_mapWinID.end())
	{
		halconID = it->second;
		pConfig = GetSceneHandle(iSceneID);
		if (-1 != halconID && NULL != pConfig)
		{
			int ImageWidth = 0,ImageHeight = 0;
			m_pVisionInstance->getimagesize(iSceneID,ImageWidth,ImageHeight);
			switch (iType)
			{
			case LINE_PRO:
				{
					stInRoiInfo.strect.row = ImageHeight/2;
					stInRoiInfo.strect.col = ImageWidth/2;
					stInRoiInfo.strect.angle = 0.0;
					stInRoiInfo.strect.length1 = ImageWidth / 20;
					stInRoiInfo.strect.length2 = ImageHeight /20;
				}
				break;
			case CIRCLE_PRO:
				{
					stInRoiInfo.stcircle.row =  ImageHeight/2;
					stInRoiInfo.stcircle.col = ImageWidth/2;
					stInRoiInfo.stcircle.Radius = ImageHeight/20;
					stInRoiInfo.stcircle.width =stInRoiInfo.stcircle.Radius/8; 
				}
				break;
			case COMBSHAPE_PRO:
				{

				}
				break;
			default:
				break;
			}
			bRet = m_pVisionInstance->OndrawRoi(iSceneID,halconID , stInRoiInfo, CombType, iType,pRoiIdList);
		}

	}

	return TRUE;
}



void CManagerCtrl::GenShape(int SceneID,const ST_ROI_INFO& roi,ShapeType shapeflag,int ShapeId,int CamIndex)
{
	string strCameraGuid;
	int widthI,heightI;
	strCameraGuid = m_pInstance->m_stSysParam.pstrCamareGUID[CamIndex];  

	//通过索引获取长宽,若为读取都pain这里就没有长宽了
	if (CAMERA_SUCCESS == getImageSize(strCameraGuid, widthI, heightI)) 
	{
		m_pVisionInstance->GenShape(SceneID,roi,shapeflag,ShapeId,widthI,heightI);

	}
}



int CManagerCtrl::GetCurShapeId( int iSceneID)
{

	return m_pVisionInstance->GetCurShapeId(iSceneID);

}

bool  CManagerCtrl::GetRoiParam(int sceneID,int ShapeId,ST_ROI_INFO* pRoi)
{

	return m_pVisionInstance->GetRoiParam(sceneID,ShapeId,pRoi);

}



void CManagerCtrl::OnShowAllRoi(int iWinID, int iSceneID,const vector<int>* const pRoiDdList)
{
	long halconID = -1;
	//SCENE_PKG *pConfig;
	map<long ,long>::iterator it;
	it=m_mapWinID.find(iWinID);
	//bool bIsRect = true;

	//int  iSceneindex = iSceneID;
	//将窗口ID转换成halcon窗口ID
	if(it!=m_mapWinID.end())
	{
		halconID = it->second;
		m_pVisionInstance->OnShowAllRoi(iSceneID,halconID,pRoiDdList);

	}

}


/**********************************************************************
函数      : DrawROI
功能      : 置为绘制ROI模式
输入参数  : iWinID 窗口ID
iSceneID  场景ID，根据该ID获取参数
index 当前场景中的第几个ROI
iType, 0,为处理图像区域, 1为模板区域，2为画直线， 3为画圆， 待改为枚举类型
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::DrawROI(int iWinID, int iSceneID, int index, PROCESS_TYPE iType)
{
	bool bRet = false;
	ST_ROI_INFO stInRoiInfo;
	ST_ROI_INFO stOutRoiInfo;
	long halconID = -1;
	SCENE_PKG *pConfig;
	map<long ,long>::iterator it;
	it=m_mapWinID.find(iWinID);
	bool bIsRect = true;
	int  iSceneindex = iSceneID;
	//将窗口ID转换成halcon窗口ID
	if(it!=m_mapWinID.end())
	{
		halconID = it->second;
		pConfig = GetSceneHandle(iSceneID);
		int iPos = m_pVisionInstance->FindParamData(pConfig, iType, index);


		if (iPos >= pConfig->vecBody.size() || iPos >= pConfig->vecDataType.size())
		{

			return false;
		}
		VIRSION_DATA_PKG *pData = pConfig->vecBody[iPos];
		if (-1 != halconID && NULL != pConfig)
		{
			//在当前功能组下找特定类型参数第一次出现的位置
			iPos = FindSpecTypePos(pConfig->vecBody[iPos], pConfig->vecDataType[iPos], ROI_TYPE);
			//查找场景ID是否有效
			switch (iType)
			{
				//case MAIN_PRO:
				//    {
				//        stInRoiInfo.strect = pConfig->vecImageProInfo[index].roiRectangle;
				//    }
				//    break;
			case MODEL_PRO:
			case LINE_PRO:
				{
					stInRoiInfo.strect.row = pData[iPos].dValue;
					stInRoiInfo.strect.col = pData[iPos + 1].dValue;
					stInRoiInfo.strect.angle = pData[iPos + 2].dValue;
					stInRoiInfo.strect.length1 = pData[iPos + 3].dValue;
					stInRoiInfo.strect.length2 = pData[iPos + 4].dValue;
				}
				break;
			case CIRCLE_PRO:
				{
					stInRoiInfo.stcircle.row = pData[iPos + 0].dValue;
					stInRoiInfo.stcircle.col = pData[iPos + 1].dValue;
					stInRoiInfo.stcircle.Radius = pData[iPos + 2].dValue;
					stInRoiInfo.stcircle.width = pData[iPos + 3].dValue;
					bIsRect = false;
				}
				break;
			default:
				break;
			}
			bRet = m_pVisionInstance->drawRoi(iSceneID,halconID , stInRoiInfo, stOutRoiInfo, bIsRect);

			//待优化
			switch (iType)
			{
				//case MAIN_PRO:
				//    {
				//        m_vecSceneParam[iSceneindex].vecImageProInfo[index].roiRectangle = stOutRoiInfo.strect;
				//    }
				//    break;
			case MODEL_PRO:
			case LINE_PRO:
				{
					pData[iPos + 0].dValue = stOutRoiInfo.strect.row;
					pData[iPos + 1].dValue = stOutRoiInfo.strect.col;
					pData[iPos + 2].dValue = stOutRoiInfo.strect.angle;
					pData[iPos + 3].dValue = stOutRoiInfo.strect.length1;
					pData[iPos + 4].dValue = stOutRoiInfo.strect.length2;
				}
				break;
			case CIRCLE_PRO:
				{
					pData[iPos + 0].dValue = stOutRoiInfo.stcircle.row;
					pData[iPos + 1].dValue = stOutRoiInfo.stcircle.col;
					pData[iPos + 2].dValue = stOutRoiInfo.stcircle.Radius;
					pData[iPos + 3].dValue = stOutRoiInfo.stcircle.width;
					bIsRect = false;
				}
				break;
			default:
				break;
			}
		}
	}
	return bRet;
}

/**********************************************************************
函数      : CreateVisionModel
功能      : 创建模板
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数
index 场景中图像序号  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::CreateVisionModel(int iWinID,int iSceneID, int index)
{
	bool bRet = false;
	SCENE_PKG *pConfig;
	char szBuf[512] = {0};
	//查找场景ID是否有效
	pConfig = GetSceneHandle(iSceneID);
	int iPos = m_pVisionInstance->FindParamData(pConfig, MODEL_PRO, index);
	if (NULL != pConfig && iPos >= 0 && iPos < pConfig->vecBody.size() && iPos < pConfig->vecDataType.size())
	{
		long halconID = -1;
		map<long ,long>::iterator it;; 
		it=m_mapWinID.find(iWinID);
		//将窗口ID转换成halcon窗口ID
		if(it!=m_mapWinID.end())
		{
			halconID = it->second;
		}
		int iModelID = -1;
		get_module_path((LPSTR)szBuf);
		sprintf_s(szBuf, "%s%s\\model%d", szBuf,m_stSysParam.pstrSceneName[iSceneID].c_str(), index);
		//调用图像处理接口创建ROI //待修改
		iModelID = m_pVisionInstance->buildModel(halconID,iSceneID,index,pConfig,pConfig->szSceneName,szBuf);
		if ( -1 != iModelID)
		{
			//在当前功能组下找特定类型参数第一次出现的位置
			VIRSION_DATA_PKG *pData = pConfig->vecBody[iPos];
			int idPos = FindSpecTypePos(pConfig->vecBody[iPos], pConfig->vecDataType[iPos], ID_TYPE);
			if (idPos >= 0 && -1 != pData[idPos].iValue)
			{
				int iType = FindSpecTypePos(pConfig->vecBody[iPos], pConfig->vecDataType[iPos], SPEC_TYPE);
				m_pVisionInstance->clearModel(iSceneID,pData[idPos].iValue, pData[iType].iValue);
			}
			pData[idPos].iValue = iModelID;
		}
	}
	return bRet;
}

bool CManagerCtrl::CreateVisionModel(int iWinID,int iSceneID,int RoiID,char* modePath, int Nevels,double AngleStart,double AngleExtent,
	int Contrast,int index )
{
	return  m_pVisionInstance->buildModel( iWinID, iSceneID, RoiID, modePath,  Nevels, AngleStart, AngleExtent,Contrast, index);
}


/**********************************************************************
函数      : UpdateVisionModel
功能      : 更新模板
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
int CManagerCtrl::UpdateVisionModel(int iSceneID, int index, int iType)
{
	int iRet = false;
	SCENE_PKG *pConfig;
	char szBuf[512] = {0};
	//查找场景ID是否有效
	pConfig = GetSceneHandle(iSceneID);
	if (NULL != pConfig)
	{
		char szBuf[256] = {0};
		GetModuleFileName(NULL, szBuf, 255);
		string strFilePath = szBuf;
		int lastSpit = (int)strFilePath.find_last_of('\\');
		strFilePath.replace(lastSpit, string::npos, "");//加载的过程处理路径	
		sprintf_s(szBuf, "%s\\%s\\mypic",strFilePath.c_str(),pConfig->szSceneName);
		sprintf_s(szBuf, "%s\\%s\\model%d", strFilePath.c_str(),m_stSysParam.pstrSceneName[iSceneID].c_str(), index);
		iRet = m_pVisionInstance->readModel(iSceneID,pConfig->szSceneName,szBuf, index, iType);
	}
	return iRet;
}
/**********************************************************************
函数      : FindVisionModel
功能      : 查找模板
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::FindVisionModel(int iSceneID, int index, bool bIsB)
{
	bool bRet = false;
	SCENE_PKG *pConfig = NULL;
	char szBuf[512] = {0};
	//查找场景ID是否有效
	pConfig = GetSceneHandle(iSceneID);
	if (NULL != pConfig)
	{
		ST_RECTANGLE_DATA rtData;
		int Score = 0;
		//判断模板ID是否有效
		int iPos = FindParamData(pConfig, MODEL_PRO, index);
		if (iPos >= pConfig->vecBody.size() || iPos >= pConfig->vecDataType.size())
		{
			return false;
		}
		int iW = FindSpecTypePos(pConfig->vecBody[iPos], pConfig->vecDataType[iPos], ID_TYPE);
		int iTypeP = FindSpecTypePos(pConfig->vecBody[iPos], pConfig->vecDataType[iPos], SPEC_TYPE);
		if (0 > pConfig->vecBody[iPos][iW].iValue)
		{
			int iType = pConfig->vecBody[iPos][iTypeP].iValue;
			pConfig->vecBody[iPos][iW].iValue = UpdateVisionModel(iSceneID, index, iType);
		}
		//调用图像处理接口获取模板
		m_pVisionInstance->findModel(iSceneID, index,pConfig, rtData, 0);
		if (bIsB && pConfig->vecBody[iPos][iW].iValue >= 0)
		{
			iW = FindSpecTypePos(pConfig->vecBody[iPos], pConfig->vecDataType[iPos], ROI_TYPE);
			pConfig->vecBody[iPos][iW + 5].dValue = rtData.row;
			pConfig->vecBody[iPos][iW + 6].dValue = rtData.col;
			pConfig->vecBody[iPos][iW + 7].dValue = rtData.angle;
			SetSceneParam(pConfig);
		}
	}
	return true;
}
/**********************************************************************
函数      : FindWay2VisionModel
功能      : 查找模板
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::FindWay2VisionModel(int iSceneID, int WinID,int index,char* szModePath,int PyramidLevel,int MatchNum,double& Score,
	int ProcIndex,bool IsBulid,double& row,double& col,double& phi)
{

	bool bRet = false;
	SCENE_PKG *pConfig = NULL;
	char szBuf[512] = {0};
	//查找场景ID是否有效
	//pConfig = GetSceneHandle(iSceneID);
	return m_pVisionInstance->FindWay2VisionModel(iSceneID,WinID,index,szModePath,PyramidLevel,MatchNum,Score, ProcIndex,IsBulid,row,col,phi);

}
bool CManagerCtrl::FindModelRealTime(int iSceneID, int WinID,int index,char* szModePath,int PyramidLevel,int MatchNum,double& Score,
	int ProcIndex,bool IsBulid,double& row,double& col,double& phi)
{

	bool bRet = false;
	SCENE_PKG *pConfig = NULL;
	char szBuf[512] = {0};
	//查找场景ID是否有效
	//pConfig = GetSceneHandle(iSceneID);
	return m_pVisionInstance->FindModelRealTime(iSceneID,WinID,index,szModePath,PyramidLevel,MatchNum,Score, ProcIndex,IsBulid,row,col,phi);

}

void CManagerCtrl::MoveAndRotInCamForOriMatch(int iSceneID,double PixSize,bool AxeMoveRotDirect,double& MotorRelX,
	double& MotorRelY,int LastSceneId,int NextSceneId,double SceneRotPhi,double PicCentX,double PicCentY,double RotCenX,double RotCenY,double MotorRotPhi,
	double CalRotPhi,double (*pCamParam),int ParamNum,double (*pCamPose),double CalThick,int CamIndex,
	int AxeIndex,int ToolIndex)
{
	m_pVisionInstance->MoveAndRotInCamForOriMatch(iSceneID,PixSize,AxeMoveRotDirect,MotorRelX, MotorRelY,
		LastSceneId,NextSceneId,SceneRotPhi,PicCentX,PicCentY,RotCenX, RotCenY, MotorRotPhi,CalRotPhi,pCamParam,ParamNum,pCamPose,CalThick,
		CamIndex, AxeIndex, ToolIndex);
}

bool CManagerCtrl::AffinePointByCamInMecRot(int CamIndex,int AxisIndex,int ToolIndex,double& PointX,double& PointY)
{
	return m_pVisionInstance->AffinePointByCamInMecRot( CamIndex, AxisIndex,ToolIndex, PointX, PointY);

}

void CManagerCtrl::AffToRobotWorld(int SceneID,int CamIndex,int AxisIndex,int ToolIndex,double PixSize,double ModRotPhi,
	double& ToolWorldX,double& ToolWorldY,bool WithCamParam,double CalThickness,double* pCamParam,int ParamNum,double* CamPos)
{

	m_pVisionInstance->AffToRobotWorld(SceneID,CamIndex,AxisIndex,ToolIndex,PixSize,ModRotPhi,ToolWorldX,ToolWorldY,
		WithCamParam,CalThickness,pCamParam,ParamNum,CamPos);

}


void CManagerCtrl::AffinePointToWorld(int CamIndex,int AxisIndex,int ToolIndex,double PixSize,double OriCamX,double OriCamY,
	double OriCamPhi,double CurCamX,double CurCamY,double CurCamPhi,double& MoveX,double& MoveY,int ResType,
	bool MoveRotDirect,double CurMatchPhi,double AxeRotX,double AxeRotY)
{

	m_pVisionInstance->AffinePointToWorld(CamIndex,AxisIndex,ToolIndex,PixSize,OriCamX, OriCamY,
		OriCamPhi,CurCamX, CurCamY, CurCamPhi,MoveX,MoveY,ResType,MoveRotDirect, CurMatchPhi, AxeRotX, AxeRotY);
}



/**********************************************************************
函数      : VisionProcess
功能      : 整体检测
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::VisionGetResult(int iSceneID, int index,CHECK_RESULT &result)
{
	bool bRet = false;
	SCENE_PKG *pConfig = NULL;
	/*CHECK_RESULT stresult;*/
	vector<SCENE_PKG>::iterator it;
	bool bIsFinde = false;
	//查找场景ID是否有效
	//int IntTest = 8;
	pConfig = GetSceneHandle(iSceneID);
	if (NULL == pConfig)
	{
		return false;
	}
	//判断处理的方式


	if (0 == pConfig->ProcProcess )
	{
		if(m_pInstance->m_SceneProcess(iSceneID,result))
		{	
			return true;
		}

		return false;
	}

	////如果模板没加载则加载一次, 模板内存一般较大
	int iPos = 0;
	int iTPos = 0;
	int indexM = 0;
	do 
	{
		iPos = FindParamData(pConfig, MODEL_PRO, indexM);
		if (0 > iPos ||iPos >= pConfig->vecBody.size() || iPos >= pConfig->vecDataType.size())
		{
			break;
		}
		iTPos = FindSpecTypePos(pConfig->vecBody[iPos], pConfig->vecDataType[iPos], ID_TYPE);
		if (iPos >= pConfig->vecBody.size() || iPos >= pConfig->vecDataType.size())
		{
			break;
		}
		int iID = pConfig->vecBody[iPos][iTPos].iValue;
		int iTypeP = FindSpecTypePos(pConfig->vecBody[iPos], pConfig->vecDataType[iPos], SPEC_TYPE);
		if(0 > pConfig->vecBody[iPos][iTPos].iValue)
		{
			int iType = pConfig->vecBody[iPos][iTypeP].iValue;
			pConfig->vecBody[iPos][iTPos].iValue = UpdateVisionModel(iSceneID, indexM, iType);
		}
		indexM++;
	} while (1);
	m_pVisionInstance->process(iSceneID,pConfig,result);
	if (result.err < 0)
	{
		bRet = false;
	}
	else
	{
		bRet = true;
	}
	return bRet;

}

/**********************************************************************
函数      : VisionDebugThreshold
功能      : 调试阈值接口
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
index， 根据该index获取找圆参数
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::VisionDebugThreshold(int iWinID, int iSceneID, int index)
{
	ST_LINE stresult;
	SCENE_PKG *pConfig;
	vector<SCENE_PKG>::iterator it;
	bool bIsFinde = false;
	long halconID = -1;
	int iMin = 0;
	int iMax = 255;
	int iCount = 0;
	map<long ,long>::iterator itW;
	itW=m_mapWinID.find(iWinID);
	//将窗口ID转换成halcon窗口ID
	if(itW!=m_mapWinID.end())
	{
		halconID = itW->second;
	}
	int iPos = 0;
	pConfig = GetSceneHandle(iSceneID);

	//找到最大阈值最小阈值
	iPos = m_pVisionInstance->FindParamData(pConfig, THRE_PRO, index);
	if (NULL == pConfig || iPos < 0 || iPos >= pConfig->vecBody.size() || iPos >= pConfig->vecDataType.size())
	{
		return false;
	}
	VIRSION_DATA_PKG *pData = pConfig->vecBody[iPos];
	unsigned int *pType = pConfig->vecDataType[iPos];
	if (index > 0)
	{
		iPos = FindSpecTypePos(pData, pType, THRE_TYPE);
		if (iPos >= sizeof(pData))
		{
			return false;
		}
		iMax = pData[iPos].iValue;
		iPos = m_pVisionInstance->FindParamData(pConfig, THRE_PRO, index - 1);
		if (iPos >= pConfig->vecBody.size() || iPos >= pConfig->vecDataType.size())
		{
			return false;
		}
		pData = pConfig->vecBody[iPos];
		pType = pConfig->vecDataType[iPos];
		iPos = FindSpecTypePos(pData, pType, THRE_TYPE);
		if (iPos >= sizeof(pData))
		{
			return false;
		}
		iMin = pData[iPos].iValue;
	}
	else
	{
		iPos = FindSpecTypePos(pData, pType, THRE_TYPE);
		if (iPos >= sizeof(pData))
		{
			return false;
		}
		iMin = pData[iPos]. iValue;
		iPos = m_pVisionInstance->FindParamData(pConfig, THRE_PRO, index + 1);
		if (iPos >= 0 && iPos < pConfig->vecBody.size() && iPos < pConfig->vecDataType.size())
		{
			pData = pConfig->vecBody[iPos];
			pType = pConfig->vecDataType[iPos];
			iPos = FindSpecTypePos(pData, pType, THRE_TYPE);
			iMax = pData[iPos].iValue;
		}
	}

	m_pVisionInstance->DebugThreshold(halconID,iSceneID, iMin, iMax);
	return true;
}

/**********************************************************************
函数      : VisionFindLine
功能      : 找线接口
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
index， 根据该index获取找圆参数
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::VisionFindLine(int iSceneID, int index, bool bIsFollow)
{
	ST_LINE stresult;
	SCENE_PKG *pConfig;
	//ST_FIND_LINE data;
	pConfig = GetSceneHandle(iSceneID);
	/* int iTPos = 0;*/

	//没找到有效场景则退出
	int iPos = m_pVisionInstance->FindParamData(pConfig, LINE_PRO, index);

	if (NULL == pConfig || iPos < 0)
	{
		return false;
	}

	m_pVisionInstance->findline(iSceneID,pConfig,index, stresult, bIsFollow);
	return true;
}
/**********************************************************************
函数      : VisionFindCircle
功能      : 找圆接口
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
index， 根据该index获取找圆参数
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::VisionFindCircle(int iSceneID, int index, bool bIsFollow)
{
	ST_CIRCLE stresult;
	SCENE_PKG *pConfig;
	//ST_FIND_CIRCLE data;
	pConfig = GetSceneHandle(iSceneID);
	int iPos = 0;
	//int iTPos = 0;  //特定功能功位置
	iPos = m_pVisionInstance->FindParamData(pConfig, CIRCLE_PRO, index);

	if (NULL == pConfig || iPos < 0)
	{
		return false;
	}
	m_pVisionInstance->findcircle(iSceneID,pConfig,index,stresult, bIsFollow);
	return true;
}


bool CManagerCtrl::VisionMeasureCircle(long HwinID,int iSceneID, int Imageindex,int MatchIndex, double& Row,double& Col,double& Rii)
{
	//ST_CIRCLE stresult;
	//SCENE_PKG *pConfig;
	return m_pVisionInstance->MeasureCircle(HwinID,iSceneID,Imageindex,MatchIndex,Row, Col,Rii);

}





bool CManagerCtrl::ClearWindow(int iWindowID)
{
	bool bRet = false;
	long halconID = -1;
	map<long ,long>::iterator it;; 
	it=m_mapWinID.find(iWindowID);
	//将窗口ID转换成halcon窗口ID
	if(it!=m_mapWinID.end())
	{
		halconID = it->second;
		if (-1 != halconID)
		{
			bRet = m_pVisionInstance->clearWindow(halconID);
		}
	}
	return bRet;
}

SYSTEM_PARAM   CManagerCtrl::m_stSysParam;

int WINAPI CManagerCtrl::RemoteManualMsg( unsigned int wParam, void * lParam, void* otherparam)
{
	// //只有图像处理第二个参数才是场景号，其他处理发信息的第二个参数是结构体。
	//1， 获取场景号 
	int TelSceneNo;
	if (MSG_COM_COMMUNITE == wParam || MSG_SCENE_BUSY == wParam)
	{
		TelSceneNo = *(int*)lParam;
	}
	else if(  RECV_UI_LOGIN_MSG == wParam || MSG_LOGIN_SYSTEM== wParam)
	{
		RemoteSystemMsg(wParam, lParam);	
		return 0;
	}
	else
	{
		TelSceneNo = *(int*)lParam;//((CALIB_DATA*)lParam)->iSceneID;
	}
	//2，对场景号进行映射 通讯场景ID变成了实际场景ID: 通讯id可能只有1个位0，但是不同的产品对应的id不同，不可能换产品就去换配置文件
	int MapSceneNo = m_pInstance->MapSceneIdTelToReal(TelSceneNo);
	if (MapSceneNo>=0)
	{
		TelSceneNo = MapSceneNo;
	}
	//3,重新赋值场景号
	if (MSG_COM_COMMUNITE == wParam )
	{
		SCENE_PKG *pConfig = NULL;
		pConfig = GetSceneHandle(TelSceneNo);
		wParam = pConfig->SceneType;      //这里有必要这样分吗?11为处理,10为标定
		CALIB_DATA CommuniteData;
		CommuniteData.iSceneID = TelSceneNo;
		lParam = &CommuniteData;
	}else
	{
		((CALIB_DATA*)lParam)->iSceneID = TelSceneNo;
	}
	RemoteSystemMsg(wParam, (void *)lParam);
	return 0;
}

void CManagerCtrl::RemoteSystemMsg(int iType, void *param)
{
	//结束管理线程
	unsigned long MainWindID = 0;
	unsigned long MessageID = 0;
	CManagerCtrl *pInstance = NULL;
	pInstance = GetInstance();
	if (NULL == pInstance)
	{
		return;
	}

	switch (iType)   //iType
	{
	case WORKING_CAMERA:
		{
			memset(m_pInstance->m_chSN,0,64);
			CALIB_DATA Data;
			memcpy(&Data, (CALIB_DATA *)param, sizeof(CALIB_DATA));
			for (int i = 0; i<strlen(Data.chSN); i++)
			{
				m_pInstance->m_chSN[i] = Data.chSN[i];
			}
			m_pInstance->TriggerVisionProcess(Data.iSceneID, *(int *)param);
		}
		break;
	case RECV_UI_LOGIN_MSG:
		{
			pInstance->m_WorkingStatus = *(int *)param;
		}
		break;
	case CLOSE_SYS:
		{
			pInstance->UnInitManager();
			// msg_post(MSG_CLOSE_SYSTEM, 0, 0);  //广播退出系统消息
		}
		break;
	case RESET_SYS:
		{
			pInstance->UnInitManager();
			//            msg_post(MSG_RESET_SYSTEM, 0, 0);  //广播重置消息
		}
		break;
	case CALIB_CAMERA:
		{
			CALIB_DATA *pData = (CALIB_DATA *)param;
			memcpy(&pInstance->m_CalibData, pData, sizeof(CALIB_DATA));

			bool bRet = false;
			//启动检测(线程执行);
			HANDLE handle = INVALID_HANDLE_VALUE;

			handle = (HANDLE)_beginthreadex(NULL,0,(unsigned (__stdcall *)(void*))CManagerCtrl::ThreadCalibProcess,(void *)&pInstance->m_CalibData,0,NULL);
			//后续不需要操作句柄， 创建后提前释放
			if (INVALID_HANDLE_VALUE != handle)
			{
				CloseHandle(handle);
			}
		}
	case CALIB_ROTTOOL:  
		{

		}
		break;
	case RECV_REMOTE_MSG:
		{
			SetEvent(m_pInstance->m_RecvEvent);
		}
		break;
	case MSG_SCENE_BUSY:
		{
			int iSceneID = *(int *)param;
			int iErrorCode = -1;
			char szErr[126] = {0};
			CHECK_RESULT result;
			SCENE_PKG *pConfig = NULL;
			pConfig = GetSceneHandle(iSceneID);
			result.SeneTpye = pConfig->SceneType;      //这里有必要这样分吗?11为处理,10为标定
			for (int i = 0;i<RESULT_NUMBER;i++)
			{
				memset(result.szValue[i],0,BUF_SIZE/2);
			}
			if (WORKING_CAMERA ==result.SeneTpye)
			{
				result.iCount = 4;
			}
			else if (CALIB_CAMERA ==result.SeneTpye)
			{
				result.iCount = 6;
			}
			else
			{
				result.iCount = RESULT_NUMBER;
			}
			sprintf_s(szErr, "当前场景(%d)正在处理中,先返回OK", iSceneID);
			msg_post(MSG_DISP_WORKING_INFO, szErr, (unsigned int)strlen(szErr) + 1, MODULE_SCENE_MAIN_UI);          
			//result.err = -1;
			//m_pInstance->ManagerProcFinish(iSceneID, &result, iErrorCode);

			//正在处理时又收到了同场景的命令，暂时先返回拍照ok
			GrabFinished(iSceneID, &result,0);

		}
		break;
	case MSG_SEND_IMAGE_PATH:
		{
			int scenceID = *(int*)param;
			int iErrorCode = -1;
			char* ImgPath = NULL;
			ImgPath = m_pInstance->getImagePath(scenceID,0,0);
			if (NULL != ImgPath)
			{
				iErrorCode = 0;
			}
			getImagePathFinished(scenceID, ImgPath,iErrorCode);
		}
		break;

	default:
		{

		}
		break;
	}
}

/**********************************************************************
函数      : OpenContinousGrab
功能      : 开户实时显示
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
通知窗口显示处理的消息ID
输出参数  : N/A
返回值    : 成功或失败
备注      : 打开实时后， 该接口注册回调给实时采集， 回调中通知窗口显示图像
消息ID   
**********************************************************************/
bool CManagerCtrl::OpenContinousGrab(int iWindowID, int iSceneID, int index, unsigned int MsgID,int CameraIndex)
{
	bool bRet = false;
	//创建线程, 通知主进程调用图像显示接口显示图像
	//获取当前场景对应相机索引
	int iCameraIndex = 0;
	char *pBuf = NULL;
	int  iWidth = 0;
	int  iHeight = 0;
	vector<SCENE_PKG>::iterator it;
	SCENE_PKG *pConfig = NULL;
	char szBuf[126] = {0};
	LOG_INFO("Open Continous Grab, Scene ID is %d", iSceneID);
	pConfig = GetSceneHandle(iSceneID);
	if ( 0 == pConfig->ProcProcess)
	{
		iCameraIndex = CameraIndex;

	}	
	if ( 0 > iCameraIndex)
	{
		return false;
	}
	int iRet = getImageSize(m_stSysParam.pstrCamareGUID[iCameraIndex],iWidth,iHeight);
	if (CAMERA_SUCCESS  == iRet)
	{
		pBuf = m_pVisionInstance->GetImageBuf(iSceneID, index, iWidth, iHeight);
		if (pBuf == NULL)
		{
			return false;
		}
		startSnap(m_stSysParam.pstrCamareGUID[iCameraIndex], 
			pBuf/*图像内存指针*/, PostMsgToUIForDisp, (void *)iSceneID/*场景信息*/);
		bRet = true;
	}
	return bRet;
}

/**********************************************************************
函数      : ColseContinousGrab
功能      : 关闭实时显示
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::ColseContinousGrab(int iSceneID, int index,int CameraIndex)
{
	bool bRet = false;
	//通知图像处理结束实时采集
	SCENE_PKG *pConfig = NULL;
	SYSTEM_PARAM param;
	int iCameraIndex = 0;
	char szBuf[126] = {0};
	pConfig = GetSceneHandle(iSceneID);

	if ( 0 == pConfig->ProcProcess)
	{
		iCameraIndex = CameraIndex;

	}else
	{
		if (  NULL != pConfig && index < pConfig->vecInfo.size())
		{

			iCameraIndex = pConfig->vecInfo[index].CameraIndex;
			//关闭光源
			if(LightCtrl(pConfig->vecInfo[index].LightIndex, 0))
			{
				sprintf_s(szBuf, "关闭光源成功(%d)", pConfig->vecInfo[index].LightIndex);
				msg_post(MSG_DISP_WORKING_INFO, szBuf,(unsigned int)strlen(szBuf)+1, MODULE_SCENE_MAIN_UI); 
			}
			else
			{
				sprintf_s(szBuf, "关闭光源失败(%d)", pConfig->vecInfo[index].LightIndex);
				msg_post(MSG_DISP_WORKING_INFO, szBuf,(unsigned int)strlen(szBuf)+1, MODULE_SCENE_MAIN_UI); 
			}
		}else
		{
			return false;
		}

	}

	if ( 0 > iCameraIndex)
	{
		return false;
	}

	if (CAMERA_SUCCESS == stopSnap(m_stSysParam.pstrCamareGUID[iCameraIndex]))
	{
		bRet = true;
	}

	return bRet;
}


/**********************************************************************
函数      : PostMsgToUIForDisp
功能      : 显示图像接口
输入参数  : context, 抓图得到的图像 
输出参数  : N/A
返回值    : N/A
备注      : 
**********************************************************************/
void CManagerCtrl::PostMsgToUIForDisp(void* context)
{
	// bool bRet = true;
	//unsigned int sceneID = (unsigned int)context;
	//拷贝图像
	//通知UI显示图像, 待修改为向UI发送消息通知UI更新
	//m_pInstance->m_pVisionInstance->DispImage(pConfig->widID, pConfig->sceneID);
	//需要把工站索引发给UI， 
	msg_post(MSG_DISP_IMAGE, context, sizeof(unsigned int), MODULE_SCENE_DEBUG_UI);
}

SCENE_PKG *CManagerCtrl::GetSceneHandle(int iSceneID)
{
	//step1 根据场景ID找到对应配置
	bool bIsFinde = false;
	SCENE_PKG *pConfig = NULL;
	vector<SCENE_PKG *>::iterator it;
	for (it = m_pInstance->m_vecSceneParam.begin(); 
		it != m_pInstance->m_vecSceneParam.end(); it++)
	{
		if (iSceneID == (*it)->sceneID)
		{
			pConfig = (*it);
			bIsFinde = true;
			break;
		}
	}
	//没找到合适场景
	if (!bIsFinde)
	{
		pConfig = NULL;
	}
	return pConfig;
}

/**********************************************************************
函数      : GetCurrentStatus
功能      : 获取指定功能初始化结果
输入参数  : ID,  各功能对应标识
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::GetCurrentStatus(unsigned long ID)
{
	bool bRet = false;
	unsigned long flag = m_CurrentError;
	flag = flag & ID;
	if (0 != flag)
	{
		bRet = true;
	}
	return bRet;
}

void CManagerCtrl::RestoreImagePart(int WinID, int iScene, int width, int height, int index)
{
	bool bRet = false;
	int widthI = 0;
	int heightI = 0;
	int widthP = 0;
	int heightP = 0;
	int x = 0;
	int y = 0;
	map<long ,long>::iterator itID;
	itID = m_mapWinID.find(WinID);
	if(itID==m_mapWinID.end())
	{
		return;
	}
	string strCameraGuid;
	strCameraGuid = m_pInstance->m_stSysParam.pstrCamareGUID[index];  

	if (CAMERA_SUCCESS != getImageSize(strCameraGuid, widthI, heightI))
	{
		SCENE_PKG *p = NULL;
		p = GetSceneHandle(iScene); //不用拷贝；//窗口不应该跟场景相关
		if (p)
		{
			widthI = (int)p->defcamwidth;
			heightI = (int)p->defcamheight;
		}

	}
	bRet = true;
	if (heightI > 0 && height > 0)
	{
		double wd = (double)width/widthI;
		double hd = (double)height/heightI;
		if (wd < hd)
		{
			widthP = widthI;
			heightP = widthI * height/width;
			x = 0;
			y = -(heightP - heightI)/2;
			heightP += y;
		}
		else
		{
			widthP = width * heightI / height;
			heightP = heightI;
			x = -(widthP - widthI)/2;
			y = 0;
			widthP += x;
		}
	}
	else
	{
		widthP = widthI;
		heightP = heightI;
	}
	if (bRet && width > 0 && height > 0)
	{
		m_pVisionInstance->setpart(itID->second, x, y, widthP, heightP);
		//m_pVisionInstance->setpart(itID->second, 0, 0, width, height);
	}
}
//图像缩放拖拽
//1. 窗口ID, 
//2. 拖拽类型（a, 左;b, 右;c, 上;d, 下;e, 放大;f, 缩小, g, 还原）
//3. 鼠标前后位置(基于当前控制上的位置)
//4. 缩放值
void CManagerCtrl::UpdateImagePart(int WinID, int iScene, int iType, double dXPer, double dYPer)
{
	int xb = 0;
	int yb = 0;
	int xe = 0;
	int ye = 0;
	int xm = 0;
	int ym = 0;
	int width = 0;
	int height = 0;
	double dP = 1;
	map<long ,long>::iterator itID;
	itID = m_mapWinID.find(WinID);
	if(itID==m_mapWinID.end())
	{
		return;
	}
	m_pInstance->m_pVisionInstance->getpart(itID->second, xb, yb, xe, ye);
	m_pVisionInstance->getimagesize(iScene, width, height);
	double xP = (double)(xe - xb)/width;
	double yP = (double)(ye - yb)/height;
	double dPer = (xP + yP) / 2;
	switch(iType)
	{
	case MOVE_TPYE:
		{
			xm = (int)(width * dXPer);
			ym = (int)(height * dYPer);
			xb -= xm;
			yb -= ym;
			xe -= xm;
			ye -= ym;
		}
		break;
		//放大
	case ENLARGE_TYPE:
		{
			dPer -= 0.1;
			xm = (int)(width * (dP- dPer));
			ym = (int)(height * (dP - dPer));
			xb = (int)(xm * dXPer);
			yb = (int)(ym * dYPer);
			xe = width - (int)(xm *(dP - dXPer));
			ye = height - (int)(ym *(dP - dYPer));
		}
		break;
	case SHRINK_TYPE:
		{
			dPer += 0.1;
			xm = (int)(width * (dP- dPer));
			ym = (int)(height * (dP - dPer));
			xb = (int)(xm * dXPer);
			yb = (int)(ym * dYPer);
			xe = width - (int)(xm *(dP - dXPer));
			ye = height - (int)(ym *(dP - dYPer));
		}
		break;
		//还原
	case RESTORE_TYPE:
		{
			xb = 0;
			yb = 0;
			xe = width;
			ye = height;
		}
		break;
	default:
		break;
	}
	if (xb < xe && yb < ye)
	{

		m_pInstance->m_pVisionInstance->setpart(itID->second, xb, yb, xe, ye);
	}
}

/**********************************************************************
函数      : SetCurrentStatus
功能      : 设置指定功能初始化成功
输入参数  : ID,  各功能对应标识
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::SetCurrentStatus(unsigned long ID)
{
	bool bRet = true;
	m_CurrentError = m_CurrentError | ID;
	return bRet;
}
/**********************************************************************
函数      : DelCurrentStatus
功能      : 取消指定功能初始化成功标识
输入参数  : ID,  各功能对应标识
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::DelCurrentStatus(unsigned long ID)
{
	bool bRet = true;
	unsigned long flag = ID;
	flag = ~flag;
	m_CurrentError = m_CurrentError & flag;
	return bRet;
}

/**********************************************************************
函数      : AffineTransPoint
功能      : 坐标系转换， 将图像坐标根据标定数据转换为标定时机器人坐标系中
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
resultIn 转换前的值
resultOut 转换后的值
输出参数  : N/A
返回值    : 成功或失败
备注      : gpz 这个result中有几个有效数据并不一定
**********************************************************************/
bool CManagerCtrl::AffineTransPoint(int iSceneID, int iCameraID, const CHECK_RESULT resultIn,CHECK_RESULT &resultOut)
{
	bool bRet = true;
	SCENE_PKG *pConfig = NULL;
	AF_POINT  pointIn;
	AF_POINT  pointOut;
	char szBuf[256] = {0};
	pConfig = GetSceneHandle(iSceneID);
	if (NULL != pConfig && pConfig->bSave)
	{
		pointIn.x = atof(resultIn.szValue[0]);
		pointIn.y = atof(resultIn.szValue[1]);
		pointIn.angle = atof(resultIn.szValue[2]);
		m_pVisionInstance->affine_apply(iSceneID, iCameraID, pConfig->szSceneName, pointIn, pointOut);
		sprintf(resultOut.szValue[0], "%lf", pointOut.x);
		sprintf(resultOut.szValue[1], "%lf", pointOut.y);
		sprintf(resultOut.szValue[2], "%lf", pointOut.angle);
	}
	return bRet;
}

/**********************************************************************
函数      : OpenCheckStateThread
功能      : 打开检测系统状态线程
输入参数  :   

输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
bool CManagerCtrl::OpenCheckStateThread()
{
	HANDLE CheckThread=CreateThread(NULL,0,CheckStateThread,NULL,0,NULL);
	CloseHandle(CheckThread);
	return true;
}

DWORD WINAPI CManagerCtrl::CheckStateThread(LPVOID lpParameter)
{

	while (NULL != m_pInstance && m_pInstance->m_iExitThread)
	{
		if (WAIT_OBJECT_0 != WaitForSingleObject(m_pInstance->m_RecvEvent, HEART_BEAT_TIME))
		{

			int reval = 1;
			if (NULL != m_pInstance && m_pInstance->m_iExitThread)
			{
				reval =PantCheckSend();
			}
			if (-1==reval)
			{    
				UnInitConnect();
				if (0 <= InitConnect((void *)&m_pInstance->m_stSysParam.stSocketParam))
				{
					SetCurrentStatus(INIT_CONNECT_OK);
					msg_post(MSG_CONNECT_SUCCESS, 0, 0, MODULE_SCENE_MAIN_UI);  //更新状态栏
					//向list中添加运行log
					msg_post(MSG_DISP_WORKING_INFO, "连接成功",(unsigned int)strlen("连接成功") + 1, MODULE_SCENE_MAIN_UI); 
				}
				else
				{
					if (GetCurrentStatus(INIT_CONNECT_OK))
					{
						DelCurrentStatus(INIT_CONNECT_OK);
						msg_post(MSG_CONNECT_FAILED, 0, 0, MODULE_SCENE_MAIN_UI);  //更新状态栏
					}
					//向list中添加运行log
					msg_post(MSG_DISP_WORKING_INFO, "连接失败",(unsigned int)strlen("连接失败") + 1, MODULE_SCENE_MAIN_UI); 
				}
			}

		}
		else
		{
			ResetEvent(m_pInstance->m_RecvEvent);//手动设置为
		}
	}
	return 0;
}

int CManagerCtrl::FindSpecTypePos(const VIRSION_DATA_PKG *pkg, unsigned int *pType, int iType)
{
	int iRet = -1;
	if (NULL != pkg && NULL != pType)
	{
		for (int i = 2; i < pkg[1].iValue + VIRSION_DATA_PKG_HEADER_SIZE; i++)
		{
			if (iType == pType[i] / 100 )
			{
				iRet = i;
				break;
			}
		}
	}
	return iRet;
}

int CManagerCtrl::FindParamData(SCENE_PKG *pkg, int iType, int index)
{
	int iRet = -1;
	CManagerCtrl *pInstance = GetInstance();
	if (NULL != pInstance->m_pVisionInstance)
	{
		iRet = pInstance->m_pVisionInstance->FindParamData(pkg, iType, index);
	}
	return iRet;
}

int CManagerCtrl::InsertParamItem(int SceneId, const VIRSION_DATA_PKG *pkg, unsigned int *pType, int iSize, int insertPos)
{
	int iRet = -1;
	SCENE_PKG *param = NULL;
	VIRSION_DATA_PKG *pkgTmp = NULL;
	unsigned int *pTypeN = NULL;
	int iNewSize = 0;

	do 
	{
		param = GetSceneHandle(SceneId);
		if (NULL == param || NULL == pkg)
		{
			break;
		}
		iNewSize = param->pkgBodySize + iSize;

		pkgTmp = new VIRSION_DATA_PKG[iSize];
		pTypeN = new unsigned int[iSize];
		memcpy(pkgTmp, pkg, iSize * sizeof(VIRSION_DATA_PKG));
		memcpy(pTypeN, pType, iSize * sizeof(unsigned int));

		vector<VIRSION_DATA_PKG *>::iterator itV = param->vecBody.begin() + insertPos;
		vector<unsigned int *>::iterator itI = param->vecDataType.begin() + insertPos;
		param->vecBody.insert(itV, pkgTmp);
		param->vecDataType.insert(itI, pTypeN);

		param->pkgBodySize = iNewSize;
		param->pkgCount++;
		int iT = pkg[0].iValue % 10000;
		int iAddValue = 1;
		if (MAIN_PRO == iT/100)
		{
			param->imageCount++;
			SCENE_DEVICE_INFO pInfo;
			int iG = pkg[0].iValue / 10000;
			vector<SCENE_DEVICE_INFO>::iterator itS = param->vecInfo.begin() + iG;
			param->vecInfo.insert(itS, pInfo);
			iAddValue += 10000;//增加了图像后， 后续Group同时加1
		}


		//更新后续元素
		int iPos = insertPos + 1;
		itV = param->vecBody.begin() + iPos;
		if (itV == param->vecBody.end())
		{
			iRet = 0;
			break;
		}
		for (;itV != param->vecBody.end(); itV++)
		{
			pkgTmp = *itV;
			if ((iT / 100) == (pkgTmp[0].iValue%10000)/100)
			{
				pkgTmp[0].iValue += iAddValue;
			}
		}
		SetSceneParam(param);
		iRet = 0;
	} while (0);

	return iRet;
}

int CManagerCtrl::UpdateParamItem( int SceneId, const VIRSION_DATA_PKG *pkg, unsigned int *pType, INTERFACE_SET_DATA *dataTmp, int iSize, int insertPos, bool insert /*= false*/ )
{
	int iRet = -1;
	SCENE_PKG *param = NULL;
	VIRSION_DATA_PKG *pkgTmp = NULL;
	unsigned int *pTypeN = NULL;
	INTERFACE_SET_DATA *pInterfaceData = NULL;
	int iNewSize = 0;

	do 
	{
		param = GetSceneHandle(SceneId);
		if (NULL == param || NULL == pkg || pkg[0].iValue != param->vecBody[insertPos][0].iValue
			|| insertPos >= param->vecBody.size() || insertPos >= param->vecDataType.size())
		{
			break;
		}
		iNewSize = param->pkgBodySize + iSize - param->vecBody[insertPos][1].iValue;
		pkgTmp = new VIRSION_DATA_PKG[iNewSize];
		pTypeN = new unsigned int[iNewSize];
		pInterfaceData = new INTERFACE_SET_DATA;
		memcpy(pkgTmp, pkg, iSize * sizeof(VIRSION_DATA_PKG));
		memcpy(pTypeN, pType, iSize * sizeof(unsigned int));
		memcpy(pInterfaceData, dataTmp, sizeof(INTERFACE_SET_DATA));
		delete param->vecBody[insertPos];
		param->vecBody[insertPos] = pkgTmp;
		delete param->vecDataType[insertPos];
		param->vecDataType[insertPos] = pTypeN;
		param->pkgBodySize = iNewSize;
		if (VIRSION_DATA_PKG_HEADER_SIZE == iSize)	//清除数据
		{
			param->InterfaceDataNumb = 0;
			param->InterfaceSetData.clear();
		}
		else if (insert)	//插入数据
		{
			param->InterfaceSetData.push_back(pInterfaceData);
			param->InterfaceDataNumb = pInterfaceData->nIndex + 1;
		}
		else if (!insert)		//更新数据
		{
			param->InterfaceSetData[pInterfaceData->ItemNumb] = pInterfaceData;
		}
		SetSceneParam(param);
		iRet = 0;
	} while (0);

	return iRet;
}
int CManagerCtrl::SwapParamItem(int SceneId, int iPosF, int iPosN)
{
	int iRet = -1;
	SCENE_PKG *param = NULL;
	VIRSION_DATA_PKG *pkgTmpF = NULL;
	VIRSION_DATA_PKG *pkgTmpN = NULL;
	unsigned int *pTypeF = NULL;
	unsigned int *pTypeN = NULL;
	int iNewSize = 0;

	do 
	{
		param = GetSceneHandle(SceneId);
		if (NULL == param || iPosN >= param->vecBody.size() || iPosN >= param->vecDataType.size() 
			|| iPosF >= param->vecBody.size() || iPosF >= param->vecDataType.size())
		{
			break;
		}
		pkgTmpF = param->vecBody[iPosF];
		pkgTmpN = param->vecBody[iPosN];
		param->vecBody[iPosF] = pkgTmpN;
		param->vecBody[iPosN] = pkgTmpF;

		pTypeF = param->vecDataType[iPosF];
		pTypeN = param->vecDataType[iPosN];
		param->vecDataType[iPosF] = pTypeN;
		param->vecDataType[iPosN] = pTypeF;


		//如果交换的类型相同， 则换下第一个元素
		//如果交换的有一个是MAIN_PRO， 则修改另一个的Group
		int iDF = pkgTmpN[0].iValue;   //交换后的
		int iDN = pkgTmpF[0].iValue;   //交换后的
		int iTF = (iDF %10000)/100;
		int iTN = (iDN %10000)/100;
		if (iTF == iTN)
		{
			pkgTmpN[0].iValue = iDN;
			pkgTmpF[0].iValue = iDF;
		}
		else if (MAIN_PRO == iTF) //交换完后前面一个, 则交换完的后一个修改group
		{
			pkgTmpF[0].iValue = pkgTmpF[0].iValue + 10000;
		}
		else if (MAIN_PRO == iTN)
		{
			pkgTmpN[0].iValue = pkgTmpN[0].iValue - 10000;
		}

		SetSceneParam(param);
		iRet = 0;
	} while (0);

	return iRet;
}
int CManagerCtrl::DeleteParamItem(int SceneId, int insertPos)
{
	int iRet = -1;
	SCENE_PKG *param = NULL;
	VIRSION_DATA_PKG *pkgTmp = NULL;
	unsigned int *pType = NULL;
	int iNewSize = 0;
	int iLastSize = 0;

	do 
	{
		param = GetSceneHandle(SceneId);
		if (NULL == param || insertPos >= param->vecBody.size() || insertPos >= param->vecDataType.size())
		{
			break;
		}

		pkgTmp = param->vecBody[insertPos];
		pType = param->vecDataType[insertPos];
		int iDelSize = pkgTmp[1].iValue + 2;
		iNewSize = param->pkgBodySize - iDelSize;
		int iT = pkgTmp[0].iValue;

		param->pkgBodySize = iNewSize;
		param->pkgCount--;
		vector<VIRSION_DATA_PKG *>::iterator itV = param->vecBody.begin() + insertPos;
		vector<unsigned int *>::iterator itU = param->vecDataType.begin() + insertPos;
		param->vecBody.erase(itV);
		param->vecDataType.erase(itU);
		delete pkgTmp;
		delete pType;

		//如果删除的是图像
		int iG = iT /10000;
		int iType = (iT %10000)/100;
		int iDelValue = 1;
		if (MAIN_PRO == iType)
		{
			vector<SCENE_DEVICE_INFO>::iterator itS = param->vecInfo.begin() + iG;
			param->vecInfo.erase(itS);
			iDelValue += 10000;
			param->imageCount--;
		}

		//更新后续元素
		int iPos = insertPos;
		itV = param->vecBody.begin() + insertPos;
		for (;itV != param->vecBody.end(); itV++)
		{
			pkgTmp = *itV;
			if (((iT %10000)/ 100) == (pkgTmp[0].iValue%10000)/100)
			{
				pkgTmp[0].iValue -= iDelValue;
			}
		}

		SetSceneParam(param);
		iRet = 0;
	} while (0);

	return iRet;
}


int CManagerCtrl::GenProductId(string name)
{
	int ProductId = -1;
	PMAPDATA::iterator it;
	for (int i = 0;i<20;i++)
	{
		for (it = m_stSysParam.ProductMap.begin();it!= m_stSysParam.ProductMap.end();it++)
		{
			if (m_stSysParam.ProductById[i] == it->first)
			{
				break;
			}
		}
		//如果没找到
		ProductId =  i;
		//m_stSysParam.ProductById[i] = name;

		break;
	}
	return ProductId;

}

int CManagerCtrl::FindProductId(string name)
{
	int ProductId = -1;
	for (int i = 0;i<20;i++)
	{	
		if (name == m_stSysParam.ProductById[i])
		{
			ProductId =  i;
			break;
		}

	}
	return ProductId;
}


void CManagerCtrl::SetCurProductName(string name)
{
	//系统参数更改
	m_stSysParam.CurProductName = name;

	//在配置文件中保存当前产品为默认产品
	ConfigSaveDefaultProduct(name);

	//在界面上显示当前产品名字
	AlterMainWndTitle(name);

}

void CManagerCtrl::AlterMainWndTitle(string name)
{

	//在界面上显示当前产品名字
	char szText[256]={0};
	sprintf_s(szText,"%s",name.c_str());
	msg_post(MSG_DIS_MAINWND_NAME, szText, (unsigned int)strlen(szText) + 1, MODULE_SCENE_MAIN_UI); 
}

//对通信传来的场景号映射到实际场景号,若只有一种产品不应该有映射表
int CManagerCtrl::MapSceneIdTelToReal(int TelSceneId)
{
	//int RealSceneID;
	int MapSceneNo = -1;
	SYSTEM_PARAM* pSysParam = (SYSTEM_PARAM*)GetSysParamPoint();		      
	//2.1,获取当前产品的名字。
	string curProductName = pSysParam->CurProductName;
	//2.2，找到当前产品的场景号映射值，如果映射值小于0则不映射。
	if (pSysParam->ProductMap.empty())//映射表为空则通讯场景为处理场景
	{	
		MapSceneNo =  TelSceneId;
	}
	PMAPDATA::iterator it;
	it = pSysParam->ProductMap.find(curProductName);
	if (it != pSysParam->ProductMap.end())
	{
		if (it->second.find(TelSceneId)!= it->second.end())
		{
			MapSceneNo =  it->second.find(TelSceneId)->second;
		}
	}
	LOG_INFO("通讯场景号:%d,实际场景号为:%d",TelSceneId,MapSceneNo);
	return MapSceneNo;
}

//将实际场景号映射到通信场景号
int CManagerCtrl::MapSceneIdRealToTel(int RealSceneId)
{
	int MapSceneNo = -1;
	SYSTEM_PARAM* pSysParam = (SYSTEM_PARAM*)GetSysParamPoint();		      
	//2.1,获取当前产品的名字。
	string curProductName = pSysParam->CurProductName;
	//2.2，找到当前产品的场景号映射值，如果映射值小于0则不映射。
	PMAPDATA::iterator it;
	SCENEMAP::iterator subit;
	it = pSysParam->ProductMap.find(curProductName);
	if (pSysParam->ProductMap.empty())
	{
		return RealSceneId;
	}
	if (it != pSysParam->ProductMap.end())
	{
		for (subit = (it->second).begin();subit!=(it->second).end();subit++ )
		{
			if (RealSceneId == subit->second)
			{
				MapSceneNo = subit->first;
			}
		}

		//if (it->second.find(SceneId)!= it->second.end())
		//{
		//	MapSceneNo =  it->second.find(SceneId)->second;
		//}

	}
	return MapSceneNo;


}

int  CManagerCtrl::ManagerProcFinish(int& iSceneID, CHECK_RESULT *pResult, int iErrorCode)
{
	int TelSceneId = MapSceneIdRealToTel(iSceneID);
	if (TelSceneId<0)
	{
		TelSceneId = iSceneID;
	}
	iSceneID = TelSceneId;
	if (m_stSysParam.IsSimulate)
	{
		for (int i= 0;i<pResult->iCount;i++)
		{
			sprintf_s(pResult->szValue[i],"%0.4lf",0.0);
		}
		sprintf_s(pResult->szValue[0],"%d",1);

	}

	return  CommunicationProcessFinished(TelSceneId,pResult,iErrorCode);


}


void CManagerCtrl::ManagerGrabFinish(int& iSceneID, CHECK_RESULT *res,int ErrorCode)
{
	int TelSceneId = MapSceneIdRealToTel(iSceneID);
	if (TelSceneId<0)
	{
		TelSceneId = iSceneID;
	}
	iSceneID = TelSceneId;

	GrabFinished(TelSceneId, res,ErrorCode);

}


bool CManagerCtrl::HalconProcedureLocate(long iWinID,int iSceneID,int Imageindex,string ProcedurePath,
	double (*inParam)[HalProcInParamNum], double& PosX,double& PosY,double& PosPhi)
{
	map<long ,long>::iterator itW;
	itW=m_mapWinID.find(iWinID);
	//将窗口ID转换成halcon窗口ID
	long halconID = -1;

	if(itW!=m_mapWinID.end())
	{
		halconID = itW->second;
	}
	if (-1 ==halconID)
	{
		halconID = 3600;
		//return false;
	}
	return m_pVisionInstance->HalconProcedureLocate(halconID,iSceneID,Imageindex,ProcedurePath,inParam,PosX,PosY,PosPhi);
}

bool CManagerCtrl::HalconProcedureProcess(long iWinID,int iSceneID,int Imageindex,string ProcedurePath,vector<double> &inparam,vector<double> &outparam,int display,int &error,string &hmessage)
{
	map<long ,long>::iterator itW;
	itW=m_mapWinID.find(iWinID);
	//将窗口ID转换成halcon窗口ID
	long halconID = -1;

	if(itW!=m_mapWinID.end())
	{
		halconID = itW->second;
	}
	if (-1 ==halconID)
	{
		halconID = 3600;
		//return false;
	}
	return m_pVisionInstance->HalconProcedureProcess(halconID,iSceneID,Imageindex,ProcedurePath,inparam,outparam,display,error,hmessage);

}
bool CManagerCtrl::ImageEnhancement(long iWinID,int iSceneID,int Imageindex,double& multinum,double& addnum,int display,int &error,string &hmessage)
{
	map<long ,long>::iterator itW;
	itW=m_mapWinID.find(iWinID);
	//将窗口ID转换成halcon窗口ID
	long halconID = -1;

	if(itW!=m_mapWinID.end())
	{
		halconID = itW->second;
	}
	if (-1 ==halconID)
	{
		halconID = 3600;
		//return false;
	}
	return m_pVisionInstance->ImageEnhancement(halconID,iSceneID,Imageindex,multinum,addnum,display,error,hmessage);

}

void CManagerCtrl::ClearSceneProcResultMap()
{
	sys_EnterCriticalSection(m_mapSceneResultSection);
	for(auto itresult = m_mapSceneProcResult.begin(); itresult != m_mapSceneProcResult.end(); itresult++)
	{
		CHECK_RESULT *temp = itresult->second;
		if (NULL != temp)
		{		
			delete temp;
		}
	}
	m_mapSceneProcResult.clear();
	sys_LeaveCriticalSection(m_mapSceneResultSection);
}
int CManagerCtrl::InitSceneProcResultMap()
{
	SYSTEM_PARAM *sysParam = NULL;
	int sceneNum = 0;
	CHECK_RESULT *tempres = NULL;
	string strSceneName ;
	sysParam = (SYSTEM_PARAM *)GetSysParamPoint();
	if (NULL == sysParam)
	{
		return MAN_ERROR;
	}
	sceneNum = sysParam->iSceneNum;
	sys_EnterCriticalSection(m_mapSceneResultSection);
	for (int i = 0;i<sceneNum;i++)
	{
		tempres = new CHECK_RESULT();
		tempres->index = i;
		strSceneName = sysParam->pstrSceneName[i];
		m_mapSceneProcResult.insert((map<string,   struct CHECK_RESULT *>::value_type(strSceneName,tempres)));
	}
	sys_LeaveCriticalSection(m_mapSceneResultSection);
	return MAN_OK;
}

CHECK_RESULT* CManagerCtrl::GetSceneProcResult(string &scneName)
{
	CHECK_RESULT* res = NULL ;

	sys_EnterCriticalSection(m_mapSceneResultSection);
	map<string,CHECK_RESULT*>::iterator it;
	it = m_mapSceneProcResult.find(scneName);
	if (it != m_mapSceneProcResult.end())
	{
		res = it->second; //返回指针,这里失去了锁的意义
	}
	sys_LeaveCriticalSection(m_mapSceneResultSection);
	return res;
}
CHECK_RESULT* CManagerCtrl::GetSceneProcResultById(int iSceneID)
{
	CHECK_RESULT* res = NULL ;
	//GetSysParamPoint();
	sys_EnterCriticalSection(m_mapSceneResultSection);
	for (auto it = m_mapSceneProcResult.begin();it!= m_mapSceneProcResult.end();it++)
	{
		if (it->second->index == iSceneID)
		{
			res = it->second; //返回指针,这里失去了锁的意义
			break;
		}
	}
	sys_LeaveCriticalSection(m_mapSceneResultSection);
	return res;
}
int CManagerCtrl::SetSceneProcResult(string &scneName,CHECK_RESULT& res)
{
	int ret = MAN_OK;
	sys_EnterCriticalSection(m_mapSceneResultSection);
	map<string,CHECK_RESULT*>::iterator it;
	it = m_mapSceneProcResult.find(scneName);
	if (it != m_mapSceneProcResult.end())
	{
		*(it->second )= res;//浅层复制
	}
	sys_LeaveCriticalSection(m_mapSceneResultSection);
	return ret;
}
int CManagerCtrl::ClearProcResultByID(int iSceneID)
{
	int ret = MAN_OK;
	sys_EnterCriticalSection(m_mapSceneResultSection);
	map<string,CHECK_RESULT*>::iterator it;
	for (it = m_mapSceneProcResult.begin();it!= m_mapSceneProcResult.end();it++)
	{
		if (it->second->index == iSceneID)
		{
			it->second->caturetime = 0;
			it->second->err = 0;
			it->second->iCount = 0;
			it->second->time = 0;
			it->second->isGeneralProcess = 0;
			for (int i = 0;i<RESULT_NUMBER;i++)
			{
				memset(it->second->szValue[i],0,BUF_SIZE/2);
			}

		}
	}
	sys_LeaveCriticalSection(m_mapSceneResultSection);
	return ret;
}
//3982行