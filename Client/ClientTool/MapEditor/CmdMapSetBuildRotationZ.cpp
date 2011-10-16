#include "stdafx.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"

#include "CmdMapSetBuildRotationZ.h"
#include "commdb.h"
#include "map.h"


CmdMapSetBuildRotationZ::CmdMapSetBuildRotationZ(void)
{
}

CmdMapSetBuildRotationZ::~CmdMapSetBuildRotationZ(void)
{
}

void CmdMapSetBuildRotationZ::SetBuildRotationZ(DWORD dwGridIndex,float fBuildRotationZ)
{
	m_dwGridIndex = dwGridIndex;
	m_fBuildRotationZ = fBuildRotationZ;
}

bool CmdMapSetBuildRotationZ::Execute(void)
{
	//±£´æ¾ÉµÄ
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);

	m_fOldBuildRotationZ = pGrid->GetBuild()->vRotation.z;
	if (m_fOldBuildRotationZ == m_fBuildRotationZ)
		return false;

	pGrid->SetBuildRotationZ(m_fBuildRotationZ);

	Cmd::Execute();
	return true;
}

void CmdMapSetBuildRotationZ::UnExecute(void)
{
	Cmd::UnExecute();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);
	pGrid->SetBuildRotationZ(m_fOldBuildRotationZ);
}

void CmdMapSetBuildRotationZ::Dump(void)
{
	OutputConsole("Map_Set_BuildRotationZ: %d %f",m_dwGridIndex,m_fBuildRotationZ);
}		