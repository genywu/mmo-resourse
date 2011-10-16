#pragma once

#include "..\..\frostengine\ui.h"

class WndGuage : public ui::Wnd
{
	float		 m_fGuagePer;
	ui::Button * m_pBtnCancel;

public:
	WndGuage(void);
	~WndGuage(void);

	virtual void Update();
	virtual void Render();

	virtual BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,ui::Wnd *pParent);
	virtual void Destroy();

	void BeginPopUp(void);
	void EndPopUp(void);

	BOOL IsCancel()						{ return m_pBtnCancel->IsClicked(); }
	void SetPercent(float fPer)			{ m_fGuagePer = fPer; }
};