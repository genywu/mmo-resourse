#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\gameengine\gamebuildlist.h"

#include "commui.h"
#include "EditWnd.h"
#include "cmdmapsetbuild.h"

#include "ToolBuildEdit.h"
#include "commdb.h"
#include "map.h"


CmdMapSetBuild::CmdMapSetBuild(void)
{
}

CmdMapSetBuild::~CmdMapSetBuild(void)
{
}

void CmdMapSetBuild::SetBuild(DWORD dwGridIndex,const Grid::tagBuild &tBuild)
{
	m_dwGridIndex = dwGridIndex;
	m_tBuild = tBuild;
}

void CmdMapSetBuild::Execute(void)
{
	Cmd::Execute();
	//保存旧的
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);

	int iMapWidth =  pMap->GetWidth();
	int iMapDepth	  =  pMap->GetDepth();

	//save Old
	m_tOldBuild = * pGrid->GetBuild();

	//设置障碍
	//int iCX = (int)m_dwGridIndex % iMapWidth;
	//int iCZ = (int)m_dwGridIndex / iMapWidth;
	//EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	//ToolBuildEdit *pToolBuildEdit = pEditWnd->GetToolBuildEdit();
	//GameBuildList *pGameBuildList = GameBuildList::GetInstance();
	//GameBuildList::Build *pBuild = pGameBuildList->GetBuild(m_tBuild.wID);
	//GameBuildList::Build::tagBlockData *pBlockData = pBuild->GetBlockData();
	//if (pBlockData)
	//{
	//	for(int i = pBlockData->iXMin; i <= pBlockData->iXMax; i++)
	//	{
	//		for(int j = pBlockData->iZMin; j <= pBlockData->iZMax; j++)
	//		{
	//			//求出周围的格子
	//			int iX = i + iCX;
	//			int iZ = j + iCZ;
	//			if (!(iX < 0 || iZ < 0 || iX >= iMapWidth || iZ >= iMapDepth))
	//			{
	//				Grid *pTempGrid = pMap->GetGrid(iX,iZ);
	//				pTempGrid->SetBlock(pBlockData->byType + 1);
	//			}
	//		}
	//	}
	//}  
	//
	pGrid->SetBuild(m_tBuild);

	
	if (m_tBuild.wID != 0xFFFF)
	{
		//set to build
		pMap->AddPalaceBuildCount();
		pMap->AddRefenceBuildCount(m_tBuild.wID);
	}
	else
	{
       	pMap->SubPalaceBuildCount();
		pMap->SubRefenceBuildCount(m_tOldBuild.wID);
	}
}

void CmdMapSetBuild::UnExecute(void)
{
	Cmd::UnExecute();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pGrid = pMap->GetGrid(m_dwGridIndex);

	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	ToolBuildEdit *pToolBuildEdit = pEditWnd->GetToolBuildEdit();
	GameBuildList *pGameBuildList = GameBuildList::GetInstance();

	int iMapWidth  = pMap->GetWidth();
	int iMapDepth  = pMap->GetDepth();
	//计算位置
	/*int iCX = (int)m_dwGridIndex % iMapWidth;
	int iCZ = (int)m_dwGridIndex / iMapWidth;*/
	//清除障碍                                  
	//GameBuildList::Build *pBuild = pGameBuildList->GetBuild(m_tBuild.wID);
	//GameBuildList::Build::tagBlockData *pBlockData = pBuild->GetBlockData();
	//if (pBlockData)
	//{
	//	for(int i = pBlockData->iXMin; i <= pBlockData->iXMax; i++)
	//	{
	//		for(int j = pBlockData->iZMin; j <= pBlockData->iZMax; j++)
	//		{
	//			//求出周围的格子
	//			int iX = i + iCX;
	//			int iZ = j + iCZ;
	//			if (!(iX < 0 || iZ < 0 || iX >= iMapWidth || iZ >= iMapDepth))
	//			{
	//				Grid *pTempGrid = pMap->GetGrid(iX,iZ);
	//				pTempGrid->SetBlock(0);	//0 none 1 normal 2 high
	//			}
	//		}
	//	}
	//}

	//设置旧障碍
	//if (m_tOldBuild.wID != 0xFFFF)
	//{
	//	GameBuildList::Build *pOldBuild = pGameBuildList->GetBuild(m_tOldBuild.wID);
	//	GameBuildList::Build::tagBlockData *pOldBlockData = pOldBuild->GetBlockData();
	//	if (pOldBlockData)
	//	{
	//		for(int i = pOldBlockData->iXMin; i <= pOldBlockData->iXMax; i++)
	//		{
	//			for(int j = pOldBlockData->iZMin; j <= pOldBlockData->iZMax; j++)
	//			{
	//				//求出周围的格子
	//				int iX = i + iCX;
	//				int iZ = j + iCZ;
	//				if (!(iX < 0 || iZ < 0 || iX >= pMap->GetWidth() || iZ >= pMap->GetDepth()))
	//				{
	//					Grid *pTempGrid = pMap->GetGrid(iX,iZ);
	//					pTempGrid->SetBlock(pOldBlockData->byType + 1);
	//				}
	//			}
	//		}
	//	}
	//}
	pGrid->SetBuild(m_tOldBuild);

	if (m_tOldBuild.wID != 0xFFFF)
	{
		//set to build
		pMap->AddPalaceBuildCount();
		pMap->AddRefenceBuildCount(m_tOldBuild.wID);
	}
	else
	{
       	pMap->SubPalaceBuildCount();
		pMap->SubRefenceBuildCount(m_tBuild.wID);
	}

}

void CmdMapSetBuild::Dump(void)
{
	OutputConsole("Map_Set_Build: %d %d",m_dwGridIndex,m_tBuild.wID);
}		