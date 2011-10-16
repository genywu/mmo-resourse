#include "StdAfx.h"
#include ".\monsterai.h"
#include "..\monster.h"
#include "..\Player.h"
#include "..\Colony.h"
#include "..\ServerRegion.h"
#include <mmsystem.h>
#include "..\..\..\setup\globesetup.h"
#include "..\script\Script.h"
#include "..\BuffSkill\BuffSkillDef.h"
#include "..\BuffSkill\BaseModule.h"
#include "..\BuffSkill\NewSkillFactory.h"
#include "../../GameServer/GameManager.h"
#include "AIUtilities.h"
#include "AIFactory.h"





CMonsterAI::CMonsterAI()
{

	mm=0;
	//选择技能id
	m_dwChoiceSkillID= 0;;
	//选择技能等级
	m_dwChoiceSkillLevel= 0;
	//选择技能类型
	m_wSkillType= 0;
	//选择技能索引
	m_wSkillIndex= 0;
	//是否必须被使用
	m_bMustUsed=FALSE;
	//是否被使用
	m_bBeenUsed=TRUE;

	//重伤标识
	//m_fGBHStamp=100.0;				
	//警戒时间标识
	m_dwGuardTimeStamp=0;			
	//最后被攻击时间标识
	m_dwHurtedTimeStamp=0;		
	//技能间隔时间标志
	m_dwSkillSpaceTimeStamp=0;	
	//过渡阶段时间标志
	m_dwTransitionTimeStamp=0;
	m_dwRebornTime = 0;

	//路径相关


	//行走点记录
	m_iRoadRecord=0;				
	//行走方向
	m_iRoadDirect=1;             
	////是否离开了路径
	//m_bIsLeaveRoad;				
	m_bAttackedByGuard = false;

}
CMonsterAI::~CMonsterAI(){}


BOOL CMonsterAI::OnCycle()
{
	CycleSpring();
	return TRUE;
}
//主动行为响应
BOOL CMonsterAI::OnActive()
{
	if (m_pOwner==NULL)
	{
		return FALSE;
	}
#ifdef _RUNSTACKINFO1_
	char pszStatckInfo[256]="";
	_snprintf(pszStatckInfo,256,"CMonsterAI::OnActive(MonsterState:%d, FightSubState:%d) Start", 
		m_pOwner->GetState(), m_aesAIState );
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),pszStatckInfo);
#endif
	BOOL bRet = TRUE;
	do
	{

		if (CheckRandomRunState()== TRUE)
		{
			break;
		}
		// 过了友好时间才能搜索敌人
		if( !IsInKindlyTime() && CheckSearchEnemy()==TRUE)
		{
			break;
		}

		//不同状态调用不同的处理
		if (m_pOwner->GetState()==CShape::STATE_PEACE)
		{
			OnPeaceState();
		}
		else if (m_pOwner->GetState()== CShape::STATE_FIGHT)
		{
			if(CheckCycleSpring(m_csCycleSpring))//处理战斗周期触发。如果周期触发影响了主动行为，直接返回
			{
				break;
			}
			if (CheckReturnPeace())
			{
				break;
			}

			if (m_aesAIState== AES_FIGHT_NORMAL)
			{
				OnFightState();
			}
			else if (m_aesAIState== AES_FIGHT_TRANSITION)
			{
				OnTransitionState();
			}
			else if (m_aesAIState== AES_FIGHT_ESCAPE)
			{
				OnEscapeState();
			}
			else
			{
				bRet = FALSE;
				break;
			}
		}
		else if(m_pOwner->GetState()== CShape::STATE_DIED)
		{
			OnNoHpState();
		}
		else
		{
			bRet = FALSE;
		}
	} while( 0 );

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"CMonsterAI::OnActive() End");
#endif

	return bRet;
}

BOOL CMonsterAI::OnChangeAI ()
{
	if (m_pOwner==NULL)
	{
		return FALSE;
	}
	return dynamic_cast<CMonster*>(m_pOwner)->ChangeAI();
}

BOOL CMonsterAI::OnReBorn()
{
	// 记录重生时间
	m_dwRebornTime = timeGetTime();
	CMonster* pMonster= dynamic_cast<CMonster*>(m_pOwner);
	CServerRegion* pRegion= dynamic_cast<CServerRegion*>(m_pOwner->GetFather());
	if (pMonster!= NULL && pMonster->GetColonyID()!= 0 && pRegion!= NULL)
	{
		CColony* pColony= pRegion->GetOneColony(pMonster->GetColonyID());
		if (pColony!= NULL)
		{
			pColony->WhenBorn(m_pOwner);
		}
	}
	return TRUE;
}
	//和平状态
BOOL CMonsterAI::OnPeaceState()
{
	extern long AREA_WIDTH;		// 区域的宽度
	extern long AREA_HEIGHT;	// 区域的高度
	DWORD x,y,  area_x,area_y;

	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);


	if ( pOwner->GetAIHangupType()==0 && pOwner->GetArea()->IsHibernate())
	{	
		x=m_pOwner->GetTileX();
		y=m_pOwner->GetTileY();
		area_x=x/AREA_WIDTH;
		area_y=y/AREA_HEIGHT;
		INT born_x=m_BornPoint.x/AREA_WIDTH;
		INT born_y=m_BornPoint.y/AREA_HEIGHT;

		if (area_x == m_pOwner->GetArea()->GetX() && area_y == m_pOwner->GetArea()->GetY()
			&& area_x== born_x && area_y== born_y)
		{
			Hibernate();
			return TRUE;
		}
	
	}
	else if (dynamic_cast<CServerRegion*>(pOwner->GetFather())->IsHibernate())
	{
		if (pOwner->GetAIHangupType()<2)
		{
			Hibernate();
			return TRUE;
		}
	}

	


	BOOL bReturnType=FALSE;

	if(CheckCycleSpring(m_csCycleSpring))
	{
		//处理和平周期的触发
		
		//return true;
		bReturnType=TRUE;
	}
	else if (OnPeaceStateOnColony()==TRUE)//集群怪的和平状态处理
	{
		bReturnType=TRUE;
	}
	else 
	{
		if(RunByRoad())
		{
			bReturnType=TRUE;
		}
		else 
		{
			OnPeaceIdle();
		}

	}

	return bReturnType;
}

BOOL CMonsterAI::OnPeaceStateOnColony()
{
	//集群ai处理
	CMonster* pMonster= dynamic_cast<CMonster*>(m_pOwner);
	if (pMonster!= NULL && pMonster->HasColony()==TRUE && pMonster->GetColonyState()==TRUE)
	{
		CServerRegion* pServer= dynamic_cast<CServerRegion*>(pMonster->GetFather());
		CColony* pColony=0;
		if (pServer!= NULL)
		{
			pColony= pServer->GetOneColony(pMonster->GetColonyID());
		}
		if (pColony!= NULL)
		{
			pColony->OnPeaceState(m_pOwner);
			
			 //point;//= pColony->GetAimPoint(pMonster->GetExID());
			if (pColony->HasRoad())
			{
				POINT point=pColony->GetAimPoint(pMonster->GetExID());
				if (point.x== 0 || point.y== 0)	return FALSE;
				LONG Distance= pMonster->Distance(point.x,point.y);
				if (Distance> pMonster->GetColonyDistance())
				{
					pMonster->SetPosition(point.x,point.y);
					Stand(pMonster->GetStopFrame());
				}
				else 
				{
					if (m_pOwner->GetTileX()== point.x && m_pOwner->GetTileY()== point.y)
					{
						Stand(STAND_ONCE_TIME);
					}
					else
					{
						CBaseAI::MoveTo(point.x, point.y);
					}
				}
			}
			else
			{
				POINT point=pColony->GetLeaderPoint();
				if (point.x== 0 || point.y== 0)	return FALSE;
				LONG Distance= pMonster->Distance(point.x,point.y);

				if (Distance> pMonster->GetColonyDistance())
				{
					POINT aimPoint;
					pServer->GetRandomPosInRange(aimPoint.x,aimPoint.y ,point.x,point.y,pMonster->GetColonyDistance(),pMonster->GetColonyDistance());
					pMonster->SetPosition(aimPoint.x,aimPoint.y);
					Stand(pMonster->GetStopFrame());
				}
				else
				{
					RandomRunOnColony();
				}
			}
			


			//else
			//{
			//	if (pColony->HasRoad() )//结构改变
			//	{
			//		if (m_pOwner->GetTileX()== point.x && m_pOwner->GetTileY()== point.y)
			//		{
 			//			Stand(STAND_ONCE_TIME);
			//		}
			//		else
			//		{
			//			CBaseAI::MoveTo(point.x, point.y);
			//		}
			//	}
			//	else
			//	{
			//		RandomRunOnColony(); 
			//	}
			//}
			return TRUE;

		}
	}
	return FALSE;
}

