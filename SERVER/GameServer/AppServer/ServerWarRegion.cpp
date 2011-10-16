//////////////////////////////////////////////////////////////////////////
//serverwarregion.cpp
//战斗争夺场景的基类
//Author：Joe
//Create Date:2005.7.5
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <MMSystem.h>
#include ".\serverwarregion.h"
#include "Player.h"
#include "..\..\setup\globesetup.h"
#include "country\country.h"

#include "..\nets\netserver\message.h"

CServerWarRegion::CServerWarRegion(void)
:CServerRegion(-1)
{
	m_listContend.clear();
	m_FacWinSymbol.clear();
	m_lSymbolTotalNum = 0;
	m_lWinVicSymbolNum = 0;
}
CServerWarRegion::CServerWarRegion(const CGUID& id)
:CServerRegion(id)
{
	m_listContend.clear();
	m_FacWinSymbol.clear();
	m_lSymbolTotalNum = 0;
	m_lWinVicSymbolNum = 0;
}
CServerWarRegion::CServerWarRegion(long id)
:CServerRegion(id)
{
	m_listContend.clear();
	m_FacWinSymbol.clear();
	m_lSymbolTotalNum = 0;
	m_lWinVicSymbolNum = 0;
}

CServerWarRegion::~CServerWarRegion(void)
{
}

// 添加到CByteArray
bool CServerWarRegion::AddToByteArray(vector<BYTE>* pByteArray, bool bExData )
{
	CServerRegion::AddToByteArray(pByteArray,bExData);
	return true;
}
// 解码
bool CServerWarRegion::DecordFromByteArray(BYTE* pByte, long& pos, bool bExData )
{
	CServerRegion::DecordFromByteArray(pByte,pos,bExData);
	m_lSymbolTotalNum = _GetLongFromByteArray(pByte, pos);
	m_lWinVicSymbolNum = _GetLongFromByteArray(pByte, pos);
	m_lVicSymbolNum = _GetLongFromByteArray(pByte, pos);

	//设置初始标志物的归属
	int i = 0;
	for(;i < m_lSymbolTotalNum;i++)
	{
		m_FacWinSymbol[i]=GetOwnedCityFaction();
	}
	return true;
}

//某个争夺时间到
void CServerWarRegion::OnContendTimeOver(tagContend Contend)
{	
	CPlayer* pPlayer = GetGame()->FindPlayer(Contend.PlayerID);
	if(pPlayer == NULL)	return;

	const CGUID& FactionID = pPlayer->GetFactionID();
	if(FactionID == NULL_GUID)	return;
	const CGUID& UnionID = pPlayer->GetUnionID();

	if( IsOwner(FactionID) == false )
	{
		if( IsApplyWarFacsMem(FactionID) == false)
			return;
	}

	//取消所有玩家对该标志物的争夺
	CancelContendBySymbolID(Contend.lID);

	SetFacWinSymbol(FactionID,UnionID,Contend.lID);

	//通告整个场景某个帮会夺取了该标志物
	DWORD dwColor = MAKELONG(MAKEWORD(255,255), MAKEWORD(255,255));
	DWORD dwBkColor = MAKELONG(MAKEWORD(0,0), MAKEWORD(255,255));
	//char strSend[256];
	//_snprintf(strSend,"%s帮会夺取了%s。",pPlayer->GetFactionName().c_str(),Contend.strName.c_str());
	//CMessage msg( MSG_S2C_OTHER_ADDINFO );
	//msg.Add((LONG)0);
	//msg.Add( dwColor );
	//msg.Add( dwBkColor );	// 背景色
	//msg.Add( strSend );
	//msg.SendToRegion(this);

	//添加顶端提示信息
	//char strTopInfo[256]="";
	//_snprintf(strTopInfo,"%s夺取了%s的[[[%s]]],暂时获得了战场的主动权。",pPlayer->GetFactionName().c_str(),GetName(),Contend.strName.c_str());
	//GetGame()->SendTopInfoToClient(-1,NULL_GUID,1,1,string(strTopInfo));

	//输出战斗调试信息
	//char strWarInfo[256];
	//_snprintf(strWarInfo,"WarNum:%d,OwnedFac:%d,OwnedUnion:%d :",GetWarNum(),GetOwnedCityFaction(),GetOwnedCityUnion() );
	//PutWarDebugString(strWarInfo);
	//_snprintf(strWarInfo,"%s(FacID:%d) 帮会夺取了 %s(ConID:%d)。(lWinFacID:%d,lWinUniID:%d)",
	//	pPlayer->GetFactionName().c_str(),pPlayer->GetFactionID(),Contend.strName.c_str(),Contend.lID,FactionID,UnionID);
	//PutWarDebugString(strWarInfo);
}

