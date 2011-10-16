#include "StdAfx.h"

#include "..\..\frostengine\utility.h" 
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\frostengine\ui.h"

#include "..\..\gameengine\gameselectray.h"

#include "editwnd.h"
#include "commdb.h"
#include "commui.h"
#include "visibleset.h"
#include "selectfrustum.h" 
#include "BlockEditTool.h"
#include "map.h"      
#include "cmdmgr.h"
#include "terrainmesh.h"
#include "cmdmapfillblock.h"

const char *szBlockTypeName[] = {"地面阻挡","飞行阻挡","人物"};

ToolBlockEdit::ToolBlockEdit(void)
{
	m_dwCurSelGrid = 0xFFFFFFFF;
	m_iBrushSize   = 0;	
	m_dwCurBLockType = 0;
}

ToolBlockEdit::~ToolBlockEdit(void)
{
}

void ToolBlockEdit::Create()
{
}

void ToolBlockEdit::Destroy(void)
{
}


void ToolBlockEdit::PaintBrush(DWORD dwBLock)
{
	if (m_dwCurSelGrid == 0xFFFFFFFF)
		return;       
	
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	DWORD dwMapWidth = pMap->GetWidth();
	DWORD dwMapDepth = pMap->GetDepth();
	int iCenterX = m_dwCurSelGrid % dwMapWidth;
	int iCenterZ = m_dwCurSelGrid / dwMapWidth;

	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagGrid *pGrid(NULL);
	TerrainMesh::_tagVertex *pVertexArray = pMesh->GetVertexList();
	D3DXVECTOR3 pos[4];

	BOOL bAllLike = TRUE;

	for(int j = - m_iBrushSize; j < m_iBrushSize + 1; j++)
	{
		for(int i = - m_iBrushSize; i < m_iBrushSize + 1;i++)
		{
			int iX = iCenterX + i;
			int iZ = iCenterZ + j;

			if (!(iX < 0 || iX >= (int)dwMapWidth || iZ < 0 || iZ >= (int)dwMapDepth))
			{
				//在地图内
				//显示格子
				DWORD dwGridIndex = iX + iZ * dwMapWidth;
				//检查是否全部相同
				if (pMap->GetGrid(dwGridIndex)->GetBlock() != dwBLock)
				{
					bAllLike = FALSE;
					break;
				}
			}
		}
	}
	if (!bAllLike)//有不同，则涂
	{
		CmdMapFillBlock *pCmd = new CmdMapFillBlock;
		pCmd->SetBlock(dwBLock);
		for(int j = - m_iBrushSize; j < m_iBrushSize + 1; j++)
		{
			for(int i = - m_iBrushSize; i < m_iBrushSize + 1;i++)
			{
				int iX = iCenterX + i;
				int iZ = iCenterZ + j;
				

				if (!(iX < 0 || iX >= (int)dwMapWidth || iZ < 0 || iZ >= (int)dwMapDepth))
				{
					//在地图内
					//显示格子
					DWORD dwGridIndex = iX + iZ * dwMapWidth;
					pCmd->AddGridIndex(dwGridIndex);					
				}
			}
		}

		CmdMgr::GetInstance()->Do(pCmd);
	}
}

