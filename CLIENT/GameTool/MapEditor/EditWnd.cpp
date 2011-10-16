#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\frostengine\math.h"
#include "..\..\frostengine\input.h"

#include "..\..\gameengine\gameskybox.h"
#include "..\..\gameengine\gamebuildlist.h"
//#include "..\..\gameengine\GameGraphics2D.h"

#include "CommDB.h"
#include "GameWorld.h"
#include "control.h"
#include "TimeSystem.h"
#include "TerrainTileSetWnd.h"
#include "aboutwnd.h"
#include "editwnd.h"
#include "map.h"
#include "editwnd.h"
#include "mtllistbox.h"
#include "listboxsecurity.h"
#include "listboxwar.h"
#include "mtledittool.h"
#include "blockedittool.h"
#include "selectfrustum.h"
#include "editwnd.h"
#include "newmapdlg.h"
#include "cmdmgr.h"
#include "changewnd.h"
#include "toolwaredit.h"
#include "toolsecurityedit.h"
#include "dooreditwnd.h"
#include "toolDoorEdit.h"
#include "toolbuildedit.h"
#include "TerrainMesh.h"
#include "SetTileTool.h"
#include "wndterrainheightedit.h"
//#include "wndheightedit.h"
#include "ToolTerrainHeight.h"
//#include "toolheightedit.h""
#include "editwnd.h"
#include "ToolBuildEdit.h"
#include "terrainmesh.h"
#include "wndminimap.h"
#include "toolgridvisibleedit.h"
#include "toolweathereffectflagedit.h"
#include "commui.h"
#include "onode.h"
#include "setting.h"
#include "toolpaintcolor.h"
#include "editwnd.h"
#include "wndlightadjust.h"
#include "progressbar.h"
#include "setting.h"
#include "light.h"
#include "WndExport.h"
#include "toolenveffectflagedit.h"
#include "toolenvcreatureflagedit.h"
#include "wndrandomreplace.h"
#include "wndcamerapath.h"
#include "wndfogadjust.h"
#include "wndselectbuild.h"
#include "WndSetTerrainTile.h"
#include "WndSetSkyBox.h"
#include "WndWater.h"

static const char *pszModeName[] = {/*"水面",*/"地表","地形","地物","材质类型","阻挡类型","切换点","安全区标志","城战区标志","环境效果标志","环境生物标志","地表可见标志","喷涂颜色","天气标志","摄像机路径"};

static const char *strRegionHead = "CLS-RGN";	// 文件头
static const long  lRegionVersion = 1;		// 版本号
static const char * pszMapVersion = "1.10";		//地图版本号



EditWnd::EditWnd(void):															 
m_pMenuBar(NULL)
{
	m_pCamera = NULL;
	m_eCamOpType = CCO_NONE;

	m_bShowGrid = TRUE;	
	m_bShowMap  = TRUE;

	m_dwStartTime = 0;

	m_dwCurFrame  = 0;

	m_pToolMtlEdit = NULL;
	m_pToolBlockEdit = NULL;
	m_pToolGridVisibleEdit = NULL;
	m_pWndPopupDoorEdit  = NULL;
	m_pWndLightAdjust		 = NULL;
	m_pWndFogAdjust		 = NULL;

	m_pWndRandomReplace = NULL;

	m_pToolDoorEdit = NULL;

	m_dwRenderFlag = TERRAINMASK | BUILDINGMASK | GRIDMASK | LIGHTMASK ;

	m_bHasEditFileName = FALSE;

	m_dwAutoSaveTimer = timeGetTime();

	m_bWireframeMode = FALSE;   
	m_bLockCamera = FALSE;

	m_pToolBuildEdit = NULL;
	m_bEditSunLightDirection = FALSE;
//	m_eCursorStatus = CURSOR_NORMAL;
	m_bShowNormal = FALSE;
	m_bShowFog = FALSE;
	m_bShowSkybox = FALSE;
	m_bShowTexture = TRUE;
}

EditWnd::~EditWnd()
{
}

BOOL EditWnd::Create(const char *pszName, const RECT *pRect, DWORD dwStyle, ui::Wnd *pParent)
{
	Setting::InitInstance();
	Setting::GetInstance()->LoadSetting();

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

//	CreateCursor();

	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

	int   iWidth = pInterface->GetWndWidth();
	int   iHeight= pInterface->GetWndHeight();

	RECT rcWnd = { 0,0,iWidth,iHeight};

	ui::Wnd::Create(pszName, &rcWnd,dwStyle, NULL); 

	/////////////////////////////////////////////////
	m_pCamera = new render::Camera;
	VERIFY(m_pCamera);
	float aspect = (float)iWidth/(float)iHeight;
	m_pCamera->SetAspect(aspect);	
	m_pCamera->SetFarClipPlane(Setting::GetInstance()->GetViewDistance());
	m_pCamera->SetPosition(&D3DXVECTOR3(0.0f,8.0f,-5.0f));
	m_pCamera->SetView(&D3DXVECTOR3(0,0,0));
	m_pCamera->SetFOVY(D3DXToRadian(45.0f));
	/////////////////////////////////////////////////
	m_pFrustum = new render::Frustum;

	CreatePopupWnd();

	//菜单
	CreateMenuBar();

	//初始化数据
	m_eEditStatus = EST_TERRAINTEXTURE;

	//建立模式菜单
	_CreateModeMenu();

	_CreateEditUI();

	_CreateEditTool();

	//灯光模型
	m_pDirectionalLight = new light::DirectionalLight;

	_NewMap(24,24);

	m_pGameSkybox = new GameSkybox;
	if(m_pGameSkybox->Create(0))
	{

	}


	return TRUE;
}

void EditWnd::Destroy()
{
	SAFEDESTROY(m_pGameSkybox);
	SAFEDELETE(m_pDirectionalLight);
	Setting::GetInstance()->SaveSetting();
	Setting::ExitInstance();

	EditMap *pMap = CommDB::GetInstance()->GetMap();
	SAFEDESTROY(pMap);
	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
	SAFEDESTROY(pMesh);

	_DestroyPopupWnd();

	_DestroyEditTool();

	_DestroyEditUI();

	_DestroyModeMenu();

	SAFEDELETE(m_pFrustum);

	SAFEDESTROY(m_pAboutWnd);

	DestroyMenuBar();

//	DestroyCursor();

	SAFEDELETE(m_pCamera);

	ui::Wnd::Destroy();
}

void EditWnd::Update()
{
	//游戏时间刷新
	TimeSystem::GetInstance()->Update();

	render::Interface *pInterface = render::Interface::GetInstance();
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	TerrainMesh* pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();


	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	BOOL  &bActive = m_tWindowInfo.bActive;
	//OutputConsole("%d\n",bActive);

	if ( (dwStyle & FWS_VISIBLE))
	{
		if (!_UpdateCamera())//摄像机改变
		{
			//菜单
			MenuBarProc();
			//模式菜单
			ModeMenuProc();

			if (pMap && pTerrainMesh)
			{
				//编辑
				switch(m_eEditStatus)
				{
				case EST_TERRAINTEXTURE:
					if(pInputInfo->eType == UIMT_MS_MOVE)
						pTerrainMesh->CalculateIntersect();
					m_pToolSetTile->MessageProc(bActive);
					break;
				case EST_TERRAINHEIGHT:
					if(pInputInfo->eType == UIMT_MS_MOVE)
						pTerrainMesh->CalculateIntersect();
					m_pToolTerrainHeight->MessageProc(bActive);
					break;
				case EST_BUILDING:
					m_pToolBuildEdit->MessageProc(bActive);
					break;
				case EST_MATERIAL:
					m_pToolMtlEdit->MessageProc(bActive);
					break;
				case EST_BLOCKFLAG:
					m_pToolBlockEdit->MessageProc(bActive);
					break;
				case EST_SWITCH:
					m_pToolDoorEdit->MessageProc(bActive);
					break;
				case EST_SECURITYFLAG:
					m_pToolSecurityEdit->MessageProc(bActive);
					break;
				case EST_WARFLAG:
					m_pToolWarEdit->MessageProc(bActive);
					break;
				case EST_ENVEFFECTFLAG:
					m_pToolEnvEffectFlagEdit->MessageProc(bActive);
					break;
				case EST_ENVCREATUREFLAG:
					m_pToolEnvCreatureFlagEdit->MessageProc(bActive);
					break;
				case EST_GRIDVISIBLE:
					m_pToolGridVisibleEdit->MessageProc(bActive);
					break;
				case EST_PAINTCOLOR:
					if(pInputInfo->eType == UIMT_MS_MOVE)
						pTerrainMesh->CalculateIntersect();
					m_pToolPaintColor->MessageProc(bActive);
					break;
				case EST_WEATHEREFFECTEDIT:
					m_pToolWeatherEffectFlagEdit->MessageProc(bActive);
					break;
				case EST_CAMERAPATH:
					//m_pToolCameraPathEdit->MessageProc(bActive);
					break;
				}
			}
		}

		//刷新FRUSTUM	
		m_pFrustum->CalculateFrustum(m_pCamera->GetViewMatrix(),m_pCamera->GetProjectionMatrix());
		//刷新map
		EditMap *pMap = CommDB::GetInstance()->GetMap();
		if (pMap)
		{
			pMap->Update(m_pFrustum);
		}

		if (m_pNewMapDlg->CheckPopUp())
		{
			if (m_pNewMapDlg->IsOK())
			{
				m_pNewMapDlg->EndPopUp();
				//新建地图
				int iWidth = m_pNewMapDlg->GetWidth();
				int iDepth = m_pNewMapDlg->GetDepth();
				_NewMap(iWidth,iDepth);
			}
		}
		if (m_pWndSetTerrainTile->CheckPopUp())
		{
			if (m_pWndSetTerrainTile->IsOK())
			{
				// 为图素面板获取贴图句柄
				render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();

				render::Interface::Layer3D::ShaderGroup *pShaderGroup = pLayer3D->GetShaderGroup(pTerrainMesh->GetShaderHandle());

				DWORD dwTextureCount = pShaderGroup->GetTextureCount(0);

				RECT rc = {0, 0, 64, 64};
				for(DWORD i = 0; i < 16; i++)
				{
					if(i < dwTextureCount)
					{
						DWORD dwTextureHandle = pShaderGroup->GetTextureHandle(0,i);
						m_pWndTerrainTileSet->SetCheckBoxTextureHandle(i, dwTextureHandle, rc);
						m_pWndTerrainTileSet->ShowCheckBox(i);
					}
					else
					{
						m_pWndTerrainTileSet->ShowCheckBox(i, FALSE);
					}
					
				}
				m_pWndSetTerrainTile->EndPopUp();
				
			}
		}
		if (m_pWndSetSkyBox->CheckPopUp())
		{
			if (m_pWndSetSkyBox->IsOK())
			{
				
				m_pWndSetSkyBox->EndPopUp();

			}
		}

		if (m_pWndPopupDoorEdit->CheckPopUp())
		{
			if (m_pWndPopupDoorEdit->IsOK())
			{
				//调整入口点数据
				tagDoor *pDoor = new tagDoor;
				m_pWndPopupDoorEdit->GetDoor(pDoor);
				m_pWndDoorEdit->InsertDoor(pDoor);
			}
		}

		if (m_pWndExport->CheckPopUp())
		{
			if (m_pWndExport->IsOK())
			{
				//导出地图
				ExportMap(m_pWndExport->GetControlCheckSurface()->IsChecked(),
					m_pWndExport->GetControlCheckHeightmap()->IsChecked(),
					m_pWndExport->GetControlCheckColormap()->IsChecked(),
					m_pWndExport->GetControlCheckGrid()->IsChecked(),
					m_pWndExport->GetControlCheckNode()->IsChecked(),m_pWndExport->GetControlCheckSound()->IsChecked());
			}
		}

		if (m_pWndLightAdjust->CheckPopUp())
		{
			if (m_pWndLightAdjust->IsOK())
			{
				//调整光照数据
				if (pMap)
				{
					pMap->SetSunLightAmbientColor(m_pWndLightAdjust->GetAmbientColor());
					pMap->SetSunLightDiffuseColor(m_pWndLightAdjust->GetDirectionalColor());
				}				
			}
		}

		if (m_pWndFogAdjust->CheckPopUp())
		{
			/*if (m_pWndFogAdjust->IsOK())
			{*/
				//调整雾
				if (pMap)
				{
					pMap->SetFogEnable(m_pWndFogAdjust->GetFogEnable());
					if (pMap->GetFogEnable())
					{
						m_bShowFog = TRUE;
						pMap->SetFogColor(m_pWndFogAdjust->GetFogColor());
						pMap->SetFogStart(m_pWndFogAdjust->GetFogStart());
						pMap->SetFogEnd(m_pWndFogAdjust->GetFogEnd());
					}
					else
					{
						m_bShowFog = FALSE;
					}
					
				}
			/*}*/
		}

		if (m_pWndRandomReplace->CheckPopUp())
		{
			//开始随机替换
			if (m_pWndRandomReplace->IsOK())
			{
				ui::ListBox *pListBox = m_pWndRandomReplace->GetControlListBox();
				int iTotal = pListBox->GetItemCount();
				//开始全图查找
				int iBaseCount = 0;
				char szTemp[256];
				m_pWndRandomReplace->GetControlMinEditBox()->GetEditText(szTemp,256);
				int iMinDisntance = atoi(szTemp);
				m_pWndRandomReplace->GetControlMaxEditBox()->GetEditText(szTemp,256);
				int iMaxDisntance = atoi(szTemp);
				if (iMaxDisntance <= iMinDisntance)
				{
					iMaxDisntance = iMinDisntance + 1;
				}

				for(DWORD n = 0; n < (DWORD)(pMap->GetDepth() * pMap->GetWidth());n++)
				{
					Grid *pGrid = pMap->GetGrid(n);
					Grid::tagBuild *pBuild = pGrid->GetBuild();
                    WORD wBuildID = pBuild->wID;
			
					if (wBuildID != 0xFFFF)
					{					
						iBaseCount++;
						if (iBaseCount > (iMinDisntance + rand() % (iMaxDisntance - iMinDisntance)))
						{
							//随机替换
							int iRandom = rand() % (iTotal + 1);
							if (iRandom < iTotal)
							{
								const char *p = pListBox->GetItemText(iRandom);
								WORD wLabel = (WORD)(p[0]);
								WORD wIndex = (WORD)atoi(p+1);
								pBuild->wID =(WORD)((wIndex & 0x0fff) | ((wLabel << 12) & 0xf000));
							}

							iBaseCount = 0;
						}
					}
				}
			}
		}

		_CheckHotKey();
		if (m_bEditSunLightDirection)
		{
			m_pDirectionalLight->HandleMessage(m_pCamera->GetViewMatrix());
		}
		pMap->SetLightDirection(m_pDirectionalLight->GetLightDirection());
	}

	if (m_theBuildTree.CheckChangeSel())
	{
		//ChangeModel(m_theModelFileTree.GetModelIndex());
	}

	_DoAutoSave();



	ui::Wnd::UpdateChild();


}

