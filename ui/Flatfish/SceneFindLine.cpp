// SceneFindLine.cpp : 实现文件
//

#include "stdafx.h"
#include "Flatfish.h"
#include "SceneFindLine.h"
#include "afxdialogex.h"
#include "iManagerCtrl.h"


// CSceneFindLine 对话框
#define    SMOOTH_INDEX      7
#define    THRESHOLD_INDEX   8
#define    ROIWIDTH_INDEX    9
#define    IPOSNUM_INDEX     10
#define    ITRANSITION_INDEX 11
#define    IFINDTYPE_INDEX   12
#define    IALGORITHM_INDEX  13
#define    IDIRECTION_INDEX  14

IMPLEMENT_DYNAMIC(CSceneFindLine, CDialogEx)

CSceneFindLine::CSceneFindLine(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSceneFindLine::IDD, pParent)
{
    m_iCurSceneID = 0;
    m_iCurIndex = 0;
    m_bIsAutoFollow = false;
}

CSceneFindLine::~CSceneFindLine()
{
}

void CSceneFindLine::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSceneFindLine, CDialogEx)
    //ON_BN_CLICKED(IDC_BUTTON_SETTING, &CSceneFindLine::OnBnClickedButtonSetting)
    ON_BN_CLICKED(IDC_BTN_EDGEADD, &CSceneFindLine::OnBnClickedBtnEdgeadd)
    ON_BN_CLICKED(IDC_BTN_EDGEMINUS, &CSceneFindLine::OnBnClickedBtnEdgeminus)
    ON_BN_CLICKED(IDC_BTN_SMOOTHADD, &CSceneFindLine::OnBnClickedBtnSmoothadd)
    ON_BN_CLICKED(IDC_BTN_SMOOTHMINUS, &CSceneFindLine::OnBnClickedBtnSmoothminus)
    ON_BN_CLICKED(IDC_BTN_ROIADD, &CSceneFindLine::OnBnClickedBtnRoiadd)
    ON_BN_CLICKED(IDC_BTN_ROIMINUS, &CSceneFindLine::OnBnClickedBtnRoiminus)
    ON_BN_CLICKED(IDC_BTN_NUMADD, &CSceneFindLine::OnBnClickedBtnNumadd)
    ON_BN_CLICKED(IDC_BTN_NUMMINUS, &CSceneFindLine::OnBnClickedBtnNumminus)
    ON_BN_CLICKED(IDC_RADIO_TRANSITION_ALL, &CSceneFindLine::OnBnClickedRadioTransitionAll)
    ON_BN_CLICKED(IDC_RADIO_TRANSITION_POSITIVE, &CSceneFindLine::OnBnClickedRadioTransitionPositive)
    ON_BN_CLICKED(IDC_RADIO_TRANSITION_NEGATIVE, &CSceneFindLine::OnBnClickedRadioTransitionNegative)
    ON_BN_CLICKED(IDC_RADIO_FIND_TYPE_ALL, &CSceneFindLine::OnBnClickedRadioFindTypeAll)
    ON_BN_CLICKED(IDC_RADIO_FIND_TYPE_FIRST, &CSceneFindLine::OnBnClickedRadioFindTypeFirst)
    ON_BN_CLICKED(IDC_RADIO_FIND_TYPE_LAST, &CSceneFindLine::OnBnClickedRadioFindTypeLast)
    ON_BN_CLICKED(IDC_RADIO_DIRECTION_P, &CSceneFindLine::OnBnClickedRadioDirectionP)
    ON_BN_CLICKED(IDC_RADIO_DIRECTION_N, &CSceneFindLine::OnBnClickedRadioDirectionN)
    ON_BN_CLICKED(IDC_RADIO_ALGORITHM_REG, &CSceneFindLine::OnBnClickedRadioAlgorithmReg)
    ON_BN_CLICKED(IDC_RADIO_ALGORITHM_HUB, &CSceneFindLine::OnBnClickedRadioAlgorithmHub)
    ON_BN_CLICKED(IDC_RADIO_ALGORITHM_TUKEY, &CSceneFindLine::OnBnClickedRadioAlgorithmTukey)
    ON_BN_CLICKED(IDC_RADIO_ALGORITHM_GAU, &CSceneFindLine::OnBnClickedRadioAlgorithmGau)
    ON_BN_CLICKED(IDC_RADIO_ALGORITHM_DROP, &CSceneFindLine::OnBnClickedRadioAlgorithmDrop)
    ON_BN_CLICKED(IDC_BTN_CHECK_ONE, &CSceneFindLine::OnBnClickedBtnCheckOne)
    ON_BN_CLICKED(IDC_BTN_UPDATE_ROI, &CSceneFindLine::OnBnClickedBtnUpdateRoi)
    ON_BN_CLICKED(IDC_CHECK_AUTO_FOLLOW, &CSceneFindLine::OnBnClickedCheckAutoFollow)
