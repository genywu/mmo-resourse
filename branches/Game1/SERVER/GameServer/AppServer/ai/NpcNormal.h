#pragma once
#include "MonsterAI.h"


class CNpcNormal :public CMonsterAI
{
public:
	CNpcNormal();
	~CNpcNormal();
protected:
	BOOL RandomRun();
	BOOL SearchEnemy();
public:
	BOOL WhenBeenHurted(LONG lType,const CGUID& guid,DWORD a);

};