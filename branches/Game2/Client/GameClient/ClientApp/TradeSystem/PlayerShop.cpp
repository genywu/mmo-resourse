#include "StdAfx.h"
#include "PlayerShop.h"

#include "../MainPlayerHand.h"
#include "../../Game.h"
#include "../Other/AudioList.h"
#include "../MsgBoxSys/MsgEventManager.h"
#include "../../../UI/GamePage/ChatList/ChatListPage.h"


extern void SetPlayerShopGoodsItemInfo(PlayerShop::tagGoodsItem& tgGoodsItem, int index);

const int PlayerShop::ITEMCOUNTS = 3;

PlayerShop::PlayerShop(void)
{

}

PlayerShop::~PlayerShop(void)
{

}

void PlayerShop::SetShopPara(const CGUID& sessionID, long plugID, const CGUID& masterID)
{
	m_sessionID = sessionID;
	m_plugID = plugID;
	m_masterID = masterID;
}

void PlayerShop::SetCurShopState(int state)
{
	if(state < SET_SHOP || state >= SHOP_STATE)
		return;

	if (m_curShopState==SET_SHOP&&state==OPEN_SHOP)
	{
		m_tradeGolds = 0;
		m_dwTradeWeiMian = 0;
	}
	m_curShopState = state;
	if(m_curShopState == SHOPPING_SHOP)
	{
		m_sellerShopGoodsList.clear();
		m_curGoodsNum = 0;
	}
}

int PlayerShop::GetCurShopState(void)
{
	return m_curShopState;
}

void PlayerShop::SetIsOpenShop(bool b)
{
	m_isOpenShop = b;
}

bool PlayerShop::OnCloseShop(void)
{
	CPlayer* pMainPlayer = GetGame()->GetMainPlayer();
	if (pMainPlayer==NULL) return false;

	if(m_masterID==pMainPlayer->GetExID())
	{
		CMessage msg(MSG_C2S_PLAYERSHOP_CLOSE);
		msg.Add(GetSessionID());
		msg.Add((long)GetPlugID());
		msg.Send();
		if (GetbSetTrue())
		{
			SetbSetTrue(false);
		}
	}
	else
	{
		CMessage msg(MSG_C2S_PLAYERSHOP_QUIT);
		msg.Add(GetSessionID());
		msg.Add((long)GetPlugID());
		msg.Send();
	}
	CloseShop();
	GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-03.wav");
	return true;
}

// 关闭商店界面
void PlayerShop::CloseShop()
{	
	GoodsItemIter GoodsIter = m_sellerShopGoodsList.begin();
	CPlayer* pMainPlayer = GetGame()->GetMainPlayer();
	if (pMainPlayer!=NULL)
	{
		for (;GoodsIter!=m_sellerShopGoodsList.end();GoodsIter++)
		{
			vector<tagTradeGoodsInfo>::iterator GoodsInforIter = GoodsIter->vecTradeGoodsInfo.begin();
			for (;GoodsInforIter!=GoodsIter->vecTradeGoodsInfo.end();GoodsInforIter++)
			{
				CGoods* pAcessGoods = pMainPlayer->GetGoodsByPosition(GoodsInforIter->contentId,GoodsInforIter->index);
				if (pAcessGoods!=NULL)
				{
					if (pAcessGoods->IsLocked())
					{
						pAcessGoods->UnLock();
					}
					pAcessGoods->SetHaveShadowState(false);
					// 更新背包
					//if (pItemPage!=NULL)
					//{
					//	pItemPage->UpdateGoodsShowByCGoods(pAcessGoods);
					//}
				}
			}
		}
	}
	ClearPlayerShopList();
	ClearMyShopList();

	m_curShopState = -1;
	m_curGoodsNum = 0;
	m_tradeGolds = 0;
	m_dwTradeWeiMian = 0;
	m_bSetSure = false;
	m_plugID=0;
	m_isOpenShop = false;
}

// 清除买家存储的店主上架物品列表
void PlayerShop::ClearPlayerShopList()
{
	GoodsItemIter Iter = m_buyerShopGoodsList.begin();
	for (; Iter!=m_buyerShopGoodsList.end(); ++Iter)
	{
		if (Iter->pItemGoods!=NULL)
		{
			SAFEDELETE(Iter->pItemGoods);
		}
	}
	m_buyerShopGoodsList.clear();
}

// 获取最新的个人商店所有物品列表。
void PlayerShop::UpdataPlayerShopGoodsList()
{
	if (m_curShopState<0||m_curShopState>=SHOP_STATE)
		return;

	if (m_curShopState == OPEN_SHOP)
	{
		for (int i = 0;i<(int)m_sellerShopGoodsList.size();i++)
		{
			SetPlayerShopGoodsItemInfo(m_sellerShopGoodsList[i],i);
		}
	}else if (m_curShopState == SHOPPING_SHOP)
	{
		for (int i = 0;i<(int)m_buyerShopGoodsList.size();i++)
		{
			SetPlayerShopGoodsItemInfo(m_buyerShopGoodsList[i],i);
		}
	}else if (m_curShopState == SET_SHOP&&!m_sellerShopGoodsList.empty())
	{
		for (int i = 0;i<(int)m_sellerShopGoodsList.size();i++)
		{
			SetPlayerShopGoodsItemInfo(m_sellerShopGoodsList[i],i);
		}
	}
}

