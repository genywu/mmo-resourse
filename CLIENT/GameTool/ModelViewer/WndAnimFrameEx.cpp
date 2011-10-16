#include "stdafx.h"

#include "..\..\frostengine\utility.h"
#include "..\..\frostengine\console.h"
#include "..\..\frostengine\render.h"
#include "..\..\frostengine\ui.h"

#include "WndAnimFrameEx.h"

WndAnimFrameEx::WndAnimFrameEx(void)
{
	m_lExportWidth = m_lExportHeight = 0;
	ZeroMemory(m_szAnimFrameSaveDir, _MAX_PATH);
}



WndAnimFrameEx::~WndAnimFrameEx(void)
{
}



BOOL WndAnimFrameEx::Create(const char * pszName, const RECT * pRect, DWORD dwStyle, ui::Wnd * pParent)
{
	ui::Wnd::Create(pszName, pRect, dwStyle, pParent);

	RECT rcLabel1 = { 10, 10, 10 + 280, 10 + 20 };
	m_pLabel1 = new ui::Static;
	m_pLabel1->Create("动画序列帧导出", &rcLabel1, FWS_VISIBLE | FWS_SHADOW | FWS_FILLBKG | FWS_DISABLE, this);

	RECT rcLabel2 = { 10, 35, 10 + 216, 35 + 20 };
	m_pLabel2 = new ui::Static;
	m_pLabel2->Create("请在左方\"角色模型列表\"中选择一个模型", &rcLabel2, FWS_VISIBLE | FWS_DISABLE, this);

	RECT rcLabel3 = { 10, 35 + 16, 10 + 232, 35 + 20 + 16 };
	m_pLabel3 = new ui::Static;
	m_pLabel3->Create("然后在右边\"模型动作列表\"中选择一个动作", &rcLabel3, FWS_VISIBLE | FWS_DISABLE, this);

	RECT rcFPS = { 50, 100, 50 + 56, 100 + 70 };
	m_pFPS = new ui::ListBox;
	m_pFPS->Create("FPSListBox", &rcFPS, FWS_VISIBLE | FWS_LBS_CHECK, this);
	m_pFPS->InsertItem("8");		//125ms
	m_pFPS->InsertItem("10");		//100ms
	m_pFPS->InsertItem("20");		//50ms
	m_pFPS->InsertItem("25");		//40ms
	m_pFPS->InsertItem("40");		//25ms
	m_pFPS->SetItemChecked(0, true);
	m_pFPS->SetSelItem(0);

	RECT rcFileFormat = { 180, 100, 180 + 56, 100 + 60 };
	m_pFileFormat = new ui::ListBox;
	m_pFileFormat->Create("FPSListBox", &rcFileFormat, FWS_VISIBLE | FWS_LBS_CHECK, this);
	m_pFileFormat->InsertItem("BMP");
	m_pFileFormat->InsertItem("JPG");
	m_pFileFormat->InsertItem("TGA");
	m_pFileFormat->InsertItem("PNG");
	m_pFileFormat->SetItemChecked(3, true);
	m_pFileFormat->SetSelItem(3);

	RECT rcStart = {0, 0, 80, 20};
	m_pBtnStart = new ui::Button;
	VERIFY(m_pBtnStart->Create("开始", &rcStart, FWS_VISIBLE, this));
	m_pBtnStart->CenterWndTo(100, pRect->bottom - 24);

	m_pBtnReturn = new ui::Button;
	VERIFY(m_pBtnReturn->Create("返回", &rcStart, FWS_VISIBLE, this));
	m_pBtnReturn->CenterWndTo(200, pRect->bottom - 24);

	RECT rcSetDir = {0, 0, 40, 20};
	m_pBtnSetDir = new ui::Button;
	VERIFY(m_pBtnSetDir->Create("目录", &rcSetDir, FWS_VISIBLE, this));
	m_pBtnSetDir->MoveWnd(243, 210);

	RECT rcWidth = {0, 0, 50, 19};
	m_pExportWidth = new ui::EditBox;
	VERIFY(m_pExportWidth->Create("宽度", &rcWidth, FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW, this));
	m_pExportWidth->CenterWndTo(110, 190);
	m_pExportWidth->InsertText(0, "800", 3);

	m_pExportHeight = new ui::EditBox;
	VERIFY(m_pExportHeight->Create("高度", &rcWidth, FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW, this));
	m_pExportHeight->CenterWndTo(190, 190);
	m_pExportHeight->InsertText(0, "600", 3);

	RECT rcDir = {0, 0, 220, 19};
	m_pBoxDir = new ui::EditBox;
	VERIFY(m_pBoxDir->Create("显示的目录", &rcDir, FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW, this));
	m_pBoxDir->CenterWndTo(125, 220);
	m_pBoxDir->InsertText(0, "...", 3);	

	return TRUE;
}



