#include "StdAfx.h"
#include "PlayerTrade.h"
#include "../../Game.h"
#include "../Other/AudioList.h"
#include "../MsgBoxSys/MsgEventManager.h"
#include "../MainPlayerHand.h"
#include "../../UI/GamePage/Package/PackagePage.h"
#include "../../../UI/UIDef.h"

extern void UpdataPlayerTradeItem(int index,DWORD dwWhose);

const long PlayerTrade::GRIDNUM = 1;

PlayerTrade::PlayerTrade(void)
{
	m_sessionID = CGUID::GUID_INVALID;
	m_myID = CGUID::GUID_INVALID;
	m_otherID = CGUID::GUID_INVALID;
	m_myPlugID = 0;
	m_otherPlugID = 0;
	m_myTradeGoodsList.clear();
	m_otherTradeGoodesList.clear();
	m_myIsLocked = false;
	m_otherIsLocked = false;
	m_result = false;
}

// 发送关闭消息
void PlayerTrade::SendClosePageMes(void)
{
	CMessage msg(MSG_C2S_PLAYERTRADE_CANCEL);
	msg.Add(m_sessionID);
	msg.Add((long)m_myPlugID);
	msg.Send();
}

// 关闭交易
void PlayerTrade::CloseTrade(void)
{
	GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-21.wav");

	m_sessionID = CGUID::GUID_INVALID;
	m_myID = CGUID::GUID_INVALID;
	m_otherID = CGUID::GUID_INVALID;
	m_myPlugID = 0;
	m_otherPlugID = 0;

	m_myIsLocked = false;
	m_otherIsLocked = false;
	m_result = false;

	if (!m_myTradeGoodsList.empty())
	{
		CPlayer* pMainPlayer = GetGame()->GetMainPlayer();
		CEGUI::Window* packagePage = GetInst(PackagePage).GetPageWindow();

		if (pMainPlayer!=NULL && packagePage!=NULL)
		{
			map<int,tagTradeGoods>::iterator GoodsIter = m_myTradeGoodsList.begin();
			for (;GoodsIter!=m_myTradeGoodsList.end();GoodsIter++)
			{
				CGoods* pGoods = pMainPlayer->GetGoodsByGUID(GoodsIter->second.guid);
				if (pGoods!=NULL)
				{
					if(pGoods->IsLocked())
						pGoods->UnLock();
					pGoods->SetHaveShadowState(false);
					//packagePage->UpdateGoodsShowByCGoods(pGoods);
				}
			}
		}
		m_myTradeGoodsList.clear();
	}
	if (!m_otherTradeGoodesList.empty())
		m_otherTradeGoodesList.clear();
}

// 设置锁定状态
void PlayerTrade::SetLockedState(bool bState,int iWhose)
{
	if (iWhose==Trade_Me)
		m_myIsLocked = bState;
	else if (iWhose==Trade_Other)
		m_otherIsLocked = bState;
}

// 取得某方的锁定状态
bool PlayerTrade::GetLockedState(int iWhose)
{
	if (iWhose==Trade_Me)
		return m_myIsLocked;
	else if (iWhose==Trade_Other)
		return m_otherIsLocked;
	return false;
}

// 取得交易对方玩家
CPlayer* PlayerTrade::GetOther()
{
	return (CPlayer*)(GetGame()->GetRegion()->FindChildObject(TYPE_PLAYER,m_otherID));
}

// 取得己方指定交易栏位物品数据
PlayerTrade::tagTradeGoods* PlayerTrade::GetMyGoodsData(int nIndex)
{
	if(nIndex>0 && nIndex<=GRIDNUM)
	{
		map<int,tagTradeGoods>::iterator its = m_myTradeGoodsList.find(nIndex);
		if (its != m_myTradeGoodsList.end())
		{
			return &(m_myTradeGoodsList[nIndex]);
		}
	}
	return NULL;
}

// 取得对方指定交易栏位物品数据
CGoods* PlayerTrade::GetOtherGoodsData(int nIndex)
{	
	if(nIndex>0 && nIndex<=GRIDNUM)
	{
		map<int,CGoods>::iterator its = m_otherTradeGoodesList.find(nIndex);
		if (its != m_otherTradeGoodesList.end())
		{
			return &(m_otherTradeGoodesList[nIndex]);
		}
	}
	return NULL;
}

