#include "stdafx.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"
#include "..\..\engine\render.h"
#include "..\..\engine\ui.h"
#include "..\..\engine\math.h"

#include "..\..\EngineExtend\gameselectray.h"
#include "..\..\EngineExtend\gamebuildlist.h"
#include "..\..\EngineExtend\gamemodel.h"
#include "..\..\EngineExtend\displaymodel.h"

#include "editwnd.h"
#include "commdb.h"
#include "visibleset.h"
#include "selectfrustum.h"
#include "ToolBuildEdit.h"
#include "commui.h"
#include "map.h"
#include "mtllistbox.h"
#include "cmdmgr.h"
#include "CmdMapFillTargetHide.h"
#include "cmdmapsetbuild.h"
#include "wndselectbuild.h"
#include "cmdmapfillbuildid.h"
#include "cmdmgr.h"
#include "toolbuildedit.h"
#include "cmdmapsetbuildrotationx.h"
#include "cmdmapsetbuildrotationy.h"
#include "cmdmapsetbuildrotationz.h"
#include "CmdMapSetBuildScaling.h"
#include "CmdMapSetBuildScalingX.h"
#include "CmdMapSetBuildScalingY.h"
#include "CmdMapSetBuildScalingZ.h"
#include "toolbuildedit.h"
#include "cmdmapsetbuildheight.h"
#include "CmdMapSetBuildOffsetXZ.h"
#include "cmdmapmovebuild.h"
#include "CmdMgr.h"
#include "terrainmesh.h"
#include "CmdMapPasteBuild.h"
#include "WndWater.h"

const char *pszBuildEditStatusName[] = {"选择","放置","屋顶"};

ToolBuildEdit::ToolBuildEdit(void)
{
	m_eEditStatus = BUILD_EDIT;
	m_bDragMouse  = FALSE;
	m_eEditSubStatus  = BESST_SELECT;
	m_dwCurHoverGridIndex = 0xFFFFFFFF;
	m_bActivityGridSwitch = true;
	m_fPalaceAngle= 0.0f;
	m_ePalaceSubStatus = BPST_PALACE;
	m_dwCurHoverBuildGridIndex = 0xFFFFFFFF;
	ZeroMemory(m_cCursorAnimInfo.GetActionInfo(),sizeof(m_cCursorAnimInfo.m_tActionInfo));
	m_cCursorAnimInfo.GetActionInfo()->dwCurAction = MAKEFOURCC('S','T','N','D');
	m_cCursorAnimInfo.GetActionInfo()->bCurActionLooped = TRUE;
	m_pClipboard = NULL;

}

ToolBuildEdit::~ToolBuildEdit(void)
{
	Destroy();
}

void ToolBuildEdit::MessageProc(BOOL bActive)
{
	//切换
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	static GameSelectRay cSelRay;
	static RECT rc;
	static EditWnd *pEditWnd;
	pEditWnd = CommUI::GetInstance()->GetEditWnd();

	pEditWnd->GetWndRect(&rc);	
	cSelRay.CalculateRay(&rc,pEditWnd->GetCamera(),pInputInfo->ptMouse);

	//计算相交
	_CalculateHoverGrid();
	_CalculateHoverBuildGrid(&cSelRay);    

	//循环可见格子
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	VisibleSet *pSet = pMap->GetVisibleSet();
	DWORD *pVIArray = pSet->GetVisibleIndexArray();
	DWORD dwMapWidth = pMap->GetWidth();
	DWORD dwMapDepth = pMap->GetDepth();

	if (pInputInfo->eType == UIMT_KB_KEYDOWN)
	{
		switch(pInputInfo->dwData)
		{
		case DIK_Q:
			{
				//切换时清空选择区域
				m_vSelGridIndexArray.clear();
				m_eEditStatus = (m_eEditStatus == BUILD_EDIT)? BUILD_PALACE : BUILD_EDIT;
			}
			break;
		case DIK_W:
			{
				//WndWater * pWnd = CommUI::GetInstance()->GetEditWnd()->GetWndWater();
				//bool bVisible = pWnd->IsVisible();
				//pWnd->ShowWnd(!bVisible);
				//if (bVisible && m_vSelGridIndexArray.size() != 0)
				//{
				//	DWORD dwGridIndex = m_vSelGridIndexArray[0];
				//	Grid * pGrid = pMap->GetGrid(dwGridIndex);
				//	Grid::tagBuild * pGridBuild = pGrid->GetBuild();
				//	WORD wBuildID = pGrid->GetBuild()->wID;

				//	GameBuildList::Build * pBuild = GameBuildList::GetInstance()->AcquireBuild(wBuildID, false);
				//	GameBuildList::Build::tagModel * pModel = pBuild->GetModel();
				//	CDisplayModel * pGameModel(NULL);
				//	if (pModel && pModel->pGameModel)
				//	{
				//		pGameModel = pModel->pGameModel;
				//		pWnd->SetParameter(pGameModel->GetWater());
				//	}
				//}
			}
			break;
		case DIK_M:
			{
				if (m_eEditStatus == BUILD_MARKVISIBLE)
				{
					m_eEditStatus = BUILD_EDIT;
				}
				else
				{
					m_eEditStatus = BUILD_MARKVISIBLE;
				}
				break;
			}
			break;
		case DIK_DELETE:
			if (m_eEditStatus == BUILD_EDIT)
			{
				//删除
				CmdMapFillBuild *pCmd = new CmdMapFillBuild;
				pCmd->SetGridIndexCount((DWORD)m_vSelGridIndexArray.size());
				for(DWORD n = 0; n < (DWORD)m_vSelGridIndexArray.size(); n++)
				{
					int iGridIndex = m_vSelGridIndexArray[n];
					pCmd->SetGridIndex(n,iGridIndex);
				}
				Grid::tagBuild tBuild;
				tBuild.wID = 0xFFFF;
				tBuild.vPosOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				tBuild.vRotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				tBuild.vScaling = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
				pCmd->SetBuild(tBuild);
				CmdMgr::GetInstance()->Do(pCmd);
				//更新统计
			};
			break;
		case DIK_RETURN:
			if (m_eEditStatus == BUILD_EDIT)
			{
				_DoFixRotation();
			}
			else
				if (m_eEditStatus == BUILD_PALACE)
				{
					//旋转默认角度
					m_fPalaceAngle += D3DXToRadian(45);
				}
				break;
		case DIK_C:
			if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
			{
				// 粘贴
				if (m_eEditStatus == BUILD_EDIT)
				{
					if (m_vSelGridIndexArray.size())
					{
						//计算中心格子
						int cx = 0;
						int cy = 0;
						m_dwCopyDataCount = (DWORD)m_vSelGridIndexArray.size();
						for(DWORD n = 0; n < m_dwCopyDataCount ;n++)
						{
							DWORD dwGridIndex = m_vSelGridIndexArray[n];
							DWORD x = dwGridIndex % dwMapWidth;
							DWORD y = dwGridIndex / dwMapWidth;
							cx +=x;
							cy +=y;
						}

						cx /= m_dwCopyDataCount;
						cy /= m_dwCopyDataCount;
						SAFEDELETEARRAY(m_pClipboard);
						m_pClipboard = new tagCopyData[m_dwCopyDataCount];

						//copy sel to clip
						for(DWORD n = 0; n < (int)m_vSelGridIndexArray.size();n++)
						{
							DWORD dwGridIndex = m_vSelGridIndexArray[n];
							tagCopyData *pCD = &m_pClipboard[n];

							DWORD x = dwGridIndex % dwMapWidth;
							DWORD y = dwGridIndex / dwMapWidth;
							pCD->ox = (int)x - (int)cx;
							pCD->oy = (int)y - (int)cy;
							//计算出参考偏移

							Grid *pGrid = pMap->GetGrid(dwGridIndex);
							Grid::tagBuild *pBuild = pGrid->GetBuild();
								
							pCD->wBuildID   = pBuild->wID;
							pCD->vPosOffset = pBuild->vPosOffset;
							pCD->vRotation  = pBuild->vRotation;
							pCD->vScaling  = pBuild->vScaling;
						}
					}
				}
			}
			break;
		case DIK_V:
			if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
			{
				//复制
				if (m_eEditStatus == BUILD_EDIT && 
					m_dwCurHoverGridIndex != 0xFFFFFFFF && m_pClipboard)	
					{
						CmdMapPasteBuild *pCmd = new CmdMapPasteBuild;
						pCmd->SetClipboard(m_pClipboard,m_dwCopyDataCount);
						pCmd->SetCenter(m_dwCurHoverGridIndex % dwMapWidth,	m_dwCurHoverGridIndex / dwMapWidth);
						CmdMgr::GetInstance()->Do(pCmd);
					}
			}
			break;
		case DIK_I:
			_AlignAxisX();
			break;
		case DIK_O:
			_AlignAxisY();
			break;
		case DIK_P:
			_AlignAxisZ();
			break;
		case DIK_GRAVE:
			// 添加到不可活动表中
			if (m_vSelGridIndexArray.size() != 0)
			{
				for (size_t st = 0; st < m_vSelGridIndexArray.size(); ++st)
				{
					std::vector<int>::iterator itMap = find(m_vNonActivityGridIndex.begin(), m_vNonActivityGridIndex.end(), m_vSelGridIndexArray[st]);
					if (itMap == m_vNonActivityGridIndex.end())
						m_vNonActivityGridIndex.push_back(m_vSelGridIndexArray[st]);
				}
				m_dwCurHoverBuildGridIndex = 0xffffffff;
				m_vSelGridIndexArray.clear();
			}
			break;
		}
	}

	//选择
	if (!bActive)
		return;

	switch(m_eEditStatus)
	{
	case BUILD_EDIT:
		if (pInputInfo->byKeyBuffer[DIK_1] & 0x80)
		{
			_DoScaling();
		}else if (pInputInfo->byKeyBuffer[DIK_2] & 0x80)
		{
			_DoScalingX();
		}else if (pInputInfo->byKeyBuffer[DIK_3] & 0x80)
		{
			_DoScalingY();
		}else if (pInputInfo->byKeyBuffer[DIK_4] & 0x80)
		{
			_DoScalingZ();
		}else if (pInputInfo->byKeyBuffer[DIK_5] & 0x80)
		{
			if (pInputInfo->eType == UIMT_KB_KEYDOWN)
			{
				if (pInputInfo->dwData == DIK_ADD)
					_DoRotationX(1);
				else if (pInputInfo->dwData == DIK_SUBTRACT)
					_DoRotationX(-1);
			}else
				_DoRotationX();		//x轴旋转
		}else if (pInputInfo->byKeyBuffer[DIK_6] & 0x80)
		{
			if (pInputInfo->eType == UIMT_KB_KEYDOWN)
			{
				if (pInputInfo->dwData == DIK_ADD)
					_DoRotationY(1);
				else if (pInputInfo->dwData == DIK_SUBTRACT)
					_DoRotationY(-1);
			}else
				_DoRotationY();		//y轴旋转
		}else if (pInputInfo->byKeyBuffer[DIK_7] & 0x80)
		{
			if (pInputInfo->eType == UIMT_KB_KEYDOWN)
			{
				if (pInputInfo->dwData == DIK_ADD)
					_DoRotationZ(1);
				else if (pInputInfo->dwData == DIK_SUBTRACT)
					_DoRotationZ(-1);
			}else
				_DoRotationZ();		//z轴旋转
		}else
		{
			if (pInputInfo->byKeyBuffer[DIK_A] & 0x80)
			{
		//		if (m_vSelGridIndexArray.size() == 0)
				{
					m_bActivityGridSwitch = false;
				}
			}else
				m_bActivityGridSwitch = true;

			if (pInputInfo->byKeyBuffer[DIK_RSHIFT] & 0x80)
				_DoChangeHeight();
			else if (pInputInfo->byKeyBuffer[DIK_LSHIFT] & 0x80)
				_DoChangeOffsetAtXZ();
			else
				_DoEdit();
		}
		break;
	case BUILD_PALACE:
		//放置
		switch(m_ePalaceSubStatus)
		{
		case BPST_PALACE: 
			if (pInputInfo->byKeyBuffer[DIK_RALT] & 0X80)
			{
				//进入suck
				m_ePalaceSubStatus = BPST_SUCK;
			}
			break;
		case BPST_SUCK:
			if (!(pInputInfo->byKeyBuffer[DIK_RALT] & 0X80))
			{
				//还原ralt
				m_ePalaceSubStatus = BPST_PALACE;
			}
			break;
		}
		if (pInputInfo->dwMouseButtonState & MK_LBUTTON)
		{
			if (m_ePalaceSubStatus == BPST_PALACE)
			{
				//判断放置物件
				DWORD dwGridIndex = m_dwCurHoverGridIndex;
				if (dwGridIndex != 0xFFFFFFFF)
				{
					EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
					WndSelectBuild * pWndSelectBuild = pEditWnd->GetWndSelectBuild();
					EditMap *pMap = CommDB::GetInstance()->GetMap();

					Grid *pGrid = pMap->GetGrid(dwGridIndex);

					WORD wCurBuildID = pWndSelectBuild->GetCurBuildID();
					Grid::tagBuild *pBuild = pGrid->GetBuild();
					if (pBuild->wID != wCurBuildID)
					{
						//这个格子被选中
						CmdMapSetBuild *pCmd = new CmdMapSetBuild;
						Grid::tagBuild tBuild;
						tBuild.wID = wCurBuildID;
						tBuild.vPosOffset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
						tBuild.vRotation.x = tBuild.vRotation.z = 0.0f;
						tBuild.vScaling = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
						if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
							tBuild.vRotation.y = D3DXToRadian(rand() % 360);
						else
							tBuild.vRotation.y = m_fPalaceAngle;
						pCmd->SetBuild(dwGridIndex, tBuild);
						CmdMgr::GetInstance()->Do(pCmd);
					}
				}
			}
			else //吸取
			{
				EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
				WndSelectBuild * pWndSelectBuild = pEditWnd->GetWndSelectBuild();
				
				if (m_dwCurHoverBuildGridIndex !=0xFFFFFFFF)
				{
					WORD wBuildID = pMap->GetGrid(m_dwCurHoverBuildGridIndex)->GetBuild()->wID;
					if (wBuildID != 0xffff)
					{
						pWndSelectBuild->Select(wBuildID);
					}
				}
			}
		}
		break;
	case BUILD_MARKVISIBLE:
		{
			Proc_BuildMarkVisible();
		}
		break;
	}


}

