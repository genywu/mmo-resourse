// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "StdAfx.h"
#include "CGoods.h"
#include "CGoodsFactory.h"
#include "../GoodsExManage.h"
#include "../Player.h"
#include "../ServerRegion.h"
#include "../LuaScript/Script.h"
#include "../LuaScript/ScriptSys.h"
#include "../../../../Public/Setup/GoodsSetup.h"
#include "../../../../Public/Setup/GlobalSetup.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


map<string,string> CGoodsFactory::s_mapSucceedOrder;
map<string,CGUID> CGoodsFactory::s_mapPendingOrder;
list<string> CGoodsFactory::s_listSuccedOrder;

set<CGoods*> CGoodsFactory::s_mGcGoodsSet;
vector<CGoods*> CGoodsFactory::s_vNoDelGoods;

map<long,set<CGoods*> >CGoodsFactory::s_UsingGoods;

BOOL CGoodsFactory::msNoDelFlag=FALSE;


CGoodsFactory::CGoodsFactory()
{
	// TODO: Add your specialized code here.
}

CGoodsFactory::~CGoodsFactory()
{
	// TODO: Add your specialized code here.
}

void CGoodsFactory::AddGoodsToGCSet(CGoods* pGoods)
{
	if(pGoods)
	{
		s_mGcGoodsSet.insert(pGoods);		
	}
}

void CGoodsFactory::ReleaseGCGoodsSet()
{
	set<CGoods*>::iterator goodsIter=s_mGcGoodsSet.begin();
	for(;goodsIter!=s_mGcGoodsSet.end();goodsIter++)
	{
		if(*goodsIter)
		{
#ifdef _GOODSLOG1_
			char pszGoodsLog[1024]="";			
			char srcGoodsGUID[50]="";
			(*goodsIter)->GetExID().tostring(srcGoodsGUID);					
			_snprintf(pszGoodsLog,1024,"[物品回收:CGoodsFactory::ReleaseGCGoodsSet][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
				(*goodsIter)->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName((*goodsIter)->GetBasePropertiesIndex()),srcGoodsGUID,(*goodsIter)->GetAmount());
			CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif
			CGoodsFactory::GarbageCollect(&(*goodsIter),43);
		}
	}
	s_mGcGoodsSet.clear();
}

void CGoodsFactory::ReleaseNoDelGoods()
{	
	for(size_t i=0;i<s_vNoDelGoods.size();i++)
	{
		OBJ_RELEASE(CGoods, s_vNoDelGoods[i]);
	}
	s_vNoDelGoods.clear();
}

bool CGoodsFactory::IsOrderExist(const char* szOrder)
{
	map<string,string>::iterator orderIt=s_mapSucceedOrder.find(szOrder);
	if(orderIt!=s_mapSucceedOrder.end())
		return true;
	return false;
}

void CGoodsFactory::PushOrder(const char* szOrder,const char* szContent)
{
	map<string,string>::iterator orderIt=s_mapSucceedOrder.find(szOrder);
	if(orderIt==s_mapSucceedOrder.end())
	{
		s_mapSucceedOrder[szOrder]=szContent;
		if(s_listSuccedOrder.size()>=1000)
		{
			list<string>::iterator it=s_listSuccedOrder.begin();
			s_listSuccedOrder.erase(it);
			s_mapSucceedOrder.erase(*it);
			s_listSuccedOrder.push_back(szOrder);
		}		
	}
}

void CGoodsFactory::PushPendingOrder(const char* szOrder,const CGUID& guid)
{
	map<string,CGUID>::iterator orderIt=s_mapPendingOrder.find(szOrder);
	if(orderIt==s_mapPendingOrder.end())
	{
		s_mapPendingOrder[szOrder]=guid;
	}
}

void CGoodsFactory::ErasePendingOrder(const char* szOrder)
{
	s_mapPendingOrder.erase(szOrder);
}

CGUID& CGoodsFactory::GetGUIDByOrderId(const char* szOrder)
{
	map<string,CGUID>::iterator orderIt=s_mapPendingOrder.find(szOrder);
	if(orderIt!=s_mapPendingOrder.end())
		return orderIt->second;
	else
		return CGUID::GUID_INVALID;
}


//从新创建物品的附加属性，保留物品的等级不变
void CGoodsFactory::ReCreateAddonProperties(CGoods* pGoods)
{
	if(pGoods == NULL)	return;
	//得到当前等级
	LONG lCurrentLevel =1;// pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_WEAPON_LEVEL, 1 );
	//清除已有的属性
	pGoods -> m_vAddonProperties.clear();

	CreateAddonProperties(pGoods);
	//还原物品的等级
	if(lCurrentLevel > 0)
		UpgradeEquipment(pGoods,lCurrentLevel);
}

//物品套装属性的生成
void CGoodsFactory::CreateSuitProperties(CGoods* pGoods)
{
	if(pGoods == NULL)	return;
	DWORD dwID = pGoods->GetBasePropertiesIndex();
	CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( dwID );

	vector<CGoodsBaseProperties::tagSuitProperty>* pSuitProVec=pProperties->GetSuitProperties();
	//##判断这些附加属性是否生效(符合出现的概率).			
	for( size_t i = 0; i < pSuitProVec->size(); i ++ )
	{	
		//##准备好该属性的结构.
		CGoodsBaseProperties::tagSuitProperty suitProperties;
		suitProperties.bActived=FALSE;
		suitProperties.gapType				=(*pSuitProVec)[i].gapType;		

		//##获取该属性的值.

		//##处理该属性的值
		for( size_t j = 0; j < ((*pSuitProVec)[i]).vValues.size(); j ++ )
		{
			//##准备好CGoods类值的结构
			CGoodsBaseProperties::tagAddonPropertyValue apvValue;

			apvValue.dwId			= ((*pSuitProVec)[i]).vValues[j].dwId;
			apvValue.lBaseValue		= ((*pSuitProVec)[i]).vValues[j].lBaseValue;
			if(suitProperties.gapType==GAP_GOODS_MAX_DURABILITY)
			{
				if(j==1)
				{
					apvValue.lBaseValue=((*pSuitProVec)[i]).vValues[0].lBaseValue;
				}
			}					
			//##将该属性值添加到该附加属性内.
			suitProperties.vValues.push_back( apvValue );
		}
		//##将该附加属性添加到物品结构内.
		pGoods ->m_vSuitProperties.push_back( suitProperties );
	}	
}