// 个人商店放入物品时给服务器发送消息
//void PlayerShop::SendAddGoodsMes(int nPos,long lPlugID)
//{
//	CPlayer* pPlayer = GetGame()->GetMainPlayer();
//	CMainPlayerHand* pHand = GetGame()->GetMainPlayerHand();
//	if (!pPlayer || !pHand) return;
//
//	CPlayer::tagGoods* pstGoods = pHand->GetTGoodsOfMainPlayerHand();
//	DWORD dwAmount = 1;
//
//	if(pstGoods==NULL || pstGoods->pGoods==NULL) return;
//	dwAmount=pstGoods->pGoods->GetNum();
//
//	pHand->SendPutDownMes(TYPE_PLAYER, pPlayer->GetExID(), 
//		pstGoods->lType, pstGoods->lPos,
//		TYPE_SESSION, m_sessionID, lPlugID,
//		nPos, TYPE_GOODS, pstGoods->pGoods->GetExID(), dwAmount);
//}

/// 发送开店消息
void PlayerShop::SendOpenShopMes(void)
{
	if (m_curShopState==SET_SHOP && !m_sellerShopGoodsList.empty())
	{
		CPlayer* pMainPlayer = GetGame()->GetMainPlayer();
		if (!pMainPlayer) return;

		GoodsItemIter iter = m_sellerShopGoodsList.begin();
		ulong num = 0;
		size_t goodsNum = m_sellerShopGoodsList.size();

		CMessage msg(MSG_C2S_PLAYERSHOP_OPENFORBUSINESS);
		msg.Add(m_sessionID);
		msg.Add(m_plugID);
		msg.Add((long)goodsNum);
		for (; iter!=m_sellerShopGoodsList.end(); ++iter)
		{
			vector<tagTradeGoodsInfo> tempTradeGoods;
			// 预备交易项
			if (iter->isReadyTrade)
			{
				vector<tagTradeGoodsInfo>::iterator Fiter = iter->vecTradeGoodsInfo.begin();
				for (; Fiter!=iter->vecTradeGoodsInfo.end(); ++Fiter)
				{
					CGoods* pReadyGoods = pMainPlayer->GetGoodsByPosition(Fiter->contentId,Fiter->index);
					if (pReadyGoods!=NULL)
					{
						pReadyGoods->SetHaveShadowState(false);
						if (pReadyGoods->IsLocked())
						{
							pReadyGoods->UnLock();
						}
					}
				}

				num = iter->readyTradeNum;		//此项物品的预备交易数
				for(int i = 0; num>0; i++)
				{
					if(num <= 0)	break;
					ulong curNum = iter->vecTradeGoodsInfo[i].curNum;
					if(num >= curNum)
						num -= curNum;
					else
					{
						curNum = num;
						num = 0;
					}
					tagTradeGoodsInfo TradeGoodInfor;
					TradeGoodInfor.goodsId = iter->vecTradeGoodsInfo[i].goodsId;
					TradeGoodInfor.contentId = iter->vecTradeGoodsInfo[i].contentId;
					TradeGoodInfor.index = iter->vecTradeGoodsInfo[i].index;
					TradeGoodInfor.curNum = curNum;
					tempTradeGoods.push_back(TradeGoodInfor);
				}		

				msg.Add((WORD)tempTradeGoods.size());
				msg.Add((LONG)iter->goodsId);
				msg.Add((DWORD)iter->tradeType);
				msg.Add((DWORD)iter->oneGroupNum);
				msg.Add((DWORD)iter->groupNum);
				msg.Add((WORD)iter->priceType);
				msg.Add((DWORD)iter->price);
				for (int j = 0;j<(int)tempTradeGoods.size();j++)
				{
					msg.Add((LONG)tempTradeGoods[j].contentId);
					msg.Add((LONG)tempTradeGoods[j].index);
					msg.Add((DWORD)tempTradeGoods[j].curNum);
					CGoods* pLockedGoods = pMainPlayer->GetGoodsByPosition(tempTradeGoods[j].contentId,tempTradeGoods[j].index);
					if (pLockedGoods!=NULL)
					{
						if (!pLockedGoods->IsLocked())
						{
							pLockedGoods->Lock();
						}
						pLockedGoods->SetHaveShadowState(true);
					}
				}
				// 背包 @todo 
				//GetGame()->GetCGuiEx()->GetItemPageEx()->UpdataAllItems();
				//GetGame()->GetCGuiEx()->GetItemPageEx()->UpdataAllSubPack();
			}
			tempTradeGoods.clear();
		}
		msg.Send();
	}
}

