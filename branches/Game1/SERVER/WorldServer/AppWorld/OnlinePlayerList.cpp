#include "stdafx.h"
#include "OnlinePlayerList.h"

//COnlinePlayerList::COnlinePlayerList(void)
//{
//}
//
//COnlinePlayerList::~COnlinePlayerList(void)
//{
//}

COnlinePlayerList *COnlinePlayerList::instance = NULL;

COnlinePlayerList* COnlinePlayerList::GetInstance()
{
	if(!instance)
	{
		instance = new COnlinePlayerList();
	}
	return instance;
}

void COnlinePlayerList::Release()
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
void COnlinePlayerList::AppendPlayer(CPlayer *pPlayer)
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
void COnlinePlayerList::RemovePlayer(CPlayer *pPlayer)
{
	if (!pPlayer) return;
	//remove the player from playerlist
	GetPlayerList()->remove(pPlayer);

	DelPlayerList(pPlayer->GetName());
}

/*
 * 从PlayerList删除一个Player
 */
void COnlinePlayerList::DeletePlayer(CPlayer *pPlayer)
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
CPlayer* COnlinePlayerList::GetPlayerByID(UINT nID)
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
UINT COnlinePlayerList::GetPlayerIDByName(LPCSTR szName)
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
CPlayer* COnlinePlayerList::ClonePlayer(UINT nID)
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