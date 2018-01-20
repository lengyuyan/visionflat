#include "StdAfx.h"
#include "NetworkCtrl.h"
#include "SocketClientLogic.h"
#include "common.h"
#include <process.h>
#include "skt.h"

#define KEEPALIVE    "keepalive"
#define DATA_END_FLAG   "#"
#define DATA_SEG_FLAG   "|"
#define DATA_COORDINATE_FLAG  "-"
#define RESET_SYSTEM    "RS"
#define GET_CHECK_RESULT "C"
#define RESPONSE_OPEN_LIGHT "ROL"
#define REQUEST_CLOSE_SYSTEM "CL"
#define CALIB_CAMERA_AFFINE "C"
#define SET_VISRION_MODEL   "SM"
#define GET_IMAGE_BUF       "GI"
#define HEART_BEAT_BUFFER    ";"   //心跳包
#define MOVE_COORDINATE_X    "x"
#define MOVE_COORDINATE_Y    "y"
#define MOVE_COORDINATE_SN    "sn"


#define TIME_OUT        2000

CNetworkCtrl * CNetworkCtrl::m_pNetworkInstance = NULL;

CNetworkCtrl::CNetworkCtrl(void)
{
	skt_init();
	m_serverstatus = 0;

    m_pSocketInstance = SocketClientLogic::GetSocketClientInstance();
    m_bIsRunning = false;
    m_SocketHandle = INVALID_HANDLE_VALUE;
    m_pTriggerFun = NULL;
    m_pRemoteSysCommand = NULL;
    memset(m_szRecvBuf, 0, 1024);
    m_Section = sys_InitializeCriticalSection();

	char szTexy[64]={0};
	sprintf_s(szTexy,"%s",GET_CHECK_RESULT);
	memcpy(m_CalibRevIPHead,CALIB_CAMERA_AFFINE,sizeof(m_CalibRevIPHead));
	memcpy(m_WorkRevIPHead,GET_CHECK_RESULT,sizeof(m_WorkRevIPHead));
	memcpy(m_CalibSendIPHead,szTexy,sizeof(m_CalibSendIPHead));
	memcpy(m_WorkSendIPHead,szTexy,sizeof(m_WorkSendIPHead));
	memcpy(m_IPDivid,DATA_SEG_FLAG,sizeof(m_IPDivid));
	memcpy(m_IPEnd,DATA_END_FLAG,sizeof(m_IPEnd));


}


CNetworkCtrl::~CNetworkCtrl(void) 
{
    sys_EnterCriticalSection(m_Section);
    m_bIsRunning = false;
    if (NULL != m_pSocketInstance)
    {
        //m_pSocketInstance->ClientSocketSend("", strlen(""));
        m_pSocketInstance->UninitClientSocket();
        SocketClientLogic::DestroySocketClientInstance();
        m_pSocketInstance = NULL;
    }
    sys_LeaveCriticalSection(m_Section);
	sys_DeleteCriticalSection(m_Section);
    //后续不需要操作句柄， 创建后提前释放
    if (INVALID_HANDLE_VALUE != m_SocketHandle)
    {
        WaitForSingleObject(m_SocketHandle, TIME_OUT);
        CloseHandle(m_SocketHandle);
        m_SocketHandle = INVALID_HANDLE_VALUE;
    }
	uninit_vision(0);//释放那个服务器ip

	skt_uninit();
}

