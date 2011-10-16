#pragma once

class CServerForGS;
class CMessage;
class CBillingPlayerManager;

class CGame
{
public:
	CGame(void);
	~CGame(void);

public:
	bool Init();										// 游戏初始化
	bool InitServer();									// 初始化游戏网络服务器端，建立网络服务器端，装载数据
	bool InitBPManager();								// Create BillingPlayerManager threads
	bool ReleaseBPManager();							// Realse BillingPlayerManager threads
	bool Release();										// 游戏释放
	bool ProcessMessage();								// 消息处理
	bool MainLoop();									// 游戏主循环

public:
	// Setup.ini
	struct tagSetup
	{
		DWORD	dwGSListenPort;							// GS侦听端口
		//string	strSqlConType;							// sql server connection type
		string	strSqlServerIP;							// sql server ip address
		string  strSqlUserName;
		string  strSqlPassWord;
		string  strDBName;

		bool	bLogSvrSwitch;							// local log server switch
		string	strLogSvrIP;							// sql server ip address
		string  strLogSvrUsrName;
		string  strLogSvrPwd;
		string  strLogDBName;

		DWORD	dwRefeashInfoTime;						// 刷新显示时间
		DWORD	dwSaveInfoTime;							// 保存一次信息的间隔时间
		DWORD	dwSaveLogSvrTime;						// Save logs for incrementshop

		DWORD	dwDBIOThreadNum;						// database i/o threads' num
	};

public:
	// GameServer信息
	CServerForGS		*GetServerForGS()				{ return m_pGSServer; }
	tagSetup			*GetSetup()						{ return &m_Setup; }
	bool				LoadSetup();
	bool				ReLoadSetup();
	bool				LoadGSSetup();
	vector<CBillingPlayerManager*> vecBPM;

private:
	tagSetup			m_Setup;
	CServerForGS		*m_pGSServer;					// 作为GS的服务器端
	bool				m_bExit;						// 退出游戏
};

extern HWND 			g_hWndInfo;
extern HWND 			g_hWndLog;
extern HWND 			g_hWndGSList;
extern HANDLE			g_hGameThreadExitEvent;			// 主线程退出事件
extern bool				g_bGameThreadExit;				// 是否退出主线程

bool CreateGame();										// 创建游戏
CGame* GetGame();										// 获取游戏
bool DeleteGame();										// 释放游戏
void RefreashInfo();									// 刷新服务器信息
void AddLogText(char* msg, ...);						// 加入一条记录
void SaveLogText(bool bForce);							// 保存记录
void AddCdkeyList(const char *str);
void DelCdkeyList(const char *str);
unsigned __stdcall GameThreadFunc(void* pArguments);	// 游戏主线程