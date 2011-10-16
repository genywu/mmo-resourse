#pragma once
#include "MonsterAI.h"
#include "..\monster.h"

/*
*AI基本类型:肉搏普通
*/
class CGladiator :
	public CMonsterAI
{
protected:

	BOOL SearchEnemy();
public:
	CGladiator										();
	~CGladiator										();
	// 修正锁定错误的BUG by kevin lynx
	VOID SetTarget( LONG lType, const CGUID& TargetGuid );
};
