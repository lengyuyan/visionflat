// ScenePreProSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneManager.h"
#include "ScenePreProSetting.h"
#include "afxdialogex.h"

#include "ScenePrePro.h"


// ScenePreProSetting 对话框

IMPLEMENT_DYNAMIC(ScenePreProSetting, CDialogEx)

ScenePreProSetting::ScenePreProSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(ScenePreProSetting::IDD, pParent)
{
	m_pContainer = NULL;
	m_ProcProcess = 0;
	m_Multiplication =1;
	m_Addition = 0.0;
}

ScenePreProSetting::~ScenePreProSetting()
{
}

void ScenePreProSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ScenePreProSetting, CDialogEx)
	ON_BN_CLICKED(IDC_BT_SETTING, &ScenePreProSetting::OnBnClickedBtSetting)
	ON_BN_CLICKED(IDC_BT_TESTENHANCE, &ScenePreProSetting::OnBnClickedBtTestenhance)
END_MESSAGE_MAP()


// ScenePreProSetting 消息处理程序
void ScenePreProSetting::GetContainer(void* pContainer)
{
	m_pContainer = pContainer;

}
void  ScenePreProSetting::UpdateUI(void)
{
	//设置消息
	//CString mess = hmessage.c_str();
	//char err[10] = {0};
	//itoa(m_error,err,10);
	//mess += " Ecode:";
	//mess+=err;
	//GetDlgItem(IDC_HALPROC_PROCPMESSAGE)->SetWindowText(mess);
	CString str;

	str.Format("%.4lf",m_Multiplication);
	GetDlgItem(IDC_EDIT_MUTITY)->SetWindowText(str);
	str.Format("%.4lf",m_Addition);
	GetDlgItem(IDC_EDIT_ADD)->SetWindowText(str);



}
void ScenePreProSetting::CloseSettingDlg()
{
	OnOK();
}
void ScenePreProSetting::SaveProcConfig(CString ConfigPath,int ProcIndex)
{
	char szValue[256]={0};
	char szName[256] ={0};
	CString ProcIndexName,strTemp;
	ProcIndexName.Format("%d",ProcIndex);

	//对配置文件的操作最好是配置管理接口操作，为了简化代码量，直接在这里操作了


	//存放输入参数值

	
	sprintf_s(szName,"Multiplication");
	sprintf_s(szValue,"%0.4lf",m_Multiplication);
	WritePrivateProfileStringA(ProcIndexName,szName,szValue, ConfigPath);
	sprintf_s(szName,"Addition");
	sprintf_s(szValue,"%0.4lf",m_Addition);
	WritePrivateProfileStringA(ProcIndexName,szName,szValue, ConfigPath);


}
void ScenePreProSetting::ReadProcConfig(CString ConfigPath,int ProcIndex)
{
	//读取倍数和家数
	CString ProcIndexName,strTemp;
	ProcIndexName.Format("%d",ProcIndex);
	char szValue[256]={0};
	char szName[256] ={0};
	sprintf_s(szName,"Multiplication");
	GetPrivateProfileString(ProcIndexName,szName,"1.0",szValue,sizeof(szValue),ConfigPath);
	m_Multiplication = atof(szValue);
	sprintf_s(szName,"Addition");
	GetPrivateProfileString(ProcIndexName,szName,"0.0",szValue,sizeof(szValue),ConfigPath);
	m_Addition  = atof(szValue);

}
BOOL ScenePreProSetting::VisionProcRun(CHECK_RESULT& ProcRes)
{
	//调用视觉库中的函数，返回X,Y,phi
	CString strTemp;
	bool res = false;
	double PosX = 0.0,PosY = 0.0,PosPhi = 0.0;
	try
	{
		res = ((ScenePrePro*)m_pContainer)->ImageEnhancement(m_Multiplication,m_Addition);
	}
	catch (CString& e)
	{

		res = false;
	}
	catch (...)
	{

		res = false;
	}

	if(res)
	{

	}else
	{
		PosX = 0.0;
		PosY = 0.0;
		PosPhi = 0.0;

	}

	//int BeginIndex = ProcRes.iCount;
	//ProcRes.iCount += 3;
	//char szTempt[128] ={0} ;
	// 是否需要将匹配的值变成
	int ResCount = ProcRes.iCount; 
	sprintf(ProcRes.szValue[0],"%d",res);
	sprintf(ProcRes.szValue[1],"%0.4lf",PosX);
	sprintf(ProcRes.szValue[2],"%0.4lf",PosY);
	sprintf(ProcRes.szValue[3],"%0.4lf",PosPhi);
	ProcRes.iCount = 4;

	return res;
}


void ScenePreProSetting::OnBnClickedBtSetting()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTemp;
	GetDlgItem(IDC_EDIT_MUTITY)->GetWindowText(strTemp);
	m_Multiplication = atof(strTemp);
	GetDlgItem(IDC_EDIT_ADD)->GetWindowText(strTemp);
	m_Addition = atof(strTemp);

	
}


void ScenePreProSetting::OnBnClickedBtTestenhance()
{
	// TODO: 在此添加控件通知处理程序代码
	bool res = false;
	try
	{
		res = ((ScenePrePro*)m_pContainer)->ImageEnhancement(m_Multiplication,m_Addition);
	}
	catch (CString& e)
	{

		res = false;
	}
}
