#include "stdafx.h"

#include "..\..\engine\utility.h"
#include "..\..\engine\console.h"
#include "..\..\engine\render.h"

#include "commdb.h"
#include "map.h"
#include "wndlightadjust.h"

WndLightAdjust::WndLightAdjust(void)
{
	m_bIsPopupCheckFlag = FALSE;
	m_bIsOK = FALSE;
	m_bPickColor = FALSE;
}

WndLightAdjust::~WndLightAdjust(void)
{
	
}

BOOL WndLightAdjust::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	//create control
	RECT rcAmbientStatic= { 7,13,60,36};
	m_pStaticAmbient = new ui::Static;
	m_pStaticAmbient->Create("环境光:",&rcAmbientStatic,FWS_VISIBLE ,this);

	RECT rcDirectionalStatic= { 99,11,153,33};
	m_pStaticDirectional = new ui::Static;
	m_pStaticDirectional->Create("方向光:",&rcDirectionalStatic,FWS_VISIBLE ,this);

	RECT rcOK = { 24,54,80,80};
	m_pBtnOK = new ui::Button;
	m_pBtnOK->Create("确定",&rcOK,FWS_VISIBLE,this);

	RECT rcCancel = { 126,54,176,80};
	m_pBtnCancel = new ui::Button;
	m_pBtnCancel->Create("取消",&rcCancel,FWS_VISIBLE,this);

	RECT rcAmbient = { 62,14,85,35};
	m_pBtnAmbient = new ColrBtn;
	m_pBtnAmbient->Create("颜色",&rcAmbient,FWS_VISIBLE,this);
	m_pBtnAmbient->SetColor(0xFFFFFFFF);

	RECT rcDirectional = { 161,14,184,35};
	m_pBtnDirectional = new ColrBtn;
	m_pBtnDirectional->Create("颜色",&rcDirectional,FWS_VISIBLE,this);
	m_pBtnDirectional->SetColor(0xFFFFFFFF);

	render::Interface * pInteface = render::Interface::GetInstance();
	m_dwColorMapHandle = pInteface->CreateTexture("ui\\color.tga");
	render::Texture   * pTexture  = pInteface->GetTexture(m_dwColorMapHandle );
	m_dwColorMapWidth  = pTexture->GetWidth(); 
	m_dwColorMapHeight = pTexture->GetHeight(); 
			   
	return TRUE;
}

void WndLightAdjust::Destroy()
{
	render::Interface * pInteface = render::Interface::GetInstance();
	pInteface->DestroyTexture(m_dwColorMapHandle);
	SAFEDESTROY(m_pStaticDirectional);
	SAFEDESTROY(m_pBtnDirectional);
	SAFEDESTROY(m_pBtnAmbient);
	SAFEDESTROY(m_pStaticAmbient);
	SAFEDESTROY(m_pBtnCancel);
	SAFEDESTROY(m_pBtnOK);
	ui::Wnd::Destroy();
}

void WndLightAdjust::Update()
{
	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();
	
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();
	render::Interface *pInterface = render::Interface::GetInstance();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		if (m_bPickColor)
		{
			if (pInputInfo->eType == UIMT_MS_BTNUP && pInputInfo->dwData == MK_LBUTTON)
			{
				m_bPickColor = FALSE;
			}
			else
			{
				//检查颜色
				DWORD px = (DWORD)pInputInfo->ptMouse.x;
				DWORD py = (DWORD)pInputInfo->ptMouse.y;
				if (px > m_dwPickX && py > m_dwPickY &&
					px < m_dwPickX + m_dwColorMapWidth && 
					py < m_dwPickX + m_dwColorMapHeight)
				{
					//计算位置
					int iPosX = px - m_dwPickX;
					int iPosY = py - m_dwPickY;
					//取得颜色
					render::Texture   * pTexture  = pInterface->GetTexture(m_dwColorMapHandle);
					DWORD dwColor = pTexture->GetPixel(iPosX,iPosY);
					if (m_ePickType == PICK_AMBIENT)
					{
						m_pBtnAmbient->SetColor(dwColor);
						EditMap *pMap = CommDB::GetInstance()->GetMap();
						pMap->SetSunLightAmbientColor(dwColor);
					}
					else
					{
						m_pBtnDirectional->SetColor(dwColor);
						EditMap *pMap = CommDB::GetInstance()->GetMap();
						pMap->SetSunLightDiffuseColor(dwColor);
					}
				}
			}
			
		}
		else
		{
			if (m_pBtnOK->IsClicked())
			{
				EndPopUp();
				m_bIsOK = TRUE;
				m_bIsPopupCheckFlag= TRUE;
			}
			if (m_pBtnCancel->IsClicked())
			{
				EndPopUp();
				m_bIsOK = FALSE;
				m_bIsPopupCheckFlag = TRUE;
			}
			if (m_pBtnAmbient->IsClicked())
			{
				m_ePickType = PICK_AMBIENT;
				m_bPickColor = TRUE;
			}
			if (m_pBtnDirectional->IsClicked())
			{
				m_ePickType = PICK_DIRECTIONAL;
				m_bPickColor = TRUE;
			}
			ui::Wnd::UpdateChild();
		}
		
	}
}

void WndLightAdjust::Render()
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

		RECT &rc = m_tWindowInfo.rcRect;
		if (m_bPickColor)
		{
			RECT rcRect;
			if (m_ePickType == PICK_AMBIENT)
			{
				m_pBtnAmbient->GetWndRect(&rcRect);

				m_dwPickX = rcRect.right; 
				m_dwPickY = rcRect.bottom;
			}
			else
			{
				m_pBtnDirectional->GetWndRect(&rcRect);

				m_dwPickX = rcRect.right; 
				m_dwPickY = rcRect.bottom;
            			
			}
			pLayer2D->DrawTexture(m_dwPickX,m_dwPickY,m_dwColorMapHandle);
		}
	}				   
	
}

void WndLightAdjust::BeginPopUp(void)
{
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	if (!pMap) return;
	m_dwOldAmbientColor = pMap->GetSunLightAmbientColor();
	m_dwOldDirectionalColor = pMap->GetSunLightDiffuseColor();
	m_pBtnAmbient->SetColor(m_dwOldAmbientColor);
	m_pBtnDirectional->SetColor(m_dwOldDirectionalColor);
	ModifyStyle(FWS_VISIBLE,0);
	ui::Manager::GetInstance()->BeginPopupWnd(this);
}

void WndLightAdjust::EndPopUp()
{
	ModifyStyle(0,FWS_VISIBLE);
	ui::Manager::GetInstance()->EndPopupWnd();
	EditMap *pMap = CommDB::GetInstance()->GetMap();
	pMap->SetSunLightAmbientColor(m_dwOldAmbientColor);
	pMap->SetSunLightDiffuseColor(m_dwOldDirectionalColor);
}


void WndLightAdjust::ColrBtn::Render()
{
	//NO DRAW DEFAULT
	ui::Wnd::RenderDefault();

	render::Interface *pInterface = render::Interface::GetInstance();
	ASSERT(pInterface);

	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
	ASSERT(pLayer2D);
	
	//SELF
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		RECT &rc = m_tWindowInfo.rcRect;
		
		int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;

		pLayer2D->DrawSolidQuad(iX,iY,iW,iH,m_dwColor);	
	}
	//NO CHILD
}