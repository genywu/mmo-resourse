
#pragma once
#include "../public/DBDefine.h"
#include "../../../dbaccess/MyAdoBase.h"
#include "..\appworld\player.h"
#include "..\appworld\worldregion.h"
#include "..\..\..\DBAccess\WorldDBQueue\WriteLogQueue.h"
#include "..\..\..\public\TimerQueue.h"
#include "..\..\..\public\msgtype.h"
#include "..\LogClient\ws\LogClient.h"
#include "..\AppWorld\Mail\MailManager.h"
#include "..\..\..\public\GoodsTailorSetup.h"
#include "..\..\..\public\WSUseMemManage.h"
// 全局对象
class Account;
class GoodsExSetup;
class CGlobalRgnManager;	
class CRgnManager;
class CEntityManager;	
class LogicLogInterface;
class CWrapDataObject;
class CServer;
class CPlayer;
class CWorldRegion;
class CDBCard;
class CDBGoods;
class CDBPlayer;
class CDBLoginPlayer;
class CDBFaction;
class CDBUnion;
class CDBCountry;
class CDBEnemyFaction;
class CDBGenVar;
class CDBSetup;
class CDBRegion;
class CDBMail;
class CDBGenVarGroup;
class CFaction;
class CUnion;
class CCountry;
class CCard;
class CDupliRegionSetup;
class CDataBlockAllocator;
class CMapFile;
struct tagEnemyFaction;
struct tagVilWarSetup;
struct tagAttackCityTime;
class CVariableList;
class CMessage;
class CCostNetClient;
class CSessionFactory;
extern HWND g_hLogText;
extern HWND g_hGameServerListBox;
extern HWND g_hPlayerListBox;

void RefeashInfoText();					// 刷新服务器信息
void AddInfoText(char* msg, ...);		// 加入一条记录
void AddLogText(char* msg, ...);		// 加入一条记录
void AddErrorLogText(char* msg,...);	// 加入一条出错记录
void AddPlayerList(const char *str);
void DelPlayerList(const char *str);

enum eNetFlag
{
	NF_Server_Client=0x001,
	NF_Gate_Client = 0x002,
	NF_LServer_Client=0x010,
	NF_CServer_Client=0x100,
	NF_DBServer_Client=0x1000,
	NF_LogServer_Client = 0x10000,
};
enum eServerInitSendMsgFlag
{
	Null_Server_InitSendFlag = 0,
	DB_Server_InitSendFlag = 1,
	Login_Server_InitSendFlag = 2,
	Log_Server_InitSendFlag = 4,
};

#define MAX_MSG_TIME_NUM 1024


enum eCreatePlayerPlace
{
	ePlayerQuitMsg = 1,
	ePlayerGsReconnectOne = 2,
	ePlayerGsReconnectTwo = 3,
	ePlayerCreateProc = 4,
	ePlayerProcessCreateProc = 5,
	ePlayerLoadData = 6,
	ePlayerPushSave = 7,
};

// DBS事件定时器类
class CMsgTimer : public CGameEvent
{
public:
	CMsgTimer()
		:	m_ID(0),
			m_PlayerID(0),
			m_DesSocketID(0),
			m_SrcSocketID(0),
			m_dwClientIP(0),
			m_RegionID(0)
	{
		memset(m_szCDKey, 0, sizeof(m_szCDKey));
	};

	CMsgTimer(eMsgTYpe msg)
	{
		m_DBSMsgType = msg;
	};

	~CMsgTimer()
	{

	}

	//网络消息响应
	virtual void OnMessage(CMessage*) {};
	//超时响应
	virtual long OnTimeOut(DWORD timerid,DWORD curtime,const void* var);
	//当定时器被取消的调用
	virtual void OnTimerCancel(DWORD timerid,const void* var);
	//处理实体间消息函数
	virtual long Proc(eEventType et, const tagProcVar* tPV) { return 0; };

	//!	注册一个排队定时器，并返回定时器ID
	long RegisterMsgTimerID(long lFutureTime);
	//!	取消一个排队的Timer
	void CancelMsgTimer(void);
	//!	响应排队定时器触发的事件
	void OnMsgTimeout(long lTimerID);


	// 设置ID
	void SetID(long ID) { m_ID = ID; }
	// 得到ID
	long GetID(void) { return m_ID; }

	// 设置MsgType
	void SetMsgType(eMsgTYpe msg) { m_DBSMsgType = msg; }
	// 得到MsgType
	eMsgTYpe GetMsgType(void) { return m_DBSMsgType; }


	// 设置PlayerID
	void SetPlayerID(long ID) { m_PlayerID = ID; }
	// 得到PlayerID
	long GetPlayerID(void) { return m_PlayerID; }

