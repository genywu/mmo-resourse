
#include "stdafx.h"

#include "..\..\frostengine\utility.h" 
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\frostengine\ui.h"

#include "..\..\gameengine\gameselectray.h"
#include "..\..\GameEngine\GameModelManager.h"

#include "editwnd.h"
#include "commdb.h"
#include "visibleset.h"
#include "selectfrustum.h"
#include "ToolDoorEdit.h"
#include "commui.h"
#include "map.h"
#include "mtllistbox.h"
#include "cmdmgr.h"
#include "cmdmapfillmtl.h"
#include "changewnd.h"
#include "cmdmapfillsecurity.h"
#include "cmdmapdeldoor.h"
#include "cmdmapsetdoor.h"
#include "terrainmesh.h"

const char *pszDoorEditStatusName[] = {"选择","放置"};

ToolDoorEdit::ToolDoorEdit(void)
{
	m_eEditStatus = DEST_PALACE;
	m_bDrag		  = FALSE;
	m_dwCurTime   = 0;

}

ToolDoorEdit::~ToolDoorEdit(void)
{
}

void ToolDoorEdit::MessageProc(BOOL bActive)
{
	//切换
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();

	WndDoorEdit *pWndDoorEdit = pEditWnd->GetWndDoorEdit();

	int iDoorIndex = pWndDoorEdit->GetCurDoorIndex() + 1;

	int iDoorCount = pWndDoorEdit->GetDoorCount();

	if (pInputInfo->eType == UIMT_KB_KEYDOWN)
	{
		switch(pInputInfo->dwData)
		{
		case DIK_Q:
			{
				m_eEditStatus = (m_eEditStatus == DEST_SELECT)? DEST_PALACE : DEST_SELECT;
			}
			break;
		case DIK_V:
			if (pInputInfo->byKeyBuffer[DIK_LCONTROL])
			{
				if (m_eEditStatus == DEST_SELECT)
				{

				}   
			}
			break;
		case DIK_DELETE:
			if (m_eEditStatus == DEST_SELECT)
			{
				int iSelSize = (int)m_vSelGridIndexArray.size();
				if (iSelSize)
				{
					////清除所选择部分D
					CmdMapDelDoor *pCmd = new CmdMapDelDoor;
					for(int i = 0; i < iSelSize; i++)
					{
						pCmd->AddGridIndex((DWORD)m_vSelGridIndexArray[i]);
					}
					CmdMgr::GetInstance()->Do(pCmd);
				}
			}
			break;
		}
	}

	//选择
	switch(m_eEditStatus)
	{
	case DEST_SELECT:
		//选择
		if (bActive)
		{
			switch(pInputInfo->eType)
			{
			case  UIMT_MS_BTNDOWN:
				if (pInputInfo->dwData == MK_LBUTTON)
				{
					m_bDrag = TRUE;
					m_ptDragStart.x = GET_X_LPARAM(pInputInfo->lParam);
					m_ptDragStart.y = GET_Y_LPARAM(pInputInfo->lParam);

				}
				break;
			case  UIMT_MS_BTNUP:
				if (pInputInfo->dwData == MK_LBUTTON)
				{
					if (m_bDrag)
					{
						m_ptDragEnd.x = GET_X_LPARAM(pInputInfo->lParam);
						m_ptDragEnd.y = GET_Y_LPARAM(pInputInfo->lParam);
						m_bDrag = FALSE;
						//结束,开始选择//画框
						SelectFrustum cSelFrustum;
						RECT rc;
						pEditWnd->GetWndRect(&rc);
						cSelFrustum.CalculateFrustum(&rc,pEditWnd->GetCamera(),m_ptDragStart,m_ptDragEnd);
						std::vector<int>vTempArray;
						//循环可见格子
						EditMap *pMap = CommDB::GetInstance()->GetMap();
						VisibleSet *pSet = pMap->GetVisibleSet();
						DWORD *pVIArray = pSet->GetVisibleIndexArray();
						DWORD dwMapWidth = pMap->GetWidth();
						DWORD dwMapDepth = pMap->GetDepth();
						for(DWORD n = 0; n< pSet->GetVisibleIndexCount(); n++)
						{
							DWORD dwGridIndex = pVIArray[n];
							Grid *pGrid = pMap->GetGrid(dwGridIndex);
							if (pGrid->GetDoorIndex())
							{

								//计算可视格子  in  选择锥
								D3DXVECTOR3 pos[4];
								D3DXVECTOR3 vCenter;
								pMap->GetGridPosition(dwGridIndex,pos,&vCenter);
								/*float x = (float)(dwGridIndex % dwMapWidth);
								float z = (float)(dwGridIndex / dwMapWidth);

								pos[0] = D3DXVECTOR3(x,0,z);
								pos[1] = D3DXVECTOR3(x+1,0,z);
								pos[2] = D3DXVECTOR3(x,0,z+1);
								pos[3] = D3DXVECTOR3(x+1,0,z+1);*/

								
								

								if (cSelFrustum.TestPoint(&vCenter))
								{
									//这个格子被选中
									vTempArray.push_back(dwGridIndex);
								}
							}
						}

						//是否是CTRL按住
						if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
						{
							//减模式
							std::vector<int>::iterator result;

							std::vector<int> vSelIndexArray = m_vSelGridIndexArray;

							for(std::vector<int>::iterator it = vSelIndexArray.begin();
								it != vSelIndexArray.end();it++)
							{		
								int iGridIndex = *it;

								//find 
								result = find(vTempArray.begin(),vTempArray.end(),iGridIndex);
								if  (result != vTempArray.end())
								{
									result = find(m_vSelGridIndexArray.begin(),m_vSelGridIndexArray.end(),iGridIndex);
									if (result != vTempArray.end())
									{
										//找到,删除
										m_vSelGridIndexArray.erase(result);
									}
								}
							}							
						}
						else
							if (pInputInfo->byKeyBuffer[DIK_LSHIFT] & 0x80)
							{
								//添加模式
								std::vector<int>::iterator result;
								for(DWORD n = 0; n < vTempArray.size(); n++)
								{	
									int iGridIndex = vTempArray[n];
									result = find(m_vSelGridIndexArray.begin(),m_vSelGridIndexArray.end(),iGridIndex);
									if (result == m_vSelGridIndexArray.end())
									{
										m_vSelGridIndexArray.push_back(iGridIndex);
									}
								}
							}
							else
							{
								//正常选择模式
								m_vSelGridIndexArray.clear();
								for(DWORD n = 0; n < vTempArray.size(); n++)
								{
									m_vSelGridIndexArray.push_back(vTempArray[n]);
								}
							}
					}
				}
				break;
			}

		}
		break;
	case DEST_PALACE:
		//放置
		{
			if (bActive)
			{
				if (pInputInfo->dwMouseButtonState & MK_LBUTTON)
				{
					//计算选中的格子
					GameSelectRay cSelRay;
					RECT rc;
					pEditWnd->GetWndRect(&rc);
					cSelRay.CalculateRay(&rc,pEditWnd->GetCamera(),pInputInfo->ptMouse);
					//显示格子
					EditMap *pMap = CommDB::GetInstance()->GetMap();
					VisibleSet *pSet = pMap->GetVisibleSet();
					DWORD *pVIArray = pSet->GetVisibleIndexArray();
					DWORD dwMapWidth = pMap->GetWidth();
					DWORD dwMapDepth = pMap->GetDepth();
					for(DWORD n = 0; n< pSet->GetVisibleIndexCount(); n++)
					{
						DWORD dwGridIndex = pVIArray[n];
						//计算可视格子  in  选择锥
						D3DXVECTOR3 pos[4];
						D3DXVECTOR3 vCenter;
						pMap->GetGridPosition(dwGridIndex,pos,&vCenter);
						/*float x = (float)(dwGridIndex % dwMapWidth);
						float z = (float)(dwGridIndex / dwMapWidth);*/
						

						/*pos[0] = D3DXVECTOR3(x,pGrid->,z);
						pos[1] = D3DXVECTOR3(x+1,0,z);
						pos[2] = D3DXVECTOR3(x,0,z+1);
						pos[3] = D3DXVECTOR3(x+1,0,z+1);

						D3DXVECTOR3 vCenter = (pos[0] + pos[1] + pos[2] + pos[3]) / 4.0f;*/

						if (cSelRay.IntersectTri(&pos[0],NULL,NULL) || cSelRay.IntersectTri(&pos[1],NULL,NULL))
						{
							//check grid mtl 
							//equal old?
							Grid *pGrid = pMap->GetGrid(dwGridIndex);
							if (pGrid->GetDoorIndex() != iDoorIndex)
							{
								//这个格子被选中
								CmdMapSetDoor *pCmd = new CmdMapSetDoor;
								pCmd->SetDoorIndex(dwGridIndex,iDoorIndex);
								CmdMgr::GetInstance()->Do(pCmd);
							}
							break;
						}
					}
				}
			}
		}
		break;
	}
}

