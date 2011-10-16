
#pragma warning(disable:4996)
#pragma warning(disable:4018;disable:4244;disable:4267;disable:4311;disable:4312)
#pragma once

#include "../../../public/BaseDef.h"
#include"../../../public/PlayerDef.h"
#include "../../../public/GameEvent.h"
#include "..\..\..\public\TimerQueue.h"
#include "..\..\..\public\msgtype.h"
#include "../../../public/StrType.h"
#include "..\..\..\public\BounsDef.h"
#include "..\..\..\LogClient\gs\LogClient.h"
#include "../AppServer/RgnDef.h"
#include <set>
#include "..\..\..\public\GoodsTailorSetup.h"
#include "../../sdoa4serverdll/include/sdoa4server.h"
#include "../../sdoa4serverdll/include/sndabase.h"
#include "..\..\Public\UseMemManage.h"
#include "../../../nets/MsgQueue.h"
//#define  _SCRIPT_INFO_

class LogicLogInterface;
class CBaseObject;
class CMoveShape;
class CServer;
class CMyDataBase;
class CPlayer;
class CGoods;
class CMonster;//添加monster类的声明
class CServerRegion;
class CProxyServerRegion;
class CMyServerClient;
class CMySocket;
extern HWND g_hLogText;
extern HWND g_hRegionListBox;
extern HWND g_hPlayerListBox;
class CSequenceString;
class CDupliRegionSetup;
class CGUID;
class CDataBlockAllocator;
class GameUnion;
class GameFaction;
class CDBPlayer;
class CDBUnion;
class CDBFaction;

class CBillClient;
class CMail;

void AddLogText(char* msg, ...);		// 加入一条记录
void AddErrorLogText(char* msg, ...);	// 加入一条出错记录
void SaveLogText(bool bForce);			// 保存记录
void AddPlayerList(const char *str);
void DelPlayerList(const char *str);
void AddRegionList(const char *str);
void DelRegionList(const char *str);

void OutMemInfo(const char *pFileName, const char *pTxt);

enum eNetFlag
{
	NF_Server_Client			=	0x001,
	NF_WServer_Client			=	0x010,
	NF_DBServer_Client			=	0x100,// [AHC 080809 DBSMSG]
	NF_BillingServer_Client		=	0x1000,
	NF_LogServer_Client = 0x10000,
	NF_BounsServer_Client		=	0x100000,
};
enum eServerInitSendMsgFlag
{
	Null_Server_InitSendFlag = 0,
	World_Server_InitSendFlag = 1,
	Bill_Server_InitSendFlag = 2,
	Log_Server_InitSendFlag = 4,
};
#define MAX_MSG_TIME_NUM 1024

//计费连接选项 
enum eBillConnectOption
{
	BILL_CONNECT_NONE,	//不连
	BILL_CONNECT_XM,	//连星漫
	BILL_CONNECT_SNDA	//连盛大
};

typedef struct tagPlayerChangeRegion
{
	long lType;
	long ltemplateID; // 普通场景用模板ID可以查找到GUID
	CGUID RegionID;
	long lX;
	long lY;
	long lDir;
	long lUseGoods;
	long lRange;
}tagPCR;

// 记录脚本文件数据信息
struct tagScriptDataInfoParam
{
	tagScriptDataInfoParam(void)
		:pInfoData(NULL),lInfoSize(0)
	{

	}
	~tagScriptDataInfoParam(void)
	{

	}
	char* pInfoData;
	long lInfoSize;
};
struct tagScriptDataInfo : public BaseMemObj
{
public:
	tagScriptDataInfo(tagScriptDataInfoParam* pParam);
	~tagScriptDataInfo(void);
	char* pInfoData;
	long lInfoSize;
};

struct tagSDOA_ITEM_SEND_REQUEST
{
	const char *szOrderId;
	const char* szPayStatus;
	const char* szSendTimes;
	const char* szCallTime;
	const char* szUserId;
	const char* szRoleName;
	const char* szGoodsNum;
	const char* szGoodsInfoEx;
	const char* szIndication;
	const char* payErrorMsg;
	int	nMsgType;
	int nMsgFrom;
	int nMsgSeq;
};	
struct tagValue
{
	string strName;
	string strValue;
};

