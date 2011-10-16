// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "CGoodsBaseProperties.h"
#include "CGoods.h"
#include "CGoodsFactory.h"
#include "..\..\..\setup\globesetup.h"
#include "../GoodsExManage.h"
#include "../Player.h"
#include "../ServerRegion.h"
#include <math.h>

map<DWORD,CGoodsBaseProperties*> CGoodsFactory::s_mGoodsBaseProperties;

map<string, DWORD> CGoodsFactory::s_mOriginalNameIndex;

map<string, DWORD> CGoodsFactory::s_mNameIndex;

map<string,DWORD> CGoodsFactory::s_mAddonNameMap;

map<string,string> CGoodsFactory::s_mapSucceedOrder;
map<string,CGUID> CGoodsFactory::s_mapPendingOrder;
list<string> CGoodsFactory::s_listSuccedOrder;

set<CGoods*> CGoodsFactory::s_mGcGoodsSet;
vector<CGoods*> CGoodsFactory::s_vNoDelGoods;

map<long,set<CGoods*> >CGoodsFactory::s_UsingGoods;

BOOL CGoodsFactory::msNoDelFlag=FALSE;

BYTE CGoodsFactory::s_GoodsAttrDBSetup[512][2];
DWORD CGoodsFactory::s_dwAddonPropertyNum;

CGoodsFactory::CGoodsFactory()
{
	// TODO: Add your specialized code here.
}

CGoodsFactory::~CGoodsFactory()
{
	// TODO: Add your specialized code here.
}

//通过编号获取物品基本属性
CGoodsBaseProperties* CGoodsFactory::QueryGoodsBaseProperties(DWORD dwID)
{
	// TODO: Add your specialized code here.
	map<DWORD,CGoodsBaseProperties*>::iterator it = s_mGoodsBaseProperties.find( dwID );
	if( it != s_mGoodsBaseProperties.end() )
	{
		return it -> second;
	}
	return NULL;
}


//通过原始名取得物品基本属性
CGoodsBaseProperties* CGoodsFactory::QueryGoodsBasePropertiesByOriginalName(const CHAR* pName)
{
	// TODO: Add your specialized code here.
	return QueryGoodsBaseProperties( QueryGoodsIDByOriginalName(pName) );
}

//通过物品名称取得物品基本属性
CGoodsBaseProperties* CGoodsFactory::QueryGoodsBasePropertiesByName(const CHAR* pName)
{
	// TODO: Add your specialized code here.
	return QueryGoodsBaseProperties( QueryGoodsIDByName(pName) );;
}
//根据ID获取物品栏位
CGoodsBaseProperties::EQUIP_PLACE CGoodsFactory::QueryGoodsEuipPlace(DWORD dwID)
{
	// TODO: Add your specialized code here.
	CGoodsBaseProperties* pProperties = QueryGoodsBaseProperties( dwID );
	if( pProperties )
	{
		return pProperties -> GetEquipPlace();
	}
	return CGoodsBaseProperties::EP_UNKNOW;
}
//根据ID获取物品栏位
CGoodsBaseProperties::GOODS_TYPE CGoodsFactory::QueryGoodsType(DWORD dwID)
{
	// TODO: Add your specialized code here.
	CGoodsBaseProperties* pProperties = QueryGoodsBaseProperties( dwID );
	if( pProperties )
	{
		return pProperties -> GetGoodsType();
	}
	return CGoodsBaseProperties::GT_USELESS;
}
//根据ID获取物品价格
DWORD CGoodsFactory::QueryGoodsPrice(DWORD dwID)
{
	// TODO: Add your specialized code here.
	CGoodsBaseProperties* pProperties = QueryGoodsBaseProperties( dwID );
	if( pProperties )
	{
		return pProperties -> GetPrice();
	}

	return 0;
}

VOID CGoodsFactory::AddGoodsToGCSet(CGoods* pGoods)
{
	if(pGoods)
	{
		s_mGcGoodsSet.insert(pGoods);		
	}
}

VOID CGoodsFactory::ReleaseGCGoodsSet()
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

VOID CGoodsFactory::ReleaseNoDelGoods()
{	
	for(size_t i=0;i<s_vNoDelGoods.size();i++)
	{
		MP_DELETE(s_vNoDelGoods[i]);
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


//根据ID获取物品描述
const CHAR* CGoodsFactory::QueryGoodsDescribe(DWORD dwID)
{
	// TODO: Add your specialized code here.
	CGoodsBaseProperties* pProperties = QueryGoodsBaseProperties( dwID );
	if( pProperties )
	{
		return pProperties -> GetDescribe();
	}

	return NULL;
}

//通过编号取得物品原始名
const CHAR* CGoodsFactory::QueryGoodsOriginalName(DWORD dwID)
{
	// TODO: Add your specialized code here.
	CGoodsBaseProperties* pProperties = QueryGoodsBaseProperties( dwID );
	if( pProperties )
	{
		return pProperties -> GetOriginalName();
	}
	return NULL;
}

//通过编号取得物品名称
const CHAR* CGoodsFactory::QueryGoodsName(DWORD dwID)
{
	// TODO: Add your specialized code here.
	CGoodsBaseProperties* pProperties = QueryGoodsBaseProperties( dwID );
	if( pProperties )
	{
		return pProperties -> GetName();
	}
	return NULL;
}

const CHAR* CGoodsFactory::QueryGoodsName(const CHAR* pOriginName)
{
	CGoodsBaseProperties* pProperties = QueryGoodsBasePropertiesByOriginalName(pOriginName);
	if( pProperties )
	{
		return pProperties -> GetName();
	}
	return NULL;
}

//通过物品原始名取得物品编号
DWORD CGoodsFactory::QueryGoodsIDByOriginalName(const CHAR* pName)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pName == NULL ) return 0;

	string strKey;
	strKey = pName;
	map<string, DWORD>::iterator it = s_mOriginalNameIndex.find( pName );
	if( it != s_mOriginalNameIndex.end() )
	{
		return it -> second;
	}
	return 0;
}

//通过物品名取得物品编号
DWORD CGoodsFactory::QueryGoodsIDByName(const CHAR* pName)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pName == NULL ) return 0;

	string strKey;
	strKey = pName;
	map<string, DWORD>::iterator it = s_mNameIndex.find( pName );
	if( it != s_mNameIndex.end() )
	{
		return it -> second;
	}
	return 0;
}

BOOL CGoodsFactory::Serialize(vector<BYTE>* pStream, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	_AddToByteArray( pStream, static_cast<DWORD>( s_mGoodsBaseProperties.size() ) );
	map<DWORD,CGoodsBaseProperties*>::iterator it = s_mGoodsBaseProperties.begin();
	for( ; it != s_mGoodsBaseProperties.end(); it ++ )
	{
		_AddToByteArray( pStream, it -> first );
		it -> second -> Serialize( pStream );
	}

	_AddToByteArray(pStream,s_dwAddonPropertyNum);
	for(DWORD i=1;i<=s_dwAddonPropertyNum;i++)
	{
		_AddToByteArray(pStream,s_GoodsAttrDBSetup[i][0]);
		_AddToByteArray(pStream,s_GoodsAttrDBSetup[i][1]);
	}

	return TRUE;
}

BOOL CGoodsFactory::Unserialize(BYTE* pStream, LONG& lOffset, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	Release();

	DWORD dwNumGoods = _GetDwordFromByteArray( pStream, lOffset );
	for( DWORD i = 0; i < dwNumGoods; i ++ )
	{
		CGoodsBaseProperties* pGoodsBaseProperties = OBJ_CREATE(CGoodsBaseProperties);
		if( pGoodsBaseProperties == NULL )
		{
			OutputDebugString( "Cann't alloc memory for goods base properties decode!\n" );
			return FALSE;
		}

		DWORD dwIndex = _GetDwordFromByteArray( pStream, lOffset );
		pGoodsBaseProperties -> Unserialize( pStream, lOffset );

		s_mGoodsBaseProperties[dwIndex]									= pGoodsBaseProperties;
		s_mOriginalNameIndex[pGoodsBaseProperties -> m_strOriginalName] = dwIndex;
		s_mNameIndex[pGoodsBaseProperties -> m_strName]					= dwIndex;
	}

	for(DWORD i=0;i<512;i++)
	{
		s_GoodsAttrDBSetup[i][0]=0;
		s_GoodsAttrDBSetup[i][1]=0;
	}

	s_dwAddonPropertyNum=_GetDwordFromByteArray(pStream,lOffset);
	for(DWORD i=1;i<=s_dwAddonPropertyNum;i++)
	{
		s_GoodsAttrDBSetup[i][0]=_GetByteFromByteArray(pStream,lOffset);
		s_GoodsAttrDBSetup[i][1]=_GetByteFromByteArray(pStream,lOffset);
	}

	InitAddonNameMap();

	return TRUE;
}

//序列化到DataBlock
BOOL CGoodsFactory::Serialize(DBWriteSet& setWriteDB, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	setWriteDB.AddToByteArray( static_cast<DWORD>( s_mGoodsBaseProperties.size() ) );
	map<DWORD,CGoodsBaseProperties*>::iterator it = s_mGoodsBaseProperties.begin();
	for( ; it != s_mGoodsBaseProperties.end(); it ++ )
	{
		setWriteDB.AddToByteArray( it -> first );
		it -> second -> Serialize( setWriteDB );
	}

	setWriteDB.AddToByteArray(s_dwAddonPropertyNum);
	for(DWORD i=1;i<=s_dwAddonPropertyNum;i++)
	{
		setWriteDB.AddToByteArray(s_GoodsAttrDBSetup[i][0]);
		setWriteDB.AddToByteArray(s_GoodsAttrDBSetup[i][1]);
	}
	return TRUE;
}

