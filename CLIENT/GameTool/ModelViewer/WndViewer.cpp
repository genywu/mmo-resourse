#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\frostengine\input.h"
#include "..\..\gameengine\gamegraphics2d.h"
#include "..\..\gameengine\gamemodelmanager.h"
//#include "..\..\gameengine\gamemodel.h"

#include "animmodifydlg.h"
#include "GridMovingSpeedModifyDlg.h"
#include "WndHotkey.h"
#include "wndabout.h"
#include "wndviewer.h"
#include "wndaction.h"
#include "WndPart.h"
#include "WndLocator.h"
#include "WndModel.h"
#include "wndadjust.h"
#include "wndadjustlightcolor.h"
#include "WndAnimFrameEx.h"
#include "WndGuage.h"
#include "AppClient\RoleModel.h"
#include "gamesetting.h"
#include "light.h"
#include "dlgselectlevel.h"
#include "WndModelShaderTree.h"
#include "WndShaderModify.h"
#include "WndModelTree.h"
#include "WndPartTree.h"
#include "WndModelFileModify.h"
#include "WndPartModify.h"
#include "WndTrailTree.h"
#include "WndTrailModify.h"
#include "WndParticleTree.h"
#include "WndParticleModify.h"
#define VK_B				0x42
#define VK_E				0x45
#define VK_H				0x48
#define VK_I				0x49
#define VK_O				0x4F
#define VK_R				0x52
#define VK_X				0x58
#define VK_S				0x53
#define VK_ALT				VK_MENU
#define KEYDOWN(VK_CODE)	((GetAsyncKeyState(VK_CODE) & 0x8000) ? 1 : 0)

//D3DXVECTOR3 g_vTri[3];

WndViewer::WndViewer(void)
{
	m_pCamera = NULL;
	m_eCamOpType = CCO_NONE;

	m_bDisplayWireFrame = FALSE;
	m_bShowGrid = TRUE;	
	m_bShowGridMoving = FALSE;
	m_bShowSizeBox = FALSE;
	m_bShowPickBox = FALSE;

	m_bShowPickBox = FALSE;
	m_bShowBone	  = FALSE;
	m_bShowNormal = FALSE;
	m_bShowCameraInfo = FALSE;
	m_bShowLocatorWnd = FALSE;

	m_dwStartTime = 0;
	m_dwCurFrame  = 0;

	m_bShowLocator = FALSE;
	m_bAnimLooped  = TRUE;
	m_bAnimPlaying = FALSE;
	m_bSwitchBK = FALSE;
	m_bShowLightArrow = TRUE;
	m_bExAnimFrame = FALSE;
	m_bShowViewfinder = FALSE;
	m_bTrail	   = FALSE;
	m_bShowUI		= TRUE;
	DWORD dwCurrent = timeGetTime();
	m_dwGridMovingStartTime = dwCurrent;
	m_dwGridTexMovingStartTime = dwCurrent;
	m_fGridMovingSpeed = 0.0016f;

	m_dwGroupMenuItemCount = 0;
	m_dwWeaponLeftMenuItemIndex = 0;
	m_dwWeaponRightMenuItemIndex = 0;
	m_dwAmbientColor = 0xFF000000;
	m_dwLightDirectionColor = 0xFFFFFFFF;

	m_iLevelModelIndex = 0;
	m_eLevelModelType = MT_ROLE;

	m_hCursorHandle = 0;
	m_dwBK			= 0;
	m_dwGridTex		= 0;
	m_ptBKTexInfo	= NULL;

	m_pMenuBar   = NULL;
	m_pQuickMenu = NULL;

	ZeroMemory(&m_vRolePos, sizeof(D3DXVECTOR3));
	m_pModel = NULL;
	m_pRefModel = NULL;

	//m_pWing = NULL;
	//m_pWeaponLeft = NULL;
	//m_pWeaponRight = NULL;

	m_pWndAnimModify	 = NULL;
	m_pWndGridMovingSpeedModify = NULL;
	m_pWndHotkeyList	 = NULL;
	m_pWndAbout			 = NULL;
	m_pWndEditBKColor	 = NULL;
	m_pWndEditLightColor = NULL;
	m_pWndSelectLevel	 = NULL;
	m_pWndAction		 = NULL;
	m_pWndPart			 = NULL;
	m_pWndModels		 = NULL;
	m_pWndLocator		 = NULL;
	m_pWndAnimFrameEx	 = NULL;
	m_pWndGuage			 = NULL;
	m_pWndModelTexture   = NULL;
	m_pWndShaderModify   = NULL;
	m_pWndModelTree		 = NULL;
	m_pWndModelModify	 = NULL;
	m_pWndPartModify	 = NULL;
	m_pWndTrailTree		 = NULL;
	m_pWndTrailModify	 = NULL;
	m_pWndParticleTree   = NULL;
	m_pWndParticleModify = NULL;
	m_dwAnimOnceLastTime = 0;

	m_bLightEnable = false;

	ZeroMemory(m_szPathNameTmp, _MAX_PATH);
	ZeroMemory(m_szViewframeDis, 20);
	m_fYRot = 0.0f;
	ZeroMemory(m_bKeyState, sizeof(bool) * 256);

	m_dwAnimTimeLength = 0;
	m_dwAnimFrameIndex = 0;
	m_dwAnimFPS		   = 0;
	m_szpFileFormat	   = NULL;
	m_IFF			   = D3DXIFF_PNG;
	m_pOldRenderTarget = NULL;
	m_pOldZStencil	   = NULL;
	m_pRenderTexture   = NULL;
	m_pDepthStencilSurface = NULL;
	TargetPos = D3DXVECTOR3(0,0,0);
}

WndViewer::~WndViewer(void)
{
}

BOOL WndViewer::Create(const char *pszName, const RECT *pRect, DWORD dwStyle, ui::Wnd *pParent)
{
	/////////////////////////////////////////////////
	//==鼠标=-=
	render::Interface *pInterface = render::Interface::GetInstance();
	m_hCursorHandle = pInterface->CreateTexture("ui/cursor.psd");
	/////////////////////////////////////////////////
	m_pFrustum = new render::Frustum;
	VERIFY(m_pFrustum);
	/////////////////////////////////////////////////
	int   iWidth = pInterface->GetWndWidth();
	int   iHeight= pInterface->GetWndHeight();
	RECT rcWnd = { 0,0,iWidth,iHeight};
	ui::Wnd::Create(pszName, &rcWnd,FWS_VISIBLE, NULL); 
	/////////////////////////////////////////////////
	m_pCamera = new render::Camera;
	VERIFY(m_pCamera);
	float aspect = (float)iWidth/(float)iHeight;
	m_pCamera->SetAspect(aspect);
	m_pCamera->SetFOVY(D3DXToRadian(60));
	m_pCamera->SetNearClipPlane(0.01f);
	m_pCamera->SetFarClipPlane(1000);
	ResetCamera();
	////////////////////////////////////////////////
	// 菜单
	CreateMenuBar();

	// 创建快捷菜单
	m_pQuickMenu = new ui::QuickMenu;
	VERIFY(m_pQuickMenu);
	RECT rc = { 0, 0, 10, 10 };
	m_pQuickMenu->Create("QuickMenu", &rc, FWS_OUTLINE | FWS_FILLBKG, this);
	RECT rectQ = { 0, 0, 65, 20 };
	m_pQuickMenu->AddItem("主模型", &rectQ);		//0
	m_pQuickMenu->AddItem("绑定模型", &rectQ);		//1
	m_pQuickMenu->AddItem("设定地表", &rectQ);		//2
	m_pQuickMenu->AddItem("设定背景", &rectQ);		//3
	m_pQuickMenu->AddItem("参考模型", &rectQ);		//4

	RECT rcBK = { 0, 0, 210, 100 };
	m_pWndEditBKColor = new WndEditBGColor;
	m_pWndEditBKColor->Create("背景调整",&rcBK,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW, this);
	m_pWndEditBKColor->CenterWndTo(iWidth/2,iHeight/2);
	m_pWndEditBKColor->ShowWnd(FALSE);

	RECT rcLight = { 0, 0, 210, 100 };
	m_pWndEditLightColor = new WndEditLightColor;
	m_pWndEditLightColor->Create("灯光调整",&rcLight,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW, this);
	m_pWndEditLightColor->CenterWndTo(iWidth/2,iHeight/2);
	m_pWndEditLightColor->ShowWnd(FALSE);

	// 热键提示窗体
	RECT rcHK = { 0, 0, 300, 293 };
	m_pWndHotkeyList = new WndHotkey;
	m_pWndHotkeyList->Create("热键列表", &rcHK, 0, this);
	m_pWndHotkeyList->CenterWndTo(iWidth / 2, iHeight / 2);

	// 关于窗体
	RECT rcAbout = {0, 0, 300, 500};
	m_pWndAbout = new WndAbout;
	m_pWndAbout->Create("模型预览工具",&rcAbout,0,this);
	m_pWndAbout->CenterWndTo(iWidth/2,iHeight/2);

	// 动作选择对话框
	RECT rcAction = { 920, 25, 920 + 100, 25 + 377 };
	m_pWndAction = new WndAction;
	m_pWndAction->Create("模型动作选择",&rcAction,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW,this);
	m_pWndAction->ShowWnd(false);

	// 扩展部件列表
	RECT rcPartList = { 810, 25, 810 + 100, 25 + 377 };
	m_pWndPart = new WndPart;
	m_pWndPart->Create("部件显示", &rcPartList, FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW | FWS_DRAG, this);
	m_pWndPart->ShowWnd(false);

	// 动画调整对话框
	RECT rcAnimMapDlg = { 885, 405, 1026, 436 };
	m_pWndAnimModify = new WndAnimModify;
	m_pWndAnimModify->Create("时间调整",&rcAnimMapDlg,FWS_VISIBLE,this);
	m_pWndAnimModify->ShowWnd(false);

	// 地表移动速度调整对话框
	RECT rcGridMovingSpeedDlg = { 885, 467, 1026, 498 };
	m_pWndGridMovingSpeedModify = new WndGridMovingSpeedModifyDlg;
	m_pWndGridMovingSpeedModify->Create("移动速度调整", &rcGridMovingSpeedDlg, FWS_VISIBLE, this);
	m_pWndGridMovingSpeedModify->ShowWnd(false);

	// 绑定模型列表框
	RECT rcBindModel = { 2, 25, 2 + 100, 25 + 303 };
	m_pWndModels = new WndModel;
	m_pWndModels->Create("载入模型列表", &rcBindModel, FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW, this);
	m_pWndModels->ShowWnd(false);

	// 绑定点选择框
	RECT rcLocator = { 105, 25, 105 + 100, 25 + 303 };
	m_pWndLocator = new WndLocator;
	m_pWndLocator->Create("绑定点选择", &rcLocator, FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW | FWS_DRAG, this);
	m_pWndLocator->AddLocator("胸口");
	m_pWndLocator->AddLocator("头顶");
	m_pWndLocator->AddLocator("脚底中心");
	m_pWndLocator->AddLocator("左手");
	m_pWndLocator->AddLocator("右手");
	m_pWndLocator->AddLocator("左脚");
	m_pWndLocator->AddLocator("右脚");
	m_pWndLocator->AddLocator("背");

	m_pWndLocator->AddLocator("左腰");
	m_pWndLocator->AddLocator("右腰");
	m_pWndLocator->AddLocator("左臂");
	m_pWndLocator->AddLocator("右臂");
	m_pWndLocator->AddLocator("左后背");
	m_pWndLocator->AddLocator("右后背");
	m_pWndLocator->AddLocator("后背长剑");
	m_pWndLocator->AddLocator("后背弓");

	m_pWndLocator->AddLocator("后背箭袋");
	m_pWndLocator->AddLocator("后背杖,斧");
	m_pWndLocator->AddLocator("左拳");
	m_pWndLocator->AddLocator("右拳");
	m_pWndLocator->AddLocator("左受伤点");
	m_pWndLocator->AddLocator("右受伤点");
	m_pWndLocator->AddLocator("抱人点");
	m_pWndLocator->AddLocator("坐标原点");
	m_pWndLocator->ShowWnd(false);

	// 级别选择对话框
	RECT rcLevel = { 920, 447, 920 + 100, 447 + 158 };
	m_pWndSelectLevel = new WndSelectLevel;
	m_pWndSelectLevel->Create("级别选择", &rcLevel, FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW, this);
	m_pWndSelectLevel->ShowWnd(false);

	// 动画帧导出对话框
	RECT rcAnimFrame  = { 0, 0, 300, 280 };
	m_pWndAnimFrameEx = new WndAnimFrameEx;
	m_pWndAnimFrameEx->Create("动画帧导出", &rcAnimFrame, FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW, this);
	m_pWndAnimFrameEx->CenterWndTo(iWidth / 2, iHeight / 2);
	m_pWndAnimFrameEx->ShowWnd(false);

	// 进度条窗口
	RECT rcGuage = { 0, 0, 300, 100 };
	m_pWndGuage = new WndGuage;
	m_pWndGuage->Create("进度条", &rcGuage, FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW, this);
	m_pWndGuage->CenterWndTo(iWidth / 2, iHeight / 2);
	m_pWndGuage->ShowWnd(false);

	//模型纹理树形窗口
	RECT rcTexture = { 451, 25, 451 + 240, 25 + 430 };
	m_pWndModelTexture = new WndModelTexture;
	m_pWndModelTexture->Create("纹理表",&rcTexture,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW,this);
	m_pWndModelTexture->ShowWnd(false);

	//模型Shader修改窗口
	RECT rcShader = { 694, 25, 694 + 180, 25 + 60};
	m_pWndShaderModify = new WndShaderModify;
	m_pWndShaderModify->Create("Shader修改",&rcShader, FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW  , this);
	m_pWndShaderModify->ShowWnd(false);
	
	//模型树形窗口
	RECT rcModel = {  208, 25, 208 + 240, 25 + 430};
	m_pWndModelTree = new WndModelTree;
	m_pWndModelTree->Create("Model修改",&rcModel, FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW  , this);
	m_pWndModelTree->ShowWnd(false);

	//模型part树形窗口
	RECT rcPart = { 451, 25, 451 + 280, 25 + 430};
	m_pWndPartTree = new WndPartTree;
	m_pWndPartTree->Create("Part修改",&rcPart, FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW  , this);
	m_pWndPartTree->ShowWnd(false);

	//模型model文件修改窗口
	RECT rcModelModify = {451, 25, 451 + 220, 25 + 150};
	m_pWndModelModify = new WndModelModify;
	m_pWndModelModify->Create("Model修改",&rcModelModify , FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW , this);
	m_pWndModelModify->ShowWnd(false);

	//part文件修改窗口
	RECT rcPartModify = {734, 25, 734 + 184, 25 + 150};
	m_pWndPartModify = new WndPartModify;
	m_pWndPartModify->Create("Part修改",&rcPartModify , FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW , this);
	m_pWndPartModify->ShowWnd(false);

	//trail树形窗口
	RECT rcTrailTreeModify = { 451, 25, 451 + 240, 25 + 430};
	m_pWndTrailTree = new WndTrailTree;
	m_pWndTrailTree->Create("TrailTree",&rcTrailTreeModify ,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW , this);
	m_pWndTrailTree->ShowWnd(false);

	//trail修改窗口
	RECT rcTrailModify = {694, 25, 694 + 200, 25 + 150};
	m_pWndTrailModify = new WndTrailModify;
	m_pWndTrailModify->Create("Trail修改",&rcTrailModify  ,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW , this);
	m_pWndTrailModify->ShowWnd(false);

	//Particle树形窗口
	RECT rcParticleTree = {451, 25, 451 + 240, 25 + 430};
	m_pWndParticleTree = new WndParticleTree;
	m_pWndParticleTree->Create("ParticleTree",&rcParticleTree  ,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW , this);
	m_pWndParticleTree->ShowWnd(false);

	//Particle修改窗口
	RECT rcParticleModify = {694, 25, 694 + 200, 25 + 150};
	m_pWndParticleModify = new WndParticleModify;
	m_pWndParticleModify->Create("ParticleModify",&rcParticleModify,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW , this);
	m_pWndParticleModify->ShowWnd(false);
	// 灯光模型
	m_pSunLight = new SunLight;

	ShowWindow(pInterface->GetWndHandle(), SW_SHOW);

	return TRUE;
}