void ToolBuildEdit::Render()
{
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();
	render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();
	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	D3DXVECTOR3 pos[4];
	D3DXVECTOR3 vCenter;
	DWORD dwCurrentTime = timeGetTime();
	WndSelectBuild * pWndSelectBuild = pEditWnd->GetWndSelectBuild();
	DWORD dwMapWidth = pMap->GetWidth();
	DWORD dwMapDepth = pMap->GetWidth();

	D3DXMATRIX matWorld,matRot,matScaling;
	const D3DXMATRIX *pBillboardMatrix = pEditWnd->GetCamera()->GetBillboard();
	const D3DXMATRIX *pBillboardMatrixY = pEditWnd->GetCamera()->GetBillboardY();

	switch(m_eEditStatus)
	{
	case BUILD_EDIT:
		switch(m_eEditSubStatus)
		{
		case BESST_SELECT:
			if (m_bDragMouse)
			{
				//画拖动线
				//pLayer2D->DrawSolidQuadRect(m_ptDragStart.x,m_ptDragStart.y,pInputInfo->ptMouse.x,pInputInfo->ptMouse.y,0x8f444455);
				pLayer2D->DrawWireQuadRect(m_ptDragStart.x,m_ptDragStart.y,pInputInfo->ptMouse.x,pInputInfo->ptMouse.y,0xffffffff);
			}
			_DrawSelected();

			break;
		case BESST_MOVE:
			{
				//画移动的物件
				if (m_vSelGridIndexArray.size() == 1)
				{
					//1.保存选中的格子索引
					DWORD dwSelGridIndex = m_vSelGridIndexArray[0];
					Grid::tagBuild *pBuild = pMap->GetGrid(dwSelGridIndex)->GetBuild();
					WORD  wSelBuildID     = pBuild->wID;
					D3DXVECTOR3 & vBuildOffset = pBuild->vPosOffset;
					D3DXVECTOR3 & vBuildRotation = pBuild->vRotation;
					D3DXVECTOR3 & vBuildScaling = pBuild->vScaling;

					DWORD dwCurGridIndex = m_dwCurHoverGridIndex;
					if (dwCurGridIndex != 0xFFFFFFFF)
					{
						GameBuildList::Build *pBuild = GameBuildList::GetInstance()->AcquireBuild(wSelBuildID,false);

						//画暗色原来模型
						{
							pMap->GetGridPosition(dwSelGridIndex,pos,&vCenter);
							D3DXMatrixTranslation(&matWorld, vCenter.x + vBuildOffset.x, vCenter.y + vBuildOffset.y, vCenter.z + vBuildOffset.z);
							D3DXMatrixRotationYawPitchRoll(&matRot, vBuildRotation.y, vBuildRotation.x, vBuildRotation.z);
							D3DXMatrixScaling(&matScaling, vBuildScaling.x, vBuildScaling.y, vBuildScaling.z);
							matWorld = matScaling * matRot * matWorld;
//*//						D3DXMatrixRotationY(&matRot,fBuildRotationY);
//							D3DXMatrixMultiply(&matWorld,&matRot,&matWorld);

							GameBuildList::Build::tagModel *pModel = pBuild->GetModel();
							CDisplayModel *pGameModel(NULL);
							if (pModel && pModel->pGameModel)
							{
								pGameModel = pModel->pGameModel;

								//在选择物件后--拖动状态
								m_cCursorAnimInfo.SetBillboard(&(matScaling * *pBillboardMatrix));
								m_cCursorAnimInfo.SetBillboardY(&(matScaling * *pBillboardMatrixY));
								m_cCursorAnimInfo.SetWorldMatrix(&matWorld);
								m_cCursorAnimInfo.SetCurrentTime(dwCurrentTime);
								m_cCursorAnimInfo.SetViewMatrix(pEditWnd->GetCamera()->GetViewMatrix());
								m_cCursorAnimInfo.SetProjectedMatrix(pEditWnd->GetCamera()->GetProjectionMatrix());
								pGameModel->SetDirLightEnable(false);
								pGameModel->ProcessAnimJoint(&m_cCursorAnimInfo);
								pGameModel->ProcessAnimTexture(&m_cCursorAnimInfo);
								pGameModel->ProcessAnimUV(&m_cCursorAnimInfo);
								pGameModel->ProcessParticleEmitter(&m_cCursorAnimInfo);
								pGameModel->ProcessLocator(&m_cCursorAnimInfo);
								pGameModel->SetModelColor(0x3F444444);
								pGameModel->ProcessAnimColor(&m_cCursorAnimInfo);
								pGameModel->ProcessVisibility(&m_cCursorAnimInfo);
								pGameModel->RenderModelTransparent();

								pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[1],&pos[2],0x30000044);
								pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[2],&pos[3],0x30000044);
							}
						}

						//2.尝试移动可能性判断
						if (pMap->GetGrid(dwCurGridIndex)->GetBuild()->wID != 0xFFFF)//已经有，不可能移动
						{
							//不可移动,画红色新位置
							pMap->GetGridPosition(dwCurGridIndex,pos,&vCenter);

							D3DXMatrixTranslation(&matWorld, vCenter.x + vBuildOffset.x, vCenter.y + vBuildOffset.y, vCenter.z + vBuildOffset.z);
							D3DXMatrixRotationYawPitchRoll(&matRot, vBuildRotation.y, vBuildRotation.x, vBuildRotation.z);
							D3DXMatrixScaling(&matScaling, vBuildScaling.x, vBuildScaling.y, vBuildScaling.z);
							matWorld = matScaling * matRot * matWorld;
//*//						D3DXMatrixRotationY(&matRot,fBuildRotationY);
							//D3DXMatrixMultiply(&matWorld,&matRot,&matWorld);

							GameBuildList::Build::tagModel *pModel = pBuild->GetModel();
							CDisplayModel *pGameModel(NULL);
							if (pModel && pModel->pGameModel)
							{
								pGameModel = pModel->pGameModel;

								m_cCursorAnimInfo.SetBillboard(&(matScaling * *pBillboardMatrix));
								m_cCursorAnimInfo.SetWorldMatrix(&matWorld);
								m_cCursorAnimInfo.SetCurrentTime(dwCurrentTime);
								m_cCursorAnimInfo.SetViewMatrix(pEditWnd->GetCamera()->GetViewMatrix());
								m_cCursorAnimInfo.SetProjectedMatrix(pEditWnd->GetCamera()->GetProjectionMatrix());
								pGameModel->SetDirLightEnable(false);
								pGameModel->ProcessAnimJoint(&m_cCursorAnimInfo);
								pGameModel->ProcessAnimTexture(&m_cCursorAnimInfo);
								pGameModel->ProcessAnimUV(&m_cCursorAnimInfo);
								pGameModel->ProcessParticleEmitter(&m_cCursorAnimInfo);
								pGameModel->ProcessLocator(&m_cCursorAnimInfo);
								pGameModel->SetModelColor(0xF0FF0000);
								pGameModel->ProcessAnimColor(&m_cCursorAnimInfo);
								pGameModel->ProcessVisibility(&m_cCursorAnimInfo);
								pGameModel->RenderModel();
								pGameModel->_DEBUG_RenderSizeBox(&matWorld,0xFFFF0000);
								

								pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[1],&pos[2],0xa00000ff);
								pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[2],&pos[3],0xa00000ff);
							}
							else
							{
								//画线框
								static D3DXVECTOR3 vCenter0(0,0.5,0);
								D3DXVECTOR3 vCenter;
								D3DXVec3TransformCoord(&vCenter,&vCenter0,&matWorld);
								pLayer3D->_DEBUG_DrawWireBox(&vCenter,0.5,0xFff4444F);
							}
						}
						else
						{
							//3.可移动,画蓝色新位置
							GameBuildList::Build *pBuild = GameBuildList::GetInstance()->AcquireBuild(wSelBuildID,false);
							pMap->GetGridPosition(dwCurGridIndex,pos,&vCenter);

							D3DXMatrixTranslation(&matWorld, vCenter.x + vBuildOffset.x, vCenter.y + vBuildOffset.y, vCenter.z + vBuildOffset.z);
							D3DXMatrixRotationYawPitchRoll(&matRot, vBuildRotation.y, vBuildRotation.x, vBuildRotation.z);
							D3DXMatrixScaling(&matScaling, vBuildScaling.x, vBuildScaling.y, vBuildScaling.z);
							matWorld = matScaling * matRot * matWorld;
//*//						D3DXMatrixRotationY(&matRot,fBuildRotationY);
							//D3DXMatrixMultiply(&matWorld,&matRot,&matWorld);

							GameBuildList::Build::tagModel *pModel = pBuild->GetModel();
							CDisplayModel *pGameModel(NULL);
							if (pModel && pModel->pGameModel)
							{
								pGameModel = pModel->pGameModel;

								m_cCursorAnimInfo.SetCurrentTime(dwCurrentTime);
								m_cCursorAnimInfo.SetBillboard(&(matScaling * *pBillboardMatrix));     
								m_cCursorAnimInfo.SetWorldMatrix(&matWorld);
								m_cCursorAnimInfo.SetViewMatrix(pEditWnd->GetCamera()->GetViewMatrix());
								m_cCursorAnimInfo.SetProjectedMatrix(pEditWnd->GetCamera()->GetProjectionMatrix());
								pGameModel->SetDirLightEnable(false);
								pGameModel->ProcessAnimJoint(&m_cCursorAnimInfo);
								pGameModel->ProcessAnimTexture(&m_cCursorAnimInfo);
								pGameModel->ProcessAnimUV(&m_cCursorAnimInfo);
								pGameModel->ProcessParticleEmitter(&m_cCursorAnimInfo);
								pGameModel->ProcessLocator(&m_cCursorAnimInfo);
								pGameModel->SetModelColor(0xf0000060);
								pGameModel->ProcessAnimColor(&m_cCursorAnimInfo);
								pGameModel->ProcessVisibility(&m_cCursorAnimInfo);
								pGameModel->RenderModel();
								pGameModel->_DEBUG_RenderSizeBox(&matWorld,0xfFFF0000);

								pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[1],&pos[2],0xa00000ff);
								pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[2],&pos[3],0xa00000ff);
							}
							else
							{
								//画线框
								static D3DXVECTOR3 vCenter0(0,0.5,0);
								D3DXVECTOR3 vCenter;
								D3DXVec3TransformCoord(&vCenter,&vCenter0,&matWorld);
								pLayer3D->_DEBUG_DrawWireBox(&vCenter,0.5,0xffff0000);
							}
						}
						
					}
				}
				else
					_DrawSelected();
			}			
			break;				
		}
		break;
	case BUILD_PALACE:
		{
			//显示被选中的物件
			DWORD dwGridIndex = m_dwCurHoverGridIndex;
			if (dwGridIndex != 0xFFFFFFFF)
			{
				pMap->GetGridPosition(dwGridIndex,pos,&vCenter);

				//这个格子被选中,显示选中位置
				pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[1],&pos[2],0xa00000ff);
				pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[2],&pos[3],0xa00000ff);

				if (m_ePalaceSubStatus == BPST_SUCK)
				{
					
				}
				else
				{
					WORD wCurBuildID = pWndSelectBuild->GetCurBuildID();
					if (wCurBuildID != 0xFFFF)
					{
						GameBuildList::Build *pBuild = GameBuildList::GetInstance()->AcquireBuild(wCurBuildID);//, false
						Grid *pGrid = pMap->GetGrid(dwGridIndex);
						Grid::tagBuild *pGridBuild = pGrid->GetBuild();

						//显示建筑--------------
						D3DXMatrixTranslation(&matWorld, vCenter.x + pGridBuild->vPosOffset.x, vCenter.y + pGridBuild->vPosOffset.y,vCenter.z + pGridBuild->vPosOffset.z);
						D3DXMatrixRotationY(&matRot,m_fPalaceAngle);//pGrid->GetBuild()->fRotationY);
						D3DXMatrixMultiply(&matWorld,&matRot,&matWorld);

						GameBuildList::Build::tagModel *pModel = pBuild->GetModel();
						CDisplayModel *pGameModel(NULL);
						if (pModel && pModel->pGameModel)
						{
							pGameModel = pModel->pGameModel;

							m_cCursorAnimInfo.SetCurrentTime(dwCurrentTime);
							m_cCursorAnimInfo.SetBillboard(pBillboardMatrix);
							//当选着了菜单里的屋面---设置的状态
							m_cCursorAnimInfo.SetBillboardY(pBillboardMatrixY);
							m_cCursorAnimInfo.SetWorldMatrix(&matWorld);
							m_cCursorAnimInfo.SetViewMatrix(pEditWnd->GetCamera()->GetViewMatrix());
							m_cCursorAnimInfo.SetProjectedMatrix(pEditWnd->GetCamera()->GetProjectionMatrix());
							pGameModel->SetDirLightEnable(false);
							pGameModel->ProcessAnimJoint(&m_cCursorAnimInfo);
							pGameModel->ProcessAnimTexture(&m_cCursorAnimInfo);
							pGameModel->ProcessAnimUV(&m_cCursorAnimInfo);
							pGameModel->ProcessParticleEmitter(&m_cCursorAnimInfo);
							pGameModel->ProcessLocator(&m_cCursorAnimInfo);
							pGameModel->SetModelColor(0xFFFFFFFF);
							pGameModel->ProcessAnimColor(&m_cCursorAnimInfo);
							pGameModel->ProcessVisibility(&m_cCursorAnimInfo);
							pGameModel->RenderModelTransparent();
						}

						//画障碍占位-------------------------
						int iCX = (int)(dwGridIndex % dwMapWidth);
						int iCZ = (int)(dwGridIndex / dwMapWidth);
						GameBuildList::Build::tagBlockData *pBlockData = pBuild->GetBlockData();
						if (pBlockData)
						{
							for(int i = pBlockData->iXMin; i <= pBlockData->iXMax; i++)
							{
								for(int j = pBlockData->iZMin; j <= pBlockData->iZMax; j++)
								{
									//求出周围的格子
									int iX = i + iCX;
									int iZ = j + iCZ;
									D3DXVECTOR3 vec3((float)iX, 0, (float)iZ);
									D3DXMATRIX matrot,matzeropoint;
									D3DXVECTOR3 vec31;
									D3DXMatrixTranslation(&matzeropoint, -(float)iCX, 0.0f, -(float)iCZ);
									matrot = matRot;
									matrot = matzeropoint * matrot;
									D3DXMatrixInverse(&matzeropoint,NULL,&matzeropoint);
									matrot = matrot * matzeropoint;
									D3DXVec3TransformCoord(&vec31,&vec3,&matrot);
									iX = (int)(vec31.x + (fabs(vec31.x - (int)vec31.x) > 0.5?1:0));
									iZ = (int)(vec31.z + (fabs(vec31.z - (int)vec31.z) > 0.5?1:0));
									if (!(iX < 0 || iZ < 0 || iX >= (int)dwMapWidth || iZ >= (int)dwMapDepth))
									{
										pMap->GetGridPosition(iX,iZ,pos,&vCenter);

										pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[1],&pos[2],0xa0a01122);
										pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[2],&pos[3],0xa0a01122);
									}
								}
							}
						}
					}
				}
			}
		}
		break;
		case BUILD_MARKVISIBLE:
		{
			//当前选中建筑所在格子
			Render_BuildMarkVisible();
		}
		break;
	}

	//显示当前模型 ==================
	if (m_dwCurHoverBuildGridIndex != 0xFFFFFFFF)
	{
		pMap->GetGridPosition(m_dwCurHoverBuildGridIndex,pos,&vCenter);
		Grid *pGrid = pMap->GetGrid(m_dwCurHoverBuildGridIndex);
		Grid::tagBuild *pBuild = pGrid->GetBuild();

		//显示建筑--------------
		D3DXMatrixTranslation(&matWorld, vCenter.x + pBuild->vPosOffset.x, vCenter.y + pBuild->vPosOffset.y, vCenter.z + pBuild->vPosOffset.z);
		D3DXMatrixRotationYawPitchRoll(&matRot, pBuild->vRotation.y, pBuild->vRotation.x, pBuild->vRotation.z);
		D3DXMatrixScaling(&matScaling, pBuild->vScaling.x, pBuild->vScaling.y, pBuild->vScaling.z);
		matWorld = matScaling * matRot * matWorld;
//*//	D3DXMatrixRotationY(&matRot,pGrid->GetBuild()->fRotationY);
//		D3DXMatrixMultiply(&matWorld,&matRot,&matWorld);

		WORD wBuildID = pGrid->GetBuild()->wID;
		if (wBuildID != 0xFFFF)
		{
			GameBuildList::Build *pBuild = GameBuildList::GetInstance()->AcquireBuild(wBuildID,false);
			GameBuildList::Build::tagModel *pModel = pBuild->GetModel();
			CDisplayModel *pGameModel(NULL);
			if (pModel && pModel->pGameModel)
			{
				pGameModel = pModel->pGameModel;

				//当选择物件后----激活状态
				//在编辑的情况下
				m_cCursorAnimInfo.SetCurrentTime(dwCurrentTime);
				m_cCursorAnimInfo.SetBillboard(&(matScaling * *pBillboardMatrix));
				m_cCursorAnimInfo.SetBillboardY(&(matScaling * *pBillboardMatrixY));
				m_cCursorAnimInfo.SetWorldMatrix(&matWorld);
				m_cCursorAnimInfo.SetViewMatrix(pEditWnd->GetCamera()->GetViewMatrix());
				m_cCursorAnimInfo.SetProjectedMatrix(pEditWnd->GetCamera()->GetProjectionMatrix());
				pGameModel->SetDirLightEnable(false);
				pGameModel->ProcessAnimJoint(&m_cCursorAnimInfo);
				pGameModel->ProcessAnimTexture(&m_cCursorAnimInfo);
				pGameModel->ProcessAnimUV(&m_cCursorAnimInfo);
				pGameModel->ProcessParticleEmitter(&m_cCursorAnimInfo);
				pGameModel->ProcessLocator(&m_cCursorAnimInfo);
				pGameModel->SetModelColor(0x80808080);
				pGameModel->ProcessAnimColor(&m_cCursorAnimInfo);
				pGameModel->ProcessVisibility(&m_cCursorAnimInfo);
				pGameModel->RenderModelTransparent();

				pGameModel->_DEBUG_RenderSizeBox(&matWorld,0xAAFF0000);
				//渲染占位
				pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[1],&pos[2],0xa0a0FF22);
				pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[2],&pos[3],0xa0a0FF22);
				const char* str = CGameModelManager::GetInstance()->GetGameModelInfoFileName(pGameModel->GetGameModelID());
				ui::Wnd::SetUIRendState();
				pLayer2D->OutPutText(100,100,str,lstrlen(str),0xffffffff);
			}
		}
	}
	_ShowStatus();
}

