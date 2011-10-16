
#include "stdafx.h"

#include "ListBoxSecurity.h"

//安全区定义
const char * pSecurityName[] = 
{
	"非安全区",
	"竞技区",
	"安全区"
};

ListBoxSecurity::ListBoxSecurity(void)
{
}

ListBoxSecurity::~ListBoxSecurity(void)
{
}

BOOL ListBoxSecurity::Create(const char *pszName,const RECT *pRect,DWORD dwStyle,Wnd *pParent)
{
	ui::ListBox::Create(pszName,pRect,dwStyle,pParent);

	for(int i = 0;i < SECURTIY_END; i++)
	{
		this->InsertItem(pSecurityName[i]);
	}

	SetSelItem(0);

	return TRUE;
}

