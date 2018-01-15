#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include "netsdk.h"

long g_LoginID=0;

bool CbAlarmInfo(long lLoginID, char *pBuf, unsigned long dwBufLen, long dwUser, int t)
{
	if (dwBufLen == sizeof(SDK_NetAlarmCenterMsg))
	{
		//CAlarmCenterDlg *pThis = (CAlarmCenterDlg *)dwUser;
		SDK_NetAlarmCenterMsg *pMsg = (SDK_NetAlarmCenterMsg *)pBuf;
		//pThis->InsertData(pMsg);
        printf("message : \n");
        printf("%d , %d , %d, %s, %s, %s\n",pMsg->nChannel,pMsg->nType, pMsg->nStatus, pMsg->sEvent, pMsg->sSerialID, pMsg->sDescrip);
        //CONFIG_IPAddress HostIP;			///< 设备IP
        //int  nChannel;                  	///< 通道
        //int  nType;                     	///< 类型 见AlarmCenterMsgType
        //int  nStatus;                   	///< 状态 见AlarmCenterStatus
        //SDK_SYSTEM_TIME Time;           	///< 发生时间
        //char sEvent[NET_MAX_INFO_LEN];  	///< 事件
        //char sSerialID[NET_MAX_MAC_LEN]; 	///< 设备序列号
        //char sDescrip[NET_MAX_INFO_LEN];  	///< 描述
		return true;
	}
	return false;
}

int  main(int argc, char* argv[])
{    
    if (argc <= 3) {
        printf("Usage: %s <camera host> <camera port> <camera user> <rtmp url>\n", argv[0]);
        printf("For example:\n");
        printf("     %s www.hslink.top 9500 rnaf \n", argv[0]);
        return -1;
    }
    
    char* camera_host = argv[1];
    const int camera_port = atoi(argv[2]);
    char* camera_user = argv[3];    
    
	H264_DVR_Init(NULL,NULL);
	printf("H264_DVR_Init\n");

	H264_DVR_DEVICEINFO OutDev;	
	memset(&OutDev,0,sizeof(OutDev));
	int nError = 0;    
    unsigned long temp;
	
    g_LoginID = H264_DVR_Login(camera_host, camera_port, camera_user,(char*)"",(LPH264_DVR_DEVICEINFO)(&OutDev),&nError);
	printf("g_LoginID=%d,nError:%d\n",g_LoginID,nError);
	
 	if(g_LoginID>0)
 	{
        printf("**************login ok***************\n");        
        if (false == H264_DVR_StartAlarmCenterListen(15002, CbAlarmInfo, NULL)){
            printf("StartAlarmCenterFail\n");
        }
        else{
            printf("StartAlarmCenterSuccess\n");
            sleep(1000000);
        }
 	}
 	else
 	{
 		printf("**************login wrong************\n");	
 	}

	if(g_LoginID>0)
	{
		H264_DVR_Logout(g_LoginID);
		printf("Logout success!!!\n");
	}
	H264_DVR_StopAlarmCenterListen();
    printf("H264_DVR_StopAlarmCenterListen!!!\n");

	printf("**************OVER************\n");
	return 0;
}
