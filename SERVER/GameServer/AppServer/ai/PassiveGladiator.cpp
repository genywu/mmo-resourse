#include "StdAfx.h"
#include "PassiveGladiator.h"
#include "..\Player.h"
#include "..\Monster.h"
#include "..\ServerRegion.h"
#include "AIUtilities.h"



CPassiveGladiator::CPassiveGladiator(void)
{
}

CPassiveGladiator::~CPassiveGladiator(void)
{
}
BOOL CPassiveGladiator::WhenBeenHurted(LONG lType,const CGUID& guid, DWORD hurt)
{
	if( lType == TYPE_PLAYER && m_pOwner != NULL )
	{
		//##首先查找这个玩家是否处于向量内部
		BOOL bAlreadyInList = FALSE;
		for( size_t i = 0; i < m_vEnemy.size(); i ++ )
		{
			if( m_vEnemy[i] == guid )
			{
				bAlreadyInList = TRUE;
				break;
			}
		}
		//##如果不存在。则直接加进去。
		if( bAlreadyInList == FALSE )
		{
			m_vEnemy.push_back( guid );
		}

		//##如果大于最大索敌数，就把这个列表中最先伤害它的人移开。
		if( m_vEnemy.size() > 10 )
		{
			m_vEnemy.erase( m_vEnemy.begin() );
		}
	}

	CMonsterAI::WhenBeenHurted (lType,guid,hurt);//如果是强制目标时间
	
	return TRUE;
}

BOOL CPassiveGladiator::SearchEnemy()
{
	if(!CanSearchEnemy())
		return false;

	if (m_vEnemy.size()==0)
	{
		return TRUE;
	}
	if (HasTarget())
	{
		return TRUE;
	}
	//##如果所有者不存在.直接返回.
	if( m_pOwner == NULL )
	{
		m_vEnemy.clear();
		return TRUE;
	}

	//##如果区域也不存在.直接返回.
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner -> GetFather() );
	if( pRegion == NULL ) 
	{
		m_vEnemy.clear();
		return TRUE;
	}

	//##查找离自己最近的攻击者.
	CPlayer* pNearestEnemy = NULL;
	vector<CGUID>::iterator it = m_vEnemy.begin(); 

	while(it!= m_vEnemy.end())
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>( pRegion ->FindAroundObject(m_pOwner, TYPE_PLAYER, *it) );

		if (pPlayer != NULL && pPlayer->IsDied() == FALSE && m_pOwner -> RealDistance(pPlayer) <= m_pOwner -> GetTrackRange())
		{
			if ( !pPlayer->IsVisible(m_pOwner) )			// 隐形
			{
				it= m_vEnemy.erase(it);
				continue;
			}

			if(  pPlayer->GetExID() == m_pOwner->GetBefoolID() )
			{
				it= m_vEnemy .erase(it);
				continue;
			}

			if( AIUtilities::IsInGodState( pPlayer ) )
			{
				it = m_vEnemy.erase( it );
				continue;
			}

			if (pNearestEnemy== NULL || m_pOwner -> RealDistance(pPlayer) < m_pOwner -> RealDistance(pNearestEnemy))
			{
				pNearestEnemy = pPlayer;
			}
			it++;
		}
		else
		{
			it= m_vEnemy.erase(it);
		}
			
	}

	//##如果找到了攻击者.
	if( pNearestEnemy )
	{
		//##满足距离条件设置对象
		SetTarget( pNearestEnemy -> GetType(), pNearestEnemy -> GetExID() );
	}
	return TRUE;


}
VOID CPassiveGladiator::Clear()
{
	CBaseAI::Clear();
	m_vEnemy.clear();
}

VOID CPassiveGladiator::ResumeAll( bool bForce )
{
	CMonsterAI::ResumeAll( bForce );
	m_vEnemy.clear();
}
VOID CPassiveGladiator::SetTarget( LONG lType, const CGUID& TargetGuid )
{
	CBaseAI::SetTarget(lType,TargetGuid);
}