void WndAnimFrameEx::Destroy()
{
	ZeroMemory(m_szAnimFrameSaveDir, _MAX_PATH);

	SAFEDESTROY(m_pLabel1);
	SAFEDESTROY(m_pLabel2);
	SAFEDESTROY(m_pLabel3);
	SAFEDESTROY(m_pFPS);
	SAFEDESTROY(m_pFileFormat);
	SAFEDESTROY(m_pBtnStart);
	SAFEDESTROY(m_pBtnReturn);
	SAFEDESTROY(m_pBtnSetDir);
	SAFEDESTROY(m_pExportWidth);
	SAFEDESTROY(m_pExportHeight);
	SAFEDESTROY(m_pBoxDir);

	ui::Wnd::Destroy();
}



void WndAnimFrameEx::Update()
{
	ui::Wnd::UpdateDefault();

	if (m_pFPS->IsSelChanged())
	{
		int	iSelItem   = m_pFPS->GetSelItem();

		for (int i = 0; i < m_pFPS->GetItemCount(); ++i)
			m_pFPS->SetItemChecked(i, false);

		m_pFPS->SetItemChecked(iSelItem, true);
	}

	if (m_pFileFormat->IsSelChanged())
	{
		int	iSelItem   = m_pFileFormat->GetSelItem();

		for (int i = 0; i < m_pFileFormat->GetItemCount(); ++i)
			m_pFileFormat->SetItemChecked(i, false);

		m_pFileFormat->SetItemChecked(iSelItem, true);
	}

	if (m_pBtnSetDir->IsClicked())
	{
		BROWSEINFO bi;
		char display[MAX_PATH] = "SelectName";

		ITEMIDLIST * pidl;
		bi.hwndOwner	  = NULL;
		bi.pidlRoot		  = 0;
		bi.pszDisplayName = display;
		bi.lpszTitle	  = "选择动画帧的保存目录";
		bi.ulFlags		  = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_USENEWUI;
		bi.lpfn			  = 0;
		bi.lParam		  = 0;
		bi.iImage		  = 0;

		pidl = SHBrowseForFolder(&bi);

		if (!pidl)
			return;

		SHGetPathFromIDList(pidl, m_szAnimFrameSaveDir);
		m_pBoxDir->Clear();
		m_pBoxDir->InsertText(0, m_szAnimFrameSaveDir, (int)strlen(m_szAnimFrameSaveDir));
	}

	if (m_pBtnReturn->IsClicked())
	{
		// 关闭自己
		ModifyStyle(0, FWS_VISIBLE);
		ui::Manager::GetInstance()->EndPopupWnd();
	}

	ui::Wnd::UpdateChild();
}



