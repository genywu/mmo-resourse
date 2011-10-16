// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"
#include "CAmountLimitGoodsContainer.h"
#include "CContainer.h"
#include "..\Message Packaging\CS2CContainerObjectMove.h"



//##ModelId=41A6C4F4030D
CAmountLimitGoodsContainer::CAmountLimitGoodsContainer()
{
	// TODO: Add your specialized code here.
	m_dwGoodsAmountLimit = 1;
	AddListener( this );
}

////##ModelId=41A6C4FD00FA
//CAmountLimitGoodsContainer::CAmountLimitGoodsContainer(LONG lID) : CGoodsContainer( lID )
//{
//	// TODO: Add your specialized code here.
//	m_dwGoodsAmountLimit = 1;
//	AddListener( this );
//}

//##ModelId=41A6C50402FD
CAmountLimitGoodsContainer::~CAmountLimitGoodsContainer()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41AFC3CC00E5
BOOL CAmountLimitGoodsContainer::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
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
	//	CContainer::CalculateHashValue( pGoods -> GetType(), pGoods -> GetID() );
	m_mGoods[pGoods->GetExID()] = pGoods;

	for( size_t i = 0; i < m_vListeners.size(); i ++ )
	{
		if( m_vListeners[i] )
		{
			m_vListeners[i] -> OnObjectAdded( this, pGoods, pGoods -> GetAmount(), pVoid );
		}
	}

	return TRUE;
}

//##ModelId=41AFC3E302AA
VOID CAmountLimitGoodsContainer::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	goodsIt it = m_mGoods.begin(); 
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it -> second == NULL )
		{
			continue;
		}
		for( size_t i = 0; i < m_vListeners.size(); i ++ )
		{
			if( m_vListeners[i] )
			{
				m_vListeners[i] -> OnObjectRemoved( this, it -> second, 
					it -> second -> GetAmount(), pVoid );
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
	if( GetContainerMode() == CGoodsContainer::GCM_NORMAL )
	{
		goodsIt it	= m_mGoods.begin();
		for( ; it != m_mGoods.end(); it ++ )
		{
#ifdef _GOODSLOG1_
			if(it->second)
			{
				char pszGoodsLog[1024]="";			
				char srcGoodsGUID[50]="";
				it->second->GetExID().tostring(srcGoodsGUID);					
				_snprintf(pszGoodsLog,1024,"[物品回收:CAmountLimitGoodsContainer析构][物品ID:%d][物品名:%s][物品GUID:%s][物品数量:%d]",
					it->second->GetBasePropertiesIndex(),CGoodsFactory::QueryGoodsName(it->second->GetBasePropertiesIndex()),srcGoodsGUID,it->second->GetAmount());
				CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
			}			
#endif	
			CGoodsFactory::GarbageCollect( &(it -> second) ,1);
		}
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

	pContainer -> m_dwGoodsAmountLimit	= this -> m_dwGoodsAmountLimit;
	pContainer -> m_mGoods				= this -> m_mGoods;

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

	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		//if( IsLocked(it -> second) == FALSE )
		//{
#ifdef _GOODSLOG1_
		char pszGoodsLog[1024]="";			
		char srcGoodsGUID[50]="";
		it->first.tostring(srcGoodsGUID);					
		_snprintf(pszGoodsLog,1024,"[GUID1][物品GUID:%s]",srcGoodsGUID);
		CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);							
#endif
			if( pListener -> OnTraversingContainer(this, it -> second) == FALSE )
			{
				//break;
			}
		//}*/
	}
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
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( dwIndex == dwPosition )
		{
			if( IsLocked(it -> second) == FALSE )
			{
				pResult = it -> second;
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
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it -> second && it -> second -> GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			if( IsLocked(it -> second) == FALSE )
			{
				vOut.push_back( it -> second );
			}
		}
	}
}

//##ModelId=41BFA2FB0290
CGoods* CAmountLimitGoodsContainer::GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it -> second && it -> second -> GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			if( IsLocked(it -> second) == FALSE )
			{
				pResult	= it -> second;
				break;
			}
		}
	}
	return pResult;
}

