#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"

#include "CmdMapSetDoor.h"
#include "commdb.h"
#include "map.h"


CmdMapSetDoor::CmdMapSetDoor(void)
{
}

CmdMapSetDoor::~CmdMapSetDoor(void)
{
}

void CmdMapSetDoor::SetDoorIndex(DWORD dwGridIndex,DWORD dwDoorIndex)
{
	m_dwGridIndex = dwGridIndex;
	m_dwDoorIndex	= dwDoorIndex;
}

void CmdMapSetDoor::Execute(void)
{
	Cmd::Execute();
	//±£´æ¾ÉµÄ
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);

	m_dwOldDoorIndex = pGrid->GetDoorIndex();
	pGrid->SetDoorIndex(m_dwDoorIndex);
}

void CmdMapSetDoor::UnExecute(void)
{
	Cmd::UnExecute();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);
	pGrid->SetDoorIndex(m_dwOldDoorIndex);
}

void CmdMapSetDoor::Dump(void)
{
	OutputConsole("Map_Set_DoorIndex: %d %d",m_dwGridIndex,m_dwDoorIndex);
}		