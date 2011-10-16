/*\
 *	created:	2005-1-2 9:54:47
 *	filename: 	d:\Project\Server\WorldServer\AppWorld\DBData.h
 *	file path:	d:\Project\Server\WorldServer\AppWorld
 *	file base:	DBData
 *	file ext:	h
 *	author:		Bugs
 *	
 *	purpose:	singleton class
\*/

#pragma once

#ifndef _INC_CDBDATA_H
#define _INC_CDBDATA_H

#include "CreationPlayerList.h"
#include "DeletionPlayerList.h"
#include "OnlinePlayerList.h"
#include "OfflinePlayerList.h"
#include "GoodsID.h"
#include "VarList.h"
#include "PlayerID.h"

class CDBData
{
private:
	CDBData();
	virtual ~CDBData();

public:
	static CDBData* GetInstance();
	static void Release();

private:
	static CDBData *instance;

public:
	struct tagDBData
	{
		// 待创建玩家列表
		list<CCreationPlayerList::tagCreationPlayer>	liDBCreationPlayer;
		// 待删除玩家列表
		list<CDeletionPlayerList::tagDeletionPlayer>	liDBDeletionPlayer;

		// 在线待保存玩家列表
		list<CPlayer*>	liDBOnlinePlayer;
		// 离线待保存玩家列表
		list<CPlayer*>	liDBOfflinePlayer;

		// 角色id
		UINT			nPlayerID;

		CGoodsID::tagGoodsID		stGoodsID[16];		// 物品起始ID

		CVariableList	*pVariableList;		// 全局脚本变量
	};

public:
	// 复制需要存盘的数据到SaveData
	/*static*/ void GenerateDBData();

	/*static*/ CPlayer* CloneDBPlayer(UINT nID);				// 根据ID从待存盘玩家列表中得到一个玩家(复制一份)

private:
	/*static*/ void AppendDBCreationPlayer(CPlayer* pPlayer, BYTE nLocation);
	/*static*/ void AppendDBDeletionPlayer(DWORD nPlayerID, time_t tDelDate);			// 添加一个玩家到待存盘的玩家列表
	/*static*/ void AppendDBOnlinePlayer(CPlayer* pPlayer);			// 添加一个玩家到待存盘的玩家列表
	/*static*/ void AppendDBOfflinePlayer(CPlayer* pPlayer);			// 添加一个玩家到待存盘的玩家列表

	/*static*/ void ClearDBCreationPlayer();							// 清空待存盘的玩家列表
	/*static*/ void ClearDBDeletionPlayer();							// 清空待存盘的玩家列表
	/*static*/ void ClearDBOnlinePlayer();							// 清空待存盘的玩家列表
	/*static*/ void ClearDBOfflinePlayer();							// 清空待存盘的玩家列表

	/*static*/ CPlayer* ClonePlayerFromOnline(UINT nID);
	/*static*/ CPlayer* ClonePlayerFromOffline(UINT nID);

private:
	/*static*/ tagDBData m_stDBData;				// 存盘数据(每个存盘点生成一次，存盘结束后释放)
};


#endif // _INC_CDBDATA_H