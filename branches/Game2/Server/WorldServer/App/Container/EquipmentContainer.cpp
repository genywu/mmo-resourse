// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "StdAfx.h"
#include "EquipmentContainer.h"
#include "Container.h"
#include "../Player.h"
#include "../Goods/Goods.h"
#include "../Goods/GoodsFactory.h"
#include "../../../../Public/Setup/GoodsSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


DWORD CEquipmentContainer::s_dwEquipmentLimit = 9;

//##ModelId=41B3CA7F02A3
CEquipmentContainer::CEquipmentContainer()
{
	// TODO: Add your specialized code here.
	AddListener( this );
}

////##ModelId=41B3CA9E02D2
//CEquipmentContainer::CEquipmentContainer(LONG lID) : CGoodsContainer( lID )
//{
//	// TODO: Add your specialized code here.
//	AddListener( this );
//}

//##ModelId=41B3CA960090
CEquipmentContainer::~CEquipmentContainer()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41B3D0AE026C
BOOL CEquipmentContainer::Add(CBaseObject* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	return bResult;
}

//##ModelId=41B3D0C20191
VOID CEquipmentContainer::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin(); 
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it->second == NULL )
		{
			continue;
		}
		for( size_t i = 0; i < m_vListeners.size(); i ++ )
		{
			if( m_vListeners[i] )
			{
				m_vListeners[i]->OnObjectRemoved( this, it->second, pVoid );
			}
		}
	}
	m_mEquipment.clear();
}

//##ModelId=41B3D0C9024D
VOID CEquipmentContainer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	map<eEquipType,CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		CGoodsFactory::GarbageCollect( &(it->second) );
	}
	m_mEquipment.clear();
	CGoodsContainer::Release();
	AddListener( this );
}

//##ModelId=41B418F902D3
VOID CEquipmentContainer::TraversingContainer(CContainerListener* pListener)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pListener == NULL ) 
	{
		return;
	}

	map<eEquipType,CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( pListener->OnTraversingContainer(this, it->second) == FALSE )
		{
			//break;
		}
	}
}

//##ModelId=41B5456901AB
DWORD CEquipmentContainer::GetContentsWeight()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwResult = 0;
	/*
	map<EQUIPMENT_TYPE, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it->second )
		{
			dwResult += it->second->GetWeight();
		}
	}*/
	return dwResult;
}

//##ModelId=41B6917E00AB
BOOL CEquipmentContainer::IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it->second && it->second->GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			bResult	= TRUE;
			break;
		}
	}
	return bResult;
}

//##ModelId=41B570CD0068
BOOL CEquipmentContainer::Swap(eEquipType ecColumn, CGoods* pIn, CGoods** ppOut, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return FALSE;
}

//##ModelId=41B3D0A1025D
CBaseObject* CEquipmentContainer::Find(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	map<eEquipType,CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it->second )
		{
			if( (*it).second->GetExID() == guid )
			{
				pResult = it->second;
				break;
			}
		}
	}
	return pResult;
}

//##ModelId=41B3D0B80395
CBaseObject* CEquipmentContainer::Remove(const CGUID& guid, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	map<eEquipType,CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it->second )
		{
			if( (*it).second->GetExID() == guid )
			{
				pResult = it->second;
				break;
			}
		}
	}
	if( pResult )
	{
		for( size_t i = 0; i < m_vListeners.size(); i ++ )
		{
			if( m_vListeners[i] )
			{
				m_vListeners[i]->OnObjectRemoved( this, pResult, pVoid );
			}
		}
		m_mEquipment.erase( it );
	}

	return pResult;
}



