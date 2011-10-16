#include "StdAfx.h"
#include "FixedPositionArcher.h"
#include "..\Player.h"
#include "..\Monster.h"
#include "..\ServerRegion.h"
#include "..\BuffSkill\BuffSkillDef.h"
#include "..\BuffSkill\BaseModule.h"



CFixedPositionArcher::CFixedPositionArcher()
{
}

CFixedPositionArcher::~CFixedPositionArcher()
{
}

INT	CFixedPositionArcher::Tracing(CMoveShape*pTarget)
{
	SearchSkill(pTarget);

	CBaseModule* basemodule=0;
	stStaticModuParam* moduparam=0;
	BOOL  HasSkill=TRUE;
	INT MinAtkDistance=0;
	INT MaxAtkDistance=0;

	if (HasChoiceSkill())
	{
		basemodule=dynamic_cast<CMonster*>(m_pOwner)->GetActiveSkill(0);//GetSearchedSkillKey()
	}
	else
	{
		Stand(dynamic_cast<CMonster*>(m_pOwner)->GetStopFrame());
		WhenLoseTarget();
		return TRACE_RESULT_TRACE;
	}




	if (basemodule!=NULL)
	{
		moduparam=basemodule->GetStaticParam();
		MinAtkDistance=moduparam->lMinAtkDistance;
		MaxAtkDistance=moduparam->lMaxAtkDistance;
	}
	else
	{
		Stand(dynamic_cast<CMonster*>(m_pOwner)->GetStopFrame());
		WhenLoseTarget();
		return TRACE_RESULT_TRACE;
	}

	//##合乎距离要求。
	LONG lDistance = m_pOwner -> RealDistance( pTarget );

	//如果在技能的距离内， 则使用技能
	if(lDistance >= MinAtkDistance && lDistance <= MaxAtkDistance )
	{
		if (HasSkill)
		{
			return TRACE_RESULT_SUCCESS;
		}
		else
		{
			Stand(dynamic_cast<CMonster*>(m_pOwner)->GetStopFrame());
			WhenLoseTarget();
			return TRACE_RESULT_TRACE;
		}

	}
	//##如果依然在追击范围内，就开始跟随玩家。
	else
	{
		Stand(dynamic_cast<CMonster*>(m_pOwner)->GetStopFrame());
		return TRACE_RESULT_TRACE;

	}
}

BOOL CFixedPositionArcher::RandomRun()
{
	Stand(1000);
	return FALSE;
}