//通过DataBlock还原
BOOL CGoodsFactory::Unserialize(DBReadSet& setReadDB, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	Release();
	DWORD dwNumGoods = setReadDB.GetDwordFromByteArray(  );
	for( DWORD i = 0; i < dwNumGoods; i ++ )
	{
		CGoodsBaseProperties* pGoodsBaseProperties = OBJ_CREATE(CGoodsBaseProperties);
		if( pGoodsBaseProperties == NULL )
		{
			OutputDebugString( "Cann't alloc memory for goods base properties decode!\n" );
			return FALSE;
		}

		DWORD dwIndex = setReadDB.GetDwordFromByteArray(  );
		pGoodsBaseProperties -> Unserialize( setReadDB );

		s_mGoodsBaseProperties[dwIndex]									= pGoodsBaseProperties;
		s_mOriginalNameIndex[pGoodsBaseProperties -> m_strOriginalName] = dwIndex;
		s_mNameIndex[pGoodsBaseProperties -> m_strName]					= dwIndex;
	}

	for(DWORD i=0;i<512;i++)
	{
		s_GoodsAttrDBSetup[i][0]=0;
		s_GoodsAttrDBSetup[i][1]=0;
	}

	s_dwAddonPropertyNum=setReadDB.GetDwordFromByteArray();
	for(DWORD i=1;i<=s_dwAddonPropertyNum;i++)
	{
		s_GoodsAttrDBSetup[i][0]=setReadDB.GetByteFromByteArray( );
		s_GoodsAttrDBSetup[i][1]=setReadDB.GetByteFromByteArray( );
	}

	InitAddonNameMap();

	return TRUE;
}

//从新创建物品的附加属性，保留物品的等级不变
VOID CGoodsFactory::ReCreateAddonProperties(CGoods* pGoods)
{
	if(pGoods == NULL)	return;
	//得到当前等级
	LONG lCurrentLevel =1;// pGoods -> GetAddonPropertyValues( GAP_WEAPON_LEVEL, 1 );
	//清除已有的属性
	pGoods -> m_vAddonProperties.clear();

	CreateAddonProperties(pGoods);
	//还原物品的等级
	if(lCurrentLevel > 0)
		UpgradeEquipment(pGoods,lCurrentLevel);
}

//物品套装属性的生成
VOID CGoodsFactory::CreateSuitProperties(CGoods* pGoods)
{
	if(pGoods == NULL)	return;
	DWORD dwID = pGoods->GetBasePropertiesIndex();
	CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( dwID );

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
			if(suitProperties.gapType==GAP_GOODS_MAXIMUM_DURABILITY)
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
VOID CGoodsFactory::CreateAddonProperties(CGoods* pGoods)
{
	if(pGoods == NULL)	return;
	DWORD dwID = pGoods->GetBasePropertiesIndex();
	CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( dwID );
	if( pProperties )
	{
		//##获取所有有效的附加属性
		vector<GOODS_ADDON_PROPERTIES> vOut;
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
	CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( dwID );
	CGoods* pGoods = NULL;
	if( pProperties )
	{
		//##创建好物品,并且设置好初始的属性.
		pGoods = MP_NEW CGoods;
		pGoods -> SetBasePropertiesIndex( dwID );
		pGoods -> SetName( pProperties -> GetName() );
		pGoods -> SetGoodsName( pProperties -> GetName() );
		
		pGoods -> SetPrice( pProperties -> GetPrice() );
		pGoods->SetSilverPrice(pProperties->GetSilverPrice());
		pGoods -> SetGraphicsID( pProperties -> GetIconID(CGoodsBaseProperties::IT_GROUND) );
		pGoods -> SetAmount( 1 );
		pGoods->SetGoodsBaseType(pProperties->GetGoodsBaseType());

		//产生物品的附加属性
		CreateAddonProperties(pGoods);
		CreateSuitProperties(pGoods);
		pGoods->InitEnchaseHole();
		pGoods->EDCalculate();
		pGoods->AtkEDUpdate();
		pGoods->DefEDUpdate();
		pGoods->CalculatePKCP(); // 初始PK CP值（增加上限值）

		pGoods->ImmunityUpdate();
		pGoods->PierceUpdate();
		pGoods->ConstellationUpdate();
		pGoods->LuckyUpdate();
		pGoods->AddonHpUpdate();

		/*
		if(pGoods->HasAddonProperty(GAP_RANDOM_ADDON_MAXHP) && 
			pGoods->HasAddonProperty(GAP_MAXHP_CORRECT))
		{
			long lRanMaxHpAddon=pGoods->GetAddonPropertyValues(GAP_RANDOM_ADDON_MAXHP,1);
			pGoods->ChangeAttribute(GAP_MAXHP_CORRECT,lRanMaxHpAddon,1);
		}*/



		//##分配编号
		CGUID	guid;
		CGUID::CreateGUID(guid);
		pGoods->SetExID(guid);
	}
	s_UsingGoods[lType].insert(pGoods);
	return pGoods;
}



//##ModelId=41A688EC0109
VOID CGoodsFactory::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	map<DWORD,CGoodsBaseProperties*>::iterator it =s_mGoodsBaseProperties.begin();	
	for( ; it != s_mGoodsBaseProperties.end(); it ++ )
	{
		OBJ_RELEASE(CGoodsBaseProperties, it -> second);		
	}
	s_mGoodsBaseProperties.clear();
	s_mOriginalNameIndex.clear();
	s_mNameIndex.clear();
	s_mapSucceedOrder.clear();
	s_mapPendingOrder.clear();
	s_listSuccedOrder.clear();
}

//创建指定数量的物品
VOID CGoodsFactory::CreateGoods(DWORD dwID, DWORD dwAmount, vector<CGoods*>& vOut,long lType)
{	
	//通过编号取得基本属性
	CGoodsBaseProperties* pProperties = QueryGoodsBaseProperties( dwID );
	if( pProperties )
	{
		//可叠加的消耗品和不能使用的物品
		if( pProperties -> GetGoodsType() == CGoodsBaseProperties::GT_CONSUMABLE ||
			pProperties -> GetGoodsType() == CGoodsBaseProperties::GT_USELESS )
		{
			DWORD dwRemainedGoods	= dwAmount;
			DWORD dwMaxStackNumber	= QueryGoodsMaxStackNumber( dwID );
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

//取得‘金钱’的编号
DWORD CGoodsFactory::GetGoldCoinIndex()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return CGlobeSetup::GetSetup()->dwGoldGoodsId;	
}

//取得‘宝石’的编号
DWORD CGoodsFactory::GetGemIndex()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.	
	return QueryGoodsIDByName("宝石");
}

const char* CGoodsFactory::GetGoldOrginName()
{
	DWORD dwGoldId=GetGoldCoinIndex();
	return QueryGoodsOriginalName(dwGoldId);
}

const char* CGoodsFactory::GetGoldName()
{
	DWORD dwGoldId=GetGoldCoinIndex();
	return QueryGoodsName(dwGoldId);
}


//取得‘元宝’的编号
DWORD CGoodsFactory::GetYuanBaoIndex()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.	
	return QueryGoodsIDByName("元宝");
}

DWORD CGoodsFactory::GetSilverIndex()
{
	return CGlobeSetup::GetSetup()->dwSilverGoodsId;
}

const char* CGoodsFactory::GetSilverOrginName()
{
	DWORD dwId=GetSilverIndex();
	return QueryGoodsOriginalName(dwId);
}

const char* CGoodsFactory::GetSilverName()
{
	DWORD dwId=GetSilverIndex();
	return QueryGoodsName(dwId);
}

//##ModelId=41C632940182
CGoods* CGoodsFactory::UnserializeGoods(BYTE* pStream, LONG& lOffset,long lType)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	if( pStream )
	{
		pResult = MP_NEW CGoods;
		if( pResult == NULL )
		{
			assert( 0 );
			OutputDebugString( "Can't alloc memory for unserialize goods.\n" );
		}
		else 
		{
			pResult -> Unserialize( pStream, lOffset );

			CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( 
				pResult -> GetBasePropertiesIndex() );
			if( pProperties )
			{
				//if( pProperties -> GetGoodsType() == CGoodsBaseProperties::GT_CONSUMABLE )
				//{
				//	//pResult -> SetID( GetGame() -> AssignTemGoodsID() );
				//	CGUID guid;
				//	CGUID::CreateGUID(guid);
				//	pResult->SetExID(guid);
				//}
			}
			else
			{
				GarbageCollect( &pResult ,100);
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
	pResult = MP_NEW CGoods;
	if( pResult == NULL )
	{
		assert( 0 );
		OutputDebugString( "Can't alloc memory for unserialize goods.\n" );
	}
	else 
	{
		pResult -> Unserialize( setReadDB );

		CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( 
			pResult -> GetBasePropertiesIndex() );
		if(! pProperties )
		{
			GarbageCollect( &pResult ,100);
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
				char pszInfo[512]="";
				char pszGUID[50]="";
				(*ppGoods)->GetExID().tostring(pszGUID);
				_snprintf(pszInfo,512,"删除物品(Name:%s,GUID:%s)时,该物品存在场景上",(*ppGoods)->GetName(),pszGUID);
				PutDebugString(pszInfo);
				//pRegion->RemoveObject((*ppGoods));
			}
			if((*ppGoods)->GetGoodsBaseType() == GT_MEDAL && !bCollectMedal ) 
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
			MP_DELETE(*ppGoods );
		}		
		bResult = TRUE;
	}
	return bResult;
}



//计算物品修理价格
DWORD CGoodsFactory::CalculateRepairPrice(CGoods* pGoods)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwGoodsPrice=pGoods->GetPrice();
	if(dwGoodsPrice>0)
	{
		dwGoodsPrice *= CGlobeSetup::GetSetup()->dwNpcSellGoldToSilver;
	}
	else
	{
		dwGoodsPrice=pGoods->GetSilverPrice();
	}
	DWORD dwRepairPrice = 0;
	if( pGoods && pGoods -> CanReparied() )
	{
		//获取当前耐久值
		LONG lCurrentDurability	= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAXIMUM_DURABILITY, 2 );
		//获取最大耐久值
		LONG lMaxDurability		= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAXIMUM_DURABILITY, 1 );
		FLOAT fFactor			= CGlobeSetup::GetSetup() -> fRepairFactor;
		FLOAT fFactor2			= CGlobeSetup::GetSetup() -> fRepairFactor2;
		FLOAT fRate				= ( lMaxDurability - lCurrentDurability ) / static_cast<FLOAT>( lMaxDurability );
		if( fRate < 0.0f )
		{
			fRate = 0.0f;
		}
		if (lCurrentDurability!=0)
		{
			dwRepairPrice = static_cast<DWORD>( dwGoodsPrice * fRate * fFactor );
		}
		else
			dwRepairPrice = static_cast<DWORD>( dwGoodsPrice * fRate * fFactor2 );
	}
	return dwRepairPrice;
}

