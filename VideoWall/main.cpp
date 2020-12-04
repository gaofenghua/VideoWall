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

		cout << "Read packet " << i + 1 << endl;
		stm.ReadFrame(packet);
		stm.WriteOutputFile(packet);
		
		
		//Sleep(40);
	}

	av_packet_free(&packet);
	stm.Close();

	printf("\nReadFrame test finished.\n");
}

void GetScreenRect(int ScreenNo)
{
	RECT m_ScrRect[2];

	BOOL flag;
	DISPLAY_DEVICE dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.cb = sizeof(dd);
	flag = EnumDisplayDevices(NULL, ScreenNo, &dd, 0);
	if (!flag) return;

	DISPLAY_DEVICE dd2;
	ZeroMemory(&dd2, sizeof(dd2));
	dd2.cb = sizeof(dd2);
	flag = EnumDisplayDevices(dd.DeviceName, 0, &dd2, 0);
	if (!flag) return;

	DEVMODE dm;
	ZeroMemory(&dm, sizeof(dm));
	dm.dmSize = sizeof(dm);
	flag = EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm);
	m_ScrRect[ScreenNo].left = dm.dmPosition.x;
	m_ScrRect[ScreenNo].top = dm.dmPosition.y;
	m_ScrRect[ScreenNo].right = dm.dmPelsWidth;
	m_ScrRect[ScreenNo].bottom = dm.dmPelsHeight;
}

void test()
{
	char szSaveDeviceName[32];

	DISPLAY_DEVICE dd;
	ZeroMemory(&dd, sizeof(dd));
	dd.cb = sizeof(DISPLAY_DEVICE);

	DWORD deviceNum = 0;
	while (EnumDisplayDevices(NULL, deviceNum, &dd, 0))
	{
		

		//DumpDevice(dd, 0);
		DISPLAY_DEVICE newdd = { 0 };
		ZeroMemory(&newdd, sizeof(newdd));
		newdd.cb = sizeof(DISPLAY_DEVICE);
		DWORD monitorNum = 0;
		while (EnumDisplayDevices(dd.DeviceName, monitorNum, &newdd, 0))
		{
			//DumpDevice(newdd, 4);
			monitorNum++;
		}
		puts("");
		deviceNum++;
	}
	//DISPLAY_DEVICE_ATTACHED_TO_DESKTOP

	//EnumDisplayMonitors();
}



int main(int argc, char* args[])
{
	DataManager dm;
	dm.QueryMonitorSettings();
	//DISPLAY_DEVICE device = { 0 };
	//EnumDisplayDevices();
	

	//int s = GetSystemMetrics(SM_CMONITORS);
	//GetScreenRect(0);
	//test();
	//std::cout << "numbers:" << GetSystemMetrics(SM_CMONITORS) << std::endl;
	//// 枚举当前的所有显示器
	//EnumDisplayMonitors(NULL, NULL, EnumMonitor, NULL);

	//dm.GlobalResourceCleanUp();

	//Test_ReadFrame();
	//Test_DataManager();

	getchar();
	

	getchar();
	printf("Testing function ends! \n");
	
	return 0;
}


