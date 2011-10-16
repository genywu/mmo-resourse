
#include "stdafx.h"
#include "utility.h"
#include "console.h"
#include "render.h"
#include "ui.h"
#define VK_B				0x42
#define VK_E				0x45
#define VK_I				0x49
#define VK_O				0x4F
#define VK_R				0x52
#define VK_X				0x58
#define VK_ALT				VK_MENU
#define VK_V				0x56
#define KEYDOWN(VK_CODE)	((GetAsyncKeyState(VK_CODE) & 0x8000) ? 1 : 0)
//___CLASS___(ui::Wnd)___
ui::Wnd::Wnd()
{
	m_pParent = NULL;
	ZeroMemory(&m_tWindowInfo,sizeof(_tagWindowInfo));

	m_tWindowInfo.colorBack =  FWC_BACK;
	m_tWindowInfo.colorText =	FWC_TEXT;	

	m_tWindowInfo.colorLow  =   FWC_LOW;
	m_tWindowInfo.colorMiddle = FWC_MIDDLE;	
	m_tWindowInfo.colorHigh   = FWC_HIGH; 
}

ui::Wnd::~Wnd()
{
	//有子窗口未销毁
	ASSERT((int)m_listChild.size() == 0);

	/*for(std::list<Wnd *>::iterator it = m_listChild.begin(); it != m_listChild.end(); it++)
	{
	Wnd *pWnd = *it; 
	ASSERT(pWnd);
	}
	m_listChild.clear();*/
}

void ui::Wnd::Update()
{
	//过程
	// 调用缺省的Update
	UpdateDefault();

	//Update Self
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	BOOL  &bActive = m_tWindowInfo.bActive;

	if ((dwStyle & FWS_VISIBLE) && !(dwStyle & FWS_DISABLE))
	{
		// 当前自己的Update
		//update self

		// 调用子节点Update
		UpdateChild();
	}
}

void ui::Wnd::Render()
{
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		//画背景
		RenderDefault();

		//画自己

		//画Child
		RenderChild();
	}
}

void ui::Wnd::UpdateDefault()
{
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	BOOL  &bActive = m_tWindowInfo.bActive;
	RECT  &rcWnd   = m_tWindowInfo.rcRect;

	ui::Manager *pUiMgr = ui::Manager::GetInstance();
	ASSERT(pUiMgr);

	if ((dwStyle & FWS_VISIBLE) && !(dwStyle & FWS_DISABLE))
	{
		ui::Manager::_tagINPUTINFO *pInputInfo = pUiMgr->GetInputInfoBuffer();

		switch(pInputInfo->eType)
		{ //处理鼠标
		case UIMT_MS_BTNUP:
			break;
		case UIMT_MS_BTNDOWN:
			switch(pInputInfo->dwData)
			{
			case MK_LBUTTON://l
				break;
			case MK_RBUTTON://r
				if (bActive)
				{
					int iPtResult = PtInRect(&pInputInfo->ptMouse);
					if (iPtResult == FW_POINT_INSIDE && (dwStyle & FWS_DRAG || dwStyle & FWS_DEBUG)) 
					{
						m_tWindowInfo.bDrag = TRUE;
					}	

					if (iPtResult == FW_POINT_INTERSECT && (dwStyle & FWS_SIZE || dwStyle & FWS_DEBUG))
					{
						WORD &wSize = m_tWindowInfo.wSize;

						if (pInputInfo->ptMouse.x >= rcWnd.left   &&  pInputInfo->ptMouse.x <= rcWnd.left + FWA_FRAMESIZE)  wSize |= 0x01;
						if (pInputInfo->ptMouse.x <= rcWnd.right  &&  pInputInfo->ptMouse.x >= rcWnd.right - FWA_FRAMESIZE) wSize |= 0x02;
						if (pInputInfo->ptMouse.y >= rcWnd.top    &&  pInputInfo->ptMouse.y <= rcWnd.top + FWA_FRAMESIZE)   wSize |= 0x04;
						if (pInputInfo->ptMouse.y <= rcWnd.bottom &&  pInputInfo->ptMouse.y >= rcWnd.bottom - FWA_FRAMESIZE)wSize |= 0x08;
					}
				}
				break;
			case MK_MBUTTON://m
				break;
			}

			break;
		default:
			break;
		}

		//size for debug
		if (m_tWindowInfo.wSize)
		{
			POINT &pt = pInputInfo->ptMouse;
			POINT &ptOld = pInputInfo->ptMouseOld;
			RECT &rc = m_tWindowInfo.rcRect;

			WORD &wSize = m_tWindowInfo.wSize;

			//RECT rcNew = rc;

			int x = pt.x - ptOld.x;
			int y = pt.y - ptOld.y;

			if (wSize & 0x01) 
			{
				SizeWnd(-x,0);
				MoveWnd(x,0);
			}
			if (wSize & 0x02) 
			{
				SizeWnd(x,0);
			}
			if (wSize & 0x04) 
			{
				SizeWnd(0,-y);
				MoveWnd(0,y);
			}
			if (wSize & 0x08) 
			{
				SizeWnd(0,y);
			}						

			if (!(pInputInfo->dwMouseButtonState & MK_RBUTTON)) m_tWindowInfo.wSize = 0;
		}

		if (m_tWindowInfo.bDrag)
		{
			int x = pInputInfo->ptMouse.x - pInputInfo->ptMouseOld.x;
			int y = pInputInfo->ptMouse.y - pInputInfo->ptMouseOld.y;

			MoveWnd(x,y);

			if (!(pInputInfo->dwMouseButtonState & MK_RBUTTON)) m_tWindowInfo.bDrag = FALSE;
		}
	}
}

void ui::Wnd::SizeWnd(int iCX,int iCY)
{
	RECT &rc = m_tWindowInfo.rcRect;
	rc.right  += iCX;
	rc.bottom += iCY;
}

void ui::Wnd::UpdateChild()
{
	for(std::list<Wnd *>::iterator it = m_listChild.begin(); it != m_listChild.end(); it++)
	{
		Wnd *pWnd = *it; 
		pWnd->Update();
	}
}
bool ui::Wnd::IsVisible()
{
	if(m_tWindowInfo.dwStyle & FWS_VISIBLE)
		return true;
	else
		return false;
}
void ui::Wnd::SwitchVisiblity()
{
	if(m_tWindowInfo.dwStyle & FWS_VISIBLE)
		m_tWindowInfo.dwStyle &= ~FWS_VISIBLE;
	else
		m_tWindowInfo.dwStyle |= FWS_VISIBLE;
}
void ui::Wnd::AddChild(ui::Wnd *pWnd)
{
	if (pWnd->GetStyle() & FWS_TOPMOST)
		m_listChild.push_front(pWnd);
	else
	{
		std::list<Wnd *>::iterator it;
		for(it = m_listChild.begin(); it != m_listChild.end(); it++)
		{
			Wnd *p = *it; 
			if (!(p->GetStyle() & FWS_TOPMOST))
			{
				m_listChild.insert(it,pWnd);
				break;
			}
		}
		if (it == m_listChild.end())
		{
			m_listChild.push_back(pWnd);

		}
	}
}

void ui::Wnd::RemoveChild(ui::Wnd *pWnd)
{
	m_listChild.remove(pWnd);
}


void ui::Wnd::HitTest(const POINT *pPoint, ui::Wnd **ppWnd)
{
	if (*ppWnd) return ;
	if (m_tWindowInfo.dwStyle & FWS_VISIBLE)
	{
		for(std::list<Wnd *>::iterator it = m_listChild.begin(); it != m_listChild.end(); it++)
		{
			Wnd *pWnd = *it; 
			pWnd->HitTest(pPoint,ppWnd);
			if (*ppWnd) return;
		}

		if (PtInRect(pPoint) != FW_POINT_OUTSIDE)
		{
			*ppWnd = this;
		}
	}
}

BOOL ui::Wnd::Create(const char *pszName, const RECT *pRect, DWORD dwStyle, ui::Wnd *pParent)
{
	ASSERT(pszName);
	ASSERT(pRect);

	strncpy(m_tWindowInfo.szName,pszName,255);
	m_tWindowInfo.szName[255] = 0;

	m_tWindowInfo.dwStyle = dwStyle; 

	m_pParent = pParent;

	if (pParent)
	{
		RECT  rc;
		pParent->GetWndRect(&rc);

		m_tWindowInfo.rcRect.left   = rc.left + pRect->left;
		m_tWindowInfo.rcRect.top    = rc.top  + pRect->top;
		m_tWindowInfo.rcRect.right  = m_tWindowInfo.rcRect.left + (pRect->right-pRect->left);
		m_tWindowInfo.rcRect.bottom = m_tWindowInfo.rcRect.top  + (pRect->bottom-pRect->top);

		pParent->AddChild(this);
	}
	else
		memcpy(&m_tWindowInfo.rcRect,pRect,sizeof(RECT));

	return TRUE;
}


void ui::Wnd::GetWndRect(RECT *pRect)
{
	memcpy(pRect,&m_tWindowInfo.rcRect,sizeof(RECT));
}

void ui::Wnd::SetWndRect(RECT *pRect)
{
	memcpy(&m_tWindowInfo.rcRect,pRect,sizeof(RECT));
}

void ui::Wnd::Destroy()
{
	if (m_pParent)
		m_pParent->RemoveChild(this);
}

int ui::Wnd::PtInRect(const POINT *pPoint)
{
	if (!(m_tWindowInfo.dwStyle & FWS_VISIBLE)) return FW_POINT_OUTSIDE;

	RECT &rc = m_tWindowInfo.rcRect;
	int x = pPoint->x;
	int y = pPoint->y;
	if (x < rc.left || y < rc.top || x > rc.right || y > rc.bottom) return FW_POINT_OUTSIDE;//outside
	if (x > rc.left + FWA_FRAMESIZE && x < rc.right - FWA_FRAMESIZE && y > rc.top + FWA_FRAMESIZE && y < rc.bottom - FWA_FRAMESIZE) return FW_POINT_INSIDE;
	return FW_POINT_INTERSECT;
}

void ui::Wnd::MoveWnd(int iOX, int iOY)
{
	RECT & rc = m_tWindowInfo.rcRect;
	rc.left   += iOX;
	rc.top    += iOY;
	rc.right  += iOX;
	rc.bottom += iOY;	

	for(std::list<Wnd *>::iterator it = m_listChild.begin(); it != m_listChild.end(); it++)
	{
		Wnd *p = *it; 
		p->MoveWnd(iOX,iOY);
	}

}


void ui::Wnd::ShowWnd(BOOL bShow)
{
	if (bShow)
		m_tWindowInfo.dwStyle |= FWS_VISIBLE;
	else
		m_tWindowInfo.dwStyle &= ~FWS_VISIBLE;
}


void ui::Wnd::MoveWndTo(int iX, int iY)
{
	int iOX = iX - m_tWindowInfo.rcRect.left;
	int iOY = iY - m_tWindowInfo.rcRect.top;

	MoveWnd(iOX,iOY);
}

void ui::Wnd::CenterWndTo(int iX, int iY)
{
	DWORD iW = m_tWindowInfo.rcRect.right - m_tWindowInfo.rcRect.left + 1;
	DWORD iH = m_tWindowInfo.rcRect.bottom- m_tWindowInfo.rcRect.top + 1;

	int iOX = iX - m_tWindowInfo.rcRect.left - iW / 2;
	int iOY = iY - m_tWindowInfo.rcRect.top  - iH / 2; 

	MoveWnd(iOX,iOY);
}

void ui::Wnd::RenderDefault(void)
{
	DWORD &dwStyle = m_tWindowInfo.dwStyle;

	render::Interface::Layer2D *pLayer2D = render::Interface::GetInstance()->GetLayer2D();

	int iFontSize = pLayer2D->GetFontSize();

	RECT &rc = m_tWindowInfo.rcRect;

	int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;

	if (dwStyle & FWS_SHADOW)
	{
		pLayer2D->DrawSolidQuad(iX+4,iY+4,iW,iH,m_tWindowInfo.colorLow);
	}

	if (dwStyle & FWS_FILLBKG)
	{
		pLayer2D->DrawSolidQuad(iX,iY,iW,iH,m_tWindowInfo.colorBack);
	}	

	if (dwStyle & FWS_DEBUG && m_tWindowInfo.bActive)
	{
		char szTemp[256];
		//显示相对的父亲的RECT
		RECT rcRelative = m_tWindowInfo.rcRect;
		if (m_pParent)
		{
			RECT rcParent;
			m_pParent->GetWndRect(&rcParent);
			rcRelative.left  -= rcParent.left;
			rcRelative.right  = rcRelative.left + iW;
			rcRelative.top   -= rcParent.top;
			rcRelative.bottom = rcRelative.top + iH;
		}

		sprintf(szTemp,"%s [%d %d %d %d][%d,%d]",m_tWindowInfo.szName,rcRelative.left,rcRelative.top,rcRelative.right,rcRelative.bottom,iW,iH);

		int iStartX = iX;
		int iStartY = iY - iFontSize;
		ui::Wnd::SetUIRendState();
		pLayer2D->TextOut(iStartX + 1,iStartY + 1,szTemp,lstrlen(szTemp),FWC_BLACK);
		pLayer2D->TextOut(iStartX - 1,iStartY - 1,szTemp,lstrlen(szTemp),FWC_BLACK);
		pLayer2D->TextOut(iStartX + 1,iStartY - 1,szTemp,lstrlen(szTemp),FWC_BLACK);
		pLayer2D->TextOut(iStartX - 1,iStartY + 1,szTemp,lstrlen(szTemp),FWC_BLACK);
		pLayer2D->TextOut(iStartX + 1,iStartY    ,szTemp,lstrlen(szTemp),FWC_BLACK);
		pLayer2D->TextOut(iStartX - 1,iStartY    ,szTemp,lstrlen(szTemp),FWC_BLACK);
		pLayer2D->TextOut(iStartX    ,iStartY - 1,szTemp,lstrlen(szTemp),FWC_BLACK);
		pLayer2D->TextOut(iStartX    ,iStartY + 1,szTemp,lstrlen(szTemp),FWC_BLACK);
		pLayer2D->TextOut(iStartX,iY - iFontSize,szTemp,lstrlen(szTemp),FWC_WHITE);		

	}

	if (dwStyle & FWS_OUTLINE)
	{
		if (m_tWindowInfo.bActive)
			pLayer2D->DrawWireQuad(iX,iY,iW,iH,m_tWindowInfo.colorHigh);
		else
			pLayer2D->DrawWireQuad(iX,iY,iW,iH,m_tWindowInfo.colorMiddle);			
	}
}
void ui::Wnd::SetUIRendState()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	LPDIRECT3DDEVICE9 pDevice     = pInterface->GetDevice();

	pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE);

	// FILL Solid Mode
	pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
	pDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE);
	pDevice->SetRenderState( D3DRS_ZENABLE,FALSE);
	pDevice->SetRenderState( D3DRS_ZWRITEENABLE,FALSE);

	// Setup Alpha 
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,	FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAREF ,0x0000);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	//Setup Color OP
	pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	//Setup Alpha OP
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	//Setup Color OP
	pDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	//Setup Alpha OP
	pDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
}
void ui::Wnd::RenderChild(void)
{
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (!(dwStyle & FWS_VISIBLE)) return ;

	for(std::list<Wnd *>::reverse_iterator it = m_listChild.rbegin(); it != m_listChild.rend(); it++)
	{
		Wnd *pWnd = *it; 
		pWnd ->Render();
	}
}

void ui::Wnd::ModifyStyle(DWORD dwAdd , DWORD dwRemove)
{
	DWORD &dwStyle = m_tWindowInfo.dwStyle;

	dwStyle |= dwAdd;
	dwStyle &= ~dwRemove;
}


//__CLASS_(ui::Button)_____
ui::Button::Button()
{
	m_eBtnState = BTN_STATE_NORMAL;
	m_bClicked = FALSE;
}

ui::Button::~Button()
{

}

void ui::Button::Update()
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
				if (m_eBtnState == BTN_STATE_DOWN && pInputInfo->dwData == MK_LBUTTON )//left btn down
				{
					m_eBtnState = bMouseOver? BTN_STATE_OVER :  BTN_STATE_NORMAL;
					if (bMouseOver)
						m_bClicked = TRUE;
				}
				break;
			case UIMT_MS_BTNDOWN:
				if (bMouseOver && m_eBtnState != BTN_STATE_DOWN && pInputInfo->dwData == MK_LBUTTON)
				{
					m_eBtnState = BTN_STATE_DOWN; 
				}
				break;
			default:
				break;
			}

			if (m_eBtnState != BTN_STATE_DOWN)
			{
				m_eBtnState = bMouseOver? BTN_STATE_OVER :  BTN_STATE_NORMAL;
			}

		}
	}

	//NO CHILD
}


