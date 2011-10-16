#include "StdAfx.h"
#include "Goods.h"
#include "GoodsFactory.h"
#include "Enchase/EnchaseHole.h"
#include "../DBEntity/EntityGroup.h"

#include "../../../../Public/Setup/GoodsSetup.h"
#include "../../../../Public/Setup/GlobalSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CGoods::CGoods()
{
	SetType(TYPE_GOODS);
	m_dwAmount	= 1;
	m_pEnchaseHoleArray=NULL;
	m_nInitHoleNum = 0;
	m_dwActivedKitCardId=0;

#ifdef __MEMORY_LEAK_CHECK__
	CBaseObject::AddConstructRef(TYPE_GOODS);
	SetSelfConsflag(GetConstructFlag());
#endif
}

CGoods::CGoods(OBJECT_TYPE otType)
{
	SetType(otType);
	m_dwAmount = 1;
}

CGoods::~CGoods()
{
	Release();
#ifdef __MEMORY_LEAK_CHECK__
	CBaseObject::RefConstructRef(TYPE_GOODS,GetSelfConsFlag());
#endif
}

ulong CGoods::GetBasePropertiesIndex()
{
	return m_dwBasePropertiesIndex;
}

void CGoods::SetAmount(ulong dwAmount)
{
	if(dwAmount == 0)
		return;
	m_dwAmount = dwAmount;
}

ulong CGoods::GetAmount()
{
	return m_dwAmount;
}


//是否能修理
bool CGoods::CanReparied()
{
	CGoodsBaseProperties* pProperties = 
		GoodsSetup::QueryGoodsBaseProperties( GetBasePropertiesIndex() );
	if( pProperties && pProperties->GetGoodsBaseType() == GBT_EQUIPMENT )
	{
		return true;
	}
	return false;
}

void CGoods::InitEnchaseHole()
{
	//如可以开孔
	ulong dwNum = GetMaxEnchaseHoleNum();
	m_nInitHoleNum = dwNum;
	if(dwNum>0)
	{
		m_pEnchaseHoleArray=new  CEnchaseHole*[dwNum];
		for(size_t i=0;i<dwNum;i++)
		{
			m_pEnchaseHoleArray[i]=new CEnchaseHole(this);
		}
	}
}

void CGoods::EnchaseRelease()
{
	//ulong dwNum=GetMaxEnchaseHoleNum();
	ulong dwNum=m_nInitHoleNum;
	if(dwNum>0 && m_pEnchaseHoleArray)
	{
		for(ulong i=0;i<dwNum;i++)
		{
			SAFE_DELETE(m_pEnchaseHoleArray[i]);
		}
	}
	SAFE_DELETE_ARRAY(m_pEnchaseHoleArray);
	m_nInitHoleNum=0;
}


bool CGoods::CanEnchase()
{
	long lBaseHoleNum=GetAddonPropertyValues(GAP_HOLE_NUM,2);
	if(lBaseHoleNum>0)
	{
		return true;
	}
	return false;
}

ulong CGoods::GetMaxEnchaseHoleNum()
{
	return GetAddonPropertyValues(GAP_HOLE_NUM,2);
}

