#pragma once

#include "..\..\..\LogClient\bs\LogBSClient.h"

class CServer;
class CMessage;
class CRsAccount;
class CRsLogInfo;
class CMyAdoBase;
class CMySocket;


class CDataBlockAllocator;
class CBillDBManager;
extern HWND g_hLogText;
extern HWND g_hWorldListBox;
extern HWND g_hCdkeyListBox;

void RefeashInfoText();					// 刷新服务器信息
void AddLogText(char* msg, ...);		// 加入一条记录
void SaveLogText(bool bForce);			// 保存记录


enum eNetFlag
{
	NF_Server_Client=0x01,
	NF_Server_World=0x10,
	NF_Log_Client = 0x100,
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

	BOOL InitNetServer_GS(void);				//初始化针对WorldServer的网络服务器端，建立网络服务器端，装载数据
	BOOL AI();
	BOOL ProcessMessage();

	void	CreateConnectLogThread(void);

	

/////////////////////////////////////////////////////////////////////////
// 游戏全局接口
/////////////////////////////////////////////////////////////////////////
public:
	struct tagCdkey
	{
		string	strCdkey;			// cdkey
		long	lWorldNumber;		// 所在World编号
	};

	// 其他连接

	CRsLogInfo *m_pRsLogInfo;						// 用于写入登录日志 logInfo 表
private:
	DWORD	s_lAITick;								// 游戏AI从启动到现在共运行的次数

    CRsAccount *m_pRsAccount;							
	
	std::map<string,int>			m_mapPWError;			// 用于密码错误的帐号
	
	long					m_StateLvl[4];
	

	//数据块分配器
	CDataBlockAllocator	*m_pDBAllocator;

	CBillDBManager* m_pBillDBManager;
	
	//负责接受GameServer消息的网络服务
	CServer	*s_pNetServer_GS;
	// 已通过QUEST_PLAYER_DATA的玩家
	map<string, long> m_PassDetailDataCdkey;


public:
	map<string, long/*GSID*/>& GetPassDetailDataCdkeyMap(void) { return m_PassDetailDataCdkey; }	

	DWORD GetAITick()							{return s_lAITick;}
	void AddAITick()							{s_lAITick++;}
	
	CServer* GetNetServer_GS()	{return s_pNetServer_GS;}
	CRsAccount* GetRsAccount() {return m_pRsAccount;}
	

	bool KickOut(const char *szCdkey);			// 踢出一个CDKEY
	std::map<string,int>* GetPWErrorMap()		{return &m_mapPWError;}

	CBillDBManager* GetBillDBManager() {return m_pBillDBManager;}

	
	
/////////////////////////////////////////////////////////////////////////
// 线程管理
/////////////////////////////////////////////////////////////////////////
public:
	HANDLE m_hEndEvent[2];				// 结束事件
	bool m_bExit;						// 退出游戏

/////////////////////////////////////////////////////////////////////////
// WorldServer信息列表
/////////////////////////////////////////////////////////////////////////
public:
	struct tagGameServerInfo
	{
		bool bConnected;	//是否连接
		DWORD dwIndex;		//编号
		string	strName;	//GameServer名字
		string  strIp;		//GS IP
		DWORD dwPort;		//GS PORT
		long	lStateLvl;	// 统计服务器的状态0:维护，1:0-75%,2:75-90%,3:90%-100%

		tagGameServerInfo ()
		{
			bConnected=false;
			dwIndex=0;
			strName="";
			strIp="";
			dwPort=0;
			lStateLvl=0;
		}
	};

private:
	map<long,tagGameServerInfo>		m_listGSInfo;

	map<long,tagGameServerInfo>		m_GSInfoSetup;//配置文件中的WorldServer信息

	LogBSClient			m_LogBSClient;
public:
	//从文件读取GameServer配置
	bool LoadGSSetup();

	void SetListWorldInfoBySetup();

	void OnLOGSLost();

	bool IsExitWorld(const char* strWorldName);
	long GetWorldNum()					{return (long)m_listGSInfo.size();}

	void AddWorldInfoToMsg(CMessage& msg,string &strCdkey);

	tagGameServerInfo* GetGameServer(const char* strIP, DWORD dwPort);	

	long AddGS(long lGSNumber,const char* strGSName);			//添加GAMESERVER
	long DelGS(long lNum);	// 删除GAMESERVER

	bool WorldServerIsOpenState(long lNum);

	long GetWorldSateLvl(long lPlayerNum);
	void OnChangeStateLvl(long lWorldNum,long lPlayerNum);
	void ChangeAllWorldSate();
	const char *GetGSNameByID(long lNum);
	long  GetGSIDByName(const char* strWorldName);
	bool SendMsg2World(CMessage* pMsg,long lWorldServerID);	//发送消息给指定的WORLDSERVER	

	void UpdateDisplayWorldInfo();

	void OnNetGSClose(long lGSID);

	//!		响应DB信息
	void	InitLogClient(CMessage *pMsg);
	//!		响应确认信息
	void	LogServerAffirm(CMessage *pMsg);
	//!		响应筛选条件信息
	void	LogServerCondition(CMessage *pMsg);

	LogBSClient&			GetLogBSClient(void){return m_LogBSClient;}

	BOOL ConnectLogServer();

