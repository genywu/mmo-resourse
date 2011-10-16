
#include "stdafx.h"

#include "..\..\..\FrostEngine\utility.h"
#include "..\..\..\FrostEngine\console.h"
#include "..\..\..\FrostEngine\math.h"

#include "..\..\..\GameEngine\gamemap.h"
#include "..\..\..\GameEngine\gameselectray.h"
#include "..\..\..\GameEngine\DisplayModel.h"


#include "BindModel.h"
#include "..\..\..\..\public\basedef.h"

CBindModel::CBindModel()
{
	m_iCurrentSelLocatorIndex = -1;
	m_iCurrentSelActionIndex  = 0;
	ChangeStatus(WEAPON_ACTION0);
}

CBindModel::~CBindModel()
{	
}

void CBindModel::ChangeStatus(DWORD eStatus)
{
	DWORD					  dwCurTime	   = timeGetTime();
	bool					  bchangestate = false;
	AnimInfo::tagActionInfo * pActionInfo  = m_pAnimInfo->GetActionInfo();

	if (m_eStatus != eStatus)
	{
		pActionInfo->dwOldAction		   = pActionInfo->dwCurAction;
		pActionInfo->dwOldActionTimeElapse = pActionInfo->dwCurActionElapseTime;
		pActionInfo->dwCurAction		   = eStatus;
		pActionInfo->bCurActionLooped	   = TRUE;
		pActionInfo->dwCurActionLoopCount  = 0;
		pActionInfo->dwCurActionStartTime  = dwCurTime;

		pActionInfo->bInterpolation			   = FALSE;
		pActionInfo->dwInterpolationStartTime  = dwCurTime;
		pActionInfo->dwInterpolationElapseTime = 0;
		pActionInfo->dwInterpolationTimeLength = 240;

		bchangestate = true;
	}

	if (bchangestate)
		m_eStatus = eStatus;
}

void CBindModel::Display(CDisplayModel * pParentModel, AnimInfo * pParentAnimInfo)
{
	if (!m_bVisiable ||	pParentModel == NULL ||	m_pDisplayModel == NULL)
		return;

	if (m_pDisplayModel->GetLoadState() != Load_Did )
		return;

	if (m_iCurrentSelLocatorIndex == 23)
	{
		D3DXMATRIX matWorld;
		D3DXMatrixTranslation(&matWorld, 0.0f, 0.0f, 0.0f);
		D3DXMATRIX matRot;
		D3DXMatrixRotationY(&matRot, D3DX_PI);
		D3DXMatrixMultiply(&matWorld, &matRot, &matWorld);

		m_pAnimInfo->SetWorldMatrix(&matWorld);
		m_pAnimInfo->SetBillboard(pParentAnimInfo->GetBillboard());
		m_pAnimInfo->SetBillboardY(pParentAnimInfo->GetBillboardY());
		m_pAnimInfo->SetCurrentTime(pParentAnimInfo->GetCurrentTime());

		m_pDisplayModel->ProcessAnimJoint(m_pAnimInfo);
		m_pDisplayModel->ProcessVisibility(m_pAnimInfo);
		m_pDisplayModel->ProcessAnimUV(m_pAnimInfo);
		m_pDisplayModel->ProcessAnimTexture(m_pAnimInfo);
		m_pDisplayModel->ProcessAnimColor(m_pAnimInfo);
		m_pDisplayModel->ProcessParticleEmitter(m_pAnimInfo);
		m_pDisplayModel->ProcessTrailEmitter(m_pAnimInfo);
		m_pDisplayModel->ProcessTextureProjective(m_pAnimInfo);
		m_pDisplayModel->RenderModel();
	}else
		CSpecialEquip::Display(pParentModel, pParentAnimInfo);
}