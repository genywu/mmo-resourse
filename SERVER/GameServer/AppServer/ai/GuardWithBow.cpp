#include "StdAfx.h"
#include "GuardWithBow.h"
#include "..\Player.h"
#include "..\Monster.h"



CGuardWithBow::CGuardWithBow()
{
}

CGuardWithBow::~CGuardWithBow()
{
}
/*
CBaseAI::AI_EXEC_STATE CGuardWithBow::Run()
{
	return CMonsterAI::Run();
}

VOID CGuardWithBow::OnSchedule()
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


VOID CGuardWithBow::OnIdle()
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
		//AddAIEvent( ASA_STAND, pOwner -> GetBaseProperty() -> dwAIInterval );
		//##之后,添加一个索敌的状态
		AddAIEvent( ASA_SEARCH_ENEMY );
	}
}

VOID CGuardWithBow::WhenBeenHurted( LONG lType, LONG lID, DWORD dwStiffenTime )
{
	CBaseAI::WhenBeenHurted( lType, lID, dwStiffenTime );
	if( HasTarget() == FALSE )
	{
		CMoveShape* pShape = SearchCriminal();
		if( pShape == NULL )
		{
			pShape = SearchMonster();
		}
		if( pShape )
		{
			SetTarget( pShape -> GetType(), pShape -> GetID() );
		}
	}
}

//##在每次需要移动的时候，搜寻最近的Player
BOOL CGuardWithBow::OnMoving()
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

BOOL CGuardWithBow::OnFighting()
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

CMoveShape* CGuardWithBow::SearchCriminal()
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
		if( pPlayer -> IsBadman() == FALSE ||
			pPlayer -> IsAttackAble(m_pOwner) == false )
		{
			continue;
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

CMoveShape* CGuardWithBow::SearchMonster()
{
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner -> GetFather() );
	if( pRegion == NULL ) return NULL;
	
	//##如果当前攻击技能不存在.
	CSkill* pSkill = m_pOwner -> GetCurrentSkill();
	if( pSkill == NULL ) return NULL;

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( m_pOwner, TYPE_MONSTER, vOut );

	CMonster* pTarget = NULL;
	for( size_t i = 0; i < vOut.size(); i ++ )
	{
		CMonster* pMonster = dynamic_cast<CMonster*>( vOut[i] );
		if( pMonster == NULL ) 
		{
			continue;
		}
		if( pMonster -> GetMonsterKind() == 
			CMonsterList::MK_GUARD ||
			pMonster -> IsAttackAble(m_pOwner) == false ) 
		{
			continue;
		}

		//##如果该玩家也在警戒范围内
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

BOOL CGuardWithBow::OnSearchEnemy()
{
	if( CMonsterAI::OnSearchEnemy() )
	{
		//##如果所有者不存在.直接返回.
		if( m_pOwner == NULL ) return TRUE;

		//##首先清除当前目标
		OnLoseTarget();

		//##时间判断.暂时不做时间判断.

		CMoveShape* pTarget = SearchCriminal();
		if( pTarget == NULL )
		{
			pTarget = SearchMonster();
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