//物品附加属性的生成计算
void CGoodsFactory::CreateAddonProperties(CGoods* pGoods)
{
	if(pGoods == NULL)	return;
	DWORD dwID = pGoods->GetBasePropertiesIndex();
	CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( dwID );
	if( pProperties )
	{
		//##获取所有有效的附加属性
		vector<eGoodsAddonProperties> vOut;
		pProperties -> GetValidAddonProperties( vOut );
		//##判断这些附加属性是否生效(符合出现的概率).			
		for( size_t i = 0; i < vOut.size(); i ++ )
		{	

			//##准备好该属性的结构.
			CGoods::tagAddonProperty apGoodsAddonProperty;
			apGoodsAddonProperty.gapType				= vOut[i];	
			apGoodsAddonProperty.lValues[0]=0;
			apGoodsAddonProperty.lValues[1]=0;

			//##获取该属性的值.
			vector<CGoodsBaseProperties::tagAddonPropertyValue> vValues;
			pProperties -> GetAddonPropertyValues( vOut[i], vValues );
			if(vValues.size()==2)
			{
				apGoodsAddonProperty.lValues[0]=vValues[0].lBaseValue;
				apGoodsAddonProperty.lValues[1]=vValues[1].lBaseValue;
			}
			//##将该附加属性添加到物品结构内.
			pGoods -> m_vAddonProperties.push_back( apGoodsAddonProperty );
		}	
	}
}

//根据物品编号创建物品
CGoods* CGoodsFactory::CreateGoods( DWORD dwID ,long lType)
{
	// 根据编号取得物品的基本属性
	CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( dwID );
	CGoods* pGoods = NULL;
	if( pProperties )
	{
		//##创建好物品,并且设置好初始的属性.
		pGoods = OBJ_CREATE(CGoods);
		pGoods->SetBaseProperties(pProperties);
		pGoods -> SetBasePropertiesIndex( dwID );
		pGoods -> SetName( pProperties -> GetName() );
		pGoods -> SetGoodsName( pProperties -> GetName() );
		
		//pGoods -> SetPrice( pProperties -> GetPrice() );
		//pGoods->SetSilverPrice(pProperties->GetSilverPrice());
		pGoods -> SetGraphicsID( pProperties -> GetIconID(IT_GROUND) );
		pGoods -> SetAmount( 1 );
		//pGoods->SetGoodsBaseType(pProperties->GetGoodsBaseType());

		//产生物品的附加属性
		CreateAddonProperties(pGoods);
		CreateSuitProperties(pGoods);
		pGoods->InitEnchaseHole();
		//pGoods->EDCalculate();
		//pGoods->AtkEDUpdate();
		//pGoods->DefEDUpdate();
		//pGoods->CalculatePKCP(); // 初始PK CP值（增加上限值）

		//pGoods->ImmunityUpdate();
		//pGoods->PierceUpdate();
		//pGoods->ConstellationUpdate();
		//pGoods->LuckyUpdate();
		//pGoods->AddonHpUpdate();

		//##分配编号
		CGUID	guid;
		CGUID::CreateGUID(guid);
		pGoods->SetExID(guid);
	}
	s_UsingGoods[lType].insert(pGoods);
	return pGoods;
}



//##ModelId=41A688EC0109
void CGoodsFactory::Release()
{
	s_mapSucceedOrder.clear();
	s_mapPendingOrder.clear();
	s_listSuccedOrder.clear();
}

//创建指定数量的物品
void CGoodsFactory::CreateGoods(DWORD dwID, DWORD dwAmount, vector<CGoods*>& vOut,long lType)
{	
	//通过编号取得基本属性
	CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( dwID );
	if( pProperties )
	{
		//可叠加的消耗品和不能使用的物品
		if( pProperties -> GetGoodsBaseType() == GBT_CONSUMABLE ||
			pProperties -> GetGoodsBaseType() == GBT_NORMAL ||
			pProperties -> GetGoodsBaseType() == GBT_USELESS )
		{
			DWORD dwRemainedGoods	= dwAmount;
			DWORD dwMaxStackNumber	= GoodsSetup::QueryGoodsMaxStackNumber( dwID );
			while( dwRemainedGoods != 0 )
			{
				CGoods* pGoods = CreateGoods( dwID,lType );
				if( pGoods )
				{
					/*
					*如果需要创建的数量大于物品最大叠加数
					*则分次创建物品 将物品数量设置为最大叠加值
					*/
					if( dwRemainedGoods > dwMaxStackNumber )
					{
						pGoods -> SetAmount( dwMaxStackNumber );
						dwRemainedGoods -= dwMaxStackNumber;
					}
					else 
					{
						pGoods -> SetAmount( dwRemainedGoods );
						dwRemainedGoods = 0;
					}
					vOut.push_back( pGoods );
				}
			}
		}
		else
		{
			for( DWORD i = 0; i < dwAmount; i ++ )
			{
				CGoods* pGoods = CreateGoods( dwID,lType );
				if( pGoods )
				{
					vOut.push_back( pGoods );
				}
			}
		}
	}
}


//##ModelId=41C632940182
CGoods* CGoodsFactory::UnserializeGoods(BYTE* pStream, LONG& lOffset,long lType)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	if( pStream )
	{
		pResult = OBJ_CREATE(CGoods);
		if( pResult == NULL )
		{
			assert( 0 );
			OutputDebugString( "Can't alloc memory for unserialize goods.\n" );
		}
		else 
		{
			pResult -> Unserialize( pStream, lOffset );

			CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( 
				pResult -> GetBasePropertiesIndex() );
			if(! pProperties )
			{
				GarbageCollect( &pResult ,100);
			}
			else
			{
				pResult->SetBaseProperties(pProperties);
			}
		}
	}
	s_UsingGoods[lType].insert(pResult);
	return pResult;
}

//通过ByteArray还原物品
CGoods* CGoodsFactory::UnserializeGoods(DBReadSet& setReadDB,long lType)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;	
	pResult = OBJ_CREATE(CGoods);
	if( pResult == NULL )
	{
		assert( 0 );
		OutputDebugString( "Can't alloc memory for unserialize goods.\n" );
	}
	else 
	{
		pResult -> Unserialize( setReadDB );

		CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( 
			pResult -> GetBasePropertiesIndex() );
		if(! pProperties )
		{
			GarbageCollect( &pResult ,100);
		}
        else
        {
            pResult->SetBaseProperties(pProperties);
        }
	}
	s_UsingGoods[lType].insert(pResult);
	return pResult;
}

