#include "StdAfx.h" 
#include "PhaseSetup.h"
#include "../Module/tools.h"
#include "../ServerPublic/Server/BaseDef/PhaseSeverDef.h"


PhaseSetup::PhaseSetup(void)
{
    Release();
}

PhaseSetup::~PhaseSetup(void)
{
    Release();
}

void   PhaseSetup::Release()
{
    memset( &m_config , 0 , sizeof(Config) );
    m_PhaseTable.clear();
}
long   PhaseSetup::Load()
{
      Release();
      const char* rfile = "data/PhaseSetup.xml";
      CRFile* prFile = rfOpen( rfile );
      if ( prFile == NULL )
      {
          Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__, "Load PhaseSetup.xml failed ");
          return  0;
      }

      TiXmlDocument doc(rfile);
      doc.LoadData( prFile->GetData() , prFile->GetDatalen() );
      rfClose( prFile );

      TiXmlElement* pRoot = doc.RootElement();
      if ( pRoot == NULL )
          return 0;

      TiXmlElement* pBaseElem = pRoot->FirstChildElement( "Base" );
      if ( pBaseElem == NULL )
      {
          Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__, "Load PhaseSetup.xml Base Node failed ");
      }
      else 
      {
          XML_QUERY_NUM( "Agonal", m_config.lAgonal, pBaseElem , long );
          XML_QUERY_NUM( "HpNum", m_config.lHpNum  , pBaseElem , long );
          XML_QUERY_NUM( "DropGoodNum",m_config.lDropGoodNum,pBaseElem,long);
          XML_QUERY_NUM( "IsOpen", m_config.lIsOpen , pBaseElem , long );
      }

      TiXmlElement* pPhaseElem = pRoot->FirstChildElement("PhaseSetup");
      if ( pPhaseElem ==  NULL )
      {
          Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__, "Load PhaseSetup Node Failed!" );
      }
      else 
      {   
          long   type = 0 ;
          PhaseTable  tpTemp;
          /// PlayerOper;
          TiXmlElement* pOper = pPhaseElem->FirstChildElement("PlayerOper");
          if( pOper == NULL )
          {
                Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,"Load PlayerOper Node Failed!" );
                return 0;
          } 
          XML_QUERY_NUM( "type", type , pOper, long );
          tpTemp.clear();
          for ( TiXmlElement* pEvent = pOper->FirstChildElement("Event"); pEvent != NULL ;  pEvent = pEvent->NextSiblingElement())
          {
              const char *file = NULL;
              int  id = 0;
              XML_QUERY_NUM( "id", id, pEvent, long );
              XML_QUERY_STR( "script", file, pEvent );

              tpTemp.insert( std::pair<long,std::string>( id, file) ) ;
          }
          m_PhaseTable.insert( std::pair<long,PhaseTable>(type,tpTemp) );

          /// PhaseTimeType
          TiXmlElement* TimeType= pPhaseElem->FirstChildElement("PhaseTimeType");
          if( TimeType == NULL )
          {
              Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__, "Load PhaseTimeType Node Failed!" );
              return 0;
          }
          XML_QUERY_NUM( "type", type , TimeType, long );
          tpTemp.clear();
          for ( TiXmlElement* PhaseTime = TimeType->FirstChildElement("Event"); PhaseTime != NULL ;  PhaseTime = PhaseTime->NextSiblingElement())
          {
              const char *file = NULL;
              int  id = 0;
              XML_QUERY_NUM( "id", id, PhaseTime, long );
              XML_QUERY_STR( "script", file, PhaseTime );

              tpTemp.insert( std::pair<long,std::string>( id, file) ) ;
          }
          m_PhaseTable.insert( std::pair<long,PhaseTable>(type,tpTemp) );

          /// PhaseEvent
          TiXmlElement* Event= pPhaseElem->FirstChildElement("PhaseEvent");
          if( Event == NULL )
          {
             Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__, "Load PhaseEvent Node Failed!" );
              return 0;
          }
          XML_QUERY_NUM( "type", type , Event, long );
          tpTemp.clear();
          for ( TiXmlElement* Ent = Event->FirstChildElement("Event"); Ent != NULL ;  Ent = Ent->NextSiblingElement())
          {
              const char *file = NULL;
              int  id = 0;
              XML_QUERY_NUM( "id", id, Ent, long );
              XML_QUERY_STR( "script", file, Ent );

              tpTemp.insert( std::pair<long,std::string>( id, file) ) ;
          }
          m_PhaseTable.insert( std::pair<long,PhaseTable>(type,tpTemp) );
      }
      return 1;
}

void  PhaseSetup::PhaseConfigDecode(DBReadSet& dbRead)
{
    dbRead.GetBufferFromByteArray(&m_config ,sizeof(Config) );
    long size=  dbRead.GetLongFromByteArray();

    PhaseTable  tpTemp;
    for ( long  i = 0 ; i < size ; i ++ )
    {
        long type = dbRead.GetLongFromByteArray();
        long count= dbRead.GetLongFromByteArray();
        tpTemp.clear();
        for ( int j = 0 ; j < count ; j++ )
        {
            long id = dbRead.GetLongFromByteArray();
            char file[512];
            dbRead.GetStringFromByteArray( file , 512 );
            tpTemp.insert( std::pair<long,std::string>( id, file) ) ; 
        }
        m_PhaseTable.insert( std::pair<long,PhaseTable>( type , tpTemp) );
    }

}

void  PhaseSetup::PhaseConfigEnCode(DBWriteSet& dbWrite)
{
    dbWrite.AddToByteArray( &m_config , sizeof(Config) );

    /// ±àÂë
    dbWrite.AddToByteArray( (long)m_PhaseTable.size() );
    for ( PHITER itr (m_PhaseTable.begin()); itr != m_PhaseTable.end() ; itr ++)
    {
        dbWrite.AddToByteArray( itr->first );
        dbWrite.AddToByteArray( (long)itr->second.size() );
        for ( PIter it = itr->second.begin() ; it != itr->second.end() ; it++ )
        {
            dbWrite.AddToByteArray( it->first );
            dbWrite.AddToByteArray( it->second.c_str() );
        }
    }
}

const char*  PhaseSetup::GetScript(long Scriptid , long type ) const
{
    PHCITER  itr = m_PhaseTable.find( type );

    if ( itr != m_PhaseTable.end() )
    {
        PCIter it = itr->second.find( Scriptid );
        if ( it != itr->second.end() )
        {
            return it->second.c_str();
        }
    }
    return "";
}