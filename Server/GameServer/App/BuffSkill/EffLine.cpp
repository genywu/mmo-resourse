#include "StdAfx.h"
#include "EffLine.h"
#include "BuffSkillDef.h"
#include "CondiGroup.h"
#include "../MoveShape.h"
#include "../ServerRegion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEffLine::CEffLine(stModuParam *pParam,CCondiGroup* pCondiGroup)
:CEffectObjects(pParam,pCondiGroup)
{
}
CEffLine::CEffLine(void *pParam)
:CEffectObjects(((tagTmpParam*)pParam)->pParam, ((tagTmpParam*)pParam)->pCondiGroup)
{
}

CEffLine::~CEffLine(void)
{
}


//得到所用的作用对象
unsigned int CEffLine::GetEffectObjs(list<CMoveShape*>& listObjs)
{
	listObjs.clear();
	int nNum = 0;

    if ( m_strNum!="" )
    {
        /// by:expter 2010/08/31 技能增加一个参数
        m_nNum = m_pParam->pFirer->GetSkillValue(m_pParam->nID,m_pParam->nLvl,m_strNum);
    }
    if( m_nNum < 0 )
        m_nNum = 0;

	list<POINT> listPos;
	CServerRegion* pServerRegion = (CServerRegion*)(m_pParam->pUser->GetFather());
	if(NULL == pServerRegion)	return 0;

	CBaseObject *pObj = pServerRegion->FindChildObject(m_pParam->nMouseShapeType,m_pParam->MouseShapeID);
	m_pParam->pMouseShape = dynamic_cast<CMoveShape*>(pObj);

    CMoveShape *pExcept = NULL;

	if(m_ePos == ePos_Self)
		return 0;
	//目标
	else if(m_ePos == ePos_Dest && m_pParam->pMouseShape)
	{
		GetLinePointToDest(listPos,
			m_pParam->pUser->GetTileX(),m_pParam->pUser->GetTileY(),
			m_pParam->pMouseShape->GetTileX(),m_pParam->pMouseShape->GetTileY(),m_nDistance);
        if( m_IsWithinObj)
            pExcept = m_pParam->pMouseShape;
	}
	//目标坐标点位置
	else if(m_ePos == ePos_Coord)
	{
		GetLinePointToDest(listPos,
			m_pParam->pUser->GetTileX(),m_pParam->pUser->GetTileY(),
			m_pParam->nMouseX,m_pParam->nMouseY,m_nDistance);
	}	
	//鼠标位置
	else if(m_ePos == ePos_Mouse)
	{		
		long lDir = m_pParam->pUser->GetDestDir(m_pParam->pUser->GetTileX(),
							m_pParam->pUser->GetTileY(),
							m_pParam->nMouseX,m_pParam->nMouseY);
		//得到与目标之间的格子
		//得到与目标之间的格子
		GetLinePointByDir(listPos,m_pParam->pUser->GetTileX(),
									m_pParam->pUser->GetTileY(),lDir,m_nDistance);
	}

    if ( pExcept )
    {
#ifdef _SKILL_INFO_D_      
        Log4c::Trace(SKILL_MODULE, "CEffLine:作用于对象外的玩家");    
#endif
    }

	//计算出moveshape
	list<POINT>::iterator it = listPos.begin();
	for(;it != listPos.end();it++)
	{
		POINT& pos = (*it);		
		CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(pServerRegion->GetShape(pos.x,pos.y));
		if(pMoveShape &&(NULL==m_pCondiGroup || m_pCondiGroup->Result(pMoveShape,m_pParam)))
		{
			nNum++;
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
#ifdef _SKILL_INFO_D_       
               Log4c::Trace(SKILL_MODULE, "CEffLine:作用于对象外的玩家");    
#endif
                continue;                       
            }
			listObjs.push_back(pMoveShape);
		}

	}
	return nNum;
}
