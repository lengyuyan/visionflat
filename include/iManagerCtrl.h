#ifndef _I_FLAT_FISH_MANAGER_H_
#define _I_FLAT_FISH_MANAGER_H_

#ifndef EXPORT_C
#define EXPORT_C extern "C"
#endif

#ifndef DLL_EXPORT
#define DLL_EXPORT _declspec(dllexport)
#endif



#include <list>
#include <string>
#include "DataType.h"

typedef BOOL (pSceneProcess)(int,CHECK_RESULT&);/*WINAPI*/

typedef enum _IMAGE_ZOOM_TYPE
{
    MOVE_TPYE = 0,          //移动
    ENLARGE_TYPE,              //放大
    SHRINK_TYPE,               //缩小
    RESTORE_TYPE,            //还原
}IMAGE_ZOOM_TYPE;


/**********************************************************************
函数      : InitManager
功能      : 初始化相机灯光,网络等
输入参数  : N/A
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool /*WINAPI*/ InitManager(); //加WINAPI,若没有def文件会出现名称改变


/**********************************************************************
函数      : UnInitManager
功能      : 去初始化
输入参数  : N/A
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool UnInitManager();

/**********************************************************************
函数      : GetSceneProcess
功能      : 获取场景处理函数地址
输入参数  : pSceneProcess  函数地址
输出参数  : 
返回值    : 
备注      : SDK入口函数
**********************************************************************/
EXPORT_C DLL_EXPORT void GetSceneProcess(pSceneProcess* pProcess);

/**********************************************************************
函数      : GetSceneNameInfo
功能      : 获取场景信息
输入参数  : N/A
输出参数  : iSceneNum 场景个数
            pSceneName 场景别名列表
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool GetSceneNameInfo(int &iSceneNum, char **pSceneName);

/**********************************************************************
函数      : SetSysParam
功能      : 更新视觉配置
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
            pData    要设置值的结构体指针
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool SetSysParam(void *pData);

/**********************************************************************
函数      : SaveSysParam
功能      : 更新并保存视觉配置
输入参数  : N/A 
            pData    要设置值的结构体指针
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool SaveSysParam(void *pData = NULL);

/**********************************************************************
函数      : GetSysParam
功能      : 获取系统参数
输入参数  : pData要获取的结构体指针 
输出参数  : pData要获取的结构体指针
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool GetSysParam(void *pData);

/**********************************************************************
函数      : GetSysParam
功能      : 获取系统参数
输入参数  : N/A
输出参数  : 
返回值    : 要获取的结构体指针
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT void* GetSysParamPoint();

/**********************************************************************
函数      : SetSceneParam
功能      : 更新视觉配置
输入参数  : pData    要设置场景的结构体指针
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool SetSceneParam(void *pData);

/**********************************************************************
函数      : SaveSceneParam
功能      : 保存并更新视觉配置
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
            pData    要设置值的结构体指针
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool SaveSceneParam(int iSceneID, void *pData = NULL);

/**********************************************************************
函数      : GetSceneParam
功能      : 更新视觉配置
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : pData    要获取的结构体指针
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool GetSceneParam(int iSceneID, void *pData);



EXPORT_C DLL_EXPORT void* GetSceneParamHandle(int iSceneID);


/**********************************************************************
函数      : GetSceneNum
功能      : 获取场景数量
输入参数  : 
输出参数  : 
返回值    : 配置文件设定的场景数量，可能会有空的场景
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT int GetSceneNum();

EXPORT_C DLL_EXPORT  char* GetSceneConfigName();

EXPORT_C DLL_EXPORT const char* GetSceneConfigPath(int SceneID);

EXPORT_C DLL_EXPORT const char* GetScenePath(int SceneID);


EXPORT_C DLL_EXPORT char* GetSysConfigPath();

EXPORT_C DLL_EXPORT int GetSceneProcType(int SceneIndex);

EXPORT_C DLL_EXPORT int GetSceneId(int SceneIndex);

/////////////////////////////////////////////////////////////////////////////////////////
//相机处理
// 
/**********************************************************************
函数      : SceneAcqImage
功能      : 获取图片数据
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
            index 当前场景中的第几张图像（存在多张图像则可能根据index区分不同光源或采图参数）
			...拍照参数

输出参数  : N/A
返回值    : 
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT int SceneGrabImage(int iSceneID,int ImageIndex,int CamIndex,int shutterTime,int Gain,int Interval,int LightIndex,int LightValue);

EXPORT_C DLL_EXPORT void SetCamParam(int CamIndex,int shutterTime,int Gain);

EXPORT_C DLL_EXPORT int SceneGrabFinished(int iSceneID, CHECK_RESULT *res,int iErrorCode);


 
/**********************************************************************
函数      : InitGrabSceneimage
功能      : 初始化获取图片数据
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
            index 当前场景中的第几张图像（存在多张图像则可能根据index区分不同光源或采图参数）
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool InitGrabSceneimage(int iSceneID, int index = 0);

/**********************************************************************
函数      : InitGrabSceneimage
功能      : 初始化获取图片数据
输入参数  : strCameraGuid 相机Uid号 
           
输出参数  : N/A
返回值    : 成功或失败
备注      :  功能还未实现
**********************************************************************/
EXPORT_C DLL_EXPORT bool InitGrabSceneimage1(int CamIndex ,int iSceneID,int ImageIndex = 0);

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
EXPORT_C DLL_EXPORT bool SetGain(int iSceneID, int iValue, int index = 0);

