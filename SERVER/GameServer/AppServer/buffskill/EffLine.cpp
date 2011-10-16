#include "StdAfx.h"
#include "EffLine.h"
#include "BuffSkillDef.h"
#include "CondiGroup.h"
#include "..\MoveShape.h"
#include "..\ServerRegion.h"


CEffLine::CEffLine(stModuParam *pParam,CCondiGroup* pCondiGroup)
:CEffectObjects(pParam,pCondiGroup)
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
	list<POINT> listPos;
	CServerRegion* pServerRegion = (CServerRegion*)(m_pParam->pUser->GetFather());
	if(NULL == pServerRegion)	return 0;


	CBaseObject *pObj = pServerRegion->FindChildObject(m_pParam->nMouseShapeType,m_pParam->MouseShapeID);
	m_pParam->pMouseShape = dynamic_cast<CMoveShape*>(pObj);

	if(m_ePos == ePos_Self)
		return 0;
	//目标
	else if(m_ePos == ePos_Dest && m_pParam->pMouseShape)
	{
		GetLinePointToDest(listPos,
			m_pParam->pUser->GetTileX(),m_pParam->pUser->GetTileY(),
			m_pParam->pMouseShape->GetTileX(),m_pParam->pMouseShape->GetTileY(),m_nDistance);
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

	//计算出moveshape
	list<POINT>::iterator it = listPos.begin();
	for(;it != listPos.end();it++)
	{
		POINT& pos = (*it);		
		CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(pServerRegion->GetShape(pos.x,pos.y));
		if(pMoveShape &&(NULL==m_pCondiGroup || m_pCondiGroup->Result(pMoveShape,m_pParam)))
		{
			nNum++;
			listObjs.push_back(pMoveShape);
		}

	}
	return nNum;
}
