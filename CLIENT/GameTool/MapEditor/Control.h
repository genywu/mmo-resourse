#pragma once
#include "..\..\frostengine\ui.h"

class RadioButton :
	public ui::Wnd
{
	DWORD m_dwTextureHandle;
	BOOL  m_bClicked;
	BOOL  m_bChecked;
	RECT  m_rc;

	BOOL  m_bDisplayText;
public:
	RadioButton(void);
	~RadioButton(void);

	BOOL IsClicked();
	void DisplayText(BOOL bValue) { m_bDisplayText = bValue;}
	void SetChecked(BOOL bFlag = TRUE);
	void SetTextureHandle(DWORD dwTextureHandle);
	void SetTextureRect(RECT &rc) { m_rc = rc;}
	RECT GetTextureRect() { return m_rc;}

	DWORD GetTextureHandle() { return m_dwTextureHandle; }

	virtual void Render();
	virtual void Update();
};

class CheckBox:
	public ui::Wnd
{
	BOOL  m_bChecked;

public:
	CheckBox(void);
	~CheckBox(void);

	void SetChecked(BOOL bFlag = TRUE);
	BOOL IsChecked() const { return m_bChecked;}

	virtual void Render();
	virtual void Update();
};
