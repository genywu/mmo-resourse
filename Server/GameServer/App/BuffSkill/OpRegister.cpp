#include "StdAfx.h"
#include "OpRegister.h"

#include "../Player.h"
#include "../ServerRegion.h"
#include "../../Server/GameManager.h"

#include "../../../../Public/Setup/GlobalSetup.h"
#include "../../../../Public/ServerPublic/Server/Common/VariableList.h"
#include "../../../../Public/ServerPublic/Server/Common/VarListUtils.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

COpRegisterTimer::COpRegisterTimer(stModuParam *pParam)
:COperator(pParam)
{
}
COpRegisterTimer::COpRegisterTimer(void* pParam)
:COperator((stModuParam*)pParam)
{

}
COpRegisterTimer::~COpRegisterTimer(void)
{

}

void COpRegisterTimer::Execute(CMoveShape* pShape)
{
    CMoveShape* pDest = pShape;
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

			tagTimerVar* pTimvar = OBJ_CREATE(tagTimerVar);
			pTimvar->TimerType = (eChildObjType)m_eChildObjType;
			pTimvar->lvar = m_pParam->dwGlobleID;
			pTimvar->lvar1 = m_pParam->nID;
			m_pParam->lUseTime = pTimerparam->lFuture - GlobalSetup::GetSetup()->dwActTime;

			//注册定时器
			pTimerparam->lTimerID = TIMER_SCHEDULE(pShape,
				pTimvar,pTimerparam->nStartTime+pTimerparam->lFuture,
				m_lInter);

#ifdef _SKILL_INFO_D_
            DWORD dwCurTime = timeGetTime();
            Log4c::Trace(ROOT_MODULE,"COpRegisterTimer:技能ID%d,键值%d,时间%d,注册时间\n",m_pParam->nID, m_pParam->dwGlobleID,m_pParam->lUseTime);
#endif
		}
	}
	else
	{
		//不存在则分配
		stTimerParam *pTimerparam = OBJ_CREATE(stTimerParam);	
		//初始化数据
		pTimerparam->lDestType = pShape->GetType();
		pTimerparam->DestID = pShape->GetExID();

		//  [5/4/2009 chenxianj]	
		if(m_strFutVarname != "")
			m_lFuture = (long)m_pParam->pVariableList->GetVarValue<double>(m_strFutVarname.c_str(),0);
		if(m_strInterVarName != "")
			m_lInter = (long)m_pParam->pVariableList->GetVarValue<double>(m_strInterVarName.c_str(),0);
		if(m_strCountVarName != "")
			m_nCount = (long)m_pParam->pVariableList->GetVarValue<double>(m_strCountVarName.c_str(),0);
		//  [5/4/2009 chenxianj]

        // by:expter 读取配置
        if (m_strFutureParamname!="")
        {
            m_lFuture = pShape->GetSkillValue(m_pParam->nID,m_pParam->nLvl,m_strFutureParamname,m_pParam->nMouseX,m_pParam->nMouseY);
        }
        if (m_strInterParamname!="")
        {
            m_lInter = pShape->GetSkillValue(m_pParam->nID,m_pParam->nLvl,m_strInterParamname,m_pParam->nMouseX,m_pParam->nMouseY);
        }
        if (m_strCountParamname!="")
        {
            m_nCount = pShape->GetSkillValue(m_pParam->nID,m_pParam->nLvl,m_strCountParamname,m_pParam->nMouseX,m_pParam->nMouseY);
        }

		m_pParam->lUseTime = m_lFuture + m_nCount*m_lInter ;//  - GlobalSetup::GetSetup()->dwActTime

		pTimerparam->lFuture = m_lFuture;
		pTimerparam->lInter = m_lInter;
		pTimerparam->nCount = m_nCount;

		pTimerparam->lFlag = m_lFlag;
		pTimerparam->nCalledCount=0;
		pTimerparam->nStartTime = timeGetTime();

		tagTimerVar* pTimvar =  OBJ_CREATE(tagTimerVar);
		pTimvar->TimerType = (eChildObjType)m_eChildObjType;
		pTimvar->lvar = m_pParam->dwGlobleID;
		pTimvar->lvar1 = m_pParam->nID;
		//注册定时器
		pTimerparam->lTimerID = TIMER_SCHEDULE(pShape,
			pTimvar,pTimerparam->nStartTime+m_lFuture,
			m_lInter);

#ifdef _SKILL_INFO_D_
        DWORD dwCurTime = timeGetTime();
        Log4c::Trace(ROOT_MODULE, "COpRegisterTimer:技能ID%d,键值%d,时间%d",m_pParam->nID, m_pParam->dwGlobleID,m_pParam->lUseTime);
#endif
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
COpRegisterEvent::COpRegisterEvent(void* pParam)
:COperator((stModuParam*)pParam)
{

}
COpRegisterEvent::~COpRegisterEvent(void)
{
}

//注册事件
void COpRegisterEvent::Execute(CMoveShape* pShape)
{	
	stEventParam *pParam = OBJ_CREATE(stEventParam);
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
COpUnRegisterTimer::COpUnRegisterTimer(void *pParam)
:COperator(((tagParam*)pParam)->pParam),m_lFlag(((tagParam*)pParam)->lFlag)
{

};
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
			OBJ_RELEASE(stTimerParam, pParam);
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
COpUnRegisterEvent::COpUnRegisterEvent(void* pParam)
:COperator(((tagParam*)pParam)->pParam)
,m_lFlag(((tagParam*)pParam)->lFlag)
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
				OBJ_RELEASE(stEventParam, pParam);
			}
			else
			{
				it++;
			}
		}
	}
}