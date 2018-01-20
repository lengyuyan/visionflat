#ifndef _VISION_PROCESS_H_
#define _VISION_PROCESS_H_
//#include "stdafx.h"
#include "DataType.h"
#include "HDevEngineCpp.h"
using namespace HDevEngineCpp;
#include <vector>
using namespace std;
#include "HalconCpp.h"

//using namespace Halcon;
using namespace HalconCpp;

#define CALIB_DATA_MAX_SIZE  10
#define CAMERA_NUM 8
#define MAX_TOOL_NUMBER 20  //设备的机械手数量

class Shape
{
public:

	HObject m_ShapeObj;
	ShapeType m_ShapeType;
	int m_ShapeId;

	Shape(void)
	{
		m_ShapeObj.Clear();
		m_ShapeType = HoNullShapeType;
		m_ShapeId = -1;
	}
	Shape(int shapeId)
	{
		m_ShapeObj.Clear();
		m_ShapeType = HoNullShapeType;
		m_ShapeId =shapeId;
	}



	~Shape(void)
	{

	}
	void operator=(const Shape& sourse)
	{
		m_ShapeObj = sourse.m_ShapeObj;
		m_ShapeType = sourse.m_ShapeType;
		//m_ShapeId = sourse.m_ShapeId;		
	}
};

class ComposeShape:public Shape
{
public:
	vector<int> m_VecComponentId;
	vector<int> m_VecCombType;
	ComposeShape()
	{
		m_ShapeType = HoComposeType;
	}
	ComposeShape(int shapeId):Shape(shapeId)
	{
		m_ShapeType = HoComposeType;
	}


	void operator=(const ComposeShape& sourse)
	{
		m_VecComponentId = sourse.m_VecComponentId ;
	}
};

class LineShape:public Shape
{
	//HoLineType
public:
	LineShape(void)
	{
		m_ShapeType = HoLineType ;
	}

	LineShape(int shapeId):Shape(shapeId)
	{
		m_ShapeType = HoRecTangleType ;
	}
	HTuple BeginRow;
	HTuple BeginCol;
	HTuple EndRow;
	HTuple EndCol;

};

class ShapeRect:public Shape
{
public:
	ShapeRect(void)
	{
		m_ShapeType = HoRecTangleType ;
	}

	ShapeRect(int shapeId):Shape(shapeId)
	{
		m_ShapeType = HoRecTangleType ;
	}
	HTuple CenRow;
	HTuple CenCol;
	HTuple pHi;
	HTuple Lenth1;
	HTuple Lenth2;
	void operator=(const ShapeRect& sourse)
	{
		CenRow = sourse.CenRow;
		CenCol = sourse.CenCol;
		pHi = sourse.pHi;
		Lenth1 = sourse.Lenth1;
		Lenth2 = sourse.Lenth2;
		m_ShapeObj =  sourse.m_ShapeObj;

	}

};

class ShapeCircle:public Shape
{
public:
	ShapeCircle(void)
	{
		m_ShapeType = HoCircleType ;
	}
	ShapeCircle(int shapeId):Shape(shapeId)
	{
		m_ShapeType = HoCircleType ;
	}
	HTuple CenRow;
	HTuple CenCol;
	HTuple CirRii;

};



