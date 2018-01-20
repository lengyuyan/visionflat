#ifndef _FLAT_FISH_MANAGER_H_
#define _FLAT_FISH_MANAGER_H_
#include <map>
using namespace std;
#pragma comment(lib, "VisionProcess.lib")

#include "DataType.h"
#include "sysapi.h"
#include "common.h"
#include "iManagerCtrl.h"

#define INIT_NULL           0x00000000
#define INIT_CONFIG_OK      0x00000001
#define INIT_CAMERA_OK      0x00000002
#define INIT_LIGHT_OK       0x00000004
#define INIT_CONNECT_OK     0x00000008
#define INIT_VISION_OK      0x00000010

class CVisionServer;

extern CHECK_RESULT  SceneProcResult;

struct WORKING_THREAD_DATA
{
    int    iSceneID;     //当前线程处理的场景编号
    HANDLE thHandle;     //当前线程句柄
    HANDLE thBegin;     //开始, 线程启动或流程结束后暂停等待该信号
    HANDLE thRunning;   //正在处理流程
    HANDLE thExit;      //退出线程信号

    WORKING_THREAD_DATA()
    {
        iSceneID = -1; 
        //thHandle = CreateEventA(NULL,TRUE,FALSE,NULL); 
        thBegin = CreateEventA(NULL,TRUE,FALSE,NULL); 
        thRunning = CreateEventA(NULL,TRUE,FALSE,NULL);
        thExit = CreateEventA(NULL,TRUE,FALSE,NULL);   
    }
    ~WORKING_THREAD_DATA()
    {
        CloseHandle(thHandle);
        CloseHandle(thBegin);
        CloseHandle(thRunning);
        CloseHandle(thExit);
    }
};

class CManagerCtrl
{
public:
    CManagerCtrl();
    ~CManagerCtrl();
    bool InitManager();
    void InitReadConfig();//读系统配置文件及场景文件
    bool UnInitManager();
    bool TriggerVisionProcess(int iSceneID = 0, int iPercent = 100);//视觉处理和标定
    int GetVisionResult(SCENE_PKG *pConfig, int iSceneID);
    static void ThreadVisionProcess(void *pParam);//线程函数视觉处理，直接调用SingleVisionProcess
    static void ThreadCalibProcess(void *pParam);//线程函数标定
    bool VisionCalibProcess(void *pParam); //ThreadCalibProcess调用，他本身调用SingleVisionProcess
	int SceneGrabImage(int iSceneID,int ImageIndex,int CamIndex,int shutterTime,int Gain,int Interval,int LightIndex,int LightValue);
    int SingleVisionProcess(int iSceneID, CHECK_RESULT &result,unsigned int type);//处理和标定都用
    bool SetSysParam(void *pData);
	void SetProctocolData(const PROCTOCOLDATA& ProctoData);	
	void SetProductMap(void* pMapData);////产品的设定函数，用于换产品时一键切换
 	static void GetProductMap(void* pMapData);
    bool SaveSysParam(void *pData = NULL);
    bool GetSysParam(void *pData);
	static void* GetSysParamPoint();
    bool SetSceneParam(void *pData);
	char* GetSysconfigPath();
	char* GetSceneName();
	int GetSceneProcType(int SceneIndex);
    int	GetSceneId(int SceneIndex);
	char* GetSceneconfigPath(int SceneID);
	char* GetScenePath(int SceneID);
    bool SaveSceneParam(int iSceneID, void *pData);
    bool GetSceneParam(int iSceneID, void *pData);
	int GetSceneNum();
	bool GrabWay2Sceneimage(int SceneID,int ImageIndex,int CamIndex,int shutterTime,int Gain,int Interval,int LightIndex,int LightValue);
    void SetCamParam(int CamIndex,int shutterTime,int Gain);
    bool LightCtrl(int iSceneID, int index = 0);
    bool InitGrabSceneimage(int iSceneID, int index = 0);
	bool InitGrabSceneimage1(int CamIndex ,int iSceneID,int ImageIndex);
    bool SetGain(int iSceneID, int iValue, int index = 0);
    int GetGain(int iSceneID, int index = 0);
    bool ReadImage(int iSceneID, const char *pFileName, int index = 0);
    bool SaveImage(int iSceneID, int index, string SNstr);
	bool SaveImageDump(const long winID,int iSceneID,char* outpath);
	void* GetImageDump(const long winID,int iSceneID,double &width,double &heigh);
	char* getImagePath(int iSceneID, int index,unsigned int type);
    bool DispObj(int iWinID, int iSceneID, bool bIsShowTmp = false);
    bool DispImage(long iWinID, int iSceneID, int index = 0);
	bool FindHalconWinID(int iWinID, long& HalconWinID);
	bool ShowVisionString(long winID,char* StrContent,int topRow = -1,int topCol = -1);
	bool SetVisionColorHeight(long winID,char* strColor,int FontHeight=0);
    bool BindWindow(long iWinID, int x, int y, int width, int hight, bool bDebug = false);
    bool SetDebugWindowID(int iSceneID, long iWinID);
    bool DispROI(int iWinID, int iSceneID, int index, PROCESS_TYPE iType);
    bool DrawROI(int iWinID, int iSceneID, int index, PROCESS_TYPE iType = MAIN_PRO);
	bool OnDrawROI(int iWinID, int iSceneID,  PROCESS_TYPE iType ,RoiCombType CombType,vector<int>* pRoiIdList);
	void GenShape(int SceneID,const ST_ROI_INFO& roi,ShapeType shapeflag,int ShapeId,int CamIndex);
	int GetCurShapeId( int iSceneID);
	bool  GetRoiParam(int sceneID,int ShapeId,ST_ROI_INFO* pRoi);
	void OnShowAllRoi(int iWinID, int iSceneID,const vector<int>* const pRoiDdList);
    bool CreateVisionModel(int iWinID,int iSceneID, int index = 0);
	bool CreateVisionModel(int iWinID,int iSceneID,int RoiID,char* modePath, int Nevels,double AngleStart,double AngleExtent,
	int Contrast,int index = 0);
    int UpdateVisionModel(int iSceneID, int index = 0, int iType = 0);
    bool FindVisionModel(int iSceneID, int index = 0, bool bIsB = false);


