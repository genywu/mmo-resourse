// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "CGoods.h"
#include "CGoodsFactory.h"
#include "enchase\CEnchaseHole.h"
#include "..\nets\networld\message.h"



CGoods::CGoods()
{
	// TODO: Add your specialized code here.
	SetType( TYPE_GOODS );
	//SetID( 0 );
	m_dwAmount	= 1;
	m_dwPrice	= 0;
	m_dwSilverPrice=0;
	m_dwBuyPrice=0;
	m_pEnchaseHoleArray=NULL;
	m_nInitHoleNum = 0;
	m_dwActivedKitCardId=0;

#ifdef __MEMORY_LEAK_CHECK__
	CBaseObject::AddConstructRef(TYPE_GOODS);
	SetSelfConsflag(GetConstructFlag());
#endif
}

CGoods::CGoods(OBJECT_TYPE otType, LONG lID)
{
	// TODO: Add your specialized code here.
	SetType( otType );
	//SetID( lID );
	m_dwAmount	= 1;
	m_dwPrice	= 0;
}

CGoods::~CGoods()
{
	// TODO: Add your specialized code here.
	Release();
#ifdef __MEMORY_LEAK_CHECK__
	CBaseObject::RefConstructRef(TYPE_GOODS,GetSelfConsFlag());
#endif
}

DWORD CGoods::GetBasePropertiesIndex()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return m_dwBasePropertiesIndex;
}

VOID CGoods::SetAmount(DWORD dwAmount)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( dwAmount == 0 )
	{
		//##把自己从容器内删除？
	}
	m_dwAmount = dwAmount;
}

DWORD CGoods::GetAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return m_dwAmount;
}

//最大叠加值
DWORD CGoods::GetMaxStackNumber()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsBaseProperties* pProperties = 
		CGoodsFactory::QueryGoodsBaseProperties( GetBasePropertiesIndex() );
	if(GetBasePropertiesIndex()==CGoodsFactory::GetGoldCoinIndex())
	{
		return CGlobeSetup::GetSetup()->dwGoldCoinLimit;
	}
	else if(GetBasePropertiesIndex()==CGoodsFactory::GetSilverIndex())
	{
		return CGlobeSetup::GetSetup()->dwSilverLimit;
	}
	DWORD dwResult = 1;
	if( pProperties && (pProperties -> GetGoodsType() == CGoodsBaseProperties::GT_CONSUMABLE ||
		pProperties -> GetGoodsType() == CGoodsBaseProperties::GT_USELESS || 
		pProperties->GetGoodsBaseType()==GT_CARD))
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

//是否能修理
BOOL CGoods::CanReparied()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsBaseProperties* pProperties = 
		CGoodsFactory::QueryGoodsBaseProperties( GetBasePropertiesIndex() );
	if( pProperties && pProperties -> GetGoodsType() == 
		CGoodsBaseProperties::GT_EQUIPMENT )
	{
		return TRUE;
	}
	return FALSE;
}

VOID CGoods::InitEnchaseHole()
{
	//如可以开孔
	DWORD dwNum=GetMaxEnchaseHoleNum();
	m_nInitHoleNum = dwNum;
	if(dwNum>0)
	{
		m_pEnchaseHoleArray=(CEnchaseHole**)M_ALLOC(sizeof(CEnchaseHole*)*dwNum);
		for(int i=0;i<dwNum;i++)
		{
			m_pEnchaseHoleArray[i]=MP_NEW CEnchaseHole(this);
		}
	}
}

VOID CGoods::EnchaseRelease()
{
	//DWORD dwNum=GetMaxEnchaseHoleNum();
	DWORD dwNum=m_nInitHoleNum;
	if(dwNum>0 && m_pEnchaseHoleArray)
	{
		for(int i=0;i<dwNum;i++)
		{
			MP_DELETE(m_pEnchaseHoleArray[i]);
		}
	}
	M_FREE(m_pEnchaseHoleArray,sizeof(CEnchaseHole*)*m_nInitHoleNum);
	m_nInitHoleNum=0;
}


