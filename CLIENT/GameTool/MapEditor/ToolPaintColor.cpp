#include "StdAfx.h"

#include "toolpaintcolor.h"

#include "..\..\frostengine\render.h"
#include "..\..\frostengine\ui.h"

#include "..\..\GameEngine\GameSelectRay.h"

#include "CommDB.h"
#include "Map.h"
#include "TerrainMesh.h"
#include "EditWnd.h"
#include "CmdMgr.h"
#include "VisibleSet.h"



ToolPaintColor::ToolPaintColor(void)
{
	m_dwForeColor = 0xffffffff;
	m_dwBackColor = 0xff000000;

	m_dwTextureIndex = 0xffffffff;

	m_eEditStatus = PCST_PAINT;
	m_dwBrushType  = 0;

	m_fBrushRadius = 0.5f;
	m_fColorChroma = 0.1f;

	m_dwHoverVertex = 0xFFFFFFFF;
}

ToolPaintColor::~ToolPaintColor(void)
{
	ASSERT(m_dwTextureIndex == 0xFFFFFFFF);
}

void ToolPaintColor::Create(TCHAR *colorfile, RECT &rcColorDialog, RECT &rcForeColorQuad, RECT &rcBackColorQuad)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	if(m_dwTextureIndex != 0XFFFFFFFF)
	{
		pInterface->DestroyTexture(m_dwTextureIndex);
	}

	m_dwTextureIndex = pInterface->CreateTexture(colorfile);
	ASSERT(m_dwTextureIndex);

	render::Texture *pTexture = pInterface->GetTexture(m_dwTextureIndex);

	DWORD dwWidth = pInterface->GetWndWidth();
	DWORD dwHeight = pInterface->GetWndHeight();

	m_rcColorDialog.left  = dwWidth - pTexture->GetWidth();
	m_rcColorDialog.top   = dwHeight - 64 + 16 - pTexture->GetHeight();
	m_rcColorDialog.right = dwWidth;
	m_rcColorDialog.bottom= dwHeight - 64 + 16;

	memcpy(&m_rcForeColorQuad, &rcForeColorQuad, sizeof(RECT));
	memcpy(&m_rcBackColorQuad, &rcBackColorQuad, sizeof(RECT));
}

void ToolPaintColor::Destroy()
{
	if(m_dwTextureIndex != 0xffffffff)
	{
		render::Interface *pInterface = render::Interface::GetInstance();
		pInterface->DestroyTexture(m_dwTextureIndex);
		m_dwTextureIndex = 0xffffffff;
	}
}

