// TLight.cpp : 定义 DLL 应用程序的导出函数。
//
#include "stdafx.h"
#include "TLight.h"

TLight* TLight::m_this = NULL;
TLight::TLight()
{
	m_hs_NameToCom = new hash_map<string,SerialPort *>;
	m_hs_NameToLightPa = new hash_map<string,LightPara>;
	m_Section = sys_InitializeCriticalSection();
}

TLight::~TLight()
{
    if (NULL != m_hs_NameToCom)
    {
        hash_map<string,SerialPort *>::iterator it;
        for(it = m_hs_NameToCom->begin(); it != m_hs_NameToCom->end();)
        {
            if (NULL != it->second)
            {
                it->second->Close();
                delete it->second;
                it->second = NULL;
            }
            m_hs_NameToCom->erase(it);
            it = m_hs_NameToCom->begin();
        }
        delete m_hs_NameToCom;
        m_hs_NameToCom = NULL;
    }
    if (NULL != m_hs_NameToLightPa)
    {
        hash_map<string,LightPara>::iterator itL;
        for(itL = m_hs_NameToLightPa->begin(); itL != m_hs_NameToLightPa->end();)
        {
            m_hs_NameToLightPa->erase(itL);
            itL = m_hs_NameToLightPa->begin();
        }
        delete m_hs_NameToLightPa;
        m_hs_NameToLightPa = NULL;
    }
	sys_DeleteCriticalSection(m_Section);
}

TLight* TLight::getInstance()
{
	if (NULL == m_this)
	{
		m_this = new TLight;
	}
	 return m_this;
}

void TLight::destroyInstance()
{
    if (NULL != m_this)
    {
        delete m_this;
        m_this = NULL;
    }
}

void TLight::testmyself()
{
	SerialPort sep;
	SerialPortInfo sei;
	//sei.PortName = "COM2";
	sep.Init(sei);
	sep.Open();
	sep.WriteData("$3103217");
	int i=0;
}

//初始化光源控制器
int TLight::Init(LightInfo lightInfo[],int cout, LightTypeInfo *pType, int iTypeCount)
{	//testmyself();
    if (NULL == pType)
    {
        return LIGHTCTL_NOFINDTHE_COM;
    }
	for (int i = 0;i < cout;i++)
	{		
		m_serialinfo = new SerialPortInfo();
		m_serialp = new SerialPort();
		//m_lightp = new LightPara();
        LightPara light;
        LightTypeInfo TypeInfo;
        if (lightInfo[i].strLightType < iTypeCount)
        {
            TypeInfo = pType[lightInfo[i].strLightType];
        }
        m_serialinfo->PortName = lightInfo[i].PortName;
		m_serialinfo->BaudRate = TypeInfo.BaudRate;
		m_serialinfo->ByteSize = TypeInfo.ByteSize;
		m_serialinfo->fParity = TypeInfo.fParity;
		m_serialinfo->Parity = TypeInfo.Parity;
		m_serialinfo->StopBits = TypeInfo.StopBits;
		m_serialinfo->ReadIntervalTimeout = TypeInfo.ReadIntervalTimeout;
		m_serialinfo->ReadTotalTimeoutConstant = TypeInfo.ReadTotalTimeoutConstant;
		m_serialinfo->ReadTotalTimeoutMultiplier = TypeInfo.ReadTotalTimeoutMultiplier;
		m_serialinfo->WriteTotalTimeoutMultiplier = TypeInfo.WriteTotalTimeoutMultiplier;
		m_serialinfo->WriteTotalTimeoutConstant = TypeInfo.WriteTotalTimeoutConstant;
		m_serialp->Init(*m_serialinfo);
        delete m_serialinfo;
        m_serialinfo = NULL;
		if (ERROR_CLOSECOM == m_serialp->Open())
		{
			return LIGHTCTL_COMOPEN_ERROR;
		}
		//m_pair_ntocom = new pair<string,SerialPort*>(lightInfo[i].PortName,m_serialp);
		//m_hs_NameToCom->insert(m_pair_ntocom);
        m_hs_NameToCom->insert(make_pair(lightInfo[i].PortName,m_serialp));
		
        light.lighttype = lightInfo[i].strLightType;// LIGHTCTL_TYPE_OPT_PV;//
        //m_lightp->lighttype = lightInfo->LightType;
        m_hs_NameToLightPa->insert(make_pair(lightInfo[i].PortName,light));
		//m_pair_ntolp = new pair<string,LightPara*>(lightInfo[i].PortName,m_lightp);
		//m_hs_NameToLightPa->insert(m_pair_ntolp);
	}

	 hash_map<string,int> testmap;
	 testmap.insert( make_pair("",1));
	
	return 0;
}


