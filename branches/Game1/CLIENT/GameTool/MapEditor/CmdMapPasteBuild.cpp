#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\gameengine\gamebuildlist.h"

#include "commui.h"
#include "EditWnd.h"
#include "CmdMapPasteBuild.h"

#include "ToolBuildEdit.h"
#include "commdb.h"
#include "map.h"
#include "cmdmappastebuild.h"


CmdMapPasteBuild::CmdMapPasteBuild(void)
{
}

CmdMapPasteBuild::~CmdMapPasteBuild(void)
{
	SAFEDELETEARRAY(m_pClipboard);
	SAFEDELETEARRAY(m_pOldArray);
}

void CmdMapPasteBuild::Execute(void)
{
	Cmd::Execute();
	//把可能被修改的格子保存起来
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	//copy sel to clip
	for(DWORD n = 0; n < m_dwBuildCount;n++)
	{
		ToolBuildEdit::tagCopyData *pNewCD = &m_pClipboard[n];
		ToolBuildEdit::tagCopyData *pOldCD = &m_pOldArray[n];

		int x = pNewCD->ox + (int)m_dwCX;
		if (x < 0 || x >= pMap->GetWidth())
			continue;
		int z = pNewCD->oy + (int)m_dwCZ;
		if (z < 0 || z >= pMap->GetDepth())
			continue;
        
		DWORD dwGridIndex = x + z * pMap->GetWidth();
		
		Grid *pGrid = pMap->GetGrid(dwGridIndex);
		Grid::tagBuild *pBuild = pGrid->GetBuild();
		pOldCD->wBuildID = pBuild->wID;
		pOldCD->fHeight  = pBuild->fHeight;
		pOldCD->fRotationY = pBuild->fRotationY;
	}
	//设置新的鸽子
	for(DWORD n = 0; n < m_dwBuildCount;n++)
	{
		ToolBuildEdit::tagCopyData *pNewCD = &m_pClipboard[n];
		ToolBuildEdit::tagCopyData *pOldCD = &m_pOldArray[n];

		int x = pNewCD->ox + (int)m_dwCX;
		if (x < 0 || x >= pMap->GetWidth())
			continue;
		int z = pNewCD->oy + (int)m_dwCZ;
		if (z < 0 || z >= pMap->GetDepth())
			continue;
		
		DWORD dwGridIndex = x + z * pMap->GetWidth();
		Grid *pGrid = pMap->GetGrid(dwGridIndex);
		Grid::tagBuild *pBuild = pGrid->GetBuild();
		pBuild->wID = pNewCD->wBuildID; 
		pBuild->fHeight = pNewCD->fHeight;
		pBuild->fRotationY = pNewCD->fRotationY;
	}

}

void CmdMapPasteBuild::UnExecute(void)
{
	Cmd::UnExecute();
	//恢复旧的格子
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	for(DWORD n = 0; n < m_dwBuildCount;n++)
	{
		ToolBuildEdit::tagCopyData *pNewCD = &m_pClipboard[n];
		ToolBuildEdit::tagCopyData *pOldCD = &m_pOldArray[n];
		
		int x = pNewCD->ox + (int)m_dwCX;
		if (x < 0 || x >= pMap->GetWidth())
			continue;
		int z = pNewCD->oy + (int)m_dwCZ;
		if (z < 0 || z >= pMap->GetDepth())
			continue;
		
		DWORD dwGridIndex = x + z * pMap->GetWidth();

		Grid *pGrid = pMap->GetGrid(dwGridIndex);
		Grid::tagBuild *pBuild = pGrid->GetBuild();
		pBuild->wID = pOldCD->wBuildID; 
		pBuild->fHeight = pOldCD->fHeight;
		pBuild->fRotationY = pOldCD->fRotationY;
	}
}

void CmdMapPasteBuild::Dump(void)
{
	OutputConsole("Map_Paste_Build:---");
}		

void CmdMapPasteBuild::SetClipboard(const ToolBuildEdit::tagCopyData *pClipboard,DWORD dwCount)
{
	m_dwBuildCount = dwCount;
	m_pClipboard = new ToolBuildEdit::tagCopyData[m_dwBuildCount];
	m_pOldArray  = new ToolBuildEdit::tagCopyData[m_dwBuildCount];
	for(DWORD n = 0; n < m_dwBuildCount;n++)
	{
		m_pClipboard[n] = pClipboard[n];
	}
}


void CmdMapPasteBuild::SetCenter(DWORD dwCX, DWORD dwCZ)
{
	m_dwCX = dwCX;
	m_dwCZ = dwCZ;
}
