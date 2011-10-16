#include "StdAfx.h"
#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"

#include "commdb.h"
#include "map.h"
#include "cmdmapfillgridvisible.h"

CmdMapFillGridVisible::CmdMapFillGridVisible(void)
{
}

CmdMapFillGridVisible::~CmdMapFillGridVisible(void)
{
}

void CmdMapFillGridVisible::AddGridIndex(DWORD dwGridIndex)
{
	m_vGridIndexArray.push_back(dwGridIndex);

}

void CmdMapFillGridVisible::Execute()
{
	Cmd::Execute();
	//填充所有
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	for(int i = 0; i < (int)m_vGridIndexArray.size(); i++)
	{
		DWORD dwGridIndex = m_vGridIndexArray[i];
		Grid * pGrid = pMap->GetGrid(dwGridIndex);
		m_vGridOldVisibleArray.push_back(pGrid->GetVisible());
		pGrid->SetVisible(m_bVisible);
	}
}																

void CmdMapFillGridVisible::UnExecute()
{
	Cmd::UnExecute();
	//恢复所有
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	for(int i = 0; i < (int)m_vGridIndexArray.size(); i++)
	{
		DWORD dwGridIndex = m_vGridIndexArray[i];
		Grid * pGrid = pMap->GetGrid(dwGridIndex);
		DWORD dwOldVisible = m_vGridOldVisibleArray[i];
		pGrid->SetVisible(dwOldVisible);
	}
	m_vGridOldVisibleArray.clear();
}

void CmdMapFillGridVisible::Dump()
{
	OutputConsole("Map_Fill_Visible: %d %d",m_vGridIndexArray.size(),m_bVisible);
}