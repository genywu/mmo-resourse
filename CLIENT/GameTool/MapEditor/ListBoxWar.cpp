
#include "stdafx.h"

#include "ListBoxWar.h"

//安全区定义
const char * pWarName[] = 
{
	"非城战区",
	"城战区",
};

ListBoxWar::ListBoxWar(void)
{
}

ListBoxWar::~ListBoxWar(void)
{
}

BOOL ListBoxWar::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent)
{
	ui::ListBox::Create(pszName,pRect,dwStyle,pParent);

	for(int i = 0;i < WAR_END; i++)
	{
		this->InsertItem(pWarName[i]);
	}

	SetSelItem(0);

	return TRUE;
}