	bool ReConnectLogServer();

/////////////////////////////////////////////////////////////////////////
// 游戏配置
/////////////////////////////////////////////////////////////////////////
public:
	struct tagSetup
	{
		DWORD	dwListenPort_GameServer;	//监听GameServer的端口号		
		string	strSqlConType;				//数据库连接类型
		string	strSqlServerIP;				//数据库 IP地址
		string  strSqlUserName;				//数据库连接用户名
		string  strSqlPassWord;				//数据库连接密码
		string  strDBName;					//数据库DB名
		bool	bUseLogServer;
		string	strLogIP;
		DWORD   dwLogPort;
		DWORD   dwLogClientNo;
		DWORD	dwWorkerThreadNum;			//工作者线程数量
		bool	bUseSystemRollback;			//是否使用系统回滚
		DWORD   dwSystemRollbackTime;		//系统回滚时间
		DWORD	dwRollbackLockTime;			//回滚冻结时间

		//Client网络服务配置参数
		DWORD	dwDataBlockNum;			//数据块数量
		DWORD	dwDataBlockSize;		//数据块大小
		DWORD	dwFreeMsgNum;			//预分配的消息数量

		//GS网络服务配置参数
		DWORD	dwGSFreeSockOperNum;	// 网络命令操作预分配数量
		DWORD	dwGSFreeIOOperNum;	// 完成端口上IO操作预分配数量
		long	lGSIOOperDataBufNum;	// 默认IO操作的DataBuf数量
		bool	bGSCheckNet;			// 是否对网络进行检测
		DWORD	dwGSBanIPTime;		// 禁止IP的时间
		DWORD	dwGSMaxMsgLen;		// 允许传输的最大消息长度		
		long	lGSMaxConnectNum;		// 客户端的最大连接数量
		long	lGSMaxClientsNum;		// 最大的客户端发送缓冲区大小
		long	lGSPendingWrBufNum;		// 最大的发送IO操作Buf总大小
		long	lGSPendingRdBufNum;		// 最大的接受IO操作Buf总大小
		long	lGSMaxSendSizePerSecond;	// 向客户端每秒发送的最大字节数
		long	lGSMaxRecvSizePerSecond;	// 从客户端接受的每秒最大字节数
		long	lGSMaxBlockedSendMsgNum;	// 最大阻塞的发送消息数量
		long	lGSConPendingWrBufNum;	// 客户端最大的发送IO操作Buf总大小
		long	lGSConPendingRdBufNum;	// 客户端最大的接受IO操作Buf总大小
		long	lGSConMaxSendSizePerSecond;//向服务器发送的每秒最大字节数
		long	lGSConMaxRecvSizePerSecond;//从服务器接受的每秒最大字节数
		long	lGSConMaxBlockedSendMsgNum;	// 最大阻塞的发送消息数量

		DWORD	dwRefeashInfoTime;		// 刷新显示时间
		DWORD	dwSaveInfoTime;			// 保存一次信息的时间间隔
		DWORD	dwDoQueueInter;			// 处理等待队列的间隔时间
		DWORD	dwSendMsgToQueInter;	// 给等待队列发送消息的间隔时间		

		DWORD	dwGSMaxPlayers;		// 登陆WorldServer的最大人数
		float	fGSBusyScale;		// 繁忙的人数比例
		float	fGSFullScale;		// 爆满的人数比例

		DWORD	dwPingGSTime;	// 定时PingWorldServer的间隔
		DWORD	dwPingGSErrorTime;	// PingWorldServer到收到返回的出错时间间隔

		bool	bCheckForbidIP;			// 是否开启检测IP禁用
		bool	bCheckAllowIP;			// 是否开启IP允许检测
		bool	bCheckBetweenIP;		// 检测ip是否在允许IP内

		tagSetup()
		{			
		}
	};

	
private:
	tagSetup	m_Setup;

public:
	bool		LoadSetup();
	bool		ReLoadSetup();
	bool		ReLoadWorldSetup();
	tagSetup*	GetSetup()	{return &m_Setup;}	

//////////////////////////////////////////////////////////////////////////
//	定时ping
//////////////////////////////////////////////////////////////////////////
public:

	// WorldServer信息
	struct tagPingWorldServerInfo
	{
		// GameServer信息
		struct tagPingGameServerInfo
		{
			string	strIP;
			DWORD	dwPort;
			DWORD	dwPlayerNum;
		};

		string	strIP;
		DWORD	dwPort;
		DWORD	dwPlayerNum;
		vector<tagPingGameServerInfo>	vectorGameServerInfo;
	};

	void AddPingWorldServerInfo(tagPingWorldServerInfo& st)	{m_vectorPingWorldServerInfo.push_back(st);}

private:
	vector<tagPingWorldServerInfo>	m_vectorPingWorldServerInfo;

public:	
	int		EnterGame( const char *szCdkey, DWORD dwIP, long lSockID, const char *szWSName, bool bLimit=false, bool bMobileUser=false );
};

BOOL CreateGame();			// 创建游戏
CGame* GetGame();			// 获取游戏
BOOL DeleteGame();			// 释放游戏

unsigned __stdcall GameThreadFunc(void* pArguments);	// 游戏主线程

extern HANDLE g_hGameThreadExitEvent;		// 主线程退出事件
extern bool	g_bGameThreadExit;				// 是否退出主线程


//////////////////////////////////////////////////////////////////////////
// 服务器状态日志部分
//////////////////////////////////////////////////////////////////////////
void AppendServerInfoLog(vector<CGame::tagPingWorldServerInfo> &vWorldServerInfo);
extern CRITICAL_SECTION g_csServerInfoLog;
extern HANDLE g_hServerInfoLog;

extern vector<CGame::tagPingWorldServerInfo> g_vWorldServerInfo;
//extern _ConnectionPtr g_SerInfoLogConn;
//////////////////////////////////////////////////////////////////////////


unsigned __stdcall UpdateOnlineUser2DB(void *param);

unsigned __stdcall		ConnectLogServerFunc( void* pArguments );