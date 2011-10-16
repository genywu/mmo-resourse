//////////////////////////////////////////////////////////////////////////
//Servervillageregion.cpp
//村战战地图类，负责村战的数据处理
//Author：Joe
//Create Date:2005.7.5
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <MMSystem.h>
#include ".\servervillageregion.h"
#include "Player.h"

#include "appserver\Organizingsystem\VillageWarSys.h"

#include "..\nets\netserver\message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CServerVillageRegion::CServerVillageRegion(void)
:CServerWarRegion(0)
{
	m_listGoods.clear();
	m_FlagOwnerFacID = NULL_GUID;
}
CServerVillageRegion::CServerVillageRegion(const CGUID& id)
:CServerWarRegion(id)
{
	m_listGoods.clear();
	m_FlagOwnerFacID = NULL_GUID;
}
CServerVillageRegion::CServerVillageRegion(long id)
:CServerWarRegion(id)
{
	m_listGoods.clear();
	m_FlagOwnerFacID = NULL_GUID;
}

CServerVillageRegion::~CServerVillageRegion(void)
{
}

// 解码
bool CServerVillageRegion::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData)
{
	CServerWarRegion::DecordFromByteArray(pByte,pos,bExData);
	return true;
}

//判断某个帮会是否能争夺
bool CServerVillageRegion::IsOwner(const CGUID& FactionID)
{
	//[RGN TEST]
/*	long lVilRegionID = GetVilWarSys()->GetVilRegionIDByTime(GetWarNum());
	CServerRegion* pVilRegion	=GetGame()->FindRegion(lVilRegionID);
	if(pVilRegion == NULL)
		pVilRegion = GetGame()->FindProxyRegion(lVilRegionID);
	if(pVilRegion == NULL)	return false;

	//判断该玩家的帮会是否申请的参加	
	if( NULL_GUID != FactionID && FactionID == pVilRegion->GetOwnedCityFaction() )
	{
		return true;
	}*/
	return false;
}

void CServerVillageRegion::OnWarDeclare(long lWarNum)
{
	CServerWarRegion::OnWarDeclare(lWarNum);

	m_FlagOwnerFacID = GetOwnedCityFaction();
}
//某个争夺时间到
void CServerVillageRegion::OnContendTimeOver(tagContend Contend)
{
	CServerWarRegion::OnContendTimeOver(Contend);
}

//添加需要的道具名
void CServerVillageRegion::AddNeedGood(string& strGoodName)
{
	if( strGoodName == "")	return;

	m_listGoods.push_back(strGoodName);
}


//攻城开始
void CServerVillageRegion::OnWarStart(long lWarNum)
{
	if( GetWarNum() != lWarNum)	return;

	CServerWarRegion::OnWarStart(lWarNum);
	m_bKickOutPlayer = false;
}

//攻村时间结束
void CServerVillageRegion::OnWarTimeOut(long lWarNum)
{
	//CMessage msg(MSG_S2W_WAR_FactionWinVillage);
	//msg.Add(GetWarNum());
	//msg.Add(GetID());
	//msg.Add(m_FlagOwnerFacID);
	//msg.Add((long)0);
	//msg.Send();
}

//战斗结束
void  CServerVillageRegion::OnWarEnd(long lWarNum)
{
	if( GetWarNum() != lWarNum)	return;

	if(GetCityState() == CIS_NO)	return;
	//清除申请帮会列表
	CServerWarRegion::OnWarEnd(lWarNum);
	ClearRegion();
	//设置无国家
	SetCountry(0);
	//设置无拥有者
	SetOwnedCityOrg(NULL_GUID, NULL_GUID);
	m_FlagOwnerFacID = NULL_GUID;
}

//当一个帮会赢得了一个标志物触发
void CServerVillageRegion::OnFactionWinOneSymbol(const CGUID& FactionID,long lSymbolID)
{
	if(lSymbolID == 0)
	{
		m_FlagOwnerFacID = FactionID;
	}
}
//当一个帮会获胜时
void  CServerVillageRegion::OnFactionVictory(const CGUID& FactionID,const CGUID& UnionID)
{
	if(GetCityState() == CIS_NO)	return;

	CServerWarRegion::OnFactionVictory(FactionID,UnionID);
}

//结束时清场景
void CServerVillageRegion::ClearRegion()
{
	CServerWarRegion::ClearRegion();	

	list<string>::iterator it = m_listGoods.begin();
	for(; it != m_listGoods.end();it++)
	{
		DelOneGoodFromAllPlayer((char*)(*it).c_str());
	}
	//开始清除玩家一分钟倒计时
	StartClearPlayerOut(60000);
}

bool CServerVillageRegion::IsApplyWarFacsMem(const CGUID& FactionID)
{
	return GetVilWarSys()->IsAlreadyDeclarForWar( GetWarNum(),FactionID );
}