void ui::Button::Render()
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
		RECT &rc       = m_tWindowInfo.rcRect;

		int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;
		int iLen = (int)strlen(m_tWindowInfo.szName);

		int iTextStartX = iX + iW/2 - pLayer2D->CalcTextWidth(m_tWindowInfo.szName,iLen)/2;
		int iTextStartY = iY + iH/2 - pLayer2D->GetFontSize()/2;

		D3DCOLOR cHigh   = m_tWindowInfo.colorHigh;
		D3DCOLOR cLow    = m_tWindowInfo.colorLow;
		D3DCOLOR cMiddle = m_tWindowInfo.colorMiddle;
		D3DCOLOR cText   = m_tWindowInfo.colorText;
		D3DCOLOR cBack   = m_tWindowInfo.colorBack;

		//shadow
		if (dwStyle & FWS_SHADOW)
			pLayer2D->DrawSolidQuad(iX+FWA_FRAMESIZE,iY+FWA_FRAMESIZE,iW,iH,cLow);

		if (dwStyle & FWS_DISABLE)
		{
			pLayer2D->DrawSolidQuad(iX,iY,iW,iH,cBack);
			pLayer2D->DrawWireQuad(iX,iY,iW,iH,cLow);
			ui::Wnd::SetUIRendState();
			pLayer2D->TextOut(iTextStartX,iTextStartY,m_tWindowInfo.szName,iLen,cLow);	
		}
		else
		{
			switch(m_eBtnState)
			{
			case BTN_STATE_NORMAL:
				pLayer2D->DrawSolidQuad(iX,iY,iW,iH,cBack);
				pLayer2D->DrawWireQuad(iX,iY,iW,iH,cMiddle);
				break;
			case BTN_STATE_DOWN:
				pLayer2D->DrawSolidQuad(++iX,++iY,iW,iH,cBack);
				pLayer2D->DrawWireQuad(iX,iY,iW,iH,cHigh);
				iTextStartX++;iTextStartY++;
				break;
			case BTN_STATE_OVER:
				pLayer2D->DrawSolidQuad(iX,iY,iW,iH,cBack);
				pLayer2D->DrawWireQuad(iX,iY,iW,iH,cHigh);
				break;
			}
			//default to center
			ui::Wnd::SetUIRendState();
			pLayer2D->TextOut(iTextStartX + 1,iTextStartY + 1,m_tWindowInfo.szName,iLen,FWC_BLACK);	
			pLayer2D->TextOut(iTextStartX - 1,iTextStartY - 1,m_tWindowInfo.szName,iLen,FWC_BLACK);	
			pLayer2D->TextOut(iTextStartX + 1,iTextStartY - 1,m_tWindowInfo.szName,iLen,FWC_BLACK);	
			pLayer2D->TextOut(iTextStartX - 1,iTextStartY + 1,m_tWindowInfo.szName,iLen,FWC_BLACK);		
			pLayer2D->TextOut(iTextStartX + 1,iTextStartY,m_tWindowInfo.szName,iLen,FWC_BLACK);	
			pLayer2D->TextOut(iTextStartX - 1,iTextStartY,m_tWindowInfo.szName,iLen,FWC_BLACK);	
			pLayer2D->TextOut(iTextStartX,iTextStartY - 1,m_tWindowInfo.szName,iLen,FWC_BLACK);	
			pLayer2D->TextOut(iTextStartX,iTextStartY + 1,m_tWindowInfo.szName,iLen,FWC_BLACK);	
			pLayer2D->TextOut(iTextStartX,iTextStartY,m_tWindowInfo.szName,iLen,cText);	
		}
	}
	//NO CHILD
}

BOOL ui::Button::IsClicked()
{
	BOOL bClicked = m_bClicked;
	m_bClicked = FALSE;
	return bClicked;
}


//__CLASS_(ui::Static)
void ui::Static::Update()
{
	//DEFAULT

	ui::Wnd::UpdateDefault();

	//SELF
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	BOOL  &bActive = m_tWindowInfo.bActive;

	//---SELF	

	if ((dwStyle & FWS_VISIBLE) && !(dwStyle & FWS_DISABLE))
	{
	}

	//NO CHILD
}


void ui::Static::Render()
{
	//NO DRAW DEFAULT
	ui::Wnd::RenderDefault();

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

	//SELF
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		RECT &rc       = m_tWindowInfo.rcRect;

		int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;
		int iLen = (int)strlen(m_tWindowInfo.szName);

		int iTextStartX = iX + iW/2 - pLayer2D->CalcTextWidth(m_tWindowInfo.szName,iLen)/2;
		int iTextStartY = iY + iH/2 - pLayer2D->GetFontSize()/2;

		D3DCOLOR cHigh   = m_tWindowInfo.colorHigh;
		D3DCOLOR cLow    = m_tWindowInfo.colorLow;
		D3DCOLOR cMiddle = m_tWindowInfo.colorMiddle;
		D3DCOLOR cText   = m_tWindowInfo.colorText;
		D3DCOLOR cBack   = m_tWindowInfo.colorBack;

		//default to center
		ui::Wnd::SetUIRendState();
		pLayer2D->TextOut(iTextStartX + 1,iTextStartY + 1,m_tWindowInfo.szName,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iTextStartX - 1,iTextStartY - 1,m_tWindowInfo.szName,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iTextStartX + 1,iTextStartY - 1,m_tWindowInfo.szName,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iTextStartX - 1,iTextStartY + 1,m_tWindowInfo.szName,iLen,FWC_BLACK);		
		pLayer2D->TextOut(iTextStartX + 1,iTextStartY,m_tWindowInfo.szName,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iTextStartX - 1,iTextStartY,m_tWindowInfo.szName,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iTextStartX,iTextStartY - 1,m_tWindowInfo.szName,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iTextStartX,iTextStartY + 1,m_tWindowInfo.szName,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iTextStartX,iTextStartY,m_tWindowInfo.szName,iLen,cText);	
	}
	//NO CHILD
}

//__CLASS_(ui::MenuBar)____
//__MenuBar::Menu__
DWORD ui::MenuBar::Menu::AddItem(const char *pszText)
{
	ASSERT(m_dwItemCnt < POPUPMENU_MAX_ITEM);
	_tagItem &stItem = m_stItemArray[m_dwItemCnt];

	strncpy(stItem.szText,pszText,POPUPMENU_MAX_TEXT - 1);
	stItem.szText[POPUPMENU_MAX_TEXT - 1] = 0;

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

	DWORD dwLastPosition = FWA_FRAMESIZE;
	if (m_dwItemCnt > 0)
		dwLastPosition = m_stItemArray[m_dwItemCnt-1].rc.bottom;

	int iTextLen = lstrlen(stItem.szText);
	int iTextWidth = pLayer2D->CalcTextWidth(stItem.szText,iTextLen);
	int iTextHeight= pLayer2D->GetFontSize();
	int iFontWidth = pLayer2D->GetFontSize();

	//根据宽度调整窗口大小
	int iOriginWindowWidth = m_iDropWidth;
	int iNewWindowWidth	   = iTextWidth + iFontWidth + iFontWidth + FWA_FRAMESIZE * 2;
	int iWindowWidth	   = (iOriginWindowWidth > iNewWindowWidth) ? iOriginWindowWidth: iNewWindowWidth;
	int iItemHeight   = iTextHeight + FWA_FRAMESIZE * 2;

	if (iOriginWindowWidth > iNewWindowWidth)
	{
		int iItemWidth = iOriginWindowWidth - FWA_FRAMESIZE * 2;
		//窗口大~直接设置即可
		SetRect(&stItem.rc,FWA_FRAMESIZE,dwLastPosition,FWA_FRAMESIZE + iItemWidth,iItemHeight);	
		stItem.rc.bottom += stItem.rc.top;
	}
	else
	{
		//窗口小了，需要调整到适合大小
		int iItemWidth = iNewWindowWidth - FWA_FRAMESIZE * 2;

		SetRect(&stItem.rc,FWA_FRAMESIZE,dwLastPosition,FWA_FRAMESIZE + iItemWidth,iItemHeight);	
		stItem.rc.bottom += stItem.rc.top;

		//调整窗口宽度
		m_iDropWidth = iWindowWidth;
		//循环所有的调整
		for(DWORD dwItem = 0; dwItem < m_dwItemCnt;dwItem++)
		{
			_tagItem &stItem = m_stItemArray[dwItem];
			stItem.rc.right = stItem.rc.left + iItemWidth;
		}
	}

	//自动计算窗口高度
	int iNewWindowHeight = m_stItemArray[m_dwItemCnt].rc.bottom + FWA_FRAMESIZE * 2;
	m_iDropHeight = iNewWindowHeight;

	m_dwItemCnt++;

	return m_dwItemCnt -1;
}

void ui::MenuBar::Menu::Render()
{
	if (m_dwStyle & MBS_DISABLE)
		return;

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

	int iX = m_rcBtn.left,iY = m_rcBtn.bottom ,iW = m_iDropWidth,iH = m_iDropHeight;

	pLayer2D->DrawSolidQuad(iX+4,iY+4,iW,iH,FWC_LOW);
	pLayer2D->DrawSolidQuad(iX,iY,iW,iH,FWC_BACK);
	pLayer2D->DrawWireQuad(iX,iY,iW,iH,FWC_LOW);

	int iFontWidth  = pLayer2D->GetFontSize();
	int iFontHeight = pLayer2D->GetFontSize();

	//画垂直条
	int iVeticalBarWidth = iFontWidth + iFontWidth/2;
	pLayer2D->DrawSolidQuad(iX + FWA_FRAMESIZE ,iY + FWA_FRAMESIZE,iVeticalBarWidth,iH - FWA_FRAMESIZE * 2,FWC_LOW);


	for(DWORD dwItem = 0; dwItem < m_dwItemCnt;dwItem++)
	{
		_tagItem &stItem = m_stItemArray[dwItem];
		RECT &rcItem = stItem.rc;
		int iLen = (int)strlen(stItem.szText);

		int iX = rcItem.left + m_rcBtn.left,iY = rcItem.top + m_rcBtn.bottom,iW = rcItem.right - rcItem.left + 1,iH = rcItem.bottom - rcItem.top + 1;

		int iTextStartX = iX + iVeticalBarWidth + FWA_FRAMESIZE;
		int iTextStartY = iY + iH/2 - iFontHeight/2; 

		DWORD cText = (stItem.m_dwStyle & MBS_DISABLE)? FWC_BACK:FWC_TEXT;

		if (m_dwHoverItem == dwItem)
		{
			pLayer2D->DrawSolidQuad(iX,iY,iW,iH,FWC_BACK);
			pLayer2D->DrawWireQuad(iX,iY,iW,iH,FWC_LOW);
		}
		ui::Wnd::SetUIRendState();

		if (stItem.m_dwStyle & MBS_CHECKED)
			pLayer2D->TextOut(iX+ iVeticalBarWidth /2 - iFontWidth/2,iTextStartY,"√",2,cText);

		pLayer2D->TextOut(iTextStartX + 1,iTextStartY + 1,stItem.szText,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iTextStartX - 1,iTextStartY - 1,stItem.szText,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iTextStartX + 1,iTextStartY - 1,stItem.szText,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iTextStartX - 1,iTextStartY + 1,stItem.szText,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iTextStartX + 1,iTextStartY,stItem.szText,iLen,FWC_BLACK);
		pLayer2D->TextOut(iTextStartX - 1,iTextStartY,stItem.szText,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iTextStartX,iTextStartY + 1,stItem.szText,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iTextStartX,iTextStartY - 1,stItem.szText,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iTextStartX,iTextStartY,stItem.szText,iLen,cText);	
	}
}


void ui::MenuBar::Menu::Update()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

	//Test cursor in rect
	ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

	m_dwHoverItem = HitTestItem(&pInputInfo->ptMouse);

	if (PtInRect(&pInputInfo->ptMouse))
	{
		//选择
		if (pInputInfo->eType == UIMT_MS_BTNDOWN && (pInputInfo->dwData == MK_LBUTTON || pInputInfo->dwData == MK_RBUTTON))
		{
			m_dwSelItem = m_dwHoverItem;
			if (m_dwSelItem != 0xffffffff)
				m_bClicked = TRUE;
		}
	}
}

const char *ui::MenuBar::Menu::GetItemText(DWORD dwItem)
{
	if (dwItem < m_dwItemCnt) 
		return m_stItemArray[dwItem].szText;
	return NULL;
}

DWORD ui::MenuBar::Menu::HitTestItem(POINT *pPoint)
{
	for(DWORD dwItem = 0; dwItem < m_dwItemCnt;dwItem++)
	{
		_tagItem &stItem = m_stItemArray[dwItem];
		if (stItem.m_dwStyle & MBS_DISABLE) 
			continue;

		RECT rc = stItem.rc;
		OffsetRect(&rc,m_rcBtn.left,m_rcBtn.bottom);

		if (::PtInRect(&rc,*pPoint))
		{
			return dwItem;
		}
	}
	return 0xffffffff;
}

void ui::MenuBar::Menu::MoveMenu(int iOX,int iOY)
{
	OffsetRect(&m_rcBtn,iOX,iOY);
}

BOOL ui::MenuBar::Menu::IsClicked()
{
	BOOL bClicked = m_bClicked;
	m_bClicked = FALSE;
	return bClicked;
}

void ui::MenuBar::Menu::ClearItem()
{
	m_dwItemCnt = 0;
	ZeroMemory(m_stItemArray,sizeof(_tagItem) * MENUBAR_MAX_ITEM);
}

BOOL ui::MenuBar::Menu::PtInRect(const POINT * pPoint)
{
	RECT rcDrop = { 0,0,m_iDropWidth,m_iDropHeight};
	OffsetRect(&rcDrop,m_rcBtn.left,m_rcBtn.bottom);
	return ::PtInRect(&rcDrop,*pPoint);
}

void ui::MenuBar::Menu::EnableItem(DWORD dwItem)
{
	ASSERT(dwItem < m_dwItemCnt);
	_tagItem &stItem = m_stItemArray[dwItem];
	stItem.m_dwStyle &= ~MBS_DISABLE;
}

void ui::MenuBar::Menu::DisableItem(DWORD dwItem)
{
	ASSERT(dwItem < m_dwItemCnt);
	_tagItem &stItem = m_stItemArray[dwItem];
	stItem.m_dwStyle |= MBS_DISABLE;
}

void  ui::MenuBar::Menu::SetCheckItem(DWORD dwItem,BOOL bCheck)
{
	ASSERT(dwItem < m_dwItemCnt);
	_tagItem &stItem = m_stItemArray[dwItem];
	if (bCheck)
		stItem.m_dwStyle |= MBS_CHECKED;
	else
		stItem.m_dwStyle &= ~MBS_CHECKED;
}

BOOL  ui::MenuBar::Menu::IsChecked(DWORD dwItem)
{
	ASSERT(dwItem < m_dwItemCnt);
	_tagItem &stItem = m_stItemArray[dwItem];
	if (stItem.m_dwStyle & MBS_CHECKED)
		return TRUE;
	else
		return FALSE;
}


//___CLASS(ui::MenuBar) ___
ui::MenuBar::MenuBar(void):m_dwMenuCnt(0),m_dwSelMenu(0),m_dwSelMenuItem(0),
m_eMenuBarState(MENUBAR_STATE_NORMAL),
m_bSelected(FALSE)
{
}

ui::MenuBar::~MenuBar(void)
{
}

void  ui::MenuBar::Update(void)
{
	//DEFAULT
	ui::Wnd::UpdateDefault();

	//SELF
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	BOOL  &bActive = m_tWindowInfo.bActive;
	if ((dwStyle & FWS_VISIBLE) && !(dwStyle & FWS_DISABLE) )
	{
		//Test cursor in rect
		ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

		BOOL bMouseOver = (PtInRect(&pInputInfo->ptMouse) > 0);

		m_dwHoverMenu = HitTestMenu(&pInputInfo->ptMouse);

		if (bActive)
		{
			switch(pInputInfo->eType)
			{ //处理鼠标
			case UIMT_MS_BTNDOWN:
				if (bMouseOver && pInputInfo->dwData == MK_LBUTTON)
				{
					if (m_dwHoverMenu != 0xffffffff)
					{
						if (m_eMenuBarState == MENUBAR_STATE_POPUP)
						{
							m_eMenuBarState = MENUBAR_STATE_NORMAL;
						}
						else
						{
							m_eMenuBarState = MENUBAR_STATE_POPUP;
							m_dwSelMenu = m_dwHoverMenu;
						}
					}
					else
					{
						if (m_eMenuBarState == MENUBAR_STATE_POPUP)
						{
							m_eMenuBarState = MENUBAR_STATE_NORMAL;
						}
					}
				}
				break;
			default:
				break;
			}

			if (m_eMenuBarState == MENUBAR_STATE_POPUP)
			{
				if (m_dwHoverMenu != 0xffffffff)
				{
					m_dwSelMenu = m_dwHoverMenu;
				}			

				if (m_dwSelMenu >=0 && m_dwSelMenu < m_dwMenuCnt)
				{
					ui::MenuBar::Menu &cMenu = m_stMenuArray[m_dwSelMenu];
					cMenu.Update();

					if (cMenu.IsClicked())
					{
						m_dwSelMenuItem = cMenu.GetSelItem();
						m_bSelected = TRUE;
						m_eMenuBarState = MENUBAR_STATE_NORMAL;
					}
				}
			}
		}
		else
		{
			//在失去击活情况下弹出状态将要取消
			if (m_eMenuBarState == MENUBAR_STATE_POPUP)
				m_eMenuBarState = MENUBAR_STATE_NORMAL;
		}
	}
}


void  ui::MenuBar::Render()
{
	//NO DRAW DEFAULT
	//SELF
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if ((dwStyle & FWS_VISIBLE) && !(dwStyle & FWS_DISABLE))
	{
		RECT &rc = m_tWindowInfo.rcRect;

		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

		D3DCOLOR &cHigh   = m_tWindowInfo.colorHigh;
		D3DCOLOR &cLow    = m_tWindowInfo.colorLow;
		D3DCOLOR &cMiddle = m_tWindowInfo.colorMiddle;
		D3DCOLOR &cText   = m_tWindowInfo.colorText;
		D3DCOLOR &cBack   = m_tWindowInfo.colorBack;

		pLayer2D->DrawSolidQuad(rc.left,rc.top,rc.right - rc.left +1 ,rc.bottom - rc.top + 1,cBack);

		for(DWORD dwMenu = 0; dwMenu < m_dwMenuCnt;dwMenu++)
		{
			Menu &cMenu = m_stMenuArray[dwMenu];
			RECT &rcMenu = cMenu.m_rcBtn;
			int iLen = (int)strlen(cMenu.m_szText);

			int iX = rcMenu.left,iY = rcMenu.top,iW = rcMenu.right - rcMenu.left + 1,iH = rcMenu.bottom - rcMenu.top + 1;

			int iTextStartX = iX + iW/2 - pLayer2D->CalcTextWidth(cMenu.m_szText,iLen)/2;
			int iTextStartY = iY + iH/2 - pLayer2D->GetFontSize()/2;

			if (cMenu.m_dwStyle & MBS_DISABLE)
			{

				pLayer2D->TextOut(iTextStartX,iTextStartY,cMenu.m_szText,iLen,cBack);	
			}
			else
			{
				switch(m_eMenuBarState)
				{
				case MENUBAR_STATE_NORMAL:
					if (m_dwHoverMenu == dwMenu)
					{
						pLayer2D->DrawSolidQuad(iX,iY,iW,iH,cHigh);
						pLayer2D->DrawWireQuad(iX,iY,iW,iH,cLow);
					}
					break;
				case MENUBAR_STATE_POPUP:
					if (m_dwSelMenu == dwMenu)
					{
						pLayer2D->DrawSolidQuad(iX+1,iY+1,iW,iH,cLow);
						pLayer2D->DrawWireQuad(iX,iY,iW,iH,cLow);
					}
					break;
				}
				ui::Wnd::SetUIRendState();
				pLayer2D->TextOut(iTextStartX + 1,iTextStartY + 1,cMenu.m_szText,iLen,FWC_BLACK);
				pLayer2D->TextOut(iTextStartX - 1,iTextStartY - 1,cMenu.m_szText,iLen,FWC_BLACK);
				pLayer2D->TextOut(iTextStartX + 1,iTextStartY - 1,cMenu.m_szText,iLen,FWC_BLACK);
				pLayer2D->TextOut(iTextStartX - 1,iTextStartY + 1,cMenu.m_szText,iLen,FWC_BLACK);
				pLayer2D->TextOut(iTextStartX + 1,iTextStartY,cMenu.m_szText,iLen,FWC_BLACK);
				pLayer2D->TextOut(iTextStartX - 1,iTextStartY,cMenu.m_szText,iLen,FWC_BLACK);	
				pLayer2D->TextOut(iTextStartX,iTextStartY + 1,cMenu.m_szText,iLen,FWC_BLACK);	
				pLayer2D->TextOut(iTextStartX,iTextStartY - 1,cMenu.m_szText,iLen,FWC_BLACK);	
				pLayer2D->TextOut(iTextStartX,iTextStartY,cMenu.m_szText,iLen,cText);	
			}
		}

		//显示弹出
		if (m_eMenuBarState == MENUBAR_STATE_POPUP)
		{
			if (m_dwSelMenu >=0 && m_dwSelMenu < m_dwMenuCnt)
			{
				Menu &cMenu = m_stMenuArray[m_dwSelMenu];
				cMenu.Render();
			}
		}
	}
}