void ToolPaintColor::MessageProc(BOOL bActive)
{
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	
	EditWnd *pWndEditor = (EditWnd*)ui::Manager::GetInstance()->GetInstance()->GetMainWnd();

	TerrainMesh *pTerrain = CommDB::GetInstance()->GetTerrainMesh();
	//Terrain *pTerrain = pWndEditor->GetTerrain();
	

	if(pTerrain)
	{
		if(pInputInfo->eType == UIMT_KB_KEYDOWN)
		{
			switch(pInputInfo->dwData)
			{
			case DIK_P:					// P
				{						// 切换画笔类型
					if(m_dwBrushType == 0)
						m_dwBrushType = 1;
					else
						m_dwBrushType = 0;
				}
				break;
			case DIK_LBRACKET:                 // 
				// 缩小画刷
				m_fBrushRadius -= 0.40f;
				if (m_fBrushRadius < 0.5f) m_fBrushRadius = 0.5f;
				break;
			case DIK_RBRACKET:  
				// 放大画刷
				m_fBrushRadius += 0.40f;
				if (m_fBrushRadius > 8.5f) m_fBrushRadius = 8.5f;
				break;
			case DIK_SUBTRACT:			// -
				{
					if (pInputInfo->byKeyBuffer[DIK_RCONTROL] & 0x80)
						m_fColorChroma -= 0.1f;
					else
						m_fColorChroma -= 0.01f;
					if (m_fColorChroma < 0.0000f) 
						m_fColorChroma = 0.0f;
				}
				break;
			case DIK_ADD:			// +
				{
					if (pInputInfo->byKeyBuffer[DIK_RCONTROL] & 0x80)
						m_fColorChroma += 0.1f;
					else
						m_fColorChroma += 0.01f;
					if (m_fColorChroma > 1.0000f)
						m_fColorChroma = 1.0f;
				}
				break;
			case DIK_X:			// +
				{
					
				}
				break;
			default:
				break;
			}
		}

		m_dwHoverVertex = GetHoverVertex();

		if (bActive)
		{
			if (pInputInfo->byKeyBuffer[DIK_LALT] & 0x80)
			{
				if(pInputInfo->eType == UIMT_MS_BTNDOWN)
				{
					if (pInputInfo->dwData == MK_LBUTTON)
					{
						
						//吸取颜色
						HWND hWnd = render::Interface::GetInstance()->GetWndHandle();
						HDC hDC = ::GetDC(hWnd);
						POINT pt = {pInputInfo->ptMouse.x,pInputInfo->ptMouse.y};

						COLORREF color = GetPixel(hDC,pInputInfo->ptMouse.x,pInputInfo->ptMouse.y);

						DWORD r = GetRValue(color);
						DWORD g = GetGValue(color);
						DWORD b = GetBValue(color);
						m_dwForeColor = 0xFF000000 | r << 16 | g << 8 | b;

						ReleaseDC(hWnd,hDC);
						
					}
				}
			}
			else
			{
						switch(m_eEditStatus)
						{
						case PCST_PAINT:// 涂色
							{
								if (pInputInfo->eType == UIMT_MS_BTNDOWN && pInputInfo->dwData & MK_LBUTTON)
								{
									if (PtInRect(&m_rcForeColorQuad,pInputInfo->ptMouse))
									{
										// 如果点击前景色颜色面板，则显示选色面板，进入选前景色状态。
										m_eEditStatus = PCST_PICKFORECOLOR;
									}
									else
										if (PtInRect(&m_rcBackColorQuad,pInputInfo->ptMouse))
										{
											// 选后景色
											m_eEditStatus = PCST_PICKBACKCOLOR;
										}
										else
										{	
											if (m_dwHoverVertex != 0xFFFFFFFF)
											{
												// 对网格上色
												EditWnd* pWndEditor = (EditWnd* )ui::Manager::GetInstance()->GetMainWnd();

												DWORD dwUseColor = (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)? m_dwBackColor : m_dwForeColor;

												CmdMapPaintColor* pCmd = new CmdMapPaintColor;
												pCmd->SetVertex(m_dwHoverVertex);
												pCmd->SetColor(dwUseColor,m_fColorChroma);
												pCmd->SetBrush(m_dwBrushType,m_fBrushRadius);
												CmdMgr::GetInstance()->Do(pCmd);
											}
										}
								}
								if (pInputInfo->eType == UIMT_MS_MOVE && pInputInfo->dwData & MK_LBUTTON)
								{
									if (m_dwHoverVertex != 0xFFFFFFFF)
									{
										// 对网格上色
										EditWnd* pWndEditor = (EditWnd* )ui::Manager::GetInstance()->GetMainWnd();

										DWORD dwUseColor = (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)? m_dwBackColor : m_dwForeColor;

										CmdMapPaintColor* pCmd = new CmdMapPaintColor;
										pCmd->SetVertex(m_dwHoverVertex);
										pCmd->SetColor(dwUseColor,m_fColorChroma);
										pCmd->SetBrush(m_dwBrushType,m_fBrushRadius);
										CmdMgr::GetInstance()->Do(pCmd);
									}


								}
							}
							break;
							// 选色
						case PCST_PICKFORECOLOR:
						case PCST_PICKBACKCOLOR:
							{
									// 选择贴图上点击位置像素的颜色
								if (PtInRect(&m_rcColorDialog,pInputInfo->ptMouse))
								{
									DWORD color = PickColor(pInputInfo->ptMouse.x - m_rcColorDialog.left, pInputInfo->ptMouse.y - m_rcColorDialog.top);

									if(m_eEditStatus == PCST_PICKFORECOLOR)
									{
										m_dwForeColor = color;
									}
									else 
										if(m_eEditStatus = PCST_PICKBACKCOLOR)
										{
											m_dwBackColor = color;
										}
								}

								if (pInputInfo->eType == UIMT_MS_BTNDOWN && pInputInfo->dwData & MK_LBUTTON)
								{
									m_eEditStatus = PCST_PAINT;
								}
								
							}
							break;
						}
					}
				}
	}
}

