// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "Stdafx.h"
#include "CGoodsBaseProperties.h"




CGoodsBaseProperties::CGoodsBaseProperties()
{
	// TODO: Add your specialized code here.
	m_dwPrice			= 0;	
	m_dwSilverPrice=0;
	m_gtGoodsType		= GT_USELESS;
	m_epEquipPlace		= EP_UNKNOW;
}

CGoodsBaseProperties::~CGoodsBaseProperties()
{
	// TODO: Add your specialized code here.
}

const CHAR* CGoodsBaseProperties::GetOriginalName()
{
	// TODO: Add your specialized code here.
	return m_strOriginalName.c_str();
}

const CHAR* CGoodsBaseProperties::GetName()
{
	// TODO: Add your specialized code here.
	return m_strName.c_str();
}

const CHAR* CGoodsBaseProperties::GetDescribe()
{
	// TODO: Add your specialized code here.
	return m_strDescribe.c_str();
}

DWORD CGoodsBaseProperties::GetPrice()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return m_dwPrice;
}

CGoodsBaseProperties::GOODS_TYPE CGoodsBaseProperties::GetGoodsType()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return m_gtGoodsType;
}

BOOL CGoodsBaseProperties::CanEquip()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( m_gtGoodsType == GT_EQUIPMENT &&
		m_epEquipPlace != EP_UNKNOW )
	{
		return TRUE;
	}
	return FALSE;
}

CGoodsBaseProperties::EQUIP_PLACE CGoodsBaseProperties::GetEquipPlace()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return m_epEquipPlace;
}

VOID CGoodsBaseProperties::Clear()
{
	m_dwPrice			= 0;
	
	m_gtGoodsType		= GT_USELESS;
	m_epEquipPlace		= EP_UNKNOW;

	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{	
		m_vAddonProperties[i].vValues.clear();
	}

	for(size_t i=0;i<m_vSuitProperties.size();i++)
	{
		m_vSuitProperties[i].vValues.clear();
	}

	m_vAddonProperties.clear();
	m_vSuitProperties.clear();
	m_vTriggerProperties.clear();
	m_vIcons.clear();
}

//##ModelId=41A5A48902CE
BOOL CGoodsBaseProperties::Serialize(vector<BYTE>* pStream, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	_AddToByteArray( pStream, const_cast<CHAR*>( m_strOriginalName.c_str() ) );
	_AddToByteArray( pStream, const_cast<CHAR*>( m_strName.c_str() ) );
	_AddToByteArray( pStream, static_cast<DWORD>(m_gtGoodsType) );
	_AddToByteArray( pStream, static_cast<DWORD>(m_epEquipPlace) );
	_AddToByteArray( pStream, m_dwPrice );
	_AddToByteArray(pStream,m_dwSilverPrice);
	_AddToByteArray(pStream,(DWORD)m_GoodsBaseType);
	
	//##添加图标
	_AddToByteArray( pStream, static_cast<DWORD>( m_vIcons.size() ) );
	for( size_t i = 0; i < m_vIcons.size(); i ++ )
	{
		_AddToByteArray( pStream, static_cast<DWORD>( m_vIcons[i].itIconType ) );
		_AddToByteArray( pStream,  m_vIcons[i].dwIconID );
	}
	//##添加附加属性的结构.
	_AddToByteArray( pStream, static_cast<DWORD>( m_vAddonProperties.size() ) );
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		m_vAddonProperties[i].Serialize( pStream );
	}

	//套装
	_AddToByteArray( pStream, static_cast<DWORD>( m_vSuitProperties.size() ) );
	
	for( size_t i = 0; i < m_vSuitProperties.size(); i ++ )
	{
		m_vSuitProperties[i].Serialize( pStream );
	}

	//属性激发
	_AddToByteArray(pStream,static_cast<WORD>(m_vTriggerProperties.size()));
	for(size_t i=0;i<m_vTriggerProperties.size();i++)
	{
		_AddToByteArray(pStream,static_cast<WORD>(m_vTriggerProperties[i].gapType));
		_AddToByteArray(pStream,m_vTriggerProperties[i].lVal1);
		_AddToByteArray(pStream,m_vTriggerProperties[i].lVal2);
		_AddToByteArray(pStream,m_vTriggerProperties[i].wTriggerLevel);
	}
	return TRUE;
}