//战斗状态
BOOL CMonsterAI::OnFightState()
{
#ifdef _DUMP
	char filename[64];
	_snprintf(filename, 64, "AI_%d",GetAIType());
	PutStringToFile(filename,"fight_state");
#endif

	if (OnFightStateOnColony()==TRUE)
	{
		return TRUE;
	}

	CMoveShape* pShape = GetTarget();

	if (pShape!=NULL)
	{
		int TraceResult=Tracing(pShape);

		if(TraceResult==TRACE_RESULT_SUCCESS)     //追寻
		{
			m_pOwner->InitSkillInstance(m_dwChoiceSkillID,m_dwChoiceSkillLevel,0,0,0,NULL_GUID,5);
			BeginSkill(m_pOwner->GetInstanceSkill(),
				pShape->GetTileX(),pShape->GetTileY(),pShape);
			RecordUsedCurrentSkill();
			CleanChoiceSkillRecord();
		}
		else
		{
			if (GetChoiceSkillType()!= SKILLTYPE_CYCLE)
			{
				CleanChoiceSkillRecord();
			}
		}
		return true;
	}
	else
	{
		Stand(dynamic_cast<CMonster*>(m_pOwner)->GetStopFrame());//return CBaseAI::RandomRun();
		LoseTarget();
		return true;
	}

	#ifdef _DUMP
			char text[64];
			_snprintf(text, 64, "OnFightState:hasTarget_%d;fightstate_%d",HasTarget(),m_aesAIState);
			PutStringToFile("ai_Exceptional",text);
	#endif


}

BOOL CMonsterAI::OnFightStateOnColony()
{
	if (HasTarget()==TRUE)	return FALSE;

	CMonster* pMonster= dynamic_cast<CMonster*>(m_pOwner);

	if (pMonster!= NULL && pMonster->HasColony()==TRUE && pMonster->GetColonyState()==TRUE)
	{
		CServerRegion* pServer= dynamic_cast<CServerRegion*>(pMonster->GetFather());
		CColony* pColony=0;
		if (pServer== NULL)	return FALSE;

		pColony= pServer->GetOneColony(pMonster->GetColonyID());

		if (pColony== NULL)	return FALSE;

		//pColony->OnFightState(m_pOwner);

		if (m_aesAIState!= AES_FIGHT_TRANSITION)
		{
			CMoveShape* pFriend= SearchFriendOnColony();
			if (pFriend!= NULL)
			{
				if (FlyToFriendOnColony(pFriend)==TRUE)
				{
					//Stand(STAND_ONCE_TIME);
				}
				else
				{
					CBaseAI::MoveTo(pFriend->GetTileX(),pFriend->GetTileY());
				}
				return TRUE;
			}
		}
		
	}
	return FALSE;
}

BOOL CMonsterAI::OnEscapeState()
{
	// 增加一个虚弱DEBUFF
	if( m_pOwner->GetStateParamByID( STATE_BASE_WEAKNESS, "Num" ) == 0 )
	{
		m_pOwner->AddNewState( m_pOwner, STATE_BASE_WEAKNESS, 1,10 );
	}

	return Escape();
}

BOOL CMonsterAI::OnNoHpState()
{
	m_pOwner->OnDied(); /* need m_bAttackedByGuard */
	m_bAttackedByGuard = false;
	ChangeStateTo(CShape::STATE_DIED);
	m_aesAIState=AES_FIGHT_NORMAL;

	if (m_pOwner->GetCollectionID()== 0)
	{
		StartDiedEvent(CGlobeSetup::GetSetup()->dwMonsterKeepDeadTime);//AddAIEvent(AET_DIED,);//具体处理可否采集。
	}
	else
	{
		StartDiedEvent(CGlobeSetup::GetSetup()->dwCanCollectMonsterKeepDeadTime);//AddAIEvent(AET_DIED,);
	}
	return TRUE;
}

BOOL CMonsterAI::OnDeadState()
{
	//m_pOwner->OnDelEvent();
	m_pOwner->AddDelEvent(0);
	return TRUE;
}

BOOL CMonsterAI::OnTransitionState()
{

	CMonster* pMonster= dynamic_cast<CMonster*>(m_pOwner);
	if (pMonster== NULL)	return FALSE;
	CServerRegion *pRegion= dynamic_cast<CServerRegion*>(m_pOwner->GetFather());
	if (pRegion== NULL)	return FALSE;
	CColony* pColony= dynamic_cast<CColony*>(pRegion->GetOneColony(pMonster->GetColonyID()));

	if (pColony!= NULL)
	{
		pColony->OnFightState(m_pOwner);
	}	

	if (timeGetTime() > m_dwTransitionTimeStamp+ CGlobeSetup::GetSetup()->dwMonsterTransitionTime)
	{	

		if (pColony!= NULL && pColony->HasTarget())
		{
			CMoveShape* pFriend= SearchFriendOnColony();
			if (pFriend!= NULL)
			{
				//FlyToFriendOnColony(pFriend);
				if (FlyToFriendOnColony(pFriend)==TRUE)
				{
					Stand(STAND_ONCE_TIME);
				}
				else
				{
					CBaseAI::MoveTo(pFriend->GetTileX(),pFriend->GetTileY());
				}

			}
			m_aesAIState= AES_FIGHT_NORMAL;
			m_dwGuardTimeStamp= timeGetTime()+pMonster->GetGuardTime();
		}
		else
		{
			ReturnPeace();
			Stand(0);
		}
	}
	else
	{
		Stand(STAND_ONCE_TIME);
	}
	return TRUE;
}

	//（重新）寻找敌人
BOOL CMonsterAI::SearchEnemy()
{
	if(!CanSearchEnemy())
		return false;


	if(m_pOwner==NULL)
		return false;

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner -> GetFather() );//寻找所在场景
	CMonster*	pOwner=dynamic_cast<CMonster*>(m_pOwner);

	if( pRegion == NULL ) return TRUE;

	vector<CPlayer*> vOut;
	pRegion -> FindAroundPlayer( m_pOwner, vOut );

	CMoveShape* pTarget = NULL;
	for( size_t i = 0; i < vOut.size(); i ++ )
	{
		CPlayer* pPlayer = vOut[i];
		if( pPlayer == NULL ) continue;
		if( pPlayer -> IsDied() ) continue;

		if ( !pPlayer->IsVisible(m_pOwner) )			// 隐形
		{
			continue;
		}

		if( AIUtilities::IsInGodState( pPlayer ) )	// 处于无敌BUF状态
		{
			continue;
		}

		if( pTarget )
		{
			if( m_pOwner -> Distance(pPlayer) <= pOwner -> GetAttribute(string("C_GuardRange")) &&//GetGuardRange() &&
				 m_pOwner->Distance(pPlayer) < m_pOwner->Distance(pTarget) &&
				 pPlayer->GetExID()!= m_pOwner->GetBefoolID())
			{
				pTarget = pPlayer;
			}
		}
		else
		{
			if( m_pOwner -> Distance(pPlayer) <= pOwner -> GetAttribute(string("C_GuardRange")) &&  pPlayer->GetExID()!= m_pOwner->GetBefoolID())
				
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

	return TRUE;
}

//随机搜索敌人
BOOL CMonsterAI::SearchEnemyRandom(int iDisMax,int iDisMin)
{
	//if(!CanSearchEnemy())
	//	return false;


	if(m_pOwner==NULL)
		return false;

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner -> GetFather() );//寻找所在场景
	CMonster*	pOwner=dynamic_cast<CMonster*>(m_pOwner);

	if( pRegion == NULL ) return TRUE;

	vector<CPlayer*> vOut;
	pRegion -> FindAroundPlayer( m_pOwner, vOut );

	//vector<CMonster*> vPets;
	//pRegion -> FindAroundPets( m_pOwner, vPets );

	//CMoveShape* pTarget = NULL;
	vector<CMoveShape*> vAim;

	for( size_t i = 0; i < vOut.size(); i ++ )
	{
		CPlayer* pPlayer = vOut[i];
		if( pPlayer == NULL ) continue;
		if( pPlayer -> IsDied() ) continue;

		if (!pPlayer->IsVisible(m_pOwner) )			// 隐形
		{
			continue;
		}

		long lDis= m_pOwner -> Distance(pPlayer);
		if (lDis<=iDisMax && lDis>=iDisMin && pPlayer->GetExID()!= m_pOwner->GetBefoolID()) 
		{
			vAim.push_back(pPlayer);
		}
	}

	CMoveShape* pTarget= vAim[random(vAim.size())];
	SetTarget(pTarget->GetType() , pTarget -> GetExID() );

	return TRUE;
}