void ToolBlockEdit::MessageProc(BOOL bActive)
{
	//切换
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();


	if (pInputInfo->eType == UIMT_KB_KEYDOWN)
	{
		switch(pInputInfo->dwData)
		{
		case DIK_LBRACKET:
			m_iBrushSize--;
			if (m_iBrushSize < 0)
			{
				m_iBrushSize = 0;
			}
			break;
		case DIK_RBRACKET:
			m_iBrushSize++;
			if (m_iBrushSize > 5)
			{
				m_iBrushSize = 5;
			}
			break;
		case DIK_1:
			m_dwCurBLockType = 0;
			break;
		case DIK_2:
			m_dwCurBLockType = 1;
			break;
		case DIK_3:
			m_dwCurBLockType = 2;
			break;
		}
	}

	m_dwCurSelGrid = GetSelectGrid();
	if (bActive)
	{
		if (pInputInfo->dwMouseButtonState & MK_LBUTTON)
		{
			if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
			{
				PaintBrush(0);
			}	
			else
			{
				PaintBrush(m_dwCurBLockType+1);
			}
		}
	}	
}
void ToolBlockEdit::DrawBrush(void)
{
	if (m_dwCurSelGrid == 0xFFFFFFFF) 
		return;

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = render::Interface::GetInstance()->GetLayer3D();

	EditMap *pMap = CommDB::GetInstance()->GetMap();
	DWORD dwMapWidth = pMap->GetWidth();
	DWORD dwMapDepth = pMap->GetDepth();
	int iCenterX = m_dwCurSelGrid % dwMapWidth;
	int iCenterZ = m_dwCurSelGrid / dwMapWidth;

	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagGrid *pGrid(NULL);
	TerrainMesh::_tagVertex *pVertexArray = pMesh->GetVertexList();
	D3DXVECTOR3 pos[4];


	for(int j = - m_iBrushSize; j < m_iBrushSize + 1; j++)
	{
		for(int i = - m_iBrushSize; i < m_iBrushSize + 1;i++)
		{
			int iX = iCenterX + i;
			int iZ = iCenterZ + j;

			if (!(iX < 0 || iX >= (int)dwMapWidth || iZ < 0 || iZ >= (int)dwMapDepth))
			{
				//在地图内
				//显示格子
				DWORD dwGridIndex = iX + iZ * dwMapWidth;
				pGrid = pMesh->GetGrid(dwGridIndex);

				for(DWORD v = 0; v < 4; v++)
				{
					pos[v] = pVertexArray[pGrid->dwVertexIndex[v]].vPosition;
					pos[v].y += 0.02f;
				}

				pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[1],&pos[2],0x8080ff80);
				pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[2],&pos[3],0x8080ff80);
				pLayer3D->_DEBUG_DrawWireQuad(&pos[0],&pos[1],&pos[2],&pos[3],0xff00ff00);
			}
		}
	}
}
void ToolBlockEdit::Render()
{
	DrawBrush();	
	_ShowStatus();
}
DWORD ToolBlockEdit::GetSelectGrid(void)
{
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	//计算选中的格子
	GameSelectRay cSelRay;
	RECT rc;
	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	pEditWnd->GetWndRect(&rc);
	cSelRay.CalculateRay(&rc,pEditWnd->GetCamera(),pInputInfo->ptMouse);
	//显示格子
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	VisibleSet *pSet = pMap->GetVisibleSet();
	DWORD *pVIArray = pSet->GetVisibleIndexArray();
	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagGrid *pGrid(NULL);
	TerrainMesh::_tagVertex *pVertexArray = pMesh->GetVertexList();
	D3DXVECTOR3 pos[4];
	DWORD dwSelGrid = 0xFFFFFFFF;
	float fDistance = 100000000.0f;
	float fTempDist;

	for(DWORD n = 0; n< pSet->GetVisibleIndexCount(); n++)
	{
		DWORD dwGridIndex = pVIArray[n];
		pGrid = pMesh->GetGrid(dwGridIndex);

		for(DWORD v = 0; v < 4; v++)
		{
			pos[v] = pVertexArray[pGrid->dwVertexIndex[v]].vPosition;
		}

	    if (cSelRay.IntersectTri(&pos[0],&pos[1],&pos[2],NULL,&fTempDist))
		{
			if (fTempDist < fDistance)
			{
				fDistance = fTempDist;
				dwSelGrid = dwGridIndex;
			}
		}
		else
			if (cSelRay.IntersectTri(&pos[0],&pos[2],&pos[3],NULL,&fTempDist))
			{	 
				if (fTempDist < fDistance)
				{
					fDistance = fTempDist;
					dwSelGrid = dwGridIndex;
				}

				
			}
	}
	return dwSelGrid;
}

void ToolBlockEdit::_ShowStatus(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

	//DRAW 编辑状态
	static char szTemp[256];

	int x(0),z(0);
	if (m_dwCurSelGrid != 0xFFFFFFFF)
	{
		EditMap *pMap = CommDB::GetInstance()->GetMap();
		DWORD dwMapWidth = pMap->GetWidth();
		x = m_dwCurSelGrid % dwMapWidth;
		z = m_dwCurSelGrid / dwMapWidth;
	}

	sprintf(szTemp,"大小:+,- 类型(1,2,3):%s [%d,%d]",szBlockTypeName[m_dwCurBLockType],x,z);
	int iTexLen = lstrlen(szTemp);

	int iTextWidth = pLayer2D->CalcTextWidth(szTemp,iTexLen);
	int iTextHeight= pLayer2D->GetFontSize();

	int iWidth = iTextWidth + 16;	

	int iStartX = 300;
	int iStartY = pInterface->GetWndHeight() - iTextHeight - 4;
    	
	pLayer2D->DrawSolidQuad(iStartX - 2,iStartY ,iTextWidth + 8,iTextHeight + 2 ,0x80bbc5ff);

	ui::Wnd::SetUIRendState();
	pLayer2D->TextOut(iStartX + 1,iStartY + 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX - 1,iStartY - 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX + 1,iStartY - 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX - 1,iStartY + 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX + 1,iStartY,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX - 1,iStartY,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX,iStartY + 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX,iStartY - 1,szTemp,iTexLen,FWC_BLACK);  
	sprintf(szTemp,"大小:#FFFD2339+,-#FFFFFFFF 类型(1,2,3):#FFFFD239%s #FFFFFFFF[%d,%d]",szBlockTypeName[m_dwCurBLockType],x,z);
	pLayer2D->TextOutEx(iStartX,iStartY,szTemp,lstrlen(szTemp));
}