DWORD ui::MenuBar::AddMenu(const char *szText)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

	ASSERT(m_dwMenuCnt < MENUBAR_MAX_MENU);
	Menu &cMenu = m_stMenuArray[m_dwMenuCnt];

	strncpy(cMenu.m_szText,szText,MENUBAR_MAX_MENU_TEXT - 1);
	cMenu.m_szText[POPUPMENU_MAX_TEXT - 1] = 0;

	DWORD dwLastPosition = 0;
	if (m_dwMenuCnt > 0)
		dwLastPosition = m_stMenuArray[m_dwMenuCnt-1].m_rcBtn.right - m_tWindowInfo.rcRect.left;

	int iTextLen = lstrlen(cMenu.m_szText);
	int iTextWidth = pLayer2D->CalcTextWidth(cMenu.m_szText,iTextLen);
	int iTextHeight= pLayer2D->GetFontSize();

	int iWindowHeight = m_tWindowInfo.rcRect.bottom - m_tWindowInfo.rcRect.top;
	int iMenuDistance = pLayer2D->GetFontSize();
	int iMenuHeight   = iTextHeight + FWA_FRAMESIZE * 2;

	SetRect(&cMenu.m_rcBtn,dwLastPosition,(iWindowHeight - iMenuHeight)/2 ,dwLastPosition + iTextWidth + iMenuDistance,iMenuHeight);
	cMenu.m_rcBtn.bottom += cMenu.m_rcBtn.top;
	//BASE WND
	int iOffsetX = m_tWindowInfo.rcRect.left;
	int iOffsetY = m_tWindowInfo.rcRect.top;

	OffsetRect(&cMenu.m_rcBtn,iOffsetX,iOffsetY);

	m_dwMenuCnt++;

	return m_dwMenuCnt - 1;
}

DWORD ui::MenuBar::AddMenuItem(DWORD dwMenu , const char * pszText)
{
	ASSERT(dwMenu < m_dwMenuCnt);
	Menu &cMenu = m_stMenuArray[dwMenu];

	DWORD dwItem = cMenu.AddItem(pszText);
	return dwItem;
}

void  ui::MenuBar::MoveWnd(int iOX,int iOY)
{
	ui::Wnd::MoveWnd(iOX,iOY);
	//move all item
	for(DWORD dwMenu = 0; dwMenu < m_dwMenuCnt; dwMenu++)
	{
		m_stMenuArray[dwMenu].MoveMenu(iOX,iOY);	
	}
}

void  ui::MenuBar::SizeWnd(int iCX,int iCY)
{
	ui::Wnd::SizeWnd(iCX,iCY);

	int iHalfWindowHeight = (m_tWindowInfo.rcRect.bottom - m_tWindowInfo.rcRect.top)/2;

	//move up down 
	for(DWORD dwMenu = 0; dwMenu < m_dwMenuCnt; dwMenu++)
	{
		Menu &cMenu = m_stMenuArray[dwMenu];	
		int iMenuHeight = cMenu.m_rcBtn.bottom - cMenu.m_rcBtn.top;

		cMenu.m_rcBtn.bottom = cMenu.m_rcBtn.top = m_tWindowInfo.rcRect.top + iHalfWindowHeight - iMenuHeight/2;
		cMenu.m_rcBtn.bottom += iMenuHeight;
	}
}

DWORD ui::MenuBar::HitTestMenu(POINT *pPoint)
{
	for(DWORD dwMenu = 0; dwMenu < m_dwMenuCnt;dwMenu++)
	{
		Menu &cMenu = m_stMenuArray[dwMenu];
		if (cMenu.m_dwStyle & MBS_DISABLE) 
			continue;

		if (::PtInRect(&cMenu.m_rcBtn,*pPoint))
		{
			return dwMenu;
		}
	}
	return 0xffffffff;
}

void  ui::MenuBar::HitTest(const POINT *pPoint, ui::Wnd **ppWnd)
{
	if (*ppWnd) return ;
	if (m_tWindowInfo.dwStyle & FWS_VISIBLE)
	{
		if (PtInRect(pPoint) != FW_POINT_OUTSIDE)
		{
			*ppWnd = this;
			return ;
		}

		if (m_eMenuBarState == MENUBAR_STATE_POPUP)
		{
			if (m_dwSelMenu >=0 && m_dwSelMenu < m_dwMenuCnt)
			{
				Menu &cMenu = m_stMenuArray[m_dwSelMenu];
				if (cMenu.PtInRect(pPoint))
				{
					*ppWnd = this;
					return;
				}
			}
		}
	}
}

BOOL  ui::MenuBar::TestSelMenu(DWORD &dwMenu,DWORD &dwItem)
{
	//条件
	//点击在弹出菜单
	//并且选中某个子项目
	if (m_bSelected)
	{
		dwMenu = m_dwSelMenu;
		dwItem = m_dwSelMenuItem;
		m_bSelected = FALSE;
		return TRUE;
	}
	return FALSE;
}

void  ui::MenuBar::ClearMenuItem(DWORD dwMenu)
{
	ASSERT(dwMenu < m_dwMenuCnt);
	Menu &cMenu = m_stMenuArray[dwMenu];
	cMenu.ClearItem();
}

ui::MenuBar::Menu * ui::MenuBar::GetMenu(DWORD dwMenu)
{
	ASSERT(dwMenu < m_dwMenuCnt);
	return &m_stMenuArray[dwMenu];
}

void  ui::MenuBar::DisableMenu(DWORD dwMenu)
{
	ASSERT(dwMenu < m_dwMenuCnt);
	Menu &cMenu = m_stMenuArray[dwMenu];
	cMenu.Disable();
}

void  ui::MenuBar::EnableMenu(DWORD dwMenu)
{
	ASSERT(dwMenu < m_dwMenuCnt);
	Menu &cMenu = m_stMenuArray[dwMenu];
	cMenu.Enable();
}

void  ui::MenuBar::EnableMenuItem(DWORD dwMenu,DWORD dwItem)
{
	ASSERT(dwMenu < m_dwMenuCnt);
	Menu &cMenu = m_stMenuArray[dwMenu];
	cMenu.EnableItem(dwItem);
}

void  ui::MenuBar::SetCheckMenuItem(DWORD dwMenu,DWORD dwItem,BOOL bCheck)
{
	ASSERT(dwMenu < m_dwMenuCnt);
	Menu &cMenu = m_stMenuArray[dwMenu];
	cMenu.SetCheckItem(dwItem,bCheck);
}

void  ui::MenuBar::DisableMenuItem(DWORD dwMenu,DWORD dwItem)
{
	ASSERT(dwMenu < m_dwMenuCnt);
	Menu &cMenu = m_stMenuArray[dwMenu];
	cMenu.DisableItem(dwItem);
}



//__CLASS_(ui::QuickMenu)__
void ui::QuickMenu::Update(void)
{
	ui::Wnd::UpdateDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if ((dwStyle & FWS_VISIBLE) && !(dwStyle & FWS_DISABLE))
	{
		ui::Wnd::UpdateChild();		
	}                          	
}

void ui::QuickMenu::Render(void)
{
	//线
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if ((dwStyle & FWS_VISIBLE) && !(dwStyle & FWS_DISABLE))
	{
		ui::Wnd::RenderDefault();

		ui::Wnd::RenderChild();

		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

		ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

		BOOL bMouseOver = (PtInRect(&pInputInfo->ptMouse) > 0);

		RECT &rc = m_tWindowInfo.rcRect;

		int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;

		pLayer2D->DrawLine(iX+iW/2  ,iY+iH/2-2,pInputInfo->ptMouse.x  ,pInputInfo->ptMouse.y-2,D3DCOLOR_ARGB(200,255,255,255));
		pLayer2D->DrawLine(iX+iW/2-2,iY+iH/2  ,pInputInfo->ptMouse.x-2,pInputInfo->ptMouse.y  ,D3DCOLOR_ARGB(200,255,255,255));
		pLayer2D->DrawLine(iX+iW/2+2,iY+iH/2  ,pInputInfo->ptMouse.x+2,pInputInfo->ptMouse.y  ,D3DCOLOR_ARGB(200,255,255,255));
		pLayer2D->DrawLine(iX+iW/2  ,iY+iH/2+2,pInputInfo->ptMouse.x  ,pInputInfo->ptMouse.y+2,D3DCOLOR_ARGB(200,255,255,255));

		pLayer2D->DrawLine(iX+iW/2  ,iY+iH/2-1,pInputInfo->ptMouse.x  ,pInputInfo->ptMouse.y-1,D3DCOLOR_ARGB(255,0,0,0));
		pLayer2D->DrawLine(iX+iW/2-1,iY+iH/2  ,pInputInfo->ptMouse.x-1,pInputInfo->ptMouse.y  ,D3DCOLOR_ARGB(255,0,0,0));
		pLayer2D->DrawLine(iX+iW/2+1,iY+iH/2  ,pInputInfo->ptMouse.x+1,pInputInfo->ptMouse.y  ,D3DCOLOR_ARGB(255,0,0,0));
		pLayer2D->DrawLine(iX+iW/2  ,iY+iH/2+1,pInputInfo->ptMouse.x  ,pInputInfo->ptMouse.y+1,D3DCOLOR_ARGB(255,0,0,0));

		pLayer2D->DrawLine(iX+iW/2  ,iY+iH/2  ,pInputInfo->ptMouse.x  ,pInputInfo->ptMouse.y  ,D3DCOLOR_ARGB(255,0,0,0));
	}
}

void ui::QuickMenu::SetRadius(DWORD dwRadius)
{
	m_dwRadius = dwRadius;
	//计算本菜单的中心位置

	DWORD iW = m_tWindowInfo.rcRect.right - m_tWindowInfo.rcRect.left + 1;
	DWORD iH = m_tWindowInfo.rcRect.bottom- m_tWindowInfo.rcRect.top + 1;

	int iOX = m_tWindowInfo.rcRect.left + iW / 2;
	int iOY = m_tWindowInfo.rcRect.top  + iH / 2; 

	//计算位置
	DWORD dwCount = (DWORD)m_vItemArray.size();

	if (dwCount)
	{
		DWORD dwAngle = 360 / dwCount;
		Item *pItem(NULL);
		for(DWORD n = 0; n < dwCount; n++)
		{
			//计算每个位置
			float fAngle = D3DXToRadian(dwAngle * n);
			int x = (int)(m_dwRadius * sin(fAngle));
			int y = (int)(m_dwRadius * - cos(fAngle));

			//调整
			pItem = m_vItemArray[n];
			pItem->CenterWndTo(iOX + x,iOY + y);
		}
	}
}


BOOL ui::QuickMenu::Create(const char *pszName, const RECT *pRECT, DWORD dwStyle, ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRECT,dwStyle,pParent);

	return TRUE;
}

void ui::QuickMenu::Destroy(void)
{
	for(int iItem = 0; iItem < (int)m_vItemArray.size(); iItem ++)
	{
		Item *pItem = m_vItemArray[iItem];
		pItem->Destroy();
		SAFEDELETE(pItem);
	}

	m_vItemArray.clear();

	ui::Wnd::Destroy();	
}
BOOL ui::QuickMenu::TestSelMenu(DWORD &dwItem)
{
	for(DWORD n = 0; n < (DWORD)m_vItemArray.size();n++)
	{
		if (m_vItemArray[n]->CheckHover())
		{
			dwItem = n;
			return TRUE;
		}
	}
	return FALSE;
}

void ui::QuickMenu::BeginPopUp()
{
	ModifyStyle(FWS_VISIBLE,0);
	ui::Manager::GetInstance()->BeginPopupWnd(this);
}

void ui::QuickMenu::EndPopUp()
{
	ModifyStyle(0,FWS_VISIBLE);
	ui::Manager::GetInstance()->EndPopupWnd();
}

DWORD ui::QuickMenu::AddItem(const char *pszName,const RECT *pRect)
{
	//增加菜单条目
	Item *pItem = new Item;
	pItem->Create(pszName,pRect,FWS_OUTLINE | FWS_VISIBLE | FWS_DRAG | FWS_SIZE | FWS_FILLBKG,this);
	m_vItemArray.push_back(pItem);


	//计算本菜单的中心位置

	DWORD iW = m_tWindowInfo.rcRect.right - m_tWindowInfo.rcRect.left + 1;
	DWORD iH = m_tWindowInfo.rcRect.bottom- m_tWindowInfo.rcRect.top + 1;

	int iOX = m_tWindowInfo.rcRect.left + iW / 2;
	int iOY = m_tWindowInfo.rcRect.top  + iH / 2; 

	//计算位置
	DWORD dwCount = (DWORD)m_vItemArray.size();

	DWORD dwAngle = 360 / dwCount;

	for(DWORD n = 0; n < dwCount; n++)
	{
		//计算每个位置
		float fAngle = D3DXToRadian(dwAngle * n);
		int x = (int)(m_dwRadius * sin(fAngle));
		int y = (int)(m_dwRadius * - cos(fAngle));

		//调整
		pItem = m_vItemArray[n];
		pItem->CenterWndTo(iOX + x,iOY + y);
	}

	return dwCount;
}



void ui::QuickMenu::Item::Render()
{
	DWORD &dwStyle = m_tWindowInfo.dwStyle;

	if ((dwStyle & FWS_VISIBLE) && !(dwStyle & FWS_DISABLE))
	{
		ui::Wnd::RenderDefault();

		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

		RECT &rc = m_tWindowInfo.rcRect;

		int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;

		switch(m_eState)
		{
		case  QMS_NORMAL:
			break;
		case  QMS_HOVER:
			pLayer2D->DrawSolidQuad(iX,iY,iW,iH,D3DCOLOR_ARGB(192,196,196,255));
			break;
		}
		ui::Wnd::SetUIRendState();
		int iStartX = iX + iW/2 - pLayer2D->CalcTextWidth(m_tWindowInfo.szName,lstrlen(m_tWindowInfo.szName))/2;
		int iStartY = iY + iH/2 - pLayer2D->GetFontSize()/2;;
		int iLen = lstrlen(m_tWindowInfo.szName);
		pLayer2D->TextOut(iStartX + 1,iStartY + 1,m_tWindowInfo.szName,iLen,FWC_BLACK);
		pLayer2D->TextOut(iStartX - 1,iStartY - 1,m_tWindowInfo.szName,iLen,FWC_BLACK);
		pLayer2D->TextOut(iStartX + 1,iStartY - 1,m_tWindowInfo.szName,iLen,FWC_BLACK);
		pLayer2D->TextOut(iStartX - 1,iStartY + 1,m_tWindowInfo.szName,iLen,FWC_BLACK);
		pLayer2D->TextOut(iStartX + 1,iStartY,m_tWindowInfo.szName,iLen,FWC_BLACK);
		pLayer2D->TextOut(iStartX - 1,iStartY,m_tWindowInfo.szName,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iStartX,iStartY + 1,m_tWindowInfo.szName,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iStartX,iStartY - 1,m_tWindowInfo.szName,iLen,FWC_BLACK);	
		pLayer2D->TextOut(iStartX,iStartY,m_tWindowInfo.szName,iLen,m_tWindowInfo.colorText);		

	}	
}

void ui::QuickMenu::Item::Update()
{
	//Test cursor in rect
	DWORD &dwStyle = m_tWindowInfo.dwStyle;

	if ((dwStyle & FWS_VISIBLE) && !(dwStyle & FWS_DISABLE))
	{
		ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

		BOOL bMouseOver = (PtInRect(&pInputInfo->ptMouse) > 0);
		if (!bMouseOver)
		{
			//判断直线是否与自己相交
			ui::Wnd *pParent = GetParent();

			RECT rcParent;
			pParent->GetWndRect(&rcParent);

			int iX = rcParent.left,iY = rcParent.top,iW = rcParent.right - rcParent.left + 1,iH = rcParent.bottom - rcParent.top + 1;


			POINT ptLineA[2] = { {pInputInfo->ptMouse.x,pInputInfo->ptMouse.y},{iX + iW/2,iY + iH/2}};

			bMouseOver = LineRectIntersect(ptLineA);		
		}		

		m_eState = (bMouseOver)? QMS_HOVER : QMS_NORMAL;

		//switch(pInputInfo->eType)
		//{ //处理鼠标
		//case UIMT_MS_BTNUP:
		//	//是否是放开 && hover
		//	if (bMouseOver)
		//	{
		//		m_bSelected = TRUE;
		//		m_eState    = QMS_NORMAL;
		//	}
		//	break;
		//case UIMT_MS_BTNDOWN:
		//	break;
		//default:
		//	break;
		//}
	}

}