//回收物品对象内存
BOOL CGoodsFactory::GarbageCollect(CGoods** ppGoods,long lFlag, BOOL bCollectMedal)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( ppGoods )
	{
		//#ifdef _DEBUG
		//		if( *ppGoods )
		//		{
		//			char szDebug[256];
		//			_snprintf( szDebug, "Goods Address: %d ", reinterpret_cast<int>(ppGoods) );
		//			OutputDebugString( szDebug );
		//			char szGoodsID[40] = {0};
		//			(*ppGoods) -> GetExID().tostring(szGoodsID);
		//			_snprintf( szDebug, "Delete Goods[%s]: id[%s], amount[%d]\n", (*ppGoods) -> GetName(), szGoodsID, (*ppGoods) -> GetAmount() );
		//			OutputDebugString( szDebug );
		//		}
		//#endif
		if(*ppGoods)
		{
			CServerRegion * pRegion = dynamic_cast<CServerRegion*>((*ppGoods)->GetFather());
			if(pRegion && pRegion->FindChildObject((*ppGoods)->GetType(),(*ppGoods)->GetExID()) )
			{
				char pszGUID[50]="";
				(*ppGoods)->GetExID().tostring(pszGUID);
				Log4c::Trace(ROOT_MODULE,FormatText("GS_GOODSFACTORY_0",(*ppGoods)->GetName(),pszGUID));//删除物品(Name:%s,GUID:%s)时,该物品存在场景上
				//pRegion->RemoveObject((*ppGoods));
			}
			if((*ppGoods)->GetGoodsBaseType() == GBT_MEDAL && !bCollectMedal ) 
				return TRUE;
		}
		map<long,set<CGoods*> >::iterator it = s_UsingGoods.begin();
		for(;it != s_UsingGoods.end();it++)
		{
			(*it).second.erase(*ppGoods);
		}
		if(msNoDelFlag)
		{
			s_vNoDelGoods.push_back(*ppGoods);
		}
		else
		{
			OBJ_RELEASE(CGoods, *ppGoods );
		}		
		bResult = TRUE;
	}
	return bResult;
}



//计算物品修理价格
DWORD CGoodsFactory::CalculateRepairPrice(CGoods* pGoods)
{
	DWORD dwGoodsPrice=pGoods->GetPrice();
	if(dwGoodsPrice>0)
	{
		dwGoodsPrice *= GlobalSetup::GetSetup()->dwNpcSellGoldToSilver;
	}
	else
	{
		dwGoodsPrice=pGoods->GetSilverPrice();
	}
	DWORD dwRepairPrice = 0;
	if( pGoods && pGoods -> CanReparied() )
	{
		//获取当前耐久值
		LONG lCurrentDurability	= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAX_DURABILITY, 2 );
		//获取最大耐久值
		LONG lMaxDurability		= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAX_DURABILITY, 1 );
		FLOAT fFactor			= GlobalSetup::GetSetup() -> fRepairFactor;
		FLOAT fRate				= ( lMaxDurability - lCurrentDurability ) / static_cast<FLOAT>( lMaxDurability );
		if( fRate < 0.0f )
		{
			fRate = 0.0f;
		}
		dwRepairPrice = static_cast<DWORD>( dwGoodsPrice * fRate * fFactor );
	}
	return dwRepairPrice;
}

//计算物品升级费用
DWORD CGoodsFactory::CalculateUpgradePrice(CGoods* pGoods)
{
	DWORD dwResult = 0;
	if( pGoods && pGoods -> CanUpgraded() )
	{
		// 每次升级费用＝物品升级价格×（1＋（38－4×lg物品升级价格）/15）pow（物品当前等级）
		LONG lCurrentLevel	=1;// pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_WEAPON_LEVEL, 1 );
		LONG lUpgradePrice	=1;// pGoods -> GetAddonPropertyValues( CGoodsBaseProperties::GAP_GOODS_UPGRADE_PRICE, 1 );
		FLOAT fFactor		= 1 + ( 38 - 4 * log10( static_cast<FLOAT>(lUpgradePrice) ) ) / 15;
		fFactor				= pow( fFactor, lCurrentLevel );
		dwResult			= static_cast<DWORD>( lUpgradePrice * fFactor );
	}
	return dwResult;
}

//计算物品出售价格
DWORD CGoodsFactory::CalculateVendPrice(CGoods *pGoods)
{
	DWORD dwVendPrice = 0;
	if( pGoods && pGoods -> GetPrice() > 0 )
	{
		//##首先获取全局设置
		FLOAT fVendPrice = static_cast<FLOAT>( pGoods -> GetPrice() );

		CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( 
			pGoods -> GetBasePropertiesIndex() );
		if( pProperties )
		{
			if( pProperties -> GetGoodsBaseType() == GBT_EQUIPMENT ) 
			{
				//##获取折旧保值比
				FLOAT fDepreciableRate = GlobalSetup::GetSetup() -> fBasePriceRate;

				//##获取当前/最大耐久
				DWORD dwCurrentDurability	= pGoods -> GetAddonPropertyValues( 
					GAP_GOODS_MAX_DURABILITY, 2 );
				DWORD dwMaxDurability		= pGoods -> GetAddonPropertyValues( 
					GAP_GOODS_MAX_DURABILITY, 1 );

				if( dwMaxDurability == 0 )
				{
					//##如果无法获取到最大耐久,那么就必须避免除0错误
					//	fVendPrice = pGoods -> GetPrice() * fDepreciableRate;
				}
				else 
				{
					if( dwCurrentDurability > dwMaxDurability )
					{
						dwCurrentDurability = dwMaxDurability;
					}
					//##计算折旧后的价格
					fVendPrice = pGoods -> GetPrice() * fDepreciableRate + 
						pGoods -> GetPrice() * ( 1.0f - fDepreciableRate ) * ( dwCurrentDurability / dwMaxDurability );
				}
			}
		}

		//##获取折价率
		FLOAT fTradeInPriceRate = GlobalSetup::GetSetup() -> fTradeInRate;

		dwVendPrice = static_cast<DWORD>( fVendPrice * fTradeInPriceRate );
	}
	return dwVendPrice;
}


