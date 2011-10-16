// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "..\Goods\CGoods.h"
#include "CDepot.h"
#include "..\Player.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//##ModelId=41DA1CEF035B
CDepot::CDepot()
{
	// TODO: Add your specialized code here.
	m_bIsLocked = TRUE;
}

//##ModelId=41DA1CF3007D
//CDepot::CDepot(LONG lID)
//{
//	// TODO: Add your specialized code here.
//}

//##ModelId=41DA1CF700CB
CDepot::~CDepot()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41DA1D0902AF
VOID CDepot::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_bIsLocked = TRUE;
	CVolumeLimitGoodsContainer::Clear();
}

//##ModelId=41DA1D1E0119
VOID CDepot::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_bIsLocked = TRUE;
	CVolumeLimitGoodsContainer::Release();
}

//##ModelId=41DA1D520148
BOOL CDepot::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_bIsLocked == FALSE )
	{
		bResult = CVolumeLimitGoodsContainer::Add( pObject, pPreviousContainer, pVoid );
	}
	return bResult;
}

//##ModelId=41DA1D5A0290
BOOL CDepot::Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_bIsLocked == FALSE )
	{
		bResult = CVolumeLimitGoodsContainer::Add( dwPosition, pObject, pPreviousContainer, pVoid );
	}
	return bResult;
}

//##ModelId=41DA1D6C01B5
BOOL CDepot::Lock()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_bIsLocked = TRUE;
	return m_bIsLocked;
}

//##ModelId=41DA1D820177
BOOL CDepot::Unlock(CHAR* strKey)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	//if( strKey )
	//{
	//	CPlayer* pOwner = GetGame() -> FindPlayer( GetOwnerID() );
	//	if( pOwner )
	//	{
	//		if( strcmp( strKey, pOwner -> GetDepotPassword() ) == 0 )
	//		{
	//			bResult		= TRUE;
	//			m_bIsLocked	= FALSE;
	//		}
	//	}
	//}
	return bResult;
}

//##ModelId=41DA1D3501E4
CBaseObject* CDepot::Find(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	CBaseObject* pResult = NULL;
	if( m_bIsLocked == FALSE )
	{
		pResult = CVolumeLimitGoodsContainer::Find( guid );
	}
	return pResult;
}

//##ModelId=41DA1D4401F4
CBaseObject* CDepot::Remove(const CGUID& guid, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	CBaseObject* pResult = NULL;
	if( m_bIsLocked == FALSE )
	{
		pResult = CVolumeLimitGoodsContainer::Remove( guid, pVoid );
	}
	return pResult;
}

BOOL CDepot::Unserialize( BYTE* pStream, LONG& lOffset, BOOL b )
{
	CDepot::Clear();
	m_bIsLocked = FALSE;
	CVolumeLimitGoodsContainer::Unserialize( pStream, lOffset, b );
	Lock();
	return TRUE;
}

BOOL CDepot::IsDepotLocked()
{
	return m_bIsLocked;
}