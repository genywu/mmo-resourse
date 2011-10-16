#pragma once
#ifndef _INC_CDELETIONPLAYER_H
#define _INC_CDELETIONPLAYER_H


class CDeletionPlayerList
{
private:
	CDeletionPlayerList(void) {};
	virtual ~CDeletionPlayerList(void) {};

public:
	static CDeletionPlayerList* GetInstance();
	static void Release();

private:
	static CDeletionPlayerList *instance;

public:
	struct tagDeletionPlayer 
	{
		//tagDeletionPlayer() {pPlayer=NULL; nLocation=0;}
		//~tagDeletionPlayer();
		UINT nPlayerID;
		time_t tDelDate;
	};

public:
	/*static*/ list<tagDeletionPlayer>* GetPlayerList() { return &m_liDeletionPlayer; }
	/*static*/ void ClearPlayerList() { GetPlayerList()->clear(); }
	/*static*/ UINT64 GetPlayerCount() { return GetPlayerList()->size(); } 

	time_t IsExist(UINT nPlayerID);
	/*static*/ void AppendPlayer(UINT nPlayerID, time_t tDelDate);
	/*static*/ void DeletePlayer(UINT nPlayerID);

private:
	// 待创建玩家列表
	/*static*/ list<tagDeletionPlayer> m_liDeletionPlayer;
};

#endif // _INC_CDELETIONPLAYER_H