void EditWnd::Render(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();
	render::Interface::Layer3D * pLayer3D = pInterface->GetLayer3D();

	ui::Wnd::RenderDefault();

	pInterface->GetDevice()->SetRenderState(D3DRS_FOGENABLE,FALSE);

	EditMap *pMap = CommDB::GetInstance()->GetMap();

	DWORD dwFogColor = (pMap)? pMap->GetFogColor() : 0xFFFFFFFF;
	pInterface->Clear(dwFogColor);
 
	float fFogStart = (pMap)? pMap->GetFogStart() : 1.0f;
	float fFogEnd = (pMap)? pMap->GetFogEnd() : 30.0f;

	// Enable fog blending.
	pInterface->GetDevice()->SetRenderState(D3DRS_FOGENABLE, m_bShowFog);
	if (m_bShowFog)
	{
		// Set the fog color.
		pInterface->GetDevice()->SetRenderState(D3DRS_FOGCOLOR, dwFogColor);

		// Set fog parameters.
		pInterface->GetDevice()->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
		pInterface->GetDevice()->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&fFogStart));
		pInterface->GetDevice()->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&fFogEnd));
		pInterface->GetDevice()->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
	}

	if (m_bShowSkybox&&m_pGameSkybox)
	{
		m_pGameSkybox->Render(m_pCamera);
	}

	_SetTransform();
	
	if (pMap)
	{
		//计算光照
		//pMap->CalculateLighting();

		DWORD dwRenderFlag = m_dwRenderFlag;

		switch(m_eEditStatus)
		{
		case EST_TERRAINTEXTURE:
		case EST_TERRAINHEIGHT:
			dwRenderFlag |= TERRAINMASK;
			break;
		case EST_BUILDING:
			dwRenderFlag |= BUILDINGMASK;
			break;
		case EST_MATERIAL:
			dwRenderFlag |= MATERIALMASK;
			break;
		case EST_BLOCKFLAG:
			dwRenderFlag |= BLOCKMASK;
			break;
		case EST_SWITCH:
			dwRenderFlag |= SWITCHMASK;
			break;
		case EST_SECURITYFLAG:
			dwRenderFlag |= SECURITYMASK;
			break;
		case EST_WARFLAG:
			dwRenderFlag |= WARMASK;
			break;
		case EST_WEATHEREFFECTEDIT:
			dwRenderFlag |= WEATHEREFFECTMASK;
			break;
		case EST_ENVCREATUREFLAG:
			dwRenderFlag |= ENVCREATUREMASK;
			break;
		case EST_ENVEFFECTFLAG:
			dwRenderFlag |= ENVEFFECTMASK;
			break;
		case EST_CAMERAPATH:
			//m_pToolCameraPathEdit
			break;
		}

		if (dwRenderFlag & TERRAINMASK)
		{
			TerrainMesh* pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
			if(pTerrainMesh)
				pTerrainMesh->Render(m_bShowNormal,m_bShowTexture, m_pCamera);

		}

		pMap->Render(dwRenderFlag, m_pCamera, m_pFrustum);	
	}

	static char szTmp[128];

	LPDIRECT3DDEVICE9 pDevice = pInterface->GetDevice();

	switch(m_eEditStatus)
	{
	case EST_TERRAINTEXTURE:
		m_pToolSetTile->Render();
		break;
	case EST_TERRAINHEIGHT:
		m_pToolTerrainHeight->Render();
		break;				
	case EST_BUILDING:
		m_pToolBuildEdit->Render();
		break;
	case EST_MATERIAL:
		m_pToolMtlEdit->Render();
		break;
	case EST_BLOCKFLAG:
		m_pToolBlockEdit->Render();
		break;
	case EST_SWITCH:
		m_pToolDoorEdit->Render();
		break;
	case EST_SECURITYFLAG:
		m_pToolSecurityEdit->Render();
		break;
	case EST_WARFLAG:
		m_pToolWarEdit->Render();
		break;
	case EST_ENVEFFECTFLAG:
		m_pToolEnvEffectFlagEdit->Render();
		break;
	case EST_ENVCREATUREFLAG:
		m_pToolEnvCreatureFlagEdit->Render();
		break;
	case EST_GRIDVISIBLE:
		m_pToolGridVisibleEdit->Render();
		break;
	case EST_WEATHEREFFECTEDIT:
		m_pToolWeatherEffectFlagEdit->Render();		
		break;
	case EST_CAMERAPATH:
		//m_pToolCameraPathEdit->Render();
		break;
	}

	//Setup Alpha OP
	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	D3DXVECTOR3 vecDir;
	static D3DLIGHT9 light;
	ZeroMemory( &light, sizeof(D3DLIGHT9) );
	light.Type       = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r  = 1.0f;
	light.Diffuse.g  = 1.0f;
	light.Diffuse.b  = 1.0f;
	light.Diffuse.a  = 1.0f;
	light.Specular.r = 1.0f;
	light.Specular.g = 1.0f;
	light.Specular.b = 1.0f;
	light.Specular.a = 1.0f;
	light.Ambient.a = 0.0f;
	light.Ambient.r = 0.0f;
	light.Ambient.g = 0.0f;
	light.Ambient.b = 0.0f;
	vecDir = *CommDB::GetInstance()->GetMap()->GetLightDirection();
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
	light.Range       = 20.0f;
	light.Falloff = 0;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	//pInterface->GetLayer3D()->_DEBUG_DrawWireBox(&TargetPos,0.05f,0xffff0000);
	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
	mtrl.Diffuse.r = 1.0f;
	mtrl.Diffuse.g = 1.0f;
	mtrl.Diffuse.b = 1.0f;
	mtrl.Diffuse.a = 1.0f;
	mtrl.Ambient.r = 1.0f;
	mtrl.Ambient.g = 1.0f;
	mtrl.Ambient.b = 1.0f;
	mtrl.Ambient.a = 1.0f;
	mtrl.Specular.r = 1.0f;
	mtrl.Specular.g = 1.0f;
	mtrl.Specular.b = 1.0f;
	mtrl.Specular.a = 1.0f;
	mtrl.Power = 50.0f;
	mtrl.Emissive.r = 0.0f;
	mtrl.Emissive.g = 0.0f;
	mtrl.Emissive.b = 0.0f;
	mtrl.Emissive.a = 0.0f;

	pLayer3D->SetMaterial(&mtrl);
	pLayer3D->SetLight(&light);
	pLayer3D->SetAmbient(0xffffffff);
	pLayer3D->SetMipMap(TRUE);
	pLayer3D->SetFogEnable(m_bShowFog);
	pLayer3D->RenderTest();
	pLayer3D->RenderBlendLow();
	pLayer3D->EndEffectsState();
	Water::CManager::GetInstance()->Render(m_bShowFog);
	//DisplayTextureProjective();
	pLayer3D->RenderBlend();
	pLayer3D->Clear();

	_ShowStatus();
	
	switch(m_eEditStatus)
	{				
	
	/*case EST_ENVEFFECTFLAG:
		m_pToolEnvEffectFlagEdit->Render();
		break;*/
	/*case EST_ENVCREATUREFLAG:
		m_pToolEnvCreatureFlagEdit->Render();
		break;*/
	case EST_PAINTCOLOR:
		m_pToolPaintColor->Render();
		break;
	//case EST_CAMERAPATH:
	//	//m_pToolCameraPathEdit->Render();
	//	break;
	}
	
	ui::Wnd::RenderChild();

	D3DXMATRIX mView;
	D3DXVECTOR3 vPos  = *m_pCamera->GetPosition() - *m_pCamera->GetView();
	D3DXVec3Normalize(&vPos,&vPos);
	D3DXVECTOR3 vView = D3DXVECTOR3(0,0,0);
	vPos *= 7.5f;
	D3DXMatrixLookAtLH(&mView,&vPos,&vView,&D3DXVECTOR3(0.0f,1.0f,0.0f));

	pInterface->GetDevice()->SetTransform(D3DTS_VIEW,&mView);
	if (m_bEditSunLightDirection)
	{
		m_pDirectionalLight->Render();
	}

	D3DVIEWPORT9 tViewPort,tViewPortOld;
	tViewPort.X = 0;
	tViewPort.Y = 668;
	tViewPort.Width = 130;
	tViewPort.Height= 100;
	tViewPort.MinZ = 0.0f;
	tViewPort.MaxZ = 1.0f;
	pInterface->GetDevice()->GetViewport(&tViewPortOld);
	pInterface->GetDevice()->SetViewport(&tViewPort);
	_ShowAxis();
	pInterface->GetDevice()->SetViewport(&tViewPortOld);

	_SetTransform();

	
	_ShowInformation();
	

	//pLayer2D->_DEBUG_TextOutCache(100,100);

}

int EditWnd::_GetFps(void)
{
	static char temp[256];
	static DWORD dwFpsTime = timeGetTime();
	static DWORD dwCount   = 0;
	dwCount ++;
	static int fps = 0 ;
	if ( timeGetTime() - dwFpsTime > 2000)
	{
		fps = dwCount / 2 ;
		dwCount = 0;		
		dwFpsTime = timeGetTime();
	}
	return fps;
}

void EditWnd::_SetMenuBarItemCheck(void)
{
	m_pMenuBar->SetCheckMenuItem(2,0,m_dwRenderFlag & TERRAINMASK);
	m_pMenuBar->SetCheckMenuItem(2,1,m_dwRenderFlag & BUILDINGMASK);
	m_pMenuBar->SetCheckMenuItem(2,2,m_dwRenderFlag & BLOCKMASK);
	m_pMenuBar->SetCheckMenuItem(2,3,m_dwRenderFlag & MATERIALMASK);
	m_pMenuBar->SetCheckMenuItem(2,4,m_dwRenderFlag & SWITCHMASK);
	m_pMenuBar->SetCheckMenuItem(2,5,m_dwRenderFlag & WARMASK);
	m_pMenuBar->SetCheckMenuItem(2,6,m_dwRenderFlag & SECURITYMASK);
	m_pMenuBar->SetCheckMenuItem(2,7,m_dwRenderFlag & ENVEFFECTMASK);
	m_pMenuBar->SetCheckMenuItem(2,8,m_dwRenderFlag & WEATHEREFFECTMASK);
	m_pMenuBar->SetCheckMenuItem(2,9,m_dwRenderFlag & ENVCREATUREMASK);
	m_pMenuBar->SetCheckMenuItem(2,11,m_dwRenderFlag & GRIDMASK);
	m_pMenuBar->SetCheckMenuItem(2,13,m_bWireframeMode);
	m_pMenuBar->SetCheckMenuItem(2,14,m_bShowFog);


	/*m_pMenuBar->SetCheckMenuItem(3, 0, m_bShowHeavyRain);
	m_pMenuBar->SetCheckMenuItem(3, 1, m_bShowLightRain);
	m_pMenuBar->SetCheckMenuItem(3, 3, m_bShowHeavySnow);
	m_pMenuBar->SetCheckMenuItem(3, 4, m_bShowLightSnow);
	m_pMenuBar->SetCheckMenuItem(3, 6, m_bShowHeavyFog);
	m_pMenuBar->SetCheckMenuItem(3, 7, m_bShowLightFog);
	m_pMenuBar->SetCheckMenuItem(3, 9, m_bShowEnvFog);*/
}

void EditWnd::MenuBarProc(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();
	render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();

	//菜单
	DWORD dwMenu,dwMenuItem;
	if (m_pMenuBar->TestSelMenu(dwMenu,dwMenuItem))
	{	
		switch(dwMenu)
		{
		case 0://文件
			switch(dwMenuItem)
			{
			case 0:
				{
					m_pNewMapDlg->BeginPopUp();
				}
				break;
			case 1://打开项目文件
				{
					OnOpenFile();
					
				}
				break;
			case 2://保存
				{
					OnSaveFile();
				}
				break;
			case 3://另存
				{
					OnSaveFileAs();
				}
				break;
			case 5://导出MAP文件
				{
					if (!m_pWndExport->CheckPopUp())
						m_pWndExport->BeginPopUp();
				}
				break;
			case 6://导入SERVER文件
				{
					_ExportRgnFile();
				}
				break;
			case 7://导出声音文件
				{
					_ExportSoundSourceListFile();	
				}
				break;
			case 9://退出   Alt+X
				{
					PostQuitMessage(0);
				}
				break;
			default:
				break;
			}
			break;
		case 1:
			{
				switch(dwMenuItem)
				{
				case 0://导出地图
					{
						SaveMiniMap();
					}
					break;
				case 1:	// 读地图参考
					{
						// Load Reference MiniMap
						OPENFILENAME ofn;       
						static char szFile[_MAX_PATH];

						// Initialize OPENFILENAME
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = pInterface->GetWndHandle();
						ofn.lpstrFile = szFile;
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = "TGA File (*.tga)\0*.tga\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = NULL;
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_FILEMUSTEXIST;

						char szOldDirectory[_MAX_PATH];
						GetCurrentDirectory(_MAX_PATH,szOldDirectory);

						// Display the Open dialog box. 
						if (!GetOpenFileName(&ofn))
							return ;

						SetCurrentDirectory(szOldDirectory);

						m_pWndMiniMap->LoadReference(ofn.lpstrFile);
					}
					break;
				case 2:
					{
						CommDB::GetInstance()->GetMap()->ClearUnusedBuild();
					}
					break;
				case 3://计算阻挡
					{
						EditMap *pMap = CommDB::GetInstance()->GetMap();
						pMap->RecalculateBlock();
					}
					break;
				case 4://环境光
					m_pWndLightAdjust->BeginPopUp();
					break;
				case 6:
					{	// Import HeightMap
						OPENFILENAME ofn;       
						static char szFile[_MAX_PATH];

						// Initialize OPENFILENAME
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = pInterface->GetWndHandle();
						ofn.lpstrFile = szFile;
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = "FLOAT File (*.tga)\0*.tga\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = NULL;
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_FILEMUSTEXIST;

						char szOldDirectory[_MAX_PATH];
						GetCurrentDirectory(_MAX_PATH,szOldDirectory);

						// Display the Open dialog box. 
						if (!GetOpenFileName(&ofn))
							return ;

						SetCurrentDirectory(szOldDirectory);

						TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
						pTerrainMesh->ImportHeightMap(ofn.lpstrFile);
					}
					break;
				case 7:
					{	// Export HeightMap
						OPENFILENAME ofn;       
						static char szFile[_MAX_PATH];

						// Initialize OPENFILENAME
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = pInterface->GetWndHandle();
						ofn.lpstrFile = szFile;
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = "FLOAT File (*.tga)\0*.tga\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = NULL;
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_FILEMUSTEXIST;

						char szOldDirectory[_MAX_PATH];
						GetCurrentDirectory(_MAX_PATH,szOldDirectory);

						// Display the Open dialog box. 
						if (!GetSaveFileName(&ofn))
							return ;

						SetCurrentDirectory(szOldDirectory);

						if (strstr(szFile,".tga") == 0)
						{
                            sprintf(szFile, "%s.tga", ofn.lpstrFile);
						}

						TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
						pTerrainMesh->ExportHeightMap(ofn.lpstrFile);
					}
					break;
				case 9:
					{	// Import VisibleMap
						OPENFILENAME ofn;       
						static char szFile[_MAX_PATH];

						// Initialize OPENFILENAME
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = pInterface->GetWndHandle();
						ofn.lpstrFile = szFile;
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = "TGA File (*.tga)\0*.tga\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = NULL;
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_FILEMUSTEXIST;

						char szOldDirectory[_MAX_PATH];
						GetCurrentDirectory(_MAX_PATH,szOldDirectory);

						// Display the Open dialog box. 
						if (!GetOpenFileName(&ofn))
							return ;

						SetCurrentDirectory(szOldDirectory);

						EditMap *pMap = CommDB::GetInstance()->GetMap();
						pMap->ImportVisibleMap(ofn.lpstrFile);
					}
					break;
				case 10:
					{	// Export VisibleMap
						OPENFILENAME ofn;       
						static char szFile[_MAX_PATH];

						// Initialize OPENFILENAME
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = pInterface->GetWndHandle();
						ofn.lpstrFile = szFile;
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = "TGA File (*.tga)\0*.tga\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = NULL;
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_FILEMUSTEXIST;

						char szOldDirectory[_MAX_PATH];
						GetCurrentDirectory(_MAX_PATH,szOldDirectory);

						// Display the Open dialog box. 
						if (!GetSaveFileName(&ofn))
							return ;

						SetCurrentDirectory(szOldDirectory);

						EditMap *pMap = CommDB::GetInstance()->GetMap();
						pMap->ExportVisibleMap(ofn.lpstrFile);
					}
					break;
				case 12:
					{	// Import BlockMap
						OPENFILENAME ofn;       
						static char szFile[_MAX_PATH];

						// Initialize OPENFILENAME
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = pInterface->GetWndHandle();
						ofn.lpstrFile = szFile;
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = "TGA File (*.tga)\0*.tga\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = NULL;
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_FILEMUSTEXIST;

						char szOldDirectory[_MAX_PATH];
						GetCurrentDirectory(_MAX_PATH,szOldDirectory);

						// Display the Open dialog box. 
						if (!GetOpenFileName(&ofn))
							return ;

						SetCurrentDirectory(szOldDirectory);

						EditMap *pMap = CommDB::GetInstance()->GetMap();
						pMap->ImportBlockMap(ofn.lpstrFile);
					}
					break;
				case 13:
					{	// Export BlockMap
						OPENFILENAME ofn;       
						static char szFile[_MAX_PATH];

						// Initialize OPENFILENAME
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = pInterface->GetWndHandle();
						ofn.lpstrFile = szFile;
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = "TGA File (*.tga)\0*.tga\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = NULL;
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_FILEMUSTEXIST;

						char szOldDirectory[_MAX_PATH];
						GetCurrentDirectory(_MAX_PATH,szOldDirectory);

						// Display the Open dialog box. 
						if (!GetSaveFileName(&ofn))
							return ;

						SetCurrentDirectory(szOldDirectory);

						EditMap *pMap = CommDB::GetInstance()->GetMap();
						pMap->ExportBlockMap(ofn.lpstrFile);
					}
					break;

				case 15:
					{	// Import WE EditMap
						OPENFILENAME ofn;       
						static char szFile[_MAX_PATH];

						// Initialize OPENFILENAME
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = pInterface->GetWndHandle();
						ofn.lpstrFile = szFile;
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = "TGA File (*.tga)\0*.tga\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = NULL;
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_FILEMUSTEXIST;

						char szOldDirectory[_MAX_PATH];
						GetCurrentDirectory(_MAX_PATH,szOldDirectory);

						// Display the Open dialog box. 
						if (!GetOpenFileName(&ofn))
							return ;

						SetCurrentDirectory(szOldDirectory);

						EditMap *pMap = CommDB::GetInstance()->GetMap();
						pMap->ImportWeatherEffectMap(ofn.lpstrFile);
					}
					break;
				case 17:
					{
						// Import ColorMap
						OPENFILENAME ofn;       
						static char szFile[_MAX_PATH];

						// Initialize OPENFILENAME
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = pInterface->GetWndHandle();
						ofn.lpstrFile = szFile;
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = "TGA File (*.tga)\0*.tga\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = NULL;
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_FILEMUSTEXIST;

						char szOldDirectory[_MAX_PATH];
						GetCurrentDirectory(_MAX_PATH,szOldDirectory);

						// Display the Open dialog box. 
						if (!GetOpenFileName(&ofn))
							return ;

						SetCurrentDirectory(szOldDirectory);

						TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
						pTerrainMesh->ApplyColorMap(ofn.lpstrFile);
					}
					break;
				case 18://EXPORT COLORMAP
					{
						OPENFILENAME ofn;       
						static char szFile[_MAX_PATH];

						// Initialize OPENFILENAME
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = pInterface->GetWndHandle();
						ofn.lpstrFile = szFile;
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = "TGA File (*.tga)\0*.tga\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = NULL;
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_FILEMUSTEXIST;

						char szOldDirectory[_MAX_PATH];
						GetCurrentDirectory(_MAX_PATH,szOldDirectory);

						// Display the Open dialog box. 
						if (!GetSaveFileName(&ofn))
							return ;

						SetCurrentDirectory(szOldDirectory);

						sprintf(szFile, "%s.tga", ofn.lpstrFile);

						TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
						pTerrainMesh->ExportColorMap(ofn.lpstrFile);
					}
					break;
				case 20://Import Material EditMap
					{
						OPENFILENAME ofn;       
						static char szFile[_MAX_PATH];

						// Initialize OPENFILENAME
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = pInterface->GetWndHandle();
						ofn.lpstrFile = szFile;
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = "TGA File (*.tga)\0*.tga\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = "导入地表材质";
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_FILEMUSTEXIST;

						char szOldDirectory[_MAX_PATH];
						GetCurrentDirectory(_MAX_PATH,szOldDirectory);

						// Display the Open dialog box. 
						if (!GetOpenFileName(&ofn))
							return ;

						SetCurrentDirectory(szOldDirectory);

						EditMap *pMap = CommDB::GetInstance()->GetMap();
						pMap->ImportMaterialMap(ofn.lpstrFile);
					}					
					break;
				case 22:
					{
						//Import Env Creature Map
						OPENFILENAME ofn;       
						static char szFile[_MAX_PATH];

						// Initialize OPENFILENAME
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = pInterface->GetWndHandle();
						ofn.lpstrFile = szFile;
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = "TGA File (*.tga)\0*.tga\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = NULL;
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_FILEMUSTEXIST;

						char szOldDirectory[_MAX_PATH];
						GetCurrentDirectory(_MAX_PATH,szOldDirectory);

						// Display the Open dialog box. 
						if (!GetOpenFileName(&ofn))
							return ;

						SetCurrentDirectory(szOldDirectory);

						EditMap *pMap = CommDB::GetInstance()->GetMap();
						pMap->ImportEnvCreateMap(ofn.lpstrFile);
					}
					break;
				case 23:
					{
						m_pWndFogAdjust->BeginPopUp();
						EditMap *pMap = CommDB::GetInstance()->GetMap();
						m_pWndFogAdjust->SetFogColor(pMap->GetFogEnable());
						m_pWndFogAdjust->SetFogColor(pMap->GetFogColor());
						m_pWndFogAdjust->SetFogStart(pMap->GetFogStart());
						m_pWndFogAdjust->SetFogEnd(pMap->GetFogEnd());
					}
					break;
				case 24:
					{
						render::Interface *pInterface = render::Interface::GetInstance();
						render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

						OPENFILENAME ofn;       // common dialog box structure
						static char szFile[_MAX_PATH];       // buffer for file name

						// Initialize OPENFILENAME
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = pInterface->GetWndHandle();
						ofn.lpstrFile = szFile;
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = "Camera File (*)\0*\0All Files\0*.*\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = NULL;
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_FILEMUSTEXIST;
						static char szOldDirectory[_MAX_PATH];
						GetCurrentDirectory(_MAX_PATH,szOldDirectory);
						if (GetOpenFileName(&ofn))
						{
							SetCurrentDirectory(szOldDirectory);
							ImportCamera(szFile);
						}
					}
					break;
				case 25:
					//导出摄像机文件
					OnExportCamera();
					break;
				case 26:
					//导出摄像机文件
					m_pWndSetTerrainTile->BeginPopUp();
					
					break;
				case 27:
					//导出摄像机文件
					m_pWndSetSkyBox->BeginPopUp();

					break;
				}
			}
			break;
		case 2://查看
			{
				switch(dwMenuItem)
				{
				case 0://地形
					{
						if (m_dwRenderFlag & TERRAINMASK)
						{
							m_dwRenderFlag &= ~TERRAINMASK;
						}
						else
							m_dwRenderFlag |= TERRAINMASK;
					}
					break;
				case 1://地物
					{
						if (m_dwRenderFlag & BUILDINGMASK)
						{
							m_dwRenderFlag &= ~BUILDINGMASK;
						}
						else
							m_dwRenderFlag |= BUILDINGMASK;
					}
					break;
				case 2://阻挡
					{
						if (m_dwRenderFlag & BLOCKMASK)
						{
							m_dwRenderFlag &= ~BLOCKMASK;
						}
						else
							m_dwRenderFlag |= BLOCKMASK;
					}
					break;
				case 3://地表
					{
						if (m_dwRenderFlag & MATERIALMASK)
						{
							m_dwRenderFlag &= ~MATERIALMASK;
						}
						else
							m_dwRenderFlag |= MATERIALMASK;
					}
					break;
				case 4://切换点
					{
						if (m_dwRenderFlag & SWITCHMASK)
						{
							m_dwRenderFlag &= ~SWITCHMASK;
						}
						else
							m_dwRenderFlag |= SWITCHMASK;


					}
					break;
				case 5://战场
					{
						if (m_dwRenderFlag & WARMASK)
						{
							m_dwRenderFlag &= ~WARMASK;
						}
						else
							m_dwRenderFlag |= WARMASK;
					}
					break;
				case 6://安全区
					{
						if (m_dwRenderFlag & SECURITYMASK)
						{
							m_dwRenderFlag &= ~SECURITYMASK;
						}
						else
							m_dwRenderFlag |= SECURITYMASK;
					}
					break;
				case 7://环境效果
					{
						if (m_dwRenderFlag & ENVEFFECTMASK)
						{
							m_dwRenderFlag &= ~ENVEFFECTMASK;
						}
						else
							m_dwRenderFlag |= ENVEFFECTMASK;


					}
					break;
				case 8://WEATHER
					{
						if (m_dwRenderFlag & WEATHEREFFECTMASK)
						{
							m_dwRenderFlag &= ~WEATHEREFFECTMASK;
						}
						else
							m_dwRenderFlag |= WEATHEREFFECTMASK;
					}
					break;
				case 9://env creature
					{
						if (m_dwRenderFlag & ENVCREATUREMASK)
						{
							m_dwRenderFlag &= ~ENVCREATUREMASK;
						}
						else
							m_dwRenderFlag |= ENVCREATUREMASK;
					}
					break;
				case 11://网格
					{
						if (m_dwRenderFlag & GRIDMASK)
						{
							m_dwRenderFlag &= ~GRIDMASK;
						}
						else
							m_dwRenderFlag |= GRIDMASK;
					}					
					break;
				case 12://光
					{
						if (m_dwRenderFlag & LIGHTMASK)
						{
							m_dwRenderFlag &= ~LIGHTMASK;
						}
						else
							m_dwRenderFlag |= LIGHTMASK;						
					}
					break;
				case 13://wireframe
					{
						m_bWireframeMode = !m_bWireframeMode;
						pLayer3D->SetFillMode((m_bWireframeMode)? D3DFILL_WIREFRAME : D3DFILL_SOLID);
					}
					break;
				case 14://Grid
					{
						m_bShowFog = !m_bShowFog;
					}
					break;
				default:
					break;
				}				
			}
			break;
		case 3: // 天气
			{
				switch(dwMenuItem)
				{
				case 0: // 大雨
					{						
					}
					break;
				case 1: // 小雨
					{						
					}
					break;
				case 3: // 大雪
					{
					}
					break;
				case 4: // 小雪
					{
					}
					break;
				case 6: // 大雾
					{
					}
					break;
				case 7: // 小雾
					{
					}
					break;
				case 9: // 环境雾
					{
					}
					break;
				}
			}
			break;
		case 4:// 帮助
			{
				switch(dwMenuItem)
				{
				case 0://关于
					{
						m_pAboutWnd->Popup();
					}
					break;
				}
			}
			break;
		case 5:
			{
			}
			break;
		default:
			break;
		}
	}
	_SetMenuBarItemCheck();
}


