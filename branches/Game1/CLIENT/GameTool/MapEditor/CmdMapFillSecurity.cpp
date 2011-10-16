#include "StdAfx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"

#include "commdb.h"
#include "map.h"
#include "cmdmapfillSecurity.h"

CmdMapFillSecurity::CmdMapFillSecurity(void)
{
}

CmdMapFillSecurity::~CmdMapFillSecurity(void)
{
}

void CmdMapFillSecurity::AddGridIndex(DWORD dwGridIndex)
{
	m_vGridIndexArray.push_back(dwGridIndex);

}

void CmdMapFillSecurity::Execute()
{
	Cmd::Execute();
	//填充所有
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	for(int i = 0; i < (int)m_vGridIndexArray.size(); i++)
	{
		DWORD dwGridIndex = m_vGridIndexArray[i];
		Grid * pGrid = pMap->GetGrid(dwGridIndex);
		m_vGridOldSecurityArray.push_back(pGrid->GetSecurity());
        pGrid->SetSecurity(m_dwSecurity);
	}
}

void CmdMapFillSecurity::UnExecute()
{
	Cmd::UnExecute();
	//恢复所有
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	for(int i = 0; i < (int)m_vGridIndexArray.size(); i++)
	{
		DWORD dwGridIndex = m_vGridIndexArray[i];
		Grid * pGrid = pMap->GetGrid(dwGridIndex);
		DWORD dwOldSecurity = m_vGridOldSecurityArray[i];
		pGrid->SetSecurity(dwOldSecurity);
	}
	m_vGridOldSecurityArray.clear();
}

void CmdMapFillSecurity::Dump()
{
	OutputConsole("Map_Fill_Security: %d %d",m_vGridIndexArray.size(),m_dwSecurity);
}