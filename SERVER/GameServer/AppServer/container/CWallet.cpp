// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "CWallet.h"
#include "CContainer.h"
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"
#include "..\Message Packaging\CS2CContainerObjectMove.h"



//##ModelId=41C92C0300EA
CWallet::CWallet()
{
	// TODO: Add your specialized code here.
	m_pGoldCoins = NULL;
	AddListener( this );
}

////##ModelId=41C92C0700DA
//CWallet::CWallet(LONG lID) : CGoodsContainer( lID )
//{
//	// TODO: Add your specialized code here.
//	m_pGoldCoins = NULL;
//	AddListener( this );
//}

//##ModelId=41C92C0C0196
CWallet::~CWallet()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41C92C2301F4
BOOL CWallet::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		bResult = CWallet::Add( 0, pGoods, pPreviousContainer, pVoid );
	}
	return bResult;
}

//##ModelId=41C92C390148
BOOL CWallet::Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( pObject )
	{
		if( m_pGoldCoins )
		{
			if( pObject -> GetBasePropertiesIndex() ==
				CGoodsFactory::GetGoldCoinIndex() )
			{
				bResult = CGoodsContainer::Add( dwPosition, pObject, pPreviousContainer, pVoid);
			}
		}
		else
		{
			m_pGoldCoins = pObject;
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
CBaseObject* CWallet::Remove(DWORD dwPosition, DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	return CGoodsContainer::Remove( dwPosition , dwAmount, pVoid );
}

//##ModelId=41C92CBB0186
VOID CWallet::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer::Clear();
#ifdef _GOODSLOG1_
	if(m_pGoldCoins)
	{
		char pszGoodsLog[1024]="";			
		char srcGoodsGUID[50]="";
		m_pGoldCoins->GetExID().tostring(srcGoodsGUID);					
		_snprintf(pszGoodsLog,1024,"[物品回收:CWallet::Clear][物品GUID:%s][物品数量:%d]",
			srcGoodsGUID,m_pGoldCoins->GetAmount());
		CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);	
	}							
#endif	
	CGoodsFactory::GarbageCollect( &m_pGoldCoins,13 );
}

//##ModelId=41C92CC2008C
VOID CWallet::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer::Release();
#ifdef _GOODSLOG1_
	if(m_pGoldCoins)
	{
		char pszGoodsLog[1024]="";			
		char srcGoodsGUID[50]="";
		m_pGoldCoins->GetExID().tostring(srcGoodsGUID);					
		_snprintf(pszGoodsLog,1024,"[物品回收:CWallet::Release][物品GUID:%s][物品数量:%d]",
			srcGoodsGUID,m_pGoldCoins->GetAmount());
		CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);	
	}							
#endif	
	CGoodsFactory::GarbageCollect( &m_pGoldCoins ,14);
}

//##ModelId=41C92CD70128
BOOL CWallet::IsFull()
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

//! 得到最大容量
long CWallet::GetMaxCapacity(void)
{
	if( m_pGoldCoins )
	{
		return m_pGoldCoins -> GetMaxStackNumber();
	}
	return 0;
}

//##ModelId=41C92D030186
VOID CWallet::TraversingContainer(CContainerListener* pListener)
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
DWORD CWallet::GetContentsWeight()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return 0;
}

//##ModelId=41C92D340167
BOOL CWallet::QueryGoodsPosition(CGoods* pGoods, DWORD& dwPosition)
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
BOOL CWallet::QueryGoodsPosition(const CGUID& guid, DWORD& dwPosition)
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
BOOL CWallet::IsGoodsExisted(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_pGoldCoins && dwGoodsBasePropertiesIndex == CGoodsFactory::GetGoldCoinIndex() )
	{
		bResult = TRUE;
	}
	return bResult;
}

//##ModelId=41C92DDF0119
CGoods* CWallet::GetTheFirstGoods(DWORD dwGoodsBasePropertiesIndex)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	if( dwGoodsBasePropertiesIndex == CGoodsFactory::GetGoldCoinIndex() )
	{
		pResult = m_pGoldCoins;
	}
	return pResult;
}

//##ModelId=41C92DF4030D
CGoods* CWallet::GetGoods(DWORD dwPosition)
{
	// TODO: Add your specialized code here.
	if( dwPosition == 0 )
	{
		return m_pGoldCoins;
	}
	return NULL;
}

//##ModelId=41C92E0603C8
VOID CWallet::GetGoods(DWORD dwGoodsBasePropertiesIndex, vector<CGoods*> vOut)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( dwGoodsBasePropertiesIndex == CGoodsFactory::GetGoldCoinIndex() )
	{
		if( m_pGoldCoins )
		{
			vOut.push_back( m_pGoldCoins );
		}
	}
}