//##ModelId=41A5A494037A
BOOL CGoodsBaseProperties::Unserialize( BYTE* pStream, LONG& lOffset, BOOL b )
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CHAR strTemp[1024];

	Clear();
	m_strOriginalName			= _GetStringFromByteArray( pStream, lOffset, strTemp );
	m_strName					= _GetStringFromByteArray( pStream, lOffset, strTemp );
	m_gtGoodsType				= static_cast<GOODS_TYPE>( _GetDwordFromByteArray(pStream, lOffset) );
	m_epEquipPlace				= static_cast<EQUIP_PLACE>( _GetDwordFromByteArray(pStream, lOffset) );
	m_dwPrice					= _GetDwordFromByteArray( pStream, lOffset );
	m_dwSilverPrice=_GetDwordFromByteArray(pStream,lOffset);
	m_GoodsBaseType=static_cast<eGoodsBaseType>(_GetDwordFromByteArray(pStream,lOffset));
	
	//##获取图标
	DWORD dwNumIconType			= _GetDwordFromByteArray( pStream, lOffset );
	for( size_t i = 0; i < dwNumIconType; i ++ )
	{
		tagIcon iIcon;
		iIcon.itIconType		= static_cast<ICON_TYPE>( _GetDwordFromByteArray(pStream, lOffset) );
		iIcon.dwIconID			= _GetDwordFromByteArray( pStream, lOffset );

		m_vIcons.push_back( iIcon );
	}
	//##获取附加属性
	DWORD dwNumAddonProperties	= _GetDwordFromByteArray( pStream, lOffset );
	for( size_t i = 0; i < dwNumAddonProperties; i ++ )
	{
		tagAddonProperty apAddonProperty;
		apAddonProperty.Unserialize( pStream, lOffset );
		m_vAddonProperties.push_back( apAddonProperty );
	}

	//套装属性
	dwNumAddonProperties	= _GetDwordFromByteArray( pStream, lOffset );
	for( size_t i = 0; i < dwNumAddonProperties; i ++ )
	{
		tagSuitProperty apAddonProperty;
		apAddonProperty.Unserialize( pStream, lOffset );
		m_vSuitProperties.push_back( apAddonProperty );
	}

	//道具属性激发
	m_vTriggerProperties.clear();
	WORD wTriggerNum=_GetWordFromByteArray(pStream,lOffset);
	for(WORD i=0;i<wTriggerNum;i++)
	{
		tagTriggerProperty stTriggerProperty;
		stTriggerProperty.gapType=static_cast<GOODS_ADDON_PROPERTIES>(_GetWordFromByteArray(pStream,lOffset));
		stTriggerProperty.lVal1=_GetLongFromByteArray(pStream,lOffset);
		stTriggerProperty.lVal2=_GetLongFromByteArray(pStream,lOffset);
		stTriggerProperty.wTriggerLevel=_GetWordFromByteArray(pStream,lOffset);
		m_vTriggerProperties.push_back(stTriggerProperty);
	}

	return TRUE;
}

BOOL CGoodsBaseProperties::Serialize(DBWriteSet& setWriteDB, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	setWriteDB.AddToByteArray( const_cast<CHAR*>( m_strOriginalName.c_str() ) );
	setWriteDB.AddToByteArray( const_cast<CHAR*>( m_strName.c_str() ) );
	setWriteDB.AddToByteArray( static_cast<DWORD>(m_gtGoodsType) );
	setWriteDB.AddToByteArray( static_cast<DWORD>(m_epEquipPlace) );
	setWriteDB.AddToByteArray( m_dwPrice );
	setWriteDB.AddToByteArray( m_dwSilverPrice);
	setWriteDB.AddToByteArray((DWORD)m_GoodsBaseType);

	//添加图标
	setWriteDB.AddToByteArray( static_cast<DWORD>( m_vIcons.size() ) );
	for( size_t i = 0; i < m_vIcons.size(); i ++ )
	{
		setWriteDB.AddToByteArray( static_cast<DWORD>( m_vIcons[i].itIconType ) );
		setWriteDB.AddToByteArray(  m_vIcons[i].dwIconID );
	}
	//添加附加属性的结构
	setWriteDB.AddToByteArray( static_cast<DWORD>( m_vAddonProperties.size() ) );
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		m_vAddonProperties[i].Serialize( setWriteDB );
	}

	//套装
	setWriteDB.AddToByteArray( static_cast<DWORD>( m_vSuitProperties.size() ) );
	
	for( size_t i = 0; i < m_vSuitProperties.size(); i ++ )
	{
		m_vSuitProperties[i].Serialize( setWriteDB );
	}

	//属性激发
	setWriteDB.AddToByteArray(static_cast<WORD>(m_vTriggerProperties.size()));
	for(size_t i=0;i<m_vTriggerProperties.size();i++)
	{
		setWriteDB.AddToByteArray(static_cast<WORD>(m_vTriggerProperties[i].gapType));
		setWriteDB.AddToByteArray(m_vTriggerProperties[i].lVal1);
		setWriteDB.AddToByteArray(m_vTriggerProperties[i].lVal2);
		setWriteDB.AddToByteArray(m_vTriggerProperties[i].wTriggerLevel);
	}

	return TRUE;
}
	
