///
/// @file ScriptSkillFunc.cpp
/// @brief ²Î¼û ScriptSkillFunc.h
///
#include "stdafx.h"
#include "../Script.h"
#include "../ScriptSys.h"
#include "../../MoveShape.h"
#include "../../Player.h"
#include "../../PKSys.h"
#include "../../Monster.h"
#include "../../Goods/CGoodsFactory.h"
#include "../../StateAI/AIEventSender.h"
#include "../../../../../Public/Setup/GoodsSetup.h"

#define CAST_SHAPE( v ) (v) == DEF_SHAPE ? m_RunEnv.srcShape : \
	reinterpret_cast<CMoveShape*>( v )


bool Script::AddState( long ID, long level, ulong shape )
{
	if( shape == DEF_SHAPE && m_RunEnv.srcShape == NULL )
	{
		return false;
	}
	CMoveShape *destShape = CAST_SHAPE( shape );
	if( destShape == NULL )
	{
		return false;
	}
	destShape->AddNewState( m_RunEnv.srcShape, ID, level, 20 );
	return true;
}

void Script::RemoveState( long ID, long level, long count, ulong shape )
{
	if( shape == DEF_SHAPE && m_RunEnv.srcShape == NULL )
	{
		return ;
	}
	CMoveShape *destShape = CAST_SHAPE( shape );
	if( destShape == NULL )
	{
		return ;
	}
	destShape->DelNewState( ID, level, count );
}

bool Script::AddSkill( long ID, long level )
{
    CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
    if( player == NULL )
    {
        return false;
    }
    if ( GetInst(SkillAttribute).IsExist(ID,level) )
    {
        player->AddSkill( ID, level );
        player->OnBeginSkill( ID, level);				
    }
    return true;
}

void Script::DelSkill( long ID, long level )
{
	if( m_RunEnv.srcShape != NULL )
	{
		m_RunEnv.srcShape->DelSkill( ID, level );
	}
}

long Script::GetSkillLevel( ulong skillPtr )
{
	stModuParam *skill = reinterpret_cast<stModuParam*>( skillPtr );
	if( skill != NULL )
	{
		return skill->GetLv();
	}
	return 0;
}

bool Script::CheckStudySkill( long ID )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player == NULL )
	{
		return false;
	}
	ulong level = 0;
	return player->CheckStudySkill( ID, level );
}

long Script::GetAllStateNum( const CGUID &shapeID, long shapeType )
{
	CMoveShape *shape = NULL;
	if( shapeID == NULL_GUID )
	{
		shape = m_RunEnv.srcShape;
	}
	else
	{
		shape = GetGame()->FindShape( (OBJECT_TYPE) shapeType, shapeID );
	}
	if( shape != NULL )
	{
		return shape->GetAllStateNum();
	}
	return 0;
}

void Script::DelDeBuff( ulong shapePtr )
{
	CMoveShape *shape = reinterpret_cast<CMoveShape*>( shapePtr );
	if( shape != NULL )
	{
		shape->EndDeBuff();
	}
}

void Script::DelAllBuff( ulong shapePtr )
{
	CMoveShape *shape = reinterpret_cast<CMoveShape*>( shapePtr );
	if( shape != NULL )
	{
		shape->EndBuffAndDebuff();
	}
}

static void SetMoveShapeAttr( ulong shapePtr, const char *attrName, double val )
{
	CMoveShape *shape = reinterpret_cast<CMoveShape*>( shapePtr );
	if( shape != NULL )
	{
		std::string name = attrName;
		if( shape->GetType() == TYPE_PLAYER )
		{
			CPlayer *player = static_cast<CPlayer*>( shape );
			long occLv = player->GetOccupation();
			char str[512]="";
			if( name == "OccuLvl" )
			{
				_snprintf( str, sizeof( str ), "OccuLvl%d", occLv );
			}
			else if( name == "OccuExp" )
			{
				_snprintf( str, sizeof( str ), "OccuExp%d", occLv );
			}
			else if( name == "OccuSP" )
			{
				_snprintf( str, sizeof( str ), "OccuSP%d", occLv );
			}
            else 
            {
                strcpy_s( str , 512 , attrName );
            }
			name = str;
		}

		shape->SetAttribute( name, (long)val );
		shape->UpdateAttribute();
	}
}

