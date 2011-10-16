// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "GemContainer.h"
#include "CContainer.h"
#include "..\Goods\CGoods.h"
#include "..\Goods\CGoodsFactory.h"
#include "..\Message Packaging\CS2CContainerObjectMove.h"




//##ModelId=41C92C0300EA
GemContainer::GemContainer()
{
	// TODO: Add your specialized code here.
	m_pGemCoins = NULL;
	AddListener( this );
}

////##ModelId=41C92C0700DA
//GemContainer::GemContainer(LONG lID) : CGoodsContainer( lID )
//{
//	// TODO: Add your specialized code here.
//	m_pGemCoins = NULL;
//	AddListener( this );
//}

//##ModelId=41C92C0C0196
GemContainer::~GemContainer()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41C92C2301F4
BOOL GemContainer::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	CGoods* pGoods = dynamic_cast<CGoods*>( pObject );
	if( pGoods )
	{
		bResult = GemContainer::Add( 0, pGoods, pPreviousContainer, pVoid );
	}
	return bResult;
}

//##ModelId=41C92C390148
BOOL GemContainer::Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( pObject )
	{
		if( m_pGemCoins )
		{
			if( pObject -> GetBasePropertiesIndex() ==
				CGoodsFactory::GetGemIndex() )
			{
				bResult = CGoodsContainer::Add( dwPosition, pObject, pPreviousContainer, pVoid );
			}
		}
		else
		{
			m_pGemCoins = pObject;
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
#ifdef _GOODSLOG1_
	if(m_pGemCoins)
	{
		char pszGoodsLog[1024]="";			
		char srcGoodsGUID[50]="";
		m_pGemCoins->GetExID().tostring(srcGoodsGUID);					
		_snprintf(pszGoodsLog,1024,"[物品回收:GemContainer::Clear][物品GUID:%s][物品数量:%d]",
			srcGoodsGUID,m_pGemCoins->GetAmount());
		CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);	
	}							
#endif	
	CGoodsFactory::GarbageCollect( &m_pGemCoins ,17);
}

//##ModelId=41C92CC2008C
VOID GemContainer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	CGoodsContainer::Release();
#ifdef _GOODSLOG1_
	if(m_pGemCoins)
	{
		char pszGoodsLog[1024]="";			
		char srcGoodsGUID[50]="";
		m_pGemCoins->GetExID().tostring(srcGoodsGUID);					
		_snprintf(pszGoodsLog,1024,"[物品回收:GemContainer::Release][物品GUID:%s][物品数量:%d]",
			srcGoodsGUID,m_pGemCoins->GetAmount());
		CMessage::AsyWriteFile(GetGame()->GetGoodsLogFile(),pszGoodsLog);	
	}							
#endif	
	CGoodsFactory::GarbageCollect( &m_pGemCoins ,18);
}

//##ModelId=41C92CD70128
BOOL GemContainer::IsFull()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_pGemCoins )
	{
		if( m_pGemCoins -> GetAmount() >= m_pGemCoins -> GetMaxStackNumber() )
		{
			bResult = TRUE;
		}
	}
	return bResult;
}

//! 得到最大容量
long GemContainer::GetMaxCapacity(void)
{
	if( m_pGemCoins )
	{
		return m_pGemCoins -> GetMaxStackNumber();
	}
	return 0;
}

//##ModelId=41C92D030186
VOID GemContainer::TraversingContainer(CContainerListener* pListener)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( pListener )
	{
		if( m_pGemCoins )
		{
			pListener -> OnTraversingContainer( this, m_pGemCoins );
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
	if( m_pGemCoins )
	{
		if( m_pGemCoins->GetExID() == guid )
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
	if( m_pGemCoins && dwGoodsBasePropertiesIndex == CGoodsFactory::GetGemIndex() )
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
		pResult = m_pGemCoins;
	}
	return pResult;
}

//##ModelId=41C92DF4030D
CGoods* GemContainer::GetGoods(DWORD dwPosition)
{
	// TODO: Add your specialized code here.
	if( dwPosition == 0 )
	{
		return m_pGemCoins;
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
		if( m_pGemCoins )
		{
			vOut.push_back( m_pGemCoins );
		}
	}
}

//##ModelId=41C92E1A0222
DWORD GemContainer::GetGoodsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	if( m_pGemCoins )
	{
		return 1;
	}
	return 0;
}

DWORD GemContainer::GetGoodsAmountByName(const char* strName)
{
	DWORD dwAmount=0;

	return dwAmount;
}

DWORD GemContainer::GetGoodsAmountByType(int nType)
{
	DWORD dwAmount=0;

	return dwAmount;
}

