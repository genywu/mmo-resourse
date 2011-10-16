#include "StdAfx.h"
#include "PlayerAI.h"
#include "MMSystem.h"
#include "..\ServerRegion.h"
#include "../BuffSkill/BaseModule.h"
#include "../../GameServer/GameManager.h"
//#include "..\Skills\SkillFactory.h"
//#include "..\Skills\SkillBaseProperties.h"



CPlayerAI::tagMessage::tagMessage()
{
	mtType=MT_NULL;
	lSkillID=0;
	lAttackX=0;
	lAttackY=0;
	lAttackType=0;
	bIsRun=0;
}

VOID CPlayerAI::tagMessage::Clean()
{
	mtType=MT_NULL;
	lSkillID=0;
	lAttackX=0;
	lAttackY=0;
	lAttackType=0;

	bIsRun=0;
}

//BOOL CPlayerAI::OnLoseTarget()
//{
//	LoseTarget();
//	return true;
//}

BOOL CPlayerAI::Tracing()
{
	return FALSE;
}

BOOL CPlayerAI::CheckMessage(tagMessage mes)						//加入事件，如果当前没有行为则处理。
{


	return FALSE;
}

BOOL CPlayerAI::CheckNextAct()				//处理后备消息
{
	if (m_tNextAct.mtType== tagMessage::MT_NULL)
	{
		return FALSE;
	}

	stModuParam *m_pCurrentSkill=m_pOwner->GetInstanceSkill();
	if (m_pCurrentSkill==NULL)
	{
		return FALSE;
	}
	SetTarget(m_tNextAct.lAttackType,m_tNextAct.AttackPlayerID);

	int x=m_tNextAct.lAttackX;
	int y=m_tNextAct.lAttackY;
	CMoveShape* pTarget= GetTarget();

	m_tNextAct.Clean();

	if(!BeginSkill(m_pCurrentSkill,x,y,pTarget))
		return FALSE;

	return TRUE;

}

BOOL CPlayerAI::RandomRun()					//随机跑动
{
	LONG lDir	= random( 8 );
	return ForceMove(lDir);
	//long lDestX = m_pOwner->GetTileX();
	//long lDestY = m_pOwner->GetTileY();
	//if( GetNextMoveDestCell(lDir,1,lDestX,lDestY) )
	//	MoveTo(lDestX,lDestY);
}
//public

CPlayerAI::CPlayerAI()
{
	checknum=1;
	m_lCount=0;
	m_bFightUseSkill=FALSE;
}

CPlayerAI::~CPlayerAI()
{
	LogoutEvent(m_aeRetPeace);
}

// 注册返回和平事件
void CPlayerAI::AddRetPeaceAct()
{
	if (m_aeRetPeace.lTimerID==-1)
	{
		m_aeRetPeace.aetEventType=AET_RETURNPEACE;

		tagTimerVar* pTimvar = CBaseObject::CreateTimerVar(33);
		pTimvar->TimerType = ChildObjType_AI;
		pTimvar->lvar = AET_RETURNPEACE;
		m_aeRetPeace.dwSpringTime=m_iReturnPeaceTime;
		m_aeRetPeace.lTimerID=GameManager::GetInstance()->Schedule(m_pOwner,(void*)pTimvar,m_iReturnPeaceTime);
	}
	else
	{}
}

void CPlayerAI::LogoutRetPeaceAct()
{
	GameManager::GetInstance()->Cancel(m_aeRetPeace.lTimerID,0);
	m_aeRetPeace.Clean();
}

