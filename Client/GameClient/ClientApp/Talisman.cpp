
#include <stdafx.h>

#include "../../Engine\utility.h"
#include "../../Engine\console.h"
#include "../../Engine\math.h"

#include "../../EngineExtend\gamemap.h"
#include "../../EngineExtend\gameselectray.h"

#include "Talisman.h"
#include "clientregion.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTalisman::CTalisman()
{
	ChangeStatus(TS_IDLE);
}

CTalisman::~CTalisman()
{
	
}


void CTalisman::ChangeStatus(DWORD eStatus)
{
	/*DWORD dwCurTime = timeGetTime();
	bool bchangestate = FALSE;
	switch(eStatus)
	{
	case TS_IDLE:
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
	case TS_STAND:
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
	case TS_WALK:
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
	case TS_RUN:
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
	case TS_HURT:
		if ( m_eStatus == eStatus )
			break;
		m_pAnimInfo->GetActionInfo()->dwCurAction = MAKEFOURCC('H','U','R','T');
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
	}*/	
}




