///
/// @file ScriptPlayerFunc.cpp
/// @brief 参见 ScriptPlayerFunc.h
///
#include "stdafx.h"
#include "../Script.h"
#include "../../Player.h"
#include "../../OrganizingSystem/GameOrganizingCtrl.h"
#include "../../OrganizingSystem/GSTeam.h"
#include "../../../../../Public/Setup/GlobalSetup.h"
#include "../../../../../Public/Setup/GMList.h"

bool Script::SetPlayerAttr( const char *attrName, double attrVal, const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return false;
	}
	if( strcmp( attrName, "E_PkCount" ) == 0 )
	{
		ushort val = attrVal > (ushort)(-1) ? (ushort)(-1) : (ushort) attrVal;
		player->SetPkValue( val );
		player->OnUpdateMurdererSign();
	}
	else
	{
		std::string attr( attrName );
		player->SetAttribute( attr, (long) attrVal ); 

		if( strcmp( attrName, "Exp" ) == 0 )
		{
			player->CheckLevel();
		}
	}
	player->UpdateAttribute();

	return true;
}

double Script::GetPlayerAttr( const char *attrName, const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return 0;
	}

	double val = player->GetAttribute( attrName );
	return val;
}

bool Script::ChangePlayerAttr( const char *attrName, double attrVal, const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return false;
	}
	if( strcmp( attrName, "E_PkCount" ) == 0 )
	{
		double val = attrVal + player->GetPkValue();
		ulong retVal = val > (ushort)(-1) ? (ushort)(-1) : (ulong)val;
		player->SetPkValue( retVal );
		player->OnUpdateMurdererSign();
	}
	else
	{
		std::string attr( attrName );
		player->ChangeAttribute( attr, (long)attrVal );
		if( strcmp( attrName, "Exp" ) == 0 )
		{
			player->CheckLevel();
		}
	}
	player->UpdateAttribute();
	return true;
}

const char *Script::GetPlayerName( const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return "";
	}
	return player->GetName();
}

CGUID Script::GetPlayerID( const char *name )
{
	CPlayer *player = FindPlayer( name );
	if( player == NULL )
	{
		return NULL_GUID;
	}
	return player->GetExID();
}

void Script::ChangePlayerMoney( long money, const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return ;
	}
	long newVal = player->GetMoney() + money;
	player->SetMoney( newVal < 0 ? 0 : newVal, 23 );
}

void Script::SetPlayerMoney( long money, const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return ;
	}
	player->SetMoney( money, 34 );
}

long Script::GetPlayerMoney( const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return 0;
	}
	return player->GetMoney();	
}

void Script::ChangePlayerTicket( long ticket, const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return ;
	}
	long newVal = player->GetTicket() + ticket;
	//if(newVal>GlobalSetup::GetSetup()->dwTicketLimit)
	//	newVal=GlobalSetup::GetSetup()->dwTicketLimit;
	player->SetTicket( newVal < 0 ? 0 : newVal);
}

long Script::GetPlayerTicket( const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return 0;
	}
	return player->GetTicket();	
}

void Script::SetPlayerTicket( long ticket, const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return ;
	}

	//if(ticket>GlobalSetup::GetSetup()->dwTicketLimit)
	//	ticket=GlobalSetup::GetSetup()->dwTicketLimit;
	player->SetTicket( ticket < 0 ? 0 : ticket);
}

void Script::ChangeBaseSpeed( const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return;
	}
	if( player->GetIsRide() )
	{
		if( !player->GetIsRun() )
		{
			player->SetSpeed( GlobalSetup::GetSetup()->fRidePlayerWalkSpeed );
		}
		else
		{
			player->SetSpeed( GlobalSetup::GetSetup()->fRidePlayerRunSpeed );
		}
	}
	else
	{
		if( !player->GetIsRun() )
		{
			player->SetSpeed( GlobalSetup::GetSetup()->fPlayerWalkSpeed );
		}
		else
		{
			player->SetSpeed( GlobalSetup::GetSetup()->fPlayerRunSpeed );
		}
	}
	player->SendChangedSpeedToClients();
}

void Script::SetPlayerPos( long x, long y, const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return;
	}
	player->ProduceEvent( ET_Move );
	player->ProduceEvent( ET_EventBreak );
	player->SetPosition( x, y );
}

void Script::SetPlayerDir( long dir, const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return;
	}
	player->SetDir( dir );
	CMessage Msg( MSG_S2C_SHAPE_CHANGEDIR );	
	Msg.Add( (uchar) dir );
	Msg.Add( player->GetDirEx() );
	Msg.Add( player->GetType() );
	Msg.Add( player->GetExID() );
	Msg.SendToAround( player );
}

long Script::GetCountry( const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return 0;
	}
	return player->GetCountry();
}

void Script::ChangeCountry( long country, const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return ;
	}
	CMessage msg( MSG_S2W_COUNTRY_CHANGE );
	msg.Add( player->GetExID() );
	msg.Add( (uchar) country );
	msg.Send();
}

long Script::GetContribute( const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return 0;
	}
	return player->GetCountryContribute();
}

void Script::SetContribute( long contribute, const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return ;
	}
	player->SetCountryContribute( contribute );
	CMessage msg( MSG_S2C_PLAYER_CHANGE_CONTRIBUTE );
	msg.Add( contribute );
	msg.SendToPlayer( player->GetExID() );
}

bool Script::IsCharged( const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return false;
	}
	return player->IsCharged();	
}

void Script::SetCharged( int c, const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return ;
	}
	player->SetCharged( c != 0 );
}

