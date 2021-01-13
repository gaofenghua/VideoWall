#include <stdio.h>
#include "DataManager.h"
#include "VWStream.h"

#include <iostream>
#include <string>

using namespace std;

void Test_DataManager()
{
	DataManager dm;

	if (dm.m_Status != 0)
	{
		cout << "DataManager = " << dm.m_Status << endl << endl;
		//cout << dm.m_ErrorString << endl;
	}

	int nViewID;
	int nIndex;
	vector<int> vRet;
	int nScreenID;
	RECT rRet;

	cout << "GetViewChannel: Please input" << endl;
	cout << "   View ID = ";
	cin >> nViewID;
	cout << "   output = " << dm.GetViewChannel(nViewID) << endl;

	cout << "\nGetSwitchInterval: " << endl;
	cout << "    output = " << dm.GetSwitchInterval() << endl;

	cout << "\nGetTouringCameras: Please input" << endl;
	cout << "    view id = ";
	cin >> nViewID;
	cout << "    index = ";
	cin >> nIndex;
	vRet = dm.GetTouringCameras(nViewID, nIndex);
	cout << "    output = ";
	for (int i = 0; i < vRet.size(); i++)
	{
		cout << " " << vRet[i];
	}
	cout << endl;

	cout << "\nGetScreenIDs: " << endl;
	vRet = dm.GetScreenIDs();
	cout << "    output = ";
	for (int i = 0; i < vRet.size(); i++)
	{
		cout << " " << vRet[i];
	}
	cout << endl;

	cout << "\nGetScreenInfo: Please input" << endl;
	cout << "   Screen ID = ";
	cin >> nScreenID;
	dm.GetScreenInfo(nScreenID, &rRet);
	cout << "   output = " << rRet.left << " " << rRet.top << "," << rRet.right <<" " << rRet.bottom << endl;
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

	cout << "GetCameraRtsp: nChannel = " << nChannel << " url = " << sUrl << endl;

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
	//nRet = stm.Connect(1, "rtsp://192.168.77.211:50010/live?camera=1&user=admin&pass=A1crUF4%3D&stream=1");
	
	nRet = stm.Connect(nChannel, sUrl);
	if (nRet < 0)
	{
		cout << "Connect failed. ret = " << nRet << endl;
		cout << "Abort. ";
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

		stm.OpenOutputFile();
		stm.WriteOutputFile(packet);
		stm.CloseOutputFile();
		
		Sleep(40);
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

void Test_Monitor_Settings()
{
	DataManager dm;
	cout << "Monitors Below: \n";

	vector<int> a = dm.GetScreenIDs();
	for (int i = 0; i < a.size(); i++)
	{
		RECT r;
		dm.GetScreenInfo(a[i], &r);
		cout << "    id = " << a[i] << " ( " << r.left << " " << r.top << " ) ( " << r.right << " " << r.bottom << " )"<< endl;
	}
}

void Test_Channels()
{
	DataManager dm;

	char* pFile;
	vector<int>* pChannels;

	cout << "Channels: \n";
	for (int i = 0; i < 6; i++)
	{
		if (dm.m_ChannelFile[i] == NULL)
		{
			pFile = "";
		}
		else
		{
			pFile = dm.m_ChannelFile[i];
		}

		cout << "\n    File = " << pFile << endl;

		cout << "        Channels = ";
		pChannels = &(dm.m_Channels[i]);
		for (int j = 0; j < pChannels->size(); j++)
		{
			cout << (*pChannels)[j] << " " ;
		}
		cout << "\n";
	}

	cout << "\n\nGetTouringCameras: ";
	vector<int> vRet;
	vRet = dm.GetTouringCameras(1, 4);
	for (int i = 0; i < vRet.size(); i++)
	{
		cout << vRet[i] << " ";
	}

}

int main(int argc, char* args[])
{

	//dm.GlobalResourceCleanUp();

	Test_ReadFrame();
	//Test_DataManager();
	
	//Test_Monitor_Settings();
	//Test_Channels();
	
	getchar();
	printf("\nTesting function ends! \n");
	getchar();

	return 0;
}


