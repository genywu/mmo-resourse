#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"

#include "commdb.h"
#include "map.h"
#include "CmdMapDelDoor.h"

CmdMapDelDoor::CmdMapDelDoor(void)
{
	m_pGridOldDoorIndexArray = NULL;
}

CmdMapDelDoor::~CmdMapDelDoor(void)
{
	SAFEDELETEARRAY(m_pGridOldDoorIndexArray);
}

void CmdMapDelDoor::AddGridIndex(DWORD dwGridIndex)
{
	m_vGridIndexArray.push_back(dwGridIndex);
	m_pGridOldDoorIndexArray = new DWORD[m_vGridIndexArray.size()];
}

void CmdMapDelDoor::Execute()
{
	Cmd::Execute();
	//填充所有,删除
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	for(int i = 0; i < (int)m_vGridIndexArray.size(); i++)
	{
		DWORD dwGridIndex = m_vGridIndexArray[i];
		Grid * pGrid = pMap->GetGrid(dwGridIndex);
		m_pGridOldDoorIndexArray[i] = pGrid->GetDoorIndex();//0为空
        pGrid->SetDoorIndex(0);
	}
}

void CmdMapDelDoor::UnExecute()
{
	Cmd::UnExecute();
	//恢复所有
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	for(int i = 0; i < (int)m_vGridIndexArray.size(); i++)
	{
		DWORD dwGridIndex = m_vGridIndexArray[i];
		Grid * pGrid = pMap->GetGrid(dwGridIndex);
		DWORD dwOldDoorIndex = m_pGridOldDoorIndexArray[i];
		pGrid->SetDoorIndex(dwOldDoorIndex);
	}
	
}

void CmdMapDelDoor::Dump()
{
	OutputConsole("Map_Del_Door: %d ",m_vGridIndexArray.size());
}