/**
*File:  Account.cpp
*brief: 账号类:
*Author:安海川
*Date:  2008.11.03
*/
#include "stdafx.h"
#include "Account.h"

#include "EntityGroup.h"
#include "EntityManager.h"
#include "../Thread/ThreadDataDef.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
	// ============================================================================
	//  Account Instance Class
	// ============================================================================
	Account::Account()
		: CEntityGroup(string("[account]"), NULL_GUID),m_lLastLoginTime(-1)
	{	  
		CBaseEntity::m_CompositeType = COM_COMPOSITE;
	}
	Account::Account(const CGUID& guid)
		: CEntityGroup(string("[account]"), guid),m_lLastLoginTime(-1)
	{
		CBaseEntity::m_CompositeType = COM_COMPOSITE;
	}

	Account::~Account()
	{
	}

	Account& Account::operator=(Account& right)
	{
		CEntityGroup::operator=(right);
		if(this != &right)
		{
			m_lLastLoginTime = right.m_lLastLoginTime;
		}
		return *this;
	}

	const CGUID& Account::GetPlayerIdByName(const char* szName)
	{
		// 该对象为LoginPlayerGroup
		CEntityGroup* pEntityGroup = (CEntityGroup*)(m_mapEntityGroup.begin()->second);
		if(pEntityGroup)
		{
			std::map<CGUID, CBaseEntity*>::iterator itr = pEntityGroup->GetEntityGroupMap().begin();
			for(; itr != pEntityGroup->GetEntityGroupMap().end(); itr++)
			{
				CEntityGroup* pEntity = (CEntityGroup*)itr->second;
				if( pEntity && strcmp(pEntity->GetName(), szName) == 0 )
					return pEntity->GetGUID();
			}
		}
		return NULL_GUID;
	}
	void Account::AddLoginPlayer(CEntityGroup* lPlayer)
	{
		if(!lPlayer) return;

		if(m_mapEntityGroup.size() != 1)
		{
			Log4c::Warn(ROOT_MODULE,"DB_ACCOUNT_1");//AddLoginPlayer LoginPlayerGroup is null!
			return;
		}

		CEntityGroup* pEntityGroup = (CEntityGroup*)(m_mapEntityGroup.begin())->second;
		if(pEntityGroup)
		{
			std::map<CGUID, CBaseEntity*>::iterator itr = pEntityGroup->GetEntityGroupMap().find(lPlayer->GetGUID());
			if(itr != pEntityGroup->GetEntityGroupMap().end())
			{
				if(lPlayer != itr->second)
				{
					delete itr->second;
					pEntityGroup->GetEntityGroupMap().erase(itr);
				}
			}
			pEntityGroup->GetEntityGroupMap()[lPlayer->GetGUID()] = lPlayer;
		}
	}
	void Account::DelLoginPlayer(const CGUID& guid)
	{
		if(m_mapEntityGroup.size() != 1)
		{
			Log4c::Warn(ROOT_MODULE,"DB_ACCOUNT_2");//DelLoginPlayer LoginPlayerGroup is null!
			return;
		}

		CEntityGroup* pEntityGroup = (CEntityGroup*)((m_mapEntityGroup.begin())->second);
		if(pEntityGroup)
		{
			std::map<CGUID, CBaseEntity*>::iterator itr = pEntityGroup->GetEntityGroupMap().find(guid);
			if(itr != pEntityGroup->GetEntityGroupMap().end())
			{
				delete itr->second;
				pEntityGroup->GetEntityGroupMap().erase(itr);
			}
		}
	}
	CEntityGroup* Account::FindLoginPlayer(const CGUID& guid)
	{
		if(m_mapEntityGroup.size() != 1)
		{
			char szGuid[128];
			guid.tostring(szGuid);
			Log4c::Warn(ROOT_MODULE,"DB_ACCOUNT_0", szGuid);//FindLoginPlayer() Account PID[%s]的Group对象为空!
			return NULL;
		}

		CEntityGroup* pEntityGroup = (CEntityGroup*)((m_mapEntityGroup.begin())->second);
		if(pEntityGroup)
		{
			std::map<CGUID, CBaseEntity*>::iterator itr = pEntityGroup->GetEntityGroupMap().find(guid);
			if(itr != pEntityGroup->GetEntityGroupMap().end())
				return (CEntityGroup*)itr->second;
		}
		return NULL;
	}