void EditWnd::CreateMenuBar(void)
{
	//建立三个
	//FILE | Edit | VIEW | HELP | 
	int iWidth = render::Interface::GetInstance()->GetWndWidth();
	RECT rc;
	m_pMenuBar = new ui::MenuBar;
	SetRect(&rc,2,2,iWidth -4,2+20);
	m_pMenuBar->Create("Menu",&rc,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG,this);
	//文件菜单 0
	DWORD dwFileMenu = m_pMenuBar->AddMenu("File");
	m_pMenuBar->AddMenuItem(dwFileMenu,"New   Ctrl+N"); //0
	m_pMenuBar->AddMenuItem(dwFileMenu,"Open  Ctrl+O"); //1
	m_pMenuBar->AddMenuItem(dwFileMenu,"Save  Ctrl+S"); //2
	m_pMenuBar->AddMenuItem(dwFileMenu,"Save As");		//3
	m_pMenuBar->DisableMenuItem(dwFileMenu,m_pMenuBar->AddMenuItem(dwFileMenu,"───────"));//4
	m_pMenuBar->AddMenuItem(dwFileMenu,"Export MAP File"); //5
	m_pMenuBar->AddMenuItem(dwFileMenu,"Export RGN File"); //6
	m_pMenuBar->AddMenuItem(dwFileMenu,"Export SSL File"); //7
	m_pMenuBar->DisableMenuItem(dwFileMenu,m_pMenuBar->AddMenuItem(dwFileMenu,"───────"));//12
	m_pMenuBar->AddMenuItem(dwFileMenu,"Exit  Alt+X");//13                                       

	//编辑菜单 1
	DWORD dwEditMenu = m_pMenuBar->AddMenu("Edit");			
	m_pMenuBar->AddMenuItem(dwEditMenu,"Export MiniMap");												// 0
	m_pMenuBar->AddMenuItem(dwEditMenu,"Import MiniMap");												// 1
	m_pMenuBar->AddMenuItem(dwEditMenu,"Clear Empty Build");											// 2
	m_pMenuBar->AddMenuItem(dwEditMenu,"Rebuild Block");												// 3	
	m_pMenuBar->AddMenuItem(dwEditMenu,"Setup Sun Light");												// 4
	m_pMenuBar->DisableMenuItem(dwEditMenu,m_pMenuBar->AddMenuItem(dwEditMenu,"──────────"));	// 5
	m_pMenuBar->AddMenuItem(dwEditMenu,"Import Height Map");												// 6
	m_pMenuBar->AddMenuItem(dwEditMenu,"Export Height Map");												// 7
	m_pMenuBar->DisableMenuItem(dwEditMenu,m_pMenuBar->AddMenuItem(dwEditMenu,"──────────"));	// 8
	m_pMenuBar->AddMenuItem(dwEditMenu,"Import Visible Map");											// 9
	m_pMenuBar->AddMenuItem(dwEditMenu,"Export Visible Map");											// 10
	m_pMenuBar->DisableMenuItem(dwEditMenu,m_pMenuBar->AddMenuItem(dwEditMenu,"──────────"));	// 11
	m_pMenuBar->AddMenuItem(dwEditMenu,"Import Block Map");												// 12
	m_pMenuBar->AddMenuItem(dwEditMenu,"Export Block Map");												// 13
	m_pMenuBar->DisableMenuItem(dwEditMenu,m_pMenuBar->AddMenuItem(dwEditMenu,"──────────"));	// 14
	m_pMenuBar->AddMenuItem(dwEditMenu,"Import Weather Map");											// 15
	m_pMenuBar->DisableMenuItem(dwEditMenu,m_pMenuBar->AddMenuItem(dwEditMenu,"──────────"));	// 16
	m_pMenuBar->AddMenuItem(dwEditMenu,"Import Color Map");												// 17
	m_pMenuBar->AddMenuItem(dwEditMenu,"Export Color Map");												// 18
	m_pMenuBar->DisableMenuItem(dwEditMenu,m_pMenuBar->AddMenuItem(dwEditMenu,"──────────"));	// 19
	m_pMenuBar->AddMenuItem(dwEditMenu,"Import Material Map");											// 20
	m_pMenuBar->AddMenuItem(dwEditMenu,"Import ENV-Effect Map");											//21
	m_pMenuBar->AddMenuItem(dwEditMenu,"Import ENV-Creature Map");										//22
	m_pMenuBar->AddMenuItem(dwEditMenu,"Setup Fog");												//23
	m_pMenuBar->AddMenuItem(dwEditMenu,"Import Camera");													//24
	m_pMenuBar->AddMenuItem(dwEditMenu,"Export Camera");													//25
	m_pMenuBar->AddMenuItem(dwEditMenu,"Setup TerrainSet");													//26
	m_pMenuBar->AddMenuItem(dwEditMenu,"Setup SkyBox");													//27

	//查看菜单 2
	DWORD dwViewMenu = m_pMenuBar->AddMenu("View");
	////子菜单
	m_pMenuBar->AddMenuItem(dwViewMenu,"地形");//0
	m_pMenuBar->AddMenuItem(dwViewMenu,"地物");//1
	m_pMenuBar->AddMenuItem(dwViewMenu,"阻挡");//2
	m_pMenuBar->AddMenuItem(dwViewMenu,"地表");//3
	m_pMenuBar->AddMenuItem(dwViewMenu,"切换点");//4	
	m_pMenuBar->AddMenuItem(dwViewMenu,"城战标志");//5
	m_pMenuBar->AddMenuItem(dwViewMenu,"安全标志");//6	
	m_pMenuBar->AddMenuItem(dwViewMenu,"环境效果");//7
	m_pMenuBar->AddMenuItem(dwViewMenu,"天气效果");//8
	m_pMenuBar->AddMenuItem(dwViewMenu,"环境生物");//9
	m_pMenuBar->DisableMenuItem(dwViewMenu,m_pMenuBar->AddMenuItem(dwViewMenu,"────"));	// 10
	m_pMenuBar->AddMenuItem(dwViewMenu,"网格");//11	
	m_pMenuBar->AddMenuItem(dwViewMenu,"光照");//12
	m_pMenuBar->AddMenuItem(dwViewMenu,"线框");//13
	m_pMenuBar->AddMenuItem(dwViewMenu,"雾");//14		



	// 天气 3
	DWORD dwWeatherMenu = m_pMenuBar->AddMenu("Weather");
	// 子菜单
	m_pMenuBar->DisableMenuItem(dwWeatherMenu,m_pMenuBar->AddMenuItem(dwWeatherMenu,"Edit Fog"));//0


	//关于 4
	DWORD dwHelpMenu = m_pMenuBar->AddMenu("Help");
	m_pMenuBar->AddMenuItem(dwHelpMenu,"About");
}

void EditWnd::DestroyMenuBar(void)
{
	SAFEDESTROY(m_pMenuBar);
}

//void EditWnd::_ShowCursor(void)
//{
//	render::Interface *pInterface = render::Interface::GetInstance();
//	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();
//
//	//鼠标
//	POINT ptCur,ptOld;
//	input::Interface::GetInstance()->GetMousePosition(&ptCur,&ptOld);
//	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
//	DWORD &dwStatus = pInputInfo->dwMouseButtonState;
//
////	tagCursor &tCursor = m_tCursor[m_eCursorStatus];
//	pLayer2D->DrawTexture(ptCur.x - tCursor.dwHotX,ptCur.y - tCursor.dwHotY,tCursor.hTextureHandle,0xFFFFFFFF);
//}

void EditWnd::_ShowInformation(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

	//DRAW FPS
	int iFps = _GetFps();
	static char szTemp[256];

	sprintf(szTemp,"%3.3d TEX:%dK(%d)",iFps,
		//m_pDirectionalLight->GetLightDirection()->x,
		//m_pDirectionalLight->GetLightDirection()->y,
		//m_pDirectionalLight->GetLightDirection()->z)
		pInterface->GetTextureMemorySize() / 1024,pInterface->GetTextureCount());
	int iLen = lstrlen(szTemp);

	int iTextWidth = pLayer2D->CalcTextWidth(szTemp,iLen);
	int iTextHeight= pLayer2D->GetFontSize();

	int iWidth = iTextWidth + 16;	

	int iStartX = pInterface->GetWndWidth() - iWidth  + (iWidth/2 - iTextWidth/2);
	int iStartY = 768  - 16 ;

	ui::Wnd::SetUIRendState();
	pLayer2D->TextOut(iStartX,iStartY,szTemp,lstrlen(szTemp),0xfff0ff90);

}

void EditWnd::_SetTransform(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pInterface->GetDevice()->SetTransform(D3DTS_WORLD,&matWorld);
	pInterface->GetDevice()->SetTransform(D3DTS_VIEW,m_pCamera->GetViewMatrix());
	pInterface->GetDevice()->SetTransform(D3DTS_PROJECTION,m_pCamera->GetProjectionMatrix());
}

