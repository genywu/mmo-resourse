 #include "StdAfx.h"
#include "BaseAI.h"
#include "..\Monster.h"
#include "..\MoveShape.h"
#include "../../GameServer/GameManager.h"
#include "AIFactory.h"
#include <MMSystem.h>
#include "..\ServerRegion.h"
#include "../BuffSkill/BaseModule.h"
#include "../BuffSkill/NewSkillFactory.h"
#include "..\Player.h"
#include "ScriptAI.h"




//--------------------------------------------------------------------------------------
// 函数:	CBaseAI构造函数
// 参数:	无
// 返回值:	无
// 描述：	无
//--------------------------------------------------------------------------------------

CBaseAI::AI_EVENT ::AI_EVENT()
{
	aetEventType=AET_NULL;      //超时事件类型
	lTimerID=-1;				//时间id
	dwSpringTime=0;				//触发时间
}

VOID CBaseAI::AI_EVENT ::Clean()
{
	aetEventType=AET_NULL;      //超时事件类型
	lTimerID=-1;				//时间id
	dwSpringTime=0;				//触发时间
}
VOID CBaseAI::AI_EVENT ::operator=(CBaseAI::AI_EVENT& ae)
{
	aetEventType=ae.aetEventType;	    //超时事件类型
	lTimerID=ae.lTimerID;				//时间id
	dwSpringTime=ae.dwSpringTime;		//触发时间
}
CBaseAI::SCycleSpringEvent::SCycleSpringEvent()
{
	csSpringType=CS_NULL;
	unActType.tgMoveToCell.lX=0;
	unActType.tgMoveToCell.lY=0;
}
VOID CBaseAI::SCycleSpringEvent::Clean()
{
	csSpringType=CS_NULL;
	memset(&unActType,0,sizeof(unActType));
	gAimGUID= NULL_GUID;
}
CBaseAI::CBaseAI()
{
	m_lAIType=0;					//##AI类型编号
	m_dwLastExecTimeStamp=0;		//##AI最近一次执行的时间

	//m_TargetGuid=0;				//##攻击对象ID
	m_lTargetType=0;				//##攻击对象类型

	m_lTargetX=0;					//##目标X
	m_lTargetY=0;					//##目标Y

	m_pOwner=NULL;					//##AI主人 怪物 OR 玩家

	m_dwDormancyTime=0;			// 休眠开始时间
	m_dwDormancyInterval=0;		// 休眠时间间隔


	m_aesAIState=AES_FIGHT_NORMAL;               //AI状态

}

//CBaseAI::CBaseAI(CBaseAI& AI)
//{
//	//m_lAIType=0;					//##AI类型编号
//	m_dwLastExecTimeStamp=0;		//##AI最近一次执行的时间
//
//	m_TargetGuid=AI.m_TargetGuid;				//##攻击对象ID
//	m_lTargetType=AI.m_lTargetType;				//##攻击对象类型
//
//	m_lTargetX=AI.m_lTargetX;					//##目标X
//	m_lTargetY=AI.m_lTargetY;					//##目标Y
//	m_pOwner=AI.m_pOwner;					//##AI主人 怪物 OR 玩家
//
//	m_dwDormancyTime=0;			// 休眠开始时间
//	m_dwDormancyInterval=0;		// 休眠时间间隔
//
//	m_aesAIState=AES_FIGHT_NORMAL;               //AI状态
//}
//VOID CBaseAI::operator=(CBaseAI& AI)
//{
//
//	m_dwLastExecTimeStamp=AI.m_dwLastExecTimeStamp;		//##AI最近一次执行的时间
//
//	m_TargetGuid=AI.m_TargetGuid;				//##攻击对象ID
//	m_lTargetType=AI.m_lTargetType;				//##攻击对象类型
//
//	m_lTargetX=AI.m_lTargetX;					//##目标X
//	m_lTargetY=AI.m_lTargetY;					//##目标Y
//
//	m_pOwner=AI.m_pOwner;					//##AI主人 怪物 OR 玩家
//
//
//	//BOOL						m_bIsDormancy;				//##是否是休眠状态。
//	// 休眠时间，下次醒来的时候通过当前时间减去休眠时间，就可以求出休眠了多长时间。
//	// 通过这个时间，乘上回血比例，就可以在醒来的时候回复HP了。
//	m_dwDormancyTime=AI.m_dwDormancyTime;			// 休眠开始时间
//	m_dwDormancyInterval=AI.m_dwDormancyInterval;		// 休眠时间间隔
//
////	m_fGBHStamp=AI.m_fGBHStamp;				//重伤标识
////	m_dwGuardTimeStamp=AI.m_dwGuardTimeStamp;			//警戒时间标识
////	m_dwHurtedTimeStamp=AI.m_dwHurtedTimeStamp;
////	m_dwSkillSpaceTimeStamp=AI.m_dwSkillSpaceTimeStamp;	//技能间隔时间标志
//	m_aeActive=AI.m_aeActive;					//主动行为记录
//	 m_aeCycle=AI.m_aeCycle;					//周期行为记录
//	m_aesAIState=AI.m_aesAIState;               //AI状态
////	m_csCycleSpring=AI.m_csCycleSpring;			//周期触发事件 
//	m_strIdleActScript=AI.m_strIdleActScript;			//空闲行为脚本
//	//m_pCurrentSkill=AI.m_pCurrentSkill;			//当前技能
//	//路径相关
//}

CBaseAI::~CBaseAI()
{
	Reset();
}

