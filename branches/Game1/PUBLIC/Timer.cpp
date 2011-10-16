////////////////////////////////////////////////////////////////////////////////
//Timer.cpp/////////////////////////////////////////////////////////////////////
//时间定时器类:
//function:1,完成某个时间点的定时事件处理2,作一个计时器处理
//Author:Joe
//Create Time:2004.7.28
#include "StdAfx.h"
#include ".\timer.h"
#include "tools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CTimer* CTimer::instance = NULL;

CTimer::CTimer(void)
{
}

CTimer::~CTimer(void)
{
}

CTimer* CTimer::getInstance()
{
	if(instance==NULL)
	{
		instance = new CTimer;
	}
	return instance;
}

//释放资源
void	CTimer::Release()
{
	SAFE_DELETE(instance);
}

bool	CTimer::SetTimerParam(ULONG uID,LPARAM lparam)
{
	m_Timers.Lock();
	itTimer it = m_Timers.find(uID);
	if( it != m_Timers.end() )
	{
		(*it).second.lparam=lparam;
		m_Timers.UnLock();
		return true;
	}
	m_Timers.UnLock();
	return false;
}
bool	CTimer::SetTimerCallBackFun(ULONG uID,CallBackFun pCallBackFun)
{
	m_Timers.Lock();
	itTimer it = m_Timers.find(uID);
	if( it != m_Timers.end() )
	{
		(*it).second.pCallBack = pCallBackFun;
		m_Timers.UnLock();
		return true;
	}
	m_Timers.UnLock();
	return false;
}

//设置一个时间事件	
//返回值,0:设置失败
ULONG	CTimer::SetTimeEvent(tagTime& Time,CallBackFun pCallBackFun,LPARAM  lparam)
{
	ULONG uID = GetULONGID();
	tagTimeEvent TimeEvent = {uID,Time,lparam,pCallBackFun};

	m_TimeEvents.Lock();
	m_TimeEvents[uID]=TimeEvent;
	m_TimeEvents.UnLock();
	return uID;
}

bool	CTimer::KillTimeEvent(ULONG uID)
{
	m_TimeEvents.Lock();
	itTimeEvent it = m_TimeEvents.find(uID);
	if(it != m_TimeEvents.end())
	{
		m_TimeEvents.erase(it);
		m_TimeEvents.UnLock();
		return true;
	}
	m_TimeEvents.UnLock();
	return false;
}


bool	CTimer::SetTimeEventParam(ULONG uID,LPARAM lparam)
{
	m_TimeEvents.Lock();
	itTimeEvent it = m_TimeEvents.find(uID);
	if(it != m_TimeEvents.end())
	{
		(*it).second.lparam=lparam;
		m_TimeEvents.UnLock();
		return true;
	}
	m_TimeEvents.UnLock();
	return false;
}
bool	CTimer::SetTimeEventCallBackFun(ULONG uID,CallBackFun pCallBackFun)
{
	m_TimeEvents.Lock();
	itTimeEvent it = m_TimeEvents.find(uID);
	if(it != m_TimeEvents.end())
	{
		(*it).second.pCallBack=pCallBackFun;
		m_TimeEvents.UnLock();
		return true;
	}
	m_TimeEvents.UnLock();
	return false;
}

void	CTimer::Run()
{
	itTimer itTimer = m_Timers.begin();
	for(;itTimer != m_Timers.end();itTimer++)
	{
		tagTimer& Timer = (*itTimer).second;
		if(Timer.bOpen)
		{
			if(Timer.bUseAITick)
			{
			}
			else
			{
				DWORD time = timeGetTime();
				if(time-Timer.uStartTime >= Timer.uElapseTime)
				{
					Timer.uStartTime=time;
					Timer.pCallBack(Timer.lparam);
				}
			}
		}
	}

	itTimeEvent itEvent = m_TimeEvents.begin();
	for(;itEvent != m_TimeEvents.end();)
	{
		tagTimeEvent& TimeEvent = (*itEvent).second;
		tagTime Time;
		if(Time >= TimeEvent.Time)
		{
			TimeEvent.pCallBack(TimeEvent.lparam);
			m_TimeEvents.erase(itEvent++);
		}
		else
		{
			++itEvent;
		}
	}
}

CTimer* GetTimer()
{
	return CTimer::getInstance();
}
