#pragma once
#include "..\..\frostengine\ui.h"

class WndGridMovingSpeedModifyDlg : public ui::Wnd
{
	ui::Static  * m_pStaticSpeed;
	ui::EditBox * m_pEditSpeed;

public:
	WndGridMovingSpeedModifyDlg();
	virtual ~WndGridMovingSpeedModifyDlg();

	BOOL Create(const char * pszName, const RECT * pRect, DWORD dwStyle, Wnd * pParent);
	void Destroy();

	virtual void Update();
	virtual void Render();
};