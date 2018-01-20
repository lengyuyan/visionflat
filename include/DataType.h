#ifndef _DATA_DEF_H_
#define _DATA_DEF_H_

#include <vector>
#include <hash_map>
#include <map>
#include <string>
using namespace std;
#ifndef SCENE_SIZE
#define SCENE_SIZE 64
#endif
#ifndef MODEL_PARAM_SIZE
#define MODEL_PARAM_SIZE 12
#endif
#ifndef BUF_SIZE
#define BUF_SIZE 256
#endif

#ifndef TOOL_NUMBER
#define  TOOL_NUMBER 6  // product number
#endif

#ifndef AXIS_NUMBER
#define  AXIS_NUMBER 8  //  
#endif

#ifndef CAMCAL_NUMBER //相机标定点数量
#define  CAMCAL_NUMBER 12
#endif

#ifndef CAMERA_NUMBER
#define  CAMERA_NUMBER 8
#endif

#ifndef RESULT_NUMBER
#define  RESULT_NUMBER 100
#endif

#define LOGIN_TIME_OUT   5 * 60 * 1000   //5分钟无操作自动退出登陆

#ifndef CONFIG_VALUE_LENGTH
#define CONFIG_VALUE_LENGTH 64
#endif
#ifndef CONFIG_GENERAL_LENGTH
#define CONFIG_GENERAL_LENGTH 64
#endif

//定义一键切换产品的映射数据。
#define LPPMAPDATA hash_map<string,map<int,int>>*
#define PMAPDATA hash_map<string,map<int,int>>
#define SCENEMAP map<int,int>

#define  HalProcInParamNum 12   //halcon输入参数数目
#define  MAXPICNUMEVESCENE 8	//每个场景处理中最大的采集相关的数目

enum DATA_TYPE
{
    VISION_INT_TYPE = 0,    //int 类型 
    VISION_DOUBLE_TYPE,     //double 类型
};

struct ST_RECTANGLE_DATA
{
	double col;          //halcon里面col
	double row;          //halcon里面row
	double length1;
	double length2;
	double angle;
    ST_RECTANGLE_DATA ()
    {
        col = 1294.0;
        row = 970.0;
        angle = 0.0;
        length1 = 200.0;
        length2 = 160.0;
        //angle = 0.0;
    }
    ST_RECTANGLE_DATA operator=(ST_RECTANGLE_DATA so)
    {
        col = so.col;
        row = so.row;
        length1 = so.length1;
        length2 = so.length2;
        angle = so.angle;
        return *this;
    }
};

struct ST_CIRCLE_DATA
{
	double col;     //halcon里面col
	double row;     //halcon里面row
	double Radius;
	double width;
    double angleB;  //圆环时用
    double angleE;
    ST_CIRCLE_DATA()
    {
        col = 1294.0;
        row = 970.0;
        Radius = 200.0;
        width = 15.0;
        angleB = 0;
        angleE = 360;
    }
    ST_CIRCLE_DATA operator=(ST_CIRCLE_DATA so)
    {
        col = so.col;
        row = so.row;
        Radius = so.Radius;
        width = so.width;
        angleB = so.angleB;
        angleE = so.angleE;
        return *this;
    }
};

struct ST_COMB_DATA
{
	vector<int> m_ShapIdList;
	vector<int> m_CombTypeList;

};

struct ST_FIND_LINE
{
    //找线感兴趣恭区域—Rectangle
    ST_RECTANGLE_DATA strect;
    double smooth;    //平滑度
    int    threshold;  //找线边缘幅度
    int    ROIWidth;  //找一点的最大宽度
    int    iPosNum;     //测量点个数
    int    iFindType;   //找点方法， 0为all, 1为找第一个点， 2为找最后一个点
    int    iTransition;  //0为all, 1为positive,2为negative,一般我们只用到positive和negative
    int    iAlgorithm;  //拟合直线时所选取的算法(0为regression；1为huber；2为tukey；3为gauss；4为drop。)
    int    iDirection;  //找线方向,0为正，1为负
	ST_FIND_LINE()
    {
		smooth = 3.0;
		threshold = 30;
		ROIWidth = 5;
		iPosNum = 30;
		iFindType = 1;
		iTransition = 1;
		iAlgorithm = 2;
		iDirection = 0;
	}
};
struct ST_FIND_CIRCLE
{ 
    //找圆感兴趣恭区域—圆环
    ST_CIRCLE_DATA stcircle;
    double smooth;    //平滑度
    int    threshold;  //找圆边缘幅度
    int    ROIWidth;  //找一点的最大宽度
    int    iPosNum;     //测量点个数
    int    iTransition;  //0为all, 1为positive,2为negative,一般我们只用到positive和negative
    int    iFindType;   //找点方法， 0为all, 1为找第一个点， 2为找最后一个点
    int    iAlgorithm;  //拟合圆时所选取的算法(algebraic, ahuber, atukey, geometric, geohuber, geotukey )
    int    iDirection;  //找圆方向,0为正，1为负
	ST_FIND_CIRCLE()
	{
		smooth = 3.0;
		threshold = 30;
		ROIWidth = 5;
		iPosNum = 30;
		iFindType = 1;
		iTransition = 1;
		iAlgorithm = 0;
		iDirection = 0;
	}
};

struct ST_MODEL_INFO
{
    ST_RECTANGLE_DATA modelRectangle; //模板区域
    //查到到模板的位置
    double row;
    double col;
    double angle;      //查到到模板的角度
    int modelID;           //模板ID
    double modelScore;           //模板匹配分数（0~0.1）
    double findNum;
    double fNumLevels;
    double cNumLevels;
    double contrast;
    double angleStart;
    double angleExtent;
    int modelType;
    ST_MODEL_INFO ()
    {
        modelType = 0;
        modelID = -1;
        modelScore = 0.8;
        findNum = 1;
        fNumLevels = 0;
        cNumLevels = -1;
        contrast = 0;
        angleStart = -0.39;
        angleExtent = 0.79;
        row = 0.0;
        col = 0.0;
        angle = 0.0;
    }
};