static double GetMoveShapeAttr( ulong shapePtr, const char *attrName )
{
	CMoveShape *shape = reinterpret_cast<CMoveShape*>( shapePtr );
	if( shape != NULL )
	{
		std::string name = attrName;
		if( shape->GetType() == TYPE_PLAYER )
		{
			CPlayer *player = static_cast<CPlayer*>( shape );
			long occLv = player->GetOccupation();
			char str[512]="";
			if( name == "OccuLvl" )
			{
				_snprintf( str, sizeof( str ), "OccuLvl%d", occLv );
			}
			else if( name == "OccuExp" )
			{
				_snprintf( str, sizeof( str ), "OccuExp%d", occLv );
			}
			else if( name == "OccuSP" )
			{
				_snprintf( str, sizeof( str ), "OccuSP%d", occLv );
			}
            else 
            {
                strcpy_s( str , 512 , attrName );
            }
			name = str;
		}
		return shape->GetAttribute( name );
	}

	return -1;
}

static void ChangeMoveShapeAttr( ulong shapePtr, const char *attrName, double val )
{
	CMoveShape *shape = reinterpret_cast<CMoveShape*>( shapePtr );
	if( shape != NULL )
	{
		std::string name = attrName;
		if( shape->GetType() == TYPE_PLAYER )
		{
			CPlayer *player = static_cast<CPlayer*>( shape );
			long occLv = player->GetOccupation();
			char str[512];
			if( name == "OccuLvl" )
			{
				_snprintf( str, sizeof( str ), "OccuLvl%d", occLv );
			}
			else if( name == "OccuExp" )
			{
				_snprintf( str, sizeof( str ), "OccuExp%d", occLv );
			}
			else if( name == "OccuSP" )
			{
				_snprintf( str, sizeof( str ), "OccuSP%d", occLv );
			}
			name = str;
		}

		shape->ChangeAttribute( name, (long)val );
		shape->UpdateAttribute();
	}
}

void Script::SetMoveShapeAttrEx( ulong shapePtr, const char *attrName, double attrVal )
{
	SetMoveShapeAttr( shapePtr, attrName, attrVal );
}

double Script::GetMoveShapeAttrEx( ulong shapePtr, const char *attrName )
{
	return GetMoveShapeAttr( shapePtr, attrName );
}

void Script::ChangeMoveShapeAttrEx( ulong shapePtr, const char *attrName, double attrVal )
{
	ChangeMoveShapeAttr( shapePtr, attrName, attrVal );
}

void Script::AddDeBuffToObj( ulong srcPtr, ulong destPtr )
{
	CMoveShape *src = reinterpret_cast<CMoveShape*>( srcPtr );
	CMoveShape *dest = reinterpret_cast<CMoveShape*>( destPtr );
	if( src != NULL && dest != NULL )
	{
		AI_EVENT_SENDER( dest->GetAI() ).WhenBeenHurted( src->GetType(), 
			src->GetExID(), 0 );
	}
}

void Script::ReliveSkill( ulong skillPtr )
{
	stModuParam *skill = reinterpret_cast<stModuParam*>( skillPtr );
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.destShape );
	if( player != NULL && skill != NULL )
	{
        /*
            CMessage msg( MSG_S2C_PLAYER_SKILL_RELIVE );
            msg.Add( (long)0 );
            msg.SendToPlayer( player->GetExID() );
		    long recoverRatio = skill->GetScriptParam( 6 );
		    if( recoverRatio > 0 && recoverRatio <= 100 )
		    {
			    player->SetReliveSkill( true );

			    CMessage msg( MSG_S2C_PLAYER_SKILL_RELIVE );
			    msg.Add( recoverRatio );
			    msg.SendToPlayer( player->GetExID() );
		    }
        */
	}
}