	// 设置CDKey
	void SetCDKey(char* szCDKey, long size) 
	{ 
		memset(m_szCDKey, 0, sizeof(m_szCDKey));

		if( size < (sizeof(szCDKey)) )
			memcpy(m_szCDKey, szCDKey, size);
		else
			memcpy( m_szCDKey, szCDKey, (sizeof(m_szCDKey)) );
	}
	// 得到CDKey
	char* GetCDKey(void) { return m_szCDKey; }

	// 设置DesSocketID
	void SetDesSocketID(long ID) { m_DesSocketID = ID; }
	// 得到DesSocketID
	long GetDesSocketID(void) { return m_DesSocketID; }
	
	// 设置SrcSocketID
	void SetSrcSocketID(long ID) { m_SrcSocketID = ID; }
	// 得到SrcSocketID
	long GetSrcSocketID(void) { return m_SrcSocketID; }

	// 设置ClientIP
	void SetClientIP(DWORD value) { m_dwClientIP = value; }
	// 得到ClientIP
	DWORD GetClientIP(void) { return m_dwClientIP; }


	// 得到RegionID
	long GetRegionID(void) { return m_RegionID; };
	// 设置RegionID
	void SetRegionID(long id) { m_RegionID = id; };

	// 得到TimerID
	long GetTimerID(void) { return m_lTimerID; }
	// 设置TimerID
	void SetTimerID(long id) { m_lTimerID = id; }
private:
	long     m_ID;
	eMsgTYpe m_DBSMsgType;
	long     m_PlayerID;
	char     m_szCDKey[64];
	long     m_DesSocketID;
	long     m_SrcSocketID;
	DWORD    m_dwClientIP;
	long     m_RegionID;
	long	 m_lTimerID;
};

typedef CTimerQueue<CMsgTimer*> MsgTimer;

//class CDBGoods;
// 游戏全局控制
class CGame
{
public:
	CGame(void);
	virtual ~CGame(void);

	BOOL Init();
	//初始化堆栈文件名
	BOOL InitStackFileName();
	BOOL Release();
	BOOL MainLoop();
	BOOL AI();
	BOOL ProcessMessage();

	void OnLSLost(void);
	void OnDBSLost(void);

	void OnLOGSLost(void);


	//////////////////////////////////////////////////////////////////////////
	// 角色
	//////////////////////////////////////////////////////////////////////////
public:
	size_t GetMapPlayerCount() { return m_mPlayer.size(); }

	void AppendMapPlayer(CPlayer *pPlayer);
	void DeleteMapPlayer(const CGUID& PlayerGuid);
	void ClearMapPlayer(); // use ti on uninitialize
	bool IsNameExistInMapPlayer(LPCSTR szName);

	CPlayer* GetMapPlayer(const CGUID& PlayerGuid);  //equal to IsExistPlayer() : NULL is not exist, otherwise is exist.
	CPlayer* CloneMapPlayer(const CGUID& PlayerGuid);
	CPlayer* GetMapPlayer(const char* playerName); 
	BYTE GetCreationPlayerCountInCdkey(LPCSTR szCdkey);
	bool ValidateCreatioPlayerGuidinCdkey(LPCSTR szCdkey, const CGUID& PlayerGuid);
	bool ValidatePlayerIDinCdkey(LPCSTR szCdkey, const CGUID& PlayerGuid);
	
	const CGUID& GetOnlinePlayerIDByName( LPCSTR szName);
	const CGUID& GetPlayerIDByName(LPCSTR szName);
	
	void ProcessTimeOutLoginPlayer();

	// [070725 AHC DB_INIT_LOAD]
	// 是否发送请求敌对关系数据标志
	bool b_HasQuestEnemyFactionMsgFlag;
	// 是否发送请求国家数据标志
	bool b_HasQuestCountryMsgFlag;
	// 是否接收到请求敌对关系数据标志
	bool b_HasGetEnemyFactionMsgFlag;
	// 是否接收到请求国家数据标志
	bool b_HasGetCountryMsgFlag;
	// 是否在内存数据中验证成功Player数据标志
	bool b_IsValidatePlayerIDinCdkey;
	// 是否在存盘数据中验证成功Player数据标志
	bool b_ValidateDBPlayerIDinCdkey;

	// 产生MSGID
	inline long AllocMsgID(void);

	// 释放一个MSGID
	inline void FreeMsgID(long id);

	// 取得 CMsgTimer 数据结构
	typedef std::map<long, CMsgTimer*> mapMsgTimer;
	typedef mapMsgTimer::iterator mapMsgTimerItr;

	// 取得记录DBS事件的时间
	inline MsgTimer* GetTimerQueue(void) { return &m_MsgTimer; }
	
	inline mapMsgTimer& GetMsgTimerMap(void) { return m_mapMsgTimer; }

	// 取得MsgTimer
	inline CMsgTimer* GetMsgTimer(long id);
	
	// 取得一个未使用的MsgTime对象
	CMsgTimer* AllocFreeMsgTimer(void);
	// Init CMsgTimer 数据结构
	void InitMsgTimerMap(void);
	// Release CMsgTimer 数据结构
	void ReleaseMsgTimerMap(void);