//计算物品出售价格
DWORD CGoodsFactory::CalculateVendSilverPrice(CGoods *pGoods)
{
	if(!pGoods)
		return 0;
	DWORD dwSilver=0;
	if(pGoods->GetPrice()>0)
	{
		dwSilver=pGoods->GetPrice() * GlobalSetup::GetSetup()->dwNpcSellGoldToSilver;
	}
	else
	{
		dwSilver=pGoods->GetSilverPrice();
	}

	DWORD dwVendPrice = 0;
	if(dwSilver>0)
	{
		//##首先获取全局设置
		FLOAT fVendPrice =(float)dwSilver;

		CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( 
			pGoods -> GetBasePropertiesIndex() );
		if( pProperties )
		{
			if( pProperties -> GetGoodsBaseType() == GBT_EQUIPMENT ) 
			{
				//##获取折旧保值比
				FLOAT fDepreciableRate = GlobalSetup::GetSetup() -> fBasePriceRate;

				//##获取当前/最大耐久
				DWORD dwCurrentDurability	= pGoods -> GetAddonPropertyValues( 
					GAP_GOODS_MAX_DURABILITY, 2 );
				DWORD dwMaxDurability		= pGoods -> GetAddonPropertyValues( 
					GAP_GOODS_MAX_DURABILITY, 1 );

				if( dwMaxDurability == 0 )
				{
					//##如果无法获取到最大耐久,那么就必须避免除0错误
					//	fVendPrice = pGoods -> GetPrice() * fDepreciableRate;
				}
				else 
				{
					if( dwCurrentDurability > dwMaxDurability )
					{
						dwCurrentDurability = dwMaxDurability;
					}
					//##计算折旧后的价格
					fVendPrice = dwSilver * fDepreciableRate + 
						dwSilver * ( 1.0f - fDepreciableRate ) * ( dwCurrentDurability / dwMaxDurability );
				}
			}
		}

		//##获取折价率
		FLOAT fTradeInPriceRate = GlobalSetup::GetSetup() -> fTradeInRate;

		dwVendPrice = static_cast<DWORD>( fVendPrice * fTradeInPriceRate );
	}
	return dwVendPrice;
	return 0;
}

//物品的修理
BOOL CGoodsFactory::RepairEquipment(CGoods* pGoods)
{
	BOOL bResult = FALSE;
	if( pGoods && pGoods -> CanReparied() )
	{
		//设置物品的当前耐久为最大耐久值
		LONG lCurrentDurability	= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAX_DURABILITY, 2 );
		LONG lMaxDurability		= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAX_DURABILITY, 1 );
		//pGoods -> SetAddonPropertyBaseValues( CGoodsBaseProperties::GAP_GOODS_MAX_DURABILITY, 2, lMaxDurability );
		pGoods->ChangeAttribute( GAP_GOODS_MAX_DURABILITY,lMaxDurability,2,true);
		bResult = TRUE;
	}
	return bResult;
}

//装备消耗
BOOL CGoodsFactory::EquipmentWaste(CGoods* pGoods, BOOL bIsSpecialWaste)
{
	BOOL bResult = FALSE;
	if( pGoods )
	{
		LONG lWeaponWaste = 0;
		//磨损因素
		if( bIsSpecialWaste )
		{
			lWeaponWaste = GlobalSetup::GetSetup() -> lGoodsDurSpecialFray;
		}
		else
		{
			lWeaponWaste = GlobalSetup::GetSetup() -> lGoodsDurFray;
		}

		LONG lCurrentDurability	= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAX_DURABILITY, 2 );
		LONG lMaxDurability		= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAX_DURABILITY, 1 );

		if( lMaxDurability > 0 )
		{
			//更新物品的当前耐久值
			lCurrentDurability -= lWeaponWaste;
            if( lCurrentDurability < 0 )
                lCurrentDurability = 0;
			//pGoods -> SetAddonPropertyBaseValues( CGoodsBaseProperties::GAP_GOODS_MAX_DURABILITY, 2, lCurrentDurability );
			pGoods->ChangeAttribute(GAP_GOODS_MAX_DURABILITY,lCurrentDurability,2,true);
			bResult = TRUE;
		}
	}
	return bResult;
}

//升级装备
BOOL CGoodsFactory::UpgradeEquipment(CGoods* pGoods, LONG lTargetLevel)
{
	return TRUE;
}

//! 改变装备等级（在原有等级基础上改变）
BOOL CGoodsFactory::ChangeEquipmentLevel(CGoods* pEquipment, long lChangeDegree)
{
	if(0 != lChangeDegree && pEquipment && pEquipment -> CanUpgraded() )
	{
		long lOldLevel = pEquipment->GetAddonPropertyValues(GAP_WEAPON_LEVEL, 1);
		lOldLevel += lChangeDegree;
		lOldLevel = (0 > lOldLevel) ? 0 : lOldLevel;

		pEquipment->SetAddonPropertyBaseValues(GAP_WEAPON_LEVEL, 1 , lOldLevel);
		UpdateEquipmentAttr(pEquipment);
		LONG lMaxDurability = pEquipment -> GetAddonPropertyValues( GAP_GOODS_MAX_DURABILITY, 1 );
		pEquipment -> SetAddonPropertyBaseValues( GAP_GOODS_MAX_DURABILITY, 2, lMaxDurability);//当前耐久
		return TRUE;
	}
	return FALSE;
}

//! 设置装备等级
BOOL CGoodsFactory::SetEquipmentLevel(CGoods* pEquipment, LONG lLevel)
{
	if(0 <= lLevel && pEquipment && pEquipment -> CanUpgraded() )
	{
		pEquipment->SetAddonPropertyBaseValues(GAP_WEAPON_LEVEL, 1 , lLevel);
		UpdateEquipmentAttr(pEquipment);
		LONG lMaxDurability = pEquipment -> GetAddonPropertyValues(GAP_GOODS_MAX_DURABILITY, 1 );
		pEquipment -> SetAddonPropertyBaseValues( GAP_GOODS_MAX_DURABILITY, 2, lMaxDurability);//当前耐久
		return TRUE;
	}
	return FALSE;
}

//! 得到装备是否是攻击类
BOOL CGoodsFactory::IsAttEquipment(CGoods* pEquipment)
{
    if(NULL != pEquipment)
    {
		if (pEquipment->GetEquipType()==EC_E_WEAPON ||
			pEquipment->GetEquipType()==EC_E_NECKLACE)
		{
			return TRUE;
		}
    }

	return FALSE;
}


