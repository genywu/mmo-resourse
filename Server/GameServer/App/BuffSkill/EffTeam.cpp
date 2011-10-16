#include "StdAfx.h"
#include "BuffSkillDef.h"
#include "CondiGroup.h"
#include "EffTeam.h"
#include "../OrganizingSystem/GSTeam.h"
#include "../OrganizingSystem/GameOrganizingCtrl.h"
#include "../Player.h"
#include "../../Server/GameManager.h"
#include "../ServerRegion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEffTeam::CEffTeam(stModuParam *pParam,CCondiGroup* pCondiGroup)
:CEffectObjects(pParam,pCondiGroup)
{
}
CEffTeam::CEffTeam(void *pParam)
:CEffectObjects(((tagTmpParam*)pParam)->pParam, ((tagTmpParam*)pParam)->pCondiGroup)
{
}

CEffTeam::~CEffTeam(void)
{
}

//得到所用的作用对象
unsigned int CEffTeam::GetEffectObjs(list<CMoveShape*>& listObjs)
{
    if ( m_strNum!="" )
    {
        /// by:expter 2010/08/31 技能增加一个参数
        m_nNum = m_pParam->pFirer->GetSkillValue(m_pParam->nID,m_pParam->nLvl,m_strNum);
    }
    if( m_nNum < 0 )
        m_nNum = 0;

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
			CBaseObject *pObj = pRegion->FindChildObject(TYPE_PLAYER,*it);// GameManager::GetInstance()->FindObject(TYPE_PLAYER,*it);
			CMoveShape* pMoveShape = dynamic_cast<CMoveShape*>(pObj);
			if(pMoveShape && (m_pCondiGroup || m_pCondiGroup->Result(pMoveShape,m_pParam) ) && (Distance(m_pParam->pUser->GetTileX(),m_pParam->pUser->GetTileY(),pMoveShape->GetTileX(),pMoveShape->GetTileY())<=m_nDistance))
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
	}
	else
	{
		nNum = 1;
		listObjs.push_back(m_pParam->pUser);
	}
		
	return nNum;
}
