#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\frostengine\ui.h"

#include "WndGuage.h"

WndGuage::WndGuage(void)
{
	m_fGuagePer = 0.0f;
}

WndGuage::~WndGuage(void)
{
}

BOOL WndGuage::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	RECT rc		 = { 0, 0, 80, 20 };
	m_pBtnCancel = new ui::Button;
	VERIFY(m_pBtnCancel->Create("取消", &rc, FWS_VISIBLE, this));
	m_pBtnCancel->CenterWndTo((m_tWindowInfo.rcRect.right - m_tWindowInfo.rcRect.left) / 2, pRect->bottom - 24);

	return TRUE;
}

void WndGuage::Destroy()
{
	SAFEDESTROY(m_pBtnCancel);

	ui::Wnd::Destroy();
}

void WndGuage::Update()
{
	ui::Wnd::UpdateDefault();
	
	if (m_pBtnCancel->IsClicked())
	{
		// 关闭自己
		ModifyStyle(0,FWS_VISIBLE);
		ui::Manager::GetInstance()->EndPopupWnd();
	}

	ui::Wnd::UpdateChild();
}

void WndGuage::Render()
{
	render::Interface		   * pInterface	= render::Interface::GetInstance();
	render::Interface::Layer2D * pLayer2D   = pInterface->GetLayer2D();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;

	if (dwStyle & FWS_VISIBLE)
	{
		RECT &rc = m_tWindowInfo.rcRect;
		int iX	 = rc.left, iY = rc.top, iW = rc.right - rc.left + 1, iH = rc.bottom - rc.top + 1;

		// 绘制背景
		pLayer2D->DrawSolidQuad(iX + 4, iY + 4, iW, iH, m_tWindowInfo.colorLow);
		pLayer2D->DrawSolidQuad(iX, iY, iW, iH, m_tWindowInfo.colorBack);
		pLayer2D->DrawWireQuad(iX, iY, iW, iH, m_tWindowInfo.colorHigh);

		pLayer2D->DrawSolidQuad(iX + 20, iY + 30, (int)(m_fGuagePer * 260.0f), 16, 0x70ffffff);
		pLayer2D->DrawWireQuad(iX + 18, iY + 28, 264, 18, 0xffeeeeee);
	}

	ui::Wnd::RenderChild();
}
void WndGuage::BeginPopUp(void)
{
	ModifyStyle(FWS_VISIBLE,0);
	ui::Manager::GetInstance()->BeginPopupWnd(this);
}
void WndGuage::EndPopUp(void)
{
	ModifyStyle(0,FWS_VISIBLE);
	ui::Manager::GetInstance()->EndPopupWnd();
}