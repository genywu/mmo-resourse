// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include <math.h>
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"
#include "CVolumeLimitGoodsContainer.h"
#include "CContainer.h"
//#include "..\Message Packaging\CS2CContainerObjectMove.h"




//##ModelId=41A6D1450290
CVolumeLimitGoodsContainer::CVolumeLimitGoodsContainer()
{
	// TODO: Add your specialized code here.
	m_dwVolume=0;
	m_vCells.clear();
}

//##ModelId=41A6D158037A
CVolumeLimitGoodsContainer::~CVolumeLimitGoodsContainer()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41A6D1E602CE
VOID CVolumeLimitGoodsContainer::SetContainerVolume(DWORD dwVolume)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	Release();
	m_dwVolume=dwVolume;
	m_vCells.resize(dwVolume, NULL_GUID );
	SetGoodsAmountLimit(dwVolume );
}

VOID CVolumeLimitGoodsContainer::SetContainerVolume(CGoods* pGoods)
{
	if(pGoods)
	{
		DWORD dwLine=pGoods->GetAddonPropertyValues(GAP_PACK_CAPACITY,1);
		SetContainerVolume(dwLine);
	}
}

//##ModelId=41A6D20F0177
DWORD CVolumeLimitGoodsContainer::GetWidth()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return 5;
}

//##ModelId=41A6D231032C
DWORD CVolumeLimitGoodsContainer::GetHeight()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	double dVal=::ceil((double)(m_dwVolume/5));
	
	DWORD dwHeight=(DWORD)dVal;
	return dwHeight;
	
}

//##ModelId=41AFF60100B6
BOOL CVolumeLimitGoodsContainer::FindPositionForGoods(CGoods* pGoods, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pGoods == NULL ) return FALSE;
	if( pGoods -> GetMaxStackNumber() > 1 )
	{
		goodsIt it = m_mGoods.begin();
		for( ; it != m_mGoods.end(); it ++ )
		{
			if( it -> second )
			{
				if( it -> second -> GetBasePropertiesIndex() == 
					pGoods -> GetBasePropertiesIndex() )
				{
					if( it -> second -> GetAmount() + pGoods -> GetAmount() <=
						pGoods -> GetMaxStackNumber() )
					{
						if( QueryGoodsPosition(it -> second, dwPosition) )
						{
							return TRUE;
						}
					}
				}
			}
		}
	}

	for( size_t i = 0; i < m_vCells.size(); i ++ )
	{
		if( m_vCells[i] == CGUID::GUID_INVALID )
		{
			if( IsSpaceEnough( pGoods, static_cast<DWORD>(i) ) )
			{
				dwPosition = static_cast<DWORD>( i );
				return TRUE;
			}
		}
	}
	return FALSE;
}

//##ModelId=41AFCFCF00D1
BOOL CVolumeLimitGoodsContainer::IsSpaceEnough(CGoods* pGoods, DWORD dwFirstCell)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	
	if( pGoods == NULL || dwFirstCell >= m_vCells.size() || IsFull() )
	{
		return FALSE;
	}

	if(m_vCells[dwFirstCell]!=CGUID::GUID_INVALID)
		return FALSE;
	return TRUE;	
}

//##ModelId=41B00B310242
BOOL CVolumeLimitGoodsContainer::IsSpaceEnough(vector<CGoods*> vGoods)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = TRUE;

	CVolumeLimitGoodsContainer cTestContainer;
	Clone( &cTestContainer );

	for( size_t i = 0; i < vGoods.size(); i ++ )
	{
		if( cTestContainer.Add( vGoods[i] ) == FALSE )
		{
			bResult = FALSE;
			break;
		}
	}

	//##退出前先清理掉所有的数据，但是不能删除物品指针
	cTestContainer.Clear();

	return bResult;
}

//##ModelId=41BFA5430213
BOOL CVolumeLimitGoodsContainer::Swap(DWORD dwFirstCell, CGoods* pIn, CGoods** ppOut, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return FALSE;
}

//##ModelId=41BFA467029F
CGoods* CVolumeLimitGoodsContainer::GetGoods(DWORD dwPosition)
{
	// TODO: Add your specialized code here.
	if( dwPosition >= m_vCells.size() )
	{
		return NULL;
	}

	if( m_vCells[dwPosition] == CGUID::GUID_INVALID )
	{
		return NULL;
	}

	return dynamic_cast<CGoods*>( Find(m_vCells[dwPosition]) );
}