class CVisionProcess
{
public:
    CVisionProcess();
    ~CVisionProcess(void);

	
public:
	//传入相机高宽，获取指向图像内存的指针
	char * GetImageBuf(int index, int iWidth , int Height);
    bool ReadImage(const char *pName, int index = 0, bool bNewPic = false);
    bool DispImage(int winID, int index = 0);
    bool DispObj(long winID, bool bIsShowTmp);
	static bool ShowWindString(long winID,HTuple StrContent,int topRow = -1,int topCol = -1);
	static bool SetDispColorHeight(long winID,HTuple strColor,int FontHeight=0);
	bool SaveImage(int index,char *path,string SNstr);
	bool saveImagePath(int index,char *path,string SNstr,unsigned int type = 0);//保存图片路径,type=0表示结果图片gpz 2016-8-16
	char* getImagePath(int index,unsigned int type = 0);//得到保存图片路径,type=0表示结果图片gpz 2016-8-16
	static bool SaveWinImage(long winID,char *path,char *outpath,string SNstr);
	static void* GetWinImage(long winID,double &width,double &heigh);
    bool addAffineData(const char *sceneName, AF_POINT apoint, AF_POINT bpoint, int index,int CameraID = 0, bool bSave = false);
	bool creat_affine(const char *sceneName, AF_POINT *aoint, AF_POINT *bpoint,int CameraID = 0);
    int  affine_apply(int iCameraID,char *sceneName, AF_POINT &apoint, AF_POINT &stOutPoint);
    int  readAffineData(int CameraID, char *sceneName);
    HTuple  GetAffineData();
    int  SetAffineData(HTuple HomMat2D);
    int  SetAngleOffset(double dAngle);

	//int buildModel(int iWinID,ST_RECTANGLE_DATA &modelroi,char *sceneName,char *path);
    int buildModel(int iWinID,VIRSION_DATA_PKG *pData, int iLength, unsigned int *pType,char *sceneName,char *path);
	bool buildModel(int iWinID,int RoiID,char* modePath, int Nevels,double AngleStart,double AngleExtent,
		int Contrast,int index = 0);
	bool  MatchModel(int winId,int index,char* szModePath,int PyramidLevel,int MatchNum,double& Score,
		                    int ProcIndex,bool IsBulid,double& row,double& col,double& phi); //使用double类型是准备后续兼容标定的匹配
	
	bool  FindModelRealTime(int winId,int index,char* szModePath,int PyramidLevel,int MatchNum,double& Score,
		int ProcIndex,bool IsBulid,double& row,double& col,double& phi); //使用double类型是准备后续兼容标定的匹配


	//匹配后与原位姿比较，计算出需要动作的机械坐标系下的数值
	void  MoveAndRotInCamForOriMatch(double PixSize,bool AxeMoveRotDirect,double& MotorRelX,double& MotorRelY,
		  int LastSceneId,int CurSceneId,int NextSceneId,double SceneRotPhi,double PicCentX,double PicCentY,double RotCenX,double RotCenY,double MotorRotPhi,double CalRotPhi,
		   double (*pCamParam),int ParamNum,double (*pCamPose),double CalThickness,
		    int CamIndex,int AxeIndex,int ToolIndex);

	static bool DobubleArrayEqueal(double * A,int arraynum);//新增一个比较数组函数,应该放在工具中,但不想依赖其他
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
    int readModel(char *sceneName,char *path, int index = 0, int iType = 0);
    int clearModel(int iModelID, int iType);
    int findModel(const char *pSceneName, VIRSION_DATA_PKG *pData, int iLength, unsigned int *pType, ST_RECTANGLE_DATA &point,unsigned int bDisplay = 1);
	bool  process(int sceneID,SCENE_PKG *param, CHECK_RESULT &result);
	bool OndrawRoi(long  halconID , const  ST_ROI_INFO  &roi, RoiCombType CombType ,PROCESS_TYPE bflag ,vector<int>* pRoiIDList , bool bcolor );
	//int  GetLastShapeId();
	void GenShape(const ST_ROI_INFO& roi,ShapeType shapeflag,int ShapeId,int Width,int Height);
	int  GetCurShapeId(void);
	bool  GetRoiParam(int ShapeId,ST_ROI_INFO* pRoi);
	void OnShowAllRoi(long  halconID,const vector<int>* const pRoiIdList); 
    bool  drawRoi(long  halconID , const  ST_ROI_INFO  &roi, ST_ROI_INFO &outroi ,bool bflag , bool bcolor );
	bool  dispRoi(long  halconID , ST_ROI_INFO  roi,bool bflag = true,bool bcolor = true);
	//新定义
	void ShowShap(const Shape& shapObj,long halconID);


	bool Imagebuffer(int cameraindex, int sceneID, long *pbuffer);
	bool getimagesize(int &width, int &height);
	bool getimagesize(int ImageIndex,int &width, int &height);
	static bool setpart(long winID,int topx, int topy,int lowx,int lowy);
    static bool getpart(long winID,int &topx, int &topy,int &lowx,int &lowy);