BOOL ui::QuickMenu::Item::LineIntersect(POINT ptLineA[2],POINT ptLineB[2])
{
	float x1,x2,x3,x4,y1,y2,y3,y4;
	x1 = (float)ptLineA[0].x;y1 = (float)ptLineA[0].y;
	x2 = (float)ptLineA[1].x;y2 = (float)ptLineA[1].y;
	x3 = (float)ptLineB[0].x;y3 = (float)ptLineB[0].y;
	x4 = (float)ptLineB[1].x;y4 = (float)ptLineB[1].y;

	float denominator = (y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1);
	if (denominator == 0.0f)
	{
		return FALSE;
	}

	float ua = ((x4 - x3)*(y1 - y3) - (y4 - y3)*(x1 - x3))/denominator;
	float ub = ((x2 - x1)*(y1 - y3) - (y2 - y1)*(x1 - x3))/denominator;
	//求出交点
	float x = x1 + ua*(x2-x1);
	float y = y1 + ub*(y2-y1);

	if ( 0.0f <= ua && ua <= 1.0f && 0.0f <= ub && ub <= 1.0f) 
	{
		return TRUE;
	}



	return FALSE;
}

BOOL ui::QuickMenu::Item::LineRectIntersect(POINT ptLine[2])
{
	RECT &rc = m_tWindowInfo.rcRect;
	int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;

	POINT ptLineB[2];
	ptLineB[0].x = iX;
	ptLineB[0].y = iY;
	ptLineB[1].x = iX + iW - 1;
	ptLineB[1].y = iY;

	if (LineIntersect(ptLine,ptLineB)) return TRUE;

	ptLineB[0].x = iX + iW - 1;
	ptLineB[0].y = iY;
	ptLineB[1].x = iX + iW - 1;
	ptLineB[1].y = iY + iH - 1;

	if (LineIntersect(ptLine,ptLineB)) return TRUE;

	ptLineB[0].x = iX;
	ptLineB[0].y = iY + iH - 1;
	ptLineB[1].x = iX + iW - 1;
	ptLineB[1].y = iY + iH - 1;

	if (LineIntersect(ptLine,ptLineB)) return TRUE;

	ptLineB[0].x = iX;
	ptLineB[0].y = iY;
	ptLineB[1].x = iX;
	ptLineB[1].y = iY + iH - 1;

	if (LineIntersect(ptLine,ptLineB)) return TRUE;

	return FALSE;	
}

BOOL ui::QuickMenu::Item::CheckHover(void)
{
	return (m_eState == QMS_HOVER);
}


//__CLASS_(ui::ScrollBar)___
ui::ScrollBar::ScrollBar(void)
{
	m_bDrag = FALSE;
	m_iPageSize = 16;

	m_iSliderRange = 0;
	m_iSliderPos = 0;
	m_iSliderSize = 0;
	m_iRange	 = 0;
	m_bSliding	 = FALSE;
}

ui::ScrollBar::~ScrollBar(void)
{
}

void ui::ScrollBar::SetRange(int iRange)
{
	m_iRange = iRange;
	if (m_iRange < 0 ) m_iRange = 0;

	//计算滚动
	CalcScrollBar();
}

void ui::ScrollBar::Update()
{
	//update default
	ui::Wnd::UpdateDefault();

	//update self
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	BOOL  &bActive = m_tWindowInfo.bActive;

	if ((dwStyle & FWS_VISIBLE) && !(dwStyle & FWS_DISABLE) && bActive)
	{
		ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

		//处理尺寸改变
		CalcScrollBar();

		//process slider

		POINT &pt = pInputInfo->ptMouse;

		int iPosX = FWA_FRAMESIZE + m_tWindowInfo.rcRect.left;
		int iPosY = FWA_FRAMESIZE + m_tWindowInfo.rcRect.top;
		int iWidth=  m_tWindowInfo.rcRect.right - m_tWindowInfo.rcRect.left - FWA_FRAMESIZE - FWA_FRAMESIZE;
		int iHeight= m_tWindowInfo.rcRect.bottom- m_tWindowInfo.rcRect.top- FWA_FRAMESIZE - FWA_FRAMESIZE;

		if (pInputInfo->eType == UIMT_MS_BTNDOWN && pInputInfo->dwData & MK_LBUTTON )
		{
			if (dwStyle & FWS_SBS_VERTICAL) 
			{
				if (pt.x > iPosX && pt.x < iPosX+iWidth)
				{
					//A----B-C------D
					int A,B,C,D;
					A = iPosY;B = A + m_iSliderPos;C = B + m_iSliderSize; D = A + m_iSliderRange + m_iSliderSize;
					if (pt.y > A && pt.y < D)
					{
						if(pt.y < B)
						{
							//up
							m_iSliderPos -= m_iPageSize;
							m_bSliding = TRUE;
						}
						else
							if (pt.y > C)
							{
								//down
								m_iSliderPos+= m_iPageSize;
								m_bSliding = TRUE;
							}
							else
							{
								//Middle
								if (!m_bDrag)
								{
									m_bDrag = TRUE;
									m_iOffset = pt.y - (iPosY + m_iSliderPos);
								}
							}
					}
				}

			}
			else
				if (pt.y > iPosY && pt.y < iPosY + iHeight)
				{
					//A----B-C------D
					int A,B,C,D;
					A = iPosX;B = A + m_iSliderPos;C = B + m_iSliderSize; D = A + m_iSliderRange + m_iSliderSize;
					if (pt.x < B)
					{
						m_iSliderPos -= m_iPageSize;
						m_bSliding = TRUE;
					}
					else
						if (pt.x > C)
						{
							m_iSliderPos += m_iPageSize;
							m_bSliding = TRUE;
						}
						else
						{
							if (!m_bDrag)
							{
								m_bDrag = TRUE;
								m_iOffset = pt.x - ( iPosX + m_iSliderPos);
							}
						}
				}
		}


		if (m_bDrag)
		{
			m_bSliding = TRUE;
			//calc new scroll pos
			if (dwStyle & FWS_SBS_VERTICAL)
			{
				m_iSliderPos = pt.y - m_iOffset -  iPosY;
			}
			else
				m_iSliderPos = pt.x - m_iOffset -  iPosX;

			if (!(pInputInfo->dwMouseButtonState & MK_LBUTTON)) 
			{
				m_bDrag = FALSE;
				m_bSliding = FALSE;
			}
		}

		if (m_iSliderPos >= m_iSliderRange)
		{
			m_iSliderPos = m_iSliderRange;
		}
		if (m_iSliderPos < 0) m_iSliderPos = 0;
	}

}

void ui::ScrollBar::Render()
{
	DWORD &dwStyle = m_tWindowInfo.dwStyle;

	if (dwStyle & FWS_VISIBLE)
	{
		ui::Wnd::RenderDefault();

		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer2D   *pLayer2D = pInterface->GetLayer2D();

		int iPosX = FWA_FRAMESIZE + m_tWindowInfo.rcRect.left;
		int iPosY = FWA_FRAMESIZE + m_tWindowInfo.rcRect.top;
		int iWidth=  m_tWindowInfo.rcRect.right - m_tWindowInfo.rcRect.left - FWA_FRAMESIZE - FWA_FRAMESIZE;
		int iHeight= m_tWindowInfo.rcRect.bottom- m_tWindowInfo.rcRect.top- FWA_FRAMESIZE - FWA_FRAMESIZE;

		if (dwStyle & FWS_SBS_VERTICAL)
		{
			pLayer2D->DrawSolidQuad(iPosX,iPosY + m_iSliderPos,iWidth,m_iSliderSize,m_tWindowInfo.colorBack);
			pLayer2D->DrawWireQuad(iPosX,iPosY + m_iSliderPos,iWidth,m_iSliderSize,m_tWindowInfo.colorHigh);
		}
		else
		{
			pLayer2D->DrawSolidQuad(iPosX + m_iSliderPos,iPosY,m_iSliderSize,iHeight,m_tWindowInfo.colorBack);
			pLayer2D->DrawWireQuad(iPosX + m_iSliderPos,iPosY,m_iSliderSize,iHeight,m_tWindowInfo.colorHigh);		
		}
	}
}

void ui::ScrollBar::SetPos(int iPos)
{
	if (iPos < 0) 
		iPos = 0;
	if (iPos > m_iRange)
		iPos = m_iRange;
	if (m_iRange == 0) 
		m_iSliderPos = 0;
	else
		m_iSliderPos = m_iSliderRange * iPos / m_iRange;

}

int ui::ScrollBar::GetPos(void)
{
	//calc scroll pos
	if (m_iSliderRange == 0 ) 
		return 0;
	else
		return m_iRange * m_iSliderPos / m_iSliderRange;

}

void ui::ScrollBar::CalcScrollBar(void)
{
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	int iWantSliderRange = m_iRange * 2;	
	int iTotalSize = 0;

	if (dwStyle & FWS_SBS_VERTICAL)
	{
		iTotalSize = m_tWindowInfo.rcRect.bottom - m_tWindowInfo.rcRect.top - FWA_FRAMESIZE - FWA_FRAMESIZE;
	}
	else
		iTotalSize = m_tWindowInfo.rcRect.right - m_tWindowInfo.rcRect.left - FWA_FRAMESIZE - FWA_FRAMESIZE;

	m_iSliderSize = iTotalSize - iWantSliderRange;

	if (m_iSliderSize < 16) //滑块最小尺寸
		m_iSliderSize = 16;

	m_iSliderRange = iTotalSize - m_iSliderSize;

	if (m_iSliderPos >= m_iSliderRange)
	{
		m_iSliderPos = m_iSliderRange;
	}
	if (m_iSliderPos < 0)
		m_iSliderPos = 0;
}

void ui::ScrollBar::SetPageSize(int iPageSize)
{
	m_iPageSize = iPageSize;
}

void ui::ScrollBar::SetWndRect(RECT *pRect)
{
	ui::Wnd::SetWndRect(pRect);
	CalcScrollBar();
}

BOOL ui::ScrollBar::IsSliding()
{
	BOOL bSliding = m_bSliding;
	m_bSliding = FALSE;
	return bSliding;
}
//__CLASS_(ui::ListBox)__
ui::ListBox::ListBox(void)
{
	m_iScrollPos = 0;	
	m_iDisplayMax = 0;
	m_iHoverIndex = -1;
	m_iSelectIndex = -1;
	m_bSelChanged = FALSE;
	m_rcWorkarea.left = 0;
	m_rcWorkarea.right= 0;
	m_rcWorkarea.bottom= 0;
	m_rcWorkarea.top= 0;
}

ui::ListBox::~ListBox(void)
{
}

void ui::ListBox::Update()
{
	//Update Default
	ui::Wnd::UpdateDefault();

	//Update Self
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	BOOL  &bActive = m_tWindowInfo.bActive;


	if ((dwStyle & FWS_VISIBLE) && !(dwStyle & FWS_DISABLE))
	{
		ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

		int iFontSize = pLayer2D->GetFontSize();

		Calculate();

		if (::PtInRect(&m_rcWorkarea,pInputInfo->ptMouse))
		{
			int iWAY = m_rcWorkarea.top;
			m_iHoverIndex = m_iScrollPos + (pInputInfo->ptMouse.y - iWAY) / iFontSize;		
			//选择
			if (pInputInfo->eType == UIMT_MS_BTNDOWN && pInputInfo->dwData & MK_LBUTTON)
		 	{
				if (m_iHoverIndex >=0 && m_iHoverIndex < (int)m_vItemArray.size())
				{
					m_bSelChanged = TRUE;
					m_iSelectIndex = m_iHoverIndex;
				}

				if (dwStyle & FWS_LBS_CHECK)
				{
					//Set Check
					if (m_iSelectIndex >=0 && m_iSelectIndex < (int)m_vItemArray.size())
					{
						Item *pItem = m_vItemArray[m_iSelectIndex]; 
						BOOL bChecked = pItem->GetCheck();
						pItem->SetCheck(!bChecked);
					}
				}
			}
		}

		ui::Wnd::UpdateChild();

		if (pInputInfo->eType == UIMT_MS_WHEEL && bActive)
		{
			short izDelta = ((short)HIWORD(pInputInfo->dwData));
			int iPos = m_pScrollBar->GetPos();
			int iPage = m_pScrollBar->GetPageSize();
			iPos += (izDelta>0) ? -iPage : iPage; 								
			m_pScrollBar->SetPos(iPos);			
		}

		//get pos
		m_iScrollPos = m_pScrollBar->GetPos();	
	}

	//Update Child -- don't need

}

void ui::ListBox::Render()
{
	DWORD &dwStyle = m_tWindowInfo.dwStyle;

	if (dwStyle & FWS_VISIBLE)
	{
		ui::Wnd::RenderDefault();

		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

		int iFontSize = pLayer2D->GetFontSize();

		int iWAX = m_rcWorkarea.left;
		int iWAY = m_rcWorkarea.top;
		int iLen  = 0;

		int iTextX = iWAX;

		int iWWidth = m_rcWorkarea.right - m_rcWorkarea.left;

		if (dwStyle & FWS_LBS_CHECK)
		{
			iTextX += iFontSize;
			iWWidth -= iFontSize;
		}

		int iItemLength = (int)m_vItemArray.size();

		for(int i = 0; i < m_iDisplayMax; i++)
		{
			int iItem = i + m_iScrollPos;

			if (iItem >= iItemLength) break;
			if (iItem < 0) 
				continue;

			Item *pItem = m_vItemArray[iItem]; 
			ui::Wnd::SetUIRendState();
			iLen = lstrlen(pItem->GetText());
			/*	
			pLayer2D->TextOut(iTextX + 1,iWAY + 1,pItem->GetText(),iLen,FWC_BLACK);
			pLayer2D->TextOut(iTextX - 1,iWAY - 1,pItem->GetText(),iLen,FWC_BLACK);
			pLayer2D->TextOut(iTextX + 1,iWAY - 1 ,pItem->GetText(),iLen,FWC_BLACK);
			pLayer2D->TextOut(iTextX - 1,iWAY + 1,pItem->GetText(),iLen,FWC_BLACK);
			pLayer2D->TextOut(iTextX + 1,iWAY,pItem->GetText(),iLen,FWC_BLACK);
			pLayer2D->TextOut(iTextX - 1,iWAY,pItem->GetText(),iLen,FWC_BLACK);
			pLayer2D->TextOut(iTextX,iWAY + 1,pItem->GetText(),iLen,FWC_BLACK);
			pLayer2D->TextOut(iTextX,iWAY - 1,pItem->GetText(),iLen,FWC_BLACK);*/
			pLayer2D->TextOut(iTextX,iWAY,pItem->GetText(),iLen,m_tWindowInfo.colorText);
			
			if (dwStyle & FWS_LBS_CHECK)
			{
				if (pItem->GetCheck())
					//pLayer2D->DrawSolidQuad(iWAX+4,iWAY+4,iFontSize-8,iFontSize-8,m_tWindowInfo.colorHigh);
					pLayer2D->TextOut(iWAX, iWAY, "∨", 2, m_tWindowInfo.colorHigh);
			}
			else
			{
				if (m_iSelectIndex == iItem)
					pLayer2D->DrawSolidQuad(iTextX,iWAY,iWWidth,iFontSize,m_tWindowInfo.colorLow);						
			}

			if (m_iHoverIndex == iItem)
			{
				pLayer2D->DrawWireQuad(iTextX,iWAY,iWWidth,iFontSize,FWC_WHITE);
			}

			iWAY += iFontSize;
		}

		ui::Wnd::RenderChild();
	}
}

void ui::ListBox::InsertItem(const char *pszText,DWORD dwData)
{
	Item *pItem = new Item;
	pItem->SetText(pszText);
	pItem->SetData(dwData);
	pItem->SetCheck(FALSE);

	m_vItemArray.push_back(pItem);
	m_pScrollBar->SetRange((int)m_vItemArray.size());

	Calculate();
}

BOOL ui::ListBox::Create(const char *pszName, const RECT *pRect, DWORD dwStyle, ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);
	//TODO:

	RECT rcSB = {0,0,0,0};

	m_pScrollBar = new ScrollBar;	
	m_pScrollBar->Create("FW_SB_VERT",&rcSB,FWS_VISIBLE | FWS_SBS_VERTICAL,this);
	m_pScrollBar->m_tWindowInfo.bActive = TRUE;
	Calculate();

	return TRUE;
}
void ui::ListBox::MoveWnd(int iOX,int iOY)
{
	ui::Wnd::MoveWnd(iOX,iOY);
	Calculate();
}

void ui::ListBox::Destroy()
{
	for(int i = 0; i < (int)m_vItemArray.size(); i++)
	{
		SAFEDELETE(m_vItemArray[i]);
	}
	m_vItemArray.clear();

	SAFEDESTROY(m_pScrollBar);

	ui::Wnd::Destroy();
}


void ui::ListBox::Calculate()
{
	//计算能显示的行数	
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

	int iFontSize = pLayer2D->GetFontSize();

	m_iDisplayMax = (m_rcWorkarea.bottom - m_rcWorkarea.top) / iFontSize;

	int iItemCount = (int)m_vItemArray.size();
	m_pScrollBar->SetRange( iItemCount - m_iDisplayMax);
	if (m_iDisplayMax >= iItemCount) 
	{
		DWORD dwStyle = m_pScrollBar->GetStyle() & ~FWS_VISIBLE;
		m_pScrollBar->SetStyle(dwStyle);
	}
	else
	{
		DWORD dwStyle = m_pScrollBar->GetStyle() | FWS_VISIBLE;
		m_pScrollBar->SetStyle(dwStyle);
	}

	BOOL bSBVisible = m_pScrollBar->GetStyle() & FWS_VISIBLE;

	//滚动区域
	int iPosX = m_tWindowInfo.rcRect.left;
	int iPosY = m_tWindowInfo.rcRect.top;
	int iWidth= m_tWindowInfo.rcRect.right - m_tWindowInfo.rcRect.left;
	int iHeight= m_tWindowInfo.rcRect.bottom - m_tWindowInfo.rcRect.top;

	RECT rcScroll;
	if (bSBVisible)
	{
		m_pScrollBar->GetWndRect(&rcScroll);
		rcScroll.left = iPosX + iWidth - FW_LB_SCROLLBAR_WIDTH - FWA_FRAMESIZE;
		rcScroll.top  = iPosY + FWA_FRAMESIZE;
		rcScroll.right= rcScroll.left + FW_LB_SCROLLBAR_WIDTH;
		rcScroll.bottom= iPosY + iHeight - FWA_FRAMESIZE - FWA_FRAMESIZE;
		m_pScrollBar->SetWndRect(&rcScroll);
	}

	//工作区域
	m_rcWorkarea.left = iPosX + FWA_FRAMESIZE;
	m_rcWorkarea.top  = iPosY + FWA_FRAMESIZE;
	m_rcWorkarea.right= bSBVisible? (rcScroll.left - FWA_FRAMESIZE) : (m_tWindowInfo.rcRect.right - FWA_FRAMESIZE);
	m_rcWorkarea.bottom= m_tWindowInfo.rcRect.bottom - FWA_FRAMESIZE;
}

