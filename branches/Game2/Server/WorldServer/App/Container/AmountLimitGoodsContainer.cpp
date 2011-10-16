// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "StdAfx.h"
#include "AmountLimitGoodsContainer.h"
#include "Container.h"
#include "../Goods/Goods.h"
#include "../Goods/GoodsFactory.h"

#include "../../../../Public/Setup/GoodsSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//##ModelId=41A6C4F4030D
CAmountLimitGoodsContainer::CAmountLimitGoodsContainer()
{
	// TODO: Add your specialized code here.
	m_dwGoodsAmountLimit = 1;
	AddListener( this );
}

//##ModelId=41A6C50402FD
CAmountLimitGoodsContainer::~CAmountLimitGoodsContainer()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41AFC3CC00E5
BOOL CAmountLimitGoodsContainer::Add(CBaseObject* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( IsFull() ) 
	{
		return FALSE;
	}

	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods == NULL )
	{
		return FALSE;
	}

	//register DWORD dwHashValue = 
	//	CContainer::CalculateHashValue( pGoods->GetType(), pGoods->GetID() );
	m_mGoods[pGoods->GetExID()] = pGoods;

	for( size_t i = 0; i < m_vListeners.size(); i ++ )
	{
		if( m_vListeners[i] )
		{
			m_vListeners[i]->OnObjectAdded( this, pGoods, pVoid );
		}
	}

	return TRUE;
}
//##ModelId=41AFC3CC00E5
BOOL CAmountLimitGoodsContainer::AddFromDB(CGoods* pObject, DWORD dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( IsFull() ) 
	{
		return bResult;
	}

	CGoods* pGoods = GetGoods( dwPosition );
	if( pGoods ) //m_pGoldCoins存在了
	{
//		char str[256];
		char szGuid[40];
		char szExGuid[40];

		CGUID guid,guidExsit;
		guid = pObject->GetExID();
		guid.tostring(szGuid);
		guidExsit = pGoods->GetExID();
		guidExsit.tostring(szExGuid);

// 		sprintf(str, "鼠标上:[%s](%s)在%d位置已有[%s](%s)。",pObject->GetName(),szGuid,
// 			dwPosition,	pGoods->GetName(), szExGuid );
// 		PutStringToFile("debug-DB", str);
        Log4c::Trace(ROOT_MODULE, "鼠标上:[%s](%s)在%d位置已有[%s](%s)。",pObject->GetName(),szGuid,
            dwPosition,	pGoods->GetName(), szExGuid );
		return bResult;
	}

	//register DWORD dwHashValue = 
	//	CContainer::CalculateHashValue( pGoods->GetType(), pGoods->GetID() );
	if( pObject )
	{
		m_mGoods[pObject->GetExID()] = pObject;
		bResult = TRUE;
	}

	return bResult;
}

//##ModelId=41AFC3E302AA
VOID CAmountLimitGoodsContainer::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//map<DWORD, CGoods*>::iterator it = m_mGoods.begin(); 
	goodsIt it = m_mGoods.begin(); 
	for( ; it != m_mGoods.end(); it ++ )
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
	m_mGoods.clear();
	m_vLockedGoods.clear();
}

//##ModelId=41AFC3F0024D
VOID CAmountLimitGoodsContainer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_dwGoodsAmountLimit				= 1;
	//map<DWORD, CGoods*>::iterator it	= m_mGoods.begin();
	goodsIt it	= m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		CGoodsFactory::GarbageCollect( &(it->second) );
	}
	m_mGoods.clear();
	m_vLockedGoods.clear();

	CGoodsContainer::Release();
	AddListener( this );
}

//##ModelId=41BFA32503C8
BOOL CAmountLimitGoodsContainer::Clone(CGoodsContainer* pObject)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CAmountLimitGoodsContainer* pContainer = 
		dynamic_cast<CAmountLimitGoodsContainer*>( pObject );
	if( pContainer == NULL )
	{
		return FALSE;
	}

	pContainer->m_dwGoodsAmountLimit	= this->m_dwGoodsAmountLimit;
	pContainer->m_mGoods				= this->m_mGoods;

	return TRUE;
}