void WndViewer::Destroy()
{
	// 销毁模型
	m_pModel = NULL;
	if (m_pRefModel)
	{
		CGameModelManager::GetInstance()->ReleaseDisplayModel(m_pRefModel);
		CGameModelManager::GetInstance()->ReleaseGameModel(m_pRefModel->GetGameModelID());
		m_pRefModel = NULL;
	}
	for (int i = 0; i < (int)m_vRoleModels.size(); ++i)
	{
		CDisplayModel * pModel = m_vRoleModels[i]->GetDisplayModel();
		CGameModelManager::GetInstance()->ReleaseDisplayModel(pModel);
		CGameModelManager::GetInstance()->ReleaseGameModel(pModel->GetGameModelID());

		SAFEDELETE(m_vRoleModels[i]);
		pModel = NULL;
	}
	m_vRoleModels.clear();

	for (int i = 0; i < (int)m_vBindModels.size(); ++i)
		SAFEDELETE(m_vBindModels[i]);
	m_vBindModels.clear();

	SAFEDELETE(m_pSunLight);
	SAFEDESTROY(m_pWndAction);
	SAFEDESTROY(m_pWndPart);
	SAFEDESTROY(m_pWndModels);
	SAFEDESTROY(m_pWndLocator);
	SAFEDESTROY(m_pWndSelectLevel);
	SAFEDESTROY(m_pWndAnimModify);
	SAFEDESTROY(m_pWndGridMovingSpeedModify);
	SAFEDESTROY(m_pWndEditBKColor);
	SAFEDESTROY(m_pWndEditLightColor);
	SAFEDESTROY(m_pWndHotkeyList);
	SAFEDESTROY(m_pWndAbout);
	SAFEDESTROY(m_pQuickMenu);
	SAFEDESTROY(m_pWndAnimFrameEx);
	SAFEDESTROY(m_pWndGuage);
	SAFEDESTROY(m_pWndModelTexture);
	SAFEDESTROY(m_pWndShaderModify);
	SAFEDESTROY(m_pWndModelTree);
	SAFEDESTROY(m_pWndPartTree);
	SAFEDESTROY(m_pWndModelModify);
	SAFEDESTROY(m_pWndPartModify);
	SAFEDESTROY(m_pWndTrailTree);
	SAFEDESTROY(m_pWndTrailModify);
	SAFEDESTROY(m_pWndParticleTree);
	SAFEDESTROY(m_pWndParticleModify);
	DestroyMenuBar();

	render::Interface * pInterface = render::Interface::GetInstance();
	pInterface->DestroyTexture(m_hCursorHandle);
	pInterface->DestroyTexture(m_dwBK);

	SAFEDELETE(m_pCamera);
	SAFEDELETE(m_pFrustum);
	
	ui::Wnd::Destroy();
}

void WndViewer::Update()
{
	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	//---SELF
	ui::Manager *pUI = ui::Manager::GetInstance();
	
	BOOL &bActive = m_tWindowInfo.bActive;
	ui::Manager::_tagINPUTINFO *pInputInfo = pUI->GetInputInfoBuffer();

	if (bActive)
	{
		DWORD dwMBS = pInputInfo->dwMouseButtonState;
		//===============camera control======================
		if (dwMBS & MK_RBUTTON && !(pInputInfo->byKeyBuffer[DIK_LALT] & 0x80))
		{
			m_eCamOpType = CCO_SCALE;
		}else if (dwMBS & MK_MBUTTON)
		{
			m_eCamOpType = CCO_MOVE;
		}else if (dwMBS & MK_LBUTTON && !(pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80))
		{
			m_eCamOpType = CCO_ROTATE;
		}else
			m_eCamOpType = CCO_NONE;

		if (pInputInfo->eType == UIMT_MS_MOVE)
		{
			//Camerat Action
			float cx = 0.01f * (float)((pInputInfo->ptMouse.x - pInputInfo->ptMouseOld.x));
			float cy = 0.01f * (float)((pInputInfo->ptMouse.y - pInputInfo->ptMouseOld.y));

			switch(m_eCamOpType)
			{
			case CCO_NONE:
				break;
			case CCO_ROTATE:
				m_pCamera->RotatePosition(&D3DXVECTOR3(cy,cx,0));    
				break;
			case CCO_SCALE:
				m_pCamera->MovePosition(&D3DXVECTOR3(0,0,cx));
				break;
			case CCO_MOVE:
				//m_pCamera->FlatMoveFB(cy);
                //m_pCamera->FlatMoveLR(-cx);
				m_pCamera->MovePosition(&D3DXVECTOR3(-cx,cy,0));
				m_pCamera->MoveView(&D3DXVECTOR3(-cx,cy,0));				
				break;
			}
		}

		if (dwMBS & MK_LBUTTON && (pInputInfo->byKeyBuffer[DIK_LCONTROL] & 0x80))
		{
			float cx = -0.01f * (float)((pInputInfo->ptMouse.x - pInputInfo->ptMouseOld.x));
			float cy =  0.01f * (float)((pInputInfo->ptMouse.y - pInputInfo->ptMouseOld.y));

			m_vRolePos.x += cy;
			m_vRolePos.y += cx;
		}
	}

	if (pInputInfo->eType == UIMT_MS_WHEEL)
	{
		float fzDelta = (float)((short)HIWORD(pInputInfo->dwData));
		m_pCamera->MovePosition(&D3DXVECTOR3(0,0,fzDelta * 0.01f));
	}

	// 判断窗体内右键事件
	/*if (pInputInfo->dwData == MK_RBUTTON)
		SetQuickMenuVisibility(false);			//取消快捷菜单显示*/

	m_pSunLight->HandleMessage(m_pCamera->GetViewMatrix());

	// 热键
	HotkeyProc();

	// 菜单
	MenuBarProc();

	// 快捷菜单事件处理
	QuickMenuProc();

	// 弹出窗口
	if (m_pWndEditBKColor->CheckPopUp())
	{
		if (m_pWndEditBKColor->IsOK())
			Setting::GetInstance()->SetBackGroundColor(m_pWndEditBKColor->GetBKColor());		//调整光照数据
	}

	if (m_pWndEditLightColor->CheckPopUp())
	{
		if (m_pWndEditLightColor->IsOK())
			Setting::GetInstance()->SetLightColor(m_pWndEditLightColor->GetLightColor());
	}

	if (m_pWndSelectLevel->IsSelLevelChange())
	{
		//切换级别
		DWORD dwGroup = m_pWndSelectLevel->GetGroupName();
		DWORD dwPart  = m_pWndSelectLevel->GetPartName();

		if (m_eLevelModelType == MT_ROLE)
			m_pModel->ShowGroup(dwGroup, dwPart, m_pWndSelectLevel->GetLevel());
		else if (m_eLevelModelType == MT_BIND)
		{
			CDisplayModel * pDisplayModel = m_vBindModels[m_iLevelModelIndex]->GetDisplayModel();
			pDisplayModel->ShowGroup(dwGroup, dwPart, m_pWndSelectLevel->GetLevel());
		}
	}

	if (m_pWndAnimModify->GetStyle() & FWS_VISIBLE)
		m_pWndAnimModify->SetCurTime(m_cAnimInfo.GetActionInfo()->dwCurActionElapseTime);

	// 处理导出动画
	if (m_pWndAnimFrameEx->IsStartExAnimFrame())
	{
		WIN32_FIND_DATA fdt;
		if (FindFirstFile(m_pWndAnimFrameEx->GetAnimFrameSaveDir(), &fdt) != INVALID_HANDLE_VALUE)
		{
			if (m_pWndAnimFrameEx->CheckInputSize())
			{
				m_szpFileFormat = m_pWndAnimFrameEx->GetFileFormat();
				if (strcmp(m_szpFileFormat, ".bmp") == 0)
					m_IFF = D3DXIFF_BMP;
				else if (strcmp(m_szpFileFormat, ".jpg") == 0)
					m_IFF = D3DXIFF_JPG;
				else if (strcmp(m_szpFileFormat, ".tga") == 0)
					m_IFF = D3DXIFF_TGA;
				else
					m_IFF = D3DXIFF_PNG;

				m_dwAnimFPS = m_pWndAnimFrameEx->GetFPS();
				if (m_dwAnimFPS != 0)
				{
					m_bExAnimFrame	   = true;
					m_dwAnimTimeLength = m_pWndAnimModify->GetTimeLength();
					m_dwAnimFrameIndex = 0;
					m_cAnimInfo.GetActionInfo()->dwCurActionElapseTime = 0;

					m_pWndGuage->SetPercent(0.0f);
					m_pWndGuage->BeginPopUp();
				}else
					MessageBox(NULL, "请选择正确的帧率!", "提示", MB_OK);
			}else
				MessageBox(NULL, "请输入正确的导出尺寸!(不能超过当前程序窗体尺寸)!", "提示", MB_OK);
		}else
			MessageBox(NULL, "动画帧的保存目录有误!", "提示", MB_OK);
	}

	// 取消导出动画
	if (m_pWndGuage->IsCancel())
	{
		m_bExAnimFrame = false;
		m_pWndGuage->EndPopUp();
	}

	// 处理取景器
	if (m_bShowViewfinder)
	{
		m_ViewfinderRay.CalculateRay(m_pCamera);

		if (m_pModel)
		{
			bool		bIsIntersect  = false;
			float		fIntersectDis = 10000.0f;
			float		fDisCompare   = 0.0f;
			D3DXVECTOR3 vIntersectPos;
			CDisplayModel::mapDisplayGroups & DisplayGroups = m_pModel->GetGroups();

			CDisplayModel::itDisplayGroup it = DisplayGroups.begin();
			for (; it != DisplayGroups.end(); ++it)
			{
				CDisplayModel::CDisplayGroup * pDG = (*it).second;

				if (pDG->GetLevelInfo() == NULL)
					break;

				GameModel::Group::Part::LevelMeshes & vRenderMeshes = pDG->GetLevelInfo()->GetRMArray();
				for(int i = 0; i < pDG->GetLevelInfo()->GetRMCount(); ++i)
				{
					GameModel::Group::Part::tagRenderMesh * pRenderMesh = vRenderMeshes[i];
					if (pRenderMesh->GetLoadState() != Load_Did)
						continue;

					model::Buffer * pBuffer = pRenderMesh->pBuffer;
					D3DXVECTOR3 * apPos = pBuffer->GetPosBuffer();
					const model::Mesh::tagTriangle * pTriangle;
					const model::Mesh::tagTriangle * pTriangleBuffer = pBuffer->GetAttachMesh()->GetTriangleBuffer();
					DWORD dwTriangleCount = pBuffer->GetAttachMesh()->GetTriangleCount();

					D3DXVECTOR3 vTri[3];

					for (DWORD dw = 0; dw < dwTriangleCount; ++dw)
					{
						pTriangle = &pTriangleBuffer[dw];

						vTri[0] = apPos[pTriangle->dwPNCIndex[0]];
						vTri[1] = apPos[pTriangle->dwPNCIndex[1]];
						vTri[2] = apPos[pTriangle->dwPNCIndex[2]];

						if (m_ViewfinderRay.IntersectTri(vTri, &vIntersectPos, &fDisCompare))
						{
							if (fDisCompare < fIntersectDis)
							{
								bIsIntersect  = true;
								fIntersectDis = fDisCompare;
								//memcpy(g_vTri, vTri, sizeof(D3DXVECTOR3) * 3);
							}
						}
					}
				}
			}

			if (bIsIntersect)
				sprintf(m_szViewframeDis, "%.4f", fIntersectDis);
			else
				sprintf(m_szViewframeDis, "+ ∞");
		}else
			sprintf(m_szViewframeDis, "+ ∞");
	}

	//update frustum 
	m_pFrustum->CalculateFrustum(m_pCamera->GetViewMatrix(),m_pCamera->GetProjectionMatrix());

	//如果鼠标左键点击在Shader窗口以外的区域 则关掉窗口
	if(pInputInfo->eType == UIMT_MS_BTNDOWN && pInputInfo->dwData & MK_LBUTTON)
	{
		RECT rect;
		m_pWndShaderModify->GetWndRect(&rect);
		if(!(::PtInRect(&rect, pInputInfo->ptMouse)))
		{
			m_pWndShaderModify->ShowWnd(false);
		}
		m_pWndModelTexture->GetWndRect(&rect);
		if(!(::PtInRect(&rect, pInputInfo->ptMouse)) && !(m_pWndShaderModify->IsVisible()))
		{
			m_pWndModelTexture->ShowWnd(false);
		}
		m_pWndPartTree->GetWndRect(&rect);
		if(!(::PtInRect(&rect, pInputInfo->ptMouse)) && !(m_pWndPartModify->IsVisible()))
		{
			m_pWndPartTree->ShowWnd(false);
		}
		m_pWndModelModify->GetWndRect(&rect);
		if(!(::PtInRect(&rect, pInputInfo->ptMouse)))
		{
			m_pWndModelModify->ShowWnd(false);
		}
		m_pWndPartModify->GetWndRect(&rect);
		if(!(::PtInRect(&rect, pInputInfo->ptMouse)))
		{
			m_pWndPartModify->ShowWnd(false);
		}
		m_pWndTrailTree->GetWndRect(&rect);
		if (!(::PtInRect(&rect, pInputInfo->ptMouse)) && !(m_pWndTrailModify->IsVisible()))
		{
			m_pWndTrailTree->ShowWnd(false);
		}
		m_pWndTrailModify->GetWndRect(&rect);
		if (!(::PtInRect(&rect, pInputInfo->ptMouse)))
		{
			m_pWndTrailModify->ShowWnd(false);
		}
		m_pWndParticleTree->GetWndRect(&rect);
		if (!(::PtInRect(&rect, pInputInfo->ptMouse)) && !(m_pWndParticleModify->IsVisible()))
		{
			m_pWndParticleTree->ShowWnd(false);
		}
		m_pWndParticleModify->GetWndRect(&rect);
		if (!(::PtInRect(&rect, pInputInfo->ptMouse)))
		{
			m_pWndParticleModify->ShowWnd(false);
		}
	}
	ui::Wnd::UpdateChild();
}
void WndViewer::GetBuffer(int x,int z,DWORD size,D3DXVECTOR3* posbuff)
{
	DWORD nowgridnum = 0;
	int i = 0;
	int j = 0;

	if (size < 1)
	{
		size = 1;
	}

	for (i = x - ((int)size - 1);i <= x + ((int)size - 1);i++)
	{
		for (j = z - ((int)size - 1);j <= z + ((int)size - 1);j++)
		{
			posbuff[nowgridnum * 6 + 0] = D3DXVECTOR3((float)i * 1.0f - 0.5f,0.0f,(float)j * 1.0f - 0.5f);
			posbuff[nowgridnum * 6 + 1] = D3DXVECTOR3((float)i * 1.0f - 0.5f,0.0f,(float)j * 1.0f + 0.5f);
			posbuff[nowgridnum * 6 + 2] = D3DXVECTOR3((float)i * 1.0f + 0.5f,0.0f,(float)j * 1.0f - 0.5f);
			posbuff[nowgridnum * 6 + 3] = D3DXVECTOR3((float)i * 1.0f + 0.5f,0.0f,(float)j * 1.0f - 0.5f);
			posbuff[nowgridnum * 6 + 4] = D3DXVECTOR3((float)i * 1.0f - 0.5f,0.0f,(float)j * 1.0f + 0.5f);
			posbuff[nowgridnum * 6 + 5] = D3DXVECTOR3((float)i * 1.0f + 0.5f,0.0f,(float)j * 1.0f + 0.5f);
			nowgridnum++;
		}
	}
}
void WndViewer::DisplayTextureProjective()
{
	CTextureProjective::SetState();
	std::list<CTextureProjective*> texprojlist = CTextureProjective::GetAllCTextureProjectiveList();
	std::list<CTextureProjective::SHADOWTEXTURE> shadowlist = CTextureProjective::GetShadowTextureList();
	//渲染投影纹理
	if (!texprojlist.empty())
	{
		CTextureProjective::SetTextureState();
		CTextureProjective* p;
		for(std::list<CTextureProjective*>::iterator it = texprojlist.begin();it != texprojlist.end();)
		{
			p = (CTextureProjective*)(*it);
			if (p->IsAlive())
			{
				for (std::list<CTextureProjective::POSDIRALPHATIME>::iterator it = p->m_listpostime.begin();it != p->m_listpostime.end();it++)
				{
					p->SetLookatPos(&(it->pos));
					p->SetNowDirY((it->fdiry));
					p->SetNowTime(&(it->time));
					p->SetNowAlpha(it->falpha);
					GetBuffer((int)p->GetPosX(),(int)p->GetPosZ(),p->GetSize(),CTextureProjective::GetBuffer());
					D3DXMATRIX mat;
					D3DXMatrixIdentity(&mat);
					p->Rend(m_pCamera->GetPosition(),mat,*m_pCamera->GetViewMatrix(),*m_pCamera->GetProjectionMatrix());
				}
				p->m_listpostime.clear();
			}
			else
			{
				it++;
				CTextureProjective::DestroyTextureProjctive(p);
				continue;
			}
			it++;

		}	
	}
	CTextureProjective::EndState();

}

