// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "StdAfx.h"
#include "CSilverWallet.h"
#include "CContainer.h"
#include "../Goods/CGoods.h"
#include "../Goods/CGoodsFactory.h"
#include "../MessagePackage/CS2CContainerObjectMove.h"

#include "../../../../Public/Setup/GoodsSetup.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//##ModelId=41C92C0300EA
CSilverWallet::CSilverWallet()
{
	// TODO: Add your specialized code here.
	m_pSilverCoins = NULL;
	AddListener( this );
}



//##ModelId=41C92C0C0196
CSilverWallet::~CSilverWallet()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41C92C2301F4
BOOL CSilverWallet::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		bResult = CSilverWallet::Add( 0, pGoods, pPreviousContainer, pVoid );
	}
	return bResult;
}

//##ModelId=41C92C390148
BOOL CSilverWallet::Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( pObject )
	{
		if( m_pSilverCoins )
		{
			if( pObject -> GetBasePropertiesIndex() ==
				GoodsSetup::GetSilverIndex() )
			{
				bResult = CGoodsContainer::Add( dwPosition, pObject, pPreviousContainer, pVoid);
			}
		}
		else
		{
			m_pSilverCoins = pObject;
			for( size_t i = 0; i < m_vListeners.size(); i ++ )
			{
				if( m_vListeners[i] )
				{
					m_vListeners[i] -> OnObjectAdded( this, pObject, pObject -> GetAmount(), pVoid );
				}
			}
			bResult = TRUE;
		}
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
void CSilverWallet::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer::Clear();
#ifdef _GOODSLOG1_
	if(m_pSilverCoins)
	{
		char pszGoodsLog[1024]="";			
		char srcGoodsGUID[50]="";
		m_pSilverCoins->GetExID().tostring(srcGoodsGUID);					
		_snprintf(pszGoodsLog,1024,"[物品回收:CSilverWallet::Clear][物品GUID:%s][物品数量:%d]",
			srcGoodsGUID,m_pSilverCoins->GetAmount());
		CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);	
	}							
#endif	
	CGoodsFactory::GarbageCollect( &m_pSilverCoins ,6);
}

//##ModelId=41C92CC2008C
void CSilverWallet::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer::Release();
#ifdef _GOODSLOG1_
	if(m_pSilverCoins)
	{
		char pszGoodsLog[1024]="";			
		char srcGoodsGUID[50]="";
		m_pSilverCoins->GetExID().tostring(srcGoodsGUID);					
		_snprintf(pszGoodsLog,1024,"[物品回收:CSilverWallet析构][物品GUID:%s][物品数量:%d]",
			srcGoodsGUID,m_pSilverCoins->GetAmount());
		CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);	
	}							
#endif
	CGoodsFactory::GarbageCollect( &m_pSilverCoins,7 );
}

//##ModelId=41C92CD70128
BOOL CSilverWallet::IsFull()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_pSilverCoins )
	{
		if( m_pSilverCoins -> GetAmount() >=  m_pSilverCoins-> GetMaxStackNumber() )
		{
			bResult = TRUE;
		}
	}
	return bResult;
}

//! 得到最大容量
long CSilverWallet::GetMaxCapacity(void)
{
	if( m_pSilverCoins )
	{
		return m_pSilverCoins -> GetMaxStackNumber();
	}
	return 0;
}

//##ModelId=41C92D030186
void CSilverWallet::TraversingContainer(CContainerListener* pListener)
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
	if( m_pSilverCoins && dwGoodsBasePropertiesIndex == GoodsSetup::GetSilverIndex() )
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
	if( dwGoodsBasePropertiesIndex == GoodsSetup::GetSilverIndex() )
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
void CSilverWallet::GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( dwGoodsBasePropertiesIndex == GoodsSetup::GetSilverIndex() )
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

DWORD CSilverWallet::GetGoodsAmountByName(const char* strName)
{
	DWORD dwAmount=0;
	
	return dwAmount;
}

DWORD CSilverWallet::GetGoodsAmountByType(int nType)
{
	DWORD dwAmount=0;
	
	return dwAmount;
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
				if( m_vListeners[i] )
				{
					m_vListeners[i] -> OnObjectRemoved( this, m_pSilverCoins, m_pSilverCoins -> GetAmount(), pVoid );
				}
			}
			pResult = m_pSilverCoins;
			m_pSilverCoins = NULL;
		}
	}
	return pResult;
}



