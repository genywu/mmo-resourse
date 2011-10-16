#pragma once

#include "../App/AccountClient.h"
#include "../App/COnCmdConnect.h"
#include "../App/CSdoaFcm.h"
#include "../App/CSdoaValidate.h"
#include "../App/LoaclAccountValidate.h"
#include "../App/LoginManager.h"

#include "../Net/AinMessage.h"
#include "../Net/Message.h"

#include "../../../Public/ServerPublic/Greta/regexpr2.h"
#include "../../../Public/ServerPublic/Server/Common/AsynchronousExecute.h"
#include "../../../Public/AppFrame/TextRes.h"
using namespace AppFrame;

using namespace regex;

class CServer;
class CMessage;
class CRsCDKey;
class CRsLogInfo;
class CMyAdoBase;
class CMySocket;
class CDataBlockAllocator;
class CMapFile;

enum eNetFlag
{
	NF_Server_Client=0x01,
	NF_Server_World=0x10,
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
	BOOL InitNetServer_World(void);				//初始化针对WorldServer的网络服务器端，建立网络服务器端，装载数据
	BOOL AI();
	BOOL ProcessMessage();

	VOID OnNetASClose(VOID);					//! 响应AccountServer连接断开
	BOOL StartConnAccountServer(VOID);			//! 建立和AccountServer的连接，作为客户端初始化
	BOOL ReConnectAccountServer(VOID);			//! 重连AccountServer
	VOID CreateConnectAccountServer(VOID);		//! 创建连接线程
	static unsigned __stdcall ConnectAccountFunc(LPVOID pArguments);	// 连接WorldServer的线程、当WorldServer断开时启用，连同时候关闭

	VOID SendToSelf(AinMessage *pMsg)			{m_AinMsgQueue.PushMessage(pMsg);}
/////////////////////////////////////////////////////////////////////////
// 验证客户端
/////////////////////////////////////////////////////////////////////////
public:


public:
	AccountClient&				GetAccountClient(VOID){return m_AccountClient;}
	AsynchronousExecute&		GetAsynchronousExecute(VOID){return m_AsynchronousExecute;}
	LoginManager&				GetLoginManager(VOID){return m_LoginManager;}
	CSdoaValidate&				GetCSdoaValidate(VOID){return m_CSdoaValidate;}
	CSdoaFcm&					GetCSdoaFcm(VOID){return m_CSdoaFcm;}


private:
	//!
	AccountClient				m_AccountClient;
	//!							异步执行对象
	AsynchronousExecute			m_AsynchronousExecute;
	//!
	LoginManager				m_LoginManager;

	//!							L2L消息
	AinMsgQueue					m_AinMsgQueue;

	//!							盛大同证验证
	CSdoaValidate				m_CSdoaValidate;
	//!							盛大防沉迷模块
	CSdoaFcm					m_CSdoaFcm;

    bool ProcMenus();

/////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////
public:
	COnCmdConnect&				GetOnCmdConnect(VOID) {return m_COnCmdConnect;}
private:
	COnCmdConnect				m_COnCmdConnect;
	
/////////////////////////////////////////////////////////////////////////
// 游戏全局接口
/////////////////////////////////////////////////////////////////////////
public:


	// 其他连接
private:
	DWORD	s_lAITick;								// 游戏AI从启动到现在共运行的次数


    map<long, string>                     m_mapLoginSocket;
	map<string, pair<string, LONG>>				m_LoginCdkeyWorld;		//表示在登陆过程中的Cdkey以及登陆的WorldServer的名字和ID
	typedef	std::list<string>		WorldCdkeyList;
	std::map<long,WorldCdkeyList>	s_listCdkey;			// 在线的CDKEY列表

	typedef std::list<string>::iterator itCdkeyList;
	typedef std::map<long,WorldCdkeyList>::iterator itWorldCdkeyList;

    void UpdateStateLvls();
		
	long					m_StateLvl[4];

	//数据块分配器
	CDataBlockAllocator	*m_pDBAllocator;
	//负责接受客户端消息的网络服务
	CServer	*s_pNetServer_Client;
	//负责接受WorldServer消息的网络服务
	CServer	*s_pNetServer_World;
	//!		作为AS的客户端
	LONG	m_lASSocket;
	// 已通过QUEST_PLAYER_DATA的玩家
	map<string, long> m_PassDetailDataCdkey;
public:
	map<string, long/*GSID*/>& GetPassDetailDataCdkeyMap(void) { return m_PassDetailDataCdkey; }

