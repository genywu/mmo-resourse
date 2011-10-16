#include "StdAfx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"

#include "cmdmapterrainsettile.h"
#include "CommDB.h"
#include "EditWnd.h"
#include "CommUI.h"
#include "TerrainTileSetWnd.h"
#include "WndMiniMap.h"

CmdMapTerrainSetTile::CmdMapTerrainSetTile(void)
{
	m_ptProcessData   = NULL;
}

CmdMapTerrainSetTile::~CmdMapTerrainSetTile(void)
{
	SAFEDELETE(m_ptProcessData);
	m_vOldVertexArray.clear();
}

void CmdMapTerrainSetTile::Execute(void)
{
	Cmd::Execute();

	TerrainMesh* pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();

	static TerrainMesh::_tagProcessData pCurrentData;
	// 保存当前的操作参数
	pTerrainMesh->GetProcessData(&pCurrentData);

	pTerrainMesh->SetProcessData(m_ptProcessData);
	pTerrainMesh->RecalculateGridTexture(m_ptProcessData->dwSelectedGridIndex, m_ptProcessData->dwSelectedVertexIndex, m_vOldVertexArray);

	// 设回当前的操作参数
	pTerrainMesh->SetProcessData(&pCurrentData);

	EditWnd* pEditWnd = CommUI::GetInstance()->GetEditWnd();
	WndTerrainTileSet *pWndTerrainTileSet = pEditWnd->GetWndTerrainTileSet();
	WndMiniMap *pWndMiniMap = pEditWnd->GetWndMiniMap();
	TerrainMesh::_tagVertex *pVertex;
	DWORD dwColor = pWndTerrainTileSet->GetBrushColor(m_ptProcessData->nCurrentTextureID);
	DWORD dwCount = (DWORD)m_vOldVertexArray.size();
	for(DWORD i = 0; i < dwCount; i++)
	{
		pVertex = &m_vOldVertexArray[i];
		DWORD dwNumGrid = pVertex->dwBelongToNum;
		for(DWORD j = 0; j < dwNumGrid; j++)
		{
			DWORD dwGridIndex = pVertex->dwBelongToGrid[j];
			pWndMiniMap->SetMapPixel(dwGridIndex, dwColor);
		}
	}
	pWndMiniMap->UpdateMiniPixel();
}

void CmdMapTerrainSetTile::UnExecute(void)
{
	Cmd::UnExecute();

	TerrainMesh* pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
	static TerrainMesh::_tagProcessData pCurrentData;

	// 保存当前的操作参数
	pTerrainMesh->GetProcessData(&pCurrentData);
	// 回填上一次操作时的操作参数
	pTerrainMesh->SetProcessData(m_ptProcessData);
	// 恢复上一次操作前的网格数据
	pTerrainMesh->FillVertex(m_vOldVertexArray);
	// 设回当前的操作参数
	pTerrainMesh->SetProcessData(&pCurrentData);

	EditWnd* pEditWnd = CommUI::GetInstance()->GetEditWnd();
	WndTerrainTileSet *pWndTerrainTileSet = pEditWnd->GetWndTerrainTileSet();
	WndMiniMap *pWndMiniMap = pEditWnd->GetWndMiniMap();
	TerrainMesh::_tagVertex *pVertex;
	DWORD dwCount = (DWORD)m_vOldVertexArray.size();
	for(DWORD i = 0; i < dwCount; i++)
	{
		pVertex = &m_vOldVertexArray[i];
		DWORD dwNumGrid = pVertex->dwBelongToNum;
		DWORD dwColor = pWndTerrainTileSet->GetBrushColor(pVertex->byTexID);
		for(DWORD j = 0; j < dwNumGrid; j++)
		{
			DWORD dwGridIndex = pVertex->dwBelongToGrid[j];
			pWndMiniMap->SetMapPixel(dwGridIndex, dwColor);
		}
	}
	pWndMiniMap->UpdateMiniPixel();
}

void CmdMapTerrainSetTile::Dump(void)
{
	// 输出错误数据
	OutputConsole("Map_Set_Tile error: %d, %d", m_ptProcessData->dwSelectedGridIndex, m_ptProcessData->dwSelectedVertexIndex);
}




//---------------------------------------------------------------------------------
// Function name   : CmdMapTerrainSetTile::GetProcessData
// Description     : 获取操作参数
// Return type     : VOID 
// Argument        : TerrainMesh::_tagProcessData* pData
//---------------------------------------------------------------------------------
VOID CmdMapTerrainSetTile::GetProcessData(TerrainMesh::_tagProcessData* pData)
{
	ASSERT(pData);

	if(m_ptProcessData)
		memcpy(pData, m_ptProcessData, sizeof(TerrainMesh::_tagProcessData));
}



//---------------------------------------------------------------------------------
// Function name   : CmdMapTerrainSetTile::SetProcessData
// Description     : 设置操作参数
// Return type     : VOID 
// Argument        : TerrainMesh::_tagProcessData* pData
//---------------------------------------------------------------------------------
VOID CmdMapTerrainSetTile::SetProcessData(TerrainMesh::_tagProcessData* pData)
{
	ASSERT(pData);

	if(!m_ptProcessData)
	{
		m_ptProcessData = new TerrainMesh::_tagProcessData;
		memcpy(m_ptProcessData, pData, sizeof(TerrainMesh::_tagProcessData));
	}
}

VOID CmdMapTerrainSetTile::Destroy()
{
	SAFEDELETE(m_ptProcessData);
	m_vOldVertexArray.clear();
}