void ToolBuildEdit::Create()
{
	//m_pGameBuildList = new GameBuildList;
	//m_pGameBuildList->Create();	
}

void ToolBuildEdit::Destroy(void)
{
	//SAFEDESTROY(m_pGameBuildList);
	SAFEDELETEARRAY(m_pClipboard);
	m_vSelGridIndexArray.clear();
	m_vNonActivityGridIndex.clear();
}


BOOL ToolBuildEdit::FindSelectIndex(DWORD dwGridIndex)
{
	std::vector<int>::iterator itFind;
	//查找在select array 
	itFind = find(m_vSelGridIndexArray.begin(),m_vSelGridIndexArray.end(),dwGridIndex);
	if (itFind != m_vSelGridIndexArray.end())
	{
		return TRUE;
	}
	return FALSE;
}


void ToolBuildEdit::_ShowStatus(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

	//DRAW 编辑状态
	static char szTemp[256];

	EditMap *pMap = CommDB::GetInstance()->GetMap();
	DWORD dwMapWidth = pMap->GetWidth();
	int x = m_dwCurHoverGridIndex % dwMapWidth;
	int z = m_dwCurHoverGridIndex / dwMapWidth;
	DWORD dwTotalPalacedBuildCount = pMap->GetPalaceBuildCount();
	DWORD dwRefrenceBuildCount = pMap->GetRefrenceBuildCount();

	sprintf(szTemp,"Q:%s SFT:移动 CTR:旋转 ENTER:锁定旋转 DEL:删除 x:%3.3d z:%3.3d 统计:P[%d] R[%d]",pszBuildEditStatusName[m_eEditStatus],x,z,dwTotalPalacedBuildCount,dwRefrenceBuildCount);
	int iLen = lstrlen(szTemp);

	int iTextWidth = pLayer2D->CalcTextWidth(szTemp,iLen);
	int iTextHeight= pLayer2D->GetFontSize();

	int iWidth = iTextWidth + 16;	

	int iStartX = 300;
	int iStartY = pInterface->GetWndHeight() - iTextHeight - 4;

	int iTexLen = lstrlen(szTemp);

	pLayer2D->DrawSolidQuad(iStartX - 2,iStartY ,iTextWidth + 8,iTextHeight + 2 ,FWC_BACK);

	ui::Wnd::SetUIRendState();
	pLayer2D->OutPutText(iStartX + 1,iStartY + 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->OutPutText(iStartX - 1,iStartY - 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->OutPutText(iStartX + 1,iStartY - 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->OutPutText(iStartX - 1,iStartY + 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->OutPutText(iStartX + 1,iStartY,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->OutPutText(iStartX - 1,iStartY,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->OutPutText(iStartX,iStartY + 1,szTemp,iTexLen,FWC_BLACK);
	pLayer2D->OutPutText(iStartX,iStartY - 1,szTemp,iTexLen,FWC_BLACK);   
	sprintf(szTemp,"#FFA8D050Q#FFFFFFFF:#FFD0FF50%s#FFFFFFFF #FFA8D050SFT#FFFFFFFF:移动 #FFA8D050CTR#FFFFFFFF:旋转 #FFA8D050ENTER:#FFFFFFFF锁定旋转 #FFA8D050DEL#FFFFFFFF:删除 x:#FFFfD050%3.3d #FFFFFFFFz:#FFFfD050%3.3d #FFFFFFFF统计:P[#FFFfD050%d#FFFFFFFF] #FFFFFFFFR[#FFFfD050%d#FFFFFFFF]" ,pszBuildEditStatusName[m_eEditStatus],x,z,dwTotalPalacedBuildCount,dwRefrenceBuildCount);
	pLayer2D->TextOutEx(iStartX,iStartY,szTemp,lstrlen(szTemp));

}	

void ToolBuildEdit::_DrawSelected(void)
{
	DWORD dwSelCount = (DWORD)m_vSelGridIndexArray.size();
	if (!dwSelCount) return;
	render::Interface::Layer3D *pLayer3D = render::Interface::GetInstance()->GetLayer3D();
	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	//循环可见格子
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagGrid *pMeshGrid(NULL);
	TerrainMesh::_tagVertex *pVertexArray = pMesh->GetVertexList();
	DWORD dwCurrentTime = timeGetTime();

	const D3DXMATRIX *pBillboardMatrix = pEditWnd->GetCamera()->GetBillboard();
	const D3DXMATRIX *pBillboardMatrixY = pEditWnd->GetCamera()->GetBillboardY();
		
	static D3DXVECTOR3 pos[4];
	static D3DXVECTOR3 vStartXOri(-10.0f, 0.0f, 0.0f), vEndXOri(10.0f, 0.0f, 0.0f), vStart, vEnd;
	static D3DXVECTOR3 vStartYOri(0.0f, -10.0f, 0.0f), vEndYOri(0.0f, 10.0f, 0.0f);
	static D3DXVECTOR3 vStartZOri(0.0f, 0.0f, -10.0f), vEndZOri(0.0f, 0.0f, 10.0f);

	for(DWORD n = 0;n < dwSelCount; n++)
	{
		DWORD dwGridIndex = m_vSelGridIndexArray[n];

		Grid *pGrid = pMap->GetGrid(dwGridIndex);
		Grid::tagBuild *pGridBuild = pGrid->GetBuild();
		WORD wBuildID = pGrid->GetBuild()->wID;

		pMeshGrid = pMesh->GetGrid(dwGridIndex);
		//
		for(DWORD v = 0; v < 4; v++)
		{
			pos[v] = pVertexArray[pMeshGrid->dwVertexIndex[v]].vPosition;
		}

		D3DXVECTOR3 vCenter = (pos[0] + pos[1] + pos[2] + pos[3]) / 4.0f;

		if (wBuildID != 0xFFFF)
		{
			D3DXMATRIX matWorld,matRot,matScaling,mTemp;
			D3DXMatrixTranslation(&matWorld, vCenter.x + pGridBuild->vPosOffset.x, vCenter.y + pGridBuild->vPosOffset.y, vCenter.z + pGridBuild->vPosOffset.z);
			D3DXMatrixRotationYawPitchRoll(&matRot, pGridBuild->vRotation.y, pGridBuild->vRotation.x, pGridBuild->vRotation.z);
			D3DXMatrixScaling(&matScaling, pGridBuild->vScaling.x, pGridBuild->vScaling.y, pGridBuild->vScaling.z);
//			D3DXMatrixRotationX(&matRot, pGridBuild->vRotation.x);
//			D3DXMatrixRotationY(&mTemp, pGridBuild->vRotation.y);
//			matRot *= mTemp;
//			D3DXMatrixRotationZ(&mTemp, pGridBuild->vRotation.z);
//			matRot *= mTemp;
			matWorld = matScaling * matRot * matWorld;
//*//		D3DXMatrixRotationY(&matRot,pGridBuild->fRotationY);
//			D3DXMatrixMultiply(&matWorld,&matRot,&matWorld);

			GameBuildList::Build *pBuild = GameBuildList::GetInstance()->AcquireBuild(wBuildID,false);
			GameBuildList::Build::tagModel *pModel = pBuild->GetModel();
			CDisplayModel *pGameModel(NULL);
			if (pModel && pModel->pGameModel)
			{
				pGameModel = pModel->pGameModel;
				m_cCursorAnimInfo.SetCurrentTime(dwCurrentTime);
				m_cCursorAnimInfo.SetBillboard(&(matScaling * *pBillboardMatrix));
				m_cCursorAnimInfo.SetBillboardY(&(matScaling * *pBillboardMatrixY));
				m_cCursorAnimInfo.SetWorldMatrix(&matWorld);
				m_cCursorAnimInfo.SetViewMatrix(pEditWnd->GetCamera()->GetViewMatrix());
				m_cCursorAnimInfo.SetProjectedMatrix(pEditWnd->GetCamera()->GetProjectionMatrix());
				pGameModel->ProcessAnimJoint(&m_cCursorAnimInfo);
				pGameModel->ProcessAnimTexture(&m_cCursorAnimInfo);
				pGameModel->ProcessAnimUV(&m_cCursorAnimInfo);
				pGameModel->ProcessParticleEmitter(&m_cCursorAnimInfo);
				pGameModel->ProcessLocator(&m_cCursorAnimInfo);
				pGameModel->SetModelColor(0xFF000080);
				pGameModel->ProcessAnimColor(&m_cCursorAnimInfo);
				pGameModel->ProcessVisibility(&m_cCursorAnimInfo);
				pGameModel->ProcessWater(&m_cCursorAnimInfo);
				pGameModel->RenderModel();

				pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[1],&pos[2],0xa00000ff);
				pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[2],&pos[3],0xa00000ff);

/*				D3DXVec3TransformCoord(&vStart, &vStartXOri, &matWorld);
				D3DXVec3TransformCoord(&vEnd, &vEndXOri, &matWorld);
				pLayer3D->_DEBUG_DrawLine(&vStart, &vEnd, 0xffffff00);

				//vStart = vStartYOri + vCenter;
				//vEnd = vEndYOri + vCenter;
				D3DXVec3TransformCoord(&vStart, &vStartYOri, &matWorld);
				D3DXVec3TransformCoord(&vEnd, &vEndYOri, &matWorld);
				pLayer3D->_DEBUG_DrawLine(&vStart, &vEnd, 0xffffff00);

				vStart = vStartZOri + vCenter;
				vEnd = vEndZOri + vCenter;
				//D3DXVec3TransformCoord(&vStart, &vStartZOri, &matWorld);
				//D3DXVec3TransformCoord(&vEnd, &vEndZOri, &matWorld);
				pLayer3D->_DEBUG_DrawLine(&vStart, &vEnd, 0xffffff00);*/
			}
			else
			{
				//画线框
				static D3DXVECTOR3 vCenter0(0,0.5,0);
				D3DXVECTOR3 vCenter;
				D3DXVec3TransformCoord(&vCenter,&vCenter0,&matWorld);
				pLayer3D->_DEBUG_DrawWireBox(&vCenter,0.5,0xFF4444FF);
			}
		}   
	}       
}

void ToolBuildEdit::_CalculateHoverGrid()
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
	m_dwCurHoverGridIndex = 0xFFFFFFFF;
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
				m_dwCurHoverGridIndex = dwGridIndex;
			}
		}
		else
			if (cSelRay.IntersectTri(&pos[0],&pos[2],&pos[3],NULL,&fTempDist))
			{	 
				if (fTempDist < fDistance)
				{
					fDistance = fTempDist;
					m_dwCurHoverGridIndex = dwGridIndex;
				}
			}
	}
}


