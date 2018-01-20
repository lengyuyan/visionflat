#pragma once
#include "DataType.h"
#include <map>
using namespace std;

//这里直接导出CVisionServer类,他管理CVisionProcess类,后续还是应该封装一层c接口导出去
class CVisionProcess;
class _declspec(dllexport) CVisionServer
{
public:
    CVisionServer(void);
    ~CVisionServer(void);

public:
    static CVisionProcess *GetInstance(int iSceneID);					//获取对应场景号的处理对象
    static void ReleaseInstance();										//释放map中的所有处理对象
    virtual bool InitVision(void);										//创建标定程序和标定数据或其他文件夹

    /**********************************************************************
    函数      : OpenWindow
    功能      : 绑定窗口
    输入参数  : winID 需要绑定图像的ID
                x,y,width,hight 要打开halcon窗口的起始位置，宽和高
    输出参数  : halconWinID halcon窗口ID
    返回值    : true成功 false失败
    备注      : 
    **********************************************************************/
    virtual bool OpenWindow(long winID, int x, int y, int width, int hight, long &halconWinID);

	/**********************************************************************
    函数      : setpart
    功能      : 设置halcon窗口在mfc窗口的显示范围
    输入参数  : winID 窗口ID
               topx,topy,得到窗口区域的左上角的坐标，lowx，lowy，右下角坐标
    输出参数  : 
    返回值    : true成功 false失败
    备注      : 
    **********************************************************************/
    virtual bool setpart(long winID,int topx, int topy,int lowx,int lowy);

	/**********************************************************************
    函数      : HalconProcedureLocate
    功能      : halcon定位功能
    输入参数  : winID 窗口ID，iSceneID 场景号,halcon程序地址,输入参数地址
    输出参数  : 坐标值,x,y,th
    返回值    : true成功 false失败
    备注      : 
    **********************************************************************/
	virtual bool HalconProcedureLocate(long HwinID,int iSceneID, int Imageindex,string ProcedurePath,
		double (*inParam)[HalProcInParamNum], double& PosX,double& PosY,double& PosPhi);

	virtual bool HalconProcedureProcess(long HwinID,int iSceneID,int Imageindex,string ProcedurePath,vector<double> &inparam,vector<double> &outparam,int display,int &error,string &hmessage);

	virtual bool ImageEnhancement(long iWinID,int iSceneID,int Imageindex,double& multinum,double& addnum,int display,int &error,string &hmessage);
	/**********************************************************************
    函数      : getpart
    功能      : 获取mfc窗口中halcon窗口的显示范围
    输入参数  : winID 窗口ID				
    输出参数  : topx,topy,得到窗口区域的左上角的坐标，lowx，lowy，右下角坐标
    返回值    : true成功 false失败
    备注      : 
    **********************************************************************/
    virtual bool getpart(long winID,int &topx, int &topy,int &lowx,int &lowy);