//计算物品升级费用
DWORD CGoodsFactory::CalculateUpgradePrice(CGoods* pGoods)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwResult = 0;
	if( pGoods && pGoods -> CanUpgraded() )
	{
		// 每次升级费用＝物品升级价格×（1＋（38－4×lg物品升级价格）/15）pow（物品当前等级）
		LONG lCurrentLevel	=1;// pGoods -> GetAddonPropertyValues( GAP_WEAPON_LEVEL, 1 );
		LONG lUpgradePrice	=1;// pGoods -> GetAddonPropertyValues( GAP_GOODS_UPGRADE_PRICE, 1 );
		FLOAT fFactor		= 1 + ( 38 - 4 * log10( static_cast<FLOAT>(lUpgradePrice) ) ) / 15;
		fFactor				= pow( fFactor, lCurrentLevel );
		dwResult			= static_cast<DWORD>( lUpgradePrice * fFactor );
	}
	return dwResult;
}

//计算物品出售价格
DWORD CGoodsFactory::CalculateVendPrice(CGoods *pGoods)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwVendPrice = 0;
	if( pGoods && pGoods -> GetPrice() > 0 )
	{
		//##首先获取全局设置
		FLOAT fVendPrice = static_cast<FLOAT>( pGoods -> GetPrice() );

		CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( 
			pGoods -> GetBasePropertiesIndex() );
		if( pProperties )
		{
			if( pProperties -> GetGoodsType() == CGoodsBaseProperties::GT_EQUIPMENT ) 
			{
				//##获取折旧保值比
				FLOAT fDepreciableRate = CGlobeSetup::GetSetup() -> fBasePriceRate;

				//##获取当前/最大耐久
				DWORD dwCurrentDurability	= pGoods -> GetAddonPropertyValues( 
					GAP_GOODS_MAXIMUM_DURABILITY, 2 );
				DWORD dwMaxDurability		= pGoods -> GetAddonPropertyValues( 
					GAP_GOODS_MAXIMUM_DURABILITY, 1 );

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
		FLOAT fTradeInPriceRate = CGlobeSetup::GetSetup() -> fTradeInRate;

		dwVendPrice = static_cast<DWORD>( fVendPrice * fTradeInPriceRate );
	}
	return dwVendPrice;
}


//计算物品出售价格
DWORD CGoodsFactory::CalculateVendSilverPrice(CGoods *pGoods)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if(!pGoods)
		return 0;
	DWORD dwSilver=0;
	if(pGoods->GetPrice()>0)
	{
		dwSilver=pGoods->GetPrice() * CGlobeSetup::GetSetup()->dwNpcSellGoldToSilver;
	}
	else
	{
		dwSilver=pGoods->GetSilverPrice();
	}

	DWORD dwVendPrice = 0;
	if(dwSilver>0)
	{
		//##首先获取全局设置
		FLOAT fVendPrice =dwSilver;

		CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( 
			pGoods -> GetBasePropertiesIndex() );
		if( pProperties )
		{
			if( pProperties -> GetGoodsType() == CGoodsBaseProperties::GT_EQUIPMENT ) 
			{
				//##获取折旧保值比
				FLOAT fDepreciableRate = CGlobeSetup::GetSetup() -> fBasePriceRate;

				//##获取当前/最大耐久
				DWORD dwCurrentDurability	= pGoods -> GetAddonPropertyValues( 
					GAP_GOODS_MAXIMUM_DURABILITY, 2 );
				DWORD dwMaxDurability		= pGoods -> GetAddonPropertyValues( 
					GAP_GOODS_MAXIMUM_DURABILITY, 1 );

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
		FLOAT fTradeInPriceRate = CGlobeSetup::GetSetup() -> fTradeInRate;

		dwVendPrice = static_cast<DWORD>( fVendPrice * fTradeInPriceRate );
	}
	return dwVendPrice;
}

//物品的修理
BOOL CGoodsFactory::RepairEquipment(CGoods* pGoods)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( pGoods && pGoods -> CanReparied() )
	{
		//设置物品的当前耐久为最大耐久值
		//LONG lCurrentDurability	= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAXIMUM_DURABILITY, 2 );
		LONG lMaxDurability		= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAXIMUM_DURABILITY, 1 );
		pGoods->ChangeAttribute( GAP_GOODS_MAXIMUM_DURABILITY,lMaxDurability,2,true);
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
			lWeaponWaste = CGlobeSetup::GetSetup() -> lGoodsDurSpecialFray;
		}
		else
		{
			lWeaponWaste = CGlobeSetup::GetSetup() -> lGoodsDurFray;
		}

		LONG lCurrentDurability	= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAXIMUM_DURABILITY, 2 );
		LONG lMaxDurability		= pGoods -> GetAddonPropertyValues( GAP_GOODS_MAXIMUM_DURABILITY, 1 );

		if( lCurrentDurability > 0 )
		{
			//更新物品的当前耐久值
			lCurrentDurability -= lWeaponWaste;
			//pGoods -> SetAddonPropertyBaseValues( GAP_GOODS_MAXIMUM_DURABILITY, 2, lCurrentDurability );
			pGoods->ChangeAttribute(GAP_GOODS_MAXIMUM_DURABILITY,lCurrentDurability,2,true);
			bResult = TRUE;
		}
	}
	return bResult;
}