float ToolBuildEdit::_CalculateDir(const D3DXVECTOR3 *pPosition,const GameSelectRay *pRay)
{
	//计算向
	D3DXPLANE plane;
	plane.a = 0.0f;
	plane.b = 1.0f;
	plane.c = 0.0f;
	plane.d = -pPosition->y;

	const D3DXVECTOR3 * pOrig = pRay->GetOrigination();
	const D3DXVECTOR3 * pDir = pRay->GetDirection();
	D3DXVECTOR3 vIntersection(0,0,0);

	FM_RadialIntersectedPlane(*pOrig,*pDir,plane,vIntersection);

	//取得鼠标移动方向,移动控制
	float fDirection(0);

	D3DXVECTOR3 dir = - (vIntersection - *pPosition);
	D3DXVec3Normalize(&dir,&dir);

	if( dir.x != 0 )
	{
		float fDir =  atanf( dir.z/dir.x );
		if( dir.x < 0 )
			fDir += D3DX_PI;
		fDirection = - fDir-D3DX_PI/2;
	}
	else
		fDirection = - dir.z/fabsf(dir.z)*D3DX_PI/2-D3DX_PI/2 ;

	return fDirection;
}

void ToolBuildEdit::_DoRotationX(int iAngle)
{
	//旋转
	//计算鼠标指向的方向
	static GameSelectRay cSelRay;
	static SelectFrustum cSelFrustum;
	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	RECT rc;
	pEditWnd->GetWndRect(&rc);
	//切换
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	//循环可见格子
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	DWORD dwMapWidth = pMap->GetWidth();
	DWORD dwMapDepth = pMap->GetDepth();

	cSelRay.CalculateRay(&rc,pEditWnd->GetCamera(),pInputInfo->ptMouse);

	//设置选中中所有计算
	for(std::vector<int>::iterator it = m_vSelGridIndexArray.begin(); it != m_vSelGridIndexArray.end(); it++)
	{
		int iGridIndex = *it;

		D3DXVECTOR3 pos[4];
		D3DXVECTOR3 vCenter;
		pMap->GetGridPosition(iGridIndex,pos,&vCenter);

//		float fDirect = _CalculateDir(&vCenter, &cSelRay);
		float fDirect;
		if (iAngle == 0)
		{
			fDirect = 0.01f * (float)(pInputInfo->ptMouse.x - pInputInfo->ptMouseOld.x);
			if (fDirect == 0.0f)
				continue;
		}else
			fDirect = D3DXToRadian((float)iAngle);

		Grid * pGrid = pMap->GetGrid(iGridIndex);
		Grid::tagBuild * pGridBuild = pGrid->GetBuild();
		fDirect += pGridBuild->vRotation.x;

		//设置方向
		CmdMapSetBuildRotationX *pCmd = new CmdMapSetBuildRotationX;
		pCmd->SetBuildRotationX(iGridIndex, fDirect);
		CmdMgr::GetInstance()->Do(pCmd);
	}
}

