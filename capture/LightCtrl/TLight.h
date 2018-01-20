#include <string>
#include <hash_map>
using namespace std;
#include "SerialPort.h"
#include "iLightCtrl.h"
#include "sysapi.h"

#define RETURN_NULLPTR(p,v) if (NULL == p) \
{	\
	return v; \
}


enum CTR_TYPE
{
	LIGHT_OFF = 0,
	LIGHT_OPEN = 1,	
	LIGHT_SET = 2,
};

typedef struct lightpara
{
	unsigned int channalVal[8];//一个光源控制器最多8个通道
	LIGHTCTL_TYPE lighttype;

    lightpara()
    {
        memset(channalVal, 0, sizeof(channalVal));
        lighttype = LIGHTCTL_TYPE_NDPS;
    }

    lightpara operator=(lightpara pa)
    {
        memcpy(channalVal, pa.channalVal, sizeof(channalVal));
        this->lighttype = pa.lighttype;
        return *this;
    }
}LightPara;


class  TLight
{
public:
    TLight();
    ~TLight();

    static TLight* getInstance();
    static void destroyInstance();
	static TLight* m_this;

	//初始化光源控制器,name:串口名列表
	virtual int Init(LightInfo seriaInfo[],int cout, LightTypeInfo *pType, int iTypeCount = 1) ; 

	//释放光源控制器资源
	virtual int DeInit(string comName);

	//打开指定频道光源
	virtual int Open(string comName,int index);

	//关闭指定频道光源
	virtual int Close(string comName,int index);

	//设置指定频道光源亮度
	virtual int SetLightValue(string comName,int index ,unsigned int value);

	//获取指定频道光源亮度值
	virtual int GetLightValue(string comName,int index ,unsigned int &val);

    //判断光源控制器连接是否成功
    virtual int JudgeLightConnect(string comName);
private:
	//命令加工处理
	int CommandProcess(int index ,CTR_TYPE ctrtype,unsigned int lvalue);

	//发关命令到指定串口，comName：串口名，comm：命令
	int SendCommd(string comName,string comm);

	//获取com对像
	int GetComer(string name);
	string ChangeCharstr2Hexstr(string Charstr);
	void HexStringFilter(string &str);
	char CombineHexChar(char CharH,char CharL);
	string DevideHexChar(char HexChar);

	LIGHTCTL_TYPE m_LightType;//光源类型
	hash_map<string,SerialPort *> *m_hs_NameToCom;//串口列表
	hash_map<string,SerialPort *>::iterator m_hs_NamToCom_it;
	hash_map<string,LightPara> *m_hs_NameToLightPa;//串口与光源参数关系表
	hash_map<string,LightPara>::iterator m_hs_NamToLightP_it;
	int m_i_result;			//函数的返回结果

	string m_command;       //控制命令

	SerialPort *m_serialp;
	SerialPortInfo *m_serialinfo;
	LightPara *m_lightp;

	void testmyself();

	HANDLE  m_Section;
};