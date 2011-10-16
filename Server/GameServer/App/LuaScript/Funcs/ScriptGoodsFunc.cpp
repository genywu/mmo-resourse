///
/// @file ScriptGoodsFunc.cpp
/// @brief 参见 ScriptGoodsFunc.h
///
#include "stdafx.h"
#include "../Script.h"
#include "../../Player.h"
#include "../../Goods/CGoodsFactory.h"
#include "../../../../../Public/Setup/GoodsSetup.h"
#include "../../../../../Public/Setup/TradeList.h"
#include "../../../../../Public/Setup/GlobalSetup.h"
#include "../../Listener/CSeekGoodsListener.h"
#include "../../GoodsExManage.h"
#include "../../Container/CTestContainer.h"
#include "../../../../../Public/Module/GUID.h"
#include "../../RgnManager.h"
#include "../../Region.h"
#include "../../Country/CountryParam.h"

bool Script::AddGoods(const char* goodsOriginalName, int goodsAmount)
{
	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;

	// creates goods
	vector<CGoods*> goods;
	uint goodsId = GoodsSetup::QueryGoodsIDByOriginalName(goodsOriginalName);
	CGoodsFactory::CreateGoods(goodsId, goodsAmount, goods, 35);
	if(goods.empty() || goods[0] == NULL)
		return false;

	// adds goods to player
	for(size_t i = 0; i < goods.size(); ++i)
	{
		if(goods[i] != NULL)
			player->AddGoodsWithProtect(goods[i]);
	}
	return true;
}

bool Script::AddArmament(const char* goodsOriginalName, int goodsAmount, int armamentLevel, int evilLevel, const char* playerName)
{
	// finds player
	CPlayer* player = FindPlayer( playerName );
	if(player==NULL)
		return false;

	// creates goods
	vector<CGoods*> goods;
	CGoodsFactory::CreateGoods(GoodsSetup::QueryGoodsIDByOriginalName(goodsOriginalName), goodsAmount, goods, 36);
	if(goods.empty() || goods[0]==NULL)
		return false;

	for(size_t i = 0; i < goods.size(); ++i)
	{
		if(goods[i] == NULL)
			continue;

		CGoodsFactory::SetEquipmentLevel(goods[i], armamentLevel);

		// TEST[未完成]
		GetGame()->UpdateSavePlayer(player, SAVE_DETAIL_ATTRPACKGOODS);

		// adds goods to player
		player->AddGoodsWithProtect(goods[i]);
	}
	return true;
}

int Script::DelGoods(const char* goodsOriginalName, int goodsAmount)
{
	if(goodsOriginalName==NULL || goodsAmount < 1)
		return 0;

	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return 0;

	int deleted=0;

	// deletes goods from original packet
	CSeekGoodsListener seeker;
	seeker.SetTarget(const_cast<char*>(goodsOriginalName));
	player->getOriginPack()->TraversingContainer(&seeker);
	for(size_t i = 0; i < seeker.m_vGoodsID.size(); ++i)
	{
		int deleting = 0;
		if(deleted < goodsAmount)
			deleting = goodsAmount - deleted;
		if(deleting == 0)
			return deleted;
		deleted += player->DeleteGoods(CS2CContainerObjectMove::PEI_PACKET, seeker.m_vGoodsID[i], deleting);
	}

	// deletes goods from equipment packet
	seeker.m_vGoodsID.clear();
	player->m_cEquipment.TraversingContainer(&seeker);
	for(size_t i = 0; i < seeker.m_vGoodsID.size(); ++i)
	{
		int deleting = 0;
		if(deleted < goodsAmount)
			deleting = goodsAmount - deleted;
		if(deleting == 0)
			return deleted;
		deleted += player->DeleteGoods(CS2CContainerObjectMove::PEI_EQUIPMENT, seeker.m_vGoodsID[i], deleting);
	}

	// deletes goods from sub packet
	for(int i=CS2CContainerObjectMove::PEI_PACK1; i<=CS2CContainerObjectMove::PEI_PACK5; ++i)
	{
		if(player->getSubpackContainer()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods)
		{			
			seeker.m_vGoodsID.clear();
			player->getSubpackContainer()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->TraversingContainer(&seeker);
			for(size_t j=0; j<seeker.m_vGoodsID.size(); ++j)
			{
				int deleting=0;
				if(deleted < goodsAmount)
					deleting = goodsAmount - deleted;
				if(deleting==0)
					return deleted;
				deleted += player->DeleteGoods(static_cast<CS2CContainerObjectMove::PLAYER_EXTEND_ID>(i), seeker.m_vGoodsID[j], deleting);
			}
		}
	}
	return deleted;
}