	// 设置定时器
	void SetMsgTimer(eMsgTYpe msg, CMsgTimer* msgTime);

	void SetNoonTimer(CMsgTimer* msgTime);
	// 删除定时器
	void DelMsgTimer(CMsgTimer* msgTime);
	void DelMsgTimer(long msgID);
	
	inline std::list<long>& GetFreeMsgIDList(void) { return m_listFreeMsgID; }
	inline std::list<CMsgTimer*>& GetFreeMsgList(void) { return m_listFreeMsgTimer; }

	void InitCountryPlayerNum();
	//得到最少人数的3个国家,通过BYTE中0到5位来确定
	BYTE GetCountryLessPlayer();

	//! 计算星座日
	void ResetStarDay(DWORD dwTime);
	//! 得到星座日
	eConstellation GetStarDay(void);
private:
	//! 当前星座日
	eConstellation m_Constellation;

private:
	// 记录DBS事件的时间,以WS向DBS发送消息时的消息时间作为判断超时的依据:发送消息AddMsgTime(),接收回应DelMsgTime()
	
	MsgTimer m_MsgTimer;
	// 被使用的CMsgTimer 数据结构
	std::map<long, CMsgTimer*> m_mapMsgTimer;
	// 空闲的CMsgTimer 数据结构
	std::list<CMsgTimer*> m_listFreeMsgTimer;
	// 当前MSGID
	long m_CurMsgID;
	// 空闲MSGID
	std::list<long> m_listFreeMsgID;

	// 所有操作角色都在这里。
	map<CGUID, CPlayer*> m_mPlayer;
	//在线玩家的国家人数统计
	map<BYTE,long>	m_ContryPlayerNum;

	typedef map<string,CGUID>	mapStringToGUID;
	typedef map<string,CGUID>::iterator itSG;
	mapStringToGUID m_PlayerNameToGUID;

	//内存影射文件
	CMapFile	*m_pMapFile;
	//没次运新产生的记录堆栈的文件名字
	char m_pszRunStackFile[MAX_PATH];

	// 操作物品的堆栈文件名
	char m_pszSaveGoodsRunStackFile[MAX_PATH];
	//邮件操作记录的文件名
	char m_pszOperateMailFile[MAX_PATH];

public:
	bool GenerateDBPlayerID(IN CGUID& guid);
	const CGUID& GetDBPlayerID() { return m_PlayerID; }
	void SetDBPlayerID(const CGUID& PlayerID) { m_PlayerID = PlayerID; }


	long GenerateDBLeaveWordID()	{return ++m_nLeaveWordID;}
	long GetDBLeaveWordID()	{return m_nLeaveWordID;	}
	void SetDBLeaveWordID(long nID)	{m_nLeaveWordID=nID;}

	const char* GetStatckFileName()	{return m_pszRunStackFile;}

	const char* GetSaveGoodsStatckFileName()	{return m_pszSaveGoodsRunStackFile;}

	const char* GetOpeateMailFileName()			{return m_pszOperateMailFile;}

private:
	CGUID m_PlayerID;
	long m_nLeaveWordID;
public:
	CVariableList* GetGeneralVariableList()		{return m_pGeneralVariableList;}

private:
	CVariableList	*m_pGeneralVariableList;		// 全局脚本变量

	//////////////////////////////////////////////////////////////////////////
	//	存盘 （操作以上三种需要进数据库的数据）
	//////////////////////////////////////////////////////////////////////////
public:
	struct tagDBData
	{
		//
		// 设置变量保存
		//
		CGUID			PlayerGuid;		// 角色id
		CVariableList	*pVariableList;		// 全局脚本变量
		long			nLeaveWordID;		//留言的ID

		map<CGUID, CPlayer*> mDBPlayer;

		//需要保存的帮会数据
		list<CFaction*>		listSaveFactions;
		//需要保存的同盟数据
		list<CUnion*>		listSaveUnions;

		//需要删除的帮会列表
		list<CGUID>			listDeleteFactions;
		//需要删除的同盟列表
		list<CGUID>			listDeleteUnions;
		//敌对帮会列表
		list<tagEnemyFaction*> listEnemyFactions;
		list<tagVilWarSetup*>	 listVillageWar;
		list<tagAttackCityTime*>  listCityWar;
		//要保存的场景参数列表
		list<CWorldRegion*> listRegionParam;

		list<CCountry*>		ltDBCountrys;
	};

public:
	int GetDBResponsedCnt() {return m_nDBResponsed;}
	void SetDBResponsedCnt(int nCnt) {m_nDBResponsed=nCnt;}

	tagDBData* GetDBData() {return &m_stDBData;}

	// 复制需要存盘的数据到SaveData
	void GenerateDBData();
	void GeterateRegionDBData();
	