BOOL CGoodsBaseProperties::Unserialize(DBReadSet& setReadDB, BOOL b)
{
	// TODO: Add your specialized code here.

	CHAR strTemp[1024];
	Clear();
	m_strOriginalName			= setReadDB.GetStringFromByteArray( strTemp ,1024);
	m_strName					= setReadDB.GetStringFromByteArray( strTemp ,1024);
	DWORD dwGoodsType=setReadDB.GetDwordFromByteArray();
	m_gtGoodsType				= static_cast<GOODS_TYPE>( dwGoodsType );
	DWORD dwEquipPlace=setReadDB.GetDwordFromByteArray();
	m_epEquipPlace				= static_cast<EQUIP_PLACE>( dwEquipPlace );
	m_dwPrice					= setReadDB.GetDwordFromByteArray(  );
	m_dwSilverPrice=setReadDB.GetDwordFromByteArray();
	m_GoodsBaseType=static_cast<eGoodsBaseType>(setReadDB.GetDwordFromByteArray());

	//##获取图标
	DWORD dwNumIconType			= setReadDB.GetDwordFromByteArray(  );
	for( size_t i = 0; i < dwNumIconType; i ++ )
	{
		tagIcon iIcon;
		iIcon.itIconType		= static_cast<ICON_TYPE>( setReadDB.GetDwordFromByteArray() );
		iIcon.dwIconID			= setReadDB.GetDwordFromByteArray(  );

		m_vIcons.push_back( iIcon );
	}
	//##获取附加属性
	DWORD dwNumAddonProperties	= setReadDB.GetDwordFromByteArray(  );
	for( size_t i = 0; i < dwNumAddonProperties; i ++ )
	{
		tagAddonProperty apAddonProperty;
		apAddonProperty.Unserialize( setReadDB );
		m_vAddonProperties.push_back( apAddonProperty );
	}

	//套装属性
	dwNumAddonProperties	= setReadDB.GetDwordFromByteArray(  );
	for( size_t i = 0; i < dwNumAddonProperties; i ++ )
	{
		tagSuitProperty apAddonProperty;
		apAddonProperty.Unserialize( setReadDB );
		m_vSuitProperties.push_back( apAddonProperty );
	}

	//道具属性激发
	m_vTriggerProperties.clear();
	WORD wTriggerNum=setReadDB.GetWordFromByteArray();
	for(WORD i=0;i<wTriggerNum;i++)
	{
		tagTriggerProperty stTriggerProperty;
		stTriggerProperty.gapType=static_cast<GOODS_ADDON_PROPERTIES>(setReadDB.GetWordFromByteArray());
		stTriggerProperty.lVal1=setReadDB.GetLongFromByteArray();
		stTriggerProperty.lVal2=setReadDB.GetLongFromByteArray();
		stTriggerProperty.wTriggerLevel=setReadDB.GetWordFromByteArray();
		m_vTriggerProperties.push_back(stTriggerProperty);
	}

	return TRUE;
}

DWORD CGoodsBaseProperties::GetIconID( ICON_TYPE itType )
{
	DWORD dwResult = 0;
	for( size_t i = 0; i < m_vIcons.size(); i ++ )
	{
		if( m_vIcons[i].itIconType == itType )
		{
			dwResult = m_vIcons[i].dwIconID;
			break;
		}
	}
	return dwResult;
}


