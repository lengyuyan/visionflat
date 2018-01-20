#include "stdafx.h"
#include "GetOtherInfo.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
BOOL WinNTHDSerialNumAsScsiRead( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen )
{
    BOOL bInfoLoaded = FALSE;
    
    for( int iController = 0; iController < 2; ++ iController )
    {
        HANDLE hScsiDriveIOCTL = 0;
        char   szDriveName[256];
        
        //  Try to get a handle to PhysicalDrive IOCTL, report failure
        //  and exit if can't.
        sprintf( szDriveName, "\\\\.\\Scsi%d:", iController );

        //  Windows NT, Windows 2000, any rights should do
        hScsiDriveIOCTL = CreateFile( szDriveName,
            GENERIC_READ | GENERIC_WRITE, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, 0, NULL);

        // if (hScsiDriveIOCTL == INVALID_HANDLE_VALUE)
        //    printf ("Unable to open SCSI controller %d, error code: 0x%lX\n",
        //            controller, GetLastError ());
        
        if( hScsiDriveIOCTL != INVALID_HANDLE_VALUE )
        {
            int iDrive = 0;
            for( iDrive = 0; iDrive < 2; ++ iDrive )
            {
                char szBuffer[sizeof( SRB_IO_CONTROL ) + SENDIDLENGTH] = { 0 };

                SRB_IO_CONTROL* p = ( SRB_IO_CONTROL* )szBuffer;
                SENDCMDINPARAMS* pin = ( SENDCMDINPARAMS* )( szBuffer + sizeof( SRB_IO_CONTROL ) );
                DWORD dwResult;

                p->HeaderLength = sizeof( SRB_IO_CONTROL );
                p->Timeout = 10000;
                p->Length = SENDIDLENGTH;
                p->ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
                strncpy( ( char* )p->Signature, "SCSIDISK", 8 );

                pin->irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
                pin->bDriveNumber = iDrive;
                
                if( DeviceIoControl( hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
                    szBuffer,
                    sizeof( SRB_IO_CONTROL ) + sizeof( SENDCMDINPARAMS ) - 1,
                    szBuffer,
                    sizeof( SRB_IO_CONTROL ) + SENDIDLENGTH,
                    &dwResult, NULL ) )
                {
                    SENDCMDOUTPARAMS* pOut = ( SENDCMDOUTPARAMS* )( szBuffer + sizeof( SRB_IO_CONTROL ) );
                    IDSECTOR* pId = ( IDSECTOR* )( pOut->bBuffer );
                    if( pId->sModelNumber[0] )
                    {
                        if( * puSerialLen + 20U <= uMaxSerialLen )
                        {
                            // 序列号
                            CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )pId ) + 10, 20 );

                            // Cut off the trailing blanks
                            UINT i = 0;
                            for( UINT i = 20; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; -- i )
                            {
                            }
                            * puSerialLen += i;

                            // 型号
                            CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )pId ) + 27, 40 );
                            // Cut off the trailing blanks
                            for( i = 40; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; -- i )
                            {
                                * puSerialLen += i;
                            }

                            bInfoLoaded = TRUE;
                        }
                        else
                        {
                            ::CloseHandle( hScsiDriveIOCTL );
                            return bInfoLoaded;
                        }
                    }
                }
            }
            ::CloseHandle( hScsiDriveIOCTL );
        }
    }
    return bInfoLoaded;
}


BOOL DoIdentify( HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
                 PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
                 PDWORD lpcbBytesReturned )
{
    // Set up data structures for IDENTIFY command.
    pSCIP->cBufferSize                  = IDENTIFY_BUFFER_SIZE;
    pSCIP->irDriveRegs.bFeaturesReg     = 0;
    pSCIP->irDriveRegs.bSectorCountReg  = 1;
    pSCIP->irDriveRegs.bSectorNumberReg = 1;
    pSCIP->irDriveRegs.bCylLowReg       = 0;
    pSCIP->irDriveRegs.bCylHighReg      = 0;
    
    // calc the drive number.
    pSCIP->irDriveRegs.bDriveHeadReg = 0xA0 | ( ( bDriveNum & 1 ) << 4 );

    // The command can either be IDE identify or ATAPI identify.
    pSCIP->irDriveRegs.bCommandReg = bIDCmd;
    pSCIP->bDriveNumber = bDriveNum;
    pSCIP->cBufferSize = IDENTIFY_BUFFER_SIZE;
    
    return DeviceIoControl( hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
        ( LPVOID ) pSCIP,
        sizeof( SENDCMDINPARAMS ) - 1,
        ( LPVOID ) pSCOP,
        sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE - 1,
        lpcbBytesReturned, NULL );
}

