#include "StdAfx.h"
#include ".\emotion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

map<LONG, BOOL> CEmotion::s_mEmotions;

CEmotion::CEmotion(void)
{
}

CEmotion::~CEmotion(void)
{
}

BOOL CEmotion::LoadSetup( CHAR* pFileName )
{
	//m_mapMonsterList.clear();

	CRFile* prfile = rfOpen(pFileName);
	if(prfile)
	{
		stringstream stream;
		prfile->ReadToStream(stream);
		rfClose(prfile);

		while(ReadTo(stream, "*"))
		{
			LONG lID;
			BOOL bLoop;
			stream >> lID >> bLoop;
			s_mEmotions[lID] = bLoop;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CEmotion::Serialize( vector<BYTE>& vIn )
{
	_AddToByteArray( &vIn, static_cast<DWORD>( s_mEmotions.size() ) );

	map<LONG, BOOL>::iterator it = s_mEmotions.begin();
	for( ; it != s_mEmotions.end(); it ++ )
	{
		_AddToByteArray( &vIn, it -> first );
		_AddToByteArray( &vIn, static_cast<LONG>(it -> second) );
	}

	return TRUE;
}

BOOL CEmotion::Unserialize( UCHAR* pArray, LONG& lPos )
{
	DWORD dwSize = _GetDwordFromByteArray( pArray, lPos );

	for( DWORD i = 0; i < dwSize; i ++ )
	{
		LONG lID	= _GetLongFromByteArray( pArray, lPos );
		BOOL bLoop	= _GetLongFromByteArray( pArray, lPos );

		s_mEmotions[lID] = bLoop;
	}
	return TRUE;
}

BOOL CEmotion::IsEmotionRepeated( LONG lID )
{
	BOOL bResult = FALSE;
	map<LONG, BOOL>::iterator it = s_mEmotions.find( lID );
	if( it != s_mEmotions.end() )
	{
		bResult = it -> second;
	}
	return bResult;
}