void ToolBuildEdit::_DoRotationY(int iAngle)
{
	//旋转
	//计算鼠标指向的方向
	static GameSelectRay cSelRay;
	static SelectFrustum cSelFrustum;
	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	RECT rc;
	pEditWnd->GetWndRect(&rc);
	//切换
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	//循环可见格子
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	DWORD dwMapWidth = pMap->GetWidth();
	DWORD dwMapDepth = pMap->GetDepth();

	cSelRay.CalculateRay(&rc,pEditWnd->GetCamera(),pInputInfo->ptMouse);

	//设置选中中所有计算
	for(std::vector<int>::iterator it = m_vSelGridIndexArray.begin(); it != m_vSelGridIndexArray.end(); it++)
	{
		int iGridIndex = *it;

		D3DXVECTOR3 pos[4];
		D3DXVECTOR3 vCenter;
		pMap->GetGridPosition(iGridIndex,pos,&vCenter);
		/*float x = (float)(iGridIndex % dwMapWidth);
		float z = (float)(iGridIndex / dwMapWidth);

		pos[0] = D3DXVECTOR3(x,0,z);
		pos[1] = D3DXVECTOR3(x+1,0,z);
		pos[2] = D3DXVECTOR3(x,0,z+1);
		pos[3] = D3DXVECTOR3(x+1,0,z+1);

		D3DXVECTOR3 vCenter = (pos[0] + pos[1] + pos[2] + pos[3]) / 4.0f;*/

		//float fDirect = _CalculateDir(&vCenter,&cSelRay);
		float fDirect;
		if (iAngle == 0)
		{
			fDirect = 0.01f * (float)(pInputInfo->ptMouse.x - pInputInfo->ptMouseOld.x);
			if (fDirect == 0.0f)
				continue;
		}else
			fDirect = D3DXToRadian((float)iAngle);

		Grid * pGrid = pMap->GetGrid(iGridIndex);
		Grid::tagBuild * pGridBuild = pGrid->GetBuild();
		fDirect += pGridBuild->vRotation.y;

		//设置方向
		CmdMapSetBuildRotationY *pCmd = new CmdMapSetBuildRotationY;
		pCmd->SetBuildRotationY(iGridIndex, fDirect);
		CmdMgr::GetInstance()->Do(pCmd);
	}
}

void ToolBuildEdit::_DoRotationZ(int iAngle)
{
	//旋转
	//计算鼠标指向的方向
	static GameSelectRay cSelRay;
	static SelectFrustum cSelFrustum;
	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	RECT rc;
	pEditWnd->GetWndRect(&rc);
	//切换
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	//循环可见格子
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	DWORD dwMapWidth = pMap->GetWidth();
	DWORD dwMapDepth = pMap->GetDepth();

	cSelRay.CalculateRay(&rc,pEditWnd->GetCamera(),pInputInfo->ptMouse);

	//设置选中中所有计算
	for(std::vector<int>::iterator it = m_vSelGridIndexArray.begin(); it != m_vSelGridIndexArray.end(); it++)
	{
		int iGridIndex = *it;

		D3DXVECTOR3 pos[4];
		D3DXVECTOR3 vCenter;
		pMap->GetGridPosition(iGridIndex,pos,&vCenter);

		//float fDirect = _CalculateDir(&vCenter,&cSelRay);
		float fDirect;
		if (iAngle == 0)
		{
			fDirect = 0.01f * (float)(pInputInfo->ptMouse.x - pInputInfo->ptMouseOld.x);
			if (fDirect == 0.0f)
				continue;
		}else
			fDirect = D3DXToRadian((float)iAngle);

		Grid * pGrid = pMap->GetGrid(iGridIndex);
		Grid::tagBuild * pGridBuild = pGrid->GetBuild();
		fDirect += pGridBuild->vRotation.z;

		//设置方向
		CmdMapSetBuildRotationZ *pCmd = new CmdMapSetBuildRotationZ;
		pCmd->SetBuildRotationZ(iGridIndex, fDirect);
		CmdMgr::GetInstance()->Do(pCmd);
	}
}

void ToolBuildEdit::_DoFixRotation(void)
{
	//旋转
	//计算鼠标指向的方向
	EditMap *pMap = CommDB::GetInstance()->GetMap();

	//设置方向
	for(std::vector<int>::iterator it = m_vSelGridIndexArray.begin(); it != m_vSelGridIndexArray.end(); it++)
	{
		int iGridIndex = *it;
		Grid *pGrid = pMap->GetGrid(iGridIndex);
		float fDir = pGrid->GetBuild()->vRotation.y;
		fDir += D3DXToRadian(45);
		CmdMapSetBuildRotationY *pCmd = new CmdMapSetBuildRotationY;
		pCmd->SetBuildRotationY(iGridIndex,fDir);
		CmdMgr::GetInstance()->Do(pCmd);
	}
}


