// SceneFindCircle.cpp : 实现文件
//

#include "stdafx.h"
#include "SceneManager.h"
#include "SceneFindCircle.h"
#include "afxdialogex.h"
#include "iManagerCtrl.h"


// CSceneFindCircle 对话框
#define    RING_WIDTH_INDEX  5
#define    RING_BEGIN_INDEX  6
#define    RING_END_INDEX    7
#define    SMOOTH_INDEX      8
#define    THRESHOLD_INDEX   9
#define    ROIWIDTH_INDEX    10
#define    IPOSNUM_INDEX     11
#define    ITRANSITION_INDEX 12
#define    IFINDTYPE_INDEX   13
#define    IALGORITHM_INDEX  14
#define    IDIRECTION_INDEX  15

IMPLEMENT_DYNAMIC(CSceneFindCircle, CDialogEx)

CSceneFindCircle::CSceneFindCircle(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSceneFindCircle::IDD, pParent)
    , m_FindCircleBeginAngle(0)
    , m_FindCircleEndAngle(0)
{
    m_iCurSceneID = 0;
    m_iCurIndex = 0;
    m_bIsAutoFollow = false;
}

CSceneFindCircle::~CSceneFindCircle()
{
}

void CSceneFindCircle::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_CIR_BEGIN, m_FindCircleBeginAngle);
    DDV_MinMaxInt(pDX, m_FindCircleBeginAngle, 0, 360);
    DDX_Text(pDX, IDC_EDIT_CIR_END, m_FindCircleEndAngle);
	DDV_MinMaxInt(pDX, m_FindCircleEndAngle, 0, 360);
}


BEGIN_MESSAGE_MAP(CSceneFindCircle, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_SETTING, &CSceneFindCircle::OnBnClickedButtonSetting)
    ON_BN_CLICKED(IDC_BTN_EDGEADD, &CSceneFindCircle::OnBnClickedBtnEdgeadd)
    ON_BN_CLICKED(IDC_BTN_EDGEMINUS, &CSceneFindCircle::OnBnClickedBtnEdgeminus)
    ON_BN_CLICKED(IDC_BTN_SMOOTHADD, &CSceneFindCircle::OnBnClickedBtnSmoothadd)
    ON_BN_CLICKED(IDC_BTN_SMOOTHMINUS, &CSceneFindCircle::OnBnClickedBtnSmoothminus)
    ON_BN_CLICKED(IDC_BTN_ROIADD, &CSceneFindCircle::OnBnClickedBtnRoiadd)
    ON_BN_CLICKED(IDC_BTN_ROIMINUS, &CSceneFindCircle::OnBnClickedBtnRoiminus)
    ON_BN_CLICKED(IDC_BTN_NUMADD, &CSceneFindCircle::OnBnClickedBtnNumadd)
    ON_BN_CLICKED(IDC_BTN_NUMMINUS, &CSceneFindCircle::OnBnClickedBtnNumminus)
    ON_BN_CLICKED(IDC_RADIO_TRANSITION_ALL, &CSceneFindCircle::OnBnClickedRadioTransitionAll)
    ON_BN_CLICKED(IDC_RADIO_TRANSITION_POSITIVE, &CSceneFindCircle::OnBnClickedRadioTransitionPositive)
    ON_BN_CLICKED(IDC_RADIO_TRANSITION_NEGATIVE, &CSceneFindCircle::OnBnClickedRadioTransitionNegative)
    ON_BN_CLICKED(IDC_RADIO_FIND_TYPE_ALL, &CSceneFindCircle::OnBnClickedRadioFindTypeAll)
    ON_BN_CLICKED(IDC_RADIO_FIND_TYPE_FIRST, &CSceneFindCircle::OnBnClickedRadioFindTypeFirst)
    ON_BN_CLICKED(IDC_RADIO_FIND_TYPE_LAST, &CSceneFindCircle::OnBnClickedRadioFindTypeLast)
    ON_BN_CLICKED(IDC_RADIO_DIRECTION_P, &CSceneFindCircle::OnBnClickedRadioDirectionP)
    ON_BN_CLICKED(IDC_RADIO_DIRECTION_N, &CSceneFindCircle::OnBnClickedRadioDirectionN)
	ON_BN_CLICKED(IDC_RADIO_ALGORITHM_ALGEBRAIC, &CSceneFindCircle::OnBnClickedRadioAlgorithmAlgebraic)
	ON_BN_CLICKED(IDC_RADIO_ALGORITHM_AHUBER, &CSceneFindCircle::OnBnClickedRadioAlgorithmAhuber)
	ON_BN_CLICKED(IDC_RADIO_ALGORITHM_ATUKEY, &CSceneFindCircle::OnBnClickedRadioAlgorithmAtukey)
	ON_BN_CLICKED(IDC_RADIO_ALGORITHM_GEOMETRIC, &CSceneFindCircle::OnBnClickedRadioAlgorithmGeometric)
	ON_BN_CLICKED(IDC_RADIO_ALGORITHM_GEOHUBER, &CSceneFindCircle::OnBnClickedRadioAlgorithmGeohuber)
	ON_BN_CLICKED(IDC_RADIO_ALGORITHM_GEOTUKEY, &CSceneFindCircle::OnBnClickedRadioAlgorithmGeotukey)
    ON_BN_CLICKED(IDC_BTN_CHECK_ONE, &CSceneFindCircle::OnBnClickedBtnCheckOne)
    ON_BN_CLICKED(IDC_BTN_UPDATE_ROI, &CSceneFindCircle::OnBnClickedBtnUpdateRoi)
    ON_BN_CLICKED(IDC_CHECK_AUTO_FOLLOW, &CSceneFindCircle::OnBnClickedCheckAutoFollow)
    ON_BN_CLICKED(IDC_BUTTON_SET_BEGIN_ANGLE, &CSceneFindCircle::OnBnClickedButtonSetBeginAngle)
    ON_BN_CLICKED(IDC_BUTTON_SET_END_ANGLE, &CSceneFindCircle::OnBnClickedButtonSetEndAngle)
