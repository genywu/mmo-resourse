#include "StdAfx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"

#include "commdb.h"
#include "map.h"
#include "cmdmapfillmtl.h"

CmdMapFillMtl::CmdMapFillMtl(void)
{
	m_pGridOldMtlArray = NULL;
}

CmdMapFillMtl::~CmdMapFillMtl(void)
{
	SAFEDELETEARRAY(m_pGridOldMtlArray);
}

void CmdMapFillMtl::AddGridIndex(DWORD dwGridIndex)
{
	m_vGridIndexArray.push_back(dwGridIndex);
	m_pGridOldMtlArray = new DWORD[m_vGridIndexArray.size()];
}

void CmdMapFillMtl::Execute()
{
	Cmd::Execute();
	//填充所有
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	for(int i = 0; i < (int)m_vGridIndexArray.size(); i++)
	{
		DWORD dwGridIndex = m_vGridIndexArray[i];
		Grid * pGrid = pMap->GetGrid(dwGridIndex);
		m_pGridOldMtlArray[i] = pGrid->GetMtl();
        pGrid->SetMtl(m_dwMtl);
	}
}

void CmdMapFillMtl::UnExecute()
{
	Cmd::UnExecute();
	//恢复所有
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	for(int i = 0; i < (int)m_vGridIndexArray.size(); i++)
	{
		DWORD dwGridIndex = m_vGridIndexArray[i];
		Grid * pGrid = pMap->GetGrid(dwGridIndex);
		DWORD dwOldMtl = m_pGridOldMtlArray[i];
		pGrid->SetMtl(dwOldMtl);
	}
	
}

void CmdMapFillMtl::Dump()
{
	OutputConsole("Map_Fill_Mtl: %d %d",m_vGridIndexArray.size(),m_dwMtl);
}