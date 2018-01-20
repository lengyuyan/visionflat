/*********************************************************************
    *文件说明: 视觉插件接口，所有视觉兼容插件必须满足此接口文档
    *作者: 罗晶
    *日期: 2017/02/07
    *修改日志:  2017/02/07 CW4945 创建

    ***注意事项***
    1. 加载插件时会检测接口，若接口检测不通过，则插件加载会失败
    2. 所有接口都必须是非阻塞实现，除非包含延时参数
    3. 初始化参数描述必须从get_vision_info接口返回，数量和数组大小匹配
    4. 索引从上层调用init_vision时会自动动态累加，第一张卡为0，以此类推.
***********************************************************************/
#pragma once
#ifndef _INCLUDE_VISION_API_H
#define _INCLUDE_VISION_API_H


#define VISION_API_VERSION "vision_api_1.1.1.20170207_beta"

#define EXPORT_C extern "C"
#define WINAPI __stdcall

#define VISION_MAX_NAME     32      //名称最大长度
#define VISION_ID_LEN       16      //插件ID长度
#define VISION_ARRAY_CNT    8       //数组数量
#define VISION_RES_CNT      16      //单场景最大支持结果数量

enum EMVision_Status
{//视觉对象状态
    EMVISION_NOINIT = 0,    //未初始化
    EMVISION_ERROR,         //异常
    EMVISION_NORMAL,        //OK
};

struct TVisionInitParam
{//初始化参数结构体
    char data[VISION_ARRAY_CNT][VISION_MAX_NAME];   //参数
};

struct TVisionPluginInfo
{//视觉插件信息
    char id[VISION_ID_LEN];         //插件类型id -- 作为视觉类型标识符,不能重复
    char desc[VISION_MAX_NAME];     //硬件描述
    char initParamDesc[VISION_ARRAY_CNT][VISION_MAX_NAME];  //初始化参数描述
    unsigned short  paramCnt;       //参数个数
    char reserved[6];               //保证8位对齐
};

struct TVisionResult
{//视觉结果结构体
    int     res;        //结果 0 - ok
    int     resCnt;     //结果个数
    double  val[VISION_RES_CNT];    //结果
};

#ifndef VISION_FUN_IMPORT    //防止引用函数声明

/***********************************************
    *Function: 获取视觉插件信息描述
    *Intput:   pInfo 视觉插件信息结构体
    *Output:   pInfo 视觉插件信息缓冲区
    *Return:   参考EM_ERR_CODE
*********************************************/
EXPORT_C int WINAPI get_vision_info(TVisionPluginInfo* pInfo);

/***********************************************
    *Function: 初始化一个视觉对象
    *Intput:   visionID 视觉索引,动态累加,从0开始
               param 初始化参数 最多支持8个参数
    *Output:   NULL
    *Return:   参考EM_ERR_CODE
*********************************************/
EXPORT_C int WINAPI init_vision(unsigned short visionID, TVisionInitParam* param);

/***********************************************
    *Function: 释放一个视觉对象
    *Intput: visionID 视觉索引
    *Output: 无
    *Return: 参考EM_ERR_CODE
    *********************************************/
EXPORT_C int WINAPI uninit_vision(unsigned short visionID);

/****************************************************
    *Function: 获取一个视觉对象的连接状态
    *Intput:  visionID 视觉索引
    *Return: EMVision_Status
******************************************************/
EXPORT_C int WINAPI vision_status(unsigned short visionID);

/***********************************************
    *Function: 向指定视觉对象发送自定义命令（万能接口）
    *Intput: visionID 视觉索引
             cmd 指令
		     msg 指令参数 可以为空
    *Output: NULL
    *Return: EM_ERR_CODE
*********************************************/
EXPORT_C int WINAPI send_custom_cmd(unsigned short visionID, const char* cmd, const char* msg/* = 0*/);

/***********************************************
    *Function: 接收指定视觉对象自定义消息
    *Intput: visionID 视觉索引
             cmd 指令
		     recvMsg，返回内容缓存区,可以为空
             timeout 超时机制 默认不等待
    *Output: recvMsg 命令返回值
    *Return: EMCARD_STATUS 超时返回ERR_TIMEOUT
*********************************************/
EXPORT_C int WINAPI recv_custom_cmd(unsigned short visionID, const char* cmd, char* recvMsg/* = 0*/, int timeout/* = 0*/);

/****************************************************
    *Function: 视觉处理指令，拍照完成返回
    *Intput:  visionID  视觉索引
              secne     场景ID，需根据视觉通讯协议传递
              timeout   超时设置
    *Output:  NULL
    *Return: EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI vision_process(unsigned short visionID, int secne);

/****************************************************
    *Function: 获取视觉处理结果
    *Intput:  visionID 视觉索引
              secne    场景ID，需根据视觉通讯协议传递
              timeout  超时设置 -1无线等待
    *Output:  pres    返回结果
    *Return: EM_ERR_CODE 超时返回ERR_TIMEOUT
******************************************************/
EXPORT_C int WINAPI recv_process_result(unsigned short visionID, int secne, TVisionResult* pres, int timeout);

/****************************************************
    *Function: 获取视觉处理图像数据流
    *Intput:  visionID 视觉索引
              secne    场景ID，需根据视觉通讯协议传递
              pdata 可先传NULL获取缓冲区大小，new内存后再次调用
    *Output:  pdata    图像缓冲区
    *Return: 成功返回数据流大小，失败返回错误码 < 0
******************************************************/
EXPORT_C int WINAPI get_process_image(unsigned short visionID, int secne, char* pdata);

/****************************************************
    *Function: 获取视觉处理图像路径
    *Intput:  visionID 视觉索引
              secne    场景ID，需根据视觉通讯协议传递
    *Output:  path    图像路径 MAX_PATH
    *Return: EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI get_process_image_path(unsigned short visionID, int secne, char* path);

/****************************************************
    *Function: 发送视觉标定数据给视觉对象（机械坐标）
    *Intput:  visionID  视觉索引
              secne     标定场景ID，需根据视觉通讯协议传递
              x，y  机械坐标
    *Output:  NULL
    *Return: EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI send_calib_data(unsigned short visionID, int secne, double x, double y);

/****************************************************
    *Function: 从视觉对象接收视觉标定数据（像素）,自己做隐射关系
    *Intput:  visionID  视觉索引
              secne     标定场景ID，需根据视觉通讯协议传递
              x，y  机械坐标
    *Output:  NULL
    *Return: EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI recv_calib_data(unsigned short visionID, int secne, double x, double y);

/****************************************************
    *Function: 把从视觉接收到的结果使用标定数据进行转换
    *Intput:  visionID  视觉索引
              secne     标定场景ID，标定时用的哪个场景
    *Output:  NULL
    *Return: EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI convert_calib_data(unsigned short visionID, int secne, TVisionResult* pres);

#endif
#endif      //防止重复包含