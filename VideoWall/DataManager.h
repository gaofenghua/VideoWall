#pragma once
#include<string>
#include <vector>
#include <Windows.h>

#define USER_ERROR	100
#define ERROR_CONFIGURATION_SETTING_MISSING		USER_ERROR+1
#define ERROR_IP_FORMAT							USER_ERROR+2

class DataManager
{
	public:
		DataManager();
		
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
		int m_ScreenNumber = 1;
		int m_ViewNumber = 16;

		int m_ChannelGroupNumber = 1;
		std::vector<std::vector<int> > m_Channels;

		int ReadConfigFile(void);
		int VerifyInputData(void);

		bool chkNumber(std::string& str);
		std::vector<std::string> split(std::string& str, char delim);
		bool validateIP(std::string ip);

	public:
		void ReadFile(void);
		void WriteFile(void);
		void GlobalResourceInitial();
		void GlobalResourceCleanUp();
};