	static bool openWindow(long winID, int x, int y, int width, int hight, long &halconWinID);//gpz 把VisionServer中的halcon相关的放到VisionProcess中
	static bool closeWindow(long halconWinID);//gpz 把VisionServer中的halcon相关的放到VisionProcess中

	void gen_arrow_contour_xld (HObject *Arrow, HTuple Row1, HTuple Column1, HTuple Row2, HTuple Column2, HTuple HeadLength, HTuple HeadWidth);
    //调试阈值分割参数
    bool  DebugThreshold(long  halconID, int iMinGray, int iMaxGray);
	bool  findline(char *pSceneName, VIRSION_DATA_PKG *pData, int iLength, unsigned int *pType, ST_LINE &result, unsigned int IsDisplay = 0);
	bool  findcircle(char *pSceneName, VIRSION_DATA_PKG *pData, int iLength, unsigned int *pType, ST_CIRCLE &result, unsigned int IsDisplay = 0);
	bool  MeasureCircle(long iHalconWind,int ImageIndex,int MatchIndex,double& Row,double& Col,double& Rii); 

	bool HalconProcedureLocate(long iWinID,int Imageindex,string ProcedurePath,double (*inParam)[HalProcInParamNum], 
		double& PosX,double& PosY,double& PosPhi);

	bool HalconProcedureProcess(long iWinID,int Imageindex,string ProcedurePath,vector<double> &inparam,vector<double> &outparam,int display,int &error,string &hmessage);

	bool ImageEnhancement(long iWinID,int Imageindex,double& multinum,double& addnum,int display,int &error,string &hmessage);
    //获取对应类型参数在pkg->pBody中的位置
    //input 类型， 索引， pkg, 
    //返回找到位置
    static int FindParamData(SCENE_PKG *pkg, int iType, int index, int iGroup = -1);
    static int FindSpecTypePos(const VIRSION_DATA_PKG *pkg, unsigned int *pType, int iType);

	//计算各相机标定的矩阵
	static void CalCamMetrix(int CamerIndex,int DataCount,double MotorX[],double MotorY[],double CamRow[],double CamCol[],
		double PixSize,double(*pCamRot),double (*pMecAndCam),double(*pImageRela),double (*pImageAbs),
		double (*pCamParam),int ParamNum,double (*pCamPose),
		int ToolIndex=0,int AxeIndex =0,double CalFirstX =0.0,
		double CalFirstY=0.0,double CalFirstAngle=0.0,double ToolFirstX =0.0,double ToolFirstY = 0.0,
		 double CamRotAngle = 0.0,double RotToCamX = 0.0,double RotToCamY =0.0,double CalThickNess=0.0);


	static void AffineCoorPoint(double CoordX,double CoordY,double CoordAng,double& PointX,double& PointY);

	void AffToRobotWorld(int CamIndex,int AxisIndex,int ToolIndex,double PixSize,double ModRotPhi,
		       double& ToolWorldX,double& ToolWorldY,bool WithCamParam,double CalThickness,double* pCamParam,int ParamNum,double* CamPos);

    bool ProcProcess(SCENE_PKG *param, HTuple &OutputValue, HTuple &ErrorCode, double &dOutTime);
    bool ToolProcProcess(const char *pSceneName, HDevProcedure &hdvpProc,int index, HTuple InputValue,
        char *szProcName, HTuple &OutputValue, HTuple &ErrorCode, unsigned int bDisplay = 0,bool bIsUserObj = true);
	

	
	//获取message的相关信息
	void get_mesage_information (HTuple Message, HTuple *String,HTuple *Color);

	HObject m_pImage[MAXPICNUMEVESCENE];//....

public:
    vector<Shape*> m_ShapeList;

private:
	HObject m_hImage;
	bool    m_bIsInit[8];//图像对象是否初始化

    HObject m_vecObj;       //保存必须显示的结果对像
    HTuple  m_vecMsg;       //

