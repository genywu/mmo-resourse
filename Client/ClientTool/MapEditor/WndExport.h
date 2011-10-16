#pragma once
#include "..\..\engine\ui.h"

class CheckBox;
//切换点编辑
//列表
class WndExport :
	public ui::Wnd
{
	CheckBox * m_pCheckSurface;
	CheckBox * m_pCheckHeightmap;
	CheckBox * m_pCheckColormap;
	CheckBox * m_pCheckGrid;
	CheckBox * m_pCheckNode;
	CheckBox * m_pCheckSound;//音效文件（列表）

	BOOL	   m_bIsPopupCheckFlag;
	ui::Static *m_pStaticTitle;
	ui::Button * m_pBtnOk;
	ui::Button * m_pBtnCancel;
	BOOL	   m_bIsOk;

public:
	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent);
	void Destroy();

	CheckBox * GetControlCheckSurface() { return m_pCheckSurface;}
	CheckBox * GetControlCheckHeightmap() { return m_pCheckHeightmap;}
	CheckBox * GetControlCheckColormap() { return m_pCheckColormap;}
	CheckBox * GetControlCheckGrid() { return m_pCheckGrid;}
	CheckBox * GetControlCheckNode() { return m_pCheckNode;}
	CheckBox * GetControlCheckSound() { return m_pCheckSound;}

	void BeginPopUp(void);
	void EndPopUp();
	BOOL CheckPopUp() { BOOL bValue = m_bIsPopupCheckFlag; m_bIsPopupCheckFlag = FALSE;return bValue;}

	BOOL IsOK() { return m_bIsOk;}

    virtual void Update();

	WndExport(void);
	~WndExport(void);
};
