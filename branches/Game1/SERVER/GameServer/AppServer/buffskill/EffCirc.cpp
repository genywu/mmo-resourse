#include "StdAfx.h"
#include "EffCirc.h"
#include "BuffSkillDef.h"
#include "CondiGroup.h"
#include "..\MoveShape.h"
#include "..\ServerRegion.h"

CEffCirc::CEffCirc(stModuParam *pParam,CCondiGroup* pCondiGroup)
:CEffectObjects(pParam,pCondiGroup)
{

}
CEffCirc::~CEffCirc(void)
{
}

//得到所用的作用对象
unsigned int CEffCirc::GetEffectObjs(list<CMoveShape*>& listObjs)
{
	listObjs.clear();
	int nNum = 0;
	list<POINT> listPos;
	m_pParam->nPosition = m_eCenter;
	

	CServerRegion* pServerRegion = (CServerRegion*)(m_pParam->pUser->GetFather());
	if(NULL == pServerRegion)	return 0;

	CBaseObject *pObj = pServerRegion->FindChildObject(m_pParam->nMouseShapeType,m_pParam->MouseShapeID);
	m_pParam->pMouseShape = dynamic_cast<CMoveShape*>(pObj);	


	if(m_eCenter == ePos_Self && m_pParam->pUser)
	{
		m_pParam->lDestX = m_pParam->pUser->GetTileX();
		m_pParam->lDestY = m_pParam->pUser->GetTileY();
	}
	else if (m_eCenter == ePos_Mouse)
	{
		m_pParam->lDestX = m_pParam->nMouseX;
		m_pParam->lDestY = m_pParam->nMouseY;
	}
	else if (m_eCenter == ePos_Dest && m_pParam->pMouseShape)
	{
		m_pParam->lDestX = m_pParam->pMouseShape->GetTileX();
		m_pParam->lDestY = m_pParam->pMouseShape->GetTileY();
	}
	else if(m_eCenter==ePos_DestCoord && m_pParam->lDestX==0 && m_pParam->lDestY==0 && m_pParam->pMouseShape)
	{
		m_pParam->lDestX = m_pParam->pMouseShape->GetTileX();
		m_pParam->lDestY = m_pParam->pMouseShape->GetTileY();
	}
	else if (m_eCenter == ePos_Coord)
	{
		m_pParam->lDestX = m_pParam->nMouseX;
		m_pParam->lDestY = m_pParam->nMouseY;
	}

	vector<CShape*> vOut;

	pServerRegion->FindAroundObject(m_pParam->pUser,TYPE_MONSTER,vOut);
	pServerRegion->FindAroundObject(m_pParam->pUser,TYPE_PLAYER,vOut);
	for (vector<CShape*>::iterator it=vOut.begin(); it!=vOut.end();++it)
	{
		if(m_eCenter == ePos_Self && (*it)!=m_pParam->pUser)
		{
			//使用者
			long lDistance = m_pParam->pUser->RealDistance((*it));
			if (lDistance<0)			
				lDistance = 0;
			
			if (lDistance>m_nRadius)
			{
				continue;
			}		
		}
		else if(m_eCenter == ePos_Mouse)
		{
			//鼠标
			if(Distance((*it)->GetTileX(),(*it)->GetTileY(),m_pParam->nMouseX,m_pParam->nMouseY)>m_nRadius)
			{
				continue;
			}			
		}
		else if(m_eCenter == ePos_Dest && m_pParam->pMouseShape && m_pParam->pMouseShape!=(*it))
		{
			//目标
			long lDistance = m_pParam->pMouseShape->RealDistance((*it));
			if (lDistance<0)
				lDistance = 0;
			if (lDistance>m_nRadius)
			{
				continue;			
			}		
		}
		else if(m_eCenter==ePos_DestCoord && m_pParam->lDestX!=0 && m_pParam->lDestY!=0)
		{
			//坐标
			if (Distance(m_pParam->lDestX,m_pParam->lDestY,(*it)->GetTileX(),(*it)->GetTileY())>m_nRadius)
			{
				continue;			
			}
		}
		else if (m_eCenter==ePos_Coord)
		{
			if (Distance(m_pParam->lDestX,m_pParam->lDestY,(*it)->GetTileX(),(*it)->GetTileY())>m_nRadius)
			{
				continue;			
			}
		}

		CMoveShape *pMoveShape = dynamic_cast<CMoveShape*>(*it);
		if(pMoveShape &&(NULL==m_pCondiGroup || m_pCondiGroup->Result(pMoveShape,m_pParam)))
		{
			++nNum;		
			if (m_nNum>0)
			{      
				if (m_nNum<nNum)
				{
					break;
				}
			}		
			listObjs.push_back(pMoveShape);
		}		
	}	
	
	return nNum;
}
