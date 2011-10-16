#include "stdafx.h"
#include "wndsetskybox.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"
#include "..\..\engine\render.h"
#include "..\..\EngineExtend\gameskybox.h"
#include "editwnd.h"
#include "CommDB.h"
#include "CommUI.h"
#include "CommUI.h"

WndSetSkyBox::WndSetSkyBox(void)
{
	m_bIsPopupCheckFlag = FALSE;
	m_bIsOK = FALSE;
}

WndSetSkyBox::~WndSetSkyBox(void)
{
}
BOOL WndSetSkyBox::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);
	RECT rcSetIDStatic = { 60,30,70,50 };
	m_pSSetSkyBoxID = new ui::Static;
	m_pSSetSkyBoxID->Create("设置SKYBOXID",&rcSetIDStatic,FWS_VISIBLE ,this);

	RECT rcSetIDEditBox = { 130,30,170,50 };
	m_pEdBoxSkyBoxID = new ui::EditBox;
	m_pEdBoxSkyBoxID->Create("设置SKYBOXID",&rcSetIDEditBox,FWS_VISIBLE|FWS_OUTLINE |FWS_FILLBKG|FWS_SHADOW,this);

	RECT rcSetScalStatic = { 60,90,70,110 };
	m_pSSetSkyBoxScal = new ui::Static;
	m_pSSetSkyBoxScal->Create("设置SKYBOX缩放",&rcSetScalStatic,FWS_VISIBLE ,this);

	RECT rcSetScalEditBox = { 130,90,170,110 };
	m_pEdBoxSkyBoxScal = new ui::EditBox;
	m_pEdBoxSkyBoxScal->Create("设置SKYBOX缩放",&rcSetScalEditBox,FWS_VISIBLE|FWS_OUTLINE |FWS_FILLBKG|FWS_SHADOW,this);

	RECT rcSetButton = { 70,120,90,140 };
	m_pBOk = new ui::Button;
	m_pBOk->Create("确定",&rcSetButton,FWS_VISIBLE,this);

	return TRUE;

}
void WndSetSkyBox::Render()
{
	//NO DRAW DEFAULT
	render::Interface *pInterface = render::Interface::GetInstance();
	ASSERT(pInterface);

	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
	ASSERT(pLayer2D);

	//SELF
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		ui::Wnd::RenderDefault();
		//NO CHILD
		ui::Wnd::RenderChild();	
	}

}
void WndSetSkyBox::Update()
{
	ui::Wnd::UpdateDefault();
	ui::Wnd::UpdateChild();
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		if (m_pBOk->IsClicked())
		{
			m_bIsOK = TRUE;
			char strID[8];
			m_pEdBoxSkyBoxID->GetEditText(strID,4);
			DWORD id = atoi(strID);
			char strScal[20];
			m_pEdBoxSkyBoxScal->GetEditText(strScal,8);
			float fscal = (float)atof(strScal);
			EditWnd* pEditWnd = CommUI::GetInstance()->GetEditWnd();
			GameSkybox*& pskybox = pEditWnd->GetSkyBox();
			SAFEDESTROY(pskybox);
			pskybox = new GameSkybox;
			if (pskybox->Create(id))
			{
				pskybox->SetScal(fscal);
			}
			else
			{
				SAFEDESTROY(pskybox);
			}
		/*	TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
			pTerrainMesh->SetTexSetIndex(i);
			TCHAR szShaderPath[_MAX_PATH];
			_snprintf(szShaderPath,_MAX_PATH,"terrain\\set%d\\shader",i);
			pTerrainMesh->CreateShader(szShaderPath);*/
		}
	}
}
void WndSetSkyBox::Destroy()
{
	SAFEDESTROY(m_pBOk);
	SAFEDESTROY(m_pEdBoxSkyBoxID);
	SAFEDESTROY(m_pSSetSkyBoxID);
	SAFEDESTROY(m_pEdBoxSkyBoxScal);
	SAFEDESTROY(m_pSSetSkyBoxScal);
	ui::Wnd::Destroy();
}
void WndSetSkyBox::BeginPopUp()
{
	m_bIsOK = FALSE;
	m_bIsPopupCheckFlag= TRUE;
	ModifyStyle(FWS_VISIBLE,0);
	ui::Manager::GetInstance()->BeginPopupWnd(this);
}
void WndSetSkyBox::EndPopUp()
{
	m_bIsPopupCheckFlag= FALSE;
	ModifyStyle(0,FWS_VISIBLE);
	ui::Manager::GetInstance()->EndPopupWnd();
}