//! 更新装备属性（仅限于物攻、魔攻、物防、魔防）
void CGoodsFactory::UpdateEquipmentAttr(CGoods* pEquipment)
{

	////! 基本公式：[（基础+冲等）*（1+魔化）+卡片+套装]
	//if (NULL == pEquipment) return;

	//pEquipment->DelAddonPropertyNotInBase();

	//

	////! ----------------------------------------物品基础值----------------------------------------
	//CGoodsBaseProperties *pGoodsBaseProperties = GoodsSetup::QueryGoodsBaseProperties(pEquipment->GetBasePropertiesIndex());
	//if (NULL == pGoodsBaseProperties) return;

	//vector<CGoodsBaseProperties::tagAddonPropertyValue> vOutProperty;

	//long lMinPA = 0, lMaxPA = 0, lMA = 0, lPR = 0, lMR = 0, 
	//	lHR = 0, lKR = 0, lMKR = 0, lP = 0, lB = 0, lF = 0, 
	//	lMaxD = 0, lSD = 0, lWDL = 0, lAML = 0; //! 基本属性

	//float fIncrementMinPA = 0,fIncrementMaxPA = 0,fIncrementMA = 0,fIncrementPR = 0,fIncrementMR = 0, 
	//	fIncrementHR = 0, fIncrementKR = 0, fIncrementMKR = 0, fIncrementP = 0, fIncrementB = 0, fIncrementF = 0,
	//	fIncrementMaxD = 0, fIncrementSD = 0, fIncrementWDL = 0, fIncrementAML = 0;//! 当前修正的属性（变化量）

	//bool bHasMinPA = false, bHasMaxPA = false, bHasMA = false, bHasPR = false, bHasMR = false, 
	//	bHasHR = false, bHasKR = false, bHasMKR = false, bHasP = false, bHasB = false, bHasF = false,
	//	bHasMaxD = false, bHasSD = false, bHasWDL = false, bHasAML = false;//! 有否属性

	////! 重置部分属性
	//{
	//	//! 需要重置的属性列表（受冲等和魔化影响的属性不需要在这里重置）
	//	DWORD arrProperties[] = 
	//	{
	//		CGoodsBaseProperties::GAP_CRI_DAMAGE,	//物理暴击伤害
	//		CGoodsBaseProperties::GAP_CRI_DEF,		//物理暴击抗性
	//		CGoodsBaseProperties::GAP_MCRI_DAMAGE,	//魔法暴击伤害
	//		CGoodsBaseProperties::GAP_MCRI_DEF,		//魔法暴击抗性
	//		CGoodsBaseProperties::GAP_MAXHP_CORRECT,	//生命上限修正
	//		CGoodsBaseProperties::GAP_DEDICANT_CORRECT,	//专注修正
	//		CGoodsBaseProperties::GAP_CON_DEF,			//基本体质抗性
	//	};
	//	
	//	//! 开始重置
	//	for (LONG i = 0; i < sizeof(arrProperties) / sizeof(DWORD); ++i)
	//	{
	//		vOutProperty.clear();
	//		pGoodsBaseProperties->GetAddonPropertyValues((eGoodsAddonProperties)arrProperties[i], vOutProperty);
	//		if(0 != vOutProperty.size())
	//			pEquipment->ChangeAttribute(arrProperties[i], vOutProperty[0].lBaseValue, 1, true);	
	//	}
	//}
	//vOutProperty.clear();
	////! 最小攻击力
	//pGoodsBaseProperties->GetAddonPropertyValues(CGoodsBaseProperties::GAP_MIN_ATTACK, vOutProperty);
	//if(0 != vOutProperty.size())
	//{
	//	lMinPA = vOutProperty[0].lBaseValue;
	//	bHasMinPA = true;
	//}
	////! 最大攻击力
	//vOutProperty.clear();
	//pGoodsBaseProperties->GetAddonPropertyValues(CGoodsBaseProperties::GAP_MAX_ATTACK, vOutProperty);
	//if(0 != vOutProperty.size())
	//{
	//	lMaxPA = vOutProperty[0].lBaseValue;
	//	bHasMaxPA = true;
	//}
	////! 魔法攻击
	//vOutProperty.clear();
	//pGoodsBaseProperties->GetAddonPropertyValues(CGoodsBaseProperties::GAP_FUMO_PROPERTY, vOutProperty);
	//if(0 != vOutProperty.size())
	//{
	//	lMA = vOutProperty[0].lBaseValue;
	//	bHasMA = true;
	//}
	////! 物理防御力
	//vOutProperty.clear();
	//pGoodsBaseProperties->GetAddonPropertyValues(CGoodsBaseProperties::GAP_NOR_DEF, vOutProperty);
	//if(0 != vOutProperty.size())
	//{
	//	lPR = vOutProperty[0].lBaseValue;
	//	bHasPR = true;
	//}
	////! 魔法防御
	//vOutProperty.clear();
	//pGoodsBaseProperties->GetAddonPropertyValues(CGoodsBaseProperties::GAP_MAGIC_DEF, vOutProperty);
	//if(0 != vOutProperty.size())
	//{
	//	lMR = vOutProperty[0].lBaseValue;
	//	bHasMR = true;
	//}
	////! 命中
	//vOutProperty.clear();
	//pGoodsBaseProperties->GetAddonPropertyValues(CGoodsBaseProperties::GAP_HIT_RATE_CORRECT, vOutProperty);
	//if(0 != vOutProperty.size())
	//{
	//	lHR = vOutProperty[0].lBaseValue;
	//	bHasHR = true;
	//}
	////! 物理致命一击
	//vOutProperty.clear();
	//pGoodsBaseProperties->GetAddonPropertyValues(CGoodsBaseProperties::GAP_NORATTACK_KILLRATE, vOutProperty);
	//if(0 != vOutProperty.size())
	//{
	//	lKR = vOutProperty[0].lBaseValue;
	//	bHasKR = true;
	//}
	////! 魔法致命一击
	//vOutProperty.clear();
	//pGoodsBaseProperties->GetAddonPropertyValues(CGoodsBaseProperties::GAP_MAGIC_KILLRATE, vOutProperty);
	//if(0 != vOutProperty.size())
	//{
	//	lMKR = vOutProperty[0].lBaseValue;
	//	bHasMKR = true;
	//}
	////! 招架能力
	//vOutProperty.clear();
	//pGoodsBaseProperties->GetAddonPropertyValues(CGoodsBaseProperties::GAP_BLOCK_CORRECT, vOutProperty);
	//if(0 != vOutProperty.size())
	//{
	//	lP = vOutProperty[0].lBaseValue;
	//	bHasP = true;
	//}
	////! 格挡能力
	//vOutProperty.clear();
	//pGoodsBaseProperties->GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARRY_CORRECT, vOutProperty);
	//if(0 != vOutProperty.size())
	//{
	//	lB = vOutProperty[0].lBaseValue;
	//	bHasB = true;
	//}
	////! 闪避能力
	//vOutProperty.clear();
	//pGoodsBaseProperties->GetAddonPropertyValues(CGoodsBaseProperties::GAP_FLEE_CORRECT, vOutProperty);
	//if(0 != vOutProperty.size())
	//{
	//	lF = vOutProperty[0].lBaseValue;
	//	bHasF = true;
	//}
	////! 道具最大耐久值成长值
	//vOutProperty.clear();
	//pGoodsBaseProperties->GetAddonPropertyValues(CGoodsBaseProperties::GAP_GOODS_MAX_DURABILITY, vOutProperty);
	//if(0 != vOutProperty.size())
	//{
	//	lMaxD = vOutProperty[0].lBaseValue;
	//	bHasMaxD = true;
	//}
	////! 盾牌物理防御成长
	//vOutProperty.clear();
	//pGoodsBaseProperties->GetAddonPropertyValues(CGoodsBaseProperties::GAP_SHIELD_DEF, vOutProperty);
	//if(0 != vOutProperty.size())
	//{
	//	lSD = vOutProperty[0].lBaseValue;
	//	bHasSD = true;
	//}
	////! 武器强度成长
	//vOutProperty.clear();
	//pGoodsBaseProperties->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_DAMAGE_LEVEL, vOutProperty);
	//if(0 != vOutProperty.size())
	//{
	//	lWDL = vOutProperty[0].lBaseValue;
	//	bHasWDL = true;
	//}
	////! 冲等追加生命上限
	//vOutProperty.clear();
	//pGoodsBaseProperties->GetAddonPropertyValues(CGoodsBaseProperties::GAP_ADD_MAXHP_LEVEL, vOutProperty);
	//if(0 != vOutProperty.size())
	//{
	//	lAML = vOutProperty[0].lBaseValue;
	//	bHasAML = true;
	//}

	////! ----------------------------------------冲等----------------------------------------
	//long lLevel = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_LEVEL, 1);
	////! 最小攻击修正
	//if(bHasMinPA)
	//{
	//	fIncrementMinPA = lLevel * pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_MIN_ATK_UP, 1) * 0.001f;
	//}
	////! 最大攻击修正
	//if(bHasMaxPA)
	//{
	//	fIncrementMaxPA = lLevel * pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_MAX_ATK_UP, 1) * 0.001f;
	//}
	////! 普通防御修正
	//if(bHasPR)
	//{
	//	fIncrementPR = lLevel * pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_DEF_UP, 1) * 0.001f;
	//}
	////! 魔法攻击修正
	//if(bHasMA)
	//{
	//	fIncrementMA = lLevel * pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_MAGIC_UP, 1) * 0.001f;
	//}
	////! 魔法防御修正
	//if(bHasMR)
	//{
	//	fIncrementMR = lLevel * pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_MAGICDEF_UP, 1) * 0.001f;
	//}
	////! 命中修正成长
	//if(bHasHR)
	//{
	//	fIncrementHR = lLevel * pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_HIT_RATE_UP, 1) * 0.001f;
	//}
	////! 物理致命一击率成长
	//if(bHasKR)
	//{
	//	fIncrementKR = lLevel * pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_KILL_RATE_UP, 1) * 0.001f;
	//}
	////! 魔法致命一击率成长
	//if(bHasMKR)
	//{
	//	fIncrementMKR = lLevel * pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_MAGIC_KILL_RATE_UP, 1) * 0.001f;
	//}
	////! 招架能力修正成长
	//if(bHasP)
	//{
	//	fIncrementP = lLevel * pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARRY_UP, 1) * 0.001f;
	//}
	////! 格挡能力修正成长
	//if(bHasB)
	//{
	//	fIncrementB = lLevel * pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_BLOCK_UP, 1) * 0.001f;
	//}
	////! 闪避能力修正成长
	//if(bHasF)
	//{
	//	fIncrementF = lLevel * pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_FLEE_UP, 1) * 0.001f;
	//}
	////! 道具最大耐久值成长值修正
	//if(bHasMaxD)
	//{
	//	fIncrementMaxD = lLevel * pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_MAXDUR_UP, 1) * 0.001f;
	//}
	////! 盾牌物理防御成长修正								
	//if(bHasSD)
	//{
	//	fIncrementSD = lLevel * pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_SHIELD_DEF_UP, 1) * 0.001f;
	//}
	////! 武器强度成长修正							
	//if(bHasWDL)
	//{
	//	fIncrementWDL = lLevel * pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_INTEN_UP, 1) * 0.001f;
	//}
	////! 冲等追加生命上限成长修正							
	//if(bHasAML)
	//{
	//	fIncrementAML = lLevel * pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_ADD_MAXHP_UP, 1) * 0.001f;
	//}
	////! ----------------------------------------ED----------------------------------------
	////限制DEF ED / ATK ED
	//if(pEquipment->HasAddonProperty(CGoodsBaseProperties::GAP_ATK_ED) && 
	//	pEquipment->HasAddonProperty(CGoodsBaseProperties::GAP_CUR_ATK_ED))
	//{
	//	long lMaxAtkEd=pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_ATK_ED,2);
	//	long lCurAtkEd=pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_CUR_ATK_ED,1);
	//	if(lCurAtkEd>lMaxAtkEd)
	//	{
	//		lCurAtkEd=lMaxAtkEd;
	//		pEquipment->SetAddonPropertyBaseValues(CGoodsBaseProperties::GAP_CUR_ATK_ED,1,lCurAtkEd);
	//	}
	//}

	//if(pEquipment->HasAddonProperty(CGoodsBaseProperties::GAP_DEF_ED) && 
	//	pEquipment->HasAddonProperty(CGoodsBaseProperties::GAP_CUR_DEF_ED))
	//{
	//	long lMaxDefEd=pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_DEF_ED,2);
	//	long lCurDefEd=pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_CUR_DEF_ED,1);
	//	if(lCurDefEd>lMaxDefEd)
	//	{
	//		lCurDefEd=lMaxDefEd;
	//		pEquipment->SetAddonPropertyBaseValues(CGoodsBaseProperties::GAP_CUR_DEF_ED,1,lCurDefEd);
	//	}
	//}
	////! 攻击
	//long lCurAtkED = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_CUR_ATK_ED,1);
	//if(lCurAtkED>=0)
	//{
	//	//! 最小攻击修正
	//	if(bHasMinPA)
	//	{
	//		fIncrementMinPA += (fIncrementMinPA + lMinPA) * lCurAtkED * 0.01f;
	//	}
	//	//! 最大攻击修正
	//	if(bHasMaxPA)
	//	{
	//		fIncrementMaxPA += (fIncrementMaxPA + lMaxPA) * lCurAtkED * 0.01f;
	//	}
	//	//! 魔法攻击修正
	//	if(bHasMA)
	//	{
	//		fIncrementMA += (fIncrementMA + lMA) * lCurAtkED  * 0.01f;
	//	}
	//}
	////! 防御
	//long lCurDefED = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_CUR_DEF_ED,1);
	//if(lCurDefED>=0)
	//{
	//	//! 普通防御修正
	//	if(bHasPR)
	//	{
	//		fIncrementPR += (fIncrementPR + lPR) * lCurDefED * 0.01f;
	//	}
	//	//! 魔法防御修正
	//	if(bHasMR)
	//	{
	//		fIncrementMR += (fIncrementMR + lMR) * lCurDefED  * 0.01f;
	//	}
	//}

	////! ----------------------------------------设置值----------------------------------------
	////! 最小攻击修正
	//if(bHasMinPA)
	//{
	//	//long fIncrementValue = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_MIN_ATTACK, 1);
	//	pEquipment->ChangeAttribute(CGoodsBaseProperties::GAP_MIN_ATTACK, DWORD(0.5f + fIncrementMinPA + lMinPA), 1, true);
	//}

	////! 最大攻击修正
	//if(bHasMaxPA)
	//{
	//	//long fIncrementValue = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_MAX_ATTACK, 1);
	//	pEquipment->ChangeAttribute(CGoodsBaseProperties::GAP_MAX_ATTACK, DWORD(0.5f + fIncrementMaxPA + lMaxPA), 1, true);
	//}

	////! 普通防御修正
	//if(bHasPR)
	//{
	//	//long fIncrementValue = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_NOR_DEF, 1);
	//	pEquipment->ChangeAttribute(CGoodsBaseProperties::GAP_NOR_DEF, DWORD(0.5f + fIncrementPR + lPR), 1, true);
	//}
	////! 魔法攻击修正
	//if(bHasMA)
	//{
	//	//long fIncrementValue = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_FUMO_PROPERTY, 1);
	//	pEquipment->ChangeAttribute(CGoodsBaseProperties::GAP_FUMO_PROPERTY, DWORD(0.5f + fIncrementMA + lMA), 1, true);
	//}
	////! 魔法防御修正
	//if(bHasMR)
	//{
	//	//long fIncrementValue = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_MAGIC_DEF, 1);
	//	pEquipment->ChangeAttribute(CGoodsBaseProperties::GAP_MAGIC_DEF, DWORD(0.5f + fIncrementMR + lMR), 1, true);
	//}
	////! 命中修正成长
	//if(bHasHR)
	//{
	//	//long fIncrementValue = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_HIT_RATE_CORRECT, 1);
	//	pEquipment->ChangeAttribute(CGoodsBaseProperties::GAP_HIT_RATE_CORRECT, DWORD(0.5f + fIncrementHR + lHR), 1, true);
	//}
	////! 物理致命一击率成长
	//if(bHasKR)
	//{
	//	//long fIncrementValue = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_NORATTACK_KILLRATE, 1);
	//	pEquipment->ChangeAttribute(CGoodsBaseProperties::GAP_NORATTACK_KILLRATE, DWORD(0.5f + fIncrementKR + lKR), 1, true);
	//}
	////! 魔法致命一击率成长
	//if(bHasMKR)
	//{
	//	//long fIncrementValue = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_MAGIC_KILLRATE, 1);
	//	pEquipment->ChangeAttribute(CGoodsBaseProperties::GAP_MAGIC_KILLRATE, DWORD(0.5f + fIncrementMKR + lMKR), 1, true);
	//}
	////! 招架能力修正成长
	//if(bHasP)
	//{
	//	//long fIncrementValue = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_BLOCK_CORRECT, 1);
	//	pEquipment->ChangeAttribute(CGoodsBaseProperties::GAP_BLOCK_CORRECT, DWORD(0.5f + fIncrementP + lP), 1, true);
	//}
	////! 格挡能力修正成长
	//if(bHasB)
	//{
	//	//long fIncrementValue = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_PARRY_CORRECT, 1);
	//	pEquipment->ChangeAttribute(CGoodsBaseProperties::GAP_PARRY_CORRECT, DWORD(0.5f + fIncrementB + lB), 1, true);
	//}
	////! 闪避能力修正成长
	//if(bHasF)
	//{
	//	//long fIncrementValue = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_FLEE_CORRECT, 1);
	//	pEquipment->ChangeAttribute(CGoodsBaseProperties::GAP_FLEE_CORRECT, DWORD(0.5f + fIncrementF + lF), 1, true);
	//}
	////! 道具最大耐久值成长值修正
	//if(bHasMaxD)
	//{
	//	//long fIncrementValue = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_GOODS_MAX_DURABILITY, 1);
	//	pEquipment->ChangeAttribute(CGoodsBaseProperties::GAP_GOODS_MAX_DURABILITY, DWORD(0.5f + fIncrementMaxD + lMaxD), 1, true);
	//}
	////! 盾牌物理防御成长修正								
	//if(bHasSD)
	//{
	//	//long fIncrementValue = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_SHIELD_DEF, 1);
	//	pEquipment->ChangeAttribute(CGoodsBaseProperties::GAP_SHIELD_DEF, DWORD(0.5f + fIncrementSD + lSD), 1, true);
	//}
	////! 武器强度成长修正							
	//if(bHasWDL)
	//{
	//	//long fIncrementValue = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_DAMAGE_LEVEL, 1);
	//	pEquipment->ChangeAttribute(CGoodsBaseProperties::GAP_WEAPON_DAMAGE_LEVEL, DWORD(0.5f + fIncrementWDL + lWDL), 1, true);
	//}
	////! 冲等追加生命上限修正							
	//if(bHasAML)
	//{
	//	//long fIncrementValue = pEquipment->GetAddonPropertyValues(CGoodsBaseProperties::GAP_WEAPON_DAMAGE_LEVEL, 1);
	//	pEquipment->ChangeAttribute(CGoodsBaseProperties::GAP_ADD_MAXHP_LEVEL, DWORD(0.5f + fIncrementAML + lAML), 1, true);
	//}
	//pEquipment->UpdateEnchaseAttr();


	
}

