#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\frostengine\ui.h"

#include "wndhotkey.h"

WndHotkey::WndHotkey(void)
{
}

WndHotkey::~WndHotkey(void)
{
}

BOOL WndHotkey::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent)
{
	ui::Wnd::Create(pszName,pRect,dwStyle,pParent);

	RECT rc = {0,0,80,20};
	m_pBtnOK = new ui::Button;
	VERIFY(m_pBtnOK->Create("确定",&rc,FWS_VISIBLE,this));
	m_pBtnOK->CenterWndTo((m_tWindowInfo.rcRect.right - m_tWindowInfo.rcRect.left)/2,pRect->bottom - 24);	

	return TRUE;
}

void WndHotkey::Destroy()
{
	SAFEDESTROY(m_pBtnOK);

	ui::Wnd::Destroy();
}

void WndHotkey::Update()
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

void WndHotkey::Render()
{
	render::Interface		   * pInterface	= render::Interface::GetInstance();
	render::Interface::Layer2D * pLayer2D   = pInterface->GetLayer2D();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;

	if (dwStyle & FWS_VISIBLE)
	{
		RECT &rc = m_tWindowInfo.rcRect;
		int iX	 = rc.left, iY = rc.top, iW = rc.right - rc.left + 1, iH = rc.bottom - rc.top + 1;

		static const char * szInfo[15] ={"热键列表",
										 "打开角色模型 Ctrl+O    是否开启灯光       - L",
										 "打开绑定模型 Ctrl+B    是否显示灯光模型   - I",
										 "打开参考模型 Ctrl+R    编辑灯光颜色       - E",
										 "退出         Alt+X     编辑背景颜色       - K",
										 "                       切换背景图片和颜色 - S",
										 "",
										 "显示网格     - G       播放动画       - SPACE",
										 "显示线框     - W       播放下一个动作     - A",
										 "显示法线     - N       是否循环动画       - P",
										 "显示骨骼     - J       是否显示摄像机信息 - C",
										 "显示模型尾迹 - T       摄像机复位         - R",
										 "                       摄像机取景器       - V",
										 "",
										 "重新载入模型 - F5",
										 };

		// 绘制背景
		pLayer2D->DrawSolidQuad(iX + 4, iY + 4, iW, iH, m_tWindowInfo.colorLow);
		pLayer2D->DrawSolidQuad(iX, iY, iW, iH, m_tWindowInfo.colorBack);
		pLayer2D->DrawWireQuad(iX, iY, iW, iH, m_tWindowInfo.colorHigh);

		// 绘制内容
		DWORD dwTextWidth = pLayer2D->CalcTextWidth(szInfo[0], lstrlen(szInfo[0]));
		int iStartX = iX + iW / 2 - dwTextWidth / 2;
		pLayer2D->DrawSolidQuad(iStartX - 3, iY + 12, dwTextWidth + 5, pLayer2D->GetFontSize() + 8, 0xf0001020);
		pLayer2D->TextOut(iStartX, iY + 15, szInfo[0], lstrlen(szInfo[0]), m_tWindowInfo.colorText);

		// 绘制版本历史
		int iPosX = iX + 12;
		int iPosY = iY + 30;
		for (int i = 1; i < 15; ++i)
			pLayer2D->TextOut(iPosX, iPosY += (pLayer2D->GetFontSize() + 3), szInfo[i], lstrlen(szInfo[i]), m_tWindowInfo.colorText);
	}

	ui::Wnd::RenderChild();
}
void WndHotkey::BeginPopUp(void)
{
	ModifyStyle(FWS_VISIBLE,0);
	ui::Manager::GetInstance()->BeginPopupWnd(this);
}
void WndHotkey::EndPopUp(void)
{
	ModifyStyle(0,FWS_VISIBLE);
	ui::Manager::GetInstance()->EndPopupWnd();
}