void ui::ListBox::DeleteAllItem()
{
	for(int i = 0; i < (int)m_vItemArray.size(); i++)
	{
		SAFEDELETE(m_vItemArray[i]);
	}
	m_vItemArray.clear();

	m_pScrollBar->SetPos(0);
	m_pScrollBar->SetRange(0);

	m_iScrollPos = 0;	
	m_iDisplayMax = 0;
	m_iHoverIndex = -1;
	m_iSelectIndex = -1;
	m_bSelChanged = FALSE;

	Calculate();
}

void ui::ListBox::DeleteItem(int iItem)
{
	int iCount = 0;
	std::vector<ui::ListBox::Item *>::iterator it;
	for(it = m_vItemArray.begin();it != m_vItemArray.end();it++)
	{
		if (iCount == iItem)
		{
			m_vItemArray.erase(it);
			break;
		}
		iCount++;
	}

	m_pScrollBar->SetPos(0);
	m_pScrollBar->SetRange(0);

	m_iScrollPos = 0;	
	m_iDisplayMax = 0;
	m_iHoverIndex = -1;
	m_iSelectIndex = -1;
	m_bSelChanged = FALSE;

	Calculate();
}

BOOL ui::ListBox::GetItemChecked(int iItem)
{
	if (iItem >= 0 && iItem < (int)m_vItemArray.size())
	{
		Item *pItem = m_vItemArray[iItem];
		return pItem->GetCheck();
	}
	ASSERT(0);
	return FALSE;
}

void ui::ListBox::SetItemChecked(int iItem,BOOL bValue)
{
	if (iItem >= 0 && iItem < (int)m_vItemArray.size())
	{
		Item *pItem = m_vItemArray[iItem];
		pItem->SetCheck(bValue);
	}
}
//__CLASS_(ui::LineEditBox)___
ui::EditBox::EditBox()
{
	ZeroMemory(m_pwInputData,(INPUT_MAX_CHAR+1)*sizeof(WORD));
	ZeroMemory(m_pwWidthTable,(INPUT_MAX_CHAR+2)*sizeof(WORD));
	m_iCursor   = 0;
	m_iSelStart = 0;
	m_iSelEnd   = 0;
	m_iDisplayPos = 0;

	UINT uiParam;
	SystemParametersInfo(SPI_GETKEYBOARDDELAY,0,&uiParam,0);
	m_dwRepeatRate = uiParam*120;

	m_dwLastRepeatTime = timeGetTime();

	m_eControlKeyStatus = CONTROL_KEY_NONE;

	m_bShiftDown = FALSE;
	m_bCtrlDown  = FALSE;

	m_dwCursorDelay = GetCaretBlinkTime();

	m_dwLastCursorTime = 0;

	m_bEnter = FALSE;

	m_bReadOnly = false;

}

ui::EditBox::~EditBox()
{

}

void ui::EditBox::Update()
{
	if ((m_tWindowInfo.dwStyle & FWS_VISIBLE) && !(m_tWindowInfo.dwStyle & FWS_DISABLE))
	{
		if (m_tWindowInfo.bActive)
		{
			if(m_bReadOnly)
				return;
			ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

			m_bShiftDown = pInputInfo->byKeyBuffer[DIK_LSHIFT];
			m_bCtrlDown  = pInputInfo->byKeyBuffer[DIK_LCONTROL];

			BOOL bMouseOver = (PtInRect(&pInputInfo->ptMouse) > 0);
			//handle mouse
			switch(pInputInfo->eType)
			{ 
			case UIMT_MS_BTNUP:
				break;
			case UIMT_MS_BTNDOWN:
				break;
			case UIMT_KB_KEYUP:
				{
					m_eControlKeyStatus = CONTROL_KEY_NONE;

				}
				break;
			case UIMT_KB_KEYDOWN:
				{
					switch(pInputInfo->dwData)
					{
					case DIK_LEFT:
						m_eControlKeyStatus = CONTROL_KEY_LEFT;
						break;
					case DIK_RIGHT:
						m_eControlKeyStatus = CONTROL_KEY_RIGHT;
						break;
					case DIK_BACKSPACE:
						m_eControlKeyStatus = CONTROL_KEY_BACKSPACE;
						break;
					case DIK_DELETE:
						m_eControlKeyStatus = CONTROL_KEY_DELETE;
						break;
					case DIK_HOME:
						m_eControlKeyStatus = CONTROL_KEY_HOME;
						break;
					case DIK_END:
						m_eControlKeyStatus = CONTROL_KEY_END;
						break;
					case DIK_RETURN:
						m_bEnter = TRUE;
						break;
					}
				}
				break;
			case UIMT_KB_CHAR:
				{
					static BYTE byLastChar = 0;
					BYTE byChar= (BYTE)pInputInfo->dwData;

					//跳过某些功能键盘
					//BACKSPACE					
					if (byChar <= 0x20)
					{
						break;
					}

					if (byChar< 0x80)//asc
					{
						if (InsertText(m_iCursor,(char*)&byChar,1))
						{
							m_iCursor ++;m_iSelEnd = m_iSelStart = m_iCursor;
							int iLen = GetLength();
							if (m_iCursor > iLen) m_iCursor = iLen;
							RightScroll();
						}
					}
					else
						if (byLastChar>0x80)
						{
							WORD wChar= MAKEWORD(byLastChar,byChar);
							if (InsertText(m_iCursor,(char *)&wChar,2))
							{
								m_iCursor ++;m_iSelEnd = m_iSelStart = m_iCursor;
								int iLen = GetLength();
								if (m_iCursor > iLen) m_iCursor = iLen;
								RightScroll();
							}
							byLastChar = 0;

						}
						else
							byLastChar = byChar;
				}
				break;
			default:
				break;
			}

			switch(m_eControlKeyStatus)
			{
			case CONTROL_KEY_LEFT:
				{
					DWORD dwCurTime = timeGetTime();
					if ((dwCurTime - m_dwLastRepeatTime) > m_dwRepeatRate)
					{
						Left();
						m_dwLastRepeatTime = dwCurTime;
					}
				}
				break;
			case CONTROL_KEY_RIGHT:
				{
					DWORD dwCurTime = timeGetTime();
					if ((dwCurTime - m_dwLastRepeatTime) > m_dwRepeatRate)
					{
						Right();
						m_dwLastRepeatTime = dwCurTime;
					}
				}	
				break;
			case CONTROL_KEY_BACKSPACE:
				{
					DWORD dwCurTime = timeGetTime();
					if ((dwCurTime - m_dwLastRepeatTime) > m_dwRepeatRate)
					{
						Backspace();
						m_dwLastRepeatTime = dwCurTime;
					}
				}
				break;
			case CONTROL_KEY_DELETE:
				{
					DWORD dwCurTime = timeGetTime();
					if ((dwCurTime - m_dwLastRepeatTime) > m_dwRepeatRate)
					{
						Delete();
						m_dwLastRepeatTime = dwCurTime;
					}
				}
				break;
			case CONTROL_KEY_HOME:
				{
					DWORD dwCurTime = timeGetTime();
					if ((dwCurTime - m_dwLastRepeatTime) > m_dwRepeatRate)
					{
						Home();
						m_dwLastRepeatTime = dwCurTime;
					}
				}
				break;
			case CONTROL_KEY_END:
				{
					DWORD dwCurTime = timeGetTime();
					if ((dwCurTime - m_dwLastRepeatTime) > m_dwRepeatRate)
					{
						End();
						m_dwLastRepeatTime = dwCurTime;
					}
				}
				break;
			}

			//set cursor blink
			DWORD dwCurTime = timeGetTime();
			if ((dwCurTime - m_dwLastCursorTime) > m_dwCursorDelay)
			{
				m_bCursorBlink = !m_bCursorBlink;
				m_dwLastCursorTime = dwCurTime;
			}
			if (KEYDOWN(VK_CONTROL) )
			{
				if(KEYDOWN(VK_V))
				{
					HANDLE hData;
					LPVOID pData;
					string sBuf;

					if ( !IsClipboardFormatAvailable(CF_TEXT) )	return;
					extern HWND hWndMain;
					OpenClipboard(hWndMain);
					hData = GetClipboardData(CF_TEXT);
					pData = GlobalLock(hData);
					sBuf = (char *)pData;
					char strText[128];
					GetEditText(strText,128);

					DeleteText(0,strlen(strText));
					InsertText(0,sBuf.c_str(),sBuf.length());
					
					GlobalUnlock(hData);
					CloseClipboard();
				}
			}
		}
		else
			m_eControlKeyStatus = CONTROL_KEY_NONE;

		
		ui::Wnd::Update();
	}
}

void ui::EditBox::Render()
{
	RECT &rc = m_tWindowInfo.rcRect;
	DWORD &dwStyle = m_tWindowInfo.dwStyle;

	if (dwStyle & FWS_VISIBLE)
	{
		ui::Wnd::RenderDefault();

		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

		PreCalcDisplayData();		

		int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;

		//make display stirng...
		char szTemp[255];
		MakeDisplayString(szTemp,255);
		ui::Wnd::SetUIRendState();
		int iTexLen = lstrlen(szTemp);
	/*	pLayer2D->TextOut(m_iTempTextX + 1,m_iTempTextY + 1,(const char*)szTemp,iTexLen,FWC_BLACK);
		pLayer2D->TextOut(m_iTempTextX - 1,m_iTempTextY - 1,(const char*)szTemp,iTexLen,FWC_BLACK);
		pLayer2D->TextOut(m_iTempTextX + 1,m_iTempTextY - 1,(const char*)szTemp,iTexLen,FWC_BLACK);
		pLayer2D->TextOut(m_iTempTextX - 1,m_iTempTextY + 1,(const char*)szTemp,iTexLen,FWC_BLACK);
		pLayer2D->TextOut(m_iTempTextX + 1,m_iTempTextY,(const char*)szTemp,iTexLen,FWC_BLACK);
		pLayer2D->TextOut(m_iTempTextX - 1,m_iTempTextY,(const char*)szTemp,iTexLen,FWC_BLACK);
		pLayer2D->TextOut(m_iTempTextX,m_iTempTextY + 1,(const char*)szTemp,iTexLen,FWC_BLACK);
		pLayer2D->TextOut(m_iTempTextX,m_iTempTextY - 1,(const char*)szTemp,iTexLen,FWC_BLACK);*/
		pLayer2D->TextOut(m_iTempTextX,m_iTempTextY,(const char*)szTemp,iTexLen,m_tWindowInfo.colorText);

		if (m_tWindowInfo.bActive)
		{
			//draw sel
			if (m_iTempSelWidth > 0)
				pLayer2D->DrawSolidQuad( m_iTempTextX+m_iTempSelX,m_iTempTextY,m_iTempSelWidth,m_iTempTextH,D3DCOLOR_ARGB(96,10,40,100));

			//draw cursor
			if (m_bCursorBlink)
			{
				pLayer2D->DrawWireQuad(m_pwWidthTable[m_iCursor] - m_pwWidthTable[m_iDisplayPos] + m_iTempTextX,m_iTempTextY,1,m_iTempTextH,D3DCOLOR_ARGB(255,255,255,255));
				pLayer2D->DrawWireQuad(m_pwWidthTable[m_iCursor] - m_pwWidthTable[m_iDisplayPos] + m_iTempTextX+1,m_iTempTextY+1,1,m_iTempTextH,D3DCOLOR_ARGB(128,0,0,0));
			}
		}
	}
}

int ui::EditBox::InsertText(int iPos, const char *pText,int iLen)
{
	WORD wTempA[INPUT_MAX_CHAR+1];
	ZeroMemory(wTempA,sizeof(WORD)*(INPUT_MAX_CHAR+1));
	WORD wTempB[INPUT_MAX_CHAR+1];
	ZeroMemory(wTempB,sizeof(WORD)*(INPUT_MAX_CHAR+1));
	WORD wTempC[INPUT_MAX_CHAR+1];
	ZeroMemory(wTempC,sizeof(WORD)*(INPUT_MAX_CHAR+1));

	if (!(iPos < INPUT_MAX_CHAR)) return 0;

	WStrncpy(wTempA,m_pwInputData,iPos);
	WStrcpy(wTempC,m_pwInputData+iPos);

	int iCnt = 0,iWCnt = 0;
	while(iCnt < iLen)
	{
		if ((BYTE)pText[iCnt++] > 0x80) iCnt++;
		iWCnt++;
	}

	int iLeft = INPUT_MAX_CHAR - GetLength();
	int iInsertLen = iWCnt;
	if (iInsertLen > iLeft) iInsertLen = iLeft;

	iCnt = 0;int iCpy = 0;
	while(iCpy < iInsertLen)
	{
		WORD wChar;		
		BYTE byChar = pText[iCnt++];
		wChar = (byChar > 0x80) ? MAKEWORD(byChar,pText[iCnt++]) : byChar;
		wTempB[iCpy++] = wChar;		
	}

	WStrcat(wTempA,wTempB);
	WStrcat(wTempA,wTempC);
	WStrcpy(m_pwInputData,wTempA);

	CalcStringWidthTable();

	return iInsertLen;
}

void ui::EditBox::Left()
{
	BOOL bScrollLeft = (m_iCursor == m_iDisplayPos);	
	m_iCursor --;
	if (m_iCursor < 0) m_iCursor = 0;

	if (!m_bShiftDown) 
		m_iSelStart = m_iSelEnd = m_iCursor;
	else
		m_iSelEnd = m_iCursor;
	if (bScrollLeft) LeftScroll(1);	
}

void ui::EditBox::Right()
{
	int iLen = GetLength();
	m_iCursor ++;
	if (m_iCursor > iLen) m_iCursor = iLen;

	if (!m_bShiftDown) 
		m_iSelStart = m_iSelEnd = m_iCursor;
	else
		m_iSelEnd = m_iCursor;
	RightScroll();
}

void ui::EditBox::WStrcpy(WORD *pDst, WORD *pSrc)
{
	WORD *pS = pSrc,*pD = pDst;
	while(*pS !=0)
	{
		*pD++ = *pS++;
	}
	*pD = 0;
}

int ui::EditBox::WStrlen(WORD *pString)
{
	int iCount =0;
	while(pString[iCount] != 0) iCount++;
	return iCount;
}

int ui::EditBox::GetLength()
{
	return WStrlen(m_pwInputData);
}

void ui::EditBox::MakeDisplayString(char *pszString, int iLen)
{
	ZeroMemory(pszString,iLen);
	int iCountC = 0;
	int iCountW = m_iDisplayPos;	
	while(iCountC< iLen && iCountW < m_iDisplayPos + m_iTempDisplayLen)
	{
		WORD wChar = m_pwInputData[iCountW++];
		if ( (wChar & 0xff) < 0x80)
		{
			pszString[iCountC++] = (BYTE)wChar;
		}
		else
		{
			pszString[iCountC++] = (wChar & 0xff);
			pszString[iCountC++] = (wChar & 0xff00) >> 8;
		}
	}
}

void ui::EditBox::Backspace()
{
	if (m_iCursor == m_iDisplayPos)
	{
		LeftScroll(8);
	}

	if (!m_iCursor) return;
	Left();
	DeleteText(m_iCursor,1);
}

void ui::EditBox::DeleteText(int iPos, int iCnt)
{
	if (iPos < 0 || iPos+iCnt > GetLength()) return ;
	if (iPos >= INPUT_MAX_CHAR) return;

	WORD wTemp[INPUT_MAX_CHAR+1];
	ZeroMemory(wTemp,(INPUT_MAX_CHAR+1)*sizeof(WORD));
	WStrcpy(wTemp,m_pwInputData+iPos+iCnt);

	int iLeftLen = WStrlen(wTemp);
	if (iLeftLen)
		memcpy(m_pwInputData+iPos,wTemp,iLeftLen*sizeof(WORD));
	m_pwInputData[iPos+iLeftLen] = 0;
	CalcStringWidthTable();
}

void ui::EditBox::CalcStringWidthTable()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();
	render::Interface::Layer2D::Font *pFont  = pLayer2D->GetFont();

	//write width table
	int iCurWidth = 0;
	int iWChar;
	for(iWChar = 0; iWChar < GetLength(); iWChar ++)
	{
		BYTE byChar = (BYTE)m_pwInputData[iWChar];
		m_pwWidthTable[iWChar] = iCurWidth;
		iCurWidth += (byChar < 0x80)? pFont->GetFontSize() / 2 : pFont->GetFontSize();
	}
	m_pwWidthTable[iWChar] = iCurWidth;//tail position
}

void ui::EditBox::Delete()
{
	int iStart = (m_iSelStart < m_iSelEnd)? m_iSelStart : m_iSelEnd ;
	int iSelLen   = abs(m_iSelStart - m_iSelEnd);
	if (iSelLen)
	{
		DeleteText(iStart,iSelLen);
		m_iSelStart = m_iSelEnd = m_iCursor = iStart;
		if (m_iDisplayPos > m_iCursor) 
		{
			m_iDisplayPos = m_iCursor;
			LeftScroll(8);
		}
	}
	else
	{
		int iLen = GetLength();
		if (m_iCursor>=iLen) return;
		DeleteText(m_iCursor,1);
	}
}