//升级装备
BOOL CGoodsFactory::UpgradeEquipment(CGoods* pGoods, LONG lTargetLevel)
{
	/*
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( pGoods && pGoods -> CanUpgraded() )
	{
	//取得当前等级
	LONG lCurrentLevel = pGoods -> GetAddonPropertyValues( GAP_WEAPON_LEVEL, 1 );
	if( lCurrentLevel >= 0 )
	{
	BOOL bUpgrade = TRUE;
	if( lTargetLevel < lCurrentLevel )
	{
	bUpgrade = FALSE;
	}

	bResult = TRUE;
	BOOL bLevelChanged;
	while( pGoods -> GetAddonPropertyValues(GAP_WEAPON_LEVEL, 1) != lTargetLevel )
	{
	bLevelChanged = FALSE;
	for( size_t i = 0; i < pGoods -> m_vAddonProperties.size(); i ++ )
	{
	switch( pGoods -> m_vAddonProperties[i].gapType )
	{
	case GAP_MINIMUM_ATTACK_UPGRADE:
	Upgrade( pGoods, GAP_MINIMUM_ATTACK_UPGRADE, 
	GAP_MINIMUM_ATTACK_CORRECTION, bUpgrade );
	break;
	case GAP_MAXIMUM_ATTACK_UPGRADE:
	Upgrade( pGoods, GAP_MAXIMUM_ATTACK_UPGRADE, 
	GAP_MAXIMUM_ATTACK_CORRECTION, bUpgrade );
	break;
	case GAP_ELEMENT_ATTACK_UPGRADE:
	Upgrade( pGoods, GAP_ELEMENT_ATTACK_UPGRADE, 
	GAP_ELEMENT_ATTACK_CORRECTION, bUpgrade );
	break;
	case GAP_ARMOR_UPGRADE:
	Upgrade( pGoods, GAP_ARMOR_UPGRADE, 
	GAP_ARMOR_CORRECTION, bUpgrade );
	break;
	case GAP_ATTACK_SPEED_UPGRADE:
	Upgrade( pGoods, GAP_ATTACK_SPEED_UPGRADE, 
	GAP_ATTACK_SPEED_CORRECTION, bUpgrade );
	break;
	case GAP_HIT_RATE_UPGRADE:
	Upgrade( pGoods, GAP_HIT_RATE_UPGRADE, 
	GAP_HIT_RATE_CORRECTION, bUpgrade );
	break;
	case GAP_FATAL_BLOW_RATE_UPGRADE:
	Upgrade( pGoods, GAP_FATAL_BLOW_RATE_UPGRADE, 
	GAP_FATAL_BLOW_RATE_CORRECTION, bUpgrade );
	break;
	case GAP_STIFFEN_PROBABILITY_CORRECTION_UPGRADE:
	Upgrade( pGoods, GAP_STIFFEN_PROBABILITY_CORRECTION_UPGRADE, 
	GAP_STIFFEN_PROBABILITY_CORRECTION, bUpgrade );
	break;
	case GAP_DODEG_UPGRADE:
	Upgrade( pGoods, GAP_DODEG_UPGRADE, 
	GAP_DODGE_CORRECTION, bUpgrade );
	break;
	case GAP_ROLE_MINIMUM_LEVEL_LIMIT_UPGRADE:
	Upgrade( pGoods, GAP_ROLE_MINIMUM_LEVEL_LIMIT_UPGRADE, 
	GAP_ROLE_MINIMUM_LEVEL_LIMIT, bUpgrade );
	break;
	case GAP_ROLE_MINIMUM_STRENGTH_LIMIT_UPGRADE:
	Upgrade( pGoods, GAP_ROLE_MINIMUM_STRENGTH_LIMIT_UPGRADE, 
	GAP_ROLE_MINIMUM_STRENGTH_LIMIT, bUpgrade );
	break;
	case GAP_ROLE_MINIMUM_AGILITY_LIMIT_UPGRADE:
	Upgrade( pGoods, GAP_ROLE_MINIMUM_AGILITY_LIMIT_UPGRADE, 
	GAP_ROLE_MINIMUM_AGILITY_LIMIT, bUpgrade );
	break;
	case GAP_ROLE_MINIMUM_CONSTITUTION_LIMIT_UPGRADE:
	Upgrade( pGoods, GAP_ROLE_MINIMUM_CONSTITUTION_LIMIT_UPGRADE, 
	GAP_ROLE_MINIMUM_CONSTITUTION_LIMIT, bUpgrade );
	break;
	case GAP_ROLE_MINIMUM_WAKAN_LIMIT_UPGRADE:
	Upgrade( pGoods, GAP_ROLE_MINIMUM_WAKAN_LIMIT_UPGRADE, 
	GAP_ROLE_MINIMUM_WAKAN_LIMIT, bUpgrade );
	break;
	case GAP_GOODS_MAXIMUM_DURABILITY_UPGRADE:
	Upgrade( pGoods, GAP_GOODS_MAXIMUM_DURABILITY_UPGRADE, 
	GAP_GOODS_MAXIMUM_DURABILITY, bUpgrade );
	break;
	case GAP_HP_UPPER_LIMIT_CORRECTION_UPGRADE:
	Upgrade( pGoods, GAP_HP_UPPER_LIMIT_CORRECTION_UPGRADE, 
	GAP_HP_UPPER_LIMIT_CORRECTION, bUpgrade );
	break;
	case GAP_MP_UPPER_LIMIT_CORRECTION_UPGRADE:
	Upgrade( pGoods, GAP_MP_UPPER_LIMIT_CORRECTION_UPGRADE, 
	GAP_MP_UPPER_LIMIT_CORRECTION, bUpgrade );
	break;
	case GAP_SKILL_REUSE_TIME_CORRECTION_UPGRADE:
	Upgrade( pGoods, GAP_SKILL_REUSE_TIME_CORRECTION_UPGRADE, 
	GAP_SKILL_REUSE_TIME_CORRECTION, bUpgrade );
	break;
	case GAP_BURDEN_UPPER_LIMIT_CORRECTION_UPGRADE:
	Upgrade( pGoods, GAP_BURDEN_UPPER_LIMIT_CORRECTION_UPGRADE, 
	GAP_BURDEN_UPPER_LIMIT_CORRECTION, bUpgrade );
	break;
	case GAP_ELEMENT_RESISTANCE_CORRECTION_UPGRADE:
	Upgrade( pGoods, GAP_ELEMENT_RESISTANCE_CORRECTION_UPGRADE, 
	GAP_ELEMENT_RESISTANCE_CORRECTION, bUpgrade );
	break;
	case GAP_WEAPON_DAMAGE_UPGRADE:
	Upgrade( pGoods, GAP_WEAPON_DAMAGE_UPGRADE, 
	GAP_WEAPON_DAMAGE_LEVEL, bUpgrade );
	break;
	case GAP_WEAPON_LEVEL:
	for( size_t j = 0; j < pGoods -> m_vAddonProperties[i].vValues.size(); j ++ )
	{
	if( pGoods -> m_vAddonProperties[i].vValues[j].dwId == 1 )
	{
	if( bUpgrade )
	{
	++( pGoods -> m_vAddonProperties[i].vValues[j].lModifier );
	}
	else
	{
	--( pGoods -> m_vAddonProperties[i].vValues[j].lModifier );
	}
	bLevelChanged = TRUE;
	break;
	}
	}
	break;
	}
	}

	if( bLevelChanged == FALSE )
	{
	bResult = FALSE;
	break;
	}
	}
	}
	}
	return bResult;
	*/
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
		LONG lMaxDurability = pEquipment -> GetAddonPropertyValues( GAP_GOODS_MAXIMUM_DURABILITY, 1 );
		pEquipment -> SetAddonPropertyBaseValues( GAP_GOODS_MAXIMUM_DURABILITY, 2, lMaxDurability);//当前耐久
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
		LONG lMaxDurability = pEquipment -> GetAddonPropertyValues( GAP_GOODS_MAXIMUM_DURABILITY, 1 );
		pEquipment -> SetAddonPropertyBaseValues( GAP_GOODS_MAXIMUM_DURABILITY, 2, lMaxDurability);//当前耐久
		return TRUE;
	}
	return FALSE;
}

//! 得到装备是否是攻击类
BOOL CGoodsFactory::IsAttEquipment(CGoods* pEquipment)
{
	if(NULL != pEquipment)
	{
		switch(pEquipment->GetGoodsBaseType())
	{
		case GT_WEAPON:
		case GT_RING:
		case GT_NECKLACE:
		return TRUE;
	}
	}

	return FALSE;
}


