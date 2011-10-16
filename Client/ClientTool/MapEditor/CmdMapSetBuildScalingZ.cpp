#include "stdafx.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"

#include "CmdMapSetBuildScalingZ.h"
#include "commdb.h"
#include "map.h"


CmdMapSetBuildScalingZ::CmdMapSetBuildScalingZ(void)
{
}

CmdMapSetBuildScalingZ::~CmdMapSetBuildScalingZ(void)
{
}

void CmdMapSetBuildScalingZ::SetBuildScalingZ(DWORD dwGridIndex, float fScalingZ)
{
	m_dwGridIndex = dwGridIndex;
	m_fScalingZ = fScalingZ;
}

bool CmdMapSetBuildScalingZ::Execute(void)
{
	//±£´æ¾ÉµÄ
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);

	m_fOldScalingZ = pGrid->GetBuild()->vScaling.z;
	if (m_fOldScalingZ == m_fScalingZ)
		return false;

	pGrid->SetBuildScalingZ(m_fScalingZ);

	Cmd::Execute();
	return true;
}

void CmdMapSetBuildScalingZ::UnExecute(void)
{
	Cmd::UnExecute();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);
	pGrid->SetBuildScalingX(m_fOldScalingZ);
}

void CmdMapSetBuildScalingZ::Dump(void)
{
	OutputConsole("Map_Set_BuildScalingZ: %d %f",m_dwGridIndex, m_fScalingZ);
}		