struct ST_IMAGE_PRO_INFO
{
    int iCamareNo;       //相机编号
    int iLightNo;         //光源编号
    int modelCount;         //模板个数, < 0 则表示不需要查找模板
    int iBlob;           //是否做阈值分割, >0为阈值个数, < 0 为不做阈值处理
    int shutter;         //爆光时间
    ST_RECTANGLE_DATA roiRectangle;  //感兴趣恭区域—Rectangle
    int iLineCount;          //当前图像中找线个数
    int iCircleCount;          //当前图像中找圆个数
};
struct SCENE_CONFIG
{
    bool   bIsDisplay;                      //是否显示调试信息
    char   szSceneName[SCENE_SIZE];         //场景名
	unsigned int sceneID;             //场景ID
	unsigned long widID;               //UI窗口ID
    unsigned int stationID;            //用于工作时判断绑定哪个窗口
    unsigned int iCountImage;                  //抓多少张图后处理
    unsigned int bSave; //添加到系统参数设置界面                     //是否进行数据转换     
    vector< ST_IMAGE_PRO_INFO > vecImageProInfo;   //与抓图个数有关
    vector< ST_FIND_LINE >vecLineParam;                  //需要找线参数队列
    vector< ST_FIND_CIRCLE >vecCircleParam;                 //需要找圆参数队列
    vector< ST_MODEL_INFO> vecModelInfo;                    //模板队列
    int threshold[SCENE_SIZE];        //Blob用到阈值列表

    SCENE_CONFIG operator=(SCENE_CONFIG so)
    {
        bIsDisplay = so.bIsDisplay;
        bSave = so.bSave;
        strncpy_s(szSceneName, so.szSceneName, SCENE_SIZE - 1);
	    sceneID = so.sceneID;
	    widID = so.widID;               //UI窗口ID
        stationID = so.stationID;
        iCountImage = so.iCountImage;                  //抓多少张图后处理
        vecImageProInfo = so.vecImageProInfo;   //与抓图个数有关
        vecLineParam = so.vecLineParam;                  //需要找线参数队列
        vecCircleParam = so.vecCircleParam;                 //需要找圆参数队列
        vecModelInfo = so.vecModelInfo;
        memcpy(threshold, so.threshold, SCENE_SIZE * sizeof(int));
        return *this;
    }
    SCENE_CONFIG()
    {
        bIsDisplay = false;
        bSave = false;
        sceneID = 0;
        widID = 0;
        stationID = 0;
        iCountImage = 0;
    }
};
struct CHECK_RESULT
{
    int err;						//处理结果
    int index;						//场景号
    int iCount;						//有效结果个数
    char szValue[RESULT_NUMBER][BUF_SIZE/2];			//存放的结果值，最多32个值
    double time;					//调用过程计算时间
	double caturetime;				//采集时间
    bool  bIsDisp;					//用于判断是否显示对象	
	int SeneTpye;					//处理到类型，标定、视觉处理等,10为标定 11为处理
	int resultType;                 //定位结果类型,0标识绝对坐标，1标识相对坐标
	int testCount;                  //本场景测试次数
	int CameraIndex[CAMERA_NUMBER]; //相机索引
	string rawimg;					//原始图片
	string outimg;					//结果图片
	bool isGeneralProcess;			//通用处理，发送结果时，不止发送一个结果
    CHECK_RESULT()
    {	
		isGeneralProcess = 0;
		resultType = 0;				//默认绝对坐标
		testCount = 0;
		SeneTpye =-1;
        err   = 0;
        index = 0;
        iCount = 0;
        memset(szValue, 0, sizeof(char) * 32 * 128);
        time = 0.0;
		caturetime =0.0;
        bIsDisp = true;
		rawimg = "";
		outimg = "";
	    memset(CameraIndex,-1,sizeof(CameraIndex));
	    
    }
	void operator=(CHECK_RESULT so)
	{
		isGeneralProcess = so.isGeneralProcess;
		err = so.err;
		resultType = so.resultType;				
		testCount = so.testCount;
		SeneTpye = so.SeneTpye;
		index = so.index;
		iCount = so.iCount;
		time = so.time;
		caturetime =caturetime;
		bIsDisp = so.bIsDisp;
		rawimg = so.rawimg;
		outimg = so.outimg;
		for (int i = 0;i<CAMERA_NUMBER;i++)
		{
			CameraIndex[i] = so.CameraIndex[i];
		}
		for (int i= 0;i<RESULT_NUMBER;i++)
		{
			strcpy_s(szValue[i],so.szValue[i]);
		}
	}
};


struct ST_CIRCLE
{
	double y;  //y.x相当于halcon中的row，col
	double x;
	double radius;
	ST_CIRCLE()
	{
		x = 0.0;
		y = 0.0;
		radius = 0.0;
	}
};

struct ST_LINE
{
	double y_begin;  //y.x相当于halcon中的row，col
	double x_begin;
	double y_end;
	double x_end;
	double phi;
	ST_LINE()
	{
		y_begin = 0.0;
		x_begin = 0.0;
		y_end = 0.0;
		x_end = 0.0;
	}
};