//##ModelId=41C92C18006D
CBaseObject* GemContainer::Find(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	if( m_pGemCoins )
	{
		if( m_pGemCoins->GetExID() == guid )
		{
			pResult = m_pGemCoins;
		}
	}
	return pResult;
}

//##ModelId=41C92C510177
CBaseObject* GemContainer::Remove(const CGUID& guid, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	CGoods* pResult = NULL;
	if( m_pGemCoins )
	{
		if( m_pGemCoins->GetExID() == guid )
		{
			for( size_t i = 0; i < m_vListeners.size(); i ++ )
			{
				if( m_vListeners[i] )
				{
					m_vListeners[i] -> OnObjectRemoved( this, m_pGemCoins, m_pGemCoins -> GetAmount(), pVoid );
				}
			}
			pResult = m_pGemCoins;
			m_pGemCoins = NULL;
		}
	}
	return pResult;
}



//##ModelId=41CA26DA000F
BOOL GemContainer::OnObjectAdded(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
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
BOOL GemContainer::OnObjectRemoved(CContainer* pContainer, CBaseObject* pObj, DWORD dwAmount, LPVOID pVoid)
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

BOOL GemContainer::Serialize( DBWriteSet& setWriteDB, BOOL b  )
{
	if( m_pGemCoins )
	{
		setWriteDB.AddToByteArray( static_cast<BYTE>(1) );
		m_pGemCoins -> Serialize( setWriteDB, b );
	}
	else
	{
		setWriteDB.AddToByteArray( static_cast<BYTE>(0) );
	}
	return TRUE;
}
BOOL GemContainer::Unserialize( DBReadSet& setReadDB, BOOL b )
{	
	GemContainer::Clear();
	BYTE bGoodsAmount = setReadDB.GetByteFromByteArray( );
	if( bGoodsAmount > 0 )
	{
		m_pGemCoins = CGoodsFactory::UnserializeGoods( setReadDB,22);
	}
	return TRUE;
}


//##ModelId=41CA6A2203C8
DWORD GemContainer::GetGoldCoinsAmount()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	DWORD dwResult = 0;
	if( m_pGemCoins )
	{
		dwResult = m_pGemCoins -> GetAmount();
	}
	return dwResult;
}

//##ModelId=41CA6A4203D8
BOOL GemContainer::AddGoldCoins(DWORD dwAmount, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	CS2CContainerObjectMove* pMsg = reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
	if( m_pGemCoins && dwAmount )
	{
		if( m_pGemCoins -> GetAmount() + dwAmount <= m_pGemCoins -> GetMaxStackNumber() )
		{
			m_pGemCoins -> SetAmount( m_pGemCoins -> GetAmount() + dwAmount );
			if( pMsg )
			{
				pMsg -> SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
				pMsg -> SetDestinationContainer( GetOwnerType(), GetOwnerID(), 0 );
				pMsg -> SetDestinationObject( m_pGemCoins -> GetType(), m_pGemCoins -> GetExID() );
				pMsg -> SetDestinationObjectAmount( m_pGemCoins -> GetAmount() );
			}
			bResult = TRUE;
		}
	}
	else if( m_pGemCoins == NULL && dwAmount )
	{
		vector<CGoods*> vOut;
		CGoodsFactory::CreateGoods( CGoodsFactory::GetGemIndex(), dwAmount, vOut,23 );
		for( size_t i = 0; i < vOut.size(); i ++ )
		{
			if( vOut[i] )
			{
				if( GemContainer::Add(vOut[i], NULL, pVoid) )
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
				m_pGemCoins -> SerializeForOldClient( &( pMsg -> GetObjectStream() ) );
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
	CS2CContainerObjectMove* pMsg = reinterpret_cast<CS2CContainerObjectMove*>( pVoid );
	if( m_pGemCoins && dwAmount )
	{
		if( m_pGemCoins -> GetAmount() - dwAmount > 0 )
		{
			m_pGemCoins -> SetAmount( m_pGemCoins -> GetAmount() - dwAmount );
			if( pMsg )
			{
				pMsg -> SetOperation( CS2CContainerObjectMove::OT_MOVE_OBJECT );
				pMsg -> SetSourceContainer( GetOwnerType(), GetOwnerID(), 0 );
				pMsg -> SetSourceObject( m_pGemCoins -> GetType(), m_pGemCoins -> GetExID() );
				pMsg -> SetSourceObjectAmount( dwAmount );
			}
			bResult = TRUE;
		}
		else if(  m_pGemCoins -> GetAmount() - dwAmount == 0 )
		{
			CGoods* pCoins = dynamic_cast<CGoods*>( Remove( m_pGemCoins->GetExID(), pVoid ) );
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

