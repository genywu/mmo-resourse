#include "StdAfx.h"
#include "Gladiator.h"
#include "..\Player.h"
#include "..\Monster.h"



CGladiator::CGladiator()
{
}

CGladiator::~CGladiator()
{
}
BOOL CGladiator::SearchEnemy()
{
	CMonsterAI::SearchEnemy();
	return TRUE;
}

VOID CGladiator::SetTarget( LONG lType, const CGUID& TargetGuid )
{	
	CBaseAI::SetTarget(lType,TargetGuid);
}