//释放光源控制器资源
int TLight::DeInit(string comName)
{
	m_i_result = GetComer(comName);
	if (LIGHTCTL_SUCCESS == m_i_result)
	{
		m_i_result = m_hs_NamToCom_it->second->Close();
        delete m_hs_NamToCom_it->second;
        m_hs_NamToCom_it->second = NULL;
        m_hs_NameToCom->erase(m_hs_NamToCom_it);
	}	
	return m_i_result;
}

//打开光源控制器
int TLight::Open(string comName,int index)
{
	m_i_result = GetComer(comName);
	if (LIGHTCTL_SUCCESS == m_i_result)
	{
		sys_EnterCriticalSection(m_Section);
		CommandProcess(index,LIGHT_OPEN,0);
		m_i_result = SendCommd(comName,m_command);
		sys_LeaveCriticalSection(m_Section);
	}	
	return m_i_result;
}

//关闭光源控制器
int TLight::Close(string comName,int index)
{
	m_i_result = GetComer(comName);
	if (LIGHTCTL_SUCCESS == m_i_result)
	{
		sys_EnterCriticalSection(m_Section);
		CommandProcess(index,LIGHT_OFF,0);
		m_i_result = SendCommd(comName,m_command);
		sys_LeaveCriticalSection(m_Section);
	}	
	return m_i_result;
}

//设置光源亮度
int TLight::SetLightValue(string comName,int index ,unsigned int value)
{
	m_i_result = GetComer(comName);
	if (LIGHTCTL_SUCCESS == m_i_result)
	{
		sys_EnterCriticalSection(m_Section);
		CommandProcess(index,LIGHT_SET,value);
		m_i_result = SendCommd(comName,m_command);
		sys_LeaveCriticalSection(m_Section);
	}	
	return m_i_result;
}

//获取光源亮度值
int TLight::GetLightValue(string comName,int index,unsigned int &val)
{
	m_i_result = GetComer(comName);
	if (LIGHTCTL_SUCCESS == m_i_result)
	{
		m_hs_NamToLightP_it = m_hs_NameToLightPa->find(m_hs_NamToCom_it->first);

		if(LIGHTCTL_TYPE_NDPS == m_LightType)
		{
			val = m_hs_NamToLightP_it->second.channalVal[index] & 0x00FF;
		}
		else if (LIGHTCTL_TYPE_OPT == m_LightType)
		{
			val = m_hs_NamToLightP_it->second.channalVal[index];
		}
		else if (LIGHTCTL_TYPE_OPT_PV == m_LightType)
		{
			val = m_hs_NamToLightP_it->second.channalVal[index];
		}
		
	}	
	return m_i_result;
}