void ToolPaintColor::Render()
{
	if(m_eEditStatus == PCST_PICKFORECOLOR || m_eEditStatus == PCST_PICKBACKCOLOR)
		ShowColorDialog();
	else
		DrawBrush();

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

	pLayer2D->DrawSolidQuad(m_rcBackColorQuad.left, 
		m_rcBackColorQuad.top, 
		m_rcBackColorQuad.right - m_rcBackColorQuad.left, 
		m_rcBackColorQuad.bottom - m_rcBackColorQuad.top, 
		m_dwBackColor);

	pLayer2D->DrawSolidQuad(m_rcForeColorQuad.left, 
		m_rcForeColorQuad.top, 
		m_rcForeColorQuad.right - m_rcForeColorQuad.left, 
		m_rcForeColorQuad.bottom - m_rcForeColorQuad.top, 
		m_dwForeColor);


	_ShowStatus();
}

void ToolPaintColor::ShowColorDialog()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

	pLayer2D->DrawTexture(m_rcColorDialog.left, m_rcColorDialog.top, 
						m_rcColorDialog.right - m_rcColorDialog.left, m_rcColorDialog.bottom - m_rcColorDialog.top, 
						m_dwTextureIndex);
}

DWORD ToolPaintColor::PickColor(DWORD x, DWORD y)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Texture *pTexture = pInterface->GetTexture(m_dwTextureIndex);

	return pTexture->GetPixel(x, y);
}

void ToolPaintColor::_ShowStatus()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

	static char szTemp[256];
	static char szType[16];
	static char szBlend[16];

	if(m_dwBrushType == 0)
	{
		sprintf(szType, "铅笔");
	}		
	else
	{
		sprintf(szType, "毛笔");
	}

	

	sprintf(szTemp,"画刷(P):%s 浓度(+,-): %d%%", szType,(int)(m_fColorChroma * 100.0f),m_dwHoverVertex);
	int iTexLen = lstrlen(szTemp);

	int iTextWidth = pLayer2D->CalcTextWidth(szTemp,iTexLen);
	int iTextHeight= pLayer2D->GetFontSize();

	int iWidth = iTextWidth + 16;
	int iStartX = 300;
	int iStartY = pInterface->GetWndHeight() - iTextHeight - 4;

	pLayer2D->DrawSolidQuad(iStartX - 2,iStartY ,iTextWidth + 8,iTextHeight + 2,FWC_BACK);
	ui::Wnd::SetUIRendState();
	pLayer2D->TextOut(iStartX + 1,iStartY + 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX - 1,iStartY - 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX + 1,iStartY - 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX - 1,iStartY + 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX + 1,iStartY,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX - 1,iStartY,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX,iStartY + 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->TextOut(iStartX,iStartY - 1,szTemp,iTexLen,FWC_BLACK);  
	sprintf(szTemp,"#FFFFFFFF画刷(#FFFFD050P#FFFFFFFF):#FFFFD050%s #FFFFFFFF浓度(#FFFFD050[#FFFFFFFF,#FFFFD050]#FFFFFFFF): #FFFFD050%d%%", szType,(int)(m_fColorChroma * 100.0f));
	pLayer2D->TextOutEx(iStartX,iStartY,szTemp,lstrlen(szTemp));
}