//事件驱动的调用接口
BOOL CBaseAI::Run(DWORD dwTimerID,int iOutTimeType)
{
#ifdef _RUNSTACKINFO1_
	char pszStatckInfo[256]="";
	_snprintf(pszStatckInfo,256,"CBaseAI::Run(TimerID:%d,OutTimeType:%d) Start",dwTimerID,iOutTimeType);
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
	bool bRet = true;
	if(iOutTimeType==AET_CYCLE)
	{
		//DeleteAIEvent((AI_EVENT_TYPE)iOutTimeType);
		OnCycle();
	}
	else if(iOutTimeType==AET_ACT || iOutTimeType==AET_MOVE )
	{
		DeleteAIEvent((AI_EVENT_TYPE)iOutTimeType);
		OnActive();
	}
	else if (iOutTimeType==AET_DIED)
	{
		DeleteAIEvent((AI_EVENT_TYPE)iOutTimeType);
		OnDeadState();
	}
	//else if (iOutTimeType==AET_RELIVE)
	//{
	//	DeleteAIEvent((AI_EVENT_TYPE)iOutTimeType);
	//	OnReLive();
	//}
	else if (iOutTimeType==AET_RETURNPEACE)
	{
		OnReturnPeace();
	}
	else if(iOutTimeType==AET_CHANGEAI)
	{
		OnChangeAI();
	}
	else if( iOutTimeType == AET_SKILL_CONTINUE && CAIFactory::IsScriptAI( GetAIType() ) )
	{
		CScriptAI *pScriptAI = static_cast<CScriptAI*>( this );
		pScriptAI->SkillContinue();
	}
		bRet = false;

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CBaseAI::Run() End");
#endif
	return bRet;

}
//添加一个ai事件,
BOOL CBaseAI::AddAIEvent(AI_EVENT_TYPE iOutTimeType,DWORD dwOutTime )
{
	if( iOutTimeType== AET_CYCLE)
	{
		if(m_aeCycle.aetEventType==AET_NULL)
		{
			m_aeCycle.aetEventType=AET_CYCLE;
			//在这里添加注册信息，注册一个事件
			tagTimerVar* pTimvar = CBaseObject::CreateTimerVar(7);
			pTimvar->TimerType = ChildObjType_AI;
			pTimvar->lvar = iOutTimeType;
			m_aeCycle.lTimerID=GameManager::GetInstance()->Schedule(m_pOwner,(void*)pTimvar,timeGetTime()+dwOutTime,dwOutTime);
			m_aeCycle.dwSpringTime=dwOutTime;
		}
		else 
			return false;
	}
	else if(iOutTimeType== AET_MOVE  )
	{
		if(m_aeActive.aetEventType==AET_NULL)
		{
			m_pOwner->SetActTime(timeGetTime(),dwOutTime);
			m_aeActive.aetEventType=(AI_EVENT_TYPE)iOutTimeType;
			//在这里添加注册信息，注册一个事件
			tagTimerVar* pTimvar = CBaseObject::CreateTimerVar(8);
			pTimvar->TimerType = ChildObjType_AI;
			pTimvar->lvar = iOutTimeType;
			m_aeActive.lTimerID=GameManager::GetInstance()->Schedule(m_pOwner,(void*)pTimvar,timeGetTime()+dwOutTime,0);
			m_aeActive.dwSpringTime=dwOutTime;
		}
		else 
			return false;
	}
	else if( iOutTimeType==AET_DIED )//|| iOutTimeType== AET_RELIVE 
	{
		if(m_aeActive.aetEventType==AET_NULL)
		{
			m_aeActive.aetEventType=(AI_EVENT_TYPE)iOutTimeType;
			//在这里添加注册信息，注册一个事件
			tagTimerVar* pTimvar = CBaseObject::CreateTimerVar(9);
			pTimvar->TimerType = ChildObjType_AI;
			pTimvar->lvar = iOutTimeType;
			m_aeActive.lTimerID=GameManager::GetInstance()->Schedule(m_pOwner,(void*)pTimvar,timeGetTime()+dwOutTime,0);
			m_aeActive.dwSpringTime=dwOutTime;
		}
		else 
			return false;
	}
	else if (iOutTimeType== AET_ACT)
	{
		if (m_aeActive.aetEventType==AET_NULL)
		{
			m_aeActive.aetEventType=AET_ACT;
		}
		else
			return false;
	}
	else if( iOutTimeType== AET_CHANGEAI )
	{
		//在这里添加注册信息，注册一个事件
		tagTimerVar* pTimvar = CBaseObject::CreateTimerVar(10);
		pTimvar->TimerType = ChildObjType_AI;
		pTimvar->lvar = iOutTimeType;
		GameManager::GetInstance()->Schedule(m_pOwner,(void*)pTimvar,timeGetTime()+dwOutTime,0);
	}
	else if( iOutTimeType == AET_SKILL_CONTINUE )
	{
		tagTimerVar *pTimvar = CBaseObject::CreateTimerVar(11);
		pTimvar->TimerType = ChildObjType_AI;
		pTimvar->lvar = iOutTimeType;
		m_aeScriptAISkill.aetEventType = (AI_EVENT_TYPE)iOutTimeType;
		m_aeScriptAISkill.lTimerID = GameManager::GetInstance()->Schedule( m_pOwner, pTimvar, timeGetTime() + dwOutTime, 0 );
	}
	else 
		return false;
	return true;

}

//删除一个AI事件
BOOL CBaseAI::DeleteAIEvent(AI_EVENT_TYPE iOutTimeType)
{
	if(iOutTimeType== AET_CYCLE)
	{
		m_aeCycle.Clean();
	}
	else if( iOutTimeType== AET_ACT || iOutTimeType ==AET_MOVE || iOutTimeType== AET_DIED)//|| iOutTimeType== AET_RELIVE 
	{
		
	m_aeActive.Clean();
	}
	else
		return false;
	return true;;
}

//在定时器上注销一个事件
BOOL CBaseAI::LogoutEvent(AI_EVENT& aeAiEvent)
{
	if(aeAiEvent.aetEventType==AET_CYCLE)
	{
		//注销事件
		GameManager::GetInstance()->Cancel(aeAiEvent.lTimerID,0);
		m_aeCycle.Clean();
		return true;
	}

	else if(aeAiEvent.aetEventType==AET_MOVE  || aeAiEvent.aetEventType ==AET_RETURNPEACE || aeAiEvent.aetEventType==AET_ACT)//|| aeAiEvent.aetEventType == AET_RELIVE
	{
		//注销事件
		GameManager::GetInstance()->Cancel(aeAiEvent.lTimerID,0);
		m_aeActive.Clean();
		return true;
	}
	else if( aeAiEvent.aetEventType == AET_SKILL_CONTINUE )
	{
		GameManager::GetInstance()->Cancel( aeAiEvent.lTimerID, 0 );
		m_aeScriptAISkill.Clean();
	}
	//else if (aeAiEvent.aetEventType==AET_ACT)
	//{
	//	//技能相关
	//	//EndCurrentSkill();
	//	m_aeActive.Clean();
	//}
	else
		return false;
	return TRUE;
}

VOID CBaseAI::OnBoring()
{
	StartActiveAct();
	BornSpring();
}


//周期调用函数
BOOL CBaseAI::OnCycle()
{

	return TRUE;
}

BOOL CBaseAI::OnChangeAI ()
{
	return TRUE;
}


//主动行为响应
BOOL CBaseAI::OnActive()
{
	return FALSE;
}

BOOL CBaseAI::OnReturnPeace()
{
	return ReturnPeace();
}

//主动行为处理的不同状态。分为  和平，战斗，逃跑，死亡，挂起
BOOL CBaseAI::OnPeaceState()
{
	return true;
}
BOOL CBaseAI::OnFightState()
{
	return true;
}
BOOL CBaseAI::OnEscapeState()
{
	return true;
}
BOOL CBaseAI::OnNoHpState()
{
	return true;
}
BOOL CBaseAI::OnDeadState()
{
	return true;
}

BOOL CBaseAI::OnTransitionState()
{
	return true;
}

BOOL CBaseAI::OnPeaceIdle()
{
	return true;
}