//命令加工处理
int TLight::CommandProcess(int index ,CTR_TYPE ctrtype,unsigned int lvalue)
{
	if (lvalue>255)
	{
		lvalue = 255;
	}

	if (lvalue < 0)
	{
		lvalue = 0;
	}

	m_hs_NamToLightP_it = m_hs_NameToLightPa->find(m_hs_NamToCom_it->first);//该串口名字下的8个通道的光源参数
	m_LightType = m_hs_NamToLightP_it->second.lighttype;
		
	if (LIGHTCTL_TYPE_NDPS == m_LightType)
	{
		//***************************************************************//
		//* 1、 每通道亮度值命令必须为三位十进制数字；
		//*	2、 对于四通道控制器，必须发送八个通道命令，最后一通道为保留通道。
		//*	例：S100T128T025F000T123F110F225T080FC#
		//*	*************************************************************//

		if (LIGHT_SET != ctrtype)
		{
			lvalue = m_hs_NamToLightP_it->second.channalVal[index] & 0x00FF;//只是打开，关闭操作则lvalue为原来的值
		}
		m_hs_NamToLightP_it->second.channalVal[index] = 0xFFFF & ((ctrtype<<8) + lvalue);//前2个FF保存亮度值，第3个F保存开关状态
		m_command = "S";
		char szvalue[4] = {0};
		unsigned int val  = 0;
		unsigned int state = 0;

		// 将光源参数组合成字符串命令
		for (int i =0;i < 8; i ++)
		{
			memset(szvalue,0 ,sizeof(szvalue));
			val  = m_hs_NamToLightP_it->second.channalVal[i] & 0x00FF;
			state = m_hs_NamToLightP_it->second.channalVal[i] & 0x0100;
			sprintf_s(szvalue,"%03d",val);
			m_command.append(szvalue);
			if (0 == state)
			{
				m_command.append("F");
			}
			else
			{
				m_command.append("T");
			}
		}
		// 增加结束符
		m_command.append("C#");
	}
	else if (LIGHTCTL_TYPE_OPT == m_LightType)
	{
		//***************************************************************//
		//* 命令格式：$3202312
		//*	左起第1位是起始符，第二位是命令类型（1打开光源，2关闭光源，3设置光源参数，4读取光源参数），第3位是通道索引
		//*	第4--6位是16进制表示的光源参数值，最后两位是校验码
		//*	*************************************************************//
		m_command = "";
		char szvalue[9] = {0};
		unsigned int command = 0;
		if (LIGHT_OPEN == ctrtype)
		{
			command = 1;			
		}
		else if (LIGHT_OFF == ctrtype)
		{
			command = 2;
		}
		else if (LIGHT_SET == ctrtype)
		{
			command = 3;
			m_hs_NamToLightP_it->second.channalVal[index] = lvalue;//设置亮度
		}
		
		int val = lvalue;
		sprintf_s(szvalue,"%c%01d%01d%03X",'$',command,index+1,val);

		int verify  = 0;
		char verifych[3] = {0};
		verify = szvalue[0];  
		//循环异或，将异或结果存于command中，此时command不表示命令字。
		for(int i=1;i<6;i++)
		{
			verify = verify ^ ((int)szvalue[i]);
		}
		sprintf_s(verifych,"%02x",verify);
		strcat_s(szvalue,verifych);
		m_command.append(szvalue);
	}
	else if(LIGHTCTL_TYPE_OPT_PV == m_LightType)
	{
		
		//***************************************************************//
		//* 命令格式 ：FF	01	01	00 FF	00
		//* 命令字	说明					通道字			数据字	说明
		//*	01		设置亮度				0-10			0-FF	
		//*	02		设置触发脉宽			0-10			1-3E7	以1 mS为单位
		//*	03		设置高亮触发脉宽		0-10			1-1F4	以0.01 mS为单位
		//*	04		设置最大电流			0-10			1-64	以10mA为单位
		//*	05		关闭/打开通道		0-10			0/1		1：打开通道；
		//*	0:		关闭通道。
		//*	06		设置IO值	X			0/1	

		//*	最后两位是校验码
		//*	*************************************************************//
		m_command = "";
		char szvalue[23] = {0};
		unsigned int command = 0;
		int val = lvalue;			
		if (LIGHT_OPEN == ctrtype)
		{			
			command = 5;
			sprintf_s(szvalue,"%02X%02X%02X%04X",255,command,index+1,1);
		}
		else if (LIGHT_OFF == ctrtype)
		{
			command = 5;
			sprintf_s(szvalue,"%02X%02X%02X%04X",255,command,index+1,0);				
		}
		if (LIGHT_SET == ctrtype)
		{
			command = 1;
			m_hs_NamToLightP_it->second.channalVal[index] = lvalue;//设置亮度
			sprintf_s(szvalue,"%02X%02X%02X%04X",255,command,index+1,lvalue);
		}		

		int verify  = 0;
		char verifych[3] = {0};
		verify = szvalue[0];  
		//循环异或，将异或结果存于command中，此时command不表示命令字。
		for(int i=1;i<13;i++)
		{
			verify = verify ^ ((int)szvalue[i]);
		}
		sprintf_s(verifych,"%02x",verify);
		strcat_s(szvalue,verifych);
		m_command.append(szvalue);
	}

	return 0;
}