long Script::GetPkResult( ulong srcPtr, ulong destPtr, long addSub )
{
	CMoveShape *src = reinterpret_cast<CMoveShape*>( srcPtr );
	CMoveShape *dest = reinterpret_cast<CMoveShape*>( destPtr );
	if( src != NULL && dest != NULL )
	{
		if( GetPKSys()->JudgeAttackRelation( src, dest, addSub ) )
		{
			return 1;
		}
	}
	return 0;
}

long Script::GetObjectType( ulong shapePtr )
{
	CMoveShape *shape = reinterpret_cast<CMoveShape*>( shapePtr );
	if( shape != NULL )
	{
		return shape->GetType();
	}
	return 0;
}

double Script::GetDistance( ulong srcPtr, ulong destPtr )
{
	CMoveShape *src = reinterpret_cast<CMoveShape*>( srcPtr );
	CMoveShape *dest = reinterpret_cast<CMoveShape*>( destPtr );
	if( src != NULL && dest != NULL )
	{
		return src->RealDistance( dest );
	}
	return 0;
}

void Script::SendSkillList( const char *str )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL && m_RunEnv.destShape != NULL && str != NULL )
	{
		player->SendBackSkill( str, m_RunEnv.destShape->GetExID() );
	}
}

float Script::GetEnergyRatio( long param, uchar magic )
{
	if( m_RunEnv.srcShape != NULL )
	{
		return m_RunEnv.srcShape->GetReinforceByEnum( param, magic );
	}
	return 0.0f;
}

void Script::DeductEnergy( uchar magic )
{
	if( m_RunEnv.srcShape != NULL )
	{
		m_RunEnv.srcShape->ExpendPower( magic );
	}
}

void Script::SetTarget( ulong srcPtr, ulong destPtr )
{
	CMoveShape *src = reinterpret_cast<CMoveShape*>( srcPtr );
	CMoveShape *dest = reinterpret_cast<CMoveShape*>( destPtr );
	if( src != NULL && dest != NULL )
	{
		dest->GetAI()->SetTarget( src->GetExID(), src->GetType() );
	}
}

long Script::GetShieldNum( ulong destPtr )
{
	/*CMoveShape *shape = reinterpret_cast<CMoveShape*>( destPtr );
	if( shape != NULL )
	{
		CGoods *equip = shape->FindGoods( CGoodsBaseProperties::GT_EQUIPMENT, 
			EC_WEAPON2 );
		if( equip != NULL )
		{
			return equip->GetAmount();
		}
	}*/
	return 0;
}

long Script::GetSkillParam( ulong skillPtr, long pos )
{
	stModuParam *skill = reinterpret_cast<stModuParam*>( skillPtr );
	if( skill != NULL )
	{
        //return skill->GetScriptParam( pos );
		return 0;
	}
	return 0;
}

long Script::GetSkillElement( ulong skillPtr )
{
	stModuParam *skill = reinterpret_cast<stModuParam*>( skillPtr );
	if( skill != NULL )
	{
		return skill->GetElemnet();
	}
	return 0;
}

long Script::GetSkillUseType( ulong skillPtr )
{
	stModuParam *skill = reinterpret_cast<stModuParam*>( skillPtr );
	if( skill != NULL )
	{
		return skill->GetUseType();
	}
	return 0;
}

long Script::GetSkillModType( ulong skillPtr )
{
	stModuParam *skill = reinterpret_cast<stModuParam*>( skillPtr );
	if( skill != NULL )
	{
		return skill->GetModType();
	}
	return 0;
}

long Script::GetStateParamByID( long ID, const char *param, const CGUID &shapeID, long shapeType )
{
	CMoveShape *shape = NULL;
	if( shapeID == NULL_GUID )
	{
		shape = m_RunEnv.srcShape;
	}
	else
	{
		shape = GetGame()->FindShape( (OBJECT_TYPE) shapeType, shapeID );
	}

	if( shape != NULL )
	{
		return shape->GetStateParamByID( ID, param );
	}
	return 0;
}