BOOL WinNTHDSerialNumAsPhysicalRead( BYTE* dwSerial, UINT* puSerialLen, UINT uMaxSerialLen )
{
	#define  DFP_GET_VERSION          0x00074080
    BOOL bInfoLoaded = FALSE;

    for( UINT uDrive = 0; uDrive < 4; ++ uDrive )
    {
        HANDLE hPhysicalDriveIOCTL = 0;

        //  Try to get a handle to PhysicalDrive IOCTL, report failure
        //  and exit if can't.
        char szDriveName [256];
        sprintf( szDriveName, "\\\\.\\PhysicalDrive%d", uDrive );

        //  Windows NT, Windows 2000, must have admin rights
        hPhysicalDriveIOCTL = CreateFile( szDriveName,
            GENERIC_READ | GENERIC_WRITE, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
            OPEN_EXISTING, 0, NULL);

        if( hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE )
        {
            GETVERSIONOUTPARAMS VersionParams = { 0 };
            DWORD               cbBytesReturned = 0;

            // Get the version, etc of PhysicalDrive IOCTL
            if( DeviceIoControl( hPhysicalDriveIOCTL, DFP_GET_VERSION,
                NULL, 
                0,
                &VersionParams,
                sizeof( GETVERSIONOUTPARAMS ),
                &cbBytesReturned, NULL ) )
            {
                // If there is a IDE device at number "i" issue commands
                // to the device
                if( VersionParams.bIDEDeviceMap != 0 )
                {
                    BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
                    SENDCMDINPARAMS  scip = { 0 };

                    // Now, get the ID sector for all IDE devices in the system.
                    // If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
                    // otherwise use the IDE_ATA_IDENTIFY command
                    bIDCmd = ( VersionParams.bIDEDeviceMap >> uDrive & 0x10 ) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
                    BYTE IdOutCmd[sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE - 1] = { 0 };

                    if( DoIdentify( hPhysicalDriveIOCTL, 
                        &scip, 
                        ( PSENDCMDOUTPARAMS )&IdOutCmd, 
                        ( BYTE )bIDCmd,
                        ( BYTE )uDrive,
                        &cbBytesReturned ) )
                    {
                        if( * puSerialLen + 20U <= uMaxSerialLen )
                        {
                            CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )( ( ( PSENDCMDOUTPARAMS )IdOutCmd )->bBuffer ) ) + 10, 20 );  // 序列号

                            // Cut off the trailing blanks
                            UINT i = 0;
                            for( UINT i = 20; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; -- i )  {}
                            * puSerialLen += i;

                            CopyMemory( dwSerial + * puSerialLen, ( ( USHORT* )( ( ( PSENDCMDOUTPARAMS )IdOutCmd )->bBuffer ) ) + 27, 40 ); // 型号

                            // Cut off the trailing blanks
                            for( i = 40; i != 0U && ' ' == dwSerial[* puSerialLen + i - 1]; -- i )  {}
                            * puSerialLen += i;

                            bInfoLoaded = TRUE;
                        }
                        else
                        {
                            ::CloseHandle( hPhysicalDriveIOCTL );
                            return bInfoLoaded;
                        }
                    }
                }
            }
            CloseHandle( hPhysicalDriveIOCTL );
        }
    }
    return bInfoLoaded;
}

