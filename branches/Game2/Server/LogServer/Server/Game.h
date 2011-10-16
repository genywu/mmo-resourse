#pragma once

#include "../../../Public/Common/NetSessionManager.h"
#include "../../../Public/ServerPublic/Server/Common/AsynchronousExecute.h"

class LogManage;
class CServer;
class CDBCountry;
class CDataBlockAllocator;

class CMessage;
class CCostNetClient;
extern HWND g_hLogText;
extern HWND g_hPlayerListBox;

void RefeashInfoText(const char *pInfoText);					// 刷新服务器信息
void AddInfoText(char* msg, ...);								// 加入一条记录
void AddLogText(char* msg, ...);								// 加入一条记录
void AddErrorLogText(char* msg,...);							// 加入一条出错记录
void AddClientList(const char *str);
void DelClientList(const char *str);

enum eNetFlag
{
	NF_Server_Client=0x001,
	NF_LServer_Client=0x010,
	NF_CServer_Client=0x100,
};

//class CDBGoods;
// 游戏全局控制
class CGame
{
public:
	CGame(void);
	virtual ~CGame(void);

	BOOL Init();
	BOOL Release();
	BOOL MainLoop();

	
	//////////////////////////////////////////////////////////////////////////
	//	全局脚本变量 (进数据库)
	//////////////////////////////////////////////////////////////////////////
public:


private:


	//////////////////////////////////////////////////////////////////////////
	//	存盘 （操作以上三种需要进数据库的数据）
	//////////////////////////////////////////////////////////////////////////
public:


public:

private:




	//////////////////////////////////////////////////////////////////////////
	//	GameServer相关数据
	//////////////////////////////////////////////////////////////////////////
public:
	// GameServer信息
	struct tagGameServer
	{
		bool			bConnected;			// 该服务器是否已经连接上
		DWORD			dwIndex;			// 服务器编号
		string			strIP;				// IP
		DWORD			dwPort;				// 端口

		long			lReceivedPlayerData;	// 接收到的玩家数据数量(零时用)
	};

public:
	tagGameServer* GetGameServer(DWORD dwIndex)	{return (s_mapGameServer.find(dwIndex)!=s_mapGameServer.end())?&s_mapGameServer[dwIndex]:NULL;}
	tagGameServer* GetGameServer(const char* strIP, DWORD dwPort);			// 获取GameServer的数据
	tagGameServer* GetRegionGameServer(long lRegionID);						// 获取region所在的GameServer
	tagGameServer* GetPlayerGameServer(const CGUID& PlayerGuid);						// 获取player所在的GameServer

	long GetGameServerNumber_ByRegionID(long lRegionID);
	long GetGameServerNumber_ByPlayerID(const CGUID& PlayerGuid);


	bool LoadServerSetup(const char *filename);								// 读取游戏服务器配置
	long GetConnectedGameServerCount();										// 返回已经连接上的GameServer数量

	void OnGameServerConnect(DWORD dwIP, LONG lSocket);
	void OnGameServerClose(long lSocket);									//当某个GameServer断开连接的时候
	void OnGameServerLost(DWORD dwIndex);									// 当某个GameServer断开时的处理

	map<DWORD, tagGameServer>* GetGameServerList(){return &s_mapGameServer;}

private:
	map<DWORD, tagGameServer>		s_mapGameServer;	// 服务器列表<dwIndex, tagGameServer>
	set<DWORD>						m_setClientIP;

	BOOL ProcessMessage();
	bool ProcMenus();

	//////////////////////////////////////////////////////////////////////////
	//	公共接口
	//////////////////////////////////////////////////////////////////////////
public:                                          
	void SendMsg2GameServer(DWORD dwIndex,CMessage* pMsg);					// 发送一个消息到指定Index的GameServer
	void SendAllGameServer(CMessage* pMsg);					// 发送一个消息到所有GameServer


	AsynchronousExecute&	GetAsynchronousExecute(VOID){return m_AsynchronousExecute;};


private:

	DWORD	m_dwLastReLoginTime;											// 上次重连LoginServer的时间

	AsynchronousExecute		m_AsynchronousExecute;
	//////////////////////////////////////////////////////////////////////////
	//	网络部分
	//////////////////////////////////////////////////////////////////////////
public:
	BOOL InitNetServer(void);					//初始化游戏网络服务器端，建立网络服务器端，装载数据
	//BOOL StartConnLS(void);						//连接LoginServer
	bool StartAccept();							//开始作为服务器监听

	
	CServer* GetNetServer()	{return s_pNetServer;}

private:
	CDataBlockAllocator	*m_pDBAllocator;
	CServer				*s_pNetServer;			// 做为GameServer的服务器端
	long				m_lLogingSocket;		//连接loginserver的客户端
	long				m_lCostSocket;			//连接costserver的客户端

	

