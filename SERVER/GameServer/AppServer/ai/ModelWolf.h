#pragma once
#include "MonsterAI.h"


class CModelWolf :public CMonsterAI
{
public:
	CModelWolf();
	~CModelWolf();
protected:
	VOID SetTarget(LONG lType, const CGUID& guid);
	BOOL OnPeaceIdle();
	INT  GetFriendNumAround(CMonster** FightMonster);
	BOOL WhenBeenHurted(LONG lType,const CGUID& guid,DWORD a);
private:
	void EnterFight();
protected:
	BOOL m_bHasFightFriendAround;
	POINT m_StartFolling;
};