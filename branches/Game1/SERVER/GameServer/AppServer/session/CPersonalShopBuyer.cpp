// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "stdafx.h"
#include "CPersonalShopSeller.h"
#include "CPersonalShopBuyer.h"
#include "CSession.h"
#include "CSessionFactory.h"
#include "..\MoveShape.h"
#include "..\Player.h"
#include "..\Container\CContainer.h"


//##ModelId=422677F80157
CPersonalShopBuyer::CPersonalShopBuyer()
{
	// TODO: Add your specialized code here.
}

//##ModelId=422677FE0271
//CPersonalShopBuyer::CPersonalShopBuyer(LONG lID)
//{
//	// TODO: Add your specialized code here.
//}

//##ModelId=422677FB0399
CPersonalShopBuyer::~CPersonalShopBuyer()
{
	// TODO: Add your specialized code here.
}

//##ModelId=4226782B007D
BOOL CPersonalShopBuyer::IsPlugAvailable()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return TRUE;
}

//##ModelId=4226783100AB
BOOL CPersonalShopBuyer::OnSessionEnded()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return OnPlugEnded();
}

//##ModelId=42267837003E
BOOL CPersonalShopBuyer::OnSessionAborted()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return OnPlugEnded();
}

//##ModelId=4226783E032C
BOOL CPersonalShopBuyer::OnPlugInserted()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return OnPlugEnded();
}

//##ModelId=4226784402FD
BOOL CPersonalShopBuyer::OnPlugAborted()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return OnPlugEnded();
}

//##ModelId=4226784C038A
BOOL CPersonalShopBuyer::OnPlugEnded()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	BOOL bResult = FALSE;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>( GetOwner() );
	if( pPlayer )
	{
		pPlayer -> SetCurrentProgress( CPlayer::PROGRESS_NONE );

		CMessage msg( MSG_S2C_PLAYERSHOP_QUIT );
		msg.Add( m_SessionID );
		msg.SendToPlayer( pPlayer -> GetExID() );

		bResult = TRUE;
	}
	return bResult;
}

//##ModelId=4226785503A9
BOOL CPersonalShopBuyer::OnChangeState(LONG lPlugID, LONG lStateID, BYTE* pData)
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
	return TRUE;
}

//##ModelId=4226785D0167
VOID CPersonalShopBuyer::Release()
{
	// TODO: Add your specialized code here.
	// NOTE: Requires a correct return value to compile.
}

//##ModelId=422678630222
CContainer* CPersonalShopBuyer::GetContainer(LONG lID)
{
	// TODO: Add your specialized code here.
	return NULL;
}

//##ModelId=422E4F910119
CPersonalShopSeller* CPersonalShopBuyer::GetSeller()
{
	// TODO: Add your specialized code here.
	CPersonalShopSeller* pResult = NULL;
	CSession* pSession = const_cast<CSession*>( CSessionFactory::QuerySession(m_SessionID) );
	if( pSession )
	{
		list<LONG>::iterator it = pSession -> GetPlugList().begin();
		for( ; it != pSession -> GetPlugList().end(); it ++ )
		{
			pResult = dynamic_cast<CPersonalShopSeller*>( pSession -> QueryPlugByID(*it) );
			if( pResult )
			{
				break;
			}
		}
	}
	return pResult;
}

