#include "stdafx.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"

#include "CmdMapSetBuildScaling.h"
#include "commdb.h"
#include "map.h"


CmdMapSetBuildScaling::CmdMapSetBuildScaling(void)
{
}

CmdMapSetBuildScaling::~CmdMapSetBuildScaling(void)
{
}

void CmdMapSetBuildScaling::SetBuildScaling(DWORD dwGridIndex, D3DXVECTOR3 & vBuildScaling)
{
	m_dwGridIndex = dwGridIndex;
	m_vBuildScaling = vBuildScaling;
}

bool CmdMapSetBuildScaling::Execute(void)
{
	//±£´æ¾ÉµÄ
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);

	m_vOldBuildScaling = pGrid->GetBuild()->vScaling;
	if (m_vOldBuildScaling == m_vBuildScaling)
		return false;

	pGrid->SetBuildScaling(m_vBuildScaling);

	Cmd::Execute();
	return true;
}

void CmdMapSetBuildScaling::UnExecute(void)
{
	Cmd::UnExecute();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);
	pGrid->SetBuildScaling(m_vOldBuildScaling);
}

void CmdMapSetBuildScaling::Dump(void)
{
	OutputConsole("Map_Set_BuildScaling: %d (%f, %f, %f)",m_dwGridIndex, m_vBuildScaling.x, m_vBuildScaling.y, m_vBuildScaling.z);
}		