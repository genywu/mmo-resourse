///
/// @file MonsterServerHandler.cpp
/// @author Kevin Lynx
/// @date 9.3.2008
/// @brief To handle the detail of the monster server.
///
#include "stdafx.h"
#include "MSHandler.h"
#include "../nets/Servers.h"
#include "../nets/NetWorld/Message.h"

///
/// pre-declartion the fucking log function.....
///
void AddLogText( char *, ... );

CMSHandler::CMSHandler() 
{
	m_lFlag = 0;
	m_pServer = NULL;
}

CMSHandler::~CMSHandler()
{
}

bool CMSHandler::Init( const std::string &cfg_file, CDataBlockAllocator *allocator, long flag )
{
	// load the config first.
	if( !m_Config.Load( cfg_file ) )
	{
		AddLogText( "Load the config file [%s] failed!", cfg_file.c_str() );
		return false;
	}

	m_lFlag = flag;
	m_pServer = new CServer();
	const CMSConfig::tagServiceParam &listen_addr = m_Config.GetLocalAddr();
	if( !m_pServer->Start( 2, allocator ) )
	{
		AddLogText( "Start the server (for monster server) failed!" );
		return false;
	}
	
	if( !m_pServer->Host( listen_addr.m_Addr.m_nPort, listen_addr.m_Addr.m_strIP.c_str(), flag ) )
	{
		AddLogText( "Host the server (for monster server) failed!" );
		return false;
	}
	return true;
}

void CMSHandler::Release()
{
	m_pServer->Exit();
	SAFE_DELETE( m_pServer );
}

void CMSHandler::ProcessMessage()
{
	long msg_count = m_pServer->GetRecvMessages()->GetSize();
	for( long i = 0; i < msg_count; ++ i )
	{
		CMessage *msg = static_cast<CMessage*>( m_pServer->GetRecvMessages()->PopMessage() );
		if( msg != NULL )
		{
			msg->Run();
			CBaseMessage::FreeMsg( msg );
		}
	}
}