//////////////////////////////////////////////////////////////////////////
//Servervillageregion.h
//村战战地图类，负责村战的数据处理
//Author：Joe
//Create Date:2005.7.5
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "serverwarregion.h"

class CServerVillageRegion :
	public CServerWarRegion
{
public:
	CServerVillageRegion(void);
	CServerVillageRegion(const CGUID& id);
	CServerVillageRegion(long id);
	~CServerVillageRegion(void);

	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码

private:
	list<string>	m_listGoods;				//进入争夺状态需要的物品
	CGUID			m_FlagOwnerFacID;			//标志物拥有者的ID
public:
	bool IsOwner(const CGUID& FactionID);				//判断某个帮会是否能争夺
	void OnWarDeclare(long lWarNum);			//开始宣战
	void OnContendTimeOver(tagContend Contend);	//某个争夺时间到
	void OnWarStart(long lWarNum);				//攻村开始
	void OnWarTimeOut(long lWarNum);			//攻村时间结束
	void OnWarEnd(long lWarNum);				//战斗结束
	void AddNeedGood(string& strGoodName);		//添加需要的道具名
	void OnFactionWinOneSymbol(const CGUID& FactionID,long lSymbolID);	//当一个帮会赢得了一个标志物触发

	void OnFactionVictory(const CGUID& FactionID,const CGUID& UnionID);	//当一个帮会获胜时
	void ClearRegion();										//结束时清场景

	bool IsApplyWarFacsMem(const CGUID& FactionID);
};