//网络通讯协议的数据
typedef struct IpProctocolSetData
{
	char CalRevHead[64];
	char WorkRevHead[64];
	char CalSendHead[64];
	char WorkSendHead[64];
	char IPDivid[64];
	char IPEnd[64];

	bool EnableData[6];

	void MakeEndable()
	{
		EnableData[0] = strlen(CalRevHead)>0 ? true:false;
		EnableData[1] = strlen(WorkRevHead)>0 ? true:false;
		EnableData[2] = strlen(CalSendHead)>0 ? true:false;
		EnableData[3] = strlen(WorkSendHead)>0 ? true:false;
		EnableData[4] = strlen(IPDivid)>0 ? true:false;
		EnableData[5] = strlen(IPEnd)>0 ? true:false;
	}

	IpProctocolSetData()
	{
		memset(CalRevHead,0,sizeof(CalRevHead));
		memset(WorkRevHead,0,sizeof(WorkRevHead));
		memset(CalSendHead,0,sizeof(CalSendHead));
		memset(WorkSendHead,0,sizeof(WorkSendHead));
		memset(IPDivid,0,sizeof(IPDivid));
		memset(IPEnd,0,sizeof(IPEnd));
		memset(EnableData,0,sizeof(EnableData));


	}
	void operator=(IpProctocolSetData so)
	{
		memcpy(CalRevHead,so.CalRevHead,sizeof(CalRevHead));
		memcpy(WorkRevHead,so.WorkRevHead,sizeof(WorkRevHead));
		memcpy(CalSendHead,so.CalSendHead,sizeof(CalSendHead));
		memcpy(WorkSendHead,so.WorkSendHead,sizeof(WorkSendHead));
		memcpy(IPDivid,so.IPDivid,sizeof(IPDivid));
		memcpy(IPEnd,so.IPEnd,sizeof(IPEnd));
		memcpy(EnableData,so.EnableData,sizeof(EnableData));
		MakeEndable();
	}

}PROCTOCOLDATA,*LPPROCTOCOLDATA;


typedef struct CamCalData
{
	int PexSize;
	int RotX;


}CAMCALDATA,*LPCAMCALDATA;


typedef struct _SOCKET_PARAM
{
    //server
    int          nettype;
    int          SeverSocketType;
    int          protocol;
    string       strServerIPAdd;
    string       strClientIPAdd;
    unsigned int ServerPort;
    unsigned int ClientPort;

	//通信数据自定义协议
	string  CalRevHead;
	string	CalSendHead;
	string	WorkRevHead;
	string	WorkSendHead;
	string	DividLabel;
	string	EndLabel;


    _SOCKET_PARAM()
    {
        strServerIPAdd = "";
        strClientIPAdd = "";

		CalRevHead="";
		CalSendHead="";
		WorkRevHead="";
		WorkSendHead="";
		DividLabel="";
		EndLabel="";

        ServerPort = 0;
        ClientPort = 0;
        nettype = 0;
        SeverSocketType = 0;
        protocol = 0;
    }
	

    _SOCKET_PARAM &operator=(_SOCKET_PARAM copy)
    {
        strServerIPAdd = copy.strServerIPAdd;
        strClientIPAdd = copy.strClientIPAdd;

		CalRevHead=copy.CalRevHead;
		CalSendHead=copy.CalSendHead;
		WorkRevHead=copy.WorkRevHead;
		WorkSendHead=copy.WorkSendHead;
		DividLabel=copy.DividLabel;
		EndLabel=copy.EndLabel;

        ServerPort = copy.ServerPort;
        ClientPort = copy.ClientPort;
        nettype = copy.nettype;
        SeverSocketType = copy.SeverSocketType;
        protocol = copy.protocol;

        return *this;
    }

	void GetProctocolData(PROCTOCOLDATA& ProctoData)
	{
		//char szText[64]={0};
		sprintf_s(ProctoData.CalRevHead,"%s",CalRevHead.c_str());
		sprintf_s(ProctoData.WorkRevHead,"%s",WorkRevHead.c_str());
		sprintf_s(ProctoData.CalSendHead,"%s",CalSendHead.c_str());
		sprintf_s(ProctoData.WorkSendHead,"%s",WorkSendHead.c_str());
		sprintf_s(ProctoData.IPDivid,"%s",DividLabel.c_str());
		sprintf_s(ProctoData.IPEnd,"%s",EndLabel.c_str());
	}

}SOCKET_PARAM;




typedef struct _FLAT_LIGHT_INFO
{
    int iType;            //光源类型，如CST
    int iCount;           //光源通道总数
    int baudrate;
    int parity;
    int bytesize;
    int stopbits;
    string strComm;       //串口名称
}FLAT_LIGHT_INFO;

struct AF_POINT    //仿射点
{
	double x;
	double y;
    double angle;
	double dis_pix; //每个像素点代表的物理距离gpz
};



struct CALIB_DATA
{
    bool bIsEnd;     //是否结束当前标定并保存
    bool bIsInteractive;     //是否需要交互
    int  iSceneID;   //场景ID
    int  index;      //点索引
    int  iCount; 
    char chSN[64];     //SN信息
    double xCalib[16];   //x坐标
    double yCalib[16];   //y坐标
	double phiCalib[16]; //旋转坐标
    CALIB_DATA()
    {
        bIsEnd = false;
        bIsInteractive = false;
        iSceneID = 0;
        index = 0;
        iCount = 1;
        memset(chSN, 0, 64);
        memset(xCalib, 0, 16 * sizeof(double));
        memset(yCalib, 0, 16 * sizeof(double));
		memset(phiCalib, 0, 16 * sizeof(double));
    }
};

struct ST_ROI_INFO //roi信息
{
	int iType;                         //区分该ROI为圆或直线， 0 为直线， 1为圆,2为矩形...
	                                   // 在新方法中类型的意义有变动，要逐步过渡到新方法中来
	ST_RECTANGLE_DATA strect;
	ST_CIRCLE_DATA stcircle; 
	ST_COMB_DATA stCombine;

};


enum CAMARA_ERRORCODE
{
	CAMERA_SUCCESS = 0,
	CAMERA_OPEN_FAIL = 1,//打开相机失败
	CAMERA_NOFIND_CAMERA = 2,//找不到指定名字的相机
	IMAGE_NOINSTANCE = 3, //iIamgeCtrl 没有实列化
	DLL_NOEXIST = 4, //DLL不存在
	FIND_NOCAM = 5, //遍历相机失败
};

enum CAMARA_ADN_CONNECT_TYPE
{
	CAMARA_TYPE_AVT_1394 = 0,//avt 1394
	CAMARA_TYPE_BASLER_GIGE = 1,

};

enum CAMARA_PARA_ITEM
{
	IMAGEFORMAT = 0,
	PHYSPEED = 1,
	PACKETSIZE = 2,
};

enum LIGHTCTL_TYPE
{
	LIGHTCTL_TYPE_NDPS = 0,//ndps
	LIGHTCTL_TYPE_OPT = 1,//opt
	LIGHTCTL_TYPE_OPT_PV = 2,//opt PV

};

