#include <stdio.h>
#include "DataManager.h"
#include "VWStream.h"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* args[])
{
	printf("Testing function begins:\n");

	//DataManager dm;

	//for (int i = 0; i < 20; i++)
	//{
	//	dm.GetCameraRtsp(i);
	//}

	//dm.ReadFile();

	//DISPLAY_DEVICE device = { 0 };
	//EnumDisplayDevices();
	
	//dm.WriteFile();

	//string sUrl;
	//sUrl = dm.GetCameraRtsp(0);

	//cout << sUrl;

	VWStream stm;
	//VWStream stm2;
	//stm.Connect(1, "rtsp://root:pass@172.20.76.100/axis-media/media.amp?videocodec=h264");
	stm.Connect(1, "e:\\temp\\test.mp4");
	//stm.Connect(1, "rtsp://192.168.77.211:50010/live?camera=1&user=admin&pass=A1crUF4%3D&stream=1");

	//stm2.Connect(1, "rtsp://root:pass@172.20.76.100/axis-media/media.amp?videocodec=h264");

	//for (int i = 0; i < 10; i++)
	//{
	//	//printf("Main VWStream Obj CameraID=%d, buffer head = %d, end = %d\r\n", stm.m_CameraID,stm.m_Decoder.Buffer_Head, stm.m_Decoder.Buffer_End);
	//	stm.PrintStatus();
	//	stm2.PrintStatus();
	//	Sleep(1000);
	//}
	

	//dm.GlobalResourceCleanUp();
	getchar();
	stm.m_Decoder.ReadFram_Thread_Exit = true;

	getchar();
	printf("Testing function ends! \n");
	
	return 0;
}

