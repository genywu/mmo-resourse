

#include "stdafx.h"
#include "GoodsTailorSystem.h"
#include "Player.h"
#include "goods\CGoodsFactory.h"
#include "container\CTestContainer.h"
#include "Message Packaging\CS2CContainerObjectAmountChange.h"

//!-------------------------------------------------------------------------------------------------

//!		按这个配方为玩家制作物品
DWORD	GsOneMakeTable::MakeGoods(CPlayer *pPlayer) const
{
	if(NULL == pPlayer)
		return eGTC_UnknownErr;

	//! 检测材料列表
	vector<tagTestArg_ID_Num>vecArg;
	for (LONG i = 0; i < m_vExpend.size(); ++i)
	{
		if(!pPlayer->FindGoodsByIndex(m_vExpend[i].lGoodsIndex, m_vExpend[i].lGoodsNum))
			return eGTC_ErrStuff;
		tagTestArg_ID_Num tagTestArg;
		tagTestArg._dwGoodsID = m_vExpend[i].lGoodsIndex;
		tagTestArg._dwNum		= m_vExpend[i].lGoodsNum;
		vecArg.push_back(tagTestArg);
	}

	//! 删除测试
	CTestContainer testContainer;
	testContainer.Init(pPlayer);
	vector<tagDelTestResult> vecDelResult;
	BOOL bDelSucceed = testContainer.DelTest(vecArg,&vecDelResult);
	if(!bDelSucceed)
		return eGTC_ErrStuff;

	//! 生成产物
	vector<CGoods*> vAddGoods;
	for (LONG i = 0; i < m_vProduction.size(); ++i)
	{
		//唯一检测	
		CGoodsBaseProperties* pBaseProperty=CGoodsFactory::QueryGoodsBaseProperties(m_vProduction[i].lGoodsIndex);
		if(pBaseProperty && pBaseProperty->IsHasAddonPropertie(GAP_UNIQUE))
		{
			
			if(!pPlayer->IsGoodsUnique(CGoodsFactory::QueryGoodsOriginalName(m_vProduction[i].lGoodsIndex)))
			{
				char* strMsg=CStringReading::LoadString(20,49);
				if(strMsg)
				{
					CMessage msg(MSG_S2C_OTHER_GAMEMESSAGE);
					msg.Add(strMsg);
					msg.Add((long)0);
					msg.Add(CGUID::GUID_INVALID);
					msg.SendToPlayer(pPlayer->GetExID());
				}
				return eGTC_UnknownErr;
			}
		}		


		if(0 >= m_vProduction[i].lOutOdds)
		{
			//! 忽略几率配置错误的生成物
			continue;
		}

		if(0 == (rand() % m_vProduction[i].lOutOdds))
		{
			CGoodsFactory::CreateGoods(m_vProduction[i].lGoodsIndex, m_vProduction[i].lGoodsNum, vAddGoods,1);
		}
	}

	//! 添加测试
	vector<tagTestResult> vecResult;
	BOOL bAddSucced = testContainer.AddTest(vAddGoods,&vecResult);
	if(!bAddSucced)
	{
		for( LONG i = 0; i < vAddGoods.size(); i ++ )
		{
			if( vAddGoods[i] )
				CGoodsFactory::GarbageCollect(&(vAddGoods[i]), 0);
		}
		vAddGoods.clear();
		return eGTC_NoEnoughSpace;
	}

	pPlayer->DelGoodsByTestResult(vecDelResult);
	

	//! 删除材料
	//for (LONG i = 0; i < m_vExpend.size(); ++i)
	//{
	//	if(!pPlayer->DelGoodsByIndex(m_vExpend[i].lGoodsIndex, m_vExpend[i].lGoodsNum))
	//	{
	//		AddErrorLogText("GsOneMakeTable::MakeGoods中发生不该出现的错误，找到的物品[索引%d，数量%d]不能删除!", m_vExpend[i].lGoodsIndex, m_vExpend[i].lGoodsNum);
	//		return eGTC_UnknownErr;
	//	}
	//}


	//! 添加产物到玩家身上
	for( LONG i = 0; i < vAddGoods.size(); i ++ )
	{
		if( vAddGoods[i] )
		{
			pPlayer->AddGoodsWithProtect(vAddGoods[i]);
			GetGameLogInterface()->logT422_goods_tailor_system(pPlayer, vAddGoods[i]);
		}
	}

	if(0 < vAddGoods.size())
		return eGTC_Succeed;
	else
		return eGTC_UnknownErr;
}



