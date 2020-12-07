#pragma once
#include <string>
#include <vector>
#include <Windows.h>

#define DATAMANAGER_ERROR	0

#define ERROR_CONFIG_FILE_MISSING				DATAMANAGER_ERROR-1
#define ERROR_CONFIGURATION_FORMAT_UNKNOW		DATAMANAGER_ERROR-2
#define ERROR_CONFIGURATION_SETTING_MISSING		DATAMANAGER_ERROR-3
#define ERROR_IP_FORMAT							DATAMANAGER_ERROR-4
#define ERROR_CHANNELSFILE_MISSING				DATAMANAGER_ERROR-5
#define ERROR_CHANNELSFILE_DATA_NOT_NUMBER		DATAMANAGER_ERROR-6

class DataManager
{
	public:
		DataManager();
		~DataManager();

		int GetScreenView(int t_ScreenID); // return the number of views
		int GetViewChannel(int t_ViewID); // return the number of channels
		int GetSwitchInterval(void); // return the switch interval
		std::vector<int> GetTouringCameras(int t_ViewID); // return the list of trackerIDs 
		std::string GetCameraRtsp(int t_CameraID); // return the rtsp url
		std::vector<int> GetCameraTourInfo(int t_ViewID, int t_ChannelID); // return the list of trackerIDs
		std::vector<int> GetScreenIDs(void); // return the list of screen ids
		int GetScreenInfo(int t_ScreenID, RECT* pScreenRect); // return content of the screen rect

	private:
		int m_Status = 0;
		std::string m_FileName = "./config.txt";

		std::string m_ServerIP = "1.1.1.1";
		int m_ServerPort = 80;

		std::string m_UserName = "root";
		std::string m_PassWord = "####";

		int m_SwitchInterval = -1;
		int m_ScreenNumber = 6;
		int m_ViewNumber = 16;

		char* m_ChannelFile[6] = {NULL,NULL,NULL,NULL,NULL,NULL};

		//int m_ChannelGroupNumber = 1;
		std::vector<std::vector<int> > m_Channels;

		int ReadConfigFile(void);
		int VerifyInputData(void);

		bool chkNumber(std::string& str);
		std::vector<std::string> split(std::string& str, char delim);
		bool validateIP(std::string ip);
public:
		int m_nTotalMonitor = 0;
		MONITORINFOEX* m_MonitorInfo = NULL;
		void QueryMonitorSettings();
		BOOL CALLBACK EnumMonitor(HMONITOR handle, HDC hdc, LPRECT rect, LPARAM param);

	public:
		int ReadChannelsFile(char *pFileName);
		void WriteFile(void);
		void GlobalResourceInitial();
		void GlobalResourceCleanUp();
};