CGoods* CAmountLimitGoodsContainer::GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> &vNotInThis)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it -> second && it -> second -> GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			BOOL bContinue = FALSE;
			for (LONG i =0; i < vNotInThis.size(); ++i)
			{
				if(vNotInThis[i] == it -> second)
				{
					bContinue = TRUE;
					break;
				}
			}
			if(bContinue)continue;
			if( IsLocked(it -> second) == FALSE )
			{
				pResult	= it -> second;
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
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it -> second == pGoods )
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
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it -> second && it -> second -> GetBasePropertiesIndex() == 
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
		//	pGoods -> GetType(), pGoods -> GetID() );
		itLockGoods it = m_vLockedGoods.begin();
		for( ; it != m_vLockedGoods.end(); it ++ )
		{
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
CBaseObject* CAmountLimitGoodsContainer::Find(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	goodsIt it = m_mGoods.find( guid );

	if( it != m_mGoods.end() )
	{
		//if( IsLocked(it -> second) == FALSE )
		//{
			return it -> second;
		//}
	}

	return NULL;
}

CBaseObject* CAmountLimitGoodsContainer::FindIgnoreLock(const CGUID& guid)
{
	goodsIt it = m_mGoods.find( guid );

	if( it != m_mGoods.end() )
	{
		return it -> second;
	}

	return NULL;
}

//##ModelId=41AF080B038A
CBaseObject* CAmountLimitGoodsContainer::Remove(const CGUID& guid, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	goodsIt it = m_mGoods.find( guid );

	if( it != m_mGoods.end() )
	{
		//if( IsLocked(it -> second) == FALSE )		
		pResult = it -> second;		
	}

	if( pResult )
	{
		for( size_t i = 0; i < m_vListeners.size(); i ++ )
		{
			if( m_vListeners[i] )
			{
				m_vListeners[i] -> OnObjectRemoved( this, pResult, pResult -> GetAmount(), pVoid );
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
		//	pGoods -> GetType(), pGoods -> GetID() );
		itLockGoods it = m_vLockedGoods.begin();
		for( ; it != m_vLockedGoods.end(); it ++ )
		{
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
BOOL CAmountLimitGoodsContainer::Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return CAmountLimitGoodsContainer::Add( pObject, pPreviousContainer, pVoid );
}

//##ModelId=41BFA2680186
VOID CAmountLimitGoodsContainer::AI()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it -> second )
		{
			it -> second -> AI();
		}
	}
}

//##ModelId=41BFA29701D4
VOID CAmountLimitGoodsContainer::SetOwner(LONG lType, const CGUID& OwnerGuid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer::SetOwner( lType, OwnerGuid );
}

//##ModelId=41BFA2E302EE
BOOL CAmountLimitGoodsContainer::QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult	= FALSE;
	DWORD dwIndex	= 0;
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it -> second &&  it -> second->GetExID() == guid )
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
BOOL CAmountLimitGoodsContainer::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer* pGoodsContainer	= dynamic_cast<CGoodsContainer*>( pContainer );
	CGoods* pGoods						= dynamic_cast<CGoods*>( pObj );
	CS2CContainerObjectMove* pMsg		= reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
	
	if( pGoodsContainer && pGoods && pMsg )
	{
		DWORD dwPosition = 0;
		if( pGoodsContainer -> QueryGoodsPosition(pGoods, dwPosition) )
		{
			pMsg -> SetDestinationContainer( GetOwnerType(), GetOwnerID(), dwPosition );
			pMsg -> SetDestinationObject( pGoods -> GetType(),pGoods->GetExID() );
			pMsg -> SetDestinationObjectAmount( pGoods -> GetAmount() );
		}
	}
	return TRUE;
}

//##ModelId=41BFBAE102CE
BOOL CAmountLimitGoodsContainer::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer* pGoodsContainer	= dynamic_cast<CGoodsContainer*>( pContainer );
	CGoods* pGoods						= dynamic_cast<CGoods*>( pObj );
	CS2CContainerObjectMove* pMsg		= reinterpret_cast<CS2CContainerObjectMove*>( pVoid );

	if( pGoodsContainer && pGoods && pMsg )
	{
		DWORD dwPosition = 0;
		if( pGoodsContainer -> QueryGoodsPosition(pGoods, dwPosition) )
		{
			pMsg -> SetSourceContainer( GetOwnerType(), GetOwnerID(), dwPosition );
			pMsg -> SetSourceObject( pGoods -> GetType(),pGoods->GetExID() );
			pMsg -> SetSourceObjectAmount( dwAmount );
		}
	}
	return TRUE;
}

