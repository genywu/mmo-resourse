#include "StdAfx.h"
#include "timesystem.h"

IMPLEMENT_SINGLETON(TimeSystem);

TimeSystem::TimeSystem(void)
{
	m_fTimeRatio = 10.0f;
	m_dwTimeLast = 0;
}

TimeSystem::~TimeSystem(void)
{
}

void TimeSystem::Update()
{
	DWORD dwCurTime = timeGetTime();
	DWORD dwElapseTime = dwCurTime - m_dwTimeLast;

	float fElapse = (float)dwElapseTime * m_fTimeRatio;
	m_fSecond += fElapse;

	m_dwTimeLast = dwCurTime;
}