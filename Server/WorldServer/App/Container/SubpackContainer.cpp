// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "StdAfx.h"
#include "SubpackContainer.h"
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

CSubpackContainer::CSubpackContainer()
{
	// TODO: Add your specialized code here.
	InitSubpack();
	AddListener( this );
}


CSubpackContainer::~CSubpackContainer()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41B3D0AE026C
BOOL CSubpackContainer::Add(CBaseObject* pObject, LPVOID pVoid)
{
	
	return FALSE;
}

//##ModelId=41B3D0C20191
VOID CSubpackContainer::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	
	for(int j=0 ;j<SUBPACK_NUM; j++)
	{
		if( m_Subpack[j].pGoods== NULL )
		{
			continue;
		}
		for( size_t i = 0; i < m_vListeners.size(); i ++ )
		{
			if( m_vListeners[i] )
			{
				m_vListeners[i]->OnObjectRemoved( this,m_Subpack[j].pGoods, pVoid );
			}
		}
		m_Subpack[j].pGoods=NULL;
	}	
}

//##ModelId=41B3D0C9024D
VOID CSubpackContainer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		//CGoodsFactory::GarbageCollect( &(m_Subpack[i].pGoods) );
		if(m_Subpack[i].pGoods)
		{
			//delete m_Subpack[i].pGoods;
			CGoodsFactory::GarbageCollect( &(m_Subpack[i].pGoods) );
			//m_Subpack[i].pGoods=NULL;
		}
	}

	
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		if(m_Subpack[i].pSubpackContainer)
		{
			delete m_Subpack[i].pSubpackContainer;
			m_Subpack[i].pSubpackContainer=NULL;
		}
	}
	//m_mEquipment.clear();
	CGoodsContainer::Release();
	AddListener( this );
}

VOID CSubpackContainer::Release(DWORD dwPos)
{
	if(dwPos>=0 && dwPos<SUBPACK_NUM)
	{
		if(m_Subpack[dwPos].pGoods)
		{
			//delete m_Subpack[i].pGoods;
			CGoodsFactory::GarbageCollect( &(m_Subpack[dwPos].pGoods) );
			m_Subpack[dwPos].pGoods=NULL;

			m_Subpack[dwPos].pSubpackContainer->Release();
		}
	}
}

//! 清空装载的所有物品
VOID CSubpackContainer::ReleaseAllGoods()
{
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		if(NULL != m_Subpack[i].pGoods)
		{
			CGoodsFactory::GarbageCollect( &(m_Subpack[i].pGoods) );
		}
		if(NULL != m_Subpack[i].pSubpackContainer)
		{
			m_Subpack[i].pSubpackContainer->Release();
		}
	}
}

VOID CSubpackContainer::InitSubpack()
{
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		m_Subpack[i].pGoods=NULL;
		m_Subpack[i].pSubpackContainer=new CVolumeLimitGoodsContainer();
		m_Subpack[i].pSubpackContainer->SetContainerVolume((DWORD)0);
	}
}

VOID CSubpackContainer::ReleaseSubpackGoods()
{
	for(int i=0;i<SUBPACK_NUM;i++)
	{		
		if(m_Subpack[i].pGoods)
		{			
			CGoodsFactory::GarbageCollect( &(m_Subpack[i].pGoods) );			
		}
	}
}

//##ModelId=41B418F902D3
VOID CSubpackContainer::TraversingContainer(CContainerListener* pListener)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pListener == NULL ) 
	{
		return;
	}

	for(int i=0;i<SUBPACK_NUM;i++)
	{
		if( pListener->OnTraversingContainer(this, m_Subpack[i].pGoods) == FALSE )
		{
			//break;
		}
	}
}

