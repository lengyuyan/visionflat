#pragma once
#ifndef FLATFISH_INCLUDE_GLOBALERROR_H
#define FLATFISH_INCLUDE_GLOBALERROR_H

//错误代码
enum EM_ERR_CODE
{
    ERR_INPUT_PARAM = -1000,        //入参为空或者不合法
    ERR_PARAM_FORMAT,               //参数格式错误
    ERR_VERSION,                    //版本错误
    ERR_TIMEOUT,                    //等待超时
    ERR_NO_INIT,                    //未初始化
    ERR_REPEAT_INIT,                //重复初始化
    ERR_NO_CONNECT,                 //未连接
    ERR_NO_REG,                     //未注册
    ERR_REPEAT_REG,                 //重复注册
    ERR_DATA_OVERLIMIT,             //数据溢出
    ERR_INVALID_ID,                 //此ID无效
    ERR_INVALID_HANDLE,             //无效句柄
    ERR_INVALID_NAME,               //无效名称
    ERR_NAME_EXIST,                 //该名称已经存在
    ERR_NOREADY,                    //不在就绪状态,不能接受该指令

    //File error
    ERR_CREATE_FILE,                //创建文件失败
    ERR_OPEN_FILE,                  //打开文件失败
    ERR_WRITE_FILE,                 //写入文件失败
    ERR_READ_FILE,                  //读取文件失败
    ERR_DEL_FILE,                   //删除文件失败
    ERR_GET_ATTRIBUTES,             //获取文件属性失败
    ERR_FILE_FORMAT,                //文件格式错误

    //commport error
    ERR_OPEN_COM,                   //打开串口失败
    ERR_COM_WRITE,                  //写入串口错误
    ERR_COM_READ,                   //读取串口错误
    ERR_GET_COMSTATE,               //获取串口参数错误
    ERR_SET_COMSTATE,               //设置串口参数错误
    ERR_CLEAR_COMSTACK,             //清除缓冲区失败

    //sysapi
    ERR_INIT_NET,                   //加载网络协议栈失败
    ERR_ABANDONED,                  //独占模式，无法访问
    ERR_CLIENT_NORESPONSE,          //客户端无响应
    ERR_CREATE_SOCKET,              //创建socket失败
    ERR_BIND_SOCKET,                //绑定端口错误
    ERR_OPERATOR_THREAD,            //操作线程失败
    ERR_OPERATOR_EVENT,             //操作事件失败
    ERR_ABANDONED_EVENT,            //事件被锁定, 无法操作
    ERR_OPERATOR_MUTEX,             //操作互斥锁错误
    ERR_LOAD_DLL,                   //加载dll失败
    ERR_LOAD_DLLFUN,                //加载dll某个接口失败

    //skt
    ERR_INVALID_IP,                 //无效IP
    ERR_INVALID_PORT,               //无效端口
    ERR_LISTEN_SKT,                 //socket监听失败
    ERR_ACCEPT_SKT,                 //接受连接失败
    ERR_CONNECT_SKT,                //连接服务端失败
    ERR_RECV_MSG,                   //接收消息出错
    ERR_SEND_MSG,                   //发送消息出错
    ERR_GET_SKTINFO,                //获取socket信息(ip,port)

    //user
    ERR_NO_LOGIN,                   //未登录
    ERR_NO_LIMIT,                   //无权限
    ERR_INVALID_PWD,                //密码无效
    ERR_LIMIT_OVER,                 //权限设置只有32位，超限了

    //motor err
    ERR_NOINIT_CARD,                //未初始化控制卡
    ERR_MOTOR_API,                  //控制卡接口命令返回错误
    ERR_NO_CARD,                    //控制卡不存在，或者无卡
    ERR_CARD_EXIST,                 //控制卡已经存在
    ERR_LOAD_CFG,                   //加载卡配置文件失败
    ERR_INIT_MOTOR,                 //初始化控制卡失败
    ERR_INVALID_AXIS,               //无效轴
    ERR_AXIS_HOME,                  //轴回原失败
    ERR_AXIS_MOVING,                //轴正在运动中
    ERR_GET_CURPOS,                 //获取当前位置失败
    ERR_CARD_RESET,                 //卡复位失败
    ERR_AXIS_RESET,                 //轴复位失败
    ERR_AXIS_TRAP,                  //轴点位运动失败
    ERR_SET_MOVETYPE,               //设置运动模式失败
    ERR_SET_CRD,                    //建立坐标系失败
    ERR_GET_CRD,                    //获取坐标系失败
    ERR_ARC_MOVE,                   //圆弧插补失败
    ERR_CRD_MOVE,                   //坐标系运动失败
    ERR_IO_TYPE,                    //io类型错误
    ERR_WRITE_IO,                   //写入io失败
    ERR_OCCUPY_IO,                  //IO被占用
    ERR_OCCUPY_AXIS,                //轴被占用

    //no init library
    ERR_NOINIT_COMM,                //未初始化comm库
    ERR_NOINIT_NET,                 //未初始化网络协议栈
    ERR_NOINIT_SKT,                 //未初始化skt库
    ERR_NOINIT_USER,                //未初始化user库
    ERR_NOINIT_CONTROL,             //未初始化control库
    ERR_NOINIT_OPS,                 //ops未初始化
    ERR_NOINIT_VISION,              //未初始化视觉库
    ERR_START_SAFEMONITOR,          //启动安全检测失败
    ERR_SAFE_CHK,                   //安全空间限制

    ERR_NO_HOMEP,                   //无原点信号
    ERR_NO_LIMITN,                  //无负限位信号
    ERR_NO_LIMITP,                  //无正限位信号

    ERR_VISION_RES,                 //视觉处理错误
    ERR_FORMAT_MSG,                 //消息格式错误
    ERR_NETCALL,                    //网络调用出错

    ERR_EM_MAX,                     //最大值,不是错误码
    RETURN_OK = 0,                  //正常返回
};

#define ERR_NO_NAME ERR_INVALID_NAME

#endif  //防止重复包含