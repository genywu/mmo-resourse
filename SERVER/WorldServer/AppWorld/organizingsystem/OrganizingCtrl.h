#pragma once
#include <set>

#include "S2CSession.h"
#include "Faction.h"
#include "Union.h"
#include "..\..\public\TimerQueue.h"
//OrganizingCtrl.h
//Function:	//该类负责控制帮派的建立、解散、以及对帮、同盟的管理控制.
			//负责对Organizings变量的管理
			//负责帮会系统的装载、存储，更新
//Author:Joe
//Create Time:2004.6.11
//Note:该类全局只有唯一的实例

class CPlayer;
class CMySocket;
class WSTeam;
class CMessage;



//用来制作帮会排行时候的Key
struct tagKey 
{
	long	l;
	tagTime Time;
};

//帮会排行榜结构
struct tagFacBillboard
{
	CGUID	FactionGuid;	//帮会ID
	string	strName;		//帮会名字
	long	lNum;			//数量
};
//创建组织系统的返回结果
enum eCrOrgResult
{
	CORT_Fail=0,		//失败
	CORT_NameExist,		//存在名字
	CORT_Ok,			//创建成功
};

//帮会信息结构，用于宣战时用
struct DeclareWarFacInfo
{
	long	lFactionID;		//帮会ID
	char	strName[20];	//帮会名字
	long	lRelations;		//和本帮的关系,0:无关系,1:自己,2:同盟,3:敌对
};


//屏幕上段的提示信息
struct stTopInfo
{
	long	lID;			//标示ID
	long	lTimerFlag;		//倒计时方式(1:计数器,2:时间)
	long	lParam;			//时间或次数量
	DWORD	dwStartTime;	//开始计时时间
	string	strInfo;		//信息内容
};

class COrganizingCtrl
{
private:
	COrganizingCtrl(VOID);
	~COrganizingCtrl(VOID);
	//属性
public:
	static	COrganizingCtrl*	getInstance();														//得到唯一实例句柄
	static	VOID				releaseInstance(){SAFE_DELETE(instance);}
	bool						Initialize();														//初始化
	VOID						Release();															//释放资源
	VOID						Run(long lInterTime);												//组织系统的Run
	VOID						ClearFaction(VOID);
	VOID						ClearUnion(VOID);

	VOID						OutOrganiInfo(VOID);

public:
	//!							响应GS的连接
	VOID						OnGsConnect(LONG lGsSocket);

	VOID 						OnPlayerEnterGame(const CGUID& PlayerGuid);							//出发一个玩家进入游戏
	VOID 						OnPlayerEnterGameLaterDataSend(const CGUID& PlayerGuid);					//出发一个玩家进入游戏
	VOID 						OnPlayerExitGame(CPlayer *pPlayer, bool IsChangeGS);			//触发一个玩家离开游戏
	VOID 						OnPlayerDied(const CGUID& PlayerGuid, const CGUID& lKiller);		//玩家死亡时触发
	int  						OnDeleteRole(const CGUID& PlayerGuid);								//玩家删除角色时触发
	VOID						OnPlayerMapChange(const CGUID &PlayerGuid, const char* pMapName);	//响应玩家地图改变
	VOID						OnPlayerOccuChange(CPlayer *pPlayer, LONG lOccu);			//响应玩家职业改变
	VOID						OnPlayerLevelChange(CPlayer *pPlayer, LONG lLevel);			//响应玩家等级改变
	VOID						OnPlayerHideChange(CPlayer *pPlayer, BYTE cHide);			//响应玩家隐匿状态改变

	VOID SendOrgaInfoToClient(const CGUID& PlayerGuid,string& strSendCon,string& strSender,long lGameServerID=-1,
		DWORD dwFontColor=MAKELONG(MAKEWORD(254,237), MAKEWORD(218,255)),
		DWORD dwBkColor=MAKELONG(MAKEWORD(0,0), MAKEWORD(0,0)) );

private:
	static	COrganizingCtrl*	instance;										//该类唯一实例句柄

	
	
	


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//工会
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

