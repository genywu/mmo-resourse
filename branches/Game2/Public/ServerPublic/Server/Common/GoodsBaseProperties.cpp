#include "StdAfx.h"
#include "GoodsBaseProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CGoodsBaseProperties::CGoodsBaseProperties()
{	
	m_dwPrice = 0;	
	m_dwSilverPrice = 0;
	m_GoodsBaseType = GBT_USELESS;
}

CGoodsBaseProperties::~CGoodsBaseProperties()
{	
}

const char* CGoodsBaseProperties::GetOriginalName()
{
	return m_strOriginalName.c_str();
}

const char* CGoodsBaseProperties::GetName()
{
	return m_strName.c_str();
}

const char* CGoodsBaseProperties::GetDescribe()
{	
	return m_strDescribe.c_str();
}

ulong CGoodsBaseProperties::GetPrice()
{
	return m_dwPrice;
}

CGoodsBaseProperties::tagSuitProperty::tagSuitProperty()
{
	gapType = GAP_UNKNOW;
}

CGoodsBaseProperties::tagSuitProperty::~tagSuitProperty()
{
	Clear();
}

void CGoodsBaseProperties::tagSuitProperty::Clear()
{
	gapType = GAP_UNKNOW;
	bActived = false;
	for(size_t i=0; i<vValues.size(); i++)
	{
		vValues[i].Clear();
	}
	vValues.clear();
}

bool CGoodsBaseProperties::tagSuitProperty::Serialize(vector<uchar>* pStream, bool b)
{
	_AddToByteArray(pStream,static_cast<ulong>(gapType));
	_AddToByteArray(pStream, static_cast<ulong>(vValues.size()));
	for(size_t i = 0; i < vValues.size(); i++)
	{
		vValues[i].Serialize(pStream);
	}
	return true;
}

bool CGoodsBaseProperties::tagSuitProperty::Unserialize(uchar* pStream, long& lOffset, bool b)
{
	Clear();

	gapType = static_cast<eGoodsAddonProperties>(_GetDwordFromByteArray(pStream, lOffset));
	bActived = false;
	ulong dwNumAddonPropertyValues = _GetDwordFromByteArray(pStream, lOffset);
	for(size_t i = 0; i < dwNumAddonPropertyValues; i++)
	{
		tagAddonPropertyValue apvAddonPropertyValue;
		apvAddonPropertyValue.Unserialize(pStream, lOffset);
		vValues.push_back(apvAddonPropertyValue);
	}
	return true;
}

bool CGoodsBaseProperties::tagSuitProperty::Serialize(DBWriteSet& setWriteDB, bool b )
{
	setWriteDB.AddToByteArray(static_cast<ulong>(gapType));
	setWriteDB.AddToByteArray(static_cast<ulong>(vValues.size()));
	for(size_t i = 0; i < vValues.size(); i++)
	{
		vValues[i].Serialize(setWriteDB);
	}
	return true;
}
		
bool CGoodsBaseProperties::tagSuitProperty::Unserialize(DBReadSet& setReadDB, bool b )
{
	Clear();
	gapType = static_cast<eGoodsAddonProperties>( setReadDB.GetDwordFromByteArray() );
	bActived = false;
	ulong dwNumAddonPropertyValues = setReadDB.GetDwordFromByteArray();
	for(size_t i = 0; i < dwNumAddonPropertyValues; i++)
	{
		tagAddonPropertyValue apvAddonPropertyValue;
		apvAddonPropertyValue.Unserialize(setReadDB);
		vValues.push_back(apvAddonPropertyValue);
	}
	return true;
}

void CGoodsBaseProperties::Clear()
{
	m_dwPrice = 0;
	m_dwSilverPrice = 0;
	m_GoodsBaseType = GBT_USELESS;

	for(size_t i = 0; i < m_vAddonProperties.size(); i++)
	{
		m_vAddonProperties[i].vValues.clear();
	}

	m_vAddonProperties.clear();
	m_vSuitProperties.clear();
	m_vTriggerProperties.clear();
	m_vIcons.clear();
}