bool Script::AddGoodsToRgn(double x, double y, const char* goodsOriginalName, int goodsAmount)
{
	if (goodsOriginalName==NULL || goodsAmount < 1)
		return false;

	// finds region
	CServerRegion* region = dynamic_cast<CServerRegion*>(m_RunEnv.region);
	if(region==NULL)
		return false;

	// creates goods
	vector<CGoods*> goods;
	CGoodsFactory::CreateGoods(GoodsSetup::QueryGoodsIDByOriginalName(goodsOriginalName), goodsAmount, goods ,37);
	for(size_t i = 0; i < goods.size(); ++i)
	{
		if(goods[i]== NULL)
			continue;

		long startPos = 0;
		long nx, ny;
		startPos = region->GetDropGoodsPos(((long)x), ((long)y), nx, ny, startPos);
		if(startPos == -1)
		{
			for(size_t idx=i; idx<goods.size(); ++idx)
			{
#ifdef _GOODSLOG1_
				if(goods[idx])
				{
					char pszGoodsLog[1024]="";			
					char srcGoodsGUID[50]="";
					goods[idx]->GetExID().tostring(srcGoodsGUID);					
					_snprintf(pszGoodsLog,1024,"[物品回收:goodsFun273][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
						goods[idx]->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(goods[idx]->GetBasePropertiesIndex()),srcGoodsGUID,goods[idx]->GetAmount());
					CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
				}			
#endif	
				CGoodsFactory::GarbageCollect(&goods[idx], 204);
			}
			return false;
		}

		goods[i]->SetPosXY(nx+0.5f, ny+0.5f);
		region->AddObject( goods[i] );

		// 通知每个玩家
		CMessage MsgSend(MSG_S2C_RGN_ADDSHAPE,goods[i]->GetExID());
		MsgSend.Add(goods[i]->GetType());
		MsgSend.Add(goods[i]->GetExID());
		DBWriteSet setWriteDB;
		MsgSend.GetDBWriteSet(setWriteDB);
		goods[i]->SerializeForRgnDisplay(setWriteDB);

		MsgSend.SendToAround(goods[i]);
	}
	return true;
}

bool Script::BindGoods(int containerID, int containerPos, int bindType)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;

	return GoodsExManage::GetInstance().SetGoodsBindProperty_Script(player, containerID, containerPos, bindType) != 0;
}

bool Script::FrostGoods(int containerID, int containerPos, int bindType)
{
	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;
	return GoodsExManage::GetInstance().SetGoodsFrostProperty_Script(player, containerID, containerPos, bindType) != 0;
}

int Script::CheckGoods(const char* goodsOriginalName)
{
	if(goodsOriginalName==NULL)
		return 0;

	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return 0;

	int goodsAmount = 0;	

	// checks the original packet
	CSeekGoodsListener seeker;
	seeker.SetTarget(const_cast<char*>(goodsOriginalName));
	player->getOriginPack()->TraversingContainer(&seeker);
	for(size_t i = 0; i < seeker.m_vGoodsID.size(); ++i)
	{
		CGoods* goods = dynamic_cast<CGoods*>(player->getOriginPack()->Find(TYPE_GOODS, seeker.m_vGoodsID[i]));
		if(goods!=NULL)
			goodsAmount += goods->GetAmount();
	}

	// checks the equipment packet
	seeker.m_vGoodsID.clear();
	player->m_cEquipment.TraversingContainer(&seeker);
	for(size_t i = 0; i < seeker.m_vGoodsID.size(); ++i)
	{
		CGoods* goods = dynamic_cast<CGoods*>(player-> m_cEquipment.Find(TYPE_GOODS, seeker.m_vGoodsID[i]) );
		if(goods!=NULL)
			goodsAmount += goods->GetAmount();
	}

	// checks the sub packet
	for(int i=CS2CContainerObjectMove::PEI_PACK1;i<=CS2CContainerObjectMove::PEI_PACK5;i++)
	{
		if(player->getSubpackContainer()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods==NULL)
			continue;
		seeker.m_vGoodsID.clear();
		player->getSubpackContainer()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->TraversingContainer(&seeker);
		for(size_t j=0; j<seeker.m_vGoodsID.size(); ++j)
		{
			CGoods* goods = dynamic_cast<CGoods*>(player->getSubpackContainer()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->Find(TYPE_GOODS, seeker.m_vGoodsID[j]));
			goodsAmount += goods->GetAmount();
		}
	}	

	return goodsAmount;
}

