#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\frostengine\ui.h"

#include <new.h>

#include "aboutwnd.h"

AboutWnd::AboutWnd(void)
{
	m_pListBox = NULL;
	m_pBtnOK = NULL;
}

AboutWnd::~AboutWnd(void)
{
}

BOOL AboutWnd::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	RECT rc = {338,572,439,593};
	m_pBtnOK = new ui::Button;
	VERIFY(m_pBtnOK->Create("确定",&rc,FWS_VISIBLE,this));

	RECT rcListBox = {4,20,765,567};
	m_pListBox = new ui::ListBox;
	VERIFY(m_pListBox->Create("帮助列表",&rcListBox,FWS_VISIBLE | FWS_OUTLINE | FWS_FILLBKG,this));
	m_pListBox->ShowWnd(TRUE);

	//读取帮助
	static char szTemp[1024];
	utility::File file;
	if (file.Open("ui/readme.txt"))
	{
		//填充		
		char *p = file.ReadLine(szTemp,1023);
		while(file.GetPosition() < file.GetLength() )
		{
			m_pListBox->InsertItem(p);
			p = file.ReadLine(szTemp,1023);
		}
		file.Close();
	}

	return TRUE;
}

void AboutWnd::Destroy()
{
	SAFEDESTROY(m_pBtnOK);
	SAFEDESTROY(m_pListBox);

	ui::Wnd::Destroy();
}

void AboutWnd::Update()
{
	ui::Wnd::UpdateDefault();
	
	if (m_pBtnOK->IsClicked())
	{
		//关闭自己
		ModifyStyle(0,FWS_VISIBLE);
		ui::Manager::GetInstance()->EndPopupWnd();
	}

	ui::Wnd::UpdateChild();
}

void AboutWnd::Render()
{
	render::Interface *pInterface = render::Interface::GetInstance();
	render::Interface::Layer2D   *pLayer2D   = pInterface->GetLayer2D();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
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

	ui::Wnd::RenderChild();
}
void AboutWnd::Popup(void)
{
	ModifyStyle(FWS_VISIBLE,0);
	ui::Manager::GetInstance()->BeginPopupWnd(this);
}
