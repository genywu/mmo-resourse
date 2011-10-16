#include "StdAfx.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "control.h"

RadioButton::RadioButton(void)
{
	m_dwTextureHandle = 0;
	m_bClicked = FALSE;
	m_bChecked = FALSE;
	m_bDisplayText = FALSE;

	m_rc.top = m_rc.bottom = m_rc.left = m_rc.right;
}

RadioButton::~RadioButton(void)
{
}

BOOL RadioButton::IsClicked()
{
	BOOL bClicked = m_bClicked;
	m_bClicked= FALSE;
	return bClicked;
}

void RadioButton::Render()
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

		RECT &rc = m_tWindowInfo.rcRect;
		
		int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;
		int iLen = (int)strlen(m_tWindowInfo.szName);

		int iTextStartX = iX + iW/2 - pLayer2D->CalcTextWidth(m_tWindowInfo.szName,iLen)/2;
		int iTextStartY = iY + iH/2 - pLayer2D->GetFontSize()/2;

		static D3DCOLOR cHigh   = m_tWindowInfo.colorHigh;
		static D3DCOLOR cLow    = m_tWindowInfo.colorLow;
		static D3DCOLOR cMiddle = m_tWindowInfo.colorMiddle;
		static D3DCOLOR cText   = m_tWindowInfo.colorText;
		static D3DCOLOR cBack   = m_tWindowInfo.colorBack;
		static D3DCOLOR cFrame  = 0xffffffFF;

		//pLayer2D->DrawWireQuad(iX+FWA_FRAMESIZE,iY+FWA_FRAMESIZE,iW,iH,cLow);

		//shadow
		if(m_bChecked)
		{
			if(m_dwTextureHandle != 0)
			{
				pLayer2D->DrawTextureSubRect(iX - 4,iY - 4,iW + 8,iH + 8,&m_rc,m_dwTextureHandle);
			}
			pLayer2D->DrawWireQuad(iX - 4,iY - 4,iW + 8,iH + 8,cFrame);
		}
		else
		{
			if(m_dwTextureHandle != 0)
			{
				pLayer2D->DrawTextureSubRect(iX,iY,iW,iH,&m_rc,m_dwTextureHandle);
			}
		}
		if (m_bDisplayText)
		{
			ui::Wnd::SetUIRendState();
			pLayer2D->TextOut(iX+iW,iY + iH /2,m_tWindowInfo.szName,lstrlen(m_tWindowInfo.szName),m_tWindowInfo.colorText);
		}
	}
}

void RadioButton::Update()
{
	//DEFAULT
	ui::Wnd::UpdateDefault();

	//SELF
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	BOOL  &bActive = m_tWindowInfo.bActive;

	//---SELF
	if ((dwStyle & FWS_VISIBLE) && !(dwStyle & FWS_DISABLE))
	{
		ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

		BOOL bMouseOver = (PtInRect(&pInputInfo->ptMouse) > 0);

		if (m_pParent->IsActive() || bActive)
		{
			//Test cursor in rect
			switch(pInputInfo->eType)
			{ //处理鼠标
			case UIMT_MS_BTNUP:
				//if (m_eBtnState == BTN_STATE_DOWN && pInputInfo->dwData == MK_LBUTTON )//left btn down
				//{
				//	m_eBtnState = bMouseOver? BTN_STATE_OVER :  BTN_STATE_NORMAL;
				//	if (bMouseOver)
				//		m_bClicked = TRUE;
				//}
				break;
			case UIMT_MS_BTNDOWN:
				if (bMouseOver && pInputInfo->dwData == MK_LBUTTON)
				{
					m_bClicked = TRUE; 
				}
				break;
			default:
				break;
			}
		}
	}
}

void RadioButton::SetChecked(BOOL bFlag)
{
	m_bChecked = bFlag;
}

void RadioButton::SetTextureHandle(DWORD dwTextureHandle)
{
	m_dwTextureHandle = dwTextureHandle;
}

CheckBox::CheckBox(void)
{
	m_bChecked = FALSE;
}

CheckBox::~CheckBox(void)
{
}

void CheckBox::Render()
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

		RECT &rc = m_tWindowInfo.rcRect;
		
		int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;
		int iLen = (int)strlen(m_tWindowInfo.szName);

		int iTextStartX = iX + iW/2 - pLayer2D->GetFontSize()/2;
		int iTextStartY = iY + iH/2 - pLayer2D->GetFontSize()/2;

		static D3DCOLOR cHigh   = m_tWindowInfo.colorHigh;
		static D3DCOLOR cLow    = m_tWindowInfo.colorLow;
		static D3DCOLOR cMiddle = m_tWindowInfo.colorMiddle;
		static D3DCOLOR cText   = m_tWindowInfo.colorText;
		static D3DCOLOR cBack   = m_tWindowInfo.colorBack;
		static D3DCOLOR cFrame  = 0xffffffFF;

		ui::Wnd::SetUIRendState();

		if(m_bChecked)
			pLayer2D->TextOut(iTextStartX,iTextStartY,"★",2,m_tWindowInfo.colorText);
		pLayer2D->TextOut(iX+iW+pLayer2D->GetFontSize()/2/2,iTextStartY,m_tWindowInfo.szName,lstrlen(m_tWindowInfo.szName),m_tWindowInfo.colorText);
	}
}

void CheckBox::Update()
{
	//DEFAULT
	ui::Wnd::UpdateDefault();

	//SELF
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	BOOL  &bActive = m_tWindowInfo.bActive;

	//---SELF
	if ((dwStyle & FWS_VISIBLE) && !(dwStyle & FWS_DISABLE))
	{
		ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

		BOOL bMouseOver = (PtInRect(&pInputInfo->ptMouse) > 0);

		if (m_pParent->IsActive() || bActive)
		{
			//Test cursor in rect
			switch(pInputInfo->eType)
			{ //处理鼠标
			case UIMT_MS_BTNUP:
				break;
			case UIMT_MS_BTNDOWN:
				if (bMouseOver && pInputInfo->dwData == MK_LBUTTON)
				{
					m_bChecked = !m_bChecked; 
				}
				break;
			default:
				break;
			}
		}
	}
}

void CheckBox::SetChecked(BOOL bFlag)
{
	m_bChecked = bFlag;
}

