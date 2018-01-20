// HalProcPosSetting.cpp : implementation file
//

#include "stdafx.h"
#include "SceneManager.h"
#include "HalProcPosSetting.h"
#include "HalProcPosition.h"
#include "afxdialogex.h"


// HalProcPosSetting dialog

IMPLEMENT_DYNAMIC(HalProcPosSetting, CDialogEx)

HalProcPosSetting::HalProcPosSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(HalProcPosSetting::IDD, pParent)
{
	m_pContainer = NULL;
	m_ProcProcess = 0;
}

HalProcPosSetting::~HalProcPosSetting()
{
}

void HalProcPosSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(HalProcPosSetting, CDialogEx)
	ON_BN_CLICKED(IDC_HALPROC_LOADPATH, &HalProcPosSetting::OnBnClickedHalprocLoadpath)
	ON_BN_CLICKED(IDC_HALPROC_TEST, &HalProcPosSetting::OnBnClickedHalprocTest)
	ON_BN_CLICKED(IDC_HALPROC_COMMIT, &HalProcPosSetting::OnBnClickedHalprocCommit)
END_MESSAGE_MAP()


// HalProcPosSetting message handlers
void HalProcPosSetting::GetContainer(void* pContainer)
{
	m_pContainer = pContainer;

}

void  HalProcPosSetting::UpdateUI(void)
{
	//初始化变量
	InputTitleId[0]= IDC_HALPROC_INTITILE1;
	InputTitleId[1]= IDC_HALPROC_INTITILE2;
	InputTitleId[2]= IDC_HALPROC_INTITILE3;
	InputTitleId[3]= IDC_HALPROC_INTITILE4;
	InputTitleId[4]= IDC_HALPROC_INTITILE5;
	InputTitleId[5]= IDC_HALPROC_INTITILE6;
	InputTitleId[6]= IDC_HALPROC_INTITILE7;
	InputTitleId[7]= IDC_HALPROC_INTITILE8;
	InputTitleId[8]= IDC_HALPROC_INTITILE9;
	InputTitleId[9]= IDC_HALPROC_INTITILE10;
	InputTitleId[10]= IDC_HALPROC_INTITILE11;
	InputTitleId[11]= IDC_HALPROC_INTITILE12;

	InputParamId[0] = IDC_HALPROC_INPUT1;
	InputParamId[1] = IDC_HALPROC_INPUT2;
	InputParamId[2] = IDC_HALPROC_INPUT3;
	InputParamId[3] = IDC_HALPROC_INPUT4;
	InputParamId[4] = IDC_HALPROC_INPUT5;
	InputParamId[5] = IDC_HALPROC_INPUT6;
	InputParamId[6] = IDC_HALPROC_INPUT7;
	InputParamId[7] = IDC_HALPROC_INPUT8;
	InputParamId[8] = IDC_HALPROC_INPUT9;
	InputParamId[9] = IDC_HALPROC_INPUT10;
	InputParamId[10] = IDC_HALPROC_INPUT11;
	InputParamId[11] = IDC_HALPROC_INPUT12;


	//显示参数名称和参数
	CString strTemp;
	for (int i =0;i<INPUTPARAMNUM;i++)
	{
		if (m_InputTitle[i].GetLength()>0)
		{
			strTemp = m_InputTitle[i];
			strTemp.Replace('@',' ');
			GetDlgItem(InputTitleId[i])->SetWindowTextA(strTemp);


		}else
		{
			//如果读取名字为空，则设置为参数默认名称
			strTemp.Format("Input%d:",i+1);
			GetDlgItem(InputTitleId[i])->SetWindowTextA(strTemp);

		}

		//显示参数
		strTemp.Format("%lf",m_InParam[i]);
		GetDlgItem(InputParamId[i])->SetWindowTextA(strTemp);

	}
	//显示HalProcedure路径
	GetDlgItem(IDC_HALPROC_PROCPATH)->SetWindowTextA(m_ProcdurePath);



}

void HalProcPosSetting::CloseSettingDlg()
{
	OnOK();
}





void HalProcPosSetting::OnBnClickedHalprocCommit()
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	for (int i = 0;i<INPUTPARAMNUM;i++)
	{
		GetDlgItem(InputParamId[i])->GetWindowTextA(strTemp);
		m_InParam[i] = atof(strTemp);

	}

	GetDlgItem(IDC_HALPROC_PROCPATH)->GetWindowTextA(m_ProcdurePath);


}


