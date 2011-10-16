
#include <stdafx.h>

#include "../../Engine\utility.h"
#include "../../Engine\console.h"
#include "../../Engine\math.h"

#include "../../EngineExtend/GameMap.h"
#include "../../EngineExtend/GameSelectray.h"
#include "../../EngineExtend/DisplayModel.h"
#include "../../../Public/Common/BaseDef.h"
#include "Wing.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CWing::CWing()
{
	ChangeStatus(WING_IDLE);
	SetType(TYPE_GOODS);
}

CWing::~CWing()
{

}
void CWing::Display(CShape * pParentObj)
{
	if (!m_bVisiable || pParentObj == NULL || pParentObj->GetDisplayModel() == NULL || m_pDisplayModel == NULL) return;
	if( m_pDisplayModel->GetLoadState() != Load_Did ||pParentObj->GetDisplayModel()->GetLoadState() != Load_Did) return;
	m_pDisplayModel->SetRendSimpleMesh(m_bsimplemesh);
	CSpecialEquip::Display(pParentObj);
}
void CWing::ChangeStatus(DWORD eStatus)
{
	//if(NULL == m_pDisplayModel)	return;

	DWORD dwCurTime = timeGetTime();
	bool bchangestate = false;
	switch(eStatus)
	{
	case WING_IDLE:
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
	case WING_STAND:
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
	case WING_WALK:
		if ( m_eStatus == eStatus )
			break;
		m_pAnimInfo->GetActionInfo()->dwOldAction = m_pAnimInfo->GetActionInfo()->dwCurAction;
		m_pAnimInfo->GetActionInfo()->dwOldActionTimeElapse = m_pAnimInfo->GetActionInfo()->dwCurActionElapseTime;

		m_pAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('W','A','L','K');
		m_pAnimInfo->GetActionInfo()->bInterpolation = FALSE;
		m_pAnimInfo->GetActionInfo()->dwInterpolationElapseTime = 0;
		m_pAnimInfo->GetActionInfo()->dwInterpolationTimeLength = 240;
		m_pAnimInfo->GetActionInfo()->bCurActionLooped = TRUE;
		m_pAnimInfo->GetActionInfo()->dwCurActionLoopCount = 0;
		m_pAnimInfo->GetActionInfo()->dwCurActionStartTime = dwCurTime;
		bchangestate = TRUE;
		break;
	case WING_RUN:
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
	}
	if (bchangestate)
	{
		m_eStatus = eStatus;
	}	
}