	/////////////////////////////////////////////////////////////////////////
	// 游戏配置
	/////////////////////////////////////////////////////////////////////////
public:
	struct tagSetup
	{
		DWORD	dwNumber;			// 服务器编号(0-7)
		string	strName;			// 服务器名字
		DWORD	dwListenPort;		// 诊听Port
		//string	strSqlConType;		//sql server connection type
		//string	strSqlServerIP;		//sql server ip address
		//string  strSqlUserName;		//sql server user name
		//string  strSqlPassWord;		//password
		//string  strDBName;			//database name

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

		DWORD	dwReleaseLoginPlayerTime;			// 释放LoginList里的Player的超时时间


		DWORD	dwMinDbOptThreadNum;	//! 最小DB操作线程保持数
		DWORD	dwMaxDbOptThreadNum;	//! 最大DB操作线程数
		DWORD	dwThreadNumAdjustSpace;	//! 自动调整线程数的间隔时间

		// 日志系统数据库服务器配置
		//bool	bUseLogSys;			//是否起用日志系统的数据库服务器
		//string	strLogSysProvider;	
		//string	strLogSysServer;
		//string	strLogSysDatabase;
		//string	strLogSysUID;
		//string	strLogSysPWD;

		tagSetup()
		{
			dwRefeashInfoTime = 1000;
			dwSaveInfoTime = 60000;
			//bUseLogSys = false;
		}
	};

private:
	tagSetup	m_Setup;
	
public:
	bool		LoadSetup();
	bool		LoadServerResource();
	tagSetup*	GetSetup()	{return &m_Setup;}
	//CDupliRegionSetup* GetDupliRegionSetup()	{return m_pDupliRegionSetup;}

	

	//////////////////////////////////////////////////////////////////////////
	//	定时ping
	//////////////////////////////////////////////////////////////////////////
public:

	// GameServer信息
	struct tagPingGameServerInfo
	{
		string	strIP;
		DWORD	dwPort;
		DWORD	dwPlayerNum;
	};

	void AddPingGameServerInfo(tagPingGameServerInfo& st)	{m_vectorPingGameServerInfo.push_back(st);}
	void ClearPingGameServerInfo()							{m_vectorPingGameServerInfo.clear();}
	void SetInPing(bool b)									{m_bInPing=b;}
	void SetLastPingGameServerTime(DWORD dw)				{m_dwLastPingGameServerTime=dw;}

private:

	vector<tagPingGameServerInfo>	m_vectorPingGameServerInfo;
	bool	m_bInPing;							// 是否在Ping的过程中
	DWORD	m_dwLastPingGameServerTime;			// 上次发起ping的时间

	//////////////////////////////////////////////////////////////////////////
	// 异步从数据库读取玩家信息
	//////////////////////////////////////////////////////////////////////////

private:

public:

	//void ProcessPlayerDataQueue();

	///////////////////////////////
	// 日志队列
	///////////////////////////////
private:
	//! 游戏日志管理对象
	LogManage *m_pGameLogManage;
	//!	响应菜单中的创建表调用，设立标志是为了在主线程中进行所有数据库操作
	BOOL	m_bCreateTable;
	//CWriteLogQueue m_qWriteLogData;
public:
	void OnClientLogin(CMessage *pMsg);
	void OnLog(CMessage *pMsg);
	void OnStrSqlLog(CMessage *pMsg);
	void CreateTableInDB(void);
	void CreateTableInSqlFile(void);
	void OutTableInfoToFile(void);
	void ReLoadConfig(void);
	//CWriteLogQueue* GetWriteLogDataQueue()			{ return &m_qWriteLogData; }


};// end of class


BOOL CreateGame();			// 创建游戏
CGame* GetGame();			// 获取游戏
BOOL DeleteGame();			// 释放游戏

unsigned __stdcall GameThreadFunc(void* pArguments);	// 游戏主线程


extern HANDLE g_hGameThreadExitEvent;		// 主线程退出事件
extern bool	g_bGameThreadExit;				// 是否退出主线程

// 显示存盘信息开关
extern bool g_bShowSaveInfo;

void ShowSaveInfo(LPCSTR szInfo, ...);