//##ModelId=41B00EDE0246
VOID CAmountLimitGoodsContainer::TraversingContainer(CContainerListener* pListener)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pListener == NULL ) 
	{
		return;
	}

	//map<DWORD,CGoods*>::iterator it = m_mGoods.begin();
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		//if( IsLocked(it->second) == FALSE )
		//{
			if( pListener->OnTraversingContainer(this, it->second) == FALSE )
			{
				//break;
			}
		//}
	}
}

//##ModelId=41B5413D014A
DWORD CAmountLimitGoodsContainer::GetContentsWeight()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwResult = 0;
	//map<DWORD, CGoods*>::iterator it = m_mGoods.begin();
	/*
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it->second )
		{
			dwResult += it->second->GetWeight();
		}
	}*/
	return dwResult;
}

//##ModelId=41B021240144
CGoods* CAmountLimitGoodsContainer::GetGoods(DWORD dwPosition)
{
	// TODO: Add your specialized code here.
	if( dwPosition >= m_dwGoodsAmountLimit )
	{
		return NULL;
	}

	CGoods*	pResult	= NULL;
	DWORD	dwIndex	= 0;
	//map<DWORD, CGoods*>::iterator it = m_mGoods.begin();
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( dwIndex == dwPosition )
		{
			if( IsLocked(it->second) == FALSE )
			{
				pResult = it->second;
			}
			break;
		}
		++ dwIndex;
	}

	return pResult;
}

//##ModelId=41B690D2003E
VOID CAmountLimitGoodsContainer::GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//map<DWORD, CGoods*>::iterator it = m_mGoods.begin();
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it->second && it->second->GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			if( IsLocked(it->second) == FALSE )
			{
				vOut.push_back( it->second );
			}
		}
	}
}

//##ModelId=41BFA2FB0290
CGoods* CAmountLimitGoodsContainer::GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	//map<DWORD, CGoods*>::iterator it = m_mGoods.begin();
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it->second && it->second->GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			if( IsLocked(it->second) == FALSE )
			{
				pResult	= it->second;
				break;
			}
		}
	}
	return pResult;
}

//##ModelId=41BFA2DB0213
BOOL CAmountLimitGoodsContainer::QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pGoods == NULL ) 
	{
		return FALSE;
	}

	BOOL bResult	= FALSE;
	DWORD dwIndex	= 0;
	//map<DWORD,CGoods*>::iterator it = m_mGoods.begin();
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it->second == pGoods )
		{
			bResult		= TRUE;
			dwPosition	= dwIndex;
			break;
		}
		++ dwIndex;
	}
	return bResult;
}

//##ModelId=41BFA2F1007D
BOOL CAmountLimitGoodsContainer::IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	//map<DWORD, CGoods*>::iterator it = m_mGoods.begin();
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
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

//##ModelId=41B7E36D03D7
BOOL CAmountLimitGoodsContainer::Lock(CGoods* pGoods)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( pGoods ) 
	{
		if( IsLocked(pGoods) == FALSE )
		{
			//m_vLockedGoods.push_back( CGoodsContainer::CalculateHashValue( 
			//	pGoods->GetType(), pGoods->GetID() ) );
			m_vLockedGoods.push_back( pGoods->GetExID() );
			bResult = TRUE;
		}
	}
	return bResult;
}

//##ModelId=41B7E3870165
BOOL CAmountLimitGoodsContainer::Unlock(CGoods* pGoods)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( pGoods ) 
	{
		//DWORD dwHashValue =  CGoodsContainer::CalculateHashValue( 
		//	pGoods->GetType(), pGoods->GetID() );
		//vector<DWORD>::iterator it = m_vLockedGoods.begin();
		itLockGoods it = m_vLockedGoods.begin();
		for( ; it != m_vLockedGoods.end(); it ++ )
		{
			//if( *it == dwHashValue )
			if( *it == pGoods->GetExID() )
			{
				break;
			}
		}

		if( it != m_vLockedGoods.end() )
		{
			m_vLockedGoods.erase( it );
			bResult = TRUE;
		}
	}

	return bResult;
}