//##ModelId=41BFA407002E
BOOL CVolumeLimitGoodsContainer::QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pGoods == NULL )
	{
		return FALSE;
	}

	//DWORD dwHashValue = CContainer::CalculateHashValue( pGoods -> GetType(), 
	//	pGoods -> GetID() );

	return QueryGoodsPosition( pGoods->GetExID(), dwPosition );
}

//##ModelId=41BFA36A0138
BOOL CVolumeLimitGoodsContainer::Add(CBaseObject* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods == NULL )
	{
		return FALSE;
	}

	DWORD dwFirstCell = 0;
	if( FindPositionForGoods(pGoods, dwFirstCell) == FALSE )
	{
		return FALSE;
	}

	return Add( dwFirstCell, pGoods, pVoid );
}

//##ModelId=41BFA3A00157
VOID CVolumeLimitGoodsContainer::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CAmountLimitGoodsContainer::Clear( pVoid );
	m_vCells.clear();
	m_vCells.resize( m_dwVolume, NULL_GUID );
}

//##ModelId=41BFA3A90196
VOID CVolumeLimitGoodsContainer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CAmountLimitGoodsContainer::Release();
	m_dwVolume=0;
	m_vCells.clear();
}

//##ModelId=41BFA55101E4
BOOL CVolumeLimitGoodsContainer::Clone(CGoodsContainer* pObject)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( CAmountLimitGoodsContainer::Clone(pObject) )
	{
		CVolumeLimitGoodsContainer* pContainer = 
			dynamic_cast<CVolumeLimitGoodsContainer*>( pObject );
		if( pContainer == NULL )
		{
			return FALSE;
		}

		pContainer->m_dwVolume=this->m_dwVolume;
		pContainer -> m_vCells		= this -> m_vCells;
		return TRUE;
	}
	return FALSE;
}

//##ModelId=41B6730200EA
VOID CVolumeLimitGoodsContainer::GetGoodsInSquare(DWORD dwFirstCell, DWORD dwWidth, DWORD dwHeight, vector<CGoods*> vGoods)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( dwFirstCell < GetGoodsAmountLimit() )
	{
		DWORD dwY = dwFirstCell / GetWidth();
		DWORD dwX = dwFirstCell % GetWidth();
		for( DWORD i = 0; i < dwHeight; i ++ )
		{
			if( i + dwY >= GetHeight() )
			{
				continue;
			}
			for( DWORD j = 0; j < dwWidth; j ++ )
			{
				if( j + dwX >= GetWidth() )
				{
					continue;
				}
				
				//DWORD dwHashValue = m_vCells[(dwY + i) * GetContainerWidth() + (dwX + j)];
				CGUID guid = m_vCells[(dwY + i) * GetWidth() + (dwX + j)];
				if( guid != CGUID::GUID_INVALID )
				{
					CGoods* pGoods = dynamic_cast<CGoods*>( Find(guid) );
					if( pGoods )
					{
						BOOL bAlreadyAdded	= FALSE;

						vector<CGoods*>::iterator it = vGoods.begin();
						for( ; it != vGoods.end(); it ++ )
						{
							if( *it == pGoods )
							{
								bAlreadyAdded = TRUE;
								break;
							}
						}
						if( bAlreadyAdded == FALSE )
						{
							vGoods.push_back( pGoods );
						}
					}
				}
			}
		}
	}
}

//##ModelId=41B673BE0261
CGoods* CVolumeLimitGoodsContainer::GetFirstGoodsInSquare(DWORD dwFirstCell, DWORD dwWidth, DWORD dwHeight)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	if( dwFirstCell < GetGoodsAmountLimit() )
	{
		DWORD dwY		= dwFirstCell / GetWidth();
		DWORD dwX		= dwFirstCell % GetWidth();
		BOOL  bFounded	= FALSE;
		for( DWORD i = 0; i < dwHeight; i ++ )
		{
			if( i + dwY >= GetHeight() )
			{
				continue;
			}
			if( bFounded )
			{
				break;
			}
			for( DWORD j = 0; j < dwWidth; j ++ )
			{
				if( j + dwX >= GetWidth() )
				{
					continue;
				}
				//DWORD dwHashValue = m_vCells[(dwY + i) * GetContainerWidth() + (dwX + j)];
				CGUID guid = m_vCells[(dwY + i) * GetWidth() + (dwX + j)];
				if( guid != CGUID::GUID_INVALID )
				{
					pResult = dynamic_cast<CGoods*>( Find(guid) );
					if( pResult )
					{
						bFounded = TRUE;
					}
				}
			}
		}
	}
	return pResult;
}