BOOL CGoods::CanEnchase()
{
	LONG lBaseHoleNum=GetAddonPropertyValues(GAP_HOLE_NUM,2);
	if(lBaseHoleNum>0)
	{
		return TRUE;
	}
	return FALSE;
}

DWORD CGoods::GetMaxEnchaseHoleNum()
{
	return GetAddonPropertyValues(GAP_HOLE_NUM,2);
}

LONG* CGoods::GetEnchaseHoleData()
{
	LONG* pArray=(LONG*)M_ALLOC(5*sizeof(long));
	//DWORD dwNum=GetMaxEnchaseHoleNum();
	DWORD dwNum=m_nInitHoleNum;
	for(int i=0;i<5;i++)
	{			
		if(i<dwNum)
		{
			if(m_pEnchaseHoleArray[i]->GetCard())
			{
				pArray[i]=m_pEnchaseHoleArray[i]->GetCard()->GetBasePropertiesIndex();
			}
			else if(m_pEnchaseHoleArray[i]->GetState())
			{
				pArray[i]=0;
			}	
			else 
			{
				pArray[i]=-1;
			}
		}
		else
		{
			pArray[i]=-1;
		}
		
	}

	return pArray;
	
}

VOID CGoods::SetEnchaseHoleState(int nIndex,BOOL bVal)
{
	//DWORD dwMaxHole=GetMaxEnchaseHoleNum();
	DWORD dwMaxHole=m_nInitHoleNum;
	if(dwMaxHole>0 && nIndex>=0 && nIndex<dwMaxHole && m_pEnchaseHoleArray)		
	{
		m_pEnchaseHoleArray[nIndex]->SetState(bVal);
	}
}

BOOL CGoods::Enchase(CGoods* pGoods,int nIndex)
{
	//物品本身必须可镶嵌
	if(!CanEnchase())
		return FALSE;
	//卡片必须是镶嵌型卡片
	
	DWORD dwMaxHoleNum=m_nInitHoleNum;
	if(dwMaxHoleNum>0 && nIndex>=0 && nIndex<dwMaxHoleNum && m_pEnchaseHoleArray)
	{
		m_pEnchaseHoleArray[nIndex]->SetCard(pGoods);
		return TRUE;
	}
	return FALSE;
}

DWORD CGoods::GetActivedHoleNum()
{
	//DWORD dwMaxHoleNum=GetMaxEnchaseHoleNum();
	DWORD dwMaxHoleNum=m_nInitHoleNum;
	if(dwMaxHoleNum>0)
	{
		DWORD dwActivedHoleNum=0;
		for(int i=0;i<dwMaxHoleNum;i++)
		{
			if(m_pEnchaseHoleArray[i]->GetState())
			{
				dwActivedHoleNum++;
			}
		}
		return dwActivedHoleNum;
	}	
	return 0;
	
}

BOOL CGoods::SetAddonPropertyBaseValues(GOODS_ADDON_PROPERTIES gapType, DWORD dwID, LONG lValue)
{
	BOOL bResult = FALSE;
	if(dwID!=1 && dwID!=2)
		return bResult;
	

	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		if( m_vAddonProperties[i].gapType == gapType )
		{
			m_vAddonProperties[i].lValues[dwID-1]=lValue;
			bResult=TRUE;
			break;
		}
	}
	return bResult;
}

BOOL CGoods::DelAddonPropertyBaseValues(GOODS_ADDON_PROPERTIES gapType)
{
	vector<CGoods::tagAddonProperty>::iterator proIter=m_vAddonProperties.begin();
	for(; proIter!=m_vAddonProperties.end(); proIter ++ )
	{
		if( proIter->gapType== gapType )
		{
			break;
		}
	}
	if(proIter!=m_vAddonProperties.end())
	{
		m_vAddonProperties.erase(proIter);
		return TRUE;
	}
	return FALSE;
}