//!-------------------------------------------------------------------------------------------------
/*
MSG_S2C_MAKE_BEGIN,										//! 开始一个制作
MSG_S2C_MAKE_RESULT,									//! 发送一个制作结果
*/

GoodsTailorSystem GoodsTailorSystem::c_GoodsTailorSystem;


//! 玩家开启一个物品触发的制作
BOOL GoodsTailorSystem::PlayerMakeBeginByGoods(const CGUID &PlayerGuid, const CGUID &GoodsGuid, LONG lMakeTableID, BOOL bDelGoodsWhenFinish)
{
	const GsOneMakeTable *pGsOneMakeTable = (const GsOneMakeTable*)GoodsTailorSetup::GetInstance().FindOneMakeTable(lMakeTableID);
	if(NULL == pGsOneMakeTable)
	{
		return FALSE;
	}

	map<CGUID, tagMakeInfo>::iterator ite = m_mapMakingPlayer.find(PlayerGuid);
	if(m_mapMakingPlayer.end() == ite)
	{
		m_mapMakingPlayer.insert(pair<CGUID, tagMakeInfo>(PlayerGuid, tagMakeInfo()));
		ite = m_mapMakingPlayer.find(PlayerGuid);
	}

	ite->second.lType				= eMT_FORM_GOODS;
	ite->second.PlayerGuid			= PlayerGuid;
	ite->second.pGsOneMakeTable		= pGsOneMakeTable;

	ite->second.GoodsGuid			= GoodsGuid;
	ite->second.bDelGoodsWhenFinish	= bDelGoodsWhenFinish;

	CMessage msg(MSG_S2C_MAKE_BEGIN);
	msg.Add(GoodsGuid);
	DBWriteSet writeSet;
	msg.GetDBWriteSet(writeSet);
	pGsOneMakeTable->Code(writeSet);

	msg.Add((LONG)eMT_FORM_GOODS);
	msg.SendToPlayer(PlayerGuid, false);

	return TRUE;
}

//! 玩家开启一个NPC触发的制作
BOOL GoodsTailorSystem::PlayerMakeBeginByNpc(const CGUID &PlayerGuid, const CGUID &RegionGuid, LONG lRadius, LONG lMakeTableID)
{
	CPlayer *pPlayer = GetGame()->FindPlayer(PlayerGuid);
	if(NULL == pPlayer || 1 > lRadius)
		return FALSE;

	//! 获得配方
	const GsOneMakeTable *pGsOneMakeTable = (const GsOneMakeTable*)GoodsTailorSetup::GetInstance().FindOneMakeTable(lMakeTableID);
	if(NULL == pGsOneMakeTable)
	{
		return FALSE;
	}

	map<CGUID, tagMakeInfo>::iterator ite = m_mapMakingPlayer.find(PlayerGuid);
	if(m_mapMakingPlayer.end() == ite)
	{
		m_mapMakingPlayer.insert(pair<CGUID, tagMakeInfo>(PlayerGuid, tagMakeInfo()));
		ite = m_mapMakingPlayer.find(PlayerGuid);
	}

	ite->second.lType			= eMT_FROM_NPC;
	ite->second.PlayerGuid		= PlayerGuid;
	ite->second.pGsOneMakeTable	= pGsOneMakeTable;

	ite->second.RegionGuid		= RegionGuid;
	ite->second.fPosX			= pPlayer->GetPosX();
	ite->second.fPosY			= pPlayer->GetPosY();
	ite->second.lRadius			= lRadius;

	CMessage msg(MSG_S2C_MAKE_BEGIN);
	msg.Add(NULL_GUID);
	DBWriteSet writeSet;
	msg.GetDBWriteSet(writeSet);
	pGsOneMakeTable->Code(writeSet);

	msg.Add((LONG)eMT_FORM_GOODS);
	msg.Add((LONG)pPlayer->GetPosX());
	msg.Add((LONG)pPlayer->GetPosY());
	msg.Add(lRadius);
	msg.SendToPlayer(PlayerGuid, false);
	
	return TRUE;
}

