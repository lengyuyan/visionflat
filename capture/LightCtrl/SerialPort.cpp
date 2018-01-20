#include "StdAfx.h"
#include "SerialPort.h"
#define MAX_DATA_SIZE_B 4096

SerialPort::SerialPort(void)
{
	m_SerailPortInfo = new SerialPortInfo();
	m_bIsOpen = false;
}


SerialPort::~SerialPort(void)
{
	//Close();
    if (NULL != m_SerailPortInfo)
    {
        delete m_SerailPortInfo;
        m_SerailPortInfo = NULL;
    }
}

//串口初始化
int SerialPort::Init(SerialPortInfo para)
{
	m_SerailPortInfo->BaudRate = para.BaudRate;
	m_SerailPortInfo->ByteSize = para.ByteSize;
	m_SerailPortInfo->fParity = para.fParity;
	m_SerailPortInfo->Parity = para.Parity;
	m_SerailPortInfo->StopBits = para.StopBits;
	m_SerailPortInfo->PortName = para.PortName;
	m_SerailPortInfo->ReadIntervalTimeout = para.ReadIntervalTimeout;
	m_SerailPortInfo->ReadTotalTimeoutConstant = para.ReadTotalTimeoutConstant;
	m_SerailPortInfo->ReadTotalTimeoutMultiplier = para.ReadTotalTimeoutMultiplier;
	m_SerailPortInfo->WriteTotalTimeoutMultiplier = para.WriteTotalTimeoutMultiplier;
	m_SerailPortInfo->WriteTotalTimeoutConstant = para.WriteTotalTimeoutConstant;
	return 0;
}
//打开串口
int SerialPort::Open()
{
	/*m_comer为文件句柄。GENERIC_READ | GENERIC_WRITE指定可以对串口进行读写操作。
	第三个参数0表示串口为独占打开。OPEN_EXISTING表示当指定串口不存在时，程序将返回失败。 
	FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED则表示文件属性。当打开串口时，必须指定 FILE_FLAG_OVERLAPPED，
	它表示文件或设备不会维护访问指针，则在读写时，必须使用OVERLAPPED 结构指定访问的文件偏移量?*/
	
	m_comer = ::CreateFile( (LPCTSTR)m_SerailPortInfo->PortName.c_str(),
		GENERIC_READ | GENERIC_WRITE, 
		0, 
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, 
		NULL );

	if (m_comer == INVALID_HANDLE_VALUE)
	{
		return ERROR_CLOSECOM;
	}
	m_bIsOpen = true;
	SetSerialInfo(*m_SerailPortInfo);
	return 0;
}

//串口是否打开
int SerialPort::IsOpen()
{
	return m_bIsOpen;
}

//关闭串口
int SerialPort::Close()
{
	m_bIsOpen = false;
	if ( INVALID_HANDLE_VALUE != m_comer )
	{
		::PurgeComm(m_comer, PURGE_TXABORT | PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);
		::CloseHandle(m_comer );
	}
	return 0;
}
//读取数据
int SerialPort::ReadData(void *pBuffer)
{
	LPDWORD com_err =0;
	LPCOMSTAT comstat = 0;
	::ClearCommError(m_comer,com_err,comstat);
	if (com_err>0)
	{
		::PurgeComm(m_comer,PURGE_RXABORT|PURGE_RXCLEAR);
	}

	DWORD dwBytesRead = 0;
	DWORD buffersize = (DWORD)strlen((const char*)pBuffer);
	if( FALSE == ::ReadFile(m_comer, pBuffer, buffersize, &dwBytesRead, NULL) )
	{		
		//CloseHandle(m_comer);
		//m_bIsOpen = false;
		return READDATA_FAIL;
	}
	return 0;
}
//发送数据
int SerialPort::WriteData( void *pBuffer,int len /*= 0*/ )
{
	if (NULL == pBuffer)
	{
		return PARA_ERROR;
	}
	LPDWORD com_err = 0;
	LPCOMSTAT comstat = 0;
	::ClearCommError(m_comer,com_err,comstat);
	if (com_err>0)
	{
		::PurgeComm(m_comer,PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	}

	DWORD dwBytesToWrite =(DWORD)strlen((const char*)pBuffer);
	if (len > 0)
	{
		dwBytesToWrite = len;
	}
	DWORD dwBytesWritten = 0;
	while (dwBytesWritten < dwBytesToWrite)
	{
		if( FALSE == ::WriteFile(m_comer,					// open file handle
			((char*)pBuffer+dwBytesWritten) ,			// start of data to write
			dwBytesToWrite - dwBytesWritten,				// number of bytes to write
			&dwBytesWritten,								// number of bytes that were written
			NULL)											// no overlapped structure
			)
		{			
			int error = GetLastError();
			return WRITEDATA_FIAL;
		}
	}
	return 0;
}

//获取串口参数
int SerialPort::GetSerialInfo(SerialPortInfo &para)
{
	if (!m_bIsOpen)
	{
		return COM_NO_OPEN;
	}

	if (::GetCommState(m_comer,&m_dcb)==0)
	{
		return GETPARA_FAIL;
	}

	if (::GetCommTimeouts(m_comer,&m_timeout)==0)
	{
		return GETPARA_FAIL;
	}

	para.BaudRate = m_dcb.BaudRate;
	para.fParity = m_dcb.fParity;
	para.ByteSize = m_dcb.ByteSize;	
	para.Parity = m_dcb.Parity;
	para.StopBits = m_dcb.StopBits;
	para.ReadIntervalTimeout = m_timeout.ReadIntervalTimeout;
	para.ReadTotalTimeoutMultiplier = m_timeout.ReadTotalTimeoutMultiplier;
	para.ReadTotalTimeoutConstant = m_timeout.ReadTotalTimeoutConstant;
	para.WriteTotalTimeoutMultiplier = m_timeout.WriteTotalTimeoutMultiplier;
	para.WriteTotalTimeoutConstant = m_timeout.WriteTotalTimeoutConstant;

	return 0;
}

//设置串口参数
int SerialPort::SetSerialInfo(SerialPortInfo para)
{
	if (!m_bIsOpen)
	{
		return COM_NO_OPEN;
	}
	if (::GetCommState(m_comer,&m_dcb)==0)
	{
		return GETPARA_FAIL;
	}

	if (::GetCommTimeouts(m_comer,&m_timeout)==0)
	{
		return GETPARA_FAIL;
	}

	m_dcb.BaudRate = para.BaudRate;
	m_dcb.fParity = para.fParity;
	m_dcb.ByteSize = para.ByteSize;	
	m_dcb.Parity = para.Parity;
	m_dcb.StopBits = para.StopBits;
	m_timeout.ReadIntervalTimeout = para.ReadIntervalTimeout;
	m_timeout.ReadTotalTimeoutMultiplier = para.ReadTotalTimeoutMultiplier;
	m_timeout.ReadTotalTimeoutConstant = para.ReadTotalTimeoutConstant;
	m_timeout.WriteTotalTimeoutMultiplier = para.WriteTotalTimeoutMultiplier;
	m_timeout.WriteTotalTimeoutConstant = para.WriteTotalTimeoutConstant;


	if (!(::SetCommState(m_comer,&m_dcb)))
	{
		DWORD  error = GetLastError();
		return ERROR_CLOSECOM;
	}

	if (!(::SetCommTimeouts(m_comer,&m_timeout)))
	{
		return ERROR_CLOSECOM;
	}

	if (!::SetupComm(m_comer, MAX_DATA_SIZE_B, MAX_DATA_SIZE_B))	
	{
		return ERROR_CLOSECOM;
	}
	return 0;
}