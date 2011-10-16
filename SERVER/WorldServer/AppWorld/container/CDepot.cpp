// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "..\Goods\CGoods.h"
#include "CDepot.h"
#include "..\Player.h"




//##ModelId=41DA1CEF035B
CDepot::CDepot()
{
	// TODO: Add your specialized code here.
	m_bIsLocked = FALSE;
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
	m_bIsLocked = FALSE;
	CVolumeLimitGoodsContainer::Clear();
}

//##ModelId=41DA1D1E0119
VOID CDepot::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_bIsLocked = FALSE;
	CVolumeLimitGoodsContainer::Release();
}

//##ModelId=41DA1D520148
BOOL CDepot::Add(CBaseObject* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_bIsLocked == FALSE )
	{
		bResult = CVolumeLimitGoodsContainer::Add( pObject, pVoid );
	}
	return bResult;
}

//##ModelId=41DA1D5A0290
BOOL CDepot::Add(DWORD dwPosition, CGoods* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_bIsLocked == FALSE )
	{
		bResult = CVolumeLimitGoodsContainer::Add( dwPosition, pObject, pVoid );
	}
	return bResult;
}


BOOL CDepot::AddFromDB( CGoods* pObject, DWORD dwPosition )
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
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

		sprintf(str, "仓库:[%s](%s)在%d位置已有[%s](%s)。",pObject->GetName(),szGuid,
			dwPosition,	pGoods->GetName(), szExGuid );
		
		PutStringToFile("debug-DB", str);
		return FALSE;
	}
	BOOL bResult = FALSE;
	if( m_bIsLocked == FALSE )
	{
		bResult = CVolumeLimitGoodsContainer::AddFromDB( pObject, dwPosition );
	}
	return bResult;
}

//##ModelId=41DA1D6C01B5
BOOL CDepot::Lock()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//m_bIsLocked = TRUE;
	//return m_bIsLocked;
	return TRUE;
}

//##ModelId=41DA1D820177
BOOL CDepot::Unlock(CHAR* strKey)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//BOOL bResult = FALSE;
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
	//return bResult;
	return TRUE;
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

