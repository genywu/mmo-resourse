#pragma once
#include "..\..\engine\ui.h"

class WndCameraPath:
	public ui::Wnd
{
public:
	WndCameraPath(void);
	~WndCameraPath(void);

private:
public:
	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent);
	void Destroy();

	void ShowWnd(BOOL bShow);
};