/**********************************************************************
函数      : GetGain
功能      : 获取相机采图增益
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
            index 当前场景中的第几张图像（存在多张图像则可能根据index区分不同光源或采图参数）
输出参数  : N/A
返回值    : 获取的增益值
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT int GetGain(int iSceneID, int index = 0);

//枚举相机
/**********************************************************************
函数      : GetCameraIDList
功能      : 获取相机GUID列表
输入参数  : guidlist， GUID列表
输出参数  : N/A
返回值    : 返回结果
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT int GetCameraIDList(std::list<std::string> &guidlist,int dllnum,string dlllist[]);

/**********************************************************************
函数      : TriggerVisionProcess
功能      : 触发视觉处理处理
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool TriggerVisionProcess(int iSceneID = 0, int iPercent = 100);

/**********************************************************************
函数      : BindWindow
功能      : 绑定halcon窗口与UI窗口
输入参数  : iWinID 界面窗口ID
            x,y,width,hight 界面窗口位置大小
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool BindWindow(long iWinID, int x, int y, int width, int hight, bool bDebug = false);

/**********************************************************************
函数      : SetDebugWindowID
功能      : 绑定halcon窗口与UI窗口
输入参数  : iWinID 界面窗口ID
            x,y,width,hight 界面窗口位置大小
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool SetDebugWindowID(int iSceneID, long iWinID);
/**********************************************************************
函数      : ClearWindow
功能      : 清除对应窗口上信息
输入参数  : iWinID 界面窗口ID
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool ClearWindow(long iWinID);

/**********************************************************************
函数      : ReadImage
功能      : 加载本地图片
输入参数  : iSceneID  窗口序号
            pFileName 文件名称
            index 当前场景中的第几张图像（存在多张图像则可能根据index区分不同光源或采图参数）
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT  bool ReadImage(const char *pFileName, int iSceneID, int index = 0);

/**********************************************************************
功能      : 保存指定图片
输入参数  : iSceneID  当前场景ID
index 当前场景中的第几张图像（存在多张图像则可能根据index区分不同光源或采图参数）
SNstr SN信息
输出参数  : 
返回值    : 成功或失败
备注      :  path 为NULL时， 保存到E:/
**********************************************************************/
EXPORT_C DLL_EXPORT bool SaveImage(int iSceneID, int index = 0 , string SNstr = "");

/**********************************************************************
函数      : SaveImageDump
功能      : 保存窗口图像
输入参数  : winID 当前窗口ID
            iSceneID  当前场景ID
输出参数  : N/A
返回值    : 成功或失败
备注      : path 为NULL时， 保存到E:/
**********************************************************************/
EXPORT_C DLL_EXPORT bool SaveImageDump(const long winID, int iSceneID,char* outpath);

