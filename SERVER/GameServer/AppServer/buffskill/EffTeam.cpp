#include "StdAfx.h"
#include "BuffSkillDef.h"
#include "CondiGroup.h"
#include "EffTeam.h"
#include "../OrganizingSystem/GSTeam.h"
#include "../OrganizingSystem/GameOrganizingCtrl.h"
#include "../Player.h"
#include "..\..\GameServer\GameManager.h"
#include "..\ServerRegion.h"

CEffTeam::CEffTeam(stModuParam *pParam,CCondiGroup* pCondiGroup)
:CEffectObjects(pParam,pCondiGroup)
{
}

CEffTeam::~CEffTeam(void)
{
}

//得到所用的作用对象
unsigned int CEffTeam::GetEffectObjs(list<CMoveShape*>& listObjs)
{
	list<CGUID> lMemberList;
	int nNum = 0;
	CServerRegion* pRegion = dynamic_cast<CServerRegion*>(m_pParam->pUser->GetFather());
	GSTeam *pGSTeam = (GSTeam*)(GetOrganiCtrl()->GetGameTeamByID(((CPlayer*)m_pParam->pUser) -> GetTeamID()));

	if (pGSTeam)
	{
		pGSTeam->GetMemberList(lMemberList);
		list<CGUID>::iterator it=lMemberList.begin();
		for (;it!=lMemberList.end();++it)
		{
			CBaseObject *pObj = pRegion->FindChildObject(TYPE_PLAYER,*it);
			CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(pObj);
			if(pMoveShape && (m_pCondiGroup || m_pCondiGroup->Result(pMoveShape,m_pParam) ) && (Distance(m_pParam->pUser->GetTileX(),m_pParam->pUser->GetTileY(),pMoveShape->GetTileX(),pMoveShape->GetTileY())<=m_nDistance))
			{					
				nNum++;
				listObjs.push_back(pMoveShape);
			}
		}
	}
	else
	{
		nNum = 1;
		listObjs.push_back(m_pParam->pUser);
	}
		
	return nNum;
}
