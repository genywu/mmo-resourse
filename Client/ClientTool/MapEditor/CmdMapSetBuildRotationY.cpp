#include "stdafx.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"

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
	m_fBuildRotationY = fBuildRotationY;
}

bool CmdMapSetBuildRotationY::Execute(void)
{
	//±£´æ¾ÉµÄ
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);

	m_fOldBuildRotationY = pGrid->GetBuild()->vRotation.y;
	if (m_fOldBuildRotationY == m_fBuildRotationY)
		return false;

	pGrid->SetBuildRotationY(m_fBuildRotationY);

	Cmd::Execute();
	return true;
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
	OutputConsole("Map_Set_BuildRotationY: %d %f",m_dwGridIndex,m_fBuildRotationY);
}		