long Script::GetStateParamByIndex( long ID, const char *param, const CGUID &shapeID, long shapeType )
{
	CMoveShape *shape = NULL;
	if( shapeID == NULL_GUID )
	{
		shape = m_RunEnv.srcShape;
	}
	else
	{
		shape = GetGame()->FindShape( (OBJECT_TYPE) shapeType, shapeID );
	}

	if( shape != NULL )
	{
		return shape->GetStateParamByIndex( ID, param );
	}
	return 0;
}

bool Script::CheckStateFirer( ulong skillPtr )
{
	stModuParam *skill = reinterpret_cast<stModuParam*>( skillPtr );
	if( skill != NULL )
	{
		int type = skill->GetFirerType();
		const CGUID &firerID = skill->GetFirerID();
		CMoveShape *shape = GetGame()->FindShape( (OBJECT_TYPE) type, firerID );
		return shape != NULL;
	}
	return false;
}

long Script::GetSkillLv( long ID )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		return player->GetSkillLv( ID );
	}
	return 0;
}

void Script::ReliveInScript( ulong shapePtr, long type )
{
	CMoveShape *shape = reinterpret_cast<CMoveShape*>( shapePtr );
	if( shape != NULL )
	{
		shape->OnRelive( type );
	}
}

CGUID Script::GetGUIDByParam( ulong shapePtr )
{
	CMoveShape *shape = reinterpret_cast<CMoveShape*>( shapePtr );
	if( shape != NULL )
	{
		return shape->GetExID();
	}
	return NULL_GUID;
}

void Script::RunScriptByDest( ulong shapePtr, const char *script )
{
	CMoveShape *shape = reinterpret_cast<CMoveShape*>( shapePtr );
	if( shape != NULL )
	{
		RunEnv env( shape );
		GetInst( ScriptSys ).RunScript( env, script );
	}
}

bool Script::BeginUseSkill( long ID, long level )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player == NULL )
	{
		return false;
	}
	std::list<long> skills = CNewSkillFactory::GetInitOccuSkill( player->GetOccupation() );
	for( std::list<long>::iterator it = skills.begin(); it != skills.end(); ++ it )
	{
		ID = *it;
		CNewSkill *skillProperty = CNewSkillFactory::QueryNewSkillProperty( ID );
		if( skillProperty != NULL )
		{
			CNewSkillProperties::tagSkill *skill = skillProperty->GetNewSkill( level );
			if( GetInst(SkillAttribute).IsExist(ID , level) )
			{
				player->AddSkill( ID, level );
			}
		}
	}
	return true;
}

void Script::SetEventDestObj( ulong skillPtr )
{
	stModuParam *skill = reinterpret_cast<stModuParam*>( skillPtr );
	if( skill != NULL )
	{
		tagProcVar *tgProc = skill->GetEventProc();
		if( tgProc && tgProc->pVar )
		{
			CMoveShape *shape = (CMoveShape*)( tgProc->pVar );	
			if( shape != NULL )
			{
				skill->SetDesObjs( shape->GetTileX(), shape->GetTileY(), shape->GetType(), shape->GetExID() );
				skill->SetDestObjByEvent( shape );
			}
		}
	}
}

int Script::GetBaseModuleLv( ulong skillPtr )
{
	stModuParam *skill = reinterpret_cast<stModuParam*>( skillPtr );
	if( skill != NULL )
	{
		return skill->GetLv();
	}
	return 0;
}

long Script::GetGoodsAddonProperty( ulong skillPtr, int addonID, int pos )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	stModuParam *skill = reinterpret_cast<stModuParam*>( skillPtr );
	if( player == NULL || skill == NULL )
	{
		return 0;
	}

	long goodsIndex = skill->GetGoodsIndex();
	CGoodsBaseProperties *property = GoodsSetup::QueryGoodsBaseProperties( goodsIndex );
	if( property == NULL )
	{
		return 0;
	}
	return property->GetAddonPropertyValue(
		static_cast<eGoodsAddonProperties>( addonID ), pos );
}

