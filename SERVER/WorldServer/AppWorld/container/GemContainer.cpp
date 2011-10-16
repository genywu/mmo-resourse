// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "GemContainer.h"
#include "CContainer.h"
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"




//##ModelId=41C92C0300EA
GemContainer::GemContainer()
{
	// TODO: Add your specialized code here.
	m_pGoldCoins = NULL;
	AddListener( this );
}

////##ModelId=41C92C0700DA
//GemContainer::GemContainer(LONG lID) : CGoodsContainer( lID )
//{
//	// TODO: Add your specialized code here.
//	m_pGoldCoins = NULL;
//	AddListener( this );
//}

//##ModelId=41C92C0C0196
GemContainer::~GemContainer()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41C92C2301F4
BOOL GemContainer::Add(CBaseObject* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		bResult = GemContainer::Add( 0, pGoods, pVoid );
	}
	return bResult;
}

//##ModelId=41C92C390148
BOOL GemContainer::Add(DWORD dwPosition, CGoods* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( pObject )
	{
		if( m_pGoldCoins )
		{
			if( pObject -> GetBasePropertiesIndex() ==
				CGoodsFactory::GetGemIndex() )
			{
				bResult = CGoodsContainer::Add( dwPosition, pObject, pVoid );
			}
		}
		else
		{
			m_pGoldCoins = pObject;
			for( size_t i = 0; i < m_vListeners.size(); i ++ )
			{
				m_vListeners[i] -> OnObjectAdded( this, pObject, pVoid );
			}
			bResult = TRUE;
		}
	}
	return bResult;
}

BOOL GemContainer::AddFromDB( CGoods* pObject, DWORD dwPosition )
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	// 就算可叠加的物品，在从数据库载入时，也会失败
	CGoods* pGoods = GetGoods( dwPosition );
	if( pGoods ) //m_pGoldCoins存在了
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
		m_pGoldCoins = pObject;
		bResult = TRUE;
	}
	return bResult;
}

//##ModelId=41C92C7000BB
CBaseObject* GemContainer::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	return CGoodsContainer::Remove( dwPosition , dwAmount, pVoid );
}

//##ModelId=41C92CBB0186
VOID GemContainer::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer::Clear();
	CGoodsFactory::GarbageCollect( &m_pGoldCoins );
}

//##ModelId=41C92CC2008C
VOID GemContainer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer::Release();
	CGoodsFactory::GarbageCollect( &m_pGoldCoins );
}

//##ModelId=41C92CD70128
BOOL GemContainer::IsFull()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_pGoldCoins )
	{
		if( m_pGoldCoins -> GetAmount() >= m_pGoldCoins -> GetMaxStackNumber() )
		{
			bResult = TRUE;
		}
	}
	return bResult;
}

//##ModelId=41C92D030186
VOID GemContainer::TraversingContainer(CContainerListener* pListener)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pListener )
	{
		if( m_pGoldCoins )
		{
			pListener -> OnTraversingContainer( this, m_pGoldCoins );
		}
	}
}

//##ModelId=41C92D0A009C
DWORD GemContainer::GetContentsWeight()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return 0;
}

//##ModelId=41C92D340167
BOOL GemContainer::QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition)
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
BOOL GemContainer::QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_pGoldCoins )
	{
		if( m_pGoldCoins->GetExID() == guid )

		{
			dwPosition = 0;
			bResult = TRUE;
		}
	}
	return bResult;
}

//##ModelId=41C92DD80213
BOOL GemContainer::IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_pGoldCoins && dwGoodsBasePropertiesIndex == CGoodsFactory::GetGemIndex() )
	{
		bResult = TRUE;
	}
	return bResult;
}

//##ModelId=41C92DDF0119
CGoods* GemContainer::GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	if( dwGoodsBasePropertiesIndex == CGoodsFactory::GetGemIndex() )
	{
		pResult = m_pGoldCoins;
	}
	return pResult;
}

//##ModelId=41C92DF4030D
CGoods* GemContainer::GetGoods(DWORD dwPosition)
{
	// TODO: Add your specialized code here.
	if( dwPosition == 0 )
	{
		return m_pGoldCoins;
	}
	return NULL;
}

//##ModelId=41C92E0603C8
VOID GemContainer::GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( dwGoodsBasePropertiesIndex == CGoodsFactory::GetGemIndex() )
	{
		if( m_pGoldCoins )
		{
			vOut.push_back( m_pGoldCoins );
		}
	}
}

//##ModelId=41C92E1A0222
DWORD GemContainer::GetGoodsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( m_pGoldCoins )
	{
		return 1;
	}
	return 0;
}

//##ModelId=41C92C18006D
CBaseObject* GemContainer::Find(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	if( m_pGoldCoins )
	{
		if( m_pGoldCoins->GetExID() == guid )

		{
			pResult = m_pGoldCoins;
		}
	}
	return pResult;
}