enum LIGHTCTL_ERRORCODE
{
	LIGHTCTL_SUCCESS = 0,
	LIGHTCTL_NOINSTANCE = 1,//光源接口没实例化
	LIGHTCTL_COMOPEN_ERROR = 2,//com 口打开失败
	LIGHTCTL_NOFINDTHE_COM = 3,//没找到指定的com口
	LIGHTCTL_SEND_FAIL = 4,//命令发送失败
};

typedef struct _lighttype
{
    string TypeName;	//串口名
	DWORD BaudRate; //波特率，指定通信设备的传输速率。这个成员可以是实际波特率值或者下面的常量值之一：
    DWORD fParity; // 指定奇偶校验使能。若此成员为1，允许奇偶校验检查 
    BYTE ByteSize; // 通信字节位数，4—8
    BYTE Parity; //指定奇偶校验方法。此成员可以有下列值：EVENPARITY(2) 偶校验     NOPARITY(0) 无校验 MARKPARITY(3) 标记校验   ODDPARITY(1) 奇校验
    BYTE StopBits; //指定停止位的位数。此成员可以有下列值：ONESTOPBIT 1位停止位   TWOSTOPBITS 2位停止位	ONE5STOPBITS   1.5位停止位
    DWORD ReadIntervalTimeout; //两字符之间最大的延时
    DWORD ReadTotalTimeoutMultiplier; //读取每字符间的超时
    DWORD ReadTotalTimeoutConstant; //一次读取串口数据的固定超时
    DWORD WriteTotalTimeoutMultiplier; //写入每字符间的超时
    DWORD WriteTotalTimeoutConstant; //一次写入串口数据的固定超时

	_lighttype()
	{
    	TypeName = "NDPS";
    	BaudRate =19200;
    	fParity = 0;
    	Parity = 0;
    	ByteSize = 8;
    	StopBits = 0;
    	ReadIntervalTimeout = MAXDWORD;
    	ReadTotalTimeoutMultiplier = 0;
    	ReadTotalTimeoutConstant = 0;
    	WriteTotalTimeoutMultiplier = 10;
    	WriteTotalTimeoutConstant = 1000;
	}
    
    _lighttype operator=(_lighttype &obj)
    {
    	TypeName = obj.TypeName;
       	BaudRate =obj.BaudRate;
    	fParity = obj.fParity;
       	Parity = obj.Parity;
       	ByteSize = obj.ByteSize;
       	StopBits = obj.StopBits;
       	ReadIntervalTimeout = obj.ReadIntervalTimeout;
       	ReadTotalTimeoutMultiplier = obj.ReadTotalTimeoutMultiplier;
       	ReadTotalTimeoutConstant = obj.ReadTotalTimeoutConstant;
       	WriteTotalTimeoutMultiplier = obj.WriteTotalTimeoutMultiplier;
       	WriteTotalTimeoutConstant = obj.WriteTotalTimeoutConstant;
        return *this;
    }
}LightTypeInfo;

typedef struct lightinfo
{	
	LIGHTCTL_TYPE strLightType;//光源类型
    int    Channel;     //通道个数
	string PortName;	//串口名
    LightTypeInfo strTypeInfo;  //串口信息
    int   DefaultValue;   //缺省亮度
	lightinfo()
	{
		strLightType = LIGHTCTL_TYPE_NDPS;
		PortName = "COM1";
        Channel = 4;
        DefaultValue = 200;
	}
    lightinfo operator=(lightinfo &so)
    {
        strLightType = so.strLightType;
        PortName = so.PortName;
        Channel = so.Channel;
        DefaultValue = so.DefaultValue;
        return *this;
    }

} LightInfo;
          