//发关命令到指定光源控制器，name：光源控制器名，comm：命令
int TLight::SendCommd( string comName,string comm)
{
	m_i_result = GetComer(comName);
	if (LIGHTCTL_SUCCESS == m_i_result)
	{
		if (LIGHTCTL_TYPE_OPT_PV == m_LightType)
		{
			comm = ChangeCharstr2Hexstr(comm);
		}
		
		m_hs_NamToCom_it->second->WriteData((void*)comm.c_str(),(int)comm.length());
		Sleep(50);
		string res = "*";//返回值长度
		m_hs_NamToCom_it->second->ReadData((void*)res.c_str());
		if (LIGHTCTL_TYPE_NDPS == m_LightType)
		{
			if (0 != res.compare("!"))
			{
				m_i_result = LIGHTCTL_SEND_FAIL; 
			}			
		}
		else if (LIGHTCTL_TYPE_OPT == m_LightType)
		{
			if (0 == res.compare("&"))
			{
				m_i_result = LIGHTCTL_SEND_FAIL; 
			}
		}	
		else if (LIGHTCTL_TYPE_OPT_PV == m_LightType)
		{
			if (0xf000 == atoi(res.c_str()))
			{
				m_i_result = LIGHTCTL_SEND_FAIL; 
			}
		}
	}	
	return m_i_result;
}

//获取com对像
int TLight::GetComer(string name)
{
	m_hs_NamToCom_it = m_hs_NameToCom->find(name);
	if (m_hs_NameToCom->end() == m_hs_NamToCom_it)
	{
		return LIGHTCTL_NOFINDTHE_COM;
	}
	return LIGHTCTL_SUCCESS;
}

int TLight::JudgeLightConnect(string comName)
{
    int iRet = -1;
    SerialPort *psei;
    SerialPort sei;
	if (LIGHTCTL_NOFINDTHE_COM != GetComer(comName))
	{
		psei = m_hs_NamToCom_it->second;
		iRet = 1;
	}
	else
    {
        psei = &sei;
		iRet = psei->Open()+1;
	}
	string sen = "!";
	string res = "*";
	if (1 == iRet)
	{
		psei->WriteData((void*)sen.c_str());
		Sleep(50);
		psei->ReadData((void*)res.c_str());
		if (0 == res.compare("&")||0 == res.compare("$"))
		{
			iRet = 0;
		}
	}
	return iRet;
}


//蛋疼的十六转化进制
string TLight::ChangeCharstr2Hexstr(string Charstr)
{
	string Hexstr="";
	HexStringFilter(Charstr);
	int Length=(int)Charstr.length();
	if(Length%2)
		Charstr.erase(Length-1);
	Length=(int)Charstr.length();
	char tc;
	for(int i=0;i<Length/2;i++)
	{
		tc = CombineHexChar(Charstr.at(i*2),Charstr.at(i*2+1));
		Hexstr.push_back(tc);
	}
	return Hexstr;
}

void TLight::HexStringFilter(string &str)
{
	BOOL bOK;
	for(unsigned int i=0;i<str.length();)
	{
		bOK=((str.at(i)>='0')&&(str.at(i)<='9'))||
			((str.at(i)>='A')&&(str.at(i)<='F'))||
			((str.at(i)>='a')&&(str.at(i)<='f'));
		if(!bOK)
			str.erase(i);
		else i++;	
	}
}

char TLight::CombineHexChar(char CharH,char CharL)
{
	char result;
	string temp;
	if(CharH>='0'&&CharH<='9')			result=(CharH-'0');
	else if(CharH>='a'&&CharH<='f')		result=(CharH-'a'+10);
	else if(CharH>='A'&&CharH<='F')		result=(CharH-'A'+10);
	else								result=0;
	result<<=4;	
	if(CharL>='0'&&CharL<='9')			result+=(CharL-'0');
	else if(CharL>='a'&&CharL<='f')		result+=(CharL-'a'+10);
	else if(CharL>='A'&&CharL<='F')		result+=(CharL-'A'+10);
	else								result+=0;
	return result;
}

string TLight::DevideHexChar(char HexChar)
{
	string result="";
	int temp=(HexChar&0xF0)>>4;
	if(temp<10)
		result+=(char)(temp+'0');
	else 
		result+=(char)(temp+'A'-10);
	temp=HexChar&0x0F;
	if(temp<10)
		result+=(char)(temp+'0');
	else 
		result+=(char)(temp+'A'-10);
	return result;
}