//##ModelId=41BFA280004E
BOOL CAmountLimitGoodsContainer::IsFull()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( GetGoodsAmount() >= m_dwGoodsAmountLimit ) 
	{
		return TRUE;
	}
	return FALSE;
}

//##ModelId=41A6C5350213
VOID CAmountLimitGoodsContainer::SetGoodsAmountLimit(DWORD dwMaxNumber)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_dwGoodsAmountLimit = dwMaxNumber;
}

//##ModelId=41B020E001B1
DWORD CAmountLimitGoodsContainer::GetGoodsAmountLimit()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return m_dwGoodsAmountLimit;
}

//##ModelId=41AF080400BB
//CBaseObject* CAmountLimitGoodsContainer::Find(DWORD dwHashCode)
CBaseObject* CAmountLimitGoodsContainer::Find(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	//map<DWORD, CGoods*>::iterator it = m_mGoods.find( dwHashCode );
	goodsIt it = m_mGoods.find( guid );
	if( it != m_mGoods.end() )
	{
		if( IsLocked(it->second) == FALSE )
		{
			return it->second;
		}
	}

	return NULL;
}

//##ModelId=41AF080B038A
//CBaseObject* CAmountLimitGoodsContainer::Remove(DWORD dwHashCode, LPVOID pVoid)
CBaseObject* CAmountLimitGoodsContainer::Remove(const CGUID& guid, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	//map<DWORD, CGoods*>::iterator it = m_mGoods.find( dwHashCode );
	goodsIt it = m_mGoods.find( guid );

	if( it != m_mGoods.end() )
	{
		if( IsLocked(it->second) == FALSE )
		{
			pResult = it->second;
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
		m_mGoods.erase( it );
	}

	return pResult;
}

//##ModelId=41B7F790000E
BOOL CAmountLimitGoodsContainer::IsLocked(CGoods* pGoods)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	
	if( pGoods ) 
	{
		//DWORD dwHashValue =  CGoodsContainer::CalculateHashValue( 
		//	pGoods->GetType(), pGoods->GetID() );
		//vector<DWORD>::iterator it = m_vLockedGoods.begin();
		itLockGoods it = m_vLockedGoods.begin();
		for( ; it != m_vLockedGoods.end(); it ++ )
		{
			//if( *it == dwHashValue )
			if( *it == pGoods->GetExID() )
			{
				bResult = TRUE;
				break;
			}
		}
	}

	return bResult;
}



//##ModelId=41BFA243005D
BOOL CAmountLimitGoodsContainer::Add(DWORD dwPosition, CGoods* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return CAmountLimitGoodsContainer::Add( pObject, pVoid );
}

//##ModelId=41BFA2680186
VOID CAmountLimitGoodsContainer::AI()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//map<DWORD,CGoods*>::iterator it = m_mGoods.begin();
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it->second )
		{
			it->second->AI();
		}
	}
}

//##ModelId=41BFA29701D4
VOID CAmountLimitGoodsContainer::SetOwner(LONG lType, LONG lID)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer::SetOwner( lType, lID );
}

//##ModelId=41BFA2E302EE
//BOOL CAmountLimitGoodsContainer::QueryGoodsPosition(DWORD dwHashValue, DWORD& dwPosition)
BOOL CAmountLimitGoodsContainer::QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct	return value to compile.
	BOOL bResult	= FALSE;
	DWORD dwIndex	= 0;
	//map<DWORD,CGoods*>::iterator it = m_mGoods.begin();
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		//if( it->second && CContainer::CalculateHashValue( it->second->GetType(), 
		//	it->second->GetID() ) == dwHashValue )
		if( it->second &&  it->second->GetExID() == guid )
		{
			bResult		= TRUE;
			dwPosition	= dwIndex;
			break;
		}
		++ dwIndex;
	} 

	return bResult;
}