long* CGoods::GetEnchaseHoleData()
{
	long* pArray=new long[5];
	//ulong dwNum=GetMaxEnchaseHoleNum();
	ulong dwNum=m_nInitHoleNum;
	for(ulong i=0;i<5;i++)
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

void CGoods::SetEnchaseHoleState(int nIndex,bool bVal)
{
	//ulong dwMaxHole=GetMaxEnchaseHoleNum();
	int dwMaxHole=m_nInitHoleNum;
	if(dwMaxHole>0 && nIndex>=0 && nIndex< dwMaxHole && m_pEnchaseHoleArray)		
	{
		m_pEnchaseHoleArray[nIndex]->SetState(bVal);
	}
}

bool CGoods::Enchase(CGoods* pGoods,int nIndex)
{
	//物品本身必须可镶嵌
	if(!CanEnchase())
		return false;
	//卡片必须是镶嵌型卡片
	//ulong dwMaxHoleNum=GetMaxEnchaseHoleNum();
	int dwMaxHoleNum=m_nInitHoleNum;
	if(dwMaxHoleNum>0 && nIndex>=0 && nIndex<dwMaxHoleNum && m_pEnchaseHoleArray)
	{
		m_pEnchaseHoleArray[nIndex]->SetCard(pGoods);
		return true;
	}
	return false;
}

ulong CGoods::GetActivedHoleNum()
{
	//ulong dwMaxHoleNum=GetMaxEnchaseHoleNum();
	ulong dwMaxHoleNum=m_nInitHoleNum;
	if(dwMaxHoleNum>0)
	{
		ulong dwActivedHoleNum=0;
		for(ulong i=0;i<dwMaxHoleNum;i++)
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

bool CGoods::SetAddonPropertyBaseValues(eGoodsAddonProperties gapType, ulong dwID, long lValue)
{
	bool bResult = false;
	if(dwID!=1 && dwID!=2)
		return bResult;

	for( size_t i = 0; i < m_vAddonPropertiesList.size(); i ++ )
	{
		if( m_vAddonPropertiesList[i].gapType == gapType )
		{
			m_vAddonPropertiesList[i].lValues[dwID-1]=lValue;
			bResult=true;
			break;
		}
	}
	return bResult;
}

bool CGoods::DelAddonPropertyBaseValues(eGoodsAddonProperties gapType)
{
	vector<CGoods::tagAddonProperty>::iterator proIter = m_vAddonPropertiesList.begin();
	for(; proIter!=m_vAddonPropertiesList.end(); ++proIter)
	{
		if( proIter->gapType == gapType )
		{
			break;
		}
	}
	if(proIter != m_vAddonPropertiesList.end())
	{
		m_vAddonPropertiesList.erase(proIter);
		return true;
	}
	return false;
}

void CGoods::AddAddonProperty(uchar* pStream, long size )
{	
	long lPos = 0;
	m_vAddonPropertiesList.clear();

	//根据ID获得物品基本属性
	CGoodsBaseProperties* pBaseProperty=GoodsSetup::QueryGoodsBaseProperties(m_dwBasePropertiesIndex);
	if(!pBaseProperty)
		return;

	ulong dwSize = _GetDwordFromByteArray(pStream,lPos);
	for(ulong i=0; i<dwSize; i++)
	{
		ulong dwGapId=_GetDwordFromByteArray(pStream,lPos);
		long lVal1=_GetLongFromByteArray(pStream,lPos);
		long lVal2=_GetLongFromByteArray(pStream,lPos);				
		if(GoodsSetup::s_GoodsAttrDBSetup[dwGapId][0]==1)
		{			
			tagAddonProperty addonProperty;			
			addonProperty.gapType = static_cast<eGoodsAddonProperties>(dwGapId);
			addonProperty.lValues[0] = lVal1;
			addonProperty.lValues[1] = lVal2;			
			m_vAddonPropertiesList.push_back(addonProperty);
		}				
	}

	//maker name
	char pszMakerName[256];
	_GetStringFromByteArray(pStream,lPos,pszMakerName);
	SetMakerName(pszMakerName);

	//enchase data
	long lActiveHoleNum=_GetLongFromByteArray(pStream,lPos);
	if(lActiveHoleNum)
	{
		long lHoleIdx=_GetLongFromByteArray(pStream,lPos);
		if(lHoleIdx==-1)
		{
			SetEnchaseHoleState(0,false);
		}
		else if(lHoleIdx==0)
		{
			SetEnchaseHoleState(0,true);
		}
		else if(lHoleIdx>0)
		{
			SetEnchaseHoleState(0,true);
			CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx,16);
			Enchase(pCard,0);
		}

		lHoleIdx=_GetLongFromByteArray(pStream,lPos);
		if(lHoleIdx==-1)
		{
			SetEnchaseHoleState(1,false);
		}
		else if(lHoleIdx==0)
		{
			SetEnchaseHoleState(1,true);
		}
		else if(lHoleIdx>0)
		{
			SetEnchaseHoleState(1,true);
			CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx,17);
			Enchase(pCard,1);
		}

		lHoleIdx=_GetLongFromByteArray(pStream,lPos);
		if(lHoleIdx==-1)
		{
			SetEnchaseHoleState(2,false);
		}
		else if(lHoleIdx==0)
		{
			SetEnchaseHoleState(2,true);
		}
		else if(lHoleIdx>0)
		{
			SetEnchaseHoleState(2,true);
			CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx,18);
			Enchase(pCard,2);
		}

		lHoleIdx=_GetLongFromByteArray(pStream,lPos);
		if(lHoleIdx==-1)
		{
			SetEnchaseHoleState(3,false);
		}
		else if(lHoleIdx==0)
		{
			SetEnchaseHoleState(3,true);
		}
		else if(lHoleIdx>0)
		{
			SetEnchaseHoleState(3,true);
			CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx,19);
			Enchase(pCard,3);
		}

		lHoleIdx=_GetLongFromByteArray(pStream,lPos);
		if(lHoleIdx==-1)
		{
			SetEnchaseHoleState(4,false);
		}
		else if(lHoleIdx==0)
		{
			SetEnchaseHoleState(4,true);
		}
		else if(lHoleIdx>0)
		{
			SetEnchaseHoleState(4,true);
			CGoods* pCard=CGoodsFactory::CreateGoods(lHoleIdx,20);
			Enchase(pCard,4);
		}
	}
}

