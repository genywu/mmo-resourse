#include "stdafx.h"
#include <mmsystem.h>
#include "SummonShape.h"
#include "Effect.h"
#include "../../Public/Common/BaseDef.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSummonShape::CSummonShape()
{
	m_tSkillID		= SKILL_UNKNOW;
	m_dwLifeTime	= 0;
	m_dwTimeStamp	= GetCurTickCount();
	m_lType			= TYPE_SUMMON;
}

CSummonShape::~CSummonShape()
{
	DelAllEffect();
}

DWORD CSummonShape::GetRemainedTime()
{
	if( GetCurTickCount() >= m_dwTimeStamp + m_dwLifeTime )
		return 0;
	else return m_dwTimeStamp + m_dwLifeTime - GetCurTickCount();
}
