#include <stdio.h>
#include "DataManager.h"
#include "VWStream.h"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* args[])
{
	printf("Main function begins:\n");

	DataManager dm;
	//dm.WriteFile();

	//string sUrl;
	//sUrl = dm.GetCameraRtsp(0);

	//cout << sUrl;

	VWStream stm;
	VWStream stm2;
	stm.Connect(1, "rtsp://root:pass@172.20.76.100/axis-media/media.amp?videocodec=h264");
	stm2.Connect(1, "rtsp://root:pass@172.20.76.100/axis-media/media.amp?videocodec=h264");

	for (int i = 0; i < 10; i++)
	{
		//printf("Main VWStream Obj CameraID=%d, buffer head = %d, end = %d\r\n", stm.m_CameraID,stm.m_Decoder.Buffer_Head, stm.m_Decoder.Buffer_End);
		stm.PrintStatus();
		stm2.PrintStatus();
		Sleep(1000);
	}
	getchar();

	dm.GlobalResourceCleanUp();
	return 0;
}
