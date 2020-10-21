#pragma once
#include<string>
#include <vector>

#define USER_ERROR	100
#define ERROR_CONFIGURATION_SETTING_MISSING		USER_ERROR+1
#define ERROR_IP_FORMAT							USER_ERROR+2

class DataManager
{
	public:
		DataManager();
		std::string GetCameraRtsp(int);
		void WriteFile(void);
		void GlobalResourceInitial();
		void GlobalResourceCleanUp();

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

};