	bool FindWay2VisionModel(int iSceneID, int WinID,int index,char* szModePath,int PyramidLevel,int MatchNum, double& Score,
		                                 int ProcIndex,bool IsBulid,double& row,double& col,double& phi);

	bool FindModelRealTime(int iSceneID, int WinID,int index,char* szModePath,int PyramidLevel,int MatchNum, double& Score,
		int ProcIndex,bool IsBulid,double& row,double& col,double& phi);

	void MoveAndRotInCamForOriMatch(int iSceneID,double PixSize,bool AxeMoveRotDirect,double& MotorRelX,double& MotorRelY,
		                    int LastSceneId,int NextSceneId,double SceneRotPhi,double PicCentX,double PicCentY,double RotCenX,double RotCenY,double MotorRotPhi,double CalRotPhi,
							 double (*pCamParam),int ParamNum,double (*pCamPose),double CalThick,
							 int CamIndex,int AxeIndex,int ToolIndex);


		/**********************************************************************
	函数      : AffinePointByCamInMecRot
	功能      : 将一个点做位移变换到，相应相机在相应机械轴对应的机械坐标系中的旋转变换
	输入参数  :  
	输出参数  : N/A
	返回值    : 成功或失败
	备注      : 
	**********************************************************************/
	bool AffinePointByCamInMecRot(int CamIndex,int AxisIndex,int ToolIndex,double& PointX,double& PointY);

	void AffToRobotWorld(int SceneID,int CamIndex,int AxisIndex,int ToolIndex,double PixSize,double ModRotPhi,
		double& ToolWorldX,double& ToolWorldY,bool WithCamParam,double CalThickness,double* pCamParam,int ParamNum,double* CamPos);

	void AffinePointToWorld(int CamIndex,int AxisIndex,int ToolIndex,double PixSize,double OriCamX,double OriCamY,
		double OriCamPhi,double CurCamX,double CurCamY,double CurCamPhi,double& MoveX,double& MoveY,int ResType,
		bool MoveRotDirect,double CurMatchPhi,double AxeRotX,double AxeRotY);