BOOL EditWnd::_UpdateCamera(void)
{
	ui::Manager *pUI = ui::Manager::GetInstance();

	BOOL &bActive = m_tWindowInfo.bActive;
	ui::Manager::_tagINPUTINFO *pInputInfo = pUI->GetInputInfoBuffer();

	if (bActive & !(pInputInfo->byKeyBuffer[DIK_SPACE] & 0x80))
	{
		DWORD dwMBS = pInputInfo->dwMouseButtonState;
		//===============camera control======================
		if (dwMBS & MK_MBUTTON)
		{
			m_eCamOpType = CCO_ROTATE;

		}
		else
			if (dwMBS & MK_RBUTTON && !(pInputInfo->byKeyBuffer[DIK_LALT] & 0x80) )
			{
				if (pInputInfo->byKeyBuffer[DIK_LSHIFT] & 0x80)
				{
					m_eCamOpType = CCO_VMOVE;								
				}
				else
				{
					m_eCamOpType = CCO_MOVE;
				}
			}
			else
				m_eCamOpType = CCO_NONE;

		if (pInputInfo->eType == UIMT_MS_MOVE)
		{
			//Camerat Action
			float cx = 0.02f * (float)((pInputInfo->ptMouse.x - pInputInfo->ptMouseOld.x));
			float cy = 0.02f * (float)((pInputInfo->ptMouse.y - pInputInfo->ptMouseOld.y));

			switch(m_eCamOpType)
			{
			case CCO_NONE:
		//		m_eCursorStatus = CURSOR_NORMAL;
				break;
			case CCO_ROTATE:
		//		m_eCursorStatus = CURSOR_ROTATION;
				if (m_bLockCamera)
					m_pCamera->RotatePosition(&D3DXVECTOR3(0,cx,0));    
				else
					m_pCamera->RotatePosition(&D3DXVECTOR3(cy,cx,0));    
				break;
			case CCO_SCALE:
				m_pCamera->MovePosition(&D3DXVECTOR3(0,0,cx));
		//		m_eCursorStatus = CURSOR_SCALE;
				break;
			case CCO_MOVE:
				m_pCamera->FlatMoveFB(cy * 1.5f);
				m_pCamera->FlatMoveLR(-cx);
		//		m_eCursorStatus = CURSOR_MOVE;
				break;
			case CCO_VMOVE:
				m_pCamera->MovePosition(&D3DXVECTOR3(0,cx,0));
				m_pCamera->MoveView(&D3DXVECTOR3(0,cx,0));
		//		m_eCursorStatus = CURSOR_MOVE;
				break;
			}
		}

		if (pInputInfo->eType == UIMT_MS_WHEEL)
		{
			float fzDelta = (float)((short)HIWORD(pInputInfo->dwData));
			m_pCamera->MovePosition(&D3DXVECTOR3(0,0,fzDelta * 0.01f));
		//	m_eCursorStatus = CURSOR_SCALE;
		}

		return (m_eCamOpType != CCO_NONE);
	}

	return FALSE;
}

void EditWnd::_CreateModeMenu(void)
{

	const RECT rcRECT = {0, 0 ,64,24};

	RECT rcModeMenu = { 0,0,16,16};

	m_pModeMenu = new ui::QuickMenu;
	m_pModeMenu->Create("ModeQM",&rcModeMenu,FWS_OUTLINE | FWS_VISIBLE | FWS_FILLBKG | FWS_TOPMOST | FWS_SIZE,this);
	m_pModeMenu->ShowWnd(FALSE);
	m_pModeMenu->SetRadius(148);

	for(DWORD n = 0; n < EST_END; n++)
	{
		//
		m_pModeMenu->AddItem(pszModeName[n],&rcRECT); 
	}


}

void EditWnd::_DestroyModeMenu(void)
{
	SAFEDESTROY(m_pModeMenu);
}

void EditWnd::ModeMenuProc(void)
{
	//handle mouse
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	if (pInputInfo->eType == UIMT_KB_KEYDOWN && pInputInfo->dwData == DIK_SPACE)
	{
		//激活主窗口
		ui::Manager::GetInstance()->ActiveWindow(this);
		//
		m_pModeMenu->MoveWndTo(pInputInfo->ptMouse.x,pInputInfo->ptMouse.y);
		m_pModeMenu->BeginPopUp();
	}
	else
		if (m_pModeMenu->GetStyle() & FWS_VISIBLE)
		{
			if ( (pInputInfo->eType == UIMT_KB_KEYUP && pInputInfo->dwData == DIK_SPACE) ||
				(pInputInfo->eType == UIMT_MS_BTNDOWN ) || pInputInfo->byKeyBuffer[DIK_SPACE] == 0)
			{
				m_pModeMenu->EndPopUp();

				//check status
				DWORD dwItem = 0;
				BOOL bSel = m_pModeMenu->TestSelMenu(dwItem);
				if (bSel)
				{
					//处理
					m_eEditStatus = (EDIT_STATUS_TYPE)dwItem;
					//显示相应的编辑选择框
					_SetupEditUI();   
				}
			}
		}
}

void EditWnd::_ShowStatus(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

	EditMap *pMap = CommDB::GetInstance()->GetMap();
	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();

	//DRAW 编辑状态
	static char szTemp[256];

	D3DXVECTOR3 *pPosition = m_pCamera->GetPosition();
	D3DXVECTOR3 vNormal = *m_pCamera->GetPosition() - *m_pCamera->GetView();

	int iCameraAngle =  (int)D3DXToDegree(asinf(vNormal.y / D3DXVec3Length(&vNormal)));

	DWORD dwVisibleTriangle = pMap->GetVisibleTirangle() /*+ pMesh->GetTriangleCacheUsage()*/;

	sprintf(szTemp,"Mode: %s Cam:%.2d %.2d Vis: %d VD: %2.2f",pszModeName[m_eEditStatus],(int)pPosition->y,iCameraAngle,dwVisibleTriangle ,Setting::GetInstance()->GetViewDistance());
	int iLen = lstrlen(szTemp);

	int iTextWidth = pLayer2D->CalcTextWidth(szTemp,iLen);
	int iTextHeight= pLayer2D->GetFontSize();

	int iWidth = iTextWidth + 16;	

	int iStartX = 4;
	int iStartY = pInterface->GetWndHeight() - iTextHeight - 4;

	pLayer2D->DrawSolidQuad(iStartX - 2,iStartY ,1024 -2,iTextHeight + 2 ,0XAA000000);
	int iTexLen = lstrlen(szTemp);
	if (dwVisibleTriangle > 3000)
	{
		if (dwVisibleTriangle > 5000)
		{
			sprintf(szTemp,"#FFFFAA6CMode: #FFC0F285%s #FFFFAA6CCam:#FFC0F285%.2d %.2d #FFFFAA6CVis: #FFFF623E%d VD: %2.2f",pszModeName[m_eEditStatus],(int)pPosition->y,iCameraAngle,dwVisibleTriangle,Setting::GetInstance()->GetViewDistance());
		}
		else
			sprintf(szTemp,"#FFFFAA6CMode: #FFC0F285%s #FFFFAA6CCam:#FFC0F285%.2d %.2d #FFFFAA6CVis: #FFFFEE61%d VD: %2.2f",pszModeName[m_eEditStatus],(int)pPosition->y,iCameraAngle,dwVisibleTriangle,Setting::GetInstance()->GetViewDistance());
	}
	else
		sprintf(szTemp,"#FFFFAA6CMode: #FFC0F285%s #FFFFAA6CCam:#FFC0F285%.2d %.2d #FFFFAA6CVis: #FFD1FF9A%d VD: %2.2f",pszModeName[m_eEditStatus],(int)pPosition->y,iCameraAngle,dwVisibleTriangle,Setting::GetInstance()->GetViewDistance());

	ui::Wnd::SetUIRendState();
	pLayer2D->TextOutEx(iStartX,iStartY,szTemp,lstrlen(szTemp));

}

void EditWnd::_CreateEditUI(void)
{
	// 编辑界面
	// 水面编辑
//	RECT rcWndWater = { 5, 30, 205, 625 };
//	m_pWndWater = new WndWater();
//	m_pWndWater->Create("WndWater", &rcWndWater, FWS_VISIBLE | FWS_OUTLINE | FWS_SIZE | FWS_DRAG | FWS_FILLBKG, this );

	//材质--使用列表框
	int iW = m_tWindowInfo.rcRect.right  - m_tWindowInfo.rcRect.left;
	int iH = m_tWindowInfo.rcRect.bottom - m_tWindowInfo.rcRect.top;
	RECT rcMtlListBox = { iW - 150,iH - 150,iW - 10,iH - 20};
	m_pMtlListBox = new MtlListBox;
	m_pMtlListBox->Create("MTLLIST",&rcMtlListBox,FWS_VISIBLE | FWS_OUTLINE | FWS_SIZE | FWS_DRAG | FWS_FILLBKG,this);

	//安全区
	RECT rcListBoxSecurity = {925,690,1010,750};
	m_pListBoxSecurity = new ListBoxSecurity;
	m_pListBoxSecurity->Create("SECLIST",&rcListBoxSecurity,FWS_VISIBLE | FWS_OUTLINE | FWS_SIZE | FWS_DRAG | FWS_FILLBKG,this);
	//城战
	RECT rcListBoxWar = { 942,710,1020,750};
	m_pListBoxWar = new ListBoxWar;
	m_pListBoxWar->Create("WARLIST",&rcListBoxWar,FWS_VISIBLE | FWS_OUTLINE | FWS_SIZE | FWS_DRAG | FWS_FILLBKG,this);
	//切换点
	RECT rcWndDoorEdit = { 741,480,1021,765};
	m_pWndDoorEdit = new WndDoorEdit;
	m_pWndDoorEdit->Create("CHANGE",&rcWndDoorEdit,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG,this);

	//地表
	RECT rcTileSetWnd = { 504,704 + 6,1022,6 +704 + 38};
	m_pWndTerrainTileSet = new WndTerrainTileSet;
	m_pWndTerrainTileSet->Create("TileSet",&rcTileSetWnd,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG,this);

	// 地形
	RECT rcTerrainHeightWnd = { 850,716,1023,716 + 40};
	m_pWndTerrainHeightEdit = new WndTerrainHeightEdit;
	m_pWndTerrainHeightEdit->Create("HeightEdit",&rcTerrainHeightWnd,FWS_VISIBLE,this);
	//地物
	RECT rcSelectBuildWnd = { 810,510,1020,760};
	m_pSelectBuildWnd = new WndSelectBuild;
	m_pSelectBuildWnd->Create("SelectBuild",&rcSelectBuildWnd,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG,this);

	//摄像机编辑窗口
	RECT rcWndCameraPath= {741,480,1021,765};
	m_pWndCameraPath = new WndCameraPath;
	m_pWndCameraPath->Create("摄像机路径",&rcWndCameraPath,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW ,this);
	m_pWndCameraPath->ShowWnd(FALSE);

	//地图
	RECT rcMiniMap = { 802,30,1020,265};
	m_pWndMiniMap = new WndMiniMap;
	m_pWndMiniMap->Create("MiniMap",&rcMiniMap,FWS_VISIBLE | FWS_OUTLINE ,this);
	m_pWndMiniMap->ShowWnd(m_bShowMap);

	_SetupEditUI();//设置显示

	#pragma warning( disable : 4312)

	render::Interface *pInterface = render::Interface::GetInstance();

	//m_theBuildTree.Create(pInterface->GetWndHandle(),(HINSTANCE)GetWindowLong(pInterface->GetWndHandle(),GWL_HINSTANCE));
}


void EditWnd::_DestroyEditUI(void)
{
	SAFEDESTROY(m_pWndCameraPath);
	SAFEDESTROY(m_pWndTerrainHeightEdit);
	SAFEDESTROY(m_pWndTerrainTileSet);
	SAFEDESTROY(m_pSelectBuildWnd);
	SAFEDESTROY(m_pWndDoorEdit);
	SAFEDESTROY(m_pWndMiniMap);
	SAFEDESTROY(m_pListBoxWar);
	SAFEDESTROY(m_pListBoxSecurity);
	SAFEDESTROY(m_pMtlListBox);	
//	SAFEDESTROY(m_pWndWater);
}


void EditWnd::_SetupEditUI(void)
{
//	m_pWndWater->ShowWnd(FALSE);
	m_pMtlListBox->ShowWnd(FALSE);
	m_pListBoxSecurity->ShowWnd(FALSE);
	m_pListBoxWar->ShowWnd(FALSE);
	m_pWndDoorEdit->ShowWnd(FALSE);
	m_pWndTerrainTileSet->ShowWnd(FALSE);
	m_pWndTerrainHeightEdit->ShowWnd(FALSE);
	m_pWndCameraPath->ShowWnd(FALSE);
	m_pSelectBuildWnd->ShowWnd(FALSE);

	switch(m_eEditStatus)
	{
	//case EST_WATER:
	//	m_pWndWater->ShowWnd(TRUE);
	//	break;
	case EST_TERRAINTEXTURE:
		m_pWndTerrainTileSet->ShowWnd(TRUE);
		break;
	case EST_TERRAINHEIGHT:
		m_pWndTerrainHeightEdit->ShowWnd(TRUE);
		break;
	case EST_BUILDING:
		m_pSelectBuildWnd->ShowWnd(TRUE);
		break;
	case EST_MATERIAL:
		m_pMtlListBox->ShowWnd(TRUE);
		break;
	case EST_BLOCKFLAG:
		break;
	case EST_SECURITYFLAG:
		m_pListBoxSecurity->ShowWnd(TRUE);
		break;
	case EST_WARFLAG:
		m_pListBoxWar->ShowWnd(TRUE);
		break;
	case EST_SWITCH:
		m_pWndDoorEdit->ShowWnd(TRUE);
		break;
	case EST_CAMERAPATH:
		m_pWndCameraPath->ShowWnd(TRUE);
		break;
	}
}

void EditWnd::_ShowAxis(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D = pInterface->GetLayer3D();
	render::Interface::Layer2D   *pLayer2D = pInterface->GetLayer2D();  

	D3DXMATRIX matViewClip;
	D3DXMatrixIdentity(&matViewClip);
	matViewClip._41 = matViewClip._11  = 130/2;
	matViewClip._42  = matViewClip._22  = 100/2;
	matViewClip._22 = -matViewClip._22;	

	D3DXMATRIX matView,matProj,matFinal,matTemp;
	pInterface->GetDevice()->GetTransform(D3DTS_VIEW,&matView);
	pInterface->GetDevice()->GetTransform(D3DTS_PROJECTION,&matProj);
	D3DXMatrixMultiply(&matFinal,&matView,&matProj);
	D3DXMatrixMultiply(&matFinal,&matFinal,&matViewClip);

	//画坐标draw axis (X , Y , Z)
	FLOAT fLength = 2.0f;
	D3DXVECTOR3 v0(0.0f,0.0f,0.0f),vx(fLength,0.0f,0.0f),vy(0.0f,fLength,0.0f),vz(0.0f,0.0f,fLength);
	D3DXVECTOR3 v[2];
	v[0] = v0; v[1] = vx;
	pLayer3D->_DEBUG_DrawLine(v,D3DCOLOR_ARGB(255,255,0,0));
	D3DXVec3TransformCoord(&v[1],&v[1],&matFinal);
	ui::Wnd::SetUIRendState();
	pLayer2D->TextOut((int)v[1].x,668 + (int)v[1].y - 16,"X",1,0xFFFF0000);

	v[0] = v0; v[1] = vy;
	pLayer3D->_DEBUG_DrawLine(v,D3DCOLOR_ARGB(255,0,255,0));
	D3DXVec3TransformCoord(&v[1],&v[1],&matFinal);
	ui::Wnd::SetUIRendState();
	pLayer2D->TextOut((int)v[1].x,668 + (int)v[1].y - 16,"Y",1,0xFF00FF00);

	v[0] = v0; v[1] = vz;
	pLayer3D->_DEBUG_DrawLine(v,D3DCOLOR_ARGB(255,0,0,255));
	D3DXVec3TransformCoord(&v[1],&v[1],&matFinal);
	ui::Wnd::SetUIRendState();
	pLayer2D->TextOut((int)v[1].x,668 + (int)v[1].y - 16,"Z",1,0xFF0000FF);
}

void EditWnd::_NewMap(int iWidth,int iDepth)
{
	//新地图
	//New
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	SAFEDESTROY(pMap);
	pMap = new EditMap;
	pMap->Create(iWidth,iDepth);
	CommDB::GetInstance()->SetMap(pMap);
	_CenterCamera(iWidth,iDepth);

	// 取得图素集
	DWORD dwSetIndex = m_pNewMapDlg->GetSelectTileSet();
	// 建立地形
	TCHAR szShaderPath[128];
	sprintf(szShaderPath, "terrain\\set%d\\shader", dwSetIndex);

	TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
	SAFEDESTROY(pTerrainMesh);
	pTerrainMesh = new TerrainMesh;
	pTerrainMesh->SetDefaultBackTextureID(m_pNewMapDlg->GetDefaultBackTextureID());
	pTerrainMesh->CreateShader(szShaderPath);
	pTerrainMesh->CreateTerrain(iWidth, iDepth);
	pTerrainMesh->SetTexSetIndex(dwSetIndex);
	CommDB::GetInstance()->SetTerrainMesh(pTerrainMesh);

	// 为图素面板获取贴图句柄
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();

	render::Interface::Layer3D::ShaderGroup *pShaderGroup = pLayer3D->GetShaderGroup(pTerrainMesh->GetShaderHandle());

	DWORD dwTextureCount = pShaderGroup->GetTextureCount(0);

	RECT rc = {0, 0, 64, 64};
	for(DWORD i = 0; i < 16; i++)
	{
		if(i < dwTextureCount)
		{
			DWORD dwTextureHandle = pShaderGroup->GetTextureHandle(0,i);
			m_pWndTerrainTileSet->SetCheckBoxTextureHandle(i, dwTextureHandle, rc);
			m_pWndTerrainTileSet->ShowCheckBox(i);
		}
		else
		{
			m_pWndTerrainTileSet->ShowCheckBox(i, FALSE);
		}
	}

	// 计算小地图
	TerrainMesh::_tagVertex *pVertex;
	TerrainMesh::_tagVertex *pVertexList = pTerrainMesh->GetVertexList();
	DWORD dwCount = pTerrainMesh->GetVertexNum();
	for(DWORD i = 0; i < dwCount; i++)
	{
		pVertex = &pVertexList[i];
		DWORD dwNumGrid = pVertex->dwBelongToNum;
		DWORD dwColor = m_pWndTerrainTileSet->GetBrushColor(pVertex->byTexID);
		for(DWORD j = 0; j < dwNumGrid; j++)
		{
			DWORD dwGridIndex = pVertex->dwBelongToGrid[j];
			//if(dwGridIndex < 0 || dwGridIndex > 576)
			//	ASSERT(0);

			m_pWndMiniMap->SetMapPixel(dwGridIndex, dwColor);
		}
	}
	m_pWndMiniMap->UpdateMiniPixel();
	m_pWndMiniMap->CalculateMiniMapPosition();

	//清空Histroy
	CmdMgr::GetInstance()->Clear();
	//复位工具
	m_pToolBuildEdit->Reset();

	//复位切换点编辑窗
	m_pWndDoorEdit->ClearAllDoor();

	//清空文件第一次保存标志
	m_bHasEditFileName = FALSE;
	strcpy(m_szCurrentEditFile,"");


}