END_MESSAGE_MAP()


// CSceneFindCircle 消息处理程序

void CSceneFindCircle::UpdateUI(int iSceneID, int index,CWnd *hROI, bool btemp)
{
    char szBuf[BUF_SIZE] = {0};
    SCENE_PKG config;  //从配置文件中读取图片有关roi信息
    int iPos = 0;
    int iTransition = TRANSITION_TYPE_ALL;
    int iFindType = FINDTYPE_ALL;
    int iAlgorithm = ALGORITHM_TYPE_REGRESSION;
    int iDirection = DIRECTION_TYPE_POSITIVE;
    GetDlgItem(IDC_CHECK_AUTO_FOLLOW)->EnableWindow(TRUE);
    if(!btemp)
    {
        GetDlgItem(IDC_CHECK_AUTO_FOLLOW)->EnableWindow(FALSE);
    }

    SetBackgroundColor(BACKGROUND_COLOR_M);

    m_iCurSceneID = iSceneID;
    m_iCurIndex = index;
    m_hROI=hROI;
    GetSceneParam(iSceneID,&config);

    iPos = IFindParamData(&config, CIRCLE_PRO, m_iCurIndex);
    if (iPos >= 0 && iPos < config.vecBody.size() && iPos < config.vecDataType.size())
    {
        /*int iNo = FindSpecTypePos(config.vecBody[iPos], config.vecDataType[iPos],ROI_TYPE) + 2;*/
        sprintf_s(szBuf, "%0.0f", config.vecBody[iPos][RING_WIDTH_INDEX].dValue);
        GetDlgItem(IDC_EDIT_CIRWIDTH)->SetWindowText(szBuf);

        m_FindCircleBeginAngle = (int)config.vecBody[iPos][RING_BEGIN_INDEX].dValue;//方便保存
        sprintf_s(szBuf, "%0.0f", (double)m_FindCircleBeginAngle);
        GetDlgItem(IDC_EDIT_CIR_BEGIN)->SetWindowText(szBuf);

        m_FindCircleEndAngle = (int)config.vecBody[iPos][RING_END_INDEX].dValue; //方便保存
        sprintf_s(szBuf, "%0.0f", (double)m_FindCircleEndAngle);
        GetDlgItem(IDC_EDIT_CIR_END)->SetWindowText(szBuf);

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



    if (iPos > 0)
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
		// 'ALGEBRAIC', 'AHUBER', 'ATUKEY', 'GEOMETRIC', 'GEOHUBER', 'GEOTUKEY' 
        CButton *btAlgebraic = (CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_ALGEBRAIC);
        CButton *btAhuber = (CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_AHUBER);
        CButton *btAtukey = (CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_ATUKEY);
        CButton *btGeometric = (CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_GEOMETRIC);
        CButton *btGeohuber = (CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_GEOHUBER);
        CButton *btGeotukey = (CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_GEOTUKEY);
        switch(iAlgorithm)
        {
        case ALGORITHM_TYPE_ALGEBRAIC:
            btAlgebraic->SetCheck(true);
            btAhuber->SetCheck(false);
            btAtukey->SetCheck(false);
            btGeometric->SetCheck(false);
            btGeohuber->SetCheck(false);
			btGeotukey->SetCheck(false);
            break;
        case ALGORITHM_TYPE_AHUBER:
			btAlgebraic->SetCheck(false);
			btAhuber->SetCheck(true);
			btAtukey->SetCheck(false);
			btGeometric->SetCheck(false);
			btGeohuber->SetCheck(false);
			btGeotukey->SetCheck(false);
            break;
        case ALGORITHM_TYPE_ATUKEY:
			btAlgebraic->SetCheck(false);
			btAhuber->SetCheck(false);
			btAtukey->SetCheck(true);
			btGeometric->SetCheck(false);
			btGeohuber->SetCheck(false);
			btGeotukey->SetCheck(false);
            break;
        case ALGORITHM_TYPE_GEOMETRIC:
			btAlgebraic->SetCheck(false);
			btAhuber->SetCheck(false);
			btAtukey->SetCheck(false);
			btGeometric->SetCheck(true);
			btGeohuber->SetCheck(false);
			btGeotukey->SetCheck(false);
            break;
        case ALGORITHM_TYPE_GEOHUBER:
			btAlgebraic->SetCheck(false);
			btAhuber->SetCheck(false);
			btAtukey->SetCheck(false);
			btGeometric->SetCheck(false);
			btGeohuber->SetCheck(true);
			btGeotukey->SetCheck(false);
            break;
		case ALGORITHM_TYPE_GEOTUKEY:
			btAlgebraic->SetCheck(false);
			btAhuber->SetCheck(false);
			btAtukey->SetCheck(false);
			btGeometric->SetCheck(false);
			btGeohuber->SetCheck(false);
			btGeotukey->SetCheck(true);
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

void CSceneFindCircle::OnBnClickedButtonSetting()
{
    // TODO: 在此添加控件通知处理程序代码
    CString str;
    int iValue = 0;
    GetDlgItem(IDC_EDIT_CIRWIDTH)->GetWindowText(str);
    iValue = atoi(str);
    SetSceneParamItemValue(RING_WIDTH_INDEX, 1, iValue, (double)iValue);
    OnBnClickedBtnCheckOne();
}


void CSceneFindCircle::OnBnClickedBtnEdgeadd()
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
    SetSceneParamItemValue(THRESHOLD_INDEX, 0, iValue, 0.0);
    str.Format("%d",iValue);
    GetDlgItem(IDC_EDIT_EDGE)->SetWindowText(str);
    OnBnClickedBtnCheckOne();
}


void CSceneFindCircle::OnBnClickedBtnEdgeminus()
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
    SetSceneParamItemValue(THRESHOLD_INDEX, 0, iValue, 0.0);
    str.Format("%d",iValue);
    GetDlgItem(IDC_EDIT_EDGE)->SetWindowText(str);
    OnBnClickedBtnCheckOne();
}


void CSceneFindCircle::OnBnClickedBtnSmoothadd()
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


void CSceneFindCircle::OnBnClickedBtnSmoothminus()
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


void CSceneFindCircle::OnBnClickedBtnRoiadd()
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


void CSceneFindCircle::OnBnClickedBtnRoiminus()
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

void CSceneFindCircle::OnBnClickedBtnNumadd()
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


void CSceneFindCircle::OnBnClickedBtnNumminus()
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

void CSceneFindCircle::OnBnClickedRadioTransitionAll()
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


void CSceneFindCircle::OnBnClickedRadioTransitionPositive()
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


void CSceneFindCircle::OnBnClickedRadioTransitionNegative()
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


void CSceneFindCircle::OnBnClickedRadioFindTypeAll()
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


void CSceneFindCircle::OnBnClickedRadioFindTypeFirst()
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


void CSceneFindCircle::OnBnClickedRadioFindTypeLast()
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


void CSceneFindCircle::OnBnClickedRadioDirectionP()
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


void CSceneFindCircle::OnBnClickedRadioDirectionN()
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


//void CSceneFindCircle::OnBnClickedRadioAlgorithmReg()
//{
//    // TODO: 在此添加控件通知处理程序代码
//    CString str;
//    SCENE_CONFIG config;
//    int iCheck = 0;
//    GetSceneParam(m_iCurSceneID, &config);
//    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_REG))->GetCheck();
//    if (1 == iCheck)
//    {
//        config.vecCircleParam[m_iCurIndex].iAlgorithm = ALGORITHM_TYPE_REGRESSION;
//        SetSceneParam(&config);
//    }
//    OnBnClickedBtnCheckOne();
//}
//
//
//void CSceneFindCircle::OnBnClickedRadioAlgorithmHub()
//{
//    // TODO: 在此添加控件通知处理程序代码
//    CString str;
//    SCENE_CONFIG config;
//    int iCheck = 0;
//    GetSceneParam(m_iCurSceneID, &config);
//    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_HUB))->GetCheck();
//    if (1 == iCheck)
//    {
//        config.vecCircleParam[m_iCurIndex].iAlgorithm = ALGORITHM_TYPE_ALGEBRAIC;
//        SetSceneParam(&config);
//    }
//    OnBnClickedBtnCheckOne();
//}
//
//
//void CSceneFindCircle::OnBnClickedRadioAlgorithmTukey()
//{
//    // TODO: 在此添加控件通知处理程序代码
//    CString str;
//    SCENE_CONFIG config;
//    int iCheck = 0;
//    GetSceneParam(m_iCurSceneID, &config);
//    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_TUKEY))->GetCheck();
//    if (1 == iCheck)
//    {
//        config.vecCircleParam[m_iCurIndex].iAlgorithm = ALGORITHM_TYPE_TUKEY;
//        SetSceneParam(&config);
//    }
//    OnBnClickedBtnCheckOne();
//}
//
//
//void CSceneFindCircle::OnBnClickedRadioAlgorithmGau()
//{
//    // TODO: 在此添加控件通知处理程序代码
//    CString str;
//    SCENE_CONFIG config;
//    int iCheck = 0;
//    GetSceneParam(m_iCurSceneID, &config);
//    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_GAU))->GetCheck();
//    if (1 == iCheck)
//    {
//        config.vecCircleParam[m_iCurIndex].iAlgorithm = ALGORITHM_TYPE_GAUSS;
//        SetSceneParam(&config);
//    }
//    OnBnClickedBtnCheckOne();
//}
//
//
//void CSceneFindCircle::OnBnClickedRadioAlgorithmDrop()
//{
//    // TODO: 在此添加控件通知处理程序代码
//    CString str;
//    SCENE_CONFIG config;
//    int iCheck = 0;
//    GetSceneParam(m_iCurSceneID, &config);
//    iCheck = ((CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_DROP))->GetCheck();
//    if (1 == iCheck)
//    {
//        config.vecCircleParam[m_iCurIndex].iAlgorithm = ALGORITHM_TYPE_DROP;
//        SetSceneParam(&config);
//    }
//    OnBnClickedBtnCheckOne();
//}

void CSceneFindCircle::OnBnClickedBtnCheckOne()
{
    VisionFindCircle(m_iCurSceneID,m_iCurIndex, m_bIsAutoFollow);
    DispObj(m_winID,m_iCurSceneID);
}

void CSceneFindCircle::OnBnClickedBtnUpdateRoi()
{
    // TODO: 在此添加控件通知处理程序代码
    CRect rt;
    m_hROI->GetWindowRect(rt);
    ::ClipCursor(&rt);
    DrawROI(m_winID, m_iCurSceneID, m_iCurIndex, CIRCLE_PRO);
    UpdateData(TRUE);	
    ClipCursor(NULL);
}

void CSceneFindCircle::SetDisplayWindowID(int winID)
{
    m_winID = winID;
}

void CSceneFindCircle::OnBnClickedCheckAutoFollow()
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


BOOL CSceneFindCircle::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类

    if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN ) 
    {
        CString str;
        int iValue = 0;

        if (pMsg->hwnd==GetDlgItem(IDC_EDIT_EDGE)->m_hWnd)
        {

            GetDlgItem(IDC_EDIT_EDGE)->GetWindowText(str);
            iValue = atoi(str.GetBuffer());
            if (iValue >100 )
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
            if (iValue >200 )
            {
                return false;
            }
            SetSceneParamItemValue(ROIWIDTH_INDEX, 0, iValue, 0.0);
        }
        else if (pMsg->hwnd==GetDlgItem(IDC_EDIT_POSNUM)->m_hWnd)
        {

            GetDlgItem(IDC_EDIT_POSNUM)->GetWindowText(str);
            iValue = atoi(str.GetBuffer());
            if (iValue < 0 )
            {
                return false;
            }
            SetSceneParamItemValue(IPOSNUM_INDEX, 0, iValue, 0.0);
        }
        else if (pMsg->hwnd==GetDlgItem(IDC_EDIT_CIRWIDTH)->m_hWnd)
        {
            GetDlgItem(IDC_EDIT_CIRWIDTH)->GetWindowText(str);
            iValue = atoi(str);
            SetSceneParamItemValue(RING_WIDTH_INDEX, 0, iValue, (double)iValue);
        }
        else if (pMsg->hwnd==GetDlgItem(IDC_EDIT_CIR_BEGIN)->m_hWnd)
        {
            int oldValue = m_FindCircleBeginAngle;
            if (UpdateData(TRUE))
            {
                SetSceneParamItemValue(RING_BEGIN_INDEX, 1, m_FindCircleBeginAngle, (double)m_FindCircleBeginAngle);
            }
            else
            {
                m_FindCircleBeginAngle = oldValue;
                UpdateData(FALSE);
            }
        }
        else if (pMsg->hwnd==GetDlgItem(IDC_EDIT_CIR_END)->m_hWnd)
        {
            int oldValue = m_FindCircleEndAngle;
            if (UpdateData(TRUE))
            {
                SetSceneParamItemValue(RING_BEGIN_INDEX, 1, m_FindCircleEndAngle, (double)m_FindCircleEndAngle);
            }
            else
            {
                m_FindCircleEndAngle = oldValue;
                UpdateData(FALSE);
            }
        }
        OnBnClickedBtnCheckOne();
        return true;
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}