bool CGoodsBaseProperties::Serialize(vector<uchar>* pStream, bool b)
{
	_AddToByteArray(pStream, m_index);
	_AddToByteArray(pStream, const_cast<char*>(m_strOriginalName.c_str()));
	_AddToByteArray(pStream, const_cast<char*>(m_strName.c_str()));
	_AddToByteArray(pStream, m_dwPrice);
	_AddToByteArray(pStream, m_dwSilverPrice);
	_AddToByteArray(pStream, (ulong)m_GoodsBaseType);

	//添加图标
	_AddToByteArray( pStream, static_cast<ulong>(m_vIcons.size()));
	for(size_t i = 0; i < m_vIcons.size(); i++)
	{
		_AddToByteArray(pStream, static_cast<ulong>(m_vIcons[i].itIconType));
		_AddToByteArray(pStream, m_vIcons[i].dwIconID);
	}
	//添加附加属性的结构
	_AddToByteArray(pStream, static_cast<ulong>(m_vAddonProperties.size()));
	for(size_t i = 0; i < m_vAddonProperties.size(); i++)
	{
		m_vAddonProperties[i].Serialize(pStream);
	}

	//套装
	_AddToByteArray(pStream, static_cast<ulong>(m_vSuitProperties.size()));
	
	for(size_t i = 0; i < m_vSuitProperties.size(); i++)
	{
		m_vSuitProperties[i].Serialize(pStream);
	}

	//属性激发
	_AddToByteArray(pStream, static_cast<ushort>(m_vTriggerProperties.size()));
	for(size_t i=0;i<m_vTriggerProperties.size();i++)
	{
		_AddToByteArray(pStream,static_cast<ushort>(m_vTriggerProperties[i].gapType));
		_AddToByteArray(pStream,m_vTriggerProperties[i].lVal1);
		_AddToByteArray(pStream,m_vTriggerProperties[i].lVal2);
		_AddToByteArray(pStream,m_vTriggerProperties[i].wTriggerLevel);
	}

	return true;
}

bool CGoodsBaseProperties::Unserialize(uchar* pStream, long& lOffset, bool b)
{
	char strTemp[1024];

	Clear();
	m_index = _GetDwordFromByteArray(pStream, lOffset);
	m_strOriginalName = _GetStringFromByteArray(pStream, lOffset, strTemp);
	m_strName = _GetStringFromByteArray(pStream, lOffset, strTemp);
	m_dwPrice = _GetDwordFromByteArray(pStream, lOffset);
	m_dwSilverPrice = _GetDwordFromByteArray(pStream,lOffset);
	m_GoodsBaseType = static_cast<eGoodsBaseType>(_GetDwordFromByteArray(pStream,lOffset));

	//##获取图标
	ulong dwNumIconType = _GetDwordFromByteArray(pStream, lOffset);
	for(size_t i = 0; i < dwNumIconType; i++)
	{
		tagIcon iIcon;
		iIcon.itIconType = static_cast<eIconType>(_GetDwordFromByteArray(pStream, lOffset));
		iIcon.dwIconID = _GetDwordFromByteArray(pStream, lOffset);

		m_vIcons.push_back(iIcon);
	}
	//##获取附加属性
	ulong dwNumAddonProperties = _GetDwordFromByteArray(pStream, lOffset);
	for(size_t i = 0; i < dwNumAddonProperties; i++)
	{
		tagAddonProperty apAddonProperty;
		apAddonProperty.Unserialize(pStream, lOffset);
		m_vAddonProperties.push_back(apAddonProperty);
	}

	//套装属性
	dwNumAddonProperties = _GetDwordFromByteArray(pStream, lOffset);
	for(size_t i = 0; i < dwNumAddonProperties; i++)
	{
		tagSuitProperty apAddonProperty;
		apAddonProperty.Unserialize(pStream, lOffset);
		m_vSuitProperties.push_back(apAddonProperty);
	}

	//道具属性激发
	m_vTriggerProperties.clear();
	ushort wTriggerNum=_GetWordFromByteArray(pStream,lOffset);
	for(ushort i=0;i<wTriggerNum;i++)
	{
		tagTriggerProperty stTriggerProperty;
		stTriggerProperty.gapType = static_cast<eGoodsAddonProperties>(_GetWordFromByteArray(pStream,lOffset));
		stTriggerProperty.lVal1 = _GetLongFromByteArray(pStream,lOffset);
		stTriggerProperty.lVal2 = _GetLongFromByteArray(pStream,lOffset);
		stTriggerProperty.wTriggerLevel = _GetWordFromByteArray(pStream,lOffset);
		m_vTriggerProperties.push_back(stTriggerProperty);
	}

	return true;
}