//##ModelId=41BFEAE30000
DWORD CAmountLimitGoodsContainer::GetGoodsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwAmount = 0;
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it -> second )
		{
			if( CGoodsFactory::QueryGoodsBaseProperties( 
				it -> second -> GetBasePropertiesIndex() ) )
			{
				++ dwAmount;
			}
		}
	}
	return dwAmount;
}

DWORD CAmountLimitGoodsContainer::GetGoodsAmountByName(const char* strName)
{
	DWORD dwAmount=0;
	goodsIt it=m_mGoods.begin();
	for(;it!=m_mGoods.end();it++)
	{
		if(it->second)
		{
			const char* strGoodsName=CGoodsFactory::QueryGoodsName(it->second->GetBasePropertiesIndex());
			if(strGoodsName)
			{
				if(strcmp(strName,strGoodsName)==0)
				{
					dwAmount+=it->second->GetAmount();
				}
			}			
		}
	}
	return dwAmount;
}

DWORD CAmountLimitGoodsContainer::GetGoodsAmountByOriginName(const char* strOriginName)
{
	DWORD dwAmount=0;
	goodsIt it=m_mGoods.begin();
	for(;it!=m_mGoods.end();it++)
	{
		if(it->second)
		{
			const char* strGoodsName=CGoodsFactory::QueryGoodsOriginalName(it->second->GetBasePropertiesIndex());
			if(strGoodsName)
			{
				if(strcmp(strOriginName,strGoodsName)==0)
				{
					dwAmount+=it->second->GetAmount();
				}
			}

		}
	}
	return dwAmount;
}


DWORD CAmountLimitGoodsContainer::GetGoodsAmountByType(int nType)
{
	DWORD dwAmount=0;
	goodsIt it=m_mGoods.begin();
	for(;it!=m_mGoods.end();it++)
	{
		if(it->second)
		{
			long lAddonVal=it->second->GetAddonPropertyValues(GAP_WEAPON_CATEGORY,1);
			if(nType==lAddonVal)
			{
				dwAmount+=it->second->GetAmount();
			}
		}
	}
	return dwAmount;
}

CBaseObject* CAmountLimitGoodsContainer::Remove( CBaseObject* pObject, LPVOID pVoid )
{
	return CGoodsContainer::Remove( pObject, pVoid );
}

CBaseObject* CAmountLimitGoodsContainer::Remove( LONG oType, const CGUID& guid, LPVOID pVoid )
{
	return CGoodsContainer::Remove( oType, guid, pVoid );
}

CBaseObject* CAmountLimitGoodsContainer::Find( CBaseObject* pObj )
{
	return CGoodsContainer::Find( pObj );
}

CBaseObject* CAmountLimitGoodsContainer::Find( LONG oType, const CGUID& guid )
{
	return CGoodsContainer::Find( oType, guid );
}

BOOL CAmountLimitGoodsContainer::Serialize( DBWriteSet& setWriteDB, BOOL b  )
{
	setWriteDB.AddToByteArray( GetGoodsAmount() );

	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it -> second )
		{
			if( CGoodsFactory::QueryGoodsBaseProperties( 
				it -> second -> GetBasePropertiesIndex() ) )
			{
				it -> second -> Serialize( setWriteDB, b );
			}
		}
	}

	return TRUE;
}
BOOL CAmountLimitGoodsContainer::Unserialize( DBReadSet& setReadDB, BOOL b )
{
	CAmountLimitGoodsContainer::Clear();

	DWORD dwGoodsAmount = setReadDB.GetDwordFromByteArray( );
	
	for( DWORD i = 0; i < dwGoodsAmount; i ++ )
	{
		CGoods* pGoods = CGoodsFactory::UnserializeGoods(setReadDB,12);
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
