#include "stdafx.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"

#include "CmdMapSetBuildOffsetXZ.h"
#include "commdb.h"
#include "map.h"


CmdMapSetBuildOffsetXZ::CmdMapSetBuildOffsetXZ(void)
{
}

CmdMapSetBuildOffsetXZ::~CmdMapSetBuildOffsetXZ(void)
{
}

void CmdMapSetBuildOffsetXZ::MapSetBuildPosOffsetXZ(DWORD dwGridIndex, float fPosX, float fPosZ)
{
	m_dwGridIndex = dwGridIndex;
	m_fBuildOffsetX	= fPosX;
	m_fBuildOffsetZ	= fPosZ;
}

bool CmdMapSetBuildOffsetXZ::Execute(void)
{
	//±£´æ¾ÉµÄ
	Grid * pGrid = CommDB::GetInstance()->GetMap()->GetGrid(m_dwGridIndex);
	m_fOldBuildOffsetX = pGrid->GetBuild()->vPosOffset.x;
	m_fOldBuildOffsetZ = pGrid->GetBuild()->vPosOffset.z;

	if (m_fOldBuildOffsetX == m_fBuildOffsetX &&
		m_fOldBuildOffsetZ == m_fBuildOffsetZ)
		return false;

	Cmd::Execute();

	pGrid->SetBuildOffsetXZ(m_fBuildOffsetX, m_fBuildOffsetZ);

	return true;
}

void CmdMapSetBuildOffsetXZ::UnExecute(void)
{
	Cmd::UnExecute();
	EditMap * pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);
	pGrid->SetBuildOffsetXZ(m_fOldBuildOffsetX, m_fOldBuildOffsetZ);
}

void CmdMapSetBuildOffsetXZ::Dump(void)
{
	OutputConsole("Map_Set_BuildOffsetXZ: %d (%f, %f)", m_dwGridIndex, m_fBuildOffsetX, m_fBuildOffsetZ);
}		