///
/// @file ScriptGlobalFunc.cpp
/// @brief 参见 ScriptGlobalFunc.h
///
#include "stdafx.h"
#include "../Script.h"
#include "../../ServerRegion.h"
#include "../../RgnManager.h"
#include "../../Player.h"
#include "../../../../../Public/Setup/GMList.h"

void Script::KickRegionPlayer( const CGUID &regionID )
{
	if( m_RunEnv.srcShape == NULL )
	{
		return;
	}
	CServerRegion *region = GetGame()->FindRegion( regionID );
	if( region != NULL )
	{
		BEGIN_LOOP_REGION_SHAPE_BY_TYPE( region, TYPE_PLAYER )
		{
			if( pShape->GetExID() != m_RunEnv.srcShape->GetExID() )
			{
				GetGame()->KickPlayer( pShape->GetExID() );
			}
		}
		END_LOOP_REGION_SHAPE_BY_TYPE
	}
	else
	{
		CMessage msg( MSG_S2W_GM_KICK_REGIONPLAYER );
		msg.Add( m_RunEnv.srcShape -> GetExID() );
		msg.Add( regionID );
		msg.Send();
	}
}

void Script::KickAllPlayer()
{
	CMessage msg( MSG_S2W_GM_KICK_ALLPLAYER );
	msg.Add( m_RunEnv.srcShape == NULL ? NULL_GUID : 
		m_RunEnv.srcShape->GetExID() );
	msg.Send();
}

void Script::KickPlayer( const char *name )
{
	CPlayer *self = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( self == NULL )
	{
		return;
	}
	CPlayer *player = GetGame()->FindPlayer( name );
	if( player != NULL )
	{
		GetGame()->KickPlayer( player->GetExID() );
		char info[128];
		_snprintf( info, sizeof( info ), "玩家%s已经被踢出游戏。", name );
		self->SendNotifyMessage( info );
	}
	else
	{
		CMessage msg( MSG_S2W_GM_KICKPLAYER );
		msg.Add( self->GetExID() );
		msg.Add( name );
		msg.Send();
	}
}

int Script::ListOnlineGM()
{
	CPlayer *self = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	CPlayer *player;
	tagGMInfo *GM;
	int count = 0;
	char buf[64];
	for( map<string, tagGMInfo>::iterator it = GMList::GetGMInfoMap()->begin(); 
		it != GMList::GetGMInfoMap()->end(); ++it )
	{
		GM = &it->second;				
		player = GetGame()->FindPlayer( GM->strName.c_str() );	
		if( player != NULL )
		{
			count ++;
			switch( GM->lLevel )
			{
			case GM_ADMIN:
				_snprintf( buf, sizeof( buf ), "%s (系统管理员)", GM->strName.c_str() );
				break;
			case GM_ARCH:
				_snprintf( buf, sizeof( buf ), "%s (游戏管理员)", GM->strName.c_str() );
				break;
			case GM_WIZARD:
				_snprintf( buf, sizeof( buf ), "%s (巫师)", GM->strName.c_str() );
				break;
			default:
				_snprintf( buf, sizeof( buf ), "%s unknown", GM->strName.c_str() );
			}

			self->SendNotifyMessage( buf );
		}
	}
	CMessage msg( MSG_S2W_GM_QUERYGM );
	msg.Add( self->GetExID() );
	msg.Send();
	return count;
}

void Script::ListSelencePlayer()
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		CMessage msg( MSG_S2W_GM_QUERY_SILENCE_PLAYERS );
		msg.Add( player->GetExID() );
		msg.Send();
	}
}

void Script::BanPlayer( const char *name, long time )
{
	CPlayer *self = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( self != NULL )
	{
		CPlayer *player = GetGame()->FindPlayer( name );
		if( player != NULL )
		{
			GetGame()->KickPlayer( player->GetExID() );
		}
		CMessage msg( MSG_S2W_GM_BANPLAYER );
		msg.Add( self->GetExID() );
		msg.Add( name );
		msg.Add( time );
		msg.Send();
	}
}

const char *Script::GetTextRes( const char* id )
{
	return AppFrame::GetText( id );
}

void Script::RollRegionInfo( long rgnType, long num, const char *str, long fileNum, long rgnID )
{
	if( m_RunEnv.srcShape == NULL )
	{
		return;
	}
	CServerRegion *region = NULL;
	if( rgnID == -1 )
	{
		region = dynamic_cast<CServerRegion*>( m_RunEnv.srcShape->GetFather() );
	}
	else
	{
		region = GameManager::GetInstance()->GetRgnManager()->FindRgnByTemplateID(
			(eRgnType) rgnType, rgnID ); 
	}
	if( region != NULL )
	{
		CMessage msg( MSG_S2C_OTHER_NEWS_TOP_WORLD );
		msg.Add( num );
		msg.Add( str );
		msg.Add( fileNum );

		BEGIN_LOOP_REGION_SHAPE_BY_TYPE( region, TYPE_PLAYER )
		{
			msg.SendToPlayer( pShape->GetExID() );
		}
		END_LOOP_REGION_SHAPE_BY_TYPE
	}
	else
	{
		CMessage msg( MSG_S2W_OTHER_NEWS_TOP_REGION );
		msg.Add( rgnID );
		msg.Add( rgnType );
		msg.Add( num );			
		msg.Add( str );
		msg.Add( fileNum );
		msg.Send();
	}
}

void Script::RollWorldInfo( long num, const char *str, long fileNum )
{
	CMessage msg( MSG_S2W_OTHER_NEWS_TOP_WORLD );
	msg.Add( num );
	msg.Add( str );
	msg.Add( fileNum );
	msg.Send();
}

void Script::ChangePlayerRegionEx( long rgnType, const char *centerPlayer, 
								  const CGUID &rgnID, long x, long y )
{
	CPlayer *player = GetGame()->FindPlayer( centerPlayer );
	CServerRegion *rgn = dynamic_cast<CServerRegion*>( player->GetFather() );
	long playerX = player->GetTileX();
	long playerY = player->GetTileY();
	long startX = playerX - 3;
	long startY = playerY - 3;
	if( playerX + 3 >= rgn->GetWidth() )
	{
		startX = rgn->GetWidth() - 7;
	}
	else if( playerX - 3 <= 0 )
	{
		startX = 0;
	}
	if( playerY + 3 >= rgn->GetHeight() )
	{
		startY = rgn->GetHeight() - 7;
	}
	else if( playerY - 3 <= 0 )
	{
		startY = 0;
	}
	std::list<CPlayer*> playerList;
	CShape *shape;
	for( long i = startX; i < startX + 7; ++ i )
	{
		for( long j = startY; j < startY + 7; ++ j )
		{
			shape = rgn->GetShape( i, j );
			if( shape && shape->GetType() == TYPE_PLAYER )
			{
				playerList.push_back( dynamic_cast<CPlayer*>( shape ) );
			}
		}
	}
	playerList.unique();
	for( std::list<CPlayer*>::iterator it = playerList.begin();
		it != playerList.end(); ++ it )
	{
		(*it)->ChangeRegion( (eRgnType) rgnType, rgnID, x, y, (*it)->GetDir() ); 
	}
}