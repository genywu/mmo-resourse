#pragma once
#include "..\..\frostengine\ui.h"


enum eSECURITY
{
	SECURITY_FREE = 0,		//非安全区
	SECURTIY_FIGHT,			//竞技区
	SECURTIY_SAFE,			//安全区
	SECURTIY_END
};

class ListBoxSecurity : public ui::ListBox
{
public:
	virtual BOOL Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent);

	ListBoxSecurity(void);
	~ListBoxSecurity(void);
};
