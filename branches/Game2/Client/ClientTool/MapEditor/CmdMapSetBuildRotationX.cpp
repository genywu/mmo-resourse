#include "stdafx.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"

#include "CmdMapSetBuildRotationX.h"
#include "commdb.h"
#include "map.h"


CmdMapSetBuildRotationX::CmdMapSetBuildRotationX(void)
{
}

CmdMapSetBuildRotationX::~CmdMapSetBuildRotationX(void)
{
}

void CmdMapSetBuildRotationX::SetBuildRotationX(DWORD dwGridIndex,float fBuildRotationX)
{
	m_dwGridIndex = dwGridIndex;
	m_fBuildRotationX = fBuildRotationX;
}

bool CmdMapSetBuildRotationX::Execute(void)
{
	//±£´æ¾ÉµÄ
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);

	m_fOldBuildRotationX = pGrid->GetBuild()->vRotation.x;
	if (m_fOldBuildRotationX == m_fBuildRotationX)
		return false;

	pGrid->SetBuildRotationX(m_fBuildRotationX);

	Cmd::Execute();
	return true;
}

void CmdMapSetBuildRotationX::UnExecute(void)
{
	Cmd::UnExecute();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);
	pGrid->SetBuildRotationX(m_fOldBuildRotationX);
}

void CmdMapSetBuildRotationX::Dump(void)
{
	OutputConsole("Map_Set_BuildRotationX: %d %f",m_dwGridIndex,m_fBuildRotationX);
}