/*
	物品扩展功能管理对象基本定义；
	
	2007-12-11 张科智
*/


#include "StdAfx.h"
#include "GoodsExManage.h"

#include "Player.h"

#include "Goods/CGoods.h"
#include "Goods/CGoodsFactory.h"

#include "MessagePackage/CS2CContainerObjectMove.h"
#include "MessagePackage/CS2CContainerObjectAmountChange.h"

#include "PlayerDepot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//! 要搜索的包袱列表
static long arr_lSearchAim[] = 
{
	CS2CContainerObjectMove::PEI_PACKET,//普通背包
	CS2CContainerObjectMove::PEI_PACK1,	//子背包栏位1
	CS2CContainerObjectMove::PEI_PACK2,	//子背包栏位2
	CS2CContainerObjectMove::PEI_PACK3,	//子背包栏位3
	CS2CContainerObjectMove::PEI_PACK4,	//子背包栏位4
	CS2CContainerObjectMove::PEI_PACK5	//子背包栏位5
};

//! 得到静态实例
GoodsExManage& GoodsExManage::GetInstance(void)
{
	return c_GoodsExManage;
}

//! 静态实例
GoodsExManage GoodsExManage::c_GoodsExManage;


GoodsExManage::GoodsExManage(void)
{
	m_Immobile.fAmendIT = 1.0f;
	m_Immobile.fAmendMA = 1.0f;
	m_Immobile.fAmendMC = 1.0f;
	m_Immobile.fAmendPA = 1.0f;
	m_Immobile.fAmendPC = 1.0f;
}

GoodsExManage::~GoodsExManage(void)
{

}

//! 通过消息获取配置信息，消息类型：MSG_W2S_SERVER_SENDSETUP、数据编号：LOAD_GOODSEX
bool GoodsExManage::InitSetup(BYTE *pData, long &lPos)
{
	if (NULL == pData || 0 > lPos) return false;
	
	return m_GoodsExSetup.Unserialize(pData, lPos);
}

//! 打开客户端物品扩展操作窗口
bool GoodsExManage::OpenGoodsExWindow(CPlayer *pPlayer, tagGoodsExWndSession& GoodsExWndSession)
{
	if (eGWT_End <= GoodsExWndSession.uWndType || 0 == GoodsExWndSession.uRadius || NULL == pPlayer) return false;

	//! 仓库会话不属于物品扩展管理
	if (eGWT_DepotWindow == GoodsExWndSession.uWndType)
	{
		if(!pPlayer->GetPlayerDepot()->CreateSession(GoodsExWndSession.uRadius, GoodsExWndSession.lPosX, GoodsExWndSession.lPosY)) 
			return false;
	}
	else
	{
		//! 判断打开的时候是否合法
		long lRadius = GoodsExWndSession.uRadius;
		//! 若会话半径设置为小于0，表示不作限制
		if (0 < lRadius)
		{
			long lPlayerPosX = pPlayer->GetTileX();
			long lPlayerPosY = pPlayer->GetTileY();

			//! 判断玩家是否在会话范围内
			long lDeltaX	= abs(lPlayerPosX - GoodsExWndSession.lPosX); //! X距离
			long lDeltaY	= abs(lPlayerPosY - GoodsExWndSession.lPosY); //! Y距离

			if (lDeltaX > lRadius || lDeltaY > lRadius)
			{
				return false;
			}
		}

		//! 关闭原有窗口
		map<CGUID, tagGoodsExWndSession>::iterator ite = m_mapOpendGEWnd.find(pPlayer->GetExID());
		if (m_mapOpendGEWnd.end() != ite)
		{
			CloseGoodsExWindow(pPlayer);
		}
		//! 打开请求窗口
		m_mapOpendGEWnd[pPlayer->GetExID()] = GoodsExWndSession;
	}

	CMessage msg(MSG_S2C_GOODS_OPEN_WINDOWS);
	msg.Add(GoodsExWndSession.uWndType);
	msg.Add(GoodsExWndSession.uRadius);
	msg.Add(GoodsExWndSession.lPosX);
	msg.Add(GoodsExWndSession.lPosY);
	msg.SendToPlayer(pPlayer->GetExID());

	return true;
}