void WndViewer::Render(void)
{
	render::Interface		   * pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D * pLayer2D   = pInterface->GetLayer2D();
	pInterface->Clear(Setting::GetInstance()->GetBackGroundColor());
	ui::Wnd::SetUIRendState();
	ui::Wnd::RenderDefault();

	render::Interface::Layer3D * pLayer3D = pInterface->GetLayer3D();
	DWORD dwFillMode = (m_bDisplayWireFrame) ? D3DFILL_WIREFRAME : D3DFILL_SOLID;
	pLayer3D->SetFillMode(dwFillMode);

	SetupTransform();

	// 处理动画导出
	if (m_bExAnimFrame)
	{
		DWORD dwCurActionElapseTime = m_cAnimInfo.GetActionInfo()->dwCurActionElapseTime;
		if (dwCurActionElapseTime < m_dwAnimTimeLength)
		{
			ExportAnimationFrame();
			m_pWndGuage->SetPercent((float)dwCurActionElapseTime / (float)m_dwAnimTimeLength);
			m_pWndGuage->Render();
		}else
		{
			m_bExAnimFrame = false;
			m_pWndGuage->EndPopUp();
		}
	}else
	{
		// 绘制图片背景
		if (m_dwBK != 0 && m_bSwitchBK)
		{
			// 计算绘制坐标(居中显示)
			static int iDrawX = (pInterface->GetWndWidth() >> 1) - (m_ptBKTexInfo->pTexture->GetWidth() >> 1);
			static int iDrawY = (pInterface->GetWndHeight() >> 1) - (m_ptBKTexInfo->pTexture->GetHeight() >> 1);

			pLayer2D->DrawTexture(iDrawX, iDrawY, m_dwBK);
		}

		ShowGrid();

		//======draw model=====
		if (!m_vRoleModels.empty())
		{
			int iSelIndex = m_pWndModels->GetSelRoleModel();
			m_pModel = m_vRoleModels[iSelIndex]->GetDisplayModel();
		}

		if (m_pModel&&m_pModel->GetLoadState() == Load_Did)
		{
			//D3DXMATRIX matscal;
			//D3DXMatrixScaling(&matscal,50,50,50);
			//m_cAnimInfo.SetScalMatrix(&matscal);
			m_cAnimInfo.SetupAnimInfo(m_vRolePos.x, m_vRolePos.y, 0.0f, 0, m_pCamera);
			
			if (m_bLightEnable)
			{
				m_pModel->SetDirLightEnable(true);

			}
			else
			{
				m_pModel->SetDirLightEnable(false);
			}
			//m_pModel->SetUseNormal(true);
			m_cAnimInfo.SetViewMatrix(m_pCamera->GetViewMatrix());
			m_cAnimInfo.SetProjectedMatrix(m_pCamera->GetProjectionMatrix());
			m_pModel->ProcessAnimJoint(&m_cAnimInfo);
			m_pModel->ProcessVisibility(&m_cAnimInfo);
			m_pModel->ProcessAnimUV(&m_cAnimInfo);
			m_pModel->ProcessAnimTexture(&m_cAnimInfo);
			m_pModel->ProcessAnimColor(&m_cAnimInfo);
			m_pModel->ProcessLocator(&m_cAnimInfo);
			m_pModel->ProcessParticleEmitter(&m_cAnimInfo);
			m_pModel->ProcessTrailEmitter(&m_cAnimInfo);
			m_pModel->ProcessEnvironmentMaps(m_pCamera->GetViewMatrix());
			m_pModel->ProcessTextureProjective(&m_cAnimInfo);
			m_pModel->ProcessWater(&m_cAnimInfo);

			if (m_pModel->GetWater() == NULL)
			{
				if (m_bLightEnable)
				{
					m_pModel->SetModelColor(0xff000000);
					m_pModel->MultiplyModelColor(0xffffffff);
				}
				//m_pModel->SetRendSimpleMesh(true);
//				m_pModel->DrawModel();
				m_pModel->RenderModel();
			}

			if (m_bShowNormal)
				m_pModel->_DEBUG_RenderNormal();
			if (m_bShowLocator)
				m_pModel->_DEBUG_RenderLocator();
			if (m_bShowSizeBox)
				m_pModel->_DEBUG_RenderSizeBox(m_cAnimInfo.GetWorldMatrix());
			if (m_bShowPickBox)
				m_pModel->_DEBUG_RenderPickBox(m_cAnimInfo.GetWorldMatrix());
			if (m_bShowBone)
				m_pModel->_DEBUG_RenderAction(&m_cAnimInfo);

			if (!m_vBindModels.empty())
			{
				for (int i = 0; i < (int)m_vBindModels.size(); ++i)
					m_vBindModels[i]->Display(m_pModel, &m_cAnimInfo);
			}
		}

		if (m_pRefModel)
		{
			D3DXMATRIX matWorld;
			D3DXMatrixIdentity(&matWorld);
			D3DXMatrixRotationY(&matWorld, m_fYRot);
			static AnimInfo cAnimInfo;

			cAnimInfo.GetActionInfo()->dwCurAction = MAKEFOURCC('S','T','N','D');
			cAnimInfo.GetActionInfo()->bCurActionLooped = TRUE;
			cAnimInfo.SetCurrentTime(timeGetTime());
			cAnimInfo.SetWorldMatrix(&matWorld);
			cAnimInfo.SetViewMatrix(m_pCamera->GetViewMatrix());
			cAnimInfo.SetProjectedMatrix(m_pCamera->GetProjectionMatrix());
			cAnimInfo.SetBillboard(m_pCamera->GetBillboard());
			cAnimInfo.SetBillboardY(m_pCamera->GetBillboardY());

			m_pRefModel->ProcessAnimJoint(&cAnimInfo);
			m_pRefModel->ProcessVisibility(&cAnimInfo);
			m_pRefModel->ProcessAnimUV(&cAnimInfo);
			m_pRefModel->ProcessAnimTexture(&cAnimInfo);
			m_pRefModel->ProcessAnimColor(&cAnimInfo);

			m_pRefModel->ProcessLocator(&cAnimInfo);
			m_pRefModel->ProcessParticleEmitter(&cAnimInfo);
			m_pRefModel->ProcessTrailEmitter(&cAnimInfo);
			m_pRefModel->ProcessTextureProjective(&cAnimInfo);

			if (m_bLightEnable)
			{
				m_pRefModel->SetModelColor(m_dwAmbientColor);
				D3DXVECTOR3 vL = m_pSunLight->GetLightDirection();
				D3DXVec3Normalize(&vL,&vL);
				m_pRefModel->ProcessDirectionalLight(m_pCamera->GetPosition(),&vL,Setting::GetInstance()->GetLightColor());
			}
			else
				m_pRefModel->SetModelColor(0xFFFFFFFF);

			m_pRefModel->RenderModel();
		}
		ui::Wnd::SetUIRendState();
		D3DXMATRIX mView;
		D3DXVECTOR3 vPos  = *m_pCamera->GetPosition() - *m_pCamera->GetView();
		D3DXVec3Normalize(&vPos,&vPos);
		D3DXVECTOR3 vView = D3DXVECTOR3(0,0,0);
		vPos *= 7.5f;
		D3DXMatrixLookAtLH(&mView,&vPos,&vView,&D3DXVECTOR3(0.0f,1.0f,0.0f));
		pInterface->GetDevice()->SetTransform(D3DTS_VIEW,&mView);
		D3DVIEWPORT9 tViewPort,tViewPortOld;
		tViewPort.X = 0;
		tViewPort.Y = 500;
		tViewPort.Width = 130;
		tViewPort.Height= 100;
		tViewPort.MinZ = .0f;
		tViewPort.MaxZ = 1.0f;
		pInterface->GetDevice()->GetViewport(&tViewPortOld);
		pInterface->GetDevice()->SetViewport(&tViewPort);
		ShowAxis();
		pInterface->GetDevice()->SetViewport(&tViewPortOld);

		SetupTransform();
		if (m_bShowLightArrow && m_bShowUI)
			m_pSunLight->Render();
	
		// 粒子渲染
		particle::Manager::GetInstance()->Render();
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
		vecDir = m_pSunLight->GetLightDirection();
		D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
		light.Range       = 20.0f;
		light.Falloff = 0;
		light.Attenuation0 = 1.0f;
		light.Attenuation1 = 0.0f;
		light.Attenuation2 = 0.0f;
		light.Position =  TargetPos;
		//pInterface->GetLayer3D()->_DEBUG_DrawWireBox(&TargetPos,0.05f,0xffff0000);
		D3DMATERIAL9 mtrl;
		ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
		mtrl.Diffuse.r = 1.0f;
		mtrl.Diffuse.g = 1.0f;
		mtrl.Diffuse.b = 1.0f;
		mtrl.Diffuse.a = 1.0f;
		mtrl.Ambient.r = 0.5f;
		mtrl.Ambient.g = 0.5f;
		mtrl.Ambient.b = 0.5f;
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
		pLayer3D->SetAmbient(0xff0000);
		pLayer3D->SetMipMap(TRUE);
		pLayer3D->SetFogEnable(FALSE);
		pLayer3D->RenderTest();
		pLayer3D->RenderBlendLow();
		pLayer3D->EndEffectsState();
		DisplayTextureProjective();
		Water::CManager::GetInstance()->Render(FALSE);
		pLayer3D->RenderBlend();
		pLayer3D->Clear();
		ui::Wnd::SetUIRendState();

		// 绘制取景器
		if (m_bShowViewfinder)
		{
			static int iViewfinderX = (pInterface->GetWndWidth() >> 1) - (100 >> 1);
			static int iViewfinderY = (pInterface->GetWndHeight() >> 1) - (100 >> 1);
			pLayer2D->DrawWireQuad(iViewfinderX, iViewfinderY, 100, 100, 0xff00ff00);
			pLayer2D->DrawLine(iViewfinderX, iViewfinderY, iViewfinderX + 100, iViewfinderY + 100, 0xffff0000);
			pLayer2D->DrawLine(iViewfinderX + 99, iViewfinderY, iViewfinderX - 1, iViewfinderY + 100, 0xffff0000);

			pLayer2D->DrawText(iViewfinderX, iViewfinderY - 12, m_szViewframeDis);

//			pInterface->GetLayer3D()->_DEBUG_DrawTriangle(&g_vTri[0], &g_vTri[1], &g_vTri[2], 0xffffffff);
		}

		ShowFps();
		ShowInfo();

		if (m_bShowUI)
			ui::Wnd::RenderChild();
	}

	ShowCursor();
}

int WndViewer::GetFps(void)
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

