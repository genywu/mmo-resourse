//Account.cpp/////////////////////////////////////////////////////////////////////
//账号类:
//Author:安海川
//Create Time:2008.11.03
#include "StdAfx.h"
#include "Account.h"
#include <strstream>
#include "EntityManager.h"

	// ============================================================================
	//  Account Instance Class
	// ============================================================================
	Account::Account()
		: CEntityGroup(string("[account]"), NULL_GUID),
		m_lSignCode(0),m_lDelTime(-1),
		m_PlayerState(PLAYER_STATE_VOID),
		m_eWsSessionStepStatus(CWorldServerSession::WORLD_SESSION_STEP_NULL),
		m_CurGamedPlayerGuid(NULL_GUID),
		m_bIsInDeleteMailSession(false),
		m_bIsLoadMailSession(false),
		m_lLoginTimeoutValue(-1)
		
	{
		m_CompositeType = COM_COMPOSITE;
	}

	Account::~Account()
	{
	}

	const CGUID& Account::GetPlayerIdByName(const char* szName)
	{
		// 该对象为LoginPlayerGroup
		CEntityGroup* pEntityGroup = (CEntityGroup*)((m_mapEntityGroup.begin())->second);
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
			AddLogText("AddLoginPlayer LoginPlayerGroup is null!");
			return;
		}
		CEntityGroup* pEntityGroup = (CEntityGroup*)((m_mapEntityGroup.begin())->second);
		if(pEntityGroup)
		{
			std::map<CGUID, CBaseEntity*>::iterator itr = pEntityGroup->GetEntityGroupMap().find(lPlayer->GetGUID());
			if(itr != pEntityGroup->GetEntityGroupMap().end())
			{
				if(lPlayer != itr->second)
				{
					GetGame()->GetEntityManager()->DelBaseEntity((CBaseEntity*)itr->second);
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
			AddLogText("DelLoginPlayer LoginPlayerGroup is null!");
			return;
		}

		CEntityGroup* pEntityGroup = (CEntityGroup*)((m_mapEntityGroup.begin())->second);
		if(pEntityGroup)
		{
			std::map<CGUID, CBaseEntity*>::iterator itr = pEntityGroup->GetEntityGroupMap().find(guid);
			if(itr != pEntityGroup->GetEntityGroupMap().end())
			{
				GetGame()->GetEntityManager()->DelBaseEntity((CBaseEntity*)itr->second);
				pEntityGroup->GetEntityGroupMap().erase(itr);
			}
		}
	}
	void	Account::ClearAllLoginPlayer(void)
	{
		if(m_mapEntityGroup.size() != 1)
		{
			AddLogText("ClearAllLoginPlayer() Account[%s]的Group对象为空!", GetName());
			return;
		}
		CEntityGroup* pEntityGroup = (CEntityGroup*)((m_mapEntityGroup.begin())->second);
		if(pEntityGroup)
		{
			pEntityGroup->ReleaseChilds();
		}
	}
	CEntityGroup* Account::FindLoginPlayer(const CGUID& guid)
	{
		if(m_mapEntityGroup.size() != 1)
		{
			char szGuid[128];
			guid.tostring(szGuid);
			AddLogText("FindLoginPlayer() Account PID[%s]的Group对象为空!", szGuid);
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