int Script::CheckGoodsInBank(const char* goodsOriginalName)
{
	if(goodsOriginalName==NULL)
		return 0;

	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return 0;

	int goodsAmount=0;	

	// check the depot
	static ulong depotContainerType[] = 
	{
		eDCT_Primary,
		eDCT_Secondary,
		eDCT_Secondary1,
		eDCT_Secondary2,
		eDCT_Secondary3,
		eDCT_Secondary4
	};
	for (long i = 0; i < sizeof(depotContainerType) / sizeof(ulong); ++i)
	{
		CSeekGoodsListener seeker;
		seeker.SetTarget( const_cast<char*>(goodsOriginalName) );
		CGoodsContainer *container = player->GetPlayerDepot()->FindContainer(depotContainerType[i], true);
		if(container==NULL)
			continue;

		container->TraversingContainer( &seeker );
		for( size_t i = 0; i < seeker.m_vGoodsID.size(); i ++ )
		{
			CGoods* goods = dynamic_cast<CGoods*>(container->Find(TYPE_GOODS, seeker.m_vGoodsID[i]));
			if(goods!=NULL)
				goodsAmount += goods->GetAmount();
		}
	}

	return goodsAmount;
}

bool Script::CheckSpace(const char* goodsOriginalName, int goodsAmount)
{
	if(goodsOriginalName==NULL || goodsAmount<1)
		return false;

	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;

	CGoods* goods = CGoodsFactory::CreateGoods(GoodsSetup::QueryGoodsIDByOriginalName(goodsOriginalName), 38);	
	if(goods==NULL)
		return false;

	goods->SetAmount(goodsAmount);

	CTestContainer testContainer;
	testContainer.Init(player);
	vector<tagTestResult> testResult;	
	bool result = testContainer.AddTest(goods, &testResult) == TRUE;	

#ifdef _GOODSLOG1_
	if(goods)
	{
		char pszGoodsLog[1024]="";			
		char srcGoodsGUID[50]="";
		goods->GetExID().tostring(srcGoodsGUID);					
		_snprintf(pszGoodsLog,1024,"[物品回收:Script::CheckSpace][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
			goods->GetBasePropertiesIndex(),GoodsSetup::QueryGoodsName(goods->GetBasePropertiesIndex()),srcGoodsGUID,goods->GetAmount());
		CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
	}		
#endif

	CGoodsFactory::GarbageCollect(&goods, 51);
	return result;
}

int Script::GetGoodsNum()
{
	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return 0;
	return player->getOriginPack()->GetGoodsAmount();
}

int Script::GetGoodsFree()
{
	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return 0;
	return player->GetTotalEmptySpaceNum();
}

int Script::GetEquipIDByPos(int pos, const CGUID& playerGuid)
{
	// finds player
	CPlayer* pAimPlayer = NULL;
	if(playerGuid==NULL_GUID)
		pAimPlayer = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	else
		pAimPlayer = GetGame()->FindPlayer(playerGuid);
	if(pAimPlayer==NULL)
		return 0;

	// finds goods
	CGoods* goods = pAimPlayer->m_cEquipment.GetGoods(pos);
	if(goods==NULL)
		return 0;

	return static_cast<int>(goods->GetBasePropertiesIndex());    
}

const char* Script::GetEquipNameByPos(int pos, const CGUID& playerGuid)
{
	// finds player
	CPlayer* pAimPlayer = NULL;
	if (playerGuid != NULL_GUID)
		pAimPlayer = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	else
		pAimPlayer = GetGame()->FindPlayer(playerGuid);
	if(pAimPlayer==NULL)
		return NULL;

	// finds goods
	CGoods* goods = pAimPlayer->m_cEquipment.GetGoods(pos);
	if(goods==NULL)
		return NULL;

	return GoodsSetup::QueryGoodsOriginalName(goods->GetBasePropertiesIndex());
}

int Script::GetPlayerGoodsAddonProperty(int containerId, int containerPos, int addonPropertyIdx, int valueId, const char* playerName)
{
	// finds player
	CPlayer* player=FindPlayer(playerName);
	if(player==NULL)
		return 0;

	// finds goods
	CGoods* goods=player->FindGoods(containerId, containerPos);
	if(goods==NULL)
		return 0;

	return goods->GetAddonPropertyValues(static_cast<eGoodsAddonProperties>(addonPropertyIdx), valueId);
}

bool Script::SetPlayerGoodsAddonProperty(int containerId, int containerPos, int addonPropertyIdx, int valueId, int value, const char* playerName)
{
	// finds player
	CPlayer* player = FindPlayer( playerName );
	if(player==NULL)
		return false;

	// finds goods
	CGoods* goods = player->FindGoods(containerId, containerPos);
	if(goods==NULL)
		return false;

	// sets addon property value for goods
	return goods->SetAddonPropertyBaseValues(static_cast<eGoodsAddonProperties>(addonPropertyIdx), valueId, value) == TRUE;
}