//##ModelId=41B6671E02BF
DWORD CVolumeLimitGoodsContainer::GetGoodsAmountInSquare(DWORD dwFirstCell, DWORD dwWidth, DWORD dwHeight)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	vector<CGUID> vGoods;
	if( dwFirstCell < GetGoodsAmountLimit() )
	{
		DWORD dwY = dwFirstCell / GetWidth();
		DWORD dwX = dwFirstCell % GetWidth();
		for( DWORD i = 0; i < dwHeight; i ++ )
		{
			if( i + dwY >= GetHeight() )
			{
				continue;
			}
			for( DWORD j = 0; j < dwWidth; j ++ )
			{
				if( j + dwX >= GetWidth() )
				{
					continue;
				}
				
				//DWORD dwHashValue = m_vCells[(dwY + i) * GetContainerWidth() + (dwX + j)];
				CGUID guid = m_vCells[(dwY + i) * GetWidth() + (dwX + j)];
				if( guid != CGUID::GUID_INVALID )
				{
					BOOL bAlreadyAdded	= FALSE;

					vector<CGUID>::iterator it = vGoods.begin();
					for( ; it != vGoods.end(); it ++ )
					{
						if( *it == guid )
						{
							bAlreadyAdded = TRUE;
							break;
						}
					}
					if( bAlreadyAdded == FALSE )
					{
						vGoods.push_back( guid );
					}
				}
			}
		}
	}
	return static_cast<DWORD>( vGoods.size() );
}

//##ModelId=41BFA3910290
CBaseObject* CVolumeLimitGoodsContainer::Remove(const CGUID& guid, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	CGoods* pGoods = dynamic_cast<CGoods*>( CAmountLimitGoodsContainer::Remove(guid, pVoid) );
	if( pGoods == NULL )
	{
		return NULL;
	}

	CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( 
		pGoods -> GetBasePropertiesIndex() );
	if( pProperties == NULL )
	{
		return FALSE;
	}

	DWORD dwFirstCell = 0;
	if( QueryGoodsPosition( guid, dwFirstCell ) == FALSE )
	{
		return NULL;
	}
	m_vCells[dwFirstCell]=CGUID::GUID_INVALID;	
	return pGoods;
}



//##ModelId=41BFA375002E
BOOL CVolumeLimitGoodsContainer::Add(DWORD dwPosition, CGoods* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( IsFull() == FALSE )
	{
		if( pObject )
		{
			CGoods* pGoods = GetGoods( dwPosition );
			if( pGoods )
			{
				bResult = CGoodsContainer::Add( dwPosition, pObject, pVoid );
			}
			else 
			{
				if( IsSpaceEnough(pObject, dwPosition) )
				{
					CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( 
						pObject -> GetBasePropertiesIndex() );
					if( pProperties )
					{
						//##暂时在这儿添加物品。
						//DWORD dwHashValue = CContainer::CalculateHashValue( pObject -> GetType(), pObject -> GetID() );
						CGUID guid = pObject->GetExID();
						m_mGoods[guid] = pObject;
						//##暂时屏蔽一下，否则QueryGoodsPosition会出错。
						//if( CAmountLimitGoodsContainer::Add(pObject, pVoid)  )	
						//{						
						m_vCells[dwPosition]=guid;
						for( size_t i = 0; i < m_vListeners.size(); i ++ )
						{
							if( m_vListeners[i] )
							{
								m_vListeners[i] -> OnObjectAdded( this, pObject, pVoid );
							}
						}
						bResult = TRUE;
						//}
					}
				}
			}
		}
	}
	return bResult;
}

