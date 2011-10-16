#include "stdafx.h"

#include "..\..\frostengine\render.h"
#include "..\..\frostengine\ui.h"

#include "titlewnd.h"

TitleWnd::TitleWnd(void)
{
}

TitleWnd::~TitleWnd(void)
{
}

void TitleWnd::Render()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

    RECT &rc = m_tWindowInfo.rcRect;
	int iX = rc.left,iY = rc.top,iW = rc.right - rc.left + 1,iH = rc.bottom - rc.top + 1;

	pLayer2D->DrawSolidQuad(iX+4,iY+4,iW,iH,m_tWindowInfo.colorLow);
	pLayer2D->DrawSolidQuad(iX,iY,iW,iH,m_tWindowInfo.colorBack);

	pLayer2D->DrawWireQuad(iX,iY,iW,iH,m_tWindowInfo.colorHigh);

	int iStartX = iX + iW/2 - pLayer2D->CalcTextWidth(m_tWindowInfo.szName,lstrlen(m_tWindowInfo.szName))/2;
	int iStartY = iY + 2;

	ui::Wnd::SetUIRendState();
	pLayer2D->TextOut(iStartX,iStartY,m_tWindowInfo.szName,lstrlen(m_tWindowInfo.szName),m_tWindowInfo.colorText);			
}