void ui::EditBox::Home()
{
	m_iCursor = m_iDisplayPos = 0;
	if (!m_bShiftDown) 
		m_iSelStart = m_iSelEnd = m_iCursor;
	else
		m_iSelEnd = m_iCursor;	
}

void ui::EditBox::End()
{
	int iLen = GetLength();
	m_iCursor = iLen;
	if (!m_bShiftDown) 
		m_iSelStart = m_iSelEnd = m_iCursor;
	else
		m_iSelEnd = m_iCursor;
	RightScroll();
}

void ui::EditBox::WStrncpy(WORD *pDst, WORD *pSrc, int iLen)
{
	WORD *pS = pSrc,*pD = pDst;
	while(*pS !=0 && (iLen-- > 0))
	{
		*pD++ = *pS++;
	}
}

void ui::EditBox::WStrcat(WORD *pDst, WORD *pSrc)
{
	int iLen = WStrlen(pDst);
	WStrcpy(pDst+iLen,pSrc);
}

void ui::EditBox::LeftScroll(int iChars)
{
	m_iDisplayPos -= iChars;
	if (m_iDisplayPos < 0 ) m_iDisplayPos = 0;
}

void ui::EditBox::SetReadOnly(BOOL bValue)
{
	m_bReadOnly = bValue;
}

void ui::EditBox::RightScroll()
{
	//clip 
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();
	render::Interface::Layer2D::Font *pFont  = pLayer2D->GetFont();

	RECT &rc = m_tWindowInfo.rcRect;
	DWORD &dwStyle = m_tWindowInfo.dwStyle;

	int iX = rc.left,iY = rc.top,iW = rc.right - rc.left +1 ,iH = rc.bottom - rc.top + 1;

	int iFontHeight = pFont->GetFontSize();

	int iTexX = iX+4;int iTexY = iY + (iH/2 - iFontHeight/2);
	int iTexW = iW-8;int iTexH = (iH/2 -iFontHeight/2);

	//check scroll is necessary?
	int iDisplayiWidth = m_pwWidthTable[m_iCursor] - m_pwWidthTable[m_iDisplayPos];
	if (iDisplayiWidth < iTexW) return ;

	//check width
	for(int iCur = m_iDisplayPos; iCur <= m_iCursor; iCur++)
	{	
		int iDisplayiWidth = m_pwWidthTable[m_iCursor] - m_pwWidthTable[iCur];
		if (iDisplayiWidth < iTexW)
		{
			m_iDisplayPos = iCur;
			break;
		}
	}
}

void ui::EditBox::PreCalcDisplayData()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();
	render::Interface::Layer2D::Font *pFont  = pLayer2D->GetFont();

	RECT &rc = m_tWindowInfo.rcRect;
	DWORD &dwStyle = m_tWindowInfo.dwStyle;

	int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;

	m_iTempFontHeight = pFont->GetFontSize();

	m_iTempTextH = m_iTempFontHeight+2;
	m_iTempTextW = iW-8;
	m_iTempTextX = iX+4;
	m_iTempTextY = iY + (iH/2 -m_iTempFontHeight/2);	

	int iDisplayX	  = m_pwWidthTable[m_iDisplayPos];
	int iCurWChar;
	for(iCurWChar = GetLength(); iCurWChar >= 0; iCurWChar--)
	{
		if ( m_pwWidthTable[iCurWChar] - iDisplayX < m_iTempTextW)
		{
			break;
		}
	}

	m_iTempDisplayLen = iCurWChar - m_iDisplayPos;

	//clip
	int iSS = (m_iSelStart > m_iSelEnd)? m_iSelEnd : m_iSelStart;
	int iSE = iSS+ abs(m_iSelEnd - m_iSelStart);
	int iCS = (iSS  > m_iDisplayPos)? iSS : m_iDisplayPos;
	int iCE = (iSE < m_iDisplayPos+m_iTempDisplayLen) ? iSE : m_iDisplayPos+m_iTempDisplayLen;

	m_iTempSelX		= m_pwWidthTable[iCS] - m_pwWidthTable[m_iDisplayPos];
	m_iTempSelWidth = m_pwWidthTable[iCE] - m_pwWidthTable[m_iDisplayPos] - m_iTempSelX;	
}

BOOL ui::EditBox::CheckEnter()
{
	if (!m_bEnter) return FALSE;
	m_bEnter = FALSE;
	return TRUE;
}
void ui::EditBox::SetEditText(const char *pszSting)
{
	if (pszSting && strlen(pszSting) >= 0)
	{
		Clear();
		InsertText(0,pszSting,strlen(pszSting));
	}
}
void ui::EditBox::GetEditText(char *pszString,int iLen)
{
	ZeroMemory(pszString,iLen);
	int iCountC = 0;
	int iCountW = 0;
	int iWLen = GetLength();
	while(iCountC< iLen && iCountW < iWLen)
	{
		WORD wChar = m_pwInputData[iCountW++];
		if ( (wChar & 0xff) < 0x80)
		{
			pszString[iCountC++] = (BYTE)wChar;
		}
		else
		{
			pszString[iCountC++] = (wChar & 0xff);
			pszString[iCountC++] = (wChar & 0xff00) >> 8;
		}
	}
}

void ui::EditBox::Clear()
{
	DeleteText(0,GetLength());
}

//__CLASS_(ui::TabCtl)____  
ui::TabCtl::TabCtl(void)
{
	m_pBtnLeft  = NULL;
	m_pBtnRight = NULL;
	m_dwHighLightLabel = 0;
	m_iLabelDisplayPos = 0;
	m_iLabelDisplayCount = 0;

}

ui::TabCtl::~TabCtl(void)
{
}

BOOL ui::TabCtl::Create(const char *pszName, const RECT *pRect, DWORD dwStyle, ui::Wnd *pParent)
{
	Wnd::Create(pszName,pRect,dwStyle,pParent);


	RECT rcBtn = {0,0,32,12};
	m_pBtnLeft = new ui::Button;
	m_pBtnLeft->Create("<",&rcBtn,FWS_VISIBLE,this);

	m_pBtnRight = new ui::Button;
	m_pBtnRight->Create(">",&rcBtn,FWS_VISIBLE,this);

	return TRUE;
}

void ui::TabCtl::Destroy()
{
	this->DeleteAllLabel();	
	SAFEDESTROY(m_pBtnLeft);       	
	SAFEDESTROY(m_pBtnRight);      	

	Wnd::Destroy();
}

void ui::TabCtl::AddLabel(const char * pszName)
{
	Label *pLabel = new Label;
	m_vectorLabel.push_back(pLabel);

	pLabel->Create(pszName);
}

void ui::TabCtl::DeleteAllLabel()
{
	for(std::vector<Label *>::iterator it = m_vectorLabel.begin(); it != m_vectorLabel.end(); it++)
	{	
		Label *pLabel = *it;
		SAFEDELETE(pLabel);
	}
	m_vectorLabel.clear();
}



void ui::TabCtl::Update()
{
	ui::Wnd::UpdateDefault();

	//SELF
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	BOOL  &bActive = m_tWindowInfo.bActive;

	//---SELF	

	if ((dwStyle & FWS_VISIBLE) && !(dwStyle & FWS_DISABLE))
	{
		ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

		SetupButton();		
		CalcDisplayLabel();		

		RECT &rc = m_tWindowInfo.rcRect;

		int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;

		int iPosX = iX;
		int iPosY = iY + FTABCTL_ITEM_OFFSET_TOP;
		int iCurSelIndex = -1;
		for(int iItem = 0; iItem < m_iLabelDisplayCount; iItem++)
		{
			Label *pLabel = m_vectorLabel[iItem + m_iLabelDisplayPos];
			pLabel->SetHighLight(FALSE);

			int iLabelWidth  = pLabel->GetWidth();
			int iLabelHeight = pLabel->GetHeight();

			RECT rcTemp = { iPosX,iPosY,iPosX + iLabelWidth,iPosY + iLabelHeight};
			pLabel->SetPosition(iPosX,iPosY);

			BOOL bOver = ::PtInRect(&rcTemp,pInputInfo->ptMouse);

			switch(pInputInfo->eType)
			{ //处理鼠标
			case UIMT_MS_BTNUP:
				break;
			case UIMT_MS_BTNDOWN:
				if (pInputInfo->dwMouseButtonState &= MK_LBUTTON)
				{
					if (bOver)
						iCurSelIndex = iItem + m_iLabelDisplayPos;
				}
				break;
			default:
				break;
			}
			iPosX += iLabelWidth;
		}

		if (iCurSelIndex != -1)
		{
			m_dwHighLightLabel = (DWORD)iCurSelIndex;
		}

		if (m_dwHighLightLabel < (int)m_vectorLabel.size())
		{
			m_vectorLabel[m_dwHighLightLabel]->SetHighLight(TRUE);
		}
	}

	Wnd::UpdateChild();

}

void ui::TabCtl::Render()
{
	Wnd::RenderDefault();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		//	render::Interface *pInterface = render::Interface::GetInstance();
		//	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

		RECT &rc = m_tWindowInfo.rcRect;
		DWORD &dwStyle = m_tWindowInfo.dwStyle;

		int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;

		for(int iItem = 0; iItem < m_iLabelDisplayCount; iItem++)
		{		
			m_vectorLabel[iItem + m_iLabelDisplayPos]->DrawLabel();			
		}

		Wnd::RenderChild();
	}

}

void ui::TabCtl::CalcLabelDisplayCount()
{
	RECT &rc = m_tWindowInfo.rcRect;
	int iWndWidth = rc.right - rc.left +1 + FTABCTL_ITEM_OFFSET_RIGHT;
	int iWidth = 0;
	int iItem;
	for(iItem = m_iLabelDisplayPos; iItem < (int)m_vectorLabel.size(); iItem ++)
	{		
		iWidth+= m_vectorLabel[iItem]->GetWidth();
		if (iWidth > iWndWidth) break;
	}

	m_iLabelDisplayCount = iItem - m_iLabelDisplayPos ;
	if (m_iLabelDisplayCount < 0) m_iLabelDisplayCount =0;

}

void ui::TabCtl::Label::DrawLabel()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

	int iX = m_iX - m_iNameWidth/2 + m_iWidth / 2;
	int iY = m_iY; 
	int iTexLen = lstrlen(m_szText);

	if (!m_bHighLight)
	{
		pLayer2D->DrawSolidQuad(iX,iY+3,m_iWidth,m_iHeight - 3,FWC_MIDDLE);
		ui::Wnd::SetUIRendState();
		pLayer2D->TextOut(iX,iY + 3,m_szText,iTexLen,0x8fFFFFFF);
	}
	else
	{
		pLayer2D->DrawSolidQuad(iX,iY,m_iWidth,m_iHeight,FWC_BACK);
		ui::Wnd::SetUIRendState();
		pLayer2D->TextOut(iX + 1,iY + 1,m_szText,iTexLen,FWC_BLACK);
		pLayer2D->TextOut(iX - 1,iY - 1,m_szText,iTexLen,FWC_BLACK);
		pLayer2D->TextOut(iX + 1,iY - 1,m_szText,iTexLen,FWC_BLACK);
		pLayer2D->TextOut(iX - 1,iY + 1,m_szText,iTexLen,FWC_BLACK);
		pLayer2D->TextOut(iX + 1,iY,m_szText,iTexLen,FWC_BLACK);
		pLayer2D->TextOut(iX - 1,iY,m_szText,iTexLen,FWC_BLACK);
		pLayer2D->TextOut(iX,iY + 1,m_szText,iTexLen,FWC_BLACK);
		pLayer2D->TextOut(iX,iY - 1,m_szText,iTexLen,FWC_BLACK);
		pLayer2D->TextOut(iX,iY, m_szText,iTexLen,FWC_TEXT);
	}
}

void ui::TabCtl::Label::Create(const char *pszName)
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

	m_iNameWidth  = pLayer2D->CalcTextWidth(pszName,lstrlen(pszName));
	m_iNameHeight = pLayer2D->GetFontSize();

	m_iWidth  = m_iNameWidth + pLayer2D->GetFontSize();
	m_iHeight = m_iNameHeight + 4;
	strcpy(m_szText,pszName);			
}

const char *ui::TabCtl::GetLabelText(DWORD dwLabel) 
{
	if (dwLabel >= (DWORD)m_vectorLabel.size())
		return NULL;

	return m_vectorLabel[dwLabel]->GetText();
}

void ui::TabCtl::SetHighLightLabel(DWORD dwLabel)
{
	DWORD dwSize = (DWORD)m_vectorLabel.size();
	if (dwLabel >= dwSize)
		return ;
	m_dwHighLightLabel = dwLabel;
	for(DWORD n = 0; n < dwSize; n++)
	{
		m_vectorLabel[n]->SetHighLight(FALSE);
	}
	m_vectorLabel[dwLabel]->SetHighLight(TRUE);
}

void ui::TabCtl::SetupButton(void)
{
	RECT rcBtn,rcWnd;
	int iWidth,iHeight;
	//left 
	GetWndRect(&rcWnd);

	m_pBtnLeft->GetWndRect(&rcBtn);
	iWidth = rcBtn.right - rcBtn.left;
	iHeight = rcBtn.bottom  - rcBtn.top;

	rcBtn.left = rcWnd.right + FTABCTL_ITEM_OFFSET_RIGHT;
	rcBtn.top  = rcWnd.top   + FTABCTL_ITEM_OFFSET_TOP;
	rcBtn.right = rcBtn.left + iWidth;
	rcBtn.bottom= rcBtn.top  + iHeight;
	m_pBtnLeft->SetWndRect(&rcBtn);

	//right
	m_pBtnRight->GetWndRect(&rcBtn);
	iWidth = rcBtn.right - rcBtn.left;
	iHeight = rcBtn.bottom  - rcBtn.top;

	rcBtn.left = rcWnd.right + FTABCTL_ITEM_OFFSET_RIGHT / 2;
	rcBtn.top  = rcWnd.top   + FTABCTL_ITEM_OFFSET_TOP;
	rcBtn.right = rcBtn.left + iWidth;
	rcBtn.bottom= rcBtn.top  + iHeight;
	m_pBtnRight->SetWndRect(&rcBtn);
}

void ui::TabCtl::CalcDisplayLabel(void)
{
	//Get Wnd Rect 

	int iWidth = 0;int iButtonCount = (int)m_vectorLabel.size();

	CalcLabelDisplayCount();
	//
	if (m_pBtnRight->IsClicked())
	{
		//right scroll
		if (m_iLabelDisplayPos + m_iLabelDisplayCount < iButtonCount  )
		{
			m_iLabelDisplayPos ++;
		}
		CalcLabelDisplayCount();
	}

	if (m_pBtnLeft->IsClicked())
	{
		m_iLabelDisplayPos--;
		if (m_iLabelDisplayPos < 0) m_iLabelDisplayPos =0;
		CalcLabelDisplayCount();
	}

	if (m_iLabelDisplayPos!=0)
	{
		DWORD dwStyle = m_pBtnLeft->GetStyle();
		dwStyle &= ~FWS_DISABLE;
		m_pBtnLeft->SetStyle(dwStyle);
	}
	else
	{
		DWORD dwStyle = m_pBtnLeft->GetStyle();
		dwStyle |= FWS_DISABLE;
		m_pBtnLeft->SetStyle(dwStyle);
	}

	if (m_iLabelDisplayCount < iButtonCount - m_iLabelDisplayPos)
	{
		DWORD dwStyle = m_pBtnRight->GetStyle();
		dwStyle &= ~FWS_DISABLE;
		m_pBtnRight->SetStyle(dwStyle);
	}
	else
	{
		DWORD dwStyle = m_pBtnRight->GetStyle();
		dwStyle |= FWS_DISABLE;
		m_pBtnRight->SetStyle(dwStyle);
	}


}

//__CLASS_(ui::Combo)____
ui::Combo::Combo()
{
	m_bListBoxVisible = FALSE;
	m_bSelChanged = FALSE;
	m_iCurSel = 0;
}

ui::Combo::~Combo()
{
}

void ui::Combo::SetWndRect(RECT *pRect)
{
	ui::Wnd::SetWndRect(pRect);

	int iFontSize = render::Interface::GetInstance()->GetLayer2D()->GetFontSize();

	int iX = pRect->left,iY = pRect->top,iW = pRect->right - pRect->left,iH = pRect->bottom - pRect->top;

	RECT rcEdit = { iX,iY,iX + iW - iFontSize * 2,iY + iH };
	m_pEditBox->SetWndRect(&rcEdit);

	RECT rcButton = { iX + (iW - iFontSize * 2),iY,iX + iW,iY + iH};
	m_pButton->SetWndRect(&rcButton);

	RECT rcListBox = { iX , iY ,iX + iW,iY + iH + 7 * iFontSize};
	m_pListBox->SetWndRect(&rcListBox);
}

BOOL ui::Combo::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	int iFontSize = render::Interface::GetInstance()->GetLayer2D()->GetFontSize();

	int iX = pRect->left,iY = pRect->top,iW = pRect->right - pRect->left,iH = pRect->bottom - pRect->top;

	RECT rcEdit = { 0,0,iW - iFontSize * 2,iH };
	m_pEditBox = new ui::EditBox;
	m_pEditBox->Create("EditBox",&rcEdit,FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE,this);
	m_pEditBox->ShowWnd(TRUE);

    RECT rcButton = { iW - iFontSize * 2,0,iW,iH};
	m_pButton = new ui::Button;
	m_pButton->Create("ˇ",&rcButton,FWS_VISIBLE | FWS_FILLBKG,this);
	m_pButton->ShowWnd(TRUE);

    RECT rcListBox = { 0, iH,iW,iH + 7 * iFontSize};
	if (dwStyle & FW_CB_TOP)
	{
		rcListBox.left   = 0;
		rcListBox.top    = pRect->bottom + 7 * iFontSize;
		rcListBox.right  = iW;
		rcListBox.bottom = 0;
	}
	m_pListBox = new ui::ListBox;
	m_pListBox->Create("ListBox",&rcListBox,FWS_VISIBLE  | FWS_FILLBKG | FWS_OUTLINE | FWS_TOPMOST,this);
	m_pListBox->ShowWnd(FALSE);
	m_pListBox->SetBkColor(FWC_BLACK);

	return TRUE;
}

void ui::Combo::Clear()
{
	m_pListBox->DeleteAllItem();
}