void EditWnd::_CenterCamera(int iWidth,int iDepth)
{
	float fW = (float)iWidth,fD = (float)iDepth;
	m_pCamera->SetPosition(&D3DXVECTOR3(fW/2.0f,10.0f,-5.0f + fD/2.0f));
	m_pCamera->SetView(&D3DXVECTOR3(fW/2.0f,0.0f,0.0f + fD/2.0f));
}

void EditWnd::_CheckHotKey(void)
{
	//handle mouse
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	//z UnDo
	if (pInputInfo->eType == UIMT_KB_KEYDOWN)
	{
		switch(pInputInfo->dwData)
		{
		case DIK_W:
			//Water::CManager::GetInstance()->s_bIsRenderWater = !Water::CManager::GetInstance()->s_bIsRenderWater;
			break;
		case DIK_Z:
			if (pInputInfo->byKeyBuffer[DIK_LSHIFT] & 0x80)
			{
				CmdMgr::GetInstance()->ReDo();
			}
			else
				CmdMgr::GetInstance()->UnDo();
			break;
		case DIK_P:
			if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
			{
				CmdMgr::GetInstance()->Dump();
			}
			break;
		case DIK_TAB:
			{
				m_bShowMap = !m_bShowMap;
				m_pWndMiniMap->ShowWnd(m_bShowMap);
			}
			break;
		case DIK_S:
			if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
			{
				OnSaveFile();
			}
			break;
		case DIK_G:
			{
				if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
				{
					if (m_dwRenderFlag & GRIDMASK)
					{
						m_dwRenderFlag &= ~GRIDMASK;
					}
					else
						m_dwRenderFlag |= GRIDMASK;
				}
			}
			break;
		case DIK_B:
			{
				if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
				{
					if (m_dwRenderFlag & BUILDINGMASK)
					{
						m_dwRenderFlag &= ~BUILDINGMASK;
					}
					else
						m_dwRenderFlag |= BUILDINGMASK;			
				}
			}
			break;
		case DIK_L:
			{
				if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
				{
					if (m_dwRenderFlag & LIGHTMASK)
					{
						m_dwRenderFlag &= ~LIGHTMASK;
					}
					else
						m_dwRenderFlag |= LIGHTMASK;
				}
			}
			break;
		case DIK_X:
			{
				m_bShowTexture = !m_bShowTexture;
			}
			break;
		case DIK_F:
			{
				if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
				{
					m_bShowFog = !m_bShowFog;
				}
			}
			break;
		case DIK_K:
			if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
			{
				m_bShowSkybox = !m_bShowSkybox;
			}
			break;
		case DIK_T:
			{
				if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
				{
					if (m_dwRenderFlag & TERRAINMASK)
					{
						m_dwRenderFlag &= ~TERRAINMASK;
					}
					else
						m_dwRenderFlag |= TERRAINMASK;			
				}
			}
			break;
		case DIK_E:
			{
				if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
				{
					if (!m_pWndExport->CheckPopUp())
						m_pWndExport->BeginPopUp();
				}
			}
			break;
		case DIK_R://摄像机锁定
			{
				if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
				{
					m_bLockCamera = !m_bLockCamera;
				}
			}
			break;
		case DIK_J://调整场景光照
			{
				if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
				{
					m_pWndLightAdjust->BeginPopUp();
				}
			}
			break;
		case DIK_F9:
			{
				_RenderReference();
			}
			break;
		case DIK_EQUALS://调整场景视野
			{
				float fVD = Setting::GetInstance()->GetViewDistance();
				fVD +=5.0f;
				if (fVD > 200.0f)
					fVD = 200.0f;
				Setting::GetInstance()->SetViewDistance(fVD);
				m_pCamera->SetFarClipPlane(Setting::GetInstance()->GetViewDistance());

			}
			break;
		case DIK_MINUS://调整场景视野
			{
				float fVD = Setting::GetInstance()->GetViewDistance();
				fVD -=5.0f;
				if (fVD < 15.0f)
					fVD = 15.0f;
				Setting::GetInstance()->SetViewDistance(fVD);
				m_pCamera->SetFarClipPlane(Setting::GetInstance()->GetViewDistance());
			}
			break;
		case DIK_O://打开文件
			{
				if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
				{
					OnOpenFile();
				}
			}
			break;
		case DIK_H:
			{
				if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80 && pInputInfo->byKeyBuffer[DIK_LALT] & 0x80)
				{
					m_pWndRandomReplace->BeginPopUp();
				}
			}
			break;
		case DIK_F5:
			{
				//刷新当前选中Build模型
				EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
				BuildTree * pBuildTree = pEditWnd->GetBuildTree();
				WORD wCurBuildID = (WORD)pBuildTree->GetModelIndex();
				ToolBuildEdit *  pToolBuildEdit = pEditWnd->GetToolBuildEdit();
				GameBuildList *  pGameBuildList = GameBuildList::GetInstance();  	
				pGameBuildList->ReleaseBuild(wCurBuildID, pGameBuildList->AcquireBuild(wCurBuildID,false));

				//pGameBuildList->ClearBuildInfo();
				//pGameBuildList->ReadBuildInfo();

			}
			break;
		case DIK_F6:
			{
				//清空所有模型
				EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
				BuildTree * pBuildTree = pEditWnd->GetBuildTree();
				WORD wCurBuildID = (WORD)pBuildTree->GetModelIndex();
				ToolBuildEdit *  pToolBuildEdit = pEditWnd->GetToolBuildEdit();
				GameBuildList *  pGameBuildList = GameBuildList::GetInstance();  	
				//pGameBuildList->Clear();
				//pGameBuildList->ClearBuildInfo();
				//pGameBuildList->ReadBuildInfo();
				//都要重新Loadbuild文件
				pBuildTree->Refresh();
			}
			break;
		case DIK_F1:
			{
				m_pAboutWnd->Popup();
			}
			break;
		case DIK_N:
			if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
			{
				m_bShowNormal = !m_bShowNormal;
			}
			break;
		case DIK_D:
			if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
			{
				m_bEditSunLightDirection = !m_bEditSunLightDirection;
			}
			break;
		case DIK_NUMPADSLASH:
			{
				float fFovY = m_pCamera->GetFOVY();
				float fRatio = D3DXToRadian(1);
				if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
				{
					fRatio = D3DXToRadian(10);
				}
				if (pInputInfo->byKeyBuffer[DIK_LSHIFT] & 0x80)
				{
					fRatio = D3DXToRadian(30);
				}
				fFovY -= fRatio;
				m_pCamera->SetFOVY(fFovY);
			}
			break;
		case DIK_NUMPADSTAR:
			{
				float fFovY = m_pCamera->GetFOVY();
				float fRatio = D3DXToRadian(1);
				if (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80)
				{
					fRatio = D3DXToRadian(10);
				}
				if (pInputInfo->byKeyBuffer[DIK_LSHIFT] & 0x80)
				{
					fRatio = D3DXToRadian(30);
				}
				fFovY += fRatio;
				m_pCamera->SetFOVY(fFovY);
			}
			break;
		}
	}
	if (pInputInfo->byKeyBuffer[DIK_HOME] & 0x80)
	{
		m_pCamera->RotatePosition(&D3DXVECTOR3(0.01f,0.0f,0));    
	}
	if (pInputInfo->byKeyBuffer[DIK_PGUP] & 0x80)
	{
		m_pCamera->RotatePosition(&D3DXVECTOR3(-0.01f,0,0));    
	}
	if (pInputInfo->byKeyBuffer[DIK_PGDN] & 0x80)
	{
		m_pCamera->RotatePosition(&D3DXVECTOR3(0,0.01f,0));    
	}
	if (pInputInfo->byKeyBuffer[DIK_END] & 0x80)
	{
		m_pCamera->RotatePosition(&D3DXVECTOR3(0,-0.01f,0));    
	}
	if (pInputInfo->byKeyBuffer[DIK_LEFT] & 0x80)
	{
		m_pCamera->FlatMoveLR(-0.08f);
	}
	if (pInputInfo->byKeyBuffer[DIK_RIGHT] & 0x80)
	{
		m_pCamera->FlatMoveLR(0.08f);
	}
	if (pInputInfo->byKeyBuffer[DIK_UP]  & 0x80)
	{
		m_pCamera->FlatMoveFB(0.08f * 1.5f);
	}
	if (pInputInfo->byKeyBuffer[DIK_DOWN] & 0x80)
	{
		m_pCamera->FlatMoveFB(-0.08f * 1.5f);
	}
}

void EditWnd::_ExportRgnFile()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	OPENFILENAME ofn;       
	static char szFile[_MAX_PATH];

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = pInterface->GetWndHandle();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "EditMap Editor File (*.map)\0*.map\0All Files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_FILEMUSTEXIST;

	char szOldDirectory[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH,szOldDirectory);

	// Display the Open dialog box. 
	if (GetSaveFileName(&ofn))
	{
		SetCurrentDirectory(szOldDirectory);

		strlwr(szFile);
		if (strstr(szFile,".rgn") == 0 )
		{
			strcat(szFile,".rgn");
		}

		FILE* file;
		file = fopen(szFile, "wb");
		if( file == NULL )
		{
			OutputConsole("Event: Save Region File:文件打开失败.\n");
			ASSERT(0);
		}

		fwrite(strRegionHead, strlen(strRegionHead), 1, file);
		fwrite(&lRegionVersion, sizeof(long), 1, file);

		long lRegionType(0);
		fwrite(&lRegionType, sizeof(long), 1, file);

		EditMap *pMap = CommDB::GetInstance()->GetMap();
		long lWidth  = pMap->GetDepth();
		long lHeight = pMap->GetWidth();

		fwrite(&lWidth, sizeof(long), 1, file);
		fwrite(&lHeight, sizeof(long), 1, file);
		//此处的2d x = 3d z
		//此处的2d y = 3d x
		tagCell cell;
		for(int y = 0; y < lHeight; y++)
			for(int x = 0; x < lWidth; x++)
			{
				Grid *pGrid = pMap->GetGrid(y,x);

				cell.bBlock    = pGrid->GetBlock();
				cell.lMaterial = pGrid->GetMtl();
				cell.bSecurity = pGrid->GetSecurity();//
				cell.lWar	   = pGrid->GetWar();//
				cell.lSwitch   = (WORD)pGrid->GetDoorIndex();//

				fwrite(&cell, sizeof(tagCell), 1, file);
			}

			//切换点
			long lSwitchNum = (long)m_pWndDoorEdit->GetDoorCount();
			fwrite(&lSwitchNum, sizeof(long), 1, file);
			for(DWORD n = 0; n < (DWORD)lSwitchNum; n++)
			{
				tagDoor *pDoor = m_pWndDoorEdit->GetDoor(n);
				fwrite(&pDoor->tSwitch, sizeof(tagDoor::tagSwitch), 1, file);
			}

			fclose(file);
	}
}

void EditWnd::_ExportSoundSourceListFile()
{
	//render::Interface *pInterface = render::Interface::GetInstance();
	//ProgressBar pb;

	//OPENFILENAME ofn;       
	//static char szFile[_MAX_PATH];

	//// Initialize OPENFILENAME
	//ZeroMemory(&ofn, sizeof(OPENFILENAME));
	//ofn.lStructSize = sizeof(OPENFILENAME);
	//ofn.hwndOwner = pInterface->GetWndHandle();
	//ofn.lpstrFile = szFile;
	//ofn.nMaxFile = sizeof(szFile);
	//ofn.lpstrFilter = "EditMap Sound Source List File (*.mdf)\0*.SSL\0All Files\0*.*\0";
	//ofn.nFilterIndex = 1;
	//ofn.lpstrFileTitle = NULL;
	//ofn.nMaxFileTitle = 0;
	//ofn.lpstrInitialDir = NULL;
	//ofn.Flags = OFN_FILEMUSTEXIST;

	//char szOldDirectory[_MAX_PATH];
	//GetCurrentDirectory(_MAX_PATH,szOldDirectory);

	//// Display the Open dialog box. 
	//if (GetSaveFileName(&ofn))
	//{
	//	SetCurrentDirectory(szOldDirectory);

	//	strlwr(szFile);
	//	if (strstr(szFile,".ssl") == 0 )
	//	{
	//		strcat(szFile,".ssl");
	//	}

	//	pb.Begin(pInterface->GetWndHandle(),0xa00000,5);

	//	FILE* file;
	//	file = fopen(szFile, "wb");
	//	if( file == NULL )
	//	{
	//		OutputConsole("error: Save Sound Source List File:文件打开失败.\n");
	//		ASSERT(0);
	//	}
	//	pb.StepIt();
	//	DWORD dwHead = MAKEFOURCC('S','S','L',' ');//Sound Source List
	//	DWORD dwVersion = MAKEFOURCC('1','.','0','0');
	//	fwrite(&dwHead,sizeof(DWORD), 1, file); 
	//	fwrite(&dwVersion,sizeof(DWORD), 1, file);

	//	EditMap *pMap = CommDB::GetInstance()->GetMap();
	//	DWORD dwMapWidth  = (DWORD)pMap->GetWidth();
	//	DWORD dwMapDepth  = (DWORD)pMap->GetDepth();

	//	EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
	//	ToolBuildEdit *  pToolBuildEdit = pEditWnd->GetToolBuildEdit();
	//	GameBuildList *  pGameBuildList = GameBuildList::GetInstance();  

	//	DWORD dwSoundSourceCount = 0;
	//	for(DWORD n = 0 ;  n < dwMapWidth * dwMapDepth; n++)
	//	{
	//		Grid *pMapGrid = pMap->GetGrid(n);
	//		if (pMapGrid->GetBuild()->wID != 0xFFFF)
	//		{
	//			//建物
	//			WORD wBuildID = pMapGrid->GetBuild()->wID;
	//			GameBuildList::Build *pBuild = pGameBuildList->GetBuild(wBuildID);
	//			if (pBuild->GetSoundSource())
	//				dwSoundSourceCount++;
	//		}
	//	}

	//	fwrite(&dwSoundSourceCount,sizeof(DWORD),1,file);
	//	for(n = 0; n < dwMapWidth * dwMapDepth; n++)
	//	{
	//		Grid *pMapGrid = pMap->GetGrid(n);
	//		if (pMapGrid->GetBuild()->wID != 0xFFFF)
	//		{
	//			//建物
	//			WORD wBuildID = pMapGrid->GetBuild()->wID;
	//			GameBuildList::Build *pBuild = pGameBuildList->GetBuild(wBuildID);
	//			if (pBuild->GetSoundSource())
	//			{
	//				//x z position
	//				float x = (float)(n % dwMapWidth);
	//				float z = (float)(n / dwMapWidth);
	//				fwrite(&x,sizeof(float),1,file);
	//				fwrite(&z,sizeof(float),1,file);
	//				//sound source struct
	//				fwrite(pBuild->GetSoundSource(),sizeof(GameBuildList::Build::tagSoundSource),1,file);
	//			}
	//		}
	//	}
	//	fclose(file);
	//}
}

void EditWnd::_CreateEditTool()
{
	m_pToolTerrainHeight = new ToolTerrainHeight;
	m_pToolTerrainHeight->Create();

	// 地表编辑工具
	m_pToolSetTile = new ToolSetTile;
	m_pToolSetTile->Create();

	//Material编辑处理器
	m_pToolMtlEdit = new ToolMtlEdit;
	m_pToolMtlEdit->Create();

	m_pToolDoorEdit = new ToolDoorEdit;
	m_pToolDoorEdit->Create();

	//阻挡编辑器
	m_pToolBlockEdit = new ToolBlockEdit;
	m_pToolBlockEdit->Create();
	//安全区设定器
	m_pToolSecurityEdit = new ToolSecurityEdit;
	m_pToolSecurityEdit->Create();

	//城战
	m_pToolWarEdit = new ToolWarEdit;
	m_pToolWarEdit->Create();

	//环境效果标志
	m_pToolEnvEffectFlagEdit = new ToolEnvEffectFlagEdit;
	m_pToolEnvEffectFlagEdit->Create();

	//环境效果标志
	m_pToolEnvCreatureFlagEdit = new ToolEnvCreatureFlagEdit;
	m_pToolEnvCreatureFlagEdit->Create();

	//建筑
	m_pToolBuildEdit = new ToolBuildEdit;
	m_pToolBuildEdit->Create();

	//网格可见编辑
	m_pToolGridVisibleEdit = new ToolGridVisibleEdit;
	m_pToolGridVisibleEdit->Create();

	// 颜色涂抹工具
	render::Interface *pInterface = render::Interface::GetInstance();
	DWORD dwWidth = pInterface->GetWndWidth();
	DWORD dwHeight = pInterface->GetWndHeight();
	RECT rcColorDialog = {dwWidth - 128, dwHeight - 64 + 16 - 128, dwWidth, dwHeight - 64 + 16};
	RECT rcForeColorQuad = {dwWidth - 64 + 16, dwHeight - 64 + 16, dwWidth - 32 + 16, dwHeight - 32 + 16};
	RECT rcBackColorQuad = {dwWidth - 32, dwHeight - 32, dwWidth, dwHeight};
	m_pToolPaintColor = new ToolPaintColor;
	m_pToolPaintColor->Create("ui\\color.tga", rcColorDialog, rcForeColorQuad, rcBackColorQuad);

	//天气效果工具
	m_pToolWeatherEffectFlagEdit = new ToolWeatherEffectFlagEdit;
	m_pToolWeatherEffectFlagEdit->Create();

}

