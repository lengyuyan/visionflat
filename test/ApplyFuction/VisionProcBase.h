#pragma once

/*
 *这个类是视觉处理各种方法的基类，后续的 视觉处理模块都继承自这个类来建立动态库，而在本程序中，该类为本程序提供基本的处理接口，接口输入参数
 *是图像，以及各种可调参数；输出参数是处理后的结果，包含坐标、区域、数字、等。
 *
 **/
class  VisionProcBase
{
public:
	VisionProcBase(void);
	~VisionProcBase(void);
public:
    //接口函数
	virtual void VisionProc(...);
	virtual VisionProcBase* CreateProcInstance(char* szName);

};

