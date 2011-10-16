#include "StdAfx.h"

#include "..\..\frostengine\utility.h" 
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\frostengine\ui.h"

#include "..\..\gameengine\gameselectray.h"

#include "editwnd.h"
#include "commdb.h"
#include "visibleset.h" 
#include "selectfrustum.h"
#include "MtlEditTool.h"
#include "map.h"
#include "mtllistbox.h"
#include "cmdmgr.h"
#include "cmdmapfillmtl.h"
#include "terrainmesh.h"
#include "mtledittool.h"
#include "commui.h"

ToolMtlEdit::ToolMtlEdit(void)
{
	m_dwCurSelGrid = 0xFFFFFFFF;
	m_iBrushSize = 0;
}

ToolMtlEdit::~ToolMtlEdit(void)
{
}


void ToolMtlEdit::Create()
{
}

void ToolMtlEdit::Destroy(void)
{
}



void ToolMtlEdit::PaintBrush(DWORD dwMtl)
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
				if (pMap->GetGrid(dwGridIndex)->GetMtl() != dwMtl)
				{
					bAllLike = FALSE;
					break;
				}
			}
		}
	}
	if (!bAllLike)//有不同，则涂
	{
		CmdMapFillMtl *pCmd = new CmdMapFillMtl;
		pCmd->SetMtl(dwMtl);
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

void ToolMtlEdit::MessageProc(BOOL bActive)
{
	//切换
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
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
		}
	}

	//选择
	m_dwCurSelGrid = GetSelectGrid();
	if (bActive)
	{
		if (pInputInfo->dwMouseButtonState & MK_LBUTTON)
		{
			MtlListBox *pMtlListBox = pEditWnd->GetMtlListBox();

			int iMtlSelIndex = pMtlListBox->GetSelItem();
			if (iMtlSelIndex < 0) 
				iMtlSelIndex = 0;

			PaintBrush((DWORD)iMtlSelIndex);
		}
	}
}


void ToolMtlEdit::DrawBrush(void)
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


void ToolMtlEdit::Render()
{
	DrawBrush();	
	_ShowStatus();
}


DWORD ToolMtlEdit::GetSelectGrid(void)
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

void ToolMtlEdit::_ShowStatus(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();


	////DRAW 编辑状态
	static char szTemp[256];

	sprintf(szTemp,"[%d]",m_iBrushSize);

	int iTexLen = lstrlen(szTemp);

	int iTextWidth = pLayer2D->CalcTextWidth(szTemp,iTexLen);
	int iTextHeight= pLayer2D->GetFontSize();

	int iWidth = iTextWidth + 16;	

	int iStartX = 300;
	int iStartY = pInterface->GetWndHeight() - iTextHeight - 4;

	ui::Wnd::SetUIRendState();
	pLayer2D->DrawSolidQuad(iStartX - 2,iStartY ,iTextWidth + 8,iTextHeight + 2 ,0x80bbc5ff);
	pLayer2D->TextOut(iStartX + 1,iStartY + 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX - 1,iStartY - 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX + 1,iStartY - 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX - 1,iStartY + 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX + 1,iStartY,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX - 1,iStartY,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX,iStartY + 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX,iStartY - 1,szTemp,iTexLen,FWC_BLACK);  	
	sprintf(szTemp,"[%d]",m_iBrushSize);	
	pLayer2D->TextOutEx(iStartX,iStartY,szTemp,lstrlen(szTemp));
}

