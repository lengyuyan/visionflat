#pragma once
#include <string>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
using namespace std;

typedef struct serialPortInfo
{	
		string PortName;	//串口名
		DWORD BaudRate; //波特率，指定通信设备的传输速率。这个成员可以是实际波特率值或者下面的常量值之一：
		DWORD fParity; // 指定奇偶校验使能。若此成员为1，允许奇偶校验检查 
		BYTE ByteSize; // 通信字节位数，4—8
		BYTE Parity; //指定奇偶校验方法。此成员可以有下列值：EVENPARITY(2) 偶校验     NOPARITY(0) 无校验 MARKPARITY(3) 标记校验   ODDPARITY(1) 奇校验
		BYTE StopBits; //指定停止位的位数。此成员可以有下列值：ONESTOPBIT 1位停止位   TWOSTOPBITS 2位停止位	ONE5STOPBITS   1.5位停止位
		DWORD ReadIntervalTimeout; //两字符之间最大的延时
		DWORD ReadTotalTimeoutMultiplier; //读取每字符间的超时
		DWORD ReadTotalTimeoutConstant; //一次读取串口数据的固定超时
		DWORD WriteTotalTimeoutMultiplier; //写入每字符间的超时
		DWORD WriteTotalTimeoutConstant; //一次写入串口数据的固定超时
		serialPortInfo()
		{
			PortName = "COM1";
			BaudRate = 9600;
			fParity = 0;
			ByteSize = 8;
			Parity = 0;
			StopBits = 0;
			ReadIntervalTimeout = MAXDWORD;
			ReadTotalTimeoutMultiplier = 0;
			ReadTotalTimeoutConstant = 0;
			WriteTotalTimeoutMultiplier = 10;
			WriteTotalTimeoutConstant = 1000;

		}
} SerialPortInfo;

enum SERIALPORT_ERRORCODE
{
	SECCESS = 0,
	ERROR_CLOSECOM = 1,//设置或创建失败，串口关闭
	GETPARA_FAIL = 2, //获取串口参数失败
	COM_NO_OPEN = 3, //串口没有打开
	WRITEDATA_FIAL = 4,//写数据失败
	READDATA_FAIL = 5,//读数据失败
	PARA_ERROR = 6, //参数错误
};

class SerialPort
{
public:
	SerialPort(void);
	~SerialPort(void);

	//串口初始化
	int Init(SerialPortInfo para);
	//打开串口
	int Open();
	//关闭串口
	int Close();
	//读取数据
	int ReadData(void *pBuffer);
	//发送数据
	int WriteData(void *pBuffer,int len = 0);
	//串口是否打开
	int IsOpen();
	//获取串口参数
	int GetSerialInfo(SerialPortInfo &para);
	//设置串口参数
	int SetSerialInfo(SerialPortInfo para);
private:
	HANDLE m_comer;//串口句柄
	DCB m_dcb;   //串口参数结构
	COMMTIMEOUTS m_timeout; //超时结构

	SerialPortInfo *m_SerailPortInfo;
	bool m_bIsOpen;
};