//##ModelId=41CA26DA000F
BOOL CSilverWallet::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
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
			pMsg -> SetDestinationObject( pGoods -> GetType(), pGoods -> GetExID() );
			pMsg -> SetDestinationObjectAmount( pGoods -> GetAmount() );
		}
	}
	return TRUE;
}

//##ModelId=41CA26E101A5
BOOL CSilverWallet::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
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
			pMsg -> SetSourceObject( pGoods -> GetType(), pGoods -> GetExID() );
			pMsg -> SetSourceObjectAmount( dwAmount );
		}
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
	CSilverWallet::Clear();
	BYTE bGoodsAmount = setReadDB.GetByteFromByteArray( );
	if( bGoodsAmount > 0 )
	{
		m_pSilverCoins = CGoodsFactory::UnserializeGoods( setReadDB,15 );
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
	CS2CContainerObjectMove* pMsg = reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
	if( m_pSilverCoins && dwAmount )
	{
		if( m_pSilverCoins -> GetAmount() + dwAmount <= m_pSilverCoins -> GetMaxStackNumber() )
		{
			m_pSilverCoins -> SetAmount( m_pSilverCoins -> GetAmount() + dwAmount );
			if( pMsg )
			{
				pMsg -> SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
				pMsg -> SetDestinationContainer( GetOwnerType(), GetOwnerID(), 0 );
				pMsg -> SetDestinationObject( m_pSilverCoins -> GetType(), m_pSilverCoins -> GetExID() );
				pMsg -> SetDestinationObjectAmount( m_pSilverCoins -> GetAmount() );
			}
			bResult = TRUE;
		}
		else
		{
			m_pSilverCoins -> SetAmount( m_pSilverCoins -> GetMaxStackNumber() );
			if( pMsg )
			{
				pMsg -> SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
				pMsg -> SetDestinationContainer( GetOwnerType(), GetOwnerID(), 0 );
				pMsg -> SetDestinationObject( m_pSilverCoins -> GetType(), m_pSilverCoins -> GetExID() );
				pMsg -> SetDestinationObjectAmount( m_pSilverCoins -> GetAmount() );
			}
			bResult = TRUE;
		}
	}
	else if( m_pSilverCoins == NULL && dwAmount )
	{
		vector<CGoods*> vOut;
		CGoodsFactory::CreateGoods( GoodsSetup::GetSilverIndex(), dwAmount, vOut ,16);
		for( size_t i = 0; i < vOut.size(); i ++ )
		{
			if( vOut[i] )
			{
				if( CSilverWallet::Add(vOut[i], NULL, pVoid) )
				{
					bResult = TRUE;
					break;
				}
			}
		}
		if( bResult )
		{
			if( pMsg )
			{
				pMsg -> SetOperation( CS2CContainerObjectMove::OT_NEW_OBJECT );
				m_pSilverCoins -> SerializeForOldClient( &( pMsg -> GetObjectStream() ) );
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
	CS2CContainerObjectMove* pMsg = reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
	if( m_pSilverCoins && dwAmount )
	{
		if( m_pSilverCoins -> GetAmount() - dwAmount > 0 )
		{
			m_pSilverCoins -> SetAmount( m_pSilverCoins -> GetAmount() - dwAmount );
			if( pMsg )
			{
				pMsg -> SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
				pMsg -> SetSourceContainer( GetOwnerType(), GetOwnerID(), 0 );
				pMsg -> SetSourceObject( m_pSilverCoins -> GetType(), m_pSilverCoins -> GetExID() );
				pMsg -> SetSourceObjectAmount( dwAmount );
			}
			bResult = TRUE;
		}
		else if(  m_pSilverCoins -> GetAmount() - dwAmount == 0 )
		{
			CGoods* pCoins = dynamic_cast<CGoods*>( Remove( m_pSilverCoins->GetExID(), pVoid ) );
			if( pCoins )
			{
				if( pMsg )
				{
					pMsg -> SetOperation( CS2CContainerObjectMove::OT_DELETE_OBJECT );
				}
				bResult = TRUE;
			}
		}
	}
	return bResult;
}