void WndViewer::CreateMenuBar(void)
{
	//建立三个	
	RECT rc;
	m_pMenuBar = new ui::MenuBar;
	SetRect(&rc,2,2,render::Interface::GetInstance()->GetWndWidth()-4,2+20);
	m_pMenuBar->Create("Menu", &rc, FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG, this);

	//文件菜单 0
	DWORD dwFileMenu = m_pMenuBar->AddMenu("File");
	m_pMenuBar->AddMenuItem(dwFileMenu,"打开角色模型  Ctrl+O");		//0
	m_pMenuBar->AddMenuItem(dwFileMenu,"打开绑定模型  Ctrl+B");		//1
	m_pMenuBar->DisableMenuItem(dwFileMenu,m_pMenuBar->AddMenuItem(dwFileMenu,"──────────"));
	m_pMenuBar->AddMenuItem(dwFileMenu,"打开参考模型  Ctrl+R");		//3
	m_pMenuBar->DisableMenuItem(dwFileMenu,m_pMenuBar->AddMenuItem(dwFileMenu,"──────────"));
	m_pMenuBar->AddMenuItem(dwFileMenu,"修改角色Model  Ctrl+S");		//3
	m_pMenuBar->AddMenuItem(dwFileMenu,"退出          Alt+X");		//5

	//查看菜单 1
	DWORD dwViewMenu = m_pMenuBar->AddMenu("View");

	//子菜单
	m_pMenuBar->AddMenuItem(dwViewMenu,"Grid         G");	  //0
	m_pMenuBar->AddMenuItem(dwViewMenu,"Wireframe    W");	  //1
	m_pMenuBar->AddMenuItem(dwViewMenu,"Normal       N");	  //2
	m_pMenuBar->AddMenuItem(dwViewMenu,"Joint        J");	  //3
	m_pMenuBar->DisableMenuItem(dwViewMenu, m_pMenuBar->AddMenuItem(dwViewMenu,"───────"));//4
	m_pMenuBar->AddMenuItem(dwViewMenu,"Show Trail   T");	  //5 
	m_pMenuBar->DisableMenuItem(dwViewMenu, m_pMenuBar->AddMenuItem(dwViewMenu,"Show Attribute"));//6 
	m_pMenuBar->DisableMenuItem(dwViewMenu, m_pMenuBar->AddMenuItem(dwViewMenu,"───────"));//7
	m_pMenuBar->AddMenuItem(dwViewMenu,"SizeBox");		  //8
	m_pMenuBar->AddMenuItem(dwViewMenu,"PickBox");		  //9	
	m_pMenuBar->AddMenuItem(dwViewMenu,"Locator");		  //10

	//编辑 2
	DWORD dwEditMenu = m_pMenuBar->AddMenu("Edit");
	m_pMenuBar->AddMenuItem(dwEditMenu,"Smooth Normal");		//0
	m_pMenuBar->AddMenuItem(dwEditMenu,"Update Normal");		//1
	m_pMenuBar->DisableMenuItem(dwEditMenu,m_pMenuBar->AddMenuItem(dwEditMenu,"────────"));
	m_pMenuBar->AddMenuItem(dwEditMenu,"Export Camera");		//3
	m_pMenuBar->AddMenuItem(dwEditMenu,"Import Camera");		//4
	m_pMenuBar->DisableMenuItem(dwEditMenu,m_pMenuBar->AddMenuItem(dwEditMenu,"────────"));
	m_pMenuBar->AddMenuItem(dwEditMenu,"Export AnimFrame");				//6
	m_pMenuBar->DisableMenuItem(dwEditMenu,m_pMenuBar->AddMenuItem(dwEditMenu,"────────"));
	m_pMenuBar->AddMenuItem(dwEditMenu,"Check Model File");				//8
	m_pMenuBar->DisableMenuItem(dwEditMenu,m_pMenuBar->AddMenuItem(dwEditMenu,"────────"));//9
	m_pMenuBar->AddMenuItem(dwEditMenu,"背景颜色       K");		//10
	m_pMenuBar->AddMenuItem(dwEditMenu,"灯光颜色       E");		//11

	//关于 3
	DWORD dwAboutMenu = m_pMenuBar->AddMenu("Help");
	m_pMenuBar->AddMenuItem(dwAboutMenu, "热键 ");
	m_pMenuBar->AddMenuItem(dwAboutMenu, "关于 ");

	//分割栏 4
	m_pMenuBar->DisableMenu(m_pMenuBar->AddMenu("|"));

	// 无效项
	//m_pMenuBar->DisableMenuItem(1, 5);
	//m_pMenuBar->DisableMenuItem(1, 6);
	//m_pMenuBar->DisableMenuItem(2, 8);

	SetMenuBarItemCheck();
}

void WndViewer::SetMenuBarItemCheck(void)
{
	m_pMenuBar->SetCheckMenuItem(1, 0, m_bShowGrid);
	m_pMenuBar->SetCheckMenuItem(1, 1, m_bDisplayWireFrame);
	m_pMenuBar->SetCheckMenuItem(1, 2, m_bShowNormal);
	m_pMenuBar->SetCheckMenuItem(1, 3, m_bShowBone);
	m_pMenuBar->SetCheckMenuItem(1, 5, m_bTrail);
	//m_pMenuBar->SetCheckMenuItem(1, 6, FALSE);
	m_pMenuBar->SetCheckMenuItem(1, 8, m_bShowSizeBox);
	m_pMenuBar->SetCheckMenuItem(1, 9, m_bShowPickBox);
	m_pMenuBar->SetCheckMenuItem(1, 10, m_bShowLocator);
}



/*
 * 功能: 切换快捷菜单是否可见
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2007.12.27
 */
void WndViewer::SwitchQuickMenuVisible(void)
{
	DWORD dwStyle;

	dwStyle = m_pQuickMenu->GetStyle();

	if (dwStyle & FWS_VISIBLE)
		m_pQuickMenu->EndPopUp();
	else
		m_pQuickMenu->BeginPopUp();
}



/*
 * 功能: 得到当前选择的绑定模型
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2010.01.05
 */
CDisplayModel * WndViewer::GetCurrentSelBindModel()
{
	return m_vBindModels[m_pWndModels->GetSelBindModel()]->GetDisplayModel();
}



