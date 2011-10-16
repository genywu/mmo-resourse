

#include "StdAfx.h"
#include "..\Monster.h"
#include "AIFactory.h"
#include "PetAI.h"
#include "NpcNormal.h"
#include "Gladiator.h"
#include "StupidGladiator.h"
#include "PassiveGladiator.h"
#include "FixedPositionArcher.h"
#include "SnipeArcher.h"
#include "Guard.h"
#include "Magic.h"
#include "ModelFairy.h"
#include "ModelWolf.h"
#include "ScriptAI.h"
#include "DHFGladiator.h"
#include "DHFPassiveGladiator.h"

#define REGISTER_AI( id, type ) \
	static CAIFactory::RegisterHelper<type> _##type##_register( id )


CAIFactory::AIFuncTable CAIFactory::mAIFuncTable;

REGISTER_AI( CAIFactory::NPC_NORMAL, CNpcNormal );
REGISTER_AI( CAIFactory::GLADIATOR, CGladiator );
REGISTER_AI( CAIFactory::PASSIVE_GLADIATOR, CPassiveGladiator );
REGISTER_AI( CAIFactory::STUPID_GLADIATOR, CStupidGladiator );
REGISTER_AI( CAIFactory::FIXED_POSITION_ARCHER, CFixedPositionArcher );
REGISTER_AI( CAIFactory::SNIPE_ARCHER, CSnipeArcher );
REGISTER_AI( CAIFactory::MODEL_WOLF, CModelWolf );
REGISTER_AI( CAIFactory::GUARD, Guard );
REGISTER_AI( CAIFactory::DHF_GLADIATOR, CDHFGladiator );
REGISTER_AI( CAIFactory::DHF_PASSIVE_GLADIATOR, CDHFPassiveGladiator );
REGISTER_AI( CAIFactory::AI_TYPE_PET, CPetAI );
REGISTER_AI( CAIFactory::SCRIPT_AI, CScriptAI );

CBaseAI *CAIFactory::CreateAI( long ai_type )
{
	AIFuncTable::iterator it = mAIFuncTable.find( ai_type );
	if( it != mAIFuncTable.end() )
	{
		CBaseAI *pAI = it->second.Create();
		pAI->SetAIType( ai_type );
		if( ai_type >= SCRIPT_AI )
		{
			CScriptAI *pScriptAI = static_cast<CScriptAI*>( pAI );
			pScriptAI->BuildFuncTable();
		}

		return pAI;
	}
	else
	{
		return NULL;
	}
}

CBaseAI *CAIFactory::CreateAI( CMonster *pMonster )
{
	CBaseAI *pAI = CreateAI( pMonster->GetAIType() );
	if( pAI != NULL )
	{
		pAI->SetAIType( pMonster->GetAIType() );
		pAI->SetOwner( pMonster );
	}
	return pAI;
}

bool CAIFactory::DeleteAI( CBaseAI *pBaseAI )
{
	AIFuncTable::iterator it = mAIFuncTable.find( pBaseAI->GetAIType() );
	if( it != mAIFuncTable.end() )
	{
		it->second.Delete( pBaseAI );
		return true;
	}
	else
	{
		return false;
	}
}

void CAIFactory::Clear()
{
	for( AIFuncTable::iterator it = mAIFuncTable.begin(); it != mAIFuncTable.end(); ++ it )
	{
		it->second.Clear();
	}
}