//当一个玩家点击进入争夺标志状态
void CServerWarRegion::OnEnterContend(CPlayer* pPlayer,long lID,const char* chName,long lTime,string strGood1,string strGood2,
								string strGood3,string strGood4)
{
	if(pPlayer == NULL)	return;
	//判断该玩家是否有帮会
	if(pPlayer->GetFactionID() == NULL_GUID)	return;
	//如果玩家是死亡状态，则退出
	if(pPlayer->GetState() == CShape::STATE_DIED || pPlayer->IsDied()) return;
	
	const CGUID& FactionID = pPlayer->GetFactionID();
	const CGUID& UnionID = pPlayer->GetUnionID();
	
	DWORD dwColor = MAKELONG(MAKEWORD(255,255), MAKEWORD(255,255));
	DWORD dwBkColor = MAKELONG(MAKEWORD(0,0), MAKEWORD(255,255));
	//判断该帮会或盟是否已经夺取了该标志物
	if(GetIsFacWinSymbol(FactionID,lID) == true)
	{
		char strSend[256]="你的所属帮会已经拥有了该标志物。";
		pPlayer->SendNotifyMessage(strSend,dwColor,dwBkColor);
		return;
	}

	if( IsOwner(FactionID) == false )
	{
		if(IsApplyWarFacsMem(FactionID) == false)
		{
			char strSend[256]="你的所属帮会还未报名参加战斗，无法占领该标志物。";
			pPlayer->SendNotifyMessage(strSend,dwColor,dwBkColor);
			return;
		}
	}
	//判断该玩家是否以在争夺该标志物]
	if( GetIsPlayerIsContendSymbol(pPlayer->GetExID(),lID) == true )	return;
	//判断该玩家是否有足够的物品
	if( ( strGood1 != "" && pPlayer->CheckGoods(strGood1.c_str() ) == 0 ) ||
		( strGood2 != "" && pPlayer->CheckGoods(strGood2.c_str() ) == 0 ) ||
		( strGood3 != "" && pPlayer->CheckGoods(strGood3.c_str() ) == 0 ) ||
		( strGood4 != "" && pPlayer->CheckGoods(strGood4.c_str() ) == 0 ) )
	{
		char strSend[256]="没有足够物品，无法夺取。";
		pPlayer->SendNotifyMessage(strSend,dwColor,dwBkColor);
		return;
	}

	CancelContendByPlayerID(pPlayer);

	AddContend(pPlayer,lID,chName,lTime);
	pPlayer->SendNotifyMessage("你进入了争夺状态。移动(被推动)、使用物品、发出攻击动作、死亡、退出游戏都会导致该状态被打断，受到伤害会延缓争夺进度。",dwColor,dwBkColor);

	AddNeedGood(strGood1);
	AddNeedGood(strGood2);
	AddNeedGood(strGood3);
	AddNeedGood(strGood4);
}

//设置某个帮会赢得某个标志物
void CServerWarRegion::SetFacWinSymbol(const CGUID& FactionID,const CGUID& UnionID,long lSymbol)
{
	m_FacWinSymbol[lSymbol] = FactionID;

	OnFactionWinOneSymbol(FactionID,lSymbol);
	//判断这个帮会夺取的标志物个数是否达到获胜个数
	if( FactionID != GetOwnedCityFaction() )
	{
		long lNum = 0;
		itFacSym it = m_FacWinSymbol.begin();
		for(;it != m_FacWinSymbol.end();it++)
		{
			if( (*it).second == FactionID )
				lNum++;
		}

		//触发一个帮会胜利
		if(lNum >= m_lWinVicSymbolNum)
		{
			OnFactionVictory(FactionID,UnionID);
		}
	}
}
//判断某个帮会是否赢得了某个标志物
bool CServerWarRegion::GetIsFacWinSymbol(const CGUID& FactionID,long lSymbolID)
{
	itFacSym it = m_FacWinSymbol.find(lSymbolID);
	if(it == m_FacWinSymbol.end())	return false;

	if((*it).second  == FactionID )	return true;
	return false;
}

