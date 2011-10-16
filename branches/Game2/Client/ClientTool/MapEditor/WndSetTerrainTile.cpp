#include "stdafx.h"
#include "wndsetterraintile.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"
#include "..\..\engine\render.h"
#include "CommDB.h"
#include "TerrainMesh.h"

WndSetTerrainTile::WndSetTerrainTile(void)
{
	m_bIsPopupCheckFlag = FALSE;
	m_bIsOK = FALSE;
}

WndSetTerrainTile::~WndSetTerrainTile(void)
{
}
BOOL WndSetTerrainTile::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);
	RECT rcSetStatic = { 40,30,50,50 };
	m_pSSet = new ui::Static;
	m_pSSet->Create("设置SET",&rcSetStatic,FWS_VISIBLE ,this);

	RECT rcSetEditBox = { 80,30,120,50 };
	m_pEdBox = new ui::EditBox;
	m_pEdBox->Create("SET值",&rcSetEditBox,FWS_VISIBLE|FWS_OUTLINE |FWS_FILLBKG|FWS_SHADOW,this);

	RECT rcSetButton = { 70,70,90,90 };
	m_pBOk = new ui::Button;
	m_pBOk->Create("确定",&rcSetButton,FWS_VISIBLE,this);
	
	return TRUE;
	
}
void WndSetTerrainTile::Render()
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
void WndSetTerrainTile::Update()
{
	ui::Wnd::UpdateDefault();
	ui::Wnd::UpdateChild();
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		if (m_pBOk->IsClicked())
		{
			m_bIsOK = TRUE;
			char str[4];
			m_pEdBox->GetEditText(str,4);
			DWORD i = atoi(str);
			if (i < 0 || i > 500)
			{
				i = 0;
			}
			TerrainMesh *pTerrainMesh = CommDB::GetInstance()->GetTerrainMesh();
			pTerrainMesh->SetTexSetIndex(i);
			TCHAR szShaderPath[_MAX_PATH];
			_snprintf(szShaderPath,_MAX_PATH,"terrain\\set%d\\shader",i);
			pTerrainMesh->CreateShader(szShaderPath);
		}
	}
}
void WndSetTerrainTile::Destroy()
{
	SAFEDESTROY(m_pBOk);
	SAFEDESTROY(m_pEdBox);
	SAFEDESTROY(m_pSSet);
	ui::Wnd::Destroy();
}
void WndSetTerrainTile::BeginPopUp()
{
	m_bIsOK = FALSE;
	m_bIsPopupCheckFlag= TRUE;
	ModifyStyle(FWS_VISIBLE,0);
	ui::Manager::GetInstance()->BeginPopupWnd(this);
}
void WndSetTerrainTile::EndPopUp()
{
	m_bIsPopupCheckFlag= FALSE;
	ModifyStyle(0,FWS_VISIBLE);
	ui::Manager::GetInstance()->EndPopupWnd();
}