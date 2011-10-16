#include "StdAfx.h"
#include "EffectObjects.h"
#include "BuffSkillDef.h"
#include "CondiGroup.h"
#include "../ServerRegion.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CEffectObjects::CEffectObjects(stModuParam *pParam,CCondiGroup* pCondiGroup)
:m_pParam(pParam)
,m_pCondiGroup(pCondiGroup)
{

}

CEffectObjects::~CEffectObjects(void)
{
	OBJ_RELEASE(CCondiGroup, m_pCondiGroup);
}


//////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//CEffDestObjs

CEffDestObjs::CEffDestObjs(stModuParam *pParam,CCondiGroup* pCondiGroup)
:CEffectObjects(pParam,pCondiGroup)
{
}

CEffDestObjs::~CEffDestObjs(void)
{
}

//得到所用的作用对象
unsigned int CEffDestObjs::GetEffectObjs(list<CMoveShape*>& listObjs)
{
	int nNum = 0;
	listObjs.clear();

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>(m_pParam->pUser->GetFather());
	itDestObj it = m_pParam->OperObjs.begin();
	for(;it != m_pParam->OperObjs.end();it++)
	{
		CBaseObject *pObj = pRegion->FindChildObject((OBJECT_TYPE)(*it)->nDestType,(*it)->DestID);
					/*GameManager::GetInstance()->FindObject(
					(OBJECT_TYPE)(*it)->nDestType,
					(*it)->DestID);*/
		CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(pObj);
		if(pMoveShape && (m_pCondiGroup || m_pCondiGroup->Result(pMoveShape,m_pParam) ))
		{
			nNum++;
			listObjs.push_back(pMoveShape);
		}
	}
	return nNum;
}

//////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//CEffDestObjs

CEffTimerObjs::CEffTimerObjs(stModuParam *pParam,CCondiGroup* pCondiGroup)
:CEffectObjects(pParam,pCondiGroup)
{
}

CEffTimerObjs::~CEffTimerObjs(void)
{
}

//得到所用的作用对象
unsigned int CEffTimerObjs::GetEffectObjs(list<CMoveShape*>& listObjs)
{
	int nNum = 0;
	listObjs.clear();

	CServerRegion* pRegion = dynamic_cast<CServerRegion*>(m_pParam->pUser->GetFather());
	stTimerParam *pTimerParam = m_pParam->FindTimerParam(m_pParam->lCurTimerID);
	if(pTimerParam)
	{
		CBaseObject *pObj = pRegion->FindChildObject((OBJECT_TYPE)pTimerParam->lDestType,pTimerParam->DestID);					
		CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(pObj);
		if(pMoveShape && m_pCondiGroup && m_pCondiGroup->Result(pMoveShape,m_pParam))
		{
			nNum++;
			listObjs.push_back(pMoveShape);
		}
	}
	return nNum;
}

