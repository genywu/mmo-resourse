
#include "stdafx.h"

#include "..\..\..\FrostEngine\utility.h"
#include "..\..\..\FrostEngine\console.h"
#include "..\..\..\FrostEngine\math.h"

#include "..\..\..\GameEngine\gamemap.h"
#include "..\..\..\GameEngine\gameselectray.h"
#include "..\..\..\GameEngine\DisplayModel.h"


#include "Wing.h"
#include "..\..\..\..\public\basedef.h"
//#include "clientregion.h"


CWing::CWing()
{
	ChangeStatus(WING_IDLE);
	SetType(TYPE_GOODS);
}

CWing::~CWing()
{
	
}

void CWing::ChangeStatus(DWORD eStatus)
{
	if(NULL == m_pDisplayModel)	return;
	
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