/// 发送关闭商店消息
void PlayerShop::SendCloseShopMes(void)
{
	CMessage msg(MSG_C2S_PLAYERSHOP_CLOSEDOWN);
	msg.Add(GetSessionID());
	msg.Add(GetPlugID());
	msg.Send();
}

/// 发送购买消息
void PlayerShop::SendBuyGoodsMes(void)
{
	if (m_curShopState == SHOPPING_SHOP&&!m_buyerShopGoodsList.empty())
	{
		GoodsItemIter iter = m_buyerShopGoodsList.begin();
		CMessage msg(MSG_C2S_PLAYERSHOP_BUYITEM);
		long lGoodsNum = GetBuyGoodsNum();
		msg.Add(m_sessionID);
		msg.Add(m_plugID);
		msg.Add(lGoodsNum);
		for(;iter!=m_buyerShopGoodsList.end();++iter)
		{
			// 预备交易项
			if(iter->isReadyTrade)
			{
				msg.Add((long)(iter->goodsId));
				msg.Add((CGUID)iter->guid);
				msg.Add((ulong)iter->readyTradeNum);	// 该类物品准备交易的总数
			}
		}
		msg.Send();
	}
}

// 发送移除物品消息
//void PlayerShop::SendRemoveGoodsMes(int index)
//{
//	if( m_curShopState!=SET_SHOP || m_sellerShopGoodsList.empty())
//		return;
//	if(index<0 || index>=m_sellerShopGoodsList.size())
//		return;
//	GoodsItemIter it = m_sellerShopGoodsList.begin();
//	for(;it!=m_sellerShopGoodsList.end();++it)
//	{
//		// 将相应项目从列表中删除;
//		DWORD dwAmount = 1;
//		LONG lExtendID = -1;
//		long lpos = -1;
//		CGUID guid = m_sellerShopGoodsList[index].guid;
//		if (CGoodsList::GetEffectVal(m_sellerShopGoodsList[index].goodsId,CGoods::GAP_MAXOVERLAP,1)==0)
//		{
//			lExtendID = m_sellerShopGoodsList[index].vecTradeGoodsInfo[0].contentId;
//			lpos = m_sellerShopGoodsList[index].vecTradeGoodsInfo[0].index;
//		}
//
//		CMessage msg(MSG_C2S_PLAYERSHOP_REMOVE);
//		msg.Add((long)m_sellerShopGoodsList[index].goodsId);
//		msg.Add((LONG)TYPE_SESSION);
//		msg.Add(m_sessionID);
//		msg.Add((LONG)m_plugID<<8);
//		msg.Add((LONG)0);
//
//		msg.Add((LONG)TYPE_PLAYER);
//		msg.Add(GetGame()->GetMainPlayer()->GetExID());
//		msg.Add((LONG)lExtendID);
//		msg.Add((LONG)lpos);
//		msg.Add((LONG)TYPE_GOODS);
//		msg.Add(guid);
//		msg.Add((LONG)dwAmount);
//		msg.Send();
//	
//		return;
//	}
//}

// 获取要买的物品种类个数
long PlayerShop::GetBuyGoodsNum(void)
{
	long lNum = 0;
	if (m_curShopState == SHOPPING_SHOP && !m_buyerShopGoodsList.empty())
	{
		for (int i=0; i<(int)m_buyerShopGoodsList.size(); i++)
		{
			if (m_buyerShopGoodsList[i].isReadyTrade)
			{
				lNum++;
			}
		}
	}
	return lNum;
}