//! 更新装备属性（仅限于物攻、魔攻、物防、魔防）
void CGoodsFactory::UpdateEquipmentAttr(CGoods* pEquipment)
{

	//! 基本公式：[（基础+冲等）*（1+魔化）+卡片+套装]
	if (NULL == pEquipment) return;

	pEquipment->DelAddonPropertyNotInBase();

	

	//! ----------------------------------------物品基础值----------------------------------------
	CGoodsBaseProperties *pGoodsBaseProperties = QueryGoodsBaseProperties(pEquipment->GetBasePropertiesIndex());
	if (NULL == pGoodsBaseProperties) return;

	vector<CGoodsBaseProperties::tagAddonPropertyValue> vOutProperty;

	long lMinPA = 0, lMaxPA = 0, lMA = 0, lPR = 0, lMR = 0, 
		lHR = 0, lKR = 0, lMKR = 0, lP = 0, lB = 0, lF = 0, 
		lMaxD = 0, lSD = 0, lWDL = 0, lAML = 0; //! 基本属性

	float fIncrementMinPA = 0,fIncrementMaxPA = 0,fIncrementMA = 0,fIncrementPR = 0,fIncrementMR = 0, 
		fIncrementHR = 0, fIncrementKR = 0, fIncrementMKR = 0, fIncrementP = 0, fIncrementB = 0, fIncrementF = 0,
		fIncrementMaxD = 0, fIncrementSD = 0, fIncrementWDL = 0, fIncrementAML = 0;//! 当前修正的属性（变化量）

	bool bHasMinPA = false, bHasMaxPA = false, bHasMA = false, bHasPR = false, bHasMR = false, 
		bHasHR = false, bHasKR = false, bHasMKR = false, bHasP = false, bHasB = false, bHasF = false,
		bHasMaxD = false, bHasSD = false, bHasWDL = false, bHasAML = false;//! 有否属性

	//! 重置部分属性
	{
		//! 需要重置的属性列表（受冲等和魔化影响的属性不需要在这里重置）
		DWORD arrProperties[] = 
		{
			GAP_CRI_DAMAGE,				//! 物理暴击伤害
			GAP_CRI_DEF,				//! 物理暴击抗性
			GAP_MCRI_DAMAGE,			//! 魔法暴击伤害
			GAP_MCRI_DEF,				//! 魔法暴击抗性
			GAP_MAXHP_CORRECT,			//! 生命上限修正
			GAP_DEDICANT_CORRECT,		//! 专注修正
			GAP_CON_DEF,				//! 基本体质抗性
			GAP_PARRY_CORRECT,			//! 格挡能力修正

			GAP_MAGIC_SPEED,			//! 施法速度
			GAP_ADD_STR,				//! 力量追加
			GAP_ADD_DEX,				//! 敏捷追加
			GAP_ADD_CON,				//! 体质追加
			GAP_ADD_INT,				//! 智力追加
			GAP_ADD_WIS,				//! 智慧追加
			GAP_ADD_SPI,				//! 精神追加

			GAP_CARD_ELEMENT_EARTH_A,	//! 卡片地攻
			GAP_CARD_ELEMENT_WATER_A,	//! 卡片水攻
			GAP_CARD_ELEMENT_FIRE_A,	//! 卡片火攻
			GAP_CARD_ELEMENT_WIND_A,	//! 卡片风攻
			GAP_CARD_ELEMENT_BRIGHT_A,	//! 卡片光攻
			GAP_CARD_ELEMENT_DARK_A,	//! 卡片暗攻

			GAP_CARD_ELEMENT_EARTH_D,	//! 卡片地防
			GAP_CARD_ELEMENT_WATER_D,	//! 卡片水防
			GAP_CARD_ELEMENT_FIRE_D,	//! 卡片火防
			GAP_CARD_ELEMENT_WIND_D,	//! 卡片风防
			GAP_CARD_ELEMENT_BRIGHT_D,	//! 卡片光防
			GAP_CARD_ELEMENT_DARK_D,	//! 卡片暗防

		};
		
		//! 开始重置
		for (LONG i = 0; i < sizeof(arrProperties) / sizeof(DWORD); ++i)
		{
			vOutProperty.clear();
			pGoodsBaseProperties->GetAddonPropertyValues((GOODS_ADDON_PROPERTIES)arrProperties[i], vOutProperty);
			if(0 != vOutProperty.size())
				pEquipment->ChangeAttribute(arrProperties[i], vOutProperty[0].lBaseValue, 1, true);	
		}
	}
	vOutProperty.clear();
	//! 最小攻击力
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_MIN_ATTACK, vOutProperty);
	if(0 != vOutProperty.size())
	{
		lMinPA = vOutProperty[0].lBaseValue;
		bHasMinPA = true;
	}
	//! 最大攻击力
	vOutProperty.clear();
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_MAX_ATTACK, vOutProperty);
	if(0 != vOutProperty.size())
	{
		lMaxPA = vOutProperty[0].lBaseValue;
		bHasMaxPA = true;
	}
	//! 魔法攻击
	vOutProperty.clear();
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_FUMO_PROPERTY, vOutProperty);
	if(0 != vOutProperty.size())
	{
		lMA = vOutProperty[0].lBaseValue;
		bHasMA = true;
	}
	//! 物理防御力
	vOutProperty.clear();
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_NOR_DEF, vOutProperty);
	if(0 != vOutProperty.size())
	{
		lPR = vOutProperty[0].lBaseValue;
		bHasPR = true;
	}
	//! 魔法防御
	vOutProperty.clear();
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_MAGIC_DEF, vOutProperty);
	if(0 != vOutProperty.size())
	{
		lMR = vOutProperty[0].lBaseValue;
		bHasMR = true;
	}
	//! 命中
	vOutProperty.clear();
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_HIT_RATE_CORRECT, vOutProperty);
	if(0 != vOutProperty.size())
	{
		lHR = vOutProperty[0].lBaseValue;
		bHasHR = true;
	}
	//! 物理致命一击
	vOutProperty.clear();
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_NORATTACK_KILLRATE, vOutProperty);
	if(0 != vOutProperty.size())
	{
		lKR = vOutProperty[0].lBaseValue;
		bHasKR = true;
	}
	//! 魔法致命一击
	vOutProperty.clear();
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_MAGIC_KILLRATE, vOutProperty);
	if(0 != vOutProperty.size())
	{
		lMKR = vOutProperty[0].lBaseValue;
		bHasMKR = true;
	}
	//! 招架能力
	vOutProperty.clear();
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_BLOCK_CORRECT, vOutProperty);
	if(0 != vOutProperty.size())
	{
		lP = vOutProperty[0].lBaseValue;
		bHasP = true;
	}
	//! 格挡能力
	vOutProperty.clear();
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_PARRY_CORRECT, vOutProperty);
	if(0 != vOutProperty.size())
	{
		lB = vOutProperty[0].lBaseValue;
		bHasB = true;
	}
	//! 闪避能力
	vOutProperty.clear();
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_FLEE_CORRECT, vOutProperty);
	if(0 != vOutProperty.size())
	{
		lF = vOutProperty[0].lBaseValue;
		bHasF = true;
	}
	//! 道具最大耐久值成长值
	vOutProperty.clear();
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_GOODS_MAXIMUM_DURABILITY, vOutProperty);
	if(0 != vOutProperty.size())
	{
		lMaxD = vOutProperty[0].lBaseValue;
		bHasMaxD = true;
	}
	//! 盾牌物理防御成长
	vOutProperty.clear();
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_SHIELD_DEF, vOutProperty);
	if(0 != vOutProperty.size())
	{
		lSD = vOutProperty[0].lBaseValue;
		bHasSD = true;
	}
	//! 武器强度成长
	vOutProperty.clear();
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_WEAPON_DAMAGE_LEVEL, vOutProperty);
	if(0 != vOutProperty.size())
	{
		lWDL = vOutProperty[0].lBaseValue;
		bHasWDL = true;
	}
	//! 冲等追加生命上限
	vOutProperty.clear();
	pGoodsBaseProperties->GetAddonPropertyValues(GAP_ADD_MAXHP_LEVEL, vOutProperty);
	if(0 != vOutProperty.size())
	{
		lAML = vOutProperty[0].lBaseValue;
		bHasAML = true;
	}

	//! ----------------------------------------冲等----------------------------------------
	long lLevel = pEquipment->GetAddonPropertyValues(GAP_WEAPON_LEVEL, 1);
	//! 最小攻击修正
	if(bHasMinPA)
	{
		fIncrementMinPA = lLevel * pEquipment->GetAddonPropertyValues(GAP_MIN_ATK_UP, 1) * 0.001f;
	}
	//! 最大攻击修正
	if(bHasMaxPA)
	{
		fIncrementMaxPA = lLevel * pEquipment->GetAddonPropertyValues(GAP_MAX_ATK_UP, 1) * 0.001f;
	}
	//! 普通防御修正
	if(bHasPR)
	{
		fIncrementPR = lLevel * pEquipment->GetAddonPropertyValues(GAP_DEF_UP, 1) * 0.001f;
	}
	//! 魔法攻击修正
	if(bHasMA)
	{
		fIncrementMA = lLevel * pEquipment->GetAddonPropertyValues(GAP_MAGIC_UP, 1) * 0.001f;
	}
	//! 魔法防御修正
	if(bHasMR)
	{
		fIncrementMR = lLevel * pEquipment->GetAddonPropertyValues(GAP_MAGICDEF_UP, 1) * 0.001f;
	}
	//! 命中修正成长
	if(bHasHR)
	{
		fIncrementHR = lLevel * pEquipment->GetAddonPropertyValues(GAP_HIT_RATE_UP, 1) * 0.001f;
	}
	//! 物理致命一击率成长
	if(bHasKR)
	{
		fIncrementKR = lLevel * pEquipment->GetAddonPropertyValues(GAP_KILL_RATE_UP, 1) * 0.001f;
	}
	//! 魔法致命一击率成长
	if(bHasMKR)
	{
		fIncrementMKR = lLevel * pEquipment->GetAddonPropertyValues(GAP_MAGIC_KILL_RATE_UP, 1) * 0.001f;
	}
	//! 招架能力修正成长
	if(bHasP)
	{
		fIncrementP = lLevel * pEquipment->GetAddonPropertyValues(GAP_PARRY_UP, 1) * 0.001f;
	}
	//! 格挡能力修正成长
	if(bHasB)
	{
		fIncrementB = lLevel * pEquipment->GetAddonPropertyValues(GAP_BLOCK_UP, 1) * 0.001f;
	}
	//! 闪避能力修正成长
	if(bHasF)
	{
		fIncrementF = lLevel * pEquipment->GetAddonPropertyValues(GAP_FLEE_UP, 1) * 0.001f;
	}
	//! 道具最大耐久值成长值修正
	if(bHasMaxD)
	{
		fIncrementMaxD = lLevel * pEquipment->GetAddonPropertyValues(GAP_MAXDUR_UP, 1) * 0.001f;
	}
	//! 盾牌物理防御成长修正								
	if(bHasSD)
	{
		fIncrementSD = lLevel * pEquipment->GetAddonPropertyValues(GAP_SHIELD_DEF_UP, 1) * 0.001f;
	}
	//! 武器强度成长修正							
	if(bHasWDL)
	{
		fIncrementWDL = lLevel * pEquipment->GetAddonPropertyValues(GAP_WEAPON_INTEN_UP, 1) * 0.001f;
	}
	//! 冲等追加生命上限成长修正							
	if(bHasAML)
	{
		fIncrementAML = lLevel * pEquipment->GetAddonPropertyValues(GAP_ADD_MAXHP_UP, 1) * 0.001f;
	}
	//! ----------------------------------------ED----------------------------------------
	//限制DEF ED / ATK ED
	if(pEquipment->HasAddonProperty(GAP_ATK_ED) && 
		pEquipment->HasAddonProperty(GAP_CUR_ATK_ED))
	{
		long lMaxAtkEd=pEquipment->GetAddonPropertyValues(GAP_ATK_ED,2);
		long lCurAtkEd=pEquipment->GetAddonPropertyValues(GAP_CUR_ATK_ED,1);
		if(lCurAtkEd>lMaxAtkEd)
		{
			lCurAtkEd=lMaxAtkEd;
			pEquipment->SetAddonPropertyBaseValues(GAP_CUR_ATK_ED,1,lCurAtkEd);
		}
	}

	if(pEquipment->HasAddonProperty(GAP_DEF_ED) && 
		pEquipment->HasAddonProperty(GAP_CUR_DEF_ED))
	{
		long lMaxDefEd=pEquipment->GetAddonPropertyValues(GAP_DEF_ED,2);
		long lCurDefEd=pEquipment->GetAddonPropertyValues(GAP_CUR_DEF_ED,1);
		if(lCurDefEd>lMaxDefEd)
		{
			lCurDefEd=lMaxDefEd;
			pEquipment->SetAddonPropertyBaseValues(GAP_CUR_DEF_ED,1,lCurDefEd);
		}
	}
	//! 攻击
	long lCurAtkED = pEquipment->GetAddonPropertyValues(GAP_CUR_ATK_ED,1);
	if(lCurAtkED>=0)
	{
		//! 最小攻击修正
		if(bHasMinPA)
		{
			fIncrementMinPA += (fIncrementMinPA + lMinPA) * lCurAtkED * 0.01f;
		}
		//! 最大攻击修正
		if(bHasMaxPA)
		{
			fIncrementMaxPA += (fIncrementMaxPA + lMaxPA) * lCurAtkED * 0.01f;
		}
		//! 魔法攻击修正
		if(bHasMA)
		{
			fIncrementMA += (fIncrementMA + lMA) * lCurAtkED  * 0.01f;
		}
	}
	//! 防御
	long lCurDefED = pEquipment->GetAddonPropertyValues(GAP_CUR_DEF_ED,1);
	if(lCurDefED>=0)
	{
		//! 普通防御修正
		if(bHasPR)
		{
			fIncrementPR += (fIncrementPR + lPR) * lCurDefED * 0.01f;
		}
		//! 魔法防御修正
		if(bHasMR)
		{
			fIncrementMR += (fIncrementMR + lMR) * lCurDefED  * 0.01f;
		}
	}

	//! ----------------------------------------设置值----------------------------------------
	//! 最小攻击修正
	if(bHasMinPA)
	{
		//long fIncrementValue = pEquipment->GetAddonPropertyValues(GAP_MIN_ATTACK, 1);
		pEquipment->ChangeAttribute(GAP_MIN_ATTACK, 0.5f + fIncrementMinPA + lMinPA, 1, true);
	}

	//! 最大攻击修正
	if(bHasMaxPA)
	{
		//long fIncrementValue = pEquipment->GetAddonPropertyValues(GAP_MAX_ATTACK, 1);
		pEquipment->ChangeAttribute(GAP_MAX_ATTACK, 0.5f + fIncrementMaxPA + lMaxPA, 1, true);
	}

	//! 普通防御修正
	if(bHasPR)
	{
		//long fIncrementValue = pEquipment->GetAddonPropertyValues(GAP_NOR_DEF, 1);
		pEquipment->ChangeAttribute(GAP_NOR_DEF, 0.5f + fIncrementPR + lPR, 1, true);
	}
	//! 魔法攻击修正
	if(bHasMA)
	{
		//long fIncrementValue = pEquipment->GetAddonPropertyValues(GAP_FUMO_PROPERTY, 1);
		pEquipment->ChangeAttribute(GAP_FUMO_PROPERTY, 0.5f + fIncrementMA + lMA, 1, true);
	}
	//! 魔法防御修正
	if(bHasMR)
	{
		//long fIncrementValue = pEquipment->GetAddonPropertyValues(GAP_MAGIC_DEF, 1);
		pEquipment->ChangeAttribute(GAP_MAGIC_DEF, 0.5f + fIncrementMR + lMR, 1, true);
	}
	//! 命中修正成长
	if(bHasHR)
	{
		//long fIncrementValue = pEquipment->GetAddonPropertyValues(GAP_HIT_RATE_CORRECT, 1);
		pEquipment->ChangeAttribute(GAP_HIT_RATE_CORRECT, 0.5f + fIncrementHR + lHR, 1, true);
	}
	//! 物理致命一击率成长
	if(bHasKR)
	{
		//long fIncrementValue = pEquipment->GetAddonPropertyValues(GAP_NORATTACK_KILLRATE, 1);
		pEquipment->ChangeAttribute(GAP_NORATTACK_KILLRATE, 0.5f + fIncrementKR + lKR, 1, true);
	}
	//! 魔法致命一击率成长
	if(bHasMKR)
	{
		//long fIncrementValue = pEquipment->GetAddonPropertyValues(GAP_MAGIC_KILLRATE, 1);
		pEquipment->ChangeAttribute(GAP_MAGIC_KILLRATE, 0.5f + fIncrementMKR + lMKR, 1, true);
	}
	//! 招架能力修正成长
	if(bHasP)
	{
		//long fIncrementValue = pEquipment->GetAddonPropertyValues(GAP_BLOCK_CORRECT, 1);
		pEquipment->ChangeAttribute(GAP_BLOCK_CORRECT, 0.5f + fIncrementP + lP, 1, true);
	}
	//! 格挡能力修正成长
	if(bHasB)
	{
		//long fIncrementValue = pEquipment->GetAddonPropertyValues(GAP_PARRY_CORRECT, 1);
		pEquipment->ChangeAttribute(GAP_PARRY_CORRECT, 0.5f + fIncrementB + lB, 1, true);
	}
	//! 闪避能力修正成长
	if(bHasF)
	{
		//long fIncrementValue = pEquipment->GetAddonPropertyValues(GAP_FLEE_CORRECT, 1);
		pEquipment->ChangeAttribute(GAP_FLEE_CORRECT, 0.5f + fIncrementF + lF, 1, true);
	}
	//! 道具最大耐久值成长值修正
	if(bHasMaxD)
	{
		//long fIncrementValue = pEquipment->GetAddonPropertyValues(GAP_GOODS_MAXIMUM_DURABILITY, 1);
		pEquipment->ChangeAttribute(GAP_GOODS_MAXIMUM_DURABILITY, 0.5f + fIncrementMaxD + lMaxD, 1, true);
	}
	//! 盾牌物理防御成长修正								
	if(bHasSD)
	{
		//long fIncrementValue = pEquipment->GetAddonPropertyValues(GAP_SHIELD_DEF, 1);
		pEquipment->ChangeAttribute(GAP_SHIELD_DEF, 0.5f + fIncrementSD + lSD, 1, true);
	}
	//! 武器强度成长修正							
	if(bHasWDL)
	{
		//long fIncrementValue = pEquipment->GetAddonPropertyValues(GAP_WEAPON_DAMAGE_LEVEL, 1);
		pEquipment->ChangeAttribute(GAP_WEAPON_DAMAGE_LEVEL, 0.5f + fIncrementWDL + lWDL, 1, true);
	}
	//! 冲等追加生命上限修正							
	if(bHasAML)
	{
		//long fIncrementValue = pEquipment->GetAddonPropertyValues(GAP_WEAPON_DAMAGE_LEVEL, 1);
		pEquipment->ChangeAttribute(GAP_ADD_MAXHP_LEVEL, 0.5f + fIncrementAML + lAML, 1, true);
	}
	pEquipment->UpdateEnchaseAttr();


	
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
	CGoodsBaseProperties* pProperty=QueryGoodsBaseProperties(dwGoodsId);
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

	map<DWORD,CGoodsBaseProperties*>::iterator ite = s_mGoodsBaseProperties.begin();
	for (; ite != s_mGoodsBaseProperties.end(); ++ite)
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