typedef struct _SYSTEM_PARAM
{     
    int  iAutoOL;         //0为远程IO控制光源，1为当前程序串口控制IO
    bool bLandType;        //登陆
    int  iIsInstall;
    int  iWindowNum;       //用于显示UI个数，小于等于有效场景个数

	int iCameraDllNum;     //要加载的相机dll数量gpz

    int  iSceneNum;        //有效场景个数
    int    iLightNum;         //光源个数, 最大支持16个光源
    int    iLightTypeNum;         //光源各类个数, 最大同时支持4种光源
    int    iLightChaNum;         //光源通道个数
    int    iCameraNum;        //相机个数
	int    iAxisNum;           //转轴实际个数
	int    iToolNum;         // 夹具个数
    //char   *pImageBuf[64];       //相机采图存储Buffer每个相机一段buffer
    char   ImageSavePath[256];        //图片保存路径
    LightInfo strLightInfo[16];       //光源信息
    LightTypeInfo strLightType[4];        //光源类型
    string pstrCamareGUID[CAMERA_NUMBER];   //相机GUID列表
    string pstrSceneName[SCENE_SIZE];    //有效场景别名列表
	string pstrCameraDll[CAMERA_NUMBER];    //要加载的相机dll名字 gpz
	void* MainImageWndVec;  //主图像窗口向量
	enum FixType
    {
	    FIXED = 0,
	    UNFIXED = 1,
	    
    };

	//相机标定
	int CalPointNum[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER];  ////相机标定点的实际数量
	double CalMotorX[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER][CAMCAL_NUMBER];    //相机标定时做X相对移动的数据
	double CalMotorY[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER][CAMCAL_NUMBER];    //相机标定时做Y相对移动的数据
	double CalMotorPhi[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER][CAMCAL_NUMBER];    //相机标定时做旋转角度相对转动的数据


	double CameraPixSize[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER];  //相机像素大小
	bool   CalWithCamParam[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER]; //九点标定使用相机参数true还是像素大小false。
	double CameraCalMachineX[CAMERA_NUMBER][16];// 相机标定的机械坐标
	double CameraCalMachineY[CAMERA_NUMBER][16];//
	double CameraCalPixCol[CAMERA_NUMBER][16] ;  // 相机标定时的像素坐标。
	double CameraCalPixRow[CAMERA_NUMBER][16] ;
	double CameraCalPixPhi[CAMERA_NUMBER][16];
	//转轴在产品（夹具）中的位置
	//double RotX[CAMERA_NUMBER][Tool_Num]; //理论上每个相机可以标定所有夹具的转轴。
	//double RotY[CAMERA_NUMBER][Tool_Num]; // 
	double RotX[AXIS_NUMBER][TOOL_NUMBER]; //理论上每个轴可以对应所有夹具。
	double RotY[AXIS_NUMBER][TOOL_NUMBER]; 
	double RotX_Adjust[AXIS_NUMBER][TOOL_NUMBER]; //对RotX值的修正
	double RotY_Adjust[AXIS_NUMBER][TOOL_NUMBER]; 

	int CamFixedType[CAMERA_NUMBER]; //相机的安装方式
	double  ToolCalOriX[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER];
	double  ToolCalOriY[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER];
	double  ToolCalOriPhi[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER];//Angle
	double  CamCalOriX[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER];
	double  CamCalOriY[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER];
	double  CamCalOriPhi[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER]; //Angle

	int PosMoveType[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER]; //运动控制定位的处理类型
	bool AxeRotDirect[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER];  //方向相同为false，相反为true，//无论相机固定还是移动，一般方向都是相反的，
	                                   //目前还没遇到方向一致的情况，但是当固定相机和轴在同一侧时（有点矛盾，但存在这种情况），方向一样。
	                                   //目前仅用在定位校正时判断校正角度的正负。
	double RotpointToCamX[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER];
	double RotpointToCamY[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER];

	//相机系在机械系中的(求相对坐标的)旋转矩阵,第一个值为数量6,后面6个数为矩阵
	double CamRotMetix[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER][7]; 
	//相机系在机械系中的（求绝对坐标的）矩阵
	double CamInMecRotMetix[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER][7];
	//图像系在机械系中的(求相对坐标的)矩阵
	double ImageRelative[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER][7];
	//图像系在机械系中的（求绝对坐标的）矩阵
	double ImageAbsolute[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER][7];

	//相机内参
	double CamParam[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER][12];
	//相机外参
	double CamPose[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER][7];
	//标定板厚度
	double CalThickNess[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER];

	double PicCenterPointX[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER];//图片中心点
	double PicCenterPointY[CAMERA_NUMBER][AXIS_NUMBER][TOOL_NUMBER];

	bool GetDivCamParam( double(*pCamParam),int CamIndex,int AxeIndex,int ToolIndex,int ArrLen )
	{
		if (8!=ArrLen||CamIndex<0||CamIndex>CAMERA_NUMBER||AxeIndex<0||AxeIndex>AXIS_NUMBER||ToolIndex<0||ToolIndex>TOOL_NUMBER)
		{
		    return false;
		}

	    for (int i = 0;i<8;i++)
	    {
	        pCamParam[i]= CamParam[CamIndex][AxeIndex][ToolIndex][i];
	    }
    
		return true;
	}

	bool GetPolCamParam( double(*pCamParam),int CamIndex,int AxeIndex,int ToolIndex,int ArrLen )
	{
		if (12!=ArrLen||CamIndex<0||CamIndex>CAMERA_NUMBER||AxeIndex<0||AxeIndex>AXIS_NUMBER||ToolIndex<0||ToolIndex>TOOL_NUMBER)
		{
		    return false;
		}

	    for (int i = 0;i<12;i++)
	    {
	        pCamParam[i]= CamParam[CamIndex][AxeIndex][ToolIndex][i];
	    }
    
		return true;
	}
	//Socket参数
    SOCKET_PARAM stSocketParam;
	//确定是否是测试
	bool  IsSimulate;
	//定义产品映射的数据
	PMAPDATA ProductMap;  //用户存放产品及其映射数据。
	string CurProductName; //切换产品时用于存放产品名称。
	string ProductById[20];
    _SYSTEM_PARAM()
    {
        iAutoOL = 0;
        bLandType = true;
        iIsInstall = 0;
        iWindowNum = 1;     
        iSceneNum = 1;      
        iLightNum = 1;    
        iLightTypeNum = 1;
        iLightChaNum = 4; 
        iCameraNum = CAMERA_NUMBER;  
		iToolNum  = TOOL_NUMBER;
		iAxisNum = 0;
		IsSimulate = false;
		memset(CameraPixSize,0,sizeof(CameraPixSize));
		memset(CalWithCamParam,0,sizeof(CalWithCamParam));
		memset(RotX,0,sizeof(RotX));
		memset(RotY,0,sizeof(RotY));
		memset(RotX_Adjust,0,sizeof(RotX_Adjust));
		memset(RotY_Adjust,0,sizeof(RotY_Adjust));
		memset(CameraCalPixPhi,0,sizeof(CameraCalPixPhi));
		memset(CalPointNum,0,sizeof(CalPointNum));
		memset(CalMotorX,0,sizeof(CalMotorX));
		memset(CalMotorY,0,sizeof(CalMotorY));
		memset(CalMotorPhi,0,sizeof(CalMotorPhi));

		memset(CamFixedType,0,sizeof(CamFixedType));
		memset(ToolCalOriX,0,sizeof(ToolCalOriX));
		memset( ToolCalOriY,0,sizeof( ToolCalOriY));//
		memset( ToolCalOriPhi,0,sizeof( ToolCalOriPhi));//ToolCalOriPhi
		memset(CamCalOriX,0,sizeof( CamCalOriX ));
		memset(CamCalOriY,0,sizeof( CamCalOriY));
		memset(CamCalOriPhi,0,sizeof( CamCalOriPhi)); 
		memset(PosMoveType,0,sizeof( PosMoveType)); //PosMoveType
		memset(AxeRotDirect,0,sizeof( AxeRotDirect)); //AxeRotDirect
		memset(RotpointToCamX,0,sizeof( RotpointToCamX)); 
        memset(RotpointToCamY,0,sizeof( RotpointToCamY)); 
		//CamRotMetix
		memset(CamRotMetix,0,sizeof(CamRotMetix));
		memset(CamInMecRotMetix,0,sizeof(CamInMecRotMetix));
		memset(ImageRelative,0,sizeof(ImageRelative));
		memset(ImageAbsolute,0,sizeof(ImageAbsolute));
		//ImageRelative,ImageAbsolute;
		memset(CamParam,0,sizeof(CamParam));
		memset(CamPose,0,sizeof(CamPose));
		memset(CalThickNess,0,sizeof(CalThickNess));
	    memset(ProductById,0,sizeof(ProductById));
	    CurProductName.empty();

		memset(PicCenterPointX,0,sizeof( PicCenterPointX ));
		memset(PicCenterPointY,0,sizeof( PicCenterPointY ));
		
    }
    void SystemParamCopy(_SYSTEM_PARAM *so)
    {
        if (NULL == so)
        {
            return;
        }
		MainImageWndVec = so->MainImageWndVec;
        iAutoOL = so->iAutoOL;  
        bLandType = so->bLandType;
        iIsInstall = so->iIsInstall;
        iWindowNum = so->iWindowNum;
        if (0 < so->iSceneNum && so->iSceneNum <= 64)
        {
            iSceneNum = so->iSceneNum;
        }
        else
        {
            iSceneNum = 1;
        }

        if (0 < so->iLightNum && so->iLightNum <= 16)
        {
            iLightNum = so->iLightNum;
        }
        else
        {
            iLightNum = 1;
        }
        iLightNum = so->iLightNum;
        if (0 < so->iLightTypeNum && so->iLightTypeNum <= 4)
        {
            iLightTypeNum = so->iLightTypeNum;
        }
        else
        {
            iLightTypeNum = 1;
        }
        iLightChaNum = so->iLightChaNum;  
        if (0 < so->iCameraNum && so->iCameraNum <= 64)
        {
            iCameraNum = so->iCameraNum;
        }
        else
        {
            //iCameraNum = 1;
        }

        int i = 0;
        for (int i = 0; i < iLightNum; i++)
        {
            strLightInfo[i] = so->strLightInfo[i];
        }
		if (0 < so->iCameraNum && so->iCameraNum <= 64)//gpz
		{
			iCameraDllNum = so->iCameraDllNum;
		}
		else
		{
			iCameraDllNum = 1;
		}
		for (i = 0; i < iCameraDllNum; i++)
		{
			pstrCameraDll[i] = so->pstrCameraDll[i];
		}
        for (i = 0; i < iSceneNum; i++)
        {
            pstrSceneName[i] = so->pstrSceneName[i];
        }
        for (i = 0; i < iCameraNum; i++)
        {
            pstrCamareGUID[i] = so->pstrCamareGUID[i];
        }

		iAxisNum = so->iAxisNum;
		IsSimulate = so->IsSimulate;
		//相机标定参数
		memcpy(CameraPixSize,so->CameraPixSize,sizeof(CameraPixSize));
		memcpy(CalWithCamParam,so->CalWithCamParam,sizeof(CalWithCamParam));

		memcpy(CameraCalMachineX,so->CameraCalMachineX,sizeof(CameraCalMachineX));
		memcpy(CameraCalMachineY,so->CameraCalMachineY,sizeof(CameraCalMachineY));
		memcpy(CameraCalPixCol,so->CameraCalPixCol,sizeof(CameraCalPixCol));
		memcpy(CameraCalPixRow,so->CameraCalPixRow,sizeof(CameraCalPixRow));
		memcpy(CameraCalPixPhi,so->CameraCalPixPhi,sizeof(CameraCalPixPhi));

		memcpy(CalPointNum,so->CalPointNum,sizeof(CalPointNum));
		memcpy(CalMotorX,so->CalMotorX,sizeof(CalMotorX));
		memcpy(CalMotorY,so->CalMotorY,sizeof(CalMotorY));
		memcpy(CalMotorPhi,so->CalMotorPhi,sizeof(CalMotorPhi));
		memcpy(CamFixedType,so->CamFixedType,sizeof(CamFixedType));
		memcpy(PicCenterPointX,so->PicCenterPointX,sizeof(PicCenterPointX));
		memcpy(PicCenterPointY,so->PicCenterPointY,sizeof(PicCenterPointY));
		memcpy(ToolCalOriX,so->ToolCalOriX,sizeof(ToolCalOriX));
		memcpy(ToolCalOriY,so->ToolCalOriY,sizeof(ToolCalOriY));
		memcpy(ToolCalOriPhi,so->ToolCalOriPhi,sizeof(ToolCalOriPhi));
		//ToolCalOriPhi
		memcpy(CamCalOriX,so->CamCalOriX,sizeof(CamCalOriX));
		memcpy(CamCalOriY,so->CamCalOriY,sizeof(CamCalOriY));
		memcpy(CamCalOriPhi,so->CamCalOriPhi,sizeof(CamCalOriPhi));
		memcpy(PosMoveType,so->PosMoveType,sizeof(PosMoveType));//PosMoveType
		memcpy(AxeRotDirect,so->AxeRotDirect,sizeof(AxeRotDirect));
		memcpy(RotpointToCamX,so->RotpointToCamX,sizeof(RotpointToCamX));
		memcpy(RotpointToCamY,so->RotpointToCamY,sizeof(RotpointToCamY));
		memcpy(CamParam,so->CamParam,sizeof(CamParam));
		memcpy(CamPose,so->CamPose,sizeof(CamPose));
		memcpy(CalThickNess,so->CalThickNess,sizeof(CalThickNess));

		//CamRotMetix
		memcpy(CamRotMetix,so->CamRotMetix,sizeof(CamRotMetix));
		memcpy(CamInMecRotMetix,so->CamInMecRotMetix,sizeof(CamInMecRotMetix));
		memcpy(ImageRelative,so->ImageRelative,sizeof(ImageRelative));
		memcpy(ImageAbsolute,so->ImageAbsolute,sizeof(ImageAbsolute));
		
		memcpy(RotX,so->RotX,sizeof(RotX));
		memcpy(RotY,so->RotY,sizeof(RotY));

		memcpy(RotX_Adjust,so->RotX_Adjust,sizeof(RotX_Adjust));
		memcpy(RotY_Adjust,so->RotY_Adjust,sizeof(RotY_Adjust));

		memcpy(ProductById,so->ProductById,sizeof(ProductById));
		ProductMap = so->ProductMap;
		CurProductName =  so->CurProductName;

		iToolNum = so->iToolNum;
        for (i = 0; i < iLightTypeNum; i++)
        {
            strLightType[i] = so->strLightType[i];
        }
        stSocketParam = so->stSocketParam;
		memcpy(ImageSavePath,so->ImageSavePath,sizeof(ImageSavePath));
        //strcpy_s(ImageSavePath, 256, so->ImageSavePath);
    }
}SYSTEM_PARAM;

