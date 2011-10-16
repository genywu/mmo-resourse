#include "StdAfx.h"
#include ".\playerid.h"

//CPlayerID::CPlayerID(void)
//{
//}
//
//CPlayerID::~CPlayerID(void)
//{
//}

CPlayerID *CPlayerID::instance = NULL;

CPlayerID* CPlayerID::GetInstance()
{
	if(!instance)
	{
		instance = new CPlayerID();
	}
	return instance;
}

void CPlayerID::Release()
{
	if(instance)
	{
		delete instance;
		instance = NULL;
	}
}

