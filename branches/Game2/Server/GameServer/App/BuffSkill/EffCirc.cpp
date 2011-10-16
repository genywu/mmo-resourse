#include "StdAfx.h"
#include "EffCirc.h"
#include "BuffSkillDef.h"
#include "CondiGroup.h"
#include "../MoveShape.h"
#include "../ServerRegion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEffCirc::CEffCirc(stModuParam *pParam,CCondiGroup* pCondiGroup)
:CEffectObjects(pParam,pCondiGroup)
{

}
CEffCirc::CEffCirc(void *pParam)
:CEffectObjects(((tagTmpParam*)pParam)->pParam, ((tagTmpParam*)pParam)->pCondiGroup)
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
	
    if ( m_strNum!="" )
    {
          /// by:expter 2010/08/31 技能增加一个参数
          m_nNum = m_pParam->pFirer->GetSkillValue(m_pParam->nID,m_pParam->nLvl,m_strNum);
    }
    if( m_nNum < 0 )
        m_nNum = 0;

	CServerRegion* pServerRegion = (CServerRegion*)(m_pParam->pUser->GetFather());
	if(NULL == pServerRegion)	return 0;

	CBaseObject *pObj = pServerRegion->FindChildObject(m_pParam->nMouseShapeType,m_pParam->MouseShapeID);
	m_pParam->pMouseShape = dynamic_cast<CMoveShape*>(pObj);	

    CMoveShape *pExcept = NULL;
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
        if( m_IsWithinObj)
            pExcept = m_pParam->pMouseShape;
	}
	else if(m_eCenter==ePos_DestCoord && m_pParam->lDestX==0 && m_pParam->lDestY==0 && m_pParam->pMouseShape)
	{
		m_pParam->lDestX = m_pParam->pMouseShape->GetTileX();
		m_pParam->lDestY = m_pParam->pMouseShape->GetTileY();
        if( m_IsWithinObj)
            pExcept = m_pParam->pMouseShape;
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

        if ( pExcept )
        {
#ifdef _SKILL_INFO_D_      
            Log4c::Trace(SKILL_MODULE, "EffCirc:作用于对象外的玩家判断!\n");    
#endif
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
            /// 除去范围内对象
            if( pExcept && pExcept == pMoveShape )
            {
                continue;                       
            }
			listObjs.push_back(pMoveShape);
		}		
	}	
	
	return nNum;
}
