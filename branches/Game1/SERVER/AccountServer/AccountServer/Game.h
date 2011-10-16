#pragma once

//! 启用防沉迷
//#define ENABLE_FCM

#include "../public/UseMemManageAS.h"
#include "..\public\AsynchronousExecute.h"
#include "..\AppAccount\ClientManager.h"


class CServer;
class CMessage;
class CMyAdoBase;
class CMySocket;
class CDataBlockAllocator;
extern HWND g_hLogText;
extern HWND g_hWorldListBox;
extern HWND g_hCdkeyListBox;

void RefeashInfoText();					// 刷新服务器信息
void AddErrorLogText(char* msg, ...);	// 加入一条出错记录
void AddLogText(char* msg, ...);		// 加入一条记录
void SaveLogText(bool bForce);			// 保存记录
void AddLoginClientList(const char *str);
void DelLoginClientList(const char *str);

enum eNetFlag
{
	NF_AsscountServer_Client=0x100000,
};
// 游戏全局控制
class CGame
{
public:
	CGame(void);
	virtual ~CGame(void);

public:
	BOOL Init();
	BOOL Release();
	BOOL MainLoop();

	BOOL InitNetServer_Client(void);			//初始化针对Client的网络服务器端，建立网络服务器端，装载数据
	BOOL AI();
	BOOL ProcessMessage();
	VOID DisConnClient(LONG lSocket);

/////////////////////////////////////////////////////////////////////////
// 游戏全局接口
/////////////////////////////////////////////////////////////////////////
public:
	struct tagCdkey
	{
		string	strCdkey;			// cdkey
		long	lWorldNumber;		// 所在World编号
	};

private:
	DWORD	s_lAITick;								// 游戏AI从启动到现在共运行的次数

	//数据块分配器
	CDataBlockAllocator	*m_pDBAllocator;
	//负责接受客户端消息的网络服务
	CServer	*s_pNetServer_Client;

public:

	DWORD GetAITick()							{return s_lAITick;}
	void AddAITick()							{s_lAITick++;}

	CServer* GetNetServer_Client()	{return s_pNetServer_Client;}

	
/////////////////////////////////////////////////////////////////////////
// 线程管理
/////////////////////////////////////////////////////////////////////////
public:
	HANDLE m_hEndEvent[2];				// 结束事件
	bool m_bExit;						// 退出游戏

	AsynchronousExecute&					GetAsynchronousExecute(VOID)	{return m_AsynchronousExecute;}
	string&									GetConnectionString(VOID)		{return m_strConnectionString;}
	string&									GetFcmConnectionString(VOID)	{return m_strFcmConnectionString;}
	string&									GetSecurityConnectionString(VOID)	{return m_strSecurityConnectionString;}

private:
	//!									异步执行对象
	AsynchronousExecute					m_AsynchronousExecute;
	//!
	string								m_strConnectionString;
	string								m_strFcmConnectionString;
	string								m_strSecurityConnectionString;

/////////////////////////////////////////////////////////////////////////
// 服务器逻辑
/////////////////////////////////////////////////////////////////////////
public:
	ClientManager&						GetClientManager(VOID) {return m_ClientManager;}
	VOID								ReLoadLsSetup(VOID){m_bReLoadLsSetup = TRUE;}

private:
	//!									客户端信息管理对象
	ClientManager						m_ClientManager;
	BOOL								m_bReLoadLsSetup;
/////////////////////////////////////////////////////////////////////////
// 游戏配置
/////////////////////////////////////////////////////////////////////////
public:
	struct tagSetup
	{
		LONG	lServerID;				//! AS自己的ID
		DWORD	dwListenPort_Client;	// 诊听Client的Port

		string	strSqlConType;			//sql server connection type
		string	strSqlServerIP;			//sql server ip address
		string  strSqlUserName;
		string  strSqlPassWord;
		string  strDBName;

		string	strSecuritySqlConType;			
		string	strSecuritySqlServerIP;
		string  strSecuritySqlUserName;
		string  strSecuritySqlPassWord;
		string  strSecurityDBName;

		string	strFcmSqlConType;			
		string	strFcmSqlServerIP;
		string  strFcmSqlUserName;
		string  strFcmSqlPassWord;
		string  strFcmDBName;