BOOL CVolumeLimitGoodsContainer::AddFromDB( CGoods* pObject, DWORD dwPosition )
{
	// 就算可叠加的物品，在从数据库载入时，也会失败
	CGoods* pGoods = GetGoods( dwPosition );

	if( pGoods )
	{
		char str[256];
		char szGuid[40];
		char szExGuid[40];
		
		CGUID guid,guidExsit;
		guid = pObject->GetExID();
		guid.tostring(szGuid);
		guidExsit = pGoods->GetExID();
		guidExsit.tostring(szExGuid);

		sprintf(str, "背包:[%s](%s)在%d位置已有[%s](%s)。",pObject->GetName(),szGuid,
			dwPosition,	pGoods->GetName(), szExGuid );

		PutStringToFile("debug-DB", str);
		return FALSE;
	}

	BOOL bResult = FALSE;
	if( !IsFull() && pObject && IsSpaceEnough(pObject, dwPosition) )
	{
		CGoodsBaseProperties* pProperties = CGoodsFactory::QueryGoodsBaseProperties( 
			pObject -> GetBasePropertiesIndex() );
		if( pProperties )
		{
			CGUID guid = pObject->GetExID();
			m_mGoods[guid] = pObject;
			m_vCells[dwPosition]=guid;			
			bResult = TRUE;
		}
	}
	return bResult;
}

//##ModelId=41BFA5EA03B9
VOID CVolumeLimitGoodsContainer::AI()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CAmountLimitGoodsContainer::AI();
}

//##ModelId=41BFA3BB001F
BOOL CVolumeLimitGoodsContainer::IsFull()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = TRUE;
	if( CAmountLimitGoodsContainer::IsFull() == FALSE )
	{
		itCell it = m_vCells.begin();
		for( ; it < m_vCells.end(); it ++ )
		{
			if( *it == CGUID::GUID_INVALID )
			{
				bResult = FALSE;
				break;
			}
		}
	}
	return bResult;
}

//##ModelId=41BFA40F0109
BOOL CVolumeLimitGoodsContainer::QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	for( size_t i = 0; i < m_vCells.size(); i ++ )
	{
		if( m_vCells[i] == guid )
		{
			dwPosition = static_cast<DWORD>( i );
			return TRUE;
		}
	}
	return FALSE;
}

DWORD CVolumeLimitGoodsContainer::GetGoodsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwAmount = 0;
	goodsIt it = m_mGoods.begin();
	for( ; it != m_mGoods.end(); it ++ )
	{
		if( it -> second )
		{
			//##如果是一个有效的物品
			if( CGoodsFactory::QueryGoodsBaseProperties( 
				it -> second -> GetBasePropertiesIndex() ) )
			{
				DWORD dwPosition = 0;
				if( QueryGoodsPosition(it -> second, dwPosition) )
				{
					++ dwAmount;
				}
			}
		}
	}
	return dwAmount;
}

//##ModelId=41BFBAF50213
BOOL CVolumeLimitGoodsContainer::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid)
{
	return TRUE;
}

//##ModelId=41BFBAED0167
BOOL CVolumeLimitGoodsContainer::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid)
{
	return TRUE;
}

CBaseObject* CVolumeLimitGoodsContainer::Remove( CBaseObject* pObject, LPVOID pVoid )
{
	return CAmountLimitGoodsContainer::Remove( pObject, pVoid );
}

CBaseObject* CVolumeLimitGoodsContainer::Remove( LONG oType, const CGUID& guid , LPVOID pVoid )
{
	return CAmountLimitGoodsContainer::Remove( oType, guid, pVoid );
}

CBaseObject* CVolumeLimitGoodsContainer::Find( CBaseObject* pObj )
{
	return CAmountLimitGoodsContainer::Find( pObj );
}

CBaseObject* CVolumeLimitGoodsContainer::Find( LONG oType, const CGUID& guid  )
{
	return CAmountLimitGoodsContainer::Find( oType, guid );
}

CBaseObject* CVolumeLimitGoodsContainer::Find( const CGUID& guid )
{
	return CAmountLimitGoodsContainer::Find( guid );
}


BOOL CVolumeLimitGoodsContainer::Serialize( vector<BYTE>* pStream, BOOL b )
{
	BOOL bResult = FALSE;

	CSerializeContainer scSerializer;
	DWORD dwValidGoodsAmount = GetGoodsAmount();

	_AddToByteArray( pStream, dwValidGoodsAmount );

 	scSerializer.m_pStream = pStream;
	scSerializer.m_bStream = true;
	TraversingContainer( &scSerializer );

	if( dwValidGoodsAmount == scSerializer.m_dwValidGoodsAmount )
	{
		bResult = TRUE;
	}

	return bResult;
}