//##ModelId=41C92E1A0222
DWORD CWallet::GetGoodsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( m_pGoldCoins )
	{
		return 1;
	}
	return 0;
}

DWORD CWallet::GetGoodsAmountByName(const char* strName)
{
	DWORD dwAmount=0;
	
	return dwAmount;
}

DWORD CWallet::GetGoodsAmountByType(int nType)
{
	DWORD dwAmount=0;
	
	return dwAmount;
}

//##ModelId=41C92C18006D
CBaseObject* CWallet::Find(const CGUID& guid)
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
CBaseObject* CWallet::Remove(const CGUID& guid, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	if( m_pGoldCoins )
	{
		if( m_pGoldCoins->GetExID() == guid )
		{
			for( size_t i = 0; i < m_vListeners.size(); i ++ )
			{
				if( m_vListeners[i] )
				{
					m_vListeners[i] -> OnObjectRemoved( this, m_pGoldCoins, m_pGoldCoins -> GetAmount(), pVoid );
				}
			}
			pResult = m_pGoldCoins;
			m_pGoldCoins = NULL;
		}
	}
	return pResult;
}



//##ModelId=41CA26DA000F
BOOL CWallet::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
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
BOOL CWallet::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
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


BOOL CWallet::Serialize( DBWriteSet& setWriteDB, BOOL b  )
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
BOOL CWallet::Unserialize( DBReadSet& setReadDB, BOOL b )
{	
	CWallet::Clear();
	BYTE bGoodsAmount = setReadDB.GetByteFromByteArray( );
	if( bGoodsAmount > 0 )
	{
		m_pGoldCoins = CGoodsFactory::UnserializeGoods( setReadDB,20);
	}
	return TRUE;
}

//##ModelId=41CA6A2203C8
DWORD CWallet::GetGoldCoinsAmount()
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
BOOL CWallet::AddGoldCoins(DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	CS2CContainerObjectMove* pMsg = reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
	if( m_pGoldCoins && dwAmount )
	{
		if( m_pGoldCoins -> GetAmount() + dwAmount <= m_pGoldCoins -> GetMaxStackNumber() )
		{
			m_pGoldCoins -> SetAmount( m_pGoldCoins -> GetAmount() + dwAmount );
			if( pMsg )
			{
				pMsg -> SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
				pMsg -> SetDestinationContainer( GetOwnerType(), GetOwnerID(), 0 );
				pMsg -> SetDestinationObject( m_pGoldCoins -> GetType(), m_pGoldCoins -> GetExID() );
				pMsg -> SetDestinationObjectAmount( m_pGoldCoins -> GetAmount() );
			}
			bResult = TRUE;
		}
		else
		{
			m_pGoldCoins -> SetAmount( m_pGoldCoins -> GetMaxStackNumber() );
			if( pMsg )
			{
				pMsg -> SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
				pMsg -> SetDestinationContainer( GetOwnerType(), GetOwnerID(), 0 );
				pMsg -> SetDestinationObject( m_pGoldCoins -> GetType(), m_pGoldCoins -> GetExID() );
				pMsg -> SetDestinationObjectAmount( m_pGoldCoins -> GetAmount() );
			}
			bResult = TRUE;
		}
	}
	else if( m_pGoldCoins == NULL && dwAmount )
	{
		vector<CGoods*> vOut;
		CGoodsFactory::CreateGoods( CGoodsFactory::GetGoldCoinIndex(), dwAmount, vOut ,21);
		for( size_t i = 0; i < vOut.size(); i ++ )
		{
			if( vOut[i] )
			{
				if( CWallet::Add(vOut[i], NULL, pVoid) )
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
				m_pGoldCoins -> SerializeForOldClient( &( pMsg -> GetObjectStream() ) );
			}
		}
	}
	return bResult;
}

//##ModelId=41CA6A6F003E
BOOL CWallet::DecreaseGoldCoins(DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	CS2CContainerObjectMove* pMsg = reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
	if( m_pGoldCoins && dwAmount )
	{
		if( m_pGoldCoins -> GetAmount() - dwAmount > 0 )
		{
			m_pGoldCoins -> SetAmount( m_pGoldCoins -> GetAmount() - dwAmount );
			if( pMsg )
			{
				pMsg -> SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
				pMsg -> SetSourceContainer( GetOwnerType(), GetOwnerID(), 0 );
				pMsg -> SetSourceObject( m_pGoldCoins -> GetType(), m_pGoldCoins -> GetExID() );
				pMsg -> SetSourceObjectAmount( dwAmount );
			}
			bResult = TRUE;
		}
		else if(  m_pGoldCoins -> GetAmount() - dwAmount == 0 )
		{
			CGoods* pCoins = dynamic_cast<CGoods*>( Remove( m_pGoldCoins->GetExID(), pVoid ) );
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