bool CGoodsBaseProperties::Serialize(DBWriteSet& setWriteDB, bool b)
{
	setWriteDB.AddToByteArray(m_index);
	setWriteDB.AddToByteArray(const_cast<char*>(m_strOriginalName.c_str()));
	setWriteDB.AddToByteArray(const_cast<char*>(m_strName.c_str()));
	setWriteDB.AddToByteArray(m_dwPrice);
	setWriteDB.AddToByteArray(m_dwSilverPrice);
	setWriteDB.AddToByteArray((ulong)m_GoodsBaseType);

	//添加图标
	setWriteDB.AddToByteArray(static_cast<ulong>(m_vIcons.size()));
	for(size_t i = 0; i < m_vIcons.size(); i++)
	{
		setWriteDB.AddToByteArray(static_cast<ulong>(m_vIcons[i].itIconType));
		setWriteDB.AddToByteArray(m_vIcons[i].dwIconID );
	}
	//添加附加属性的结构
	setWriteDB.AddToByteArray(static_cast<ulong>(m_vAddonProperties.size()));
	for( size_t i = 0; i < m_vAddonProperties.size(); i ++ )
	{
		m_vAddonProperties[i].Serialize( setWriteDB );
	}

	//套装
	setWriteDB.AddToByteArray(static_cast<ulong>(m_vSuitProperties.size()));
	
	for(size_t i = 0; i < m_vSuitProperties.size(); i++)
	{
		m_vSuitProperties[i].Serialize( setWriteDB );
	}

	//属性激发
	setWriteDB.AddToByteArray(static_cast<ushort>(m_vTriggerProperties.size()));
	for(size_t i=0;i<m_vTriggerProperties.size();i++)
	{
		setWriteDB.AddToByteArray(static_cast<ushort>(m_vTriggerProperties[i].gapType));
		setWriteDB.AddToByteArray(m_vTriggerProperties[i].lVal1);
		setWriteDB.AddToByteArray(m_vTriggerProperties[i].lVal2);
		setWriteDB.AddToByteArray(m_vTriggerProperties[i].wTriggerLevel);
	}

	return true;
}
	
bool CGoodsBaseProperties::Unserialize(DBReadSet& setReadDB, bool b)
{
	char strTemp[1024];
	Clear();

	m_index = setReadDB.GetDwordFromByteArray();
	m_strOriginalName = setReadDB.GetStringFromByteArray(strTemp ,1024);
	m_strName = setReadDB.GetStringFromByteArray(strTemp ,1024);
	m_dwPrice = setReadDB.GetDwordFromByteArray();
	m_dwSilverPrice = setReadDB.GetDwordFromByteArray();
	m_GoodsBaseType = static_cast<eGoodsBaseType>(setReadDB.GetDwordFromByteArray());

	//##获取图标
	ulong dwNumIconType = setReadDB.GetDwordFromByteArray();
	for(size_t i = 0; i < dwNumIconType; i++)
	{
		tagIcon iIcon;
		iIcon.itIconType = static_cast<eIconType>(setReadDB.GetDwordFromByteArray());
		iIcon.dwIconID = setReadDB.GetDwordFromByteArray();

		m_vIcons.push_back( iIcon );
	}
	//##获取附加属性
	ulong dwNumAddonProperties = setReadDB.GetDwordFromByteArray();
	for(size_t i = 0; i < dwNumAddonProperties; i++)
	{
		tagAddonProperty apAddonProperty;
		apAddonProperty.Unserialize(setReadDB);
		m_vAddonProperties.push_back(apAddonProperty);
	}

	//套装属性
	dwNumAddonProperties = setReadDB.GetDwordFromByteArray();
	for(size_t i = 0; i < dwNumAddonProperties; i++)
	{
		tagSuitProperty apAddonProperty;
		apAddonProperty.Unserialize(setReadDB);
		m_vSuitProperties.push_back(apAddonProperty);
	}

	//道具属性激发
	m_vTriggerProperties.clear();
	ushort wTriggerNum=setReadDB.GetWordFromByteArray();
	for(ushort i=0;i<wTriggerNum;i++)
	{
		tagTriggerProperty stTriggerProperty;
		stTriggerProperty.gapType=static_cast<eGoodsAddonProperties>(setReadDB.GetWordFromByteArray());
		stTriggerProperty.lVal1=setReadDB.GetLongFromByteArray();
		stTriggerProperty.lVal2=setReadDB.GetLongFromByteArray();
		stTriggerProperty.wTriggerLevel=setReadDB.GetWordFromByteArray();
		m_vTriggerProperties.push_back(stTriggerProperty);
	}
	return true;
}



