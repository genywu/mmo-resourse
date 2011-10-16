#include "stdafx.h"
#include <mmsystem.h>
#include "SummonedCreature.h"
#include "ServerRegion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSummonedCreature::CSummonedCreature()
{
	m_dwSummonedTimeStamp	= 0;
	m_dwLifeTime			= 0;
}

CSummonedCreature::~CSummonedCreature()
{
}

VOID CSummonedCreature::AI()
{
	//##如果死亡则直接返回.
	if( IsDied() )
	{
		Evanish();
		return;
	}

	//##考虑一下，可能消失还是和上层AI有关联。
	if( m_dwSummonedTimeStamp )
	{
		if( timeGetTime() > m_dwSummonedTimeStamp + m_dwLifeTime )
		{
			//##消失
			Evanish();
			return;
		}
	}
	//##调用Monster的AI
	CMonster::AI();
}

//##设置生命时长
VOID CSummonedCreature::SetLifeTime( DWORD dwLifeTime )
{
	m_dwSummonedTimeStamp	= timeGetTime();
	m_dwLifeTime			= dwLifeTime;
}

//##获取剩余的生命时间
DWORD CSummonedCreature::GetRemainedTime()
{
	if( m_dwSummonedTimeStamp )
	{
		if( timeGetTime() >= m_dwSummonedTimeStamp + m_dwLifeTime )
			return 0;
		else 
			return m_dwSummonedTimeStamp + m_dwLifeTime - timeGetTime();
	}
	else 
	{
		return 0xffffffff;
	}
}
