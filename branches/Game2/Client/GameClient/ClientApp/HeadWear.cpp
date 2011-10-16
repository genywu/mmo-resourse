
#include <stdafx.h>

#include "../../Engine\utility.h"
#include "../../Engine\console.h"
#include "../../Engine\math.h"

#include "../../EngineExtend\gamemap.h"
#include "../../EngineExtend\gameselectray.h"
#include "../../EngineExtend\DisplayModel.h"
#include "../../GameClient/Game.h"
#include "HeadWear.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHeadWear::CHeadWear()
{
	ChangeStatus(HeadWear_IDLE);
	SetType(TYPE_GOODS);
}

CHeadWear::~CHeadWear()
{

}
void CHeadWear::Display(CShape * pParentObj)
{
	if (!m_bsimplemesh)
	{
		if (!m_bVisiable || pParentObj == NULL || pParentObj->GetDisplayModel() == NULL || m_pDisplayModel == NULL)
			return;
		if (pParentObj->GetDisplayModel()->GetLoadState() != Load_Did || m_pDisplayModel->GetLoadState() != Load_Did )
			return;

		m_pDisplayModel->GetAnimInfobyLocator(m_pAnimInfo, pParentObj->GetDisplayModel(), pParentObj->GetAnimInfo(), m_dwLocator);
		//m_pDisplayModel->Display(m_pAnimInfo);
		m_pDisplayModel->SetDirLightEnable(GetGame()->GetSetup()->lLight>0);
		m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);
		m_pDisplayModel->ProcessVisibility(m_pAnimInfo);
		m_pDisplayModel->ProcessAnimUV(m_pAnimInfo);
		m_pDisplayModel->ProcessAnimTexture(m_pAnimInfo);
		m_pDisplayModel->ProcessEnvironmentMaps(pParentObj->GetAnimInfo()->GetViewMatrix());
		m_pDisplayModel->ProcessAnimColor(m_pAnimInfo);
		m_pDisplayModel->ProcessLocator(m_pAnimInfo);
		m_pDisplayModel->ProcessParticleEmitter(m_pAnimInfo);
		m_pDisplayModel->ProcessTrailEmitter(m_pAnimInfo);
		m_pDisplayModel->ProcessTextureProjective(m_pAnimInfo);

		// 处理模型渐变效果
		if (pParentObj->GetIsNeedAlphaFade() && pParentObj->GetAlphaFadeMode() != CAlphaFadeObjectList::ALPHA_FADE_NONE)
		{
			if (pParentObj->GetAlphaFadeMode() == CAlphaFadeObjectList::ALPHA_FADE_IN)
			{
				long lElapseTime = timeGetTime() - pParentObj->GetAlphaFadeInStartTime();
				m_pDisplayModel->MultiplyModelAlpha((float)lElapseTime / (float)pParentObj->GetAlphaFadeInTimeLength());
				m_pDisplayModel->RenderModelTransparent();
			}else if (pParentObj->GetAlphaFadeMode() == CAlphaFadeObjectList::ALPHA_FADE_OUT)
			{
				m_pDisplayModel->MultiplyModelAlpha(pParentObj->GetAlphaValue());
				m_pDisplayModel->RenderModelTransparent();
			}
		}else
			m_pDisplayModel->RenderModel();
		//CSpecialEquip::Display(pParentObj);
	}
}
void CHeadWear::ChangeStatus(DWORD eStatus)
{
	//if(NULL == m_pDisplayModel)	return;

	DWORD dwCurTime = timeGetTime();
	bool bchangestate = false;
	switch(eStatus)
	{
	case HeadWear_IDLE:
		if ( m_eStatus == eStatus )
			break;
		m_pAnimInfo->GetActionInfo()->dwOldAction = m_pAnimInfo->GetActionInfo()->dwCurAction;
		m_pAnimInfo->GetActionInfo()->dwOldActionTimeElapse = m_pAnimInfo->GetActionInfo()->dwCurActionElapseTime;

		m_pAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('I','D','L','E');

		m_pAnimInfo->GetActionInfo()->bInterpolation = FALSE;
		m_pAnimInfo->GetActionInfo()->dwInterpolationStartTime = dwCurTime;
		m_pAnimInfo->GetActionInfo()->dwInterpolationElapseTime = 0;
		m_pAnimInfo->GetActionInfo()->dwInterpolationTimeLength = 240;

		m_pAnimInfo->GetActionInfo()->bCurActionLooped = TRUE;
		m_pAnimInfo->GetActionInfo()->dwCurActionLoopCount = 0;
		m_pAnimInfo->GetActionInfo()->dwCurActionStartTime = dwCurTime;
		bchangestate = TRUE;
		break;
	case HeadWear_STAND:
		if ( m_eStatus == eStatus )
			break;
		m_pAnimInfo->GetActionInfo()->dwOldAction = m_pAnimInfo->GetActionInfo()->dwCurAction;
		m_pAnimInfo->GetActionInfo()->dwOldActionTimeElapse = m_pAnimInfo->GetActionInfo()->dwCurActionElapseTime;


		m_pAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('S','T','N','D');

		m_pAnimInfo->GetActionInfo()->bInterpolation = FALSE;
		m_pAnimInfo->GetActionInfo()->dwInterpolationStartTime = dwCurTime;
		m_pAnimInfo->GetActionInfo()->dwInterpolationElapseTime = 0;
		m_pAnimInfo->GetActionInfo()->dwInterpolationTimeLength = 240;

		m_pAnimInfo->GetActionInfo()->bCurActionLooped = TRUE;
		m_pAnimInfo->GetActionInfo()->dwCurActionLoopCount = 0;
		m_pAnimInfo->GetActionInfo()->dwCurActionStartTime = dwCurTime;
		bchangestate = TRUE;
		break;
	case HeadWear_WALK:
		if ( m_eStatus == eStatus )
			break;
		m_pAnimInfo->GetActionInfo()->dwOldAction = m_pAnimInfo->GetActionInfo()->dwCurAction;
		m_pAnimInfo->GetActionInfo()->dwOldActionTimeElapse = m_pAnimInfo->GetActionInfo()->dwCurActionElapseTime;

		m_pAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('R','U','N','0');
		m_pAnimInfo->GetActionInfo()->bInterpolation = FALSE;
		m_pAnimInfo->GetActionInfo()->dwInterpolationElapseTime = 0;
		m_pAnimInfo->GetActionInfo()->dwInterpolationTimeLength = 240;
		m_pAnimInfo->GetActionInfo()->bCurActionLooped = TRUE;
		m_pAnimInfo->GetActionInfo()->dwCurActionLoopCount = 0;
		m_pAnimInfo->GetActionInfo()->dwCurActionStartTime = dwCurTime;
		bchangestate = TRUE;
		break;
	case HeadWear_RUN:
		if ( m_eStatus == eStatus )
			break;
		m_pAnimInfo->GetActionInfo()->dwOldAction = m_pAnimInfo->GetActionInfo()->dwCurAction;
		m_pAnimInfo->GetActionInfo()->dwOldActionTimeElapse = m_pAnimInfo->GetActionInfo()->dwCurActionElapseTime;

		m_pAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('R','U','N','1');
		m_pAnimInfo->GetActionInfo()->bInterpolation = FALSE;
		m_pAnimInfo->GetActionInfo()->dwInterpolationElapseTime = 0;
		m_pAnimInfo->GetActionInfo()->dwInterpolationTimeLength = 240;
		m_pAnimInfo->GetActionInfo()->bCurActionLooped = TRUE;
		m_pAnimInfo->GetActionInfo()->dwCurActionLoopCount = 0;
		m_pAnimInfo->GetActionInfo()->dwCurActionStartTime = dwCurTime;
		bchangestate = TRUE;
		break;
	}
	if (bchangestate)
	{
		m_eStatus = eStatus;
	}	
}