//! 关闭客户端物品扩展操作窗口（清除相关会话）
void GoodsExManage::CloseGoodsExWindow(CPlayer *pPlayer)
{
	m_mapOpendGEWnd.erase(pPlayer->GetExID());
	//CMessage msg(MSG_S2C_GOODS_CLOSE_WINDOWS);
	//msg.SendToPlayer(pPlayer->GetExID());
}

//!	验证消息会话存在
bool GoodsExManage::TestSession(const CGUID& guid, eGoodsExWindowType eGoodsWinType)
{
	map<CGUID, tagGoodsExWndSession>::iterator ite = m_mapOpendGEWnd.find(guid);
	if(m_mapOpendGEWnd.end() == ite) return false;

	CPlayer *pPlayer = GetGame()->FindPlayer(guid);
	assert(NULL != pPlayer);
	if(NULL == pPlayer) return false;
	
	long lRadius	= ite->second.uRadius;
	//! 若会话半径设置为小于0，表示不作限制
	if (0 >= lRadius) return true;

	long lPlayerPosX = pPlayer->GetTileX();
	long lPlayerPosY = pPlayer->GetTileY();

	//! 判断玩家是否在会话范围内
	long lDeltaX	= abs(lPlayerPosX - ite->second.lPosX); //! X距离
	long lDeltaY	= abs(lPlayerPosY - ite->second.lPosY); //! Y距离
	

	if (lDeltaX > lRadius || lDeltaY > lRadius)
	{
		m_mapOpendGEWnd.erase(ite);
		return false;
	}

	return (eGoodsWinType == ite->second.uWndType);
}

//! 响应客户端关闭物品扩展窗口
void GoodsExManage::OnClientCloseGoodsExWindow(CMessage *pMsg)
{
	m_mapOpendGEWnd.erase(pMsg->GetPlayerID());
}

//! 在玩家身上找到一个容器
CGoodsContainer* GoodsExManage::GetGoodsContainer(CPlayer *pPlayer, long lContianerId)
{
	switch(lContianerId)
	{
	case CS2CContainerObjectMove::PEI_PACKET:
		return pPlayer -> getOriginPack();
		break;
	//case CS2CContainerObjectMove::PEI_EQUIPMENT:
	//	return &( pPlayer -> m_cEquipment );
	//	break;
	//case CS2CContainerObjectMove::PEI_HAND:
	//	return &( pPlayer -> m_cHand );
	//	break;				
	//case CS2CContainerObjectMove::PEI_DEPOT:										
	//	return &( pPlayer -> m_cDepot);					
	//	break;				
	//case CS2CContainerObjectMove::PEI_PACK:
	//	return pPlayer ->getSubpackContainer();
	//	break;
	case CS2CContainerObjectMove::PEI_PACK1:
		return pPlayer->getSubpackContainer()->GetSubpack(0)->pSubpackContainer;
		break;
	case CS2CContainerObjectMove::PEI_PACK2:
		return pPlayer->getSubpackContainer()->GetSubpack(1)->pSubpackContainer;
		break;
	case CS2CContainerObjectMove::PEI_PACK3:
		return pPlayer->getSubpackContainer()->GetSubpack(2)->pSubpackContainer;
		break;
	case CS2CContainerObjectMove::PEI_PACK4:
		return pPlayer->getSubpackContainer()->GetSubpack(3)->pSubpackContainer;
		break;
	case CS2CContainerObjectMove::PEI_PACK5:
		return pPlayer->getSubpackContainer()->GetSubpack(4)->pSubpackContainer;
		break;
	}
	return NULL;
}