bool PlayerTrade::OnInputGoods(CPlayer::tagGoods* pstGoods)
{
	if (pstGoods!=NULL&&pstGoods->pGoods!=NULL/*&&!m_bMeIsLocked*/)
	{
		CEGUI::Window* packagePage = GetInst(PackagePage).GetPageWindow();
		if (!packagePage)
			return false;

		//判断是否是勋章，是勋章则不能放入
		if(pstGoods->pGoods->GetProperty()->dwType == GBT_MEDAL)
			return false;
		// 不可交易的物品不能放入交易页面中交易
		if (pstGoods->pGoods->GetEffectVal(GAP_PARTICULAR_ATTRIBUTE,1)&32)
		{
			return false;
		}
		if (pstGoods->lNum!=pstGoods->pGoods->GetNum())
		{
			return false;
		}
		long lFreezeVal = pstGoods->pGoods->GetEffectVal(GAP_FREEZE,1);
		long lBindVal = pstGoods->pGoods->GetEffectVal(GAP_BIND,1);
		if (lFreezeVal!=eFT_UnFrost&&lFreezeVal!=eFT_CantFrost)
		{
			const char* strInfo = AppFrame::GetText("Goods_152");
			GetInst(MsgEventManager).PushEvent(Msg_Ok, strInfo);

			pstGoods->pGoods->SetHaveShadowState(false);
			//packagePage->UpdateGoodsShowByCGoods(pstGoods->pGoods);

			return true;
		}
		if (lBindVal==1||lBindVal==3)
		{
			const char* strInfo = AppFrame::GetText("Goods_153");
			GetInst(MsgEventManager).PushEvent(Msg_Ok, strInfo);

			pstGoods->pGoods->SetHaveShadowState(false);
			//packagePage->UpdateGoodsShowByCGoods(pstGoods->pGoods);

			return true;
		}
		int order = -1;
		for (int i = 0;i<GRIDNUM;i++)
		{
			if (NULL==GetMyGoodsData(i))
			{
				order = i;
				break;
			}
		}
		if (order!=-1)
		{
			CPlayer* pPlayer = GetGame()->GetMainPlayer();
			if (pPlayer==NULL) return false;
			DWORD dwAmount=pstGoods->pGoods->GetNum();
			CGUID PlayerGUID = pPlayer->GetExID();
			CGUID GoodsGUID = pstGoods->pGoods->GetExID();
			GetGame()->GetMainPlayerHand()->SendPutDownMes(TYPE_PLAYER,PlayerGUID,pstGoods->lType,pstGoods->lPos,TYPE_SESSION,
				m_sessionID,m_myPlugID<<8,order,TYPE_GOODS,GoodsGUID,dwAmount);
			pstGoods->pGoods->Lock();
			//packagePage->UpdateGoodsShowByCGoods(pstGoods->pGoods);

			GetGame()->GetAudioList()->PlayEquipmentSound(pstGoods->pGoods->GetIndex());
			return true;
		}
	}
	return false;
}

// 发送放物品到页面上的消息
void PlayerTrade::SendAddTradeGoodsMes(int nPos)
{
	CMainPlayerHand* pHand = GetGame()->GetMainPlayerHand();
	CPlayer* pPlayer = GetGame()->GetMainPlayer();
	if (pHand==NULL || pPlayer==NULL) 
		return;

	DWORD dwAmount=1;
	CPlayer::tagGoods* pstGoods = pHand->GetTGoodsOfMainPlayerHand();
	if(pstGoods==NULL||pstGoods->pGoods==NULL) 
		return;

	dwAmount = pstGoods->pGoods->GetNum();
	CGUID PlayerGUID = pPlayer->GetExID();
	CGUID GoodsGUID = pstGoods->pGoods->GetExID();

	pHand->SendPutDownMes(TYPE_PLAYER,PlayerGUID,pstGoods->lType,pstGoods->lPos,TYPE_SESSION,
		m_sessionID,m_myPlugID<<8,nPos,TYPE_GOODS,GoodsGUID,dwAmount);
}

// 更新货币交易信息
void PlayerTrade::UpdataTradeInfo(WORD wPriceType,long lPrice,int iWhose)
{
}

// 添加我方要交易的物品（参数2表示在交易页面中的位置）
void PlayerTrade::AddMyTradeGoods(CGoods* pGoods,int iPos)
{
	tagTradeGoods myGoods;
	myGoods.lNum = pGoods->GetNum();
	myGoods.lPos = iPos;
	myGoods.dwIconId = pGoods->GetProperty()->dwIconId;
	myGoods.guid = pGoods->GetExID();
	map<int,tagTradeGoods>::iterator OldGoodsIter = m_myTradeGoodsList.find(iPos);
	if (OldGoodsIter!=m_myTradeGoodsList.end())
	{
		int ipos = -1;
		long lExtendID = 0;
		CGoods* pOldGoods = GetGame()->GetMainPlayer()->GetGoodsByGUID(OldGoodsIter->second.guid,&ipos,&lExtendID);
		if (pOldGoods!=NULL)
		{
			if (pOldGoods->IsLocked())
			{
				pOldGoods->UnLock();
			}
			GetGame()->GetMainPlayerHand()->SetMainPlayerHandByGoods(false,pOldGoods->GetNum(),ipos,pOldGoods,lExtendID);
			m_myTradeGoodsList.erase(OldGoodsIter);
		}
	}
	m_myTradeGoodsList[iPos] = myGoods;
	CGoods* pRealGoods = GetGame()->GetMainPlayer()->GetGoodsByGUID(myGoods.guid);
	if (pRealGoods!=NULL&&!pRealGoods->IsLocked())
	{
		pRealGoods->Lock();
	}

	CMainPlayerHand* pHand = GetGame()->GetMainPlayerHand();
	CGoods* pHandGoods = pHand->GetPGoodsOfMainPlayerHand();
	if (pHand!=NULL&&pHandGoods!=NULL&&pHandGoods->GetExID()==pRealGoods->GetExID())
	{
		GetGame()->GetMainPlayerHand()->ClearMainPlayerHand();
	}
	SAFEDELETE(pGoods);
}

