/*\
 *	created:	2005-31-1 12:34:30
 *	filename: 	d:\Project\Server\WorldServer\AppWorld\OnlinePlayerList.h
 *	file path:	d:\Project\Server\WorldServer\AppWorld
 *	file base:	OnlinePlayerList
 *	file ext:	h
 *	author:		Bugs
 *	
 *	purpose:	保存在线角色列表
\*/

#pragma once

#ifndef _INC_ONLINEPLAYERLIST_H
#define _INC_ONLINEPLAYERLIST_H


class COnlinePlayerList
{
private:
	COnlinePlayerList(void) {};
	virtual ~COnlinePlayerList(void) {};

public:
	static COnlinePlayerList* GetInstance();
	static void Release();

private:
	static COnlinePlayerList *instance;

public:
	/*static*/ list<CPlayer*>* GetPlayerList() { return &m_liOnlinePlayer; }
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
	/*static*/ list<CPlayer*>	m_liOnlinePlayer;
};


#endif // _INC_ONLINEPLAYERLIST_H