eEquipLimitType CGoodsFactory::QueryPlayerEquipLimitType(CPlayer* pPlayer)
{
	BYTE btOccu=pPlayer->GetOccupation();
	if(btOccu==OCC_Mystic || btOccu==OCC_Warlock || btOccu==OCC_Cleric || btOccu==OCC_Sorcerer ||
		btOccu==OCC_Necromancer || btOccu==OCC_Bishop || btOccu==OCC_Prophet)
	{
		return ELT_LIGHT;
	}
	else if(btOccu==OCC_Rogue || btOccu==OCC_Scout || btOccu==OCC_Craftsman || btOccu==OCC_Ranger ||
		btOccu==OCC_Assassin || btOccu==OCC_BladeDancer || btOccu==OCC_Bard)
	{
		return ELT_MID;
	}
	else if(btOccu==OCC_Fighter || btOccu==OCC_Knight || btOccu==OCC_Warrior || btOccu==OCC_Paladin ||
		btOccu==OCC_DarkKnight || btOccu==OCC_Berserker || btOccu==OCC_Gladiator)
	{
		return ELT_HEAVY;
	}
	return ELT_UNKNOWN;
}

eBusinessGoodsType CGoodsFactory::GetBusinessGoodsType(DWORD dwGoodsId)
{
	CGoodsBaseProperties* pProperty=GoodsSetup::QueryGoodsBaseProperties(dwGoodsId);
	if(!pProperty)
		return BGT_NOT_BUSINESS_GOODS;
	DWORD dwVal=pProperty->GetAddonPropertyValue(GAP_TRADEGOODS_TYPE,1);
	if(dwVal>=BGT_FOOD && dwVal<BGT_MAX_TYPE)
	{
		eBusinessGoodsType eVal=static_cast<eBusinessGoodsType>(dwVal);
		return eVal;
	}	
	return BGT_NOT_BUSINESS_GOODS;
}

