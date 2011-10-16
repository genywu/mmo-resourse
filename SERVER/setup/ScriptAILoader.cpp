///
/// @file ScriptAILoader.cpp
/// @author Kevin Lynx
/// @brief To manage the script ai function table.
///
#include "stdafx.h"
#include "ScriptAILoader.h"
#include "../../public/tinyxml.h"


#define QUERY_STR( name, var, elem ) \
	do { \
	const char *_t = elem->Attribute( name ); \
	if( _t != 0 ) var = _t; \
	}while( 0 ) 

#define QUERY_NUM( name, var, elem, type ) \
	do { \
	int _t = 0; \
	elem->Attribute( name, &_t ) ; \
	var = static_cast<type>( _t ); \
	}while( 0 )

CScriptAILoader::~CScriptAILoader()
{
	for( FuncRefTableType::iterator it = mFuncRefTable.begin(); it != mFuncRefTable.end(); ++ it )
	{
		delete it->second;
	}
}

bool CScriptAILoader::Load( const std::string &file )
{
	CRFile *prFile = rfOpen( file.c_str() );
	if( prFile == NULL )
	{
		return false;
	}

	TiXmlDocument doc( file.c_str() );
	doc.LoadData( prFile->GetData(), prFile->GetDatalen() );
	rfClose( prFile );

	TiXmlElement *pRoot = doc.RootElement();
	if( pRoot == NULL )
	{
		return false;
	}

	TiXmlElement *pElem = pRoot->FirstChildElement();
	while( pElem != NULL )
	{
		if( !LoadOneChunk( pElem ) )
		{
			return false;
		}
		pElem = pElem->NextSiblingElement();
	}

	return true;
}

bool CScriptAILoader::LoadOneChunk( TiXmlElement *pElem )
{
	// type
	long type;
	std::string funcName, scriptName;
	QUERY_NUM( "type", type, pElem, long );
	TiXmlElement *pFuncElem = pElem->FirstChildElement();
	FuncTableType *pFuncTable = NULL;
	if( pFuncElem != NULL )
	{
		pFuncTable = new FuncTableType();
	}
	while( pFuncElem != NULL )
	{
		QUERY_STR( "name", funcName, pFuncElem );
		QUERY_STR( "script", scriptName, pFuncElem );
		pFuncElem = pFuncElem->NextSiblingElement();

		// convert to lowcase
		char *t =  _strlwr( _strdup( scriptName.c_str() ));
		pFuncTable->insert( FuncTableType::value_type( funcName, t ) );
		free( t );
	}

	if( pFuncTable != NULL )
	{
		mFuncRefTable[type] = pFuncTable;
	}

	return true;
}

void CScriptAILoader::AddToByteArray( std::vector<unsigned char> &OutBuf )
{
	_AddToByteArray( &OutBuf, (long) mFuncRefTable.size() );
	for( FuncRefTableType::iterator it = mFuncRefTable.begin(); it != mFuncRefTable.end(); ++ it )
	{
		// id
		_AddToByteArray( &OutBuf, it->first );
		FuncTableType *pFuncTable = it->second ;
		// function size
		_AddToByteArray( &OutBuf, (long)pFuncTable->size() );
		for( FuncTableType::iterator itFunc = pFuncTable->begin(); itFunc != pFuncTable->end(); ++ itFunc )
		{
			// name
			_AddToByteArray( &OutBuf, itFunc->first.c_str() );
			// script
			_AddToByteArray( &OutBuf, itFunc->second.c_str() );
		}
	}
}

void CScriptAILoader::DecodeFromByteArray( const unsigned char *_pBuf, long &lPos )
{
	mFuncRefTable.clear();
	unsigned char *pBuf = const_cast<unsigned char*>( _pBuf );
	long table_count = _GetLongFromByteArray( pBuf, lPos );

	for( long i = 0; i < table_count; ++ i )
	{
		FuncTableType *pFuncTable = new FuncTableType();
		long id = _GetLongFromByteArray( pBuf, lPos );
		long func_count = _GetLongFromByteArray( pBuf, lPos );
		for( long j = 0; j < func_count; ++ j )
		{
			char funcName[1024];
			char scriptName[1024];
			_GetStringFromByteArray( pBuf, lPos, funcName );
			_GetStringFromByteArray( pBuf, lPos, scriptName );
			pFuncTable->insert( FuncTableType::value_type( funcName, scriptName ) );
		}
		mFuncRefTable[id] = pFuncTable;
	}
}

CScriptAILoader::FuncTableType *CScriptAILoader::QueryFuncTable( long id ) const
{
	FuncRefTableType::const_iterator it = mFuncRefTable.find( id );
	if( it == mFuncRefTable.end() )
	{
		return NULL;
	}

	return it->second;
}