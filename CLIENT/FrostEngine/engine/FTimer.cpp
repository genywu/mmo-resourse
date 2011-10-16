
#include "stdafx.h"
#include "futility.h"
#include "fconsole.h"
#include "ftimer.h"

IMPLEMENT_SINGLETON(FTimer);

FTimer::FTimer(void)
{
}

FTimer::~FTimer(void)
{
}

void FTimer::Create(void)
{
	//Not implement yet
	ZeroMemory(m_pCounterArray,sizeof(_tagCounter) * MAX_TIME_COUNTER);
}

void FTimer::Destroy(void)
{

}

DWORD FTimer::FrameRate(void)
{
	static DWORD dwFpsTime = timeGetTime();
	static DWORD dwCount   = 0;
	dwCount ++;
	static DWORD dwFps = 0 ;
	if ( timeGetTime() - dwFpsTime > 2000)
	{
		dwFps = dwCount / 2 ;
		dwCount = 0;		
		dwFpsTime = timeGetTime();
	}
	return dwFps;
}

void FTimer::BeginCount(const char *pszName,DWORD dwCountIndex)
{
	_tagCounter &tCounter = m_pCounterArray[dwCountIndex];
	tCounter.dwBegineTime = timeGetTime();;
	tCounter.bUsed = TRUE;
	strncpy(tCounter.szName,pszName,256);

}

void FTimer::EndCount(DWORD dwCountIndex)
{
	_tagCounter &tCounter = m_pCounterArray[dwCountIndex];
	tCounter.dwEndTime = timeGetTime();
}

void FTimer::OutputCounter()
{
	for(int i = 0; i < MAX_TIME_COUNTER; i++)
	{
		_tagCounter &tCounter = m_pCounterArray[i];
		if (tCounter.bUsed)
          OutputConsole("Name: %s: Useage: %d\n",tCounter.szName,tCounter.dwEndTime - tCounter.dwBegineTime);
	}
}