// 输入参数:lana_num为网卡编号,一般地,从0开始,但在Windows 2000中并不一定是连续分配的
//int getmac_one (int lana_num,char *buf)
//{
//	// 定义一个存放返回网卡信息的变量
//	ASTAT Adapter;
//	NCB ncb;
//	UCHAR uRetCode;
//	memset( &ncb, 0, sizeof(ncb) );
//	ncb.ncb_command = NCBRESET;
//	ncb.ncb_lana_num = lana_num ;// 指定网卡号
//	// 首先对选定的网卡发送一个NCBRESET命令,以便进行初始化
//	uRetCode = Netbios( &ncb );
//	memset( &ncb, 0, sizeof(ncb) );
//
//	ncb.ncb_command = NCBASTAT;
//
//	ncb.ncb_lana_num = lana_num; // 指定网卡号
//
//	strcpy((char *)ncb.ncb_callname,"*" );
//
//	ncb.ncb_buffer = (unsigned char *) &Adapter; // 指定返回的信息存放的变量
//
//	ncb.ncb_length = sizeof(Adapter);
//
//	// 接着,可以发送NCBASTAT命令以获取网卡的信息
//
//	uRetCode = Netbios( &ncb );
//
//	printf( "The NCBASTAT return code is: 0x%x \n", uRetCode );
//
//	if ( uRetCode == 0 )
//
//	{
//		//char buf[1024];
//		
//		// 把网卡MAC地址格式化成常用的16进制形式,如0010A4E45802
//		sprintf( buf,"%02d%02X%02X%02X%02X%02X%02X",	
//		lana_num,
//		Adapter.adapt.adapter_address[0],
//		Adapter.adapt.adapter_address[1],
//		Adapter.adapt.adapter_address[2],
//		Adapter.adapt.adapter_address[3],
//		Adapter.adapt.adapter_address[4],
//		Adapter.adapt.adapter_address[5] );
//		buf[14] = '\0';
//		return 14;
//	}
//	return 0;
//
//}
UINT FindAwardBios( BYTE** ppBiosAddr )
{
    BYTE* pBiosAddr = * ppBiosAddr + 0xEC71;

    BYTE szBiosData[128];
    CopyMemory( szBiosData, pBiosAddr, 127 );
    szBiosData[127] = 0;
    
    int iLen = lstrlen( ( char* )szBiosData );
    if( iLen > 0 && iLen < 128 )
    {
        //AWard:         07/08/2002-i845G-ITE8712-JF69VD0CC-00 
        //Phoenix-Award: 03/12/2002-sis645-p4s333
        if( szBiosData[2] == '/' && szBiosData[5] == '/' )
        {
            BYTE* p = szBiosData;
            while( * p )
            {
                if( * p < ' ' || * p >= 127 )
                {
                    break;
                }
                ++ p;
            }
            if( * p == 0 )
            {
                * ppBiosAddr = pBiosAddr;
                return ( UINT )iLen;
            }
        }
    }
    return 0;
}
UINT FindAmiBios( BYTE** ppBiosAddr )
{
    BYTE* pBiosAddr = * ppBiosAddr + 0xF478;
    
    BYTE szBiosData[128];
    CopyMemory( szBiosData, pBiosAddr, 127 );
    szBiosData[127] = 0;
    
    int iLen = lstrlen( ( char* )szBiosData );
    if( iLen > 0 && iLen < 128 )
    {
        // Example: "AMI: 51-2300-000000-00101111-030199-"
        if( szBiosData[2] == '-' && szBiosData[7] == '-' )
        {
            BYTE* p = szBiosData;
            while( * p )
            {
                if( * p < ' ' || * p >= 127 )
                {
                    break;
                }
                ++ p;
            }
            if( * p == 0 )
            {
                * ppBiosAddr = pBiosAddr;
                return ( UINT )iLen;
            }
        }
    }
    return 0;
}

UINT FindPhoenixBios( BYTE** ppBiosAddr )
{
    UINT uOffset[3] = { 0x6577, 0x7196, 0x7550 };
    for( UINT i = 0; i < 3; ++ i )
    {
        BYTE* pBiosAddr = * ppBiosAddr + uOffset[i];

        BYTE szBiosData[128];
        CopyMemory( szBiosData, pBiosAddr, 127 );
        szBiosData[127] = 0;

        int iLen = lstrlen( ( char* )szBiosData );
        if( iLen > 0 && iLen < 128 )
        {
            // Example: Phoenix "NITELT0.86B.0044.P11.9910111055"
            if( szBiosData[7] == '.' && szBiosData[11] == '.' )
            {
                BYTE* p = szBiosData;
                while( * p )
                {
                    if( * p < ' ' || * p >= 127 )
                    {
                        break;
                    }
                    ++ p;
                }
                if( * p == 0 )
                {
                    * ppBiosAddr = pBiosAddr;
                    return ( UINT )iLen;
                }
            }
        }
    }
    return 0;
}
int GetEXAKEYCpu(int iea,char *szCPUInfo)
{
	BOOL bException = FALSE;
	UINT ua,ub,uc,ud;
	__try 
    {		
        _asm 
        {
			mov eax, iea
			cpuid
			mov ua, eax
			mov ub, ebx
			mov uc, ecx
			mov ud, edx	
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
    {
        bException = TRUE;
    }
	if (!bException)
	{
		int num = 0;		
		sprintf(szCPUInfo,"%d:%.8x %.8x %.8x %.8x  ",iea,ua,ub,uc,ud);		
		return 39;
	}
	else
		return 0;
}
/*int  GetAllCpuInfo(char *szCPUInfo)
{
	
	int num = 0;
	for (int i=0;i<5;i++)
	{
		char buf[1024];
		int count = GetEXAKEYCpu(i,buf);
		CopyMemory( szCPUInfo + num, buf, count);
		num += count;
	}
	return num;
	
}*/