VOID CGoods::AddAddonProperty(BYTE* pStream, long size )
{	
	long lPos=0;
	m_vAddonProperties.clear();
	//根据ID获得物品基本属性
	CGoodsBaseProperties* pBaseProperty=CGoodsFactory::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
	if(!pBaseProperty)
		return;
	
	m_vAddonProperties.clear();
	DWORD dwSize=_GetDwordFromByteArray(pStream,lPos);
	for(int i=0;i<dwSize;i++)
	{
		DWORD dwGapId=_GetDwordFromByteArray(pStream,lPos);
		long lVal1=_GetLongFromByteArray(pStream,lPos);
		long lVal2=_GetLongFromByteArray(pStream,lPos);				
		if(pBaseProperty->IsHasAddonPropertie(static_cast<GOODS_ADDON_PROPERTIES>(dwGapId)))
		{
			if(CGoodsFactory::s_GoodsAttrDBSetup[dwGapId][0]==1)
			{			
			tagAddonProperty addonProperty;			
			addonProperty.gapType=static_cast<GOODS_ADDON_PROPERTIES>(dwGapId);
			addonProperty.lValues[0]=lVal1;
			addonProperty.lValues[1]=lVal2;			
			m_vAddonProperties.push_back(addonProperty);
			}				
		}
		
	}


	//maker name
	char pszMakerName[256];
	_GetStringFromByteArray(pStream,lPos,pszMakerName);
	SetMakerName(pszMakerName);

	//enchase data
	LONG lActiveHoleNum=_GetLongFromByteArray(pStream,lPos);
	if(lActiveHoleNum)
	{
		LONG lHoleIdx=_GetLongFromByteArray(pStream,lPos);
		if(lHoleIdx==-1)
		{
			SetEnchaseHoleState(0,FALSE);
		}
		else if(lHoleIdx==0)
		{
			SetEnchaseHoleState(0,TRUE);
		}
		else if(lHoleIdx>0)
		{
			SetEnchaseHoleState(0,TRUE);
			CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx,16);
			Enchase(pCard,0);
		}

		lHoleIdx=_GetLongFromByteArray(pStream,lPos);
		if(lHoleIdx==-1)
		{
			SetEnchaseHoleState(1,FALSE);
		}
		else if(lHoleIdx==0)
		{
			SetEnchaseHoleState(1,TRUE);
		}
		else if(lHoleIdx>0)
		{
			SetEnchaseHoleState(1,TRUE);
			CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx,17);
			Enchase(pCard,1);
		}

		lHoleIdx=_GetLongFromByteArray(pStream,lPos);
		if(lHoleIdx==-1)
		{
			SetEnchaseHoleState(2,FALSE);
		}
		else if(lHoleIdx==0)
		{
			SetEnchaseHoleState(2,TRUE);
		}
		else if(lHoleIdx>0)
		{
			SetEnchaseHoleState(2,TRUE);
			CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx,18);
			Enchase(pCard,2);
		}

		lHoleIdx=_GetLongFromByteArray(pStream,lPos);
		if(lHoleIdx==-1)
		{
			SetEnchaseHoleState(3,FALSE);
		}
		else if(lHoleIdx==0)
		{
			SetEnchaseHoleState(3,TRUE);
		}
		else if(lHoleIdx>0)
		{
			SetEnchaseHoleState(3,TRUE);
			CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx,19);
			Enchase(pCard,3);
		}

		lHoleIdx=_GetLongFromByteArray(pStream,lPos);
		if(lHoleIdx==-1)
		{
			SetEnchaseHoleState(4,FALSE);
		}
		else if(lHoleIdx==0)
		{
			SetEnchaseHoleState(4,TRUE);
		}
		else if(lHoleIdx>0)
		{
			SetEnchaseHoleState(4,TRUE);
			CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx,20);
			Enchase(pCard,4);
		}
	}

}

