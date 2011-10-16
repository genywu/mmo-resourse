///
/// @file AHConf.h
/// @brief Config reader for AH module
///
#include "stdafx.h"
#include "AHConf.h"

namespace AH
{
    ConfReader::ConfReader()
    {
        Reset();
    }

    void ConfReader::Reset()
    {
        memset( &m_Cfg, 0, sizeof( m_Cfg ) );
    }

    bool ConfReader::Load()
    {
        Reset();
		const std::string file = "data/AHConf.xml";
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
		TiXmlElement *elem = root->FirstChildElement( "Global" );
        if( elem != NULL )
        {
            LoadGlobal( elem );
        }
        return true;
    }

    void ConfReader::Encode( DBWriteSet &db )
    {
        db.AddToByteArray( &m_Cfg, sizeof( m_Cfg ) );
    }

    void ConfReader::Decode( DBReadSet &db )
    {
        Reset();
        db.GetBufferFromByteArray( &m_Cfg, sizeof( m_Cfg ) );
    }

    void ConfReader::LoadGlobal( TiXmlElement *elem )
    {
        XML_QUERY_NUM( "TimeOut", m_Cfg.timeOut, elem, int );
        XML_QUERY_NUM( "CheckInterval", m_Cfg.checkInter, elem, int );
        XML_QUERY_NUM( "SaveInterval", m_Cfg.saveInter, elem, int );
        XML_QUERY_NUM( "GoodsCountPerOrder", m_Cfg.goodsCnt, elem, int );
        XML_QUERY_NUM( "ChargePercent", m_Cfg.chargePer, elem, float );
        XML_QUERY_NUM( "DisplayOrderCount", m_Cfg.orderCnt, elem, int );
        XML_QUERY_NUM( "AgentMax", m_Cfg.agentMax, elem, int );

        const char *str;
        XML_QUERY_STR( "GoodsOrigName", str, elem );
        strncpy( m_Cfg.goodsOrig, str, sizeof( m_Cfg.goodsOrig ) );
    }
}
