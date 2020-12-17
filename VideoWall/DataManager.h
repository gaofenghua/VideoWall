#pragma once
#include <string>
#include <vector>
#include <Windows.h>

#define DATAMANAGER_ERROR	0

#define ERROR_CONFIG_FILE_MISSING				DATAMANAGER_ERROR-1
// According to the weekly meeting comments on 2020-12-14,
// When ERROR_CONFIGURATION_FORMAT_UNKNOW happens, m_ErrorString is set to indicate the exact line number and error.  
#define ERROR_CONFIGURATION_FORMAT_UNKNOW		DATAMANAGER_ERROR-2
#define ERROR_CONFIGURATION_SETTING_MISSING		DATAMANAGER_ERROR-3
#define ERROR_IP_FORMAT							DATAMANAGER_ERROR-4
#define ERROR_CHANNELSFILE_MISSING				DATAMANAGER_ERROR-5
#define ERROR_CHANNELSFILE_DATA_NOT_NUMBER		DATAMANAGER_ERROR-6
#define ERROR_INVALID_PARAMETER					DATAMANAGER_ERROR-7



class DataManager
{
	public:
		DataManager();
		~DataManager();

		int m_Status = 0;
		std::string m_ErrorString;

		int GetScreenView(int t_ScreenID); // return the number of views. Removed according to the weekly meeting on 11/16/2020
		int GetViewChannel(int t_ViewID); // return the number of channels
		int GetSwitchInterval(void); // return the switch interval
		std::vector<int> GetTouringCameras(int t_ViewID,int t_Index); // return the list of trackerIDs 
		std::string GetCameraRtsp(int t_CameraID); // return the rtsp url
		std::vector<int> GetCameraTourInfo(int t_ViewID, int t_ChannelID); // return the list of trackerIDs. Low priority according to the weekly meeting on 11/16/2020
		std::vector<int> GetScreenIDs(void); // return the list of screen ids
		int GetScreenInfo(int t_ScreenID, RECT* pScreenRect); // return content of the screen rect

	private:
		
		std::string m_FileName = "./config.txt";

		std::string m_ServerIP = "1.1.1.1";
		int m_ServerPort = 80;

		std::string m_UserName = "root";
		std::string m_PassWord = "####";

		int m_SwitchInterval = 30;
		int m_ScreenNumber = 6;
		int m_ViewNumber = 16;

		int ReadConfigFile(void);
		int VerifyInputData(void);

		bool chkNumber(std::string& str);
		std::vector<std::string> split(std::string& str, char delim);
		bool validateIP(std::string ip);

		int m_nTotalMonitor = 0;
		MONITORINFOEX* m_MonitorInfo = NULL;
		void QueryMonitorSettings();
		BOOL CALLBACK EnumMonitor(HMONITOR handle, HDC hdc, LPRECT rect, LPARAM param);

	public:

		char* m_ChannelFile[6] = { NULL,NULL,NULL,NULL,NULL,NULL };
		std::vector<std::vector<int> > m_Channels;

		int ReadChannelsFile(int nChannelGroup);
		void WriteFile(void);
		void GlobalResourceInitial();
		void GlobalResourceCleanUp();
};