BOOL CGoods::AddAddonPropertyBaseValues(GOODS_ADDON_PROPERTIES gapType,DWORD dwID,LONG lVal1,LONG lVal2,BOOL bEnable,BOOL bIsImplicit)
{	
	
	if(dwID!=1 && dwID!=2)
		return FALSE;
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		if( m_vAddonProperties[i].gapType == gapType )
		{
			return FALSE;		
		}
	}
	CGoods::tagAddonProperty stPro;
	stPro.gapType=gapType;
	stPro.lValues[dwID-1]=lVal1;	
	m_vAddonProperties.push_back(stPro);
	return TRUE;
}


VOID CGoods::EnchaseArrange()
{	
	vector<CGoods*> vecGoods;
	vecGoods.clear();

	
	DWORD dwNum=m_nInitHoleNum;
	DWORD dwActived=GetActivedHoleNum();
	for(DWORD i=0;i<dwNum;i++)
	{
		if(m_pEnchaseHoleArray[i]->GetState() && 
			m_pEnchaseHoleArray[i]->GetCard())
		{
			vecGoods.push_back(m_pEnchaseHoleArray[i]->GetCard());
		}
		m_pEnchaseHoleArray[i]->SetState(FALSE);
		m_pEnchaseHoleArray[i]->SetCard(NULL);
	}	
	
	for(DWORD i=0;i<dwNum;i++)
	{
		if(i<dwActived)
		{
			m_pEnchaseHoleArray[i]->SetState(TRUE);
		}
	}
	vector<CGoods*>::iterator goodsIter=vecGoods.begin();
	int j=0;
	for(;goodsIter!=vecGoods.end();goodsIter++)
	{
		m_pEnchaseHoleArray[j]->SetCard(*goodsIter);
		j++;
	}
}

//##ModelId=41A6A059036B
VOID CGoods::GetValidAddonProperties(vector<GOODS_ADDON_PROPERTIES>& vOut)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{		
		vOut.push_back( m_vAddonProperties[i].gapType );		
	}
}


//##ModelId=41AD1D4000FA
VOID CGoods::GetAddonPropertyValues(GOODS_ADDON_PROPERTIES gapType, 
									vector<LONG>& vOut)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		if( m_vAddonProperties[i].gapType == gapType )
		{
			vOut.push_back(m_vAddonProperties[i].lValues[0]);
			vOut.push_back(m_vAddonProperties[i].lValues[1]);			
			break;
		}
	}
}


//##ModelId=41AEB13D0196
LONG CGoods::GetAddonPropertyValues(GOODS_ADDON_PROPERTIES gapType, DWORD dwID)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	LONG lResult = 0;
	if(dwID!=1 && dwID!=2)
		return lResult;
	BOOL bFound=FALSE;
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		if( m_vAddonProperties[i].gapType == gapType )
		{
			lResult=m_vAddonProperties[i].lValues[dwID-1];
			bFound=TRUE;
			break;			
		}
	}

	if(!bFound)
	{
		CGoodsBaseProperties* pProperty=CGoodsFactory::QueryGoodsBaseProperties(GetBasePropertiesIndex());
		if(pProperty)
		{
			lResult=pProperty->GetAddonPropertyValue(gapType,dwID);
		}
	}
	return lResult;
}

bool CGoods::IsAddonProperyExist(GOODS_ADDON_PROPERTIES gapType)
{
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		if( m_vAddonProperties[i].gapType == gapType )
		{
			return true;
		}
	}
	return false;
}

//##ModelId=41A6961A00AB
VOID CGoods::AI()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
}

//##ModelId=41A696A3034B
BOOL CGoods::Clone(CBaseObject* pObject)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		vector<BYTE> vData;
		LONG lOffset = 0;

		Serialize( &vData );
		pGoods -> Unserialize( &vData[0], lOffset );

		return TRUE;
	}
	return FALSE;
}