//! 创建同一个类型的所有物品，并放入一个按物品索引为key的map
void  CGoodsFactory::CreateGoodsListByType(eGoodsBaseType eType, map<LONG, CGoods*> &mapGoods)
{
	mapGoods.clear();

	map<DWORD,CGoodsBaseProperties*>* pGoodsBasePropertiesList = GoodsSetup::GetGoodsBasePropertiesList();
	map<DWORD,CGoodsBaseProperties*>::iterator ite = pGoodsBasePropertiesList->begin();
	for (; ite != pGoodsBasePropertiesList->end(); ++ite)
	{
		if(eType == ite->second->GetGoodsBaseType())
		{
			CGoods *pGoods = CreateGoods(ite->first,10000);
			if(NULL != pGoods)
			{
				mapGoods[ite->first] = pGoods;
			}
		}
	}

}

BOOL CGoodsFactory::Upgrade(CGoods* pGoods, eGoodsAddonProperties gapSource,
							eGoodsAddonProperties gapDest, BOOL bUpgrade)
{
	return TRUE;
}


void CGoodsFactory::RunScriptOnGet(DWORD dwGoodsId,CPlayer* pPlayer)
{
	if(pPlayer==NULL)
		return;
	CGoodsBaseProperties* pProperty=GoodsSetup::QueryGoodsBaseProperties(dwGoodsId);
	if(!pProperty)
		return;

	DWORD dwVal=pProperty->GetAddonPropertyValue(GAP_GET_TRIGGER,1);
	if(dwVal!=0)
	{
		char strFile[64];
		_snprintf(strFile, 64, "scripts/Goods/%d.lua",dwVal);
		Script::RunEnv env( pPlayer, NULL, static_cast<CRegion*>( pPlayer->GetFather() ) );
		GetInst( ScriptSys ).RunScript( env, strFile );
	}
}