	bool IsNameExistInDBData(LPCSTR szName);

	//断开所有玩家
	void KickAllPlayer();

private:
	tagDBData m_stDBData;				// 存盘数据(每个存盘点生成一次，存盘结束后释放)
	int	m_nDBResponsed;			// 收到的GameServer存盘回复数据计数器


	map<long,long>	m_mapMsgVirMemUseSize;

	/////////////////////////////////////////////////////////////////////////
	// 场景相关数据
	/////////////////////////////////////////////////////////////////////////
public:
	// 场景分配信息
	struct tagRegion
	{
		CWorldRegion	*pRegion;			// 场景指针
		DWORD			dwGameServerIndex;	// 所在Gameserver编号
		eRWT			RegionType;			// 场景类型
	};	
public:
	// GameServer信息
	struct tagGameServer
	{
		bool			bConnected;			// 该服务器是否已经连接上
		DWORD			dwIndex;			// 服务器编号
		string			strIP;				// IP
		DWORD			dwPort;				// 端口

		long			lReceivedPlayerData;	// 接收到的玩家数据数量(零时用)
		long			lPlayerNum;			// 玩家数量
	};

public:
	VOID	ReturnGsPlayerNum(VOID);
	tagGameServer* GetGameServer(DWORD dwIndex)	{return (s_mapGameServer.find(dwIndex)!=s_mapGameServer.end())?&s_mapGameServer[dwIndex]:NULL;}
	tagGameServer* GetGameServer(const char* strIP, DWORD dwPort);			// 获取GameServer的数据	
	tagGameServer* GetPlayerGameServer(const CGUID& PlayerGuid);			// 获取player所在的GameServer
	//得到非副本的场景ID的GameServer
	tagGameServer* GetRegionGameServer(long RegionID);						// 获取region所在的GameServer
	tagGameServer* GetRegionGameServer(const CGUID& RegionID);				// 获取region所在的GameServer

	long GetGameServerNumber_ByRegionID(long RegionGUID);
	long GetGameServerNumber_ByRegionGUID(const CGUID& RegionGuid);

	long GetGameServerNumber_ByPlayerID(const CGUID& PlayerGuid);


	bool LoadServerSetup(const char *filename);								// 读取游戏服务器配置
	//! 读取非副职业制作配置，这个接口应该在物品配置完成后再调用
	bool LoadGoodsTailorSetup(const char *filename);
	long GetConnectedGameServerCount();										// 返回已经连接上的GameServer数量
	
	bool OnGameServerClose(DWORD dwIndex);									//当某个GameServer断开连接的时候
	bool OnGameServerLost(DWORD dwIndex);									// 当某个GameServer断开时的处理

	long GetGSNum(void) { return s_mapGameServer.size(); }
	map<DWORD, tagGameServer>* GetGameServerList(){return &s_mapGameServer;}

private:
	map<DWORD, tagGameServer>		s_mapGameServer;	// 服务器列表<dwIndex, tagGameServer>
public:
	// 读取LoadLoginGMList.dat
	bool LoadLoginGMList(const char* szName);
	vector<string>& GetLoginGMListVec(void) { return m_vecLoginGMList; }
	bool CheckCdkeyIsExsitInLoginGMList(const char* szCdkey);
	// 判断玩家是否是GM
	bool PlayerIsGM(CPlayer* pPlayer); 
	private:
		vector<string> m_vecLoginGMList;
	//////////////////////////////////////////////////////////////////////////
public:
	VOID   CreateConnectLoginThread();
	void CreateConnectDBThread();
	void CreateConnectLogThread(void);
	void CreateConnectCostThread();
	bool ReConnectLoginServer();											// 重新连接LoginServer
	bool ReConnectDBServer();
	bool ReConnectLogServer();
	bool ReConnectCostServer();                                             
	void SendCdkeyToLoginServer(BOOL bIsReady);											// 发送当前WorldServer上的所有Cdkey到LoginServer
	void SendMsg2GameServer(DWORD dwIndex,CMessage* pMsg);					// 发送一个消息到指定Index的GameServer
	//! [2007-7-9 add]
	void SendAllGameServer(CMessage* pMsg);					// 发送一个消息到所有GameServer
	//! [2007-7-9 add end]

	bool CheckInvalidString(string& str,bool bReplace=false);				// 检查非法字符串

	// 从player身上获取装备信息
	void GetPlayerEquipID(CPlayer* pPlayer,
		DWORD& dwHelm, DWORD& dwBody, DWORD& dwGlov, DWORD& dwBoot, DWORD& dwWeapon, DWORD& dwBack,
		DWORD& dwHeadgear, DWORD& dwFrock, DWORD& dwWing, DWORD& dwManteau, DWORD& dwFairy,
		BYTE& nHelmLevel, BYTE& nBodyLevel, BYTE& nGlovLevel, BYTE& nBootLevel, BYTE& nWeaponLevel, BYTE& nBackLevel,
		BYTE& nHeadgearLevel, BYTE& nFrockLevel, BYTE& nWingLevel, BYTE& nManteauLevel, BYTE& nFairyLevel);

