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
 * ׷��һ��Player��PlayerList
 */
void COnlinePlayerList::AppendPlayer(CPlayer *pPlayer)
{
	if(!pPlayer) return;

	for(list<CPlayer*>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		// ����б��д��ڸ���ң������µ��滻���б��еġ�
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
 * ��PlayerList�Ƴ�һ��player	
 */
void COnlinePlayerList::RemovePlayer(CPlayer *pPlayer)
{
	if (!pPlayer) return;
	//remove the player from playerlist
	GetPlayerList()->remove(pPlayer);

	DelPlayerList(pPlayer->GetName());
}

/*
 * ��PlayerListɾ��һ��Player
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
 * ͨ��PlayerID��PlayerList����һ��Player
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
 * ͨ��PlayerName��PlayerList����һ��PlayerID
 */
UINT COnlinePlayerList::GetPlayerIDByName(LPCSTR szName)
{
	long nID = 0;
	
	for(list<CPlayer*>::iterator it=GetPlayerList()->begin(); it!=GetPlayerList()->end(); it++)
	{
		// ��PlayerList�ҵ�������ͬ��
		if( !strcmpi((*it)->GetName(), szName) )
		{
			nID = (*it)->GetID();
			break;
		}
	}
	return nID;
}

/*
 * ͨ��PlayerID����һ��Player
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