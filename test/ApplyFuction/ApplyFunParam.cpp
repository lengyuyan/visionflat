// ApplyFunParam.cpp : 实现文件
//

#include "stdafx.h"
#include "ApplyFuction.h"
#include "ApplyFunParam.h"
#include "afxdialogex.h"


// ApplyFunParam 对话框

IMPLEMENT_DYNAMIC(ApplyFunParam, CDialogEx)

ApplyFunParam::ApplyFunParam(CWnd* pParent /*=NULL*/)
	: CDialogEx(ApplyFunParam::IDD, pParent)
{

}

ApplyFunParam::~ApplyFunParam()
{
}

void ApplyFunParam::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ApplyFunParam, CDialogEx)
END_MESSAGE_MAP()


// ApplyFunParam 消息处理程序