BOOL CPlayerAI::OnActive()						//主动行为
{
#ifdef _DUMP
	/*	char str[512]="";
	sprintf(str,"计数器:%d,移动标志符:%d",++m_lCount,m_pOwner->GetRandomMove());
	PutStringToFile("State.log",str);*/
#endif
	if (!m_pOwner->GetRandomMove()  )
	{
		RandomRun();
		if (m_csCycleSpring.csSpringType!=CS_NULL)
		{
			CleanCycleSpring();
		}
	}
	else if (!CheckCycleSpring(m_csCycleSpring))//如果周期触发中有数据，则处理周期触发
	{

		CheckNextAct();	//否则处理后备消息
	}

	if (m_tNextAct.mtType!=tagMessage::MT_NULL)
	{
		CMessage msg(MSG_S2C_SKILL_USE);
		msg.Add((CHAR)SKILL_USE_RESULT_FAILED);
		msg.Add( m_pOwner->GetExID() );
		//id和等级
		msg.Add((long)m_tNextAct.lSkillID);
		msg.Add((BYTE)1);
		msg.Add((DWORD)0);
		//错误类型
		msg.Add((BYTE)SKILL_USE_FAILED_INVALID_UNKNOWN);
		msg.SendToPlayer(m_pOwner->GetExID());
		m_tNextAct.Clean();
#ifdef _SKILL_DEBUG
		char str[64]="";
		_snprintf(str, 64, "AI发送技能失败消息:技能ID:%6d\n",m_tNextAct.lSkillID);
		PutStringToFile("SKill.log",str);
#endif
	}

	return TRUE;
}

BOOL CPlayerAI::OnCycle()
{return TRUE;}

BOOL CPlayerAI::OnReturnPeace()
{
	m_aeRetPeace.Clean();
	if (!m_pOwner->IsDied())
	{
		ChangeStateTo(CShape::STATE_PEACE);
	}

	return TRUE;
}

BOOL CPlayerAI::OnSkillEnd(long lAddSub)
{
	m_iReturnPeaceTime=timeGetTime()+CGlobeSetup::GetSetup()->dwFightStateTime;
	m_bFightUseSkill= FALSE;
	LogoutRetPeaceAct();
	AddRetPeaceAct();
#ifdef _DEBUG
	char str[512]="";
	_snprintf(str, 512, "PlayerAI计数器:%d",--m_lCount);
	PutStringToFile("PlayerAI.log",str);
#endif
	if (lAddSub==eAdd_Type && m_pOwner->GetState()==CShape::STATE_PEACE)
	{	
		m_pOwner->ChangeStateTo(CShape::STATE_PEACE);
	}
	return TRUE;
}

VOID CPlayerAI::ChangeStateToFight(BOOL bUseSkill)
{
	if (m_pOwner->IsDied())
	{
		return;
	}
	m_iReturnPeaceTime=timeGetTime()+CGlobeSetup::GetSetup()->dwFightStateTime;
	ChangeStateTo(CShape::STATE_FIGHT);
	LogoutRetPeaceAct();
#ifdef _DEBUG
	char str[512]="";
	_snprintf(str, 512, "PlayerAI计数器:%d",++m_lCount);
	PutStringToFile("PlayerAI.log",str);
#endif
	if (!bUseSkill && m_bFightUseSkill == FALSE)
	{
#ifdef _DEBUG
		_snprintf(str, 512, "PlayerAI计数器:%d",--m_lCount);
		PutStringToFile("PlayerAI.log",str);
#endif
		AddRetPeaceAct();
	}
	else
	{
		m_bFightUseSkill= TRUE;
	}
}

//VOID CPlayerAI::StartSkillAct(LONG lTime)
//{
//	AddAIEvent(AET_ACT,lTime);
//}

VOID CPlayerAI::SetForceTarget( LONG lType, const CGUID& TargetGuid )
{
	m_pOwner->SetForceAttack(FALSE);
	SetTarget(  lType, TargetGuid );

	CMessage msg(MSG_S2C_PLAYER_FORCE_TARGET);
	msg.Add( TargetGuid );//目标guid
	msg.Add( lType );//目标类型
	msg.SendToPlayer( m_pOwner->GetExID() );
}