void Script::CheckOccuLvl( long occu )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		player->CheckOccuLevel( (eOccupation) occu );
	}
}

void Script::Relive( long type, long ratio )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		player->OnRelive( type, ratio );
	}
}

void Script::ChangeOccu( long occu )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		player->ChangeOccu( (eOccupation) occu );
	}
}

void Script::AutoMove( float x, float y )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		CMessage msg( MSG_S2C_PLAYER_AUTOMOVE );
		msg.Add( x );
		msg.Add( y );
		msg.SendToPlayer( player->GetExID() );
	}
}

void Script::MovePlayer( const CGUID &src, long x1, long y1, long x2, long y2,
				long type, const CGUID &dest, long dx1, long dy1, long dx2, long dy2 )
{
	CServerRegion *srcRgn = GetGame()->FindRegion( src );
	CServerRegion *destRgn = GetGame()->FindRegion( dest );

	if( srcRgn == NULL || destRgn == NULL )
	{
		return;
	}
	long dw = dx2 - dx1;
	long dh = dy2 - dy1;
	BEGIN_LOOP_REGION_SHAPE_BY_TYPE( srcRgn, TYPE_PLAYER )
		CPlayer *player = dynamic_cast<CPlayer*>( pShape );
		if( player != NULL )
		{
			long x = player->GetTileX();
			long y = player->GetTileY();
			if( x >= x1 && x <= x2 && y >= y1 && y <= y2 )
			{
				long retX = 0, retY = 0;
				destRgn->GetRandomPosInRange( retX, retY, dx1, dy1, dw, dh );
				player->ChangeRegion( (eRgnType) type, dest, retX, retY, player->GetDir() );
			}
		}
	END_LOOP_REGION_SHAPE_BY_TYPE
}

void Script::GodMode( int mode )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		player->God( mode != 0 );
	}
}

bool Script::IsGM( const char *name )
{
	CPlayer *player = FindPlayer( name );
	if( player == NULL )
	{
		return false;
	}
	if( GMList::GetInfoByName( player->GetName() ) == NULL )
	{
		return false;
	}
	return true;
}

void Script::DurancePlayer( const char *name, int flag )
{
	CPlayer *self = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( self != NULL )
	{
		return;
	}
	CPlayer *player = FindPlayer( name );
	if( player == NULL )
	{
		//self->SendNotifyMessage( "该玩家不再当前服务器", 0xffff0000, 0, eNOTIFYPOS_LEFT_BOTTOM );
		self->SendNotifyMessage( "该玩家不再当前服务器",eNOTIFYPOS_LEFT_BOTTOM );
		return;
	}
	player->SetBoolAttribute( "bUseItem", flag != 0 );
	player->SetBoolAttribute( "bMoveable", flag != 0 );

	if( flag == 0 )
	{
		self->SendNotifyMessage( "玩家被解禁", eNOTIFYPOS_LEFT_BOTTOM );
		player->SendNotifyMessage( "你可以活动了",eNOTIFYPOS_LEFT_BOTTOM );
	}
	else
	{
		self->SendNotifyMessage( "玩家被禁锢",eNOTIFYPOS_LEFT_BOTTOM );
		player->SendNotifyMessage( "你已经被禁锢",eNOTIFYPOS_LEFT_BOTTOM );
	}
}

long Script::GetServer()
{
	return GetGame()->GetIDIndex();
}

CGUID Script::GetOtherOneInMarriageTeam( const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return NULL_GUID;
	}
	GSTeam *team = GetOrganiCtrl()->GetGameTeamByID( player->GetTeamID() );
	if( team != NULL )
	{
		return team->GetMarriageOtherOne( player );
	}
	return NULL_GUID;
}

long Script::GetPlayerMarriageStep( const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return 0;
	}
	return player->GetMarriageStep();
}

void Script::SetPlayerMarriageStep( long step, const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return;
	}
	player->SetMarriageStep( step );
}

void Script::SetPlayerSpouse( const CGUID &src, const CGUID &dest )
{
	CPlayer *destPlayer = FindPlayer( dest );
	if( destPlayer == NULL )
	{
		return;
	}
	CPlayer *srcPlayer = FindPlayer( src );
	if( srcPlayer == NULL )
	{
		return;
	}
	destPlayer->SetSpouse( srcPlayer );
}

void Script::DelPlayerSpouse( const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return;
	}
	player->SetSpouse( NULL );
}

CGUID Script::GetPlayerSpouse( const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return NULL_GUID;
	}
	return player->GetSpouseGuid();
}

const char *Script::GetPlayerSpouseName( const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return "";
	}
	return player->GetSpouseName();
}

long Script::GetElementProperty( long etype, long ptype, const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return 0;
	}
	return player->GetElementProperty( etype, ptype );
}

long Script::SetElementProperty( long etype, long ptype, long val, const CGUID &playerID )
{
	CPlayer *player = FindPlayer( playerID );
	if( player == NULL )
	{
		return 0;
	}

	return player->SetElementProperty( etype, ptype, val );
}

void Script::StudyGenius(long id,long lvl,const CGUID& playerID)
{
    CPlayer *player = FindPlayer( playerID );
    if( player == NULL )
    {
        return ;
    }
    player->StudyGenius(id,lvl);
}

void Script::RemoveGenius(long id,const CGUID& playerID)
{
    CPlayer *player = FindPlayer( playerID );
    if( player == NULL )
    {
        return ;
    }
    player->RemoveGenius(id);
}