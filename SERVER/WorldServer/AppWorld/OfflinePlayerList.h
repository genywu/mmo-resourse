/*\
 *	created:	2005-31-1 12:34:53
 *	filename: 	d:\Project\Server\WorldServer\AppWorld\OfflinePlayerList.h
 *	file path:	d:\Project\Server\WorldServer\AppWorld
 *	file base:	OfflinePlayerList
 *	file ext:	h
 *	author:		Bugs
 *	
 *	purpose:	保存离线后还未写盘的角色列表
\*/

#pragma once

#ifndef _INC_OFFLINEPLAYERLIST_H
#define _INC_OFFLINEPLAYERLIST_H

class COfflinePlayerList
{
private:
	COfflinePlayerList(void) {};
	virtual ~COfflinePlayerList(void) {};

public:
	static COfflinePlayerList* GetInstance();
	static void Release();

private:
	static COfflinePlayerList *instance;

public:
	/*static*/ list<CPlayer*>* GetPlayerList() { return &m_liOfflinePlayer; }
	/*static*/ void ClearPlayerList() { GetPlayerList()->clear(); }
	/*static*/ UINT64 GetPlayerCount() { return GetPlayerList()->size(); } 

	/*static*/ void AppendPlayer(CPlayer *pPlayer);
	/*static*/ void RemovePlayer(CPlayer *pPlayer);
	/*static*/ void DeletePlayer(CPlayer *pPlayer);
	/*static*/ CPlayer* GetPlayerByID(UINT nID);
	/*static*/ UINT GetPlayerIDByName(LPCSTR szName);
	/*static*/ CPlayer* ClonePlayer(UINT nID);

private:
	// 在线玩家玩家列表
	/*static*/ list<CPlayer*>	m_liOfflinePlayer;
};

#endif // _INC_OFFLINEPLAYERLIST_H