END_MESSAGE_MAP()


// CSceneFindLine 消息处理程序

void CSceneFindLine::UpdateUI(int iSceneID, int index,CWnd *hROI)
{
    char szBuf[256] = {0};
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    int iTransition = TRANSITION_TYPE_ALL;
    int iFindType = FINDTYPE_ALL;
    int iAlgorithm = ALGORITHM_TYPE_REGRESSION;
    int iDirection = DIRECTION_TYPE_POSITIVE;

    SetBackgroundColor(BACKGROUND_COLOR_M);

    m_iCurSceneID = iSceneID;
    m_iCurIndex = index;
    m_hROI=hROI;
    GetSceneParam(iSceneID,&config);

    int iPos = IFindParamData(&config, LINE_PRO, m_iCurIndex);
    if (iPos >= 0)
    {
        int iNo = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos]);
        sprintf_s(szBuf, "%0.1f", config.vecBody[iPos][SMOOTH_INDEX].dValue);
        GetDlgItem(IDC_EDIT_SMOOTH)->SetWindowText(szBuf);
        iNo++;
        sprintf_s(szBuf, "%d", config.vecBody[iPos][THRESHOLD_INDEX].iValue);
        GetDlgItem(IDC_EDIT_EDGE)->SetWindowText(szBuf);
        iNo++;
        sprintf_s(szBuf, "%d", config.vecBody[iPos][ROIWIDTH_INDEX].iValue);
        GetDlgItem(IDC_EDIT_ROI)->SetWindowText(szBuf);
        iNo++;
        sprintf_s(szBuf, "%d", config.vecBody[iPos][IPOSNUM_INDEX].iValue);
        GetDlgItem(IDC_EDIT_POSNUM)->SetWindowText(szBuf);
        iNo++;
        iTransition = config.vecBody[iPos][ITRANSITION_INDEX].iValue;
        iNo++;
        iFindType = config.vecBody[iPos][IFINDTYPE_INDEX].iValue;
        iNo++;
        iAlgorithm = config.vecBody[iPos][IALGORITHM_INDEX].iValue;
        iNo++;
        iDirection = config.vecBody[iPos][IDIRECTION_INDEX].iValue;
    }



    if (iPos >= 0)
    {
        CButton *btRTrALL = (CButton *)GetDlgItem(IDC_RADIO_TRANSITION_ALL);
        CButton *btP = (CButton *)GetDlgItem(IDC_RADIO_TRANSITION_POSITIVE);
        CButton *btN = (CButton *)GetDlgItem(IDC_RADIO_TRANSITION_NEGATIVE);
        switch(iTransition)
        {
        case TRANSITION_TYPE_ALL:
            btRTrALL->SetCheck(true);
            btP->SetCheck(false);
            btN->SetCheck(false);
            break;
        case TRANSITION_TYPE_POSITIVE:
            btRTrALL->SetCheck(false);
            btP->SetCheck(true);
            btN->SetCheck(false);
            break;
        case TRANSITION_TYPE_NEGATIVE:
            btRTrALL->SetCheck(false);
            btP->SetCheck(false);
            btN->SetCheck(true);
            break;
        }
        CButton *btRTypeALL = (CButton *)GetDlgItem(IDC_RADIO_FIND_TYPE_ALL);
        CButton *btF = (CButton *)GetDlgItem(IDC_RADIO_FIND_TYPE_FIRST);
        CButton *btL = (CButton *)GetDlgItem(IDC_RADIO_FIND_TYPE_LAST);
        switch(iFindType)
        {
        case FINDTYPE_ALL:
            btRTypeALL->SetCheck(true);
            btF->SetCheck(false);
            btL->SetCheck(false);
            break;
        case FINDTYPE_FIRST:
            btRTypeALL->SetCheck(false);
            btF->SetCheck(true);
            btL->SetCheck(false);
            break;
        case FINDTYPE_LAST:
            btRTypeALL->SetCheck(false);
            btF->SetCheck(false);
            btL->SetCheck(true);
            break;
        }
        CButton *btRegression = (CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_REG);
        CButton *btHuber = (CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_HUB);
        CButton *btTukey = (CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_TUKEY);
        CButton *btGauss = (CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_GAU);
        CButton *btDrop = (CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_DROP);
        switch(iAlgorithm)
        {
        case ALGORITHM_TYPE_REGRESSION:
            btRegression->SetCheck(true);
            btHuber->SetCheck(false);
            btTukey->SetCheck(false);
            btGauss->SetCheck(false);
            btDrop->SetCheck(false);
            break;
        case ALGORITHM_TYPE_HUBER:
            btRegression->SetCheck(false);
            btHuber->SetCheck(true);
            btTukey->SetCheck(false);
            btGauss->SetCheck(false);
            btDrop->SetCheck(false);
            break;
        case ALGORITHM_TYPE_TUKEY:
            btRegression->SetCheck(false);
            btHuber->SetCheck(false);
            btTukey->SetCheck(true);
            btGauss->SetCheck(false);
            btDrop->SetCheck(false);
            break;
        case ALGORITHM_TYPE_GAUSS:
            btRegression->SetCheck(false);
            btHuber->SetCheck(false);
            btTukey->SetCheck(false);
            btGauss->SetCheck(true);
            btDrop->SetCheck(false);
            break;
        case ALGORITHM_TYPE_DROP:
            btRegression->SetCheck(false);
            btHuber->SetCheck(false);
            btTukey->SetCheck(false);
            btGauss->SetCheck(false);
            btDrop->SetCheck(true);
            break;
        }
        switch(iDirection)
        {
        case DIRECTION_TYPE_POSITIVE:
            ((CButton *)GetDlgItem(IDC_RADIO_DIRECTION_P))->SetCheck(true);
            ((CButton *)GetDlgItem(IDC_RADIO_DIRECTION_N))->SetCheck(false);
            break;
        case DIRECTION_TYPE_NEGATIVE:
            ((CButton *)GetDlgItem(IDC_RADIO_DIRECTION_P))->SetCheck(false);
            ((CButton *)GetDlgItem(IDC_RADIO_DIRECTION_N))->SetCheck(true);
            break;
        }
    }
}
//
//void CSceneFindLine::OnBnClickedButtonSetting()
//{
//    // TODO: 在此添加控件通知处理程序代码
//    SCENE_CONFIG config;
//    GetSceneParam(m_iCurSceneID, &config);
//    SetSceneParam(&config);
//}

