#include "StdAfx.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"

#include "commdb.h"
#include "map.h"
#include "cmdmapfillBlock.h"

CmdMapFillBlock::CmdMapFillBlock(void)
{
}

CmdMapFillBlock::~CmdMapFillBlock(void)
{
}

void CmdMapFillBlock::AddGridIndex(DWORD dwGridIndex)
{
	m_vGridIndexArray.push_back(dwGridIndex);
}

bool CmdMapFillBlock::Execute()
{
	Cmd::Execute();

	//填充所有
	m_vGridOldBlockArray.clear();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	for(int i = 0; i <(int)m_vGridIndexArray.size(); i++)
	{
		DWORD dwGridIndex = m_vGridIndexArray[i];
		Grid * pGrid = pMap->GetGrid(dwGridIndex);
		m_vGridOldBlockArray.push_back(pGrid->GetBlock());
        pGrid->SetBlock(m_dwBlock);
	}

	return true;
}

void CmdMapFillBlock::UnExecute()
{
	Cmd::UnExecute();
	//恢复所有
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	for(int i = 0; i < (int)m_vGridIndexArray.size(); i++)
	{
		DWORD dwGridIndex = m_vGridIndexArray[i];
		Grid * pGrid = pMap->GetGrid(dwGridIndex);
		DWORD dwOldBlock = m_vGridOldBlockArray[i];
		pGrid->SetBlock(dwOldBlock);
	}
}

void CmdMapFillBlock::Dump()
{
	OutputConsole("Map_Fill_Block: %d %d",m_vGridIndexArray.size(),m_dwBlock);
}