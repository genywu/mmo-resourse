///
/// @file ScriptClientFunc.cpp
/// @brief ²Î¼û ScriptClientFunc.h
///
#include "stdafx.h"
#include "../Script.h"
#include "../../Player.h"
#include "../../ServerRegion.h"

void Script::AddInfo( const char *str, ulong fgColor, ulong bkColor, const CGUID &playerID )
{
    CPlayer *player = FindPlayer( playerID );
    if( player != NULL )
    {
       // player->SendNotifyMessage( const_cast<char*>( str ), fgColor, bkColor, eNOTIFYPOS_LEFT_BOTTOM );
		player->SendNotifyMessage(const_cast<char*>(str),eNOTIFYPOS_LEFT_BOTTOM);
    }
}

void Script::PostRegionInfo( const char *str, ulong fgColor, ulong bkColor )
{
	if( m_RunEnv.srcShape == NULL )
	{
		return;
	}
	CServerRegion *region = static_cast<CServerRegion*>( m_RunEnv.srcShape->GetFather() );
	if( region != NULL )
	{
		CMessage msg( MSG_S2C_OTHER_ADDINFO );
		//msg.Add( fgColor );
		//msg.Add( bkColor );
		msg.Add( (uchar) eNOTIFYPOS_LEFT_BOTTOM );
		msg.Add( str );
		BEGIN_LOOP_REGION_SHAPE_BY_TYPE( region, TYPE_PLAYER )
		{
			msg.SendToPlayer( pShape->GetExID() );
		}
		END_LOOP_REGION_SHAPE_BY_TYPE
	}
}

void Script::PostWorldInfo( const char *str, ulong fgColor, ulong bkColor )
{		
	CMessage msg( MSG_S2W_GM_WORLD_MESSAGE );
	if( m_RunEnv.srcShape )
	{
		msg.Add( m_RunEnv.srcShape->GetExID() );
	}
	else
	{
		msg.Add( NULL_GUID );
	}
	msg.Add( str );
	msg.Add( fgColor );
	msg.Add( bkColor );
	msg.Send();
}

void Script::PostCountryInfo( const char *str, long country, ulong fgColor, ulong bkColor )
{
	CMessage msg( MSG_S2W_GM_COUNTRY_MESSAGE );
	msg.Add( m_RunEnv.srcShape == NULL ? NULL_GUID : 
		m_RunEnv.srcShape->GetExID() );
	msg.Add( str );
	msg.Add( country );
	msg.Add( fgColor );
	msg.Add( bkColor );
	msg.Send();
}

void Script::PlayEffectByIndex( long index )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		CMessage msg( MSG_S2C_OTHER_EFFECT );
		msg.Add( index );
		msg.SendToPlayer( player->GetExID() );
	}
}

void Script::PlayerTalk( const char *str )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		CMessage msg( MSG_S2C_OTHER_TALK );
		msg.Add( (uchar)0 );
		msg.Add( static_cast<long>( CHAT_NORMAL ) );
		msg.Add( static_cast<long>( TYPE_PLAYER ) );
		msg.Add( player->GetExID() );
		msg.Add( (long)0 );
		msg.Add( player->GetName() );
		msg.Add( str );
		msg.SendToAround( player ); 
	}
}

void Script::PlayerTalkByIndex( long textType, long textID )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		CMessage msg( MSG_S2C_OTHER_TALK );
		msg.Add( (uchar)1 );
		msg.Add( static_cast<long>( CHAT_NORMAL ) );
		msg.Add( static_cast<long>( TYPE_PLAYER ) );
		msg.Add( player->GetExID() );
		msg.Add( (long)0 );
		msg.Add( player->GetName() );
		msg.Add( textType );
		msg.Add( textID );
		msg.SendToAround( player ); 
	}
}

void Script::PackGlint()
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		player->PackGlint();
	}
}

void Script::PostPlayerInfo( const CGUID &playerID, const char *str, ulong fgColor, ulong bkColor )
{
	CPlayer *player = FindPlayer( playerID );
	if( player != NULL )
	{
		CMessage msg( MSG_S2C_OTHER_ADDPLAYERINFO );
		msg.Add( (long) 0 );
		msg.Add( fgColor );
		msg.Add( bkColor );
		msg.Add( str );
		msg.SendToPlayer( player->GetExID() );
	}
}

void Script::PostPlayerInfoByIndex( const CGUID &playerID, long textType, long textID, 
								   ulong fgColor, ulong bkColor )
{
	CPlayer *player = FindPlayer( playerID );
	if( player != NULL )
	{
		CMessage msg( MSG_S2C_OTHER_ADDPLAYERINFO );
		msg.Add( (long) 1 );
		msg.Add( fgColor );
		msg.Add( bkColor );
		msg.Add( textType );
		msg.Add( textID );
		msg.SendToPlayer( player->GetExID() );
	}
}

void Script::ShowString( long posX, long posY, const char *str, long isShow )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		CMessage msg( MSG_S2C_OTHER_SHOW_STRING );
		msg.Add( isShow );
		if( isShow != 0 )
		{
			msg.Add( posX );
			msg.Add( posY );
			msg.Add( str );
		}
		msg.SendToPlayer( player->GetExID() );
	}
}