void CSceneFindLine::OnBnClickedBtnEdgeadd()
{
    CString str;
    int iValue = 0;
    GetDlgItem(IDC_EDIT_EDGE)->GetWindowText(str);
    iValue = atoi(str.GetBuffer());
    iValue = iValue + 1;
    if (iValue >100)
    {
        return;
    }
    SetSceneParamItemValue(THRESHOLD_INDEX,0, iValue, 0);
    str.Format("%d",iValue);
    GetDlgItem(IDC_EDIT_EDGE)->SetWindowText(str);
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedBtnEdgeminus()
{
    CString str;
    int iValue = 0;
    GetDlgItem(IDC_EDIT_EDGE)->GetWindowText(str);
    iValue = atoi(str.GetBuffer());
    iValue = iValue - 1;
    if (iValue >100)
    {
        return;
    }
    SetSceneParamItemValue(THRESHOLD_INDEX,0, iValue, 0);
    str.Format("%d",iValue);
    GetDlgItem(IDC_EDIT_EDGE)->SetWindowText(str);
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedBtnSmoothadd()
{
    CString str;
    double dSmooth = 0.0;
    GetDlgItem(IDC_EDIT_SMOOTH)->GetWindowText(str);
    dSmooth = atof(str.GetBuffer());
    dSmooth = dSmooth + 0.1;
    if (dSmooth >25)
    {
        return;
    }
    str.Format("%0.1f",dSmooth);
    GetDlgItem(IDC_EDIT_SMOOTH)->SetWindowText(str);
    SetSceneParamItemValue(SMOOTH_INDEX, 1, 0, dSmooth);
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedBtnSmoothminus()
{
    CString str;
    double dSmooth = 0.0;
    GetDlgItem(IDC_EDIT_SMOOTH)->GetWindowText(str);
    dSmooth = atof(str.GetBuffer());
    dSmooth = dSmooth - 0.1;
    if (dSmooth >25)
    {
        return;
    }
    str.Format("%0.1f",dSmooth);
    GetDlgItem(IDC_EDIT_SMOOTH)->SetWindowText(str);
    SetSceneParamItemValue(SMOOTH_INDEX, 1, 0, dSmooth);
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedBtnRoiadd()
{
    CString str;
    int iValue = 0;
    GetDlgItem(IDC_EDIT_ROI)->GetWindowText(str);
    iValue = atoi(str.GetBuffer());
    iValue = iValue + 1;
    if (iValue >200)
    {
        return;
    }
    str.Format("%d",iValue);
    GetDlgItem(IDC_EDIT_ROI)->SetWindowText(str);
    SetSceneParamItemValue(ROIWIDTH_INDEX, 0, iValue, 0.0);
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedBtnRoiminus()
{
    CString str;
    int iValue = 0;
    GetDlgItem(IDC_EDIT_ROI)->GetWindowText(str);
    iValue = atoi(str.GetBuffer());
    iValue = iValue - 1;
    if (iValue >200)
    {
        return;
    }
    str.Format("%d",iValue);
    GetDlgItem(IDC_EDIT_ROI)->SetWindowText(str);
    SetSceneParamItemValue(ROIWIDTH_INDEX, 0, iValue, 0.0);
    OnBnClickedBtnCheckOne();
}

void CSceneFindLine::OnBnClickedBtnNumadd()
{
    CString str;
    int iValue = 0;
    GetDlgItem(IDC_EDIT_POSNUM)->GetWindowText(str);
    iValue = atoi(str.GetBuffer());
    iValue = iValue + 1;
    if (iValue < 0)
    {
        return;
    }
    str.Format("%d",iValue);
    GetDlgItem(IDC_EDIT_POSNUM)->SetWindowText(str);
    SetSceneParamItemValue(IPOSNUM_INDEX, 0, iValue, 0.0);
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedBtnNumminus()
{
    CString str;
    int iValue = 0;
    GetDlgItem(IDC_EDIT_POSNUM)->GetWindowText(str);
    iValue = atoi(str.GetBuffer());
    iValue = iValue - 1;
    if (iValue < 0)
    {
        return;
    }
    str.Format("%d",iValue);
    GetDlgItem(IDC_EDIT_POSNUM)->SetWindowText(str);
    SetSceneParamItemValue(IPOSNUM_INDEX, 0, iValue, 0.0);
    OnBnClickedBtnCheckOne();
}

void CSceneFindLine::OnBnClickedRadioTransitionAll()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    int iCheck = 0;
    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_TRANSITION_ALL))->GetCheck();
    if (1 == iCheck)
    {
        SetSceneParamItemValue(ITRANSITION_INDEX, 0, TRANSITION_TYPE_ALL, 0.0);
    }
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedRadioTransitionPositive()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    int iCheck = 0;
    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_TRANSITION_POSITIVE))->GetCheck();
    if (1 == iCheck)
    {
        SetSceneParamItemValue(ITRANSITION_INDEX, 0, TRANSITION_TYPE_POSITIVE, 0.0);
    }
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedRadioTransitionNegative()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    int iCheck = 0;
    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_TRANSITION_NEGATIVE))->GetCheck();
    if (1 == iCheck)
    {
        SetSceneParamItemValue(ITRANSITION_INDEX, 0, TRANSITION_TYPE_NEGATIVE, 0.0);
    }
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedRadioFindTypeAll()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    int iCheck = 0;
    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_FIND_TYPE_ALL))->GetCheck();
    if (1 == iCheck)
    {
        SetSceneParamItemValue(IFINDTYPE_INDEX, 0, FINDTYPE_ALL, 0.0);
    }
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedRadioFindTypeFirst()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    int iCheck = 0;
    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_FIND_TYPE_FIRST))->GetCheck();
    if (1 == iCheck)
    {
        SetSceneParamItemValue(IFINDTYPE_INDEX, 0, FINDTYPE_FIRST, 0.0);
    }
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedRadioFindTypeLast()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    int iCheck = 0;
    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_FIND_TYPE_LAST))->GetCheck();
    if (1 == iCheck)
    {
        SetSceneParamItemValue(IFINDTYPE_INDEX, 0, FINDTYPE_LAST, 0.0);
    }
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedRadioDirectionP()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    int iCheck = 0;
    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_DIRECTION_P))->GetCheck();
    if (1 == iCheck)
    {
        SetSceneParamItemValue(IDIRECTION_INDEX, 0, DIRECTION_TYPE_POSITIVE, 0.0);
    }
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedRadioDirectionN()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    int iCheck = 0;
    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_DIRECTION_N))->GetCheck();
    if (1 == iCheck)
    {
        SetSceneParamItemValue(IDIRECTION_INDEX, 0, DIRECTION_TYPE_NEGATIVE, 0.0);
    }
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedRadioAlgorithmReg()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    int iCheck = 0;
    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_REG))->GetCheck();
    if (1 == iCheck)
    {
        SetSceneParamItemValue(IALGORITHM_INDEX, 0, ALGORITHM_TYPE_REGRESSION, 0.0);
    }
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedRadioAlgorithmHub()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    int iCheck = 0;
    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_HUB))->GetCheck();
    if (1 == iCheck)
    {
        SetSceneParamItemValue(IALGORITHM_INDEX, 0, ALGORITHM_TYPE_HUBER, 0.0);
    }
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedRadioAlgorithmTukey()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    int iCheck = 0;
    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_TUKEY))->GetCheck();
    if (1 == iCheck)
    {
        SetSceneParamItemValue(IALGORITHM_INDEX, 0, ALGORITHM_TYPE_TUKEY, 0.0);
    }
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedRadioAlgorithmGau()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    int iCheck = 0;
    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_GAU))->GetCheck();
    if (1 == iCheck)
    {
        SetSceneParamItemValue(IALGORITHM_INDEX, 0, ALGORITHM_TYPE_GAUSS, 0.0);
    }
    OnBnClickedBtnCheckOne();
}


