#include "StdAfx.h"

#include "GridMovingSpeedModifyDlg.h"
#include "wndviewer.h"

WndGridMovingSpeedModifyDlg::WndGridMovingSpeedModifyDlg(void)
{
}

WndGridMovingSpeedModifyDlg::~WndGridMovingSpeedModifyDlg(void)
{
}

BOOL WndGridMovingSpeedModifyDlg::Create(const char * pszName, const RECT * pRect, DWORD dwStyle, Wnd * pParent)
{
	ui::Wnd::Create(pszName, pRect, dwStyle, pParent);

	RECT rcTextTip = { 2, 3, 41, 25 };
	m_pStaticSpeed = new ui::Static;
	m_pStaticSpeed->Create("地表移动速度(Grid/MS)", &rcTextTip, FWS_VISIBLE, this);

    RECT rcSpeed = { 85, 4, 135, 23 };
	m_pEditSpeed = new ui::EditBox;
	m_pEditSpeed->Create("TimeLength", &rcSpeed, FWS_VISIBLE | FWS_FILLBKG | FWS_OUTLINE | FWS_SHADOW | FWS_DRAG | FWS_SIZE, this);
	m_pEditSpeed->SetEditText("0.0016");

	return TRUE;
}
void WndGridMovingSpeedModifyDlg::Destroy()
{
	SAFEDESTROY(m_pStaticSpeed);
    SAFEDESTROY(m_pEditSpeed);

	ui::Wnd::Destroy();
}

void WndGridMovingSpeedModifyDlg::Update()
{
	//UPDATE DEFAULT
	ui::Wnd::UpdateDefault();

	DWORD & dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE && !(dwStyle & FWS_DISABLE))
	{
		if (m_pEditSpeed->CheckEnter())
		{
			char szTemp[256];
			m_pEditSpeed->GetEditText(szTemp, 256);
			float fSpeed = (float)atof(szTemp);
			if (fSpeed <= 0.0f)
			{
				fSpeed = 0.0f;
				m_pEditSpeed->SetEditText("0.0");
			}

			((WndViewer *)(this->GetParent()))->SetGridMovingSpeed(fSpeed);
		}                     

		ui::Wnd::UpdateChild();
	}
}

void WndGridMovingSpeedModifyDlg::Render()
{
	DWORD &dwStyle = m_tWindowInfo.dwStyle;
	if (dwStyle & FWS_VISIBLE)
	{
		ui::Wnd::RenderDefault();

		ui::Wnd::RenderChild();
	}
}