//##ModelId=41BF9DAB0109
BOOL CEquipmentContainer::Add(DWORD dwPosition, CGoods* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pObject == NULL )
	{
		return FALSE;
	}

	eEquipType ecColumn = static_cast<eEquipType>( dwPosition );
	map<eEquipType,CGoods*>::iterator it = m_mEquipment.find( ecColumn );
	if( it != m_mEquipment.end() )
	{
		if( it->second != NULL )
		{
// 			char str[256];
// 			sprintf(str, "此(名称:%s)装备装备在%d位置出错,此位置已有装备。",pObject->GetName(),dwPosition);
// 			PutStringToFile("debug-DB", str);

            Log4c::Trace(ROOT_MODULE,"此(名称:%s)装备装备在%d位置出错,此位置已有装备。",pObject->GetName(),dwPosition);
			return FALSE;
		}
	}

	BOOL bResult = FALSE;
	CGoodsBaseProperties* pProperties = 
		GoodsSetup::QueryGoodsBaseProperties( pObject->GetBasePropertiesIndex() );
	if( pProperties )
	{
		if( pProperties->GetGoodsBaseType() == GBT_EQUIPMENT 
			|| pProperties->GetGoodsBaseType() == GBT_SUBPACK )
		{
			m_mEquipment[ecColumn] = pObject;
			bResult = TRUE;
		}

	}
	if( bResult )
	{
		for( size_t i = 0; i < m_vListeners.size(); i ++ )
		{
			if( m_vListeners[i] )
			{
				m_vListeners[i]->OnObjectAdded( this, pObject, pVoid );
			}
		}
	}
	return bResult;
}

BOOL CEquipmentContainer::AddFromDB( CGoods* pObject, DWORD dwPosition )
{
	if( pObject == NULL )
	{
		return FALSE;
	}

	CGoods* pGoods = GetGoods( dwPosition );
	if( pGoods )
	{
//		char str[256];
		char szGuid[40];
		char szExGuid[40];
		
		CGUID guid,guidExsit;
		guid = pObject->GetExID();
		guid.tostring(szGuid);
		guidExsit = pGoods->GetExID();
		guidExsit.tostring(szExGuid);

// 		sprintf(str, "装备:[%s](%s)在%d位置已有[%s](%s)。",pObject->GetName(),szGuid,
// 			dwPosition,	pGoods->GetName(), szExGuid );
// 		
// 		PutStringToFile("debug-DB", str);

        Log4c::Trace(ROOT_MODULE, "装备:[%s](%s)在%d位置已有[%s](%s)。",pObject->GetName(),szGuid,
            dwPosition,	pGoods->GetName(), szExGuid );
		return FALSE;
	}

	BOOL bResult = FALSE;
	eEquipType ecColumn = static_cast<eEquipType>( dwPosition );
	CGoodsBaseProperties* pProperties = 
		GoodsSetup::QueryGoodsBaseProperties( pObject->GetBasePropertiesIndex() );
	if( pProperties )
	{
		if( pProperties->GetGoodsBaseType() == GBT_EQUIPMENT 
			|| pProperties->GetGoodsBaseType() == GBT_SUBPACK  )
		{
			m_mEquipment[ecColumn] = pObject;
			bResult = TRUE;
		}	
	}
	return bResult;
}
//##ModelId=41BFA0F30177
VOID CEquipmentContainer::AI()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it->second )
		{
			it->second->AI();
		}
	}
}

//##ModelId=41BFA1030271
BOOL CEquipmentContainer::IsFull()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult			= FALSE;
	DWORD dwNumEquipment	= 0;
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it->second )
		{
			++ dwNumEquipment;
		}
	}
	if( dwNumEquipment == CEquipmentContainer::s_dwEquipmentLimit )
	{
		bResult = TRUE;
	}
	return bResult;
}

//##ModelId=41BFA16E03B9
BOOL CEquipmentContainer::QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pGoods == NULL )
	{
		return FALSE;
	}

	BOOL bResult = FALSE;
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it->second == pGoods )
		{
			dwPosition	= it->first;
			bResult		= TRUE;
			break;
		}
	}
	return bResult;
}

//##ModelId=41BFA17A00BB
BOOL CEquipmentContainer::QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if(it->second && it->second->GetExID() == guid)
		{
			dwPosition	= it->first;
			bResult		= TRUE;
			break;
		}
	}
	return bResult;
}

//##ModelId=41BFA197036B
CGoods* CEquipmentContainer::GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it->second && it->second->GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			pResult	= it->second;
			break;
		}
	}
	return pResult;
}

//##ModelId=41BFA1AC0242
CGoods* CEquipmentContainer::GetGoods(DWORD dwPosition)
{
	// TODO: Add your specialized code here.
	CGoods* pResult				= NULL;
	eEquipType ecColumn	= static_cast<eEquipType>( dwPosition );
	map<eEquipType,CGoods*>::iterator it = m_mEquipment.find( ecColumn );
	if( it != m_mEquipment.end() )
	{
		pResult = it->second;
	}	
	return pResult;
}