	bool VisionGetResult(int iSceneID, int index,CHECK_RESULT &result);
    bool VisionDebugThreshold(int iWinID, int iSceneID, int index);
    bool VisionFindLine(int iSceneID, int index, bool bIsFollow = false);
    bool VisionFindCircle(int iSceneID, int index, bool bIsFollow = false);
	bool VisionMeasureCircle(long HwinID,int iSceneID, int Imageindex,int MatchIndex, double& Row,double& Col,double& Rii);
    bool ClearWindow(int iWindowID);
    bool ShowPart(long iWinID, int xdistance, int ydistance, int iPercent);
    bool OpenContinousGrab(int iWindowID, int iSceneID, int index, unsigned int MsgID,int CameraIndex);
    bool ColseContinousGrab(int iSceneID, int index,int CameraIndex);
    static bool GetCurrentStatus(unsigned long ID);
    static bool SetCurrentStatus(unsigned long ID);
    static bool DelCurrentStatus(unsigned long ID);
    void RestoreImagePart(int WinID, int iScene, int width, int height, int index = 0);   
    //图像缩放拖拽
    //1. 窗口ID, 
    //2. 拖拽类型（a, 左;b, 右;c, 上;d, 下;e, 放大;f, 缩小, g, 还原）
    //3. 鼠标前后位置(基于当前控制上的位置)
    //4. 缩放值
    void UpdateImagePart(int WinID, int iScene, int iType, double dXPer = 1.0, double dYPer = 1.0);
	bool AffineTransPoint(int iSceneID, int iCameraID, const CHECK_RESULT resultIn,CHECK_RESULT &resultOut);
    static void PostMsgToUIForDisp(void* context);
    bool CManagerCtrl::OpenCheckStateThread();
    static DWORD WINAPI CManagerCtrl::CheckStateThread(LPVOID lpParameter);
    static void RemoteSystemMsg(int iType = 0, void *param = NULL);
    static int WINAPI RemoteManualMsg( unsigned int wParam, void * lParam, void* otherparam);
    static CManagerCtrl *GetInstance();
    static void ReleaseInstance();
    static int FindSpecTypePos(const VIRSION_DATA_PKG *pkg, unsigned int *pType, int iType);
    static int FindParamData(SCENE_PKG *pkg, int iType, int index);
    int InsertParamItem(int SceneId, const VIRSION_DATA_PKG *pkg, unsigned int *pType, int iSize, int insertPos);
	int UpdateParamItem(int SceneId, const VIRSION_DATA_PKG *pkg, unsigned int *pType, INTERFACE_SET_DATA *dataTmp, int iSize, int insertPos, bool insert = false);
    int SwapParamItem(int SceneId, int iPosF, int iPosN);
    int DeleteParamItem(int SceneId, int insertPos);
	int GenProductId(string name);
	int FindProductId(string name);
	void SetCurProductName(string name);
	
	void AlterMainWndTitle(string name);

	//对通信传来的场景号映射到实际场景号
	int MapSceneIdTelToReal(int TelSceneId);

	//将实际场景号映射到通信场景号
	int MapSceneIdRealToTel(int RealSceneId);

	void ManagerGrabFinish(int& iSceneID, CHECK_RESULT *res,int ErrorCode);
	int  ManagerProcFinish(int& iSceneID, CHECK_RESULT *pResult, int iErrorCode);

	bool HalconProcedureLocate(long iWinID,int iSceneID,int Imageindex,string ProcedurePath,
		double (*inParam)[HalProcInParamNum], double& PosX,double& PosY,double& PosPhi);

	bool HalconProcedureProcess(long iWinID,int iSceneID,int Imageindex,string ProcedurePath,vector<double> &inparam,vector<double> &outparam,int display,int &error,string &hmessage);
	bool ImageEnhancement(long iWinID,int iSceneID,int Imageindex,double& multinum,double& addnum,int display,int &error,string &hmessage);
	
public:
	void GetSceneProcess(pSceneProcess* pProcess);
	pSceneProcess* m_SceneProcess;


	static SCENE_PKG *GetSceneHandle(int iSceneID);
	CHECK_RESULT* GetSceneProcResult(string &scneName);						//获取指定场景名字的结果		new
	CHECK_RESULT* GetSceneProcResultById(int  iSceneID);					//获取指定场景名字的结果		new
	int SetSceneProcResult(string &scneName,CHECK_RESULT& res);				//获取指定场景名字的结果		new
	int ClearProcResultByID(int iSceneID);                                  //清除对应场景ID的结果          new
private:
	void ClearSceneProcResultMap();											//清除场景结果保存map			new
	int  InitSceneProcResultMap();											//根据系统参数来初始化结果map	new 

private:
    static CManagerCtrl *m_pInstance;
    map<long, long>      m_mapWinID;
    map<int, WORKING_THREAD_DATA*> m_ThSceneData;
    static CRITICAL_SECTION *m_pSection;
    static SYSTEM_PARAM   m_stSysParam;             //系统参数
    CVisionServer    *m_pVisionInstance;
    static vector<SCENE_PKG *> m_vecSceneParam;
	char m_ScenePath[256];
    unsigned long         m_SceneWidth[SCENE_SIZE];
    unsigned long         m_SceneHeight[SCENE_SIZE];
    unsigned long         m_CalibWindID;     //调试窗口
    CALIB_DATA            m_CalibData;      //   相机标定数据
	double                m_MachineX[16];
	double                m_MachineY[16];
	int                   m_PixCol[16];
	int                   m_PixRow[16];
    bool                  m_iExitThread;
    HANDLE                m_RecvEvent;
    bool                  m_bIsWorking[SCENE_SIZE];  //当前场景是否在工作中
    unsigned long         m_WorkingStatus;       //0为工作模式， 1为调试模式
    char                  m_chSN[64];              //SN信息

	//定义一个数据结构用来记录测试的实时数据,每个场景一个CHECK_RESULT
	map<string,CHECK_RESULT*>m_mapSceneProcResult;
	Handle                  m_mapSceneResultSection; //临界区

};
#endif 