#include "StdAfx.h"
#include "MsgDescribe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMsgDescribe* CMsgDescribe::g_pMsgDesc = NULL;

CMsgDescribe* CMsgDescribe::GetInstance()
{
	if( g_pMsgDesc == NULL ) 
	{
		g_pMsgDesc = new CMsgDescribe;
		assert( g_pMsgDesc );
	}
	return g_pMsgDesc;
}

VOID CMsgDescribe::FinalRelease()
{
	if( g_pMsgDesc ) 
	{
		SAFE_DELETE( g_pMsgDesc );
	}
}

CMsgDescribe::CMsgDescribe()
{
	ifstream fFile( "MsgDescribe.txt");
	if( !fFile.is_open() )
	{
		return;
	}
	ulong dwMsgID;
	string strDesc;
	while( 1 )
	{
		if( fFile.eof() ) break;
		fFile >> dwMsgID;
		fFile >> strDesc;
		m_mapMsg[dwMsgID] = strDesc;
	}
	fFile.close();
}

CMsgDescribe::~CMsgDescribe()
{
	m_mapMsg.clear();
}

const CHAR* CMsgDescribe::GetDesc( ulong dwID )
{
	static CHAR szID[32];
	map<ulong, string>::iterator itRet = m_mapMsg.find( dwID );
	if( itRet == m_mapMsg.end() )
	{
		sprintf( szID, "%d", dwID );
		return szID;
	}
	else
	{
		return m_mapMsg[dwID].c_str();
	}
}