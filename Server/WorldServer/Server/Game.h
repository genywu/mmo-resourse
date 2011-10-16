#pragma once

#include "../App/Mail/MailManager.h"
#include "../App/Player.h"
#include "../App/WorldRegion.h"
#include "../App/Log/LogicLogInterface.h"

// 全局对象
class Account;
class GoodsExSetup;

class CGlobalRgnManager;	
class CRgnManager;
class CEntityManager;	
class CRankManager;

class CWrapDataObject;

class CServer;
class CPlayer;
class CWorldRegion;


class CFaction;
class CUnion;
class CCountry;

class CDupliRegionSetup;
class CDataBlockAllocator;
class CMapFile;

struct tagEnemyFaction;
struct tagVilWarSetup;
struct tagAttackCityTime;

struct GSConnection;
class CVariableList;
class CMessage;
class CSessionFactory;
class VariableList;

void RefreshInfo();					// 刷新服务器信息

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

enum LoadFlag
{
    LOADFLAG_NIL = 0x00,
    LOADFLAG_PKG = 0x01,  // 重新加载资源包
    LOADFLAG_MOD = 0x02,  // 重新加载模块
    LOADFLAG_ALL = 0x03,
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

    MultiMemPool<InvalidLock>&	GetMemPool(void){return m_MemPool;}
private:
    bool ProcMenus();
    MultiMemPool<InvalidLock>	m_MemPool;

    //////////////////////////////////////////////////////////////////////////
    // 角色
    //////////////////////////////////////////////////////////////////////////
public:
    long GetMapPlayerCount() { return (long)m_mPlayer.size(); }

    void AppendMapPlayer(CPlayer *pPlayer);
    void DeleteMapPlayer(const CGUID& PlayerGuid);
    //	void ClearMapPlayerForOffline(); // use it on save done
    void ClearMapPlayer(); // use ti on uninitialize
    bool IsNameExistInMapPlayer(LPCSTR szName);

    CPlayer* GetMapPlayer(const CGUID& PlayerGuid);  //equal to IsExistPlayer() : NULL is not exist, otherwise is exist.
    CPlayer* CloneMapPlayer(const CGUID& PlayerGuid);
    CPlayer* GetMapPlayer(const char* playerName); 
    BYTE GetCreationPlayerCountInCdkey(LPCSTR szCdkey);
    bool ValidateCreatioPlayerGuidinCdkey(LPCSTR szCdkey, const CGUID& PlayerGuid);
    bool ValidatePlayerIDinCdkey(LPCSTR szCdkey, const CGUID& PlayerGuid);

    //time_t GetDeletionPlayerTime(const CGUID& PlayerGuid); // == time_t IsDeletionPlayerExist(const CGUID& PlayerGuid);

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

    //void SetNoonTimer(CMsgTimer* msgTime);
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
    VariableList* GetGeneralVariableList()		{return m_pGeneralVariableList;}

private:
    VariableList	*m_pGeneralVariableList;		// 全局脚本变量


public:
    bool IsNameExistInDBData(LPCSTR szName);

    //断开所有玩家
    void KickAllPlayer();

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
    GSConnection* GetPlayerGameServer(const CGUID& PlayerGuid);			    // 获取player所在的GameServer
    GSConnection* GetRegionGameServer(long RegionID);						// 获取region所在的GameServer
    GSConnection* GetRegionGameServer(const CGUID& RegionID);				// 获取region所在的GameServer
    long GetGameServerNumber_ByRegionID(long RegionGUID);
    long GetGameServerNumber_ByRegionGUID(const CGUID& RegionGuid);
    long GetGameServerNumber_ByPlayerID(const CGUID& PlayerGuid);
    long GetConnectedGameServerCount();										// 返回已经连接上的GameServer数量
    bool OnGameServerClose(DWORD dwIndex);									// 当某个GameServer断开连接的时候
    bool OnGameServerLost(DWORD dwIndex);									// 当某个GameServer断开时的处理
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

    bool CheckInvalidString(string& str,bool bReplace=false);				// 检查非法字符串

    // 从player身上获取装备信息
    void GetPlayerEquipID(CPlayer* pPlayer,
        DWORD& dwHead, DWORD& dwNecklace, DWORD& dwWing, DWORD& dwBody, DWORD& dwBack, DWORD& dwGlove,
        DWORD& dwShoulder, DWORD& dwWrist, DWORD& dwLeg, DWORD& dwBoot, DWORD& dwLring,DWORD& dwRring,
        DWORD& dwWeapon, DWORD& dwCard, DWORD& dwGodGoods, DWORD& dwEffigy,DWORD& dwLifeCowry,DWORD& dwSoulGuard,
        BYTE& nHeadLevel, BYTE& nNecklaceLevel, BYTE& nWingLevel, BYTE& nBodyLevel, BYTE& nBackLevel, BYTE& nGloveLevel,
        BYTE& nShoulderLevel, BYTE& nwWristLevel, BYTE& nLegLevel, BYTE& nBootLevel, BYTE& nLringLevel, BYTE& nRringLevel,
        BYTE& nWeaponLevel, BYTE& nCardLevel, BYTE& nGodGoodsLevel, BYTE& nEffigyLevel,BYTE& nLifeCowryLevel,BYTE& nSoulGuardLevel);

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