ulong CGoodsBaseProperties::GetIconID( eIconType itType )
{
	ulong dwResult = 0;
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

bool CGoodsBaseProperties::IsImplicit(eGoodsAddonProperties gapType)
{
	bool bResult = false;
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
bool CGoodsBaseProperties::IsHasAddonPropertie(eGoodsAddonProperties gapType)
{
	for(size_t i = 0; i < m_vAddonProperties.size(); i++)
	{
		if(m_vAddonProperties[i].gapType == gapType)
		{
			return true;
		}
	}
	return false;
}

//根据指定的附加属性TYPE编号获取值
void CGoodsBaseProperties::GetAddonPropertyValues( eGoodsAddonProperties gapType, 
												  AddonPropertyValueList& vOut )
{
	for(size_t i = 0; i < m_vAddonProperties.size(); i++)
	{
		if(m_vAddonProperties[i].gapType == gapType)
		{
			for(size_t j = 0; j < m_vAddonProperties[i].vValues.size(); j++)
			{
				vOut.push_back( m_vAddonProperties[i].vValues[j] );
			}
			break;
		}
	}
}

ulong CGoodsBaseProperties::GetAddonPropertyValue(eGoodsAddonProperties gapType,ulong dwId)
{
	for(size_t i = 0; i < m_vAddonProperties.size(); i++)
	{
		if(m_vAddonProperties[i].gapType == gapType)
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

//获得所有有效的附加属性
void CGoodsBaseProperties::GetValidAddonProperties(vector<eGoodsAddonProperties>& vOut)
{
	for(size_t i = 0; i < m_vAddonProperties.size(); i++)
	{
		if(m_vAddonProperties[i].bIsEnabled == true)
		{
			vOut.push_back( m_vAddonProperties[i].gapType );
		}
	}
}

CGoodsBaseProperties::tagAddonPropertyValue::tagAddonPropertyValue()
{
	dwId = 0;
	lBaseValue = 0;
}

CGoodsBaseProperties::tagAddonPropertyValue::~tagAddonPropertyValue()
{
	Clear();
}


void CGoodsBaseProperties::tagAddonPropertyValue::Clear()
{
	dwId = 0;
	lBaseValue = 0;
}

bool CGoodsBaseProperties::tagAddonPropertyValue::Serialize(vector<uchar>* pStream, bool b)
{
	_AddToByteArray(pStream, dwId);
	_AddToByteArray(pStream, lBaseValue);

	return true;
}

bool CGoodsBaseProperties::tagAddonPropertyValue::Unserialize(uchar* pStream, long& lOffset, bool b)
{
	Clear();

	dwId = _GetDwordFromByteArray(pStream, lOffset);
	lBaseValue = _GetLongFromByteArray(pStream, lOffset);

	return true;
}

bool CGoodsBaseProperties::tagAddonPropertyValue::Serialize(DBWriteSet& setWriteDB, bool b )
{
	setWriteDB.AddToByteArray(dwId);
	setWriteDB.AddToByteArray(lBaseValue);
	return true;
}
	
bool CGoodsBaseProperties::tagAddonPropertyValue::Unserialize(DBReadSet& setReadDB, bool b )
{
	Clear();
	dwId = setReadDB.GetDwordFromByteArray( );
	lBaseValue = setReadDB.GetLongFromByteArray( );
	return true;
}

CGoodsBaseProperties::tagAddonProperty::tagAddonProperty()
{
	gapType = GAP_UNKNOW;
	bIsEnabled = false;
	bIsImplicitAttribute = false;
}

CGoodsBaseProperties::tagAddonProperty::~tagAddonProperty()
{
	Clear();
}

void CGoodsBaseProperties::tagAddonProperty::Clear()
{
	gapType = GAP_UNKNOW;
	bIsEnabled = false;
	bIsImplicitAttribute = false;

	for(size_t i = 0; i < vValues.size(); i++)
	{
		vValues[i].Clear();
	}

	vValues.clear();
}

bool CGoodsBaseProperties::tagAddonProperty::Serialize(vector<uchar>* pStream, bool b)
{
	_AddToByteArray(pStream, static_cast<ulong>(gapType) );
	_AddToByteArray(pStream, static_cast<long>(bIsEnabled));
	_AddToByteArray(pStream, static_cast<long>(bIsImplicitAttribute));
	_AddToByteArray(pStream, static_cast<ulong>(vValues.size()));
	for(size_t i = 0; i < vValues.size(); i++)
	{
		vValues[i].Serialize( pStream );
	}

	return true;
}

bool CGoodsBaseProperties::tagAddonProperty::Unserialize(uchar* pStream, long& lOffset, bool b)
{
	Clear();

	gapType = static_cast<eGoodsAddonProperties>(_GetDwordFromByteArray(pStream, lOffset));
    bIsEnabled = _GetLongFromByteArray(pStream, lOffset) ? true : false;
	bIsImplicitAttribute = _GetLongFromByteArray(pStream, lOffset) ? true : false;

	ulong dwNumAddonPropertyValues = _GetDwordFromByteArray(pStream, lOffset);
	for(size_t i = 0; i < dwNumAddonPropertyValues; i++)
	{
		tagAddonPropertyValue apvAddonPropertyValue;
		apvAddonPropertyValue.Unserialize(pStream, lOffset);
		vValues.push_back(apvAddonPropertyValue);
	}
	return true;
}


bool CGoodsBaseProperties::tagAddonProperty::Serialize(DBWriteSet& setWriteDB, bool b)
{
	setWriteDB.AddToByteArray(static_cast<ulong>(gapType) );
	setWriteDB.AddToByteArray(static_cast<long>(bIsEnabled) );
	setWriteDB.AddToByteArray(static_cast<long>(bIsImplicitAttribute) );
	setWriteDB.AddToByteArray(static_cast<ulong>(vValues.size()));
	for(size_t i = 0; i < vValues.size(); i++)
	{
		vValues[i].Serialize(setWriteDB);
	}

	return true;
}
		
bool CGoodsBaseProperties::tagAddonProperty::Unserialize(DBReadSet& setReadDB, bool b)
{
	Clear();
	gapType = static_cast<eGoodsAddonProperties>(setReadDB.GetDwordFromByteArray());
	bIsEnabled = setReadDB.GetLongFromByteArray() ? true : false;
	bIsImplicitAttribute = setReadDB.GetLongFromByteArray() ? true : false;
	ulong dwNumAddonPropertyValues = setReadDB.GetDwordFromByteArray();
	for(size_t i = 0; i < dwNumAddonPropertyValues; i++)
	{
		tagAddonPropertyValue apvAddonPropertyValue;
		apvAddonPropertyValue.Unserialize(setReadDB);
		vValues.push_back(apvAddonPropertyValue);
	}
	return true;
}