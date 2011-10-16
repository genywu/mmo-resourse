//WSTeam:		继承于COrganizing，保存玩家队伍数据，为GS提供全局数据同步操作
//Author:		Fred
//Create Time:	2007.7.18

#pragma once
#include "..\..\public\TeamDef.h"

class WSTeam:public BaseMemObj
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
	//退出
	bool Exit(const CGUID& PlayerGuid);																						
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
								
	//该玩家是帮主否
	const CGUID& IsMaster(const CGUID& guid);													
	//该玩家是否是该帮成员否
	const CGUID& IsMember(const CGUID& guid);													
	//得到成员数
	long GetMemberNum();		
	//! 是否成员都在一个地图
	bool IsInSameRegion(void);
	//得到头领
	const CGUID& GetPlayerHeader();														
	//! 判断队员的最大等级差是否在输入的范围内
	BOOL InLvlDifference(LONG lMaxLvlSpace);
	//! 判断所有队员的等级是否都在这个范围内
	BOOL InLvlSpace(LONG lMin, LONG lMax);
	//出发一个成员进入游戏
	void OnMemberEnterGame(CPlayer *pMember);										
	//触发一个成员离开游戏
	void OnMemberExitGame(const CGUID& MemGuid);											
	//! 响应队员积分改变
	void OnMebereChangePoint(const CGUID &MemberGuid, long ChangePoint);
	//当队员退出的时候删除共享列表此队员的推荐任务
	void OnExit(const char* szname,const CGUID& PlayerGuid);


public:
	bool Initial(const CGUID& TeamGuid, const CGUID& MastterGuid, const CGUID& MemberGuid);			//! 初始化对象

	list<tagWSMemberInfo>::iterator FindMember(const CGUID& PlayerGuid);	//! 找到一个成员的位置

	long GetOnlineMemberNum(void);											//! 得到在线成员的数量

	long GetMemberMinLvl(void);												//! 得到成员最小等级，若有队员不在线，会返回0

	bool PlayerInTeam(const CGUID& PlayerGuid);

	list<CGUID>	GetMemberIDList();											//! 得到成员ID列表

	//void UpdateTeamInfoToGS(void);										//! 将队伍信息更新到GS

	const CGUID& SetNewMastter();											//! 任命新队长

	void SetGoodsAllot(long eSet);											//! 设置物品分配办法

	//! 得到战队积分（所有成员积分的总和）
	long GetSentaiPoint(void){return m_SentaiPoint;}
	//! 设置惩罚分数
	void SetPenaltyPoints(long Points);
	//! 设置操作控制
	void SetOptControl(long ControlType);
	//! 获得操作控制
	long GetOptControl(void){return m_eOptControl;}	
	//! 能否进行一个操作
	bool IsAbleControl(eTeamOptControlType ControlType){return (0 != (ControlType & m_eOptControl));}					


	bool AddToByteArray(vector<BYTE>& pByteArray);
	
	//! 更新共享任务列表,true清空共享列表
	void UpdateShareQuestList(long lGSID,tagShareQuestList* quest=NULL,bool flag=false);		
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
	//! 队伍操作控制
	long								m_eOptControl;
	//! 队伍的战队总积分
	long								m_SentaiPoint;
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