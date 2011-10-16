
#include "StdAfx.h"
#include "EffAppObject.h"
#include "BuffSkillDef.h"
#include "../MoveShape.h"
#include "../ServerRegion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEffAppObject::CEffAppObject(stModuParam *pParam,CCondiGroup* pCondiGroup)
:CEffectObjects(pParam,pCondiGroup)
{
}
CEffAppObject::CEffAppObject(void *pParam)
:CEffectObjects(((tagTmpParam*)pParam)->pParam, ((tagTmpParam*)pParam)->pCondiGroup)
{
}
CEffAppObject::~CEffAppObject()
{

}

unsigned int CEffAppObject::GetEffectObjs(list<CMoveShape*>& listObjs)
{
	int nNum=0;

	CServerRegion* pServerRegion = (CServerRegion*)(m_pParam->pUser->GetFather());
	if(NULL == pServerRegion)	return 0;

	itDestObj it = m_pParam->OperObjs.begin();
	for(;it != m_pParam->OperObjs.end();it++)
	{
		CBaseObject *pObj = pServerRegion->FindChildObject((*it)->nDestType,(*it)->DestID);		
		CMoveShape *pMoveShape = dynamic_cast<CMoveShape*>(pObj);
		if (pMoveShape)
		{
			listObjs.push_back(pMoveShape);
			m_pParam->pMouseShape = pMoveShape;
			++nNum;
		}
	}

	return nNum;
}