void CSceneFindLine::OnBnClickedRadioAlgorithmDrop()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    int iCheck = 0;
    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_DROP))->GetCheck();
    if (1 == iCheck)
    {
        SetSceneParamItemValue(IALGORITHM_INDEX, 0, ALGORITHM_TYPE_DROP, 0.0);
    }
    OnBnClickedBtnCheckOne();
}

void CSceneFindLine::OnBnClickedBtnCheckOne()
{
    VisionFindLine(m_iCurSceneID,m_iCurIndex,m_bIsAutoFollow);
    DispObj(m_winID,m_iCurSceneID);
}

void CSceneFindLine::OnBnClickedBtnUpdateRoi()
{
    // TODO: 在此添加控件通知处理程序代码
	CRect irt=0;
    m_hROI->GetWindowRect(irt);
    ::ClipCursor(&irt);
    DrawROI(m_winID, m_iCurSceneID, m_iCurIndex, LINE_PRO);
    UpdateData(TRUE);	
    ClipCursor(NULL);
}

void CSceneFindLine::SetDisplayWindowID(int winID)
{
    m_winID = winID;
}

void CSceneFindLine::OnBnClickedCheckAutoFollow()
{
    // TODO: 在此添加控件通知处理程序代码
    CButton *pBt = (CButton *)GetDlgItem(IDC_CHECK_AUTO_FOLLOW);
    if (1 == pBt->GetCheck())
    {
        m_bIsAutoFollow = true;
    }
    else
    {
        m_bIsAutoFollow = false;
    }
} 


