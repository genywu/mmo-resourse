#pragma once

#include "../Net/Message.h"
#include "../../../Public/Common/NetSessionManager.h"

class CServer;
class CEntity;
class CEntityGroup;
class CRsEntity;

class CDupliRegionSetup;
class CDataBlockAllocator;

class CVariableList;
class CCostNetClient;
class CEntityManager;
class DBAdoBase;
class DBThread;
class CMapFile;

struct tagEnemyFaction;
struct tagVilWarSetup;
struct tagAttackCityTime;
struct ThreadData;

extern CRITICAL_SECTION g_CriticalSectionSavePlayerList;	// 待存盘玩家的线程同步(GameThread <-> SaveThread)
extern CRITICAL_SECTION g_CriticalSectionSaveThread;		// 存盘线程同步(SaveThread <-> SystemThread)

enum eNetFlag
{
	NF_Server_Client=0x001,
	NF_LServer_Client=0x010,
	NF_CServer_Client=0x100,
	NF_DBServer_Client=0x1000,
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
	BOOL AI();
	BOOL ProcessMessage();
	BOOL InitStackFileName();
	BOOL InitMainStackFileName();

	//////////////////////////////////////////////////////////////////////////
	//	WorldServer相关数据
	//////////////////////////////////////////////////////////////////////////
public:
	// WorldServer信息
	struct tagWorldServer
	{
		bool			bConnected;			// 该服务器是否已经连接上
		DWORD			dwIndex;			// 服务器编号
		string			strIP;				// IP
		DWORD			dwPort;				// 端口
	};

public:
	tagWorldServer* GetWorldServer(DWORD dwIndex)	{return (s_mapWorldServer.find(dwIndex)!=s_mapWorldServer.end())?&s_mapWorldServer[dwIndex]:NULL;}
	tagWorldServer* GetWorldServer(const char* strIP, DWORD dwPort);			// 获取GameServer的数据
	bool LoadWSSetup(const char *filename);										// 读取WS游戏服务器配置
	long GetConnectedWorldServerCount();										// 返回已经连接上的WS数量
	bool OnWorldServerClose(DWORD dwIndex);									//当某个WS断开连接的时候
	bool OnWorldServerLost(DWORD dwIndex);									// 当某个WS断开时的处理

	map<DWORD, tagWorldServer>* GetWorldServerList(){return &s_mapWorldServer;}

private:
	map<DWORD, tagWorldServer>		s_mapWorldServer;	// 服务器列表<dwIndex, tagWorldServer>
    bool ProcMenus();

	//////////////////////////////////////////////////////////////////////////
	//	数据库相关
	//////////////////////////////////////////////////////////////////////////
public:	
	CRsEntity* GetRsEntity()					{return m_pRsEntity;}  
	
private:
	CRsEntity			*m_pRsEntity;				// 对象
	
	/////////////////////////////////////////////////////////////////////////
	// WorldServer信息列表
	/////////////////////////////////////////////////////////////////////////
public:
	struct tagWorldInfo
	{
		string	strName;						// 世界名字
		long	lStateLvl;						// 统计服务器的状态0:维护，1:0-75%,2:75-90%,3:90%-100%
	};

private:
	map<long,tagWorldInfo>		m_listWorldInfo;

	map<long,tagWorldInfo>		m_WorldInfoSetup;//配置文件中的WorldServer信息
public:

	bool LoadWorldSetup();
	void SetListWorldInfoBySetup();

	bool IsExitWorld(const char* strWorldName);
	long GetWorldNum()					{return (long)m_listWorldInfo.size();}

	void AddWorldInfoToMsg(CMessage& msg,string &strCDkey);

	long AddWorld(long lWorldNumber,const char* strWorldName);			// 添加世界
	long DelWorld(long lNum);	// 删除世界

	bool WorldServerIsOpenState(long lNum);

	long GetWorldSateLvl(long lPlayerNum);
	void OnChangeStateLvl(long lWorldNum,long lPlayerNum);
	void ChangeAllWorldSate();
	const char *GetWorldNameByID(long lNum);
	long  GetWorldIDByName(const char* strWorldName);
	bool SendMsg2World(CMessage* pMsg,long lWorldServerID);	//发送消息给指定的WORLDSERVER

	//////////////////////////////////////////////////////////////////////////
	//	公共接口
	//////////////////////////////////////////////////////////////////////////
public:
	void SendMsg2WS(DWORD dwIndex,CMessage* pMsg);					// 发送一个消息到指定Index的S
	//! [2007-7-19 add]
	void SendAllWS(CMessage* pMsg);					// 发送一个消息到所有WS
	//! [2007-7-19 add end]

	DWORD GetAITick()							{return s_lAITick;}
	void AddAITick()							{s_lAITick++;}

private:
	DWORD	s_lAITick;														// 游戏AI从启动到现在共运行的次数
	DWORD	m_dwLastReLoginTime;											// 上次重连LoginServer的时间
	//////////////////////////////////////////////////////////////////////////
	//	网络部分
	//////////////////////////////////////////////////////////////////////////
public:
	BOOL InitNetServer(void);					//初始化游戏网络服务器端，建立网络服务器端，装载数据
	bool StartAccept();							//开始作为服务器监听

	
	CServer* GetNetServer()	{return s_pNetServer;}

	bool	LoadMapFileSetup();

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
		string	strLoginIP;			// LoginServerIP
		DWORD	dwLoginPort;		// LoginServerPort
		DWORD	dwListenPort;		// 诊听Port
		string	strSqlConType;		//sql server connection type
		string	strSqlServerIP;		//sql server ip address
		string  strSqlUserName;		//sql server user name
		string  strSqlPassWord;		//password
		string  strDBName;			//database name

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


