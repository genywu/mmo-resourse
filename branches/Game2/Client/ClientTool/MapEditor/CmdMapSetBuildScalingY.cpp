#include "stdafx.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"

#include "CmdMapSetBuildScalingY.h"
#include "commdb.h"
#include "map.h"


CmdMapSetBuildScalingY::CmdMapSetBuildScalingY(void)
{
}

CmdMapSetBuildScalingY::~CmdMapSetBuildScalingY(void)
{
}

void CmdMapSetBuildScalingY::SetBuildScalingY(DWORD dwGridIndex, float fScalingY)
{
	m_dwGridIndex = dwGridIndex;
	m_fScalingY = fScalingY;
}

bool CmdMapSetBuildScalingY::Execute(void)
{
	//±£´æ¾ÉµÄ
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);

	m_fOldScalingY = pGrid->GetBuild()->vScaling.y;
	if (m_fOldScalingY == m_fScalingY)
		return false;

	pGrid->SetBuildScalingY(m_fScalingY);

	Cmd::Execute();
	return true;
}

void CmdMapSetBuildScalingY::UnExecute(void)
{
	Cmd::UnExecute();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);
	pGrid->SetBuildScalingX(m_fOldScalingY);
}

void CmdMapSetBuildScalingY::Dump(void)
{
	OutputConsole("Map_Set_BuildScalingY: %d %f",m_dwGridIndex, m_fScalingY);
}		