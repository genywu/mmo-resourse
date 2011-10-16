#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"

#include "CmdMapSetBuildRotationY.h"
#include "commdb.h"
#include "map.h"


CmdMapSetBuildRotationY::CmdMapSetBuildRotationY(void)
{
}

CmdMapSetBuildRotationY::~CmdMapSetBuildRotationY(void)
{
}

void CmdMapSetBuildRotationY::SetBuildRotationY(DWORD dwGridIndex,float fBuildRotationY)
{
	m_dwGridIndex = dwGridIndex;
	m_fBuildRotationY	= fBuildRotationY;
}

void CmdMapSetBuildRotationY::Execute(void)
{
	Cmd::Execute();
	//±£´æ¾ÉµÄ
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);

	m_fOldBuildRotationY = pGrid->GetBuild()->fRotationY;
	pGrid->SetBuildRotationY(m_fBuildRotationY);
}

void CmdMapSetBuildRotationY::UnExecute(void)
{
	Cmd::UnExecute();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);
	pGrid->SetBuildRotationY(m_fOldBuildRotationY);
}

void CmdMapSetBuildRotationY::Dump(void)
{
	OutputConsole("Map_Set_BuildRotationY: %d %d",m_dwGridIndex,m_fBuildRotationY);
}		