BOOL CGoodsFactory::Upgrade(CGoods* pGoods, GOODS_ADDON_PROPERTIES gapSource,
							GOODS_ADDON_PROPERTIES gapDest, BOOL bUpgrade)
{
	/*
	BOOL bResult = FALSE;
	if( pGoods )
	{
	LONG lMinValue = pGoods -> GetAddonPropertyValues( gapSource, 1 );
	LONG lMaxValue = pGoods -> GetAddonPropertyValues( gapSource, 2 );
	LONG lModifier = 0;
	if( lMinValue > 0 )
	{
	if( lMaxValue > 0 )
	{
	lModifier = lMinValue + random( lMaxValue - lMinValue );
	}
	else
	{
	lModifier = lMinValue;
	}

	for( size_t i = 0; i < pGoods -> m_vAddonProperties.size(); i ++ )
	{
	if( pGoods -> m_vAddonProperties[i].gapType == gapDest )
	{
	//##是否是升级
	if( bUpgrade )
	{
	pGoods -> m_vAddonProperties[i].vValues[0].lModifier += lModifier;
	if( pGoods -> m_vAddonProperties[i].vValues[0].lModifier > 65535 )
	{
	pGoods -> m_vAddonProperties[i].vValues[0].lModifier = 65535;
	}
	}
	else
	{
	pGoods -> m_vAddonProperties[i].vValues[0].lModifier -= lModifier;
	if( pGoods -> m_vAddonProperties[i].vValues[0].lModifier < 0 )
	{
	pGoods -> m_vAddonProperties[i].vValues[0].lModifier = 0;
	}
	}
	bResult = TRUE;
	break;
	}
	}
	}
	}
	return bResult;
	*/
	return TRUE;
}

