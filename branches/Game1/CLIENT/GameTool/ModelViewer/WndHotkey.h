#pragma once

#include "..\..\frostengine\ui.h"

class WndHotkey : public ui::Wnd
{
	ui::Button * m_pBtnOK;

public:
	WndHotkey(void);
	~WndHotkey(void);

	virtual void Update();
	virtual void Render();

	virtual BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	virtual void Destroy();
	void BeginPopUp(void);
	void EndPopUp(void);
};