class CGame
{
public:
	CGame(void);
	virtual ~CGame(void);

	BOOL Init();
	//初始化网络消息的优先级和丢弃信息
	BOOL InitMsgInfo();
	BOOL InitStackFileName();
	BOOL InitGoodsLogFileName();
	BOOL Release();
	BOOL MainLoop();

	BOOL InitNetServer(void);			//初始化游戏网络服务器端，建立网络服务器端，装载数据

	BOOL StartConnWS(void);			//建立和WordServer的连接，作为客户端初始化
	//建立和DBServer的连接，作为客户端初始化
	BOOL StartConnDBS(void);
	//建立和LogServer的连接，作为客户端初始化
	bool StartConnLogS(void);

	BOOL StartConnBS();//建立和BillingServer的连接

	//建立和BounsServer的连接，作为客户端初始化
	bool StartConnBounsServer(bool bReCon = false);

	BOOL StartAccept();
	void OnPlayerLost(const CGUID& PlayerGuid,long lSocketID);
	void OnPlayerExit(const CGUID& PlayerGuid, BOOL isChangeServer=FALSE);
	void OnWSLost();
	void OnDBSLost();
	void OnLOGSLost(void);
	void OnBSLost();
	void OnBounsServerLost();

	//网络传输从畅通到拥塞时触发
	void OnTransferCongestion(const CGUID& PlayerGuid,long lSocketID);
	//网络传输从拥塞到畅通时触发
	void OnTransferSmoothly(const CGUID& PlayerGuid,long lSocketID);
	void RemoveTransferCongPlayerID(hash_set<CGUID,hash_guid_compare>& PlayersGUID);
	bool IsTransferCongPlayerID(const CGUID& PlayerGUID);

	BOOL ProcessMessage();


	// [AHC 080809 DBSMSG]
	void AddReconnectionMsg(DBWriteSet& setWriteDB);
	BOOL ReConnectWorldServer();
	BOOL ReConnectDBServer();
	bool ReConnectLogServer();
	BOOL ReConnectBSServer();
	bool ReConnectBounsServer();


	VOID	CreateConnectWorldThread();						//创建连接线程
	void	CreateConnectDBThread();
	void	CreateConnectLogThread(void);
	VOID	CreateConnectBSThread();
	void	CreateConnectBounsServerThread();

	/////////////////////////////////////////////////////////////////////////
	// 游戏全局接口
	/////////////////////////////////////////////////////////////////////////

	long GetWSSocket(void) { return m_lWSSocket; }
	long GetDBSSocket(void) { return m_lDBSocket; }
	long GetBSSocket(void) {return m_lBSSocket;}
	bool GetFirstConnectionWSFlag(void) { return b_IsFirstConnectionWS; }
	void SetFirstConnectionWSFlag(bool flag) { b_IsFirstConnectionWS = flag; }

	void SetServerInitSendFlag(eServerInitSendMsgFlag flag);
	void ProcessServerInitSendMsg();

	bool GetIsGenVarInit(void) { return m_bIsGenVarInit; }
	void SetIsGenVarInit(bool flag) { m_bIsGenVarInit = flag; }

	void SetSaveDeltaTime(long lTime) { g_lSaveDeltaTime = lTime; }

	void OutPlayerAndRegionInfo(const char* pszFileName);

	//! 响应新的一天开始了
	void OnNewDayStart(DWORD dwTime);

	//! 计算星座日
	void ResetStarDay(DWORD dwTime);
	//! 得到星座日
	eConstellation GetStarDay(void);
private:
	//! 当前星座日
	eConstellation m_Constellation;

private:	
	DWORD	s_lAITick;							// 游戏AI从启动到现在共运行的次数
	long	g_lSaveDeltaTime;					// 保存时间间隔

	CDataBlockAllocator	*m_pDBAllocator;
	CServer				*s_pNetServer;			// 做为GameServer的服务器端
	long				m_lWSSocket;			// 连接worldserver的客户端
	long				m_lDBSocket;			// 连接DBserver的客户端
	long				m_lBounsSocket;			// 连接BounsServer的客户端
	long				m_lBSSocket;			// 连接BillingServer的客户端
	long				m_ServerInitSendFlag;
	CRITICAL_SECTION	m_CSServerInitSendFlag;