int Script::DelPlayerGoods(const char* playerName, const char* goodsOriginalName, int goodsAmount)
{
	if(playerName==NULL || goodsOriginalName==NULL || goodsAmount<1)
		return 0;

	// finds player
	CPlayer* player = GetGame()->FindPlayer(playerName);
	if(player==NULL)
		return 0;

	int deleted=0;

	// deletes goods from player's original packet
	CSeekGoodsListener seeker;
	seeker.SetTarget(goodsOriginalName);
	player->getOriginPack()->TraversingContainer(&seeker);
	for(size_t i = 0; i < seeker.m_vGoodsID.size(); ++i)
	{
		int deleting=0;
		if(deleted < goodsAmount)
			deleting = goodsAmount - deleted;
		if(deleting==0) 
			return deleted;
		deleted += player->DeleteGoods(CS2CContainerObjectMove::PEI_PACKET, seeker.m_vGoodsID[i], deleting);
	}

	// deletes goods from player's equipment packet
	seeker.m_vGoodsID.clear();
	player->m_cEquipment.TraversingContainer(&seeker);
	for(size_t i=0; i<seeker.m_vGoodsID.size(); ++i)
	{
		int deleting=0;
		if(deleted<goodsAmount)
			deleting = goodsAmount - deleted;
		if(deleting==0)
			return deleted;
		deleted += player->DeleteGoods(CS2CContainerObjectMove::PEI_EQUIPMENT, seeker.m_vGoodsID[i], deleting);
	}

	// deletes goods from player's sub packet
	for(int i=CS2CContainerObjectMove::PEI_PACK1; i<=CS2CContainerObjectMove::PEI_PACK5; ++i)
	{
		if(player->getSubpackContainer()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pGoods==NULL)
			continue;

		seeker.m_vGoodsID.clear();
		player->getSubpackContainer()->GetSubpack(i-CS2CContainerObjectMove::PEI_PACK1)->pSubpackContainer->TraversingContainer(&seeker);
		for(size_t j=0; j<seeker.m_vGoodsID.size(); ++j)
		{
			int deleting=0;
			if(deleted<goodsAmount)
				deleting=goodsAmount-deleted;
			if(deleting==0)
				return deleted;
			deleted += player->DeleteGoods(static_cast<CS2CContainerObjectMove::PLAYER_EXTEND_ID>(i), seeker.m_vGoodsID[j], deleting);
		}

	}

	// deletes goods from player's depot
	static int depotContainerType[] = 
	{
		eDCT_Primary,
		eDCT_Secondary,
		eDCT_Secondary1,
		eDCT_Secondary2,
		eDCT_Secondary3,
		eDCT_Secondary4
	};
	for (int i=0; i<sizeof(depotContainerType)/sizeof(int); ++i)
	{
		CGoodsContainer *container = player->GetPlayerDepot()->FindContainer(depotContainerType[i], true);
		if(container==NULL)
			continue;

		seeker.m_vGoodsID.clear();
		container->TraversingContainer(&seeker);
		for(size_t j=0; j<seeker.m_vGoodsID.size(); ++j)
		{
			int deleting=0;
			if(deleted<goodsAmount)
				deleting = goodsAmount-deleted;
			if(deleting==0)
				return deleted;
			deleted += player->DeleteDepotGoods(depotContainerType[i], seeker.m_vGoodsID[j], deleting);
		}
	}

	return deleted;
}

int Script::DelUsedGoods(int goodsAmount)
{
	if(goodsAmount<1)
		return 0;

	// finds player
	CPlayer* player=dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return 0;

	if(m_RunEnv.goodsId == CGUID::GUID_INVALID)
		return 0;

	// deletes goods
	CGoods *goods = player->GetGoodsById_FromPackage(m_RunEnv.goodsId);
	ulong containerId=0, containterPos=0;
	bool found = player->FindGoodsByGuid(m_RunEnv.goodsId, containerId, containterPos) == TRUE;
	if(!found)
		return 0;
	return player->DeleteGoods(
		static_cast<CS2CContainerObjectMove::PLAYER_EXTEND_ID>(containerId), 
		m_RunEnv.goodsId, goodsAmount);
}

int Script::CheckUsedGoods()
{
	// finds player
	CPlayer* player=dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return 0;

	// finds goods
	if(m_RunEnv.goodsId == CGUID::GUID_INVALID)
		return 0;
	CGoods *goods =player->GetGoodsById_FromPackage(m_RunEnv.goodsId);
	if(goods==NULL)
		return 0;

	return goods->GetAmount();
}

int Script::GetUsedGoodsPropertiy(int addonPropertyId, int valueId)
{
	if(valueId != 1 && valueId != 2)
		return -1;

	// finds player
	CPlayer* player=dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return -1;

	if(m_RunEnv.goodsId == CGUID::GUID_INVALID)
		return -1;

	eGoodsAddonProperties gap = 
		static_cast<eGoodsAddonProperties>(addonPropertyId);

	CGoods *goods = player->GetGoodsById_FromPackage(m_RunEnv.goodsId);
	if (goods==NULL)
		return -1;

	int n = goods->GetAddonPropertyValues(gap, valueId);

	// 判断是否有该属性
	if(n==0)
	{
		if (goods->HasAddonPropertyValues(gap))
			return 0;
		else
			return -1;
	}
	else
		return n;
}