void ToolBuildEdit::_DoEdit(void)
{
	static SelectFrustum cSelFrustum;
	static GameSelectRay cSelRay;
	//切换
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	//循环可见格子
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	VisibleSet *pSet = pMap->GetVisibleSet();
	DWORD *pVIArray = pSet->GetVisibleIndexArray();
	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagGrid *pMeshGrid(NULL);
	TerrainMesh::_tagVertex *pVertexArray = pMesh->GetVertexList();
	D3DXVECTOR3 pos[4],vCenter;

	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	RECT rc;
	pEditWnd->GetWndRect(&rc);	

	std::vector<int>::iterator itFind;

	//有选中的目标,然后的鼠标在选中目标上，则开始移动式drag
	//有/无选中的目标，鼠标在空白处，则开始选择
	switch(pInputInfo->eType)
	{
	case  UIMT_MS_BTNDOWN:
		if (pInputInfo->dwData == MK_LBUTTON)
		{
			//判断是否选中物件
			m_ptDragStart.x = GET_X_LPARAM(pInputInfo->lParam);
			m_ptDragStart.y = GET_Y_LPARAM(pInputInfo->lParam);

			if (m_dwCurHoverBuildGridIndex != 0xFFFFFFFF && !(pInputInfo->byKeyBuffer[DIK_LSHIFT] & 0x80))
			{
				//查找在select array 
				itFind = find(m_vSelGridIndexArray.begin(),m_vSelGridIndexArray.end(),m_dwCurHoverBuildGridIndex);
				if (itFind != m_vSelGridIndexArray.end())
				{
					//找到，则是拖动模式
					m_eEditSubStatus = BESST_MOVE;
					m_bDragMouse = FALSE;

				}
				else
				{
					//Not found , 选择模式
					//如果点击中建筑，那么将要选中
					m_eEditSubStatus = BESST_MOVE;	
					m_vSelGridIndexArray.clear();
					m_vSelGridIndexArray.push_back(m_dwCurHoverBuildGridIndex);
					m_bDragMouse = FALSE;

					if (!m_bActivityGridSwitch)
					{
						std::vector<int>::iterator itMap = find(m_vNonActivityGridIndex.begin(), m_vNonActivityGridIndex.end(), m_dwCurHoverBuildGridIndex);
						if (itMap != m_vNonActivityGridIndex.end())
							m_vNonActivityGridIndex.erase(itMap);
					}
				}           				                           
			}
			else
			{
				m_bDragMouse = TRUE;//什么都没选中则开始画选择框
				if (!(pInputInfo->byKeyBuffer[DIK_LSHIFT] & 0x80))
				{
					m_vSelGridIndexArray.clear();
				}
				m_eEditSubStatus = BESST_SELECT;
			}
		}
		break;
	case  UIMT_MS_BTNUP:
		if (pInputInfo->dwData == MK_LBUTTON)
		{
			if (m_bDragMouse)
			{
				m_ptDragEnd.x = GET_X_LPARAM(pInputInfo->lParam);
				m_ptDragEnd.y = GET_Y_LPARAM(pInputInfo->lParam);
				m_bDragMouse  = FALSE;//鼠标拖动结束
			}

			switch(m_eEditSubStatus)
			{
			case BESST_SELECT://判断拖动选择过程中被SELECTBOX框中的物件
				{
					cSelFrustum.CalculateFrustum(&rc,pEditWnd->GetCamera(),m_ptDragStart,m_ptDragEnd);

					std::vector<int>vTempArray;
					D3DXVECTOR3 vIntersection;
					float fPickDistance = 1000000000.0F;
					DWORD dwPickGrid = 0xFFFFFFFF;

					for(DWORD n = 0; n< pSet->GetVisibleIndexCount(); n++)
					{
						DWORD dwGridIndex = pVIArray[n];
						Grid *pGrid = pMap->GetGrid(dwGridIndex);
						Grid::tagBuild *pGridBuild = pGrid->GetBuild();

						pMeshGrid = pMesh->GetGrid(dwGridIndex);

						for(DWORD v = 0; v < 4; v++)
						{
							pos[v] = pVertexArray[pMeshGrid->dwVertexIndex[v]].vPosition;
						}

						vCenter = (pos[0] + pos[1] + pos[2] + pos[3]) / 4.0f;
						D3DXMATRIX matWorld;
						D3DXMatrixTranslation(&matWorld, vCenter.x + pGridBuild->vPosOffset.x, vCenter.y + pGridBuild->vPosOffset.y, vCenter.z + pGridBuild->vPosOffset.z);

						//计算格子物件被选中
						WORD wBuildID = pGrid->GetBuild()->wID;  

						if (wBuildID != 0xFFFF)//有物件
						{
							GameBuildList::Build *pBuild = GameBuildList::GetInstance()->AcquireBuild(wBuildID,false);
							GameBuildList::Build::tagModel *pModel = pBuild->GetModel();
							CDisplayModel *pGameModel(NULL);
							if (pModel && pModel->pGameModel)
							{
								pGameModel = pModel->pGameModel;
								static render::BoundingBox cSizeBox;
								cSizeBox.Set(D3DXVECTOR3(0,0,0),0.5f);
								render::BoundingBox* pcSizeBox;
								if (pModel->pGameModel->GetLoadState() == Load_Did)
								{
									pcSizeBox = pGameModel->GetPickBox();
								}
								else
								{
									pcSizeBox = &cSizeBox;
								}


								if (cSelFrustum.TestBoundingBox(&matWorld,pcSizeBox))
								{
									std::vector<int>::iterator itMap = find(m_vNonActivityGridIndex.begin(), m_vNonActivityGridIndex.end(), dwGridIndex);
									if (itMap == m_vNonActivityGridIndex.end())
									{
										if (pInputInfo->byKeyBuffer[DIK_LALT] & 0x80)
										{
											//测试是否是和选择框id相同的，不是则排除
											//重新当前选中Build模型
											EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
											WndSelectBuild * pWndSelectBuild = pEditWnd->GetWndSelectBuild();
											WORD wCurBuildID = pWndSelectBuild->GetCurBuildID();
											Grid *pTestGrid = pMap->GetGrid(dwGridIndex);
											if (pTestGrid->GetBuild()->wID == wCurBuildID)
											{
												//这个格子被选中
												vTempArray.push_back(dwGridIndex);
											}
										}
										else
										{
											//所有
											vTempArray.push_back(dwGridIndex);
										}
									}
								}
							}
						}
					}

					////是否是Shift按住,+ -模式 存在的从选择列表减去，不在的从选择列表加入
					if (pInputInfo->byKeyBuffer[DIK_LSHIFT] & 0x80)
					{
						for(std::vector<int>::iterator it = vTempArray.begin(); it != vTempArray.end(); it++)
						{
							int iGridIndex = *it;

							//查找在select array 
							itFind = find(m_vSelGridIndexArray.begin(),m_vSelGridIndexArray.end(),iGridIndex);
							if (itFind != m_vSelGridIndexArray.end())
							{
								//找到，del
								m_vSelGridIndexArray.erase(itFind);
							}
							else
							{
								//Not found , add this index
								m_vSelGridIndexArray.push_back(iGridIndex);
							}
						}    
					}
					else
					{
						//正常选择模式
						m_vSelGridIndexArray.clear();
						for(DWORD n = 0; n < vTempArray.size(); n++)
							m_vSelGridIndexArray.push_back(vTempArray[n]);
					}
				}
				break;
			case BESST_MOVE://拖动建筑，在移动中完成处理，此处不需要处理
				{
					m_eEditSubStatus = BESST_SELECT;

					if (m_vSelGridIndexArray.size() == 1)
					{
						//1.保存选中的格子索引
						DWORD dwSelGridIndex = m_vSelGridIndexArray[0];
						WORD wCurBuildID   = pMap->GetGrid(dwSelGridIndex)->GetBuild()->wID;
						//拖动建筑->当前鼠标选中的格子
						DWORD dwCurGridIndex = m_dwCurHoverGridIndex;
						if (m_dwCurHoverGridIndex == -1)
						{
							break;
						}
						//2.尝试移动可能性判断
						if (pMap->GetGrid(dwCurGridIndex)->GetBuild()->wID != 0xFFFF)//已经有，不可能移动
						{
							//4.不可能,则不动	
						}
						else
						{
							//3.可能,设置新格子
							CmdMapMoveBuild *pCmd = new CmdMapMoveBuild;
							pCmd->SetBuildData(dwCurGridIndex,dwSelGridIndex);
							CmdMgr::GetInstance()->Do(pCmd);
							//设置新索引到选择列表
							m_vSelGridIndexArray[0] = dwCurGridIndex;
							//恢复选择状态
						}
					}   
				}
				break;
			}
		}	
		break;
	}
}

void ToolBuildEdit::_DoChangeHeight(void)
{
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	DWORD dwMapWidth = pMap->GetWidth();
	DWORD dwMapDepth = pMap->GetDepth();
	//
	//设置选中中所有计算
	for(std::vector<int>::iterator it = m_vSelGridIndexArray.begin(); it != m_vSelGridIndexArray.end(); it++)
	{
		int iGridIndex = *it;
		Grid *pGrid = pMap->GetGrid(iGridIndex);

		//	D3DXVECTOR3 pos[4];
		//	float x = (float)(iGridIndex % dwMapWidth);
		//	float z = (float)(iGridIndex / dwMapWidth);

		//	pos[0] = D3DXVECTOR3(x,0,z);
		//	pos[1] = D3DXVECTOR3(x+1,0,z);
		//	pos[2] = D3DXVECTOR3(x,0,z+1);
		//	pos[3] = D3DXVECTOR3(x+1,0,z+1);

		//	D3DXVECTOR3 vCenter = (pos[0] + pos[1] + pos[2] + pos[3]) / 4.0f;

		float fHeightOffset = -0.01f * (float)(pInputInfo->ptMouse.y - pInputInfo->ptMouseOld.y);
		if (fHeightOffset == 0.0f)
			continue;

		CmdMapSetBuildHeight *pCmd = new CmdMapSetBuildHeight;
		Grid::tagBuild *pGridBuild = pGrid->GetBuild();
		float fHeight = pGridBuild->vPosOffset.y;
		fHeight += fHeightOffset;
		pCmd->MapSetBuildHeight((DWORD)iGridIndex,fHeight);
		CmdMgr::GetInstance()->Do(pCmd);
	}
}



void ToolBuildEdit::_DoChangeOffsetAtXZ(void)
{
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
//	DWORD dwMapWidth = pMap->GetWidth();
//	DWORD dwMapDepth = pMap->GetDepth();
	//
	//设置选中中所有计算
	for(std::vector<int>::iterator it = m_vSelGridIndexArray.begin(); it != m_vSelGridIndexArray.end(); it++)
	{
		int iGridIndex = *it;
		Grid * pGrid = pMap->GetGrid(iGridIndex);

		float fPosOffsetX = 0.01f * (float)(pInputInfo->ptMouse.x - pInputInfo->ptMouseOld.x);
		float fPosOffsetZ = -0.01f * (float)(pInputInfo->ptMouse.y - pInputInfo->ptMouseOld.y);
		if (fPosOffsetX == 0.0f && fPosOffsetZ == 0.0f)
			continue;

		CmdMapSetBuildOffsetXZ * pCmd = new CmdMapSetBuildOffsetXZ;
		Grid::tagBuild *pGridBuild = pGrid->GetBuild();
		float fPosX = pGridBuild->vPosOffset.x + fPosOffsetX;
		float fPosZ = pGridBuild->vPosOffset.z + fPosOffsetZ;

		pCmd->MapSetBuildPosOffsetXZ((DWORD)iGridIndex, fPosX, fPosZ);
		CmdMgr::GetInstance()->Do(pCmd);
	}
}



