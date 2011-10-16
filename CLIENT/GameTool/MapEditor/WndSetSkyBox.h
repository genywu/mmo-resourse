#pragma once
#include "..\..\frostengine\ui.h"

class WndSetSkyBox :
	public ui::Wnd
{
	ui::Static* m_pSSetSkyBoxID;
	ui::EditBox* m_pEdBoxSkyBoxID;
	ui::Static* m_pSSetSkyBoxScal;
	ui::EditBox* m_pEdBoxSkyBoxScal;
	ui::Button* m_pBOk;
	BOOL m_bIsOK;
	BOOL m_bIsPopupCheckFlag;
public:
	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent);
	void Destroy();
	void Update();
	void Render();
	void BeginPopUp();
	void EndPopUp(void);
	BOOL IsOK() { return m_bIsOK;}
	BOOL CheckPopUp() { return m_bIsPopupCheckFlag;}
	WndSetSkyBox(void);
	~WndSetSkyBox(void);
};