void ui::Combo::InsertItem(const char *szText)
{
	m_pListBox->InsertItem(szText);
}
void ui::Combo::SetEditText(const char *strText)
{
	if(strText && strlen(strText)>=0)
	{
		m_pEditBox->Clear();
		m_pEditBox->InsertText(0,strText,lstrlen(strText));
	}
}
void ui::Combo::Update()
{
	ui::Wnd::UpdateDefault();

	//SELF
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	BOOL  &bActive = m_tWindowInfo.bActive;

	//---SELF	
	if ((dwStyle & FWS_VISIBLE) && !(dwStyle & FWS_DISABLE))
	{
		ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

		RECT &rc = m_tWindowInfo.rcRect;

		if (m_bListBoxVisible)
		{
			if (m_pListBox->IsSelChanged())
			{
				m_bSelChanged = TRUE;
				m_iCurSel = m_pListBox->GetSelItem();
				m_bListBoxVisible = FALSE;
				m_pListBox->ShowWnd(m_bListBoxVisible);
				const char *pText = m_pListBox->GetItemText(m_iCurSel);
				m_pEditBox->Clear();
				m_pEditBox->InsertText(0,pText,lstrlen(pText));
			}
		}

		if (m_pButton->IsClicked())
		{
			m_bListBoxVisible = !m_bListBoxVisible;
			m_pListBox->ShowWnd(m_bListBoxVisible);
		}
	}

	Wnd::UpdateChild();

}

BOOL ui::Combo::IsSelChanged()
{
	BOOL bResult = m_bSelChanged;
	m_bSelChanged = FALSE;
	return bResult;	
}

void ui::Combo::SetSelItem(int iItem)
{
	if (iItem >= 0 && iItem < m_pListBox->GetItemCount())
	{
		m_iCurSel = iItem;
		m_pListBox->SetSelItem(iItem);
		const char *pText = m_pListBox->GetItemText(m_iCurSel);
		m_pEditBox->Clear();
		m_pEditBox->InsertText(0,pText,lstrlen(pText));
	}
}

int ui::Combo::GetSelItem()
{
	return m_pListBox->GetSelItem();
}

int ui::Combo::GetItemCount()
{
	return m_pListBox->GetItemCount();
}


void ui::Combo::Destroy()
{
	ui::Wnd::Destroy();
	SAFEDESTROY(m_pListBox);
	SAFEDESTROY(m_pEditBox);	
	SAFEDESTROY(m_pButton);
}

//__CLASS_(ui::CheckBox)____
ui::CheckBox::CheckBox(void)
{
	m_bChecked = FALSE;
}

ui::CheckBox::~CheckBox(void)
{
}

void ui::CheckBox::Render()
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

		int iTextStartX = iX + iW/2 - pLayer2D->GetFontSize()/2;
		int iTextStartY = iY + iH/2 - pLayer2D->GetFontSize()/2;
		ui::Wnd::SetUIRendState();
		pLayer2D->TextOut(iTextStartX,iTextStartY,m_tWindowInfo.szName,strlen(m_tWindowInfo.szName),m_tWindowInfo.colorText);
		pLayer2D->DrawWireQuadRect(iX,iTextStartY-2,iX+12,iTextStartY+10);
		if(m_bChecked)
		{
			const char *checked = "√";
			ui::Wnd::SetUIRendState();
			pLayer2D->TextOut(iX,iTextStartY,"√",strlen(checked),m_tWindowInfo.colorText);
		}

	}
}

void ui::CheckBox::Update()
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

void ui::CheckBox::SetChecked(BOOL bFlag)
{
	m_bChecked = bFlag;
}



//__CLASS_(ui::Manager)____
IMPLEMENT_SINGLETON(ui::Manager);
ui::Manager::Manager()
{
	m_pActiveWnd = NULL;
	m_pMainWnd = NULL;

	m_stPopupWindowState.m_ePopupWindowState = UPS_NONE;
	m_stPopupWindowState.pTargetWnd = NULL;
	m_stPopupWindowState.pOldActive = NULL;

	m_bEdit = FALSE;
}

void ui::Manager::Update()
{
	//process popup op
	if (m_stPopupWindowState.m_ePopupWindowState != UPS_NONE)
	{
		switch(m_stPopupWindowState.m_ePopupWindowState)
		{
		case UPS_BEGIN:
			{
				m_stPopupWindowState.pOldActive = m_pActiveWnd;
				ActiveWindow(m_stPopupWindowState.pTargetWnd);
				m_stPopupWindowState.m_ePopupWindowState = UPS_RUNNING;
			}
			break;
		case UPS_RUNNING:
			break;
		case UPS_END:
			{
				ActiveWindow(m_stPopupWindowState.pOldActive);
				m_stPopupWindowState.m_ePopupWindowState = UPS_NONE;
			}
			break;
		}
	}

	//缺省的消息处理
	switch(m_stInputInfoBuffer.eType)
	{ 
	case UIMT_MS_BTNUP:
		break;
	case UIMT_MS_BTNDOWN:
		{
			switch(m_stInputInfoBuffer.dwData)
			{
			case MK_LBUTTON:
			case MK_MBUTTON:
			case MK_RBUTTON:
				{
					if (m_stPopupWindowState.m_ePopupWindowState == UPS_NONE)
					{
						Wnd * pWndOver = NULL;
						m_pMainWnd->HitTest(&m_stInputInfoBuffer.ptMouse,&pWndOver);

						if (pWndOver && !(pWndOver->GetStyle() & FWS_DISABLE))
							ActiveWindow(pWndOver);
					}	
					else
					{
						//只允许激活弹出的窗口和其子窗口
						Wnd * pWndOver = NULL;
						m_stPopupWindowState.pTargetWnd->HitTest(&m_stInputInfoBuffer.ptMouse,&pWndOver);
						if (pWndOver)
							ActiveWindow(pWndOver);
					}
				}
				break;
			}
		}
		break;
	case UIMT_KB_KEYDOWN:
		{//打开所有窗口的调试属性
			if (m_stInputInfoBuffer.dwData== DIK_U)
			{
				if (m_stInputInfoBuffer.byKeyBuffer[DIK_LCONTROL] & 0x80
					&& m_stInputInfoBuffer.byKeyBuffer[DIK_LSHIFT] & 0x80
					&& m_stInputInfoBuffer.byKeyBuffer[DIK_LALT] & 0x80)
				{
					if (m_bEdit)
					{
						m_bEdit = FALSE;
						ModifyStyle(m_pMainWnd,0,FWS_DEBUG);

					}
					else
					{
						m_bEdit = TRUE;
						ModifyStyle(m_pMainWnd,FWS_DEBUG,0);
					}
				}
			}
			if (m_bEdit)
			{
				switch(m_stInputInfoBuffer.dwData)
				{
				case DIK_S:
					if (m_stInputInfoBuffer.byKeyBuffer[DIK_LCONTROL] & 0x80)
					{

					}
					break;
				case DIK_R:
					if (m_stInputInfoBuffer.byKeyBuffer[DIK_LCONTROL] & 0x80)
					{
					} 
					break;
				case DIK_UP:
					if (m_pActiveWnd)
						if (m_stInputInfoBuffer.byKeyBuffer[DIK_LSHIFT] & 0x80)
						{
							m_pActiveWnd->MoveWnd(0,-1);
							m_pActiveWnd->SizeWnd(0,1);	
						}
						else
							if (m_stInputInfoBuffer.byKeyBuffer[DIK_LCONTROL] & 0x80)
							{
								m_pActiveWnd->SizeWnd(0,-1);	
							}
							else
								m_pActiveWnd->MoveWnd(0,-1);
					break;
				case DIK_DOWN:
					if (m_pActiveWnd)
						if (m_stInputInfoBuffer.byKeyBuffer[DIK_LSHIFT] & 0x80)
						{
							m_pActiveWnd->SizeWnd(0,1);	
						}
						else
							if (m_stInputInfoBuffer.byKeyBuffer[DIK_LCONTROL] & 0x80)
							{
								m_pActiveWnd->MoveWnd(0,1);
								m_pActiveWnd->SizeWnd(0,-1);	
							}
							else
								m_pActiveWnd->MoveWnd(0,1);			
					break;	  
				case DIK_LEFT:
					if (m_pActiveWnd)
						if (m_stInputInfoBuffer.byKeyBuffer[DIK_LSHIFT] & 0x80)
						{
							m_pActiveWnd->MoveWnd(-1,0);
							m_pActiveWnd->SizeWnd(1,0);	
						}
						else
							if (m_stInputInfoBuffer.byKeyBuffer[DIK_LCONTROL] & 0x80)
							{
								m_pActiveWnd->SizeWnd(-1,0);	
							}
							else
								m_pActiveWnd->MoveWnd(-1,0);
					break;
				case DIK_RIGHT:
					if (m_pActiveWnd)
						if (m_stInputInfoBuffer.byKeyBuffer[DIK_LSHIFT] & 0x80)
						{
							m_pActiveWnd->MoveWnd(0,0);
							m_pActiveWnd->SizeWnd(1,0);	
						}
						else
							if (m_stInputInfoBuffer.byKeyBuffer[DIK_LCONTROL] & 0x80)
							{
								m_pActiveWnd->MoveWnd(1,0);
								m_pActiveWnd->SizeWnd(-1,0);	
							}
							else
								m_pActiveWnd->MoveWnd(1,0);
					break;
				}			
			}
		}
		break;
	default:
		break;
	}

	m_pMainWnd->Update();
}

void ui::Manager::Render()
{
	m_pMainWnd->Render();
}


void ui::Manager::InactiveWindow(ui::Wnd *pWnd)
{
	for(std::list<Wnd *>::iterator it = pWnd->m_listChild.begin(); it != pWnd->m_listChild.end(); it++)
	{
		Wnd *pWnd = *it; 
		InactiveWindow(pWnd);
	}
	pWnd->m_tWindowInfo.bActive = FALSE;
}


void ui::Manager::BeginPopupWnd(ui::Wnd * pWnd)
{
	//set a window pop up//only one!!
	m_stPopupWindowState.m_ePopupWindowState = UPS_BEGIN;
	m_stPopupWindowState.pTargetWnd = pWnd;	
}

void ui::Manager::EndPopupWnd()
{
	m_stPopupWindowState.m_ePopupWindowState = UPS_END;
	m_stPopupWindowState.pTargetWnd = NULL;	
}

void ui::Manager::ActiveWindow(ui::Wnd * pWnd)
{
	InactiveWindow(m_pMainWnd);
	m_pActiveWnd = pWnd;	
	if (pWnd)
	{
		pWnd->m_tWindowInfo.bActive = TRUE;
		Wnd *pChild = pWnd;
		Wnd *pParent = pChild->GetParent();
		Wnd *pTemp = NULL;
		//modify order
		while(pChild)
		{
			pTemp = pParent;

			if (pParent)
			{
				pParent->RemoveChild(pChild);
				pParent->AddChild(pChild);
				pParent = pParent->GetParent();
			}
			pChild = pTemp;
		}
	}
}

BOOL ui::Manager::Create()
{
	//add code here
	return TRUE;
}

void ui::Manager::Destroy(void)
{

}

void ui::Manager::SetMainWnd(ui::Wnd *pWnd)
{
	m_pMainWnd = pWnd;
	if (pWnd)
		ActiveWindow(pWnd);
}

void ui::Manager::ModifyStyle(ui::Wnd * pWnd,DWORD dwAdd,DWORD dwRemove)
{
	pWnd->ModifyStyle(dwAdd,dwRemove);
	for(std::list<Wnd *>::iterator it = pWnd->m_listChild.begin(); it != pWnd->m_listChild.end(); it++)
	{
		Wnd *pWnd = *it; 
		ModifyStyle(pWnd,dwAdd,dwRemove);
	}
}

/// 节点类
ui::TreeView::Node::Node():m_bIsExpanded(false),m_pParent(NULL),m_iNodeCount(0)
{
	m_NodeContent.name = "";
	m_NodeContent.value = "";
}

ui::TreeView::Node::~Node()
{
	for(m_iter = m_ChildNodeList.begin();m_iter!=m_ChildNodeList.end();++m_iter)
	{
		Node *pNode = *m_iter;
		SAFEDELETE(pNode);
	}
	m_ChildNodeList.clear();
}
int ui::TreeView::Node::GetSelfGeneration()
{
	int i = 0;
	Node *pNodeParent = m_pParent;
	while(pNodeParent != NULL)
	{
		i++;
		pNodeParent = pNodeParent->GetParent();
	}
	return i;
}
ui::TreeView::Node* ui::TreeView::Node::AddChildNode(const char * name,const char * value,ui::TreeView::Node *pParent)
{
	ui::TreeView::Node *pChildNode = new ui::TreeView::Node;
	pChildNode->SetParent(pParent);
	pChildNode->SetName(name);
	pChildNode->SetValue(value);
	m_ChildNodeList.push_back(pChildNode);
	m_iNodeCount++;
	return pChildNode;
}
DWORD ui::TreeView::Node::GetChildLength()
{
	DWORD length = 0;
	if(m_bIsExpanded)
	{
		length += (DWORD)m_ChildNodeList.size();

		for(m_iter = m_ChildNodeList.begin();m_iter!=m_ChildNodeList.end();++m_iter)
		{
			ui::TreeView::Node *pNode = *m_iter;
			if(pNode->IsExpanded())		//如果是展开状态就要递归计算下面的长度
				length += pNode->GetChildLength();
		}
	}
	else
	{
		length += 1;
	}
	return length;
}

//////////////////////////////////

//// 树形控件 TreeView

ui::TreeView::TreeView()
{
	m_iDisplayMax = 0;		//显示的最大行数;
	m_pSelNode = NULL;		//当前选中的节点
	m_rcWorkarea = m_tWindowInfo.rcRect;
	m_wChildCount = 0;
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
	int iFontSize = pLayer2D->GetFontSize();
	m_iLineHeight = iFontSize + 2;
	m_pRootNode = NULL;
}

ui::TreeView::~TreeView()
{

}

void ui::TreeView::UpdateWorkArea()
{
	
}
ui::TreeView::Node * ui::TreeView::GetNodeByName(Node *pNode, const char * name)
{
	const char * strName = pNode->GetName();
	if(strcmp(strName,name) == 0)
	{
		return pNode;
	}
	else
	{
		Node *pNextNode = 0;
		if (pNode->GetChildCount() > 0)
		{		
			std::vector<Node*> *vecChildList = pNode->GetChildList();
			std::vector<Node*>::iterator iter = vecChildList->begin();
			for(;iter != vecChildList->end();++iter)
			{
				Node *pChildNode = *iter;
				pNextNode = GetNodeByName(pChildNode,name);
				if (pNextNode && strcmp(pNextNode->GetName(),name) == 0)
				{
					return pNextNode;
				}
			}
		}
	}
	return 0;
}

void ui::TreeView::AppendChildNode(Node *pParentNode, Node* pChildNode)		//在父节点下加一个子节点
{
	pChildNode->SetParent(pParentNode);
	bool bExpanded = pParentNode->IsExpanded();
	if(bExpanded)
	{
		ProcessGather(pParentNode);
		pParentNode->GetChildList()->push_back(pChildNode);
		ProcessExpandedOrGather(pParentNode);
	}
	else					
	{
		pParentNode->GetChildList()->push_back(pChildNode);
		pParentNode->SetExpanded(true);
		ProcessExpandedOrGather(pParentNode);
		ProcessGather(pParentNode);
		pParentNode->SetExpanded(false);
	}
}
void ui::TreeView::DeleteLastNode(Node *pParentNode)							//删除一个子节点
{
	std::vector<ui::TreeView::Node *> *vecChildList = pParentNode->GetChildList();
	ui::TreeView::Node *pChildNode = (*vecChildList)[pParentNode->GetChildCount() - 1];
	bool bExpanded = pParentNode->IsExpanded();
	if(bExpanded)
	{
		ProcessGather(pParentNode);
		vecChildList->erase(--(vecChildList->end()));
		ProcessExpandedOrGather(pParentNode);
		
	}
	else					
	{
		vecChildList->erase(--(vecChildList->end()));
		pParentNode->SetExpanded(true);
		ProcessExpandedOrGather(pParentNode);
		ProcessGather(pParentNode);
		pParentNode->SetExpanded(false);
	}
	SAFEDELETE(pChildNode);
}

void ui::TreeView::Node::SetParent(Node *pNode)
{
	if(m_pParent != pNode)
	{
		m_pParent = pNode;
	}
}