////获取物品最大叠加数
//DWORD CGoodsFactory::QueryGoodsMaxStackNumber( DWORD dwGoodsBasePropertiesID )
//{
//	DWORD dwResult = 1;
//	CGoodsBaseProperties* pProperties = GoodsSetup::QueryGoodsBaseProperties( dwGoodsBasePropertiesID );
//
//	if(dwGoodsBasePropertiesID==GoodsSetup::GetGoldCoinIndex())
//	{			
//		return GlobalSetup::GetSetup()->dwGoldCoinLimit;
//	}
//	else if(dwGoodsBasePropertiesID==GoodsSetup::GetSilverIndex())
//	{
//		return GlobalSetup::GetSetup()->dwSilverLimit;
//	}
//	if( pProperties && (pProperties -> GetGoodsType() == GT_CONSUMABLE ||
//		pProperties -> GetGoodsType() == GT_USELESS))
//	{
//		vector<CGoodsBaseProperties::tagAddonPropertyValue> vOut;
//		pProperties -> GetAddonPropertyValues( GAP_MAXOVERLAP, vOut );
//		for( size_t i = 0; i < vOut.size(); i ++ )
//		{
//			if( vOut[i].dwId == 1 )
//			{
//				dwResult = static_cast<DWORD>( vOut[i].lBaseValue );
//				break;
//			}
//		}
//		vOut.clear();
//	}
//	return dwResult;
//}

map<CGUID, vector<BYTE>>	CGoodsFactory::m_mapChatGoodsInfo;
list<pair<CGUID, LONG>>		CGoodsFactory::m_listSaveTimeOrder;

//! 添加一个物品信息
void CGoodsFactory::AddGoodsInfo(const CGUID &Key, const vector<BYTE> &vGoodsInfo)
{
	if(m_mapChatGoodsInfo.end() != m_mapChatGoodsInfo.find(Key)) return;

	if (GlobalSetup::GetSetup()->lMaxGoodInfoNum <= (long)m_mapChatGoodsInfo.size())
	{
		ClearInfo(GlobalSetup::GetSetup()->lMaxGoodInfoNum / 10);
	}

	m_mapChatGoodsInfo[Key] = vGoodsInfo;
	m_listSaveTimeOrder.push_back(pair<CGUID, LONG>(Key, (long)time(NULL)));
}

//! 取得一个物品信息
BOOL CGoodsFactory::GetGoodsInfo(const CGUID &Key, vector<BYTE> &vGoodsInfo)
{
	map<CGUID, vector<BYTE>>::iterator ite = m_mapChatGoodsInfo.find(Key);
	if (m_mapChatGoodsInfo.end() != ite)
	{
		if(0 >= ite->second.size())
			return FALSE;
		vGoodsInfo.resize(ite->second.size());
		memmove(&vGoodsInfo[0], &(ite->second[0]), ite->second.size());
		return TRUE;
	}
	return FALSE;
}


//! 清理
void CGoodsFactory::ClearInfo(LONG lClearNum)
{
	if (0 >= lClearNum)
		lClearNum = 1;

	list<pair<CGUID, LONG>>::iterator ite = m_listSaveTimeOrder.begin();
	LONG i = 0;
	for ( ; ite != m_listSaveTimeOrder.end(); ++ite)
	{
		if(i < lClearNum)
			break;
		++i;
	}

	m_listSaveTimeOrder.erase(m_listSaveTimeOrder.begin(), ite);
}


long CGoodsFactory::OutputUseGoodsInfo(const char *pszFileName)
{
	Log4c::Trace(ROOT_MODULE,"UseGoodsInfo:");
	map<long,set<CGoods*> >::iterator it = s_UsingGoods.begin();
	for(;it != s_UsingGoods.end();it++)
	{
		if((*it).second.size() > 0)
		{
			Log4c::Trace(ROOT_MODULE, "Flag:%d,Count:%d",(*it).first,(*it).second.size() );
		}
	}
	return 0;
}