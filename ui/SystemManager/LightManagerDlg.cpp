// LightManagerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LightManagerDlg.h"
#include "afxdialogex.h"


// LightManagerDlg 对话框

IMPLEMENT_DYNAMIC(LightManagerDlg, CDialogEx)

LightManagerDlg::LightManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(LightManagerDlg::IDD, pParent)
{

}

LightManagerDlg::~LightManagerDlg()
{
}

void LightManagerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_CTRL_INDEX, m_CommList);
}


BEGIN_MESSAGE_MAP(LightManagerDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_OP_LIGHT, &LightManagerDlg::OnBnClickedButtonOpLight)
END_MESSAGE_MAP()


// LightManagerDlg 消息处理程序


void LightManagerDlg::OnBnClickedButtonOpLight()
{
    // TODO: 在此添加控件通知处理程序代码
}


BOOL LightManagerDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化
    InitLightComBox();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void LightManagerDlg::InitLightComBox()
{
    HKEY hKey; 
    int  index = 0;
    if(::RegOpenKeyEx( HKEY_LOCAL_MACHINE, "Hardware\\DeviceMap\\SerialComm", 
        NULL,
        KEY_READ,
        &hKey ) == ERROR_SUCCESS ) //打开串口注册表对应的键值 
    {
        int i=0; 
        char portName[256],commName[256];
        DWORD dwLong,dwSize;
        while(1)
        { 
            dwLong = dwSize = sizeof(portName); 
            if( ::RegEnumValue( hKey, i, portName, &dwLong,NULL,NULL, (PUCHAR)commName, &dwSize ) == ERROR_NO_MORE_ITEMS )// 枚举串口 
            {
                break; 
            }

            m_CommList.AddString( commName ); // commName就是串口名字 

            i++; 
        } 

        if( m_CommList.GetCount() == 0 ) 
        { 
            //::AfxMessageBox("串口枚举失败!!!"); 

        } 
        RegCloseKey(hKey);
        m_CommList.SetCurSel(0);
    } 
}