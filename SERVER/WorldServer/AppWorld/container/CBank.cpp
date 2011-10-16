// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "CBank.h"
#include "..\Goods\CGoods.h"
#include "..\Player.h"



//##ModelId=41DA01E80186
CBank::CBank()
{
	// TODO: Add your specialized code here.
	m_bLocked = FALSE;
}

//##ModelId=41DA01FD00DA
//CBank::CBank(LONG lID)
//{
//	// TODO: Add your specialized code here.
//}

//##ModelId=41DA01ED0280
CBank::~CBank()
{
	// TODO: Add your specialized code here.
	Release();
}

//##ModelId=41DA0217029F
CBaseObject* CBank::Find(const CGUID& guid)
{
	// TODO: Add your specialized code here.
	CBaseObject* pResult = NULL;
	if( m_bLocked == FALSE )
	{
		pResult = CWallet::Find( guid );
	}
	return pResult;
}

//##ModelId=41DA022600EA
CBaseObject* CBank::Remove(const CGUID& guid, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	CBaseObject* pResult = NULL;
	if( m_bLocked == FALSE )
	{
		pResult = CWallet::Remove( guid, pVoid );
	}
	return pResult;
}



//##ModelId=41DA0384001F
BOOL CBank::Add(CBaseObject* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_bLocked == FALSE )
	{
		bResult = CWallet::Add( pObject, pVoid );
	}
	return bResult;
}

//##ModelId=41DA038C00DA
BOOL CBank::Add(DWORD dwPosition, CGoods* pObject, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_bLocked == FALSE )
	{
		bResult = CWallet::Add( dwPosition, pObject, pVoid );
	}
	return bResult;
}
BOOL CBank::AddFromDB( CGoods* pObject, DWORD dwPosition )
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

		sprintf(str, "银行:[%s](%s)在%d位置已有[%s](%s)。",pObject->GetName(),szGuid,
			dwPosition,	pGoods->GetName(), szExGuid );
		
		PutStringToFile("debug-DB", str);
		return FALSE;
	}
	BOOL bResult = FALSE;
	if( m_bLocked == FALSE )
	{
		bResult = CWallet::AddFromDB( pObject, dwPosition );
	}
	return bResult;
}
//##ModelId=41DA039601B5
BOOL CBank::Lock()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	//m_bLocked = TRUE;
	//return m_bLocked;
	return TRUE;
}

//##ModelId=41DA039E00EA
BOOL CBank::Unlock(CHAR* strKey)
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
	//			m_bLocked	= FALSE;
	//		}
	//	}
	//}
	//return bResult;
	return TRUE;
}


//##ModelId=41DA099C01A5
VOID CBank::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_bLocked = FALSE;
	CWallet::Release();
}



//##ModelId=41DA09B702FD
VOID CBank::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_bLocked = FALSE;
	CWallet::Clear( pVoid );
}