void ui::TreeView::CaculateTreeView()
{
	//计算能显示的行数	
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();

	int iFontSize = pLayer2D->GetFontSize();
	
	m_iDisplayMax = (m_rcWorkarea.bottom - m_rcWorkarea.top) / (iFontSize + ExpandIcon_Y_MARGIN);

	DWORD iItemLength = m_TreeDisplayList.size();	//当前Tree下面需要显示的有多少行节点

	m_pScrollBar->SetRange( iItemLength - m_iDisplayMax);
	if (m_iDisplayMax >= iItemLength) 
	{
		DWORD dwStyle = m_pScrollBar->GetStyle() & ~FWS_VISIBLE;
		m_pScrollBar->SetStyle(dwStyle);
	}
	else
	{
		DWORD dwStyle = m_pScrollBar->GetStyle() | FWS_VISIBLE;
		m_pScrollBar->SetStyle(dwStyle);
	}

	BOOL bSBVisible = m_pScrollBar->GetStyle() & FWS_VISIBLE;

	//滚动区域
	int iPosX = m_tWindowInfo.rcRect.left;
	int iPosY = m_tWindowInfo.rcRect.top;
	int iWidth= m_tWindowInfo.rcRect.right - m_tWindowInfo.rcRect.left;
	int iHeight= m_tWindowInfo.rcRect.bottom - m_tWindowInfo.rcRect.top;

	RECT rcScroll;
	if (bSBVisible)
	{
		m_pScrollBar->GetWndRect(&rcScroll);
		rcScroll.left = iPosX + iWidth - FW_LB_SCROLLBAR_WIDTH - FWA_FRAMESIZE;
		rcScroll.top  = iPosY + FWA_FRAMESIZE;
		rcScroll.right= rcScroll.left + FW_LB_SCROLLBAR_WIDTH;
		rcScroll.bottom= iPosY + iHeight - FWA_FRAMESIZE - FWA_FRAMESIZE;
		m_pScrollBar->SetWndRect(&rcScroll);
	}

	//工作区域
	m_rcWorkarea.left = iPosX + FWA_FRAMESIZE;
	m_rcWorkarea.top  = iPosY + FWA_FRAMESIZE;
	m_rcWorkarea.right= bSBVisible? (rcScroll.left - FWA_FRAMESIZE) : (m_tWindowInfo.rcRect.right - FWA_FRAMESIZE);
	m_rcWorkarea.bottom= m_tWindowInfo.rcRect.bottom - FWA_FRAMESIZE;
	//更新所有节点位置
	int maxWidth = 0;
	for(int i=0;i < m_iDisplayMax; ++i)
	{
		int iItem = i + m_pScrollBar->GetPos();
		if (iItem >= m_TreeDisplayList.size()) break;
		if (iItem < 0) 
			continue;
		
		Node *pNode = m_TreeDisplayList[iItem];
		if (pNode)
		{
			RECT IconRect;
			IconRect.left = m_rcWorkarea.left + pNode->GetSelfGeneration() * ExpandIcon_X_MARGIN;
			IconRect.top  = m_rcWorkarea.top +  i * (iFontSize + ExpandIcon_Y_MARGIN);
			IconRect.right = IconRect.left + ExpandIcon_SIZE;
			IconRect.bottom = IconRect.top + ExpandIcon_SIZE;

			pNode->SetExpandIconRect(&IconRect);

			RECT TextRect;
			TextRect.left = IconRect.right + ICON_TEXT_MARGIN;
			TextRect.top  = IconRect.top;
			const char * name = pNode->GetName();
			const char * value = pNode->GetValue();
			int textWidth = iFontSize/2 * (strlen(name) + strlen(value) + 3);
			if(strcmp(value,"") == 0)
			{
				textWidth = iFontSize/2 * (strlen(name) );
			}

			TextRect.right = TextRect.left + textWidth;;
			TextRect.bottom = TextRect.top + iFontSize;
			pNode->SetContentRect(&TextRect);

			maxWidth = maxWidth>TextRect.right?maxWidth:TextRect.right;
		}
	}
}

void ui::TreeView::Render()
{
	DWORD &dwStyle = m_tWindowInfo.dwStyle;

	if (dwStyle & FWS_VISIBLE)
	{
		ui::Wnd::RenderDefault();

		DrawTree();

		ui::Wnd::RenderChild();
	}
}

void ui::TreeView::DrawTree()
{

	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
	
	if(m_TreeDisplayList.size() == 0)
		return;
	int iFontSize = pLayer2D->GetFontSize();
	static char strText[256] = "";		
	for(int i = 0;i < m_iDisplayMax; ++i)
	{
		int iItem = i + m_pScrollBar->GetPos();
		if (iItem >= m_TreeDisplayList.size()) 
			break;
		if (iItem < 0) 
			continue;
		
		Node *pNode = m_TreeDisplayList[iItem];
		
		if(pNode)
		{
			DrawTreeLine(pNode);
			int iIconX = pNode->GetIconX();
			int iIconY = pNode->GetIconY();
			int ix = pNode->GetContentX();
			int iy = pNode->GetContentY();
			const char * name = pNode->GetName();
			const char * value = pNode->GetValue();
			strcpy(strText,name);
			if(pNode == m_pSelNode)
			{
				RECT rect;
				memcpy(&rect,pNode->GetTextRect(),sizeof(RECT));
				pLayer2D->DrawSolidQuadRect(rect.left,rect.top,rect.right,rect.bottom,0x550000ee);
			}
			if(pNode->IsExpanded())
			{
				if(pNode->GetChildCount() != 0)
					DrawExpand(iIconX, iIconY);
				if(strcmp(value,"")!=0)
				{
					strcat(strText," = ");
					strcat(strText,value);
				}
				ui::Wnd::SetUIRendState();
				pLayer2D->TextOut(ix, iy, strText,strlen(strText),m_tWindowInfo.colorText);
			}
			else
			{
				if(pNode->GetChildCount() != 0)
					DrawGather(iIconX, iIconY);
				ui::Wnd::SetUIRendState();
				pLayer2D->TextOut(ix, iy, strText,strlen(strText),m_tWindowInfo.colorText);
				continue;
			}

		}
	}

}

//bool	ui::TreeView::IsShowing(Node *pNode);		//是否正在显示
//{
//	bool find = false;
//	for (int i = 0; i < (int)m_TreeDisplayList.size(); i++)
//	{
//		if (pNode == m_TreeDisplayList[i])
//		{
//			find = true;
//			break;
//		}
//	}
//	return find;
//}

void ui::TreeView::DrawTreeLine(Node *pNode)
{	
	//render::Interface *pInterface = render::Interface::GetInstance();
	//render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
	//int iFontSize = pLayer2D->GetFontSize();
	//int iStartX = pNode->GetIconX() + ExpandIcon_SIZE / 2;
	//int iStartY = pNode->GetIconY() + ExpandIcon_SIZE;
	//int iEndX =  iStartX;
	//int iEndY = iStartY;
	//if (pNode->IsExpanded() && pNode->GetChildCount() > 0)
	//{
	//	iEndX = iStartX;
	//	iEndY = iStartY + pNode->GetChildLength() * (ExpandIcon_Y_MARGIN + iFontSize);
	//	if (iEndY > m_rcWorkarea.bottom)
	//		iEndY = m_rcWorkarea.bottom;
	//}
	//pLayer2D->DrawLine(iStartX, iStartY, iEndX, iEndY);			//画竖线
	////父节点如果没显示，则补画竖线
	//int i = 0;
	//while(i<pNode->GetSelfGeneration())
	//{
	//	Node *pParentNode = pNode->GetParent();
	//	if (!pNode->IsShowing())
	//	{
	//		iStartX = pNode->GetIconX() + ExpandIcon_SIZE / 2;
	//		iStartY = pNode->GetIconY() + ExpandIcon_SIZE;
	//		iEndX =  iStartX;
	//		iEndY = iStartY;
	//	}
	//	i++;
	//}
	//Node *pParentNode = pNode->GetParent();
	//if (pParentNode)
	//{
	//	if (pNode->GetChildCount()>0)
	//	{
	//		iStartX = pParentNode->GetIconX() + ExpandIcon_SIZE / 2;
	//		iStartY = pNode->GetIconY() + ExpandIcon_SIZE / 2;
	//		iEndX =  pNode->GetIconX();
	//		iEndY = iStartY;
	//	} 
	//	else
	//	{
	//		iStartX = pParentNode->GetIconX() + ExpandIcon_SIZE / 2;
	//		iStartY = pNode->GetIconY() + ExpandIcon_SIZE;
	//		iEndX =  pNode->GetIconX() + ICON_TEXT_MARGIN;
	//		iEndY = iStartY;
	//	}

	//	pLayer2D->DrawLine(iStartX, iStartY, iEndX, iEndY);			//画横线
	//}
	
}
void ui::TreeView::Update()
{
	//Update Default
	ui::Wnd::UpdateDefault();

	//Update Self
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	BOOL  &bActive = m_tWindowInfo.bActive;

	if ((dwStyle & FWS_VISIBLE) && !(dwStyle & FWS_DISABLE))
	{
		ui::Manager::_tagINPUTINFO *pInputInfo = ui::Manager::GetInstance()->GetInputInfoBuffer();

		render::Interface *pInterface = render::Interface::GetInstance();
		render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

		int iFontSize = pLayer2D->GetFontSize();
		
		if (::PtInRect(&m_rcWorkarea,pInputInfo->ptMouse))
		{
			
			int iWAY = m_rcWorkarea.top;
			m_dwHoverIndex = m_pScrollBar->GetPos() + (pInputInfo->ptMouse.y - iWAY) / iFontSize;		
			//选择
			if (pInputInfo->eType == UIMT_MS_BTNUP && pInputInfo->dwData & MK_LBUTTON)
			{
				for(int i=0;i < m_iDisplayMax; ++i)
				{
					int iItem = i + m_pScrollBar->GetPos();
					if (iItem >= m_TreeDisplayList.size()) break;
					if (iItem < 0) 
						continue;

					Node *pNode = m_TreeDisplayList[iItem];

					if(pNode)
					{
						if(::PtInRect(pNode->GetExpandIconRect(),pInputInfo->ptMouse) && pNode->GetChildCount() != 0)
						{
							bool set = pNode->IsExpanded();
							pNode->SetExpanded(!set);
							ProcessExpandedOrGather(pNode);
							return;
						}
						if(::PtInRect(pNode->GetTextRect(),pInputInfo->ptMouse))
						{
							m_pSelNode = pNode;
							return;
						}
					}
				}
				
			}
			if(pInputInfo->eType == UIMT_MS_BTNUP && pInputInfo->dwData & MK_RBUTTON)
			{
				for(int i=0;i < m_iDisplayMax; ++i)
				{
					int iItem = i + m_pScrollBar->GetPos();
					if (iItem >= m_TreeDisplayList.size()) break;
					if (iItem < 0) 
						continue;

					Node *pNode = m_TreeDisplayList[iItem];

					if(pNode)
					{
						if(::PtInRect(pNode->GetTextRect(),pInputInfo->ptMouse))
						{
							m_pSelNode = pNode;
							return;
						}
					}
				}
			}
		}

		ui::Wnd::UpdateChild();
		
		if (pInputInfo->eType == UIMT_MS_WHEEL && bActive)
		{
			short izDelta = ((short)HIWORD(pInputInfo->dwData));
			int iPos = m_pScrollBar->GetPos();
			int iPage = m_pScrollBar->GetPageSize();
			iPos += (izDelta>0) ? -iPage : iPage; 								
			m_pScrollBar->SetPos(iPos);
		}
		CaculateTreeView();
		//get pos
		//m_iScrollPos = m_pScrollBar->GetPos();	
	}	
}

void ui::TreeView::ProcessExpanded(Node *pNode)
{
	for(int i=0;i<pNode->GetChildCount();i++)
	{
		std::vector<Node*> *pVecChildlist = pNode->GetChildList();
		Node *pChildNode = (*pVecChildlist)[i];
		
		for(m_TreeIter = m_TreeDisplayList.begin();m_TreeIter!= m_TreeDisplayList.end();++m_TreeIter)
		{
			if(pNode == *m_TreeIter)
			{
				if(pNode->GetChildCount() != 0)
				{
					std::vector<Node*>::iterator iter = pNode->GetChildList()->begin();
					for(;iter != pNode->GetChildList()->end();++iter)
					{
						Node *ppNode = *iter;
						if(ppNode->GetChildCount() == 0)
						{
							ppNode->SetExpanded(true);
							ProcessExpanded(ppNode);
						}
					}
				}
				break;
			}
		}
		m_TreeList.push_back(pChildNode);
		
		if(pChildNode->IsExpanded())
		{
			ProcessExpanded(pChildNode);
		}
	}
}

void ui::TreeView::ProcessGather(Node *pNode)
{
	for(int i=0;i<pNode->GetChildCount();i++)
	{
		std::vector<Node*> *pVecChildlist = pNode->GetChildList();
		Node *pChildNode = (*pVecChildlist)[i];
		bool find = false;
		for(m_TreeIter = m_TreeDisplayList.begin();m_TreeIter!= m_TreeDisplayList.end();++m_TreeIter)
		{
			if(pChildNode == *m_TreeIter)
			{
				find = true;
				break;
			}
		}
		if (find)
		{
			m_TreeDisplayList.erase(m_TreeIter);
			if(pChildNode->GetChildCount() && pChildNode->IsExpanded())
				ProcessGather(pChildNode);
		}
	}
}

void ui::TreeView::ProcessExpandedOrGather(Node *pNode)
{
	if(pNode->IsExpanded())
	{
		m_TreeList.clear();
		bool first = true;
		bool find  = false;
		ProcessExpanded(pNode);
		//先找到插入点位置
		for(m_TreeIter = m_TreeDisplayList.begin();m_TreeIter!= m_TreeDisplayList.end();++m_TreeIter)
		{
			if(pNode == *m_TreeIter)
			{
				m_TreeIter++;
				find = true;
				break;
			}
		}
		if(!find)
			return;
		//更新显示LIST
		std::vector<Node*>::iterator iter = m_TreeList.begin();
		for (int i = 0;iter != m_TreeList.end();++iter,i++)
		{
			if(!first)
			{
				//因为插入后指针改变，需要重新定位插入位置
				for(m_TreeIter = m_TreeDisplayList.begin();m_TreeIter!= m_TreeDisplayList.end();++m_TreeIter)
				{
					if(pNode == *m_TreeIter)
					{
						m_TreeIter++;
						break;
					}
				}
			}
			m_TreeDisplayList.insert(m_TreeIter + i, *iter);
			first = false;
		}
	}
	else
	{
		ProcessGather(pNode);
	}
}

BOOL ui::TreeView::Create(const char *pszName, const RECT *pRect, DWORD dwStyle, ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);
	RECT rcSB = {0,0,0,0};

	m_pScrollBar = new ScrollBar;	
	m_pScrollBar->Create("FW_SB_VERT",&rcSB,FWS_VISIBLE | FWS_SBS_VERTICAL,this);
	m_pScrollBar->m_tWindowInfo.bActive = TRUE;
	CaculateTreeView();

	return TRUE;
}
void ui::TreeView::Destroy()
{
	SAFEDESTROY(m_pScrollBar);
	SAFEDELETE(m_pRootNode);
	m_TreeDisplayList.clear();
	ui::Wnd::Destroy();
}
void ui::TreeView::MoveWnd(int iOX,int iOY)
{
	ui::Wnd::MoveWnd(iOX,iOY);
	CaculateTreeView();
}
ui::TreeView::Node* ui::TreeView::AddChildNode(const char * name,const char * value, Node *pParent)
{
	ui::TreeView::Node * pNewNode = pParent->AddChildNode(name,value,pParent);

	return pNewNode;
}
void ui::TreeView::AddNode(utility::Text::Node *pFileNode, ui::TreeView::Node *pTreeParentNode)
{
	int iChildCount = pFileNode->GetChildCount();
	int iVarCount = pFileNode->GetVarCount();
	if(iVarCount != 0)
	{
		for(int i = 0;i<iVarCount;i++)
		{
			const char * name = pFileNode->GetVar(i)->GetName();
			const char * value = pFileNode->GetVar(i)->GetStringValue();
			AddChildNode(name,value,pTreeParentNode);
		}
	}
	if(iChildCount == 0)
		return;
	for(int i=0; i < iChildCount; i++)
	{	
		const char * name = pFileNode->GetChildNode(i)->GetName();
		ui::TreeView::Node* pTreeNextParent = AddChildNode(name,"",pTreeParentNode);

		utility::Text::Node *pNextFileNode = pFileNode->GetChildNode(i);

		AddNode(pNextFileNode,pTreeNextParent);	//递归添加下个结点
	}
}
void ui::TreeView::DrawGather(int x,int y)	//绘制聚拢的图形
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
	pLayer2D->DrawWireQuadRect(x, y, x + ExpandIcon_SIZE, y + ExpandIcon_SIZE);	//画方框
	pLayer2D->DrawLine(x, y + 4, x + ExpandIcon_SIZE, y + 4);			//画横线
	pLayer2D->DrawLine(x + 4, y, x + 4, y + ExpandIcon_SIZE);			//画竖线
}
void ui::TreeView::DrawExpand(int x,int y)	//绘制展开的图形
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D *pLayer2D = pInterface->GetLayer2D();
	pLayer2D->DrawWireQuadRect(x, y, x + ExpandIcon_SIZE, y + ExpandIcon_SIZE);	//画方框
	pLayer2D->DrawLine(x, y + 4, x + ExpandIcon_SIZE, y + 4);			//画竖线
}
void ui::TreeView::SetRootNode(ui::TreeView::Node *pNode)
{
	m_TreeDisplayList.clear();
	SAFEDELETE(m_pRootNode)
	m_pRootNode = pNode;
	m_TreeDisplayList.push_back(m_pRootNode);
}
void ui::TreeView::ExportToScript(string *strBuffer, ui::TreeView::Node *pNode)
{
	string strMargin = "";
	if (strcmp(pNode->GetName(),"main")!=0)
	{			
		if (strcmp(pNode->GetName(),"新增加的Group") == 0)
		{
			int A = 0;
		}
		*strBuffer += GetNodeLeftMargin(&strMargin, pNode);
		*strBuffer += pNode->GetName();
		*strBuffer += "\n";
		strMargin = "";
		*strBuffer += GetNodeLeftMargin(&strMargin, pNode);
		*strBuffer += "{\n";
	}
	for(int i=0;i<(int)pNode->GetChildCount();i++)
	{
		std::vector<ui::TreeView::Node*> *pVecChildlist = pNode->GetChildList();
		ui::TreeView::Node *pChildNode = (*pVecChildlist)[i];
		const char * name = pChildNode->GetName();
		const char * value = pChildNode->GetValue();
		//if (pChildNode->GetChildCount() == 0 && strcmp(value,"") == 0)
		//{
		//	strMargin = "";
		//	*strBuffer += GetNodeLeftMargin(&strMargin,pChildNode);
		//	*strBuffer += name;
		//	*strBuffer += "\n";
		//}
		if (strcmp(value,"") != 0)
		{	
			strMargin = "";
			*strBuffer += GetNodeLeftMargin(&strMargin,pChildNode);
			*strBuffer += name;	
			*strBuffer += " = ";
			*strBuffer += value;
			*strBuffer += ";";
			*strBuffer += "\n";
		}
		if (pChildNode->GetChildCount()>0)
		{
			ExportToScript(strBuffer,pChildNode);
		}
		
	}
	if (strcmp(pNode->GetName(),"main")!=0)
	{
		strMargin = "";
		*strBuffer += GetNodeLeftMargin(&strMargin, pNode);
		*strBuffer += "}\n";
	}
}
const char * ui::TreeView::GetNodeLeftMargin(string *strMargin, ui::TreeView::Node *pNode)
{
	int i = pNode->GetSelfGeneration() - 1;
	while (i>0)
	{
		(*strMargin) += "	";
		i--;
	}
	return (*strMargin).c_str();
}