void WndAnimFrameEx::Render()
{
	render::Interface		   * pInterface	= render::Interface::GetInstance();
	render::Interface::Layer2D * pLayer2D   = pInterface->GetLayer2D();

	DWORD &dwStyle = m_tWindowInfo.dwStyle;

	if (dwStyle & FWS_VISIBLE)
	{
		RECT &rcWnd = m_tWindowInfo.rcRect;
		int iX	 = rcWnd.left, iY = rcWnd.top, iW = rcWnd.right - rcWnd.left + 1, iH = rcWnd.bottom - rcWnd.top + 1;

		// 绘制背景
		pLayer2D->DrawSolidQuad(iX + 4, iY + 4, iW, iH, m_tWindowInfo.colorLow);
		pLayer2D->DrawSolidQuad(iX, iY, iW, iH, m_tWindowInfo.colorBack);
		pLayer2D->DrawWireQuad(iX, iY, iW, iH, m_tWindowInfo.colorHigh);

		RECT rc;
		m_pFPS->GetWndRect(&rc);
		iX = rc.left;
		iY = rc.top - 12;

		pLayer2D->DrawWireQuadRect(rc.left, rc.top, rc.right, rc.bottom, FWC_HIGH);
		ui::Wnd::SetUIRendState();
		static char * szpFPS = "选择帧率";
		pLayer2D->TextOut(iX + 1 + 4, iY + 1 + 2, szpFPS, 12, FWC_BLACK);
		pLayer2D->TextOut(iX - 1+ 4, iY - 1+ 2, szpFPS, 12, FWC_BLACK);
		pLayer2D->TextOut(iX + 1+ 4, iY - 1+ 2, szpFPS, 12, FWC_BLACK);
		pLayer2D->TextOut(iX - 1+ 4, iY + 1+ 2, szpFPS, 12, FWC_BLACK);
		pLayer2D->TextOut(iX + 1+ 4, iY+ 2, szpFPS, 12, FWC_BLACK);
		pLayer2D->TextOut(iX - 1+ 4, iY+ 2, szpFPS, 12, FWC_BLACK);
		pLayer2D->TextOut(iX+ 4, iY + 1+ 2, szpFPS, 12, FWC_BLACK);
		pLayer2D->TextOut(iX+ 4, iY - 1+ 2, szpFPS, 12, FWC_BLACK);
		pLayer2D->TextOut(iX+ 4, iY+ 2, szpFPS, 12, FWC_WHITE);

		m_pFileFormat->GetWndRect(&rc);
		iX = rc.left;
		iY = rc.top - 12;

		pLayer2D->DrawWireQuadRect(rc.left, rc.top, rc.right, rc.bottom, FWC_HIGH);
		ui::Wnd::SetUIRendState();
		static char * szpFileFormat = "导出格式";
		pLayer2D->TextOut(iX + 1 + 4, iY + 1 + 2, szpFileFormat, 12, FWC_BLACK);
		pLayer2D->TextOut(iX - 1+ 4, iY - 1+ 2, szpFileFormat, 12, FWC_BLACK);
		pLayer2D->TextOut(iX + 1+ 4, iY - 1+ 2, szpFileFormat, 12, FWC_BLACK);
		pLayer2D->TextOut(iX - 1+ 4, iY + 1+ 2, szpFileFormat, 12, FWC_BLACK);
		pLayer2D->TextOut(iX + 1+ 4, iY+ 2, szpFileFormat, 12, FWC_BLACK);
		pLayer2D->TextOut(iX - 1+ 4, iY+ 2, szpFileFormat, 12, FWC_BLACK);
		pLayer2D->TextOut(iX+ 4, iY + 1+ 2, szpFileFormat, 12, FWC_BLACK);
		pLayer2D->TextOut(iX+ 4, iY - 1+ 2, szpFileFormat, 12, FWC_BLACK);
		pLayer2D->TextOut(iX+ 4, iY+ 2, szpFileFormat, 12, FWC_WHITE);
	}

	ui::Wnd::RenderChild();
}



void WndAnimFrameEx::BeginPopUp(void)
{
	ModifyStyle(FWS_VISIBLE, 0);
	ui::Manager::GetInstance()->BeginPopupWnd(this);
}



void WndAnimFrameEx::EndPopUp(void)
{
	ModifyStyle(0, FWS_VISIBLE);
	ui::Manager::GetInstance()->EndPopupWnd();
}



// 检查输入的尺寸
bool WndAnimFrameEx::CheckInputSize()
{
	static char szTmp[256];

	m_pExportWidth->GetEditText(szTmp, 256);
	m_lExportWidth = atoi(szTmp);
	m_pExportHeight->GetEditText(szTmp, 256);
	m_lExportHeight = atoi(szTmp);

	if (m_lExportWidth > render::Interface::GetInstance()->GetWndWidth())
		return false;

	if (m_lExportHeight > render::Interface::GetInstance()->GetWndHeight())
		return false;

	return true;
}



// 得到动画帧时间间隔
DWORD WndAnimFrameEx::GetFPS()
{
	DWORD dw = 0;

	switch (m_pFPS->GetSelItem())
	{
	case 0:
		dw = 125;
		break;
	case 1:
		dw = 100;
		break;
	case 2:
		dw = 50;
		break;
	case 3:
		dw = 40;
		break;
	case 4:
		dw = 25;
		break;
	}

	return dw;
}



//得到导出格式
char * WndAnimFrameEx::GetFileFormat()
{
	static char * szpFileFormat[] = {".bmp",
									 ".jpg",
									 ".tga",
									 ".png"};

	return szpFileFormat[m_pFileFormat->GetSelItem()];
}