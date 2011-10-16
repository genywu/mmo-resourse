#include "StdAfx.h"
#include "ModelWolf.h"
#include "..\Player.h"
#include "..\Monster.h"
#include "..\ServerRegion.h"



CModelWolf::CModelWolf()
{
	m_bHasFightFriendAround=FALSE;

}

CModelWolf::~CModelWolf()
{

}

VOID CModelWolf::SetTarget(LONG lType, const CGUID& guid)
{
	
	m_lTargetType	= lType;
	m_TargetGuid		= guid;
	m_dwGuardTimeStamp=0;
	m_StartFolling.x =m_pOwner->GetTileX();
	m_StartFolling.y =m_pOwner->GetTileY();
	if (m_bHasFightFriendAround==TRUE)
	{
		ChangeStateTo(CShape::STATE_FIGHT);
	}
	//StartActiveAct();
}

BOOL CModelWolf::OnPeaceIdle()
{
	CMonster* monster=0;
	int friendnum=GetFriendNumAround(&monster);
	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);

	if (monster!=0)
	{

		m_bHasFightFriendAround=TRUE;
		CBaseAI::MoveTo(monster->GetTileX(),monster->GetTileY());
	}
	else if (HasTarget())
	{
		m_bHasFightFriendAround=FALSE;
		CMoveShape* pTarget=GetTarget();

		if (!pTarget )
		{
			WhenLoseTarget();
			Stand(dynamic_cast<CMonster*>(m_pOwner)->GetStopFrame());
			return FALSE;//###########################################
		}

		if (m_pOwner->Distance(m_StartFolling.x,m_StartFolling.y)>pOwner->GetAIReferrence(2))
		{
			LoseTarget();
			long lDir= GetLineDir(m_pOwner->GetTileX(),m_pOwner->GetTileY(),m_StartFolling.x,m_StartFolling.y);
			MoveByTimeOnPeace(lDir,5000);
		}

		if (friendnum>= pOwner->GetAIReferrence(0)-1)//周围同伴数量多于ai参考1
		{
			ChangeStateTo(CShape::STATE_FIGHT);
		}

		LONG ldistance=pOwner->Distance(pTarget);
		if (ldistance>=pOwner->GetAttribute(string("C_ChaseRange")))
		{
			CBaseAI::MoveTo(pTarget->GetTileX(),pTarget->GetTileY());
			WhenLoseTarget();
		}
		else if (ldistance>pOwner->GetAIReferrence(1))
		{
			CBaseAI::MoveTo(pTarget->GetTileX(),pTarget->GetTileY());
		}
		else if (ldistance==pOwner->GetAIReferrence(1))
		{
			//AddAIEvent(AET_MOVE,STAND_ONCE_TIME);
			//CBaseAI::RandomRun();
			Stand(dynamic_cast<CMonster*>(m_pOwner)->GetStopFrame());
			long dir=GetLineDir(m_pOwner->GetTileX(),m_pOwner->GetTileY(),pTarget->GetTileX(),pTarget->GetTileY());
			if (dir!= pOwner->GetDir())
			{
				m_pOwner->SetDir(dir);
				CMessage Msg( MSG_S2C_SHAPE_CHANGEDIR );
				Msg.Add( static_cast<BYTE>(m_pOwner->GetDir()) );
				Msg.Add( m_pOwner->GetDirEx());
				Msg.Add( m_pOwner->GetType() );
				Msg.Add( m_pOwner->GetExID() );
				Msg.SendToAround( m_pOwner );
			}
		}
		else
		{
			Escape(dynamic_cast<CRegion*>(m_pOwner->GetFather()),pTarget->GetTileX(),pTarget->GetTileY(),FALSE);
		}
	}
	else
	{
		m_bHasFightFriendAround=FALSE;
		RandomRun();
	}
	return FALSE;
}

INT  CModelWolf::GetFriendNumAround(CMonster** FightMonster)
{
	*FightMonster=0;
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner -> GetFather() );
	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	if( pRegion == NULL ) return 0;

	////##如果当前攻击技能不存在.
	//CSkill* pSkill = m_pOwner -> GetCurrentSkill();
	//if( pSkill == NULL ) return NULL;

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( m_pOwner, TYPE_MONSTER, vOut );

	CMonster* pTarget = NULL;
	INT iFriendNum=0;

	for( size_t i = 0; i < vOut.size(); i ++ )
	{
		CMonster* pMonster = dynamic_cast<CMonster*>( vOut[i] );
		if( pMonster == NULL ) 
		{
			continue;
		}
		if(strcmp(pMonster->GetOriginName(),pOwner->GetOriginName())!=0)
		{
			continue;
		}
		if (pMonster->GetNpcType()== NT_Guard)
		{
			continue;
		}
		if (pMonster->GetIndex()!=pOwner->GetIndex())
		{
			continue;
		}

		//##如果该生物也在警戒范围内
		int Distance=m_pOwner -> RealDistance(pMonster);

		if (Distance > pOwner -> GetAttribute(string("C_GuardRange")))
		{
			continue;
		}
		iFriendNum++;
		if (*FightMonster!=NULL)
		{
			int distance_=m_pOwner->Distance(*FightMonster);
			if (
				pMonster->GetState()==CShape::STATE_FIGHT&&
				pMonster->GetAI()->GetCurrentActiveAction()!= CBaseAI::AES_FIGHT_ESCAPE&&
				Distance<m_pOwner->Distance(*FightMonster)
				)
			{
				*FightMonster=pMonster;
			}
		}
		else
		{
			if (
				pMonster->GetState()==CShape::STATE_FIGHT&&
				pMonster->GetAI()->GetCurrentActiveAction()!= CBaseAI::AES_FIGHT_ESCAPE
				)
			{
				*FightMonster=pMonster;
			}
		}
	}
	return iFriendNum;
}


BOOL CModelWolf::WhenBeenHurted(LONG lType,const CGUID& guid,DWORD hurt)
{
	if(!CMonsterAI::WhenBeenHurted (lType,guid,hurt))//如果是强制目标时间
		return FALSE;
	m_bHasFightFriendAround=TRUE;
	return TRUE;
}

void CModelWolf::EnterFight()
{
	m_EnterFightPoing= m_StartFolling;
	ChangeStateTo(CShape::STATE_FIGHT);
}