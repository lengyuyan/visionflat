// SRBtn.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneManager.h"
#include "SRBtn.h"
#include "afxdialogex.h"
#include "iManagerCtrl.h"


// CSRBtn 对话框
#define    ITRANSITION_INDEX 11
#define    IFINDTYPE_INDEX   12
#define    IALGORITHM_INDEX  13
#define    IDIRECTION_INDEX  14

IMPLEMENT_DYNAMIC(CSRBtn, CDialogEx)

CSRBtn::CSRBtn(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSRBtn::IDD, pParent)
	, m_radioNumb(0)
{
	m_iCurSceneID = 0;
	m_iCurIndex = 0;
	m_winID = 0;
}

CSRBtn::~CSRBtn()
{
}

void CSRBtn::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_radioNumb);
}


BEGIN_MESSAGE_MAP(CSRBtn, CDialogEx)

	//自定义消息
	ON_MESSAGE(BTN_CLICKED_RADIO, &CSRBtn::OnBtnMessage)

	ON_BN_CLICKED(IDC_RADIO1, &CSRBtn::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CSRBtn::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CSRBtn::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &CSRBtn::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO5, &CSRBtn::OnBnClickedRadio5)
	ON_BN_CLICKED(IDC_RADIO6, &CSRBtn::OnBnClickedRadio6)
	ON_BN_CLICKED(IDC_RADIO7, &CSRBtn::OnBnClickedRadio7)
	ON_BN_CLICKED(IDC_RADIO8, &CSRBtn::OnBnClickedRadio8)
END_MESSAGE_MAP()

void CSRBtn::getInterfaceSetData( INTERFACE_SET_DATA interfaceSetData )
{
    GetDlgItem(IDC_STATIC_ITEM_NAME)->SetWindowText(interfaceSetData.BtnName.c_str());
	m_interfaceSetData = interfaceSetData;
	m_radioNumb = (int)interfaceSetData.InitValue;  //数据个数
	GetDlgItem(IDC_RADIO1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO5)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO6)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO7)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO8)->ShowWindow(SW_HIDE);

	for (int i = 0; i < interfaceSetData.OptionNumb; ++i)
	{
		switch (i)
		{
		case 0:
			GetDlgItem(IDC_RADIO1)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_RADIO1)->SetWindowText(interfaceSetData.OptionName[i]);
			break;
		case 1:
			GetDlgItem(IDC_RADIO2)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_RADIO2)->SetWindowText(interfaceSetData.OptionName[i]);
			break;
		case 2:
			GetDlgItem(IDC_RADIO3)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_RADIO3)->SetWindowText(interfaceSetData.OptionName[i]);
			break;
		case 3:
			GetDlgItem(IDC_RADIO4)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_RADIO4)->SetWindowText(interfaceSetData.OptionName[i]);
			break;
		case 4:
			GetDlgItem(IDC_RADIO5)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_RADIO5)->SetWindowText(interfaceSetData.OptionName[i]);
			break;
		case 5:
			GetDlgItem(IDC_RADIO6)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_RADIO6)->SetWindowText(interfaceSetData.OptionName[i]);
			break;
		case 6:
			GetDlgItem(IDC_RADIO7)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_RADIO7)->SetWindowText(interfaceSetData.OptionName[i]);
			break;
		case 7:
			GetDlgItem(IDC_RADIO8)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_RADIO8)->SetWindowText(interfaceSetData.OptionName[i]);
			break;
		}
	}
	UpdateData(FALSE);
}


// CSRBtn 消息处理程序


void CSRBtn::OnBnClickedRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
	SendMessage(BTN_CLICKED_RADIO, BTN_RADIO_ZERO);
}


void CSRBtn::OnBnClickedRadio2()
{
	// TODO: 在此添加控件通知处理程序代码
	SendMessage(BTN_CLICKED_RADIO, BTN_RADIO_ONE);
}


void CSRBtn::OnBnClickedRadio3()
{
	// TODO: 在此添加控件通知处理程序代码
	SendMessage(BTN_CLICKED_RADIO, BTN_RADIO_TOW);
}


void CSRBtn::OnBnClickedRadio4()
{
	// TODO: 在此添加控件通知处理程序代码
	SendMessage(BTN_CLICKED_RADIO, BTN_RADIO_THREE);
}


void CSRBtn::OnBnClickedRadio5()
{
	// TODO: 在此添加控件通知处理程序代码
	SendMessage(BTN_CLICKED_RADIO, BTN_RADIO_FOUR);
}


void CSRBtn::OnBnClickedRadio6()
{
	// TODO: 在此添加控件通知处理程序代码
	SendMessage(BTN_CLICKED_RADIO, BTN_RADIO_FIVE);
}

void CSRBtn::OnBnClickedRadio7()
{
	// TODO: 在此添加控件通知处理程序代码
	SendMessage(BTN_CLICKED_RADIO, BTN_RADIO_SIX);
}


void CSRBtn::OnBnClickedRadio8()
{
	// TODO: 在此添加控件通知处理程序代码
	SendMessage(BTN_CLICKED_RADIO, BTN_RADIO_SEVEN);
}