BOOL CMonsterAI::CanSearchEnemy()
{
	if(m_pOwner && m_pOwner->GetForceAttack())
	{
		if (dynamic_cast<CMonster*>(m_pOwner)->GetSearchEnemyScript()!="0"
			&& dynamic_cast<CMonster*>(m_pOwner)->GetSearchEnemyScript()!="null")
		{
			SearchEnemyByScript();
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}
BOOL CMonsterAI::CheckSearchEnemy()
{
	// 如果处于归位周期，就不能索敌
	if( m_csCycleSpring.csSpringType == CS_LEVELFIGHT )
	{
		return FALSE;
	}

	//是否应该丢失目标
	if (m_aesAIState== AES_FIGHT_TRANSITION)
	{
		return FALSE;
	}
	if (m_pOwner->GetState()!= CShape::STATE_PEACE &&
		m_pOwner->GetState()!= CShape::STATE_FIGHT)
	{
		return FALSE;
	}
	if (HasTarget() )
	{
		CMoveShape* pShape = GetTarget();
		if( pShape == NULL || pShape -> IsDied() ||
			pShape -> IsAttackAble(m_pOwner) == false )
		{
			WhenLoseTarget();
		}
		else
		{
			LONG lDistance=m_pOwner->Distance(pShape);
			if ( lDistance> m_pOwner->GetTrackRange() )
			{
				WhenLoseTarget();
			}

			if ( !pShape->IsVisible(m_pOwner) )			// 隐形
			{
				WhenLoseTarget();
			}
		}
	}
	
	//集群寻怪
	if (SearchEnemyOnColony()== TRUE)
	{
		return TRUE;
	}

	
	//正常ai的
	if (m_pOwner->GetState()==CShape::STATE_PEACE ||
		(m_pOwner->GetState()== CShape::STATE_FIGHT && m_aesAIState!= AES_FIGHT_NOHP))
	{
		if (!m_pOwner->GetRandomMove())
		{
			RandomRun();
			return TRUE;
		}
		
		if (m_pOwner->GetState()== CShape::STATE_FIGHT && m_aesAIState!= AES_FIGHT_NOHP && m_csCycleSpring.csSpringType!= CS_NULL)
		{
			//空
		}
		else
		{
			if (m_pOwner->GetState()== CShape::STATE_FIGHT && m_aesAIState== AES_FIGHT_ESCAPE)
			{
				CMonsterAI::SearchEnemy();
			}
			else
			{
				SearchEnemy();
			}
		}	
	}

	if( HasTarget() && m_aesAIState != AES_FIGHT_ESCAPE )
	{
		EscapeSpring();
	}

	return FALSE;
}
//在集群中搜索敌人
BOOL CMonsterAI::SearchEnemyOnColony()
{
	CMonster* pOwner= dynamic_cast<CMonster*>(m_pOwner);
	if (pOwner== NULL ) return FALSE;
	CServerRegion* pRegion= dynamic_cast<CServerRegion*>(m_pOwner->GetFather());
	if (pRegion== NULL)	return FALSE;

	DWORD dwColonyID= pOwner->GetColonyID();
	CColony* pColony= pRegion->GetOneColony(dwColonyID);

	CMoveShape* pTarget=0;

	if (pColony!=NULL)
	{
		pColony->OnFightState(m_pOwner);
		if (pOwner->GetMemberType()==2)   //2 被定义为此怪的目标和实际领导的目标一致
		{
			CMonster* pLeader= pColony->GetLeader();
			pTarget= pColony->GetLeaderTarget();
			if (pLeader!= NULL && pTarget!= NULL)
			{
				if (GetTarget()!= pTarget)
				{
					SetTarget(pTarget->GetType(),pTarget->GetExID());
				}

				LONG lDistance= pOwner->Distance(pTarget);
				LONG lFriendDis=pOwner->Distance(pLeader);

				if( lDistance < pOwner->GetColonyDistance() || lFriendDis < pOwner->GetColonyDistance() )
				{
					return FALSE;
				}
				if (FlyToFriendOnColony(pLeader)==FALSE)
				{
					Stand(STAND_ONCE_TIME);
				}
				
				return TRUE;
			}
		}
		vector<CMoveShape*> vecMoveShape;
		pColony->GetTargetList(vecMoveShape);
		if (vecMoveShape.size()==0)	return FALSE;
		for (vector<CMoveShape*>::iterator it= vecMoveShape.begin();
			it!= vecMoveShape.end(); it++)
		{
			CMoveShape* pMoveShape= *it;
			if( pMoveShape == NULL ) continue;
			if( pMoveShape -> IsDied() ) continue;

			if (!pMoveShape->IsVisible(m_pOwner) )		// 隐形
			{
				continue;
			}

			if( pTarget )
			{
				if( m_pOwner -> Distance(pMoveShape) < pOwner -> GetTrackRange() &&
					m_pOwner->Distance(pMoveShape) < m_pOwner->Distance(pTarget) )
				{
					pTarget = pMoveShape;
				}
			}
			else
			{
					pTarget = pMoveShape;
			}
		}

		if( pTarget != NULL )
		{
			//##满足条件设置对象			
			SetTarget( pTarget -> GetType(), pTarget -> GetExID() );
			LONG lDistance=m_pOwner->Distance(pTarget);
			if ( lDistance> m_pOwner->GetTrackRange() )
			{
				WhenLoseTarget();
			}
		}
		return FALSE;
	}
	return FALSE;
}
//在集群中寻找最近的且有目标的成员
CMonster* CMonsterAI::SearchFriendOnColony()
{
	CMonster* pOwner= dynamic_cast<CMonster*>(m_pOwner);
	if (pOwner== NULL ) return FALSE;
	CServerRegion* pRegion= dynamic_cast<CServerRegion*>(m_pOwner->GetFather());
	if (pRegion== NULL)	return FALSE;

	DWORD dwColonyID= pOwner->GetColonyID();
	CColony* pColony= pRegion->GetOneColony(dwColonyID);

	CMonster* pTarget=0;


	if (pColony!=NULL)
	{
		vector<CMonster*> vecMonster;
		pColony->GetFightMemberList(vecMonster);
		for (vector<CMonster*>::iterator it= vecMonster.begin();
			it!= vecMonster.end(); it++)
		{
			CMonster* pMonster= *it;
			if (pMonster==NULL)	continue;
			if (pMonster->IsDied())	continue;
			if (pTarget== NULL)
			{
				pTarget= pMonster;
			}
			else
			{
				if (m_pOwner->Distance(pTarget)<m_pOwner->Distance(pMonster))
				{
					pTarget=pMonster;
				}
			}

			//if( pTarget== NULL )
			//{
			//	if (pMonster->GetAI()->HasTarget())
			//	{
			//		pTarget= pMonster;
			//	}
			//}
			//else
			//{
			//	if (pMonster->GetAI()->HasTarget() &&
			//		pOwner->Distance(pMonster)< pOwner->Distance(pTarget))
			//	{
			//		pTarget= pMonster;
			//	}
			//}
			
		}
	}
	return pTarget;

}
//用脚本寻找敌人
BOOL CMonsterAI::SearchEnemyByScript()
{return FALSE;}
	//寻找技能
BOOL CMonsterAI::SearchOneTypeSkill(DWORD dwSkillID,DWORD dwSkillLevel,DWORD dwType,LONG lDistance)
{

	CNewSkill *pSkillProperty = CNewSkillFactory::QueryNewSkillProperty(dwSkillID);
	if (pSkillProperty== NULL)
	{		
		return FALSE;
	}		
	if (dwType!= SKILLTYPE_CYCLE)
	{
		int maxDis=pSkillProperty->GetStaticParam(dwSkillLevel)->lMaxAtkDistance;
		int minDis=pSkillProperty->GetStaticParam(dwSkillLevel)->lMinAtkDistance;
		if ( lDistance>maxDis || lDistance<minDis)
		{
			return FALSE;
		}
	}

	if (!m_pOwner->GetUseRabbSkill() && pSkillProperty->GetUseType()==SKILL_TYPE_MAGIC)
	{
		return FALSE;
	}
	if (!m_pOwner->GetUseFightSkill() && pSkillProperty->GetUseType()==SKILL_TYPE_PHYSICAL)
	{
		return FALSE;
	}
	

	BOOL BufferCheck=TRUE;
	list<long> lBuffID = pSkillProperty->GetStaticParam(dwSkillLevel)->lBuffID;//必须有的状态
	for (list<long>::iterator it=lBuffID.begin();it!=lBuffID.end();++it)
	{

		map<LONG,stModuParam*>::iterator bt = m_pOwner->GetBuff().find(*it);
		if (bt == m_pOwner->GetBuff().end())
		{
			continue;
		}
		else
		{
			BufferCheck=FALSE;
			break;
		}
	}

	if (BufferCheck==FALSE)
	{
		return FALSE;
	}

	list<long> lBuffID_=pSkillProperty->GetStaticParam(dwSkillLevel)->lNBuffID;//必须没有的状态
	for (list<long>::iterator it=lBuffID_.begin();it!=lBuffID_.end();it++)
	{
		map<LONG,stModuParam*>::iterator bt = m_pOwner->GetBuff().find(*it);
		if (bt != m_pOwner->GetBuff().end())
		{
			continue;
		}
		else
		{
			BufferCheck=FALSE;
			break;
		}
	}

	if (BufferCheck==FALSE)
	{
		return FALSE;
	}

	return TRUE;
}
BOOL CMonsterAI::SearchSkill(CMoveShape* pTarget)//
{
	//m_pOwner->SetInstanceSkill(NULL);

	if(!CBaseAI::SearchSkill(pTarget))		//看是否应该继续寻找技能
		return FALSE;
	if (HasChoiceSkill())
		return FALSE;

	BOOL bMustUsed=FALSE;

	if (!m_pOwner ) //如果owner为空， 或则已经有技能，则返回
	{
		return FALSE;
	}
	if (!m_pOwner->GetUseFightSkill())
	{
		return FALSE;
	}
	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	LONG lDistance = m_pOwner ->RealDistance( pTarget );

	if( lDistance < 0 )
	{
		lDistance = 0;
	}
	int skill_min=pOwner->GetAckRangeMin();
	int skill_max=pOwner->GetAckRangeMax();
	int iProportion=pOwner->GetSkillProportion();

	if (lDistance<skill_min || lDistance> skill_max)
	{
		return TRUE;
	}
	LONG thisTime=timeGetTime();
	//
	struct SkillChoicRecord
	{
		DWORD SkillID;
		DWORD SkillLevel;
		WORD SkillType;
		WORD SkillIndex;
	};
	vector<SkillChoicRecord> SearchedSkill;


	//选择周期触发技能
	vector<CMonster::tagSkillRecord>& cycleskillmap=pOwner->GetCycleSkillList();
	if (cycleskillmap.size()!=0)
	{
		for (int a=0; a<cycleskillmap.size(); a++)
		{
			if (cycleskillmap[a].dwCoolDownTime> thisTime) continue;

			if (!SearchOneTypeSkill(cycleskillmap[a].wID,cycleskillmap[a].wLevel,SKILLTYPE_CYCLE,lDistance))
			{
				continue;
			}

			SkillChoicRecord record;
			record.SkillType = SKILLTYPE_CYCLE;
			record.SkillIndex = a;
			record.SkillID= cycleskillmap[a].wID;
			record.SkillLevel= cycleskillmap[a].wLevel;
			SearchedSkill.push_back(record);
		}
	}


	//选择普通技能
	if( SearchedSkill.size() == 0 )
	{
		if( thisTime > m_dwSkillSpaceTimeStamp )
		{
			m_dwSkillSpaceTimeStamp = thisTime + pOwner->GetSkillSpace();
			if( random( 100 ) < iProportion )
			{
				vector<CMonster::tagSkillRecord>& skillmap=pOwner->GetSkillList();
				if (skillmap.size()!=0)
				{
					for (int a=0; a<skillmap.size(); a++)
					{	
						if (skillmap[a].dwCoolDownTime> thisTime) continue;

						if (!SearchOneTypeSkill(skillmap[a].wID,skillmap[a].wLevel,SKILLTYPE_NORMAL,lDistance))
						{
							continue;
						}

						SkillChoicRecord record;
						record.SkillType = SKILLTYPE_NORMAL;
						record.SkillIndex =a;
						record.SkillID= skillmap[a].wID;
						record.SkillLevel= skillmap[a].wLevel;
						SearchedSkill.push_back(record);
					}
				}
			}
		}
	}

	//选择基本技能
	if (SearchedSkill.size()==0 && iProportion!=100)
	{
		vector<CMonster::tagSkillRecord>& skillmap=pOwner->GetBaseSkillList();
		if (skillmap.size()!= 0)
		{
			for (int a=0; a<skillmap.size(); a++)
			{
				if (skillmap[a].dwCoolDownTime> thisTime) continue;
				if (!SearchOneTypeSkill(skillmap[a].wID,skillmap[a].wLevel,SKILLTYPE_BASE,lDistance))
				{
					continue;
				}

				SkillChoicRecord record;
				record.SkillType = SKILLTYPE_BASE;
				record.SkillIndex =a;
				record.SkillID= skillmap[a].wID;
				record.SkillLevel= skillmap[a].wLevel;
				SearchedSkill.push_back(record);
			}
		}
	}


	if (SearchedSkill.size()!=0)
	{
		int random_num=random(SearchedSkill.size());
		SkillChoicRecord& record=SearchedSkill[random_num];
		RecordChoiceSkill(record.SkillID,record.SkillLevel,record.SkillType,record.SkillIndex);
	}
	else
	{
		m_pOwner->SetInstanceSkill(NULL);
	}

	return TRUE;

}

	//逃跑
BOOL CMonsterAI::Escape()
{
	CMoveShape* pTarget = GetTarget();

	if (pTarget!=NULL)
	{
		LONG lX		= pTarget ->GetTileX();
		LONG lY		= pTarget ->GetTileY();
		Escape(dynamic_cast<CRegion*>(m_pOwner->GetFather()),lX,lY,FALSE);
		return true;
	}
	else
	{
		// 移除虚弱状态，恢复正常
		m_pOwner->DelNewState( STATE_BASE_WEAKNESS, 1 );

		Stand(dynamic_cast<CMonster*>(m_pOwner)->GetStopFrame());
		WhenLoseTarget();
		return TRUE;
	}
}

VOID CMonsterAI::Escape(CRegion* pRegion, LONG lX_, LONG lY_, BOOL bRun )
{
	LONG lX = m_pOwner -> GetTileX();
	LONG lY = m_pOwner -> GetTileY();

	LONG lDir=GetLineDir(lX_,lY_,lX,lY);
	//m_pOwner -> GetDirPos( lDir, lX, lY );

	MoveTo(lDir);
}

VOID CMonsterAI::Escape(CRegion* pRegion, LONG lDir, BOOL bRun)
{
	LONG lDirect=(lDir+4)%8;
	MoveTo(lDirect);
}

	//追击,由于没做 目标 和 所有者 的判断，所以需要在调用此函数前先做好判断。
INT CMonsterAI::Tracing(CMoveShape* pTarget)
{
	SearchSkill(pTarget);
	BOOL HasSkillConditon= FALSE;
	BOOL HasSkillToAttac= TRUE;

	INT MinAtkDistance=0;
	INT MaxAtkDistance=0;

	CNewSkill *pSkillProperty = CNewSkillFactory::QueryNewSkillProperty(m_dwChoiceSkillID);
	if (pSkillProperty== NULL)
	{	
		vector<CMonster::tagSkillRecord>& skillmap=dynamic_cast<CMonster*>(m_pOwner)->GetBaseSkillList();
		if (skillmap.size()>0)
		{
			pSkillProperty = CNewSkillFactory::QueryNewSkillProperty(skillmap[0].wID);
		}
		HasSkillToAttac= FALSE;
		CleanChoiceSkillRecord();
	}	
	if (pSkillProperty!=NULL)
	{
		
		if (HasSkillToAttac)
		{
			MaxAtkDistance=pSkillProperty->GetStaticParam(m_dwChoiceSkillLevel)->lMaxAtkDistance;
			MinAtkDistance=pSkillProperty->GetStaticParam(m_dwChoiceSkillLevel)->lMinAtkDistance;
		}
		else
		{
			vector<CMonster::tagSkillRecord>& skillmap=dynamic_cast<CMonster*>(m_pOwner)->GetBaseSkillList();
			if (skillmap.size()>0)
			{
				MaxAtkDistance=pSkillProperty->GetStaticParam(skillmap[0].wLevel)->lMaxAtkDistance;
				MinAtkDistance=pSkillProperty->GetStaticParam(skillmap[0].wLevel)->lMinAtkDistance;
			}
		}

		HasSkillConditon =TRUE;
	}
	if (HasSkillConditon==FALSE)
	{
		RandomRun();
		return TRACE_RESULT_TRACE;
	}

	//##合乎距离要求。
	LONG lDistance = m_pOwner -> RealDistance( pTarget );
	//LONG lDistance= Distance(m_pOwner->GetTileX(),m_pOwner->GetTileY(),pTarget->GetTileX(),pTarget->GetTileY());
	if (lDistance<0) lDistance=0;

	//如果在技能的距离内， 则使用技能
	if(lDistance >= MinAtkDistance && lDistance <= MaxAtkDistance )
	{
		if (HasSkillToAttac==TRUE )
		{
			return TRACE_RESULT_SUCCESS;
		}
		else
		{
			Stand(dynamic_cast<CMonster*>(m_pOwner)->GetStopFrame());
			return TRACE_RESULT_TRACE;
		}
		
	}
	//##如果依然在追击范围内，就开始跟随玩家。
	else if(m_pOwner -> GetTrackRange() >=  lDistance )
	{
		//##如果是在最大攻击距离范围外,向玩家靠近
		if( lDistance > MaxAtkDistance)
		{
			CBaseAI::MoveTo(pTarget->GetTileX(),pTarget -> GetTileY());
		}
		//##否则（在最小攻击距离之内）反方向移开
		else
		{
			LONG lX		= m_pOwner -> GetTileX();
			LONG lY		= m_pOwner -> GetTileY();

			LONG lDir	= GetLineDir( 
				pTarget -> GetTileX(), pTarget -> GetTileY(), lX, lY );

			//m_pOwner -> GetDirPos( lDir, lX, lY );

			MoveTo( lDir);
		}

		return TRACE_RESULT_TRACE;
	}
	//##玩家超出范围。继续追一步。返回丢失目标
	else
	{
		CBaseAI::MoveTo(pTarget -> GetTileX(), pTarget -> GetTileY());
		WhenLoseTarget();
		return TRACE_RESULT_OUT;
	}
}

//判断该格子是否能移动
BOOL CMonsterAI::IsCanMoveTo(CRegion* pRegion,LONG lX, LONG lY)
{
	if(NULL == pRegion)	return FALSE;
	CMonster *pMonster = dynamic_cast<CMonster*>(m_pOwner);
	//判断该monster是否有阻挡
	if(!pMonster)	return FALSE;
	//有阻挡
	BYTE byBlock = pRegion->GetBlock(lX,lY);
	if (byBlock !=CRegion::BLOCK_NO && byBlock!= CRegion::BLOCK_AIM)	return FALSE;

	BYTE byBlockType= pMonster->GetBlockType();	
	if(byBlockType==CRegion::BLOCK_AIM)//如果怪物不能踩怪物阻挡
	{
		if ( byBlock!= CRegion::BLOCK_NO)
			return FALSE;
	}

	vector<CRegion::tagSwitch> &vecSwitch = pRegion->GetSwitchVec();
	WORD SwitchIndex = pRegion->GetCell( lX, lY )->lSwitch;
	if( SwitchIndex != 0 && SwitchIndex < vecSwitch.size() && vecSwitch[SwitchIndex].lState != CRegion::CPT_SCRIPT ) //是切换点但不是脚本触发点
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMonsterAI::MoveTo(long lDir)
{
	//##条件检查
	if( !m_pOwner ) return FALSE;
	//*
	if( m_pOwner -> CanMove() == FALSE )
	{
		AddAIEvent(AET_MOVE,MOVE_FAIL_APACE_TIME);
		return FALSE;
	}

	//*/
	//##移动测试前的准备，获取当前的坐标，需要移动的方向
	LONG lPosX		= m_pOwner -> GetTileX();
	LONG lPosY		= m_pOwner -> GetTileY();
	LONG lNewX		= lPosX;
	LONG lNewY		= lPosY;

	float fDis = 1.0f;
	BOOL bMoveable	= GetNextMoveDestCell(lDir,fDis,lNewX,lNewY);
	//##可以行走
	if( bMoveable )
	{
		//##保存开始的位置信息
		//m_lLastPositionX = lPosX;
		//m_lLastPositionY = lPosY;
		if( GetLineDir(lPosX, lPosY, lNewX, lNewY) % 2 != 0 )
		{
			fDis *=1.414f;
		}
		float	fTime	= fDis/m_pOwner->GetSpeed();
		if(m_pOwner->GetIsRun())
			fTime /= 2.00f;

		//##移动Shape到指定位置。
		OnMove((float)lNewX+0.5f,(float)lNewY+0.5f,(DWORD)fTime);
		//设置位置值
		m_pOwner->SetPosXY(lNewX+0.5f,lNewY+0.5f);		
	}
	else
	{
		//m_pOwner -> OnCannotMove();
		AddAIEvent(AET_MOVE,MOVE_FAIL_APACE_TIME);
	}
	return TRUE;

}

//脚本触发：周期触发，战斗触发，重伤触发，死亡触发
DWORD CMonsterAI::CycleSpring()
{	
	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	if (pOwner==NULL)return 0;
	return ScriptSpring(pOwner,pOwner->GetCycleScript());
}
DWORD CMonsterAI::BornSpring()
{
	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	if (pOwner==NULL)return 0;
	return ScriptSpring(pOwner,pOwner->GetBornScriptName());
}
DWORD CMonsterAI::FightSpring()
{
	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	if (pOwner==NULL)return 0;
	return ScriptSpring(pOwner,pOwner->GetFightScriptName());
}
DWORD CMonsterAI::GBHSpring(int hurt)//重伤触发
{
	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	if (pOwner==NULL)return 0;

	int iHurtProportionHp= pOwner->GetMaxHP()* pOwner->GetHurtProportion()/100;

	int Hp= pOwner->GetHP();
	int LastHp= Hp+ hurt;

	if (iHurtProportionHp<LastHp && iHurtProportionHp>= Hp)
	{
		return ScriptSpring(pOwner,pOwner->GetGBHScriptName());
	}
	return 0;
	
}

DWORD CMonsterAI::DeadSpring()//死亡触发
{
	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	if (pOwner==NULL)return 0;
	return ScriptSpring(pOwner,pOwner->GetDeadScriptName());
}

DWORD CMonsterAI::FindTargetSpring()
{
	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	if (pOwner==NULL)return 0;
	return ScriptSpring(pOwner,pOwner->GetFindTargetScript());
}

DWORD CMonsterAI::EscapeSpring()
{
	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	if (pOwner->GetHP()*100.0f/pOwner->GetMaxHP() <= pOwner->GetEscapePoint())
	{
		m_aesAIState=AES_FIGHT_ESCAPE;
	}
	return 0;
}
DWORD CMonsterAI::ScriptSpring(CMonster* pMonster, const string& str)
{
	if (pMonster== NULL) return 0;
	if (str=="0" || str=="null")return 0;

	stRunScript st;
	st.pszFileName   = (char*)(str.c_str());
	st.srcShape  = pMonster;
	st.desShape	 = pMonster;
	st.pRegion   = dynamic_cast<CRegion*>(m_pOwner->GetFather());
	RunScript(&st,(char*)GetGame()->GetScriptFileData(st.pszFileName));	
	return 0;
}
VOID CMonsterAI::ChangeStateTo(INT state)
{
	if (m_pOwner->GetState()==state)return;

	if (m_pOwner->IsDied())return;

	if (state==CShape::STATE_PEACE)
	{
		if (m_pOwner->GetType()==TYPE_MONSTER)
		{
			m_pOwner->SetSpeed(dynamic_cast<CMonster*>(m_pOwner)->GetMoveSpeed());
			m_pOwner->SendChangedSpeedToClients();
		}

		m_pOwner->ChangeStateTo((CShape::eSTATE)state);
		m_aesAIState=AES_FIGHT_NORMAL;
		CleanCycleSpring();

	}
	else if (state==CShape::STATE_FIGHT)
	{
		if (m_pOwner->GetType()==TYPE_MONSTER &&m_pOwner->GetWeaknessState()==true)
		{
			m_pOwner->SetSpeed(dynamic_cast<CMonster*>(m_pOwner)->GetFightMoveSpeed());
			m_pOwner->SendChangedSpeedToClients();
		}
		if( GetAIType() == CAIFactory::GUARD )
		{
			// 对于守卫而言最好设置战斗点为出生点
			m_EnterFightPoing = m_BornPoint;
		}
		else
		{
			m_EnterFightPoing.x=m_pOwner->GetTileX();
			m_EnterFightPoing.y=m_pOwner->GetTileY();
		}
		m_pOwner->ChangeStateTo((CShape::eSTATE)state);
		m_aesAIState=AES_FIGHT_NORMAL;
		CleanCycleSpring();
		FightSpring();
	}
	else if (state==CShape::STATE_DIED || state==CShape::STATE_HANGUP)
	{
		m_pOwner->ChangeStateTo((CShape::eSTATE)state);
		m_aesAIState=AES_FIGHT_NORMAL;
		CleanCycleSpring();
	}
}
//回到和平状态
BOOL CMonsterAI::ReturnPeace()
{
	//  [11/11/2009 chenxianj]
	//产生归位事件
	m_pOwner->ProduceEvent(ET_Homing);

	m_bAttackedByGuard = false;
	m_pOwner->CleanBeneficiary();
	static_cast<CMonster*>( m_pOwner )->CleanHpBar();
	LoseTarget();
	ChangeStateTo(CShape::STATE_PEACE);
	ResumeAll();
	OnBoring(); // 恢复和平状态时，按照重生处理11.21.2008
	FlyToRecordePoint();
	
	// 更新部分属性
	m_pOwner->AddAttributeToDirty( string( "Hp" ) );
	m_pOwner->UpdateAttribute( true );

	return TRUE;
}

BOOL CMonsterAI::RunByRoad()
{
	if(m_pOwner==NULL)
		return FALSE;
	if(m_vectRoad.size()==0)
		return FALSE;

	if (m_LeaveRoadPoint.x!=m_pOwner->GetTileX() || m_LeaveRoadPoint.y!= m_pOwner->GetTileY())
	{
		CBaseAI::MoveTo(m_LeaveRoadPoint.x,m_LeaveRoadPoint.y);
		return TRUE;
	}

	/*if (m_vectRoad[0].x!=m_pOwner->GetTileX() || m_vectRoad[0].y!=m_pOwner->GetTileY())
	{
		CBaseAI::MoveTo(m_vectRoad[0].x,m_vectRoad[0].y);
		m_LeaveRoadPoint.x		= m_pOwner -> GetTileX();
		m_LeaveRoadPoint.y		= m_pOwner -> GetTileY();
		return TRUE;
	}*/

	if (m_vectRoad.size()==1)
	{
		Stand(STAND_ONCE_TIME);
		return TRUE;
	}

	LONG lX		= m_pOwner -> GetTileX();
	LONG lY		= m_pOwner -> GetTileY();
	INT CurrentPointIndex=m_iRoadRecord;

	INT  iGoStep=0;
	for(int a=0;a<3;a++)
	{
		iGoStep++;
		LONG x=lX;
		LONG y=lY;
		CountNextStepByRoad(lX,lY,CurrentPointIndex);
		BOOL bCanrun=FALSE;

		if (iGoStep==1)
		{
			LONG lDir=GetLineDir(x,y,lX,lY);
			bCanrun=MoveToCell(lDir,x,y);
		}
		else
		{
			bCanrun=MoveToCell(lX,lY);
		}

		if(bCanrun)
		{
			m_iRoadRecord=CurrentPointIndex;

			if(iGoStep==1)
			{
				CBaseAI::MoveTo(lX,lY);
				m_LeaveRoadPoint.x		= m_pOwner -> GetTileX();
				m_LeaveRoadPoint.y		= m_pOwner -> GetTileY();
				return true;
			}
			else
			{
				m_pOwner->SetPosition(lX,lY);
				//添加移动的定时器
				AddAIEvent(AET_MOVE, 2000);
				m_LeaveRoadPoint.x		= m_pOwner -> GetTileX();
				m_LeaveRoadPoint.y		= m_pOwner -> GetTileY();
				return true;
			}
		}
	}
	return false;
}


BOOL CMonsterAI::RandomRun()
{
	//自由走动部分放到和平周期处理

	CMonster* pOwner = dynamic_cast<CMonster*>( GetOwner() );
	if (pOwner== NULL)	return FALSE;
	if( static_cast<DWORD>( random(10000) ) < pOwner->GetMoveRandomValue() )
	{
		LONG lDir	=0;
		int distance=pOwner->Distance(pOwner->GetTileX(),pOwner->GetTileY(),m_BornPoint.x,m_BornPoint.y);
		if (distance> pOwner->GetPeaceMoveRange())
		{
			LONG dir = GetLineDir( pOwner->GetTileX(),pOwner->GetTileY(), m_BornPoint.x, m_BornPoint.y );
			int a=(lDir-dir+8)%8;
			if (a>=2 || a<=6)
			{
				lDir=(dir+random(3)-1+8)%8;
			}
		}
		else
		{
			lDir	= random( 8 );
		}

		INT MaxRunTimes=pOwner->GetMaxRunTimes();
		INT MinRunTimes=pOwner->GetMinRunTimes();

		MoveTo(lDir);
			
		MoveByStepOnPeace(lDir,random(MaxRunTimes-MinRunTimes)+MinRunTimes);
		return true;
	}
	else
	{
		Stand(pOwner->GetStopFrame());
		Stand(random(pOwner->GetMaxStopTimes() - pOwner->GetMinStopTimes())+pOwner->GetMinStopTimes() ,pOwner->GetStopFrame());
	}

	return true;
}

BOOL CMonsterAI::RandomRunOnColony()
{
	return RandomRun();
}
//随机状态处理
BOOL CMonsterAI::CheckRandomRunState()
{
	if (m_pOwner->GetRandomMove())	return FALSE;
	LONG dir= random(8);
	MoveTo(dir);
	return TRUE;
}

BOOL CMonsterAI::OnPeaceIdle()
{
	return RandomRun();
}

BOOL CMonsterAI::CheckReturnPeace()
{

	if (m_aesAIState== AES_FIGHT_TRANSITION)
	{
		return FALSE;
	}

	int dis = m_pOwner->Distance(m_EnterFightPoing.x , m_EnterFightPoing.y , m_pOwner->GetTileX(),m_pOwner->GetTileY() );


	if ( dis > dynamic_cast<CMonster*>(m_pOwner)->GetReturnRange())
	{
		if ((HasTarget()==FALSE && timeGetTime()>m_dwGuardTimeStamp)||timeGetTime()> m_dwHurtedTimeStamp)
		{
			if (HasTarget())
			{
				WhenLoseTarget();
			}
			m_aesAIState= AES_FIGHT_TRANSITION;
			m_dwTransitionTimeStamp= timeGetTime();
			return FALSE;
		}
	}
	else
	{
		if (HasTarget()==FALSE && timeGetTime()>m_dwGuardTimeStamp)
		{
			m_aesAIState= AES_FIGHT_TRANSITION;
			m_dwTransitionTimeStamp= timeGetTime();
			return FALSE;
		}
	}

	return FALSE;
}

//记录出生点
VOID CMonsterAI::RecordBornPoint(int x, int y)
{
	m_BornPoint.x=x;
	m_BornPoint.y=y;
	m_LeaveRoadPoint.x=x;
	m_LeaveRoadPoint.y=y;
	m_EnterFightPoing.x= x;
	m_EnterFightPoing.y= y;
}
//飞回到和平记录点，或则是路径点
VOID CMonsterAI::FlyToRecordePoint()
{
	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	if (pOwner==NULL)	return;

	//判断是否应该飞回去
	if (pOwner->GetIsFlyToPeace()==0) return;

	if (pOwner->GetIsFlyToPeace()==1)
	{
		POINT point;
		point.x=0;
		point.y =0;
		if (pOwner->HasColony() )//&& pOwner->GetColonyState()!= CMonster::COLONY_STATE_NOLEADER)
		{
			CServerRegion* pRegion= dynamic_cast<CServerRegion*>(pOwner->GetFather());
			CColony* pColony= 0;
			if (pRegion!= NULL)
			{
				pColony= pRegion->GetOneColony(pOwner->GetColonyID());
				if (pColony!= NULL && pColony->HasRoad()== TRUE)
				{
					point= pColony->GetAimPoint(pOwner->GetExID());
					//if (point.x!=0 || point.y!=0)
					//{
					//	m_pOwner->SetPosition(point.x,point.y);
					//	return;
					//}
				}
			}
		}
		if (point.x==0 && point.y==0)
		{		
			if (m_vectRoad.size()==0)
			{
				point= m_EnterFightPoing;//m_pOwner->SetPosition(m_EnterFightPoing.x,m_EnterFightPoing.y);
			}
			else
			{
				point= m_LeaveRoadPoint;//m_pOwner->SetPosition(m_LeaveRoadPoint.x,m_LeaveRoadPoint.y);
			}
		}

		LONG dis= pOwner->Distance(point.x,point.y);
		DWORD time= dis/CGlobeSetup::GetSetup()->fReturnPeaceSpeed;
		if (time> CGlobeSetup::GetSetup()->dwReturnPeaceEffectTime)
		{
			time= CGlobeSetup::GetSetup()->dwReturnPeaceEffectTime;
		}
		pOwner->SendLeaveFightMessage(point.x,point.y,time);
		LevelFight(time,point.x,point.y);
		//设置无敌
	}
	else
	{
		ChangeStateTo(CShape::STATE_DIED);
	}

	//m_bIsLeaveRoad=FALSE;
}
//飞到最近的朋友周围
BOOL CMonsterAI::FlyToFriendOnColony(CMoveShape* pFriend)
{
	//= SearchFriendOnColony();
	CMoveShape* pAim=0;
	if (pFriend!= NULL && pFriend->GetAI()!= NULL)
	{
		pAim= dynamic_cast<CMoveShape*>(pFriend->GetAI()->GetTarget());
	}

	if (pAim== NULL)	return FALSE;
	LONG distance= m_pOwner->Distance(pAim);
	LONG frienddis=m_pOwner->Distance(pFriend);
	//if (distance< 6 || frienddis<6)
	//{
	//	CBaseAI::MoveTo(pFriend->GetTileX(),pFriend->GetTileY());
	//	m_dwHurtedTimeStamp=timeGetTime()+dynamic_cast<CMonster*>(m_pOwner)->GetChaseTime();
	//	return FALSE;
	//}
	CMonster *pOwner = static_cast<CMonster*>( m_pOwner );
	DWORD ColDis = pOwner->GetColonyDistance();
	if (distance<ColDis+6 && distance >= ColDis)
	{
		CBaseAI::MoveTo(pAim->GetTileX(),pAim->GetTileY());
		return TRUE;
	}
	else if (frienddis<ColDis+6 && frienddis >= ColDis)
	{
		CBaseAI::MoveTo(pFriend->GetTileX(),pFriend->GetTileY());
		return TRUE;
	}
	else 
	{
		LONG x,y,dir;
		x= pFriend->GetTileX();
		y= pFriend->GetTileY();
		dir= GetLineDir(x,y,pAim->GetTileX(),pAim->GetTileY());

		if (GetNearPosition(x,y,dir,dir))
		{
			m_pOwner->SetPosition(x,y);
			m_pOwner->SetDir(dir);
			m_dwHurtedTimeStamp=timeGetTime()+dynamic_cast<CMonster*>(m_pOwner)->GetChaseTime();
		}
		return FALSE;
	}

	return FALSE;


}
//完全恢复，包括ai也调整回以前的状态
VOID CMonsterAI::ResumeAll( bool bForce )
{
	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	if ( !bForce && pOwner->GetIsResumeAll() ==0 )
	{
		return ;
	}
	pOwner->ResumeAll();
}
//进入战斗
void CMonsterAI::EnterFight(){ChangeStateTo(CShape::STATE_FIGHT);}

//使用技能
BOOL CMonsterAI::BeginSkill(stModuParam *pSkill, LONG lx, LONG ly,CMoveShape* pTarget)
{

	if (CBaseAI::BeginSkill(pSkill, lx, ly, pTarget))
	{
		return TRUE;
	}
	else
	{
		Stand(dynamic_cast<CMonster*>(m_pOwner)->GetStopFrame());
		return FALSE;
	}
}
	//当被攻击的时候(最基本类型）
BOOL CMonsterAI::WhenBeenHurted(LONG lType,const CGUID& guid, DWORD hurt)
{
	if (m_pOwner->IsGod())	return TRUE;
	m_dwHurtedTimeStamp=timeGetTime()+dynamic_cast<CMonster*>(m_pOwner)->GetChaseTime();
	if(CBaseAI::WhenBeenHurted (lType,guid,hurt))//如果是强制目标时间
	{
		CServerRegion *pRegion = static_cast<CServerRegion*>(m_pOwner->GetFather());
		if (pRegion)
		{
			CMoveShape* pTarget = static_cast<CMoveShape*>(pRegion->FindChildObject(lType, guid));
			// 标志是否被守卫攻击过	
			if( !m_bAttackedByGuard && pTarget != NULL && pTarget->GetType() == TYPE_MONSTER )
			{
				CMonster *pMonster = static_cast<CMonster*>( pTarget );
				m_bAttackedByGuard = ( pMonster->GetNpcType() == NT_Guard );
			}
			if( pTarget != NULL && pTarget->IsGod() )
			{
				CMoveShape* pHost = pTarget->GetHost();
				if( pHost != NULL )
				{
					CBaseAI::SetTarget(pHost->GetType(), pHost->GetExID());
				}
			}
			else
			{
				CBaseAI::SetTarget(lType, guid);
			}
		}		
	}
	// 多血条脚本触发处理
	static_cast<CMonster*>( m_pOwner )->OnHpChanged( m_pOwner->GetHP(), hurt );
	GBHSpring(hurt);
	
	return TRUE;
}

	//被杀害的时候(注销周期和主动事件，死亡触发
BOOL CMonsterAI::WhenBeenKilled(LONG lType,const CGUID& guid)
{
	CleanCycleSpring();
	m_aesAIState=AES_FIGHT_NOHP;
	DeadSpring();
	LogoutEvent(m_aeActive);
	LogoutEvent(m_aeCycle);
	OnActive();
	LoseTarget();
	CMonster* pMonster= dynamic_cast<CMonster*>(m_pOwner);
	CServerRegion* pRegion= dynamic_cast<CServerRegion*>(m_pOwner->GetFather());
	if (pMonster !=NULL && pMonster->GetColonyID()!= 0 && pRegion!= NULL)
	{
		CColony* pColony= pRegion->GetOneColony(pMonster->GetColonyID());
		if (pColony!= NULL)
		{
			pColony->WhenBeenKilled(m_pOwner);
		}
	}

	return FALSE;
}

BOOL CMonsterAI::WhenLoseTarget()
{
	if (CBaseAI::WhenLoseTarget())
	{
		CMonster* pOwner= dynamic_cast<CMonster*>(m_pOwner);
 		m_dwGuardTimeStamp	= timeGetTime() +pOwner->GetGuardTime();
		pOwner->SetForceAttack(TRUE);
		return TRUE;
	}
	return FALSE;
}
BOOL CMonsterAI::WhenBeenHurtedOnColony(LONG lType,const CGUID& guid, DWORD hunrt)
{
	CMonster * pOwner= dynamic_cast<CMonster*>(m_pOwner);
	if (pOwner== NULL || pOwner->HasColony()== FALSE)
	{
		return FALSE;
	}
	CServerRegion* pRegion= dynamic_cast<CServerRegion*>(m_pOwner->GetFather());
	if (pRegion== NULL) return FALSE;
	CColony* pColony= pRegion->GetOneColony(pOwner->GetColonyID());
	if (pColony!= NULL)
	{
		//pColony->AddTarget(lType,guid);
	}
	return TRUE;
}
//加入路径
VOID CMonsterAI::AddRoad(vector<POINT>& vectRoad)
{
  	if (vectRoad.size() > 0 && m_vectRoad.size()==0)
	{
		m_vectRoad=vectRoad;
		m_iRoadRecord=0;				//行走点记录
		m_iRoadDirect=1;              //行走方向
	}

}

	//计算出下一步要走的位置，并返回是进入下个记录段或则是停留或是。。
INT  CMonsterAI::CountNextStepByRoad(LONG &lX, LONG &lY,INT & CurrentIndex)
{
	int countresult=0;
	int NextPointIndex=CurrentIndex + m_iRoadDirect;

	if ( NextPointIndex < 0 || NextPointIndex >= m_vectRoad.size())
	{
		//assert(1);
		PutDebugString("路径出错。原因： 数组越界");
		return NEXTSTEP_RESULT_NULL;
	}
	
	//如果当前坐标已经到了下一个记录
	if(lX==m_vectRoad[NextPointIndex].x && lY==m_vectRoad[NextPointIndex].y)
	{
		//如果这个记录点是首尾点
		if(NextPointIndex==m_vectRoad.size()-1 || NextPointIndex==0)		
		{
			//如果就路径是首尾相连的
			if(m_vectRoad[NextPointIndex].x==m_vectRoad[(NextPointIndex+m_iRoadDirect+m_vectRoad.size())% m_vectRoad.size()].x
				&& m_vectRoad[NextPointIndex].y==m_vectRoad[(NextPointIndex+m_iRoadDirect+m_vectRoad.size())% m_vectRoad.size()].y)
			{
				countresult=NEXTSTEP_RESULT_NEXTCYCLE;
				CurrentIndex=(NextPointIndex+m_iRoadDirect+m_vectRoad.size())% m_vectRoad.size();
			}
			//不是首尾相连
			else
			{
				countresult=NEXTSTEP_RESULT_RETURN;
				CurrentIndex=NextPointIndex;
				m_iRoadRecord=CurrentIndex;
				m_iRoadDirect=-m_iRoadDirect;
			}

		}
		//不是首尾点
		else																
		{
			countresult=NEXTSTEP_RESULT_NEXT;
			CurrentIndex=NextPointIndex;
		}
	}
	else
	{
		countresult=NEXTSTEP_RESULT_NULL;
	}

	LONG lPosX		= m_vectRoad[CurrentIndex+m_iRoadDirect].x;
	LONG lPosY		= m_vectRoad[CurrentIndex+m_iRoadDirect].y;
	LONG lDir		= GetLineDir(  lX, lY ,lPosX, lPosY);
	m_pOwner -> GetDirPos( lDir, lX, lY );

	return countresult;
}
VOID CMonsterAI::SetTarget( LONG lType, const CGUID& TargetGuid )
{
	if(HasTarget()==FALSE || m_aesAIState== AES_FIGHT_TRANSITION)
	{
		CServerRegion *pRegion = static_cast<CServerRegion*>(m_pOwner->GetFather());
		CMoveShape* pTarget = static_cast<CMoveShape*>(pRegion->FindChildObject(lType, TargetGuid));
		if( pTarget != NULL && pTarget->IsGod() )
		{
			CMoveShape* pHost = pTarget->GetHost();
			if( pHost != NULL )
			{
				CBaseAI::SetTarget(pHost->GetType(), pHost->GetExID());
			}
		}
		else
		{
			CBaseAI::SetTarget(lType, TargetGuid);
		}
	}
}

VOID CMonsterAI::SetForceTarget( LONG lType, const CGUID& TargetGuid )
{
	m_pOwner->SetForceAttack(FALSE);
	SetTarget(lType,TargetGuid);
}
VOID CMonsterAI::CleanForceTarget()
{
	m_pOwner->SetForceAttack(TRUE);
}

BOOL CMonsterAI::CallFriendAround()  //呼唤周围的同类型怪物
{
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner -> GetFather() );
	if( pRegion == NULL ) return 0;

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( m_pOwner, TYPE_MONSTER, vOut );

	CMoveShape* pTarget = GetTarget();
	if (pTarget==NULL)
	{
		return FALSE;
	}

	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	INT iFriendNum=0;

	for( size_t i = 0; i < vOut.size(); i ++ )
	{
		CMonster* pMonster = dynamic_cast<CMonster*>( vOut[i] );
		if( pMonster == NULL ) 
		{
			continue;
		}
		if (pMonster->GetNpcType()== NT_Guard ||
			pMonster->GetIndex()!=pOwner->GetIndex())
		{
			continue;
		}
		//##如果该生物也在警戒范围内
		if( m_pOwner -> RealDistance(pMonster) <= 
			pOwner -> GetAttribute(string("C_GuardRange")) )
		{
			//操作
			
			pMonster->GetAI()->MoveToCellOnPeace(pTarget->GetTileX(),pTarget->GetTileY());
		}
	}
	return TRUE;
}

BOOL CMonsterAI::CallMonsterAround()//呼唤周围的怪物
{
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>( m_pOwner -> GetFather() );
	if( pRegion == NULL ) return 0;

	vector<CShape*> vOut;
	pRegion -> FindAroundObject( m_pOwner, TYPE_MONSTER, vOut );

	CMoveShape* pTarget = GetTarget();
	if (pTarget==NULL)
	{
		return FALSE;
	}

	CMonster* pOwner=dynamic_cast<CMonster*>(m_pOwner);
	INT iFriendNum=0;

	for( size_t i = 0; i < vOut.size(); i ++ )
	{
		CMonster* pMonster = dynamic_cast<CMonster*>( vOut[i] );
		if( pMonster == NULL ) 
		{
			continue;
		}
		if (pMonster->GetNpcType()== NT_Guard )
		{
			continue;
		}
		//##如果该生物也在警戒范围内
		if( m_pOwner -> RealDistance(pMonster) <= 
			m_pOwner -> GetAlertRange() )
		{
			//操作
			pMonster->GetAI()->MoveToCellOnPeace(pTarget->GetTileX(),pTarget->GetTileY());
		}
	}
	return TRUE;
}



BOOL CMonsterAI::AddChangeAIEvent(LONG AIType)
{
	if (m_pOwner==NULL)
	{
		return FALSE;
	}
	CMonster* pOwner=(CMonster*)m_pOwner;
	if(pOwner->GetAIType()==AIType)
	{
		return TRUE;
	}
	pOwner->SetAIType(AIType);
	AddAIEvent(AET_CHANGEAI,100);
	return TRUE;
}

VOID CMonsterAI::RecordChoiceSkill(DWORD SkillID,DWORD SkillLevel,WORD SkillType,WORD SkillIndex)
{
	m_dwChoiceSkillID= SkillID;
	m_dwChoiceSkillLevel= SkillLevel;
	m_wSkillType= SkillType;
	m_wSkillIndex= SkillIndex;
	m_bMustUsed= SkillType==SKILLTYPE_CYCLE;
	m_bBeenUsed= FALSE;
}

VOID CMonsterAI::RecordUsedCurrentSkill()
{
	m_bBeenUsed= TRUE;

	vector<CMonster::tagSkillRecord>* skillrecordlist=0;
	if (m_wSkillType== SKILLTYPE_BASE) skillrecordlist= & (dynamic_cast<CMonster*>(m_pOwner)->GetBaseSkillList());
	else if (m_wSkillType== SKILLTYPE_NORMAL) skillrecordlist= &(dynamic_cast<CMonster*>(m_pOwner)->GetSkillList());
	else if (m_wSkillType== SKILLTYPE_CYCLE) skillrecordlist= &(dynamic_cast<CMonster*>(m_pOwner)->GetCycleSkillList());
	else return;

	if (skillrecordlist->size() > m_wSkillIndex)
	{
		(*skillrecordlist)[m_wSkillIndex].dwCoolDownTime= timeGetTime()+(*skillrecordlist)[m_wSkillIndex].dwCool;
	}

}

VOID CMonsterAI::CleanChoiceSkillRecord()
{
	m_dwChoiceSkillID= 0;
	m_dwChoiceSkillLevel= 0;
	m_wSkillType= 0;
	m_wSkillIndex= 0;
	m_bMustUsed= FALSE;
	m_bBeenUsed= TRUE;
}

BOOL CMonsterAI::IsInKindlyTime()
{
	// 刚出生时（非重生）为不友好状态
	if( m_dwRebornTime == 0 )
	{
		return FALSE;
	}

	return m_dwRebornTime + CGlobeSetup::GetSetup()->dwMonsterKindlyTime >= timeGetTime();	
}

bool CMonsterAI::UseSkillSyncSafe( long skillType, long skillID, long skillLevel, const CGUID &targetID )
{
	CMonster *pMonster = static_cast<CMonster*>( m_pOwner );
	CMoveShape *pTarget = NULL;
	if( targetID != NULL_GUID )
	{
		CServerRegion *pRegion = static_cast<CServerRegion*>( pMonster->GetFather() );
		pTarget = static_cast<CMoveShape*>( pRegion->FindChildObject( TYPE_MONSTER, targetID ));
		if( pTarget == NULL )
		{
			pTarget = static_cast<CMoveShape*>( pRegion->FindChildObject( TYPE_PLAYER,targetID ) );
		}
	}
	else
	{
		pTarget = GetTarget();
	}
	
	if( pTarget == NULL )
	{
		Stand( static_cast<CMonster*>( m_pOwner )->GetStopFrame() );	
		return false;
	}
	else if( !UseSkillSync( skillType, skillID, skillLevel, pTarget->GetTileX(), pTarget->GetTileY(), pTarget ) )
	{
		Stand( static_cast<CMonster*>( m_pOwner )->GetStopFrame() );
		return false;
	}

	return true;
}

bool CMonsterAI::UseSkillOnCellSyncSafe( long skillType, long skillID, long skillLevel, long targetX, long targetY )
{
	if( targetX == 0 && targetY == 0 )
	{
		CMoveShape *pTarget = GetTarget();
		if( pTarget == NULL )
		{
			Stand( static_cast<CMonster*>( m_pOwner )->GetStopFrame() );
			return false;
		}

		targetX = pTarget->GetTileX();
		targetY = pTarget->GetTileY();
	}

	if( !UseSkillSync( skillType, skillID, skillLevel, targetX, targetY ) )
	{
		Stand( static_cast<CMonster*>( m_pOwner )->GetStopFrame() );
		return false;
	}

	return true;
}

bool CMonsterAI::UseSkillSync( long skillType, long skillID, long skillLevel, long targetX, long targetY, CMoveShape *pTarget )
{
	if( m_pOwner == NULL )
	{
		return false;
	}

	CMonster *pMonster = static_cast<CMonster*>( m_pOwner );
	vector<CMonster::tagSkillRecord> *skillRecordList = NULL;
	if( skillType == SKILLTYPE_BASE )
	{
		skillRecordList = &pMonster->GetBaseSkillList();
	}
	else if( skillType == SKILLTYPE_NORMAL )
	{
		skillRecordList = &pMonster->GetSkillList();
	}
	else if( skillType == SKILLTYPE_CYCLE )
	{
		skillRecordList = &pMonster->GetCycleSkillList();
	}

	if( skillRecordList == NULL )
	{
		return false;
	}

	// find the record by its id.
	CMonster::tagSkillRecord *skillRecord = NULL;
	for( vector<CMonster::tagSkillRecord>::iterator it = skillRecordList->begin(); it != skillRecordList->end(); ++ it )
	{
		if( it->wID == skillID )
		{
			skillRecord = &(*it);

			break;
		}
	}

	if( skillRecord == NULL )
	{
		// not found
		return false;
	}

	DWORD this_time = timeGetTime();
	// someone told me do NOT consider the cool time for monsters - -|
	if( 0 && skillRecord->dwCoolDownTime > this_time )
	{
		// cd is not ready
		return false;
	}

	// ready to fire....
	skillRecord->dwCoolDownTime = this_time + skillRecord->dwCool ;

	pMonster->InitSkillInstance( skillID, skillLevel ,0,0,0,NULL_GUID,6);
	return BeginSkill( pMonster->GetInstanceSkill(), targetX, targetY, pTarget ) == TRUE;
}