/**********************************************************************
函数      : SendImageDump
功能      : 发送窗口图像
输入参数  : winID 当前窗口ID
            iSceneID  当前场景ID
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool SendImageDump(void *buff, int size,int w,int h,int iSceneID);

/**********************************************************************
功能      : 获取窗口图像
输入参数  : winID 当前窗口ID
            iSceneID  当前场景ID
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT void* GetImageDump(int winID, int sence,double &width,double &heigh);

/**********************************************************************
函数      : getImagePath
功能      : 获取图像路径
输入参数  : iSceneID 当前窗口ID
            index  图片索引
			type  图片类型
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT char* getImagePath(int iSceneID, int index,unsigned int type);

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
EXPORT_C DLL_EXPORT bool DispObj(int iWinID, int iSceneID, bool bIsShowTmp = false);

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
EXPORT_C DLL_EXPORT bool DispImage(long iWinID, int iSceneID, int index = 0);

EXPORT_C DLL_EXPORT bool FindHalconWinID(int iWinID, long& HalconWinID);

EXPORT_C DLL_EXPORT bool ShowVisionString(long hwinID,char* StrContent,int topRow = -1,int topCol = -1);
EXPORT_C DLL_EXPORT bool SetVisionColorHeight(long hwinID,char* strColor,int FontHeight=0);

/**********************************************************************
函数      : DispROI
功能      : 显示ROI
输入参数  : iWinID  窗口序号
index 当前场景中的第几个ROI
iType, 0,为处理图像区域, 1为模板区域，2为画直线， 3为画圆， 
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT  bool DispROI(int iWinID, int iSceneID, int index, PROCESS_TYPE iType = MODEL_PRO);

/**********************************************************************
函数      : DrawROI
功能      : 置为绘制ROI模式
输入参数  : iWinID  窗口序号
index 当前场景中的第几个ROI
iType, 0,为处理图像区域, 1为模板区域，2为画直线， 3为画圆， 
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT  bool DrawROI(int iWinID, int iSceneID, int index, PROCESS_TYPE iType = MODEL_PRO);


/**********************************************************************
函数      : DrawROI
功能      : 直接绘制ROI
输入参数  : iSceneID  窗口序号
            iType, 画直线， 画圆， 
输出参数  : 
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool OnDrawROI(int iWinID, int iSceneID,PROCESS_TYPE iType,RoiCombType CombType,vector<int>* const pRoiDdList);
EXPORT_C DLL_EXPORT void  GenShape(int SceneID,const ST_ROI_INFO& roi,ShapeType shapeflag,int ShapeId,int CamIndex);

EXPORT_C DLL_EXPORT int GetCurShapeId(int iSceneID);


EXPORT_C DLL_EXPORT bool   GetRoiParam(int sceneID,int ShapeId,ST_ROI_INFO* pRoi);


EXPORT_C DLL_EXPORT void OnShowAllRoi(int iWinID, int iSceneID,const vector<int>* const pRoiDdList);

/**********************************************************************
函数      : CreateVisionModel
功能      : 创建模板
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
                index 场景中图像序号
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool CreateVisionModel(int iWinID,int iSceneID, int index = 0);


EXPORT_C DLL_EXPORT bool CreateWay2VisionModel(int iWinID,int iSceneID,int RoiID,char* modePath, int Nevels,double AngleStart,double AngleExtent,
	                 int Contrast,int index = 0);

//
///**********************************************************************
//函数      : UpdateVisionModel
//功能      : 更新模板
//输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
//            index 场景中图像序号
//输出参数  : N/A
//返回值    : 成功或失败
//备注      : 
//**********************************************************************/
//EXPORT_C DLL_EXPORT bool UpdateVisionModel(int iSceneID, int index = 0, int iType = 0);

