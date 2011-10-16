#include "StdAfx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"

#include "commdb.h"
#include "map.h"
#include "cmdmapfillWar.h"

CmdMapFillWar::CmdMapFillWar(void)
{
}

CmdMapFillWar::~CmdMapFillWar(void)
{
}

void CmdMapFillWar::AddGridIndex(DWORD dwGridIndex)
{
	m_vGridIndexArray.push_back(dwGridIndex);

}

void CmdMapFillWar::Execute()
{
	Cmd::Execute();
	//填充所有
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	for(int i = 0; i < (int)m_vGridIndexArray.size(); i++)
	{
		DWORD dwGridIndex = m_vGridIndexArray[i];
		Grid * pGrid = pMap->GetGrid(dwGridIndex);
		m_vGridOldWarArray.push_back(pGrid->GetWar());
        pGrid->SetWar(m_dwWar);
	}
}

void CmdMapFillWar::UnExecute()
{
	Cmd::UnExecute();
	//恢复所有
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	for(int i = 0; i < (int)m_vGridIndexArray.size(); i++)
	{
		DWORD dwGridIndex = m_vGridIndexArray[i];
		Grid * pGrid = pMap->GetGrid(dwGridIndex);
		DWORD dwOldWar = m_vGridOldWarArray[i];
		pGrid->SetWar(dwOldWar);
	}
	m_vGridOldWarArray.clear();
}

void CmdMapFillWar::Dump()
{
	OutputConsole("Map_Fill_War: %d %d",m_vGridIndexArray.size(),m_dwWar);
}