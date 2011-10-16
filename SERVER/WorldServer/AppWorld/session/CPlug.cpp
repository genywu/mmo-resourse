//// Copyright (C) 1991 - 1999 Rational Software Corporation
//
#include "stdafx.h"
//#include "CSession.h"
//#include "CSessionFactory.h"
//#include "CPlug.h"
//#include "..\Player.h"
//#include "..\MoveShape.h"
//
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif
//
////##ModelId=41DCD92303A9
//CPlug::CPlug()
//{
//	// TODO: Add your specialized code here.
//	m_SessionGuid	= 0;
//	m_lOwnerType	= 0;
//	//m_OwnerGUID		= 0;
//	m_bPlugEnded	= FALSE;
//}
//
////##ModelId=41DCD92B002E
////CPlug::CPlug(LONG lID)
////{
////	// TODO: Add your specialized code here.
////}
//
////##ModelId=41DCD932032C
//CPlug::~CPlug()
//{
//	// TODO: Add your specialized code here.
//}
//
////##ModelId=41DCEF33006D
//BOOL CPlug::IsPlugAvailable()
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	//BOOL bResult = FALSE;
//	//
//	//CMoveShape* pOwner = GetOwner();
//	//if( pOwner )
//	//{
//	//	CPlayer* pPlayer = dynamic_cast<CPlayer*>( pOwner );
//	//	if( pPlayer )
//	//	{
//	//		//pPlayer ->
//	//	}
//	//	else
//	//	{
//	//		bResult = TRUE;
//	//	}
//	//}
//
//	//return bResult;
//	return TRUE;
//}
//
////##ModelId=41DCD96200EA
//BOOL CPlug::OnSessionEnded()
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	return TRUE;
//}
//
////##ModelId=41DCD97402EE
//BOOL CPlug::OnSessionAborted()
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	return TRUE;
//}
//
////##ModelId=41DDE92303A9
//BOOL CPlug::OnChangeState(const CGUID& PlugGuid, LONG lStateID, BYTE *pData)
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	return TRUE;
//}
//
////##ModelId=41DE25B8002E
//BOOL CPlug::ChangeState(LONG lState, BYTE* pData)
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	BOOL bResult = FALSE;
//	CSession* pSession = GetSession();
//	if( pSession )
//	{
//		pSession -> OnPlugChangeState( GetExID(), lState, pData );
//		bResult = TRUE;
//	}
//	return bResult;
//}
//
////##ModelId=41DE250E0157
//BOOL CPlug::Exit()
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	BOOL bResult = FALSE;
//	CSession* pSession = GetSession();
//	if( pSession )
//	{
//		pSession -> OnPlugChangeState( GetExID(), PS_REMOVED, NULL );
//		bResult	= TRUE;
//		m_bPlugEnded = TRUE;
//	}
//	return bResult;
//}
//
////##ModelId=41DE37B900AB
//VOID CPlug::SetOwner(LONG lType/*, const CGUID& guid*/)
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	m_lOwnerType	= lType;
//	//SetExID(guid);
//}
//
////##ModelId=41DE3FF20186
//BOOL CPlug::IsPlugEnded()
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	return m_bPlugEnded;
//}
//
////##ModelId=41DE41140261
//BOOL CPlug::OnPlugAborted()
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	return TRUE;
//}
//
////##ModelId=41DE411E034B
//BOOL CPlug::OnPlugEnded()
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	return TRUE;
//}
//
////##ModelId=41DE46230213
//BOOL CPlug::OnPlugInserted()
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	return TRUE;
//}
//
////##ModelId=41DE49D8003E
//LONG CPlug::GetOwnerType()
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	return m_lOwnerType;
//}
//
////##ModelId=41DE49DD034B
//const CGUID& CPlug::GetOwnerID()
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	return GetExID();
//}
//
////##ModelId=41DE5134005D
//CMoveShape* CPlug::GetOwner()
//{
//	// TODO: Add your specialized code here.
//	CMoveShape* pResult = NULL;
//	//if( GetOwnerType() == TYPE_PLAYER )
//	//{
//	//	pResult = dynamic_cast<CMoveShape*>( GetGame() -> FindPlayer( GetOwnerID() ) );
//	//}
//	MessageBox( NULL, "Warning : Can't use GetOwner() in world server side.", "Warning", MB_OK );
//	return pResult;
//}
//
////##ModelId=41DF54FD004C
//VOID CPlug::SetSession(const CGUID& guid)
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//	m_SessionGuid = guid;
//}
//
////##ModelId=41DF550A002C
//CSession* CPlug::GetSession()
//{
//	// TODO: Add your specialized code here.
//	return const_cast<CSession*>( CSessionFactory::QuerySession(m_SessionGuid) );
//}
//
////##ModelId=41E1E28F01F4
//VOID CPlug::Release()
//{
//	// TODO: Add your specialized code here.
//	// NOTE: Requires a correct return value to compile.
//}
//
////##ModelId=41EB65C20399
//CContainer* CPlug::GetContainer(LONG lID)
//{
//	// TODO: Add your specialized code here.
//	return NULL;
//}
//
//VOID CPlug::SetPlugType( DWORD dwPlugType )
//{
//	m_dwPlugType = dwPlugType;
//}
//
//DWORD CPlug::GetPlugType()
//{
//	return m_dwPlugType;
//}
//
//BOOL CPlug::Serialize(vector<BYTE>* pStream)
//{
//	_AddToByteArray( pStream, GetPlugType() );
//	_AddToByteArray( pStream, m_lOwnerType );
//	_AddToByteArray( pStream, GetExID() );
//	_AddToByteArray( pStream, static_cast<LONG>(m_bPlugEnded) );
//	return TRUE;
//}
//
//BOOL CPlug::Unserialize(BYTE* pStream, LONG& lOffset)
//{
//	//## Plug Type, Owner Type Owner ID会在之前就取出来。
//	m_bPlugEnded	= _GetLongFromByteArray( pStream, lOffset ); 
//
//	return TRUE;
//}