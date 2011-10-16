#include "StdAfx.h"
#include <MMSystem.h>
#include "Pet.h"
#include "..\Player.h"
#include "..\Monster.h"
#include "..\..\..\setup\globesetup.h"

//#include "../Skills/SkillBaseProperties.h"
//#include "../skills/SkillFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//*
CPet::CPet() : m_dwInvalidMasterTimeStamp( 0 ), m_dwSeekMasterTimeStamp( 0 ), m_bMasterLogout( FALSE ), 
m_dwPetLifeCycleTimeStamp( 0 ), m_dwPetLifeCycleCounter( 0 ), m_paPetAction( PA_FALLOWING ), 
m_psemPetAIMode( PSEM_DEFENSE )
{
}

CPet::~CPet()
{
}


VOID CPet::SetPetCurrentAIMode( PET_SEARCH_ENEMY_MODE pm )
{
	
}

CPet::PET_SEARCH_ENEMY_MODE CPet::GetPetCurrentAIMode()
{
	return m_psemPetAIMode;
}

VOID CPet::SetPetCurrentAction( CPet::PET_ACTION pa )
{
}

CPet::PET_ACTION CPet::GetPetCurrentAction()
{
	return m_paPetAction;
}


VOID CPet::OnSchedule()
{
}

//##攻击动作的AI调度
VOID CPet::OnAttackingSchedule()
{
}

VOID CPet::OnFallowingSchedule()
{
}

VOID CPet::OnStayingSchedule()
{		
}


CMoveShape* CPet::GetPetMaster()
{
	CMoveShape* pResult = NULL;
	return pResult;
}

//##在每次需要移动的时候，搜寻最近的目标
BOOL CPet::OnMoving()
{
	return FALSE;
}


BOOL CPet::OnSearchEnemy()
{
	return TRUE;
}

//--------------------------------------------------------------------------------------
// 函数:	OnIdle
// 参数:	无
// 返回值:	无
// 描述：	该函数在AI管线被闲置的时候调用。怪物AI可以用这个函数添加站立和搜索敌人的事件
//			到AI管线。也可以在闲置的时候添加一些随机移动的操作。物品AI可以用这个函数添加
//			站立事件和死亡（消失）事件。来达到定时清楚地目的。
//--------------------------------------------------------------------------------------
VOID CPet::OnIdle()
{
	//##Todo: 怪物的该函数主要目的是为了在怪物发呆期间给他加一个Stand的状态。
	//##怪物的Stand状态的主要目的是为了消耗时间片，另外，偶尔会随机移动。
	//##在一定时间间隔后，此时，怪物的站立状态会消失，其后会增添一个搜寻敌人
	//##的状态。这个状态里面。怪物会搜寻是否有玩家在附近
	CMonster* pOwner = dynamic_cast<CMonster*>( GetOwner() );
	if( pOwner && pOwner -> IsDied() == FALSE )
	{
		
	}
}

BOOL CPet::OnLoseTarget()
{
	return FALSE;
}

VOID CPet::SetTarget( LONG lType, const CGUID& TargetGuid )
{
	CMonsterAI::SetTarget( lType, TargetGuid );
}