enum EN_TRANSITION_TYPE
{
    TRANSITION_TYPE_ALL = 0,
    TRANSITION_TYPE_POSITIVE,
    TRANSITION_TYPE_NEGATIVE,
};
enum EN_FIND_TYPE
{
    FINDTYPE_ALL = 0,
    FINDTYPE_FIRST,
    FINDTYPE_LAST,
};
enum EN_ALGORITHM_TYPE
{
    ALGORITHM_TYPE_REGRESSION = 0,
    ALGORITHM_TYPE_HUBER,
    ALGORITHM_TYPE_TUKEY,
    ALGORITHM_TYPE_GAUSS,
    ALGORITHM_TYPE_DROP,
};
enum EN_ALGORITHM_TYPE_CIRCLE
{
	ALGORITHM_TYPE_ALGEBRAIC = 0,
	ALGORITHM_TYPE_AHUBER,
	ALGORITHM_TYPE_ATUKEY,
	ALGORITHM_TYPE_GEOMETRIC,
	ALGORITHM_TYPE_GEOHUBER,
	ALGORITHM_TYPE_GEOTUKEY,
};
enum EN_DIRECTION_TYPE
{
    DIRECTION_TYPE_POSITIVE = 0,
    DIRECTION_TYPE_NEGATIVE,
};

