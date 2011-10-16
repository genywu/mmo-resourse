//GSTeam:		继承于COrganizing，保存玩家队伍数据，为GS提供全局数据同步操作
//Author:		Fred
//Create Time:	2007.7.18

#pragma once
#include "..\..\public\TeamDef.h"

class GSTeam : public BaseMemObj
{
public:
	GSTeam();
	~GSTeam(void);

public:

	//! GS成员信息
	struct tagMemberInfo : tagWSMemberInfo
	{
		bool bChanged;		//非成员信息发生了改变的标记
	};

	//! 全局默认设置
	struct tagDefaultSetup
	{
		long	lCorrelativeSkillID;//与组队关联的技能ID
		long	lMinSkillLevel;		//可以组队的最小技能等级
		long	lGoodsAllot;		//默认分配方式
		long	lMaxMemberNum;		//默认最大人数
		long	lSessionTimeoutS;	//会话超时秒数

		float	fIncreasedExp_2;	//2人经验加成比例
		float	fIncreasedExp_3;	//3人经验加成比例
		float	fIncreasedExp_4;	//4人经验加成比例
		float	fIncreasedExp_5;	//5人经验加成比例
		float	fIncreasedExp_6;	//6人经验加成比例

		long	lExperienceRadii;	//经验分配半径（单位：格）
		long	lExpDiff;			//
		long	lExpLimit;			//

		float	fExpAmerce;			//经验惩罚系数

		tagDefaultSetup(long lGA, long lMMN, long lSTS);
	};
	struct tagShareQuestList
	{
		long lQuestID;
		char szName[20];
	};
public:
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////							
	const	CGUID&	GetExID(void);														//! 得到ID
	const	CGUID&	SetExID(const CGUID& guid);											//! 设置ID
	const	char*	GetName(void);
	const	char*	SetName(const char *pName);

	//退出
	bool	Exit(const CGUID& PlayerGuid);																				
	//传位
	bool	Demise(const CGUID& MasterGuid,const CGUID& guid);										
	//解散
	bool	Disband(const CGUID& guid, BOOL bNotifyClient = TRUE);														
															
	//得到成员列表
	void GetMemberList(list<CGUID>& TemptList);
	//该玩家是帮主否
	const CGUID& IsMaster(const CGUID& guid);
	//该玩家是否是该帮成员否
	const CGUID& IsMember(const CGUID& guid);
	bool IsMember( const std::string &name ) const;
	//得到掌门的ID
	const CGUID& GetMasterID(void);													
	//得到成员数
	long GetMemberNum();
	long GetMemberNumInRgn(const CGUID& RgnGuid);
private:
	//! 响应队员进入
	void OnMemberEnter(const CGUID& MemGuid);
	//! 响应队员退出
	void OnMemberExit(const CGUID& MemGuid);

public:
	void SwapTeamRgnID(CPlayer *player, CPlayer *pAim);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:	
	void GetDataFromArray(BYTE *pData, LONG &lPos);
	static void SendAboutSkillToC(vector<BYTE> *pByteArray);						//! 发送限制组队的技能ID和限制的级数到客户端
	static void SendAboutSkillToC(DBWriteSet& setWriteDB);				//! 发送限制组队的技能ID和限制的级数到客户端

	bool Initial(CMessage *pMsg);											//! 初始化对象

	bool DoJoin(CMessage *pMsg);											//! 玩家加入消息

	void RadiateInfo(char* szInfo);											//! 将信息发给所有成员

	void SetGoodsAllot(long eSet);											//! 修改物品分配设置

	void SetOptControl(long ControlType);									//! 设置操作控制

	//! 根据配置的分配方式，返回一个在拾取者服务器、拾取者地图的玩家指针，
	//! 参数为拾取者ID
	CPlayer* GetOneOwner(const CGUID& Picker);
	//! 得到可以分配者的数量
	long GetOneOwnerNum(void);

