#include "common.h"
#include "vision_api.h"
#include "manage.h"
#include "cvision.h"

#include "GolobalError.h"

const char* g_genvisionDesc[] = {
    "服务器IP:",
    "端口:",
    "标识(S/C):",
};

/***********************************************
    *Function: 获取视觉插件信息描述
    *Intput:   pInfo 视觉插件信息结构体
    *Output:   pInfo 视觉插件信息缓冲区
    *Return:   参考EM_ERR_CODE
*********************************************/
EXPORT_C int WINAPI get_vision_info(TVisionPluginInfo* pInfo)
{
	RETURN_CHK(pInfo, ERR_INPUT_PARAM);

	memset(pInfo, 0, sizeof(TVisionPluginInfo));    //这些信息要跟配置文件里的一致
	strcpy_s(pInfo->id, "genvision");               //注意id为插件id
	strcpy_s(pInfo->desc, "通用视觉");
    pInfo->paramCnt = sizeof(g_genvisionDesc) / sizeof(char*);
	for (int i = 0; i < pInfo->paramCnt; i++)
	{
		strcpy_s(pInfo->initParamDesc[i], g_genvisionDesc[i]);
	}
	return 0;
}

/***********************************************
    *Function: 初始化一个视觉对象
    *Intput:   visionID 视觉索引,动态累加,从0开始
               param 初始化参数 最多支持8个参数
    *Output:   NULL
    *Return:   参考EM_ERR_CODE
*********************************************/
EXPORT_C int WINAPI init_vision(unsigned short visionID, TVisionInitParam* param)
{
    return INSTANCE->InitVision(visionID, param);
}

/***********************************************
    *Function: 释放一个视觉对象
    *Intput: visionID 视觉索引
    *Output: 无
    *Return: 参考EM_ERR_CODE
    *********************************************/
EXPORT_C int WINAPI uninit_vision(unsigned short visionID)
{
	return INSTANCE->UnInitVision(visionID);
}

/****************************************************
    *Function: 获取一个视觉对象的连接状态
    *Intput:  visionID 视觉索引
    *Return: EMVision_Status
******************************************************/
EXPORT_C int WINAPI vision_status(unsigned short visionID)
{
	return INSTANCE->GetVisionStatus(visionID);
}

/***********************************************
    *Function: 向指定视觉对象发送自定义命令（万能接口）
    *Intput: visionID 视觉索引
             cmd 指令
		     msg 指令参数 可以为空
    *Output: NULL
    *Return: EM_ERR_CODE
*********************************************/
EXPORT_C int WINAPI send_custom_cmd(unsigned short visionID, const char* cmd, const char* msg/* = 0*/)
{
	return INSTANCE->SendCustomCmd(visionID,cmd, msg);
}

/***********************************************
    *Function: 接收指定视觉对象自定义消息
    *Intput: visionID 视觉索引
             cmd 指令
		     recvMsg，返回内容缓存区,可以为空
             timeout 超时机制 默认不等待
    *Output: recvMsg 命令返回值
    *Return: EMCARD_STATUS 超时返回ERR_TIMEOUT
*********************************************/
EXPORT_C int WINAPI recv_custom_cmd(unsigned short visionID, const char* cmd, char* recvMsg/* = 0*/, int timeout/* = 0*/)
{
	return INSTANCE->RecvCustomCmd(visionID, cmd, recvMsg, timeout);
}

/****************************************************
    *Function: 视觉处理指令，拍照完成返回
    *Intput:  visionID  视觉索引
              secne     场景ID，需根据视觉通讯协议传递
              timeout   超时设置
    *Output:  NULL
    *Return: EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI vision_process(unsigned short visionID, int secne)
{
	return INSTANCE->VisionProcess(visionID,secne);
}

/****************************************************
    *Function: 获取视觉处理结果
    *Intput:  visionID 视觉索引
              secne    场景ID，需根据视觉通讯协议传递
              timeout  超时设置 -1无线等待
    *Output:  pres    返回结果
    *Return: EM_ERR_CODE 超时返回ERR_TIMEOUT
******************************************************/
EXPORT_C int WINAPI recv_process_result(unsigned short visionID, int secne, TVisionResult* pres, int timeout)
{
	return INSTANCE->RecvProcessResult(visionID,secne,pres,timeout);
}

/****************************************************
    *Function: 获取视觉处理图像数据流
    *Intput:  visionID 视觉索引
              secne    场景ID，需根据视觉通讯协议传递
              pdata 可先传NULL获取缓冲区大小，new内存后再次调用
    *Output:  pdata    图像缓冲区
    *Return: 成功返回数据流大小，失败返回错误码 < 0
******************************************************/
EXPORT_C int WINAPI get_process_image(unsigned short visionID, int secne, char* pdata)
{
    return ERR_INPUT_PARAM;
}

/****************************************************
    *Function: 获取视觉处理图像路径
    *Intput:  visionID 视觉索引
              secne    场景ID，需根据视觉通讯协议传递
    *Output:  path    图像路径 MAX_PATH
    *Return: EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI get_process_image_path(unsigned short visionID, int secne, char* path)
{
    return ERR_INPUT_PARAM;
}

/****************************************************
    *Function: 发送视觉标定数据给视觉对象（机械坐标）
    *Intput:  visionID  视觉索引
              secne     标定场景ID，需根据视觉通讯协议传递
              x，y  机械坐标
    *Output:  NULL
    *Return: EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI send_calib_data(unsigned short visionID, int secne, double x, double y)
{
	return INSTANCE->SendCalibData(visionID, secne, x, y);
}

/****************************************************
    *Function: 从视觉对象接收视觉标定数据（像素）,自己做隐射关系
    *Intput:  visionID  视觉索引
              secne     标定场景ID，需根据视觉通讯协议传递
              x，y  机械坐标
    *Output:  NULL
    *Return: EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI recv_calib_data(unsigned short visionID, int secne, double x, double y)
{
    return ERR_INPUT_PARAM;
}

/****************************************************
    *Function: 把从视觉接收到的结果使用标定数据进行转换
    *Intput:  visionID  视觉索引
              secne     标定场景ID，标定时用的哪个场景
    *Output:  NULL
    *Return: EM_ERR_CODE
******************************************************/
EXPORT_C int WINAPI convert_calib_data(unsigned short visionID, int secne, TVisionResult* pres)
{
	return ERR_INPUT_PARAM;
}