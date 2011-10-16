// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "CBank.h"
#include "..\Goods\CGoods.h"
#include "..\Player.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//##ModelId=41DA01E80186
CBank::CBank()
{
	// TODO: Add your specialized code here.
	m_bLocked = TRUE;
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
BOOL CBank::Add(CBaseObject* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_bLocked == FALSE )
	{
		bResult = CWallet::Add( pObject, pPreviousContainer, pVoid );
	}
	return bResult;
}

//##ModelId=41DA038C00DA
BOOL CBank::Add(DWORD dwPosition, CGoods* pObject, tagPreviousContainer* pPreviousContainer, LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;
	if( m_bLocked == FALSE )
	{
		bResult = CWallet::Add( dwPosition, pObject, pPreviousContainer, pVoid );
	}
	return bResult;
}

//##ModelId=41DA039601B5
BOOL CBank::Lock()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_bLocked = TRUE;
	return m_bLocked;
}

//##ModelId=41DA039E00EA
BOOL CBank::Unlock(CHAR* strKey)
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
	//			m_bLocked	= FALSE;
	//		}
	//	}
	//}
	return bResult;
}


//##ModelId=41DA099C01A5
VOID CBank::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_bLocked = TRUE;
	CWallet::Release();
}



//##ModelId=41DA09B702FD
VOID CBank::Clear(LPVOID pVoid)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	m_bLocked = TRUE;
	CWallet::Clear( pVoid );
}

BOOL CBank::Unserialize( BYTE* pStream, LONG& lOffset, BOOL b )
{
	CBank::Clear();
	m_bLocked = FALSE;
	CWallet::Unserialize( pStream, lOffset, b );
	Lock();
	return TRUE;
}