	/**********************************************************************
    函数      : getimagesize
    功能      : 获取图像大小
    输入参数  : iSceneID 场景号
    输出参数  : 
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
    virtual bool getimagesize(int sceneID,int &width, int &height);


    /**********************************************************************
    函数      : OpenWindow
    功能      : 关闭窗口
    输入参数  : halconWinID halcon窗口ID
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
    virtual bool CloseHalconWindow(long halconWinID);

	//传入相机高宽，获取指向图像内存的指针
	virtual char * GetImageBuf(int iSceneID, int index, int iWidth, int Height);
    /**********************************************************************
    函数      : ReadImage
    功能      : 绑定窗口
    输入参数  : pFileName 图片路径
                sceneID   场景ID
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
    virtual bool ReadImage(const char *pFileName, int sceneID, int index = 0, bool bIsNew = false);

    /**********************************************************************
    函数      : DispImage
    功能      : 绑定窗口
    输入参数  : iWinIndex 显示的目标窗口
                sceneID   场景ID
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
    virtual bool DispImage(int iWinIndex, int sceneID, int index = 0);

    /**********************************************************************
    函数      : DispObj
    功能      : 绑定窗口
    输入参数  : iWinIndex 显示的目标窗口
                sceneID   场景ID
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
    virtual bool DispObj(int iWinIndex, int sceneID, bool bIsShowTmp = false);

	bool ShowWindString(long winID,char* StrContent,int topRow = -1,int topCol = -1);

	bool SetDispColorHeight(long winID,char* strColor,int FontHeight=0);
     
    /**********************************************************************
    函数      : VisionProcess
    功能      : 初始化处理
    输入参数  : N/A
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
    virtual bool VisionProcess(void);

  /**********************************************************************
    函数      : CalCamMetrix
    功能      : 计算相机九点标定后的同质转换矩阵，即相机在（或假想）机械坐标系中的位姿
    输入参数  : N/A
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
	virtual void CalCamMetrix(int CamerIndex,int DataCount,double MotorX[],double MotorY[],double CamRow[],double CamCol[],
		double PixSize,double(*pCamRot),double (*pMecAndCam),double(*pImageRela),double (*pImageAbs),double (*pCamParam),int ParamNum,double (*pCamPose),
		int ToolIndex=0,int AxeIndex=0,double CalFirstX=0.0,double CalFirstY=0.0,double CalFirstAngle=0.0,double ToolFirstX=0.0,
		double ToolFirstY=0.0,double CamRotAngle=0.0,double RotToCamX=0.0,double RotToCamY=0.0,double CalThickNess=0.0);


	virtual void AffineCoorPoint(double CoordX,double CoordY,double CoordAng,double& PointX,double& PointY);

	/**********************************************************************
    函数      : Imagebuffer
    功能      : 图像缓存区
    输入参数  : cameraindex 相机索引号
				sceneID   场景ID
				pbuffer 指向缓存指针
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
    virtual  bool Imagebuffer(int cameraindex, int sceneID, long *pbuffer);
	/**********************************************************************
    函数      : drawRoi
    功能      : 画ROI
    输入参数  : halconID 相机索引号
				roi   传入传出roi参数
				bflag 默认为画矩形，否则为圆形
				bcolor 默认roi画完之后的颜色为绿色，false可自定义颜色
    输出参数  :outroi ,输出roi参数
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
    virtual  bool  drawRoi(int sceneID, long  halconID , const  ST_ROI_INFO  &roi, ST_ROI_INFO &outroi ,bool bflag = true,bool bcolor = true);


    /**********************************************************************
    函数      : OndrawRoi
    功能      : 画ROI
    输入参数  : halconID 相机索引号
				roi   传入传出roi参数
				bflag 默认为画矩形，否则为圆形
				bcolor 默认roi画完之后的颜色为绿色，false可自定义颜色
    输出参数  :outroi ,输出roi参数
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
    virtual  bool  OndrawRoi(int sceneID, long  halconID , const  ST_ROI_INFO  &roi, RoiCombType CombType ,PROCESS_TYPE bflag ,vector<int>* pRoiIDList,bool bcolor = true);


	void GenShape(int SceneID,const ST_ROI_INFO& roi,ShapeType shapeflag,int ShapeId,int Width,int Height);

	virtual int GetCurShapeId(int sceneID);

		/**********************************************************************
    函数      : 
    功能      : 获取基本图元的参数
    输入参数  : halconID 显示窗口ID
    输出参数  :
    返回值    : 无
    备注      : 
    **********************************************************************/
	 virtual  bool  GetRoiParam(int sceneID,int ShapeId,ST_ROI_INFO* pRoi);

	/**********************************************************************
    函数      : OnShowAllRoi
    功能      : 显示所有ROI
    输入参数  : halconID 显示窗口ID
    输出参数  :
    返回值    : 无
    备注      : 
    **********************************************************************/
	virtual  void OnShowAllRoi(int sceneID,long  halconID,const vector<int>* const pRoiDdList);