bool Script::SetUsedGoodsPropertiy(int addonPropertyId, int valueId, int value)
{
	if(valueId != 1 && valueId != 2)
		return false;

	// finds player
	CPlayer* player=dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;

	if(m_RunEnv.goodsId == CGUID::GUID_INVALID)
		return false;

	eGoodsAddonProperties gap = 
		static_cast<eGoodsAddonProperties>(addonPropertyId);

	CGoods *goods = player->GetGoodsById_FromPackage(m_RunEnv.goodsId);
	if (goods==NULL)
		return false;

	////not implement now
	//long lRet =1;// goods->SetAddonPropertyModifier( gap, 1, lVal );
	//if( lRet )
	//{
	//	// 更新客户端物品属性
	//	vector<uchar> goods;
	//	goods -> SerializeForOldClient( &goods );
	//	CMessage Msg( MSG_S2C_GOODS_UPDATEPROPERTY );
	//	Msg.Add( ((CPlayer*)m_RunEnv.srcShape)->GetExID() );
	//	Msg.Add( goods -> GetExID() );
	//	Msg.Add( static_cast<long>( goods.size() ) );
	//	Msg.AddEx( &goods[0], static_cast<long>( goods.size() ) );
	//	Msg.SendToPlayer( ((CPlayer*)m_RunEnv.srcShape)->GetExID() );
	//	return lRet;
	//}
	return true;
}

void Script::BuyGoods(const char* npcName)
{
	if(npcName==NULL)
		return;

	// finds player
	CPlayer* player=dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return;

	ulong areaId=0;
	CServerRegion* region=dynamic_cast<CServerRegion*>(player->GetFather());
	if(region)
		areaId = region->GetAreaId();

	TradeList::tagTrade *trade = TradeList::GetTradeList(npcName);
	if(trade==NULL)
	{
		player->SetCurrentProgress( CPlayer::PROGRESS_NONE );
		player->SetMoveable();
		return;
	}

	player->ProduceEvent(ET_EventBreak);
	CMessage msg(MSG_S2C_SHOP_OPEN);
	if(m_RunEnv.destShape)
	{
		msg.Add(m_RunEnv.destShape->GetExID());
	}
	else
	{
		CGUID emptyGuid;
		msg.Add(emptyGuid);
	}

	msg.Add(long(trade->shopType));//商店类型
	msg.Add(uchar(trade->tradeType));//类型：金钱交易 OR 以物换物
	msg.Add(uchar(trade->_creditType));//区域声望或组织声望类型 by tanglei 09-9-15
	msg.Add(GlobalSetup::GetSetup() -> fBasePriceRate);//FLOAT
	msg.Add(GlobalSetup::GetSetup() -> fTradeInRate);//FLOAT	

	long num = (long)trade->vecGoods.size();			
	msg.Add(num);//物品数量
	if(trade->tradeType==TradeList::ETT_MONEY || trade->tradeType==TradeList::ETT_SILVER)
	{
		for(int i=0; i< (int)trade->vecGoods.size(); ++i)
		{					
			CGoodsBaseProperties* properties = GoodsSetup::QueryGoodsBaseProperties(
				trade->vecGoods[i].dwGoodsId);
			if(properties==NULL)
				return ;	

			msg.Add(trade->vecGoods[i].dwGoodsId);//获得品ID		
			msg.Add(trade->vecGoods[i].dwGoodsNum);//获得品数量
			msg.Add(trade->vecGoods[i].dwCreditId);//需求声望ID
			msg.Add(trade->vecGoods[i].dwCreditVal);//需求声望值
		}
	}
	else if(trade->tradeType==TradeList::ETT_GOODS)
	{
		//struct tagTradeData
		//{
		//	ulong dwGoodsId;	//物品ID
		//	ulong dwGoodsNum;	//物品数量
		//	ulong dwCreditId;		//所需声望ID
		//	ulong dwCreditVal;		//所需声望值
		//	vector<tagTradeGoods>_vecTreadGoods;//以物易物所需物品列表	
		//};
		for(int i=0; i<(int)trade->vecGoods.size(); ++i)
		{
			TradeList::tagTradeData& tradeData=trade->vecGoods[i];
			msg.Add(tradeData.dwGoodsId);
			msg.Add(tradeData.dwGoodsNum);
			msg.Add(tradeData.dwCreditId);
			msg.Add(tradeData.dwCreditVal);
			msg.Add((long)tradeData._vecTreadGoods.size());
			for(long i=0; i<(long)tradeData._vecTreadGoods.size(); ++i)
			{	
				TradeList::tagTradeGoods& tradeGoods=tradeData._vecTreadGoods[i];
				msg.Add(tradeGoods.dwGoodsChgId);
				msg.Add(tradeGoods.dwChgAmount);
			}
		}
	}
	msg.SendToPlayer(player->GetExID());
	return;
}