typedef union _VIRSION_DATA_PKG_
{
    double dValue; // 指定union大小，VC编译器
    int    iValue;

   _VIRSION_DATA_PKG_()
   {
     dValue = 0.0;
     iValue = 0;
   }

}VIRSION_DATA_PKG;

typedef struct _SCENE_DEVICE_INFO
{
    unsigned int CameraIndex;      //相机索引
    unsigned int LightIndex;       //光源通道索引
    unsigned int CameraShutter;    //相机爆光
    unsigned int CameraGain;    //相机增益

    _SCENE_DEVICE_INFO()
    {
        CameraIndex = 0;
        LightIndex = 0;
        CameraShutter = 20;
        CameraGain = 8;
    }
}SCENE_DEVICE_INFO;

typedef struct _INTERFACE_SET_DATA
{
	int nIndex;			 //已有控件个数
	int CurSceneID;		 //场景ID
	int iT;				 //列表编号 编号 * 100
	string BtnType;      //按钮类型
	string BtnName;		 //按钮名字
	int ItemType;		 //Item类型
	int ItemNumb;		 //Item编号
	double InitValue;    //初始值
	double ChangeRate;   //变化率
	int OptionNumb;		 //选择项个数
	int height;		 //控件高
	char OptionName[8][16];	//选中名字

	_INTERFACE_SET_DATA()
	{
		CurSceneID = 0;
		nIndex = 0;
		iT = 0;
		BtnType = "";
		BtnName = "";
		ItemType = 0;
		ItemNumb = 0;
		InitValue = 0.0;
		ChangeRate = 0.0;
		OptionNumb = 0;
		height = 0;
		memset(OptionName, 0, 8 * 16);
    }
    void clear()
    {
        CurSceneID = 0;
        nIndex = 0;
        iT = 0;
        BtnType = "";
        BtnName = "";
        ItemType = 0;
        ItemNumb = 0;
        InitValue = 0.0;
        ChangeRate = 0.0;
        OptionNumb = 0;
        height = 0;
        memset(OptionName, 0, 8 * 16);
    }

	_INTERFACE_SET_DATA operator=(_INTERFACE_SET_DATA so)
	{
		CurSceneID = so.CurSceneID;
		nIndex = so.nIndex;
		iT = so.iT;
		BtnType = so.BtnType;
		BtnName = so.BtnName;
		ItemType = so.ItemType;
		ItemNumb = so.ItemNumb;
		InitValue = so.InitValue;
		ChangeRate = so.ChangeRate;
		OptionNumb = so.OptionNumb;
		height = so.height;
		memcpy(OptionName, so.OptionName, 8 * 16 );
		return *this;
	}
}INTERFACE_SET_DATA;

//包体中由小包体组成， 小包体前两个内容为 类型及大小之后才是小包数据
struct SCENE_PKG
{
    bool   bSave; //添加到系统参数设置界面                     //是否进行数据转换  
    bool   bIsSimulate;     //是否使用图片模拟
    char   szSceneName[SCENE_SIZE];         //场景名
    int  display;                       //是否显示调试信息
    unsigned int  pkgBodySize;          //整个包体的总大小
    unsigned int  pkgCount;             //包个数
    unsigned int  imageCount;           //处理图像个数
    unsigned int  ModelCount;           //处理图像个数
    unsigned int  sceneID;             //场景ID
    unsigned long halconID;               //UI窗口ID
    unsigned int  stationID;            //用于工作时判断绑定哪个窗口
	int ProcProcess;    //场景处理的方式：0为C++封装的固定方式；1为用halcon函数调用的实时方式，绝大多数采用固定的方式，这是定义的趋势
    void *pReserve;                      //预留
    vector<SCENE_DEVICE_INFO>  vecInfo;          //固有信息， 获取图像时相关设备参数， 不需要传给处理过程处理
    vector<unsigned int *>     vecDataType;      //包体, 传入图像处理中的数据类型  0为int， 1为double， ...
    vector<VIRSION_DATA_PKG *> vecBody;          //包体, 传入处理过程中的数据内容
                                      //包体中 每个小包体第一个值为 类型 （分组 * 10000 + 算法类型 * 100 + 索引）
                                      //第二个值为该小包中的数据个数
	vector<INTERFACE_SET_DATA *> InterfaceSetData;
	unsigned int InterfaceDataNumb;		//界面数据个数
	int MotorToolIndex;   //该场景对应的夹具（机械手），主要是用于标定场景
	int CalPointNum;      //该场景所使用的标定的点数，最后两点用于旋转角度标定
	int CalPointIndex;    //记忆已经标定的点数量，当完成所有点标定后清零.
	int SceneType;   //业务类型
	int AxisIndex;
	double defcamwidth;//场景中相机的默认大小
	double defcamheight;
	int testcount;  //场景测试次数

