#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include "netsdk.h"
#include "../../objs/include/srs_librtmp.h"

srs_rtmp_t rtmp;
long g_LoginID=0;
int timestamp=0;
FILE * g_pFile;

int  RealDataCallBack_V2(long lRealHandle, const PACKET_INFO_EX *pFrame, long dwUser)
{
    /*
    printf("frame type : %d\n",pFrame->nPacketType);
    printf("dwPacketSize: %d\n",pFrame->dwPacketSize);    
	printf("time:%04d-%02d-%02d %02d:%02d:%02d\n",pFrame->nYear,pFrame->nMonth,pFrame->nDay,pFrame->nHour,pFrame->nMinute,pFrame->nSecond);
    printf("dwTimeStamp: %d\n",pFrame->dwTimeStamp);
    printf("dwTimeStampHigh: %d\n",pFrame->dwTimeStampHigh);
    printf("dwFrameNum: %d\n",pFrame->dwFrameNum);
    printf("dwFrameRate: %d\n",pFrame->dwFrameRate);
    printf("uWidth: %d\n",pFrame->uWidth);
    printf("uHeight: %d\n",pFrame->uHeight); 
    */
    printf("time:%04d-%02d-%02d %02d:%02d:%02d\n",pFrame->nYear,pFrame->nMonth,pFrame->nDay,pFrame->nHour,pFrame->nMinute,pFrame->nSecond);
    
	BOOL bResult = TRUE;
	try{
        int skipByte = 8;
        if( pFrame->nPacketType == 1){
            skipByte = 16;
        }
        
		fwrite(pFrame->pPacketBuffer + skipByte ,1,pFrame->dwPacketSize - skipByte,g_pFile);
        
        int ret = srs_h264_write_raw_frames(rtmp, pFrame->pPacketBuffer + skipByte, pFrame->dwPacketSize - skipByte, timestamp, timestamp);
        if (ret != 0) {
            if (srs_h264_is_dvbsp_error(ret)) {
                srs_human_trace("ignore drop video error, code=%d", ret);
            } else if (srs_h264_is_duplicated_sps_error(ret)) {
                srs_human_trace("ignore duplicated sps, code=%d", ret);
            } else if (srs_h264_is_duplicated_pps_error(ret)) {
                srs_human_trace("ignore duplicated pps, code=%d", ret);
            } else {
                srs_human_trace("send h264 raw data failed. ret=%d", ret);
                return -1;
            }
        }
        timestamp += 50;
	}
	catch (...)
	{
	}

	// it must return TRUE if decode successfully,or the SDK will consider the decode is failed
	return bResult;
}

int  main(int argc, char* argv[])
{    
    if (argc <= 4) {
        printf("Usage: %s <camera host> <camera port> <camera user> <rtmp url>\n", argv[0]);
        //printf("     h264_raw_file: the h264 raw steam file.\n");
        //printf("     rtmp_publish_url: the rtmp publish url.\n");
        printf("For example:\n");
        printf("     %s www.hslink.top 9500 test rtmp://10.16.4.175/live/test\n", argv[0]);
        return -1;
    }
    
    char* camera_host = argv[1];
    const int camera_port = atoi(argv[2]);
    char* camera_user = argv[3];
    const char* rtmp_url = argv[4];
    
    // connect rtmp server
    if(true)
    {
        // connect rtmp context
        rtmp = srs_rtmp_create(rtmp_url);
        
        if (srs_rtmp_handshake(rtmp) != 0) {
            srs_human_trace("simple handshake failed.");
            return -1;
        }
        srs_human_trace("simple handshake success");
        
        if (srs_rtmp_connect_app(rtmp) != 0) {
            srs_human_trace("connect vhost/app failed.");
            return -1;
        }
        srs_human_trace("connect vhost/app success");
        
        if (srs_rtmp_publish_stream(rtmp) != 0) {
            srs_human_trace("publish stream failed.");
            return -1;
        }
        srs_human_trace("publish stream success");
    }
    // --
    
	H264_DVR_Init(NULL,NULL);
	printf("H264_DVR_Init\n");

	H264_DVR_DEVICEINFO OutDev;	
	memset(&OutDev,0,sizeof(OutDev));
	int nError = 0;
	
    g_LoginID = H264_DVR_Login(camera_host, camera_port, camera_user,(char*)"",(LPH264_DVR_DEVICEINFO)(&OutDev),&nError);
	printf("g_LoginID=%d,nError:%d\n",g_LoginID,nError);
	
 	if(g_LoginID>0)
 	{
        printf("**************login ok***************\n");	
		H264_DVR_CLIENTINFO playstru;

		playstru.nChannel = 0;
		playstru.nStream = 0;
		playstru.nMode = 0;
		long m_iPlayhandle = H264_DVR_RealPlay(g_LoginID, &playstru);	
		if(m_iPlayhandle == 0 )
		{
			printf("start RealPlay wrong!\n");
		}
		else
		{
			g_pFile = fopen("TestRealPlay.h264", "wb+");
			H264_DVR_SetRealDataCallBack_V2(m_iPlayhandle, RealDataCallBack_V2, 0);

			printf("start RealPlay ok!");
			sleep(300);
			if(H264_DVR_StopRealPlay(m_iPlayhandle))
			{
				printf("stop realPlay ok\n");
			}
			else
			{
				printf("stop realPlay Wrong\n");
			}
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
	H264_DVR_Cleanup();
    printf("H264_DVR_Cleanup!!!\n");
    
    // clenn rtmp
    srs_rtmp_destroy(rtmp);

	if(g_pFile)
	{
		fclose(g_pFile);
	}
	printf("**************OVER************\n");
	return 0;
}