//##ModelId=41F5B3230104
/*
BOOL CGoodsFactory::ModifyGoodsAddonProperty(CGoods* pGoods, DWORD dwID, LONG lValue1, LONG lValue2, BOOL bIsImplicitAttribute)
{
// TODO: Add your specialized code here.
// NOTE: Requires a correct return value to compile.
BOOL bResult = FALSE;
if( pGoods )
{
for( size_t i = 0; i < pGoods -> m_vAddonProperties.size(); i ++ )
{
if( pGoods -> m_vAddonProperties[i].gapType == dwID )
{
pGoods -> m_vAddonProperties[i].bIsImplicitAttribute = bIsImplicitAttribute;

for( size_t j = 0; j < pGoods -> m_vAddonProperties[i].vValues.size(); j ++ )
{
if( pGoods -> m_vAddonProperties[i].vValues[j].dwId == 1 )
{
pGoods -> m_vAddonProperties[i].vValues[j].lBaseValue	= lValue1;
pGoods -> m_vAddonProperties[i].vValues[j].lModifier	= 0;
}
else if( pGoods -> m_vAddonProperties[i].vValues[j].dwId == 2 )
{
pGoods -> m_vAddonProperties[i].vValues[j].lBaseValue	= lValue2;
pGoods -> m_vAddonProperties[i].vValues[j].lModifier	= 0;
}
}
bResult = TRUE;
break;
}
}
if( bResult == FALSE )
{
CGoods::tagAddonProperty tProperty;
CGoods::tagAddonPropertyValue tValue;
tProperty.gapType				= static_cast<GOODS_ADDON_PROPERTIES>( dwID );
tProperty.bIsImplicitAttribute	= bIsImplicitAttribute;
tProperty.bIsEnabled			= TRUE;
tValue.dwId						= 1;
tValue.lBaseValue				= lValue1;
tValue.lModifier				= 0;
tProperty.vValues.push_back( tValue );
tValue.dwId						= 2;
tValue.lBaseValue				= lValue2;
tValue.lModifier				= 0;
tProperty.vValues.push_back( tValue );
pGoods -> m_vAddonProperties.push_back( tProperty );
bResult = TRUE;
}
}
return bResult;
}*/


VOID CGoodsFactory::InitAddonNameMap()
{
	s_mAddonNameMap.clear();	
	s_mAddonNameMap["GAP_GOODS_CATEGORY"]=GAP_GOODS_CATEGORY;
	s_mAddonNameMap["GAP_COLOR"]=GAP_COLOR;
	s_mAddonNameMap["GAP_COSPLAYID"]=GAP_COSPLAYID;
	s_mAddonNameMap["GAP_EQUIP_EFFECT1"]=GAP_EQUIP_EFFECT1;
	s_mAddonNameMap["GAP_EQUIP_EFFECT2"]=GAP_EQUIP_EFFECT2;
	s_mAddonNameMap["GAP_EQUIP_EFFECT3"]=GAP_EQUIP_EFFECT3;
	s_mAddonNameMap["GAP_WEAPON_CATEGORY"]=GAP_WEAPON_CATEGORY;
	s_mAddonNameMap["GAP_ROLE_MINIMUM_LEVEL_LIMIT"]=GAP_ROLE_MINIMUM_LEVEL_LIMIT;
	s_mAddonNameMap["GAP_SKILL_LIMIT"]=GAP_SKILL_LIMIT;
	s_mAddonNameMap["GAP_REQUIRE_OCCUPATION"]=GAP_REQUIRE_OCCUPATION;
	s_mAddonNameMap["GAP_REQUIRE_GENDER"]=GAP_REQUIRE_GENDER;
	s_mAddonNameMap["GAP_PARTICULAR_ATTRIBUTE"]=GAP_PARTICULAR_ATTRIBUTE;
	s_mAddonNameMap["GAP_WEAPON_DAMAGE_LEVEL"]=GAP_WEAPON_DAMAGE_LEVEL;
	s_mAddonNameMap["GAP_MIN_ATTACK"]=GAP_MIN_ATTACK;
	s_mAddonNameMap["GAP_MAX_ATTACK"]=GAP_MAX_ATTACK;
	s_mAddonNameMap["GAP_NOR_DEF"]=GAP_NOR_DEF;
	s_mAddonNameMap["GAP_HURT_SUCK_PER"]=GAP_HURT_SUCK_PER;
	s_mAddonNameMap["GAP_HIT_RATE_CORRECT"]=GAP_HIT_RATE_CORRECT;
	s_mAddonNameMap["GAP_NORATTACK_KILLRATE"]=GAP_NORATTACK_KILLRATE;
	s_mAddonNameMap["GAP_FLEE_CORRECT"]=GAP_FLEE_CORRECT;
	s_mAddonNameMap["GAP_PARRY_CORRECT"]=GAP_PARRY_CORRECT;
	s_mAddonNameMap["GAP_BLOCK_CORRECT"]=GAP_BLOCK_CORRECT;
	s_mAddonNameMap["GAP_FUMO_PROPERTY"]=GAP_FUMO_PROPERTY;
	s_mAddonNameMap["GAP_MAGIC_KILLRATE"]=GAP_MAGIC_KILLRATE;
	s_mAddonNameMap["GAP_DEDICANT_CORRECT"]=GAP_DEDICANT_CORRECT;
	s_mAddonNameMap["GAP_MAGIC_SPEED"]=GAP_MAGIC_SPEED;
	s_mAddonNameMap["GAP_MAGIC_DEF"]=GAP_MAGIC_DEF;
	s_mAddonNameMap["GAP_FIREDEF_CORRECT"]=GAP_FIREDEF_CORRECT;
	s_mAddonNameMap["GAP_WATERDEF_CORRECT"]=GAP_WATERDEF_CORRECT;
	s_mAddonNameMap["GAP_WINDDEF_CORRECT"]=GAP_WINDDEF_CORRECT;
	s_mAddonNameMap["GAP_EARTHDEF_CORRECT"]=GAP_EARTHDEF_CORRECT;
	s_mAddonNameMap["GAP_HOLYDEF_CORRECT"]=GAP_HOLYDEF_CORRECT;
	s_mAddonNameMap["GAP_DARKDEF_CORRECT"]=GAP_DARKDEF_CORRECT;
	s_mAddonNameMap["GAP_WILL_DEF"]=GAP_WILL_DEF;
	s_mAddonNameMap["GAP_HYPNOSIS_DEF"]=GAP_HYPNOSIS_DEF;
	s_mAddonNameMap["GAP_POISON_DEF"]=GAP_POISON_DEF;
	s_mAddonNameMap["GAP_FIX_DEF"]=GAP_FIX_DEF;
	s_mAddonNameMap["GAP_CON_DEF"]=GAP_CON_DEF;
	s_mAddonNameMap["GAP_BLED_DEF"]=GAP_BLED_DEF;
	s_mAddonNameMap["GAP_DIZZY_DEF"]=GAP_DIZZY_DEF;
	s_mAddonNameMap["GAP_LULL_DEF"]=GAP_LULL_DEF;
	s_mAddonNameMap["GAP_ADD_STR"]=GAP_ADD_STR;
	s_mAddonNameMap["GAP_ADD_DEX"]=GAP_ADD_DEX;
	s_mAddonNameMap["GAP_ADD_CON"]=GAP_ADD_CON;
	s_mAddonNameMap["GAP_ADD_INT"]=GAP_ADD_INT;
	s_mAddonNameMap["GAP_ADD_WIS"]=GAP_ADD_WIS;
	s_mAddonNameMap["GAP_ADD_SPI"]=GAP_ADD_SPI;
	s_mAddonNameMap["GAP_MAXHP_CORRECT"]=GAP_MAXHP_CORRECT;
	s_mAddonNameMap["GAP_HP_RESUME_CORRECT"]=GAP_HP_RESUME_CORRECT;
	s_mAddonNameMap["GAP_MAXMP_CORRECT"]=GAP_MAXMP_CORRECT;
	s_mAddonNameMap["GAP_MP_RESUME_CORRECT"]=GAP_MP_RESUME_CORRECT;
	s_mAddonNameMap["GAP_ADD_HP"]=GAP_ADD_HP;
	s_mAddonNameMap["GAP_ADD_PERCENT_HP"]=GAP_ADD_PERCENT_HP;
	s_mAddonNameMap["GAP_ADD_MP"]=GAP_ADD_MP;
	s_mAddonNameMap["GAP_ADD_PERCENT_MP"]=GAP_ADD_PERCENT_MP;
	s_mAddonNameMap["GAP_GOODS_MAXIMUM_DURABILITY"]=GAP_GOODS_MAXIMUM_DURABILITY;
	s_mAddonNameMap["GAP_MAXOVERLAP"]=GAP_MAXOVERLAP;
	s_mAddonNameMap["GAP_LEARN_SKILL"]=GAP_LEARN_SKILL;
	s_mAddonNameMap["GAP_ADD_SKILL"]=GAP_ADD_SKILL;
	s_mAddonNameMap["GAP_LEECH_HP"]=GAP_LEECH_HP;
	s_mAddonNameMap["GAP_LEECH_MP"]=GAP_LEECH_MP;
	s_mAddonNameMap["GAP_HURT2MP"]=GAP_HURT2MP;
	s_mAddonNameMap["GAP_KNOCK"]=GAP_KNOCK;
	s_mAddonNameMap["GAP_HOME"]=GAP_HOME;
	s_mAddonNameMap["GAP_RAN_TRANS"]=GAP_RAN_TRANS;
	s_mAddonNameMap["GAP_SCRIPT		"]=GAP_SCRIPT		;
	s_mAddonNameMap["GAP_GEM_TYPE"]=GAP_GEM_TYPE;
	s_mAddonNameMap["GAP_GEM_LEVEL"]=GAP_GEM_LEVEL;
	s_mAddonNameMap["GAP_GEM_PROBABILITY"]=GAP_GEM_PROBABILITY;
	s_mAddonNameMap["GAP_GEM_UPGRADE_SUCCEED_RESULT"]=GAP_GEM_UPGRADE_SUCCEED_RESULT;
	s_mAddonNameMap["GAP_GEM_UPGRADE_FAILED_RESULT"]=GAP_GEM_UPGRADE_FAILED_RESULT;
	s_mAddonNameMap["GAP_MOUNT_TYPE	"]=GAP_MOUNT_TYPE	;
	s_mAddonNameMap["GAP_MOUNT_LEVEL"]=GAP_MOUNT_LEVEL;
	s_mAddonNameMap["GAP_MOUNT_PLAYER_ROLE_LIMIT"]=GAP_MOUNT_PLAYER_ROLE_LIMIT;
	s_mAddonNameMap["GAP_UNLIMITED_ACCESS"]=GAP_UNLIMITED_ACCESS;
	s_mAddonNameMap["GAP_EXCEPTION_STATE"]=GAP_EXCEPTION_STATE;
	s_mAddonNameMap["GAP_EXP_BONUS"]=GAP_EXP_BONUS;
	s_mAddonNameMap["GAP_EXPBALL_USERID"]=GAP_EXPBALL_USERID;
	s_mAddonNameMap["GAP_EXPBALL_EXP"]=GAP_EXPBALL_EXP;
	s_mAddonNameMap["GAP_YUANBAO_QUITS"]=GAP_YUANBAO_QUITS;
	s_mAddonNameMap["GAP_RECORD_REGIONID"]=GAP_RECORD_REGIONID;
	s_mAddonNameMap["GAP_RECORD_COOR"]=GAP_RECORD_COOR;
	s_mAddonNameMap["GAP_EQUIP_MAXEXP"]=GAP_EQUIP_MAXEXP;
	s_mAddonNameMap["GAP_SUITID"]=GAP_SUITID;
	s_mAddonNameMap["GAP_SUIT_NUMBER"]=GAP_SUIT_NUMBER;
	s_mAddonNameMap["GAP_CREATOR_NAME"]=GAP_CREATOR_NAME;
	s_mAddonNameMap["GAP_DEL_TIME"]=GAP_DEL_TIME;
	s_mAddonNameMap["GAP_LIFE"]=GAP_LIFE;
	s_mAddonNameMap["GAP_GET_TRIGGER"]=GAP_GET_TRIGGER;
	s_mAddonNameMap["GAP_LOST_TRIGGER"]=GAP_LOST_TRIGGER;
	s_mAddonNameMap["GAP_HOLE_NUM"]=GAP_HOLE_NUM;
	s_mAddonNameMap["GAP_ENCHASE_TARGET"]=GAP_ENCHASE_TARGET;
	s_mAddonNameMap["GAP_PACK_TYPE"]=GAP_PACK_TYPE;
	s_mAddonNameMap["GAP_PACK_CAPACITY"]=GAP_PACK_CAPACITY;
	s_mAddonNameMap["GAP_MIN_ATK_UP"]=GAP_MIN_ATK_UP;	
	s_mAddonNameMap["GAP_MAX_ATK_UP"]=GAP_MAX_ATK_UP;	
	s_mAddonNameMap["GAP_MAGIC_UP"]=GAP_MAGIC_UP;
	s_mAddonNameMap["GAP_DEF_UP"]=GAP_DEF_UP;	
	s_mAddonNameMap["GAP_MAGICDEF_UP"]=GAP_MAGICDEF_UP;	
	s_mAddonNameMap["GAP_HIT_RATE_UP"]=GAP_HIT_RATE_UP;
	s_mAddonNameMap["GAP_KILL_RATE_UP"]=GAP_KILL_RATE_UP;
	s_mAddonNameMap["GAP_PARRY_UP"]=GAP_PARRY_UP;
	s_mAddonNameMap["GAP_BLOCK_UP"]=GAP_BLOCK_UP;
	s_mAddonNameMap["GAP_FLEE_UP"]=GAP_FLEE_UP;
	s_mAddonNameMap["GAP_MAXDUR_UP"]=GAP_MAXDUR_UP;
	s_mAddonNameMap["GAP_WEAPON_LEVEL"]=GAP_WEAPON_LEVEL;
	s_mAddonNameMap["GAP_MINATK_MODIFY_PER"]=GAP_MINATK_MODIFY_PER;
	s_mAddonNameMap["GAP_MAXATK_MODIFY_PER"]=GAP_MAXATK_MODIFY_PER;
	s_mAddonNameMap["GAP_DEF_MODIFY_PER"]=GAP_DEF_MODIFY_PER;
	s_mAddonNameMap["GAP_MAGICATK_MODIFY_PER"]=GAP_MAGICATK_MODIFY_PER;
	s_mAddonNameMap["GAP_MAGICDEF_MODIFY_PER"]=GAP_MAGICDEF_MODIFY_PER;
	s_mAddonNameMap["GAP_GUARD_EQUIP_CATEGORY"]=GAP_GUARD_EQUIP_CATEGORY;
	s_mAddonNameMap["GAP_EQUIP_TRIGGER"]=GAP_EQUIP_TRIGGER;
	s_mAddonNameMap["GAP_UNEQUIP_TRIGGER"]=GAP_UNEQUIP_TRIGGER;
	s_mAddonNameMap["GAP_ADD_STATE	"]=GAP_ADD_STATE	;
	s_mAddonNameMap["GAP_COOLDOWN"]=GAP_COOLDOWN;
	s_mAddonNameMap["GAP_BOTHHAND_GRAPHIC"]=GAP_BOTHHAND_GRAPHIC;
	s_mAddonNameMap["GAP_CARD_CATEGORY"]=GAP_CARD_CATEGORY;
	s_mAddonNameMap["GAP_CARD_ENCHASE_NUM_LIMIT"]=GAP_CARD_ENCHASE_NUM_LIMIT;
}