//##ModelId=41A696BB02BF
BOOL CGoods::Serialize(vector<BYTE>* pStream, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CShape::AddToByteArray( pStream, b==0?false:true );

	_AddToByteArray( pStream, m_dwBasePropertiesIndex );
	_AddToByteArray( pStream, m_dwAmount );
	_AddToByteArray( pStream, m_dwPrice );
	_AddToByteArray(pStream,m_dwBuyPrice);
	_AddToByteArray(pStream,(DWORD)m_GoodsBaseType);
	
	_AddToByteArray( pStream, const_cast<CHAR*>( m_strMakerName.c_str() ) );

	CGoodsBaseProperties* pBaseProperty=CGoodsFactory::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
	if(!pBaseProperty)
		return FALSE;
	DWORD dwAddonNum=0;
	DWORD dwPropertyId=0;

	vector<BYTE> vecAddonData;
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{		
		if(pBaseProperty->IsHasAddonPropertie(m_vAddonProperties[i].gapType))	
		{			
			dwPropertyId=m_vAddonProperties[i].gapType;
			if(CGoodsFactory::s_GoodsAttrDBSetup[dwPropertyId][1]==1)
			{
				dwAddonNum++;
				m_vAddonProperties[i].Serialize(&vecAddonData);
			}
		}
	}
	_AddToByteArray( pStream,dwAddonNum);
	if(dwAddonNum>0)
	{
		_AddToByteArray(pStream,&vecAddonData[0],vecAddonData.size());
	}
	

	_AddToByteArray( pStream, static_cast<DWORD>( m_vSuitProperties.size() ) );
	for( size_t i = 0; i < m_vSuitProperties.size(); i ++ )
	{
		m_vSuitProperties[i].Serialize( pStream );
	}


	EnchaseArrange();
	//enchase data	
	
	DWORD dwMaxHoleNum=m_nInitHoleNum;
	_AddToByteArray(pStream,dwMaxHoleNum);
	//激活孔数量
	_AddToByteArray( pStream, GetActivedHoleNum());
	_AddToByteArray(pStream,m_dwActivedKitCardId);
	for(int i=0;i<dwMaxHoleNum;i++)
	{
		//状态
		if(m_pEnchaseHoleArray[i]->GetState())
		{
			_AddToByteArray(pStream,BYTE(1));
		}
		else
		{
			_AddToByteArray(pStream,BYTE(0));
		}
		//CARD ID
		CGoods* pCard=m_pEnchaseHoleArray[i]->GetCard();
		if(pCard)
		{
			_AddToByteArray(pStream,pCard->GetBasePropertiesIndex());
			pCard->Serialize(pStream);
		}
		else
		{
			_AddToByteArray(pStream,DWORD(0));
		}
	}
	return TRUE;
}