	void AddOrginGoodsToPlayer(CPlayer* pPlayer);							// 为玩家添加初始物品装备
	void InitSkillToPlayer(CPlayer* pPlayer);								//为玩家初始化技能

	BOOL ReLoad(const char* name, bool bSendToGameServer=false,bool bReloadFileInfo = false);			// 重读设置

	// Fox@20090105 扩展的配置读取
	bool LoadEx(unsigned uFlag,			// 配置类型
		long lSocketID = 0,				// -1:发送所有GS, 0:不发送至GS, other:单发.
		bool bReload = true);			// 是否重读配置

	DWORD GetAITick()							{return s_lAITick;}
	void AddAITick()							{s_lAITick++;}

	// 响应玩家操作
	bool	OnPlayerEnter(CPlayer* pPlayer, Account* pAcc, long lSignCode, long lIP,  long gsid, long lSocketIDC2S, long lMsgSocketID);
	//!		在玩家编码发送后响应
	bool	OnPlayerEnterLateCode(CPlayer* pPlayer, Account* pAcc);
	bool	OnPlayerQuit(CPlayer* pPlayer, long gsid, DWORD dwServerKey[], bool bIsChangingGs=false);
private:
	DWORD	s_lAITick;														// 游戏AI从启动到现在共运行的次数
	DWORD	m_dwLastReLoginTime;											// 上次重连LoginServer的时间
	//////////////////////////////////////////////////////////////////////////
	//	网络部分
	//////////////////////////////////////////////////////////////////////////
public:
	BOOL InitNetServer(void);					//初始化游戏网络服务器端，建立网络服务器端，装载数据
	BOOL StartConnLS(void);						//连接LoginServer
	bool StartConnCS();							//开始连接计费服务器
	bool StartAccept();							//开始作为服务器监听

	VOID BeginPingDS(VOID){m_bPinging = FALSE;}
	VOID EndPingDS(VOID){m_bPinging = TRUE;}
	BOOL FinishPingDS(VOID){return m_bPinging;}

	//建立和DBServer的连接，作为客户端初始化
	BOOL StartConnDBS(void);
	//建立和LogServer的连接，作为客户端初始化
	bool StartConnLogS(void);
	
	void SetInitVarLoadFlag(bool flag)
	{
		m_bInitVarLoadFlag = flag;
	}
	bool GetInitVarLoadFlag(void)
	{
		return m_bInitVarLoadFlag;
	}
	void SetInitRegionLoadFlag(bool flag)
	{
		m_bInitRegionLoadFlag = flag;
	}
	bool GetInitRegionLoadFlag(void)
	{
		return m_bInitRegionLoadFlag;
	}
	void SetInitCountryLoadFlag(bool flag)
	{
		m_bInitCountryLoadFlag = flag;
	}
	bool GetInitCountryLoadFlag(void)
	{
		return m_bInitCountryLoadFlag;
	}
	void SetInitFactionLoadFlag(bool flag)
	{
		m_bInitFactionLoadFlag = flag;
	}
	bool GetInitFactionLoadFlag(void)
	{
		return m_bInitFactionLoadFlag;
	}
	void SetInitUnionLoadFlag(bool flag)
	{
		m_bInitUnionLoadFlag = flag;
	}
	bool GetInitUnionLoadFlag(void)
	{
		return m_bInitUnionLoadFlag;
	}
	void SetInitPersonalHouseLoadFlag(bool flag)
	{
		m_bInitPersonalHouseLoadFlag = flag;
	}
	bool GetInitPersonalHouseLoadFlag(void)
	{
		return m_bInitPersonalHouseLoadFlag;
	}
	void SetRestartSessionFlag(bool flag)
	{
		m_bRestartSessionFlag = flag;
	}
	bool GetRestartSessionFlag(void)
	{
		return m_bRestartSessionFlag;
	}

	void SetKickPlayerFlag(bool flag) { m_bIsKickPlayerFlag = flag; }
	bool GetKickPlayerFlag(void) { return m_bIsKickPlayerFlag; }

	CServer* GetNetServer()	{return s_pNetServer;}

	// get dbsocketID
	long GetDBSocketID(void) { return m_lDBSocket; }
	// get LSsocketID
	long GetLSSocketID(void) { return m_lLogingSocket; }

	bool GetDbIsReady(void) { return m_bDbReady; }
	void SetDbIsReady(bool flag) { m_bDbReady = flag; }