void Script::ChangeWeather( long ID )
{
	if( m_RunEnv.srcShape != NULL )
	{
		CServerRegion *rgn = dynamic_cast<CServerRegion*>( m_RunEnv.srcShape->GetFather() );
		if( rgn != NULL )
		{
			rgn->ChangeWeather( ID );
		}
	}
}

void Script::PlayEffect( long effID, long bindPos, uchar inRgn, long x, long y )
{
	if( m_RunEnv.srcShape == NULL )
	{
		return;
	}
	if( inRgn == 0 )
	{
		x = m_RunEnv.srcShape->GetTileX();
		y = m_RunEnv.srcShape->GetTileY();
	}
	CMessage msg( MSG_S2C_RGN_PLAYEFFECT );
	msg.Add( inRgn );
	msg.Add( effID );
	msg.Add( m_RunEnv.srcShape->GetType() );
	msg.Add( m_RunEnv.srcShape->GetExID() );
	msg.Add( x + 0.5f );
	msg.Add( y + 0.5f );
	msg.Add( bindPos );
	msg.SendToAround( static_cast<CServerRegion*>( m_RunEnv.region ), x, y );
}

void Script::PlayAction( ushort action )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		CMessage msg( MSG_S2C_RGN_PLAYACTION );
		msg.Add( player->GetType() );
		msg.Add( player->GetExID() );
		msg.Add( action );
		msg.SendToAround( player );
	}
}

void Script::PlaySound( const char *file, uchar toAround )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		CMessage msg( MSG_S2C_RGN_PLAYSOUND );
		msg.Add( file );
		if( toAround )
		{
			msg.SendToAround( player );
		}
		else
		{
			msg.SendToPlayer( player->GetExID() );
		}
	}
}

void Script::Play3DSound( const char *file, long loop, long x, long y, long z )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		CMessage msg( MSG_S2C_RGN_PLAY3DSOUND );
		msg.Add( file );
		msg.Add( loop );
		msg.Add( x );
		msg.Add( y );
		msg.Add( z );
		msg.SendToPlayer( player->GetExID() );
	}
}

void Script::PlayBkMusic( const char *file, long times )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		CMessage msg( MSG_S2C_RGN_PLAYMUSIC );
		msg.Add( file );
		msg.Add( times );
		msg.SendToPlayer( player->GetExID() );
	}
}

void Script::StopBkMusic()
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		CMessage msg( MSG_S2C_RGN_STOPMUSIC );
		msg.SendToPlayer( player->GetExID() );
	}
}

int Script::AddGoodsToShortcutBar( const char *goodsName, long barIndex )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player == NULL )
	{
		return 0;
	}
	CGoods *goods = player->FindGoodInPack( goodsName );
	if( goods == NULL )
	{
		return 0;
	}
	player->SetHotKey( barIndex, goods->GetBasePropertiesIndex() );

	CMessage msg( MSG_S2C_SETHOTKEY );
	msg.Add( (uchar) HOTKEY_SET_SUCCEED );
	msg.Add( (uchar) barIndex );
	msg.Add( goods->GetBasePropertiesIndex() );
	msg.SendToPlayer( player->GetExID() );

	return 1;
}

int Script::AddSkillToShortcutBar( long skillID, long barIndex )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player == NULL )
	{
		return 0;
	}
	if( !player->CheckSkill( skillID ) )
	{
		return 0;
	}
	skillID |= 0x80000000;
	player->SetHotKey( barIndex, skillID );

	CMessage msg( MSG_S2C_SETHOTKEY );
	msg.Add( (uchar) HOTKEY_SET_SUCCEED );
	msg.Add( (uchar) barIndex );
	msg.Add( skillID );
	msg.SendToPlayer( player->GetExID() );

	return 1;
}

void Script::TransformCamera( long mode, long param1, long param2, long param3 )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		CMessage msg( MSG_S2C_OTHER_TRANSFORCAMERA );
		msg.Add( mode );
		msg.Add( param1 );
		msg.Add( param2 );
		msg.Add( param3 );
		msg.SendToPlayer( player->GetExID() );
	}
}

void Script::OperInterface( long mode, const char *interfaceName )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		CMessage msg( MSG_S2C_OTHER_OPERAINTERFACE );
		msg.Add( mode );
		msg.Add( interfaceName );
		msg.SendToPlayer( player->GetExID() );
	}
}

void Script::HelpHint( const char *script, long delTime )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		player->AddHelpHint( script, delTime );
	}
}

void Script::InterfaceHint( const char *script, long delTime, const char *interfaceName,
				   long x, long y, long w, long h )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		player->AddInterfaceHint( script, delTime, interfaceName, x, y, w, h );
	}
}

int Script::SendExpAction( long actionID, long srcType, const CGUID &srcID, 
                          long destType, const CGUID &destID )
{
	CMoveShape *shape = dynamic_cast<CMoveShape*>( m_RunEnv.region->FindChildObject(
		(OBJECT_TYPE) srcType, srcID ) );
	if( shape == NULL )
	{
		return 0;
	}
	CMessage msg( MSG_S2C_OTHER_EXPATC );
	msg.Add( actionID );
	msg.Add( srcType );
	msg.Add( srcID );
	msg.Add( destType );
	msg.Add( destID );
	msg.SendToAround( shape );
	return 1;
}
