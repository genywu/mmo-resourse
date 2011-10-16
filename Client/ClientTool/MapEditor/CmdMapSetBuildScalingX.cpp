#include "stdafx.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"

#include "CmdMapSetBuildScalingX.h"
#include "commdb.h"
#include "map.h"


CmdMapSetBuildScalingX::CmdMapSetBuildScalingX(void)
{
}

CmdMapSetBuildScalingX::~CmdMapSetBuildScalingX(void)
{
}

void CmdMapSetBuildScalingX::SetBuildScalingX(DWORD dwGridIndex, float fScalingX)
{
	m_dwGridIndex = dwGridIndex;
	m_fScalingX = fScalingX;
}

bool CmdMapSetBuildScalingX::Execute(void)
{
	//±£´æ¾ÉµÄ
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);

	m_fOldScalingX = pGrid->GetBuild()->vScaling.x;
	if (m_fOldScalingX == m_fScalingX)
		return false;

	pGrid->SetBuildScalingX(m_fScalingX);

	Cmd::Execute();
	return true;
}

void CmdMapSetBuildScalingX::UnExecute(void)
{
	Cmd::UnExecute();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);
	pGrid->SetBuildScalingX(m_fOldScalingX);
}

void CmdMapSetBuildScalingX::Dump(void)
{
	OutputConsole("Map_Set_BuildScalingX: %d %f",m_dwGridIndex, m_fScalingX);
}		