//（重新）寻找敌人
//BOOL CBaseAI::SearchEnemy()
//{
//	 if(m_pOwner && m_pOwner->GetForceAttack())
//	 {
//		 if (dynamic_cast<CMonster*>(m_pOwner)->GetSearchEnemyScript()!="0"
//			 && dynamic_cast<CMonster*>(m_pOwner)->GetSearchEnemyScript()!="null")
//		 {
//			 SearchEnemyByScript();
//			 return FALSE;
//		 }
//		 return TRUE;
//	 }
//	 return FALSE;
//}

//通过脚本寻找敌人
//BOOL CBaseAI::SearchEnemyByScript()
//{
//	return FALSE;
//}

//寻找技能
BOOL CBaseAI::SearchSkill(CMoveShape* pTarget)
{
	return m_pOwner->IsFightable();

}


//触发：战斗触发，重伤触发，死亡触发
DWORD CBaseAI::CycleSpring()
{
	return 0;
}
DWORD CBaseAI::BornSpring()
{
	return 0;
}
DWORD CBaseAI::FightSpring()
{
	return 0;
}
DWORD CBaseAI::GBHSpring(int hurt)
{
	return 0;
}
DWORD CBaseAI::DeadSpring()
{
	return 0;
}
DWORD CBaseAI::FindTargetSpring()
{
	return 0;
}
DWORD CBaseAI::EscapeSpring()
{
	return 0;
}

//得到下一个移动的目标点
//lMaxDistance:一次移动的最远距离
BOOL CBaseAI::GetNextMoveDestCell(long lDestX,long lDestY,long lMaxDistance,long &lX,long &lY)
{
	long lTileX = m_pOwner->GetTileX();
	long lTileY = m_pOwner->GetTileY();
	long lDir	= GetLineDir(lTileX,lTileY,lDestX,lDestY);	
	long lDistance = m_pOwner->Distance(lDestX,lDestY);
	lMaxDistance = min(lMaxDistance,lDistance);
	return GetNextMoveDestCell(lDir,lMaxDistance,lX,lY);
}

//得到下一个移动的目标点
//lMaxDistance:一次移动的最远距离
BOOL CBaseAI::GetNextMoveDestCell(long lDir,long lMaxDistance,long &lX,long &lY)
	{
	if( m_pOwner == NULL ) return FALSE;
	CRegion* pRegion = dynamic_cast<CRegion*>( m_pOwner -> GetFather() );
	if( pRegion == NULL ) return FALSE;

	long lCurX = m_pOwner->GetTileX();
	long lCurY = m_pOwner->GetTileY();
	long lDestX = lCurX;
	long lDestY = lCurY;

	long i = 0;
	for(; i < 8; i++ )
	{
		bool bMove		= true;
		long lSlipDir	= _slip_order[lDir][i];

		//##根据不同体形的shape，计算下一步是否可以行走，3-10-2009，寻路不计体型
		for( list<POINT>::iterator it = s_listMoveCheckCell[0][lSlipDir].begin();
			it != s_listMoveCheckCell[0][lSlipDir].end(); it++ )
		{
			long lCellX = lCurX + it -> x;
			long lCellY = lCurY + it -> y;

			if( !IsCanMoveTo(pRegion,lCellX,lCellY) )
			{
				bMove = false;
				break;
			}
		}
		if( bMove )
		{
			lDestX = lDestX + _dir[lSlipDir][0];
			lDestY = lDestY + _dir[lSlipDir][1];
			break;
		}
	}
	if(i == 9)	return FALSE;

	//在此方向上行走最大格子数
	lDir = GetLineDir(m_pOwner->GetTileX(), m_pOwner->GetTileY(), lDestX,lDestY);
	i = lMaxDistance-1;
	while(i)
	{
		long lNextX = lDestX + _dir[lDir][0];
		long lNextY = lDestY + _dir[lDir][1];
		if(  !IsCanMoveTo(pRegion,lNextX,lNextY) )
		{
			break;
		}

		lDestX = lNextX;
		lDestY = lNextY;
		i--;
	}
	lX = lDestX;
	lY = lDestY;
	return TRUE;
}

	//判断该格子是否能移动
BOOL CBaseAI::IsCanMoveTo(CRegion* pRegion,LONG lX, LONG lY)
{
	if(NULL == pRegion)	return FALSE;

	if (pRegion->GetBlock(lX,lY) != CRegion::BLOCK_NO)
	{
		return FALSE;
	}
	return TRUE;
}

