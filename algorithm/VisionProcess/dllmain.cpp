// dllmain.cpp : 定义 DLL 应用程序的入口点。
//#include "stdafx.h"
//#include "VisionServer.h"
//
//
///**********************************************************************
//函数      : InitVision
//功能      : 初始化处理
//输入参数  : N/A
//输出参数  : N/A
//返回值    : 成功或失败
//备注      : 
//**********************************************************************/
//bool InitVision(void)
//{
//    return true;
//}
//
///**********************************************************************
//函数      : OpenWindow
//功能      : 绑定窗口
//输入参数  : winID 窗口ID
//            x,y,width,hight, 窗口位置及宽高
//输出参数  : halconWinID halcon窗口ID
//返回值    : 成功或失败
//备注      : 
//**********************************************************************/
//bool OpenWindow(long winID, int x, int y, int width, int hight, long &halconWinID)
//{
//    open_window(y, x,width, hight, winID, "visible", "", &halconWinID);
//    return true;
//}
//
///**********************************************************************
//函数      : OpenWindow
//功能      : 关闭窗口
//输入参数  : halconWinID halcon窗口ID
//输出参数  : N/A
//返回值    : 成功或失败
//备注      : 
//**********************************************************************/
//bool /*__stdcall*/ CloseHalconWindow(long halconWinID)
//{
//    close_window(halconWinID);
//    return true;
//}
///**********************************************************************
//函数      : ReadImageFile
//功能      : 绑定窗口
//输入参数  : pFileName 图片路径
//            sceneID   场景ID
//输出参数  : N/A
//返回值    : 成功或失败
//备注      : 
//**********************************************************************/
//bool ReadImage(const char *pFileName, int sceneID, int index)
//{
//    CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
//    pVision->ReadImage(pFileName, index);
//    
//    return true;
//}
//
///**********************************************************************
//函数      : DispImage
//功能      : 绑定窗口
//输入参数  : iWinIndex 显示的目标窗口
//            sceneID   场景ID
//输出参数  : N/A
//返回值    : 成功或失败
//备注      : 
//**********************************************************************/
//bool DispVisionImage(int iWinIndex, int sceneID)
//{
//    CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
//    pVision->DispImage(iWinIndex);
//    return true;
//}
//
//bool DispObj(int iWinIndex, int sceneID, bool bIsShowTmp)
//{
//    CVisionProcess *pVision = CVisionServer::GetInstance(sceneID);
//    pVision->DispObj(iWinIndex, bIsShowTmp);
//
//    return true;
//}
///**********************************************************************
//函数      : VisionProcess
//功能      : 初始化处理
//输入参数  : N/A
//输出参数  : N/A
//返回值    : 成功或失败
//备注      : 
//**********************************************************************/
//bool VisionProcess(void)
//{
//    return true;
//}
//
// 