	ISDOAOrderHandler* m_pSDOAOrderHandler;
	bool m_bSDOAInitialized;


	typedef map<CGUID, CPlayer*,guid_compare>::iterator itPlayer;
	std::map<CGUID, CPlayer*,guid_compare> s_mapPlayer;			// 保存本服务器玩家列表
	//传输拥塞的PlayerGUID
	hash_set<CGUID,hash_guid_compare>	m_TransCongPlayers;

	typedef map<string,CGUID>	mapStringToGUID;
	typedef map<string,CGUID>::iterator itSG;
	mapStringToGUID m_PlayerNameToGUID;

	char*	m_strFunctionList_FileData;				// 脚本函数列表文件内容
	long	m_lFuncFileDataLen;
	char*	m_strVariableList_FileData;				// 脚本变量列表文件内容
	long	m_lVariableFileDataLen;
	map<string, tagScriptDataInfo*>		m_mapScript_FileData;	// 所有脚本文件的内容map<文件名, 文件内容>
	
	// 是否初次连接WS
	bool					b_IsFirstConnectionWS;

	BOOL					m_bBSConnected;
	bool					m_bBounsSConnected;			// BounsServer是否连接

	//没次运新产生的记录堆栈的文件名字
	char m_pszRunStackFile[MAX_PATH];
	//物品操作日志
	char m_pszGoodsLogFile[MAX_PATH];

	// 数据是否初始化完成
	bool m_bIsGenVarInit;
public:
	ISDOAOrderHandler* GetSDOAOrderHandler(){return m_pSDOAOrderHandler;}
	bool IsSDOAInitialized(){return m_bSDOAInitialized;}
	bool IsSDOAValid(){return (m_pSDOAOrderHandler && m_bSDOAInitialized);}

	const char* GetCountryName(BYTE cCountryType);

	DWORD GetAITick()							{return s_lAITick;}
	void AddAITick()							{s_lAITick++;}
	CServer* GetNetServer()				{return s_pNetServer;}

	const char* GetStatckFileName()	{return m_pszRunStackFile;}
	const char* GetGoodsLogFile()	{return m_pszGoodsLogFile;}

	long GetPlayerCount()						{return (long)s_mapPlayer.size();}
    CMoveShape* FindShape(OBJECT_TYPE type, const CGUID& entityID);
	CBaseObject *FindBaseObject(unsigned long objecttype,const CGUID& guid);
	// 通过RgnGUID查找场景
	CServerRegion* FindRegionByRgnID(const CGUID& RgnGUID);
	// 通过RgnGUID 或者 OwnerGUID查找场景
	CServerRegion* FindRegion(const CGUID& RegionID);

	CServerRegion* FindProxyRegion(const CGUID& lRegionID);
	CServerRegion* FindRegion(const char* strName);
	CPlayer* FindPlayer(const CGUID& PlayerGuid)			{return (s_mapPlayer.find(PlayerGuid) != s_mapPlayer.end()) ? s_mapPlayer[PlayerGuid] : NULL;}
	CPlayer* FindPlayer(const char* strName);
	void SendSysMailMsgToOnlinePlayer(CMail *pMail);//为在线玩家发信 {return &s_mapPlayer;}

	void AddPlayer(CPlayer* pPlayer);
	void RemovePlayer(const CGUID& PlayerID);
	CMonster* FindMonster(const CGUID& MonsterID);//寻找一个monster
	bool KickPlayer(const CGUID& PlayerGuid);
	void KickAllPlayer(const CGUID& exceptID=NULL_GUID);
	void AddRegion(eRgnType rgnType, const CGUID& RegionID, CServerRegion* pRegion);
	
	map<string, tagScriptDataInfo*>* GetScriptFileDataMap()		{return &m_mapScript_FileData;}
	const char* GetFuncitonList_FileData()			{return m_strFunctionList_FileData;}
	const char* GetVariableList_FileData()			{return m_strVariableList_FileData;}

