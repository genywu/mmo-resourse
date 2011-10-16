#include "StdAfx.h"
#include "NpcNormal.h"



CNpcNormal::CNpcNormal()
{
}
CNpcNormal::~CNpcNormal()
{
}
BOOL CNpcNormal::RandomRun()
{
	Stand(1000);
	return TRUE;
}
BOOL CNpcNormal::SearchEnemy()
{
	return TRUE;
}

BOOL CNpcNormal::WhenBeenHurted(LONG lType,const CGUID& guid,DWORD a)
{
	return TRUE;
}