    void AddOrginGoodsToPlayer(CPlayer* pPlayer);							// 为玩家添加初始物品装备
    void InitSkillToPlayer(CPlayer* pPlayer);								//为玩家初始化技能

    // Fox@20090105 扩展的配置读取
    bool LoadEx(long flag,   			// 配置类型
        long load = LOADFLAG_MOD,   	// 是否重新加载 资源包 和 模块
        uint id = 0); 		    	    // 0, 发送至所有已连接GS; other, 单发.

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

    bool ReLoadRegion(long lRegionID);
    bool ReLoadAllRegions();

    void SendCdkeyToLoginServer(BOOL bIsReady);

private:
    char*	m_strFunctionList_FileData;				// 脚本函数列表文件内容
    char*	m_strVariableList_FileData;				// 脚本变量列表文件内容
    long	m_lFunctionList_FileData_Size;			// 脚本函数列表文件内容长度
    long	m_lVariableList_FileData_Size;			// 脚本变量列表文件内容长度
    map<string, char*>		m_mapScript_FileData;	// 所有脚本文件的内容map<文件名, 文件内容>

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
    // 游戏配置
    /////////////////////////////////////////////////////////////////////////
public:

private:
    CDupliRegionSetup	*m_pDupliRegionSetup;

    //!					扩展物品功能配置
    GoodsExSetup		*m_pGoodsExSetup;


public:
    bool		LoadServerResource();
    bool		UnloadServerResource();

    //!			序列化物品扩展配置
    bool		SerializeGoodsExSetup(vector<BYTE> &vecData);
    CDupliRegionSetup* GetDupliRegionSetup()	{return m_pDupliRegionSetup;}

    //////////////////////////////////////////////////////////////////////////
    //	攻城战相关
    //////////////////////////////////////////////////////////////////////////
public:	
    void SendMsg2CityRegionServer(CMessage* pMsg,DWORD dwIndex=-1);
    void SetCityRegiomState(long lRegionID,eCityState eState);
    void SetCityRegiomState(eCityState eState);
    void RefreshOwnedCityOrg(long lRegionID, const CGUID& FactionID, const CGUID&  UnionID);

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
    LONG GetPingGsNum(VOID)									{return (long)m_vectorPingGameServerInfo.size();}

    void SetInPing(bool b)									{m_bInPing=b;}
    void SetLastPingGameServerTime(DWORD dw)				{m_dwLastPingGameServerTime=dw;}
    VOID BeginPingDS(VOID){m_bPinging = FALSE;}
    VOID EndPingDS(VOID){m_bPinging = TRUE;}
    BOOL FinishPingDS(VOID){return m_bPinging;}

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

    //////////////////////////////

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
    //	LogicLogInterface
    //////////////////////////////////////////////////////////////////////////
public:
    LogicLogInterface*			GetLogInterface(void)  { return m_pLogInterface; }

private:
    LogicLogInterface			*m_pLogInterface;

public:
    CGlobalRgnManager*	GetGlobalRgnManager(void);
    CRgnManager*		GetRgnManager(void);
    CEntityManager*		GetEntityManager(void);
    CMailSystem*		GetMailManager(void);
    CRankManager*       GetRankManager(void) { return g_RankManager; }

    // 当GS连接WS时初始化个人房屋副本
    void InitPersonalHouseDataOnGS(long gsid);

    // 根据时间创建验证码值
    void GenerateSignCodeByTime(long& lRetCodeValue);

private:
    // 全局对象
    CGlobalRgnManager*	g_GlobalRgnManager;
    CRgnManager*		g_RgnManager;
    CEntityManager*		g_EntityManager;
    CRankManager*       g_RankManager;

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
private:
    std::map<CGUID, CPlayer*> m_mapSavePlayers;
    static CSessionFactory* m_pSessionFactory;

    std::map<eCreatePlayerPlace, set<CPlayer*>> m_mapCreatePlayerLog;
};// end of class


LogicLogInterface*	GetLogicLogInterface();			// 获取游戏日志逻辑接口对象

unsigned __stdcall GameThreadFunc(void* pArguments);	// 游戏主线程
unsigned __stdcall ProcessWriteLogDataFunc(void* pArguments);	// 写入日志线程

unsigned __stdcall LoadPlayerDataFromDB(void* pArguments);			// 玩家载入数据线程

extern HANDLE g_hGameThreadExitEvent;		// 主线程退出事件
extern bool	g_bGameThreadExit;				// 是否退出主线程

void ShowSaveInfo(LPCSTR szInfo, ...);
void DoSaveLog();

/// 获取Game单件实例
#define GetGame() GetInst(CGame)