void ToolDoorEdit::Render()
{
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();
	render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();

	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();

	WndDoorEdit *pWndDoorEdit= pEditWnd->GetWndDoorEdit();
	DWORD dwSelIndex = pWndDoorEdit->GetCurDoorIndex();

	switch(m_eEditStatus)
	{
	case DEST_SELECT:
		if (m_bDrag)
		{
			//画拖动线
			pLayer2D->DrawWireQuadRect(m_ptDragStart.x,m_ptDragStart.y,pInputInfo->ptMouse.x,pInputInfo->ptMouse.y,0xffffffff);
		}
		DrawSelected();
		break;
	case DEST_PALACE:
		{
			D3DXMATRIX matTRS;

			//显示被点亮的格子
			static GameSelectRay cSelRay;
			RECT rc;
			pEditWnd->GetWndRect(&rc);
			cSelRay.CalculateRay(&rc,pEditWnd->GetCamera(),pInputInfo->ptMouse);
			//显示格子
			
			EditMap *pMap = CommDB::GetInstance()->GetMap();
			if (pMap)
			{
				VisibleSet *pSet = pMap->GetVisibleSet();
				DWORD *pVIArray = pSet->GetVisibleIndexArray();
				TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
				TerrainMesh::_tagGrid *pGrid(NULL);
				TerrainMesh::_tagVertex *pVertexArray = pMesh->GetVertexList();
				D3DXVECTOR3 pos[4];
				D3DXVECTOR3 vCenter;

				for(DWORD n = 0; n< pSet->GetVisibleIndexCount(); n++)
				{
					DWORD dwGridIndex = pVIArray[n];

					pGrid = pMesh->GetGrid(dwGridIndex);

					for(DWORD v= 0; v < 4; v++)
					{
						pos[v] = pVertexArray[pGrid->dwVertexIndex[v]].vPosition;
					}

					vCenter = (pos[0] + pos[1] + pos[2] + pos[3]) / 4.0f;
					if (cSelRay.IntersectTri(&pos[0],&pos[1],&pos[2],NULL,NULL) || cSelRay.IntersectTri(&pos[0],&pos[2],&pos[3],NULL,NULL))
					{
						//这个格子被选中
						pLayer3D->_DEBUG_DrawWireQuad(&pos[0],&pos[1],&pos[2],&pos[3],0xffff0000);

						D3DXMatrixTranslation(&matTRS,vCenter.x,vCenter.y,vCenter.z);
						
						RnderDoorModel(&matTRS);	

						break;
					}
				}
			}
		}
		break;
	}

	_ShowStatus();
}
void ToolDoorEdit::Create()
{
	m_pDoorModel = CGameModelManager::GetInstance()->CreateDisplayModel("ui\\door\\model",false);
	m_pDoorModel->ShowAllGroup(false);
	//m_pDoorModel->Create("ui\\door\\model");
	//m_pDoorModel->SetGroupVisible(TRUE);
	//m_pDoorModel->SelectDefaultPart();
	m_cAnimInfo.GetActionInfo()->dwCurAction = MAKEFOURCC('S','T','N','D');
	m_cAnimInfo.GetActionInfo()->bCurActionLooped = TRUE;
}