	unsigned GetCdkeyCount();

	DWORD GetAITick()							{return s_lAITick;}
	void AddAITick()							{s_lAITick++;}

	CServer* GetNetServer_Client()	{return s_pNetServer_Client;}
	CServer* GetNetServer_World()	{return s_pNetServer_World;}
 

	void	WsPlayerNumIntoStringFoot(char *pString);
	long	GetLoginWorldPlayerNumByWorldID(long lID);
	long	GetLoginWorldPlayerNumByWorldName(const char* strWorldName);


	bool AddCdkey(const char *szCdkey, long lWorldNumber);
	VOID DelWsCdKey(long lWorldNumber);
	void ClearCDKey(const char *szCdkey);
	void ClearCDKeyByWorldServerID(long lID);
	int FindCdkey(const char *szCdkey);
	long GetLoginWorldCdkeyNumbers();

    void AddSocketCdkey(long lSocketID, const char* szCdkey);
    void RemoveSocketCdkey(long lSocketID);
    void ClearSocketCdkey(const char* szCdkey);
    bool IsInLoginServer(const char* szCdkey);

	bool KickOut(const char *szCdkey);			// 踢出一个CDKEY
	void KickOut(LONG lSocket);			// 踢出一个连接

	VOID SendOnlineInfoToAs(VOID);

	BOOL		WsIsReady(LONG lWorldNumber)
	{
		map<LONG, BOOL>::iterator ite = m_mapWsIsReady.find(lWorldNumber);
		if(m_mapWsIsReady.end() != ite)
			return ite->second;
		return FALSE;
	}
	VOID		SetWsReadyState(LONG lWorldNumber, BOOL bIsReady)
	{
		m_mapWsIsReady[lWorldNumber] = bIsReady;
		if(bIsReady)
        {
			Log4c::Trace(ROOT_MODULE,FormatText("LOGIN_GAME_25", lWorldNumber));//WorldServer[%d] is ready!!!
        }
		else
		{
			Log4c::Trace(ROOT_MODULE,FormatText("LOGIN_GAME_26", lWorldNumber));//Wait WorldServer[%d] reset!!!
			GetOnCmdConnect().ClearGsInfo();
		}
	}

private:
	map<LONG, BOOL>		m_mapWsIsReady;
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

	void AddWorldInfoToMsg(CMessage& msg,string &strCdkey);

	long AddWorld(long lWorldNumber,const char* strWorldName);			// 添加世界
	long DelWorld(long lNum);	// 删除世界

	bool WorldServerIsOpenState(long lNum);

	long GetWorldSateLvl(long lPlayerNum);
	void OnChangeStateLvl(long lWorldNum,long lPlayerNum);
	void ChangeAllWorldSate();
	const char *GetWorldNameByID(long lNum);
	long  GetWorldIDByName(const char* strWorldName);
	bool SendMsg2World(CMessage* pMsg,long lWorldServerID);	//发送消息给指定的WORLDSERVER
		
	void SetLoginCdkeyWorldServer(const char* szCdkey,const char* strWorldServer);	//设置在登陆过程中的Cdkey
	const char* GetLoginCdkeyWorldServer(const char* szCdkey);						//得到登陆过程中的Cdkey的WS信息
	LONG CGame::GetLoginCdkeyWorldServerID(const char* szCdkey);					//得到登陆过程中的Cdkey的WS的ID信息
	void ClearLoginCdkey(const char* szCdkey);										//清楚登陆过程中的Cdkey
	/*
	 *	消息发送
	 */
	////////////////////loginserver to worldserver
	bool L2W_PlayerBase_Send(const char *wsName,const char *szCdkey);//loginserver向worldserver请求玩家基本信息 
	void L2W_DeleteRole_Send(const char *szWSName,const char *szCdkey,CGUID& PlayerID,DWORD dwClientIP);//loginserver向worldserver发送删除角色的请求
	
	void L2W_RestoreRole_Send(LPCSTR szWSName, LPCSTR szCdkey, const CGUID& playerID); // ls 向 ws 发送恢复角色请求

	void L2W_CreateRole_Send(const char *szWSName,const char *szCdkey,CMessage *pMsg);//loginserver向worldserver发送创建角色的请求
	void L2W_QuestDetail_Send(const char *szWSName,const char *szCdkey,CGUID& PlayerID,DWORD dwClientIP, DWORD dwSignCode);//loginserver向worldserver请求玩家的详细信息