//判断某个玩家是否正在争夺某个标志物中
bool CServerWarRegion::GetIsPlayerIsContendSymbol(const CGUID& PlayerID,long lSymbolID)
{
	itContend it = m_listContend.begin();
	for(;it != m_listContend.end();it++)
	{
		if((*it).PlayerID == PlayerID)
		{
			if( (*it).lID == lSymbolID )
				return true;
			else
				return false;
		}
	}
	return false;
}
//添加一个争夺
void CServerWarRegion::AddContend(CPlayer* pPlayer,long lID,const char* chName,long lTime)
{
	if(pPlayer == NULL)	return;

	bool bFacFirst = true;
	itContend it = m_listContend.begin();
	for(;it != m_listContend.end();it++)
	{
		if((*it).FactionID == pPlayer->GetFactionID())
		{
			bFacFirst = false;
			break;
		}
	}
	
	tagContend Contend = {lID,string(chName),pPlayer->GetExID(),pPlayer->GetFactionID(),0,lTime,timeGetTime()};
	m_listContend.push_back(Contend);
	//更新争夺状态
	//pPlayer->SetContendState(true);
	//刷新时间到客户端
	UpdateContentTime(pPlayer->GetExID(),0);

	//如果是该帮会的第一次，发消息通知玩家某个帮会开始封印
	if(bFacFirst)
	{
		//DWORD dwColor = MAKELONG(MAKEWORD(255,255), MAKEWORD(255,255));
		//DWORD dwBkColor = MAKELONG(MAKEWORD(0,0), MAKEWORD(255,255));
		//char strSend[256]="";
		//BYTE bCountry = pPlayer->GetCountry();
		//_snprintf(strSend,"%s的%s帮会开始封印%s。",CountryName[bCountry],pPlayer->GetFactionName().c_str(),chName);
		//CMessage msg( MSG_S2C_OTHER_ADDINFO );
		//msg.Add((LONG)0);
		//msg.Add( dwColor );
		//msg.Add( dwBkColor );
		//msg.Add( strSend );
		//msg.SendToRegion(this);
	}
}

//当玩家攻击受到伤害
void CServerWarRegion::OnPlayerDamage(CPlayer* pPlayer,long lDamHp)
{
	if(lDamHp <= 0)	return;
	if(pPlayer == NULL)	return;

	float fPercent = (float)lDamHp/(float)pPlayer->GetMaxHP()*CGlobeSetup::GetSetup()->fDecTimeParam;

	list<tagContend>::iterator it = m_listContend.begin();
	for(;it != m_listContend.end();it++)
	{
		if((*it).PlayerID == pPlayer->GetExID())
		{
			long lDecTime = (*it).lMaxTime*fPercent;
			(*it).lCurTime -= lDecTime;
			if((*it).lCurTime < 0)
				(*it).lCurTime = 0;

			long lPercent = 0;
			if( (*it).lMaxTime != 0)
			{
				lPercent = 100*(*it).lCurTime/(*it).lMaxTime;
			}

			//刷新客户端时间
			UpdateContentTime( (*it).PlayerID,lPercent );
			break;
		}
	}
}
//减少时间
void CServerWarRegion::DecContendTime(CPlayer* pPlayer,long lDecTime)
{
	if(pPlayer == NULL)	return;

	list<tagContend>::iterator it = m_listContend.begin();
	for(;it != m_listContend.end();it++)
	{
		if((*it).PlayerID == pPlayer->GetExID())
		{
			(*it).lCurTime -= lDecTime;
			if((*it).lCurTime < 0)
				(*it).lCurTime = 0;

			long lPercent = 0;
			if( (*it).lMaxTime != 0)
			{
				lPercent = 100*(*it).lCurTime/(*it).lMaxTime;
			}

			//刷新客户端时间
			UpdateContentTime( (*it).PlayerID,lPercent );
			break;
		}
	}
}

