// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "CSilverWallet.h"
#include "CContainer.h"
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"





//##ModelId=41C92C0300EA
CSilverWallet::CSilverWallet()
{
	// TODO: Add your specialized code here.
	m_pSilverCoins = NULL;
	AddListener( this );
}

////##ModelId=41C92C0700DA
//CSilverWallet::CSilverWallet(LONG lID) : CGoodsContainer( lID )
//{
//	// TODO: Add your specialized code here.
//	m_pSilverCoins = NULL;
//	AddListener( this );
//}

//##ModelId=41C92C0C0196
CSilverWallet::~CSilverWallet()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41C92C2301F4
BOOL CSilverWallet::Add(CBaseObject* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		bResult = CSilverWallet::Add( 0, pGoods, pVoid );
	}
	return bResult;
}

//##ModelId=41C92C390148
BOOL CSilverWallet::Add(DWORD dwPosition, CGoods* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( pObject )
	{
		if( m_pSilverCoins )
		{
			if( pObject -> GetBasePropertiesIndex() ==
				CGoodsFactory::GetSilverIndex() )
			{
				bResult = CGoodsContainer::Add( dwPosition, pObject, pVoid );
			}
		}
		else
		{
			m_pSilverCoins = pObject;
			for( size_t i = 0; i < m_vListeners.size(); i ++ )
			{
				m_vListeners[i] -> OnObjectAdded( this, pObject, pVoid );
			}
			bResult = TRUE;
		}
	}
	return bResult;
}

BOOL CSilverWallet::AddFromDB( CGoods* pObject, DWORD dwPosition )
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	// 就算可叠加的物品，在从数据库载入时，也会失败
	CGoods* pGoods = GetGoods( dwPosition );
	if( pGoods ) //m_pSilverCoins存在了
	{
		char str[256];
		char szGuid[40];
		char szExGuid[40];

		CGUID guid,guidExsit;
		guid = pObject->GetExID();
		guid.tostring(szGuid);
		guidExsit = pGoods->GetExID();
		guidExsit.tostring(szExGuid);

		sprintf(str, "钱包:[%s](%s)在%d位置已有[%s](%s)。",pObject->GetName(),szGuid,
			dwPosition,	pGoods->GetName(), szExGuid );
		PutStringToFile("debug-DB", str);
		return FALSE;
	}

	BOOL bResult = FALSE;
	if( pObject )
	{
		m_pSilverCoins = pObject;
		bResult = TRUE;
	}
	return bResult;
}

//##ModelId=41C92C7000BB
CBaseObject* CSilverWallet::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	return CGoodsContainer::Remove( dwPosition , dwAmount, pVoid );
}

//##ModelId=41C92CBB0186
VOID CSilverWallet::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer::Clear();
	CGoodsFactory::GarbageCollect( &m_pSilverCoins );
}

//##ModelId=41C92CC2008C
VOID CSilverWallet::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer::Release();
	CGoodsFactory::GarbageCollect( &m_pSilverCoins );
}

//##ModelId=41C92CD70128
BOOL CSilverWallet::IsFull()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_pSilverCoins )
	{
		if( m_pSilverCoins -> GetAmount() >= m_pSilverCoins -> GetMaxStackNumber() )
		{
			bResult = TRUE;
		}
	}
	return bResult;
}

//##ModelId=41C92D030186
VOID CSilverWallet::TraversingContainer(CContainerListener* pListener)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pListener )
	{
		if( m_pSilverCoins )
		{
			pListener -> OnTraversingContainer( this, m_pSilverCoins );
		}
	}
}

//##ModelId=41C92D0A009C
DWORD CSilverWallet::GetContentsWeight()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return 0;
}

//##ModelId=41C92D340167
BOOL CSilverWallet::QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( pGoods )
	{
		bResult = QueryGoodsPosition( pGoods->GetExID(), dwPosition );
	}
	return bResult;
}

//##ModelId=41C92DBA0157
BOOL CSilverWallet::QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_pSilverCoins )
	{
		if( m_pSilverCoins->GetExID() == guid )

		{
			dwPosition = 0;
			bResult = TRUE;
		}
	}
	return bResult;
}