//##ModelId=41C92C510177
CBaseObject* GemContainer::Remove(const CGUID& guid, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	if( m_pGoldCoins )
	{
		if( m_pGoldCoins->GetExID() == guid )
		{
			for( size_t i = 0; i < m_vListeners.size(); i ++ )
			{
				m_vListeners[i] -> OnObjectRemoved( this, m_pGoldCoins, pVoid );
			}
			pResult = m_pGoldCoins;
			m_pGoldCoins = NULL;
		}
	}
	return pResult;
}



//##ModelId=41CA26DA000F
BOOL GemContainer::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid)
{
	return TRUE;
}

//##ModelId=41CA26E101A5
BOOL GemContainer::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, LPVOID pVoid)
{
	return TRUE;
}

BOOL GemContainer::Serialize( vector<BYTE>* pStream, BOOL b )
{
	if( m_pGoldCoins )
	{
		_AddToByteArray( pStream, static_cast<BYTE>(1) );
		m_pGoldCoins -> Serialize( pStream, b );
	}
	else
	{
		_AddToByteArray( pStream, static_cast<BYTE>(0) );
	}
	return TRUE;
}

BOOL GemContainer::Unserialize( BYTE* pStream, LONG& lOffset, BOOL b )
{
	Release();
	BYTE bGoodsAmount = _GetByteFromByteArray( pStream, lOffset );
	if( bGoodsAmount > 0 )
	{
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(81);
#endif
		m_pGoldCoins = CGoodsFactory::UnserializeGoods( pStream, lOffset,48 );
	}
	return TRUE;
}


BOOL GemContainer::Serialize( DBWriteSet& setWriteDB, BOOL b  )
{
	if( m_pGoldCoins )
	{
		setWriteDB.AddToByteArray( static_cast<BYTE>(1) );
		m_pGoldCoins -> Serialize( setWriteDB, b );
	}
	else
	{
		setWriteDB.AddToByteArray( static_cast<BYTE>(0) );
	}
	return TRUE;
}

BOOL GemContainer::Unserialize( DBReadSet& setReadDB, BOOL b )
{	
	Release();
	BYTE bGoodsAmount = setReadDB.GetByteFromByteArray( );
	if( bGoodsAmount > 0 )
	{
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(81);
#endif
		m_pGoldCoins = CGoodsFactory::UnserializeGoods( setReadDB,49);
	}
	return TRUE;
}

//##ModelId=41CA6A2203C8
DWORD GemContainer::GetGoldCoinsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwResult = 0;
	if( m_pGoldCoins )
	{
		dwResult = m_pGoldCoins -> GetAmount();
	}
	return dwResult;
}

//##ModelId=41CA6A4203D8
BOOL GemContainer::AddGoldCoins(DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	//CS2CContainerObjectMove* pMsg = reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
	if( m_pGoldCoins && dwAmount )
	{
		if( m_pGoldCoins -> GetAmount() + dwAmount <= m_pGoldCoins -> GetMaxStackNumber() )
		{
			m_pGoldCoins -> SetAmount( m_pGoldCoins -> GetAmount() + dwAmount );
			bResult = TRUE;
		}
	}
	else if( m_pGoldCoins == NULL && dwAmount )
	{
		vector<CGoods*> vOut;
#ifdef __MEMORY_LEAK_CHECK__
		CBaseObject::SetConstructFlag(57);
#endif
		CGoodsFactory::CreateGoods( CGoodsFactory::GetGemIndex(), dwAmount, vOut,24 );
		for( size_t i = 0; i < vOut.size(); i ++ )
		{
			if( vOut[i] )
			{
				if( GemContainer::Add(vOut[i], pVoid) )
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
BOOL GemContainer::DecreaseGoldCoins(DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	//CS2CContainerObjectMove* pMsg = reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
	if( m_pGoldCoins && dwAmount )
	{
		if( m_pGoldCoins -> GetAmount() - dwAmount > 0 )
		{
			m_pGoldCoins -> SetAmount( m_pGoldCoins -> GetAmount() - dwAmount );
			bResult = TRUE;
		}
		else if(  m_pGoldCoins -> GetAmount() - dwAmount == 0 )
		{
			CGoods* pCoins = dynamic_cast<CGoods*>( Remove( m_pGoldCoins->GetExID(), pVoid ) );
			if( pCoins )
			{
				bResult = TRUE;
			}
		}
	}
	return bResult;
}


BOOL GemContainer::AddGoldCoinOfLargess(CGoods* pObject, DWORD dwPosition, LPVOID pVoid)
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
			m_pGoldCoins = pObject;
			for( size_t i = 0; i < m_vListeners.size(); i ++ )
			{
				m_vListeners[i] -> OnObjectAdded( this, pObject, pVoid );
			}
			bResult = TRUE;
		}
	}	
	return bResult;
}

CBaseObject* GemContainer::Find( LONG oType, const CGUID& guid )
{
	return CGoodsContainer::Find( oType, guid );
}