	void UpdateDisplayWorldInfo();
	void UpdateWorldInfoToAllClient();	//更新WorldServer的状态信息到客户端

	void OnNetWorldClose(long lWorldID);
	void OnNetClientClose(const char* szCdkey, long lSocketID);

	bool LoadServerResource();
	bool UnLoadServerResource();

/////////////////////////////////////////////////////////////////////////
// 游戏配置
/////////////////////////////////////////////////////////////////////////
public:
	struct tagSetup
	{
		DWORD	dwListenPort_World;			// 诊听World的Port	
		DWORD	dwListenPort_Client;		// 诊听Client的Port

		string	strAccountServerIp;		//! 验证服务器IP
		DWORD	dwAccountServerPort;	//! 验证服务器端口
		DWORD	dwAccountClientNo;		//! 验证客户端编号

		string	strLocalSqlConType;		//! 本地账号验证配置，DB相关
		string	strLocalSqlServerIP;	
		string  strLocalSqlUserName;
		string  strLocalSqlPassWord;
		string  strLocalDBName;

		//Client网络服务配置参数
		DWORD	dwDataBlockNum;			//数据块数量
		DWORD	dwDataBlockSize;		//数据块大小
		DWORD	dwFreeMsgNum;			//预分配的消息数量

		DWORD	dwFreeSockOperNum;		//网络命令操作预分配数量
		DWORD	dwFreeIOOperNum;		//完成端口上IO操作预分配数量
		long	lIOOperDataBufNum;		//默认IO操作的DataBuf数量
		long	lEncryptType;			// 通信加密客户端类型 (eNetFlag: NF_Server_Client)
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
		long	lClientMaxBlockConNum;
		long	lClientValidDelayRecDataTime;

	
		//World网络服务配置参数
		DWORD	dwWorldFreeSockOperNum;	// 网络命令操作预分配数量
		DWORD	dwWorldFreeIOOperNum;	// 完成端口上IO操作预分配数量
		long	lWorldIOOperDataBufNum;	// 默认IO操作的DataBuf数量
		bool	bWorldCheckNet;			// 是否对网络进行检测
		DWORD	dwWorldBanIPTime;		// 禁止IP的时间
		DWORD	dwWorldMaxMsgLen;		// 允许传输的最大消息长度		
		long	lWorldMaxConnectNum;		// 客户端的最大连接数量
		long	lWorldMaxClientsNum;		// 最大的客户端发送缓冲区大小
		long	lWorldPendingWrBufNum;		// 最大的发送IO操作Buf总大小
		long	lWorldPendingRdBufNum;		// 最大的接受IO操作Buf总大小
		long	lWorldMaxSendSizePerSecond;	// 向客户端每秒发送的最大字节数
		long	lWorldMaxRecvSizePerSecond;	// 从客户端接受的每秒最大字节数
		long	lWorldMaxBlockedSendMsgNum;	// 最大阻塞的发送消息数量
		long	lWorldConPendingWrBufNum;	// 客户端最大的发送IO操作Buf总大小
		long	lWorldConPendingRdBufNum;	// 客户端最大的接受IO操作Buf总大小
		long	lWorldConMaxSendSizePerSecond;//向服务器发送的每秒最大字节数
		long	lWorldConMaxRecvSizePerSecond;//从服务器接受的每秒最大字节数
		long	lWorldConMaxBlockedSendMsgNum;	// 最大阻塞的发送消息数量
		long	lWorldMaxBlockConNum;
		long	lWorldValidDelayRecDataTime;

		DWORD	dwRefeashInfoTime;			// 刷新显示时间
		DWORD	dwSaveInfoTime;				// 保存一次信息的时间间隔
		DWORD	dwPingWorldServerTime;	// 定时PingWorldServer的间隔
		DWORD	dwPingWorldServerErrorTime;	// PingWorldServer到收到返回的出错时间间隔

		DWORD	dwMinDbOptThreadNum;	//! 本地账号验证配置, 最小DB操作线程保持数
		DWORD	dwMaxDbOptThreadNum;	//! 本地账号验证配置，最大DB操作线程数
		DWORD	dwThreadNumAdjustSpace;	//! 本地账号验证配置，自动调整线程数的间隔时间
		DWORD	dwGappID;				//GAPP的编号
		tagSetup()
		{
			bCheckNet = 1;
			dwMaxMsgLen = 102400;
			dwBanIPTime = 10;
			dwRefeashInfoTime = 1000;
			dwSaveInfoTime = 60000;
		}
	};