VOID CPlayerAI::CleanForceTarget()
{
	m_pOwner->SetForceAttack(TRUE);
	SetTarget(  0, NULL_GUID );
	CMessage msg(MSG_S2C_PLAYER_FORCE_TARGET);
	msg.Add( NULL_GUID );//目标guid
	msg.Add( (LONG)0 );//目标类型
	msg.SendToPlayer( m_pOwner->GetExID() );
}

VOID CPlayerAI::SetTarget( LONG lType, const CGUID& TargetGuid )
{
	m_lTargetType	= lType;
	m_TargetGuid	= TargetGuid;
}

//VOID CPlayerAI::Attack( long tID, CMoveShape* pTarget )	//接受攻击某个目标消息
//{
//
//	tagMessage mes;
//	mes.mtType=tagMessage::MT_ATTACKTOTARGET;
//	mes.message.tagAttackToTarget.lSkillID=tID;
//	mes.message.tagAttackToTarget.lPlayerID=pTarget->GetID();
//	mes.message.tagAttackToTarget.lType=pTarget->GetType();
//	AddNextAct(mes);
//}
//
//VOID CPlayerAI::Attack( long tID, LONG lX, LONG lY )		//接受攻击某点的消息
//{
//	tagMessage mes;
//	mes.mtType=tagMessage::MT_ATTACKTOCELL;
//	mes.message.tagAttackToCell.lShillID=tID;
//	mes.message.tagAttackToCell.lX=lX;
//	mes.message.tagAttackToCell.lY=lY;
//	AddNextAct(mes);
//}
VOID CPlayerAI::Attack(long tID, LONG lX/* =0 */, LONG lY/* =0 */, CMoveShape* pTarget/* =NULL  */)
{

	stModuParam *m_pCurrentSkill=m_pOwner->GetInstanceSkill();
	if (m_aeActive.aetEventType==AET_MOVE)
	{

#ifdef _SKILL_DEBUG
		char str[64]="";
		_snprintf(str, 64, "AI发送技能失败消息:技能ID:%6d\n",m_tNextAct.lSkillID);
		PutStringToFile("SKill.log",str);
#endif
		if (m_pCurrentSkill)
		{			
			CNewSkillFactory::RealeaseBaseMoudule(m_pCurrentSkill);
		}

		CMessage msg(MSG_S2C_SKILL_USE);
		msg.Add((CHAR)SKILL_USE_RESULT_FAILED);
		msg.Add( m_pOwner->GetExID() );
		//id和等级
		msg.Add((long)m_tNextAct.lSkillID);
		msg.Add((BYTE)1);
		//全局id
		msg.Add((DWORD)0);
		//错误类型
		msg.Add((BYTE)SKILL_USE_FAILED_INVALID_UNKNOWN);
		msg.SendToPlayer(m_pOwner->GetExID());
		return;
	}

	CGUID AttackPlayerID;
	LONG  lAttackType=0;

	if (m_pOwner->GetForceAttack()== false && HasTarget())
	{
		CMoveShape* pMonster= GetTarget();
		if (pMonster!= NULL)
		{
			pTarget= pMonster;
		}
	}

	if (pTarget)
	{
		AttackPlayerID=pTarget->GetExID();
		lAttackType=pTarget->GetType();
		SetTarget(lAttackType,AttackPlayerID);
	}
	else
	{
		AttackPlayerID=NULL_GUID;
		lAttackType=0;
	}

	
	if (m_pCurrentSkill)
	{
		WORD wSkillType= m_pCurrentSkill->GetAddSub();
		if(!BeginSkill(m_pCurrentSkill,lX,lY,pTarget))
		{
			return;
		}
		else if (wSkillType ==eSub_Type)
		{
			ChangeStateToFight(TRUE);
		}
	}	
}

