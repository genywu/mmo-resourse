#pragma once

#include "..\..\frostengine\ui.h"

class WndAbout : public ui::Wnd
{
	ui::Button	*m_pBtnOK;
	ui::ListBox	*m_pListUpdateInfo;
	string		m_strEditionTile;
public:
	WndAbout(void);
	~WndAbout(void);

	virtual void Update();
	virtual void Render();

	virtual BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	virtual void Destroy();
	void BeginPopUp(void);
	void EndPopUp(void);
};