void ToolDoorEdit::Destroy(void)
{
	//SAFEDESTROY(m_pDoorModel);
}

void ToolDoorEdit::_ShowStatus(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

	//DRAW 编辑状态
	static char szTemp[256];

	sprintf(szTemp,"[Q]:%s\n",pszDoorEditStatusName[m_eEditStatus]);
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
	sprintf(szTemp,"[#a080FF80Q#FFFFFFFF]:#FFFFFF44%s",pszDoorEditStatusName[m_eEditStatus]);
	pLayer2D->TextOutEx(iStartX,iStartY,szTemp,lstrlen(szTemp));
}

void ToolDoorEdit::DrawSelected(void)
{
	DWORD dwSelCount = (DWORD)m_vSelGridIndexArray.size();
	if (!dwSelCount) return;
	render::Interface::Layer3D *pLayer3D = render::Interface::GetInstance()->GetLayer3D();
	render::Interface::Layer2D *pLayer2D = render::Interface::GetInstance()->GetLayer2D();

	D3DVIEWPORT9 tViewport;
	render::Interface::GetInstance()->GetDevice()->GetViewport(&tViewport);
	D3DXMATRIX matWrold;
	D3DXMatrixIdentity(&matWrold);

	//循环可见格子
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid	*pMapGrid(NULL);
	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagGrid *pGrid(NULL);
	TerrainMesh::_tagVertex *pVertexArray = pMesh->GetVertexList();
	
	static D3DXVECTOR3 pos[4],vCenter;;
	
	WndDoorEdit *pWndDoorEdit = CommUI::GetInstance()->GetEditWnd()->GetWndDoorEdit();
	render::Camera *pCamera = CommUI::GetInstance()->GetEditWnd()->GetCamera();
	for(DWORD n = 0;n < dwSelCount; n++)
	{
		DWORD dwGridIndex = m_vSelGridIndexArray[n];
		pMapGrid = pMap->GetGrid(dwGridIndex);
		pMap->GetGridPosition(dwGridIndex,pos,&vCenter);
		pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[1],&pos[2],0xa077ff77);
		pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[2],&pos[3],0xa077ff77);
		//画名字
		int iDoorIndex = pMapGrid->GetDoorIndex() - 1;
		if (iDoorIndex >= 0)
		{
			D3DXVec3Project(&vCenter,&vCenter,&tViewport,pCamera->GetProjectionMatrix(),
				pCamera->GetViewMatrix(),&matWrold);
			tagDoor *pDoor = pWndDoorEdit->GetDoor(iDoorIndex);
			int iTexLen = lstrlen(pDoor->szName);

			int iTextWidth = pLayer2D->CalcTextWidth(pDoor->szName,iTexLen);
			int iTextHeight= pLayer2D->GetFontSize();

			int iWidth = iTextWidth + 16;	

			int iStartX = (int)vCenter.x - iWidth / 2;
			int iStartY = (int)vCenter.y - iTextHeight/2;

			pLayer2D->DrawSolidQuad(iStartX - 2,iStartY ,iTextWidth + 8,iTextHeight + 2 ,0x80bbc5ff);

			ui::Wnd::SetUIRendState();
			pLayer2D->TextOut(iStartX + 1,iStartY + 1,pDoor->szName,iTexLen,FWC_BLACK);
			pLayer2D->TextOut(iStartX - 1,iStartY - 1,pDoor->szName,iTexLen,FWC_BLACK);
			pLayer2D->TextOut(iStartX + 1,iStartY - 1,pDoor->szName,iTexLen,FWC_BLACK);
			pLayer2D->TextOut(iStartX - 1,iStartY + 1,pDoor->szName,iTexLen,FWC_BLACK);
			pLayer2D->TextOut(iStartX + 1,iStartY,pDoor->szName,iTexLen,FWC_BLACK);
			pLayer2D->TextOut(iStartX - 1,iStartY,pDoor->szName,iTexLen,FWC_BLACK);
			pLayer2D->TextOut(iStartX,iStartY + 1,pDoor->szName,iTexLen,FWC_BLACK);
			pLayer2D->TextOut(iStartX,iStartY - 1,pDoor->szName,iTexLen,FWC_BLACK);  
			pLayer2D->TextOut(iStartX,iStartY,pDoor->szName,iTexLen,0xFF44FF55);  

		}
	}
}

void ToolDoorEdit::RnderDoorModel(const D3DXMATRIX *pMat)
{
	EditWnd * pEditWnd = CommUI::GetInstance()->GetEditWnd();
	m_dwCurTime = timeGetTime();
	m_cAnimInfo.SetWorldMatrix(pMat);
	m_cAnimInfo.SetCurrentTime(m_dwCurTime);
	m_cAnimInfo.SetViewMatrix(pEditWnd->GetCamera()->GetViewMatrix());
	m_cAnimInfo.SetProjectedMatrix(pEditWnd->GetCamera()->GetProjectionMatrix());
	m_pDoorModel->ProcessAnimJoint(&m_cAnimInfo);
	m_pDoorModel->SetModelColor(0xFFFFFFFF);
	m_pDoorModel->RenderModel();
}