void CSceneFindCircle::OnBnClickedRadioAlgorithmAlgebraic()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	int iCheck = 0;
	iCheck = ((CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_ALGEBRAIC))->GetCheck();
	if (1 == iCheck)
    {
        SetSceneParamItemValue(IALGORITHM_INDEX, 0, ALGORITHM_TYPE_ALGEBRAIC, 0.0);
	}
	OnBnClickedBtnCheckOne();
}


void CSceneFindCircle::OnBnClickedRadioAlgorithmAhuber()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	int iCheck = 0;
	iCheck = ((CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_AHUBER))->GetCheck();
	if (1 == iCheck)
    {
        SetSceneParamItemValue(IALGORITHM_INDEX, 0, ALGORITHM_TYPE_AHUBER, 0.0);
	}
	OnBnClickedBtnCheckOne();
}


void CSceneFindCircle::OnBnClickedRadioAlgorithmAtukey()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	int iCheck = 0;
	iCheck = ((CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_ATUKEY))->GetCheck();
	if (1 == iCheck)
    {
        SetSceneParamItemValue(IALGORITHM_INDEX, 0, ALGORITHM_TYPE_ATUKEY, 0.0);
	}
	OnBnClickedBtnCheckOne();
}


void CSceneFindCircle::OnBnClickedRadioAlgorithmGeometric()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	int iCheck = 0;
	iCheck = ((CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_GEOMETRIC))->GetCheck();
	if (1 == iCheck)
    {
        SetSceneParamItemValue(IALGORITHM_INDEX, 0, ALGORITHM_TYPE_GEOMETRIC, 0.0);
	}
	OnBnClickedBtnCheckOne();
}


