#include <stdio.h>
#include "DataManager.h"
#include "VWStream.h"

#include <iostream>
#include <string>

using namespace std;

void Test_DataManager()
{
	DataManager dm;
}

void Test_ReadFrame()
{
	int nChannel = 0;
	string sUrl;

	int nPackages;
	char sYes;

	cout << "Please input Channel id number like 0, 1, 2 ... \n  Channel ID = ";
	cin >> nChannel;

	cout << "Please input Packages to receive like 0, 1, 2 ...\n  Packages = ";
	cin >> nPackages;


	DataManager dm;
	sUrl = dm.GetCameraRtsp(nChannel);

	cout << "\nStart y/n?  ";
	cin >> sYes;

	if (sYes != 'y')
	{
		cout << "\nAbort.\n";
		return;
	}

	int nRet = 0;
	VWStream stm;
	//nRet = stm.Connect(1, "rtsp://root:pass@172.20.76.100/axis-media/media.amp?videocodec=h264");
	//stm.Connect(1, "e:\\temp\\test.mp4");
	//stm.Connect(1, "rtsp://192.168.77.211:50010/live?camera=1&user=admin&pass=A1crUF4%3D&stream=1");
	
	nRet = stm.Connect(nChannel, sUrl);
	if (nRet < 0)
	{
		cout << "Connect failed. ret = " << nRet;
		return;
	}

	AVPacket* packet;
	packet = av_packet_alloc();

	for (int i = 0; true; i++)
	{
		if (nPackages != 0)
		{
			if (i >= nPackages)
			{
				break;
			}
		}

		stm.ReadFrame(packet);
		stm.WriteOutputFile(packet);
		cout << "Read packet " << i+1 << endl;
		
		Sleep(40);
	}

	av_packet_free(&packet);
	stm.Close();

	printf("\nReadFrame test finished.\n");
}


int main(int argc, char* args[])
{

	//DISPLAY_DEVICE device = { 0 };
	//EnumDisplayDevices();
	

	//dm.GlobalResourceCleanUp();

	//Test_ReadFrame();
	Test_DataManager();

	getchar();
	

	getchar();
	printf("Testing function ends! \n");
	
	return 0;
}