	const char* GetScriptFileData(const char* filename)	
	{
		if(NULL == filename)	return NULL;
		map<string, tagScriptDataInfo*>::iterator it = m_mapScript_FileData.find(filename);
		if(it != m_mapScript_FileData.end()) 
		{
			if(it->second && it->second->pInfoData)
				return it->second->pInfoData;
		}
		return NULL;
	}

	BOOL IsBSConnected(){return m_bBSConnected;}
    bool IsBounsSConnected()    { return m_bBounsSConnected; }
	BOOL SetBSConnectedFlag(BOOL bVal){m_bBSConnected=bVal;}

	void SetFunctionFileData(char* pData, long lLen);
	void SetVariableFileData(char* pData, long lLen);
	void SetGeneralVariableFileData(BYTE* pByte,long lPos);
	void SetScriptFileData(const char* filename, char* pData, long lLen);

	//申请共享数据块
	BYTE* AllockShareDB(long lSize);
	//释放共享数据块
	void FreeShareDB(BYTE* pData);

	bool CheckInvalidString(string& str,bool bReplace=false);

	void SendTopInfoToClient(long lTopInfoID,const CGUID& PlayerGuid,long lFlag,long lParam,string strInfo);//发送顶部显示信息到客户端

	VOID SendNotifyMessage( char* szText,const CGUID& playerGuid, DWORD dwColor = 0xffffffff, DWORD dwBkColor = 0,eNOTIFY_POS ePos=eNOTIFYPOS_CENTER);
	VOID SendNotifyMessage( char* szText,CServerRegion* pRegion, DWORD dwColor = 0xffffffff, DWORD dwBkColor = 0,eNOTIFY_POS ePos=eNOTIFYPOS_CENTER);
	//发送消息到所有玩家
	void SendMsgToAllPlayer(CMessage *pMsg);
	//发送消息到同国家的玩家
	void SendMsgToSameCountryPlayer(CMessage *pMsg,BYTE btCountry);
	//发送消息到同阵营国家的玩家
	void SendMsgToSameCampPlayer(CMessage *pMsg,BYTE btCountry);
	//查询禁言玩家
	void QuerySilencePlayers(CGUID& GmID);
	/////////////////////////////////////////////////////////////////////////
	// 线程管理
	/////////////////////////////////////////////////////////////////////////
public:
	HANDLE m_hEndEvent[2];				// 结束事件

	/////////////////////////////////////////////////////////////////////////
	// 游戏配置
	/////////////////////////////////////////////////////////////////////////
public:
	struct tagSetup
	{
		long	lUseLogServer;		// 是否使用LogServer；0=不使用，1=使用
		string	strLogIP;			// LogServerIP
		DWORD	dwLogPort;			// LogServerPort
		string  strErrFile;			// 错误文件路径+文件名
		string  strLogFile;			// 本地日志文件路径+文件名（SQL格式）
		string  strTxtLogFile;		// 本地日志文件路径+文件名（明文格式）
		DWORD	dClientNo;			// 客户端编号
		long	lUseBillingServer;		//是否连接BillingServers
		long	lUseBounsServer;		// 是否连接BounsServer
		DWORD	dwGameServerNum;		// GS连接BounsServer的编号

		string	strWorldIP;			// WorldServerIP
		DWORD	dwWorldPort;		// WorldServerPort
		DWORD	dwListenPort;		// 诊听Port
		string	strLocalIP;			// 本地IP

		long	lMaxBlockConNum;
		long	lValidDelayRecDataTime;

		DWORD	dwDataBlockNum;			//数据块数量
		DWORD	dwDataBlockSize;		//数据块大小
		DWORD	dwFreeMsgNum;			//预分配的消息数量

		DWORD	dwFreeSockOperNum;		//网络命令操作预分配数量
		DWORD	dwFreeIOOperNum;		//完成端口上IO操作预分配数量
		long	lIOOperDataBufNum;		//默认IO操作的DataBuf数量
		long	lEncryptType;			// 加密类型

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
		long	lConMaxBlockedSendMsgNum;//最大阻塞的发送消息数量