	//!
	long GetSentaiPoints(void){return m_SentaiPoint;}
	//!
	void SetSentaiPoints(long Points){m_SentaiPoint = Points;}
	//! 
	void ChangeRegion(long RegionID, long x, long y);


	void	GetOnServerTeamMember(list<CGUID> &relist);						//! 获得当前服务器上的成员列表

	void MarkPlayerPosChange(const CGUID& PlayerGuid);						//! 标记玩家所在位置发生了改变

	void RadiateAllMemberPos(void);											//! 广播所有成员位置

	void OnPlayerIdioinfoChange(const CGUID& PlayerGuid, eUpdateIdioinfo eInfo);		//! 响应玩家非队伍状态改变

	void OnPlayerIdioinfoChange(const CGUID& PlayerGuid, eUpdateIdioinfo eInfo, long lValue);		//! new 响应玩家非队伍状态改变

	void OnPlayerStateChange(const CGUID& PlayerGuid, long lState);					//! 响应玩家非队伍状态改变

	void SendToAllMember(CMessage *pMsg);									//! 将消息发送给所有成员

	void SendToAllRegionMember(CMessage *pMsg, long lRegionID);				//! 发送到所有在本地图的成员

	void PlayerLeaveHere(const CGUID& PlayerGuid);										//! 重新计算在本服务器的成员人数

	void MemberStateChange(const CGUID& PlayerGuid, long lState);					//! 成员在线状态改变

	void MemberMapChange(const CGUID& PlayerGuid);					//! 成员地图改变

	void TeamChat(CPlayer *pSender, char *pInfo);												//! 队聊

	void UpdateLeaderRecruitedNum(void);									//! 更新队长的已招募人数

	void SendMemberBuffToMember(CPlayer *pPlayer, const CGUID &SendAimGuid);

	void SendMemberBuffToAll(CPlayer *pPlayer, DWORD dwBuffID, DWORD dwBuffLevel, BOOL bOpen);



	const CGUID& GetLeader(void);													//! 得到领导ID

	long GetTeamatesAmount(void);											//! 成员数量

	//!	 得到一个受益区域内的活着的成员
	void GetLucrativeArea_Alive(map<CGUID,CPlayer*>& RegionTeamates, const CGUID &RegionGUID, LONG lPosX, LONG lPosY, BOOL bForGoods);
	void GetLucrativeArea_Alive(list<CGUID>& listMember, const CGUID &RegionGUID, LONG lPosX, LONG lPosY, BOOL bForGoods);

	//! 得到一个受益区域内的活着的成员的平均等级
	long GetLucrativeAreaAverageLevel_Alive(const CGUID &RegionGUID, LONG lPosX, LONG lPosY, BOOL bForGoods);	

	//! 得到一个受益区域内的活着的成员的平均职业等级
	long GetLucrativeAreaAverageOccuLevelCoe_Alive(const CGUID &RegionGUID, LONG lPosX, LONG lPosY, BOOL bForGoods);

	long GetCurrentRegionTeamatesAmount(const CGUID &RegionGUID);					//! 得到区域内队伍成员的数量
	
	//! 得到一个受益区域内的活着的成员数量
	long GetLucrativeAreaMemberAmount_Alive(const CGUID &RegionGUID, LONG lPosX, LONG lPosY, BOOL bForGoods);				

	long GetCurrentServerTeamatesAmount();									//! 得到在服务器上的队员数量

	CPlayer* FindTeamatesInCurrentRegion(const CGUID& RegionGUID);					//! 得到这个区域内的一个队伍成员

	long CalculateExperience(CPlayer *pPlayer, long lMonsterLevel, DWORD lExp, LONG lPosX, LONG lPosY);		//! 计算经验
	long CalculateOccuExperience(CPlayer *pPlayer, long lMonsterLevel, DWORD lExp, LONG lPosX, LONG lPosY);	//! 计算职业经验
	long CalculateOccuPoint(CPlayer *pPlayer, long lMonsterLevel, DWORD lOccuP, LONG lPosX, LONG lPosY);	//! 计算职业点数

	