void EditWnd::_DestroyEditTool()
{
	SAFEDESTROY(m_pToolWeatherEffectFlagEdit);
	SAFEDESTROY(m_pToolGridVisibleEdit);
	SAFEDESTROY(m_pToolTerrainHeight);
	SAFEDESTROY(m_pToolSecurityEdit);
	SAFEDESTROY(m_pToolWarEdit);
	SAFEDESTROY(m_pToolEnvEffectFlagEdit);
	SAFEDESTROY(m_pToolEnvCreatureFlagEdit);
	SAFEDESTROY(m_pToolBlockEdit);
	SAFEDESTROY(m_pToolMtlEdit);
	SAFEDESTROY(m_pToolDoorEdit);
	SAFEDESTROY(m_pToolSetTile);
	SAFEDESTROY(m_pToolBuildEdit);
	SAFEDESTROY(m_pToolPaintColor);
}

void EditWnd::OnSaveFile(void)
{
	if (m_bHasEditFileName)
	{
		SaveMap(m_szCurrentEditFile);
	}
	else
	{
		//得到文件名再保存
		OnSaveFileAs();
	}
}
void EditWnd::OnSaveFileAs(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
	if (pMap)
	{
		OPENFILENAME ofn;       
		static char szFile[_MAX_PATH];

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = pInterface->GetWndHandle();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "EditMap Editor File (*.map)\0*.map\0All Files\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_FILEMUSTEXIST;

		char szOldDirectory[_MAX_PATH];
		GetCurrentDirectory(_MAX_PATH,szOldDirectory);

		// Display the Open dialog box. 
		if (GetSaveFileName(&ofn))
		{
			strlwr(szFile);
			if (strstr(szFile,".map") == 0 )
			{
				strcat(szFile,".map");
			}
			_SetEditFileName(szFile);
			SaveMap(szFile);
		}
		SetCurrentDirectory(szOldDirectory);
	}
}

void EditWnd::OnOpenFile(void)
{  
	render::Interface *pInterface = render::Interface::GetInstance();
	OPENFILENAME ofn;       
	static char szFile[_MAX_PATH];
	bool bnew = false;

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = pInterface->GetWndHandle();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "EditMap Editor File (*.map)\0*.map\0All Files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_FILEMUSTEXIST;

	char szOldDirectory[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH,szOldDirectory);

	// Display the Open dialog box. 
	if (!GetOpenFileName(&ofn))
		return ;

	SetCurrentDirectory(szOldDirectory);

	//保存项目文件
	FILE *fp = fopen(szFile,"rb");
	if (!fp)
	{
		MessageBox(NULL,"文件打开失败!","错误",MB_OK);
		fclose(fp);
		return ;
	}
	
	//设置文件名存在
	_SetEditFileName(szFile);
	
	//文件头
	char szFlag[2] = "";
	fread(szFlag,2,1,fp);
	if (szFlag[0] != 'M' || szFlag[1] != 'F')
	{
		MessageBox(NULL,"文件类型不对","错误",MB_OK);
		fclose(fp);
		return ;
	}

	//版本号
	DWORD dwVersion;
	fread(&dwVersion,sizeof(DWORD),1,fp);
	if (dwVersion !=  CommDB::VERSION)
	{
		if (dwVersion == 0x00010006)		//1.5版
			bnew = true;
		else
		{
			MessageBox(NULL,"文件版本不兼容","错误",MB_OK);
			fclose(fp);
			return ;
		}
	}

	//地图使用的图片集合
	DWORD dwTileSet;
	fread(&dwTileSet,sizeof(DWORD),1,fp);

	//宽度，高度
	DWORD dwWidth,dwDepth; 
	fread(&dwWidth,sizeof(DWORD),1,fp);			
	fread(&dwDepth,sizeof(DWORD),1,fp);
	

	//NEW 
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	SAFEDESTROY(pMap);

	pMap = new EditMap;
	//读取地图
	pMap->Create(dwWidth,dwDepth);
	pMap->Load(fp);
	CommDB::GetInstance()->SetMap(pMap);
	_CenterCamera(dwWidth,dwDepth);

	// 建立地形
	TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
	SAFEDESTROY(pTerrainMesh);
	pTerrainMesh = new TerrainMesh;
	VERIFY(pTerrainMesh->LoadTerrainMesh(fp,bnew));
	
	CommDB::GetInstance()->SetTerrainMesh(pTerrainMesh);

	fclose(fp);
	

	// 为图素面板获取贴图句柄
	render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();

	render::Interface::Layer3D::ShaderGroup *pShaderGroup = pLayer3D->GetShaderGroup(pTerrainMesh->GetShaderHandle());

	DWORD dwTextureCount = pShaderGroup->GetTextureCount(0);

	RECT rc = {0, 0, 64, 64};
	for(DWORD i = 0; i < 16; i++)
	{
		if(i < dwTextureCount)
		{
			DWORD dwTextureHandle = pShaderGroup->GetTextureHandle(0,i);
			m_pWndTerrainTileSet->SetCheckBoxTextureHandle(i, dwTextureHandle, rc);
			m_pWndTerrainTileSet->ShowCheckBox(i);
		}
		else
		{
			m_pWndTerrainTileSet->ShowCheckBox(i, FALSE);
		}
	}

	TerrainMesh::_tagVertex *pVertex;
	TerrainMesh::_tagVertex *pVertexList = pTerrainMesh->GetVertexList();
	DWORD dwCount = pTerrainMesh->GetVertexNum();
	for(DWORD i = 0; i < dwCount; i++)
	{
		pVertex = &pVertexList[i];
		DWORD dwNumGrid = pVertex->dwBelongToNum;
		DWORD dwColor = m_pWndTerrainTileSet->GetBrushColor(pVertex->byTexID);
		for(DWORD j = 0; j < dwNumGrid; j++)
		{
			DWORD dwGridIndex = pVertex->dwBelongToGrid[j];
			m_pWndMiniMap->SetMapPixel(dwGridIndex, dwColor);
		}
	}
	m_pWndMiniMap->UpdateMiniPixel();
	m_pWndMiniMap->CalculateMiniMapPosition();//计算地图比例

	//清空Histroy
	CmdMgr::GetInstance()->Clear();
	


}


void EditWnd::_SetEditFileName(const char * pszFileName)
{
	m_bHasEditFileName = TRUE;
	strcpy(m_szCurrentEditFile,pszFileName);
	static char szNameTemp[1024];
	render::Interface *pInterface = render::Interface::GetInstance();
	::GetWindowText(pInterface->GetWndHandle(),szNameTemp,1024);
	_snprintf(szNameTemp,1024,"%s %d -%s",MAPEDITORNAME,__COUNTER__,utility::File::GetFileName(m_szCurrentEditFile));
	::SetWindowText(pInterface->GetWndHandle(),szNameTemp);
}

void EditWnd::_DoAutoSave(void)
{
	DWORD dwTime = timeGetTime();
	if (m_bHasEditFileName)
	{
		if (dwTime - m_dwAutoSaveTimer > 5 * 60 * 1000)
		{
			m_dwAutoSaveTimer = dwTime;
			char szBackName[1024];
			_snprintf(szBackName,1024,"%s.auto",m_szCurrentEditFile);
			SaveMap(szBackName);
			OutputConsole("DoAutoSave::SaveMap\n");
		}
	}
}

void EditWnd::SaveMap(const char * pszFileName)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	EditMap *pMap = CommDB::GetInstance()->GetMap();

	TerrainMesh *pMesh = CommDB::GetInstance()->GetTerrainMesh();
	//直接保存
	FILE *fp = fopen(pszFileName,"wb");
	if (fp)
	{
		//文件头
		fwrite("MF",2,1,fp);

		//版本号
		DWORD dwVersion = CommDB::VERSION;
		fwrite(&dwVersion,sizeof(DWORD),1,fp);
		//但前地表图素集合
		DWORD dwTileSet = pMesh->GetTexSetIndex();
		fwrite(&dwTileSet,sizeof(DWORD),1,fp);
		//宽度，高度
		DWORD dwWidth = pMap->GetWidth();
		DWORD dwDepth = pMap->GetDepth();
		fwrite(&dwWidth,sizeof(DWORD),1,fp);			
		fwrite(&dwDepth,sizeof(DWORD),1,fp);			
		//其他其他
		pMap->Save(fp);
		pMesh->SaveTerrainMesh(fp);

		fclose(fp);
	}
	else
	{
		MessageBox(NULL,"写入文件错误","错误",MB_OK);
	}
}

void EditWnd::SaveMiniMap(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	OPENFILENAME ofn;       
	static char szFile[_MAX_PATH];

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = pInterface->GetWndHandle();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "TGA File (*.tga)\0*.tga\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_FILEMUSTEXIST;

	char szOldDirectory[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH,szOldDirectory);

	// Display the Open dialog box. 
	if (!GetSaveFileName(&ofn))
		return ;

	SetCurrentDirectory(szOldDirectory);

	sprintf(szFile, "%s.tga", ofn.lpstrFile);

	FILE *fp = fopen(szFile, "wb");
	if (!fp)
	{
		MessageBox(NULL,"RenderMiniMap: 文件打开失败!", "错误",MB_OK);
		fclose(fp);
		return ;
	}

	pInterface->GetDevice()->EndScene();

	IDirect3DSurface9 *pRenderTarget, *pDepthStencilSurface;
	HRESULT hResult;

	TerrainMesh* pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
	EditMap *pMap = CommDB::GetInstance()->GetMap();

	DWORD dwWidth, dwDepth;

	dwWidth  = pMap->GetWidth();
	dwDepth  = pMap->GetDepth();

	//save old render target
	IDirect3DSurface9* 	pOldRenderTarget;
	IDirect3DSurface9* 	pOldZStencil;

	hResult = pInterface->GetDevice()->GetRenderTarget(0, &pOldRenderTarget);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}
	hResult = pInterface->GetDevice()->GetDepthStencilSurface(&pOldZStencil);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	dwWidth = dwWidth * 4;
	dwDepth = dwDepth * 4;

	hResult = pInterface->GetDevice()->CreateRenderTarget( dwWidth, 
		dwDepth, 
		D3DFMT_X8R8G8B8, 
		D3DMULTISAMPLE_NONE, 
		0, 
		TRUE, 
		&pRenderTarget, 
		NULL);
	if ( FAILED(hResult))
	{
		ASSERT(0 && "创建小地图贴图失败");
	}

	hResult = pInterface->GetDevice()->CreateDepthStencilSurface( dwWidth,
		dwDepth, 
		D3DFMT_D16, 
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&pDepthStencilSurface,
		NULL);

	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	hResult = pInterface->GetDevice()->SetRenderTarget(0, pRenderTarget);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	hResult = pInterface->GetDevice()->SetDepthStencilSurface(pDepthStencilSurface);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	pInterface->GetDevice()->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixScaling(&matWorld,4.0f,4.0f,4.0f);

	D3DXVECTOR3 vEyePt( dwWidth * 0.5f, 400.0f, -5.0f + dwDepth * 0.5f );
	D3DXVECTOR3 vLookatPt( dwWidth * 0.5f, 0.0f, dwDepth * 0.5f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIX  matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );

	D3DXMATRIX  matProj;
	D3DXMatrixOrthoLH( &matProj, (FLOAT)dwWidth, (FLOAT)dwDepth , 0.1f, 10000.0f );

	pInterface->GetDevice()->SetTransform( D3DTS_WORLD, &matWorld);
	pInterface->GetDevice()->SetTransform( D3DTS_VIEW, &matView );
	pInterface->GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );


	pMap->CalculateAllBaseLighting();
	pMap->CalculateAllLightSourceLighting();

	BOOL bFogEnable;
	pInterface->GetDevice()->GetRenderState(D3DRS_FOGENABLE,(DWORD*)&bFogEnable);
	pInterface->GetDevice()->SetRenderState(D3DRS_FOGENABLE,FALSE);	

	pTerrainMesh->RenderAll();
	pMap->DrawAll();
		
	pInterface->GetDevice()->SetRenderState(D3DRS_FOGENABLE,bFogEnable);

	//restore render
	hResult = pInterface->GetDevice()->SetRenderTarget(0, pOldRenderTarget);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	hResult = pInterface->GetDevice()->SetDepthStencilSurface(pOldZStencil);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	D3DSURFACE_DESC dsds;
	hResult = pRenderTarget->GetDesc(&dsds);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	HDC dc;
	hResult = pRenderTarget->GetDC(&dc);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	UINT uBits = 24;
	UINT uWidth = dsds.Width;
	UINT uHeight = dsds.Height;

	UINT uBytesPerPixel = uBits / 8;
	UINT uImageSize = uWidth * uHeight * uBytesPerPixel;

	BYTE *pixels = new BYTE[uImageSize];
	//static BYTE pixels[786432];		// 地图上限为 512 x 512，小地图用24位。
	ZeroMemory(pixels, sizeof(BYTE)*uImageSize);

	for(UINT height = uHeight - 1; height > 0; height--)
	{
		for(UINT width = 0; width < uWidth; width++)
		{
			DWORD color = GetPixel(dc, width, height);
			DWORD count = ((uHeight - 1 - height) * uWidth + width) * 3;
			pixels[count + 0] =  GetRValue(color); 
			pixels[count + 1] =  GetGValue(color);
			pixels[count + 2] =  GetBValue(color);
		}
	}

	WriteTGA(uBits, uWidth, uHeight, pixels, fp);

	SAFEDELETE(pixels);

	pRenderTarget->ReleaseDC(dc);

	fclose(fp);

	SAFERELEASE(pOldRenderTarget);
	SAFERELEASE(pOldZStencil);

	SAFERELEASE(pRenderTarget);
	SAFERELEASE(pDepthStencilSurface);

	pInterface->GetDevice()->BeginScene();
}

void EditWnd::_RenderReference()
{
	render::Interface *pInterface = render::Interface::GetInstance();

	pInterface->GetDevice()->EndScene();

	IDirect3DSurface9 *pRenderTarget, *pDepthStencilSurface;
	HRESULT hResult;

	TerrainMesh* pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
	EditMap *pMap = CommDB::GetInstance()->GetMap();

	DWORD dwWidth, dwDepth;

	dwWidth  = pMap->GetWidth();
	dwDepth  = pMap->GetDepth();

	//save old render target
	IDirect3DSurface9* 	pOldRenderTarget;
	IDirect3DSurface9* 	pOldZStencil;

	hResult = pInterface->GetDevice()->GetRenderTarget(0, &pOldRenderTarget);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}
	hResult = pInterface->GetDevice()->GetDepthStencilSurface(&pOldZStencil);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	hResult = pInterface->GetDevice()->CreateRenderTarget( dwWidth, 
		dwDepth, 
		D3DFMT_X8R8G8B8, 
		D3DMULTISAMPLE_NONE, 
		0, 
		TRUE, 
		&pRenderTarget, 
		NULL);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	hResult = pInterface->GetDevice()->CreateDepthStencilSurface( dwWidth,
		dwDepth, 
		D3DFMT_D16, 
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&pDepthStencilSurface,
		NULL);

	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	hResult = pInterface->GetDevice()->SetRenderTarget(0, pRenderTarget);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	hResult = pInterface->GetDevice()->SetDepthStencilSurface(pDepthStencilSurface);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	pInterface->GetDevice()->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	D3DXVECTOR3 vEyePt( dwWidth * 0.5f, 400.0f, -5.0f + dwDepth * 0.5f );
	D3DXVECTOR3 vLookatPt( dwWidth * 0.5f, 0.0f, dwDepth * 0.5f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );

	D3DXMATRIXA16 matProj;
	D3DXMatrixOrthoLH( &matProj, (FLOAT)dwWidth, (FLOAT)dwDepth, 0.1f, 10000.0f );

	pInterface->GetDevice()->SetTransform( D3DTS_WORLD, &matWorld);
	pInterface->GetDevice()->SetTransform( D3DTS_VIEW, &matView );
	pInterface->GetDevice()->SetTransform( D3DTS_PROJECTION, &matProj );

	pMap->CalculateAllBaseLighting();
	pMap->CalculateAllLightSourceLighting();

	pTerrainMesh->RenderAll();

	pMap->DrawAll();

	//restore render
	hResult = pInterface->GetDevice()->SetRenderTarget(0, pOldRenderTarget);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	hResult = pInterface->GetDevice()->SetDepthStencilSurface(pOldZStencil);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	D3DSURFACE_DESC dsds;
	hResult = pRenderTarget->GetDesc(&dsds);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	HDC dc;
	hResult = pRenderTarget->GetDC(&dc);
	if ( FAILED(hResult))
	{
		ASSERT(0);
	}

	render::Texture *pTexture = pInterface->GetTexture(m_pWndMiniMap->NewReference());
	LPDIRECT3DTEXTURE9 pd3dTexture = pTexture->GetD3DTexture(); 

	D3DLOCKED_RECT lcdst;
	pd3dTexture->LockRect(0,&lcdst,NULL,D3DLOCK_DISCARD);

	//拷贝数据
	float fRatioX = (float)dsds.Width  / 256.0f;
	float fRatioY = (float)dsds.Height / 256.0f;

	render::Interface::_tagSetting *pSetting = pInterface->GetSetting();
	if (pSetting->bTextureMode32)
	{
		DWORD *pColor = (DWORD*)lcdst.pBits;
		for(int x = 0; x < 256; x++)
		{
			for(int y = 0; y < 256; y++)
			{
				pColor[x + y * (lcdst.Pitch / 4)] = 0xFF000000 | GetPixel(dc,(int)(x * fRatioX),(int)(y * fRatioY));
			}
		}
	}
	else
	{
		WORD *pColor = (WORD*)lcdst.pBits;
		for(int x = 0; x < 256; x++)
		{
			for(int y = 0; y < 256; y++)
			{
				COLORREF c = GetPixel(dc,(int)(x * fRatioX),(int)(y * fRatioY));
				DWORD b = (c & 0x00FF0000 >> 16) >> 4;
				DWORD g = (c & 0x0000FF00 >> 8) >> 4;
				DWORD r = (c & 0x000000FF ) >> 4;
				pColor[x + y * (lcdst.Pitch / 2)] =(WORD)( 0xF000 |  b  | (g << 4) | (b << 8));
			}
		}
	}
	pd3dTexture->UnlockRect(0);

	pRenderTarget->ReleaseDC(dc);

	SAFERELEASE(pOldRenderTarget);
	SAFERELEASE(pOldZStencil);

	SAFERELEASE(pRenderTarget);
	SAFERELEASE(pDepthStencilSurface);

	pInterface->GetDevice()->BeginScene();
}