		DWORD	dwRefeashInfoTime;	// 刷新一次信息的时间
		DWORD	dwSaveInfoTime;		// 保存一次信息的间隔时间
		DWORD	dwGoodsLogTime;		// 物品Log多少分钟清除一次
		bool	bWatchRunTimeInfo;	// 是否监控运行的时间信息
		DWORD	dwWatchRunTime;		// 监控间隔
		DWORD	dwMsgValidateTime;	// 消息验证时间（毫秒，0为关闭）
		DWORD	dwGappID;			//映射文件客户端ID
		tagSetup()
		{
			strWorldIP = "127.0.0.1";
			dwWorldPort = 8100;
			dwListenPort = 2347;
			strLocalIP = "127.0.0.1";
		
			lUseBillingServer=BILL_CONNECT_NONE;
		}
	};

	struct tagBSSetup
	{
		DWORD dwIndex;
		string strIp;
		DWORD dwPort;
	};

private:
	tagSetup		m_Setup;
	BYTE			m_lIDIndex;			// 服务器编号，给worldserver用的
	DWORD			m_dwWSNumber;		//连接的WS编号
	CDupliRegionSetup	*m_pDupliRegionSetup;

	list<tagBSSetup> m_listBSSetup;
	list<tagBSSetup> m_listBDSSetup;	// 赠送服务器列表

public:
	bool	LoadSetup();
	bool	ReLoadSetup();			//重读配置文件
	bool	ReloadSetupEx();

	//重新装载所有配置
	tagSetup*	GetSetup()			{return &m_Setup;}
	BYTE GetIDIndex()				{return m_lIDIndex;}
	void SetIDIndex(BYTE byteVal)	{m_lIDIndex=byteVal;}
	CDupliRegionSetup* GetDupliRegionSetup()	{return m_pDupliRegionSetup;}

	DWORD GetWSNumber(){return m_dwWSNumber;}
	VOID SetWSNumber(DWORD dwNumber){m_dwWSNumber=dwNumber;}

	//////////////////////////////////////////////////////////////////////////
	//	组队
	//////////////////////////////////////////////////////////////////////////
public:
	long GetNumTeams();	
public:
	void RefreashAllMonsterBaseProperty();		// 刷新所有怪物的BaseProperty指针

	//////////////////////////////////////////////////////////////////////////
	// 保存所有玩家Client消息效验序列字符串
	//////////////////////////////////////////////////////////////////////////
private:
	map<CGUID,CSequenceString*> m_SequenceMap;
public:
	map<CGUID,CSequenceString*>* GetSequenceMap() { return &m_SequenceMap; }

	CSequenceString* GetSequence(const CGUID& guid);
	void CleanSequenceMap();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 保存所有玩家Client消息效验时间
	//////////////////////////////////////////////////////////////////////////
private:
	map<CGUID,bool> m_ValidateTime;
public:
	map<CGUID,bool>* GetValidateTimeMap() { return &m_ValidateTime; }

	bool *GetValidateTime(const CGUID& guid);
	void AppendValidateTime(const CGUID& guid, bool enable = false);
	void RemoveValidateTime(const CGUID& guid);
	void CleanValidateTime() { GetValidateTimeMap()->clear(); }

	// [AHC 080809 DBSMSG]

	// 产生MSGID
	inline long AllocMsgID(void);

	// 释放一个MSGID
	inline void FreeMsgID(long id);

	// 发送保存PHDupRgn 数据
	void SendSavePHDupRgn(const CGUID& ownerGUID);

	// 玩家数据分层保存接口
	// 发送玩家金钱/等级改变 保存属性消息
	void SendPlayerAttrData(const CGUID& playerID);
	// 发送玩家容器物品改变保存消息
	void SendPlayerContainerGoodsData(const CGUID& playerID, long place/*区别容器类型*/);
	// 发送玩家变量列表改变保存消息
	void SendPlayerVarListData(const CGUID& playerID);
	// 发送玩家任务数据改变保存消息
	void SendPlayerQuestData(const CGUID& playerID);
	// 发送玩家技能数据改变消息
	void SendPlayerSkillData(const CGUID& playerID);
	// 发送玩家状态数据改变消息
	void SendPlayerStateData(const CGUID& playerID);