	void AddQuest(DWORD dwQuestID,CPlayer* pPlayer,long lDistance);			//! 给队伍添加一个任务

	void RunScript(char* strScirptName,CPlayer* pPlayer,long lDistance, char* strScirptName2 = NULL);	//! 运行脚本	//获取队伍当前任务 
	
	void SetCurrShareQuest(long questID);

	long GetCurrShareQuest()					{return m_lCurrShareQuestID;}

	void UpdateShareQuestList(tagShareQuestList *quest=NULL,bool flag = false);						//! 更新共享任务列表
	long FindShareQuest(long lquest);
	void ClearShareQuestList();
	void SetAcceptTime(DWORD time)				{ m_dwAcceptTime=time; }
	DWORD GetAcceptTime()						{return m_dwAcceptTime;}
	void SetMemberQuest(CGUID &guid,long lQuestID);
	void ClearMemberQuest();
	bool IsCompleteQuest(CGUID &guid,long lQuestID);

	//!				判断是否为一个结婚队伍，如果是，获得另一方的GUID
	const CGUID&	GetMarriageOtherOne(CPlayer *pOnePlayer);
private:
	list<tagMemberInfo>::iterator FindMember(const CGUID& PlayerGuid);



	void PushBackToMsg(CMessage *pMsg);										//! 将完整的队伍数据添加到消息末尾

	void RadiateMemberData(list<tagMemberInfo>::iterator ite);				//! 广播一个成员数据

	void RadiateMemberData(const CGUID& PlayerGuid);									//! 广播一个成员数据

	void SendMemberData(tagMemberInfo &MemberInfo, list<tagMemberInfo>::iterator iteAim);					//! 发送一个成员数据给另一个成员



	void RadiateAllMemberIdioinfo(void);									//! 在队伍中广播所有成员非队伍信息

	bool GetIdioinfo(CPlayer *pPlayer, tagMemberIdioinfo &MemberIdioinfo);	//! 获取一个玩家的非队伍信息

	void IdioinfoMemberToMember(list<tagMemberInfo>::iterator iteSrc, list<tagMemberInfo>::iterator iteAim); //! 发送一个成员的信息给另一个成员

	void RadiateMemberIdioinfo(list<tagMemberInfo>::iterator ite);			//! 在队伍中广播某个成员非队伍信息

	void RadiateMemberIdioinfo(const CGUID& PlayerGuid);								//! 在队伍中广播某个成员非队伍信息

	void SendAllMemberIdioinfo(list<tagMemberInfo>::iterator ite);			//! 把所有成员非队伍信息发送给一个队员

	void SendAllMemberIdioinfo(const CGUID& PlayerGuid);								//! 把所有成员非队伍信息发送给一个队员



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//成员
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	//! 队伍ID
	CGUID								m_guid;
	//!
	char								m_szName[20];
	//! 队长ID
	CGUID								m_MestterGuid;
	//! 成员列表
	list<tagMemberInfo>					m_MemberList;
	//! 物品分配方式
	long								m_eGoodsAllot;
	//! 队伍操作控制
	long								m_eOptControl;
	//! 队伍的战队总积分
	long								m_SentaiPoint;
	//! 上一次的分配目标
	CGUID								m_LastAllotAim;
	//! 在本服务器上的成员数量
	long								m_lOnThisGsMemberNum;
	//共享赏金任务列表
	list<tagShareQuestList>				m_ShareQuestList;
	//队伍正在进行的赏金任务
	long								m_lCurrShareQuestID;
	//队长接受全队任务的时间
	DWORD								m_dwAcceptTime;
public:
	//! 队伍默认设置
	static	tagDefaultSetup				s_tagDefaultSetup;

};