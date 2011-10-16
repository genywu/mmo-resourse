#include "stdafx.h"
#include "AttackPower.h"

tagAttackPower::tagAttackPower()
{
	ZeroMemory( this, sizeof(tagAttackPower) );
}

tagAttackInformation::tagAttackInformation()
{
	tSkillID				= SKILL_UNKNOW;
	bySkillLevel			= 1;
	lAttackerType			= 0;
	

	lTargetDodgeModifier	= 0;
	lHitModifier			= 0;
	fDamageFactor			= 1.0;
	lDamageModifier			= 0;
	bCritical				= false;
	bBlastAttack			= false;
	bAllowKillPlayer		= false;
	bAllowKillTeammate		= false;
	bAllowKillGuildMember	= false;
	bAllowKillCriminal		= false;
	bAllowKillCountry		= false;
	bFullMiss				= false;

}

tagAttackInformation::~tagAttackInformation()
{
	Clear();
}

VOID tagAttackInformation::Clear()
{
	tSkillID				= SKILL_UNKNOW;
	lAttackerType			= 0;

	lTargetDodgeModifier	= 0;
	lHitModifier			= 0;
	fDamageFactor			= 1.0;
	lDamageModifier			= 0;
	bCritical				= false;
	bBlastAttack			= false;
	bAllowKillPlayer		= false;
	bAllowKillTeammate		= false;
	bAllowKillGuildMember	= false;
	bAllowKillCriminal		= false;
	bFullMiss				= false;


	for( size_t i = 0; i < vDamage.size(); i ++ )
	{
		SAFE_DELETE( vDamage[i] );
	}
	vDamage.clear();
}