void WndViewer::MenuBarProc(void)
{
	render::Interface		   * pInterface	= render::Interface::GetInstance();
	render::Interface::Layer2D * pLayer2D	= pInterface->GetLayer2D();
	ui::Manager::_tagINPUTINFO * pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	//菜单
	DWORD dwMenu,dwMenuItem;
	if (m_pMenuBar->TestSelMenu(dwMenu,dwMenuItem))
	{
		BOOL & bActive = m_tWindowInfo.bActive;
		m_pWndModels->ModifyStyle(FWS_DISABLE, 0);

		if (pInputInfo->dwData == MK_LBUTTON)
		{
			switch(dwMenu)
			{
			case 0:		//文件
				switch(dwMenuItem)
				{
				case 0:		//打开主模型文件
					{
						OPENFILENAME ofn;								// common dialog box structure
						static char szFile[_MAX_PATH] = "model";		// buffer for file name

						// Initialize OPENFILENAME
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = pInterface->GetWndHandle();
						ofn.lpstrFile = szFile;
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = "Model File (*)\0*\0All Files\0*.*\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = "model";
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_FILEMUSTEXIST;

						// Display the Open dialog box. 
						static char szOldDirectory[_MAX_PATH];
						GetCurrentDirectory(_MAX_PATH ,szOldDirectory);
						if (GetOpenFileName(&ofn))
						{
							SetCurrentDirectory(szOldDirectory);
							OpenMainModelFile(szFile);

							if (!m_bShowViewfinder)
								ResetCamera();
						}
					}
					break;
				case 1:		//打开绑定模型文件
					{
						OPENFILENAME ofn;								// common dialog box structure
						static char szFile[_MAX_PATH] = "model";		// buffer for file name

						// Initialize OPENFILENAME
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = pInterface->GetWndHandle();
						ofn.lpstrFile = szFile;
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = "Model File (*)\0*\0All Files\0*.*\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = "model";
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_FILEMUSTEXIST;

						// Display the Open dialog box. 
						static char szOldDirectory[_MAX_PATH];
						GetCurrentDirectory(_MAX_PATH ,szOldDirectory);
						if (GetOpenFileName(&ofn))
						{
							SetCurrentDirectory(szOldDirectory);
							OpenBindModelFile(szFile);
							//ResetCamera();
						}
					}
					break;
				case 3:		// 打开参考模型
					{
						OPENFILENAME ofn;       // common dialog box structure
						static char szFile[_MAX_PATH] = "model";       // buffer for file name

						// Initialize OPENFILENAME
						ZeroMemory(&ofn, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = pInterface->GetWndHandle();
						ofn.lpstrFile = szFile;
						ofn.nMaxFile = sizeof(szFile);
						ofn.lpstrFilter = "Model File (*)\0*\0All Files\0*.*\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFileTitle = NULL;
						ofn.nMaxFileTitle = 0;
						ofn.lpstrInitialDir = NULL;
						ofn.Flags = OFN_FILEMUSTEXIST;

						// Display the Open dialog box. 
						static char szOldDirectory[_MAX_PATH];
						GetCurrentDirectory(_MAX_PATH,szOldDirectory);
						if (GetOpenFileName(&ofn))
						{
							SetCurrentDirectory(szOldDirectory);
							OpenRefModelFile(szFile);
						}
					}
					break;
				case 5://
					{
						m_pWndModelTree->SwitchVisiblity();
					}
					break;
				case 6://退出
					PostQuitMessage(0);
					break;
				default:
					break;
				}
				break;
			case 1://查看
				{
					switch(dwMenuItem)
					{
					case 0://网格
						m_bShowGrid = !m_bShowGrid;
						break;
					case 1:
						m_bDisplayWireFrame = !m_bDisplayWireFrame;
						break;
					case 2://点法向
						m_bShowNormal = !m_bShowNormal;
						break;
					case 3://骨架
						m_bShowBone = !m_bShowBone;
						break;
					case 5://显示模型尾迹
						m_bTrail = !m_bTrail;

						if (!m_vRoleModels.empty())
						{
							for (size_t st = 0; st < m_vRoleModels.size(); ++st)
								m_vRoleModels[st]->GetDisplayModel()->SetAllTrailEmitter(m_bTrail);
						}

						break;
					case 6://
						break;
					case 8://包围盒
						m_bShowSizeBox = !m_bShowSizeBox;
						break;
					case 9:
						m_bShowPickBox = !m_bShowPickBox;
						break;
					case 10:
						m_bShowLocator = !m_bShowLocator;
						break;
					case 12:
						break;
					case 14:
						m_bShowLocator = !m_bShowLocator;
						break;
					default:
						break;
					}
				}
				break;
			case 2://编辑
				switch(dwMenuItem)
				{
				case 0://计算法向
					CalculateNormal();
					break;
				case 1://写入法向
					SaveNormal();
					break;
				case 3://导出摄像机文件
					ExportCamera();
					break;
				case 4://导入摄像机文件
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
				case 6:		//导出动画序列帧
					m_pWndAnimFrameEx->BeginPopUp();
					//ExportAnimationFrame();
					break;
				case 8:		//检查文件
					//CreateTemplateModelFile();			//生成模版文件
					CheckFile();
					break;
				case 10://调整BackColor
					m_pWndEditBKColor->BeginPopUp();
					break;
				case 11://调整灯光
					m_pWndEditLightColor->BeginPopUp();
					break;
				}
				break;
			case 3://帮助
				{
					switch(dwMenuItem)
					{
					case 0:			// 热键列表
						{
							m_pWndHotkeyList->BeginPopUp();
						}
						break;
					case 1:			//关于
						{
							m_pWndAbout->BeginPopUp();
						}
						break;
					}
				}
				break;
			default:
				{
					if (m_pModel && dwMenu < 5 + m_dwGroupMenuItemCount)			//判断主模型菜单
					{
						//int					iTypeIndex = (int)dwMenu - 5;
						DWORD				code	 = 0;
						DWORD				dwGroup	 = 0;
						DWORD				dwPart	 = 0;
						ui::MenuBar::Menu * pMenu	 = m_pMenuBar->GetMenu(dwMenu);
						BOOL				bChecked = pMenu->IsChecked(dwMenuItem);

						for(DWORD dwItem = 0; dwItem < pMenu->GetItemCnt(); ++dwItem)
							pMenu->SetCheckItem(dwItem, FALSE);
						pMenu->SetCheckItem(dwMenuItem, !bChecked);

						//m_iLevelModelIndex = m_pWndModels->GetSelRoleModel();
						//m_eLevelModelType  = MT_ROLE;

						// 判断是否需要展开
						if (stricmp(pMenu->GetItemText(dwMenuItem), "More") == 0)
						{
							m_pWndPart->ShowWnd(!bChecked);
							m_pWndPart->SetCurrentGroupName(pMenu->GetText());
							m_pWndPart->SetCurrentModelType(MT_ROLE);

							CreatePartList(m_vRoleModels[m_pWndModels->GetSelRoleModel()]->GetFilePathName().c_str());

							break;
						}

						strncpy((char *)&code, pMenu->GetText(), 4);
						dwGroup = code;
						strncpy((char *)&code, pMenu->GetItemText(dwMenuItem), 4);
						dwPart = code;

						m_pModel->ShowGroup(dwGroup, dwPart, m_pModel->GetLvlOfGroupPart(dwGroup, dwPart));

						if (bChecked)
							m_pModel->HideGroup(dwGroup);
					}else if (m_vBindModels.size() != 0)				//判断绑定模型菜单
					{
						DWORD				code	 = 0;
						DWORD				dwGroup	 = 0;
						DWORD				dwPart	 = 0;
						ui::MenuBar::Menu * pMenu	 = m_pMenuBar->GetMenu(dwMenu);
						BOOL				bChecked = pMenu->IsChecked(dwMenuItem);

						for(DWORD dwItem = 0; dwItem < pMenu->GetItemCnt(); ++dwItem)
							pMenu->SetCheckItem(dwItem, FALSE);
						pMenu->SetCheckItem(dwMenuItem, !bChecked);

						//m_iLevelModelIndex = m_pWndModels->GetSelBindModel();
						//m_eLevelModelType  = MT_BIND;

						// 判断是否需要展开
						if (stricmp(pMenu->GetItemText(dwMenuItem), "More") == 0)
						{
							m_pWndPart->ShowWnd(!bChecked);
							m_pWndPart->SetCurrentGroupName(pMenu->GetText());
							m_pWndPart->SetCurrentModelType(MT_BIND);

							CreatePartList(m_vBindModels[m_pWndModels->GetSelBindModel()]->GetFilePathName().c_str());

							break;
						}

						strncpy((char *)&code, pMenu->GetText(), 4);
						dwGroup = code;
						strncpy((char *)&code, pMenu->GetItemText(dwMenuItem), 4);
						dwPart = code;

						CDisplayModel * pModel = m_vBindModels[m_pWndModels->GetSelBindModel()]->GetDisplayModel();
						pModel->ShowGroup(dwGroup, dwPart, pModel->GetLvlOfGroupPart(dwGroup, dwPart));

						if (bChecked)
							pModel->HideGroup(dwGroup);
					}
				}
				break;
			}
		}else
		{
			if (m_pModel && dwMenu >= 5 && dwMenu < 5 + m_dwGroupMenuItemCount)		//右键显示主模型级别
			{
				DWORD				code	= 0;
				DWORD				dwGroup = 0;
				DWORD				dwPart	= 0;
				ui::MenuBar::Menu *	pMenu	= m_pMenuBar->GetMenu(dwMenu);

				// 判断是否展开
				if (stricmp(pMenu->GetItemText(dwMenuItem), "More") != 0)
				{
					strncpy((char *)&code, pMenu->GetText(), 4);
					dwGroup = code;
					strncpy((char *)&code, pMenu->GetItemText(dwMenuItem), 4);
					dwPart = code;

					GameModel * pGM = CGameModelManager::GetInstance()->GetGameModel(m_pModel->GetGameModelID());

					GameModel::Group	   * pGroup = pGM->GetGroup(dwGroup);
					GameModel::Group::Part * pPart  = pGroup->GetPart(dwPart);
					m_pWndSelectLevel->AddLevel(dwGroup, dwPart, pPart->GetLevelCount());
					m_pWndSelectLevel->ShowWnd(true);

					m_iLevelModelIndex = m_pWndModels->GetSelRoleModel();
					m_eLevelModelType  = MT_ROLE;
				}
			}else if (m_vBindModels.size() != 0)		//右键显示绑定模型级别
			{
				DWORD				code	= 0;
				DWORD				dwGroup	= 0;
				DWORD				dwPart	= 0;
				ui::MenuBar::Menu *	pMenu	= m_pMenuBar->GetMenu(dwMenu);

				// 判断是否展开
				if (stricmp(pMenu->GetItemText(dwMenuItem), "More") != 0)
				{
					strncpy((char *)&code, pMenu->GetText(), 4);
					dwGroup = code;
					strncpy((char *)&code, pMenu->GetItemText(dwMenuItem), 4);
					dwPart = code;

					m_iLevelModelIndex = m_pWndModels->GetSelBindModel();
					m_eLevelModelType  = MT_BIND;

					GameModel * pGM = CGameModelManager::GetInstance()->GetGameModel(m_vBindModels[m_iLevelModelIndex]->GetDisplayModel()->GetGameModelID());

					GameModel::Group	   * pGroup = pGM->GetGroup(dwGroup);
					GameModel::Group::Part * pPart  = pGroup->GetPart(dwPart);
					m_pWndSelectLevel->AddLevel(dwGroup, dwPart, pPart->GetLevelCount());
					m_pWndSelectLevel->ShowWnd(true);
				}
			}
		}
	}

	SetMenuBarItemCheck();
	
	m_pWndModels->ModifyStyle(0, FWS_DISABLE);
}

void WndViewer::DestroyMenuBar(void)
{
	SAFEDESTROY(m_pMenuBar);
}



/*
 * 功能: 快捷菜单事件处理
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2007.12.27
 * 修改日志:
 *	2008.12.31 - lpf
 *		增加了地表纹理的创建
 */
void WndViewer::QuickMenuProc(void)
{
	ui::Manager::_tagINPUTINFO * pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	
	DWORD dwMenuItem;
	if (m_pQuickMenu->TestSelMenu(dwMenuItem) &&
		m_pQuickMenu->GetStyle() & FWS_VISIBLE)
	{
		if (pInputInfo->dwData == MK_LBUTTON)
		{
			switch (dwMenuItem)
			{
			case 0:			//打开主模型文件
				OpenMainModelFile(m_szPathNameTmp);
				SetQuickMenuVisible(false);

				if (!m_bShowViewfinder)
					ResetCamera();
				break;
			case 1:			//打开绑定模型文件
				OpenBindModelFile(m_szPathNameTmp);
				SetQuickMenuVisible(false);
				break;
			case 2:			//设定地表
				OpenGridTexFile(m_szPathNameTmp);
				SetQuickMenuVisible(false);
				break;
			case 3:			//打开背景图片
				OpenBKPicFile(m_szPathNameTmp);
				SetQuickMenuVisible(false);
				break;
			case 4:			//打开参考模型文件
				OpenRefModelFile(m_szPathNameTmp);
				SetQuickMenuVisible(false);
				break;
			}
		}
	}
}

void WndViewer::ShowFps(void)
{
	if (!m_bShowUI)
		return;

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

	//DRAW FPS
	int iFps = GetFps();
	static char szTemp[256];

	sprintf(szTemp," FPS: %2d",iFps);
	int iLen = lstrlen(szTemp);

	int iTextWidth  = pLayer2D->CalcTextWidth(szTemp,iLen);
	int iTextHeight = pLayer2D->GetFontSize();

	int iWidth = iTextWidth + 16;

	int iStartX = (iWidth / 2 - iTextWidth / 2) + 948;//12;
	int iStartY = pInterface->GetWndHeight() - iTextHeight - iTextHeight / 2 - 10;
	
	pLayer2D->DrawSolidQuad(iStartX - 2, iStartY, iTextWidth + 8, iTextHeight + 2, 0x80bbc5ff);
	ui::Wnd::SetUIRendState();
	pLayer2D->TextOut(iStartX, iStartY, szTemp, lstrlen(szTemp), 0xfff0ff90);
}

void WndViewer::ShowInfo(void)
{
    render::Interface		   * pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D * pLayer2D   = pInterface->GetLayer2D();

	static char szTemp[1024];

	//DRAW FPS
	if(!m_pModel || !m_bShowUI)
		return;

	int iTriCount;

	iTriCount = m_pModel->GetTriangleCount();
	AnimInfo::tagActionInfo *pActionInfo = m_cAnimInfo.GetActionInfo();
	GameModel::Action *pAction = CGameModelManager::GetInstance()->GetActionAnim(m_pModel->GetGameModelID(), pActionInfo->dwCurAction);
	char szActionDump[256] = {0};
	if (pAction)
	{
		if (pAction->GetAnimJoint())
		{
			char szName[5] = {0};
			*(DWORD*)szName = pAction->GetName();
			_snprintf(szActionDump,255,"#FF70b689动作:#FFFAFA88%s\n#FF70b689关键帧:#FFFAFA88%d\n#FF70b689循环播放:#FFFAFA88%s\n",
				szName,
				pAction->GetAnimJoint()->GetFrameCount(),
				(m_bAnimLooped)? "开" : "关");
		}
	}

	POINT ptCur,ptOld;
	input::Interface::GetInstance()->GetMousePosition(&ptCur,&ptOld);
	sprintf(szTemp,"#FF70b689面数:#FFFAFA88%d\n%s\n",iTriCount,szActionDump);
	int iLen = lstrlen(szTemp);

	int iTextWidth  = pLayer2D->CalcTextWidth(szTemp,iLen);
	int iTextHeight = pLayer2D->GetFontSize();

	int iWidth = iTextWidth + 16;	

	int iStartX = 12;
	int iStartY = 708;

	//pLayer2D->DrawSolidQuad(iStartX - 2,iStartY ,iTextWidth + 8,iTextHeight + 2 ,0x80bbc5ff);
	pLayer2D->TextOutEx(iStartX,iStartY,szTemp,lstrlen(szTemp));

	D3DXVECTOR3 * pos  = m_pCamera->GetPosition();
	D3DXVECTOR3 * view = m_pCamera->GetView();
	char szCameraInfo[256] = {0};
	if (m_bShowCameraInfo)
	{
		_snprintf(szCameraInfo,255,"#FF509669CAMERA:\n#FFFAFA88[%f %f %f]\n[%f %f %f]\n#FF509669\n",pos->x,pos->y,pos->z,view->x,view->y,view->z);
		pLayer2D->TextOutEx(iStartX + 410,iStartY + 12,szCameraInfo,lstrlen(szCameraInfo));
	}
}

void WndViewer::ShowCursor(void)
{
	if (!m_bShowUI)
		return;

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

	//鼠标
	POINT ptCur,ptOld;
	input::Interface::GetInstance()->GetMousePosition(&ptCur,&ptOld);

	pLayer2D->DrawTexture(ptCur.x,ptCur.y,m_hCursorHandle,0xffffffff); 
}

void WndViewer::ShowGrid(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D   = pInterface->GetLayer3D();
	pInterface->GetDevice()->SetRenderState(D3DRS_LIGHTING,FALSE);
	if (m_bShowGrid && m_bShowUI)
	{
		//draw grid plane along to y
		D3DXVECTOR3 vec[2];

		if (m_bShowGridMoving)
		{
			DWORD dwCurrentTime = timeGetTime();
			DWORD dwElapseTime = dwCurrentTime - m_dwGridMovingStartTime;
			float m_fGridMovingDis = dwElapseTime * m_fGridMovingSpeed;

			if (m_fGridMovingDis >= 1.0f)
				m_dwGridMovingStartTime = dwCurrentTime;

			float f;
			for(int iStep = -4 ; iStep <= 5; iStep++)
			{
				vec[0] = D3DXVECTOR3((float)iStep, 0.0f, 5.0f);
				vec[1] = D3DXVECTOR3((float)iStep, 0.0f, -5.0f);

				pLayer3D->_DEBUG_DrawLine(vec,D3DCOLOR_ARGB(255,128,128,128));

				f = (float)iStep - m_fGridMovingDis;
				if (f > -5.0f)
				{
					vec[0] = D3DXVECTOR3(5.0f, 0.0f, f);
					vec[1] = D3DXVECTOR3(-5.0f, 0.0f, f);
					pLayer3D->_DEBUG_DrawLine(vec,D3DCOLOR_ARGB(255,128,128,128));
				}
			}

			vec[0] = D3DXVECTOR3(-5.0f, 0.0f, 5.0f);
			vec[1] = D3DXVECTOR3(-5.0f, 0.0f, -5.0f);
			pLayer3D->_DEBUG_DrawLine(vec, D3DCOLOR_ARGB(255,128,128,128));

			vec[0] = D3DXVECTOR3(5.0f, 0.0f, -5.0f);
			vec[1] = D3DXVECTOR3(-5.0f, 0.0f, -5.0f);
			pLayer3D->_DEBUG_DrawLine(vec, D3DCOLOR_ARGB(255,128,128,128));

			vec[0] = D3DXVECTOR3(5.0f, 0.0f, 5.0f);
			vec[1] = D3DXVECTOR3(-5.0f, 0.0f, 5.0f);
			pLayer3D->_DEBUG_DrawLine(vec, D3DCOLOR_ARGB(255,128,128,128));

			if (m_dwGridTex != 0)
			{
				dwElapseTime = dwCurrentTime - m_dwGridTexMovingStartTime;
				float fTmp = dwElapseTime * m_fGridMovingSpeed * 0.1f;

				D3DXVECTOR3 vertex[3];
				D3DXVECTOR2 vertexUV[3];
				vertex[0] = D3DXVECTOR3(5.0f, 0.0f, -5.0f);
				vertex[1] = D3DXVECTOR3(-5.0f, 0.0f, -5.0f);
				vertex[2] = D3DXVECTOR3(5.0f, 0.0f, 5.0f);
				vertexUV[0] = D3DXVECTOR2(0.0f, 0.0f + fTmp);
				vertexUV[1] = D3DXVECTOR2(1.0f, 0.0f + fTmp);
				vertexUV[2] = D3DXVECTOR2(0.0f, 1.0f + fTmp);
				pLayer3D->_DEBUG_DrawTriangle(m_dwGridTex, vertex, vertexUV, 0xffffffff);
				vertex[0] = D3DXVECTOR3(-5.0f, 0.0f, -5.0f);
				vertex[1] = D3DXVECTOR3(-5.0f, 0.0f, 5.0f);
				vertex[2] = D3DXVECTOR3(5.0f, 0.0f, 5.0f);
				vertexUV[0] = D3DXVECTOR2(1.0f, 0.0f + fTmp);
				vertexUV[1] = D3DXVECTOR2(1.0f, 1.0f + fTmp);
				vertexUV[2] = D3DXVECTOR2(0.0f, 1.0f + fTmp);
				pLayer3D->_DEBUG_DrawTriangle(m_dwGridTex, vertex, vertexUV, 0xffffffff);
			}
		}else
		{
			for(int iStep = -5 ; iStep <= 5; iStep++)
			{
				vec[0] = D3DXVECTOR3((float)iStep,0.0f,5.0f);
				vec[1] = D3DXVECTOR3((float)iStep,0.0f,-5.0f);

				pLayer3D->_DEBUG_DrawLine(vec,D3DCOLOR_ARGB(255,128,128,128));

				vec[0] = D3DXVECTOR3(5.0f,0.0f,(float)iStep);
				vec[1] = D3DXVECTOR3(-5.0f,0.0f,(float)iStep);

				pLayer3D->_DEBUG_DrawLine(vec,D3DCOLOR_ARGB(255,128,128,128));
			}
		}
	}
}

void WndViewer::SetupTransform(void)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pInterface->GetDevice()->SetTransform(D3DTS_WORLD,&matWorld);
	pInterface->GetDevice()->SetTransform(D3DTS_VIEW,m_pCamera->GetViewMatrix());
	pInterface->GetDevice()->SetTransform(D3DTS_PROJECTION,m_pCamera->GetProjectionMatrix());
}

void WndViewer::ShowAxis(void)
{
	if (!m_bShowUI)
		return;

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D   *pLayer3D = pInterface->GetLayer3D();
	render::Interface::Layer2D   *pLayer2D = pInterface->GetLayer2D();  

	D3DXMATRIX matViewClip;
	D3DXMatrixIdentity(&matViewClip);
	matViewClip._41 = matViewClip._11 = (float)(130 >> 1);
	matViewClip._42 = matViewClip._22 = (float)(100 >> 1);
	matViewClip._22 = -matViewClip._22;

	D3DXMATRIX matView,matProj,matFinal,matTemp;
	pInterface->GetDevice()->GetTransform(D3DTS_VIEW,&matView);
	pInterface->GetDevice()->GetTransform(D3DTS_PROJECTION,&matProj);
	D3DXMatrixMultiply(&matFinal,&matView,&matProj);
	D3DXMatrixMultiply(&matFinal,&matFinal,&matViewClip);

	//画坐标draw axis (X , Y , Z)
	D3DXMATRIX matTran;
	D3DXMatrixTranslation(&matTran, -5.0f, 10.0f, 0.0f);
	D3DXMatrixMultiply(&matFinal,&matFinal,&matTran);

	FLOAT fLength = 2.0f;
	D3DXVECTOR3 v0(0.0f,0.0f,0.0f),vx(fLength,0.0f,0.0f),vy(0.0f,fLength,0.0f),vz(0.0f,0.0f,fLength);
	D3DXVECTOR3 v[2];
	v[0] = v0; v[1] = vx;
	pLayer3D->_DEBUG_DrawLine(v,D3DCOLOR_ARGB(255,255,0,0));
	ui::Wnd::SetUIRendState();
	D3DXVec3TransformCoord(&v[1],&v[1],&matFinal);
	ui::Wnd::SetUIRendState();
	pLayer2D->TextOut((int)v[1].x,500 + (int)v[1].y - 16,"X",1,0xFFFF0000);
	v[0] = v0; v[1] = vy;
	pLayer3D->_DEBUG_DrawLine(v,D3DCOLOR_ARGB(255,0,255,0));
	ui::Wnd::SetUIRendState();
	D3DXVec3TransformCoord(&v[1],&v[1],&matFinal);
	pLayer2D->TextOut((int)v[1].x,500 + (int)v[1].y - 16,"Y",1,0xFF00FF00);
	v[0] = v0; v[1] = vz;
	pLayer3D->_DEBUG_DrawLine(v,D3DCOLOR_ARGB(255,0,0,255));
	D3DXVec3TransformCoord(&v[1],&v[1],&matFinal);
	ui::Wnd::SetUIRendState();
	pLayer2D->TextOut((int)v[1].x,500 + (int)v[1].y - 16,"Z",1,0xFF0000FF);
}