void ToolPaintColor::DrawBrush(void)
{
	if (m_dwHoverVertex == 0xFFFFFFFF) 
		return;

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = render::Interface::GetInstance()->GetLayer3D();

	EditWnd *pWndEditor = (EditWnd*)ui::Manager::GetInstance()->GetMainWnd();

	TerrainMesh *pTerrain = CommDB::GetInstance()->GetTerrainMesh();
	//Terrain *pTerrain = pWndEditor->GetTerrain();

	DWORD dwMapWidth = pTerrain->GetWidth();
	DWORD dwMapDepth = pTerrain->GetDepth();

	TerrainMesh::_tagVertex * pVertexList = pTerrain->GetVertexList();
	
	D3DXVECTOR3 *pHoverVertex = &pVertexList[m_dwHoverVertex].vPosition;

	float fCenterX = pHoverVertex->x;
	float fCenterZ = pHoverVertex->z;
	float fCenterY = pHoverVertex->y;

	//计算位置
	DWORD dwCount = 18;
	DWORD dwAngle = 360 / dwCount;
	float fLastX,fLastZ;
	D3DXVECTOR3 vStart,vEnd;
	for(DWORD n = 0; n <= dwCount; n++)
	{
		//计算每个位置
		DWORD dwTempAngle = (n % dwCount) * dwAngle;
		float fAngle = D3DXToRadian(dwTempAngle);
		float x = m_fBrushRadius * sin(fAngle) + fCenterX;
		float z = m_fBrushRadius * - cos(fAngle) + fCenterZ;

		if (n)
		{
			vStart.x = fLastX;vStart.y = fCenterY + 0.02f;vStart.z = fLastZ;
			vEnd.x = x;vEnd.y = fCenterY + 0.02f;vEnd.z = z;

			pLayer3D->_DEBUG_DrawLine(&vStart,&vEnd,0xff00ff00);			
		}		
		fLastX = x;
		fLastZ = z;
	}
}

