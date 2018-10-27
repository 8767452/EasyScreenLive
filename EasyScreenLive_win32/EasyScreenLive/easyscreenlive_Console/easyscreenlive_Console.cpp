// easyscreenlive_Console.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "../bin/libEasyScreenLiveAPI.h"
#pragma comment(lib, "../bin/libEasyScreenLive.lib")
#include <string.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32")        //���ӵ�ws2_32��̬���ӿ�


#define EASY_RTSP_KEY "6A36334A743469576B5A754142394662704337344A65684659584E3555324E795A57567554476C325A56394462334A6C4C6D56345A56634D5671442F70654E4659584E355247467964326C755647566862556C7A5647686C516D567A644541794D4445345A57467A65513D3D"
#define EASY_RTMP_KEY "79397037795969576B5A734157644A627043326D7065684659584E3555324E795A57567554476C325A56394462334A6C4C6D56345A56634D5671442F70654E4659584E355247467964326C755647566862556C7A5647686C516D567A644541794D4445345A57467A65513D3D"
#define EASY_IPC_KEY "6D72754B7A4969576B5A754171744E62704378564A65684659584E3555324E795A57567554476C325A56394462334A6C4C6D56345A56634D5671442F70654E4659584E355247467964326C755647566862556C7A5647686C516D567A644541794D4445345A57467A65513D3D"

const char* server_ip = "www.easydss.com";
int server_port = 10085;
int encode_bitrate = 2048;
const char* stream_name = "Sword";
void* g_pusher = NULL;
SOURCE_TYPE g_sourceType = SOURCE_SCREEN_CAPTURE;
// 	SOURCE_LOCAL_CAMERA = 0,//��������Ƶ
// 	SOURCE_SCREEN_CAPTURE =1,//��Ļ����
// 	SOURCE_FILE_STREAM = 2,       //�ļ�������(mp4,ts,flv???)
// 	SOURCE_RTSP_STREAM=3,//RTSP��
// 	SOURCE_RTMP_STREAM=4,//RTMP��
//nEncoderType �������ͣ� 
//		0=Ĭ�ϱ�������Ч����ͣ�ͨ����ǿ�� 
//		1=����루Ч�ʸߣ�ͨ���Բ�ǿ��
//		2=Ӳ�����루Ч����ߣ�ͨ������ͣ���ҪӢΰ������Կ�֧�֣�
int g_nEncoderType = 1;
bool g_bRecord = true ;

#define MAX_CHANNEL_NUM 1

#include <string>
using namespace std;

int GetLocalIP( std::string &local_ip )  
{  
	WSADATA wsaData = {0};  
	if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0)  
		return -1;  
	char szHostName[MAX_PATH] = {0};  
	int nRetCode;  
	nRetCode = gethostname(szHostName, sizeof(szHostName));  
	PHOSTENT hostinfo;  
	if (nRetCode != 0)  
		return WSAGetLastError();          
	hostinfo = gethostbyname(szHostName);  
	local_ip = inet_ntoa(*(struct in_addr*)*hostinfo->h_addr_list);  
	WSACleanup();  
	return 1;  
} 

int _tmain(int argc, _TCHAR* argv[])
{
	if(!g_pusher )
		g_pusher =  EasyScreenLive_Create(EASY_IPC_KEY, EASY_RTMP_KEY, EASY_RTSP_KEY);

	//1 �ɼ�
	int ret = EasyScreenLive_StartCapture(g_pusher, g_sourceType, NULL, -1, -1, NULL, g_nEncoderType, 1920,1080,25, encode_bitrate, (char*)("RGB24"),44100,2);

	//2 ����
	//ret = EasyScreenLive_StartPush(g_pusher, PUSH_RTMP, (char*)server_ip, server_port,  (char*)stream_name, 1,1024, g_bRecord );
	//2.1 RTSPServer
	EASYLIVE_CHANNEL_INFO_T	liveChannel[MAX_CHANNEL_NUM];
	memset(&liveChannel[0], 0x00, sizeof(EASYLIVE_CHANNEL_INFO_T)*MAX_CHANNEL_NUM);
	for (int i=0; i<MAX_CHANNEL_NUM; i++)
	{
		liveChannel[i].id = i;
		//strcpy(liveChannel[i].name, channel[i].name);
		sprintf(liveChannel[i].name, "channel=%d", i);
#if 1
		if (i==0)
		{
			liveChannel[i].enable_multicast = 0;
			strcpy(liveChannel[i].multicast_addr, ("238.255.255.255"));//"238.255.255.255"
			liveChannel[i].ttl = 255 ;//255;
		}
#endif
	}
	ret = EasyScreenLive_StartServer(g_pusher, 8554, "", "",  liveChannel, MAX_CHANNEL_NUM );
	string ip;
	GetLocalIP(ip);

	printf("start stream: rtsp://%s:8554/channel=0\n", ip.c_str() );

	printf("Press enter key to exit!!!\n");
	getchar();

	//3 ֹͣ����	
	EasyScreenLive_StopPush(g_pusher, PUSH_RTMP);
	//4 ֹͣ�ɼ�	
	EasyScreenLive_StopCapture(g_pusher);
	//5. ����ʵ��
	EasyScreenLive_Release(g_pusher);

	return 0; 
}

