#pragma once

#include "..\..\engine\ui.h"

class AboutWnd :
	public ui::Wnd
{
	ui::Button * m_pBtnOK;

	ui::ListBox * m_pListBox;

public:
	AboutWnd(void);
	~AboutWnd(void);

	virtual void Update();
	virtual void Render();

	virtual BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	virtual void Destroy();
	void Popup(void);
};