void WndViewer::GetModelName(string & sModelName, const string & sModelPathName)
{
	int iFindCount	  = 0;
	int iNameStartPos = 0;
	int iNameEndPos   = 0;

	for (size_t st = sModelPathName.size() - 1; st >= 0; --st)
	{
		if (sModelPathName[st] != '\\')
			continue;

		++iFindCount;
		if (iFindCount == 1)
			iNameEndPos = (int)st;
		else if (iFindCount == 2)
		{
			iNameStartPos = (int)st + 1;
			break;
		}
	}

	sModelName = sModelPathName.substr(iNameStartPos, iNameEndPos - iNameStartPos);
}



/*
 * 功能: 全局热键处理
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: -
 * 创建日期: -
 * 修改日志:
 *	2008.12.31 - lpf
 *		增加了快捷键DIK_M,处理地表滚动
 */
void WndViewer::HotkeyProc(void)
{
	//---SELF
	ui::Manager *pUI = ui::Manager::GetInstance();
	
	render::Interface *pInterface = render::Interface::GetInstance();

	ui::Manager::_tagINPUTINFO *pInputInfo = pUI->GetInputInfoBuffer();

	//键盘
	if (pInputInfo->eType == UIMT_KB_KEYDOWN)
	{
		switch(pInputInfo->dwData)
		{
		case DIK_A:		//下一个动做
			m_pWndAction->NextAction();
			break;
		case DIK_B:
			break;
		case DIK_C:
			m_bShowCameraInfo = !m_bShowCameraInfo;
			break;
		case DIK_D:
			break;
		case DIK_E:		//灯光编辑
			if (!KEYDOWN(VK_CONTROL))
				m_pWndEditLightColor->BeginPopUp();
			break;
		case DIK_G:
			m_bShowGrid = !m_bShowGrid;
			break;
		case DIK_I:		//灯光模型显示
			m_bShowLightArrow = !m_bShowLightArrow;
			break;
		case DIK_J:
			m_bShowBone = !m_bShowBone;
			break;
		case DIK_K:		//背景编辑
			m_pWndEditBKColor->BeginPopUp();
			break;
		case DIK_L:
			m_bLightEnable = !m_bLightEnable;
			break;
		case DIK_M:
			{
				DWORD dwCurrent			   = timeGetTime();
				m_bShowGridMoving		   = !m_bShowGridMoving;
				m_dwGridMovingStartTime	   = dwCurrent;
				m_dwGridTexMovingStartTime = dwCurrent;
				m_pWndGridMovingSpeedModify->ShowWnd(m_bShowGridMoving);
			}
			break;
		case DIK_N:
			m_bShowNormal = !m_bShowNormal;
			break;
		case DIK_O:
			break;
		case DIK_P:
			m_bAnimLooped = !m_bAnimLooped;
			m_cAnimInfo.GetActionInfo()->bCurActionLooped = m_bAnimLooped;
			break;
		case DIK_R:		//摄象机复位,角色也相应复位
			ResetCamera();
			ZeroMemory(&m_vRolePos, sizeof(D3DXVECTOR3));
			break;
		case DIK_S:		//切换背景图片和背景颜色
			m_bSwitchBK = !m_bSwitchBK;
			break;
		case DIK_T:		//切换尾迹显示
			m_bTrail = !m_bTrail;

			if (!m_vRoleModels.empty())
			{
				for (size_t st = 0; st < m_vRoleModels.size(); ++st)
					m_vRoleModels[st]->GetDisplayModel()->SetAllTrailEmitter(m_bTrail);
			}
			break;
		case DIK_V:
			m_bShowViewfinder = !m_bShowViewfinder;
			break;
		case DIK_W:
			m_bDisplayWireFrame = !m_bDisplayWireFrame;
			break;
		case DIK_X:
			break;
		case DIK_Z:
			break;
		case DIK_F5:
			{
				for (size_t st = 0; st < m_vRoleModels.size(); ++st)
					m_vRoleModels[st]->SaveShowState();

				for (size_t st = 0; st < m_vBindModels.size(); ++st)
					m_vBindModels[st]->SaveShowState();

				CGameModelManager::GetInstance()->ReloadAllModel(false);

				for (size_t st = 0; st < m_vRoleModels.size(); ++st)
					m_vRoleModels[st]->SetShowState();

				for (size_t st = 0; st < m_vBindModels.size(); ++st)
					m_vBindModels[st]->SetShowState();

				DWORD dwCurrentTime = timeGetTime();
				m_cAnimInfo.SetAnimTime(dwCurrentTime);

				AnimInfo::tagActionInfo * pActionInfo = m_cAnimInfo.GetActionInfo();
				pActionInfo->dwCurActionStartTime	  = dwCurrentTime;
				pActionInfo->dwInterpolationStartTime = dwCurrentTime;

				if (!m_vBindModels.empty())
				{
					CBindModel * pBindModel;
					AnimInfo   * pAnimInfo;

					for (int i = 0; i < (int)m_vBindModels.size(); ++i)
					{
						pBindModel = m_vBindModels[i];
						pAnimInfo  = pBindModel->GetAnimInfo();
						pAnimInfo->SetAnimTime(dwCurrentTime);

						pActionInfo							  = pAnimInfo->GetActionInfo();
						pActionInfo->dwCurActionStartTime	  = dwCurrentTime;
						pActionInfo->dwInterpolationStartTime = dwCurrentTime;
					}
				}
			}
			break;
		case DIK_F6:
			break;
		case DIK_SPACE:
			{
				if (m_bAnimLooped)
				{
					if (!m_bAnimPlaying)
					{
						//播放
						//播放一次
						m_bAnimPlaying = TRUE;
						m_cAnimInfo.GetActionInfo()->dwCurActionLoopCount = 0;
						m_cAnimInfo.GetActionInfo()->dwCurActionStartTime = timeGetTime();
						m_cAnimInfo.GetActionInfo()->bCurActionLooped = TRUE;
					}
					else
					{
						m_bAnimPlaying = FALSE;
						//停止
					}
				}
				else
				{
					if (!m_bAnimPlaying)
					{
						//播放一次
						m_bAnimPlaying = TRUE;
						m_cAnimInfo.GetActionInfo()->dwCurActionLoopCount = 0;
						m_cAnimInfo.GetActionInfo()->dwCurActionStartTime = timeGetTime();
						m_cAnimInfo.GetActionInfo()->bCurActionLooped = FALSE;
					}
				}
			}
			break;
		case DIK_ESCAPE:		//关闭所有弹出窗体
			m_pWndEditBKColor->EndPopUp();
			m_pWndEditLightColor->EndPopUp();
			m_pWndHotkeyList->EndPopUp();
			m_pWndAbout->EndPopUp();
			SetQuickMenuVisible(false);
			m_pWndLocator->ShowWnd(false);

			for (int iItem = 0; iItem < (int)m_pWndModels->GetItemCount(); ++iItem)
				m_pWndModels->SetItemChecked(iItem, false);
			break;
		}

		// 组合键处理
		if (KEYDOWN(VK_CONTROL))
		{
			if (KEYDOWN(VK_O))			//打开主模型文件
			{
				OPENFILENAME ofn;							// common dialog box structure
				static char szFile[_MAX_PATH] = "model";	// buffer for file name

				// Initialize OPENFILENAME
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = pInterface->GetWndHandle();
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = "Model File (*)\0*\0All Files\0*.*\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_FILEMUSTEXIST;

				static char szOldDirectory[_MAX_PATH];
				GetCurrentDirectory(_MAX_PATH,szOldDirectory);
				// Display the Open dialog box. 
				if (GetOpenFileName(&ofn))
				{
					SetCurrentDirectory(szOldDirectory);
					OpenMainModelFile(szFile);

					if (!m_bShowViewfinder)
						ResetCamera();
				}
			}else if (KEYDOWN(VK_B))		//打开绑定模型
			{
				OPENFILENAME ofn;							// common dialog box structure
				static char szFile[_MAX_PATH] = "model";    // buffer for file name

				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = pInterface->GetWndHandle();
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = "Model File (*)\0*\0All Files\0*.*\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_FILEMUSTEXIST;

				// Display the Open dialog box. 
				static char szOldDirectory[_MAX_PATH];
				GetCurrentDirectory(_MAX_PATH,szOldDirectory);
				if (GetOpenFileName(&ofn))
				{
					SetCurrentDirectory(szOldDirectory);
					OpenBindModelFile(szFile);
				}
			}else if (KEYDOWN(VK_R))		//打开参考模型
			{
				OPENFILENAME ofn;							// common dialog box structure
				static char szFile[_MAX_PATH] = "model";    // buffer for file name

				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = pInterface->GetWndHandle();
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = "Model File (*)\0*\0All Files\0*.*\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_FILEMUSTEXIST;

				// Display the Open dialog box. 
				static char szOldDirectory[_MAX_PATH];
				GetCurrentDirectory(_MAX_PATH,szOldDirectory);
				if (GetOpenFileName(&ofn))
				{
					SetCurrentDirectory(szOldDirectory);
					OpenRefModelFile(szFile);
				}
			}else if (KEYDOWN(VK_S))
			{
				m_pWndModelTree->SwitchVisiblity();
			}else if (KEYDOWN(VK_H))
			{
				m_bShowUI = !m_bShowUI;
			}
		}else if (KEYDOWN(VK_ALT))
		{
			if (KEYDOWN(VK_X))			//退出程序
				PostQuitMessage(0);
		}
	}
	///处理目标位置
 	if (GetAsyncKeyState(VK_LEFT))
	{
		TargetPos.x += 0.02f;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		TargetPos.x -= 0.02f;
	}
	if (GetAsyncKeyState(VK_UP))
	{
		TargetPos.z -= 0.02f;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		TargetPos.z += 0.02f;
	}
	if (GetAsyncKeyState(VK_PRIOR))		//PAGE-UP
	{
		TargetPos.y += 0.02f;
	}
	if (GetAsyncKeyState(VK_NEXT))		//PAGE-DOWN
	{
		TargetPos.y -= 0.02f;
	}
	//处理主模型位置
	if (GetAsyncKeyState(VK_NUMPAD4))
	{
		m_vRolePos.x += 0.02f;
	}
	if (GetAsyncKeyState(VK_NUMPAD6))
	{
		m_vRolePos.x -= 0.02f;
	}
	if (GetAsyncKeyState(VK_NUMPAD8))
	{
		m_vRolePos.z += 0.02f;
	}
	if (GetAsyncKeyState(VK_NUMPAD2))
	{
		m_vRolePos.z -= 0.02f;
	}
	if (GetAsyncKeyState(VK_ADD))		//PAGE-UP
	{
		m_vRolePos.y += 0.02f;
	}
	if (GetAsyncKeyState(VK_SUBTRACT))		//PAGE-DOWN
	{
		m_vRolePos.y -= 0.02f;
	}
}

void WndViewer::CalculateNormal(void)
{
	if (!m_vRoleModels.empty())
		return;

	//计算所有模型的法向，并保存
	int iCurrentModel = m_pWndModels->GetSelRoleModel();
	CRoleModel * pRoleModel = m_vRoleModels[iCurrentModel];
	const char * pszFilePathName = pRoleModel->GetFilePathName().c_str();

	//读取配置文件名称
	utility::File file;
	if (file.Open(pszFilePathName))
	{
		utility::Text script;
		script.Create(&file);
		utility::Text::Node *pGroupListNode = script.GetRootNode()->GetChildNode("group");

		char szPath[_MAX_PATH];
		utility::File::GetFilePath(pszFilePathName,szPath);
		strcat(szPath,"\\");
		for(DWORD dwGroup = 0; dwGroup < pGroupListNode->GetChildCount(); dwGroup++)
		{
			DWORD dwGroupName = 0;
			utility::Text::Node *pGroupNode = pGroupListNode->GetChildNode(dwGroup);
			strncpy((char*)&dwGroupName,pGroupNode->GetName(),4);
			for(DWORD dwPart = 0; dwPart < pGroupNode->GetChildCount(); dwPart++)
			{
				utility::Text::Node *pPartNode = pGroupNode->GetChildNode(dwPart);

             	//得到对应的mesh
				DWORD dwPartName = 0;
				strncpy((char*)&dwPartName,pPartNode->GetName(),4);

				GameModel * pGM = CGameModelManager::GetInstance()->GetGameModel(m_pModel->GetGameModelID());

				GameModel::Group *pGroup  = pGM->GetGroup(dwGroupName);
				GameModel::Group::Part *pPart = pGroup->GetPart(dwPartName);

				GameModel::Group::Part::tagRenderMesh *pRenderMeshArray = pPart->GetRenderMeshArray();
				for(DWORD n = 0; n < pPart->GetRenderMeshArrayCount();n++)
				{
					if (pRenderMeshArray[n].pMesh)
						pRenderMeshArray[n].pMesh->CalculateVertexNormal();
				}
			}
		}

		script.Destroy();
		file.Close();
	}
}