//! 玩家请求制作完成
BOOL GoodsTailorSystem::PlayerMakeFinish(const CGUID &PlayerGuid)
{
	map<CGUID, tagMakeInfo>::iterator ite = m_mapMakingPlayer.find(PlayerGuid);
	if(m_mapMakingPlayer.end() == ite)
	{
		return FALSE;
	}

	if (MakeGoods(ite->second))
	{
		m_mapMakingPlayer.erase(ite);

		CMessage msg(MSG_S2C_MAKE_RESULT);
		msg.Add((BYTE)TRUE);
		msg.SendToPlayer(PlayerGuid, false);
		return TRUE;
	}

	m_mapMakingPlayer.erase(ite);
	return FALSE;
}

//! 玩家请求制作取消
BOOL GoodsTailorSystem::PlayerMakeCancel(const CGUID &PlayerGuid)
{
	m_mapMakingPlayer.erase(PlayerGuid);
	return TRUE;
}

//! 进行制作
BOOL GoodsTailorSystem::MakeGoods(const tagMakeInfo& MakeInfo)
{
	CPlayer *pPlayer = GetGame()->FindPlayer(MakeInfo.PlayerGuid);
	if(NULL == pPlayer)
		return FALSE;

	//! 验证合法性
	switch(MakeInfo.lType)
	{
	case eMT_FORM_GOODS:
		{
			CGoods *pGoods = pPlayer->FindGoodsByGuidInPack(MakeInfo.GoodsGuid);
			if(NULL == pGoods)
				return FALSE;
		}
		break;
	case eMT_FROM_NPC:
		{
			//! 地图判断
			if(MakeInfo.RegionGuid != pPlayer->GetRegionGUID())
				return FALSE;
			//! 距离判断
			if(MakeInfo.lRadius < abs(LONG(pPlayer->GetPosX() - MakeInfo.fPosX)) || MakeInfo.lRadius <  abs((LONG)(pPlayer->GetPosY() - MakeInfo.fPosY)))
				return FALSE;
		}
	    break;
	default:
	    return FALSE;
	}

	//! 开始制作
	if(NULL == MakeInfo.pGsOneMakeTable)
	{
		assert(NULL != MakeInfo.pGsOneMakeTable);
		AddErrorLogText("GoodsTailorSystem::MakeGoods中发生不该出现的错误，物品制作配方对象为空!");
		return FALSE;
	}
	DWORD dwResult = MakeInfo.pGsOneMakeTable->MakeGoods(pPlayer);

	//! 处理结果
	if(eGTC_Succeed == dwResult)
	{
		switch(MakeInfo.lType)
		{
		case eMT_FORM_GOODS:
			{
				if(MakeInfo.bDelGoodsWhenFinish)
				{
					if(!pPlayer->DelGoods(MakeInfo.GoodsGuid, 1))
					{
						AddErrorLogText("GoodsTailorSystem::MakeGoods中发生严重错误，物品制作已经完成，但消耗物品没有被删除成功，还有一种可能就是配置的消耗物中包含了触发制作的物品!");
					}
				}
			}
			break;
		}
	}
	else
	{
		CMessage msg(MSG_S2C_MAKE_RESULT);
		msg.Add((BYTE)FALSE);
		msg.Add(dwResult);
		msg.SendToPlayer(pPlayer->GetExID(), false);
	}

	return (eGTC_Succeed == dwResult);
}


bool LogicLogInterface::logT422_goods_tailor_system(CPlayer *pPlayer, CGoods *pGoods)
{
	if(0 == GetGame()->GetSetup()->lUseLogServer) return true;

	if(NULL == m_pLogClient)
		return false;
	if (NULL == pPlayer || NULL == pGoods) return false;

	string strTime;
	GetCurrTimeString(strTime);

	return m_pLogClient->SendLogToServer( 
		422, strTime.c_str(),
		&(pPlayer->GetExID()),	pPlayer->GetName(),
		pGoods->GetBasePropertiesIndex(), &(pGoods->GetExID()), pGoods->GetName()
		);
}