//移动到特定格子的检测
BOOL CBaseAI::MoveToCell(LONG lX, LONG lY)
{
	CRegion* pRegion = dynamic_cast<CRegion*>( m_pOwner -> GetFather() );
	if( pRegion == NULL ) 
	{
		return FALSE;
	}

	int figure=m_pOwner->GetFigure();
	// 3-10-2009,对于怪物，移动忽略体型
	if( m_pOwner->GetType() == TYPE_MONSTER )
	{
		figure = 0;
	}

	for (int x=-figure; x<=figure; x++)
	{
		for (int y=-figure; y<=figure; y++)
		{
			if ( !IsCanMoveTo(pRegion,lX+x,lY+y) )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CBaseAI::MoveToCell(LONG lDir,LONG lCurrentX,LONG lCurrentY)
{
	if( m_pOwner == NULL ) 
	{
		return FALSE;
	}

	if( m_pOwner ->CanMove() == FALSE )
	{
		return FALSE;
	}

	CRegion* pRegion = dynamic_cast<CRegion*>( m_pOwner -> GetFather() );
	if( pRegion == NULL ) 
	{
		return FALSE;
	}

	//##求能够移动的方向
	LONG lCellX,lCellY;
	BOOL bMove=TRUE;

	// 3-10-2009 对于怪物，忽略体型
	for( list<POINT>::iterator it = s_listMoveCheckCell[0][lDir].begin();
		it != s_listMoveCheckCell[0][lDir].end(); it++ )
	{
		lCellX = lCurrentX + it -> x;
		lCellY = lCurrentY + it -> y;

		if(  !IsCanMoveTo(pRegion,lCellX,lCellY) )
		{
			bMove = FALSE;
			break;
		}
	}
	return bMove;
}

//使用一个技能
BOOL CBaseAI::BeginSkill(stModuParam *pModuParam, LONG lx, LONG ly,CMoveShape* pTarget)
{
	if (pModuParam==NULL)
	{
#ifdef _SKILL_DEBUG
		char str[256];
		_snprintf(str, 256, "Skill 失败:技能ID:%d,步骤%d\n",pSkill->GetID(),4);
		PutStringToFile("SKill.log",str);
#endif
		return FALSE;
	}
	long ltype=0;
	CGUID guid;
	if (pTarget)
	{
		ltype = pTarget->GetType();
		guid  = pTarget->GetExID();
	}

	CBaseModule *pSkill = CNewSkillFactory::CreateModule(pModuParam->GetID(),pModuParam->GetLv(),pModuParam,pModuParam->GetGlobleID(),pModuParam->GetGoodsIndex());
	if (NULL == pSkill)
	{
		//使用技能失败时候的处理	
		CNewSkillFactory::RealeaseModuParam(pModuParam);
		return FALSE;
	}

	if(pModuParam->GetModType()==ModuType_Skill_Passive	|| pModuParam->GetModType()==ModuType_Buff
														|| pModuParam->GetModType()==ModuType_DeBuff 
														|| pModuParam->GetModType()==ModuType_Skill_OnOff
														|| pSkill->Begin(lx,ly,ltype,guid)==MR_Modu_End)
	{
		
		//使用技能失败时候的处理	
		CNewSkillFactory::RealeaseModuParam(pModuParam);
		return FALSE;
	}
	else
	{	
		if (m_pOwner->GetType()==TYPE_PLAYER && pModuParam->GetID()<50000)
		{
			((CPlayer*)m_pOwner)->OnWeaponDamaged();
		}
		if (pModuParam->GetAttack()==eYesAttack)
		{
			//攻击技能
			m_pOwner->SetAttackTarget(guid,ltype);
			m_pOwner->ProduceEvent(ET_Attack);
		}
		m_pOwner->ProduceEvent(ET_UseSkill);
		m_pOwner->AddActiveSkill(pModuParam);
		return TRUE;
	}
}

BOOL CBaseAI::OnMove(LONG lX, LONG lY,LONG actTime)
{
	if (m_pOwner && m_pOwner->CanMove())
	{
		m_pOwner->OnMove((float)lX+0.5f,(float)lY+0.5f);
		AddAIEvent(AET_MOVE,actTime);
	}
	else
	{
		AddAIEvent(AET_MOVE,MOVE_FAIL_APACE_TIME);
	}

	return true;
}

//删除周期触发
bool CBaseAI::CleanCycleSpring()
{
	if (m_csCycleSpring.csSpringType!=CS_LEVELFIGHT)
	{
		m_csCycleSpring.Clean();
	}
	return true;
}

//回到和平状态
BOOL CBaseAI::ReturnPeace()
{
	return false;
}

BOOL CBaseAI::CheckCycleSpring(SCycleSpringEvent& cycleevent)
{
	if(cycleevent.csSpringType==CS_NULL)
		return FALSE;

	else if(cycleevent.csSpringType==CS_MOVEBY_STEP)
	{
		if(--cycleevent.unActType.tgMoveByStep.lStepNum<0)
		{
			cycleevent.Clean();
			//chenaly_zhanshi
			//m_bIsLeaveRoad=FALSE;
			return FALSE;
		}

		//CheckLevelRoad();

		CRegion* pRegion = dynamic_cast<CRegion*>( m_pOwner -> GetFather() );

		if (!pRegion)
		{
			cycleevent.Clean();
			return FALSE;
		}

		MoveTo((LONG)cycleevent.unActType.tgMoveByStep.lDirect);

		return TRUE;
	}
	else if (cycleevent.csSpringType==CS_MOVEBY_TIME)
	{
		//CheckLevelRoad();

		if (timeGetTime()> cycleevent.unActType.tgMoveByTime.lStopTime)
		{
			cycleevent.Clean();
			//chenaly_zhanshi
			//m_bIsLeaveRoad=FALSE;
			return FALSE;
		}

		CRegion* pRegion = dynamic_cast<CRegion*>( m_pOwner -> GetFather() );
		if (!pRegion)
		{
			cycleevent.Clean();
			return FALSE;
		}
		MoveTo((LONG)cycleevent.unActType.tgMoveByTime.lDirect);
		return TRUE;
	}
	else if (cycleevent.csSpringType==CS_MOVETO_CELL)
	{
		//CheckLevelRoad();

		LONG lX		= m_pOwner -> GetTileX();
		LONG lY		= m_pOwner -> GetTileY();
		LONG lDir	= GetLineDir(  lX, lY ,cycleevent.unActType.tgMoveToCell.lX, cycleevent.unActType.tgMoveToCell.lY);
		m_pOwner -> GetDirPos( lDir, lX, lY );
		CRegion* pRegion = dynamic_cast<CRegion*>( m_pOwner -> GetFather() );
		if (!pRegion)
		{
			cycleevent.Clean();
			return FALSE;
		}
		if (lX== cycleevent.unActType.tgMoveToCell.lX && lY==cycleevent.unActType.tgMoveToCell.lY)
		{
			cycleevent.Clean();
			//chenaly_zhanshi
			//m_bIsLeaveRoad=FALSE;
			return FALSE;
		}
		MoveTo(lX,lY);
		return TRUE;
	}
	else if(cycleevent.csSpringType==CS_SKILL)
	{
		CMoveShape* pShape=GetTarget();
		if(pShape==NULL)
		{
			cycleevent.Clean();
			return FALSE;
		}
		CMonster *pMonster = dynamic_cast<CMonster*>(m_pOwner);
		if (pMonster!=NULL)
		{
#ifdef _RUNSTACKINFO1_
			char pszStatckInfo[256]="";
			_snprintf(pszStatckInfo,256,"CBaseAI::CheckCycleSpring(SCycleSpringEvent& cycleevent) skillid:%d,lv:%d Start",cycleevent.unActType.tgUseSkill.lSkillID,cycleevent.unActType.tgUseSkill.lSkillLevel);
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif

			pMonster->InitSkillInstance(cycleevent.unActType.tgUseSkill.lSkillID,cycleevent.unActType.tgUseSkill.lSkillLevel,0,0,0,NULL_GUID,9);
			BeginSkill(pMonster->GetInstanceSkill(),
				pShape->GetTileX(),pShape->GetTileY(),pShape);
#ifdef _RUNSTACKINFO1_
			//char pszStatckInfo[256]="";
			_snprintf(pszStatckInfo,256,"CBaseAI::CheckCycleSpring(SCycleSpringEvent& cycleevent) end");
			CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
		}
		else
		{
			char pszInfo[1024]="";
			_snprintf(pszInfo,1024,"CBaseAI::CheckCycleSpring(SCycleSpringEvent& cycleevent) exception");
			PutStringToFile("CheckCycleSpring",pszInfo);
		}	

		cycleevent.Clean();
		return TRUE;
	}
	else if( cycleevent.csSpringType == CS_SKILLEX )
	{
		// find the target specified by cycleevent.gAimGUID
		CServerRegion *pRegion = static_cast<CServerRegion*>( m_pOwner->GetFather() );
		CMoveShape *pTarget = static_cast<CMoveShape*>( pRegion->FindChildObject( TYPE_MONSTER, cycleevent.gAimGUID ));
		if( pTarget == NULL )
		{
			pTarget = static_cast<CMoveShape*>( pRegion->FindChildObject( TYPE_PLAYER, cycleevent.gAimGUID ));
		}

		if (pTarget!=NULL)
		{
			static_cast<CMonster*>( m_pOwner )->InitSkillInstance( cycleevent.unActType.tgUseSkill.lSkillID, cycleevent.unActType.tgUseSkill.lSkillLevel,0,0,0,NULL_GUID,8 );
			BeginSkill( m_pOwner->GetInstanceSkill(), pTarget->GetTileX(), pTarget->GetTileY(), pTarget );
		}
		

		cycleevent.Clean();
		return TRUE;
	}
	else if( cycleevent.csSpringType == CS_SKILLCELL )
	{
		LONG lPos = cycleevent.unActType.tgUseSkill.lPos;
		LONG lPosX = lPos >> 16;
		LONG lPosY = lPos & 0x0000ffff;

		static_cast<CMonster*>( m_pOwner )->InitSkillInstance( cycleevent.unActType.tgUseSkill.lSkillID, cycleevent.unActType.tgUseSkill.lSkillLevel,0,0,0,NULL_GUID,7 );
		BeginSkill( m_pOwner->GetInstanceSkill(), lPosX, lPosY, NULL );

		cycleevent.Clean();

		return TRUE;
	}
	else if (cycleevent.csSpringType==CS_STAND)
	{
		if (--cycleevent.unActType.tgStand.lStandTimes<0)
		{
			cycleevent.Clean();
			return FALSE;
		}
		Stand(cycleevent.unActType.tgStand.lStandTime);
		return TRUE;
	}
	else if (cycleevent.csSpringType==CS_RANDOMMOVE)
	{
		if (timeGetTime()<cycleevent.unActType.lRandomMoveTime)
		{
			RandomRun();
			return TRUE;
		}
		else
		{
			cycleevent.Clean();
			//chenaly_zhanshi
			//m_bIsLeaveRoad=FALSE;
			return FALSE;
		}
	}
	else if (cycleevent.csSpringType==CS_ESCAPE)
	{
		if (timeGetTime()<cycleevent.unActType.lRandomMoveTime)
		{
			OnEscapeState();
			return TRUE;
		}
		else
		{
			// 移除虚弱状态，恢复正常(如果该状态已经在OnEscapeState中被移除，这里调用也是安全的)
			m_pOwner->DelNewState( STATE_BASE_WEAKNESS, 1 );
			cycleevent.Clean();
			return FALSE;
		}

	}
	else if (cycleevent.csSpringType== CS_SENDEXPACT)
	{
		//CMonster* pMonster= dynamic_cast<CMonster*>(m_pOwner);
		Stand(EXCEPTION_STATE_SPACE);
		AddCycleSpring(CS_SENDEXPACTNEXT,cycleevent.unActType.tgSendExpAct.lActionID,
			cycleevent.unActType.tgSendExpAct.lActTime,
			cycleevent.unActType.tgSendExpAct.lAimType,
			cycleevent.gAimGUID);
		return TRUE;
	}
	else if (cycleevent.csSpringType== CS_SENDEXPACTNEXT)
	{
		CMonster* pMonster= dynamic_cast<CMonster*>(m_pOwner);
		if (pMonster!= NULL)
		{
			pMonster->ExpAction(cycleevent.unActType.tgSendExpAct.lActionID,cycleevent.unActType.tgSendExpAct.lAimType,cycleevent.gAimGUID);
		}

		AddCycleSpring(CS_STAND,cycleevent.unActType.tgSendExpAct.lActTime/EXCEPTION_STATE_SPACE ,EXCEPTION_STATE_SPACE);
		Stand(EXCEPTION_STATE_SPACE);
		return TRUE;
	}
	else if(cycleevent.csSpringType==CS_LEVELFIGHT)
	{

		CMonster* pMonster= dynamic_cast<CMonster*>(m_pOwner);
		long aimX,aimY;
		aimX=cycleevent.unActType.tgLevelFight.AimX;
		aimY=cycleevent.unActType.tgLevelFight.AimY;
		long x= pMonster->GetPosX();
		long y= pMonster->GetPosY();

		if (cycleevent.unActType.tgLevelFight.lTime>= timeGetTime())
		{
			int lDir=GetLineDir(x,y,aimX,aimY);
			m_pOwner -> GetDirPos( lDir, x, y );
			float fDis=1;
			if (lDir%2!=0) fDis*= 1.414f;
			float fTime= fDis/m_pOwner->GetSpeed();

			OnMove(x,y,(DWORD)fTime);
			//m_pOwner->OnMove()
			//设置位置值
			m_pOwner->SetPosXY(x,y);

			return TRUE;
		}
		else
		{
			m_pOwner->ProduceEvent(ET_Move);
			m_pOwner->ProduceEvent(ET_EventBreak);

			m_pOwner->SetPosition(aimX,aimY);

			// 设置归位坐标失败，目标点不是BLOCK_NO
			if( m_pOwner->GetTileX() != aimX || m_pOwner->GetTileY() != aimY )
			{
				CRegion *region = static_cast<CRegion*>( m_pOwner->GetFather()  );
				if( region != NULL )
				{
					// 目标点存在阻挡，在周围查找无阻挡点
					long rx, ry;
					region->GetRandomPosInRange( rx, ry, aimX - 3, aimY - 3, 6, 6 );

					char msg[1024];
					_snprintf( msg, sizeof( msg ), "归位设置坐标失败，目标阻挡类型：%u，重新查找点：(%d,%d):%u.", 
						region->GetBlock( aimX, aimY ), rx, ry, region->GetBlock( rx, ry ) );
					PutStringToFile( "debug_monster", msg );

					m_pOwner->SetPosition( rx, ry );
				}
			}
			CMonsterList::tagMonster* p = CMonsterList::GetPropertyByOrginName(
				static_cast<CMonster*>( m_pOwner )->GetOriginName());
			m_pOwner->God( p->byIsunbeatable == 1 ? true : false );
			m_pOwner->SetChangeSpeed(0);
			m_pOwner->SendChangedSpeedToClients();
			Stand(1000);
			cycleevent.Clean();
			return TRUE;
		}
			
	}
	else
		return false;
}

BOOL CBaseAI::GetNearPosition(LONG &x, LONG &y, LONG dir,LONG searchDir)
{
	if (dir= -1)
		dir= random(8);
	else
		dir=dir%8;

	if (searchDir= -1)
		searchDir= random(8);
	else
		searchDir=searchDir%8;

	// 第一圈
	static int pos_1[8][2]=
	{
		-1,-1, 0,-1, 1,-1,
		-1, 0,	   1, 0,
		-1, 1, 0, 1, 1, 1,
	};

	// 第二圈
	static int pos_2[16][2]=
	{
		-2,-2, -1,-2, 0,-2, 1,-2, 2,-2,
		-2,-1,					 2,-1,
		-2, 0,					 2, 0,
		-2, 1,					 2, 1,
		-2, 2, -1, 2, 0, 2, 1, 2, 2, 2,
	};

	// 第三圈
	static int pos_3[24][2]=
	{
		-3,-3, -2,-3, -1,-3, 0,-3, 1,-3, 2,-3, 3,-3,
		-3,-2,								   3,-2,
		-3,-1,								   3,-1,
		-3, 0,								   3, 0,
		-3, 1,								   3, 1,
		-3, 2,								   3, 2,
		-3, 3, -2, 3, -1, 3, 0, 3, 1, 3, 2, 3, 3, 3
	};

	CRegion* pRegion= dynamic_cast<CRegion*>(m_pOwner->GetFather());
	if (pRegion== NULL)	return FALSE;

	POINT  point;	
	int iSearchPos= searchDir;
	int iIndex=0;
	for (int a=0; a<8/2; a++)
	{
		iIndex=(iSearchPos+a)%8;
		point.x= x+ pos_1[iIndex][0];
		point.y= y+ pos_1[iIndex][1];
		if (pRegion->GetBlock(point.x,point.y)== CRegion::BLOCK_NO)
		{x= point.x; y= point.y;return TRUE;}

		iIndex=(iSearchPos-a-1)%8;
		point.x= x+ pos_1[iIndex][0];
		point.y= y+ pos_1[iIndex][1];
		if (pRegion->GetBlock(point.x,point.y)== CRegion::BLOCK_NO)
		{x= point.x; y= point.y; return TRUE;}
	}

	iSearchPos= searchDir*2;
	for (int a=0; a<16/2; a++)
	{
		iIndex= (iSearchPos+a)%8;
		point.x= x+ pos_2[iIndex][0];
		point.y= y+ pos_2[iIndex][1];
		if (pRegion->GetBlock(point.x,point.y)== CRegion::BLOCK_NO)
		{x= point.x; y= point.y; return TRUE;}

		iIndex=(iSearchPos-a-1)%8;
		point.x= x+ pos_2[iIndex][0];
		point.y= y+ pos_2[iIndex][1];
		if (pRegion->GetBlock(point.x,point.y)== CRegion::BLOCK_NO)
		{x= point.x; y= point.y; return TRUE;}
	}

	iSearchPos= searchDir*3;
	for (int a=0; a<24/2; a++)
	{
		iIndex= (iSearchPos+a)%8;
		point.x= x+ pos_3[iIndex][0];
		point.y= y+ pos_3[iIndex][1];
		if (pRegion->GetBlock(point.x,point.y)== CRegion::BLOCK_NO)
		{x= point.x; y= point.y; return TRUE;}

		iIndex= (iSearchPos-a-1)%8;
		point.x= x+ pos_3[iIndex][0];
		point.y= y+ pos_3[iIndex][1];
		if (pRegion->GetBlock(point.x,point.y)== CRegion::BLOCK_NO)
		{x= point.x; y= point.y; return TRUE;}
	}

	return FALSE;
}
//加入周期触发
BOOL CBaseAI::AddCycleSpring(CYCLE_SPRINGACT type, LONG m_1, LONG m_2,LONG m_3,CGUID guid)
{
	if (m_csCycleSpring.csSpringType==CS_LEVELFIGHT) return FALSE;
	
	m_csCycleSpring.csSpringType=type;
	m_csCycleSpring.unActType.tgSendExpAct.lActionID=m_1;
	m_csCycleSpring.unActType.tgSendExpAct.lActTime=m_2;
	m_csCycleSpring.unActType.tgSendExpAct.lAimType=m_3;

	m_csCycleSpring.gAimGUID= guid;
	return TRUE;
}

BOOL CBaseAI::RunByRoad()
{return FALSE;}


BOOL CBaseAI::RandomRun()
{
	return TRUE;
}

VOID CBaseAI::StartCycle(DWORD dwStartTime,DWORD dwCycleTime )
{
	if (m_aeCycle.aetEventType==AET_NULL)
	{
		m_aeCycle.aetEventType=AET_CYCLE;
		//在这里添加注册信息，注册一个事件
		tagTimerVar* pTimvar = CBaseObject::CreateTimerVar(12);
		pTimvar->TimerType = ChildObjType_AI;
		pTimvar->lvar = AET_CYCLE;
		m_aeCycle.lTimerID=GameManager::GetInstance()->Schedule(m_pOwner,(void*)pTimvar,timeGetTime()+dwStartTime,dwCycleTime);
		m_aeCycle.dwSpringTime=dwCycleTime;
	}
}

VOID CBaseAI::EndCycle()
{
	LogoutEvent(m_aeCycle);
}

void CBaseAI::Reset()
{
	LogoutEvent( m_aeCycle );
	LogoutEvent( m_aeActive );
	LogoutEvent( m_aeScriptAISkill );
	m_aesAIState = AES_INVALID;
}

VOID CBaseAI::StartActiveAct()
{
	AddAIEvent(AET_MOVE,0);
}

VOID CBaseAI::ChangeStateTo(INT state)
{
	if (m_pOwner->GetState()==state)return;

	if (m_pOwner->IsDied())return;

	if (state==CShape::STATE_PEACE)
	{
		m_pOwner->ChangeStateTo((CShape::eSTATE)state);
		m_aesAIState=AES_FIGHT_NORMAL;
		CleanCycleSpring();
	}
	else if (state==CShape::STATE_FIGHT)
	{
		m_pOwner->ChangeStateTo((CShape::eSTATE)state);
		m_aesAIState=AES_FIGHT_NORMAL;
		CleanCycleSpring();
		FightSpring();
	}
	else if (state==CShape::STATE_DIED)
	{
		m_pOwner->ChangeStateTo((CShape::eSTATE)state);
		m_aesAIState=AES_FIGHT_NORMAL;
		CleanCycleSpring();
	}

}

VOID CBaseAI::Stand(LONG time)
{
	AddAIEvent(AET_MOVE ,time);
}
CMoveShape*	CBaseAI::GetOwner()
{
	return m_pOwner;
}
VOID CBaseAI::SetOwner(CMoveShape* pOwner)
{
	if( pOwner )
	{
		Clear();
		m_pOwner = pOwner;
	}
}

//AI类型
LONG CBaseAI::GetAIType()
{
	return m_lAIType;
}
VOID CBaseAI::SetAIType(LONG lAIType)
{
	m_lAIType=lAIType;
}
//当技能结束的时候，（技能结束的时候被调用）
BOOL CBaseAI::OnSkillEnd(long lAddSub)
{
	// 技能回调时AI已经无效
	if( m_aesAIState == AES_INVALID )
	{
		return FALSE;
	}

	DWORD	dwActStartTime= m_pOwner->GetActStartTime();				//动作开始时间
	DWORD   dwActTime= m_pOwner->GetActTime();					//动作时间
	DWORD   dwLeftTime= dwActStartTime+dwActTime-timeGetTime();
	//DeleteAIEvent(AET_ACT);
	AddAIEvent(AET_MOVE,dwLeftTime>0?dwLeftTime:0);
	return TRUE;
}
//当被攻击的时候
BOOL CBaseAI::WhenBeenHurted(LONG lType,const CGUID& guid ,DWORD a)
{
	return m_pOwner->GetForceAttack();
}

//被杀害的时候
BOOL CBaseAI::WhenBeenKilled(LONG lType,const CGUID& guid)
{
	return false;
}

//冬眠：冬眠，苏醒，获取冬眠时间，查询是否冬眠
VOID CBaseAI::Hibernate()
{
	//m_aesAIState=AES_FIGHT_NULL;
	ChangeStateTo(CShape::STATE_HANGUP);
	//m_bIsHibernate=TRUE;
	m_dwDormancyTime = timeGetTime();
	m_dwDormancyInterval = 0;
}
VOID CBaseAI::WakeUp()
{
 	m_dwDormancyInterval = timeGetTime() - m_dwDormancyTime;
	m_dwDormancyTime = 0;
	ChangeStateTo(CShape::STATE_PEACE);
	//m_bIsHibernate=FALSE;
	StartActiveAct();

	//if(m_aeActive.aetEventType==AET_ACT)
	//{
	//	ChangeStateTo(CShape::STATE_FIGHT);
	//	m_aesAIState=AES_FIGHT_ATTACK;
	//}
	//else if( m_aeActive.aetEventType==AET_MOVE)
	//{
	//	//
	//	ChangeStateTo(CShape::STATE_PEACE);
	//	//ChangeStateTo(AES_PEACE);
	//	//m_aesAIState=AES_PEACE;

	//}
	//else 
	//{
	//	//m_aesAIState=AES_PEACE;
	//	ChangeStateTo(CShape::STATE_PEACE);
	//}

	//if(m_aeCycle.aetEventType==AET_NULL)
	//{
	//	AddAIEvent(AET_CYCLE,1500);
	//}
}

DWORD CBaseAI::GetDormancyInterval()
{
	return m_dwDormancyInterval;
}
BOOL CBaseAI::IsHibernated()
{
	return m_pOwner->GetState()==CShape::STATE_HANGUP? true:false;
}

//移动
BOOL CBaseAI::MoveTo(float fDestX, float fDestY)
{
	if( !m_pOwner ) return	FALSE;

	LONG lPosX		= m_pOwner -> GetTileX();
	LONG lPosY		= m_pOwner -> GetTileY();
	LONG lDir		= GetLineDir( lPosX, lPosY, (long)fDestX, (long)fDestY );
	return MoveTo(lDir);
}



//VOID CBaseAI::ForceMove( LONG lDestX, LONG lDestY,DWORD dwTime )
//{
//	LogoutEvent(m_aeActive);
//	AddAIEvent(AET_MOVE,dwTime);//未完成
//}
//攻击

VOID CBaseAI::Attack(long tID, LONG lX/* =0 */, LONG lY/* =0 */, CMoveShape* pTarget/* =NULL  */)
{

}

//目标： 设置，询问，获取类型，获取id，获取指针
VOID CBaseAI::SetTarget( LONG lType, const CGUID& TargetGuid )
{
	//if (m_pOwner->IsGod()) return;
	// 如果处于归位周期(特定于怪物)，就不能设置目标
	if( m_csCycleSpring.csSpringType == CS_LEVELFIGHT )
	{
		return;
	}
	m_lTargetType	= lType;
	m_TargetGuid	= TargetGuid;
	FindTargetSpring();
	ChangeStateTo(CShape::STATE_FIGHT);
	if (m_aesAIState==AES_FIGHT_TRANSITION)
	{
		m_aesAIState= AES_FIGHT_NORMAL;
	}
}

VOID CBaseAI::SetForceTarget( LONG lType, const CGUID& TargetGuid )
{
}
VOID CBaseAI::CleanForceTarget ()
{}

BOOL CBaseAI::HasTarget()
{
	if( m_pOwner != NULL && 
		( (NULL_GUID != m_TargetGuid && m_lTargetType > 0) ||
		(m_lTargetX != 0 && m_lTargetY != 0) ) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

LONG CBaseAI::GetTargetType()
{
	return m_lTargetType;
}
const CGUID& CBaseAI::GetTargetID()
{
	return m_TargetGuid;
}
CMoveShape* CBaseAI::GetTarget()
{
	if( m_pOwner == NULL ) 
	{
		return NULL;
	}
	if( m_lTargetType == 0 || m_TargetGuid == NULL_GUID )
	{
		return NULL;
	}
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner ->GetFather() );
	if( pRegion == NULL ) 
	{
		return NULL;
	}

	return dynamic_cast<CMoveShape*>( pRegion -> 
		FindChildObject( m_lTargetType, m_TargetGuid ) );
}
//丢失目标
BOOL CBaseAI::WhenLoseTarget()
{
	return LoseTarget();
}

BOOL CBaseAI::LoseTarget()
{
	if( m_pOwner == NULL ) return FALSE;

	//##如果当前的AI事件是Attack，是否需要清掉该事件？

	CMonster*pOwner=dynamic_cast<CMonster*>(m_pOwner);
	//##清空一些成员变量
	m_TargetGuid			= NULL_GUID;
	m_lTargetType			= 0;
	m_lTargetX				= 0;
	m_lTargetY				= 0;

	return TRUE;
}
//强制目标
//BOOL CBaseAI::ForceTarget(CMoveShape * pTarget , DWORD dwForceTime)
//{
//
//	return true;
//}

//改变为异常状态
//BOOL CBaseAI::ChangeToExceptional(AI_EXEC_STATE ExpectionalType,LONG ExpectionalTime)
//{
//	return FALSE;
//}
//暂时与以前统一的接口
//##获取当前主动事件队列的头部
BOOL CBaseAI::IsInAct()
{
	return m_aeActive.aetEventType==AET_ACT;
}

//##清除AI管线中所有事件的接口
VOID CBaseAI::Clear()                                    //有改动
{
	m_dwLastExecTimeStamp	= 0;
	//m_TargetGuid				= 0;
	m_lTargetType			= 0;
	m_pOwner				= NULL;
	m_aesAIState            = AES_FIGHT_NORMAL;
	LogoutEvent(m_aeActive);
	m_aeActive.Clean();
	LogoutEvent(m_aeCycle);
	m_aeCycle.Clean();
	CleanCycleSpring();

}
//##添加后台技能时调用的函数
//VOID CBaseAI::WhenAddBackStageSkill(long tID)
//{
//	//##如果所有者不存在。直接退出。
//	if( m_pOwner == NULL ) return;
//
//	if( tID != SKILL_UNKNOW )
//	{
//		m_vBackStageSkills.push_back( tID );
//	}
//}


//添加和平时候的脚本名
VOID CBaseAI::AddIdleActScript(string & strScriptName)
{
	m_strIdleActScript=strScriptName;
}


//脚本接口
BOOL CBaseAI::UseSkillByID(LONG skillid,LONG skilllevel, const CGUID &targetID)
{
	if( targetID == NULL_GUID )
	{
		AddCycleSpring(CS_SKILL,skillid,skilllevel);
	}
	else
	{
		AddCycleSpring( CS_SKILLEX, skillid, skilllevel, 0, targetID );
	}
	return TRUE;
}

BOOL CBaseAI::UseSkillByIDOnCell( LONG skillID, LONG skillLevel, LONG posX, LONG posY )
{
	if( posX == 0 && posY == 0 )
	{
		CMoveShape *pTarget = GetTarget();
		if( pTarget == NULL )
		{
			return FALSE;
		}

		posX = pTarget->GetTileX();
		posY = pTarget->GetTileY();
	}
	// maybe it will occur some bugs somedays
	LONG pos = ( posX << 16 ) | ( posY );
	AddCycleSpring( CS_SKILLCELL, skillID, skillLevel, pos );
	return true;
}

BOOL CBaseAI::MoveToCellOnPeace(LONG x,LONG y)
{
	if (m_pOwner->GetState()== CShape::STATE_PEACE)
	{
		AddCycleSpring(CS_MOVETO_CELL,x,y);
		return TRUE;
	}
	return FALSE;
}

BOOL CBaseAI::MoveToCellOnFight		(LONG x,LONG y)
{
	if (m_pOwner->GetState()==CShape::STATE_FIGHT && m_aesAIState!= AES_FIGHT_ESCAPE	)
	{
		AddCycleSpring(CS_MOVETO_CELL,x,y);
		return TRUE;
	}
	return FALSE;
}

BOOL CBaseAI::MoveToCellAround(LONG x,LONG y)
{
	if (m_pOwner->Distance(x,y)>POST_AREA)
	{
		AddCycleSpring(CS_MOVETO_CELL,x,y);
	}
	return TRUE;
}

BOOL CBaseAI::MoveByStepOnPeace(LONG dir, LONG step)
{
	if (m_pOwner->GetState()== CShape::STATE_PEACE)
	{
		AddCycleSpring(CS_MOVEBY_STEP,dir,step);
		return TRUE;
	}
	return FALSE;
}

BOOL CBaseAI::MoveByStepOnFight(LONG dir, LONG step)
{
	if (m_pOwner->GetState()==CShape::STATE_FIGHT && m_aesAIState!= AES_FIGHT_ESCAPE)
	{
		AddCycleSpring(CS_MOVEBY_STEP,dir,step);
		return TRUE;
	}
	return FALSE;
}

//在和平状态下播放一个动作
BOOL CBaseAI::SendExpActOnPeace(LONG lActID,LONG lAimTime,LONG lAimType,CGUID guid )
{
	if (m_pOwner!=NULL && m_pOwner->GetState()==CShape::STATE_PEACE &&m_csCycleSpring.csSpringType!= CS_LEVELFIGHT)	
	{
		AddCycleSpring(CS_SENDEXPACT,lActID,lAimTime,lAimType,guid);
		return TRUE;
	}
	return FALSE;
}
//在战斗状态下播放一个动作
BOOL CBaseAI::SendExpActOnFight(LONG lActID,LONG lAimTime,LONG lAimType,CGUID guid )
{
	if (m_pOwner!=NULL && m_pOwner->GetState()==CShape::STATE_FIGHT && m_csCycleSpring.csSpringType!= CS_LEVELFIGHT)	
	{
		AddCycleSpring(CS_SENDEXPACT,lActID,lAimTime,lAimType,guid);
		return TRUE;
	}
	return FALSE;
}

BOOL CBaseAI::MoveByTimeOnPeace(LONG dir, LONG time)
{
	if (m_pOwner->GetState()== CShape::STATE_PEACE)
	{
		AddCycleSpring(CS_MOVEBY_TIME,dir,timeGetTime()+time);
		return TRUE;
	}
	return FALSE;
}

BOOL CBaseAI::MovebyTimeOnFight(LONG dir, LONG time)
{
	if (m_pOwner->GetState()==CShape::STATE_FIGHT && m_aesAIState!= AES_FIGHT_ESCAPE)
	{
		AddCycleSpring(CS_MOVEBY_TIME,dir,timeGetTime()+time);
		return TRUE;
	}
	return FALSE;
}

BOOL CBaseAI::RandomMoveByTime(LONG time)//在战斗状态的使之随意走动
{
		AddCycleSpring(CS_RANDOMMOVE,time+timeGetTime());
		return TRUE;

	return FALSE;
}

BOOL CBaseAI::Stand(LONG times,LONG time)
{
	AddCycleSpring(CS_STAND,times,time);
	return TRUE;
}


//逃跑一段时间(和平状态下，  只需要模拟逃跑，即为反方向行走)
BOOL CBaseAI::EscapeByTimeOnPeace(LONG dir,LONG time)
{
	return MoveByTimeOnPeace(dir,time);
}

//逃跑一段时间
BOOL CBaseAI::EscapeByTimeOnFight(LONG time)
{
	if (m_pOwner->GetState()==CShape::STATE_FIGHT && m_aesAIState!= AES_FIGHT_ESCAPE)
	{
		AddCycleSpring(CS_ESCAPE,timeGetTime()+time);
		return TRUE;
	}
	return FALSE;
}

BOOL CBaseAI::LevelFight(LONG time,LONG aimX,LONG aimY)
{
	//ChangeStateTo(m_)

	AddCycleSpring(CS_LEVELFIGHT,timeGetTime()+time,aimX ,aimY);
	int speed=1/(CGlobeSetup::GetSetup()->fReturnPeaceSpeed - m_pOwner->GetSpeed());
	m_pOwner->SetChangeSpeed(speed);
	m_pOwner->SendChangedSpeedToClients();
	m_pOwner->God(true);
	return TRUE;

}

//取消注册的行为
BOOL CBaseAI::DeleteSpring()
{
	CleanCycleSpring();;
	return TRUE;
}
//关闭逃跑状态
BOOL CBaseAI::EndEscape()
{
	if (m_aesAIState==AES_FIGHT_ESCAPE)
	{
		m_aesAIState=AES_FIGHT_NORMAL;
	}
	if (m_csCycleSpring.csSpringType==CS_ESCAPE)
	{
		CleanCycleSpring();
	}
	return TRUE;
}

bool LogicLogInterface::logT470_AIExc_log(CMonster* pMonster,DWORD dwExcType)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) return true;

	if(NULL == m_pLogClient)
		return false;

	if (pMonster== NULL)
	{
		return false;
	}
	string strTime;
	GetCurrTimeString(strTime);

	return m_pLogClient->SendLogToServer( 
		470, strTime.c_str(),
		&(pMonster->GetExID()),pMonster->GetName(),
		dwExcType);
}