#include "StdAfx.h"
#include "effpoint.h"
#include "BuffSkillDef.h"
#include "CondiGroup.h"
#include "../MoveShape.h"
#include "../ServerRegion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEffPoint::CEffPoint(stModuParam *pParam,CCondiGroup* pCondiGroup)
:CEffectObjects(pParam,pCondiGroup)
{
}

CEffPoint::CEffPoint(void *pParam)
:CEffectObjects(((tagTmpParam*)pParam)->pParam, ((tagTmpParam*)pParam)->pCondiGroup)
{
}

CEffPoint::~CEffPoint(void)
{
}

//得到所用的作用对象
unsigned int CEffPoint::GetEffectObjs(list<CMoveShape*>& listObjs)
{
	listObjs.clear();
	CMoveShape* pShape = NULL;
	if(m_ePos==ePos_Self)
	{
		pShape= m_pParam->pUser;
	}
	else if(m_ePos==ePos_Dest )
	{
		CServerRegion* pServerRegion = (CServerRegion*)(m_pParam->pUser->GetFather());
		if(pServerRegion)
		{
			CBaseObject *pObj = pServerRegion->FindChildObject(m_pParam->nMouseShapeType,m_pParam->MouseShapeID);
			m_pParam->pMouseShape = dynamic_cast<CMoveShape*>(pObj);
			pShape = m_pParam->pMouseShape;

		}
	}
	else if(m_ePos == ePos_Mouse)
	{
		CServerRegion* pServerRegion = (CServerRegion*)(m_pParam->pUser->GetFather());
		if(pServerRegion)
		{		
				pShape = dynamic_cast<CMoveShape*>(pServerRegion->GetShape(m_pParam->nMouseX,m_pParam->nMouseY));
				m_pParam->pMouseShape = pShape;
		}
	}
	else if (m_ePos == ePos_Firer)
	{
		if (m_pParam->pFirer)
		{
			pShape = m_pParam->pFirer;
		}
		else
		{
			CServerRegion* pServerRegion = (CServerRegion*)(m_pParam->pUser->GetFather());
			if(pServerRegion)
			{		
				CBaseObject *pObj = pServerRegion->FindChildObject(m_pParam->nFirerType,m_pParam->FirerID);
				pShape = dynamic_cast<CMoveShape*>(pObj);
			}
		}
	}
	else if (m_ePos == ePos_Attacker)
	{
		CServerRegion* pServerRegion = (CServerRegion*)(m_pParam->pUser->GetFather());
		if(pServerRegion)
		{		
			CBaseObject *pObj = pServerRegion->FindChildObject(m_pParam->pUser->GetAttackerType(),m_pParam->pUser->GetAttackerGuid());
			if (pObj)
			{
				pShape = dynamic_cast<CMoveShape*>(pObj);
			}			
		}
	}
	else if (m_ePos == ePos_Target)
	{
		CServerRegion* pServerRegion = (CServerRegion*)(m_pParam->pUser->GetFather());
		if(pServerRegion)
		{		
			CGUID TargetGuid =NULL_GUID;
			long  lTargetType = 0;
			m_pParam->pUser->GetTarget(TargetGuid,lTargetType);
			CBaseObject *pObj = pServerRegion->FindChildObject(lTargetType,TargetGuid);
			if (pObj)
			{
				pShape = dynamic_cast<CMoveShape*>(pObj);
			}			
		}
	}
	else if (m_ePos==ePos_AttackTarget)
	{
		CServerRegion* pServerRegion = (CServerRegion*)(m_pParam->pUser->GetFather());
		if(pServerRegion)
		{
			CGUID AttackTargetGuid =NULL_GUID;
			long  lAttackTargetType = 0;
			m_pParam->pUser->GetAttacktarget(AttackTargetGuid,lAttackTargetType);
			CBaseObject *pObj = pServerRegion->FindChildObject(lAttackTargetType,AttackTargetGuid);
			if (pObj)
			{
				pShape = dynamic_cast<CMoveShape*>(pObj);
			}			
		}

	}
	else if (m_ePos == ePos_Host)
	{
		pShape = m_pParam->pUser->GetHost();
	}
	
	
	//加入shape
	if(pShape)
	{
		if(m_pCondiGroup )
		{	//有条件且为真
			if( m_pCondiGroup->Result(pShape,m_pParam) )
				listObjs.push_back(pShape);
		}
		else
		{
			//没有条件对象
			listObjs.push_back(pShape);
		}
	}
	return (unsigned int)listObjs.size();
}