		DWORD dwTravleMaxSize; // 主线程每次遍历更新列表的最大深度
		DWORD dwPlayerLoadThreadNumber;	
		DWORD dwPlayerSaveThreadNumber;
		DWORD dwPingWorldServerTime;
		DWORD dwPingWorldServerErrorTime;
		DWORD dwClearPlayerDataTime;

		DWORD dwViewDbOperDeltaTime; // 显示数据库操作的时间阙值
		DWORD dwGappID;				 //映射文件客户端ID
        bool  bOpenRanks;               // 排行榜开关
        DWORD dwRefreshRanksTime;       // 排行数据刷新时间间隔(分钟)
		tagSetup()
		{
			dwRefeashInfoTime = 1000;
			dwSaveInfoTime = 60000;
		}

	};

private:
	tagSetup	m_Setup;
	
public:
	bool		LoadSetup();
	bool		LoadServerResource();
	tagSetup*	GetSetup()	{return &m_Setup;}

public:
	//系统广播数据结构
	struct tagSysBroadcast
	{
		long	lImportLevel;		//重要度为0的信息只出现在聊天框内，为1的信息会出现在屏幕上方的GM公告位置
		long	lRegionID;			//场景如果为0就是全世界广播
		DWORD	dwMinTime;			//小间隔时间长度
		DWORD	dwMaxTime;			//大间隔时间长度
		DWORD	dwOdds;				//每条信息可以指定出现的几率，100分之N
		DWORD	dwTextColor;		//字体颜色
		DWORD	dwBackColor;		//背景条颜色（如果指定255，255，0，255那么不显示背景条）
		string	strMessage;			//广播内容

		DWORD	dwIntervalTime;		// 实际的间隔时间
		DWORD	dwLastNotifyTime;	// 上一次发送出去的时间
	};

	list<tagSysBroadcast>* GetBroadcastList(){return &m_listBroadcast;}

private:
	list<tagSysBroadcast> m_listBroadcast;			//系统广播内容列表

	//////////////////////////////////////////////////////////////////////////
	//	定时ping
	//////////////////////////////////////////////////////////////////////////
public:
	// WorldServer信息
	struct tagPingWorldServerInfo
	{
		string	strIP;
		DWORD	dwPort;
		DWORD	dwPlayerNum;
		vector<tagPingWorldServerInfo>	vectorGameServerInfo;
	};

	void AddPingWorldServerInfo(tagPingWorldServerInfo& st)	{m_vectorPingWorldServerInfo.push_back(st);}

private:
	vector<tagPingWorldServerInfo>	m_vectorPingWorldServerInfo;
	
private:
	bool m_bInPing; 
	DWORD m_dwLastPingGameServerTime;

public:
	CEntityManager* GetEntityManager(void);

	long GetWSSocket(void) { return m_lWSSocket; }
	void SetWSSocket(long s) { m_lWSSocket = s; }

	// 发送数据库配置信息
	void SendDBTableInfo(long socketID);

	void Test3GoodsImageDataBuild();
	void Test3MailGoodsDataBuild();

private:
	CEntityManager* p_DBEntityManager;
	long m_lWSSocket;

	//内存影射文件
	CMapFile	*m_pMapFile;
	//没次运新产生的记录堆栈的文件名字
	char m_pszRunStackFile[MAX_PATH];
	char m_pszMainRunStackFile[MAX_PATH];

	char m_pszThreadRunStackFile[2048];

public:
	bool GetSaveStartFlag(void) { return m_bIsSaveStartFlag; }
	void SetSaveStartFlag(bool flag) { m_bIsSaveStartFlag = flag; }
	const char* GetStatckFileName();
	const char* GetMainStatckFileName()	{return m_pszMainRunStackFile;}

	bool m_bIsSaveStartFlag;

};// end of class


BOOL CreateGame();			// 创建游戏
CGame* GetGame();			// 获取游戏
BOOL DeleteGame();			// 释放游戏

unsigned __stdcall GameThreadFunc(void* pArguments);	// 游戏主线程
unsigned __stdcall ConnectLoginServerFunc( void* pArguments );	// 连接dbserver的线程、当dbserver断开时启用，连同时候关闭
unsigned __stdcall ConnectCostServerFunc( void* pArguments );	// 


unsigned __stdcall ProcessWriteLogDataFunc(void* pArguments);	// 写入日志线程

unsigned __stdcall LoadPlayerDataFromDB(void* pArguments);			// 玩家载入数据线程

extern HANDLE g_hGameThreadExitEvent;		// 主线程退出事件
extern bool	g_bGameThreadExit;				// 是否退出主线程

void DoSaveData();
void DoSaveLog();

extern long g_ThreadCountID;
// 定时刷新信息
static long lConnects;
static long lMapAccounts;
static long lMapScripts;
static long lMapProcs;
static long lMapPlayers;
static long lPlayers;
static long lOfflinePlayers;
static long lLoginPlayers;
static long lCreatePlayers;
static long lDeletePlayers;
static long lResumePlayers;
static long lOperingLoadEntitys;
static long lOperingSaveEntitys;
static long lTeams;
static long lLargess;
static long lWriteLog;
static long lLoadPlayer; 
static long lMails;
static long lFactions;
static long lBeginLoadNum;
static long lBeginSaveNum;
static long lEndNum;