bool CGoods::AddAddonPropertyBaseValues(eGoodsAddonProperties gapType,ulong dwID,long lVal1,long lVal2,bool bEnable,bool bIsImplicit)
{	
	if(dwID!=1 && dwID!=2)
		return false;

	for( size_t i = 0; i < m_vAddonPropertiesList.size(); ++i)
	{
		if(m_vAddonPropertiesList[i].gapType == gapType)
		{
			return false;		
		}
	}

	tagAddonProperty stPro;
	stPro.gapType=gapType;
	stPro.lValues[dwID-1]=lVal1;	
	m_vAddonPropertiesList.push_back(stPro);
	return true;
}


void CGoods::EnchaseArrange()
{	
	vector<CGoods*> vecGoods;
	vecGoods.clear();

	//ulong dwNum=GetMaxEnchaseHoleNum();
	ulong dwNum=m_nInitHoleNum;
	ulong dwActived=GetActivedHoleNum();
	for(ulong i=0;i<dwNum;i++)
	{
		if(m_pEnchaseHoleArray[i]->GetState() && 
			m_pEnchaseHoleArray[i]->GetCard())
		{
			vecGoods.push_back(m_pEnchaseHoleArray[i]->GetCard());
		}
		m_pEnchaseHoleArray[i]->SetState(false);
		m_pEnchaseHoleArray[i]->SetCard(NULL);
	}	
	
	for(ulong i=0;i<dwNum;i++)
	{
		if(i<dwActived)
		{
			m_pEnchaseHoleArray[i]->SetState(true);
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

void CGoods::GetAddonPropertyValues(eGoodsAddonProperties gapType, vector<long>& vOut)
{
	for(size_t i = 0; i < m_vAddonPropertiesList.size(); i++)
	{
		if(m_vAddonPropertiesList[i].gapType == gapType)
		{
			vOut.push_back(m_vAddonPropertiesList[i].lValues[0]);
			vOut.push_back(m_vAddonPropertiesList[i].lValues[1]);			
			break;
		}
	}
}

long CGoods::GetAddonPropertyValues(eGoodsAddonProperties gapType, ulong dwID)
{
	long lResult = 0;
	if(dwID!=1 && dwID!=2)
		return lResult;

	bool bFound=false;
	for( size_t i = 0; i < m_vAddonPropertiesList.size(); i ++ )
	{
		if( m_vAddonPropertiesList[i].gapType == gapType )
		{
			lResult = m_vAddonPropertiesList[i].lValues[dwID-1];
			bFound = true;
			break;			
		}
	}

	if(!bFound)
	{
		CGoodsBaseProperties* pProperty=GoodsSetup::QueryGoodsBaseProperties(GetBasePropertiesIndex());
		if(pProperty)
		{
			lResult = pProperty->GetAddonPropertyValue(gapType,dwID);
		}
	}
	return lResult;
}

bool CGoods::IsAddonProperyExist(eGoodsAddonProperties gapType)
{
	for(size_t i = 0; i < m_vAddonPropertiesList.size(); i++)
	{
		if(m_vAddonPropertiesList[i].gapType == gapType)
		{
			return true;
		}
	}
	return false;
}

bool CGoods::Clone(CBaseObject* pObject)
{
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		vector<uchar> vData;
		long lOffset = 0;

		Serialize( &vData );
		pGoods->Unserialize( &vData[0], lOffset );

		return true;
	}
	return false;
}

bool CGoods::Serialize(vector<uchar>* pStream, bool b)
{
	CShape::AddToByteArray( pStream, b==0?false:true );

	_AddToByteArray( pStream, m_dwBasePropertiesIndex );
	_AddToByteArray( pStream, m_dwAmount );
	_AddToByteArray( pStream, const_cast<char*>( m_strMakerName.c_str() ) );

	ulong dwAddonNum=0;
	ulong dwPropertyId=0;

	vector<uchar> vecAddonData;
	for(size_t i = 0; i < m_vAddonPropertiesList.size(); i++)
	{
		dwPropertyId=m_vAddonPropertiesList[i].gapType;
		if(GoodsSetup::s_GoodsAttrDBSetup[dwPropertyId][1]==1)
		{
			dwAddonNum++;
			m_vAddonPropertiesList[i].Serialize(&vecAddonData);
		}
	}
	_AddToByteArray(pStream, dwAddonNum);
	if(dwAddonNum>0)
	{
		_AddToByteArray(pStream,&vecAddonData[0],(long)vecAddonData.size());
	}
	
	EnchaseArrange();
	//enchase data	
	//ulong dwMaxHoleNum=GetMaxEnchaseHoleNum();
	ulong dwMaxHoleNum=m_nInitHoleNum;
	_AddToByteArray(pStream,dwMaxHoleNum);
	//激活孔数量
	_AddToByteArray( pStream, GetActivedHoleNum());
	_AddToByteArray(pStream,m_dwActivedKitCardId);
	for(ulong i=0;i<dwMaxHoleNum;i++)
	{
		//状态
		if(m_pEnchaseHoleArray[i]->GetState())
		{
			_AddToByteArray(pStream,uchar(1));
		}
		else
		{
			_AddToByteArray(pStream,uchar(0));
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
			_AddToByteArray(pStream,ulong(0));
		}
	}
	return true;
}

bool CGoods::Unserialize(uchar* pStream, long& lOffset, bool b)
{
	Release();

	char szString[256];

	CShape::DecordFromByteArray(pStream, lOffset, b==0? false:true);

	m_dwBasePropertiesIndex	= _GetDwordFromByteArray(pStream, lOffset);
	m_dwAmount = _GetDwordFromByteArray( pStream, lOffset );

	//获取物品描述
	ZeroMemory(szString, 256);
	_GetStringFromByteArray(pStream, lOffset, szString );
	m_strMakerName = szString;

	m_vAddonPropertiesList.clear();

	// 获取物品扩展属性
	ulong dwNumProperties = _GetDwordFromByteArray(pStream, lOffset);
	for(ulong i = 0; i < dwNumProperties; i++)
	{
		tagAddonProperty apAddonProperty;
		apAddonProperty.Unserialize(pStream, lOffset);
		m_vAddonPropertiesList.push_back( apAddonProperty );
	}

	InitEnchaseHole();

	ulong dwMaxHoleNum=_GetDwordFromByteArray(pStream,lOffset);
	ulong dwActivedHoleNum=_GetDwordFromByteArray( pStream, lOffset );
	m_dwActivedKitCardId=_GetDwordFromByteArray( pStream, lOffset );
	for(ulong i=0;i<dwMaxHoleNum;i++)
	{		
		uchar btState=_GetByteFromByteArray(pStream,lOffset);
		
		if(btState==1)
		{
			if( (int)i < m_nInitHoleNum)
				m_pEnchaseHoleArray[i]->SetState(true);
		}
		else
		{
			if( (int)i< m_nInitHoleNum)
				m_pEnchaseHoleArray[i]->SetState(false);
		}
		//CARD ID
		ulong dwGoodsId=_GetDwordFromByteArray(pStream,lOffset);		

		if(dwGoodsId>0)
		{
#ifdef __MEMORY_LEAK_CHECK__
			CBaseObject::SetConstructFlag(82);
#endif
			CGoods* pGoods = CGoodsFactory::UnserializeGoods( pStream, lOffset,50 );
			if( (int)i< m_nInitHoleNum)
				m_pEnchaseHoleArray[i]->SetCard(pGoods);
		}
		else
		{
			
		}
	}
	return true;
}


bool CGoods::Serialize(DBWriteSet& setWriteDB, bool b)
{
	CShape::CodeToDataBlock( setWriteDB, b==false? false:true );

	setWriteDB.AddToByteArray( m_dwBasePropertiesIndex );
	setWriteDB.AddToByteArray( m_dwAmount );
	setWriteDB.AddToByteArray( const_cast<char*>( m_strMakerName.c_str() ) );		

	ulong dwAddonNum=0;
	ulong dwPropertyId=0;
	vector<uchar> vecAddonData;
	for(size_t i = 0; i < m_vAddonPropertiesList.size(); i++)
	{		
		dwPropertyId=m_vAddonPropertiesList[i].gapType;
		if(GoodsSetup::s_GoodsAttrDBSetup[dwPropertyId][0]==1)
		{
			dwAddonNum++;
			m_vAddonPropertiesList[i].Serialize(&vecAddonData);
		}
	}
	setWriteDB.AddToByteArray(dwAddonNum);
	if(dwAddonNum>0)
	{
		setWriteDB.AddToByteArray(&vecAddonData[0],(long)vecAddonData.size());
	}

	EnchaseArrange();
	//enchase data	
	
	ulong dwMaxHoleNum=m_nInitHoleNum;
	setWriteDB.AddToByteArray(dwMaxHoleNum);
	//激活孔数量
	setWriteDB.AddToByteArray( GetActivedHoleNum());
	setWriteDB.AddToByteArray( m_dwActivedKitCardId);
	for(ulong i=0;i<dwMaxHoleNum;i++)
	{
		//状态
		if(m_pEnchaseHoleArray[i]->GetState())
		{
			setWriteDB.AddToByteArray(uchar(1));
		}
		else
		{
			setWriteDB.AddToByteArray(uchar(0));
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
			setWriteDB.AddToByteArray(ulong(0));
		}
	}
	return true;
}
bool CGoods::Unserialize(DBReadSet& setReadDB, bool b)
{
	Release();
	char szString[256];
	CShape::DecodeFromDataBlock( setReadDB, b==false?false:true );

	m_dwBasePropertiesIndex	= setReadDB.GetDwordFromByteArray();
	m_dwAmount				= setReadDB.GetDwordFromByteArray(  );

	//获取物品描述
	ZeroMemory( szString, 256 );
	setReadDB.GetStringFromByteArray( szString ,256);
	m_strMakerName			= szString;

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame().GetStatckFileName(),"goods base pro decode ok");
#endif

	m_vAddonPropertiesList.clear();

	// 获取物品扩展属性
	ulong dwNumProperties	= setReadDB.GetDwordFromByteArray();
	for(ulong i = 0; i < dwNumProperties; i++)
	{
		tagAddonProperty apAddonProperty;
		apAddonProperty.Unserialize(setReadDB);
		m_vAddonPropertiesList.push_back(apAddonProperty);
	}	

#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame().GetStatckFileName(),"goods addon pro decode ok");
#endif

	InitEnchaseHole();

	ulong dwMaxHoleNum=setReadDB.GetDwordFromByteArray();
	ulong dwActivedHoleNum=setReadDB.GetDwordFromByteArray( );
	m_dwActivedKitCardId=setReadDB.GetDwordFromByteArray( );
	for(ulong i=0;i<dwMaxHoleNum;i++)
	{	
        uchar btState=setReadDB.GetByteFromByteArray( );
		
		if(btState==1)
		{
			if( (int)i< m_nInitHoleNum)
				m_pEnchaseHoleArray[i]->SetState(true);
		}
		else
		{
			if( (int)i< m_nInitHoleNum)
				m_pEnchaseHoleArray[i]->SetState(false);
		}
		//CARD ID
		ulong dwGoodsId=setReadDB.GetDwordFromByteArray( );		

		if(dwGoodsId>0)
		{
#ifdef __MEMORY_LEAK_CHECK__
			CBaseObject::SetConstructFlag(82);
#endif
			CGoods* pGoods = CGoodsFactory::UnserializeGoods( setReadDB,51);
			if( (int)i < m_nInitHoleNum)
				m_pEnchaseHoleArray[i]->SetCard(pGoods);
			else
				CGoodsFactory::GarbageCollect(&pGoods);
		}
		else
		{
			
		}
	}
#ifdef _RUNSTACKINFO1_
	CMessage::AsyWriteFile(GetGame().GetStatckFileName(),"goods card pro decode");
#endif
	return true;
}

void CGoods::Release()
{
	EnchaseRelease();
	m_dwBasePropertiesIndex	= 0;
	m_dwAmount = 0;
	m_baseProperties = NULL;
	for(size_t i = 0; i < m_vAddonPropertiesList.size(); i++)
	{
		m_vAddonPropertiesList[i].Clear();
	}
	m_vAddonPropertiesList.clear();
}

void CGoods::SetBasePropertiesIndex(ulong dwID)
{
	m_dwBasePropertiesIndex	= dwID;
}

CGoods::tagAddonProperty::tagAddonProperty()
{
	gapType	= GAP_UNKNOW;
	ZeroMemory(lValues,2*sizeof(long));
}

CGoods::tagAddonProperty::~tagAddonProperty()
{
	Clear();
}

void CGoods::tagAddonProperty::Clear()
{
	gapType	= GAP_UNKNOW;
	ZeroMemory(lValues,2*sizeof(long));
}

bool CGoods::tagAddonProperty::Serialize(vector<uchar>* pStream, bool b)
{
	_AddToByteArray( pStream, static_cast<ulong>(gapType) );
	_AddToByteArray(pStream,lValues[0]);
	_AddToByteArray(pStream,lValues[1]);
	return true;
}

bool CGoods::tagAddonProperty::Unserialize(uchar* pStream, long& lOffset, bool b)
{
	gapType	= static_cast<eGoodsAddonProperties>(_GetDwordFromByteArray(pStream, lOffset));
	lValues[0]=_GetLongFromByteArray(pStream,lOffset);
	lValues[1]=_GetLongFromByteArray(pStream,lOffset);
	return true;
}

bool CGoods::tagAddonProperty::Serialize(DBWriteSet& setWriteDB, bool b)
{
	setWriteDB.AddToByteArray( static_cast<ulong>(gapType) );
	setWriteDB.AddToByteArray(lValues[0]);
	setWriteDB.AddToByteArray(lValues[1]);
	return true;
}
bool CGoods::tagAddonProperty::Unserialize(DBReadSet& setReadDB, bool b)
{
	gapType	= static_cast<eGoodsAddonProperties>(setReadDB.GetDwordFromByteArray() );
	lValues[0]=setReadDB.GetLongFromByteArray();
	lValues[1]=setReadDB.GetLongFromByteArray();
	return true;
}

ulong CGoods::GetPrice()
{
	return m_baseProperties->GetPrice();
}

ulong CGoods::GetTotalPrice()
{
	return m_baseProperties->GetPrice() * m_dwAmount;
}

const char* CGoods::GetGoodsName()
{
	return CShape::GetName();
}

vector<CGoods::tagAddonProperty>& CGoods::GetAllAddonProperties()
{
	return m_vAddonPropertiesList;
}

/// 获取物品装备位置栏位
eEquipType CGoods::GetEquipType(void)
{
	return (eEquipType)GetAddonPropertyValues(GAP_EQUIP_CATEGORY, 1);
}

/// 获取物品法宝类型
eTrumpType CGoods::GetTrumpType(void)
{
	return (eTrumpType)GetAddonPropertyValues(GAP_TRUMP_CATEGORY, 1);
}