//DWORD CGoodsBaseProperties::GetOccurProbability(GOODS_ADDON_PROPERTIES gapType)
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	DWORD dwResult = 0;
//	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
//	{
//		if( m_vAddonProperties[i].gapType == gapType )
//		{
//			dwResult = m_vAddonProperties[i].dwOccurProbability;
//			break;
//		}
//	}
//	return dwResult;
//}

BOOL CGoodsBaseProperties::IsImplicit(GOODS_ADDON_PROPERTIES gapType)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		if( m_vAddonProperties[i].gapType == gapType )
		{
			bResult = m_vAddonProperties[i].bIsImplicitAttribute;
			break;
		}
	}
	return bResult;
}


//是否有这个附加属性
BOOL CGoodsBaseProperties::IsHasAddonPropertie(GOODS_ADDON_PROPERTIES gapType)
{
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		if( m_vAddonProperties[i].gapType == gapType )
		{
			return TRUE;
		}
	}
	return FALSE;
}


//##ModelId=41AAD39702AF
VOID CGoodsBaseProperties::GetAddonPropertyValues( GOODS_ADDON_PROPERTIES gapType, 
												  vector<CGoodsBaseProperties::tagAddonPropertyValue>& vOut )
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		if( m_vAddonProperties[i].gapType == gapType )
		{
			for( size_t j = 0; j < m_vAddonProperties[i].vValues.size(); j ++ )
			{
				vOut.push_back( m_vAddonProperties[i].vValues[j] );
			}
			break;
		}
	}
}

//##ModelId=41AC27F100DA
VOID CGoodsBaseProperties::GetValidAddonProperties(vector<GOODS_ADDON_PROPERTIES>& vOut)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		if( m_vAddonProperties[i].bIsEnabled == TRUE )
		{
			vOut.push_back( m_vAddonProperties[i].gapType );
		}
	}
}

DWORD CGoodsBaseProperties::GetAddonPropertyValue(GOODS_ADDON_PROPERTIES gapType,DWORD dwId)
{
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		if( m_vAddonProperties[i].gapType == gapType )
		{			
			if(dwId==1)
			{
				return m_vAddonProperties[i].vValues[0].lBaseValue;
			}
			else if(dwId==2)
			{
				return m_vAddonProperties[i].vValues[1].lBaseValue;
			}
		}
	}
	return 0;
}

//##ModelId=41AAB47F0177
CGoodsBaseProperties::tagAddonPropertyValue::tagAddonPropertyValue()
{
	// TODO: Add your specialized code here.
	dwId					= 0;
	lBaseValue				= 0;
	//bIsEnableModifier		= FALSE;
}

//##ModelId=41AAB4820251
CGoodsBaseProperties::tagAddonPropertyValue::~tagAddonPropertyValue()
{
	// TODO: Add your specialized code here.
	Clear();
}

//##ModelId=41AAB48501C5
VOID CGoodsBaseProperties::tagAddonPropertyValue::Clear()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	dwId					= 0;
	lBaseValue				= 0;
	/*bIsEnableModifier		= FALSE;

	for( size_t i = 0; i < vModifiers.size(); i ++ )
	{
		vModifiers[i].Clear();
	}

	vModifiers.clear();*/
}

//##ModelId=41AAB4B80242
BOOL CGoodsBaseProperties::tagAddonPropertyValue::Serialize(vector<BYTE>* pStream, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	_AddToByteArray( pStream, dwId );
	_AddToByteArray( pStream, lBaseValue );
	//_AddToByteArray( pStream, static_cast<LONG>(bIsEnableModifier) );
	//_AddToByteArray( pStream, static_cast<DWORD>( vModifiers.size() ) );
	//for( size_t i = 0; i < vModifiers.size(); i ++ )
	//{
	//	vModifiers[i].Serialize( pStream );
	//}
	return TRUE;
}

