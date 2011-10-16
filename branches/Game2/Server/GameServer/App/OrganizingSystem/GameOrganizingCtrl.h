#pragma once


//OrganizingCtrl.h
//Function:	//该类负责控制帮派的建立、解散、以及对帮、同盟的管理控制.
			//负责对Organizings变量的管理
			//负责帮会系统的装载、存储，更新
//Author:Joe
//Create Time:2004.6.11
//Note:该类全局只有唯一的实例
#include "GameFaction.h"
#include "GameUnion.h"

class CPlayer;
class GSTeam;

//用来制作帮会排行时候的Key
struct tagKey 
{
	long	l;
	tagTime Time;
};

//帮会排行榜结构
struct tagFacBillboard
{
	long	lFactionID;		//帮会ID
	string	strName;		//帮会名字
	long	lNum;			//数量
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



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GameOrganizingCtrl
{
public:
	GameOrganizingCtrl(void);
	~GameOrganizingCtrl(void);
	//属性
public:
	//!							得到唯一实例句柄
	static GameOrganizingCtrl*	getInstance();									
	//!							释放对象
	static		void			Release(void);													



	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//工会
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	//!							响应工会系统的初始化消息
	void						OnInitGsFaction(CMessage *pMsg);
	

	//!							响应工会创建回复
	void						OnCreateFactionRe(CMessage *pMsg);
	//!							响应工会删除
	void						OnWsDelFaction(CMessage *pMsg);
	//!							响应WS邀请回复
	void						OnWsInvitePlayerRe(CMessage *pMsg);
	//!							响应WS添加成员通知
	void						OnWsAddMember(CMessage *pMsg);
	//!							响应WS职级名称设置
	void						OnWsSetJobName(CMessage *pMsg);
	//!							响应WS职级权限设置
	void						OnWsSetJobPurview(CMessage *pMsg);
	//!							响应成员退出通知
	void						OnWsMemberQuit(CMessage *pMsg);
	//!							响应公告编辑通知
	void						OnWsEditPronounce(CMessage *pMsg);
	//!							响应备忘编辑通知
	void						OnWsEditLeaveword(CMessage *pMsg);
	//!							响应开除成员通知
	void						OnWsFire(CMessage *pMsg);
	//!							设置成员的职级
	void						OnWsSetMemJobLvl(CMessage *pMsg);
	//!							设置成员的称号
	void						OnWsSetMemberTitle(CMessage *pMsg);
	//!							成员地图通知
	void						OnWsMemberMap(CMessage *pMsg);
	//!							成员等级通知
	void						OnWsMemberLevel(CMessage *pMsg);
	//!							成员隐匿通知
	void						OnWsMemberHide(CMessage *pMsg);
	//!							成员职业通知
	void						OnWsMemberOccu(CMessage *pMsg);
	//!							设置新会长
	void						OnWsSetMaster(CMessage *pMsg);
	//!							在线状态变化
	void						OnWsMemOnlineChange(CMessage *pMsg);
	//!							申请人员变动
	void						OnWsApplicantChange(CMessage *pMsg);
	//!							行会等级改变
	void						OnWsFacLvlChange(CMessage *pMsg);
	//!							行会图标改变
	void						OnWsIconChange(CMessage *pMsg);
	//!							响应执行福利
	void						OnBoonRun(void);
	//--------------------------------------------------------------------------------
	//!							找出一个工会
	GameFaction*				GetFactionOrganizing(const CGUID &FacGuid);
	//!							找出一个玩家的工会ID
	const		CGUID&			FindPlayerFacionID(const CGUID &PlayerGuid);
	//!							更新一个玩家的工会信息
	void						UpdateFacInfo(CPlayer *pPlayer);

	//!							玩家打开加入行会申请面板
	void						OpenApplyBoard(const CGUID &PlayerGuid);
	//!							玩家打开上传图标面板
	void						OpenUpLoadIconBoard(const CGUID &PlayerGuid);
	//!							玩家设置工会招募状态
	bool						SetFactionRecruit(CPlayer *pPlayer, bool bOpen);
	//!							得到玩家所在工会等级
	LONG						GetFactionLvl(CPlayer *pPlayer);


	//--------------------------------------------------------------------------------
	//! 玩家工会操作类型
	enum ePlayerFacOptType
	{
		//! 创建操作
		ePFOT_Create,
		//! 升级操作
		ePFOT_Upgrade,
		//! 申请加入操作
		ePFOT_Apply,
		//! 上传图标
		ePFOT_UpLoadIcon,

		//! 创建联盟操作
		ePFOT_CreateUnion,
	};
	//!							一个玩家开始工会操作
	void						PlayerFacOptBegin(const CGUID &PlayerGuid, LONG lOptType);
	
private:
	//!							验证玩家操作
	BOOL						TestFacOpt(const CGUID &PlayerGuid, LONG lOptType);
	//!							一个玩家结束工会操作
	void						PlayerFacOptEnd(const CGUID &PlayerGuid, LONG lOptType);

	//--------------------------------------------------------------------------------
public:
	//!							响应玩家创建请求
	void						OnPlayerCreateFactionReques(CMessage *pMsg);
	//!							响应玩家解散请求
	void						OnPlayerDisbandFactionReques(CMessage *pMsg);
	//!							响应玩家邀请操作
	void						OnPlayerFacInviteReques(CMessage *pMsg);
	//!							响应玩家回答邀请
	void						OnPlayerFacInviteAnswer(CMessage *pMsg);
	//!							响应玩家修改公告
	void						OnPlayerFacSetPlacardRequest(CMessage *pMsg);
	//!							响应玩家修改备忘
	void						OnPlayerFacSetLeavewordRequest(CMessage *pMsg);
	//!							响应玩家踢人
	void						OnPlayerKickReques(CMessage *pMsg);
	//!							响应设置称号
	void						OnPlayerFacSetMemberTitleReques(CMessage *pMsg);
	//!							响应传位
	void						OnPlayerFacSetChairmanReques(CMessage *pMsg);
	//!							审批入会请求
	void						OnPlayerFacCheckRequisition(CMessage *pMsg);
	//!							玩家申请入会
	void						OnPlayerJoinFac(CMessage *pMsg);
	//!							响应玩家上传图标请求
	void						OnPlayerUpLoadIconReques(CMessage *pMsg);

	//!							响应行会升级的调用
	void						OnPlayerUpgradeFac(CPlayer *pPlayer, DWORD dwUpgradeType);
	//!							检测升级工会的条件
	eFactionOptErrInfo			TestUpgradeFacCondition(CPlayer *pPlayer, LONG lGoalLvl, DWORD dwUpgradeType);
	//!							检测创建联盟的条件
	eFactionOptErrInfo			TestCreateUnionCondition(CPlayer *pPlayer);

private:
	
	//!							升级工会，按配置的条件
	eFactionOptErrInfo			UpgradeFac_ByCondition(CPlayer *pPlayer, LONG lGoalLvl, DWORD dwUpgradeType);
	//!							按升级条件回滚GS上的升级工会操作
	void						BackRoll_UpgradeFac(CPlayer *pPlayer, LONG lGoalLvl, DWORD dwUpgradeType);
	//!							创建工会，按配置的条件
	eFactionOptErrInfo			CreateFac_ByCondition(CPlayer *pPlayer);
	//!							创建联盟，按配置的条件
	eFactionOptErrInfo			CreateUnion_ByCondition(CPlayer *pPlayer);
public:
	//!							发送初始化玩家通知
	void						SendInitFacDataToPlayer(const CGUID &PlayerGuid);
	void						SendInitFacDataToPlayer(void);
private:
	//!							广播一个行会的公共数据
	void						RadiateFactionPublic(GameFaction *pFaction);
	//！						更新图标临时数据
	//void						UpdateIconData(void);


private:
	//!									工会数据完整性标记
	BOOL								m_bFacDataFull;
	//!									当前工会图标的CRC32效验码
	//DWORD								m_dwCurrIconCRC32Code;
	////!									当前工会图标数据编码
	//vector<BYTE>						m_vCurrIconData;
	//!									等待初始化数据的客户端
	set<CGUID>							m_setsetWaitingClient;

	//!									全局所有的帮派组织
	map<CGUID, GameFaction*>			m_FactionOrganizings;
	//!									等待创建的玩家
	set<CGUID>							m_mapWaitCreatePlayer;
	//!									所有已申请入会的玩家
	map<CGUID, CGUID>					m_mapAllApplicant;


	
	struct tagFacOptSession
	{
		CGUID	PlayerGuid;
		LONG	lOptType;
		BOOL operator ==(const tagFacOptSession& right)
		{
			return (right.PlayerGuid == PlayerGuid) && (right.lOptType == lOptType);
		}
	};
	//!									工会操作会话
	S2CSession<tagFacOptSession>		m_PlayerOptSession;
	

	struct tagFacInviteSession
	{
		CGUID	InviterGuid;
		CGUID	AimGuid;
		CGUID	FactionGuid;
		BOOL operator ==(const tagFacInviteSession& right)
		{
			return (right.InviterGuid == InviterGuid) && (right.AimGuid == AimGuid);
		}
	};
	//!									工会邀请会话
	S2CSession<tagFacInviteSession>		m_PlayerInviteSession;

	


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//联盟
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	GameUnion*					GetUnion(const CGUID &UnionGuid);

	//!							响应联盟系统的初始化消息
	void						OnInitGsUnion(vector<BYTE> &vData);
	//!							响应建立新联盟消息
	void						OnCreateNewUnion(vector<BYTE> &vData);
	//!							响应WS解散联盟
	void						OnWsDisbandUnion(const CGUID &UnionGuid);
	//!							响应退出联盟
	void						OnWsQuitUnion(const CGUID &FactionGuid, const CGUID &UnionGuid);
	//!							响应开除联盟成员
	void						OnWsKickUnion(const CGUID &OptGuid, const CGUID &AimGuid, const CGUID &UnionGuid);
	//!							响应传位
	void						OnWsSetChairman(const CGUID &UnionGuid, const CGUID &NewMasterFacGuid, const char *pNewMasterName);
	//!							响应设置权限
	void						OnWsSetPurviewUnion(const CGUID &UnionGuid, const CGUID &AimFacGuid, DWORD dwPurview);


	//!							响应玩家创建联盟请求
	void						OnPlayerCreateUnion(CPlayer *pPlayer, const char *pUnionName);
	//!							响应玩家解散联盟消息
	void						OnPlayerDisbandUnion(CPlayer *pPlayer);
	//!							响应玩家请求退出联盟
	void						OnPlayerQuitUnion(CPlayer *pPlayer);
	//!							响应玩家开除请求
	void						OnPlayerKick(CPlayer *pPlayer, const CGUID &AimFactionGuid);
	//!							响应设置盟主
	void						OnPlayerSetChairman(CPlayer *pPlayer, const CGUID &AimFactionGuid);
	//!							响应玩家设置权限
	void						OnPlayerSetPurview(CPlayer *pPlayer, const CGUID &AimFactionGuid, DWORD dwPurview);
	//!							响应玩家邀请加入联盟
	void						OnPlayerUnionInvite(CPlayer *pPlayer, const char *pAimPlayerName);

	//!							测试一个工会是否在战争状态
	BOOL						TestWarState(const CGUID &FactionGuid);
	//!							发送玩家联盟数据
	void						SendPlayerUnionData(CPlayer *pPlayer, const CGUID &UnionGuid);


private:
	//!							所有的同盟组织
	map<CGUID, GameUnion*>		m_mapUnion;

	//!							等待创建的玩家
	set<CGUID>					m_mapWaitCreateUnionPlayer;



	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//组队操作begin
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	GSTeam*			GetGameTeamByID(const CGUID& TeamID);		//! 得到指定的队伍指针

	void			GetTeamData(CMessage *pMsg);				//! 从消息中获取全局所有队伍

	GSTeam*			CreateTeam(CMessage *pMsg);					//! 根据WS消息，创建队伍

	void			DisbandTeam(const CGUID& TeamID);			//! 解散队伍

	void			DisbandAllTeam(void);						//! 解散所有队伍

	void			OnTeamSetupInit(CMessage *pMsg);			//! 响应队伍配置初始化

	void			UnpdateTeamMemberPos();						//! 更新一次队伍中的成员位置

	long			GetTeamNum(void){return (long)m_TeamOrganizings.size();};

private:
	map<CGUID,GSTeam*>					m_TeamOrganizings;	//! 全局所有组队


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////


private:
	static GameOrganizingCtrl* instance;										//该类唯一实例句柄

};

//得到组织控制类句柄
inline GameOrganizingCtrl* GetOrganiCtrl()
{
	return GameOrganizingCtrl::getInstance();
}