BOOL CGoods::Unserialize(BYTE* pStream, LONG& lOffset, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	Release();

	CHAR szString[256];

	CShape::DecordFromByteArray( pStream, lOffset, b==0?false:true );

	m_dwBasePropertiesIndex	= _GetDwordFromByteArray( pStream, lOffset );
	CGoodsBaseProperties* pProperty=CGoodsFactory::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
	if(pProperty)
	{
		m_dwSilverPrice=pProperty->GetSilverPrice();
	}
	m_dwAmount				= _GetDwordFromByteArray( pStream, lOffset );
	m_dwPrice				= _GetDwordFromByteArray( pStream, lOffset );
	m_dwBuyPrice=_GetDwordFromByteArray( pStream, lOffset );
	m_GoodsBaseType=static_cast<eGoodsBaseType>(_GetDwordFromByteArray(pStream,lOffset));

	

	//##获取物品描述
	ZeroMemory( szString, 256 );
	_GetStringFromByteArray( pStream, lOffset, szString );
	m_strMakerName			= szString;

	m_vAddonProperties.clear();
	CGoodsBaseProperties* pBaseProperty=CGoodsFactory::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
	if(!pBaseProperty)
		return FALSE;
	
	CGoodsFactory::CreateAddonProperties(this);

	//##获取物品扩展属性
	DWORD dwNumProperties	= _GetDwordFromByteArray( pStream, lOffset );
	for( DWORD i = 0; i < dwNumProperties; i ++ )
	{
		CGoods::tagAddonProperty apAddonProperty;
		apAddonProperty.Unserialize( pStream, lOffset );		
		if(pBaseProperty->IsHasAddonPropertie(apAddonProperty.gapType))
		{			
			for(int i=0;i<m_vAddonProperties.size();i++)
			{
				if(m_vAddonProperties[i].gapType==apAddonProperty.gapType)
				{
					m_vAddonProperties[i].lValues[0]=apAddonProperty.lValues[0];
					m_vAddonProperties[i].lValues[1]=apAddonProperty.lValues[1];
				}
			}			
		}
		
	}

	dwNumProperties	= _GetDwordFromByteArray( pStream, lOffset );
	for( DWORD i = 0; i < dwNumProperties; i ++ )
	{
		CGoodsBaseProperties::tagSuitProperty apAddonProperty;
		apAddonProperty.Unserialize( pStream, lOffset );
		m_vSuitProperties.push_back( apAddonProperty );
	}
	
	InitEnchaseHole();


	DWORD dwMaxHoleNum=_GetDwordFromByteArray(pStream,lOffset);
	DWORD dwActivedHoleNum=_GetDwordFromByteArray( pStream, lOffset );
	m_dwActivedKitCardId=_GetDwordFromByteArray( pStream, lOffset );
	for(int i=0;i<dwMaxHoleNum;i++)
	{		
		BYTE btState=_GetByteFromByteArray(pStream,lOffset);
		
		if(btState==1)
		{
			if( i< m_nInitHoleNum)
				m_pEnchaseHoleArray[i]->SetState(TRUE);
		}
		else
		{
			if( i< m_nInitHoleNum)
				m_pEnchaseHoleArray[i]->SetState(FALSE);
		}
		//CARD ID
		DWORD dwGoodsId=_GetDwordFromByteArray(pStream,lOffset);		

		if(dwGoodsId>0)
		{
#ifdef __MEMORY_LEAK_CHECK__
			CBaseObject::SetConstructFlag(82);
#endif
			CGoods* pGoods = CGoodsFactory::UnserializeGoods( pStream, lOffset,50 );
			if( i< m_nInitHoleNum)
				m_pEnchaseHoleArray[i]->SetCard(pGoods);
		}
		else
		{
			
		}
	}
	return TRUE;
}