void Script::GetGoodsPlacePos( ulong skillPtr, long *place, long *pos )
{
	*place = 0;
	*pos = 0;
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	stModuParam *skill = reinterpret_cast<stModuParam*>( skillPtr );
	if( player == NULL || skill == NULL )
	{
		return;
	}
	*place = skill->GetContainerID();
	*pos = skill->GetPos();
}

void Script::UseGoodsByPlacePos( long place, long pos )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		player->UseItem( place, pos );
	}
}

void Script::SuccedUseItem( ulong skillPtr )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	stModuParam *skill = reinterpret_cast<stModuParam*>( skillPtr );
	if( player == NULL || skill == NULL )
	{
		return;
	}

	long goodsIndex = skill->GetGoodsIndex();
	if( player->UpdateCooldown( goodsIndex ) )
	{
		player->SendUseItemSucceed( goodsIndex );
	}
}

long Script::GetActModify( ulong skillPtr )
{
	CMonster *monster = dynamic_cast<CMonster*>( m_RunEnv.srcShape );
	stModuParam *skill = reinterpret_cast<stModuParam*>( skillPtr );
	if( monster == NULL || skill == NULL )
	{
		return 0;
	}
	return monster->GetActModify( skill->GetID() );

}

void Script::SendUnlockGoodsMsg( long place, long pos )
{
	CPlayer *player = dynamic_cast<CPlayer*>( m_RunEnv.srcShape );
	if( player != NULL )
	{
		CMessage msg( MSG_S2C_SKILL_UNLOCK_GOODS );
		msg.Add( place );
		msg.Add( pos );
		msg.SendToPlayer( player->GetExID() );
	}
}

CGUID Script::GetBeneficiaryInfo( ulong shapePtr, long *type )
{
	CMonster *monster = reinterpret_cast<CMonster*>( shapePtr );
	CPlayer *benShape = monster->GetBeneficiary();
	if( benShape != NULL )
	{
		*type = benShape->GetType();
		return benShape->GetExID();
	}
	else if( m_RunEnv.srcShape != NULL )
	{
		*type = m_RunEnv.srcShape->GetType();
		return m_RunEnv.srcShape->GetExID();
	}
	*type = 0;
	return NULL_GUID;
}

CGUID Script::GetKillerInfo( long *type )
{
	if( m_RunEnv.srcShape == NULL )
	{
		*type = 0;
		return NULL_GUID;
	}
	CGUID killerID;
	m_RunEnv.srcShape->GetKillerInfo( killerID, *type );
	return killerID;
}

CGUID Script::GetAttackerInfo( long *type )
{
	if( m_RunEnv.srcShape == NULL )
	{
		*type = 0;
		return NULL_GUID;
	}
	*type = m_RunEnv.srcShape->GetAttackerType();
	return m_RunEnv.srcShape->GetAttackerGuid();
}

long  Script::GetSkillValue(long skillid,long lvl,std::string name)
{
    if( m_RunEnv.srcShape == NULL )
    {
        return 0;
    }
    return  m_RunEnv.srcShape->GetSkillValue(skillid,lvl,name);
}

long  Script::GetSkillAttriValue( ulong skillPtr,std::string name)
{
    stModuParam *skill = reinterpret_cast<stModuParam*>( skillPtr );
    if( m_RunEnv.srcShape == NULL || skill == NULL )
    {
        return 0;
    }
    return m_RunEnv.srcShape->GetSkillValue(skill->nID,skill->nLvl,name);//monster->GetActModify( skill->GetID() );
}

void  Script::ClearSkillAttackType(long  SkillType,long Count)
{
    if( Count <= 0 )  return ;
    if( m_RunEnv.srcShape == NULL )
    {
        return ;
    }
    m_RunEnv.srcShape->ClearState( (eSkillAttackType)SkillType , Count );
}