void HalProcPosSetting::SaveProcConfig(CString ConfigPath,int ProcIndex)
{
	char szValue[256]={0};
	char szName[256] ={0};
	CString ProcIndexName,strTemp;
	ProcIndexName.Format("%d",ProcIndex);

	//对配置文件的操作最好是配置管理接口操作，为了简化代码量，直接在这里操作了

	//存放输入参数名称定义
	for (int i = 0 ;i<INPUTPARAMNUM;i++)
	{
		sprintf_s(szName,"InputTitle%d",i);
		WritePrivateProfileStringA(ProcIndexName, szName,m_InputTitle[i], ConfigPath);
	}

	//存放输入参数值
	for (int i = 0 ;i<INPUTPARAMNUM;i++)
	{
		sprintf_s(szName,"InputParam%d",i);
		sprintf_s(szValue,"%0.12lf",m_InParam[i]);
		WritePrivateProfileStringA(ProcIndexName,szName,szValue, ConfigPath);
	}

	//存放调用过程路径、名称
	sprintf_s(szName,"HalProcedurePath");
	WritePrivateProfileStringA(ProcIndexName, szName,m_ProcdurePath, ConfigPath);

}

void HalProcPosSetting::ReadProcConfig(CString ConfigPath,int ProcIndex)
{
	char szValue[256]={0};
	char szName[256] ={0};
	CString ProcIndexName,strTemp;
	ProcIndexName.Format("%d",ProcIndex);

	//读取输入参数名称定义
	for (int i = 0 ;i<INPUTPARAMNUM;i++)
	{
		sprintf_s(szName,"InputTitle%d",i);
		GetPrivateProfileStringA(ProcIndexName,szName,"",szValue,sizeof(szValue),ConfigPath);
		m_InputTitle[i] = szValue;

	}

	//读取输入参数值
	for (int i = 0 ;i<INPUTPARAMNUM;i++)
	{
		sprintf_s(szName,"InputParam%d",i);
		GetPrivateProfileStringA(ProcIndexName,szName,"-1.0",szValue,sizeof(szValue),ConfigPath);
		m_InParam[i] = atof(szValue);

	}

	//读取调用过程路径、名称
	sprintf_s(szName,"HalProcedurePath");
	GetPrivateProfileStringA(ProcIndexName,szName,"",szValue,sizeof(szValue),ConfigPath);
	m_ProcdurePath = szValue;

}

BOOL HalProcPosSetting::VisionProcRun(CHECK_RESULT& ProcRes)
{
	//调用视觉库中的函数，返回X,Y,phi
	CString strTemp;
	bool res = false;
	double PosX = 0.0,PosY = 0.0,PosPhi = 0.0;
	try
	{
		res = ((HalProcPosition*)m_pContainer)->CommonLocate(m_ProcdurePath,&m_InParam,PosX,PosY,PosPhi);
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



void HalProcPosSetting::OnBnClickedHalprocLoadpath()
{
	// TODO: Add your control notification handler code here
	CString strFilePath;
	const char pszFilter[] = _T("(*.*)|*.*|*.hdvp|*.hdpl");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,pszFilter, this);

	if(IDOK == dlg.DoModal())
	{
		strFilePath = dlg.GetPathName();
		if (0 == strFilePath.GetLength())
		{
			return;
		}
		strFilePath.Replace('\\','/');
		m_ProcdurePath = strFilePath;
		GetDlgItem(IDC_HALPROC_PROCPATH)->SetWindowTextA(m_ProcdurePath);
	}


}


void HalProcPosSetting::OnBnClickedHalprocTest()
{
	// TODO: Add your control notification handler code here
	bool res = false;
	CString strTemp;
	double PosX = 0.0,PosY = 0.0,PosPhi = 0.0;
	try
	{
		((HalProcPosition*)m_pContainer)->CommonLocate(m_ProcdurePath,&m_InParam,PosX,PosY,PosPhi);
		res = true;
	}
	catch (char* e)
	{
		GetDlgItem(IDC_HALPROC_TSETSTATE)->SetWindowTextA(e);
		res = false;
	}
	catch (...)
	{
		res = false;
	}

	if (res)
	{
		GetDlgItem(IDC_HALPROC_TSETSTATE)->SetWindowTextA("成功!");

		strTemp.Format("%lf",PosX);
		GetDlgItem(IDC_HALPROC_OUTX)->SetWindowTextA(strTemp);
		strTemp.Format("%lf",PosY);
		GetDlgItem(IDC_HALPROC_OUTY)->SetWindowTextA(strTemp);
		strTemp.Format("%lf",PosPhi);
		GetDlgItem(IDC_HALPROC_OUTPHI)->SetWindowTextA(strTemp);
	}else
	{
		GetDlgItem(IDC_HALPROC_OUTX)->SetWindowTextA("");
		GetDlgItem(IDC_HALPROC_OUTY)->SetWindowTextA("");
		GetDlgItem(IDC_HALPROC_OUTPHI)->SetWindowTextA("");
	}


}







