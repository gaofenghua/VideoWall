#include "DataManager.h"
#include <libconfig.h++>

#include <fstream>
#include <iostream>

#include "VWStream.h"

using namespace std;
using namespace libconfig;

#define   LIBPATH(p,f)   p##f 
#pragma comment(lib,LIBPATH(__FILE__,   "\\..\\lib\\libconfig++.lib")) 


DataManager::DataManager()
{
	m_Status = ReadConfigFile();
	if (m_Status != 0)
	{
		return;
	}

	m_Status = VerifyInputData();
	if (m_Status != 0)
	{
		return;
	}

	GlobalResourceInitial();

	QueryMonitorSettings();
}
DataManager::~DataManager()
{
	for (int i = 0; i < 6; i++)
	{
		if (m_ChannelFile[i] != NULL)
		{
			delete m_ChannelFile[i];
			m_ChannelFile[i] = NULL;
		}
	}

	if (m_nTotalMonitor > 0 && m_MonitorInfo != NULL)
	{
		delete m_MonitorInfo;

		m_MonitorInfo = NULL;
		m_nTotalMonitor = 0;
	}
}
void DataManager::GlobalResourceInitial()
{
	avformat_network_init();
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
}
void DataManager::GlobalResourceCleanUp()
{
	SDL_Quit();
	avformat_network_deinit();
}

int DataManager::ReadConfigFile()
{
	Config vw_config;
	int nRet;

	try
	{
		vw_config.readFile(m_FileName);
	}
	catch (const FileIOException &fioex)
	{
		std::cerr << "I/O error while reading file." << std::endl;
		return(ERROR_CONFIG_FILE_MISSING);
	}
	catch (const ParseException &pex)
	{
		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
			<< " - " << pex.getError() << std::endl;
		return(ERROR_CONFIGURATION_FORMAT_UNKNOW);
	}

	try
	{
		//string vw_version = vw_config.lookup("version");
		m_ServerIP = vw_config.lookup("avms_server_ip").c_str();
		m_ServerPort = vw_config.lookup("avms_server_port");
		m_UserName = vw_config.lookup("avms_server_username").c_str();
		m_PassWord = vw_config.lookup("avms_server_password").c_str();
	
	}
	catch (const SettingNotFoundException &nfex)
	{
		cerr << "ERROR: Setting not found in configuration file: " << nfex.getPath() << endl;
		return(ERROR_CONFIGURATION_SETTING_MISSING);
	}

	try
	{
		m_SwitchInterval = vw_config.lookup("switch_interval");
	}
	catch (const SettingNotFoundException &nfex)
	{
		cerr << "Warnning: Setting not found in configuration file: " << nfex.getPath() << endl;
	}

	//try
	//{
	//	m_ScreenNumber = vw_config.lookup("screen_number");
	//}
	//catch (const SettingNotFoundException &nfex)
	//{
	//	cerr << "Warnning: Setting not found in configuration file: " << nfex.getPath() << endl;
	//}
	//m_Channels.resize(m_ScreenNumber);

	try
	{
		m_ViewNumber = vw_config.lookup("view_number");
	}
	catch (const SettingNotFoundException &nfex)
	{
		cerr << "Warnning: Setting not found in configuration file: " << nfex.getPath() << endl;
	}
		
	//for (int i = 0; i < m_ScreenNumber; i++)
	//{
	//	try
	//	{
	//		string parameterName = "channels_" + to_string(i);
	//		const Setting &vw_channels = vw_config.lookup(parameterName);
	//		int nLength = vw_channels.getLength();

	//		m_Channels[i].resize(nLength);
	//		for (int j = 0; j < nLength; j++)
	//		{
	//			m_Channels[i][j] = vw_channels[j];
	//			//printf("%d\r\n", m_Channels[i][j]);
	//		}

	//	}
	//	catch (const SettingNotFoundException &nfex)
	//	{
	//		cerr << "Warnning: Setting not found in configuration file: " << nfex.getPath() << endl;
	//	}
	//}

	for (int i = 0; i < 6; i++)
	{
		string parameterName = "channels_" + to_string(i);
		try
		{
			string filename = vw_config.lookup(parameterName);

			int nNameSize = filename.length();
			char *pNameBuf = new char[nNameSize + 1];
			strcpy(pNameBuf, filename.c_str());
			m_ChannelFile[i] = pNameBuf;
		}
		catch (const SettingNotFoundException &nfex)
		{
			//cerr << "Warnning: Setting not found in configuration file: " << nfex.getPath() << endl;
		}
	}

	m_Channels.resize(6);
	for (int i = 0; i < 6; i++)
	{
		if (m_ChannelFile[i] != NULL)
		{
			nRet = ReadChannelsFile(i);
			if (nRet != 0)
			{
				return nRet;
			}
		}
	}
	

	return 0;
}

int DataManager::VerifyInputData(void)
{
	if (false == validateIP(m_ServerIP))
	{
		return ERROR_IP_FORMAT;
	}
	return 0;
}

