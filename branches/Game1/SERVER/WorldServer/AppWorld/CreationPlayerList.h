#pragma once

#ifndef _INC_CREATIONPLAYERLIST_H
#define _INC_CREATIONPLAYERLIST_H

class CCreationPlayerList
{
private:
	CCreationPlayerList(void) {};
	virtual ~CCreationPlayerList(void) {};

public:
	static CCreationPlayerList* GetInstance();
	static void Release();

private:
	static CCreationPlayerList *instance;

public:
	struct tagCreationPlayer 
	{
		//tagCreationPlayer() {pPlayer=NULL; nLocation=0;}
		//~tagCreationPlayer();

		CPlayer *pPlayer;

		// 1 ~ 3 (0 ERROR)
		BYTE	nLocation;
	};

public:
	/*static*/ list<tagCreationPlayer>* GetPlayerList() { return &m_liCreationPlayer; }
	/*static*/ void ClearPlayerList() { GetPlayerList()->clear(); }
	/*static*/ size_t GetPlayerCount() { return GetPlayerList()->size(); } 

	/*static*/ void AppendPlayer(CPlayer *pPlayer, BYTE nLocation);
	/*static*/ void DeletePlayer(CPlayer *pPlayer);
	/*static*/ bool IsNameExist(LPCSTR szName);

	/*static*/ BYTE GetLocationStateInCdkey(LPCSTR szCdkey);

	/*static*/ BYTE GetPlayerCountInCdkey(LPCSTR szCdkey);

	/*static*/ vector<tagCreationPlayer> GetPlayerVectorByCdkey(LPCSTR szCdkey);

	/*static*/ CPlayer* GetPlayerByID(UINT nID);
	/*static*/ CPlayer* ClonePlayer(UINT nID);

private:
	// 待创建玩家列表
	/*static*/ list<tagCreationPlayer> m_liCreationPlayer;
};

#endif // _INC_CREATIONPLAYERLIST_H