	VOID						SaveAllFaction(VOID);
	VOID						SaveAllUnion(VOID);

public:
	//!							响应初始化工会完成
	VOID						OnInitFactionEndMsg						(VOID);
	//!							相应工会删除的回复
	VOID						OnDelFactionFinishMsg					(const CGUID &AimGuid, BOOL bIsSysOpt);
	//!							响应GS的工会创建请求
	VOID						OnGsCreateFaction						(CMessage *pMsg);
	//!							响应GS的工会升级请求
	VOID						OnGsUpgradeFaction						(CMessage *pMsg);
	//!							响应GS的删除工会请求
	VOID						OnDelFaction							(const CGUID &DelFacGuid, BOOL bIsSysOpt);
	//!							响应邀请玩家入会
	VOID						OnGsFacInvitePlayer						(CMessage *pMsg);
	//!							响应添加玩家动作
	VOID						OnGsFacAddMember						(CMessage *pMsg);
	//!							响应设置官阶名称
	VOID						OnGsFacSetJobName						(CMessage *pMsg);
	//!							响应设置官阶权限
	VOID						OnGsFacSetJobPurview					(CMessage *pMsg);
	//!							响应成员退出
	VOID						OnGsFacQuit								(CMessage *pMsg);
	//!							响应设置公告
	VOID						OnGsSetPronounce						(CMessage *pMsg);
	//!							响应设置备忘
	VOID						OnGsSetLeaveword						(CMessage *pMsg);
	//!							响应开除成员
	VOID						OnGsFire								(CMessage *pMsg);
	//!							设置成员职级
	VOID						OnGsSetMemberJob						(CMessage *pMsg);
	//!							设置成员称号
	VOID						OnGsSetMemberTitle						(CMessage *pMsg);
	//!							传位
	VOID						OnGsSetNewMaster						(CMessage *pMsg);
	//!							操作应征者
	VOID						OnGsCheckRequisition					(CMessage *pMsg);
	//!							申请加入
	VOID						OnGsJoinFac								(CMessage *pMsg);
	//!							上传图标
	VOID						OnGsUpLoadIcon							(CMessage *pMsg);

public:
	//!							将一个工会对象加入管理
	BOOL						AddFactionOrganizing					(CFaction *pFaction);
	//!							找出一个工会对象
	CFaction*					GetFactionOrganizing					(const CGUID &FacGuid);	
	//!							找出一个玩家的工会ID
	const		CGUID&			FindPlayerFacionID						(const CGUID &PlayerGuid);

	//!							发送初始化GS工会消息
	VOID						SendFactionDataToGS						(LONG lGsSocket);
	VOID						SendFactionDataToGS						(set<LONG> &setSocket);

	//!							检测升级工会的条件
	eFactionOptErrInfo			TestUpgradeFacCondition					(CFaction *pFaction, DWORD dwUpgradeType);
	//!							升级工会，按配置的条件
	eFactionOptErrInfo			UpgradeFac_ByCondition					(CFaction *pFaction, DWORD dwUpgradeType);


public:
	typedef						CTimerQueue<COrganizingCtrl*>			OrganTimer;

public:
	//!							响应新的一天到了
	void						OnNewDay								(DWORD dwTime);
	//!							定时器超时响应
	long						OnTimeOut								(DWORD timerid,DWORD curtime,const void* var);
	//!							当定时器被取消的调用
	void						OnTimerCancel							(DWORD timerid,const void* var);
	//!							
	void						TimerRun								(DWORD dwCurrTime){m_TimerQueue.Expire(dwCurrTime);}

private:
	//!							得到定时器对象
	OrganTimer&					GetTimerQueue							(void){return m_TimerQueue;}
	//!
	VOID						BeginTimer								(VOID);
	//!
	VOID						EndTimer								(VOID);

private:
	//!							工会数据完整性标记
	BOOL						m_bFacDataFull;
	//!							等待初始化数据的GS
	set<LONG>					m_setWaitFacSocket;

	//!							全局所有的帮派组织
	map<CGUID, CFaction*>		m_FactionOrganizings;
	//!							所有已申请入会的玩家
	map<CGUID, CGUID>			m_mapAllApplicant;
	
	//!							定时器
	OrganTimer					m_TimerQueue;
	//!							处理家族福利的TimerID
	LONG						m_lExpTimerID;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//联盟
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	//!							将一个联盟对象加入管理
	BOOL						AddUnionOrganizing(CUnion *pUnion);
	//!							找出一个联盟对象
	CUnion*						GetUnionOrganizing(const CGUID &UnionGuid);

	//!							发送初始化GS联盟消息
	VOID						SendUnionDataToGS(LONG lGsSocket);
	VOID						SendUnionDataToGS(set<LONG> &setSocket);

	//!							响应初始化联盟完成
	VOID						OnInitUnionEndMsg(VOID);
	//!							响应删除里面成功
	VOID						OnDelUnionFinishMsg(const CGUID &UnionGuid);

	//!							响应玩家创建联盟
	VOID						OnGsCreateUnion(CPlayer *pPlayer, const char *pUnionName);
	//!							响应玩家解散联盟
	VOID						OnGsDisbandUnion(CPlayer *pPlayer);
	//!							响应退出联盟请求
	VOID						OnGsQuitUnion(const CGUID &FactionGuid, const CGUID &UnionGuid);
	//!							响应开除联盟请求
	VOID						OnGsKickUnion(const CGUID &OptGuid, const CGUID &AimGuid, const CGUID &UnionGuid);
	//!							传位
	VOID						OnGsSetChairmanUnion(const CGUID &MasterGuid, const CGUID &AimFacGuid);
	//!							设置权限
	VOID						OnGsSetPurviewUnion(const CGUID &MasterGuid, const CGUID &AimFacGuid, DWORD dwPurview);
	//!							邀请加入联盟
	VOID						OnGsUnionInvit(const CGUID &MasterGuid, const char *pAimPlayerName);
	//!							应答邀请
	VOID						OnGsUnionAnswerInvit(const CGUID &InviterGuid, const CGUID &RespondentGuid, const CGUID & UnionGuid, LONG lResult);

private:
	//!							联盟数据完整性标记
	BOOL						m_bUnionDataFull;
	//!							等待初始化数据的GS
	set<LONG>					m_setWaitUnionSocket;