int Script::GetGoodsAddonProperty(int goodsId, int propertyId, int valueId)
{
	CGoodsBaseProperties* property=GoodsSetup::QueryGoodsBaseProperties(goodsId);
	if(property==NULL)
		return -1;
	return property->GetAddonPropertyValue(static_cast<eGoodsAddonProperties>(propertyId), valueId);
}

void Script::FullRefine()
{
	// finds player
	CPlayer* player=dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return;

	player->AllEquipmentRefineUpdate(100000);
}

int Script::GetWeaponStr(ulong srcPoint)
{
	CMoveShape *src = (CMoveShape*)(srcPoint);
	if(src==NULL)
		return 0;

	CGoods* equip = src->FindGoods(GBT_EQUIPMENT, EC_E_WEAPON);
	if(equip==NULL)
		return 0;

	return equip->GetAddonPropertyValues(GAP_WEAPON_DAMAGE_LEVEL,1);
}

const char* Script::GetGoodsDisplayName(const char* goodsOriginalName)
{
	if(goodsOriginalName==NULL)
		return NULL;

	// finds player
	CPlayer* player=dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return NULL;

	return GoodsSetup::QueryGoodsName(goodsOriginalName);
}

bool Script::BackToCity()
{
	// finds player
	CPlayer* player=dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return NULL;

	CServerRegion* region = dynamic_cast<CServerRegion*>(player->GetFather());
	if(region==NULL)
		return NULL;

	//摆摊和交易时无效
	if(player->GetCurrentProgress()==CPlayer::PROGRESS_OPEN_STALL 
		|| player->GetCurrentProgress()==CPlayer::PROGRESS_TRADING)
		return NULL;

	//该场景是否允许使用回城符
	if(!region->GetSetup().bRuneStoneIsUsed)
	{
		const char* msg=AppFrame::GetText("42");
		if(msg)
			player->SendSystemInfo(msg);
		return NULL;
	}

	//[9/7/2009 chenxianj]	
	if ((long)player->GetPkValue()>=GlobalSetup::GetSetup()->lPrisonIslandPkValue)					
	{
		//pk值大于18000后，放到流放岛
		player->ChangeRgnToIsland();
		return true;
	}

	//判断玩家PK值
	if(player->GetPkValue()>20)
	{
		static long con_rgn[7]={0,11007,12007,13007,14007,15007,16007};
		uchar country = player->GetCountry();
		if(country>=1 && country<=6)
		{
			long rgnId=con_rgn[country];
			CGUID rgnGUID=NULL_GUID;
			CServerRegion* returnRegion = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, rgnId);
			long ranX=360+::random(10);
			long ranY=318+::random(10);
			
			if(returnRegion)
			{
				rgnGUID=returnRegion->GetExID();																
			}
			player->ChangeRegion(RGN_NORMAL, rgnGUID, ranX, ranY, player->GetDir(), rgnId);
			return true;
		}
		return false;
	}
	else
	{
		long regionId = 0;
		long l = 0;
		long t = 0;
		long r = 0;
		long b = 0;
		long d = -1;
		CGUID rgnGuid=NULL_GUID;

		//场景国家和玩家国家是否一致
		if(region->GetCountry()==player->GetCountry() || region->GetCountry()==7)
		{
			//回.rs配置的场景
			if(region->GetSetup().lRuneStoneRegionID == 0) // 原地
			{
				return 0;
			}
			else if(region->GetSetup().lRuneStoneRegionID == -1) // 回国都
			{
				uchar country = player->GetCountry();
				regionId = GetCountryParam()->GetCountryMainRegionID(country);
				l = GetCountryParam()->GetCountryMainRectLeft(country);
				t = GetCountryParam()->GetCountryMainRectTop(country);
				r = GetCountryParam()->GetCountryMainRectRight(country);
				b = GetCountryParam()->GetCountryMainRectBottom(country);
			}
			else if(region->GetSetup().lRuneStoneRegionID>0) // > 0 切换到该ID场景
			{			
				if(region->GetSetup().bRuneStoneIsUsed)
				{
					regionId=region->GetSetup().lRuneStoneRegionID;
					l=region->GetSetup().lRuneStonePosX1;
					t=region->GetSetup().lRuneStonePosY1;
				}
			}
		}
		else
		{
			//回countryparam.ini
			uchar country = player->GetCountry();
			regionId = GetCountryParam()->GetCountryMainRegionID(country);
			l = GetCountryParam()->GetCountryMainRectLeft(country);
			t = GetCountryParam()->GetCountryMainRectTop(country);
			r = GetCountryParam()->GetCountryMainRectRight(country);
			b = GetCountryParam()->GetCountryMainRectBottom(country);
		}

		CServerRegion* rgn = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(RGN_NORMAL, regionId);
		if(rgn)
			rgnGuid = rgn->GetExID();
		player->ChangeRegion(RGN_NORMAL, rgnGuid, l, t, player->GetDir(), regionId);
		return true;
	}
	return false;
}