BOOL CVolumeLimitGoodsContainer::Unserialize( BYTE* pStream, LONG& lOffset, BOOL b )
{
	Clear();
	DWORD dwNumGoods = _GetDwordFromByteArray( pStream, lOffset );

	//CHAR szMsg[256];
	//sprintf( szMsg, "Total Goods: %d \n", dwNumGoods );
	//OutputDebugString( szMsg );

	for( DWORD i = 0; i < dwNumGoods; i ++ )
	{
		DWORD dwPosition = _GetDwordFromByteArray( pStream, lOffset );

		/*sprintf( szMsg, "Try to unserialize goods at position %d\n", dwPosition );
		OutputDebugString( szMsg );*/
		CGoods* pGoods = CGoodsFactory::UnserializeGoods( pStream, lOffset,44 );
		if( pGoods )
		{
			//sprintf( szMsg, "Unserialize goods %s[%d] succeed!\n", pGoods -> GetName(), pGoods -> GetID() );
			//OutputDebugString( szMsg );

			Add( dwPosition, pGoods );
		}
	}

	return TRUE;
}

BOOL CVolumeLimitGoodsContainer::Serialize( DBWriteSet& setWriteDB, BOOL b  )
{
	BOOL bResult = FALSE;

	CSerializeContainer scSerializer;
	DWORD dwValidGoodsAmount = GetGoodsAmount();
	setWriteDB.AddToByteArray( dwValidGoodsAmount );
 	scSerializer.m_pDBWriteData = &setWriteDB;
	scSerializer.m_bStream = false;
	TraversingContainer( &scSerializer );

	if( dwValidGoodsAmount == scSerializer.m_dwValidGoodsAmount )
	{
		bResult = TRUE;
	}

	return bResult;
}
BOOL CVolumeLimitGoodsContainer::Unserialize( DBReadSet& setReadDB, BOOL b )
{
	Clear();

	DWORD dwNumGoods = setReadDB.GetDwordFromByteArray( );
	for( DWORD i = 0; i < dwNumGoods; i ++ )
	{
		DWORD dwPosition = setReadDB.GetDwordFromByteArray(  );
		CGoods* pGoods = CGoodsFactory::UnserializeGoods( setReadDB,45 );
		if( pGoods )
		{
			Add( dwPosition, pGoods, NULL );
		}
	}

	return TRUE;
}

//##ModelId=41C0E6EC0196
CBaseObject* CVolumeLimitGoodsContainer::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	return CGoodsContainer::Remove( dwPosition, dwAmount, pVoid );
}

CVolumeLimitGoodsContainer::CSerializeContainer::CSerializeContainer()
{
	m_pStream = NULL;
	m_pDBWriteData = NULL;
}

CVolumeLimitGoodsContainer::CSerializeContainer::~CSerializeContainer()
{
	m_pStream = NULL;
	m_pDBWriteData = NULL;
}

BOOL CVolumeLimitGoodsContainer::CSerializeContainer::OnTraversingContainer( CContainer* pContainer, CBaseObject* pObject )
{
	BOOL bResult	= FALSE;
	CGoods* pGoods	= dynamic_cast<CGoods*>( pObject );
	
	CVolumeLimitGoodsContainer* pVolumeLimitGoodsContainer	= 
		dynamic_cast<CVolumeLimitGoodsContainer*>( pContainer );

	if( pVolumeLimitGoodsContainer && pGoods && ( m_pStream || m_pDBWriteData) )
	{
		if( CGoodsFactory::QueryGoodsBaseProperties( pGoods -> GetBasePropertiesIndex() ) )
		{
			DWORD dwPosition = 0;
			if( pVolumeLimitGoodsContainer -> QueryGoodsPosition(pGoods, dwPosition) )
			{
				if(m_bStream)
				{
					_AddToByteArray( m_pStream, dwPosition );
					pGoods -> Serialize( m_pStream );
				}
				else
				{
					m_pDBWriteData->AddToByteArray(dwPosition );
					pGoods -> Serialize( *m_pDBWriteData );
				}
				m_dwValidGoodsAmount ++;
				bResult = TRUE;
			}
		}
	}

	return bResult;
}