DWORD CGoodsFactory::GetAddonByName(string str)
{
	map<string,DWORD>::iterator iter=s_mAddonNameMap.find(str);
	if(iter!=s_mAddonNameMap.end())
	{
		return iter->second;
	}
	return 0;
}

VOID CGoodsFactory::RunScriptOnGet(DWORD dwGoodsId,CPlayer* pPlayer)
{
	if(pPlayer==NULL)
		return;
	CGoodsBaseProperties* pProperty=CGoodsFactory::QueryGoodsBaseProperties(dwGoodsId);
	if(!pProperty)
		return;

	DWORD dwVal=pProperty->GetAddonPropertyValue(GAP_GET_TRIGGER,1);
	if(dwVal!=0)
	{
		char strFile[64];
		_snprintf(strFile, 64, "scripts/goods/%d.script",dwVal);
		stRunScript st;
		st.pszFileName = strFile;
		st.srcShape=dynamic_cast<CPlayer*>(pPlayer);		
		st.pRegion = dynamic_cast<CRegion*>(pPlayer->GetFather());
		st.desShape = NULL;
		st.guUsedItemID = CGUID::GUID_INVALID;
		RunScript(&st, (char*)GetGame()->GetScriptFileData(strFile));
	}
}

//获取物品最大叠加数
DWORD CGoodsFactory::QueryGoodsMaxStackNumber( DWORD dwGoodsBasePropertiesID )
{
	DWORD dwResult = 1;
	CGoodsBaseProperties* pProperties = QueryGoodsBaseProperties( dwGoodsBasePropertiesID );

	if(dwGoodsBasePropertiesID==GetGoldCoinIndex())
	{			
		return CGlobeSetup::GetSetup()->dwGoldCoinLimit;
	}
	else if(dwGoodsBasePropertiesID==GetSilverIndex())
	{
		return CGlobeSetup::GetSetup()->dwSilverLimit;
	}
	if( pProperties && (pProperties -> GetGoodsType() == CGoodsBaseProperties::GT_CONSUMABLE ||  pProperties -> GetGoodsType() == CGoodsBaseProperties::GT_CARD ||
		pProperties -> GetGoodsType() == CGoodsBaseProperties::GT_USELESS))
	{
		vector<CGoodsBaseProperties::tagAddonPropertyValue> vOut;
		pProperties -> GetAddonPropertyValues( GAP_MAXOVERLAP, vOut );
		for( size_t i = 0; i < vOut.size(); i ++ )
		{
			if( vOut[i].dwId == 1 )
			{
				dwResult = static_cast<DWORD>( vOut[i].lBaseValue );
				break;
			}
		}
		vOut.clear();
	}
	return dwResult;
}

map<CGUID, vector<BYTE>>	CGoodsFactory::m_mapChatGoodsInfo;
list<pair<CGUID, LONG>>		CGoodsFactory::m_listSaveTimeOrder;

//! 添加一个物品信息
void CGoodsFactory::AddGoodsInfo(const CGUID &Key, const vector<BYTE> &vGoodsInfo)
{
	if(m_mapChatGoodsInfo.end() != m_mapChatGoodsInfo.find(Key)) return;

	if (CGlobeSetup::GetSetup()->lMaxGoodInfoNum <= m_mapChatGoodsInfo.size())
	{
		ClearInfo(CGlobeSetup::GetSetup()->lMaxGoodInfoNum / 10);
	}

	m_mapChatGoodsInfo[Key] = vGoodsInfo;
	m_listSaveTimeOrder.push_back(pair<CGUID, LONG>(Key, time(NULL)));
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
	char str[64];
	PutStringToFile(pszFileName,"UseGoodsInfo:");
	map<long,set<CGoods*> >::iterator it = s_UsingGoods.begin();
	for(;it != s_UsingGoods.end();it++)
	{
		if((*it).second.size() > 0)
		{
			_snprintf(str,64, "Flag:%d,Count:%d",(*it).first,(*it).second.size() );
			PutStringToFile(pszFileName,str);
		}
	}
	return 0;
}