void ToolBuildEdit::_DoScaling(void)
{
	//旋转
	//计算鼠标指向的方向
	D3DXVECTOR3 vOffset;
	EditMap * pMap = CommDB::GetInstance()->GetMap();

	//切换
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	//循环可见格子
	//设置选中中所有计算
	for(std::vector<int>::iterator it = m_vSelGridIndexArray.begin(); it != m_vSelGridIndexArray.end(); it++)
	{
		int iGridIndex = *it;
		Grid * pGrid = pMap->GetGrid(iGridIndex);

		vOffset.x = vOffset.y = vOffset.z = 0.01f * (float)(pInputInfo->ptMouse.x - pInputInfo->ptMouseOld.x);
		if (vOffset.x == 0.0f)
			continue;

		Grid::tagBuild *pGridBuild = pGrid->GetBuild();
		vOffset += pGridBuild->vScaling;

		//设置方向
		CmdMapSetBuildScaling *pCmd = new CmdMapSetBuildScaling;
		pCmd->SetBuildScaling(iGridIndex, vOffset);
		CmdMgr::GetInstance()->Do(pCmd);
	}
}



void ToolBuildEdit::_DoScalingX(void)
{
	//旋转
	//计算鼠标指向的方向
	float fOffset;
	EditMap * pMap = CommDB::GetInstance()->GetMap();

	//切换
	ui::Manager::_tagINPUTINFO * pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	//循环可见格子
	//设置选中中所有计算
	for(std::vector<int>::iterator it = m_vSelGridIndexArray.begin(); it != m_vSelGridIndexArray.end(); it++)
	{
		int iGridIndex = *it;
		Grid * pGrid = pMap->GetGrid(iGridIndex);

		fOffset = 0.01f * (float)(pInputInfo->ptMouse.x - pInputInfo->ptMouseOld.x);
		if (fOffset == 0.0f)
			continue;

		Grid::tagBuild * pGridBuild = pGrid->GetBuild();
		fOffset += pGridBuild->vScaling.x;

		//设置方向
		CmdMapSetBuildScalingX * pCmd = new CmdMapSetBuildScalingX;
		pCmd->SetBuildScalingX(iGridIndex, fOffset);
		CmdMgr::GetInstance()->Do(pCmd);
	}
}



void ToolBuildEdit::_DoScalingY(void)
{
	//旋转
	//计算鼠标指向的方向
	float fOffset;
	EditMap * pMap = CommDB::GetInstance()->GetMap();

	//切换
	ui::Manager::_tagINPUTINFO * pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	//循环可见格子
	//设置选中中所有计算
	for(std::vector<int>::iterator it = m_vSelGridIndexArray.begin(); it != m_vSelGridIndexArray.end(); it++)
	{
		int iGridIndex = *it;
		Grid * pGrid = pMap->GetGrid(iGridIndex);

		fOffset = 0.01f * (float)(pInputInfo->ptMouseOld.y - pInputInfo->ptMouse.y);
		if (fOffset == 0.0f)
			continue;

		Grid::tagBuild * pGridBuild = pGrid->GetBuild();
		fOffset += pGridBuild->vScaling.y;

		//设置方向
		CmdMapSetBuildScalingY * pCmd = new CmdMapSetBuildScalingY;
		pCmd->SetBuildScalingY(iGridIndex, fOffset);
		CmdMgr::GetInstance()->Do(pCmd);
	}
}



void ToolBuildEdit::_DoScalingZ(void)
{
	//旋转
	//计算鼠标指向的方向
	float fOffset;
	EditMap * pMap = CommDB::GetInstance()->GetMap();

	//切换
	ui::Manager::_tagINPUTINFO * pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	//循环可见格子
	//设置选中中所有计算
	for(std::vector<int>::iterator it = m_vSelGridIndexArray.begin(); it != m_vSelGridIndexArray.end(); it++)
	{
		int iGridIndex = *it;
		Grid * pGrid = pMap->GetGrid(iGridIndex);

		fOffset = 0.01f * (float)(pInputInfo->ptMouse.x - pInputInfo->ptMouseOld.x);
		if (fOffset == 0.0f)
			continue;

		Grid::tagBuild * pGridBuild = pGrid->GetBuild();
		fOffset += pGridBuild->vScaling.z;

		//设置方向
		CmdMapSetBuildScalingZ * pCmd = new CmdMapSetBuildScalingZ;
		pCmd->SetBuildScalingZ(iGridIndex, fOffset);
		CmdMgr::GetInstance()->Do(pCmd);
	}
}



void ToolBuildEdit::_AlignAxisX(void)
{
	ui::Manager::_tagINPUTINFO * pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	EditMap * pMap = CommDB::GetInstance()->GetMap();

	D3DXVECTOR3 v[4], c;
	if (m_dwCurHoverBuildGridIndex != 0xFFFFFFFF)
	{
		// 求鼠标 hover 的物件高度
		pMap->GetGridPosition(m_dwCurHoverBuildGridIndex, v, &c);
		Grid::tagBuild * pBuild = pMap->GetGrid(m_dwCurHoverBuildGridIndex)->GetBuild();
		if (pBuild->wID != 0xFFFF)
		{
			float fOffset = pBuild->vPosOffset.x + c.x;
			
			// 设置选中中所有计算
			for(std::vector<int>::iterator it = m_vSelGridIndexArray.begin(); it != m_vSelGridIndexArray.end(); it++)
			{
				int iGridIndex = *it;
				pMap->GetGridPosition(iGridIndex,v,&c);
				pBuild = pMap->GetGrid(iGridIndex)->GetBuild();
				float fBuildOffset = fOffset - c.x;

				CmdMapSetBuildOffsetXZ *pCmd = new CmdMapSetBuildOffsetXZ;
				pCmd->MapSetBuildPosOffsetXZ((DWORD)iGridIndex, fBuildOffset, pBuild->vPosOffset.z);
				CmdMgr::GetInstance()->Do(pCmd);
			}
		}
	}
}



void ToolBuildEdit::_AlignAxisY(void)
{
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	
	D3DXVECTOR3 v[4], c;
	if (m_dwCurHoverBuildGridIndex != 0xFFFFFFFF)
	{
		// 求鼠标 hover 的物件高度
		pMap->GetGridPosition(m_dwCurHoverBuildGridIndex,v,&c);
		Grid::tagBuild *pBuild = pMap->GetGrid(m_dwCurHoverBuildGridIndex)->GetBuild();
		if (pBuild->wID != 0xFFFF)
		{
			float fHeight = pBuild->vPosOffset.y + c.y;
			
			//设置选中中所有计算
			for(std::vector<int>::iterator it = m_vSelGridIndexArray.begin(); it != m_vSelGridIndexArray.end(); it++)
			{
				int iGridIndex = *it;
				pMap->GetGridPosition(iGridIndex,v,&c);
				float fBuildHeight = fHeight - c.y;

				CmdMapSetBuildHeight *pCmd = new CmdMapSetBuildHeight;
				/*Grid::tagBuild *pGridBuild = pGrid->GetBuild();
				float fHeight = pGridBuild->fHeight;
				fHeight += fHeightOffset;*/
				pCmd->MapSetBuildHeight((DWORD)iGridIndex,fBuildHeight);
				CmdMgr::GetInstance()->Do(pCmd);
			}
		}
	}
}



void ToolBuildEdit::_AlignAxisZ(void)
{
	ui::Manager::_tagINPUTINFO * pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	EditMap * pMap = CommDB::GetInstance()->GetMap();
	
	D3DXVECTOR3 v[4], c;
	if (m_dwCurHoverBuildGridIndex != 0xFFFFFFFF)
	{
		// 求鼠标 hover 的物件高度
		pMap->GetGridPosition(m_dwCurHoverBuildGridIndex,v,&c);
		Grid::tagBuild * pBuild = pMap->GetGrid(m_dwCurHoverBuildGridIndex)->GetBuild();
		if (pBuild->wID != 0xFFFF)
		{
			float fOffset = pBuild->vPosOffset.z + c.z;
			
			//设置选中中所有计算
			for(std::vector<int>::iterator it = m_vSelGridIndexArray.begin(); it != m_vSelGridIndexArray.end(); it++)
			{
				int iGridIndex = *it;
				pMap->GetGridPosition(iGridIndex, v, &c);
				pBuild = pMap->GetGrid(iGridIndex)->GetBuild();
				float fBuildOffset = fOffset - c.z;

				CmdMapSetBuildOffsetXZ * pCmd = new CmdMapSetBuildOffsetXZ;
				pCmd->MapSetBuildPosOffsetXZ((DWORD)iGridIndex, pBuild->vPosOffset.x, fBuildOffset);
				CmdMgr::GetInstance()->Do(pCmd);
			}      
		}
	}
}