	void SetServerInitSendFlag(eServerInitSendMsgFlag flag);
	void ProcessServerInitSendMsg();
private:
	CDataBlockAllocator	*m_pDBAllocator;
	CServer				*s_pNetServer;			// 做为GameServer的服务器端
	long				m_lLogingSocket;		//连接loginserver的客户端
	long				m_lDBSocket;			//连接dbserver的客户端
	long				m_lCostSocket;			//连接costserver的客户端
	bool				m_bInitVarLoadFlag;
	bool				m_bInitCountryLoadFlag;
	bool				m_bInitRegionLoadFlag;
	bool				m_bInitFactionLoadFlag;
	bool				m_bInitUnionLoadFlag;
	bool				m_bRestartSessionFlag;
	bool				m_bInitPersonalHouseLoadFlag;
	bool				m_bDbReady;
	long				m_ServerInitSendFlag;
	CRITICAL_SECTION	m_CSServerInitSendFlag;
	bool				m_bIsKickPlayerFlag;
	BOOL				m_bPinging;


	/////////////////////////////////////////////////////////////////////////
	// 脚本数据
	/////////////////////////////////////////////////////////////////////////
public:
	bool LoadScriptFileData(const char* dir_name, const char* function_filename, const char* var_filename, const char* general_var_filename);	// 读取指定目录中的所有脚本文件(包含子目录)
	bool LoadOneScript(const char* filename);		// 读取一个脚本文件
	map<string, char*>* GetScriptFileDataMap()			{return &m_mapScript_FileData;}

	const char* GetFuncitonList_FileData()				{return m_strFunctionList_FileData;}
	const char* GetVariableList_FileData()				{return m_strVariableList_FileData;}
	long GetFuncitonList_FileData_Size()			{return m_lFunctionList_FileData_Size;}
	long GetVariableList_FileData_Size()			{return m_lVariableList_FileData_Size;}

	const char* GetScriptFileData(const char* filename)	
	{
		if(m_mapScript_FileData.find(filename)!=m_mapScript_FileData.end()) 
			return m_mapScript_FileData[filename];
		return NULL;
	}

	bool ReLoadOneScript(const char* filename);
	bool ReLoadOneRegionSetup(long lRegionID);
	bool ReLoadAllRegionSetup();
	bool ReLoadAllRegion();

private:
	char*	m_strFunctionList_FileData;				// 脚本函数列表文件内容
	char*	m_strVariableList_FileData;				// 脚本变量列表文件内容
	long	m_lFunctionList_FileData_Size;			// 脚本函数列表文件内容长度
	long	m_lVariableList_FileData_Size;			// 脚本变量列表文件内容长度
	map<string, char*>		m_mapScript_FileData;	// 所有脚本文件的内容map<文件名, 文件内容>


	/////////////////////////////////////////////////////////////////////////
	// 游戏配置
	/////////////////////////////////////////////////////////////////////////
public:
	struct tagSetup
	{
		string	strDBIP;			// DBServerIP
		DWORD	dwDBPort;			// DBServerPort

		long	lUseLogServer;		// 是否使用LogServer；0=不使用，1=使用
		string	strLogIP;			// LogServerIP
		DWORD	dwLogPort;			// LogServerPort
		string  strErrFile;			// 错误文件路径+文件名
		string  strLogFile;			// 本地日志文件路径+文件名（SQL格式）
		string  strTxtLogFile;		// 本地日志文件路径+文件名（明文格式）
		DWORD	dClientNo;			// 客户端编号

		DWORD	dwNumber;			// 服务器编号(0-7)
		string	strName;			// 服务器名字
		string	strLoginIP;			// LoginServerIP
		long	dwLoginPort;		// LoginServerPort
		DWORD	dwListenPort;		// 诊听Port
		string	strLocalIP;			// 本地IP
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

		DWORD	dwReleaseLoginPlayerTime;			// 释放LoginList里的Player的超时时间

		DWORD	dwClearPlayerDataTime;
		
		DWORD	dwGappID;				//内存映射客户端ID
		tagSetup()
		{
			dwRefeashInfoTime = 1000;
			dwSaveInfoTime = 60000;
		}
	};

private:
	tagSetup	m_Setup;
	CDupliRegionSetup	*m_pDupliRegionSetup;

	//!					扩展物品功能配置
	GoodsExSetup		*m_pGoodsExSetup;
	
	
public:
	bool		LoadSetup();
	bool		LoadMapFileSetup();
	bool		LoadServerResource();
	bool		UnloadServerResource();

	//!			序列化物品扩展配置
	bool		SerializeGoodsExSetup(vector<BYTE> &vecData);

	tagSetup*	GetSetup()	{return &m_Setup;}
	CDupliRegionSetup* GetDupliRegionSetup()	{return m_pDupliRegionSetup;}

	//////////////////////////////////////////////////////////////////////////
	// 组队相关
	//////////////////////////////////////////////////////////////////////////
public:
	long GetNumTeams();
	
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
	//	物品超链
	//////////////////////////////////////////////////////////////////////////
public:
	//物品超链数据结构
	struct tagGoodsLink
	{
		bool bChange;	//物品属性是否变化
		CGoods* pGoods;
		DWORD dwIndex;	//物品类型编号（消耗性道具使用该字段)
		DWORD dwType;
		BYTE lNum;		//物品数量
	};