/**********************************************************************
函数      : FindVisionModel
功能      : 查找模板
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
            index 场景中图像序号
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool FindVisionModel(int iSceneID, int index = 0, bool bIsB = false);


EXPORT_C DLL_EXPORT bool FindWay2VisionModel(int iSceneID, int WinID, int index,char* szModePath,int PyramidLevel,int MatchNum, double& Score,
	            int ProcIndex,bool IsBulid,double& row,double& col,double& phi);

//新增一个可以实时实现机器人对准的十字
EXPORT_C DLL_EXPORT bool FindModelRealTime(int iSceneID, int WinID, int index,char* szModePath,int PyramidLevel,int MatchNum, double& Score,
	int ProcIndex,bool IsBulid,double& row,double& col,double& phi);

EXPORT_C DLL_EXPORT void MoveAndRotInCamForOriMatch(int iSceneID,double PixSize,bool AxeMoveRotDirect,double& MotorRelX,double& MotorRelY,
	     int LastSceneId,int NextSceneId,double SceneRotPhi,double PicCentX =0.0,double PicCentY = 0.0,double* pCamParam = NULL,int ParamNum = 0,double* pCamPose = NULL,double CalThick=0.0,
		 double RotCenX= 0.0,double RotCenY = 0.0 ,double MotorRotPhi = 0.0,double CalRotPhi = 0.0,
		 int CamIndex = -1,int AxeIndex = -1,int ToolIndex = -1);

EXPORT_C DLL_EXPORT bool AffinePointByCamInMecRot(int CamIndex,int AxisIndex,int ToolIndex,double& PointX,double& PointY);

EXPORT_C DLL_EXPORT void AffinePointToWorld(int CamIndex,int AxisIndex,int ToolIndex,double PixSize,double OriCamX,double OriCamY,
	double OriCamPhi,double CurCamX,double CurCamY,double CurCamPhi,double& MoveX,double& MoveY,int ResType,
	bool MoveRotDirect,double CurMatchPhi,double AxeRotX,double AxeRotY);

EXPORT_C DLL_EXPORT void AffToRobotWorld(int SceneID,int CamIndex,int AxisIndex,int ToolIndex,double PixSize,double ModRotPhi,
	      double& ToolWorldX,double& ToolWorldY,bool WithCamParam,double CalThickness,double* pCamParam,int ParamNum,double* CamPos);


/**********************************************************************
函数      : VisionProcess
功能      : 整体检测
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool VisionProcess(int iSceneID, int index,CHECK_RESULT &result);

/**********************************************************************
函数      : VisionCalibProcess
功能      : 标定处理
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool VisionCalibProcess(CALIB_DATA *pData);
/**********************************************************************
函数      : VisionDebugThreshold
功能      : 调试阈值接口
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
index， 根据该index获取参数
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
 EXPORT_C DLL_EXPORT bool VisionDebugThreshold(int iWinID, int iSceneID, int index);

/**********************************************************************
函数      : VisionFindLine
功能      : 找线接口
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
            index， 根据该index获取找圆参数
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool VisionFindLine(int iSceneID, int index, bool bIsFollow = false);

/**********************************************************************
函数      : VisionFindCircle
功能      : 找圆接口
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
            index， 根据该index获取找圆参数
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool VisionFindCircle(int iSceneID, int index, bool bIsFollow = false);


EXPORT_C DLL_EXPORT bool VisionMeasureCircle(long WinId,int iSceneID, int Imageindex,int MatchIndex, double& Row,double& Col,double& Rii);


/**********************************************************************
函数      : OpenContinousGrab
功能      : 开户实时显示
输入参数  : iWindowID 窗口句柄
            iSceneID 场景ID， 根据ID判断调用接口及参数  
            index 抓图序号
            通知窗口显示处理的消息ID
输出参数  : N/A
返回值    : 成功或失败
备注      : 打开实时后， 该接口注册回调给实时采集， 回调中通知窗口显示图像
            消息ID   
**********************************************************************/
EXPORT_C DLL_EXPORT bool OpenContinousGrab(int iWindowID, int iSceneID, int index, unsigned int MsgID,int CameraIndex = -1);