	//!							全局所有的同盟组织
	map<CGUID, CUnion*>			m_mapUnion;

	struct tagUnionSession
	{
		CGUID SourceID;		//! 发起者
		CGUID TargetID;		//! 应答者
		CGUID UnionID;		//! 联盟ID
		tagUnionSession():SourceID(NULL_GUID),TargetID(NULL_GUID){}
		tagUnionSession(const CGUID &SrcID, const CGUID &AimID, const CGUID &UnionGuid)
			:SourceID(SrcID),TargetID(AimID),UnionID(UnionGuid){}
		BOOL operator ==(const tagUnionSession& right)
		{
			return ((right.SourceID == SourceID) && (right.TargetID == TargetID) && (right.UnionID == UnionID));
		}
	};
	S2CSession<tagUnionSession>			m_UnionSession;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//组队
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	//队伍会话结构
	struct tagPlayerTeamSession
	{
		CGUID SourceID;		//! 发起者
		CGUID TargetID;		//! 应答者
		tagPlayerTeamSession():SourceID(NULL_GUID),TargetID(NULL_GUID){}
		tagPlayerTeamSession(const CGUID &SrcID, const CGUID &AimID):SourceID(SrcID),TargetID(AimID){}
		BOOL operator ==(const tagPlayerTeamSession& right)
		{
			return ((right.SourceID == SourceID) && (right.TargetID == TargetID));
		}
	};

public:
	VOID			SendTeamSetupToGS(LONG lGsSocket);							//! 发送队伍配置信息到GS

	VOID			SendTeamDataToGS(LONG lGsSocket);							//! 发送全局队伍数据到GS

	VOID			SendInfoToOneClient(const CGUID& PlayerGuid, char *szInfo);	//! 向指定玩家发送一个信息

	WSTeam*			GetGameTeamByID(const CGUID& TeamID);						//! 得到指定的队伍的指针

	long			GetGameTeamNum(VOID);										//! 得到队伍数量

	//long			GetNewTeamID(VOID);											//! 得到一个新的队伍ID

	VOID			OnQueryJoin(CMessage *pMsg);								//! 响应玩家发出组队邀请
	
	VOID			OnQueryJoinByName(CPlayer* pInvitee, CPlayer* pCaptain);	//! 响应玩家发出组队邀请
	
	VOID			OnAnswerJoin(CMessage *pMsg);								//! 响应玩家答复组队邀请

	VOID			OnChangLeader(CMessage *pMsg);								//! 响应改变队长

	VOID			OnKickPlayer(CMessage *pMsg);								//! 响应踢人

	VOID			OnSetGoods(CMessage *pMsg);									//! 响应物品分配设置

	VOID			OnRecruit(CMessage *pMsg);									//! 响应招募设置

	VOID			CloseRecruit(const CGUID &PlayerGuid);						//! 关闭玩家的招募

	VOID			OnBeRecruited(CMessage *pMsg);								//! 响应应征

	VOID			OnChat(CMessage *pMsg);										//! 响应聊天

	VOID			OnTeamMemberAllIdioinfo(CMessage *pMsg);					//! 响应成员详细信息转发

	VOID			OnTeamMemberPos(CMessage *pMsg);							//! 响应成员位置信息转发

	bool			PlayerJoinTeam(const CGUID& PlayerGuid, const CGUID& TeamGuid);		//! 玩家进入一个队伍

	VOID			PlayerLeaveTeam(const CGUID& PlayerGuid);					//! 玩家离开队伍

	const CGUID&	FindPlayerTeamID(const CGUID& PlayerGuid);					//! 得到一个玩家的队伍ID

	WSTeam*	CreateTeam(const CGUID& TeamGuid, const CGUID& MastterGuid, const CGUID& MemberGuid);	//! 创建队伍，并加入管理列表


	VOID			DisbandTeam(WSTeam *pTeam);									//! 解散队伍
	

private:
	map<CGUID, WSTeam*>				m_TeamOrganizings;							//! 全局所有组队

	map<CGUID, CGUID>				m_MemberIDvsTeamID;							//! 全局所有成员ID对应的队伍ID

	map<CGUID, CPlayer*>			m_RecruitingPlayer;							//! 正在招募的玩家

	//map<CGUID, long>				m_ChangeGSPlayerIDAndOuttime;				//! 正在转换服务器的队伍玩家对应的超时时间
	S2CSession<tagPlayerTeamSession>	m_InviteSession;						//! 组队邀请会话
	

};

//得到组织控制类句柄
inline COrganizingCtrl* GetOrganiCtrl()
{
	return COrganizingCtrl::getInstance();
}
