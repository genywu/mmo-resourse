/*\
 *	created:	2005-31-1 12:32:07
 *	filename: 	d:\Project\Server\WorldServer\AppWorld\LoginPlayerList.h
 *	file path:	d:\Project\Server\WorldServer\AppWorld
 *	file base:	LoginPlayerList
 *	file ext:	h
 *	author:		Bugs
 *	
 *	purpose:	保存login server在发送消息到world server请求即将登录的角色信息的角色列表
\*/

#pragma once

class CLoginPlayerList
{
private:
	CLoginPlayerList(void) {};
	virtual ~CLoginPlayerList(void) {};

public:
	static CLoginPlayerList* GetInstance();
	static void Release();

private:
	static CLoginPlayerList *instance;

public:
	/*static*/ list<CPlayer*>* GetPlayerList() { return &m_liLoginPlayer; }
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
	/*static*/ list<CPlayer*>	m_liLoginPlayer;
};