// 添加要贩卖的物品
void PlayerShop::AddMyShopGoods(CGoods *pGoods,int iPos/*在商店中的位置*/,eTradeType eDType,
								DWORD dwGoodsNumPerTeam,DWORD dwTeamNum ,DWORD dwPrice,int iPriceType )
{
	CPlayer* pMainPlayer = GetGame()->GetMainPlayer();
	if (!pGoods || !pMainPlayer || m_masterID!=pMainPlayer->GetExID())
		return;

	// 表示此物品是否为可重叠的
	long bCanLaped = false;
	// 查看此物品是否可重叠
	if(pGoods->HasEffect(GAP_MAXOVERLAP)) 
	{
		bCanLaped = true;
	}
	tagGoodsItem item;
	// 能重叠，如果此物品已存在，则返回，否则统计该类物品在玩家身上的数量，并构造商店物品结构加入到商店列表中去
	if(bCanLaped)
	{
		// 查找商店列表
		for(int i=0; i<(int)m_sellerShopGoodsList.size(); ++i)
		{
			// 已存在，返回
			if(m_sellerShopGoodsList[i].goodsId == pGoods->GetIndex())
				return;
		}

		// 查找玩家物品列表
		int iNum = 0;
		list<CPlayer::tagGoods>::iterator its;
		list<CPlayer::tagGoods>::iterator itend;
		for(int i=0; i<PACK_NUM; ++i)
		{
			list<CPlayer::tagGoods> *pGoodsList = pMainPlayer->GetGoodsList(i);
			if(!pGoodsList || pGoodsList->empty())
				continue;
			// 获得背包物品
			its = pGoodsList->begin();
			itend = pGoodsList->end();
			// 遍历该背包的物品列表
			for(; its != itend; its++)
			{
				// 找到同类物品，物品数累加
				if(pGoods->GetIndex() == its->pGoods->GetIndex()&&!its->pGoods->IsLocked())
				{
					its->pGoods->SetHaveShadowState(true);
					// 背包@todo 
					//GetGame()->GetCGuiEx()->GetItemPageEx()->UpdateGoodsShow(its->lType,its->lPos);
					iNum += its->lNum;
					// 物品的交易信息
					tagTradeGoodsInfo goodsInfo;
					if (i==0)
					{
						goodsInfo.contentId = PEI_PACKET;
					}else
					{
						goodsInfo.contentId = PEI_SUBPACK+i;
					}
					goodsInfo.curNum = its->lNum;
					goodsInfo.goodsId = pGoods->GetIndex();
					goodsInfo.index = its->lPos;
					item.vecTradeGoodsInfo.push_back(goodsInfo);
				}
			}
		}
		item.totalNum = iNum;
	}
	else
	{
		item.totalNum = 1;
		long lExtendID = 0;
		int	 iPos = 0;
		GetGame()->GetMainPlayer()->GetGoodsByGUID(pGoods->GetExID(),&iPos,&lExtendID);
		// 物品的交易信息
		tagTradeGoodsInfo goodsInfo;
		goodsInfo.contentId = lExtendID;
		goodsInfo.curNum = item.totalNum;
		goodsInfo.goodsId = pGoods->GetIndex();
		goodsInfo.index = iPos;
		item.vecTradeGoodsInfo.push_back(goodsInfo);
		pGoods->SetHaveShadowState(true);
		// 背包@todo 
		//GetGame()->GetCGuiEx()->GetItemPageEx()->UpdateGoodsShow(lExtendID,iPos);
	}
	// 构造一个物品结构
	item.goodsIconId = pGoods->GetProperty()->dwIconId;
	item.goodsId = pGoods->GetIndex();
	item.strGoodsName = pGoods->GetBaseProperty()->strName;
	item.guid = pGoods->GetExID();
	item.tradeType = eDType;
	item.oneGroupNum = dwGoodsNumPerTeam;
	item.groupNum = dwTeamNum;
	item.priceType = iPriceType;
	item.price = dwPrice;
	item.pItemGoods = pGoods;
	// 将新增加的物品可获的货币量加到总盈利量中去
	if (item.priceType==PT_Money)
	{
		m_tradeGolds += item.groupNum*item.price;
	}else if (item.priceType==PT_WeiMian)
	{
		m_dwTradeWeiMian += item.groupNum*item.price;
	}
	if (eDType==TT_SINGLE)
	{
		item.readyTradeNum = dwTeamNum;
	}else if (eDType==TT_GROUP)
	{
		item.readyTradeNum = dwGoodsNumPerTeam*dwTeamNum;
	}
	item.isReadyTrade = true;
	if (!bCanLaped)
	{
		item.pItemGoods = pGoods;
	}

	// 临时变量，表示该物品存在于个人商店的位置
	int index = 0;
	// 当前商店中的物品个数，小于玩家能设置的最大物品数，自动将新加入物品设置到个人商店的最后一项
	if(m_curGoodsNum < ITEMCOUNTS)
	{
		index = m_curGoodsNum;
		m_sellerShopGoodsList.push_back(item);
	}
	// 如果此时个人商店的物品数达到上限，用户再往商店添加商品时，则用新的物品替换原来项目位置的物品项
	else
	{
		index = iPos;
		if (index>=ITEMCOUNTS)
		{
			index = ITEMCOUNTS-1;
		}
		ReMoveShopGoods(m_sellerShopGoodsList[index].goodsId,m_sellerShopGoodsList[index].guid);
		// 替换
		m_sellerShopGoodsList.push_back(item);
		index = m_curGoodsNum;
        GetInst(ChatListPage).AddChatWords(AppFrame::GetText("Shop_6")  //您没有更多的位置摆放货物啦！
            , 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
	}

	m_curGoodsNum= (int)(m_sellerShopGoodsList.size());

	// 为个人商店列表设置相关物品信息
	SetPlayerShopGoodsItemInfo(m_sellerShopGoodsList[index],index);

	FireUIEvent("PlayerShop", "UpdateOperBtn");
}

/// 将个人商店中指定序列的物品移出
bool PlayerShop::ReMoveShopGoods(long lGoodsID, CGUID guid)							
{
	CPlayer* pMainPlayer = GetGame()->GetMainPlayer();
	if( pMainPlayer==NULL||m_curShopState!=SET_SHOP || m_sellerShopGoodsList.empty())
		return false;

	// 将相应项目从列表中删除
	GoodsItemIter iter = m_sellerShopGoodsList.begin();
	for (; iter!=m_sellerShopGoodsList.end(); ++iter)
	{
		if (iter->guid==guid)
		{
			vector<tagTradeGoodsInfo>::iterator GoodsInforIter = iter->vecTradeGoodsInfo.begin();
			for (;GoodsInforIter!=iter->vecTradeGoodsInfo.end();GoodsInforIter++)
			{
				CGoods* pAcessGoods = pMainPlayer->GetGoodsByPosition(GoodsInforIter->contentId,GoodsInforIter->index);
				if (pAcessGoods!=NULL)
				{
					if (pAcessGoods->IsLocked())
					{
						pAcessGoods->UnLock();
					}
					pAcessGoods->SetHaveShadowState(false);
					// 背包 @todo
					//pItemPage->UpdateGoodsShowByCGoods(pAcessGoods);
				}
			}
			if (iter->priceType==PT_Money)
			{
				m_tradeGolds -= iter->groupNum*iter->price;
			}
			else if (iter->priceType==PT_WeiMian)
			{
				m_dwTradeWeiMian -= iter->groupNum*iter->price;
			}

			m_sellerShopGoodsList.erase(iter);
			--m_curGoodsNum;

			FireUIEvent("PlayerShop","UpdateOperBtn");
			break;
		}
	}

	UpdataPlayerShopGoodsList();

	return true;
}

// 个人商店中加入店主的上架物品
void PlayerShop::AddPlayerShopGoods(CGoods* pGoods, ulong groupNum, ulong priceType,
									ulong price, ulong tradeType, ulong oneGroupNum)
{
	if (pGoods)
	{
		tagGoodsItem item;
		item.goodsId = pGoods->GetIndex();
		item.pItemGoods = pGoods;
		item.goodsIconId = pGoods->GetProperty()->dwIconId;
		item.price = price;
		item.groupNum = groupNum;
		item.oneGroupNum = oneGroupNum;
		item.tradeType= (eTradeType)tradeType;
		item.strGoodsName = pGoods->GetName();
		item.priceType = (int)priceType;
		item.guid = pGoods->GetExID();
		m_buyerShopGoodsList.push_back(item);
	}
}

// 点击重设按钮时候，取消预备交易项
void PlayerShop::CancelAllTrade()
{
	if(m_curShopState == OPEN_SHOP)
		return;

	CPlayer* pMainPlayer = GetGame()->GetMainPlayer();
	//CItemPageEx* pItemPage = GetGame()->GetCGuiEx()->GetItemPageEx();

	// 恢复页面功能按钮（确定，开始摆摊/购买）
	FireUIEvent("PlayerShop", "UpdateOperBtn");

	vector<tagGoodsItem> *pVecGoodsItem = NULL;
	m_tradeGolds = 0;
	m_dwTradeWeiMian = 0;

	/// 判断当前是否是由正在摆摊状态转成设置状态的
	bool bIsStopSell = false;

	// 设置商店状态
	if(m_curShopState == SET_SHOP)
	{
		pVecGoodsItem = &m_sellerShopGoodsList;
		bIsStopSell = true;
	}
	// 购物状态
	else if(m_curShopState == SHOPPING_SHOP)
	{
		pVecGoodsItem = &m_buyerShopGoodsList;
	}
	if(pVecGoodsItem)
	{
		if(pVecGoodsItem->empty())	return;	

		vector<tagTradeGoodsInfo> vecGoodsInfor;
		map<DWORD,tagGoodsItem>	  mapGoodsItem;

		GoodsItemIter it=pVecGoodsItem->begin();
		for(;it!=pVecGoodsItem->end();it++)
		{
			if (m_curShopState == SET_SHOP)
			{		
				CMainPlayerHand* pHand = GetGame()->GetMainPlayerHand();
				if (pMainPlayer!=NULL)
				{
					vector<tagTradeGoodsInfo>::iterator GoodsIterInfor = it->vecTradeGoodsInfo.begin();
					for (;GoodsIterInfor!=it->vecTradeGoodsInfo.end();GoodsIterInfor++)
					{
						CGoods* pGoods = pMainPlayer->GetGoodsByPosition(GoodsIterInfor->contentId,GoodsIterInfor->index);
						if (pGoods!=NULL&&pGoods->IsLocked())
						{
							pGoods->UnLock();
						}
						if (pHand!=NULL&&pHand->GetPGoodsOfMainPlayerHand()==pGoods)
						{
							pHand->ClearMainPlayerHand();
						}

						if(bIsStopSell)
						{
							tagTradeGoodsInfo temp;
							temp.contentId = GoodsIterInfor->contentId;
							temp.index = GoodsIterInfor->index;
							temp.goodsId = GoodsIterInfor->goodsId;
							temp.curNum = GoodsIterInfor->curNum;

							vecGoodsInfor.push_back(temp);
						}

					}
					//if (pItemPage!=NULL&&!bIsStopSell)
					//{
					//	pItemPage->UpdataAllItems();
					//	pItemPage->UpdataAllSubPack();
					//}
				}

				if(bIsStopSell)
				{
					tagGoodsItem tempGoodsItem;
					tempGoodsItem.goodsId = it->goodsId;
					tempGoodsItem.tradeType = it->tradeType;
					tempGoodsItem.readyTradeNum = it->readyTradeNum;
					tempGoodsItem.oneGroupNum = it->oneGroupNum;
					tempGoodsItem.groupNum = it->groupNum;
					tempGoodsItem.price = it->price;
					tempGoodsItem.priceType = it->priceType;
					mapGoodsItem.insert(pair<ulong,tagGoodsItem>(tempGoodsItem.goodsId,tempGoodsItem));
				}

			}
			else if (m_curShopState == SHOPPING_SHOP)
			{
				(*it).isReadyTrade = false;
				(*it).readyTradeNum = 0;
			}
		}

		if (bIsStopSell&&m_curShopState == SET_SHOP)
		{
			if (pMainPlayer!=NULL&&!vecGoodsInfor.empty())
			{
				m_sellerShopGoodsList.clear();
				m_curGoodsNum = 0;
				vector<tagTradeGoodsInfo>::iterator GoodsInforIter = vecGoodsInfor.begin();
				for (int i = 0;GoodsInforIter!=vecGoodsInfor.end();i++,GoodsInforIter++)
				{
					CGoods* pNeedGoods = pMainPlayer->GetGoodsByPosition(GoodsInforIter->contentId,GoodsInforIter->index);
					if (pNeedGoods!=NULL)
					{
						eTradeType eDType = TT_SINGLE;
						DWORD dwGoodsNumPerTeam = 0;
						DWORD dwTeamNum = 1;
						DWORD dwPrice = 0;
						int	  iPriceType = PT_Money;
						map<DWORD,tagGoodsItem>::iterator mapIter = mapGoodsItem.find(GoodsInforIter->goodsId);
						if(mapIter!=mapGoodsItem.end())
						{
							eDType = mapIter->second.tradeType;
							dwGoodsNumPerTeam = mapIter->second.oneGroupNum;
							dwTeamNum = mapIter->second.groupNum;
							dwPrice = mapIter->second.price;
							iPriceType = mapIter->second.priceType;
						}
						AddMyShopGoods(pNeedGoods,i,eDType,dwGoodsNumPerTeam,dwTeamNum,dwPrice,iPriceType);
						FireUIEvent("PlayerShop", "UpdateMoneyInfo");
					}
				}
				// @todo 背包
				//if (pItemPage!=NULL)
				//{
				//	pItemPage->UpdataAllItems();
				//	pItemPage->UpdataAllSubPack();
				//}
			}
		}
	}
	UpdataPlayerShopGoodsList();
}

//更新店主的物品列表
void PlayerShop::UpdateMyShopList(const CGUID& guid,DWORD dwIndex,long lExtendID,DWORD dwPos,long lNum,int iAddOrSub)
{
	CPlayer* pPlayer = GetGame()->GetMainPlayer();
	if (pPlayer)
	{
		GoodsItemIter iter = m_sellerShopGoodsList.begin();
		for (; iter!=m_sellerShopGoodsList.end(); ++iter)
		{
			if (iter->guid == guid && CGoodsList::GetEffectVal(dwIndex,GAP_MAXOVERLAP,1)==0)
			{
				m_sellerShopGoodsList.erase(iter);
				m_curGoodsNum--;
				return;
			}else if (CGoodsList::GetEffectVal(dwIndex,GAP_MAXOVERLAP,1)!=0)
			{
				vector<tagTradeGoodsInfo>::iterator it = iter->vecTradeGoodsInfo.begin();
				for (;it!=iter->vecTradeGoodsInfo.end();it++)
				{
					if (it->contentId == lExtendID&&it->index == dwPos)
					{
						// 如果此格子的物品数量与传入的变化数量相同，则表示此格子的物品全部被卖掉了
						// 此种物品的总数要减去卖掉的数量
						if (it->curNum==lNum&&iAddOrSub==-1)
						{		
							iter->vecTradeGoodsInfo.erase(it);				
							iter->totalNum -= lNum;
							iter->readyTradeNum -= lNum; 
						}
						// 当传入的数量不等于此格子的数量时，表示这个数量是此格子物品剩下的数量
						// 此种物品的总数要减去被扣掉的物品数
						else if (iAddOrSub==0)
						{
							iter->totalNum -= lNum;
							iter->readyTradeNum -= lNum; 
							it->curNum -= lNum;
						}
						if (iter->readyTradeNum==0)
						{
							it = iter->vecTradeGoodsInfo.begin();
							for (;it!=iter->vecTradeGoodsInfo.end();it++)
							{
								CGoods* pGoods = pPlayer->GetGoodsByPosition(it->contentId,it->index);
								if (pGoods!=NULL)
								{
									// 解除物品的锁定
									if (pGoods->IsLocked())
									{
										pGoods->UnLock();
										// @todo 刷新背包界面的显示
										/*if (pItemPage!=NULL)
										{
											pItemPage->UpdateGoodsShowByCGoods(pGoods);
										}*/
									}
								}
							}
							m_sellerShopGoodsList.erase(iter);
							m_curGoodsNum--;
							// @todo刷新背包界面的显示
							//CItemPageEx* pItemPage = GetGame()->GetCGuiEx()->GetItemPageEx();
							//if (pItemPage!=NULL)
							//{
							//	pItemPage->UpdataAllItems();
							//	pItemPage->UpdataAllSubPack();
							//}
						}
						return;
					}
				}
			}			
		}
	}
}

// 更新店主上架物品数量
void PlayerShop::UpdateSellGoodsNum(const CGUID& guid, ulong index,ulong groupNum)
{
	GoodsItemIter iter = m_sellerShopGoodsList.begin();
	for (; iter!=m_sellerShopGoodsList.end(); ++iter)
	{
		if (iter->guid == guid && CGoodsList::GetEffectVal(index,GAP_MAXOVERLAP,1)==0 ||
			iter->goodsId==index && CGoodsList::GetEffectVal(index,GAP_MAXOVERLAP,1)!=0)
		{
			iter->groupNum = groupNum;
		}		
	}
}

// 处理个人商店打开时，背包中物品被右击的响应功能
bool PlayerShop::OnInputGoods(CPlayer::tagGoods* pstGoods)
{
	if (pstGoods!=NULL&&pstGoods->pGoods!=NULL&&m_curShopState==SET_SHOP&&!m_bSetSure)
	{
		//如果是勋章，不能放入
		if(pstGoods->pGoods->GetProperty()->dwType == GBT_MEDAL)
			return true;

		long lFreezeVal = pstGoods->pGoods->GetEffectVal(GAP_FREEZE,1);
		long lBindVal = pstGoods->pGoods->GetEffectVal(GAP_BIND,1);
		//冻结的物品不能放入个人商店进行交易
		if (lFreezeVal!=eFT_UnFrost&&lFreezeVal!=eFT_CantFrost)
		{
            GetInst(MsgEventManager).PushEvent(Msg_Ok, AppFrame::GetText("Goods_156"));
			pstGoods->pGoods->SetHaveShadowState(false);
			//@todo 背包
			/*if (pItemPage!=NULL)
			{
				pItemPage->UpdateGoodsShowByCGoods(pstGoods->pGoods);
			}*/
			return true;
		}
		//绑定的物品不能放入个人商店进行交易
		if (lBindVal==1||lBindVal==3)
		{
			GetInst(MsgEventManager).PushEvent(Msg_Ok, AppFrame::GetText("Goods_157"));
			pstGoods->pGoods->SetHaveShadowState(false);
			//@todo 背包
			/*if (pItemPage!=NULL)
			{
				pItemPage->UpdateGoodsShowByCGoods(pstGoods->pGoods);
			}*/
			return true;
		}
		// 不可交易的不能放入个人商店中贩卖
		if (pstGoods->pGoods->GetEffectVal(GAP_PARTICULAR_ATTRIBUTE,1)&32)
		{
			GetChatListPage().AddChatWords(AppFrame::GetText("Shop_11")
                , 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
			return true;
		}

		// 打开商品贩卖清单界面
		if (m_curGoodsNum<ITEMCOUNTS)
		{
			OnOrderPageOpen(pstGoods->pGoods,m_curGoodsNum);
			return true;
		}
	}
	return false;
}

///检测当前交易清单中的物品要设置的数据是否都设置完全了，如果没设置完就要提示
bool PlayerShop::IsOrderSetupOK(void)
{
	stCacheIter CacheIter = m_mapCacheGoods.find(m_orderGoods->GetIndex());
	if (CacheIter!=m_mapCacheGoods.end())
	{
		if (CacheIter->second.treadeType==TT_SINGLE)
		{
			if (CacheIter->second.groupNum==0)
			{
                GetChatListPage().AddChatWords(AppFrame::GetText("Shop_22")
                    , 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
				return false;
			}
			else if (CacheIter->second.price<=0)
			{
				GetChatListPage().AddChatWords(AppFrame::GetText("Shop_15")
                    , 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
				return false;
			}
		}
		else if (CacheIter->second.treadeType==TT_GROUP)
		{
			if (CacheIter->second.groupNum==0)
			{
				GetChatListPage().AddChatWords(AppFrame::GetText("Shop_22")
                    , 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
				return false;
			}
			else if (CacheIter->second.oneGroupNum==0)
			{
				GetChatListPage().AddChatWords(AppFrame::GetText("Shop_21"), 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
				return false;
			}
			else if (CacheIter->second.price<=0)
			{
				GetChatListPage().AddChatWords(AppFrame::GetText("Shop_15"), 0xffffffff, 0, 0xff000000,"", eNOTIFYPOS_CENTER);
				return false;
			}
		}
		return true;
	}
	return false;
}

bool PlayerShop::OnShopOrderOK(void)
{
	if (!m_orderGoods) return false;

	stCacheIter CacheIter = m_mapCacheGoods.find(m_orderGoods->GetIndex());
	if (CacheIter==m_mapCacheGoods.end()) return false;

	if (IsOrderSetupOK())
	{
		AddMyShopGoods(m_orderGoods, CacheIter->second.shopPos, CacheIter->second.treadeType,
			CacheIter->second.oneGroupNum, CacheIter->second.groupNum,
			CacheIter->second.price,CacheIter->second.priceType);

		FireUIEvent("PlayerShop", "UpdateMoneyInfo");
		GetGame()->GetAudioList()->PlayEquipmentSound(m_orderGoods->GetIndex());

		FireUIEvent("PlayerShopOrder", "Closed");
		return true;
	}
	return false;
}

bool PlayerShop::OnOrderPageOpen(CGoods* pGoods,int shopPos)
{
	// 叠加物品先查找上架物品列表中是否存在
	if (pGoods->HasEffect(GAP_MAXOVERLAP))
	{
		// 查找商店列表
		for(int i=0; i<(int)m_sellerShopGoodsList.size(); ++i)
		{
			// 已存在，返回
			if(m_sellerShopGoodsList[i].goodsId == pGoods->GetIndex())
				return false;
		}
	}

	/// 将此物品指针暂保存于m_tpGoods
	m_orderGoods = pGoods;

	stCacheIter CacheIter = m_mapCacheGoods.find(pGoods->GetIndex());
	if (CacheIter!=m_mapCacheGoods.end())
	{
		CacheIter->second.shopPos = shopPos;
	}else
	{
		stCacheGoodsInfo _stCache;
		_stCache.shopPos = shopPos;
		m_mapCacheGoods.insert(pair<ulong,stCacheGoodsInfo>(pGoods->GetIndex(),_stCache));
	}

	FireUIEvent("PlayerShopOrder", "Opened");

	return true;
}

/// 根据物品ID在缓存列表中查找物品
PlayerShop::stCacheGoodsInfo* PlayerShop::GetCacheGoodsInfo(ulong goodsID)
{
	if (m_mapCacheGoods.empty()) return NULL;
	
	stCacheIter CacheIter = m_mapCacheGoods.find(goodsID);
	if (CacheIter!=m_mapCacheGoods.end())
		return  &(CacheIter->second);
	return NULL;
}

// 查找上架物品信息
PlayerShop::tagGoodsItem* PlayerShop::FindtagGoods(CGoods *pGoods)
{
	if(!pGoods)	return NULL;
	if(m_curShopState < SET_SHOP || m_curShopState >= SHOP_STATE)
		return NULL;

	// 设置商店状态
	if(m_curShopState == SET_SHOP || m_curShopState == OPEN_SHOP)
	{
		GoodsItemIter it = m_sellerShopGoodsList.begin();
		for(;it!=m_sellerShopGoodsList.end();it++)
		{
			if((*it).pItemGoods == pGoods)
				return &(*it);
		}
	}
	// 在玩家物品列表中查找
	else if(!m_buyerShopGoodsList.empty())
	{
		GoodsItemIter it=m_buyerShopGoodsList.begin();
		for(;it!=m_buyerShopGoodsList.end();it++)
		{
			if((*it).pItemGoods == pGoods)
				return &(*it);
		}
	}
	return NULL;
}

// 判断交易项是否整备完整<此接口只针对买家>
bool PlayerShop::IsTradeItemReady(void)
{
	bool isReady = false;

	if(m_curShopState==SHOPPING_SHOP)
	{
		
		if(m_buyerShopGoodsList.empty())
			return false;
		for(int i=0; i<(int)m_buyerShopGoodsList.size(); ++i)
		{
			if(m_buyerShopGoodsList[i].readyTradeNum > 0)
				isReady = m_buyerShopGoodsList[i].isReadyTrade = true;
			else
				m_buyerShopGoodsList[i].isReadyTrade = false;
		}
	}
	return isReady;
}

// 根据当前准备交易物品项计算交易额
void PlayerShop::CalcTradeGolds(void)
{
	m_tradeGolds =  0;
	m_dwTradeWeiMian = 0;

	// 其它玩家商店状态
	if(!m_buyerShopGoodsList.empty())
	{
		GoodsItemIter it=m_buyerShopGoodsList.begin();
		for(;it!=m_buyerShopGoodsList.end();it++)
		{
			// 是预备交易项
			if((*it).isReadyTrade)
			{
				// 该项目钱币风格为金币
				if((*it).priceType == PT_Money)
				{
					m_tradeGolds += (*it).price * ((*it).readyTradeNum);
					if (m_tradeGolds>=2000000000)
					{
						m_tradeGolds=2000000000;
					}
				}
				else if(it->priceType == PT_WeiMian)
				{
					m_dwTradeWeiMian += (*it).price * ((*it).readyTradeNum);
					if (m_dwTradeWeiMian>=2000000000)
					{
						m_dwTradeWeiMian=2000000000;
					}
				}
			}
		}
	}
}