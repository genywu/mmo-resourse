
#include "stdafx.h"

#include "..\..\..\FrostEngine\utility.h"
#include "..\..\..\FrostEngine\console.h"
#include "..\..\..\FrostEngine\math.h"

#include "..\..\..\GameEngine\gamemap.h"
#include "..\..\..\GameEngine\gameselectray.h"
#include "..\..\..\GameEngine\DisplayModel.h"
#include "..\..\..\gameengine\gamemodelmanager.h"


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

void CBindModel::SaveShowState()
{
	CGameModelManager * pModelManager = CGameModelManager::GetInstance();

	typedef map<DWORD, CDisplayModel::CDisplayGroup *>::iterator itDisplayGroup;

	stGPL st;
	itDisplayGroup it = m_pDisplayModel->GetGroups().begin();
	for (; it != m_pDisplayModel->GetGroups().end(); ++it)
	{
		st.dwGroup = it->first;
		st.dwPart = it->second->GetPartName();
		st.dwLevel = it->second->GetPartLevel();

		m_vecGPLs.push_back(st);
	}
}

void CBindModel::SetShowState()
{
	m_pDisplayModel->HideAllGroup();

	for (size_t st = 0; st < m_vecGPLs.size(); ++st)
		m_pDisplayModel->ShowGroup(m_vecGPLs[st].dwGroup, m_vecGPLs[st].dwPart, m_vecGPLs[st].dwLevel, true);

	m_vecGPLs.clear();
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
		m_pAnimInfo->SetViewMatrix(pParentAnimInfo->GetViewMatrix());
		m_pAnimInfo->SetProjectedMatrix(pParentAnimInfo->GetProjectedMatrix());
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