BOOL CGoods::Serialize(DBWriteSet& setWriteDB, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CShape::CodeToDataBlock( setWriteDB, b==FALSE?false:true );

	setWriteDB.AddToByteArray( m_dwBasePropertiesIndex );
	setWriteDB.AddToByteArray( m_dwAmount );
	setWriteDB.AddToByteArray( m_dwPrice );
	setWriteDB.AddToByteArray( m_dwBuyPrice);
	setWriteDB.AddToByteArray( (DWORD)m_GoodsBaseType);
	setWriteDB.AddToByteArray( const_cast<CHAR*>( m_strMakerName.c_str() ) );		


	CGoodsBaseProperties* pBaseProperty=CGoodsFactory::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
	if(!pBaseProperty)
		return FALSE;
	DWORD dwAddonNum=0;
	DWORD dwPropertyId=0;
	vector<BYTE> vecAddonData;
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{		
		if(pBaseProperty->IsHasAddonPropertie(m_vAddonProperties[i].gapType))	
		{			
			dwPropertyId=m_vAddonProperties[i].gapType;
			if(CGoodsFactory::s_GoodsAttrDBSetup[dwPropertyId][0]==1)
			{
				dwAddonNum++;
				m_vAddonProperties[i].Serialize(&vecAddonData);
			}
		}		
	}
	setWriteDB.AddToByteArray(dwAddonNum);
	if(dwAddonNum>0)
	{
		setWriteDB.AddToByteArray(&vecAddonData[0],vecAddonData.size());
	}
	

	setWriteDB.AddToByteArray( static_cast<DWORD>( m_vSuitProperties.size() ) );
	for( size_t i = 0; i < m_vSuitProperties.size(); i ++ )
	{
		m_vSuitProperties[i].Serialize( setWriteDB );
	}


	EnchaseArrange();
	//enchase data	
	
	DWORD dwMaxHoleNum=m_nInitHoleNum;
	setWriteDB.AddToByteArray(dwMaxHoleNum);
	//激活孔数量
	setWriteDB.AddToByteArray( GetActivedHoleNum());
	setWriteDB.AddToByteArray( m_dwActivedKitCardId);
	for(int i=0;i<dwMaxHoleNum;i++)
	{
		//状态
		if(m_pEnchaseHoleArray[i]->GetState())
		{
			setWriteDB.AddToByteArray(BYTE(1));
		}
		else
		{
			setWriteDB.AddToByteArray(BYTE(0));
		}
		//CARD ID
		CGoods* pCard=m_pEnchaseHoleArray[i]->GetCard();
		if(pCard)
		{
			setWriteDB.AddToByteArray(pCard->GetBasePropertiesIndex());
			pCard->Serialize(setWriteDB);
		}
		else
		{
			setWriteDB.AddToByteArray(DWORD(0));
		}
	}
	return TRUE;
}
BOOL CGoods::Unserialize(DBReadSet& setReadDB, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	Release();
	CHAR szString[256];
	CShape::DecodeFromDataBlock( setReadDB, b==FALSE?false:true );

	m_dwBasePropertiesIndex	= setReadDB.GetDwordFromByteArray(  );
	CGoodsBaseProperties* pProperty=CGoodsFactory::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
	if(pProperty)
	{
		m_dwSilverPrice=pProperty->GetSilverPrice();
	}
	m_dwAmount				= setReadDB.GetDwordFromByteArray(  );
	m_dwPrice				= setReadDB.GetDwordFromByteArray(  );
	m_dwBuyPrice=setReadDB.GetDwordFromByteArray(  );
	m_GoodsBaseType=static_cast<eGoodsBaseType>(setReadDB.GetDwordFromByteArray());

	//##获取物品描述
	ZeroMemory( szString, 256 );
	setReadDB.GetStringFromByteArray( szString ,256);
	m_strMakerName			= szString;

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"goods base pro decode ok");
#endif

	m_vAddonProperties.clear();
	CGoodsBaseProperties* pBaseProperty=CGoodsFactory::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
	if(!pBaseProperty)
		return FALSE;
	

	//##获取物品扩展属性
	DWORD dwNumProperties	= setReadDB.GetDwordFromByteArray(  );
	for( DWORD i = 0; i < dwNumProperties; i ++ )
	{
		CGoods::tagAddonProperty apAddonProperty;
		apAddonProperty.Unserialize( setReadDB );
		m_vAddonProperties.push_back(apAddonProperty);		
	}	

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"goods addon pro decode ok");
#endif


	dwNumProperties	= setReadDB.GetDwordFromByteArray(  );
	for( DWORD i = 0; i < dwNumProperties; i ++ )
	{
		CGoodsBaseProperties::tagSuitProperty apAddonProperty;
		apAddonProperty.Unserialize( setReadDB );
		m_vSuitProperties.push_back( apAddonProperty );
	}

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"goods suit pro decode");
#endif

	InitEnchaseHole();


	DWORD dwMaxHoleNum=setReadDB.GetDwordFromByteArray();
	DWORD dwActivedHoleNum=setReadDB.GetDwordFromByteArray( );
	m_dwActivedKitCardId=setReadDB.GetDwordFromByteArray( );
	for(int i=0;i<dwMaxHoleNum;i++)
	{	BYTE btState=setReadDB.GetByteFromByteArray( );
		
		if(btState==1)
		{
			if( i< m_nInitHoleNum)
				m_pEnchaseHoleArray[i]->SetState(TRUE);
		}
		else
		{
			if( i< m_nInitHoleNum)
				m_pEnchaseHoleArray[i]->SetState(FALSE);
		}
		//CARD ID
		DWORD dwGoodsId=setReadDB.GetDwordFromByteArray( );		

		if(dwGoodsId>0)
		{
#ifdef __MEMORY_LEAK_CHECK__
			CBaseObject::SetConstructFlag(82);
#endif
			CGoods* pGoods = CGoodsFactory::UnserializeGoods( setReadDB,51);
			if( i< m_nInitHoleNum)
				m_pEnchaseHoleArray[i]->SetCard(pGoods);
			else
				CGoodsFactory::GarbageCollect(&pGoods);
		}
		else
		{
			
		}
	}
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame()->GetStatckFileName(),"goods card pro decode");
#endif
	return TRUE;
}