	// 数据分层存储接口
	void AppendSaveMsgProperty		(CPlayer* player, DBWriteSet& setWriteDB);
	void AppendSaveMsgEquipmentGoods(CPlayer* player, DBWriteSet& setWriteDB);
	void AppendSaveMsgPackGoods		(CPlayer* player, DBWriteSet& setWriteDB);
	void AppendSaveMsgSubPackGoods	(long packID, CPlayer* player, DBWriteSet& setWriteDB);
	void AppendSaveMsgVarList		(CPlayer* player, DBWriteSet& setWriteDB);
	void AppendSaveMsgQuestList		(CPlayer* player, DBWriteSet& setWriteDB);
	void AppendSaveMsgMerQuestList	(CPlayer* player, DBWriteSet& setWriteDB);
	void AppendSaveMsgSkillList		(CPlayer* player, DBWriteSet& setWriteDB);
	void AppendSaveMsgStateList		(CPlayer* player, DBWriteSet& setWriteDB);
	void AppendSaveMsgDeputyOccuList(CPlayer* player, DBWriteSet& setWriteDB);
	void AppendSaveMsgWalletGoods	(CPlayer* player, DBWriteSet& setWriteDB);
	void AppendSaveMsgSilverGoods	(CPlayer* player, DBWriteSet& setWriteDB);
	void AppendSaveMsgDepotGold		(CPlayer* player, DBWriteSet& setWriteDB);
	void AppendSaveMsgDepotSilver	(CPlayer* player, DBWriteSet& setWriteDB);
	void AppendSaveMsgDepotGoods	(CPlayer* player, DBWriteSet& setWriteDB);
	void AppendSaveMsgSubDepot		(long packID, CPlayer* player, DBWriteSet& setWriteDB);
	void AppendSaveMsgSpriteSystem	(CPlayer* player, DBWriteSet& setWriteDB);
	

	void AppendSaveMsgRegion(CServerRegion* pRegion,CMessage* pSendMsg);
	void UpdatePlayerPack(CPlayer* pPlayer,DWORD dwContainerId,DWORD dwPos);	
	// 添加或者修改一个玩家要保存的属性枚举值
	void UpdateSavePlayer(CPlayer* player, SAVE_DETAIL_ATTR_TYPE type);
	// 添加或者修改一个玩家要保存的属性枚举值
	void UpdateSavePlayer(const CGUID& playerID, SAVE_DETAIL_ATTR_TYPE type);
	// 处理玩家保存发送消息
	void SendQueuePlayerOperMsg(void);

	// 延迟保存操作清除玩家对象(delete)
	void ClearSavePlayer(const CGUID& guid);
	void AddSaveDetailPartToByteArray(const CGUID& guid, DWORD flag, CPlayer* player);

	//添加要保存场景
	void UpdateSaveRegion(CServerRegion* pRegion);
	void ClearSaveRegion(CServerRegion* pRegion);
	void ClearSaveRegion();
	void PushSavePlayer(CPlayer* pPlayer);
	CPlayer* FindSavePlayer(const CGUID& guid);
	std::map<CGUID, CPlayer*>&	GetSavePlayerMap(void) { return m_mapSavePlayers; }

	VOID LogRegion(VOID);

	CPlayer* FindChangeRgnPlayer(const CGUID& guid);
	void ClearChangeRgnPlayerVec(void);

	void PushChangeRgnPlayer(const CGUID& playerID,tagPCR& ChangeRegion);
		
	void PushDelDupRgn(CServerRegion* pRgn);
	void ClearDelDupRgn(void);
//时间消耗统计
private:
	//消息消耗统计
	map<long,long>	m_mapMsgExpendTime;
	//定时器消耗统计
	map<long,long>	m_mapTimerExpendTime;
public:

#ifdef _SCRIPT_INFO_
	struct tagScriptRunInfo
	{
		DWORD	runTime;
		long	runCount;
		tagScriptRunInfo(void){memset(this, 0, sizeof(tagScriptRunInfo));}
	};

	typedef map<string, tagScriptRunInfo>			ScriptInfoMap;
	typedef map<string, tagScriptRunInfo>::iterator	ScriptInfoMapItr;

	ScriptInfoMap m_ScriptInfo;