//取消某个玩家的争夺
bool CServerWarRegion::CancelContendByPlayerID(CPlayer* pPlayer)
{
	bool bFlag = false;
	if(pPlayer == NULL)	return bFlag;

	list<tagContend>::iterator it = m_listContend.begin();
	for(;it != m_listContend.end();)
	{
		if((*it).PlayerID == pPlayer->GetExID())
		{
			bFlag = true;
			it = m_listContend.erase(it);
		}
		else
		{
			++it;
		}
	}

	//更新争夺状态
	//pPlayer->SetContendState(false);
	//刷新时间到客户端
	UpdateContentTime(pPlayer->GetExID(),0);
	return true;
}

//取消所有玩家对某个标志物的争夺
void CServerWarRegion::CancelContendBySymbolID(long lSymbolID)
{
	itContend it = m_listContend.begin();
	for(;it != m_listContend.end();)
	{
		if((*it).lID == lSymbolID )
		{
			//刷新时间到客户端
			UpdateContentTime((*it).PlayerID,0);
			//更新争夺状态
			CPlayer* pPlayer = GetGame()->FindPlayer((*it).PlayerID);
			if(pPlayer)
				//pPlayer->SetContendState(false);

			it = m_listContend.erase(it);
		}
		else
		{
			++it;
		}
	}
}


//根据申请的帮会更新争夺玩家
void CServerWarRegion::UpdateContendPlayer()
{
	//如果是战斗时间
	if(GetCityState() == CIS_Fight)
	{
		itContend it = m_listContend.begin();
		for(;it != m_listContend.end();)
		{
			//更新争夺状态
			CPlayer* pPlayer = GetGame()->FindPlayer((*it).PlayerID);
			if(pPlayer)
			{
				//如果该帮会没有在申请列表中
				if(  IsApplyWarFacsMem( pPlayer->GetFactionID() ) == false)
				{
					//刷新时间到客户端
					UpdateContentTime((*it).PlayerID,0);
					//pPlayer->SetContendState(false);
					it = m_listContend.erase(it);
				}
				else
				{
					it++;
				}
			}
			else
			{
				it++;
			}
		}
	}
}

//刷新争斗状态时间到客户端
void CServerWarRegion::UpdateContentTime(const CGUID& PlayerID,long lPercent)
{
	//CMessage msg(MSG_S2C_WAR_UpdateContendTime);
	//msg.Add(lPercent);
	//msg.SendToPlayer(PlayerID);
}

//开始宣战
void CServerWarRegion::OnWarDeclare(long lWarNum)
{
	CServerRegion::OnWarDeclare(lWarNum);

	//设置初始标志物的归属
	int i = 0;
	m_FacWinSymbol.clear();
	for(;i < m_lSymbolTotalNum;i++)
	{
		m_FacWinSymbol[i]=GetOwnedCityFaction();
	}

}
//攻城开始
void CServerWarRegion::OnWarStart(long lWarNum)
{
	CServerRegion::OnWarStart(lWarNum);
}
//攻城集结
void CServerWarRegion::OnWarMass(long lWarNum)
{
	CServerRegion::OnWarMass(lWarNum);
}

//战斗结束
void  CServerWarRegion::OnWarEnd(long lWarNum)
{
	CServerRegion::OnWarEnd(lWarNum);
	m_FacWinSymbol.clear();
}

//当一个帮会获胜时
void CServerWarRegion::OnFactionVictory(const CGUID& FactionID,const CGUID& UnionID)
{
}

//结束时清场景
void CServerWarRegion::ClearRegion()
{
	list<tagContend>::iterator it = m_listContend.begin();
	for(;it != m_listContend.end();it++)
	{
		//恢复状态
		UpdateContentTime((*it).PlayerID,0);
		//CPlayer* pPlayer = dynamic_cast<CPlayer*>(this->FindChildObject(TYPE_PLAYER,(*it).PlayerID));
		//if(pPlayer)
			//pPlayer->SetContendState(false);
	}
	m_listContend.clear();
}

void CServerWarRegion::ReSetWarState(long lWarID,eCityState eState)
{
	CServerRegion::ReSetWarState(lWarID,eState);

	if(eState >= CIS_DUTH && eState <= CIS_Fight)
	{
		//设置初始标志物的归属
		int i = 0;
		m_FacWinSymbol.clear();
		for(;i < m_lSymbolTotalNum;i++)
		{
			m_FacWinSymbol[i]=GetOwnedCityFaction();
		}
	}
}

