///
/// @file DHFGladiator.h
/// @author Kevin Lynx
/// @brief 优先记录-主动怪物AI
///
#pragma once

#include "MonsterAI.h"
#include <list>

///
/// DHF( Distance and Hurted First )优先攻击离自己最近且攻击过自己的目标
///
class CDHFGladiator : public CMonsterAI
{
public:
	typedef std::list<CGUID> TargetListType;
public:
	BOOL SearchEnemy();

	BOOL WhenBeenHurted( LONG lAttackerType, const CGUID &attackerID, DWORD dwHurt );

	void SetTarget( LONG lType, const CGUID &TargetGUID );

	void ResumeAll( bool bForce = false )
	{
		CMonsterAI::ResumeAll( bForce );
		m_Targets.clear();
	}

private:
	TargetListType m_Targets;
};
