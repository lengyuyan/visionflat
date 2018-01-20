#pragma once
//#pragma comment(lib, "\\FGCamer.lib")
#include "FGCamera.h"
#include <list>
using namespace std;
class AVTCam
{
public:		
	AVTCam(void);
	~AVTCam(void);

	void (*m_fcallback)(void* context); // 回调函数定义

	//初始化相机
	bool InitCam(unsigned long Index);
    bool UnInitCam();

	int GetImageWidth();				// 获取图片宽度
	int GetImageHeight();				// 获取图片高度
	
    void SetWindowID();
	// 设置回调函数, Index-相机号，pWnd-目标窗口指针
	void SetCallBack(void(*Callback)(void* context), void* pWnd);	 
	void SetBuffer(BYTE* pBuffer);		// 设置图像缓冲区
	
	bool SingleSnap(bool bDisplay = true);	// 单帧采集 // 叶

	bool StartSnap();	// 连续采集
	bool StopSnap();	// 停止采集	
	
	bool SetTriggerMode(bool Mode);		//设置触发模式	
    bool SetExposureTime(int iValue);	//设置曝光时间	
	bool SetGain(int iValue);			//设置增益

	int GetExposureTime();		//获取曝光时间
	int GetGain();				//获取增益\

	int GetParameInfo(FG_PARAMETER speciPara,unsigned long &info);
	int SetParameInfo(FG_PARAMETER speciPara,int val);

	static int GetGuidList(list<string> &guidList);         //获取GUID列表;

public:
	CFGCamera m_Camera;
	BYTE* m_pImageBuffer;	
	UINT32 m_iImageWidth;
	UINT32 m_iImageHeight;
	
	void* m_pParent;
	int m_iCamIdx;
	bool m_bBusy;		// 连续采集

private:	
	FGNODEINFO m_NodeInfo[3];
	UINT32 m_iResult;	
	UINT32 m_iNodeCount;
	static bool m_b_FGIintFlag;   //初始化標志
	
	HANDLE m_hThread;		// 采集线程
};
