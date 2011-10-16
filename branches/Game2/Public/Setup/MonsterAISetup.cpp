///
/// @file MonsterAISetup.cpp
/// @author Kevin Lynx
/// @brief Load Monster AI setup file. "data/MonsterAI.xml"
///
#include "stdafx.h"
#include "MonsterAISetup.h"
#include "../ServerPublic/Server/BaseDef/AIStateType.h"


namespace StateAI
{
	static struct State2Type
	{
		const char *desc;
		long type;
	} State2TypeTable[] = {
		"Fight", ST_FIGHT,
		"Peace", ST_PEACE,
		"Hangup", ST_HANGUP,
		"Cycle", ST_CYCLE,
		"Dead", ST_DEAD,
		"Search", ST_SEARCH,
		"Return", ST_RETURN,
		"Escape", ST_ESCAPE,
		NULL, 0
	};

	static long StateDesc2Type( const char *stateDesc )
	{
		for( int i = 0; stateDesc != NULL && State2TypeTable[i].desc != NULL; ++ i )
		{
			if( strcmp( State2TypeTable[i].desc, stateDesc ) == 0 )
			{
				return State2TypeTable[i].type;
			}
		}
		return ST_INVALID;
	}

	void ImplConfig::Encode( DBWriteSet &db ) const
	{
		db.AddToByteArray( (long) m_Impl.size() );
		for( ImplTableT::const_iterator it = m_Impl.begin();
			it != m_Impl.end(); ++ it )
		{
			db.AddToByteArray( (long) it->first );
            const Impl &impl = it->second;
			db.AddToByteArray( (void*) &impl, sizeof( impl ) );
		}
        db.AddToByteArray( m_EventScript );
	}

	void ImplConfig::Decode( DBReadSet &db )
	{
		m_Impl.clear();	
		long size = db.GetLongFromByteArray();
		for( long i = 0; i < size; ++ i )
		{
			long type = db.GetLongFromByteArray();
            Impl impl;
			db.GetBufferFromByteArray( &impl, sizeof( impl ) );
			m_Impl[type] = impl;
		}
        db.GetStringFromByteArray( m_EventScript, sizeof( m_EventScript ) );
	}

    void ImplConfig::AddImpl( long type, long implType )
    {
        Impl impl;
        impl.impl = implType;
        impl.script[0] = '\0';
        m_Impl[type] = impl;
    }

	bool MonsterAISetup::Load()
	{
		Clear();
		const std::string file = "data/MonsterAI.xml";
		CRFile *rFile = rfOpen( file.c_str() );
		if( rFile == NULL )
		{
			return false;
		}

		TiXmlDocument doc( file.c_str() );
		doc.LoadData( rFile->GetData(), rFile->GetDatalen() );
		rfClose( rFile );

		TiXmlElement *root = doc.RootElement();
		if( root == NULL )
		{
			return false;
		}
		TiXmlElement *elem = root->FirstChildElement();
		while( elem != NULL )
		{
			if( !LoadImpls( elem ) )
			{
				return false;
			}
			elem = elem->NextSiblingElement();
		}

		return true;
	}

	bool MonsterAISetup::LoadImpls( TiXmlElement *elem )
	{
		long type;
		XML_QUERY_NUM( "type", type, elem, long );
		assert( m_AITable.find( type ) == m_AITable.end() );
		ImplConfig implCfg;
		TiXmlElement *childElem = elem->FirstChildElement();
		long inherit ;
		XML_QUERY_NUM( "inherit", inherit, elem, long );
		if( inherit != 0 )
		{
			const ImplConfig *baseImpl = GetImpl( inherit );
			if( baseImpl != NULL )
			{
				implCfg = *baseImpl;
			}
		}

        const char *eventScript = NULL;
        XML_QUERY_STR_DEFAULT( "eventHandler", eventScript, elem, "" );
        strncpy( implCfg.m_EventScript, eventScript, sizeof( implCfg.m_EventScript ) );
        implCfg.m_EventScript[strlen( eventScript )] = '\0';

		for( ; childElem != NULL; childElem = childElem->NextSiblingElement() )
		{
			const char *stateDesc = NULL;
            ImplConfig::Impl impl ;
			XML_QUERY_STR( "name", stateDesc, childElem );
            XML_QUERY_NUM( "type", impl.impl, childElem, long );
            const char *implScript = NULL;
            XML_QUERY_STR_DEFAULT( "script", implScript, childElem, "" );
            strncpy( impl.script, implScript, sizeof( impl.script ) - 1 );
            impl.script[strlen( implScript )] = '\0';
            
			long state = StateDesc2Type( stateDesc );
			if( state != ST_INVALID )
			{
				implCfg[state] = impl;
			}
		}

		m_AITable[type] = implCfg;
		return true;
	}

	void MonsterAISetup::Clear()
	{
		m_AITable.clear();
	}

	const ImplConfig *MonsterAISetup::GetImpl( long aiType ) const
	{
		AITableT::const_iterator it = m_AITable.find( aiType );
		if( it == m_AITable.end() )
		{
			return NULL;
		}
		return &it->second;
	}

	void MonsterAISetup::Encode( DBWriteSet &db ) const
	{
		db.AddToByteArray( (long) m_AITable.size() );
		for( AITableT::const_iterator it = m_AITable.begin();
			it != m_AITable.end(); ++ it )
		{
			db.AddToByteArray( (long) it->first );
			it->second.Encode( db );
		}
	}

	void MonsterAISetup::Decode( DBReadSet &db )
	{
		Clear();
		long size = db.GetLongFromByteArray();
		for( long i = 0; i < size; ++ i )
		{
			long aiType = db.GetLongFromByteArray();
			ImplConfig impl;
			impl.Decode( db );
			m_AITable[aiType] = impl;
		}
	}
}
