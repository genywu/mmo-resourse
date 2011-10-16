#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"

#include "gamesetting.h"
#include "WndAdjust.h"

WndEditBGColor::WndEditBGColor(void)
{
	m_bIsPopupCheckFlag = FALSE;
	m_bIsOK = FALSE;
	m_bPickColor = FALSE;
}

WndEditBGColor::~WndEditBGColor(void)
{
	
}

BOOL WndEditBGColor::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	//create control
	RECT rcBKColorStatic= { 7,13,60,36};
	m_pStaticBKColor = new ui::Static;
	m_pStaticBKColor->Create("背景:",&rcBKColorStatic,FWS_VISIBLE ,this);

	RECT rcOK = { 24,54,80,80};
	m_pBtnOK = new ui::Button;
	m_pBtnOK->Create("确定",&rcOK,FWS_VISIBLE,this);

	RECT rcCancel = { 126,54,176,80};
	m_pBtnCancel = new ui::Button;
	m_pBtnCancel->Create("取消",&rcCancel,FWS_VISIBLE,this);

	RECT rcBKColor = { 62,14,85,35};
	m_pBtnBKColor = new ColrBtn;
	m_pBtnBKColor->Create("颜色",&rcBKColor,FWS_VISIBLE,this);
	m_pBtnBKColor->SetColor(0xFFFFFFFF);


	render::Interface * pInteface = render::Interface::GetInstance();
	m_dwColorMapHandle = pInteface->CreateTexture("ui\\color.tga");
	render::Texture   * pTexture  = pInteface->GetTexture(m_dwColorMapHandle );
	m_dwColorMapWidth  = pTexture->GetWidth(); 
	m_dwColorMapHeight = pTexture->GetHeight(); 
			   
	return TRUE;
}

void WndEditBGColor::Destroy()
{
	render::Interface * pInteface = render::Interface::GetInstance();
	pInteface->DestroyTexture(m_dwColorMapHandle);
	SAFEDESTROY(m_pBtnBKColor);
	SAFEDESTROY(m_pStaticBKColor);
	SAFEDESTROY(m_pBtnCancel);
	SAFEDESTROY(m_pBtnOK);
	ui::Wnd::Destroy();
}

void WndEditBGColor::Update()
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
					m_pBtnBKColor->SetColor(dwColor);
					Setting::GetInstance()->SetBackGroundColor(dwColor);
					m_bPickColor = FALSE;				
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
			if (m_pBtnBKColor->IsClicked())
			{
				m_bPickColor = TRUE;
			}
			ui::Wnd::UpdateChild();
		}
		
	}
}

void WndEditBGColor::Render()
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

		if (m_bPickColor)
		{
			RECT rcRect;
			m_pBtnBKColor->GetWndRect(&rcRect);

			m_dwPickX = rcRect.right; 
			m_dwPickY = rcRect.bottom;
			pLayer2D->DrawTexture(m_dwPickX,m_dwPickY,m_dwColorMapHandle);
		}
	}				   
	
}

void WndEditBGColor::BeginPopUp(void)
{
	m_dwOldBKColor = Setting::GetInstance()->GetBackGroundColor();
	m_pBtnBKColor->SetColor(m_dwOldBKColor);
	ModifyStyle(FWS_VISIBLE,0);
	ui::Manager::GetInstance()->BeginPopupWnd(this);
}

void WndEditBGColor::EndPopUp()
{
	ModifyStyle(0,FWS_VISIBLE);
	ui::Manager::GetInstance()->EndPopupWnd();
	m_dwOldBKColor = Setting::GetInstance()->GetBackGroundColor();
	Setting::GetInstance()->SetBackGroundColor(m_dwOldBKColor);
}


void WndEditBGColor::ColrBtn::Render()
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