//##ModelId=41C92DD80213
BOOL CSilverWallet::IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_pSilverCoins && dwGoodsBasePropertiesIndex == CGoodsFactory::GetSilverIndex() )
	{
		bResult = TRUE;
	}
	return bResult;
}

//##ModelId=41C92DDF0119
CGoods* CSilverWallet::GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	if( dwGoodsBasePropertiesIndex == CGoodsFactory::GetSilverIndex() )
	{
		pResult = m_pSilverCoins;
	}
	return pResult;
}

//##ModelId=41C92DF4030D
CGoods* CSilverWallet::GetGoods(DWORD dwPosition)
{
	// TODO: Add your specialized code here.
	if( dwPosition == 0 )
	{
		return m_pSilverCoins;
	}
	return NULL;
}

//##ModelId=41C92E0603C8
VOID CSilverWallet::GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( dwGoodsBasePropertiesIndex == CGoodsFactory::GetSilverIndex() )
	{
		if( m_pSilverCoins )
		{
			vOut.push_back( m_pSilverCoins );
		}
	}
}

//##ModelId=41C92E1A0222
DWORD CSilverWallet::GetGoodsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( m_pSilverCoins )
	{
		return 1;
	}
	return 0;
}

//##ModelId=41C92C18006D
CBaseObject* CSilverWallet::Find(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	if( m_pSilverCoins )
	{
		if( m_pSilverCoins->GetExID() == guid )

		{
			pResult = m_pSilverCoins;
		}
	}
	return pResult;
}

//##ModelId=41C92C510177
CBaseObject* CSilverWallet::Remove(const CGUID& guid, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	if( m_pSilverCoins )
	{
		if( m_pSilverCoins->GetExID() == guid )
		{
			for( size_t i = 0; i < m_vListeners.size(); i ++ )
			{
				m_vListeners[i] -> OnObjectRemoved( this, m_pSilverCoins, pVoid );
			}
			pResult = m_pSilverCoins;
			m_pSilverCoins = NULL;
		}
	}
	return pResult;
}



//##ModelId=41CA26DA000F
BOOL CSilverWallet::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid)
{
	return TRUE;
}

//##ModelId=41CA26E101A5
BOOL CSilverWallet::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid)
{
	return TRUE;
}

BOOL CSilverWallet::Serialize( vector<BYTE>* pStream, BOOL b )
{
	if( m_pSilverCoins )
	{
		_AddToByteArray( pStream, static_cast<BYTE>(1) );
		m_pSilverCoins -> Serialize( pStream, b );
	}
	else
	{
		_AddToByteArray( pStream, static_cast<BYTE>(0) );
	}
	return TRUE;
}

BOOL CSilverWallet::Unserialize( BYTE* pStream, LONG& lOffset, BOOL b )
{
	Release();
	BYTE bGoodsAmount = _GetByteFromByteArray( pStream, lOffset );
	if( bGoodsAmount > 0 )
	{
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(76);
#endif
		m_pSilverCoins = CGoodsFactory::UnserializeGoods( pStream, lOffset,40 );
	}
	return TRUE;
}


BOOL CSilverWallet::Serialize( DBWriteSet& setWriteDB, BOOL b  )
{
	if( m_pSilverCoins )
	{
		setWriteDB.AddToByteArray( static_cast<BYTE>(1) );
		m_pSilverCoins -> Serialize( setWriteDB ,b );
	}
	else
	{
		setWriteDB.AddToByteArray( static_cast<BYTE>(0) );
	}
	return TRUE;
}
BOOL CSilverWallet::Unserialize( DBReadSet& setReadDB, BOOL b )
{
	Release();
	BYTE bGoodsAmount = setReadDB.GetByteFromByteArray( );
	if( bGoodsAmount > 0 )
	{
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(76);
#endif
		m_pSilverCoins = CGoodsFactory::UnserializeGoods( setReadDB,41 );
	}
	return TRUE;
}