void EditWnd::WriteTGA(UINT uBits, UINT uWidth, UINT uHeight, BYTE *pixels, FILE *pf)
{
	static UINT count = 0;
	BYTE btTGAHeader[12] = { 0,0,2,0,0,0,0,0,0,0,0,0 };
	BYTE btHeader[6];
	UINT uBytesPerPixel = 0;
	UINT uImageSize = 0;
	UINT uTemp;

	btHeader[0] = (BYTE)(uWidth % 256);
	btHeader[1] = (BYTE)(uWidth / 256);
	btHeader[2] = (BYTE)(uHeight % 256);
	btHeader[3] = (BYTE)(uHeight / 256);
	btHeader[4] = (BYTE)uBits;
	btHeader[5] = 8;

	uBytesPerPixel = uBits / 8;
	uImageSize = uWidth * uHeight * uBytesPerPixel;

	fwrite(btTGAHeader, 12, sizeof(BYTE), pf);
	fwrite(btHeader, 6, sizeof(BYTE), pf);

	for(UINT i = 0; i < uImageSize; i += uBytesPerPixel)
	{
		uTemp=pixels[i];
		pixels[i] = pixels[i + 2];
		pixels[i + 2] = uTemp;
	}

	fwrite(pixels, uImageSize, sizeof(BYTE), pf);
}
void EditWnd::CreatePopupWnd(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();

	int   iWidth = pInterface->GetWndWidth();
	int   iHeight= pInterface->GetWndHeight();

	//about wnd	
	RECT rc = {0,0,768,600};
	m_pAboutWnd = new AboutWnd;
	m_pAboutWnd->Create("地图编辑器_2.0 2003-2006_贾涛,2007-2009 彭谦",&rc,0,this);
	m_pAboutWnd->CenterWndTo(iWidth/2,iHeight/2);

	//生成 新地图窗口
	RECT rcNewMapDlg = { 0,0,436,600};
	m_pNewMapDlg = new WndNewMap;
	m_pNewMapDlg->Create("新建地图",&rcNewMapDlg,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW,this);
	m_pNewMapDlg->CenterWndTo(iWidth/2,iHeight/2);
	m_pNewMapDlg->ShowWnd(FALSE);

	//生成 奇幻点编辑窗口
	RECT rcWndPopupDoorEdit = { 393,197,595,542};
	m_pWndPopupDoorEdit = new WndPopupDoorEdit;
	m_pWndPopupDoorEdit->Create("切换点编辑",&rcWndPopupDoorEdit,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW ,this);
	m_pWndPopupDoorEdit->CenterWndTo(iWidth/2,iHeight/2);
	m_pWndPopupDoorEdit->ShowWnd(FALSE);

	//生成 灯光编辑窗口
	RECT rcWndLightAdjust= { 0,0,203,92};
	m_pWndLightAdjust = new WndLightAdjust;
	m_pWndLightAdjust->Create("场景灯光",&rcWndLightAdjust,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW ,this);
	m_pWndLightAdjust->CenterWndTo(iWidth/2,iHeight/2);
	m_pWndLightAdjust->ShowWnd(FALSE);
	m_pWndLightAdjust->SetAmbientColor(AMBIENTCOLOR);
	m_pWndLightAdjust->SetDirectionalColor(DIRECTIONALCOLOR);

	//生成 雾编辑窗口
	RECT rcWndFogAdjust= { 0,0,300,120};
	m_pWndFogAdjust = new WndFogAdjust;
	m_pWndFogAdjust->Create("场景雾效",&rcWndFogAdjust,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW ,this);
	m_pWndFogAdjust->CenterWndTo(iWidth/2,iHeight/2);
	m_pWndFogAdjust->ShowWnd(FALSE);

	//生成导出窗口
	RECT rcWndExport= { 0,0,204,235};
	m_pWndExport = new WndExport;
	m_pWndExport->Create("导出地图",&rcWndExport,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW ,this);
	m_pWndExport->CenterWndTo(iWidth/2,iHeight/2);
	m_pWndExport->ShowWnd(FALSE);

	//生成随机替换窗口
	RECT rcWndRandomReplace= { 0,0,228,257};
	m_pWndRandomReplace = new WndRandomReplace;
	m_pWndRandomReplace->Create("随机替换",&rcWndRandomReplace,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW ,this);
	m_pWndRandomReplace->CenterWndTo(iWidth/2,iHeight/2);
	m_pWndRandomReplace->ShowWnd(FALSE);
	
	RECT rcWndTerrainSet = { 0,0,204,235};
	m_pWndSetTerrainTile = new WndSetTerrainTile;
	m_pWndSetTerrainTile->Create("地形SET",&rcWndTerrainSet,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW ,this);
	m_pWndSetTerrainTile->CenterWndTo(iWidth/2,iHeight/2);
	m_pWndSetTerrainTile->ShowWnd(FALSE);

	RECT rcWndSkyBox = { 0,0,204,235};
	m_pWndSetSkyBox = new WndSetSkyBox;
	m_pWndSetSkyBox->Create("SKYBOX",&rcWndSkyBox,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW ,this);
	m_pWndSetSkyBox->CenterWndTo(iWidth/2,iHeight/2);
	m_pWndSetSkyBox->ShowWnd(FALSE);

}

void EditWnd::_DestroyPopupWnd(void)
{
	SAFEDESTROY(m_pWndFogAdjust);
	SAFEDESTROY(m_pWndSetTerrainTile);
	SAFEDESTROY(m_pWndSetSkyBox);
	SAFEDESTROY(m_pWndRandomReplace);
	SAFEDESTROY(m_pWndPopupDoorEdit);
	SAFEDESTROY(m_pWndExport);
	SAFEDESTROY(m_pWndCameraPath);
	SAFEDESTROY(m_pWndLightAdjust);		
	SAFEDESTROY(m_pNewMapDlg);
}

void EditWnd::ExportMap(BOOL bSurface, BOOL bHeightmap, BOOL bColormap, BOOL bGrid, BOOL bNode,BOOL bSound)
{
	render::Interface *pInterface = render::Interface::GetInstance();

	BROWSEINFO bi;
	char display[MAX_PATH] = "SelectName";
	char path[MAX_PATH];

	ITEMIDLIST *pidl;
	bi.hwndOwner=NULL;	//AfxGetMainWnd()->GetSafeHwnd();
	bi.pidlRoot=0;
	bi.pszDisplayName = display;
	bi.lpszTitle = "选择目录";
	bi.ulFlags= BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	bi.lpfn = 0;
	bi.lParam = 0;
	bi.iImage = 0;

	pidl = SHBrowseForFolder(&bi);

	if(!pidl)
		return ;
	SHGetPathFromIDList(pidl, path);

	//开始导出
	FILE* file(NULL);
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();

	TerrainMesh::_tagVertex *pVertexList = pTerrainMesh->GetVertexList();
	char szFileName[1024];
	ProgressBar pb;
	pb.Begin(pInterface->GetWndHandle(),0xa00000,6);

	///////地图配置文件//////////////////////////////////////////////////////////
	pb.StepIt();
	sprintf(szFileName,"%s\\config",path);
	file = fopen(szFileName, "wb");
	if( file == NULL )
	{
		MessageBox(NULL,szFileName,"文件打开失败",MB_OK);
		return ;
	}

	fprintf(file,"//----Map Config File -----\n");
	fprintf(file,"Version = %s;\n", pszMapVersion);
	fprintf(file,"Width = %d;\n",pMap->GetWidth());
	fprintf(file,"Depth = %d;\n",pMap->GetDepth());
	fprintf(file,"Vertices = %d\n",pTerrainMesh->GetVertexNum());
	fprintf(file,"SunLightAmbientColor = %8.8x;\n",pMap->GetSunLightAmbientColor());
	fprintf(file,"SunLightDiffuseColor = %8.8x;\n",pMap->GetSunLightDiffuseColor());
	fprintf(file,"SunLightDirectionX = %f;\n",pMap->GetSunLightDirection()->x);
	fprintf(file,"SunLightDirectionY = %f;\n",pMap->GetSunLightDirection()->y);
	fprintf(file,"SunLightDirectionZ = %f;\n",pMap->GetSunLightDirection()->z);
	fprintf(file,"FogEnable = %d;\n",pMap->GetFogEnable());
	fprintf(file,"FogColor = %X;\n",pMap->GetFogColor());
	fprintf(file,"FogStart = %d;\n",pMap->GetFogStart());
	fprintf(file,"FogEnd   = %d;\n",pMap->GetFogEnd());
	if (m_pGameSkybox)
	{
		if (m_pGameSkybox->GetModelID())
		{
			fprintf(file,"SkyBoxID   = %d;\n",m_pGameSkybox->GetModelID());
			fprintf(file,"SkyBoxSize   = %d;\n",m_pGameSkybox->GetScal());
		}
	}
	fclose(file);
	///////////////////////////////////////////////////////////////////////////////

	//导出高度图
	pb.StepIt();
	if (bHeightmap)
	{	

		sprintf(szFileName,"%s\\heightmap",path);
		file = fopen(szFileName, "wb");
		if( file == NULL )
		{
			MessageBox(NULL,szFileName,"文件打开失败",MB_OK);
			return ;
		}
		//先输出顶点,

		//位置
		for(DWORD n = 0 ; n < pTerrainMesh->GetVertexNum(); n ++)
		{
			fwrite(&pVertexList[n].vPosition.y,sizeof(float),1,file);
		}
		OutputConsole("Write Height data  %d \n",pTerrainMesh->GetVertexNum());
		fclose(file);
	}
	//导出颜色图
	pb.StepIt();
	if (bColormap)
	{	
		/////////////////////////	
		////计算所有颜色	
		pMap->CalculateAllBaseLighting();
		pMap->CalculateAllLightSourceLighting();
		pMap->CalculateTerrainShadow();
		sprintf(szFileName,"%s\\colormap",path);
		file = fopen(szFileName, "wb");
		if( file == NULL )
		{
			MessageBox(NULL,szFileName,"文件打开失败",MB_OK);
			return ;
		}
		//先输出顶点,
		TerrainMesh::_tagVertex *pVertexList = pTerrainMesh->GetVertexList();
		//位置
		for(DWORD n = 0 ; n < pTerrainMesh->GetVertexNum(); n ++)
		{
			D3DXCOLOR color1 = D3DXCOLOR(pVertexList[n].cColor);
			D3DXCOLOR color2 = D3DXCOLOR(pVertexList[n].cAddColor);
			DWORD dwColor = D3DCOLOR_ARGB((DWORD)(255 * color1.a * color2.a), 
				(DWORD)(255 * color1.r * color2.r), 
				(DWORD)(255 * color1.g * color2.g), 
				(DWORD)(255 * color1.b * color2.b));
			fwrite(&dwColor,sizeof(DWORD),1,file);
		}
		OutputConsole("Write Color data  %d \n",pTerrainMesh->GetVertexNum());
		fclose(file);
	}

	//导出地表
	pb.StepIt();
	if (bSurface)
	{
		sprintf(szFileName,"%s\\surface",path);
		file = fopen(szFileName, "wb");
		if( file == NULL )
		{
			MessageBox(NULL,szFileName,"文件打开失败",MB_OK);
			return ;
		}
		DWORD dwTextureSetIndex = pTerrainMesh->GetTexSetIndex();
		fwrite(&dwTextureSetIndex,sizeof(DWORD),1,file);

		for(DWORD n = 0 ;  n < (DWORD)(pMap->GetWidth() * pMap->GetDepth()); n++)
		{
			TerrainMesh::_tagGrid *pTerrainGrid = pTerrainMesh->GetGrid(n);
			Grid *pMapGrid = pMap->GetGrid(n);

			//uv coord
			fwrite(pTerrainGrid->uv,sizeof(BYTE),4,file);
			//textureindex
			WORD wLayerTextureIndex = 0x0000;
			for(int i = 0;i < 4; i++)
			{
				if (pTerrainGrid->byTexCode[i] != 0)
				{
					wLayerTextureIndex |= ((pTerrainGrid->byTexIndex[i] & 0xF) << (i * 4) );
				}                        
				else
					wLayerTextureIndex |= (0xF << (i * 4));
			}
			fwrite(&wLayerTextureIndex,sizeof(WORD),1,file);
		}
		OutputConsole("Write Surface %d x %d \n",pMap->GetWidth(),pMap->GetDepth());
		fclose(file);
	}

	//导出格子属性
	pb.StepIt();
	if (bGrid)
	{
		sprintf(szFileName,"%s\\grid",path);
		file = fopen(szFileName, "wb");
		if( file == NULL )
		{
			MessageBox(NULL,szFileName,"文件打开失败",MB_OK);
			return ;
		}

		for(DWORD n = 0 ;  n < (DWORD)(pMap->GetWidth() * pMap->GetDepth()); n++)
		{
			TerrainMesh::_tagGrid *pTerrainGrid = pTerrainMesh->GetGrid(n);
			Grid *pMapGrid = pMap->GetGrid(n);

			//揭房顶文件标记
			DWORD dwTargetHideGridIndex = pMapGrid->GetTargetHideGridIndex();
			fwrite(&dwTargetHideGridIndex,sizeof(DWORD),1,file);

			BYTE byFlag = pMapGrid->GetFlag();
			fwrite(&byFlag,sizeof(BYTE),1,file);
		}

		OutputConsole("Write grid %d x %d \n",pMap->GetWidth(),pMap->GetDepth());
		fclose(file);
	}

	//导出Node
	pb.StepIt();
	if (bNode)
	{
		sprintf(szFileName,"%s\\node",path);
		file = fopen(szFileName, "wb");
		if( file == NULL )
		{
			MessageBox(NULL,szFileName,"文件打开失败",MB_OK);
			return ;
		}
		//建筑列表
		DWORD dwCount = 0;
		DWORD n;
		for(n = 0 ;  n <(DWORD)(pMap->GetWidth() * pMap->GetDepth()); n++)
		{
			TerrainMesh::_tagGrid *pTerrainGrid = pTerrainMesh->GetGrid(n);
			Grid *pMapGrid = pMap->GetGrid(n);

			//建物
			if (pMapGrid->GetBuild()->wID != 0xFFFF)
			{
				dwCount++;
			}
		}
		fwrite(&dwCount,sizeof(DWORD),1,file);
		dwCount = 0;
		for(n = 0 ;  n <(DWORD)(pMap->GetWidth() * pMap->GetDepth()); n++)
		{
			TerrainMesh::_tagGrid *pTerrainGrid = pTerrainMesh->GetGrid(n);
			Grid *pMapGrid = pMap->GetGrid(n);

			//建物
			WORD wBuildID = pMapGrid->GetBuild()->wID;
			if (wBuildID != 0xFFFF)
			{
				float fBuildHeight = pMapGrid->GetBuild()->fHeight;
				float fBuildRotationY = pMapGrid->GetBuild()->fRotationY;

				fwrite(&n,sizeof(DWORD),1,file);
				fwrite(&wBuildID,sizeof(WORD),1,file);
				fwrite(&fBuildHeight,sizeof(float),1,file);
				fwrite(&fBuildRotationY,sizeof(float),1,file);     
				dwCount++;
			}
		}

		OutputConsole("Write build %d\n",dwCount);
		//写节点
		//分割空间	
		pb.StepIt();
		std::vector<int> rootListIndices;
		std::vector<int> rootBuildListIndices;
		for(DWORD n = 0; n < pTerrainMesh->GetGridNum(); n++)
		{
			rootListIndices.push_back(n);
		}
		for(n = 0; n < pTerrainMesh->GetGridNum();n++)
		{
			Grid *pGrid = pMap->GetGrid(n);
			if (pGrid->GetBuild()->wID != 0xFFFF)
			{
				rootBuildListIndices.push_back(n);
			}
		}

		//计算boudningbox
		D3DXVECTOR3 vMin(0,0,0),vMax(0,0,0);
		for(n = 0; n < pTerrainMesh->GetVertexNum(); n++)
		{
			D3DXVECTOR3 &vVtx = pVertexList[n].vPosition;
			if (vVtx.x < vMin.x) vMin.x = vVtx.x;
			if (vVtx.y < vMin.y) vMin.y = vVtx.y;
			if (vVtx.z < vMin.z) vMin.z = vVtx.z;
			if (vVtx.x > vMax.x) vMax.x = vVtx.x;
			if (vVtx.y > vMax.y) vMax.y = vVtx.y;
			if (vVtx.z > vMax.z) vMax.z = vVtx.z;
		}

		//求出正方体包围盒
		float fMin = vMin.x;
		if (fMin > vMin.y) fMin = vMin.y;
		if (fMin > vMin.z) fMin = vMin.z;
		float fMax = vMax.x;
		if (fMax < vMax.y) fMax = vMax.y;
		if (fMax < vMax.z) fMax = vMax.z;

		DWORD dwNodeCount(0);
		ONode *pRootNode = new ONode;
		pRootNode->SetBoundingBox(D3DXVECTOR3(fMin,fMin,fMin),D3DXVECTOR3(fMax,fMax,fMax));
		pRootNode->OptimizeBuild(rootListIndices,rootBuildListIndices,dwNodeCount);
		rootListIndices.clear();   
		rootBuildListIndices.clear();
		OutputConsole("Node:%d %d(byte)\n",dwNodeCount,dwNodeCount * sizeof(ONode) );
		//写入
		pRootNode->Write(file);
		//释放
		SAFEDELETE(pRootNode);
		fclose(file);
	}

	//声音列表文件
	if (bSound)
	{
		//EditWnd *pEditWnd = CommUI::GetInstance()->GetEditWnd();
		//ToolBuildEdit *  pToolBuildEdit = pEditWnd->GetToolBuildEdit();
		//GameBuildList *  pGameBuildList = GameBuildList::GetInstance();  
		////	pb.Begin(pInterface->GetWndHandle(),0xa00000,5);
		//sprintf(szFileName,"%s\\sound",path);
		//file = fopen(szFileName, "wb");
		//if( file == NULL )
		//{
		//	MessageBox(NULL,szFileName,"文件打开失败",MB_OK);
		//	return ;
		//}
		//DWORD dwHead = MAKEFOURCC('S','S','L',' ');//Sound Source List
		//DWORD dwVersion = MAKEFOURCC('1','.','0','0');
		//fwrite(&dwHead,sizeof(DWORD), 1, file);
		//fwrite(&dwVersion,sizeof(DWORD), 1, file);

		//DWORD dwMapWidth  = (DWORD)pMap->GetWidth();
		//DWORD dwMapDepth  = (DWORD)pMap->GetDepth();
		//DWORD dwSoundSourceCount = 0;
		//for(DWORD n = 0 ;  n < dwMapWidth * dwMapDepth; n++)
		//{
		//	Grid *pMapGrid = pMap->GetGrid(n);
		//	if (pMapGrid->GetBuild()->wID != 0xFFFF)
		//	{
		//		//建物
		//		WORD wBuildID = pMapGrid->GetBuild()->wID;
		//		GameBuildList::Build *pBuild = pGameBuildList->GetBuild(wBuildID);
		//		if (pBuild->GetSoundSource())
		//			dwSoundSourceCount++;
		//	}
		//}
		//fwrite(&dwSoundSourceCount,sizeof(DWORD),1,file);
		//for(n = 0; n < dwMapWidth * dwMapDepth; n++)
		//{
		//	Grid *pMapGrid = pMap->GetGrid(n);
		//	if (pMapGrid->GetBuild()->wID != 0xFFFF)
		//	{
		//		//建物
		//		WORD wBuildID = pMapGrid->GetBuild()->wID;
		//		GameBuildList::Build *pBuild = pGameBuildList->GetBuild(wBuildID);
		//		if (pBuild->GetSoundSource())
		//		{
		//			//x z position
		//			float x = (float)(n % dwMapWidth);
		//			float z = (float)(n / dwMapWidth);
		//			fwrite(&x,sizeof(float),1,file);
		//			fwrite(&z,sizeof(float),1,file);
		//			//sound source struct
		//			fwrite(pBuild->GetSoundSource(),sizeof(GameBuildList::Build::tagSoundSource),1,file);
		//		}
		//	}
		//}
		//fclose(file);
	}
	pb.End();
}

