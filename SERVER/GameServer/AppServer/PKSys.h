//////////////////////////////////////////////////////////////////////////
//PKsys.h
//Fun:PK系统的先手攻击、杀人、死亡规则
//Author:Joe
//Create Time:2005.6.24
//////////////////////////////////////////////////////////////////////////
#include "Region.h"

struct tagDiedLost;
class CPlayer;
class CServerRegion;

#pragma once

class CPKSys : public BaseMemObj
{
public:
	CPKSys(void);
	~CPKSys(void);
public:
	//判断一个MoveShape是否可以攻击另外一个MoveShape
	bool	IsAttackAble(CMoveShape* pAttacker,CMoveShape* pBeAttacker);
	//判断是否是敌对关系
	bool	IsEnemy(CMoveShape* pShape1,CMoveShape* pShape2);

	void	OnFirstAttack(CPlayer* pAttackPlayer,CPlayer* pBeAttackedPlayer,
							CServerRegion* pRegion,CRegion::eSecurity SecurityType);					//先手攻击

	void	OnFirstSkill(CPlayer* pAttackPlayer,CPlayer* pBeAttackedPlayer,
							CServerRegion* pRegion,CRegion::eSecurity SecurityType);					//先手技能
	void	OnKill(CPlayer* pKillPlyaer,CPlayer* pBeKilledPlayer,CServerRegion* pRegion);				//杀死后处理
	void	OnDeath(CPlayer* pPlayer,CServerRegion* pRegion);											//死亡后处理
	
	long	GetDiedLostExp(CPlayer* pBeKilledPlayer, CServerRegion* pRegion,
		CRegion::eSecurity SecurityType);										//得到死亡掉的经验

	bool GetDiedLostGoods(CPlayer* pBeKilledPlayer,long lMurdererType,const CGUID& MurdererID,CServerRegion* pRegion,
						CRegion::eSecurity SecurityType,long lNameColor,tagDiedLost& DiedLost);//得到死亡掉落的物品的概率															//得到死亡的爆率
	//攻击方和被攻击方的的判断
	BOOL    JudgeAttackRelation(CMoveShape *pAttacker,CMoveShape *pBeenAttacker,int nAddSub);
	void    SetPKValue(CMoveShape *pAttacker,CMoveShape *pBeenAttacker,int nAddSub);
	void	GetCountryList(	long lCountyID,list<BYTE> &lCountryIDList);			//根据同盟ID得到国家列表
	bool	IsSameCamp(CMoveShape *pAttacker,CMoveShape *pBeenAttacker);	//阵营判断
private:
	bool	IsCityWarState(CPlayer* pPlayerA,CPlayer* pPlayerB);			//判断两个玩家是否出于城战状态
	bool	ISFactionWarState(CPlayer* pPlayerA,CPlayer* pPlayerB);			//判断两个玩家是否出于敌对帮战状态

	bool	IsCountryWarState(CPlayer* pPlayerA,CPlayer* pPlayerB);			//判断两个玩家是否出于国战状态
	

	

	VOID	ReportMurderer(CPlayer* pMurderer,CPlayer* pBMurderer);								//报告凶手
private:
	static CPKSys* pInstance;
public:
	static CPKSys* getInstance();
	void	Release();														//释放资源
};

inline CPKSys* GetPKSys()
{
	return CPKSys::getInstance();
}