//##ModelId=41A6977C03D8
const CHAR* CGoods::ToString()
{
	// TODO: Add your specialized code here.
	return (const CHAR*)0;
}

//##ModelId=41A6979401D4
VOID CGoods::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	EnchaseRelease();
	m_dwBasePropertiesIndex	= 0;
	m_dwAmount				= 0;
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		m_vAddonProperties[i].Clear();
	}
	m_vAddonProperties.clear();
}

//##ModelId=41A692BE0290
VOID CGoods::SetBasePropertiesIndex(DWORD dwID)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_dwBasePropertiesIndex	= dwID;
}

//##ModelId=41AC0E880213
CGoods::tagAddonProperty::tagAddonProperty()
{
	// TODO: Add your specialized code here.
	gapType					= GAP_UNKNOW;
	ZeroMemory(lValues,2*sizeof(long));
}

//##ModelId=41AC0E900290
CGoods::tagAddonProperty::~tagAddonProperty()
{
	// TODO: Add your specialized code here.
	Clear();
}

//##ModelId=41AC0E92032C
VOID CGoods::tagAddonProperty::Clear()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	gapType					= GAP_UNKNOW;
	ZeroMemory(lValues,2*sizeof(long));
}

//##ModelId=41AC0E9601C5
BOOL CGoods::tagAddonProperty::Serialize(vector<BYTE>* pStream, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	_AddToByteArray( pStream, static_cast<DWORD>(gapType) );
	_AddToByteArray(pStream,lValues[0]);
	_AddToByteArray(pStream,lValues[1]);
	return TRUE;
}

//##ModelId=41AC0EAC038A
BOOL CGoods::tagAddonProperty::Unserialize(BYTE* pStream, LONG& lOffset, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	gapType					= static_cast<GOODS_ADDON_PROPERTIES>( 
		_GetDwordFromByteArray(pStream, lOffset) );
	lValues[0]=_GetLongFromByteArray(pStream,lOffset);
	lValues[1]=_GetLongFromByteArray(pStream,lOffset);
	return TRUE;
}

BOOL CGoods::tagAddonProperty::Serialize(DBWriteSet& setWriteDB, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	setWriteDB.AddToByteArray( static_cast<DWORD>(gapType) );
	setWriteDB.AddToByteArray(lValues[0]);
	setWriteDB.AddToByteArray(lValues[1]);
	return TRUE;
}
BOOL CGoods::tagAddonProperty::Unserialize(DBReadSet& setReadDB, BOOL b)
{
	gapType					= static_cast<GOODS_ADDON_PROPERTIES>( 
		setReadDB.GetDwordFromByteArray() );

	lValues[0]=setReadDB.GetLongFromByteArray();
	lValues[1]=setReadDB.GetLongFromByteArray();
	return TRUE;
}

//##ModelId=41B809B6014F
VOID CGoods::SetPrice(DWORD dwPrice)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_dwPrice = dwPrice;
}

//##ModelId=41B809CB0298
DWORD CGoods::GetPrice()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return m_dwPrice;
}

//##ModelId=41B80BB60332
DWORD CGoods::GetTotalPrice()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return m_dwPrice * m_dwAmount;
}


//##ModelId=41B8088C01F3
VOID CGoods::SetGoodsName(const CHAR* strGoodsName)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CShape::SetName( strGoodsName );
}

//##ModelId=41B808A502DD
const CHAR* CGoods::GetGoodsName()
{
	// TODO: Add your specialized code here.
	return CShape::GetName();
}

vector<CGoods::tagAddonProperty>& CGoods::GetAllAddonProperties()
{
	return m_vAddonProperties;
}