//##ModelId=41BFA1B8007D
VOID CEquipmentContainer::GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it->second && it->second->GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			vOut.push_back( it->second );
		}
	}
}

//##ModelId=41BFB91C01D4
BOOL CEquipmentContainer::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid)
{
	return TRUE;
}

//##ModelId=41BFB92601A5
BOOL CEquipmentContainer::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid)
{
	return TRUE;
}

//##ModelId=41BFEAD70138
DWORD CEquipmentContainer::GetGoodsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwAmount = 0;
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it->second )
		{
			if( GoodsSetup::QueryGoodsBaseProperties( 
				it->second->GetBasePropertiesIndex() ) )
			{
				++ dwAmount;
			}
		}
	}
	return dwAmount;
}

CBaseObject* CEquipmentContainer::Remove( CBaseObject* pObject, LPVOID pVoid )
{
	return CGoodsContainer::Remove( pObject, pVoid );
}

CBaseObject* CEquipmentContainer::Remove( LONG oType, const CGUID& guid, LPVOID pVoid )
{
	return CGoodsContainer::Remove( oType, guid, pVoid );
}

CBaseObject* CEquipmentContainer::Find( CBaseObject* pObj )
{
	return CGoodsContainer::Find( pObj );
}

CBaseObject* CEquipmentContainer::Find( LONG oType, const CGUID& guid )
{
	return CGoodsContainer::Find( oType, guid );
}

BOOL CEquipmentContainer::Serialize( vector<BYTE>* pStream, BOOL b )
{
	_AddToByteArray( pStream, GetGoodsAmount() );
	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it->second )
		{
			if( GoodsSetup::QueryGoodsBaseProperties( 
				it->second->GetBasePropertiesIndex() ) )
			{
				_AddToByteArray( pStream, static_cast<DWORD>(it->first) );
				it->second->Serialize( pStream,  b ? true : false );
			}
		}
	}

	return TRUE;
}

BOOL CEquipmentContainer::Unserialize( BYTE* pStream, LONG& lOffset, BOOL b )
{
	Clear();
	DWORD dwGoodsAmount = _GetDwordFromByteArray( pStream, lOffset );
	
	for( DWORD i = 0; i < dwGoodsAmount; i ++ )
	{
		eEquipType ecColumn = static_cast<eEquipType>( 
			_GetDwordFromByteArray(pStream, lOffset) );
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(74);
#endif
		CGoods* pGoods = CGoodsFactory::UnserializeGoods( pStream, lOffset,38 );
		if( pGoods )
		{
			Add( ecColumn, pGoods );
		}
	}

	return TRUE;
}


BOOL CEquipmentContainer::Serialize( DBWriteSet& setWriteDB, BOOL b  )
{
	setWriteDB.AddToByteArray( GetGoodsAmount() );

	map<eEquipType, CGoods*>::iterator it = m_mEquipment.begin();
	for( ; it != m_mEquipment.end(); it ++ )
	{
		if( it->second )
		{
			if( GoodsSetup::QueryGoodsBaseProperties( 
				it->second->GetBasePropertiesIndex() ) )
			{
				setWriteDB.AddToByteArray( static_cast<DWORD>(it->first) );
				it->second->Serialize( setWriteDB,  b ? true : false );
			}
		}
	}

	return TRUE;
}
BOOL CEquipmentContainer::Unserialize( DBReadSet& setReadDB, BOOL b )
{
	Clear();
	DWORD dwGoodsAmount = setReadDB.GetDwordFromByteArray( );
	
	for( DWORD i = 0; i < dwGoodsAmount; i ++ )
	{
		eEquipType ecColumn = static_cast<eEquipType>( 
			setReadDB.GetDwordFromByteArray() );
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(74);
#endif
		CGoods* pGoods = CGoodsFactory::UnserializeGoods( setReadDB,39 );
		if( pGoods )
		{
			Add( ecColumn, pGoods, NULL );
		}
	}

	return TRUE;
}

//##ModelId=41C0E6DB0119
CBaseObject* CEquipmentContainer::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	return CGoodsContainer::Remove( dwPosition, dwAmount, pVoid );
}
