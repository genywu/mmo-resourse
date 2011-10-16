#include "StdAfx.h"
#include "CityGuardWithBow.h"
#include "..\Player.h"
#include "..\Monster.h"



CCityGuardWithBow::CCityGuardWithBow()
{
}

CCityGuardWithBow::~CCityGuardWithBow()
{
}
/*
CBaseAI::AI_EXEC_STATE CCityGuardWithBow::Run()
{
	return CMonsterAI::Run();
}

VOID CCityGuardWithBow::OnSchedule()
{
	CBaseAI::OnSchedule();

	//##判断是否开始执行的条件
	if( m_pOwner == NULL || m_pOwner -> IsDied() ) return;

	if( HasTarget() == TRUE &&
		m_qActiveAction.size() == 0 &&
		m_qPassiveAction.size() == 0 )
	{
		if( m_pOwner -> IsFightable() == FALSE )
		{
			OnLoseTarget();
			return;
		}

		CMoveShape* pShape = GetTarget();
		if( pShape == NULL || pShape -> IsDied() )
		{
			OnLoseTarget();
			return;
		}

		//##第一次进入时会首先选择技能，之后，每次Attack事件完成时，
		//##都会添加一个ChangeSkill事件，并随机选取当前的技能。
		CSkill* pSkill = m_pOwner -> GetCurrentSkill();
		if( pSkill == NULL ) 
		{
			OnChangeSkill();
			pSkill = m_pOwner -> GetCurrentSkill();
			if( pSkill == NULL )
			{
				OnLoseTarget();
				return;
			}
		}

		//##判断敌人是否已经进入攻击范围.
		//##合乎距离要求。
		LONG lDistance = m_pOwner -> RealDistance( pShape );
		if( lDistance >= pSkill -> GetAffectRangeMin() &&
			lDistance <= pSkill -> GetAffectRangeMax() )
		{
			if( pSkill -> Begin(m_pOwner, pShape) )
			{
				//##如果攻击成功了，就添加一个攻击事件。
				AddAIEvent( ASA_ATTACK );
			}
			else
			{
				//##因为某些情况，导致攻击失败。最有可能是距离不够等。
				//##添加一个索敌事件。这儿要注意的是：可能攻击距离大于警戒范围。
				//##导致使用技能失败。
				OnLoseTarget();
				AddAIEvent( ASA_SEARCH_ENEMY );
			}
		}
		else
		{
			OnLoseTarget();
			AddAIEvent( ASA_SEARCH_ENEMY );
		}
	}
}


VOID CCityGuardWithBow::OnIdle()
{
	//##如果当前的Area内没有玩家。
	if( m_pOwner -> GetArea() -> GetAllPlayers().size() == 0 )
	{
		CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner -> GetFather() );
		LONG lX = m_pOwner -> GetArea() -> GetX();
		LONG lY = m_pOwner -> GetArea() -> GetY();

		BOOL bNoPlayerAround = TRUE;
		for( LONG i = 0; i < 9; i ++ )
		{
			CArea* pArea = pRegion -> GetArea( lX + _area[i][0], lY + _area[i][1] );
			if( pArea )
			{
				if( pArea -> GetAllPlayers().size() > 0 )
				{
					bNoPlayerAround = FALSE;
					break;
				}
			}
		}
		//##如果没有玩家，进入休眠状态。
		if( bNoPlayerAround )
		{
			Hibernate();
			return;
		}
	}	

	CMonster* pOwner = dynamic_cast<CMonster*>( GetOwner() );
	if( pOwner && pOwner -> IsDied() == FALSE )
	{
		//##首先添加一个选择技能的事件
		AddAIEvent( ASA_CHANGE_SKILL );
		//##添加一个站立状态
//		AddAIEvent( ASA_STAND, pOwner -> GetBaseProperty() -> dwAIInterval );
		//##之后,添加一个索敌的状态
		AddAIEvent( ASA_SEARCH_ENEMY );
	}
}

VOID CCityGuardWithBow::WhenBeenHurted( LONG lType, LONG lID, DWORD dwStiffenTime )
{
	CBaseAI::WhenBeenHurted( lType, lID, dwStiffenTime );
	if( HasTarget() == FALSE )
	{
		CMoveShape* pTarget = SearchEnemyGuildMember();
		CMoveShape* pPet	= SearchEnemyGuildPet();
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
			SetTarget( pTarget -> GetType(), pTarget -> GetID() );
		}
	}
}

//##在每次需要移动的时候，搜寻最近的Player
BOOL CCityGuardWithBow::OnMoving()
{
	if( m_pOwner == NULL ) return TRUE;

	if( CMonsterAI::OnMoving() )
	{
		if( m_pOwner -> IsDied() == FALSE )
		{
			AddAIEvent( ASA_SEARCH_ENEMY );
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CCityGuardWithBow::OnFighting()
{
	if( m_pOwner == NULL ) return TRUE;

	if( CMonsterAI::OnFighting() )
	{
		if( m_pOwner -> IsDied() == FALSE )
		{
			AddAIEvent( ASA_SEARCH_ENEMY );
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

CMoveShape* CCityGuardWithBow::SearchEnemyGuildMember()
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

		//##这儿添加代码，如果Player是这个工会/联盟的，则跳过。
		if( pPlayer->GetFactionID() && pPlayer->GetFactionID() == pRegion->GetOwnedCityFaction() )
		{
			continue;
		}
		else
		{
			if( pPlayer->GetUnionID() && pPlayer->GetUnionID() == pRegion->GetOwnedCityUnion() )
			{
				continue;
			}
		}

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

CMoveShape* CCityGuardWithBow::SearchEnemyGuildPet()
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

		//##这儿添加代码，如果主人是这个工会/联盟的，则跳过。
		if( pMonster -> GetMasterInfo().lMasterType == TYPE_PLAYER )
		{
			CPlayer* pMaster = GetGame() -> FindPlayer( pMonster -> GetMasterInfo().lMasterID );
			if( pMaster )
			{
				//##这儿添加代码，如果主人是这个工会/联盟的，则跳过。
				if( pMaster->GetFactionID() && pMaster->GetFactionID() == pRegion->GetOwnedCityFaction() )
				{
					continue;
				}
				else
				{
					if( pMaster->GetUnionID() && pMaster->GetUnionID() == pRegion->GetOwnedCityUnion() )
					{
						continue;
					}
				}
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
CMoveShape* CCityGuardWithBow::SearchEnemyGuildCarriage()
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

		//##这儿添加代码，如果主人是这个工会/联盟的，则跳过。
		if( pMonster -> GetMasterInfo().lMasterType == TYPE_PLAYER )
		{
			CPlayer* pMaster = GetGame() -> FindPlayer( pMonster -> GetMasterInfo().lMasterID );
			if( pMaster )
			{
				//##这儿添加代码，如果主人是这个工会/联盟的，则跳过。
				if( pMaster->GetFactionID() && pMaster->GetFactionID() == pRegion->GetOwnedCityFaction() )
				{
					continue;
				}
				else
				{
					if( pMaster->GetUnionID() && pMaster->GetUnionID() == pRegion->GetOwnedCityUnion() )
					{
						continue;
					}
				}
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
BOOL CCityGuardWithBow::OnSearchEnemy()
{
	if( CMonsterAI::OnSearchEnemy() )
	{
		//##如果所有者不存在.直接返回.
		if( m_pOwner == NULL ) return TRUE;

		//##首先清除当前目标
		OnLoseTarget();

		//##时间判断.暂时不做时间判断.
		CMoveShape* pTarget = SearchEnemyGuildMember();
		CMoveShape* pPet	= SearchEnemyGuildPet();
		///CMoveShape* pCar	= SearchEnemyGuildCarriage();
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
			//else if( pCar )
			//{
			//	if( m_pOwner -> RealDistance(pCar) < m_pOwner -> RealDistance(pTarget) )
			//	{
			//		pTarget = pCar;
			//	}
			//}
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
*/