//##ModelId=41BFBADA0251
BOOL CAmountLimitGoodsContainer::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid)
{
	return TRUE;
}

//##ModelId=41BFBAE102CE
BOOL CAmountLimitGoodsContainer::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid)
{
	return TRUE;
}

//##ModelId=41BFEAE30000
DWORD CAmountLimitGoodsContainer::GetGoodsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwAmount = 0;
	//map<DWORD, CGoods*>::iterator it = m_mGoods.begin();
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
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

CBaseObject* CAmountLimitGoodsContainer::Remove( CBaseObject* pObject, LPVOID pVoid )
{
	return CGoodsContainer::Remove( pObject, pVoid );
}

//CBaseObject* CAmountLimitGoodsContainer::Remove( LONG oType, LONG lID, LPVOID pVoid )
CBaseObject* CAmountLimitGoodsContainer::Remove( LONG oType, const CGUID& guid, LPVOID pVoid )
{
	//return CGoodsContainer::Remove( oType, lID, pVoid );
	return CGoodsContainer::Remove( oType, guid, pVoid );
}

CBaseObject* CAmountLimitGoodsContainer::Find( CBaseObject* pObj )
{
	return CGoodsContainer::Find( pObj );
}

//CBaseObject* CAmountLimitGoodsContainer::Find( LONG oType, LONG lID )
CBaseObject* CAmountLimitGoodsContainer::Find( LONG oType, const CGUID& guid )
{
	//return CGoodsContainer::Find( oType, lID );
	return CGoodsContainer::Find( oType, guid );
}

BOOL CAmountLimitGoodsContainer::Serialize( vector<BYTE>* pStream, BOOL b )
{
	_AddToByteArray( pStream, GetGoodsAmount() );

	//map<DWORD, CGoods*>::iterator it = m_mGoods.begin();
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it->second )
		{
			if( GoodsSetup::QueryGoodsBaseProperties( 
				it->second->GetBasePropertiesIndex() ) )
			{
                it->second->Serialize( pStream, b ? true : false );
			}
		}
	}

	return TRUE;
}

BOOL CAmountLimitGoodsContainer::Unserialize( BYTE* pStream, LONG& lOffset, BOOL b )
{
	Clear();

	DWORD dwGoodsAmount = _GetDwordFromByteArray( pStream, lOffset );
	
	for( DWORD i = 0; i < dwGoodsAmount; i ++ )
	{
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(73);
#endif
		CGoods* pGoods = CGoodsFactory::UnserializeGoods( pStream, lOffset,36 );
		if( pGoods )
		{
			Add( pGoods );
		}
	}

	return TRUE;
}


BOOL CAmountLimitGoodsContainer::Serialize( DBWriteSet& setWriteDB, BOOL b  )
{
	setWriteDB.AddToByteArray( GetGoodsAmount() );
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it->second )
		{
			if( GoodsSetup::QueryGoodsBaseProperties( 
				it->second->GetBasePropertiesIndex() ) )
			{
                it->second->Serialize( setWriteDB, b ? true : false );
			}
		}
	}

	return TRUE;
}
BOOL CAmountLimitGoodsContainer::Unserialize( DBReadSet& setReadDB, BOOL b )
{
	Clear();

	DWORD dwGoodsAmount = setReadDB.GetDwordFromByteArray( );
	
	for( DWORD i = 0; i < dwGoodsAmount; i ++ )
	{
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(73);
#endif
		CGoods* pGoods = CGoodsFactory::UnserializeGoods(setReadDB,37);
		if( pGoods )
		{
			Add( pGoods, NULL );
		}
	}

	return TRUE;
}

//##ModelId=41C0E6E40109
CBaseObject* CAmountLimitGoodsContainer::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return CGoodsContainer::Remove( dwPosition, dwAmount, pVoid );
}