void WndViewer::SaveNormal(void)
{
	if (!m_vRoleModels.empty())
		return;

	// 计算所有模型的法向，并保存
	int iCurrentModel = m_pWndModels->GetSelRoleModel();
	CRoleModel * pRoleModel = m_vRoleModels[iCurrentModel];
	const char * pszFilePathName = pRoleModel->GetFilePathName().c_str();

	utility::File file;
	if (file.Open(pszFilePathName))
	{
		utility::Text script;
		script.Create(&file);			//打开model文件脚本
		utility::Text::Node *pGroupListNode = script.GetRootNode()->GetChildNode("group");

		// 得到模型文件路径
		char szPath[_MAX_PATH],szFileName[_MAX_PATH];
		utility::File::GetFilePath(pszFilePathName,szPath);
		strcat(szPath,"\\");

		// 循环处理模型下每个组件
		for(DWORD dwGroup = 0; dwGroup < pGroupListNode->GetChildCount(); dwGroup++)
		{
			DWORD dwGroupName = 0;
			utility::Text::Node *pGroupNode = pGroupListNode->GetChildNode(dwGroup);		//得到某个组件节点
			strncpy((char*)&dwGroupName,pGroupNode->GetName(),4);

			// 循环处理组件下的每个部件
			for(DWORD dwPart = 0; dwPart < pGroupNode->GetChildCount(); dwPart++)
			{
				utility::Text::Node *pPartNode = pGroupNode->GetChildNode(dwPart);			//得到组件下的某个部件节点

             	// 得到对应的mesh名字
				DWORD dwPartName = 0;
				strncpy((char*)&dwPartName,pPartNode->GetName(),4);

				GameModel * pGM = CGameModelManager::GetInstance()->GetGameModel(m_pModel->GetGameModelID());

				GameModel::Group	   * pGroup = pGM->GetGroup(dwGroupName);
				GameModel::Group::Part * pPart  = pGroup->GetPart(dwPartName);

				// 创建Part文件脚本
				string		  sPartFileName;
				utility::File fPartFile;
				utility::Text tPartScript;

				sPartFileName = szPath;
				sPartFileName += pPartNode->GetVar("file")->GetStringValue();

				fPartFile.Open(sPartFileName.c_str());
				tPartScript.Create(&fPartFile);
				fPartFile.Close();

				// 得到mesharray节点
				utility::Text::Node *pMeshArrayNode = tPartScript.GetRootNode()->GetChildNode("mesharray");

				GameModel::Group::Part::tagRenderMesh * pRenderMeshArray = pPart->GetRenderMeshArray();
				for(DWORD n = 0; n < pPart->GetRenderMeshArrayCount();n++)
				{
					if (pRenderMeshArray[n].pMesh)
					{
						model::Mesh * pMesh = pRenderMeshArray[n].pMesh;
						D3DXVECTOR3 *pNmlBuffer = pMesh->GetNmlBuffer();				
						// 写入新文件
						utility::File fileInput;
						strcpy(szFileName, szPath);
						strcat(szFileName,pMeshArrayNode->GetChildNode(n)->GetVar("mesh")->GetStringValue());
						VERIFY(fileInput.Open(szFileName));				
						int iLength = fileInput.GetLength();
						BYTE *p = new BYTE[iLength];
						iLength = fileInput.GetLength();
						memcpy(p,fileInput.GetData(),iLength);
						fileInput.Close();
						// 填充normal数据
						memcpy(p + 4 * sizeof(DWORD) + pMesh->GetPNCCount() * sizeof(D3DXVECTOR3),pNmlBuffer,pMesh->GetPNCCount() * sizeof(D3DXVECTOR3));

						VERIFY(utility::File::FileExist(szFileName));
						FILE *fp = fopen(szFileName,"wb");
						if (fp == NULL)
						{
							MessageBox(NULL,".mesh文件可能属性为只读，无法写入，将跳过!","提示",MB_OK);
						}
						else
						{
							fwrite(p,iLength,1,fp);
							fclose(fp);
						}
					}
				}
				tPartScript.Destroy();
			}
		}

		MessageBox(NULL,"重写法向完成","提示",MB_OK);

		script.Destroy();
		file.Close();
	}
	
}

void WndViewer::ExportCamera(void)
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
}

void WndViewer::ImportCamera(const char *szFileName)
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



/*
 * 功能: 导出动画序列帧
 * 摘要: -
 * 参数: -
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.04.07
 */
void WndViewer::ExportAnimationFrame()
{
	render::Interface * pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D * pLayer3D = pInterface->GetLayer3D();
	LPDIRECT3DSURFACE9 pRenderTarget;

	pInterface->GetDevice()->EndScene();

	// 保存相关信息
	pInterface->GetDevice()->GetRenderTarget(0, &m_pOldRenderTarget);
	pInterface->GetDevice()->GetDepthStencilSurface(&m_pOldZStencil);

	pInterface->GetDevice()->CreateTexture(m_pWndAnimFrameEx->GetExportWidth(),
										   m_pWndAnimFrameEx->GetExportHeight(),
										   1,
										   D3DUSAGE_RENDERTARGET,
										   D3DFMT_A8R8G8B8,
										   D3DPOOL_DEFAULT,
										   &m_pRenderTexture,
										   NULL);

	pInterface->GetDevice()->CreateDepthStencilSurface(m_pWndAnimFrameEx->GetExportWidth(),
													   m_pWndAnimFrameEx->GetExportHeight(), 
													   pInterface->GetInfo()->d3dpp.AutoDepthStencilFormat, 
													   D3DMULTISAMPLE_NONE,
													   0,
													   TRUE,
													   &m_pDepthStencilSurface,
													   NULL);

	m_pRenderTexture->GetSurfaceLevel(0, &pRenderTarget);

	pInterface->GetDevice()->SetRenderTarget(0, pRenderTarget);
	pInterface->GetDevice()->SetDepthStencilSurface(m_pDepthStencilSurface);

	pInterface->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	//======draw model=====
	if (!m_vRoleModels.empty())
	{
		int iSelIndex = m_pWndModels->GetSelRoleModel();
		m_pModel = m_vRoleModels[iSelIndex]->GetDisplayModel();
	}

	if (m_pModel)
	{
		m_cAnimInfo.SetCurrentTime(m_cAnimInfo.GetActionInfo()->dwCurActionStartTime + m_dwAnimFrameIndex * m_dwAnimFPS);
		m_cAnimInfo.SetBillboard(m_pCamera->GetBillboard());
		m_cAnimInfo.SetBillboardY(m_pCamera->GetBillboardY());

		m_pModel->ProcessAnimJoint(&m_cAnimInfo);
		m_pModel->ProcessVisibility(&m_cAnimInfo);
		m_pModel->ProcessAnimUV(&m_cAnimInfo);
		m_pModel->ProcessAnimTexture(&m_cAnimInfo);
		m_pModel->ProcessAnimColor(&m_cAnimInfo);
		m_pModel->ProcessLocator(&m_cAnimInfo);
		m_pModel->ProcessParticleEmitter(&m_cAnimInfo);
		m_pModel->ProcessTrailEmitter(&m_cAnimInfo);
		m_pModel->ProcessEnvironmentMaps(m_pCamera->GetViewMatrix());
		m_pModel->ProcessTextureProjective(&m_cAnimInfo);

		if (m_bLightEnable)
		{
			m_pModel->SetModelColor(m_dwAmbientColor);
			D3DXVECTOR3 vL = m_pSunLight->GetLightDirection();
			D3DXVec3Normalize(&vL,&vL);
			m_pModel->ProcessDirectionalLight(m_pCamera->GetPosition(),&vL,Setting::GetInstance()->GetLightColor());
		}

		m_pModel->RenderModel();

		if (m_bShowNormal)
			m_pModel->_DEBUG_RenderNormal();
		if (m_bShowLocator)
			m_pModel->_DEBUG_RenderLocator();
		if (m_bShowSizeBox)
			m_pModel->_DEBUG_RenderSizeBox(m_cAnimInfo.GetWorldMatrix());
		if (m_bShowPickBox)
			m_pModel->_DEBUG_RenderPickBox(m_cAnimInfo.GetWorldMatrix());
		if (m_bShowBone)
			m_pModel->_DEBUG_RenderAction(&m_cAnimInfo);

		if (!m_vBindModels.empty())
		{
			for (int i = 0; i < (int)m_vBindModels.size(); ++i)
				m_vBindModels[i]->Display(m_pModel, &m_cAnimInfo);
		}
	}

	if (m_pRefModel)
	{
		D3DXMATRIX matWorld;
		D3DXMatrixIdentity(&matWorld);
		D3DXMatrixRotationY(&matWorld, m_fYRot);
		/*static AnimInfo */m_cAnimInfoRef;

		m_cAnimInfoRef.GetActionInfo()->dwCurAction = MAKEFOURCC('S','T','N','D');
		m_cAnimInfoRef.GetActionInfo()->bCurActionLooped = TRUE;
		m_cAnimInfoRef.SetCurrentTime(m_cAnimInfo.GetCurrentTime());
		m_cAnimInfoRef.SetWorldMatrix(&matWorld);
		m_cAnimInfoRef.SetBillboard(m_pCamera->GetBillboard());
		m_cAnimInfoRef.SetBillboardY(m_pCamera->GetBillboardY());

		m_pRefModel->ProcessAnimJoint(&m_cAnimInfoRef);
		m_pRefModel->ProcessVisibility(&m_cAnimInfoRef);
		m_pRefModel->ProcessAnimUV(&m_cAnimInfoRef);
		m_pRefModel->ProcessAnimTexture(&m_cAnimInfoRef);
		m_pRefModel->ProcessAnimColor(&m_cAnimInfoRef);

		m_pRefModel->ProcessLocator(&m_cAnimInfoRef);
		m_pRefModel->ProcessParticleEmitter(&m_cAnimInfoRef);
		m_pRefModel->ProcessTrailEmitter(&m_cAnimInfoRef);
		m_pRefModel->ProcessTextureProjective(&m_cAnimInfoRef);

		if (m_bLightEnable)
		{
			m_pRefModel->SetModelColor(m_dwAmbientColor);
			D3DXVECTOR3 vL = m_pSunLight->GetLightDirection();
			D3DXVec3Normalize(&vL,&vL);
			m_pRefModel->ProcessDirectionalLight(m_pCamera->GetPosition(),&vL,Setting::GetInstance()->GetLightColor());
		}
		else
			m_pRefModel->SetModelColor(0xFFFFFFFF);

		m_pRefModel->RenderModel();
	}

	particle::Manager::GetInstance()->Render();
	pLayer3D->Render();
	pLayer3D->Clear();
	DisplayTextureProjective();

	pInterface->GetDevice()->SetRenderTarget(0, m_pOldRenderTarget);
	pInterface->GetDevice()->SetDepthStencilSurface(m_pOldZStencil);

	char szSavePathName[_MAX_PATH];
	sprintf(szSavePathName, "%s/%08d%s", m_pWndAnimFrameEx->GetAnimFrameSaveDir(), m_dwAnimFrameIndex++, m_szpFileFormat);
	D3DXSaveTextureToFile(szSavePathName, m_IFF, m_pRenderTexture, NULL);

	SAFERELEASE(m_pOldRenderTarget);
	SAFERELEASE(m_pOldZStencil);
	SAFERELEASE(pRenderTarget);
	SAFERELEASE(m_pDepthStencilSurface);
	SAFERELEASE(m_pRenderTexture);
}



void WndViewer::ResetCamera(void)
{
	m_pCamera->SetPosition(&D3DXVECTOR3(0.0f,5.0f,4.0f));
	m_pCamera->SetView(&D3DXVECTOR3(0,0.25f,0));
}

void WndViewer::CreateTemplateModelFile()
{
}

void WndViewer::CheckFile()
{
	BROWSEINFO bi;
	char display[MAX_PATH] = "SelectName";
	char path[MAX_PATH];
	char name[MAX_PATH];
	char szFileName[MAX_PATH];

	ITEMIDLIST *pidl;
	bi.hwndOwner=NULL;	//AfxGetMainWnd()->GetSafeHwnd();
	bi.pidlRoot=0;
	bi.pszDisplayName = display;
	bi.lpszTitle = "选择目录";
	bi.ulFlags= BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_USENEWUI;
	bi.lpfn = 0;
	bi.lParam = 0;
	bi.iImage = 0;

	pidl = SHBrowseForFolder(&bi);

	if(!pidl)
		return ;
	SHGetPathFromIDList(pidl, path);

	strcpy(name,path);
	strcat(name,"\\*.*");

	struct _finddata_t c_file;
	intptr_t hFile;

	/* Find first .c file in current directory */
	if((hFile = _findfirst( name, &c_file )) != -1L)
	{
		//WORD wIndex = (WORD)atoi(c_file.name);
		//if (c_file.name[0] != '.')
		//{
		//	(szFileName,"%s\\%s\\model",path,c_file.name);
		//	OpenFile(szFileName);
		//}

		/* Find the rest of the .c files */
		do
		{
			if (strcmp(c_file.name, ".") && strcmp(c_file.name, "..") && !strcmp(c_file.name, "model"))
			{
				sprintf(szFileName,"%s\\model",path);
				OpenMainModelFile(szFileName);
			}
		}while(!_findnext(hFile, &c_file));

	/*	do
		{
			if (!strcmp(files.name, ".") || !strcmp(files.name, ".."))
				continue;

			if (uFileCount > MAX_FILE)
				break;

			sTmpFileName[uFileCount++] = files.name;
		}while (!_findnext(iFileHandle, &files));*/

		_findclose( hFile );
	}
}



/*
 * 功能: 打开主模型文件
 * 摘要: -
 * 参数: 模型文件名
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2007.12.19
 */
void WndViewer::OpenMainModelFile(const char * pszFileName)
{
	// 判断输入的正确性
	if (string(pszFileName).find("model") == string::npos)
	{
		MessageBox(NULL, "输入的路径下没有找到 model 文件\n请确认路径是否正确", "Error:", MB_OK);
		return;
	}

	string			sMainModelName;
	DWORD			dwCurrentTime = timeGetTime();
	CRoleModel	  * pRoleModel	  = new CRoleModel();
	CBindModel	  * pBindModel	  = NULL;
	CDisplayModel * pModel		  = CGameModelManager::GetInstance()->CreateDisplayModel(pszFileName,false);
	VERIFY(pModel);

	pModel->ShowAllGroup(false);
	pModel->EnableParticleEmitter(TRUE);
	pModel->EnableTrailEmitter(TRUE);

	pRoleModel->SetDisplayModel(pModel);
	pRoleModel->SetFilePathName(string(pszFileName));
	m_vRoleModels.push_back(pRoleModel);
	pModel = NULL;

	m_cAnimInfo.GetActionInfo()->dwCurAction	= 0;
	m_cAnimInfo.GetActionInfo()->bInterpolation = FALSE;
	m_cAnimInfo.SetTextureProjectiveTime(dwCurrentTime);
	m_cAnimInfo.SetAnimTime(dwCurrentTime);

	//设置菜单
	//清除原来菜单
	DestroyMenuBar();
	CreateMenuBar();
	CreateMainModelMenu(pszFileName);

	if (!m_vBindModels.empty())
	{
		pBindModel = m_vBindModels[m_pWndModels->GetSelBindModel()];
		CreateBindModelMenu(pBindModel);
	}

	GetModelName(sMainModelName, string(pszFileName));
	m_pWndModels->AddRoleModel(sMainModelName.c_str());
	m_pWndModels->ShowWnd(true);
	m_pWndAction->SetCurrentActionType(AT_ROLE);
	m_pWndAction->SetCurrentModelName(sMainModelName.c_str());
	m_pWndAction->ShowWnd(true);
	m_pWndSelectLevel->ShowWnd(false);
	m_pWndModelTree->CreateModelTree(pszFileName);
	m_pWndModelTree->SetCurrentModelName(sMainModelName.c_str());
	CreateActionMenu(pszFileName);

	ZeroMemory(&m_vRolePos, sizeof(D3DXVECTOR3));
}



/*
 * 功能: 打开绑定模型文件
 * 摘要: -
 * 参数: 模型文件名
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.01.02
 */