	HTuple m_vecSize;

	HTuple width;
	HTuple height;

	HTuple  m_modelid;
    int     m_modelFindType;
    int     m_modelBuildType;
    int     m_modelReadType;
	char *buffer;
	//vector<Shape*> m_DrawList;
	
	//char m_testBuffer[9000000];  //临时定义，可以删除。

	char resultImgPath[_MAX_PATH];// 增加图片处理后保存路径 gpz 2016-8-16
	int m_index;

	int m_FirstCamIndex;  //本场景第一个相机的索引
    //处理过程对象
    HDevProcedure m_hdvProc;
    HDevProcedure m_hdvMainProc;
    HDevProcedure m_hdvFModelProc;
    HDevProcedure m_hdvFNccModelProc;
    HDevProcedure m_hdvRModelProc;
    HDevProcedure m_hdvRNccModelProc;
    HDevProcedure m_hdvBModelProc;
    HDevProcedure m_hdvBNccModelProc;
    HDevProcedure m_hdvBManualModelProc;
    HDevProcedure m_hdvAffineProc;
    HDevProcedure m_hdvCAffineProc;
    HDevProcedure m_hdvCircleProc;
    HDevProcedure m_hdvLineProc;
    HDevEngine    m_HEngine;
    HTuple        m_HomMat2D;    //仿射句柄
    HTuple        m_AngleOffset;    //角度偏差
    HTuple        m_Direction;    //方向
	HTuple        m_Dispix;    //每个像素的物理距离 gpz 2016-8-18

    AF_POINT      m_RealArray[CALIB_DATA_MAX_SIZE];
    AF_POINT      m_PixelArray[CALIB_DATA_MAX_SIZE];

    //运动坐标系点数据
    HTuple        m_MoveX;   //按照之前的设计，好像只能保存一个相机的标定数据，不如把所有相机的标定数据统一放在系统参数中。
    HTuple        m_MoveY;
    HTuple        m_PixelRow;
    HTuple        m_PixelCol;
    int           m_iValidSize;   //仿射点中最大有效索引

public:
    //相机标定的转换矩阵，标定的是相机坐标系和机械坐标系（或辅助机械坐标系）的关系
	//而每个机械手都应该建立一个机械坐标系（即夹具坐标系，或称产品坐标系），原点就是马达原点时的机械手工作点，所以是个二维矩阵。
	static HTuple  m_CamerasHomMat2D[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER]; //相机坐标绝对
	static HTuple  m_CamRotToMotorHomMat2D[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER];//相机坐标相对
	static HTuple  m_ImageToMotorRelative[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER]; //像素坐标相对
	static HTuple  m_ImageToMotorAbsolute[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER]; //像素坐标绝对

	//定义场景（多数时候表现位工位）之间位置变换的关系
	static HTuple m_ScenePosPass[30][30]; //暂时定义30个场景，不够再增加， 如果数量太多，需要重新设计。
	                                      //第一个表示前一个场景号，第二个表示当前计算的场景号。
	 

private:
    bool          m_bIsAppend;
    /*static*/ bool          m_bIsPass;
    static CRITICAL_SECTION *m_pSection;
    double        m_MotorAngle;   //世界坐标系与相素坐标系夹角

	int m_ShapeIdCount;
	HTuple m_HvMatchModID;

	//场景作业原始匹配值
	HTuple m_OrgionMatchRow;
	HTuple m_OrgionMatchCol;
	HTuple m_OrgionMatchPhi;
	//场景作业当前匹配值
	HTuple m_CurMatchRow;
	HTuple m_CurMatchCol;
	HTuple m_CurMatchPhi;
	int m_CurMatchTypeProcIndex;
	//场景作业的匹配变换列表,表示新的匹配位置相对于原始模板位置的变化矩阵,该场景中匹配的序号对应向量的下标。
	vector<HTuple> m_MatchHomm2DList; //单位为像素	
	 //vector<HTuple> m_RealMatchHomm2DList;//单位为实际机械单位,   暂时没使用


public:
	
	

};

#endif