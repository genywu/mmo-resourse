#include "StdAfx.h"
#include "CConfiger.h"
#include "AntiCheatCommon.h"
#include <algorithm>

#define QUERY_NUM( name, var, elem, type ) \
	do { \
	double _t = 0; \
	elem->Attribute( name, &_t ) ; \
	var = static_cast<type>( _t ); \
	}while( 0 )

#define QUERY_STR( name, var, elem ) \
	do { \
	const char *_t = elem->Attribute( name ); \
	if( _t != 0 ) strcpy( var, _t ); \
	}while( 0 ) 


#define ADJUST_RANGE( r ) \
	if( r.second < r.first ) { \
	PutoutLog( LOG_FILE, LT_WARNING, "%s : %d < %d.", #r, (int)r.second, (int)r.first ); \
	r.second = r.first; \
	}
#define ADJUST_VALUE( var, min, max ) \
	{ \
		if( var < min ) var = min; \
		if( var > max ) var = max; \
	}

namespace AntiCheat
{
	/// 读文件
	bool CConfiger::Load()
	{
		const char *file = "setup/AntiCheat.xml";
		CRFile *prFile = rfOpen( file );
		if( prFile == NULL )
		{
			PutoutLog( LOG_FILE, LT_ERROR, "Load AntiCheat.xml FAILED." );
			return false;
		}

		TiXmlDocument doc( file );
		doc.LoadData( prFile->GetData(), prFile->GetDatalen() );
		rfClose( prFile );

		TiXmlElement *pRoot = doc.RootElement();
		if (pRoot == NULL)
		{
			return false;
		}

		TiXmlElement *pSystemElem = pRoot->FirstChildElement("System");
		if (pSystemElem == NULL)
		{
			PutoutLog( LOG_FILE, LT_ERROR, "Load AntiCheat.xml System Node Failed");
		}
		else 
		{
			QUERY_NUM( "CheckIntervalMin",m_sys.CheckIntervalRange.first,pSystemElem,int);
			QUERY_NUM( "CheckIntervalMax",m_sys.CheckIntervalRange.second,pSystemElem,int);

			QUERY_NUM( "QuitActivityMin",m_sys.QuitActivityRange.first,pSystemElem,int);
			QUERY_NUM( "QuitActivityMax",m_sys.QuitActivityRange.second,pSystemElem,int);

			QUERY_NUM( "RefuseMax",m_sys.RefuseMax,pSystemElem,int);
			QUERY_NUM( "BonusMax",m_sys.BonusMax,pSystemElem,int);
			QUERY_NUM( "AnswerInterval",m_sys.AnswerInterval,pSystemElem,int);
			QUERY_NUM( "PKTimeInterval",m_sys.PkTimeInterval,pSystemElem,int);
			QUERY_NUM( "ActivityMax",m_sys.ActivityMax,pSystemElem,int);

			QUERY_NUM( "PKAddValue",m_sys.PkAddValue,pSystemElem,int);
			QUERY_NUM( "PKDropValue",m_sys.PkDropValue,pSystemElem,int);
			QUERY_NUM( "PKDropInterval",m_sys.PkDropInterval,pSystemElem,int);

			QUERY_NUM( "AtkActivityAddValue",m_sys.AtkActivityAddValue,pSystemElem,int);
			QUERY_NUM( "AtkActivityDropValue",m_sys.AtkActivityDropValue,pSystemElem,int);
			QUERY_NUM( "AtkActivityDropInterval",m_sys.AtkActivityDropInterval,pSystemElem,int);

			QUERY_NUM( "FightActivityMin",m_sys.FightActivityRange.first,pSystemElem,int);
			QUERY_NUM( "FightActivityMax",m_sys.FightActivityRange.second,pSystemElem,int);
	
			QUERY_NUM( "RushHourMin",m_sys.RushHourRange.first,pSystemElem,int);
			QUERY_NUM( "RushHourMax",m_sys.RushHourRange.second,pSystemElem,int);

			QUERY_NUM( "AnswerTimeOutInterval",m_sys.AnswerTimeOutInterval,pSystemElem,int);
			QUERY_NUM( "DropCurWrongValue",m_sys.DropCurWrongValue,pSystemElem,int);

			QUERY_NUM( "UpdateQuestionInterval",m_sys.UpdateQuestionInterval,pSystemElem,int);
			QUERY_NUM( "GenQuestionCount", m_sys.GenQuestionCount, pSystemElem, int );

			QUERY_NUM( "AnswerAlphaMin", m_sys.AnswerAlphaRange.first, pSystemElem, int );
			QUERY_NUM( "AnswerAlphaMax", m_sys.AnswerAlphaRange.second, pSystemElem, int );

			QUERY_NUM( "BanHours", m_sys.BanHours, pSystemElem, int );
			QUERY_NUM( "LevelFilter",m_sys.LevelFilter, pSystemElem,int);
			QUERY_NUM( "Enable", m_sys.Enable, pSystemElem, int );
		}

		TiXmlElement *pTalkElem = pRoot->FirstChildElement( "Talk" );
		if( pTalkElem == NULL )
		{
			PutoutLog( LOG_FILE, LT_ERROR, "Talk element is null" );
		}
		else
		{
			QUERY_NUM( "TalkActivityMin",m_sys.TalkActivityRange.first,pTalkElem,int);
			QUERY_NUM( "TalkActivityMax",m_sys.TalkActivityRange.second,pTalkElem,int);
			QUERY_NUM( "TalkSensActivityMin", m_sys.TalkSensActivityRange.first, pTalkElem, int );
			QUERY_NUM( "TalkSensActivityMax", m_sys.TalkSensActivityRange.second, pTalkElem, int );
			QUERY_NUM( "CheckSensWords", m_sys.CheckSensWords, pTalkElem, int );

			int talk_flag;
			m_sys.TalkCheckFlag = 0;
			QUERY_NUM( "TalkCheckNormal", talk_flag, pTalkElem, int );
			m_sys.TalkCheckFlag |= ( talk_flag != 0 ? eTC_NORMAL : eTC_NONE );

			QUERY_NUM( "TalkCheckRegion", talk_flag, pTalkElem, int );
			m_sys.TalkCheckFlag |= ( talk_flag != 0 ? eTC_REGION : eTC_NONE );

			QUERY_NUM( "TalkCheckPrivate", talk_flag, pTalkElem, int );
			m_sys.TalkCheckFlag |= ( talk_flag != 0 ? eTC_PRIVATE : eTC_NONE );

			QUERY_NUM( "TalkCheckTeam", talk_flag, pTalkElem, int );
			m_sys.TalkCheckFlag |= ( talk_flag != 0 ? eTC_TEAM : eTC_NONE );

			QUERY_NUM( "TalkCheckFaction", talk_flag, pTalkElem, int );
			m_sys.TalkCheckFlag |= ( talk_flag != 0 ? eTC_FACTION : eTC_NONE );

			char SensWordsFile[1024] = { 0 };
			QUERY_STR( "SensWordsList", SensWordsFile, pTalkElem );
			LoadSensWords( SensWordsFile );
		}

		// 校正部分数据
		ADJUST_RANGE( m_sys.CheckIntervalRange );
		ADJUST_RANGE( m_sys.QuitActivityRange );
		ADJUST_RANGE( m_sys.AnswerAlphaRange );
		ADJUST_RANGE( m_sys.TalkActivityRange );
		ADJUST_VALUE( m_sys.AnswerAlphaRange.first, 0, 255 );
		ADJUST_VALUE( m_sys.AnswerAlphaRange.second, 0, 255 );

		LoadScriptTable( pRoot->FirstChildElement( "AnswerRight" ), m_RightScripts );
		LoadScriptTable( pRoot->FirstChildElement( "AnswerWrong" ), m_WrongScripts );

		return true;
	}

	bool CConfiger::LoadScriptTable( TiXmlElement *pElem, ScriptsTable &st )
	{
		if( pElem == NULL )
		{
			return false;
		}

		st.clear();
		ScriptCfg sc;
		char file[512];
		int count;
		for( TiXmlElement *pAnswerElem = pElem->FirstChildElement( "Answer" ); 
			pAnswerElem != NULL;
			pAnswerElem = pAnswerElem->NextSiblingElement() )
		{
			QUERY_NUM( "count", count, pAnswerElem, int );
			QUERY_STR( "script", file, pAnswerElem );
			QUERY_NUM( "limit", sc.limit, pAnswerElem, int );
			sc.file = file;	
			st.insert( ScriptsTable::value_type( count, sc ) );
		}	
		return true;
	}

	/// 编码配置信息到消息
	void CConfiger::EncodeToDataBlock(DBWriteSet &db )
	{
		db.AddToByteArray(&m_sys,sizeof(m_sys));
		db.AddToByteArray((long)m_RightScripts.size());
		SCon_LIST  itConf = m_RightScripts.begin();
		for (;itConf != m_RightScripts.end(); itConf++)
		{
			db.AddToByteArray((long)itConf->first);
			db.AddToByteArray( itConf->second.file.c_str() );
			db.AddToByteArray( (BYTE) itConf->second.limit );
		}

		db.AddToByteArray((long)m_WrongScripts.size());
		itConf = m_WrongScripts.begin();
		for (;itConf != m_WrongScripts.end(); itConf++)
		{
			db.AddToByteArray((long)itConf->first);
			db.AddToByteArray( itConf->second.file.c_str() );
			db.AddToByteArray( (BYTE)itConf->second.limit );
		}

		EncodeSensWords( db );
	}

	void CConfiger::EncodeSensWords( DBWriteSet &db )
	{
		db.AddToByteArray( (long) m_SensWords.size() );
		for( SensWordsList::iterator it = m_SensWords.begin(); 
			it != m_SensWords.end(); ++ it )
		{
			db.AddToByteArray( (*it).c_str() );
		}
	}

	bool CConfiger::LoadSensWords( const char *file )
	{
		char invalid[] = { ' ', 10, 13 };

		m_SensWords.clear();
		FILE *fp = fopen( file, "r" );	
		if( fp == NULL )
		{
			PutoutLog( LOG_FILE, LT_ERROR, "Load %s FAILED.", file );
			return false;
		}

		char Word[512];
		while( fgets( Word, sizeof( Word ), fp ) )
		{
			// to avoid buffer overflow
			Word[sizeof( Word ) - 1] = '\0'; 
			// to remove invalid character after the word
			std::string str( Word );
			std::string::size_type pos = str.find_first_of( invalid );
			if( pos != std::string::npos )
			{
				str.erase( pos );
			}
			if( str.size() > 0 &&
				std::find( m_SensWords.begin(), m_SensWords.end(), str ) ==
				m_SensWords.end() )
			{
				m_SensWords.push_back( str );
			}
		}
		fclose( fp );
		return true;
	}
}