	/**********************************************************************
    函数      : DispRoi
    功能      : 画ROI
    输入参数  : halconID 相机索引号
				roi   传入传出roi参数
				bflag 默认为画矩形，否则为圆形
				bcolor 默认roi画完之后的颜色为绿色，false可自定义颜色
    输出参数  :outroi ,输出roi参数
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
    virtual  bool  dispRoi(int sceneID, long  halconID , ST_ROI_INFO  roi,bool bflag = true,bool bcolor = true);

	/**********************************************************************
    函数      : saveWinImage
    功能      : 保存窗口图像
    输入参数  : winID 当前窗口ID
				path 保存路径
                SNstr  SN信息
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
	virtual bool saveWinImage(const long winID,char *path,char *outpath,string SNstr);

	/**********************************************************************
    函数      : getWinImage
    功能      : 保存窗口图像
    输入参数  : winID 当前窗口ID
			    pBuffer 图像缓存
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
	virtual void* getWinImage(const long winID,double &width,double &heigh);

	/**********************************************************************
    函数      : saveImage
    功能      : 图像缓存区
    输入参数  : sceneID   场景ID
				index 图片索引
				path 图像保存路径	
                SNstr  SN信息
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
    virtual bool  saveImage(int  sceneID,int index,char *path,string SNstr);

	/**********************************************************************
    函数      : saveImagePath
    功能      : 保存结果路径
    输入参数  : sceneID   场景ID
				index 图片索引
				path 图像保存路径	
                SNstr  SN信息
				type 什么图片路径：结果还是原始图片路径，默认结果
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
	virtual bool saveImagePath(int  sceneID,int index,char *path,string SNstr,unsigned int type = 0);

	/**********************************************************************
    函数      : getImagePath
    功能      : 保存结果路径
    输入参数  : sceneID   场景ID
				index 图片索引	
                SNstr  SN信息
				type 什么图片路径：结果还是原始图片路径，默认结果
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
	virtual char* getImagePath(int  sceneID,int index,unsigned int type = 0);
	/**********************************************************************
    函数      : buildModel
    功能      : 创建模板
    输入参数  :  winID 窗口ID   modelID  模板ID
				modelroi   创建模板的感兴趣区
				path  模板保存路径
    输出参数  : N/A
    返回值    :模板ID
    备注      : 
    **********************************************************************/
    virtual  int  buildModel(int iWinID,int sceneID,int index, SCENE_PKG *pkg,char *sceneName,char *path);


	virtual  bool  buildModel(int iWinID,int iSceneID,int RoiID,char* modePath, int Nevels,double AngleStart,double AngleExtent,
                     int Contrast,int index = 0);
    
	//virtual  bool  MatchModel(double& row,double& col,double& phi);

	/**********************************************************************
    函数      : clearModel
    功能      : 清除模板
    输入参数  : sceneID  场景ID   
                iModelID  模板ID
                iType     模板类型
    输出参数  : N/A
    返回值    : 结果
    备注      : 
    **********************************************************************/
    int clearModel(int sceneID, int iModelID, int iType = 0);
	/**********************************************************************
    函数      : readModel
    功能      : 读取模板
    输入参数  : path 模板路径
				sceneID   场景ID
                index    模板索引
                iType    模板类型
    输出参数  : N/A
    返回值    : 模板ID
    备注      : 
    **********************************************************************/
    virtual  int  readModel(int sceneID,char *sceneName,char *path, int index = 0, int iType = 0);

	/**********************************************************************
    函数      : findModel
    功能      : 查找模板
    输入参数  : sceneID   场景ID
                param 模板参数
				data  模板定位结果
                Score 模板分数
    输出参数  : N/A
    返回值    : 模板ID
    备注      : 
    **********************************************************************/
    virtual  int  findModel(const int sceneID,int index, SCENE_PKG *pkg,  ST_RECTANGLE_DATA &data, unsigned int bDisplay = 1);


	/**********************************************************************
    功能      : 查找模板
    输入参数  : sceneID   场景ID
               	
    输出参数  : N/A
    返回值    : 模板ID
    备注      : 
    **********************************************************************/
	 virtual  bool FindWay2VisionModel(const int iSceneID,int WinID,int index,char* szModePath,int PyramidLevel,int MatchNum, double& Score,
		                      int ProcIndex,bool IsBulid,double& row,double& col,double& phi);

	 virtual  bool FindModelRealTime(const int iSceneID,int WinID,int index,char* szModePath,int PyramidLevel,int MatchNum, double& Score,
		 int ProcIndex,bool IsBulid,double& row,double& col,double& phi);


	 void MoveAndRotInCamForOriMatch(int iSceneID,double PixSize,bool AxeMoveRotDirect,double& MotorRelX,double& MotorRelY,
		          int LastSceneId,int NextSceneId,double SceneRotPhi,double PicCentX,double PicCentY,double RotCenX,double RotCenY,double MotorRotPhi,
					double CalRotPhi,double (*pCamParam),int ParamNum,double (*pCamPose),double CalThickness,
					 int CamIndex,int AxeIndex,int ToolIndex);
	 	