	struct tagLoginSetup
	{
		bool	bSndaValidate;				//! 是否使用盛大方式验证
		bool	bXmCardEnable;				//! 是否使用星漫密保卡
		bool	bSndaFcmEnable;				//! 是否启用盛大防沉迷
		
		DWORD	dwDoQueueInter;				//! 处理等待队列的间隔时间
		DWORD	dwSendMsgToQueInter;		//! 给等待队列发送消息的间隔时间

		DWORD	m_dwMaxWsMum;				//!	WS最大人数
		DWORD	m_dwProcessRankInterval;	//!	处理排队间隔（毫秒）
		DWORD	m_dwSendQueueMsgInterval;	//!	发送排队消息间隔（毫秒）
		DWORD	m_dwOneProcessNum;			//!	每次处理人数（每个WS）

		float	fWorldBusyScale;			//! 繁忙的人数比例
		float	fWorldFullScale;			//! 爆满的人数比例

		
		DWORD	m_ValidateWaitTimeSpace;	//!	验证等待时间（毫秒）
		DWORD	m_SelectWaitTimeSpace;		//!	选人界面的最长时间（毫秒）
		DWORD	m_JustOutTimeSpace;			//!	小退超时时间（毫秒）

		DWORD	m_MaxPwdErrCount;			//!	连续密码输入错误最大次数
		DWORD	m_PwdErrSaveTime;			//!	密码错误次数保存时间(秒)
		DWORD	m_FrostCdkeyTime;			//! 冻结登陆时间(秒)

		char	m_szCdkeyRegex[128];		//! 验证账号用的正则表达式
		bool	m_bValideClientResourceVersion;//是否验证客户端资源版本
		char	m_szClientResourceVersion[128];//客户端资源版本
	};


private:
	tagSetup		m_Setup;
	tagLoginSetup	m_LoginSetup;
	rpattern		*m_pPat;

	//内存影射文件
	CMapFile	*m_pMapFile;
	//没次运新产生的记录堆栈的文件名字
	char m_pszRunStackFile[MAX_PATH];

	bool			m_bReLoadLoginSetup;
	bool			m_bReLoadIpLimitSetup;

public:
	bool			LoadSetup();
	bool			LoadMapFileSetup();
	bool			ReLoadSetup();
	bool			ReLoadSetupEx();
	bool			ReLoadWorldSetup();
	bool			LoadLoginSetup(VOID);
	bool			LoadIpLimitSetup(VOID);
	rpattern&		GetRpattern(VOID);
	tagSetup*		GetSetup()	{return &m_Setup;}
	tagLoginSetup*	GetLoginSetup(VOID){return &m_LoginSetup;}
	bool			UseAsFcm(VOID){return !(GetLoginSetup()->bSndaValidate && GetLoginSetup()->bSndaFcmEnable);}
	

	//初始化堆栈文件名
	BOOL InitStackFileName();

	const char* GetStatckFileName()	{return m_pszRunStackFile;}

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
		string	strName;
		DWORD	dwPlayerNum;
		vector<tagPingGameServerInfo>	vectorGameServerInfo;
	};

	void AddPingWorldServerInfo(tagPingWorldServerInfo& st)	{m_vectorPingWorldServerInfo.push_back(st);}

private:
	vector<tagPingWorldServerInfo>	m_vectorPingWorldServerInfo;


public:

	int		ReadyToEnter(const char *szCdkey, DWORD dwIP, long lSockID, const char *szWSName);
	BOOL	EnterToSelect(const char *pCdkey, const char *pWSName, DWORD dwIP, long lSockID);
	int		EnterGame( const char *szCdkey, DWORD dwIP, long lSockID, const char *szWSName, bool bLimit=false, bool bMobileUser=false );

#ifdef SHOW_LOGIN_MSG_20090817
public:
	void	OpenCloseInfo(void)
    {
        bOutInfo = !bOutInfo;   
        //登陆消息输出【%s】/开启/关闭
        Log4c::Trace(ROOT_MODULE,FormatText("LOGIN_GAME_4",((bOutInfo) ? GetText("LOGIN_GAME_5") : GetText("LOGIN_GAME_6")))
    }
	bool	OutInfo(void){return bOutInfo;}

private:
	bool	bOutInfo;
#endif
};

BOOL CreateGame();			// 创建游戏
CGame* GetGame();			// 获取游戏
BOOL DeleteGame();			// 释放游戏

unsigned __stdcall GameThreadFunc(void* pArguments);	// 游戏主线程