int DataManager::GetScreenView(int t_ScreenID)
{
	return 0;
}
int DataManager::GetViewChannel(int t_ViewID)
{
	return 0;
}
int DataManager::GetSwitchInterval(void)
{
	return 0;
}
vector<int> DataManager::GetTouringCameras(int t_ViewID, int t_Index)
{
	int nStart = t_Index * m_ViewNumber;
	int nTotal = m_Channels[t_ViewID].size();

	int nLeft = nStart % nTotal;

	vector<int> channels;
	for (int i = 0; i < m_ViewNumber; i++)
	{
		if (nLeft >= nTotal)
		{
			nLeft = 0;
		}
		channels.push_back(m_Channels[t_ViewID][nLeft]);

		nLeft = nLeft + 1;
	}

	return channels;
}
vector<int> DataManager::GetCameraTourInfo(int t_ViewID, int t_ChannelID)
{
	vector<int> a;
	return a;
}
vector<int> DataManager::GetScreenIDs(void)
{
	vector<int> screenids;

	for (int i = 0; i < m_nTotalMonitor; i++)
	{
		screenids.push_back(i);
	}

	return screenids;
}
int DataManager::GetScreenInfo(int t_ScreenID, RECT* pScreenRect)
{
	RECT screenrect;

	screenrect = m_MonitorInfo[t_ScreenID].rcMonitor;
	*pScreenRect = screenrect;

	return 0;
}
string DataManager::GetCameraRtsp(int t_CameraID)
{
	if (m_Status != 0)
	{
		return "";
	}
	//g_CameraList[0].path = "rtsp://192.168.31.223:50010/live?camera=30&user=admin&pass=A1crUF4%3D&stream=3"; 
	string sUrl = "rtsp://";
	sUrl += m_ServerIP;
	sUrl += ":";
	sUrl += to_string(m_ServerPort);
	sUrl += "/live?camera=";
	sUrl += to_string(t_CameraID);
	sUrl += "&user=";
	sUrl += m_UserName;
	sUrl += "&pass=";
	sUrl += m_PassWord;
	sUrl += "&stream=2";

	//printf("GetCameraRtsp: input = %d, output = %s\n", t_CameraID, sUrl.data());

	return sUrl ;
}

void DataManager::WriteFile()
{
	ofstream outFile;
	outFile.open("d:\\temp\\junk.txt", ios::out | ios::trunc);

	for (int i = 0; i < 80; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			outFile << i * 10 + j << ",";
		}
		outFile << "\r";
	}

	outFile.close();
}

int DataManager::ReadChannelsFile(int nChannelGroup)
{
	char *pFileName = m_ChannelFile[nChannelGroup];
	vector<int>* pChannels = &(m_Channels[nChannelGroup]);

	string s;
	char buffer[256];
	ifstream inFile;
	inFile.open(pFileName, ios::in | ios::binary );

	if (false == inFile.is_open())
	{
		return ERROR_CHANNELSFILE_MISSING;
	}

	while (!inFile.eof())
	{
		getline(inFile,s);
		
		//cout << s << endl;
		if (s == "")
		{
			continue;
		}

		try
		{
			int nChannel = stoi(s);
			pChannels->push_back(nChannel);
		}
		catch (...) 
		{
			cout << "Caught Invalid Argument Exception\n";
			
			inFile.close();
			return ERROR_CHANNELSFILE_DATA_NOT_NUMBER;
		}
		
	}
	inFile.close();
	return 0;
}

// check if the given string is a numeric string or not
bool DataManager::chkNumber(string& str)
{
	return !str.empty() &&
		(str.find_first_not_of("[0123456789]") == std::string::npos);
}
// Function to split string str using given delimiter
vector<string> DataManager::split(string& str, char delim)
{
	auto i = 0;
	vector<string> list;
	auto pos = str.find(delim);
	while (pos != string::npos) {
		list.push_back(str.substr(i, pos - i));
		i = ++pos;
		pos = str.find(delim, pos);
	}
	list.push_back(str.substr(i, str.length()));
	return list;
}
// Function to validate an IP address
bool DataManager::validateIP(string ip)
{
	// split the string into tokens
	vector<string> slist = split(ip, '.');
	// if token size is not equal to four
	if (slist.size() != 4)
		return false;
	for (string str : slist) {
		// check that string is number, positive, and range
		if (!chkNumber(str) || stoi(str) < 0 || stoi(str) > 255)
			return false;
	}
	return true;
}

BOOL CALLBACK DataManager::EnumMonitor(HMONITOR handle, HDC hdc, LPRECT rect, LPARAM param)
{

	handle = (HMONITOR)this;

	DataManager *pm = (DataManager*)rect;

	MONITORINFOEX mi;
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(handle, &mi);
	//std::cout << "is MONITORINFOF_PRIMARY:" << mi.dwFlags << std::endl;
	//std::cout << "width:" << (mi.rcMonitor.right - mi.rcMonitor.left) << std::endl;
	//std::cout << "height" << (mi.rcMonitor.bottom - mi.rcMonitor.top) << std::endl;

	int nMonitorIndex = 0;
	for (int i = 0; i < pm->m_nTotalMonitor; i++)
	{
		if (pm->m_MonitorInfo[i].cbSize == 0)
		{
			nMonitorIndex = i;
			break;
		}
	}

	pm->m_MonitorInfo[nMonitorIndex] = mi;

	return true;

}

void DataManager::QueryMonitorSettings()
{
	m_nTotalMonitor = GetSystemMetrics(SM_CMONITORS);

	if (m_nTotalMonitor > 0)
	{
		m_MonitorInfo = new MONITORINFOEX[m_nTotalMonitor];

		for (int i = 0; i < m_nTotalMonitor; i++)
		{
			m_MonitorInfo[i].cbSize = 0;
		}
	}
	else
	{
		return;
	}

	union
	{
		MONITORENUMPROC FUNC;
		BOOL(CALLBACK DataManager::*EnumMonitor)(HMONITOR handle, HDC hdc, LPRECT rect, LPARAM param);
	} _proc;
	_proc.EnumMonitor = &DataManager::EnumMonitor;

	// 枚举当前的所有显示器
	EnumDisplayMonitors(NULL, NULL, _proc.FUNC, (LPARAM)this);

	return;
}