/**********************************************************************
函数      : ColseContinousGrab
功能      : 关闭实时显示
输入参数  : iSceneID 场景ID， 根据ID判断调用接口及参数  
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool ColseContinousGrab(int iSceneID, int index,int CameraIndex = -1);

//comm口操作接口
/**********************************************************************
函数      : CtrlComm
功能      : 打开or关闭光源
输入参数  : pCommName 串口名称
            bOpen     打开操作或关闭操作
            iType     光源类型
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool CtrlComm(LightInfo *pInfo, bool bOpen = true, LightTypeInfo *pTypeInfo = NULL);

/**********************************************************************
函数      : RestoreImagePart
功能      : 恢复图像显示比例
输入参数  : WinID   窗口ID
            iScene  场景ID
            
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT void RestoreImagePart(int WinID, int iScene, int width, int height, int index = 0);
//图像缩放拖拽
//1. 窗口ID, 
//2. 拖拽类型（a, 左;b, 右;c, 上;d, 下;e, 放大;f, 缩小, g, 还原）
//3. 鼠标前后位置(基于当前控制上的位置)
//4. 缩放值
EXPORT_C DLL_EXPORT void UpdateImagePart(int WinID, int iScene, int iType, double dXPer = 1.0, double dYPer = 1.0);
/**********************************************************************
函数      : ResetFinished
功能      : 重置系统返回消息
输入参数  : iErrorCode 错误码
输出参数  : N/A
返回值    : 成功或失败
备注      : N/A
**********************************************************************/
EXPORT_C DLL_EXPORT int ResetFinishedSendMsg(int iErrorCode = 0);
/**********************************************************************
函数      : OpenCheckStateThread
功能      : 打开检测系统状态线程
输入参数  :   

输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT bool OpenCheckStateThread();
/**********************************************************************
函数      : JudgeLightConnect
功能      : 判断光源控制器是否连接成功
输入参数  : 串口名称
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/
EXPORT_C DLL_EXPORT int JudgeLightConnectStatus(string comName);

//获取对应类型参数在pkg->pBody中的位置
//input 类型， 索引， pkg, 
//返回找到位置
EXPORT_C DLL_EXPORT int IFindParamData(SCENE_PKG *pkg, int iType, int index);

EXPORT_C DLL_EXPORT int FindSpecTypePos(const VIRSION_DATA_PKG *pkg, unsigned int *pType, int iType = 0);

EXPORT_C DLL_EXPORT int InsertParamItem(int SceneId, VIRSION_DATA_PKG *pkg, unsigned int *pType, int iSize, int insertPos);
EXPORT_C DLL_EXPORT int UpdateParamItem(int SceneId, const VIRSION_DATA_PKG *pkg, unsigned int *pType, INTERFACE_SET_DATA *dataTmp, int iSize, int insertPos, bool insert = false);
EXPORT_C DLL_EXPORT int SwapParamItem(int SceneId, int iPosF, int iPosN);
EXPORT_C DLL_EXPORT int DeleteParamItem(int SceneId, int insertPos);

//操作光源， comName为串口名称,  index为通道索引， 从0开始， value为设置的值, type为操作类型，打开、关闭、设置
EXPORT_C DLL_EXPORT int OpratorLight(string comName, int index, int value, int type);

/**********************************************************************
函数      : 绘制形状
功能      : 在图像窗口上绘制图像
输入参数  : 
输出参数  : N/A
返回值    : 成功或失败
备注      : 
**********************************************************************/

//增加网络通讯数据协议的保存
EXPORT_C DLL_EXPORT void  SetWebProctocol(const PROCTOCOLDATA& ProctoData);

EXPORT_C DLL_EXPORT void  GetWebProctocol(PROCTOCOLDATA& ProctoData);


////产品的设定函数，用于换产品时一键切换
EXPORT_C DLL_EXPORT void SetProductMap(void* pMapData);

EXPORT_C DLL_EXPORT void GetProductMap(void* pMapData);

EXPORT_C DLL_EXPORT void SetSysCurProductName(string ProductName);

EXPORT_C DLL_EXPORT void SaveSysRotAxleAdjust(int AxisIndex, int ToolIndex,double RotXAd,double RotYAd);
EXPORT_C DLL_EXPORT void ReadSysRotAxleAdjust(int AxisIndex, int ToolIndex,double RotXAd,double RotYAd);

//halcon定位,可以直接定位到算法dll,无需经过manager，不需要他管理
EXPORT_C DLL_EXPORT bool HalconProcedureLocate(long iWinID,int iSceneID,int Imageindex,string ProcedurePath,
	double (*inParam)[HalProcInParamNum],double& PosX,double& PosY,double& PosPhi);

//halcon处理
EXPORT_C DLL_EXPORT bool HalconProcedureProcess(long iWinID,int iSceneID,int Imageindex,string ProcedurePath,vector<double> &inparam,vector<double> &outparam,int display,int &error,string &hmessage);

//halcon增强
EXPORT_C DLL_EXPORT bool IImageEnhancement(long iWinID,int iSceneID,int Imageindex,double& multinum,double& addnum,int display,int &error,string &hmessage);


EXPORT_C DLL_EXPORT CHECK_RESULT* GetSceneProcResult(string &scneName);						//获取指定场景名字的结果返回指针失去了所的意义,但又不能返回结构体,后续改进		new
EXPORT_C DLL_EXPORT CHECK_RESULT* GetSceneProcResultById(int  iSceneID);
EXPORT_C DLL_EXPORT int SetSceneProcResult(string &scneName,CHECK_RESULT& res);				//设置指定场景名字的结果		new
EXPORT_C DLL_EXPORT int ClearProcResultByID(int iSceneID);									//清除指定场景id里的结果内容
#endif 
