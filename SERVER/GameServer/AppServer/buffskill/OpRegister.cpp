#include "StdAfx.h"
#include "OpRegister.h"
#include "..\public\GameEvent.h"
#include "..\..\GameServer\GameManager.h"
#include "..\ServerRegion.h"
#include "..\Player.h"
#include "..\script\VariableList.h"


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

COpRegisterTimer::COpRegisterTimer(stModuParam *pParam)
:COperator(pParam)
{
}
COpRegisterTimer::~COpRegisterTimer(void)
{

}

void COpRegisterTimer::Execute(CMoveShape* pShape)
{
	pShape = GetOpShape(pShape);
	if(NULL == pShape)	return;

	stTimerParam *pTimerparam =
		m_pParam->FindTimerParam(m_lFlag,pShape->GetType(),pShape->GetExID());
	//存在重设
	if(pTimerparam)
	{
		pTimerparam->lInter = m_lInter;
		//已经注册
		if(pTimerparam->lTimerID == -1)
		{
			pTimerparam->nStartTime = timeGetTime();

			tagTimerVar* pTimvar = CBaseObject::CreateTimerVar(36);
			pTimvar->TimerType = (eChildObjType)m_eChildObjType;
			pTimvar->lvar = m_pParam->dwGlobleID;
			pTimvar->lvar1 = m_pParam->nID;
			m_pParam->lUseTime = pTimerparam->lFuture - CGlobeSetup::GetSetup()->dwActTime;

			//注册定时器
			pTimerparam->lTimerID = TIMER_SCHEDULE(pShape,
				pTimvar,pTimerparam->nStartTime+pTimerparam->lFuture,
				m_lInter);
		}
	}
	else
	{
		//不存在则分配
		stTimerParam *pTimerparam = MP_NEW stTimerParam;	
		//初始化数据
		pTimerparam->lDestType = pShape->GetType();
		pTimerparam->DestID = pShape->GetExID();

		//  [5/4/2009 chenxianj]	
		if(m_strFutVarname != "")
			m_lFuture = m_pParam->pVariableList->GetVarValue(m_strFutVarname.c_str(),0);
		if(m_strInterVarName != "")
			m_lInter = m_pParam->pVariableList->GetVarValue(m_strInterVarName.c_str(),0);
		if(m_strCountVarName != "")
			m_nCount = m_pParam->pVariableList->GetVarValue(m_strCountVarName.c_str(),0);
		//  [5/4/2009 chenxianj]	

		m_pParam->lUseTime = m_lFuture + m_nCount*m_lInter ;
		pTimerparam->lFuture = m_lFuture;
		pTimerparam->lInter = m_lInter;
		pTimerparam->nCount = m_nCount;

		pTimerparam->lFlag = m_lFlag;
		pTimerparam->nCalledCount=0;
		pTimerparam->nStartTime = timeGetTime();

		tagTimerVar* pTimvar =  CBaseObject::CreateTimerVar(37);
		pTimvar->TimerType = (eChildObjType)m_eChildObjType;
		pTimvar->lvar = m_pParam->dwGlobleID;
		pTimvar->lvar1 = m_pParam->nID;
		//注册定时器
		pTimerparam->lTimerID = TIMER_SCHEDULE(pShape,
			pTimvar,pTimerparam->nStartTime+m_lFuture,
			m_lInter);

		//保存定时器
		m_pParam->Timers.push_back(pTimerparam);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
COpRegisterEvent::COpRegisterEvent(stModuParam *pParam)
:COperator(pParam)
{
}
COpRegisterEvent::~COpRegisterEvent(void)
{
}

//注册事件
void COpRegisterEvent::Execute(CMoveShape* pShape)
{	
	stEventParam *pParam = MP_NEW stEventParam;
	pParam->lEventType = m_lEventType;
	pParam->lFlag = m_lFlag;

	long lEventID = pShape->GetEventIDCount();
	tagEvenRegVar RegVar = tagEvenRegVar(lEventID,pShape->GetType(),pShape->GetExID(),
		m_eChildObjType,m_pParam->dwGlobleID);

	pParam->RegVar = RegVar;
	pShape->RegisterEvent(eEventType(m_lEventType),RegVar);	
	//保存注册的事件
	m_pParam->Events.push_back(pParam);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

COpUnRegisterTimer::COpUnRegisterTimer(stModuParam *pParam,
									   long lFlag)
									   :COperator(pParam)
									   ,m_lFlag(lFlag)
{
}
COpUnRegisterTimer::~COpUnRegisterTimer(void)
{
}

void COpUnRegisterTimer::Execute(CMoveShape* pShape)
{
	//取消相同标记的定时器
	stModuParam::itTimer it = m_pParam->Timers.begin();
	for(;it != m_pParam->Timers.end();)
	{
		stTimerParam* pParam = (*it);
		if(pParam && (pParam->lFlag==m_lFlag || m_lFlag==0))
		{
			if(pParam->lTimerID > 0)
				int nNum = TIMER_CANCEL(pParam->lTimerID);
			pParam->lTimerID = -1;

			it=m_pParam->Timers.erase(it);
			MP_DELETE(pParam);
		}
		else
		{
			it++;
		}
	}
	m_pParam->Timers.clear();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
COpUnRegisterEvent::COpUnRegisterEvent(stModuParam *pParam,
									   long lFlag)
									   :COperator(pParam)
									   ,m_lFlag(lFlag)
{
}
COpUnRegisterEvent::~COpUnRegisterEvent(void)
{
}

//注销事件
void COpUnRegisterEvent::Execute(CMoveShape* pShape)
{
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>(m_pParam->pUser->GetFather());
	if (pRegion)
	{
		stModuParam::itEvent it = m_pParam->Events.begin();
		for(;it!=m_pParam->Events.end();)
		{
			stEventParam *pParam = (*it);
			if(pParam && (pParam->lFlag == m_lFlag || m_lFlag==0) )
			{
				CBaseObject *pObj = pRegion->FindChildObject((OBJECT_TYPE)(pParam->RegVar.lType1),pParam->RegVar.ID1);
				
				if(!pObj && pParam->RegVar.lType1 == TYPE_PLAYER)
					CBaseObject *pObj = GetGame()->FindPlayer(pParam->RegVar.ID1);
				if(pObj)
					pObj->UnRegisterEvent(eEventType(pParam->lEventType),pParam->RegVar.lEventID);
			
				it = m_pParam->Events.erase(it);
				MP_DELETE(pParam);
			}
			else
			{
				it++;
			}
		}
	}
}