		//Client网络服务配置参数
		DWORD	dwDataBlockNum;			//数据块数量
		DWORD	dwDataBlockSize;		//数据块大小
		DWORD	dwFreeMsgNum;			//预分配的消息数量

		DWORD	dwFreeSockOperNum;		//网络命令操作预分配数量
		DWORD	dwFreeIOOperNum;		//完成端口上IO操作预分配数量
		long	lIOOperDataBufNum;		//默认IO操作的DataBuf数量
		bool	bCheckNet;				// 是否对网络进行检测
		DWORD	dwBanIPTime;			// 禁止IP的时间
		DWORD	dwMaxMsgLen;			// 允许传输的最大消息长度		
		long	lMaxConnectNum;			// 客户端的最大连接数量
		long	lMaxClientsNum;			// 最大的客户端发送缓冲区大小
		long	lPendingWrBufNum;		// 最大的发送IO操作Buf总大小
		long	lPendingRdBufNum;		// 最大的接受IO操作Buf总大小
		long	lMaxSendSizePerSecond;	// 向客户端每秒发送的最大字节数
		long	lMaxRecvSizePerSecond;	// 从客户端接受的每秒最大字节数
		long	lMaxBlockedSendMsgNum;	// 最大阻塞的发送消息数量
		long	lConPendingWrBufNum;	// 客户端最大的发送IO操作Buf总大小
		long	lConPendingRdBufNum;	// 客户端最大的接受IO操作Buf总大小
		long	lConMaxSendSizePerSecond;//向服务器发送的每秒最大字节数
		long	lConMaxRecvSizePerSecond;//从服务器接受的每秒最大字节数
		long	lConMaxBlockedSendMsgNum;// 最大阻塞的发送消息数量

		DWORD	dwRefeashInfoTime;		// 刷新显示时间
		DWORD	dwSaveInfoTime;			// 保存一次信息的时间间隔
		DWORD	dwDoQueueInter;			// 处理等待队列的间隔时间
		DWORD	dwSendMsgToQueInter;	// 给等待队列发送消息的间隔时间

		DWORD	dwWorldMaxPlayers;		// 登陆WorldServer的最大人数
		float	fWorldBusyScale;		// 繁忙的人数比例
		float	fWorldFullScale;		// 爆满的人数比例

		DWORD	dwPingWorldServerTime;	// 定时PingWorldServer的间隔
		DWORD	dwPingWorldServerErrorTime;	// PingWorldServer到收到返回的出错时间间隔

		bool	bCheckForbidIP;			// 是否开启检测IP禁用
		bool	bCheckAllowIP;			// 是否开启IP允许检测
		bool	bCheckBetweenIP;		// 检测ip是否在允许IP内

		DWORD	dwMinDbOptThreadNum;	//! 最小DB操作线程保持数
		DWORD	dwMaxDbOptThreadNum;	//! 最大DB操作线程数
		DWORD	dwThreadNumAdjustSpace;	//! 自动调整线程数的间隔时间

		tagSetup()
		{
			bCheckNet = 1;
			dwMaxMsgLen = 102400;
			dwBanIPTime = 10;
			dwRefeashInfoTime = 1000;
			dwSaveInfoTime = 60000;
		}
	};

	struct tagSetupEx
	{
		long	lClientMaxBlockConNum;
		long	lClientValidDelayRecDataTime;
	};

private:
	tagSetup	m_Setup;
	tagSetupEx	m_SetupEx;

public:
	bool		LoadSetup();
	bool		ReLoadSetup();
	bool		LoadSetupEx();
	bool		ReLoadSetupEx();


	tagSetup*	GetSetup()	{return &m_Setup;}
	tagSetupEx* GetSetupEx(){return &m_SetupEx;}

};

BOOL CreateGame();			// 创建游戏
CGame* GetGame();			// 获取游戏
BOOL DeleteGame();			// 释放游戏

unsigned __stdcall GameThreadFunc(void* pArguments);	// 游戏主线程

extern HANDLE g_hGameThreadExitEvent;		// 主线程退出事件
extern bool	g_bGameThreadExit;				// 是否退出主线程