void CSceneFindCircle::OnBnClickedRadioAlgorithmGeohuber()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	int iCheck = 0;
	iCheck = ((CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_GEOHUBER))->GetCheck();
	if (1 == iCheck)
    {
        SetSceneParamItemValue(IALGORITHM_INDEX, 0, ALGORITHM_TYPE_GEOHUBER, 0.0);
	}
	OnBnClickedBtnCheckOne();
}


void CSceneFindCircle::OnBnClickedRadioAlgorithmGeotukey()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	int iCheck = 0;
	iCheck = ((CButton *)GetDlgItem(IDC_RADIO_ALGORITHM_GEOTUKEY))->GetCheck();
	if (1 == iCheck)
	{
        SetSceneParamItemValue(IALGORITHM_INDEX, 0, ALGORITHM_TYPE_GEOTUKEY, 0.0);
	}
	OnBnClickedBtnCheckOne();
}

bool CSceneFindCircle::SetSceneParamItemValue(int index, int iType, int iValue, double dValue)
{
    bool bRet = false;
    SCENE_PKG config;
    int iPos = 0;
    do 
    { 
        GetSceneParam(m_iCurSceneID, &config);
        iPos = IFindParamData(&config, CIRCLE_PRO, m_iCurIndex);
        if(iPos < 0 || iPos >= config.vecBody.size())
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

void CSceneFindCircle::OnBnClickedButtonSetBeginAngle()
{
    // TODO: 在此添加控件通知处理程序代码
    int oldValue = m_FindCircleBeginAngle;
    if (UpdateData(TRUE))
    {
        SetSceneParamItemValue(RING_BEGIN_INDEX, 1, m_FindCircleBeginAngle, (double)m_FindCircleBeginAngle);
        OnBnClickedBtnCheckOne();
    }
    else
    {
        m_FindCircleBeginAngle = oldValue;
        UpdateData(FALSE);
    }
}


void CSceneFindCircle::OnBnClickedButtonSetEndAngle()
{
    // TODO: 在此添加控件通知处理程序代码
    int oldValue = m_FindCircleEndAngle;
    if (UpdateData(TRUE))
    {
        SetSceneParamItemValue(RING_END_INDEX, 1, m_FindCircleEndAngle, (double)m_FindCircleEndAngle);
        OnBnClickedBtnCheckOne();
    }
    else
    {
        m_FindCircleEndAngle = oldValue;
        UpdateData(FALSE);
    }
}