DWORD ToolPaintColor::GetHoverVertex(void)
{
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	//计算选中的格子
	GameSelectRay cSelRay;
	RECT rc;
	EditWnd *pWndEditor = (EditWnd *)ui::Manager::GetInstance()->GetMainWnd();
	pWndEditor->GetWndRect(&rc);
	cSelRay.CalculateRay(&rc,pWndEditor->GetCamera(),pInputInfo->ptMouse);
	//显示格子
	
	TerrainMesh *pTerrain = CommDB::GetInstance()->GetTerrainMesh();
	//Terrain *pTerrain = pWndEditor->GetTerrain();	

	EditMap *pMap = CommDB::GetInstance()->GetMap();
	VisibleSet *pSet = pMap->GetVisibleSet();
	DWORD *pVIArray = pSet->GetVisibleIndexArray();
	TerrainMesh::_tagGrid *pGrid(NULL);
	TerrainMesh::_tagVertex *pVertexArray = pTerrain->GetVertexList();
	D3DXVECTOR3 vGridPosition[4];
	DWORD dwSelGrid = 0xFFFFFFFF;
	float fDistance = 100000000.0f;
	float fTempDist;

	for(DWORD n = 0; n< pSet->GetVisibleIndexCount(); n++)
	{
		DWORD dwGridIndex = pVIArray[n];
		pGrid = pTerrain->GetGrid(dwGridIndex);

		for(DWORD v = 0; v < 4; v++)
		{
			vGridPosition[v] = pVertexArray[pGrid->dwVertexIndex[v]].vPosition;
		}

	    if (cSelRay.IntersectTri(&vGridPosition[0],&vGridPosition[1],&vGridPosition[2],NULL,&fTempDist))
		{
			if (fTempDist < fDistance)
			{
				fDistance = fTempDist;
				dwSelGrid = dwGridIndex;
			}
		}
		else
			if (cSelRay.IntersectTri(&vGridPosition[0],&vGridPosition[2],&vGridPosition[3],NULL,&fTempDist))
			{	 
				if (fTempDist < fDistance)
				{
					fDistance = fTempDist;
					dwSelGrid = dwGridIndex;
				}				
			}
	}
	if (dwSelGrid == 0xFFFFFFFF)
	{
		return 0xFFFFFFFF;
	}

	
	//求出当前选中的点
	fDistance = 100000000.0f;
	DWORD dwSelVertex = 0xFFFFFFFF;
	DWORD dwVertexIndex;
	D3DXVECTOR3 vQuadPosition[4];
	enum { BOTTOM_LEFT = 0,BOTTOM_RIGHT,TOP_RIGHT,TOP_LEFT};
	pGrid = pTerrain->GetGrid(dwSelGrid);
	for(DWORD v = 0; v < 4; v++)
	{
		vGridPosition[v] = pVertexArray[pGrid->dwVertexIndex[v]].vPosition;
	}
	for(int i = 0; i < 4; i++)
	{
		switch(i)
		{
		case BOTTOM_LEFT:
			{
				vQuadPosition[0] = vGridPosition[0];
				vQuadPosition[1] = (vGridPosition[1] + vGridPosition[0]) * 0.5f;
				vQuadPosition[2] = (vGridPosition[2] + vGridPosition[0]) * 0.5f;
				vQuadPosition[3] = (vGridPosition[3] + vGridPosition[0]) * 0.5f;
				dwVertexIndex = pGrid->dwVertexIndex[0];
			}
			break;
		case BOTTOM_RIGHT:
			{
				vQuadPosition[0] = (vGridPosition[0] + vGridPosition[1]) * 0.5f;
				vQuadPosition[1] = vGridPosition[1];
				vQuadPosition[2] = (vGridPosition[2] + vGridPosition[1]) * 0.5f;
				vQuadPosition[3] = (vGridPosition[0] + vGridPosition[2]) * 0.5f;
				dwVertexIndex = pGrid->dwVertexIndex[1];
			}
			break;
		case TOP_RIGHT:
			{
				vQuadPosition[0] = (vGridPosition[0] + vGridPosition[2]) * 0.5f;
				vQuadPosition[1] = (vGridPosition[1] + vGridPosition[2]) * 0.5f;
				vQuadPosition[2] = vGridPosition[2];
				vQuadPosition[3] = (vGridPosition[3] + vGridPosition[2]) * 0.5f;
				dwVertexIndex = pGrid->dwVertexIndex[2];
			}
			break;
		case TOP_LEFT:
			{
				vQuadPosition[0] = (vGridPosition[0] + vGridPosition[3]) * 0.5f;
				vQuadPosition[1] = (vGridPosition[0] + vGridPosition[2]) * 0.5f;
				vQuadPosition[2] = (vGridPosition[2] + vGridPosition[3]) * 0.5f;
				vQuadPosition[3] = vGridPosition[3];
				dwVertexIndex = pGrid->dwVertexIndex[3];
			}
			break;
		default:
			break;
		}

		//计算三角形
		
	    if (cSelRay.IntersectTri(&vQuadPosition[0],&vQuadPosition[1],&vQuadPosition[2],NULL,&fTempDist))
		{
			if (fTempDist < fDistance)
			{
				fDistance = fTempDist;
				dwSelVertex = dwVertexIndex;
			}
		}
		else
			if (cSelRay.IntersectTri(&vQuadPosition[0],&vQuadPosition[2],&vQuadPosition[3],NULL,&fTempDist))
			{	 
				if (fTempDist < fDistance)
				{
					fDistance = fTempDist;
					dwSelVertex = dwVertexIndex;
				}				
			}
	}

	return dwSelVertex;
}