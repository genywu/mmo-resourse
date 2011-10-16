///
/// @file ScriptShapeFunc.cpp
/// @brief ²Î¼û ScriptShapeFunc.h
///
#include "stdafx.h"
#include "../Script.h"
#include "../ScriptSys.h"
#include "../../Player.h"
#include "../../Monster.h"

void Script::SetAttr( long type, const CGUID &id, const char *attrName, double attrVal )
{
	if( type == TYPE_MONSTER )
	{
		// TODO
	}
	else if( type == TYPE_PLAYER )
	{
		SetPlayerAttr( attrName, attrVal, id );
	}
	else
	{
		CMoveShape *shape = FindShape( type, id );
		if( shape != NULL )
		{
			std::string name( attrName );
			shape->SetAttribute( name, (long) attrVal );
			shape->UpdateAttribute();
		}
	}
}

double Script::GetAttr( long type, const CGUID &id, const char *attrName )
{
	if( type == TYPE_MONSTER )
	{
		// TODO
	}
	else if( type == TYPE_PLAYER )
	{
		return GetPlayerAttr( attrName, id );
	}
	else
	{
		CMoveShape *shape = FindShape( type, id );
		if( shape != NULL )
		{
			return shape->GetAttribute( std::string( attrName ) );
		}
	}
	return 0;
}

void Script::ChangeAttr( long type, const CGUID &id, const char *attrName, double attrVal )
{
	if( type == TYPE_MONSTER )
	{
		// TODO
	}
	else if( type == TYPE_PLAYER )
	{
		ChangePlayerAttr( attrName, attrVal, id );
	}
	else
	{
		CMoveShape *shape = FindShape( type, id );
		if( shape != NULL )
		{
			std::string name( attrName );
			shape->ChangeAttribute( name, (long) attrVal );
			shape->UpdateAttribute();
		}
	}
}

void Script::PlayMotion( long motion, long around )
{
	if( m_RunEnv.srcShape != NULL )
	{
		CMessage msg( MSG_S2C_SHAPE_EMOTION );
		msg.Add( m_RunEnv.srcShape->GetType() );
		msg.Add( m_RunEnv.srcShape->GetExID() );
		msg.Add( motion );
		if( around )
		{
			msg.SendToAround( m_RunEnv.srcShape );
		}
		else if( m_RunEnv.srcShape->GetType() == TYPE_PLAYER )
		{
			msg.SendToPlayer( m_RunEnv.srcShape->GetExID() );
		}
	}
}

void Script::ShapeRunScript( long type, const CGUID &id, const char *script )
{
	CMoveShape *shape = FindShape( type, id );
	if( shape != NULL )
	{
		RunEnv env( shape, NULL, (CRegion*) shape->GetFather() );
		GetInst( ScriptSys ).RunScript( env, script );
	}
}

long Script::GetAroundPlayerNum( long range, long diedFlag, long hideFlag )
{
	if( m_RunEnv.srcShape == NULL )
	{
		return 0;
	}
	CServerRegion *region = dynamic_cast<CServerRegion*>( m_RunEnv.srcShape->GetFather() );
	if( region == NULL )
	{
		return 0;
	}
	CMoveShape *self = m_RunEnv.srcShape;
	long ret = 0;
	vector<CShape*> shapeList;
	region->FindAroundObject( m_RunEnv.srcShape, TYPE_PLAYER, shapeList );
	for( size_t i = 0; i < shapeList.size(); ++ i )
	{
		CPlayer *player = dynamic_cast<CPlayer*>( shapeList[i] );
		if( self->Distance( player ) > range ) 
		{
			continue;
		}
		if( !diedFlag && player->IsDied() )
		{
			continue;
		}
		if( !hideFlag && !player->IsVisible( self ) )
		{
			continue;
		}
		++ ret;
	}
	return ret;
}

long Script::GetAroundMonsterNum( long range )
{
	if( m_RunEnv.srcShape == NULL )
	{
		return 0;
	}
	CServerRegion *region = dynamic_cast<CServerRegion*>( m_RunEnv.srcShape->GetFather() );
	if( region == NULL )
	{
		return 0;
	}
	CMoveShape *self = m_RunEnv.srcShape;
	long ret = 0;
	vector<CShape*> shapeList;
	region->FindAroundObject( m_RunEnv.srcShape, TYPE_MONSTER, shapeList );
	for( size_t i = 0; i < shapeList.size(); ++ i )
	{
		if( self->Distance( shapeList[i] ) > range ) 
		{
			continue;
		}
		++ ret;
	}
	return ret;
}

long Script::GetAroundSameMonsterNum( long range )
{
	if( m_RunEnv.srcShape == NULL )
	{
		return 0;
	}
	CServerRegion *region = dynamic_cast<CServerRegion*>( m_RunEnv.srcShape->GetFather() );
	if( region == NULL )
	{
		return 0;
	}
	CMonster *self = dynamic_cast<CMonster*>( m_RunEnv.srcShape );
	if( self == NULL )
	{
		return 0;
	}
	long ret = 0;
	vector<CShape*> shapeList;
	region->FindAroundObject( m_RunEnv.srcShape, TYPE_MONSTER, shapeList );
	for( size_t i = 0; i < shapeList.size(); ++ i )
	{
		CMonster *monster = dynamic_cast<CMonster*>( shapeList[i] );
		if( self->Distance( monster ) > range ) 
		{
			continue;
		}
		if( monster->GetIndex() != self->GetIndex() )
		{
			continue;
		}
		++ ret;
	}
	return ret;
}

void Script::SetDelTime( long type, const CGUID &id, long delTime )
{
	CMoveShape *shape = FindShape( type, id );
	if( shape != NULL )
	{
		shape->AddDelEvent( delTime );
	}
}

void Script::ChangeMoveSpeed( long type, const CGUID &id, short speed )
{
	CMoveShape *shape = FindShape( type, id );
	if( shape != NULL )
	{
		shape->ChangeSpeed( speed );
		shape->SendChangedSpeedToClients();
	}
}

void Script::SetMoveSpeed( long type, const CGUID &id, short speed )
{
	CMoveShape *shape = FindShape( type, id );
	if( shape != NULL )
	{
		shape->SetChangeSpeed( speed );
		shape->SendChangedSpeedToClients();
	}
}

float Script::GetCurrentSpeed( long type, const CGUID &id )
{
	CMoveShape *shape = FindShape( type, id );
	if( shape != NULL )
	{
		return shape->GetSpeed();
	}
	return 0.0f;
}

bool Script::IsShapeValid( long type, const CGUID &id )
{
	CMoveShape *shape = FindShape( type, id );
	return shape != NULL;
}
