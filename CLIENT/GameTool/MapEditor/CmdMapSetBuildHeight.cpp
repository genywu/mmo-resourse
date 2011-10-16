#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"

#include "CmdMapSetBuildHeight.h"
#include "commdb.h"
#include "map.h"


CmdMapSetBuildHeight::CmdMapSetBuildHeight(void)
{
}

CmdMapSetBuildHeight::~CmdMapSetBuildHeight(void)
{
}

void CmdMapSetBuildHeight::MapSetBuildHeight(DWORD dwGridIndex,float fBuildHeight)
{
	m_dwGridIndex = dwGridIndex;
	m_fBuildHeight	= fBuildHeight;
}

void CmdMapSetBuildHeight::Execute(void)
{
	Cmd::Execute();
	//±£´æ¾ÉµÄ
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);

	m_fOldBuildHeight = pGrid->GetBuild()->fHeight;
	pGrid->SetBuildHeight(m_fBuildHeight);
}

void CmdMapSetBuildHeight::UnExecute(void)
{
	Cmd::UnExecute();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);
	pGrid->SetBuildHeight(m_fOldBuildHeight);
}

void CmdMapSetBuildHeight::Dump(void)
{
	OutputConsole("Map_Set_BuildHeight: %d %d",m_dwGridIndex,m_fBuildHeight);
}		