//WSTeam:		继承于COrganizing，保存玩家队伍数据，为GS提供全局数据同步操作
//Author:		Fred
//Create Time:	2007.7.18

#pragma once

#include "../../../../Public/Common/TeamDef.h"

class WSTeam
{
public:
	WSTeam();
	~WSTeam(void);

public:

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
		long	lExpDiff;			//经验差
		long	lExpLimit;			//玩家经验值份额基本保障比例

		float	fExpAmerce;			//经验惩罚系数	
		
		tagDefaultSetup(long lGA, long lMMN, long lSTS);
	};
	struct tagShareQuestList
	{
		long lQuestID;
		char szName[20];
	};
public:
	//处理要加入组织的人员
	bool DoJoin(const CGUID& ExecuteGuid,const CGUID& BeExecutedGuid,long lAgree,tagTime& Time);	
	//邀请人员加入
	bool Invite(const CGUID& InvitedGuid, const CGUID& BeInvitedGuid) ;							
	//退出
	bool Exit(const CGUID& PlayerGuid);													
	//开除
	bool FireOut(const CGUID& FireID,const CGUID& BeFiredID);									
	//传位
	bool Demise(const CGUID& MasterID,const CGUID& guid);										
	//解散
	bool Disband(const CGUID& guid);														
	//得到组织的ID
	//long GetID();
	const	CGUID& GetExID();	
	void	SetExID(const CGUID& guid);
	//得到掌门的ID
	const CGUID& GetMasterID();													
	//得到成员列表
	void GetMemberList(list<CGUID>& TemptList);									
	//该玩家是帮主否
	const CGUID& IsMaster(const CGUID& guid);													
	//该玩家是否是该帮成员否
	const CGUID& IsMember(const CGUID& guid);													
	//得到成员数
	long GetMemberNum();														
	//得到头领
	const CGUID& GetPlayerHeader();														
	//更新玩家的信息到组织的成员列表
	bool UpdatePlayerDataToOrgMemberList(const CGUID& PlayerGuid);	
	//! 判断队员的最大等级差是否在输入的范围内
	BOOL InLvlDifference(LONG lMaxLvlSpace);
	//! 判断所有队员的等级是否都在这个范围内
	BOOL InLvlSpace(LONG lMin, LONG lMax);
	//出发一个成员进入游戏
	void OnMemberEnterGame(const CGUID& MemGuid);										
	//触发一个成员离开游戏
	void OnMemberExitGame(const CGUID& MemGuid);											
	//成员等级发生了变化
	void OnMemberLvlChange(const CGUID& MemGuid,long lLvl);								
	//成员改变了位置(场景)
	void OnMemberPosChange(const CGUID& MemGuid,long lNewPosID);							
	//当队员退出的时候删除共享列表此队员的推荐任务
	void OnExit(const char* szname,const CGUID& PlayerGuid);
protected:
	bool Initial();															//! 初始化

public:
	bool Initial(const CGUID& TeamGuid, const CGUID& MastterGuid, const CGUID& MemberGuid);			//! 初始化对象

	list<tagWSMemberInfo>::iterator FindMember(const CGUID& PlayerGuid);	//! 找到一个成员的位置

	long GetOnlineMemberNum(void);											//! 得到在线成员的数量

	bool PlayerInTeam(const CGUID& PlayerGuid);

	list<CGUID>	GetMemberIDList();											//! 得到成员ID列表

	//void UpdateTeamInfoToGS(void);										//! 将队伍信息更新到GS

	const CGUID& SetNewMastter();											//! 任命新队长

	void SetGoodsAllot(long eSet);											//! 设置物品分配办法

	bool AddToByteArray(vector<BYTE>& pByteArray);
	
	//! 更新共享任务列表,true清空共享列表
	void UpdateShareQuestList(long lGSID,tagShareQuestList* quest=NULL,bool flag=false,CPlayer* player=NULL);		
	bool FindShareQuest(long lquest);										//! 查找共享任务列表
	void SetCurrShareQuest(long lGSID,long questID);
	long GetCurrShareQuest()					{return m_lCurrShareQuestID;}
	bool DelCurrShareQuest(long lGSID,char* name);
	void SetMemberQuest(long lGSID,CGUID &guid,long lQuestID = 0);
	void ClearMemberQuest();
private:
	void PushBackToMsg(CMessage &msg);										//! 将完整的队伍数据添加到消息末尾

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//成员
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	//! 队伍ID
	CGUID								m_guid;
	//! 队长ID
	CGUID								m_MestterGuid;
	//! 成员列表
	list<tagWSMemberInfo>				m_MemberList;
	//! 物品分配方式
	long								m_eGoodsAllot;
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