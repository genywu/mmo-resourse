#include "StdAfx.h"
#include "EffLineFirst.h"
#include "BuffSkillDef.h"
#include "..\MoveShape.h"


CEffLineFirst::CEffLineFirst(stModuParam *pParam,CCondiGroup* pCondiGroup)
:CEffectObjects(pParam,pCondiGroup)
{
}
CEffLineFirst::~CEffLineFirst(void)
{
}

unsigned int CEffLineFirst::GetEffectObjs(list<CMoveShape*>& listObjs)
{
	int nNum=0;
	return nNum;
}

CEffEventByParam::CEffEventByParam(stModuParam *pParam,CCondiGroup* pCondiGroup)
:CEffectObjects(pParam,pCondiGroup)
{
}
CEffEventByParam::~CEffEventByParam(void)
{
}

unsigned int CEffEventByParam::GetEffectObjs(list<CMoveShape*>& listObjs)
{
	listObjs.clear();	
	int nNum=0;
	if (m_pParam->pEventVar && m_pParam->pEventVar->pVar)
	{
		CMoveShape *pMoveShape = (CMoveShape*)(m_pParam->pEventVar->pVar);
			if (pMoveShape)
			{
				m_pParam->pMouseShape = pMoveShape;
				m_pParam->nMouseShapeType = pMoveShape->GetType();
				m_pParam->MouseShapeID = pMoveShape->GetExID();
				listObjs.push_back(pMoveShape);
				++nNum;
			}	
	}
	return nNum;
}