void EditWnd::QuickExprot()
{
	render::Interface *pInterface = render::Interface::GetInstance();


	//开始导出
	FILE* file(NULL);
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();

	TerrainMesh::_tagVertex *pVertexList = pTerrainMesh->GetVertexList();
	char szFileName[1024];
	ProgressBar pb;
	pb.Begin(pInterface->GetWndHandle(),0xa00000,6);

	char path[1024] = "demo";

	///////地图配置文件//////////////////////////////////////////////////////////
	pb.StepIt();
	sprintf(szFileName,"%s\\config",path);
	file = fopen(szFileName, "wb");
	if( file == NULL )
	{
		MessageBox(NULL,szFileName,"文件打开失败",MB_OK);
		return ;
	}

	fprintf(file,"//----Map Config File -----\n");
	fprintf(file,"Version = %s;\n", pszMapVersion);
	fprintf(file,"Width = %d;\n",pMap->GetWidth());
	fprintf(file,"Depth = %d;\n",pMap->GetDepth());
	fprintf(file,"Vertices = %d\n",pTerrainMesh->GetVertexNum());
	fprintf(file,"SunLightAmbientColor = %8.8x;\n",pMap->GetSunLightAmbientColor());
	fprintf(file,"SunLightDiffuseColor = %8.8x;\n",pMap->GetSunLightDiffuseColor());
	fprintf(file,"SunLightDirectionX = %f;\n",pMap->GetSunLightDirection()->x);
	fprintf(file,"SunLightDirectionY = %f;\n",pMap->GetSunLightDirection()->y);
	fprintf(file,"SunLightDirectionZ = %f;\n",pMap->GetSunLightDirection()->z);
	fprintf(file,"FogColor = %X;\n",pMap->GetFogColor());
	fprintf(file,"FogStart = %f;\n",pMap->GetFogStart());
	fprintf(file,"FogEnd   = %f;\n",pMap->GetFogEnd());
	if (m_pGameSkybox)
	{
		if (m_pGameSkybox->GetModelID())
		{
			fprintf(file,"SkyBoxID   = %d;\n",m_pGameSkybox->GetModelID());
			fprintf(file,"SkyBoxSize   = %d;\n",m_pGameSkybox->GetScal());
		}
	}
	fclose(file);
	///////////////////////////////////////////////////////////////////////////////

	//导出高度图
	pb.StepIt();
	{	

		sprintf(szFileName,"%s\\heightmap",path);
		file = fopen(szFileName, "wb");
		if( file == NULL )
		{
			MessageBox(NULL,szFileName,"文件打开失败",MB_OK);
			return ;
		}
		//先输出顶点,

		//位置
		for(DWORD n = 0 ; n < pTerrainMesh->GetVertexNum(); n ++)
		{
			fwrite(&pVertexList[n].vPosition.y,sizeof(float),1,file);
		}
		OutputConsole("Write Height data  %d \n",pTerrainMesh->GetVertexNum());
		fclose(file);
	}
	//导出颜色图
	pb.StepIt();
	{	
		/////////////////////////	
		////计算所有颜色	
		pMap->CalculateAllBaseLighting();
		pMap->CalculateAllLightSourceLighting();
		pMap->CalculateTerrainShadow();
		sprintf(szFileName,"%s\\colormap",path);
		file = fopen(szFileName, "wb");
		if( file == NULL )
		{
			MessageBox(NULL,szFileName,"文件打开失败",MB_OK);
			return ;
		}
		//先输出顶点,
		TerrainMesh::_tagVertex *pVertexList = pTerrainMesh->GetVertexList();
		//位置
		for(DWORD n = 0 ; n < pTerrainMesh->GetVertexNum(); n ++)
		{
			D3DXCOLOR color1 = D3DXCOLOR(pVertexList[n].cColor);
			D3DXCOLOR color2 = D3DXCOLOR(pVertexList[n].cAddColor);
			DWORD dwColor = D3DCOLOR_ARGB((DWORD)(255 * color1.a * color2.a), 
				(DWORD)(255 * color1.r * color2.r), 
				(DWORD)(255 * color1.g * color2.g), 
				(DWORD)(255 * color1.b * color2.b));
			fwrite(&dwColor,sizeof(DWORD),1,file);
		}
		OutputConsole("Write Color data  %d \n",pTerrainMesh->GetVertexNum());
		fclose(file);
	}

	//导出地表
	pb.StepIt();
	{
		sprintf(szFileName,"%s\\surface",path);
		file = fopen(szFileName, "wb");
		if( file == NULL )
		{
			MessageBox(NULL,szFileName,"文件打开失败",MB_OK);
			return ;
		}
		DWORD dwTextureSetIndex = pTerrainMesh->GetTexSetIndex();
		fwrite(&dwTextureSetIndex,sizeof(DWORD),1,file);

		for(DWORD n = 0 ;  n < (DWORD)(pMap->GetWidth() * pMap->GetDepth()); n++)
		{
			TerrainMesh::_tagGrid *pTerrainGrid = pTerrainMesh->GetGrid(n);
			Grid *pMapGrid = pMap->GetGrid(n);

			//uv coord
			fwrite(pTerrainGrid->uv,sizeof(BYTE),4,file);
			//textureindex
			WORD wLayerTextureIndex = 0x0000;
			for(int i = 0;i < 4; i++)
			{
				if (pTerrainGrid->byTexCode[i] != 0)
				{
					wLayerTextureIndex |= ((pTerrainGrid->byTexIndex[i] & 0xF) << (i * 4) );
				}                        
				else
					wLayerTextureIndex |= (0xF << (i * 4));
			}
			fwrite(&wLayerTextureIndex,sizeof(WORD),1,file);
		}
		OutputConsole("Write Surface %d x %d \n",pMap->GetWidth(),pMap->GetDepth());
		fclose(file);
	}

	//导出格子属性
	pb.StepIt();
	{
		sprintf(szFileName,"%s\\grid",path);
		file = fopen(szFileName, "wb");
		if( file == NULL )
		{
			MessageBox(NULL,szFileName,"文件打开失败",MB_OK);
			return ;
		}

		for(DWORD n = 0 ;  n < (DWORD)(pMap->GetWidth() * pMap->GetDepth()); n++)
		{
			TerrainMesh::_tagGrid *pTerrainGrid = pTerrainMesh->GetGrid(n);
			Grid *pMapGrid = pMap->GetGrid(n);

			//揭房顶文件标记
			DWORD dwTargetHideGridIndex = pMapGrid->GetTargetHideGridIndex();
			fwrite(&dwTargetHideGridIndex,sizeof(DWORD),1,file);

			BYTE byFlag = pMapGrid->GetFlag();
			fwrite(&byFlag,sizeof(BYTE),1,file);
		}

		OutputConsole("Write grid %d x %d \n",pMap->GetWidth(),pMap->GetDepth());
		fclose(file);
	}

	//导出Node
	pb.StepIt();
	{
		sprintf(szFileName,"%s\\node",path);
		file = fopen(szFileName, "wb");
		if( file == NULL )
		{
			MessageBox(NULL,szFileName,"文件打开失败",MB_OK);
			return ;
		}
		//建筑列表
		DWORD dwCount = 0;
		DWORD n;
		for(n = 0 ;  n <(DWORD)(pMap->GetWidth() * pMap->GetDepth()); n++)
		{
			TerrainMesh::_tagGrid *pTerrainGrid = pTerrainMesh->GetGrid(n);
			Grid *pMapGrid = pMap->GetGrid(n);

			//建物
			if (pMapGrid->GetBuild()->wID != 0xFFFF)
			{
				dwCount++;
			}
		}
		fwrite(&dwCount,sizeof(DWORD),1,file);
		dwCount = 0;
		for(n = 0 ;  n <(DWORD)(pMap->GetWidth() * pMap->GetDepth()); n++)
		{
			TerrainMesh::_tagGrid *pTerrainGrid = pTerrainMesh->GetGrid(n);
			Grid *pMapGrid = pMap->GetGrid(n);

			//建物
			WORD wBuildID = pMapGrid->GetBuild()->wID;
			if (wBuildID != 0xFFFF)
			{
				float fBuildHeight = pMapGrid->GetBuild()->fHeight;
				float fBuildRotationY = pMapGrid->GetBuild()->fRotationY;			

				fwrite(&n,sizeof(DWORD),1,file);
				fwrite(&wBuildID,sizeof(WORD),1,file);                          				
				fwrite(&fBuildHeight,sizeof(float),1,file);
				fwrite(&fBuildRotationY,sizeof(float),1,file);     
				dwCount++;
			}
		}

		OutputConsole("Write build %d\n",dwCount);
		//写节点
		//分割空间	
		pb.StepIt();
		std::vector<int> rootListIndices;
		std::vector<int> rootBuildListIndices;
		for(DWORD n = 0; n < pTerrainMesh->GetGridNum(); n++)
		{
			rootListIndices.push_back(n);
		}
		for(n = 0; n < pTerrainMesh->GetGridNum();n++)
		{
			Grid *pGrid = pMap->GetGrid(n);
			if (pGrid->GetBuild()->wID != 0xFFFF)
			{
				rootBuildListIndices.push_back(n);
			}
		}

		//计算boudningbox
		D3DXVECTOR3 vMin(0,0,0),vMax(0,0,0);
		for(n = 0; n < pTerrainMesh->GetVertexNum(); n++)
		{
			D3DXVECTOR3 &vVtx = pVertexList[n].vPosition;
			if (vVtx.x < vMin.x) vMin.x = vVtx.x;
			if (vVtx.y < vMin.y) vMin.y = vVtx.y;
			if (vVtx.z < vMin.z) vMin.z = vVtx.z;
			if (vVtx.x > vMax.x) vMax.x = vVtx.x;
			if (vVtx.y > vMax.y) vMax.y = vVtx.y;
			if (vVtx.z > vMax.z) vMax.z = vVtx.z;
		}

		//求出正方体包围盒
		float fMin = vMin.x;
		if (fMin > vMin.y) fMin = vMin.y;
		if (fMin > vMin.z) fMin = vMin.z;
		float fMax = vMax.x;
		if (fMax < vMax.y) fMax = vMax.y;
		if (fMax < vMax.z) fMax = vMax.z;

		DWORD dwNodeCount(0);
		ONode *pRootNode = new ONode;
		pRootNode->SetBoundingBox(D3DXVECTOR3(fMin,fMin,fMin),D3DXVECTOR3(fMax,fMax,fMax));
		pRootNode->OptimizeBuild(rootListIndices,rootBuildListIndices,dwNodeCount);
		rootListIndices.clear();   
		rootBuildListIndices.clear();
		OutputConsole("Node:%d %d(byte)\n",dwNodeCount,dwNodeCount * sizeof(ONode) );
		//写入
		pRootNode->Write(file);
		//释放
		SAFEDELETE(pRootNode);
		fclose(file);
	}

	pb.End();
}

void EditWnd::OnExportCamera(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

	OPENFILENAME ofn;       // common dialog box structure
	static char szFile[_MAX_PATH];       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = pInterface->GetWndHandle();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Camera File (*)\0*\0All Files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_FILEMUSTEXIST;

	char szOldDirectory[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH,szOldDirectory);
	

	// Display the Open dialog box. 
	if (GetSaveFileName(&ofn))
	{
		FILE *fp = fopen(szFile,"wb");
		fprintf(fp,"camera\n{\n");
		fprintf(fp,"    position\n");
		fprintf(fp,"    {\n");
		fprintf(fp,"        x = %f\n        y = %f\n        z = %f\n",m_pCamera->GetPosition()->x,
			m_pCamera->GetPosition()->y,m_pCamera->GetPosition()->z);
		fprintf(fp,"    }\n");
		fprintf(fp,"    view\n");
		fprintf(fp,"    {\n");
		fprintf(fp,"        x = %f\n        y = %f\n        z = %f\n",m_pCamera->GetView()->x,
			m_pCamera->GetView()->y,m_pCamera->GetView()->z);
		fprintf(fp,"    }\n");
		fprintf(fp,"    fovy = %f;\n",m_pCamera->GetFOVY());
		fprintf(fp,"    near = %f;\n",m_pCamera->GetNearPlane());
		fprintf(fp,"    far  = %f;\n",m_pCamera->GetFarPlane());
		fprintf(fp,"    aspect = %f;\n",m_pCamera->GetAspect());
		fprintf(fp,"}\n");
		fclose(fp);
	}        	
	
	SetCurrentDirectory(szOldDirectory);
}

void EditWnd::ImportCamera(const char *szFileName)
{
	utility::File file;
	if (file.Open(szFileName))
	{
		utility::Text script;
		script.Create(&file);
		utility::Text::Node *pCameraNode = script.GetRootNode()->GetChildNode("camera");
		utility::Text::Node *pPositionNode = pCameraNode->GetChildNode("position");
		ASSERT(pPositionNode);
		m_pCamera->GetPosition()->x = pPositionNode->GetVar("x")->GetFloatValue();
		m_pCamera->GetPosition()->y = pPositionNode->GetVar("y")->GetFloatValue();
		m_pCamera->GetPosition()->z = pPositionNode->GetVar("z")->GetFloatValue();
		utility::Text::Node *pViewNode = pCameraNode->GetChildNode("view");
		m_pCamera->GetView()->x = pViewNode->GetVar("x")->GetFloatValue();
		m_pCamera->GetView()->y = pViewNode->GetVar("y")->GetFloatValue();
		m_pCamera->GetView()->z = pViewNode->GetVar("z")->GetFloatValue(); 

		m_pCamera->SetFOVY(pCameraNode->GetVar("fovy")->GetFloatValue());
		m_pCamera->SetNearClipPlane(pCameraNode->GetVar("near")->GetFloatValue());
		m_pCamera->SetFarClipPlane(pCameraNode->GetVar("far")->GetFloatValue());
		m_pCamera->SetAspect(pCameraNode->GetVar("aspect")->GetFloatValue());
		script.Destroy();
		file.Close();
	}

}