LRESULT CSRBtn::OnBtnMessage( WPARAM wParam, LPARAM lParam )
{
	bool insert = false;

	switch (wParam)
	{
	case BTN_RADIO_ZERO:
		break;
	case BTN_RADIO_ONE:
		break;
	case BTN_RADIO_TOW:
		break;
	case BTN_RADIO_THREE:
		break;
	case BTN_RADIO_FOUR:
		break;
	case BTN_RADIO_FIVE:
		break;
	case BTN_RADIO_SIX:
		break;
	case BTN_RADIO_SEVEN:
		break;
	case BTN_RADIO_INIT:
		insert = true;
		break;
	case BTN_RADIO_DELETE:
		break;
	}

	UpdateItem((BTN_RADIO_NUMB)wParam, insert);

	return 0;
}

void CSRBtn::UpdateUI( int iSceneID, int index, CWnd *hROI, int revise /*= 0*/ )
{
	m_iCurSceneID = iSceneID;
	m_iCurIndex = index;
	if (1 == revise)
	{
		SendMessage(BTN_CLICKED_RADIO, BTN_RADIO_INIT);
	}
	else if (0 == revise)
	{
		SendMessage(BTN_CLICKED_RADIO, -1);
	}
	else if (-1 == revise)
	{
		SendMessage(BTN_CLICKED_RADIO, BTN_RADIO_DELETE);
	}
}

void CSRBtn::SetDisplayWindowID( int winID )
{
	m_winID = winID;
}

void CSRBtn::UpdateItem( BTN_RADIO_NUMB radioNumb, bool insert /*= false*/ )
{
	UpdateData(TRUE);
	vector<VIRSION_DATA_PKG *>::iterator it;
	vector<unsigned int *>::iterator itType;
	SCENE_PKG pConfig;				//从配置文件中读取图片有关roi信息
	int insertPos = 0;
	GetSceneParam(m_iCurSceneID, &pConfig);
	//GetSceneParam(m_interfaceSetData.CurSceneID, &pConfig);
	VIRSION_DATA_PKG *pTmp = NULL;
	unsigned int *pTmpType = NULL;
	//获取原来数据
	for (it = pConfig.vecBody.begin(), itType = pConfig.vecDataType.begin(); it != pConfig.vecBody.end(); it++, itType++)
	{
		pTmp = *it;
		pTmpType = *itType;
		if ((pTmp->iValue % 10000) == m_interfaceSetData.iT)
		{
			break;
		}
		++insertPos;
	}
	if (insertPos >= pConfig.pkgCount)
	{
		return;
	}

	int iROISize = 0;
	if (BTN_RADIO_INIT == radioNumb)	//初始化
	{
		iROISize = pTmp[1].iValue + 1;
	}
	else if (BTN_RADIO_DELETE == radioNumb)
	{
		iROISize = 0;
	}
	else
	{
		iROISize = pTmp[1].iValue;
	}

	//根据原来数据分配空间
	unsigned int iFlag = ROI_TYPE * 100 + 1;
	int iSize = VIRSION_DATA_PKG_HEADER_SIZE + iROISize;
	VIRSION_DATA_PKG *pInfo =  new VIRSION_DATA_PKG[iSize];
	unsigned int *pType = new unsigned int[iSize];
	memset(pInfo, 0, iSize * sizeof(VIRSION_DATA_PKG));
	memset(pType, 0, iSize * sizeof(unsigned int));

	//更新数据
	for (int indexT = 0; indexT < iSize; ++indexT)
	{
		//pTmp更新
		if (1 == indexT)	//数据numb
		{
			if (BTN_RADIO_DELETE == radioNumb)
			{
				pInfo[indexT].iValue  = 0;
				memset(m_interfaceSetData.OptionName, 0, 8 * 8);
			}
			else if (pTmp[indexT].iValue < m_iCurIndex + 1)
			{
				pInfo[indexT].iValue = m_iCurIndex + 1;
			}
			else if (pTmp[indexT].iValue >= m_iCurIndex + 1)
			{
				pInfo[indexT].iValue = pTmp[indexT].iValue;
			}
			pType[indexT] = pTmpType[indexT];
		}
		else if (m_interfaceSetData.nIndex + VIRSION_DATA_PKG_HEADER_SIZE == indexT)	//修改值
		{
			pInfo[indexT].iValue = m_radioNumb;
			m_interfaceSetData.InitValue = m_radioNumb;
			//pType更新
			pType[indexT] = 0;
		}
		else
		{
			pInfo[indexT].iValue = pTmp[indexT].iValue;
			pInfo[indexT].dValue = pTmp[indexT].dValue;
			pType[indexT] = pTmpType[indexT];
		}
	}

	//更新ParamItem
	UpdateParamItem(m_iCurSceneID, pInfo, pType, &m_interfaceSetData, iSize, insertPos, insert);
}


BOOL CSRBtn::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetBackgroundColor(BACKGROUND_COLOR_M);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

int CSRBtn::CurSceneID()
{
	return m_interfaceSetData.CurSceneID;
}

string CSRBtn::BtnType()
{
	return m_interfaceSetData.BtnType;
}
