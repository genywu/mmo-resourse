#pragma once
#include "..\..\frostengine\ui.h"

//class CheckBox;

class WndAnimModify :
	public ui::Wnd
{
	ui::Static		*m_pStaticTimeLength;

	ui::EditBox *m_pEditTimeLength;
	ui::EditBox *m_pEditCurTime;

public:
	WndAnimModify(void);
	virtual ~WndAnimModify(void);

	BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent);
	void Destroy();

	virtual void Update();
	virtual void Render();

	void SetTimeLength(DWORD dwTime);
	void SetCurTime(DWORD dwElapseTime);

	DWORD GetTimeLength();
};