//##ModelId=41B5456901AB
DWORD CSubpackContainer::GetContentsWeight()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwResult = 0;
	/*
	map<EQUIPMENT_COLUMN, CGoods*>::iterator it = m_mEquipment.begin();
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
BOOL CSubpackContainer::IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		if(m_Subpack[i].pGoods && m_Subpack[i].pGoods-> GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			bResult	= TRUE;
			break;
		}
	}
	return bResult;
}

//##ModelId=41B570CD0068
BOOL CSubpackContainer::Swap(CSubpackContainer::SUBPACK_COLUMN ecColumn, CGoods* pIn, CGoods** ppOut, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return FALSE;
}

//##ModelId=41B3D0A1025D
CBaseObject* CSubpackContainer::Find(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		if(m_Subpack[i].pGoods)
		{
			if( m_Subpack[i].pGoods->GetExID() == guid )
			{
				pResult = m_Subpack[i].pGoods;
				break;
			}
		}
	}
	return pResult;
}

//##ModelId=41B3D0B80395
CBaseObject* CSubpackContainer::Remove(const CGUID& guid, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	int index=-1;
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		if(m_Subpack[i].pGoods)
		{
			if(m_Subpack[i].pGoods->GetExID() == guid )
			{
				pResult =m_Subpack[i].pGoods;
				index=i;
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
		m_Subpack[index].pGoods=NULL;
	}
	
	return pResult;
}



//##ModelId=41BF9DAB0109
BOOL CSubpackContainer::Add(DWORD dwPosition, CGoods* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pObject == NULL )
	{
		return FALSE;
	}

	SUBPACK_COLUMN ecColumn = static_cast<SUBPACK_COLUMN>( dwPosition );
	int index=ecColumn-10001;	
	if(m_Subpack[index].pGoods)
	{		
// 		char str[256];
// 		sprintf(str, "此(名称:%s)装备装备在%d位置出错,此位置已有装备。",pObject->GetName(),dwPosition);
// 		PutStringToFile("debug-DB", str);

        Log4c::Trace(ROOT_MODULE,"此(名称:%s)装备装备在%d位置出错,此位置已有装备。",pObject->GetName(),dwPosition);
		return FALSE;		
	}

	BOOL bResult = FALSE;
	CGoodsBaseProperties* pProperties = 
		GoodsSetup::QueryGoodsBaseProperties( pObject->GetBasePropertiesIndex() );
	if( pProperties )
	{		
		if( pProperties->GetGoodsBaseType() == GBT_SUBPACK )
		{
			m_Subpack[index].pGoods=pObject;
			bResult=TRUE;
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
BOOL CSubpackContainer::AddFromDB( CGoods* pObject, DWORD dwPosition )
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

        Log4c::Trace(ROOT_MODULE,"装备:[%s](%s)在%d位置已有[%s](%s)。",pObject->GetName(),szGuid,
            dwPosition,	pGoods->GetName(), szExGuid );
		return FALSE;
	}

	BOOL bResult = FALSE;
	SUBPACK_COLUMN ecColumn = static_cast<SUBPACK_COLUMN>( dwPosition );
	CGoodsBaseProperties* pProperties = 
		GoodsSetup::QueryGoodsBaseProperties( pObject->GetBasePropertiesIndex() );
	if( pProperties )
	{		
		if(pProperties->GetGoodsBaseType() == GBT_SUBPACK)
		{
			m_Subpack[ecColumn-10001].pGoods=pObject;
			DWORD dwVolume=pObject->GetAddonPropertyValues(GAP_PACK_CAPACITY,1);
			m_Subpack[ecColumn-10001].pSubpackContainer->SetContainerVolume(dwVolume);

			bResult=TRUE;
		}	
	}
	return bResult;	
}
//##ModelId=41BFA0F30177
VOID CSubpackContainer::AI()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		if( m_Subpack[i].pGoods )
		{
			m_Subpack[i].pGoods-> AI();
		}
	}
}

//##ModelId=41BFA1030271
BOOL CSubpackContainer::IsFull()
{
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		if(m_Subpack[i].pGoods==NULL)
			return FALSE;
	}
	return TRUE;
}

//##ModelId=41BFA16E03B9
BOOL CSubpackContainer::QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pGoods == NULL )
	{
		return FALSE;
	}
	BOOL bResult = FALSE;	
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		if(m_Subpack[i].pGoods== pGoods )
		{
			dwPosition	=10001+i;
			bResult		= TRUE;
			break;
		}
	}
	return bResult;
}

//##ModelId=41BFA17A00BB
BOOL CSubpackContainer::QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition)
{
	BOOL bResult = FALSE;	
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		if(m_Subpack[i].pGoods && m_Subpack[i].pGoods->GetExID() == guid)
		{
			dwPosition	=10001+i;
			bResult		= TRUE;
			break;
		}
	}
	return bResult;
}

//##ModelId=41BFA197036B
CGoods* CSubpackContainer::GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;	
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		if(m_Subpack[i].pGoods && m_Subpack[i].pGoods-> GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			pResult	= m_Subpack[i].pGoods;
			break;
		}
	}
	return pResult;
}

//##ModelId=41BFA1AC0242
CGoods* CSubpackContainer::GetGoods(DWORD dwPosition)
{
	// TODO: Add your specialized code here.
	CGoods* pResult				= NULL;
	SUBPACK_COLUMN ecColumn	= static_cast<SUBPACK_COLUMN>( dwPosition );
	int index=ecColumn-10001;
	if( m_Subpack[index].pGoods)
	{
		pResult =m_Subpack[index].pGoods;
	}	
	return pResult;
}

//##ModelId=41BFA1B8007D
VOID CSubpackContainer::GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		if(m_Subpack[i].pGoods && m_Subpack[i].pGoods-> GetBasePropertiesIndex() == 
			dwGoodsBasePropertiesIndex )
		{
			vOut.push_back( m_Subpack[i].pGoods);
		}
	}
}

//##ModelId=41BFB91C01D4
BOOL CSubpackContainer::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid)
{
	return TRUE;
}

//##ModelId=41BFB92601A5
BOOL CSubpackContainer::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid)
{
	return TRUE;
}

//##ModelId=41BFEAD70138
DWORD CSubpackContainer::GetGoodsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwAmount = 0;	
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		if(m_Subpack[i].pGoods)
		{
			if( GoodsSetup::QueryGoodsBaseProperties( 
				m_Subpack[i].pGoods-> GetBasePropertiesIndex() ))
			{
				++ dwAmount;
			}
		}
	}
	return dwAmount;
}

CSubpackContainer::tagSubpack* CSubpackContainer::GetSubpack(int i)
{
	if(i>=0 && i<SUBPACK_NUM)
	{
		return &m_Subpack[i];
	}
	return NULL;
}

CBaseObject* CSubpackContainer::Remove( CBaseObject* pObject, LPVOID pVoid )
{
	return CGoodsContainer::Remove( pObject, pVoid );
}

CBaseObject* CSubpackContainer::Remove( LONG oType, const CGUID& guid, LPVOID pVoid )
{
	return CGoodsContainer::Remove( oType, guid, pVoid );
}

CBaseObject* CSubpackContainer::Find( CBaseObject* pObj )
{
	return CGoodsContainer::Find( pObj );
}

CBaseObject* CSubpackContainer::Find( LONG oType, const CGUID& guid )
{
	return CGoodsContainer::Find( oType, guid );
}

BOOL CSubpackContainer::Serialize( vector<BYTE>* pStream, BOOL b )
{
	_AddToByteArray( pStream, GetGoodsAmount() );
	
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		if(m_Subpack[i].pGoods)
		{
			if( GoodsSetup::QueryGoodsBaseProperties( 
				m_Subpack[i].pGoods-> GetBasePropertiesIndex() ) )
			{
				_AddToByteArray( pStream, static_cast<DWORD>(10001+i) );
				m_Subpack[i].pGoods-> Serialize( pStream,  b ? true : false );				
				m_Subpack[i].pSubpackContainer->Serialize(pStream,b);
			}
		}
	}
	return TRUE;
}



BOOL CSubpackContainer::Unserialize( BYTE* pStream, LONG& lOffset, BOOL b )
{
	//CSubpackContainer::Clear();
	Release();

	DWORD dwGoodsAmount = _GetDwordFromByteArray( pStream, lOffset );
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		m_Subpack[i].pSubpackContainer=new CVolumeLimitGoodsContainer();
		m_Subpack[i].pSubpackContainer->SetContainerVolume((DWORD)0);

	}
	
	for( DWORD i = 0; i < dwGoodsAmount; i ++ )
	{
		SUBPACK_COLUMN ecColumn = static_cast<SUBPACK_COLUMN>( 
			_GetDwordFromByteArray(pStream, lOffset) );
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(77);
#endif
		CGoods* pGoods = CGoodsFactory::UnserializeGoods( pStream, lOffset,42 );
		if( pGoods )
		{
			Add( ecColumn, pGoods, NULL );
		}
		DWORD dwVal=pGoods->GetAddonPropertyValues(GAP_PACK_CAPACITY,1);
		m_Subpack[ecColumn-10001].pSubpackContainer->SetContainerVolume(dwVal);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(781);
#endif
		m_Subpack[ecColumn-10001].pSubpackContainer->Unserialize(pStream,lOffset);
	}
	return TRUE;
}

BOOL CSubpackContainer::Serialize( DBWriteSet& setWriteDB, BOOL b  )
{
	setWriteDB.AddToByteArray( GetGoodsAmount() );
	
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		if(m_Subpack[i].pGoods)
		{
			if( GoodsSetup::QueryGoodsBaseProperties( 
				m_Subpack[i].pGoods-> GetBasePropertiesIndex() ) )
			{
				setWriteDB.AddToByteArray( static_cast<DWORD>(10001+i) );
				m_Subpack[i].pGoods-> Serialize( setWriteDB,  b ? true : false  );				
				m_Subpack[i].pSubpackContainer->Serialize(setWriteDB,b);
			}
		}
	}
	return TRUE;
}
BOOL CSubpackContainer::Unserialize( DBReadSet& setReadDB, BOOL b )
{
	Release();
	DWORD dwGoodsAmount = setReadDB.GetDwordFromByteArray( );
	for(int i=0;i<SUBPACK_NUM;i++)
	{
		m_Subpack[i].pSubpackContainer=new CVolumeLimitGoodsContainer();
		m_Subpack[i].pSubpackContainer->SetContainerVolume((DWORD)0);

	}

	for( DWORD i = 0; i < dwGoodsAmount; i ++ )
	{
		SUBPACK_COLUMN ecColumn = static_cast<SUBPACK_COLUMN>( 
			setReadDB.GetDwordFromByteArray( ) );
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(77);
#endif
		CGoods* pGoods = CGoodsFactory::UnserializeGoods( setReadDB,43);
		if( pGoods )
		{
			Add( ecColumn, pGoods, NULL );
		}
		DWORD dwVal=pGoods->GetAddonPropertyValues(GAP_PACK_CAPACITY,1);
		m_Subpack[ecColumn-10001].pSubpackContainer->SetContainerVolume(dwVal);
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(781);
#endif
		m_Subpack[ecColumn-10001].pSubpackContainer->Unserialize(setReadDB);
	}
	return TRUE;
}

//##ModelId=41C0E6DB0119
CBaseObject* CSubpackContainer::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	return CGoodsContainer::Remove( dwPosition, dwAmount, pVoid );
}