#include "StdAfx.h"
#include ".\varlist.h"

//CVarList::CVarList(void)
//{
//}
//
//CVarList::~CVarList(void)
//{
//}
CVarList *CVarList::instance = NULL;

CVarList* CVarList::GetInstance()
{
	if(!instance)
	{
		instance = new CVarList();
	}
	return instance;
}

void CVarList::Release()
{
	if(instance)
	{
		delete instance;
		instance = NULL;
	}
}