BOOL CFixedPositionArcher::SearchEnemy()
{
	if(!CanSearchEnemy())
		return false;

	if(m_pOwner==NULL)
		return false;

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner -> GetFather() );//寻找所在场景
	if( pRegion == NULL ) return TRUE;

	vector<CPlayer*> vOut;
	pRegion -> FindAroundPlayer( m_pOwner, vOut );

	//vector<CMonster*> vPets;
	//pRegion -> FindAroundPets( m_pOwner, vPets );

	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	CMoveShape* pTarget = NULL;//GetTarget();

	for( size_t i = 0; i < vOut.size(); i ++ )
	{
		CPlayer* pPlayer = vOut[i];
		if( pPlayer == NULL ) continue;
		if( pPlayer -> IsDied() ) continue;

		if ( !pPlayer->IsVisible(pOwner) )			// 隐形
		{
			continue;
		}

		int iAimDistance=m_pOwner -> Distance(pPlayer);
		if( pTarget )
		{
			if(iAimDistance <=pOwner->GetAckRangeMin() &&
				iAimDistance>= pOwner->GetAckRangeMax()&&
				pPlayer->GetExID()!= m_pOwner->GetBefoolID())
			{
				pTarget = pPlayer;
			}
		}
		else
		{
			if( iAimDistance >= pOwner -> GetAckRangeMin() &&
				iAimDistance <= pOwner -> GetAckRangeMax() &&
				pPlayer->GetExID()!= m_pOwner->GetBefoolID())
			{
				pTarget = pPlayer;
			}
		}
	}

	if( pTarget )
	{
		//##满足条件设置对象
		SetTarget( pTarget -> GetType(), pTarget -> GetExID() );
	}
	else
	{
		WhenLoseTarget();
	}

	return FALSE;
}
/*
CBaseAI::AI_EXEC_STATE CFixedPositionArcher::Run()
{
	return CMonsterAI::Run();
}

BOOL CFixedPositionArcher::OnChangeSkill()
{
	CBaseAI::OnChangeSkill();
	if( m_pOwner )
	{
		SelectAttackSkill();
		CSkill* pSkill = m_pOwner -> GetCurrentSkill();
		//##如果当前技能不存在,或者当前技能没有冷却完毕.
		if( pSkill == NULL || pSkill -> IsRestored() == FALSE )
		{
			if( pSkill == NULL )
			{
				m_pOwner -> SetCurrentSkill( m_pOwner -> GetDefaultAttackSkillID() );
			}
			else 
			{
				AddAIEvent( ASA_STAND, pSkill -> GetRestoreTime() );
			}
		}
	}
	return TRUE;
}

VOID CFixedPositionArcher::OnSchedule()
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


VOID CFixedPositionArcher::OnIdle()
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

VOID CFixedPositionArcher::WhenBeenHurted( LONG lType, LONG lID, DWORD dwStiffenTime )
{
	CBaseAI::WhenBeenHurted( lType, lID, dwStiffenTime );
	if( HasTarget() == FALSE )
	{
		SetTarget( lType, lID );
	}
}

//##在每次需要移动的时候，搜寻最近的Player
BOOL CFixedPositionArcher::OnMoving()
{
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

BOOL CFixedPositionArcher::OnFighting()
{
	if( CMonsterAI::OnFighting() )
	{
		AddAIEvent( ASA_SEARCH_ENEMY );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CFixedPositionArcher::OnSearchEnemy()
{
	if( CMonsterAI::OnSearchEnemy() )
	{
		//##如果所有者不存在.直接返回.
		if( m_pOwner == NULL ) return TRUE;

		CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner -> GetFather() );
		if( pRegion == NULL ) return TRUE;
		
		//##如果当前攻击技能不存在.
		CSkill* pSkill = m_pOwner -> GetCurrentSkill();
		if( pSkill == NULL ) return TRUE;

		vector<CPlayer*> vOut;
		pRegion -> FindAroundPlayer( m_pOwner, vOut );

		vector<CMonster*> vPets;
		pRegion -> FindAroundPets( m_pOwner, vPets );

		//vector<CMonster*> vCars;
		//pRegion->FindAroundCarriages( m_pOwner, vCars );

		CMoveShape* pTarget = NULL;
		for( size_t i = 0; i < vOut.size(); i ++ )
		{
			CPlayer* pPlayer = vOut[i];
			if( pPlayer == NULL ) continue;
			if( pPlayer -> IsDied() ) continue;

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

		for( size_t i = 0; i < vPets.size(); i ++ )
		{
			CMonster* pPet = vPets[i];
			if( pPet == NULL ) continue;
			if( pPet -> IsDied() ) continue;

			//##如果该宠物也在警戒范围内
			if( m_pOwner -> RealDistance(pPet) <= 
				m_pOwner -> GetAlertRange() )
			{
				//##如果当前目标不存在，就先把这个目标锁定。
				if( pTarget == NULL )
				{
					pTarget = pPet;
					continue;
				}

				//##如果该玩家比选择的目标的距离要短。
				if( m_pOwner -> RealDistance(pPet) < 
					m_pOwner -> RealDistance(pTarget) )
				{
					//##如果该玩家的距离比技能的最小攻击距离要大
					if( m_pOwner -> RealDistance(pPet) >=
						pSkill -> GetAffectRangeMin() )
					{
						pTarget = pPet;
					}
				}
				//##如果该玩家比选中目标的距离要远.
				else
				{
					//##如果选中的目标在技能的最小作用范围内.
					if( m_pOwner -> RealDistance(pTarget) <
						pSkill -> GetAffectRangeMin() )
					{
						pTarget = pPet;
					}
				}
			}
		}
		//for( size_t i = 0; i < vCars.size(); i ++ )
		//{
		//	CMonster* pCar = vCars[i];
		//	if( pCar == NULL ) continue;
		//	if( pCar -> IsDied() ) continue;

		//	//##如果该宠物也在警戒范围内
		//	if( m_pOwner -> RealDistance(pCar) <= 
		//		m_pOwner -> GetAlertRange() )
		//	{
		//		//##如果当前目标不存在，就先把这个目标锁定。
		//		if( pTarget == NULL )
		//		{
		//			pTarget = pCar;
		//			continue;
		//		}

		//		//##如果该玩家比选择的目标的距离要短。
		//		if( m_pOwner -> RealDistance(pCar) < 
		//			m_pOwner -> RealDistance(pTarget) )
		//		{
		//			//##如果该玩家的距离比技能的最小攻击距离要大
		//			if( m_pOwner -> RealDistance(pCar) >=
		//				pSkill -> GetAffectRangeMin() )
		//			{
		//				pTarget = pCar;
		//			}
		//		}
		//		//##如果该玩家比选中目标的距离要远.
		//		else
		//		{
		//			//##如果选中的目标在技能的最小作用范围内.
		//			if( m_pOwner -> RealDistance(pTarget) <
		//				pSkill -> GetAffectRangeMin() )
		//			{
		//				pTarget = pCar;
		//			}
		//		}
		//	}
		//}
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