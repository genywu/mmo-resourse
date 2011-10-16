///
///
///
#include "Stdafx.h"
#include "MSConfig.h"
#include "../../../public/tinyxml.h"

bool CMSConfig::Load( const std::string &file )
{
	// load from package
	CRFile *pRFile = rfOpen( file.c_str() );
	if( pRFile == NULL )
	{
		return false;
	}

	bool ret = true;
	TiXmlDocument doc( file.c_str() );
	if( !doc.LoadData( pRFile->GetData(), pRFile->GetDatalen() ) )
	{
		rfClose( pRFile );
		return false;
	}
	rfClose( pRFile );
	
	// root element
	TiXmlElement *root = doc.RootElement();

	// local address element
	TiXmlElement *elem = root->FirstChildElement( "Service" );
	if( elem == NULL )
	{
		return false;
	}
	else
	{
		ret = LoadLocal( elem ) && ret;		
	}

	// monster server list
	elem = root->FirstChildElement( "MonsterServers" );
	if( elem == NULL )
	{
		return false;
	}
	else
	{
		ret = LoadMSList( elem ) && ret;
	}

	// region list
	elem = root->FirstChildElement( "Regions" );
	if( elem == NULL )
	{
		return false;
	}
	else
	{
		ret = LoadRNGList( elem ) && ret;
	}

	return ret;
}

CMSConfig::tagAddr CMSConfig::GetMSAddr( int id ) const
{
	MSListType::const_iterator it = m_MSList.find( id );
	if( it != m_MSList.end() )
	{
		return it->second ;
	}

	return tagAddr();
}

int CMSConfig::GetMSID( const tagAddr &addr ) const
{
	for( MSListType::const_iterator it = m_MSList.begin(); it != m_MSList.end(); ++ it )
	{
		if( it->second == addr )
		{
			return it->first;
		}
	}

	return 0;
}

CMSConfig::tagRgn CMSConfig::GetRGNInfo( int id ) const
{
	RGNListType::const_iterator it = m_RGNList.find( id );
	if( it != m_RGNList.end() )
	{
		return it->second;
	}

	return tagRgn();
}


bool CMSConfig::LoadLocal( TiXmlElement *pElem ) 
{
  int tmp;
	int port;
  m_ServiceParam.m_Addr.m_strIP = pElem->Attribute( "ip" );
	pElem->Attribute( "port", &port );
  m_ServiceParam.m_Addr.m_nPort = static_cast<unsigned short>( port );

#define QueryAttr(key,var) \
  if (pElem->Attribute (key, &tmp)) {var = tmp;}

  // ÍøÂçÀ©Õ¹ÅäÖÃ
  QueryAttr ("MaxFreeSockOperNum", m_ServiceParam.m_nMaxFreeSockOperNum);
  QueryAttr ("MaxFreeIOOperNum", m_ServiceParam.m_nMaxFreeIOOperNum);
  QueryAttr ("IOOperDataBufNum", m_ServiceParam.m_lIOOperDataBufNum);
  QueryAttr ("Check", m_ServiceParam.m_bCheck);
  QueryAttr ("ForbidTime", m_ServiceParam.m_lForbidTime);
  QueryAttr ("MaxMsgLen", m_ServiceParam.m_lMaxMsgLen);
  QueryAttr ("MaxConNums", m_ServiceParam.m_lMaxConNums);
  QueryAttr ("TotalClientsNum", m_ServiceParam.m_lTotalClientsNum);
  QueryAttr ("PendingWrBufNum", m_ServiceParam.m_lPendingWrBufNum);
  QueryAttr ("PendingRdBufNum", m_ServiceParam.m_lPendingRdBufNum);
  QueryAttr ("MaxSendSizePerSecond", m_ServiceParam.m_lMaxSendSizePerSecond);
  QueryAttr ("MaxRecvSizePerSecond", m_ServiceParam.m_lMaxRecvSizePerSecond);
  QueryAttr ("MaxBlockSendMsgNum", m_ServiceParam.m_lMaxBlockSendMsgNum);
  QueryAttr ("ConPendingWrBufNum", m_ServiceParam.m_lConPendingWrBufNum);
  QueryAttr ("ConPendingRdBufNum", m_ServiceParam.m_lConPendingRdBufNum);
  QueryAttr ("ConMaxSendSizePerSecond", m_ServiceParam.m_lConMaxSendSizePerSecond);
  QueryAttr ("ConMaxRecvSizePerSecond", m_ServiceParam.m_lConMaxRecvSizePerSecond);
  QueryAttr ("ConMaxBlockSendMsgNum", m_ServiceParam.m_lConMaxBlockSendMsgNum);

#undef QueryAttr

  return true;
}

bool CMSConfig::LoadMSList( TiXmlElement *pElem ) 
{
	TiXmlElement *child = pElem->FirstChildElement();
	int id, port;
	const char *ip;

	while( child != NULL )
	{
		child->Attribute( "id", &id );
		child->Attribute( "port", &port );
		ip = child->Attribute( "ip" );

		// save it
		m_MSList.insert( MSListType::value_type( id, tagAddr( ip, port ) ) );
		// next
		child = child->NextSiblingElement(); 
	}
	return true;
}

bool CMSConfig::LoadRNGList( TiXmlElement *pElem )
{
	TiXmlElement *child = pElem->FirstChildElement();
	int id, value, msid;

	while( child != NULL )
	{
		child->Attribute( "id", &id );
		child->Attribute( "value", &value );
		child->Attribute( "msid", &msid );

		// save it
		m_RGNList.insert( RGNListType::value_type( id, tagRgn( value, msid ) ) );
		// next
		child = child->NextSiblingElement(); 
	}
	return true;
}