//##ModelId=41AAB4C300EA
BOOL CGoodsBaseProperties::tagAddonPropertyValue::Unserialize(BYTE* pStream, LONG& lOffset, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	Clear();

	dwId					= _GetDwordFromByteArray( pStream, lOffset );
	lBaseValue				= _GetLongFromByteArray( pStream, lOffset );
	/*bIsEnableModifier		= _GetLongFromByteArray( pStream, lOffset );

	DWORD dwNumAddonPropertyValueModifiers = _GetDwordFromByteArray( pStream, lOffset );
	for( size_t i = 0; i < dwNumAddonPropertyValueModifiers; i ++ )
	{
		tagAddonPropertyValueModifier apvmAddonPropertyValueModifier;
		apvmAddonPropertyValueModifier.Unserialize( pStream, lOffset );
		vModifiers.push_back( apvmAddonPropertyValueModifier );
	}*/

	return TRUE;
}


BOOL CGoodsBaseProperties::tagAddonPropertyValue::Serialize(DBWriteSet& setWriteDB, BOOL b )
{
	setWriteDB.AddToByteArray( dwId );
	setWriteDB.AddToByteArray( lBaseValue );
	return TRUE;
}
	
BOOL CGoodsBaseProperties::tagAddonPropertyValue::Unserialize(DBReadSet& setReadDB, BOOL b )
{
	Clear();
	dwId					= setReadDB.GetDwordFromByteArray( );
	lBaseValue				= setReadDB.GetLongFromByteArray( );
	return TRUE;
}


//##ModelId=41AAB2E301E4
CGoodsBaseProperties::tagAddonProperty::tagAddonProperty()
{
	// TODO: Add your specialized code here.
	gapType					= GAP_UNKNOW;
	bIsEnabled				= FALSE;
	bIsImplicitAttribute	= FALSE;
	//dwOccurProbability		= 0;
}

CGoodsBaseProperties::tagAddonProperty::~tagAddonProperty()
{
	// TODO: Add your specialized code here.
	Clear();
}

CGoodsBaseProperties::tagSuitProperty::tagSuitProperty()
{
	// TODO: Add your specialized code here.
	gapType					= GAP_UNKNOW;
	bActived=FALSE;
	//dwOccurProbability		= 0;
}

CGoodsBaseProperties::tagSuitProperty::~tagSuitProperty()
{
	// TODO: Add your specialized code here.
	Clear();
}

VOID CGoodsBaseProperties::tagSuitProperty::Clear()
{
	gapType=GAP_UNKNOW;
	bActived=FALSE;
	for(size_t i=0;i<vValues.size();i++)
	{
		vValues[i].Clear();
	}
	vValues.clear();
}


//##ModelId=41AAB2F003D8
VOID CGoodsBaseProperties::tagAddonProperty::Clear()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	gapType					= GAP_UNKNOW;
	bIsEnabled				= FALSE;
	bIsImplicitAttribute	= FALSE;
	//dwOccurProbability		= 0;

	for( size_t i = 0; i < vValues.size(); i ++ )
	{
		vValues[i].Clear();
	}

	vValues.clear();
}


BOOL CGoodsBaseProperties::tagSuitProperty::Serialize(vector<BYTE>* pStream, BOOL b)
{
	_AddToByteArray(pStream,static_cast<DWORD>(gapType));
	_AddToByteArray( pStream, static_cast<DWORD>( vValues.size() ) );
	for( size_t i = 0; i < vValues.size(); i ++ )
	{
		vValues[i].Serialize( pStream );
	}
	return TRUE;
}

//##ModelId=41AAB30D0109
BOOL CGoodsBaseProperties::tagSuitProperty::Unserialize(BYTE* pStream, LONG& lOffset, BOOL b)
{
	Clear();

	gapType					= static_cast<GOODS_ADDON_PROPERTIES>( _GetDwordFromByteArray(pStream, lOffset) );
	bActived=FALSE;
	DWORD dwNumAddonPropertyValues = _GetDwordFromByteArray( pStream, lOffset );
	for( size_t i = 0; i < dwNumAddonPropertyValues; i ++ )
	{
		tagAddonPropertyValue apvAddonPropertyValue;
		apvAddonPropertyValue.Unserialize( pStream, lOffset );
		vValues.push_back( apvAddonPropertyValue );
	}
	return TRUE;
}

