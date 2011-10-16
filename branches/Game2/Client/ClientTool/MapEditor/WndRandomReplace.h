#pragma once
#include "..\..\engine\ui.h"

class WndRandomReplace :
	public ui::Wnd
{

	ui::ListBox * m_pListBox;

	ui::Button * m_pBtnAdd;
	ui::Button * m_pBtnDel;

	ui::Button * m_pBtnOk;
	ui::Button * m_pBtnCancel;

	ui::EditBox * m_pEditTarget;
	ui::EditBox * m_pEditSource;

	ui::EditBox * m_pEditMinDistance;
	ui::EditBox * m_pEditMaxDistance;

	BOOL m_bIsOK;
	BOOL m_bIsPopupCheckFlag;//¼ì²éµ¯³ö½áÊø	

public:
	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent);
	void Destroy();

	BOOL IsOK() { return m_bIsOK;}

	BOOL CheckPopUp() { BOOL bValue = m_bIsPopupCheckFlag; m_bIsPopupCheckFlag = FALSE;return bValue;}

	ui::ListBox * GetControlListBox() { return m_pListBox;}
	ui::EditBox * GetControlMinEditBox() { return m_pEditMinDistance;}
	ui::EditBox * GetControlMaxEditBox() { return m_pEditMaxDistance;}

    virtual void Update();

	void BeginPopUp(void);
	void EndPopUp(void);

	WndRandomReplace(void);
	~WndRandomReplace(void);

};