bool Script::BeginContainerGoodsOpt(const char* onFinishScript, 
									const char* onPutScript, 
									ulong titleId, 
									ulong buttonId, 
									const char* txt, 
									long radius,
									ulong maxPushNum)
{
	if(onFinishScript==NULL || onPutScript==NULL || txt==NULL)
		return false;

	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player==NULL)
		return false;

	if(!player->GetGoodsAllPurposeOpt().BeginOpt(
		player->GetExID(), titleId, buttonId, 
		txt, onPutScript, onFinishScript, radius, 
		(long)player->GetPosX(), (long)player->GetPosY(), player->GetRegionGUID(), maxPushNum))
	{
		player->GetGoodsAllPurposeOpt().Release();
		return false;
	}
	return true;
}

CGUID Script::GetOptGoodsInContainer(ulong goodsIdx)
{
	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player == NULL)
		return NULL_GUID;

	return player->GetGoodsAllPurposeOpt().GetOptGoodsGuid(goodsIdx - 1);
}

bool Script::PutOptGoodsInContainer(const CGUID& goodsGuid, ulong goodsIdx)
{
	if(goodsGuid==NULL_GUID)
		return false;

	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player == NULL)
		return false;

	GoodsAllPurposeOpt::eOptResult optResult = 
		player->GetGoodsAllPurposeOpt().OnGoodsPut(goodsGuid, goodsIdx - 1);
	if (GoodsAllPurposeOpt::eOR_Succeed == optResult)
		return true;
	if (GoodsAllPurposeOpt::eOR_Lawless == optResult)
		player->GetGoodsAllPurposeOpt().Release();
	return false;
}

bool Script::RemoveOptGoodsInContainer(ulong goodsIdx)
{
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player == NULL)
		return false;

	if(0 == goodsIdx)
		goodsIdx = 0xFFFFFFFF;
	else
		goodsIdx -= 1;

	GoodsAllPurposeOpt::eOptResult optResult = player->GetGoodsAllPurposeOpt().OnGoodsPop(goodsIdx);
	if(GoodsAllPurposeOpt::eOR_Lawless == optResult)
	{
		player->GetGoodsAllPurposeOpt().Release();
		return false;
	}
	return true;
}

bool Script::RefreshContainerTxt(const char* txt)
{
	if(txt==NULL)
		return false;

	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player == NULL)
		return false;

	GoodsAllPurposeOpt::eOptResult optResult = player->GetGoodsAllPurposeOpt().SetTxt(txt);
	if(GoodsAllPurposeOpt::eOR_Lawless == optResult)
	{
		player->GetGoodsAllPurposeOpt().Release();
		return false;
	}
	return true;
}

bool Script::RefreshContainerGold(ulong goldAmount)
{
	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player == NULL)
		return false;

	GoodsAllPurposeOpt::eOptResult optResult = player->GetGoodsAllPurposeOpt().SetGold(goldAmount);
	if (GoodsAllPurposeOpt::eOR_Lawless == optResult)
	{
		player->GetGoodsAllPurposeOpt().Release();
		return false;
	}
	return true;
}

void Script::EndContainerGoodsOpt()
{
	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player == NULL)
		return;

	player->GetGoodsAllPurposeOpt().Release();
}

const char* Script::GetGoodsOriginalName(const CGUID& goodsGuid)
{
	if(NULL_GUID == goodsGuid)
		return NULL;

	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player == NULL)
		return NULL;

	CGoods *goods = player->FindGoodsByGuidInPack(goodsGuid);
	if(NULL == goods)
		return NULL;

	return GoodsSetup::QueryGoodsOriginalName(goods->GetBasePropertiesIndex());
}

long Script::GetGoodsAddonPropertyValues(const CGUID& goodsGuid, long addonPropertyId, long valueId)
{
	if(NULL_GUID == goodsGuid)
		return 0;

	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player == NULL)
		return 0;

	// finds goods
	CGoods *goods = player->FindGoodsByGuidInPack(goodsGuid);
	if(NULL == goods)
		return 0;

	return goods->GetAddonPropertyValues((eGoodsAddonProperties)addonPropertyId, valueId);
}

bool Script::SetGoodsAddonPropertyValues(const CGUID& goodsGuid, long addonPropertyId, long valueId, long newValue)
{
	if(NULL_GUID == goodsGuid)
		return false;

	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player == NULL)
		return false;

	CGoods *goods = player->FindGoodsByGuidInPack(goodsGuid);
	if(NULL == goods)
		return false;

	//! 值的合法判定
	//! 冲等
	if(GAP_WEAPON_LEVEL == addonPropertyId && 1 == valueId)
	{
		newValue = max(newValue, 0);
		newValue = min(newValue, 20);
	}
	//! 魔化
	else if (GAP_PLUG_EVIL_LEVEL == addonPropertyId && 1 == valueId)
	{
		newValue = max(newValue, 0);
		newValue = min(newValue, 10);
	}

	goods->ChangeAttribute((eGoodsAddonProperties)addonPropertyId, newValue, valueId, true);
	CGoodsFactory::UpdateEquipmentAttr(goods);
	goods->UpdateAttribute(player->GetExID());
	return true;
}