BOOL CSceneFindLine::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN ) 
    {
        CString str;
        int iValue = 0;
        int index = 0;

        if (pMsg->hwnd==GetDlgItem(IDC_EDIT_EDGE)->m_hWnd)
        {

            GetDlgItem(IDC_EDIT_EDGE)->GetWindowText(str);
            iValue = atoi(str.GetBuffer());
            if (iValue >100)
            {
                return false;
            }
            SetSceneParamItemValue(THRESHOLD_INDEX, 0, iValue, 0.0);
        }
        else if (pMsg->hwnd==GetDlgItem(IDC_EDIT_SMOOTH)->m_hWnd)
        {
            double dSmooth = 0.0;
            GetDlgItem(IDC_EDIT_SMOOTH)->GetWindowText(str);
            dSmooth = atof(str.GetBuffer());
            if (dSmooth >25)
            {
                return false;
            }
            SetSceneParamItemValue(SMOOTH_INDEX, 1, 0, dSmooth);
        }
        else if (pMsg->hwnd==GetDlgItem(IDC_EDIT_ROI)->m_hWnd)
        {
            GetDlgItem(IDC_EDIT_ROI)->GetWindowText(str);
            iValue = atoi(str.GetBuffer());
            if (iValue >200)
            {
                return false;
            }
            SetSceneParamItemValue(ROIWIDTH_INDEX, 0, iValue, 0.0);
        }
        else if (pMsg->hwnd==GetDlgItem(IDC_EDIT_POSNUM)->m_hWnd)
        {

            GetDlgItem(IDC_EDIT_POSNUM)->GetWindowText(str);
            iValue = atoi(str.GetBuffer());
            if (iValue < 0)
            {
                return false;
            }
            SetSceneParamItemValue(IPOSNUM_INDEX, 0, iValue, 0.0);
        }
        OnBnClickedBtnCheckOne();
        return true;
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}


bool CSceneFindLine::SetSceneParamItemValue(int index, int iType, int iValue, double dValue)
{
    bool bRet = false;
    SCENE_PKG config;
    int iPos = 0;
    do 
    { 
        GetSceneParam(m_iCurSceneID, &config);
        iPos = IFindParamData(&config, LINE_PRO, m_iCurIndex);
        if(iPos < 0)
        {
            break;
        }
        switch (iType)
        {
        case 0:
            {
                config.vecBody[iPos][index].iValue = iValue;
            }
            break;
        case 1:
            {
                config.vecBody[iPos][index].dValue = dValue;
            }
            break;
        default:
            break;
        }
        SetSceneParam(&config);
        bRet = true;
    } while (0);
    
    return bRet;
}