	//!				清理所有信息
	void			ClearScriptInfo(void)									{m_ScriptInfo.clear();}
	//!				添加一个脚本信息
	void			AddScriptInfo(const char *pScriptName, DWORD runTime)	{
		string strScriptName(pScriptName);
		ScriptInfoMapItr itr = m_ScriptInfo.find(strScriptName);
		if(m_ScriptInfo.end() == itr)
		{
			m_ScriptInfo.insert(make_pair(strScriptName, tagScriptRunInfo()));
			itr = m_ScriptInfo.find(strScriptName);
		}
		itr->second.runTime += runTime;
		++itr->second.runCount;
	}
	void			GetScriptInfo(string &strOutInfo)
	{
		strOutInfo.append("\r\n");
		ScriptInfoMapItr itr = m_ScriptInfo.begin();
		for (; itr != m_ScriptInfo.end(); ++itr)
		{
			char szTmpInfo[10240] = {0};
			_snprintf(szTmpInfo, 10240, "%-80s, %6d次, %8ums\r\n", itr->first.c_str(), itr->second.runCount, itr->second.runTime);
			strOutInfo.append(szTmpInfo);
		}
	}
#endif

	inline void AddMsgExpendTime(long lType,long lTime)
	{
		if(GetSetup()->bWatchRunTimeInfo)
			m_mapMsgExpendTime[lType] += lTime;
	}
	inline void AddTimerExpendTime(long lType,long lTime)
	{
		if(GetSetup()->bWatchRunTimeInfo)
			m_mapTimerExpendTime[lType] += lTime;
	}

	//输出信息
	void OutputMsgExpendTimeInfo(const char* pszFileName);
private:
	std::map<CGUID, CPlayer*> m_mapSavePlayers;
	std::set<CServerRegion*> m_setSaveRegion;

	typedef map<CGUID,tagPCR,guid_compare>::iterator itCRP;
	map<CGUID,tagPCR,guid_compare>	m_ChangeRgnPlayers;
	map<CGUID, CServerRegion*> m_mapDelDupRgn;

private:
	//接收SDOA_ITEM_SEND_REQUEST消息队列
	CMsgQueue m_RecvSDOMessages;
public:
	void OnReceiveSdoItemRequest(tagSDOA_ITEM_SEND_REQUEST &stSdo_Item_Send_Request);

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
	LogClient&			GetLogClient(void){return m_LogClient;}
	//CMailManager		*GetMailManager();
private:

	LogClient			m_LogClient;
	LogicLogInterface	*m_pLogicLogInterface;


	//-------------------------------------------------------------------------------
	// 计费客户端对象. by Fox.		2008-01-22
	// func : 负责计费客户端的初始化、回调及与主线程通信
	//-------------------------------------------------------------------------------
	HINSTANCE		m_hBsipDll;
	CBillClient		*pBillClient;


	
	//////////////////////////////////////////////////////////////////////////
	//	奖励
	//////////////////////////////////////////////////////////////////////////

private:
	map<DWORD, tagBounsDes*>		m_mapBounsDes;

public:
	map<DWORD, tagBounsDes*>* GetBounsDes()	{return &m_mapBounsDes;}
	void AddBounsDes(tagBounsDes* pBD);
	tagBounsDes* FindBounsDes(DWORD dwBounsID);
	void ClearBounsDes();

};

BOOL				CreateGame();			// 创建游戏
CGame*				GetGame();				// 获取游戏
BOOL				DeleteGame();			// 释放游戏
LogicLogInterface*	GetGameLogInterface();	// 获取游戏日志逻辑接口对象

unsigned __stdcall		GameThreadFunc(void* pArguments);	// 游戏主线程
extern HANDLE g_hGameThreadExitEvent;		// 主线程退出事件
extern bool	g_bGameThreadExit;				// 是否退出主线程

unsigned __stdcall		ConnectWorldFunc( void* pArguments );	// 连接WorldServer的线程、当WorldServer断开时启用，连同时候关闭

unsigned __stdcall		ConnectLogServerFunc( void* pArguments );

unsigned __stdcall		ConnectBSFunc( void* pArguments );

// 连接BounsServer的线程
unsigned __stdcall		ConnectBounsServerFunc(void* pArguments);