void WndViewer::OpenBindModelFile(const char * pszFileName)
{
	// 判断输入的正确性
	if (string(pszFileName).find("model") == string::npos)
	{
		MessageBox(NULL, "输入的路径下没有找到 model 文件\n请确认路径是否正确", "Error:", MB_OK);
		return;
	}
	if (m_pModel == NULL)
	{
		MessageBox(NULL, "打开绑定模型前必须首先打开角色主模型！", "Tip:", MB_OK);
		return;
	}

	string					  sBindModelName;
	DWORD					  dwCurTime   = 0;
	CBindModel				* pBindModel  = NULL;
	AnimInfo::tagActionInfo * pActionInfo = NULL;

	// 设定创建新绑定模型,并设定其相关属性
	pBindModel = new CBindModel();
	pBindModel->SetLevel(0);
	pBindModel->SetVisible(true);
	pBindModel->SetDisplayModel(pszFileName);
	pBindModel->SetFilePathName(string(pszFileName));

	m_vBindModels.push_back(pBindModel);

	// 创建主菜单条目
	DestroyMenuBar();
	CreateMenuBar();
	CreateMainModelMenu((m_vRoleModels[m_pWndModels->GetSelRoleModel()]->GetFilePathName()).c_str());
	CreateBindModelMenu(pBindModel);

	GetModelName(sBindModelName, string(pszFileName));
	m_pWndModels->AddBindModel(sBindModelName.c_str());
	m_pWndModels->ShowWnd(true);
	m_pWndLocator->ShowWnd(true);
	m_pWndLocator->SetCurrentSelBindModelName(sBindModelName.c_str());
	m_pWndLocator->SetCurrentSelBindModelIndex((int)m_vBindModels.size() - 1);
	m_pWndLocator->ClearAllItemChecked();
	m_pWndSelectLevel->ShowWnd(false);

	CreateActionMenu(pszFileName);

	m_pWndAction->SetCurrentModelIndex((int)m_vBindModels.size() - 1);
	m_pWndAction->SetCurrentModelName(sBindModelName.c_str());
	m_pWndAction->SetCurrentActionType(AT_BIND);
	m_pWndAction->SetActionItemCheck();

	pBindModel = NULL;
}



/*
 * 功能: 打开参考模型文件
 * 摘要: -
 * 参数: 模型文件名
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2007.12.28
 */
void WndViewer::OpenRefModelFile(const char * pszFileName)
{
	// 判断输入的正确性
	if (string(pszFileName).find("model") == string::npos)
	{
		MessageBox(NULL, "输入的路径下没有找到 model 文件\n请确认路径是否正确", "Error:", MB_OK);
		return;
	}

	if (m_pRefModel)
	{
		CGameModelManager::GetInstance()->ReleaseDisplayModel(m_pRefModel);
		CGameModelManager::GetInstance()->ReleaseGameModel(m_pRefModel->GetGameModelID());
		m_pRefModel = NULL;
	}

	m_pRefModel = CGameModelManager::GetInstance()->CreateDisplayModel(pszFileName);
	//VERIFY(m_pModel->Create(pszFileName));
	m_pRefModel->ShowAllGroup();
	m_pRefModel->EnableParticleEmitter(TRUE);
	m_pRefModel->EnableTrailEmitter(TRUE);

	//m_pRefModel = new CDisplayModel;
	//VERIFY(m_pRefModel->Create(pszFileName));
	//m_pRefModel->SetGroupVisible(TRUE);
	//m_pRefModel->SelectDefaultPart();
}



/*
 * 功能: 打开背景图片文件
 * 摘要: -
 * 参数: 图片文件名
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.01.07
 */
void WndViewer::OpenBKPicFile(const char * pszFileName)
{
	render::Interface * pInterface = render::Interface::GetInstance();

	// 如果存在背景图片,则首先销毁它
	if (m_dwBK != 0)
		pInterface->DestroyTexture(m_dwBK);

	// 创建新的背景图片
	m_dwBK = pInterface->CreateTexture(pszFileName);
	VERIFY(m_dwBK);

	m_ptBKTexInfo = pInterface->GetTextureInfo(m_dwBK);
	m_bSwitchBK	  = TRUE;
}



/*
 * 功能: 打开地表纹理
 * 摘要: 地表纹理将会以平铺的方式显示在水平地表上
 * 参数: 纹理文件名
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.01.07
 */
void WndViewer::OpenGridTexFile(const char * pszFileName)
{
	render::Interface * pInterface = render::Interface::GetInstance();

	// 如果存在背景图片,则首先销毁它
	if (m_dwGridTex != 0)
		pInterface->DestroyTexture(m_dwGridTex);

	// 创建新的背景图片
	m_dwGridTex = pInterface->CreateTexture(pszFileName);
	VERIFY(m_dwGridTex);
}



/*
 * 功能: 创建主模型菜单
 * 摘要: -
 * 参数: 模型文件名
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2007.12.28
 */
void WndViewer::CreateMainModelMenu(const char * pszFileName)
{
	DWORD dwShaderGroupHandle;
	DWORD dwShaderCount = 0;
	GameModel								* pGM;
	utility::File							  file;
	utility::Text							  script;
	utility::Text::Node						* pGroupListNode;
	render::Interface						* pInterface;
	render::Interface::Layer3D				* pLayer3D;
	render::Interface::Layer2D				* pLayer2D;
	render::Interface::Layer3D::ShaderGroup * pShaderGroup;
	int										  iItemHeight;

	file.Open(pszFileName);
	script.Create(&file);

	CDisplayModel * pDisplayModel = m_vRoleModels[m_vRoleModels.size() - 1]->GetDisplayModel();

	pGM						= CGameModelManager::GetInstance()->GetGameModel(pDisplayModel->GetGameModelID());
	dwShaderGroupHandle		= pGM->GetShaderGroupHandle();
	pInterface				= render::Interface::GetInstance();
	pLayer3D				= pInterface->GetLayer3D();
	pLayer2D				= pInterface->GetLayer2D();
	pShaderGroup			= pLayer3D->GetShaderGroup(dwShaderGroupHandle);
	m_dwGroupMenuItemCount	= 0;
	iItemHeight				= pLayer2D->GetFontSize() + FWA_FRAMESIZE * 2;

	if (pShaderGroup)
		dwShaderCount = pShaderGroup->GetShaderCount();

	pGroupListNode = script.GetRootNode()->GetChildNode("group");
	if (pGroupListNode)
	{
		DWORD code;
		for(DWORD n = 0; n < pGroupListNode->GetChildCount() ;n ++)
		{
			utility::Text::Node *pGroupNode = pGroupListNode->GetChildNode(n);
			DWORD dwMenu = m_pMenuBar->AddMenu(pGroupNode->GetName());
			++m_dwGroupMenuItemCount;
			ui::MenuBar::Menu *pMenu = m_pMenuBar->GetMenu(dwMenu);

			for(DWORD m = 0; m < pGroupNode->GetChildCount(); m++)
			{
				utility::Text::Node *pPartNode = pGroupNode->GetChildNode(m);
				pMenu->AddItem(pPartNode->GetName());

				code = 0;
				strncpy((char*)&code,pMenu->GetText(),4);
				DWORD dwGroup = code;
				strncpy((char*)&code,pMenu->GetItemText(m),4);
				DWORD dwPart  = code;

				//检查
				GameModel::Group * pGroup = pGM->GetGroup(dwGroup);
				if (pGroup)
				{
					GameModel::Group::Part *pPart = pGroup->GetPart(dwPart);
					if (pPart)			//pPart有时会为空
					{
						for(DWORD i = 0; i < pPart->GetRenderMeshArrayCount();i++)
						{
							GameModel::Group::Part::tagRenderMesh *pRMA = pPart->GetRenderMeshArray();
							if (pRMA[i].pMesh && pRMA[i].pMesh->GetShaderIndex() >= dwShaderCount)		//pRMA[i].pMesh有时候为空
							{
								char szTemp[256];
								_snprintf(szTemp,255,"组别:[%s] 部件:[%s] 编号:[%d] 的MESH文件SHADER文件索引错误,有可能是导出Mesh时候没有对应导出Shader文件.",
									pGroupNode->GetName(),pPartNode->GetName(),i);
								MessageBox(NULL,szTemp,"文件数据错误",MB_OK);
							}
						}
					}
				}

				if (!m)
				{
					pMenu->SetCheckItem(0,TRUE);
					pMenu->SetSelItem(0);
					
					pGM->SetGroupVisible(dwGroup,TRUE);
					pGM->SelectPart(dwGroup,dwPart);
				}
			}

			// 超出屏幕高度的处理
			if ((int)pGroupNode->GetChildCount() * iItemHeight + 50 >= pInterface->GetSetting()->iWndHeight)
			{
				pMenu->ClearItem();
				DWORD dwItem = pMenu->AddItem("More");
			}
		}
	}

	file.Close();
	script.Destroy();
}



/*
 * 功能: 创建动作菜单
 * 摘要: -
 * 参数: 模型文件名
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.01.02
 */
void WndViewer::CreateActionMenu(const char * pszFileName)
{
	utility::File file;
	utility::Text script;
	file.Open(pszFileName);
	script.Create(&file);

	// 动画菜单
	m_pWndAction->ClearAction();
	utility::Text::Node * pActionNode = script.GetRootNode()->GetChildNode("action");
	if (pActionNode)
	{
		for(DWORD n = 0; n < pActionNode->GetChildCount(); ++n)
			m_pWndAction->AddAction(pActionNode->GetChildNode(n)->GetName());
	}
	file.Close();
	script.Destroy();
}



/*
 * 功能: 创建部件列表菜单
 * 摘要: -
 * 参数: 模型文件名
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2008.01.02
 */
void WndViewer::CreatePartList(const char * pszFileName)
{
	DWORD dwShaderCount = 0;
	utility::File file;
	utility::Text script;
	utility::Text::Node * pGroupListNode;

	file.Open(pszFileName);
	script.Create(&file);

	m_pWndPart->ClearPart();

	pGroupListNode = script.GetRootNode()->GetChildNode("group");
	if (pGroupListNode)
	{
		utility::Text::Node * pGroupNode = pGroupListNode->GetChildNode(m_pWndPart->GetCurrentGroupName());
		for(DWORD m = 0; m < pGroupNode->GetChildCount(); m++)
		{
			utility::Text::Node * pPartNode = pGroupNode->GetChildNode(m);
			m_pWndPart->AddPart(pPartNode->GetName());
		}
	}

	file.Close();
	script.Destroy();
}



/*
 * 功能: 创建主菜单绑定模型条目
 * 摘要: -
 * 参数: 绑定装备指针
 * 返回值: -
 * 作者: lpf
 * 创建日期: 2007.12.28
 */
void WndViewer::CreateBindModelMenu(CBindModel * pBindModel)
{
	// 打开模型文件
	utility::File file;
	utility::Text script;
	file.Open(pBindModel->GetFilePathName().c_str());
	script.Create(&file);

	m_pMenuBar->DisableMenu(m_pMenuBar->AddMenu("|"));
	GameModel * pGM = CGameModelManager::GetInstance()->GetGameModel(pBindModel->GetDisplayModel()->GetGameModelID());
	DWORD dwShaderGroupHandle = pGM->GetShaderGroupHandle();
	render::Interface * pInterface = render::Interface::GetInstance();
	render::Interface::Layer3D * pLayer3D = pInterface->GetLayer3D();
	render::Interface::Layer2D * pLayer2D = pInterface->GetLayer2D();
	render::Interface::Layer3D::ShaderGroup * pShaderGroup = pLayer3D->GetShaderGroup(dwShaderGroupHandle);
	int		  iItemHeight				= pLayer2D->GetFontSize() + FWA_FRAMESIZE * 2;

	DWORD dwShaderCount = 0;
	if (pShaderGroup)
		dwShaderCount = pShaderGroup->GetShaderCount();

	utility::Text::Node * pGroupListNode = script.GetRootNode()->GetChildNode("group");
	if (pGroupListNode)
	{
		DWORD code;
		for(DWORD n = 0; n < pGroupListNode->GetChildCount() ;n ++)
		{
			utility::Text::Node * pGroupNode = pGroupListNode->GetChildNode(n);
			DWORD dwMenu = m_pMenuBar->AddMenu(pGroupNode->GetName());
			m_dwWeaponLeftMenuItemIndex = m_pMenuBar->GetMenuCnt() - 1;
			ui::MenuBar::Menu * pMenu = m_pMenuBar->GetMenu(dwMenu);

			for(DWORD m = 0; m < pGroupNode->GetChildCount(); m++)
			{
				utility::Text::Node *pPartNode = pGroupNode->GetChildNode(m);
				DWORD dwItem = pMenu->AddItem(pPartNode->GetName());

				code = 0;
				strncpy((char*)&code,pMenu->GetText(),4);
				DWORD dwGroup = code;
				strncpy((char*)&code,pMenu->GetItemText(m),4);
				DWORD dwPart  = code;

				//检查
				GameModel::Group * pGroup = pGM->GetGroup(dwGroup);
				if (pGroup)
				{
					GameModel::Group::Part * pPart = pGroup->GetPart(dwPart);
					for(DWORD i = 0; i < pPart->GetRenderMeshArrayCount();i++)
					{
						GameModel::Group::Part::tagRenderMesh * pRMA = pPart->GetRenderMeshArray();
						if (pRMA[i].pMesh && pRMA[i].pMesh->GetShaderIndex() >= dwShaderCount)
						{
							char szTemp[256];
							_snprintf(szTemp,255,"组别:[%s] 部件:[%s] 编号:[%d] 的MESH文件SHADER文件索引错误,有可能是导出Mesh时候没有对应导出Shader文件.",
								pGroupNode->GetName(),pPartNode->GetName(),i);
							MessageBox(NULL, szTemp, "文件数据错误", MB_OK);
						}
					}
				}

				if (!m)
				{
					pMenu->SetCheckItem(0, TRUE);
					pMenu->SetSelItem(0);
					
					pGM->SetGroupVisible(dwGroup, TRUE);
					pGM->SelectPart(dwGroup, dwPart);
				}
			}

			// 超出屏幕高度的处理
			if ((int)pGroupNode->GetChildCount() * iItemHeight + 50 >= pInterface->GetSetting()->iWndHeight)
			{
				pMenu->ClearItem();
				DWORD dwItem = pMenu->AddItem("More");
			}
		}
	}

	// 销毁文件
	file.Close();
	script.Destroy();
}



/*
 * 功能: 检测键盘单击事件
 * 摘要: -
 * 参数: 键盘状态缓冲
 *		 按键扫描码
 * 返回值: 如果单击状态返回真,否则返回假
 * 作者: lpf
 * 创建日期: 2007.12.28
 */
bool WndViewer::KeyClick(BYTE * pbyKeyBuffer, int iScanCodes)
{
	if (pbyKeyBuffer[iScanCodes] & 0x80)
	{
		if (!m_bKeyState[iScanCodes])
		{
			m_bKeyState[iScanCodes] = true;
			return true;
		}
	}else
		m_bKeyState[iScanCodes] = false;

	return false;
}