	list<tagGoodsLink>* GetGoodsLinkList()	{return &m_listGoodsLink;}
	void AddGoodsLink(tagGoodsLink* pstGoodsLink);	//添加一个物品超链
	tagGoodsLink* FindGoodsLink(DWORD dwID);        //根据ID寻找一个物品超链

private:
	list<tagGoodsLink> m_listGoodsLink;				//玩家提交的物品超链存放处

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

	void AddPingGameServerInfoDB(tagPingGameServerInfo& st)	{m_vectorPingGameServerInfoDB.push_back(st);}
	void ClearPingGameServerInfoDB()						{m_vectorPingGameServerInfoDB.clear();}	

	void AddPingGameServerInfo(tagPingGameServerInfo& st)	{m_vectorPingGameServerInfo.push_back(st);}
	void ClearPingGameServerInfo()							{m_vectorPingGameServerInfo.clear();}
	LONG GetPingGsNum(VOID)									{return m_vectorPingGameServerInfo.size();}

	void SetInPing(bool b)									{m_bInPing=b;}
	void SetLastPingGameServerTime(DWORD dw)				{m_dwLastPingGameServerTime=dw;}

private:

	vector<tagPingGameServerInfo>	m_vectorPingGameServerInfoDB;
	vector<tagPingGameServerInfo>	m_vectorPingGameServerInfo;
	bool	m_bInPing;							// 是否在Ping的过程中
	DWORD	m_dwLastPingGameServerTime;			// 上次发起ping的时间

	//////////////////////////////////////////////////////////////////////////
	// 异步从数据库读取玩家信息
	//////////////////////////////////////////////////////////////////////////
public:

	// 将刚从DB读取的Player相关数据初始化
	void InitPlayerInfo(CPlayer* pPlayer, char* szCdkey);


	///////////////////////////////
	// 日志队列
	///////////////////////////////

public:
	void DoSaveData(bool isSavePlayerFlag);
	// 发送SendSetup
	void SendSetup(void);
	// 发送MSG_W2DB_SERVER_UNREGENTITY 清除DBS管理的对象
	void SendUnRegDBEntity(const char* szCdkey);

	// 发送查询player的CDKEY消息
	void SendGMBanPlayerToDBSMsg(char* strPlayerName, long time);

	// decode MSG_S2W_PLAYER_DETAIL_SAVE_PROPERTY 消息
	bool DecodeSavePropertyMsg		(CPlayer* pPlayer, DBReadSet& setReadDB);
	bool DecodeSaveEquipmentGoodsMsg(CPlayer* pPlayer, DBReadSet& setReadDB);
	bool DecodeSavePackGoodsMsg		(CPlayer* pPlayer, DBReadSet& setReadDB);
	bool DecodeSaveSubPackGoodsMsg	(long subPackPos, CPlayer* pPlayer, DBReadSet& setReadDB);
	bool DecodeSaveVarListMsg		(CPlayer* pPlayer, DBReadSet& setReadDB);
	bool DecodeSaveQuestListMsg		(CPlayer* pPlayer, DBReadSet& setReadDB);
	bool DecodeDepotGoldMsg			(CPlayer* pPlayer, DBReadSet& setReadDB);
	bool DecodeDepotSilverMsg		(CPlayer* pPlayer, DBReadSet& setReadDB);
	bool DecodeDepotGoodsMsg		(CPlayer* pPlayer, DBReadSet& setReadDB);
	bool DecodeSaveSubDepotGoodsMsg (long subPackPos, CPlayer* pPlayer, DBReadSet& setReadDB);
	//俑兵任务数据
	bool DecodeSaveMerQuestListMsg	(CPlayer* pPlayer, DBReadSet& setReadDB);
	bool DecodeSaveSkillListMsg		(CPlayer* pPlayer, DBReadSet& setReadDB);
	bool DecodeSaveStateListMsg		(CPlayer* pPlayer, DBReadSet& setReadDB);
	bool DecodeSaveDeputyOccuListMsg(CPlayer* pPlayer, DBReadSet& setReadDB);
	bool DecodeSaveWalletGoodsMsg	(CPlayer* pPlayer, DBReadSet& setReadDB);
	bool DecodeSaveSivlerGoodsMsg	(CPlayer* pPlayer, DBReadSet& setReadDB);
	bool DecodeSaveSpriteSystemMsg	(CPlayer* pPlayer, DBReadSet& setReadDB);	
	
	//////////////////////////////////////////////////////////////////////////
	//	LogClient
	//////////////////////////////////////////////////////////////////////////
public:
	//!		响应DB信息
	void	InitLogClient(CMessage *pMsg);
	//!		响应确认信息
	void	LogServerAffirm(CMessage *pMsg);
	//!		响应筛选条件信息
	void	LogServerCondition(CMessage *pMsg);
	