	 bool AffinePointByCamInMecRot(int CamIndex,int AxisIndex,int ToolIndex,double& PointX,double& PointY);

	 void AffToRobotWorld(int SceneId,int CamIndex,int AxisIndex,int ToolIndex,double PixSize,double ModRotPhi,
		         double& ToolWorldX,double& ToolWorldY,bool WithCamParam,double CalThickness,double* pCamParam,int ParamNum,double* CamPos);

	 void AffinePointToWorld(int CamIndex,int AxisIndex,int ToolIndex,double PixSize,double OriCamX,double OriCamY,
		 double OriCamPhi,double CurCamX,double CurCamY,double CurCamPhi,double& PointX,double& PointY,int ResType,
		 bool MoveRotDirect,double CurMatchPhi,double AxeRotX,double AxeRotY);

	 void SetCamInMecRot(int CamIndex,int AxisIndex,int ToolIndex,double(*pMecAndCam),int MetixCount);
	 void SetCamRot(int CamIndex,int AxisIndex,int ToolIndex,double(*pCamRot),int MetixCount);
	  void SetImageMetrix(int CamIndex,int AxisIndex,int ToolIndex,double(*pImageRela),double(*pImageAbs),int MetixCount);

	/**********************************************************************
    函数      : addAffineData
    功能      : 创建仿射
    输入参数  : sceneID   场景ID
                aoint ，boint 创建仿射的两个不同平面上的点数组
				index 点索引
				CameraID 相机ID
    输出参数  : N/A
    返回值    : 仿射ID
    备注      : 
    **********************************************************************/
	virtual bool addAffineData(const int sceneID,SCENE_PKG *sceneName, AF_POINT apoint, AF_POINT bpoint, int index, bool bIsSave = false);

	/**********************************************************************
    函数      : affine_apply
    功能      : 仿射应用
    输入参数  : path  仿射模板路径
				point   进行仿射前的点
    输出参数  : N/A
    返回值    : 进行仿射后的点
    备注      : 
    **********************************************************************/
   virtual  int affine_apply(int sceneID, int iCameraID, char *sceneName, AF_POINT &apoint, AF_POINT &stOutPoint);

	/**********************************************************************
    函数      : clearWindow
    功能      : 清除窗口
    输入参数  : winID 窗口ID			
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
    virtual  bool  clearWindow(const  long  winID );
    
	/**********************************************************************
    函数      : clearWindow
    功能      : 调试阈值分割参数
    输入参数  : winID 窗口ID	
                iMinGray 起点阈值
                iMaxGray 终点阈值
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
    bool  DebugThreshold(long  halconID, int sceneID, int iMinGray, int iMaxGray);

	/**********************************************************************
    函数      : findline
    功能      : hdvp处理过程
    输入参数  : sceneID 场景ID	
				param 找线参数
				result 检测结果
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
	virtual bool  findline(int sceneID,SCENE_PKG *param, int index, ST_LINE &result, bool bFollow = false);

	/**********************************************************************
    函数      : findcircle
    功能      : hdvp处理过程
    输入参数  : sceneID 场景ID	
				param  找圆参数
				result 检测结果
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
	virtual bool  findcircle(int sceneID,SCENE_PKG *param, int index, ST_CIRCLE &result, bool bFollow = false);

	virtual bool MeasureCircle(long HwinID,int iSceneID, int Imageindex,int MatchIndex, double& Row,double& Col,double& Rii);



	/**********************************************************************
    函数      : process
    功能      : hdvp处理过程
    输入参数  : sceneID 场景ID	
				param 从UI获取的设置参数
				result 检测结果
    输出参数  : N/A
    返回值    : 成功或失败
    备注      : 
    **********************************************************************/
	virtual bool  process(int sceneID,SCENE_PKG *param, CHECK_RESULT &result);

    //获取对应类型参数在pkg->pBody中的位置
    //input 类型， 索引， pkg, 
    //返回找到位置
    virtual int FindParamData(SCENE_PKG *pkg, int iType, int index, int iGroup = -1);
    virtual int FindSpecTypePos(const VIRSION_DATA_PKG *pkg, unsigned int *pType, int iType);

	//绘制Roi

	//virtual void DrawRegionRect(int SenceId);


private:
    static map<int, CVisionProcess *>m_vecVisionInstance;
};