CNetworkCtrl *CNetworkCtrl::GetInstance(const char* pIp)
{
    if (NULL == m_pNetworkInstance)
    {
        m_pNetworkInstance = new CNetworkCtrl;
        m_pNetworkInstance->m_bIsRunning = true;
    }
    return m_pNetworkInstance;                                           
}
void CNetworkCtrl::ReleaseInstance(const char* pIp)
{
    if (NULL != m_pNetworkInstance)
    {
        m_pNetworkInstance->m_bIsRunning = false;
        delete m_pNetworkInstance;
        m_pNetworkInstance = NULL;
    }
}

    /*************************************************************************
    函数      : InitConnect
    功能      : 初始化处理
    输入参数  : pCount 初始化配置项
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : N/A
    **********************************************************************/
    int CNetworkCtrl::InitConnect(void *pCount)
    {
        int iRet = -1;
        if (NULL == m_pSocketInstance)//m_pSocketInstance为客户端实例
        {
            return iRet;
        }

		SOCKET_PARAM *pParam = (SOCKET_PARAM *)pCount;		
		TVisionInitParam initParam={0};
		//ip，端口，标识(S/C)
		strcpy_s(initParam.data[0], pParam->strClientIPAdd.c_str());
		itoa(pParam->ClientPort,initParam.data[1],10);
		strcpy_s(initParam.data[2], "S");
		int sret = init_vision(0,&initParam);

        sys_EnterCriticalSection(m_pNetworkInstance->m_Section);
        if (!m_pSocketInstance->IsConnected())
        {
            bool bResutl = false;
            if(m_pSocketInstance->InitClientSocket())
            {
             //   bResutl = m_pSocketInstance->ClientSocketConnect(pParam->strServerIPAdd.c_str(), pParam->ServerPort,
             //                                 pParam->strClientIPAdd.c_str(), pParam->ClientPort);
				bResutl = m_pSocketInstance->ClientSocketConnect(pParam->strServerIPAdd.c_str(), pParam->ServerPort,
					pParam->strClientIPAdd.c_str(), 0);
            }
            if (bResutl)
            {
                //创建线程等待服务器请求
                HANDLE handle = INVALID_HANDLE_VALUE;
                m_bIsRunning = true;
                m_SocketHandle = CreateThread(NULL, 0, CNetworkCtrl::ThreadClient,0, 0, NULL);
                iRet = 0;
                //m_SocketHandle = (HANDLE)_beginthreadex(NULL,0,(unsigned (__stdcall *)(void*))CNetworkCtrl::ThreadClient,0,0,NULL);
            }
            else
            {
                iRet = -1;
            }
        }
        else
        {
            iRet = 0;
        }
        sys_LeaveCriticalSection(m_pNetworkInstance->m_Section);
        return iRet;
    }
    /**********************************************************************
    函数      : UnInitConnect
    功能      : 去初始化处理
    输入参数  : pCount 初始化配置项
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : N/A
    **********************************************************************/
    int CNetworkCtrl::UnInitConnect(void *pCount)
    {
        int iRet = -1;
        if (NULL == m_pSocketInstance)
        {
            return iRet;
        }
        SOCKET_PARAM *pParam = (SOCKET_PARAM *)pCount;
        if (m_pSocketInstance->IsConnected())
        {
            m_pSocketInstance->UninitClientSocket();
        }

		
        return 0;
    }

    /**********************************************************************
    函数      : ThreadVisionProcess
    功能      : 动态创建的图像处理线程
    输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
    DWORD WINAPI  CNetworkCtrl::ThreadClient(void *pParam)
    {
        if (NULL != m_pNetworkInstance)
        {
            m_pNetworkInstance->RecvQueue();
        }
        return 0;
    }
	 /**********************************************************************
    函数      : ThreadVisionProcess
    功能      : 动态创建的图像处理线程
    输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
    DWORD WINAPI  CNetworkCtrl::ThreadSendImg(void *pParam) 
	{
		IMG_BUF imgb = *(IMG_BUF *)pParam;
		BYTE* pdata = (BYTE*)imgb.buf;
		int dataSize = imgb.size;
		int sence = imgb.sence;
		int sendSize = 0;
		int iRet = -1;
		char endstr[10] = {0};
		int len = 0;
		sprintf_s(endstr,"%s","RGIEND");

		while(dataSize > 0)
		{
			sendSize = dataSize > 1024 ? 1024 : dataSize;

			iRet = ((SocketClientLogic*)imgb.network)->ImgeSend(sence,(char*)pdata, sendSize);
			pdata += sendSize;
			dataSize -= sendSize;
		}
		len = (int)strlen(endstr);
		//iRet = ((SocketClientLogic*)imgb.network)->ImgeSend(sence,endstr, len);
		return 0;
	}
    
    /**********************************************************************
    函数      : RegesterCallBack
    功能      : 注册触发接口
    输入参数  : 回调函数
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : N/A
    **********************************************************************/
    bool CNetworkCtrl::RegesterCallBack(pFunCallback pfun)                                              
    {
        m_pTriggerFun = pfun;
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
    bool CNetworkCtrl::RegesterCloseSys(pFunCloseSys pfun)   
    {
        m_pRemoteSysCommand = pfun;
        return true;
    }


 int CNetworkCtrl::GrabFinished(int iSceneID, CHECK_RESULT *pResult,int iErrorCode)
 {
        int iRet = 0;
        char szBuf[128] = {0};
		char szBuffer[128] = {0};
		char DataszBuf[128] = {0};
		char DszBuffer[128] = {0};

		int testNum = pResult->testCount++;

		if (iErrorCode>=0)//拍照成功
		{
			strcpy_s(pResult->szValue[0],"0");
			sprintf_s(szBuf, "%s%s%d%s%d%s%s",m_CalibSendIPHead,m_IPDivid,iSceneID,m_IPDivid,1,m_IPDivid,m_IPEnd);

		}else //拍照失败还会给数据服务器返回数据
		{
			double caturetime = pResult->caturetime ;
			strcpy_s(pResult->szValue[0],"-1"); //拍照失败,测试结果为-1
			char sid[10]= {0};
			char camerindex[128] = {0};
			int valuetype = pResult->resultType;//坐标类型,绝对坐标
			
			if (testNum>= 65000)
			{
				pResult->testCount = 0;
			}
			itoa(pResult->index,sid,10);
			itoa(pResult->CameraIndex[0],camerindex,10);
			//测试结果,坐标值(x,y th),是否绝对坐标,本场景测试次数,拍照时间,测试时间,相机id,rawpath,outimg
			sprintf_s(DszBuffer, "%d,%lf,%lf,%lf,%d,%d,%lf,%lf,%s,%s,%s",-1,0.0,0.0,0.0,valuetype,testNum,caturetime,0.0,camerindex,"null","null");
			send_custom_cmd(0,sid,DszBuffer);//发送给数据服务器

			sprintf_s(szBuf, "%s%s%d%s%d%s%s",m_CalibSendIPHead,m_IPDivid,iSceneID,m_IPDivid,0,m_IPDivid,m_IPEnd);
		}
        LOG_INFO("%s\n", szBuf);
		sprintf_s(szBuffer, "Send Cmd: %s",szBuf);
		msg_post(MSG_DISP_WORKING_INFO, (void *)szBuffer, (int)strlen(szBuffer)+1, MODULE_SCENE_MAIN_UI);

        iRet = m_pSocketInstance->ClientSocketSend(szBuf, (int)strlen(szBuf));
        if (0 <= iRet)
        {
            LOG_INFO(szBuf);
        }
        return iRet;
  }

	/**********************************************************************
    函数      : getImagePathFinished
    功能      : 图像采集完成
    输入参数  : iSceneID 场景ID
                iErrorCode 错误码
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : N/A
	修改记录：
	日期               修改人         修改内容
	2016-08-16         guopengzhou    新协议指定拍照完毕后返回RGI-->RGC命令          
    **********************************************************************/
    int CNetworkCtrl::getImagePathFinished(int iSceneID, char* path,int iErrorCode)
    {
        int iRet = 0;
        char szBuf[128] = {0};
		char szBuffer[512] = {0};

        sprintf_s(szBuf, "RGI,%d,%d,%s;", iSceneID, iErrorCode,path);
        LOG_INFO("%s\n", szBuf);
		sprintf_s(szBuffer, "Send Cmd: %s",szBuf);
		msg_post(MSG_DISP_WORKING_INFO, (void *)szBuffer, (int)strlen(szBuffer)+1, MODULE_SCENE_MAIN_UI);
        iRet = m_pSocketInstance->ClientSocketSend(szBuf, (int)strlen(szBuf));
        if (0 <= iRet)
        {
            LOG_INFO(szBuf);
        }
        return iRet;
    }
int CNetworkCtrl::SendData2Board(int iSceneID, CHECK_RESULT *pResult, int iErrorCode)
{
	int res = 0;
	if (pResult->err == IMAGE_ACQISITION)//采集出了问题就不会有结果
	{
		return IMAGE_ACQISITION;
	}
	char DszBuf[4096] = {0};
	char DataszBuf[128] = {0};
		//发送给数据服务器
	int datanum = pResult->SeneTpye == WORKING_CAMERA? pResult->iCount:pResult->iCount-2;
	for (int i = 0; i < datanum; i++)
	{
		if (!strcmp(pResult->szValue[i],""))
		{
			sprintf_s(DataszBuf, "%d,", 0);
		}
		else
		{
			sprintf_s(DataszBuf, "%s,", pResult->szValue[i]);
		}			
		strcat_s(DszBuf, DataszBuf);
	}
	char sid[10]= {0};
	char cindex[128]= {0};
	itoa(iSceneID,sid,10);	
	itoa(pResult->CameraIndex[0],cindex,10);	
	////把是否绝对坐标,测试次数，拍照时间，测试时间写进去,相机id,rawpath,resultpath
	sprintf_s(DataszBuf, "%d,%d,%lf,%lf,%s,%s,%s", pResult->resultType,pResult->testCount,pResult->caturetime,pResult->time,cindex,pResult->rawimg.c_str()
			,pResult->outimg.c_str());
	strcat_s(DszBuf, DataszBuf);
	send_custom_cmd(0,sid,DszBuf);
	return res;

}
 int CNetworkCtrl::ProcessFinished(int iSceneID, CHECK_RESULT *pResult, int iErrorCode)
 {
    int iRet = 0;
   // char szBuf[4096] = {0};//数据太大时用
	char szBuf[256] = {0};
    if (NULL == pResult || pResult->iCount>RESULT_NUMBER)
    {
		msg_post(MSG_DISP_WORKING_INFO, "没有结果", (int)strlen("没有结果")+1, MODULE_SCENE_MAIN_UI);
        return -1;
     }
	//char *szBuf = NULL;
	//szBuf = new char(pResult->iCount * 8);
	//memset(szBuf,0,pResult->iCount * 8);
	switch (pResult->SeneTpye)
	{
		case CALIB_CAMERA: //拼接数据头: C|0|
		{
			sprintf_s(szBuf, "%s%s%d%s", m_CalibSendIPHead,m_IPDivid,iSceneID,m_IPDivid);  //pResult->err == 0 ：成功				 
		}
		break;
		case WORKING_CAMERA:
		{
			sprintf_s(szBuf, "%s%s%d%s", m_WorkSendIPHead,m_IPDivid,iSceneID,m_IPDivid);  //pResult->err == 0 ：成功
		}
		break;
	}     
    char szDoubValue[128] = {0};
		
     for (int i = 0; i < pResult->iCount; i++)
     {
		if (i< pResult->iCount-1)
		{
             sprintf_s(szDoubValue, "%s,", pResult->szValue[i]);
		}else
		{
			sprintf_s(szDoubValue, "%s%s", pResult->szValue[i],m_IPDivid);//最后一个数拼接一个|
		}
			strcat_s(szBuf, szDoubValue);
      }
	 strcat_s(szBuf, m_IPEnd);//最后拼接上一个#
     LOG_INFO("%s\n", szBuf);
	 int iLen = (int)strlen(szBuf);

		//发送内容到窗口
	SYSTEMTIME st;
	GetLocalTime(&st);
	//sprintf_s(szBuf, "Send Cmd: %s",szBuf);
	msg_post(MSG_DISP_WORKING_INFO, (void *)szBuf, (int)strlen(szBuf)+1, MODULE_SCENE_MAIN_UI);
    iRet = m_pSocketInstance->ClientSocketSend(szBuf, (int)strlen(szBuf));
    if (0 <= iRet)
    {
        LOG_ERR(szBuf);
    }
	//delete szBuf;

		//下面发送数据到数据服务器
	SendData2Board(iSceneID, pResult, iErrorCode);
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
	int CNetworkCtrl::SendImage( int iSceneID, void* pBuffer,int size,int w,int h, int iErrorCode )
	{

        int iRet = 0;
        char szBuf[54] = {0};

		BYTE* pdata = (BYTE*)pBuffer;
		int dataSize = size;
		int sendSize = 0;

		if (NULL == pBuffer)
		{
			sprintf_s(szBuf, "R%s,%d,%d,%d,%d,%d", GET_IMAGE_BUF, iSceneID,-1, 0,0,0);
			strcat_s(szBuf, ";");
			int iLen = (int)strlen(szBuf);
			iRet = m_pSocketInstance->ClientSocketSend(szBuf, (int)strlen(szBuf));
			return iRet;
		}
		sprintf_s(szBuf, "R%s,%d,%d,%d,%d,%d", GET_IMAGE_BUF, iSceneID,0,size,w,h);
		int iLen = (int)strlen(szBuf);
		iRet = m_pSocketInstance->ClientSocketSend(szBuf,iLen);
		if (0 >= iRet)
		{
			return iRet;
		}

		//创建线程等待服务器请求
		HANDLE handle = INVALID_HANDLE_VALUE;
		
		IMG_BUF imgbuf;
		imgbuf.size = size;
		imgbuf.network = this->m_pSocketInstance;
		imgbuf.sence = iSceneID;
		imgbuf.buf = pBuffer;
		m_imgb = imgbuf;	
		handle = CreateThread(NULL, 0, CNetworkCtrl::ThreadSendImg,&m_imgb, 0, NULL);
       
        return iRet;
    }
    /**********************************************************************
    函数      : OpenLight
    功能      : 光源操作
    输入参数  : index 光源索引
                type  开关类型, 1为开， 0为关
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 光源通过IO口操作， IO口独占
    **********************************************************************/
    bool CNetworkCtrl::OpenLight(int index, int type)
    {
        bool bRet = false;
        int iRet = -1;
        char szBuf[128] = {0};
        sprintf_s(szBuf, "%s,%d,%d;", "OL", index, type);
        map<int, int>::iterator itL;
        itL = m_vecOperateResult.find(index);
        if (itL == m_vecOperateResult.end())
        {
            m_vecOperateResult.insert(make_pair(index, iRet));
            itL = m_vecOperateResult.find(index);
        }
        if (NULL != m_pSocketInstance)
        {
            itL->second = -1;
            LOG_INFO("%s\n", szBuf);
            iRet = m_pSocketInstance->ClientSocketSend(szBuf,(int)strlen(szBuf));
        }
        if (0 <= iRet)
        {
            LOG_INFO(szBuf);
            int iCount = 0;
            iRet = -1;
            do
            {
                //待增加判断结果, 有返回则退出, 否则2秒超时
                if(-1 != itL->second)
                {
                    if(0 == itL->second)
                    {
                         bRet = true;
                    }
                    break;
                }
                Sleep(50);
                iCount++;
            }while(iCount * 50 < TIME_OUT);
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
    int CNetworkCtrl::ResetFinished(int iErrorCode)
	{
		int iRet=-1;
		char szBuf[128]={0};
        sprintf_s(szBuf,"%s,%d;","RRS",iErrorCode);
        LOG_INFO("%s\n", szBuf);
		iRet = m_pSocketInstance->ClientSocketSend(szBuf,(int)strlen(szBuf));
		if (0 <= iRet)
		{
			LOG_INFO(szBuf);
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
    int CNetworkCtrl::PantCheckSend()
	{
		int iRet=-1;
        if (NULL == m_pSocketInstance)
        {
            iRet = 1;
        }
        else if (m_pSocketInstance->IsConnected())
        {
            iRet = 0;
        }
		int status =0;
		status = vision_status(0);
		if (status != m_serverstatus )
		{
			m_serverstatus = status;
			char msg[256]= {0};
			sprintf_s(msg,"%s",status==0?"数据服务器未初始化":(status==1?"数据服务器未连接":"数据服务器已连接"));
			if (status ==2)
			{
				//send_custom_cmd(0,"1","12");
			}
			msg_post(MSG_DISP_WORKING_INFO, msg, (unsigned int)strlen(msg) + 1, MODULE_SCENE_MAIN_UI); 
		}
		return iRet;
	}

	void CNetworkCtrl::GetCommuniWebProctocol( PROCTOCOLDATA& ProctoData)
	{
		memcpy(ProctoData.CalRevHead,m_CalibRevIPHead,sizeof(m_CalibRevIPHead));
		memcpy(ProctoData.WorkRevHead,m_WorkRevIPHead,sizeof(m_WorkRevIPHead));
		memcpy(ProctoData.CalSendHead,m_CalibSendIPHead,sizeof(m_CalibSendIPHead));
		memcpy(ProctoData.WorkSendHead,m_WorkSendIPHead,sizeof(m_WorkSendIPHead));
		memcpy(ProctoData.IPDivid,m_IPDivid,sizeof(m_IPDivid));
		memcpy(ProctoData.IPEnd,m_IPEnd,sizeof(m_IPEnd));

	}

	void CNetworkCtrl::SetCommuniWebProctocol(const PROCTOCOLDATA& ProctoData)
	{
		ProctoData.EnableData[0]?SetCalibRevIPHead(ProctoData.CalRevHead):false; 
		ProctoData.EnableData[1]?SetWorkRevIPHead(ProctoData.WorkRevHead):false; 
		ProctoData.EnableData[2]?SetCalibSendIPHead(ProctoData.CalSendHead):false; 
		ProctoData.EnableData[3]?SetWorkSendIPHead(ProctoData.WorkSendHead):false; 
		ProctoData.EnableData[4]?SetIPDividLable(ProctoData.IPDivid):false; 
		ProctoData.EnableData[5]?SetIPEndLable(ProctoData.IPEnd):false; 
		//保存到系统参数中，注意也要从系统参数中读取


	}


	void  CNetworkCtrl::SetCalibRevIPHead(const char* szRvHead)
	{
		strcpy_s(m_CalibRevIPHead,szRvHead);

	}
	void  CNetworkCtrl::SetWorkRevIPHead(const char* szRvHead)
	{
		strcpy_s(m_WorkRevIPHead,szRvHead);
	}

	void  CNetworkCtrl::SetCalibSendIPHead(const char* szSendHead)
	{
		strcpy_s(m_CalibSendIPHead,szSendHead);
	}

	void  CNetworkCtrl::SetWorkSendIPHead(const char* szSendHead)
	{
		strcpy_s(m_WorkSendIPHead,szSendHead);
	}

	void  CNetworkCtrl::SetIPDividLable(const char* szIpDivid)
	{
		strcpy_s(m_IPDivid,szIpDivid);
	}

    void  CNetworkCtrl::SetIPEndLable(const char* szIpEnd)
	{
		strcpy_s(m_IPEnd,szIpEnd);
	}


    int CNetworkCtrl::RecvQueue()
    {
        char szBuf[512] = {0};
        int  iRecv = 0;
        bool bReset = false;
		SYSTEMTIME st;
        while (m_bIsRunning && NULL != m_pNetworkInstance)
        {
            //循环接收
            memset(szBuf, 0, 512);
            sys_EnterCriticalSection(m_pNetworkInstance->m_Section);
            iRecv = 0;
            if (NULL != m_pSocketInstance)
            {
                iRecv = m_pSocketInstance->ClientSocketRecevie(szBuf, 512);
				if (-1 == iRecv)
				{
					LOG_INFO("Disconect! \n");
					sys_LeaveCriticalSection(m_pNetworkInstance->m_Section);
					break;
				}
                if (0 != strcmp(szBuf,""))
                {
                    LOG_INFO("%s\n", szBuf);

					if (NULL != strstr(szBuf, KEEPALIVE))
					{
						sys_LeaveCriticalSection(m_pNetworkInstance->m_Section);
						continue;
					}
					char szBuffer[512] = {0};
					GetLocalTime(&st);
					sprintf_s(szBuffer, "%d:%d:%d:%d Recv Cmd: %s",st.wHour,st.wMinute,st.wSecond,st.wMilliseconds,szBuf);
					msg_post(MSG_DISP_WORKING_INFO, (void *)szBuffer, (int)strlen(szBuffer)+1, MODULE_SCENE_MAIN_UI);
                }
            }
            sys_LeaveCriticalSection(m_pNetworkInstance->m_Section);
            if (!m_bIsRunning)
            {
                return 0;
            }
			if (iRecv <= 0)
			{
				Sleep(100);
				continue;
			}

			if (1024 > (iRecv + strlen(m_szRecvBuf)))
			{
				LOG_INFO(szBuf);
				strcat_s(m_szRecvBuf, szBuf);

			}
			else
			{
				//接收缓存越界
			}
			//msg_post(RECV_REMOTE_MSG, 0,0, MODULE_SCENE_MAIN_SDK);

            if (NULL != strstr(m_szRecvBuf, RESET_SYSTEM))
            {
                bReset = true;
            }

            //如果有结束符，则进行处理
			if(NULL != strstr(m_szRecvBuf, m_IPEnd))
			{
				
			     m_SceneIdTemp = ResolveProtocol();
				 if ( m_SceneIdTemp>=0)
				 {
					 //只有图像处理才会执行这步，第二个参数是场景号，其他处理发信息的第二个参数是结构体。
				      msg_post(MSG_COM_COMMUNITE, (void *)&m_SceneIdTemp, sizeof(CALIB_DATA), MODULE_SCENE_MAIN_SDK);
				 }
			}
   }
   return 0;
}
    /**********************************************************************
    函数      : ResolveProtocol
    功能      : 协议解析
    输入参数  : N/A
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : N/A
	修改记录：
	日期               修改人         修改内容
	2016-08-16         guopengzhou    GI命令解析，返回图片路径          
    **********************************************************************/

    int CNetworkCtrl::ResolveProtocol()                           
    {
        char szData[512] = {0};
        char *pData = NULL;
        unsigned int iLengthB = 0;
        unsigned int iLengthE = 0;
        iLengthB = (int)strlen(m_szRecvBuf);
        strtok(m_szRecvBuf, m_IPEnd);  //strtok将找到的第一个字符替换为结束符
        strcpy_s(szData, m_szRecvBuf);
        iLengthE = (int)strlen(szData);
        //如果只有一个数据，则清空缓存， 有未接完后多个数据则将后面数据前移
        if (iLengthB - strlen(m_IPEnd) <= iLengthE)
        {
            memset(m_szRecvBuf, 0, 1024);
        }
        else
        {
            strcpy_s(m_szRecvBuf, 1024, m_szRecvBuf+iLengthE + 1);
        }
        pData = strtok(szData, m_IPDivid);
        if (NULL == pData)
        {
			LOG_ERR("没有接收到数据或者无分隔符");
            return -1;
        }
        //通知进行图像处理,看第一个字符是分隔符,网络层跟业务相关了，应该把解析函数放出去
        if (0 == strcmp(pData, m_WorkRevIPHead))//视觉接收到的字符
        {
            CALIB_DATA data;
            char *pSignData = NULL;
            pData = strtok(NULL, m_IPDivid);
            if (NULL != pData)
            {
                data.iSceneID = atoi(pData);
            }
            else
            {
				LOG_ERR("场景号不为数字或者格式不对!");
                return -1; 
            }
            bool bIsOk = false;
            int indexX = 0;
            int indexY = 0;
            //这里简单的返回一个场景号
			return data.iSceneID; 
            do 
            {
                pData = strtok(NULL, DATA_COORDINATE_FLAG);
                if (NULL == pData)
                {
                    break;
                }
                //pSignData = pData;
                //pData = strtok(NULL, DATA_COORDINATE_FLAG);
                if (0 == stricmp(pData,MOVE_COORDINATE_X))
                {
                    pData = strtok(NULL, m_IPDivid);
                    if (NULL != pData)
                    {
                        data.xCalib[0] = atof(pData);
                        indexX++;
                    }
                    else
                    {
                        break; 
                    }
                }
                else if (0 == stricmp(pData,MOVE_COORDINATE_Y))
                {
                    pData = strtok(NULL, m_IPDivid);
                    if (NULL != pData)
                    {
                        data.yCalib[0] = atof(pData);
                        indexY++;
                    }
                    else
                    {
                        break; 
                    }
                }
                else if (0 == stricmp(pData,MOVE_COORDINATE_SN))
                {
                    pData = strtok(NULL, m_IPDivid);
                    if (NULL != pData)
                    {
                        strcpy_s(data.chSN, 64,pData);
                    }
                    else
                    {
                        break; 
                    }
                }
            } while (1);
            data.iCount = indexX;
            msg_post(WORKING_CAMERA, (void *)&data, sizeof(CALIB_DATA), MODULE_SCENE_MAIN_SDK);
        }
        //开关光源返回值
        else if (0 == strcmp(pData, RESPONSE_OPEN_LIGHT))
		{
			//待处理
			int iRet = 0;
			int index = 0;
			pData = strtok(NULL, m_IPDivid);
			if (NULL != pData)
			{
				index = atoi(pData);
				pData = strtok(NULL, m_IPDivid);
				if (NULL != pData)
				{
					iRet = atoi(pData);
				}
				map<int, int>::iterator it;
				it = m_vecOperateResult.find(index);
				if (it != m_vecOperateResult.end())
				{
					it->second = iRet;
				}
			}

		}
		//通知退出程序
		else if (0 == strcmp(pData, REQUEST_CLOSE_SYSTEM))
        {
            msg_post(CLOSE_SYS, 0, 0,MODULE_SCENE_MAIN_SDK);
		}
		//通知重置参数
		else if(0 == strcmp(pData, RESET_SYSTEM))
        {
            msg_post(RESET_SYS, 0, 0, MODULE_SCENE_MAIN_SDK);
		}
        else if (0 == strcmp(pData, m_CalibRevIPHead)) //标定字符
        {
            //通知拍照, 标定处理
            CALIB_DATA data;
            //场景ID
            pData = strtok(NULL, m_IPDivid);
            if (NULL != pData)
            {
                data.iSceneID = atoi(pData);
            }
            else
            {
                return -1; 
            }
            //抓点索引，当存在多组点时， 索引无效
            pData = strtok(NULL, m_IPDivid);
            if (NULL != pData)
            {
                data.index = atoi(pData);
            }
            else
            {
               //return -1; 
            }
            //是否创建仿射关系，当存在多组点时， 该值无效
            pData = strtok(NULL, m_IPDivid);
            if (NULL != pData)
            {
                if (strcmp("1", pData) == 0)
                {
                    data.bIsEnd = true;
                }
                else
                {
                    data.bIsEnd = false;
                }
            }
            else
            {
                //return -1; 
            }

            //是否与用户有交互， 有交互时， 必需登陆场景调试界面， 且同时只有一个
            pData = strtok(NULL, m_IPDivid);
            if (NULL != pData)
            {
                if (strcmp("1", pData) == 0)
                {
                    data.bIsInteractive = true;
                }
                else
                {
                    data.bIsInteractive = false;
                }
            }
            else
            {
               // return -1; 
            }
            bool bIsOk = false;
			bIsOk = true;
            int index = 0;
            do 
            {
                pData = strtok(NULL, m_IPDivid);
                if (NULL != pData)
                {
                    data.xCalib[index] = atof(pData);
                    bIsOk = true;
                }
                else
                {
                    break; 
                }
                pData = strtok(NULL, m_IPDivid);
                if (NULL != pData)
                {
                    data.yCalib[index] = atof(pData);
                }
                else
                {
                    //return -1;
                }
                index++;
            } while (1);
            data.iCount = index;
            if (!bIsOk)
            {
               // return -1;
            }
            //需要交互则处理过程在主线程中调度，否则在线程中处理
            if (data.bIsInteractive)
            {
                msg_post(CALIB_CAMERA, (void *)&data, sizeof(CALIB_DATA), MODULE_SCENE_DEBUG_UI);//OnRemoteMessage
            }
            else
            {
                msg_post(CALIB_CAMERA, (void *)&data, sizeof(CALIB_DATA), MODULE_SCENE_MAIN_SDK);//RemoteManualMsg
            }

			return -1;

        }
        else if(0 == strcmp(pData, SET_VISRION_MODEL))
        {
            char szBuf[128] = {0};
            sprintf_s(szBuf, "R%s,%d\n", SET_VISRION_MODEL, 0);

            if (0 <= m_pSocketInstance->ClientSocketSend(szBuf, (int)strlen(szBuf)) + 1)
            {
                LOG_INFO(szBuf);
            }
        }
        else if(0 == strcmp(pData, GET_IMAGE_BUF))
        {
			int senceid = atoi(strtok(NULL, m_IPDivid));
			//senceid = 0;
			msg_post(MSG_SEND_IMAGE_PATH, (void *)&senceid, sizeof(senceid),MODULE_SCENE_MAIN_SDK);////解析GI返回路径
			//msg_post(MSG_SEND_IMAGE_PATH, (void *)&senceid, sizeof(senceid), MODULE_SCENE_MAIN_UI);///显示在运行listinfo中
        }

		return -1;
    }