//! 为玩家加锁或者解锁物品
BOOL GoodsExManage::LockGoods(CPlayer *pPlayer, const CGUID& GoodsGuid, BOOL bLock)
{
	static DWORD arrContainerType[] = 
	{
		CS2CContainerObjectMove::PEI_PACKET,
		CS2CContainerObjectMove::PEI_PACK1,
		CS2CContainerObjectMove::PEI_PACK2,
		CS2CContainerObjectMove::PEI_PACK3,
		CS2CContainerObjectMove::PEI_PACK4,
		CS2CContainerObjectMove::PEI_PACK5
	};

	assert(NULL != pPlayer);
	for (LONG i = 0; i < sizeof(arrContainerType) / sizeof(DWORD); ++i)
	{
		CAmountLimitGoodsContainer *pContainer = (CAmountLimitGoodsContainer*)GetGoodsContainer(pPlayer, arrContainerType[i]);
		if(NULL != pContainer)
		{
			CGoods *pGoods = (CGoods*)pContainer->FindIgnoreLock(GoodsGuid);
			if(NULL != pGoods)
			{
				if(bLock)
					pContainer->Lock(pGoods);
				else
					pContainer->Unlock(pGoods);
			}
		}
	}

	return FALSE;
}

//!		在玩家身上指定的位置找到一个物品
CGoods* GoodsExManage::FindGoodsFromPlayer(CPlayer *pPlayer, long lContianerID, long lPos)
{
	CGoodsContainer *pContainer = GetGoodsContainer(pPlayer, lContianerID);

	if( NULL != pContainer )
	{
		return pContainer->GetGoods(lPos);
	}
	return NULL;
}


//! 测试一个物品是否在指定的位置上
CGoods* GoodsExManage::TestGoodsHere(CPlayer *pPlayer, const CGUID &GoodsGuid, long lContianerID, long lPos, BOOL bTestFrost)
{
	CGoods *pGoods = FindGoodsFromPlayer(pPlayer, lContianerID, lPos);

	if( NULL != pGoods )
	{
		if (pGoods->GetExID() == GoodsGuid)
		{
			if(bTestFrost && IsFrost(pGoods))
				return NULL;
			return pGoods;
		}
	}

	return NULL;
}

//!		测试物品放入空间是否足够
long GoodsExManage::TestPutWay(CPlayer *pPlayer, vector<tagGoodsOperate> &vecDelGoods, vector<CGoods*> &vPutGoods)
{
	if (NULL == pPlayer) return false;

	long NullGrilleNum	= 0; //! 空格数

	//! 计算删除的东西空出来的格子
	for (long i = 0; i < (long)vecDelGoods.size(); ++i)
	{
		CGoods			*pGoods		= NULL;
		CGoodsContainer *pContainer = GetGoodsContainer(pPlayer, vecDelGoods[i].lContianerPos);
		if( NULL == pContainer ) return false;
		
		pGoods = pContainer->GetGoods(vecDelGoods[i].lGoodsPos);
		if( NULL == pGoods ) return false;


		vecDelGoods[i].lOperateWay = CS2CContainerObjectMove::OT_DELETE_OBJECT;
		if (!pGoods->HasAddonProperty(GAP_MAXOVERLAP))
		{
			++NullGrilleNum;	
		}
		else
		{
			long lCurrOverlapNum = pGoods->GetAmount();
			if(vecDelGoods[i].lOperateNum == lCurrOverlapNum)
				++NullGrilleNum;
			else 
				vecDelGoods[i].lOperateWay = CS2CContainerObjectMove::OT_CHANGE_AMOUNT;
		}
	}
	
	LONG lCanPutNum = NullGrilleNum;
	for (long i = 0; i < 6; ++i)
	{
		CVolumeLimitGoodsContainer* pCGoodsContainer = (CVolumeLimitGoodsContainer*)GetGoodsContainer(pPlayer, arr_lSearchAim[i]);
		if (NULL == pCGoodsContainer)
			continue;
		
		lCanPutNum += pCGoodsContainer->GetCanPutNum(vPutGoods);
		if (lCanPutNum >= (long)vPutGoods.size())
		{
			return 1;
		}
	}

	return lCanPutNum - (LONG)vPutGoods.size();
}