// 添加对方要交易的物品（参数2表示在交易页面中的位置）
void PlayerTrade::AddOtherTradeGoods(CGoods *pGoods,int iPos)
{
	CGoods yourGoods(*pGoods);
	m_otherTradeGoodesList.insert(pair<int,CGoods>(iPos,yourGoods));
	SAFEDELETE(pGoods);
}

void PlayerTrade::SetTradePara(const CGUID& sessionID, const CGUID& myID,long myPlugID,
							 const CGUID& otherID,long otherPlugID)
{
	m_sessionID = sessionID;
	m_myID = myID;
	m_otherID = otherID;
	m_myPlugID = myPlugID;
	m_otherPlugID = otherPlugID;
}

void PlayerTrade::SendTradingGoodsMes(void)
{
	if (m_otherTradeGoodesList.empty() && m_myTradeGoodsList.empty()
		/*&&m_Golds==0&&m_YourGold!=NULL&&atol(m_YourGold->GetText())==0 */)
	{
		return;
	}

	CMessage msg(MSG_C2S_PLAYERTRADE_TRADING);
	msg.Add(m_sessionID);
	msg.Add((long)m_myPlugID);
	msg.Add((DWORD)m_otherTradeGoodesList.size());
	for (map<int,CGoods>::iterator its = m_otherTradeGoodesList.begin();
		its!=m_otherTradeGoodesList.end();its++)
	{
		msg.Add((CGUID)its->second.GetExID());
		msg.Add((DWORD)its->second.GetNum());
	}
	msg.Send();

	GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-03.wav");
}

void PlayerTrade::SendChangeTradeGoodsMes(void)
{
	CMessage msg(MSG_C2S_PLAYERTRADE_LOCK);
	msg.Add(m_sessionID);
	msg.Add((long)m_myPlugID);
	msg.Send();

	m_myIsLocked = false;
	m_otherIsLocked = false;
	//UpdatePage();

	GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-03.wav");
}

// 发送确定后准备交易的消息
void PlayerTrade::SendReadyTradeGoodsMes(void)
{
	CMessage msg(MSG_C2S_PLAYERTRADE_LOCK);
	msg.Add(m_sessionID);
	msg.Add((long)m_myPlugID);
	msg.Send();

	m_myIsLocked = true;
	//UpdatePage();

	GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-03.wav");
}

// 发送货币信息
void PlayerTrade::SendCoinMes(void)
{
	long lPlugID = m_myPlugID;
	if(GetGame()->GetMainPlayer()->GetExID() == m_otherID)
	{
		lPlugID = m_otherPlugID;
	}

	CMessage msg(MSG_C2S_PLAYERTRADE_CHANGEMONEY);
	msg.Add((CGUID)m_sessionID);
	msg.Add(lPlugID);
	msg.Add(m_myGolds);
	msg.Send();
}

// 发送移除页面上物品的消息
void PlayerTrade::SendRemoveTradeGoodsMes(int iPos)
{
	tagTradeGoods *pstGoods = &m_myTradeGoodsList[iPos];
	if (pstGoods==NULL)
		return;

	CMessage msg(MSG_C2S_CONTAINER_OBJECT_UNLOCK);
	msg.Add((long)eGWT_PlayerTradeWindow);
	msg.Add(m_sessionID);
	msg.Add((long)m_myPlugID<<8);
	msg.Add((long)iPos);
	msg.Add(pstGoods->guid);
	msg.Send();
}

// 从交易界面中移除物品
void PlayerTrade::RemoveTradeGoods(int nPos,int iWhose)
{
	if (nPos<=0 || nPos>GRIDNUM)
		return;

	if (iWhose==Trade_Me)
	{
		map<int,tagTradeGoods>::iterator its = m_myTradeGoodsList.find(nPos);
		if (its != m_myTradeGoodsList.end())
		{
			m_myTradeGoodsList.erase(its);
			UpdataPlayerTradeItem(nPos,Trade_Me);
		}
	}
	else if (iWhose==Trade_Other)
	{
		map<int,CGoods>::iterator its = m_otherTradeGoodesList.find(nPos);
		if (its != m_otherTradeGoodesList.end())
		{
			// 悬浮框不显示
			//CGoodsDescPageEx* pDescPage = GetGame()->GetCGuiEx()->GetGoodsDescPageEx();
			//if (pDescPage!=NULL&&pDescPage->IsShow())
			//{
			//	CGoods* pShowGoods = pDescPage->GetShowGoods();
			//	if (pShowGoods!=NULL&&pShowGoods==&(its->second))
			//	{
			//		pDescPage->SetIsShow(false);
			//	}
			//}
			m_otherTradeGoodesList.erase(its);
			UpdataPlayerTradeItem(nPos,Trade_Other);
		}
	}
}