BOOL CPlayerAI::MoveTo(float fDestX, float fDestY)
{
	//##条件检查
	if( !m_pOwner ) return FALSE;
	if( m_pOwner -> CanMove() == FALSE || m_pOwner->IsInAct()==TRUE)
	{
#ifdef _DEBUG	
		//char str[256];
		//sprintf(str,"\n不能移动:%d\n",timeGetTime());
		//OutputDebugString(str);
#endif
		m_pOwner -> OnCannotMove();
		return FALSE;
	}
#ifdef _DEBUG	
	//char str[256];
	//_snprintf(str, 256, "\n移动动作开始时间:%d\n",timeGetTime());
	//OutputDebugString(str);
#endif
	m_pOwner -> OnMove( fDestX, fDestY );
	return TRUE;
}

BOOL CPlayerAI::ForceMove(long lDir)
{
	long lNewX,lNewY;
	float fDis = 1.0f;
	BOOL bMoveable	= GetNextMoveDestCell(lDir,fDis,lNewX,lNewY);
	if( bMoveable )
	{
		long lPosX= m_pOwner -> GetTileX();
		long lPosY=  m_pOwner -> GetTileY();
		if( GetLineDir(lPosX, lPosY, lNewX, lNewY) % 2 != 0 )
		{
			fDis *=1.414f;
		}
		float	fTime	= fDis/m_pOwner->GetSpeed();
		//if(m_pOwner->GetIsRun())
		//	fTime /= 2.00f;

		//##移动Shape到指定位置。
		m_pOwner->ForceMove((float)lNewX+0.5f,(float)lNewY+0.5f,fTime);
		OnMove((float)lNewX+0.5f,(float)lNewY+0.5f,(DWORD)fTime);
		//设置位置值
		//m_pOwner->SetPosXY(lNewX+0.5f,lNewY+0.5f);		
	}
	else
	{
		AddAIEvent(AET_MOVE,MOVE_FAIL_APACE_TIME);
	}
	//GetLineDirect();ForceMove();
	//CMoveShape::ForceMove();
	return TRUE;
}

//判断是否能移动到某一点
BOOL CPlayerAI::IsCanMoveTo(CRegion* pRegion,LONG lX, LONG lY )
{
	BYTE byBlock = pRegion->GetBlock(lX,lY);
	if(byBlock != CRegion::BLOCK_NO && byBlock != CRegion::BLOCK_AIM)
		return FALSE;
	return TRUE;
}

BOOL CPlayerAI::CanUseSkill()
{
	/*map<long, CBaseModule*> & skillmap=m_pOwner->GetActiveSkill();
	DWORD TimeMax=0;
	for (map<long,CBaseModule*>::iterator it= skillmap.begin(); it!= skillmap.end(); it++)
	{
		CBaseModule* basemodule=(*it).second;
		DWORD time = basemodule->GetStaticParam()->lActTime;
		if (time>TimeMax)
		{
			TimeMax = time;
		}
	}
	if (timeGetTime()> TimeMax)
	{
		return TRUE;
	}*/
	return FALSE;
}

INT CPlayerAI::CanUseSkill(BOOL bRunUse)
{
	if (m_aeActive.aetEventType==AET_MOVE && bRunUse== FALSE)
	{
		return SKILL_USE_FAILED_INVALID_MOVING;
	}
	//if (!CanUseSkill())
	//{
	//	return ERROR_SKILLUSING;
	//}
	//if(m_pOwner->GetState()== CShape::STATE_FIGHT && bUseOnFight == FALSE)
	//{
	//	return ERROR_FIGHTSTATE;
	//}
	return RIGHT;
}
//被伤害的时候
BOOL CPlayerAI::WhenBeenHurted(LONG lType,const CGUID& guid,DWORD a)
{
	if (m_pOwner->IsGod()) return TRUE;
	ChangeStateToFight(FALSE);
	return TRUE;
}

//被杀害的时候
BOOL CPlayerAI::WhenBeenKilled(LONG lType,const CGUID& guid)
{
	ChangeStateTo(CShape::STATE_DIED);
	dynamic_cast<CPlayer*>(m_pOwner)->ActivateAllPets(false);
	LogoutRetPeaceAct();
	dynamic_cast<CPlayer*>(m_pOwner)->OnDied();
	return TRUE;
}