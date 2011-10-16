
#include "stdafx.h"

#include "..\..\..\FrostEngine\utility.h"
#include "..\..\..\FrostEngine\console.h"
#include "..\..\..\FrostEngine\math.h"

#include "..\..\..\GameEngine\gamemap.h"
#include "..\..\..\GameEngine\gameselectray.h"
#include "..\..\..\GameEngine\DisplayModel.h"
#include "..\..\..\gameengine\gamemodelmanager.h"


#include "RoleModel.h"
#include "..\..\..\..\public\basedef.h"
//#include "clientregion.h"

CRoleModel::CRoleModel()
{
	m_iCurrentSelActionIndex = -1;
	m_pDisplayModel			 = NULL;
}

CRoleModel::~CRoleModel()
{
}

void CRoleModel::SaveShowState()
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

void CRoleModel::SetShowState()
{
	m_pDisplayModel->HideAllGroup();

	for (size_t st = 0; st < m_vecGPLs.size(); ++st)
		m_pDisplayModel->ShowGroup(m_vecGPLs[st].dwGroup, m_vecGPLs[st].dwPart, m_vecGPLs[st].dwLevel, true);

	m_vecGPLs.clear();
}