//!				给玩家添加物品
bool GoodsExManage::AddGoods(CPlayer *pPlayer, vector<CGoods*>& vAddGoods, vector<long>& vOptType, vector<long>& vOptNum)
{
	if(NULL == pPlayer || vAddGoods.size() == 0 || vAddGoods.size() != vOptType.size()) return false;

	for( size_t i = 0; i < vAddGoods.size(); i ++ )
	{
		if( vAddGoods[i] )
		{
			if( vAddGoods[i] )
			{
				vOptNum[i] = vAddGoods[i]->GetAmount();
				BOOL bAdded=pPlayer->AddAndReturnGoodsWithProtect(&vAddGoods[i], TRUE, &vOptType[i]);					
			}
		}
	}
	return true;
}

//!				删除玩家物品
bool GoodsExManage::DelGoods(CPlayer *pPlayer, vector<tagGoodsOperate> &vDelGoods, DWORD dwLogType, const CGUID &optGuid)
{
	if (NULL == pPlayer) return false;
	
	for (long i = 0; i < (long)vDelGoods.size(); ++i)
	{
		//! 找到该物品的容器
		CVolumeLimitGoodsContainer* pCGoodsContainer = (CVolumeLimitGoodsContainer*)GetGoodsContainer(pPlayer, vDelGoods[i].lContianerPos);
		if( NULL != pCGoodsContainer )
		{
			CGoods* pGoods = dynamic_cast<CGoods*>( pCGoodsContainer -> GetGoods(vDelGoods[i].lGoodsPos) );
			assert(NULL != pGoods);
			if (NULL != pGoods)
			{
				long lAddType = 1;

				if( CS2CContainerObjectMove::OT_CHANGE_AMOUNT == vDelGoods[i].lOperateWay)
				{
					//! 改变数量
					lAddType = 2;
					pGoods -> SetAmount( pGoods -> GetAmount() - vDelGoods[i].lOperateNum);

					CS2CContainerObjectAmountChange coacMsg;
					coacMsg.SetSourceContainer( TYPE_PLAYER, pPlayer->GetExID(), vDelGoods[i].lGoodsPos );
					coacMsg.SetSourceContainerExtendID( vDelGoods[i].lContianerPos );
					coacMsg.SetObject( pGoods -> GetType(), pGoods -> GetExID() );
					coacMsg.SetObjectAmount( pGoods -> GetAmount());
					coacMsg.Send( pPlayer->GetExID() );
				}
				else
				{
					//##删除
					CS2CContainerObjectMove comMsg;
					if( pCGoodsContainer -> Remove(pGoods, &comMsg) )
					{
						pPlayer->OnLoseGoods(pGoods);
						comMsg.SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
						comMsg.SetSourceContainerExtendID( vDelGoods[i].lContianerPos );
						comMsg.Send( pPlayer->GetExID() );

						//##删除该物品?
#ifdef _GOODSLOG1_
						char pszGoodsLog[1024]="";			
						char srcGoodsGUID[50]="";
						pGoods->GetExID().tostring(srcGoodsGUID);					
						_snprintf(pszGoodsLog,1024,"[物品回收:GoodsExManage::DelGoods][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
							pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
						CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif
					}
				}

				switch(dwLogType)
				{
				case 380:
					GetLogicLogInterface()->logT380_goods_disassemble_log(pPlayer, pGoods, lAddType, optGuid, FALSE);
					break;
				case 390:
					GetLogicLogInterface()->logT390_goods_synthesize_log(pPlayer, pGoods, lAddType, optGuid, FALSE);
				    break;
				case 400:
					GetLogicLogInterface()->logT400_goods_upgrade_log(pPlayer, optGuid, pGoods, 0, 0);
					break;
				default:
				    break;
				}

				if(1 == lAddType)
				{
#ifdef _GOODSLOG1_
					if(pGoods)
					{
						char pszGoodsLog[1024]="";			
						char srcGoodsGUID[50]="";
						pGoods->GetExID().tostring(srcGoodsGUID);					
						_snprintf(pszGoodsLog,1024,"[物品回收:exmanageBase418][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
							pGoods->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(pGoods->GetBasePropertiesIndex()),srcGoodsGUID,pGoods->GetAmount());
						CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
					}			
#endif	
					CGoodsFactory::GarbageCollect( &pGoods,42 );
				}
			}
			
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//! GoodsExSetup的解码定义，对应编码接口（Serialize）位置：WS Game.cpp
bool GoodsExSetup::Unserialize(BYTE* pStream, long& lOffset)
{
	//! 分解配置
	m_mapDisassemblePrescription.clear();

	long lDpNum = _GetLongFromByteArray(pStream, lOffset);
	for (long i = 0; i < lDpNum; ++i)
	{
		tagDisassemblePrescription DisassemblePrescription;
		_GetBufferFromByteArray(pStream, lOffset, &DisassemblePrescription, sizeof(tagDisassemblePrescription));
		m_mapDisassemblePrescription[DisassemblePrescription.lOutGoodsQuality] = DisassemblePrescription;
	}

	//! 合成配置
	m_mapSynthesizePrescription.clear();

	long lSpNum = _GetLongFromByteArray(pStream, lOffset);
	for (long i = 0; i < lSpNum; ++i)
	{
		tagSynthesizePrescription SynthesizePrescription;
		_GetBufferFromByteArray(pStream, lOffset, &SynthesizePrescription, sizeof(tagSynthesizePrescription));
		m_mapSynthesizePrescription[SynthesizePrescription.lIndex] = SynthesizePrescription;
	}

	//! 武器充能
	m_lReinforceMaxPercent = _GetLongFromByteArray(pStream, lOffset);

	m_vecPowerIni.clear();

	long lPowerIniNum = _GetLongFromByteArray(pStream, lOffset);
	for (long i = 0; i < lPowerIniNum; ++i)
	{
		tagLevelBoundPA LevelBoundPA;
		LevelBoundPA.lMinLevel = _GetLongFromByteArray(pStream, lOffset);
		LevelBoundPA.lMaxLevel = _GetLongFromByteArray(pStream, lOffset);
		long lPANum = _GetLongFromByteArray(pStream, lOffset);
		for (long j = 0; j < lPANum; ++j)
		{
			tagPowerAccessorial PowerAccessorial;
			_GetBufferFromByteArray(pStream, lOffset, &PowerAccessorial, sizeof(tagPowerAccessorial));
			LevelBoundPA.mapPowerAccessorial[PowerAccessorial.lIntensity] = PowerAccessorial;
		}
		m_vecPowerIni.push_back(LevelBoundPA);
	}

	//! 绑定物品索引列表
	long lIndexNum = _GetLongFromByteArray(pStream, lOffset);
	for (long i = 0; i < lIndexNum; ++i)
	{
		m_setBindGoodsIndex.insert(_GetLongFromByteArray(pStream, lOffset));
	}
	//! 解绑物品索引列表
	lIndexNum = _GetLongFromByteArray(pStream, lOffset);
	for (long i = 0; i < lIndexNum; ++i)
	{
		m_setUntieGoodsIndex.insert(_GetLongFromByteArray(pStream, lOffset));
	}
	//! 冻结物品索引列表
	lIndexNum = _GetLongFromByteArray(pStream, lOffset);
	for (long i = 0; i < lIndexNum; ++i)
	{
		m_setFrostGoodsIndex.insert(_GetLongFromByteArray(pStream, lOffset));
	}
	//! 解冻物品索引列表
	lIndexNum = _GetLongFromByteArray(pStream, lOffset);
	for (long i = 0; i < lIndexNum; ++i)
	{
		m_setThawGoodsIndex.insert(_GetLongFromByteArray(pStream, lOffset));
	}

	//! 装备升级配置
	long lUpgradeSetNum = _GetLongFromByteArray(pStream, lOffset);
	for (long i = 0; i < lUpgradeSetNum; ++i)
	{
		tagArmUpgradeSet ArmUpgradeSet;
		ArmUpgradeSet.lMinLimitLevel = _GetLongFromByteArray(pStream, lOffset);
		ArmUpgradeSet.lMaxLimitLevel = _GetLongFromByteArray(pStream, lOffset);

		long lUpgradeSetNum = _GetLongFromByteArray(pStream, lOffset);
		for (long j = 0; j < lUpgradeSetNum; ++j)
		{
			tagUpgradeSet UpgradeSet;
			_GetBufferFromByteArray(pStream, lOffset, &UpgradeSet, sizeof(tagUpgradeSet));
			ArmUpgradeSet.mapUpgradeSet[UpgradeSet.ArmLevel] = UpgradeSet;
		}
		m_vecUpgradeSetList.push_back(ArmUpgradeSet);
	}

	//! 特殊物品配置
	long lExtraStuffNum = _GetLongFromByteArray(pStream, lOffset);
	for (long i = 0; i < lExtraStuffNum; ++i)
	{
		tagExtraStuff ExtraStuff;
		_GetBufferFromByteArray(pStream, lOffset, &ExtraStuff, sizeof(tagExtraStuff));
		m_vecExtraStuff.push_back(ExtraStuff);
	}

	//! 装备升级配置
	long lUpgradeMoneySetNum = _GetLongFromByteArray(pStream, lOffset);
	for (long i = 0; i < lUpgradeMoneySetNum; ++i)
	{
		tagArmUpgradeMoneySet ArmUpgradeMoneySet;
		ArmUpgradeMoneySet.lMinLimitLevel = _GetLongFromByteArray(pStream, lOffset);
		ArmUpgradeMoneySet.lMaxLimitLevel = _GetLongFromByteArray(pStream, lOffset);

		long lUpgradeSetNum = _GetLongFromByteArray(pStream, lOffset);
		for (long j = 0; j < lUpgradeSetNum; ++j)
		{
			long id = _GetLongFromByteArray(pStream, lOffset);
			ArmUpgradeMoneySet.mapUpgradeMoneySet[id] = _GetLongFromByteArray(pStream, lOffset);
			
		}
		m_vecUpgradeMoneySetList.push_back(ArmUpgradeMoneySet);
	}

	m_lUpgradeLimit = _GetLongFromByteArray(pStream, lOffset);

	//! 魔化配置
	{
		//! 装备等级范围设置
		m_ElementSetup.vEquipmentLevelBound.clear();
		//! 魔化升级配置
		m_ElementSetup.vElementBound.clear();
		//! 几率辅助材料配置
		m_ElementSetup.vHelpfulSetup.clear();
		//! 魔化等级对应魔化属性值
		m_ElementSetup.vElementValue.clear();
		//! 元素重置配置
		m_ElementSetup.vResetElement.clear();
		//! 相性重置配置
		m_ElementSetup.vResetXiangXing.clear();


		//! 装备等级范围设置
		long lElementBoundSize = _GetLongFromByteArray(pStream, lOffset);
		for (long i = 0; i < lElementBoundSize; ++i)
		{
			tagElementBound ElementBound;
			long lStuffSize = _GetLongFromByteArray(pStream, lOffset);
			if(0 < lStuffSize)
			{
				ElementBound.vElementStuff.resize(lStuffSize);
				_GetBufferFromByteArray(pStream, lOffset, &ElementBound.vElementStuff[0], sizeof(tagElementStuff) * lStuffSize);
			}
			long lNeedNumSize = _GetLongFromByteArray(pStream, lOffset);
			if(0 < lNeedNumSize)
			{
				ElementBound.vNeedNum.resize(lNeedNumSize);
				_GetBufferFromByteArray(pStream, lOffset, &ElementBound.vNeedNum[0], sizeof(tagNeedNum) * lNeedNumSize);
			}

			m_ElementSetup.vElementBound.push_back(ElementBound);
		}

		//! 魔化升级配置
		long lEquipmentLevelBoundSize = _GetLongFromByteArray(pStream, lOffset);
		if(0 < lEquipmentLevelBoundSize)
		{
			m_ElementSetup.vEquipmentLevelBound.resize(lEquipmentLevelBoundSize);
			_GetBufferFromByteArray(pStream, lOffset, &m_ElementSetup.vEquipmentLevelBound[0], sizeof(tagEquipmentLevelBound) * lEquipmentLevelBoundSize);
		}

		//! 几率辅助材料配置
		long lHelpfulSetupSize = _GetLongFromByteArray(pStream, lOffset);
		for (long i = 0; i < lHelpfulSetupSize; ++i)
		{
			tagHelpfulSetup HelpfulSetup;
			HelpfulSetup.lMinLevel = _GetLongFromByteArray(pStream, lOffset);
			HelpfulSetup.lMaxLevel = _GetLongFromByteArray(pStream, lOffset);
			long lStuffSize = _GetLongFromByteArray(pStream, lOffset);
			if(0 < lStuffSize)
			{
				HelpfulSetup.vHelpfulStuff.resize(lStuffSize);
				_GetBufferFromByteArray(pStream, lOffset, &HelpfulSetup.vHelpfulStuff[0], sizeof(tagHelpfulStuff) * lStuffSize);
			}
			m_ElementSetup.vHelpfulSetup.push_back(HelpfulSetup);
		}

		//! 魔化等级对应魔化属性值
		long lElementValueSize = _GetLongFromByteArray(pStream, lOffset);
		if(0 < lElementValueSize)
		{
			m_ElementSetup.vElementValue.resize(lElementValueSize);
			_GetBufferFromByteArray(pStream, lOffset, &m_ElementSetup.vElementValue[0], sizeof(tagElementValue) * lElementValueSize);
		}

		//! 元素重置配置
		long lResetElementSize = _GetLongFromByteArray(pStream, lOffset);
		for (long i = 0; i < lResetElementSize; ++i)
		{
			tagResetElement ResetElement;
			ResetElement.lMinLevel = _GetLongFromByteArray(pStream, lOffset);
			ResetElement.lMaxLevel = _GetLongFromByteArray(pStream, lOffset);
			long lStuffSize = _GetLongFromByteArray(pStream, lOffset);
			if(0 < lStuffSize)
			{
				ResetElement.vResetStuff.resize(lStuffSize);
				_GetBufferFromByteArray(pStream, lOffset, &ResetElement.vResetStuff[0], sizeof(tagResetStuff) * lStuffSize);
			}
			m_ElementSetup.vResetElement.push_back(ResetElement);
		}

		//! 相性重置配置
		long lResetXiangXingSize = _GetLongFromByteArray(pStream, lOffset);
		for (long i = 0; i < lResetXiangXingSize; ++i)
		{
			tagResetXiangXing ResetXiangXing;
			ResetXiangXing.lMinLevel = _GetLongFromByteArray(pStream, lOffset);
			ResetXiangXing.lMaxLevel = _GetLongFromByteArray(pStream, lOffset);
			_GetBufferFromByteArray(pStream, lOffset, &ResetXiangXing.XiangXingOdds, sizeof(tagXiangXingOdds));
			long lStuffSize = _GetLongFromByteArray(pStream, lOffset);
			if(0 < lStuffSize)
			{
				ResetXiangXing.vResetStuff.resize(lStuffSize);
				_GetBufferFromByteArray(pStream, lOffset, &ResetXiangXing.vResetStuff[0], sizeof(tagResetStuff) * lStuffSize);
			}
			m_ElementSetup.vResetXiangXing.push_back(ResetXiangXing);
		}
	}
	
	return true;
}