//##ModelId=41AAB2F70196
BOOL CGoodsBaseProperties::tagAddonProperty::Serialize(vector<BYTE>* pStream, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	_AddToByteArray( pStream, static_cast<DWORD>(gapType) );
	_AddToByteArray( pStream, static_cast<LONG>(bIsEnabled) );
	_AddToByteArray( pStream, static_cast<LONG>(bIsImplicitAttribute) );
	//_AddToByteArray( pStream, dwOccurProbability );
	_AddToByteArray( pStream, static_cast<DWORD>( vValues.size() ) );
	for( size_t i = 0; i < vValues.size(); i ++ )
	{
		vValues[i].Serialize( pStream );
	}

	return TRUE;
}

//##ModelId=41AAB30D0109
BOOL CGoodsBaseProperties::tagAddonProperty::Unserialize(BYTE* pStream, LONG& lOffset, BOOL b)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	Clear();

	gapType					= static_cast<GOODS_ADDON_PROPERTIES>( _GetDwordFromByteArray(pStream, lOffset) );
	bIsEnabled				= _GetLongFromByteArray( pStream, lOffset );
	bIsImplicitAttribute	= _GetLongFromByteArray( pStream, lOffset );
	//dwOccurProbability		= _GetDwordFromByteArray( pStream, lOffset );

	DWORD dwNumAddonPropertyValues = _GetDwordFromByteArray( pStream, lOffset );
	for( size_t i = 0; i < dwNumAddonPropertyValues; i ++ )
	{
		tagAddonPropertyValue apvAddonPropertyValue;
		apvAddonPropertyValue.Unserialize( pStream, lOffset );
		vValues.push_back( apvAddonPropertyValue );
	}

	return TRUE;
}

BOOL CGoodsBaseProperties::tagAddonProperty::Serialize(DBWriteSet& setWriteDB, BOOL b)
{
	// TODO: Add your specialized code here.
	setWriteDB.AddToByteArray(static_cast<DWORD>(gapType) );
	setWriteDB.AddToByteArray(static_cast<LONG>(bIsEnabled) );
	setWriteDB.AddToByteArray(static_cast<LONG>(bIsImplicitAttribute) );
	setWriteDB.AddToByteArray(static_cast<DWORD>( vValues.size() ) );
	for( size_t i = 0; i < vValues.size(); i ++ )
	{
		vValues[i].Serialize( setWriteDB );
	}

	return TRUE;
}
		
BOOL CGoodsBaseProperties::tagAddonProperty::Unserialize(DBReadSet& setReadDB, BOOL b)
{
	// TODO: Add your specialized code here.
	Clear();
	gapType					= static_cast<GOODS_ADDON_PROPERTIES>( setReadDB.GetDwordFromByteArray() );
	bIsEnabled				= setReadDB.GetLongFromByteArray();
	bIsImplicitAttribute	= setReadDB.GetLongFromByteArray();
	DWORD dwNumAddonPropertyValues = setReadDB.GetDwordFromByteArray( );
	for( size_t i = 0; i < dwNumAddonPropertyValues; i ++ )
	{
		tagAddonPropertyValue apvAddonPropertyValue;
		apvAddonPropertyValue.Unserialize(setReadDB);
		vValues.push_back( apvAddonPropertyValue );
	}
	return TRUE;
}

BOOL CGoodsBaseProperties::tagSuitProperty::Serialize(DBWriteSet& setWriteDB, BOOL b )
{
	setWriteDB.AddToByteArray( static_cast<DWORD>(gapType));
	setWriteDB.AddToByteArray( static_cast<DWORD>( vValues.size() ) );
	for( size_t i = 0; i < vValues.size(); i ++ )
	{
		vValues[i].Serialize( setWriteDB );
	}
	return TRUE;
}
		
BOOL CGoodsBaseProperties::tagSuitProperty::Unserialize(DBReadSet& setReadDB, BOOL b )
{
	Clear();
	gapType					= static_cast<GOODS_ADDON_PROPERTIES>( setReadDB.GetDwordFromByteArray() );
	bActived=FALSE;
	DWORD dwNumAddonPropertyValues = setReadDB.GetDwordFromByteArray(  );
	for( size_t i = 0; i < dwNumAddonPropertyValues; i ++ )
	{
		tagAddonPropertyValue apvAddonPropertyValue;
		apvAddonPropertyValue.Unserialize( setReadDB);
		vValues.push_back( apvAddonPropertyValue );
	}
	return TRUE;
}

