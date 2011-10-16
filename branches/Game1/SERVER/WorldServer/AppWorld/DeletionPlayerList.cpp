#include "StdAfx.h"
#include ".\DeletionPlayerlist.h"

//CDeletionPlayerList::CDeletionPlayerList(void)
//{
//}
//
//CDeletionPlayerList::~CDeletionPlayerList(void)
//{
//}
CDeletionPlayerList *CDeletionPlayerList::instance = NULL;

CDeletionPlayerList* CDeletionPlayerList::GetInstance()
{
	if(!instance)
	{
		instance = new CDeletionPlayerList();
	}
	return instance;
}

void CDeletionPlayerList::Release()
{
	if(instance)
	{
		delete instance;
		instance = NULL;
	}
}

time_t CDeletionPlayerList::IsExist(UINT nPlayerID)
{
	for(list<tagDeletionPlayer>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		if(nPlayerID == (*it).nPlayerID)
		{
			return (*it).tDelDate;
		}
	}
	return 0;
}

/*
* 追加一个Player到PlayerList
*/
void CDeletionPlayerList::AppendPlayer(UINT nPlayerID, time_t tDelDate)
{
	// player==NULL THEN exit
	assert(nPlayerID);

	for(list<tagDeletionPlayer>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		// 如果列表中存在该玩家
		if( nPlayerID == (*it).nPlayerID ) return;
	}
	tagDeletionPlayer DeletionPlayer = {nPlayerID, tDelDate};
	GetPlayerList()->push_back(DeletionPlayer);
}
void CDeletionPlayerList::DeletePlayer(UINT nPlayerID)
{
	if (!nPlayerID) return;

	for(list<tagDeletionPlayer>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		if(nPlayerID == (*it).nPlayerID)
		{
			GetPlayerList()->erase(it);
		}
	}
}
