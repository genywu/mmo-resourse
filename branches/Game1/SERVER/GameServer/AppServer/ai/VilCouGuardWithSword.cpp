#include "StdAfx.h"
#include ".\vilcouguardwithsword.h"
#include "..\Player.h"
#include "..\Monster.h"


CVilCouGuardWithSword::CVilCouGuardWithSword(void)
{
}

CVilCouGuardWithSword::~CVilCouGuardWithSword(void)
{
}
/*
CBaseAI::AI_EXEC_STATE CVilCouGuardWithSword::Run()
{
	return CMonsterAI::Run();
}

VOID CVilCouGuardWithSword::OnSchedule()
{
	CMonsterAI::OnSchedule();
}

VOID CVilCouGuardWithSword::OnIdle()
{
	//##第一次进入AI，首先保存位置坐标，然后添加一个改变当前技能的事件以初始化技能。
	if( m_pOwner && m_lX == -1 && m_lY == -1 )
	{
		m_lX = m_pOwner -> GetTileX();
		m_lY = m_pOwner -> GetTileY();

		AddAIEvent( ASA_CHANGE_SKILL );
	}
	CMonsterAI::OnIdle();
}

BOOL CVilCouGuardWithSword::OnMoving()
{
	if( CMonsterAI::OnMoving() )
	{
		AddAIEvent( ASA_SEARCH_ENEMY );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

VOID CVilCouGuardWithSword::WhenBeenHurted( LONG lType, LONG lID, DWORD dwStiffenTime )
{
	CBaseAI::WhenBeenHurted( lType, lID, dwStiffenTime );
	if( HasTarget() == FALSE )
	{
		SetTarget( lType, lID );
	}
}

CMoveShape* CVilCouGuardWithSword::SearchEnemyMember()
{
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner -> GetFather() );
	if( pRegion == NULL ) return NULL;
	
	//##如果当前攻击技能不存在.
	CSkill* pSkill = m_pOwner -> GetCurrentSkill();
	if( pSkill == NULL ) return NULL;

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( m_pOwner, TYPE_PLAYER, vOut );

	CPlayer* pTarget = NULL;
	for( size_t i = 0; i < vOut.size(); i ++ )
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>( vOut[i] );
		if( pPlayer == NULL ) 
		{
			continue;
		}

		//##这儿添加代码,如果Player和据点所有帮会属于同一个国家则继续
		if(pRegion->GetCountry() != 0 && pPlayer->GetCountry() == pRegion->GetCountry())
			continue;

		//##如果该玩家也在警戒范围内
		if( m_pOwner -> RealDistance(pPlayer) <= 
			m_pOwner -> GetAlertRange() )
		{
			//##如果当前目标不存在，就先把这个目标锁定。
			if( pTarget == NULL )
			{
				pTarget = pPlayer;
				continue;
			}

			//##如果该玩家比选择的目标的距离要短。
			if( m_pOwner -> RealDistance(pPlayer) < 
				m_pOwner -> RealDistance(pTarget) )
			{
				//##如果该玩家的距离比技能的最小攻击距离要大
				if( m_pOwner -> RealDistance(pPlayer) >=
					pSkill -> GetAffectRangeMin() )
				{
					pTarget = pPlayer;
				}
			}
			//##如果该玩家比选中目标的距离要远.
			else
			{
				//##如果选中的目标在技能的最小作用范围内.
				if( m_pOwner -> RealDistance(pTarget) <
					pSkill -> GetAffectRangeMin() )
				{
					pTarget = pPlayer;
				}
			}
		}
	}
	return pTarget;
}

CMoveShape* CVilCouGuardWithSword::SearchEnemyPet()
{
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner -> GetFather() );
	if( pRegion == NULL ) return NULL;
	
	//##如果当前攻击技能不存在.
	CSkill* pSkill = m_pOwner -> GetCurrentSkill();
	if( pSkill == NULL ) return NULL;

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( m_pOwner, TYPE_PET, vOut );

	CMonster* pTarget = NULL;
	for( size_t i = 0; i < vOut.size(); i ++ )
	{
		CMonster* pMonster = dynamic_cast<CMonster*>( vOut[i] );
		if( pMonster == NULL ) 
		{
			continue;
		}

		if( pMonster -> GetMasterInfo().lMasterType == TYPE_PLAYER )
		{
			CPlayer* pMaster = GetGame() -> FindPlayer( pMonster -> GetMasterInfo().lMasterID );
			if( pMaster )
			{
				//##这儿添加代码,如果Player和据点所有帮会属于同一个国家则继续
				if(pRegion->GetCountry() != 0 && pMaster->GetCountry() == pRegion->GetCountry())
					continue;
			}
		}

		//##如果该对象也在警戒范围内
		if( m_pOwner -> RealDistance(pMonster) <= 
			m_pOwner -> GetAlertRange() )
		{
			//##如果当前目标不存在，就先把这个目标锁定。
			if( pTarget == NULL )
			{
				pTarget = pMonster;
				continue;
			}

			//##如果该玩家比选择的目标的距离要短。
			if( m_pOwner -> RealDistance(pMonster) < 
				m_pOwner -> RealDistance(pTarget) )
			{
				//##如果该玩家的距离比技能的最小攻击距离要大
				if( m_pOwner -> RealDistance(pMonster) >=
					pSkill -> GetAffectRangeMin() )
				{
					pTarget = pMonster;
				}
			}
			//##如果该玩家比选中目标的距离要远.
			else
			{
				//##如果选中的目标在技能的最小作用范围内.
				if( m_pOwner -> RealDistance(pTarget) <
					pSkill -> GetAffectRangeMin() )
				{
					pTarget = pMonster;
				}
			}
		}
	}
	return pTarget;
}


CMoveShape* CVilCouGuardWithSword::SearchEnemyCarriage()
{
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner -> GetFather() );
	if( pRegion == NULL ) return NULL;
	
	//##如果当前攻击技能不存在.
	CSkill* pSkill = m_pOwner -> GetCurrentSkill();
	if( pSkill == NULL ) return NULL;

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( m_pOwner, TYPE_CARRIAGE, vOut );

	CMonster* pTarget = NULL;
	for( size_t i = 0; i < vOut.size(); i ++ )
	{
		CMonster* pMonster = dynamic_cast<CMonster*>( vOut[i] );
		if( pMonster == NULL ) 
		{
			continue;
		}

		if( pMonster -> GetMasterInfo().lMasterType == TYPE_PLAYER )
		{
			CPlayer* pMaster = GetGame() -> FindPlayer( pMonster -> GetMasterInfo().lMasterID );
			if( pMaster )
			{
				//##这儿添加代码,如果Player和据点所有帮会属于同一个国家则继续
				if(pRegion->GetCountry() != 0 && pMaster->GetCountry() == pRegion->GetCountry())
					continue;
			}
		}

		//##如果该对象也在警戒范围内
		if( m_pOwner -> RealDistance(pMonster) <= 
			m_pOwner -> GetAlertRange() )
		{
			//##如果当前目标不存在，就先把这个目标锁定。
			if( pTarget == NULL )
			{
				pTarget = pMonster;
				continue;
			}

			//##如果该玩家比选择的目标的距离要短。
			if( m_pOwner -> RealDistance(pMonster) < 
				m_pOwner -> RealDistance(pTarget) )
			{
				//##如果该玩家的距离比技能的最小攻击距离要大
				if( m_pOwner -> RealDistance(pMonster) >=
					pSkill -> GetAffectRangeMin() )
				{
					pTarget = pMonster;
				}
			}
			//##如果该玩家比选中目标的距离要远.
			else
			{
				//##如果选中的目标在技能的最小作用范围内.
				if( m_pOwner -> RealDistance(pTarget) <
					pSkill -> GetAffectRangeMin() )
				{
					pTarget = pMonster;
				}
			}
		}
	}
	return pTarget;
}
BOOL CVilCouGuardWithSword::OnSearchEnemy()
{
	if( CMonsterAI::OnSearchEnemy() )
	{
		//##如果所有者不存在.直接返回.
		if( m_pOwner == NULL ) 
		{
			return TRUE;
		}
		if( HasTarget() ) 
		{
			//##如果距离不在自己的原来位置...
			if( m_lX != -1 && m_lY != -1 && m_pOwner -> 
				RealDistance( m_lX, m_lY ) > m_pOwner -> GetTrackRange() )
			{
				OnLoseTarget();
			}
			return TRUE;
		}

		CMoveShape* pTarget = SearchEnemyMember();
		CMoveShape* pPet	= SearchEnemyPet();
		
		if( pTarget == NULL )
		{
			pTarget = pPet;
		}
		else
		{
			if( pPet )
			{
				if( m_pOwner -> RealDistance(pPet) < m_pOwner -> RealDistance(pTarget) )
				{
					pTarget = pPet;
				}
			}
		}

		if( pTarget )
		{
			//##满足条件设置对象			
			SetTarget( pTarget -> GetType(), pTarget -> GetID() );
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CVilCouGuardWithSword::OnLoseTarget()
{
	if( CMonsterAI::OnLoseTarget() )
	{
		if( m_pOwner && m_lX != -1 && m_lY != -1 )
		{
			CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner -> GetFather() );
			if( pRegion )
			{
				if( pRegion -> GetBlock(m_lX, m_lY) == CRegion::BLOCK_NO )
				{
					m_pOwner -> OnSetPosition( m_lX, m_lY );
				}
				else
				{
					LONG lX, lY;
					pRegion -> GetRandomPosInRange( lX, lY, m_lX - 1, m_lY - 1, 3, 3 );
					m_pOwner -> OnSetPosition( lX, lY );
				}
			}
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
*/