//##ModelId=41CA6A2203C8
DWORD CSilverWallet::GetSilverCoinsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwResult = 0;
	if( m_pSilverCoins )
	{
		dwResult = m_pSilverCoins -> GetAmount();
	}
	return dwResult;
}

//##ModelId=41CA6A4203D8
BOOL CSilverWallet::AddSilverCoins(DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	//CS2CContainerObjectMove* pMsg = reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
	if( m_pSilverCoins && dwAmount )
	{
		if( m_pSilverCoins -> GetAmount() + dwAmount <= m_pSilverCoins -> GetMaxStackNumber() )
		{
			m_pSilverCoins -> SetAmount( m_pSilverCoins -> GetAmount() + dwAmount );
			bResult = TRUE;
		}
	}
	else if( m_pSilverCoins == NULL && dwAmount )
	{
		vector<CGoods*> vOut;
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(54);
#endif
		CGoodsFactory::CreateGoods( CGoodsFactory::GetSilverIndex(), dwAmount, vOut,22 );
		for( size_t i = 0; i < vOut.size(); i ++ )
		{
			if( vOut[i] )
			{
				if( CSilverWallet::Add(vOut[i], pVoid) )
				{
					bResult = TRUE;
					break;
				}
			}
		}
	}
	return bResult;
}

//##ModelId=41CA6A6F003E
BOOL CSilverWallet::DecreaseSilverCoins(DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	//CS2CContainerObjectMove* pMsg = reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
	if( m_pSilverCoins && dwAmount )
	{
		if( m_pSilverCoins -> GetAmount() - dwAmount > 0 )
		{
			m_pSilverCoins -> SetAmount( m_pSilverCoins -> GetAmount() - dwAmount );
			bResult = TRUE;
		}
		else if(  m_pSilverCoins -> GetAmount() - dwAmount == 0 )
		{
			CGoods* pCoins = dynamic_cast<CGoods*>( Remove( m_pSilverCoins->GetExID(), pVoid ) );
			if( pCoins )
			{
				bResult = TRUE;
			}
		}
	}
	return bResult;
}


BOOL CSilverWallet::AddSilverCoinOfLargess(CGoods* pObject, DWORD dwPosition, LPVOID pVoid)
{
	BOOL bResult = FALSE;
	if( pObject )
	{
		CGoods* pGoods = GetGoods( dwPosition );
		if( pGoods )
		{
			//##如果要加入的物品，类型相同且物品是可堆叠的。
			if( pGoods -> GetBasePropertiesIndex() ==
				pObject -> GetBasePropertiesIndex())
			{
				DWORD dwRemainedSlot = CGlobeSetup::GetSetup()->dwGoldCoinLimit -
					pGoods -> GetAmount();
				//##如果加入的数量少于等于这个，则直接加入。
				if( pObject -> GetAmount() <= dwRemainedSlot )
				{
					pGoods -> SetAmount( pGoods -> GetAmount() + pObject -> GetAmount() );
					CGoodsFactory::GarbageCollect( &pObject );

					//##重置目标物，避免错误。
					//CS2CContainerObjectMove* pMsg = reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
					//if( pMsg )
					//{
					//	pMsg -> SetObject( 0, 0 );
					//	pMsg -> SetObjectAmount( 0 );
					//	pMsg -> GetObjectStream().clear();
					//}

					for( size_t i = 0; i < m_vListeners.size(); i ++ )
					{
						if( m_vListeners[i] )
						{
							//##该合并消息必须设置目标物品
							m_vListeners[i] -> OnObjectAdded( this, pGoods, pVoid );
						}
					}

					bResult = TRUE;
				}
				//##否则什么都不作
				else
				{
				}
			}
		}
		else
		{
			m_pSilverCoins = pObject;
			for( size_t i = 0; i < m_vListeners.size(); i ++ )
			{
				m_vListeners[i] -> OnObjectAdded( this, pObject, pVoid );
			}
			bResult = TRUE;
		}
	}	
	return bResult;
}

CBaseObject* CSilverWallet::Find( LONG oType, const CGUID& guid )
{
	return CGoodsContainer::Find( oType, guid );
}