bool Script::ResetGoodsStochastic(const CGUID& goodsGuid)
{
	if(NULL_GUID == goodsGuid)
		return false;

	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player == NULL)
		return false;

	// finds goods
	CGoods *goods = player->FindGoodsByGuidInPack(goodsGuid);
	if(NULL == goods)
		return false;

	// set goods
	//goods->EDCalculate();
	//goods->CalculatePKCP();
	//goods->ImmunityUpdate();
	//goods->PierceUpdate();
	//goods->ConstellationUpdate();
	//goods->LuckyUpdate();
	//goods->AddonHpUpdate();
	CGoodsFactory::UpdateEquipmentAttr(goods);
	goods->UpdateAttribute(player->GetExID());
	return true;
}

bool Script::DelGoodsByGuid(const CGUID& goodsGuid)
{
	if(NULL_GUID == goodsGuid)
		return false;

	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player == NULL)
		return false;

	// finds goods
	CGoods *goods = player->FindGoodsByGuidInPack(goodsGuid);
	if(NULL == goods)
		return false;

	return player->DelGoods(goods->GetExID(), goods->GetAmount()) == TRUE;
}

CGUID Script::AddGoodsReturnGuid(const char* goodsOriginalName, uint goodsAmount)
{
	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player == NULL)
		return NULL_GUID;

	// create goods
	vector<CGoods*> goods;
	CGoodsFactory::CreateGoods(GoodsSetup::QueryGoodsIDByOriginalName(goodsOriginalName), goodsAmount, goods, 39);
	if(goods.empty())
		return NULL_GUID;
	if(NULL == goods[0])
		return NULL_GUID;

	// adds goods to player
	for(size_t i = 0; i < goods.size(); ++i)
	{
		if(goods[i]!=NULL)
			player->AddGoodsWithProtect(goods[i]);
	}

	// return goods guid
	return goods[0]->GetExID();
}

int Script::IsFrostGoods(const CGUID& goodsGuid)
{
	if(goodsGuid==NULL_GUID)
		return 0;

	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player == NULL)
		return 0;

	// finds goods
	CGoods *goods = player->FindGoodsByGuidInPack(goodsGuid);
	if(NULL == goods)
		return 0;

	// return frost state
	if(GoodsExManage::GetInstance().IsFrost(goods))
		return 1;
	else
		return 2;
}

const char* Script::GetUsedGoodsOriginalName()
{
	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>(m_RunEnv.srcShape);
	if(player == NULL)
		return NULL;

	// finds goods
	CGoods* goods=player->FindGoodByGuid(m_RunEnv.goodsId);
	if(!goods)
		return NULL;

	// get goods original name
	const char* goodsOriginalName = GoodsSetup::QueryGoodsOriginalName(goods->GetBasePropertiesIndex());
	if(goodsOriginalName==NULL)
		return "";
	else
		return goodsOriginalName;
}

CGUID Script::GetUsedGoodsGuid()
{
	return m_RunEnv.goodsId;
}

bool Script::SetAddonDesc(const CGUID& goodsGuid, const char* txt)
{
	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if(player == NULL)
		return false;

	CGoods *goods = player->FindGoodsByGuidInPack(goodsGuid);
	if(goods == NULL)
		return false;

	if(!goods->HasAddonProperty(GAP_GOODSDESC_RES_ID))
		return false;

	if(!goods->SetScriptAddonDesc(txt))
		return false;

	goods->UpdateScriptAddonDesc(player->GetExID());
	return true;
}

const char* Script::GetAddonDesc(const CGUID& goodsGuid)
{
	// finds player
	CPlayer* player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if(player == NULL)
		return NULL;

	// finds goods
	CGoods* goods = player->FindGoodsByGuidInPack(goodsGuid);
	if(goods == NULL)
		return NULL;

	if(!goods->HasAddonProperty(GAP_GOODSDESC_RES_ID))
		return NULL;

	return goods->GetScriptAddonDesc();
}

long Script::GetUsedEquipProperty( int pidx, int vidx )
{
    if( m_RunEnv.goodsId == NULL_GUID )
    {
        return 0;
    }
    CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
    if( player == NULL )
    {
        return 0;
    }
    CGoods *goods = dynamic_cast<CGoods*>( player->GetEquipmentContainer()->Find( m_RunEnv.goodsId ) );
    if( goods == NULL )
    {
        return 0;
    }

    return goods->GetAddonPropertyValues((eGoodsAddonProperties)pidx, vidx);
}