	//!		
	LogicLogInterface*	GetLogicLogInterface(void);

public:
	LogClient			&GetLogClient(void){return m_LogClient;}
private:

	LogClient			m_LogClient;
	LogicLogInterface	*m_pLogicLogInterface;

	/*
	
	*/
public:
	CGlobalRgnManager*	GetGlobalRgnManager(void);
	CRgnManager*		GetRgnManager(void);
	CEntityManager*		GetEntityManager(void);
	CMailSystem*		GetMailManager(void);

	// 当GS连接WS时初始化个人房屋副本
	void InitPersonalHouseDataOnGS(long gsid);

	// 根据时间创建验证码值
	void GenerateSignCodeByTime(long& lRetCodeValue);

	void SendSaveGenVarGroupMsg(CDBGenVarGroup* pGenVarGroup);

private:
	// 全局对象
	CGlobalRgnManager*	g_GlobalRgnManager;
	CRgnManager*		g_RgnManager;
	CEntityManager*		g_EntityManager;

	CMailSystem		*g_pMailManager;

	bool				b_IsFirstConn;
// Fox@20080922----------------------------------------------
public:
	bool ConnectGate(bool bRec = false);		// 连接GateServer
	void OnGateLost(void);						// GateServer断开;
	void CreateGateThread(void);				// 断开后创建连接线程
	int GetGateSocketID(void) { return m_nGateSocket; }

private:
	int					m_nGateSocket;			// 连接GateServer的SocketID

public:
	void PushSavePlayer(CPlayer* pPlayer);
	CPlayer* FindSavePlayer(const CGUID& guid);
	std::map<CGUID, CPlayer*>&	GetSavePlayerMap(void) { return m_mapSavePlayers; }
	void ProcessDelaySavePlayer(void);
	void SavePlayer(CPlayer *pPlayer);
	static CSessionFactory* GetSessionFactoryInst(void);

	void AddCreatePlayerLog(eCreatePlayerPlace place, CPlayer* player);
	void DelPlayerLog(CPlayer* player);
	void OutputCreatePlayerInfo(void);
	CPlayer* NewRawPlayer(eCreatePlayerPlace place);
	void DelRawPlayer(CPlayer* player);

	//添加消息消耗类型和时间
	inline void AddMsgExpendTime(long lType,long lTime)
	{
		//if(GetSetup()->bWatchRunTimeInfo)
		m_mapMsgExpendTime[lType] += lTime;
	}
	inline void AddTimerExpendTime(long lType,long lTime)
	{
		m_mapTimerExpendTime[lType] += lTime;
	}
	//添加更新全局变量次数统计
	inline void AddVarScriptExpendTime(const char* pszVarName,long lTick)
	{
		if (NULL != pszVarName)
			m_mapVarScriptExpendTime[string(pszVarName)] += lTick;
	}
	//输出消耗统计信息
	void OutputMsgExpendTimeInfo(const char* pszFileName);
private:
	//消息消耗统计
	map<long,long>	m_mapMsgExpendTime;
	//定时器消耗统计
	map<long,long>	m_mapTimerExpendTime;
	//更新全局脚本变量时间消耗统计
	map<string,long> m_mapVarScriptExpendTime;
private:
	std::map<CGUID, CPlayer*> m_mapSavePlayers;
	static CSessionFactory* m_pSessionFactory;

	std::map<eCreatePlayerPlace, set<CPlayer*>> m_mapCreatePlayerLog;
};// end of class


BOOL CreateGame();						// 创建游戏
CGame* GetGame();						// 获取游戏
LogicLogInterface*	GetGameLogInterface();			// 获取游戏日志逻辑接口对象
BOOL DeleteGame();						// 释放游戏

unsigned __stdcall GameThreadFunc(void* pArguments);	// 游戏主线程
unsigned __stdcall ConnectLoginServerFunc( void* pArguments );	// 连接WorldServer的线程、当WorldServer断开时启用，连同时候关闭
unsigned __stdcall ConnectCostServerFunc( void* pArguments );	// 
unsigned __stdcall ConnectDBServerFunc( void* pArguments );	// 
unsigned __stdcall ConnectLogServerFunc( void* pArguments );	// 
unsigned __stdcall ConnectGateFunc(void* pArg);				// 连接GateServer的线程, 断开时启用, 连通时关闭

unsigned __stdcall LoadPlayerDataFromDB(void* pArguments);			// 玩家载入数据线程

extern HANDLE g_hGameThreadExitEvent;		// 主线程退出事件
extern bool	g_bGameThreadExit;				// 是否退出主线程

// 显示存盘信息开关
extern bool g_bShowSaveInfo;

void ShowSaveInfo(LPCSTR szInfo, ...);
void DoSaveLog();