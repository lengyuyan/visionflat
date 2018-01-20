// PositionConfig.cpp : 实现文件
//

#include "stdafx.h"
#include "PositionFunc.h"
#include "PositionConfig.h"
#include "afxdialogex.h"


// PositionConfig 对话框

IMPLEMENT_DYNAMIC(PositionConfig, CDialogEx)

PositionConfig::PositionConfig(CWnd* pParent /*=NULL*/)
	: CDialogEx(PositionConfig::IDD, pParent)
{

}

PositionConfig::~PositionConfig()
{
}

void PositionConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PositionConfig, CDialogEx)
END_MESSAGE_MAP()


// PositionConfig 消息处理程序
