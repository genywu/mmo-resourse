#include "StdAfx.h"
#include ".\loginplayerlist.h"

//CLoginPlayerList::CLoginPlayerList(void)
//{
//}
//
//CLoginPlayerList::~CLoginPlayerList(void)
//{
//}

CLoginPlayerList *CLoginPlayerList::instance = NULL;

CLoginPlayerList* CLoginPlayerList::GetInstance()
{
	if(!instance)
	{
		instance = new CLoginPlayerList();
	}
	return instance;
}

void CLoginPlayerList::Release()
{
	if(instance)
	{
		delete instance;
		instance = NULL;
	}
}
/*
* 追加一个Player到PlayerList
*/
void CLoginPlayerList::AppendPlayer(CPlayer *pPlayer)
{
	if(!pPlayer) return;

	for(list<CPlayer*>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		// 如果列表中存在该玩家，就用新的替换到列表中的。
		if( pPlayer->GetID() == (*it)->GetID() )
		{
			CPlayer *pTemp = *it;
			SAFE_DELETE(pTemp);
			*it = pPlayer;
		}
	}

	//add to player list in back
	GetPlayerList()->push_back(pPlayer);

	// pronounce server managerment this palyer has been added
	AddPlayerList(pPlayer->GetName());
}

/*
* 从PlayerList移出一个player	
*/
void CLoginPlayerList::RemovePlayer(CPlayer *pPlayer)
{
	if (!pPlayer) return;
	//remove the player from playerlist
	GetPlayerList()->remove(pPlayer);

	DelPlayerList(pPlayer->GetName());
}

/*
* 从PlayerList删除一个Player
*/
void CLoginPlayerList::DeletePlayer(CPlayer *pPlayer)
{
	if (!pPlayer) return;

	for(list<CPlayer*>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		if (pPlayer->GetID() == (*it)->GetID())
		{
			CPlayer* pTemp = *it;
			SAFE_DELETE(pTemp);
			GetPlayerList()->erase(it);
			DelPlayerList(pPlayer->GetName());
		}
	}
}

/*
* 通过PlayerID在PlayerList返回一个Player
*/
CPlayer* CLoginPlayerList::GetPlayerByID(UINT nID)
{
	CPlayer* pPlayer = NULL;
	for(list<CPlayer*>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		if( (*it)->GetID() == nID )
		{
			pPlayer = *it;
			break;
		}
	}
	return pPlayer;
}

/*
* 通过PlayerName在PlayerList返回一个PlayerID
*/
UINT CLoginPlayerList::GetPlayerIDByName(LPCSTR szName)
{
	long nID = 0;

	for(list<CPlayer*>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		// 在PlayerList找的名字相同的
		if( !strcmpi((*it)->GetName(), szName) )
		{
			nID = (*it)->GetID();
			break;
		}
	}
	return nID;
}

/*
* 通过PlayerID复制一个Player
*/
CPlayer* CLoginPlayerList::ClonePlayer(UINT nID)
{
	CPlayer* pClonePlayer = NULL;
	for(list<CPlayer*>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		if( (*it)->GetID() == nID )
		{
			pClonePlayer = new CPlayer;
			long lPos=0;
			CPlayer* pPlayer = *it;
			vector<BYTE> vBuf;
			pPlayer->AddToByteArray(&vBuf);
			pClonePlayer->DecordFromByteArray(&vBuf[0],lPos);
			break;
		}
	}
	return pClonePlayer;
}