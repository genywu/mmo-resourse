#include "stdafx.h"

#include "..\..\..\FrostEngine\utility.h"
#include "..\..\..\FrostEngine\console.h"
#include "..\..\..\FrostEngine\math.h"

#include "..\..\..\GameEngine\gamemap.h"
#include "..\..\..\GameEngine\gameselectray.h"
#include "..\..\..\GameEngine\DisplayModel.h"

#include "Weapon.h"
#include "..\..\..\..\public\basedef.h"
//#include "clientregion.h"

CWeapon::CWeapon()
{
	ChangeStatus(WEAPON_ACTION0);
	SetType(TYPE_GOODS);
	m_dwWeaponType = 0;
}

CWeapon::~CWeapon()
{
}

void CWeapon::Copy(CWeapon* psequip)
{
	
	CSpecialEquip::Copy(psequip);
	m_dwWeaponType = psequip->m_dwWeaponType;
	
}

void CWeapon::ChangeStatus(DWORD eStatus)
{
	DWORD dwCurTime = timeGetTime();
	bool bchangestate = false;
	if ( m_eStatus != eStatus )
	{
		m_pAnimInfo->GetActionInfo()->dwOldAction = m_pAnimInfo->GetActionInfo()->dwCurAction;
		m_pAnimInfo->GetActionInfo()->dwOldActionTimeElapse = m_pAnimInfo->GetActionInfo()->dwCurActionElapseTime;

		m_pAnimInfo->GetActionInfo()->dwCurAction = eStatus;

		m_pAnimInfo->GetActionInfo()->bInterpolation = FALSE;
		m_pAnimInfo->GetActionInfo()->dwInterpolationStartTime = dwCurTime;
		m_pAnimInfo->GetActionInfo()->dwInterpolationElapseTime = 0;
		m_pAnimInfo->GetActionInfo()->dwInterpolationTimeLength = 240;

		m_pAnimInfo->GetActionInfo()->bCurActionLooped = TRUE;
		m_pAnimInfo->GetActionInfo()->dwCurActionLoopCount = 0;
		m_pAnimInfo->GetActionInfo()->dwCurActionStartTime = dwCurTime;
		bchangestate = true;
	}
	if (bchangestate)
	{
		m_eStatus = eStatus;
	}
}