void ToolBuildEdit::_CalculateHoverBuildGrid(GameSelectRay *pRay)
{
	DWORD dwCurTime = timeGetTime();
	std::vector<int>vTempArray;
	D3DXVECTOR3 vIntersection;
	float fPickDistance = 100000000000.0F;
	DWORD dwPickGrid = 0xFFFFFFFF;
	D3DXVECTOR3 pos[4];
	D3DXVECTOR3 vCenter; 
	static D3DXMATRIX matWorld, matRot, matScaling;
	Grid *pGrid(NULL);
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	VisibleSet *pSet = pMap->GetVisibleSet();
	DWORD *pVIArray = pSet->GetVisibleIndexArray();
	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagGrid *pMeshGrid(NULL);
	TerrainMesh::_tagVertex *pVertexArray = pMesh->GetVertexList();

	for(DWORD n = 0; n< pSet->GetVisibleIndexCount(); n++)
	{
		DWORD dwGridIndex = pVIArray[n];

		pGrid = pMap->GetGrid(dwGridIndex);
		Grid::tagBuild *pGridBuild = pGrid->GetBuild();

		pMeshGrid = pMesh->GetGrid(dwGridIndex);

		for(DWORD i = 0; i < 4; i++)
		{
			pos[i] = pVertexArray[pMeshGrid->dwVertexIndex[i]].vPosition;
		}

		vCenter = (pos[0] + pos[1] + pos[2] + pos[3]) / 4.0f;

		D3DXMatrixTranslation(&matWorld, vCenter.x + pGridBuild->vPosOffset.x, vCenter.y + pGridBuild->vPosOffset.y, vCenter.z + pGridBuild->vPosOffset.z);
		D3DXMatrixRotationYawPitchRoll(&matRot, pGridBuild->vRotation.y, pGridBuild->vRotation.x, pGridBuild->vRotation.z);
		D3DXMatrixScaling(&matScaling, pGridBuild->vScaling.x, pGridBuild->vScaling.y, pGridBuild->vScaling.z);
		matWorld = matScaling * matRot * matWorld;

		//计算格子物件被选中
		WORD wBuildID = pGrid->GetBuild()->wID;  

		if (wBuildID != 0xFFFF)//有物件
		{
			std::vector<int>::iterator itMap = find(m_vNonActivityGridIndex.begin(), m_vNonActivityGridIndex.end(), dwGridIndex);
			if (m_bActivityGridSwitch)
			{
				if (itMap != m_vNonActivityGridIndex.end())
					continue;
			}else
			{
				if (itMap == m_vNonActivityGridIndex.end())
					continue;
			}

			GameBuildList::Build *pBuild = GameBuildList::GetInstance()->AcquireBuild(wBuildID,false);
			GameBuildList::Build::tagModel *pModel = pBuild->GetModel();

			CDisplayModel *pGameModel(NULL); 
			if (pModel && pModel->pGameModel&&pModel->pGameModel->GetLoadState() == Load_Did)
			{
				pGameModel = pModel->pGameModel;

				//判断是否选择
				float fDis;
				//详细计算
				//判断建筑内模型是否为空
				/*if (pGameModel->GetGroups().size())
				{
					pGrid->m_cBuildModelAnimInfo.SetWorldMatrix(&matWorld);
					pGrid->m_cBuildModelAnimInfo.SetCurrentTime(dwCurTime);
					pGameModel->ProcessAnimJoint(&pGrid->m_cBuildModelAnimInfo);
					if (pRay->TestGameModel(&matWorld,pGameModel,&fDis))
					{
						if (fDis < fPickDistance)
						{
							dwPickGrid = dwGridIndex; 
							fPickDistance = fDis;
						}
					}
				}
				else
				{
				}*/
				//计算sizebox
				//计算box
				render::BoundingBox *pBoundingBox = pGameModel->GetPickBox();
				if (pBoundingBox)
				{
					if (pBoundingBox->TestRadialIntersected(&matWorld,pRay->GetOrigination(),pRay->GetDirection(),fDis))
					{
						if (fDis < fPickDistance)
						{
							dwPickGrid = dwGridIndex;
							fPickDistance = fDis;
						}
					}
				}
				else
				{
					dwGridIndex = 0;
					fDis = 0;
				}
			}
			else
			{
				//计算box
				static render::BoundingBox boundingbox;
				static D3DXVECTOR3 vCenter0(0,0.5,0);
				static D3DXVECTOR3 vCenter;
				float  fDis;

				D3DXVec3TransformCoord(&vCenter,&vCenter0,&matWorld);
				boundingbox.Set(vCenter,0.5);
				if (boundingbox.TestRadialIntersected(pRay->GetOrigination(),pRay->GetDirection(),fDis))
				{
					if (fDis < fPickDistance)
					{
						dwPickGrid = dwGridIndex;
						fPickDistance = fDis;
					}
				}
			}
		}
	}

	//std::vector<int>::iterator itMap = find(m_vNonActivityGridIndex.begin(), m_vNonActivityGridIndex.end(), dwPickGrid);
	//if (m_bActivityGridSwitch)
	//{
	//	if (itMap == m_vNonActivityGridIndex.end())
	//		m_dwCurHoverBuildGridIndex = dwPickGrid;
	//}else
	//{
	//	if (itMap != m_vNonActivityGridIndex.end())
			m_dwCurHoverBuildGridIndex = dwPickGrid;
//	}
}


void ToolBuildEdit::Reset(void)
{
	m_vSelGridIndexArray.clear();
}

void ToolBuildEdit::Render_BuildMarkVisible(void)
{
	if (!m_vSelGridIndexArray.size())
		return ;
	DWORD dwSelGridIndex = m_vSelGridIndexArray[0];

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D *pLayer3D = render::Interface::GetInstance()->GetLayer3D();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	Grid * pMapGrid(NULL);
	DWORD dwMapWidth = pMap->GetWidth();
	DWORD dwMapDepth = pMap->GetDepth();
	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagGrid *pMeshGrid(NULL);
	TerrainMesh::_tagVertex *pVertexArray = pMesh->GetVertexList();
	static D3DXVECTOR3 pos[4];
	VisibleSet *pVisibleSet = pMap->GetVisibleSet();
	DWORD *pVIArray = pVisibleSet->GetVisibleIndexArray();

	//循环格子，
	//如果有格子标志等于当前选中的建筑的则画
	
	for(DWORD n = 0; n< pVisibleSet->GetVisibleIndexCount(); n++)
	{
		DWORD dwGridIndex = pVIArray[n];
		pMapGrid = pMap->GetGrid(dwGridIndex);
		DWORD dwTargetHideGridIndex = pMapGrid->GetTargetHideGridIndex();
		if (dwTargetHideGridIndex == dwSelGridIndex)
		{
			//在地图内,显示格子
			pMeshGrid = pMesh->GetGrid(dwGridIndex);

			for(DWORD v = 0; v < 4; v++)
			{
				pos[v] = pVertexArray[pMeshGrid->dwVertexIndex[v]].vPosition;
			}
			pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[1],&pos[2],0x80F5CE00);
								pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[2],&pos[3],0x80F5CE00);
		}
	}

	Draw_BuildMarkVisibleSelect();
}

void ToolBuildEdit::Proc_BuildMarkVisible(void)
{
	//取得鼠标
	//取得键盘
	//切换
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	EditMap *pMap = CommDB::GetInstance()->GetMap();

	if (pInputInfo->eType == UIMT_MS_BTNDOWN)
	{
		if (pInputInfo->dwData & MK_LBUTTON)
		{
			if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80) 
			{
				if (m_vSelGridIndexArray.size() == 1)
				{
					//擦除	
					CmdMapFillTargetHide *pCmd = new CmdMapFillTargetHide;
					pCmd->AddGridIndex(m_dwCurHoverGridIndex);
					pCmd->SetTargetHideGridIndex(0xFFFFFFFF);
					CmdMgr::GetInstance()->Do(pCmd);
				}
			}
			else
				if (pInputInfo->byKeyBuffer[DIK_LSHIFT] & 0x80)
				{
					if (m_vSelGridIndexArray.size() == 1)
					{
						//增加
						CmdMapFillTargetHide *pCmd = new CmdMapFillTargetHide;
						pCmd->AddGridIndex(m_dwCurHoverGridIndex);
						pCmd->SetTargetHideGridIndex(m_vSelGridIndexArray[0]);
						CmdMgr::GetInstance()->Do(pCmd);
					}
				}
				else
				{
					m_vSelGridIndexArray.clear();
					if (m_dwCurHoverBuildGridIndex != 0xFFFFFFFF)
					{
						m_vSelGridIndexArray.push_back(m_dwCurHoverBuildGridIndex);
					}
				}
		}
	}

	//处理增加揭顶标志
	

	//处理减少揭顶标志

}

void ToolBuildEdit::Draw_BuildMarkVisibleSelect(void)
{
	if (m_vSelGridIndexArray.size() !=1 )
		return ;
	render::Interface::Layer3D *pLayer3D = render::Interface::GetInstance()->GetLayer3D();
	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	//循环可见格子
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
	TerrainMesh::_tagGrid *pMeshGrid(NULL);
	TerrainMesh::_tagVertex *pVertexArray = pMesh->GetVertexList();
	DWORD dwCurrentTime = timeGetTime();

	const D3DXMATRIX *pBillboardMatrix = pEditWnd->GetCamera()->GetBillboard();
	const D3DXMATRIX *pBillboardMatrixY = pEditWnd->GetCamera()->GetBillboardY();

	static D3DXVECTOR3 pos[4];

	DWORD dwGridIndex = m_vSelGridIndexArray[0];

	Grid *pGrid = pMap->GetGrid(dwGridIndex);
	Grid::tagBuild *pGridBuild = pGrid->GetBuild();

	WORD wBuildID = pGrid->GetBuild()->wID;

	pMeshGrid = pMesh->GetGrid(dwGridIndex);
	for(DWORD v = 0; v < 4; v++)
	{
		pos[v] = pVertexArray[pMeshGrid->dwVertexIndex[v]].vPosition;
	}

	D3DXVECTOR3 vCenter = (pos[0] + pos[1] + pos[2] + pos[3]) / 4.0f;

	if (wBuildID != 0xFFFF)
	{
		D3DXMATRIX matWorld,matRot,matScaling;
		D3DXMatrixTranslation(&matWorld, vCenter.x + pGridBuild->vPosOffset.x, vCenter.y + pGridBuild->vPosOffset.y, vCenter.z + pGridBuild->vPosOffset.z);
		D3DXMatrixRotationYawPitchRoll(&matRot, pGridBuild->vRotation.y, pGridBuild->vRotation.x, pGridBuild->vRotation.z);
		D3DXMatrixScaling(&matScaling, pGridBuild->vScaling.x, pGridBuild->vScaling.y, pGridBuild->vScaling.z);
		matWorld = matScaling * matRot * matWorld;
//*//	D3DXMatrixRotationY(&matRot,pGrid->GetBuild()->fRotationY);
//		D3DXMatrixMultiply(&matWorld,&matRot,&matWorld);

		GameBuildList::Build *pBuild = GameBuildList::GetInstance()->AcquireBuild(wBuildID,false);
		GameBuildList::Build::tagModel *pModel = pBuild->GetModel();
		CDisplayModel *pGameModel(NULL);
		if (pModel && pModel->pGameModel)
		{
			pGameModel = pModel->pGameModel;
			m_cCursorAnimInfo.SetCurrentTime(dwCurrentTime);
			m_cCursorAnimInfo.SetBillboard(&(matScaling * *pBillboardMatrix));
			m_cCursorAnimInfo.SetWorldMatrix(&matWorld);
			m_cCursorAnimInfo.SetViewMatrix(pEditWnd->GetCamera()->GetViewMatrix());
			m_cCursorAnimInfo.SetProjectedMatrix(pEditWnd->GetCamera()->GetProjectionMatrix());
			pGameModel->ProcessAnimJoint(&m_cCursorAnimInfo);
			pGameModel->ProcessAnimTexture(&m_cCursorAnimInfo);
			pGameModel->ProcessAnimUV(&m_cCursorAnimInfo);
			pGameModel->ProcessParticleEmitter(&m_cCursorAnimInfo);
			pGameModel->ProcessLocator(&m_cCursorAnimInfo);
			pGameModel->SetModelColor(0x40408040);
			pGameModel->ProcessAnimColor(&m_cCursorAnimInfo);
			pGameModel->ProcessVisibility(&m_cCursorAnimInfo);
			pGameModel->RenderModelTransparent();

			pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[1],&pos[2],0xa00000ff);
			pLayer3D->_DEBUG_DrawTriangle(&pos[0],&pos[2],&pos[3],0xa00000ff);
		}
		else
		{
			//画线框
			static D3DXVECTOR3 vCenter0(0,0.5,0);
			D3DXVECTOR3 vCenter;
			D3DXVec3TransformCoord(&vCenter,&vCenter0,&matWorld);
			pLayer3D->_DEBUG_DrawWireBox(&vCenter,0.5,0xFF44ff44);
		}
	}       
}