    SCENE_PKG()
    {
        display = false;
        bIsSimulate = false;
        bSave = false;
        memset(szSceneName, 0, SCENE_SIZE);
        pkgBodySize = 0;
        pkgCount = 1;
        imageCount = 1;
        ModelCount = 0;
        sceneID = 0;
		ProcProcess = 0;
        halconID = 0;
        stationID = 0;
        //pReserve = NULL;
		InterfaceDataNumb = 0;
        vecInfo.reserve(1);
        vecDataType.reserve(0);
        vecBody.reserve(0);
		MotorToolIndex = 0;
		CalPointNum = 6;
	    CalPointIndex = 0;
		SceneType = -1;
		defcamwidth = 2552;
		defcamheight =1944;
		AxisIndex = -1;
		testcount = 0;

    }

    SCENE_PKG operator=(SCENE_PKG so)
    {
		ProcProcess = so.ProcProcess;
        display = so.display;
        bSave = so.bSave;
        bIsSimulate = so.bIsSimulate;
        memcpy(szSceneName, so.szSceneName, SCENE_SIZE);
        pkgBodySize = so.pkgBodySize;
        pkgCount = so.pkgCount;
        imageCount = so.imageCount;
        ModelCount = so.ModelCount;
        sceneID = so.sceneID;
        halconID = so.halconID;
        stationID = so.stationID;
       // pReserve = so.pReserve;
        vecInfo = so.vecInfo;
        vecDataType = so.vecDataType;
        vecBody = so.vecBody;
		InterfaceSetData = so.InterfaceSetData;
		InterfaceDataNumb = so.InterfaceDataNumb;
		MotorToolIndex =  so.MotorToolIndex;
		CalPointNum = so.CalPointNum;
		SceneType= so.SceneType;
		defcamwidth = so.defcamwidth;
		defcamheight =so.defcamheight;
		AxisIndex = so.AxisIndex;
		testcount = so.testcount;
        return *this;
    }
    SCENE_PKG *SceneCopy(SCENE_PKG *so)
    {
		ProcProcess = so->ProcProcess;
        display = so->display;
        bSave = so->bSave;
        bIsSimulate = so->bIsSimulate;
        memcpy(szSceneName, so->szSceneName, SCENE_SIZE);
        pkgBodySize = so->pkgBodySize;
        pkgCount = so->pkgCount;
        imageCount = so->imageCount;
        ModelCount = so->ModelCount;
        sceneID = so->sceneID;
        halconID = so->halconID;
        stationID = so->stationID;
        vecInfo = so->vecInfo;
        vecDataType = so->vecDataType;
        vecBody = so->vecBody;
        InterfaceSetData = so->InterfaceSetData;
        InterfaceDataNumb = so->InterfaceDataNumb;
		MotorToolIndex =  so->MotorToolIndex;
		CalPointNum = so->CalPointNum;
		SceneType= so->SceneType;
		defcamwidth = so->defcamwidth;
		defcamheight =so->defcamheight;
		AxisIndex = so->AxisIndex;
		testcount = so->testcount;
        return this;
    }
};

#define VIRSION_DATA_PKG_HEADER_SIZE      2    //小包包体头大小



typedef enum _PROCESS_TYPE
{

	MAIN_PRO = 0,          //处理区域

	MODEL_PRO,              //模板(区域)匹配
	THRE_PRO,              //阈值
	LINE_PRO,               //找线区域
	CIRCLE_PRO,            //找圆区域
	MORE_PRO,				//更多选项

	//新加功能 
	IMAGE_READ,             //图像读取
	IMAGE_ACQISITION,      //图像采集 =7
	IMAGE_PREPROCESS,      //图像预处理 gray*a+b
	CONTOUR_POSITION,       //轮廓定位
	COMBSHAPE_PRO,           //绘制组合区域
	LOGIC_CTR,             //逻辑控制

	POSITION_EXCUTE,     // 定位处理 
	HALPROC_POSITIONG,    //HalProc定位
	HALPROC_PROCESS,    //HalProc通用处理




}PROCESS_TYPE;


//定义绘制的图形类型
enum ShapeType
{
	HoNullShapeType = 0,//无类型
	HoLineType = 1,//线类型
	HoRecTangleType = 2,//矩形
	HoCircleType = 3,//圆
	HoComposeType = 4//组合类型

};


//定义绘制组合区域的类型
enum RoiCombType
{
	NullCombine =0,
	RoiUnionType = 1,
	RoiSubType = 2,
	RoiInsectionType = 3,
};



typedef enum _ITEM_TYPE
{
    NORMARL_TYPE = 0,
    ID_TYPE,
    ROI_TYPE,
    THRE_TYPE,
    SPEC_TYPE,
}ITEM_TYPE;

typedef enum _ITEM_VALUE
{
    INT_TYPE = 0,
    DOUBLE_TYPE,
}ITEM_VALUE_TYPE;

typedef enum _MODEL_TYPE
{
    NORMAL_MODEL = 0,
    NCC_MODEL,
    MANUAL_MODEL,   //手绘模板轮廓
};


//  
//extern  CHECK_RESULT g_SceneProcResult;





#endif 
