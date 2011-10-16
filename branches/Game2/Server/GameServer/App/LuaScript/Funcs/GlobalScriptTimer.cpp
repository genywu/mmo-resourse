///
/// @file GlobalScriptTimer.cpp
/// @brief 脚本内不直接使用
/// @author wangqiao
///
#include "StdAfx.h"
#include "GlobalScriptTimer.h"
#include "../ScriptSys.h"

CGlobalScriptTimer* CGlobalScriptTimer::instance=NULL;

CGlobalScriptTimer::CGlobalScriptTimer(void)
{
}

CGlobalScriptTimer::~CGlobalScriptTimer(void)
{
}

//得到实例
CGlobalScriptTimer* CGlobalScriptTimer::GetInstance()
{
	if(instance == NULL)
	{
		instance = OBJ_CREATE(CGlobalScriptTimer);
	}
	return instance;
}

//释放资源
void CGlobalScriptTimer::Release()
{
	if(instance)
		instance->ClearScriptTimer();
	OBJ_RELEASE(CGlobalScriptTimer, instance);
}


//定制一个脚本定时器
long CGlobalScriptTimer::RegisterTimer(long lFutureTime,long lInterTime,
						const char* pszScriptName,long lUserData)
{
	long lTimerID = -1;
	if(NULL == pszScriptName)	return lTimerID;

	tagScriptTimer scripttimer = {lUserData,pszScriptName,lInterTime>0?true:false};
	lTimerID = TIMER_SCHEDULE(this,(void*)TST_Timer,timeGetTime()+lFutureTime,lInterTime);
	if(lTimerID != -1)
	{
		m_ScriptTimers[lTimerID]=scripttimer;
	}
	else
	{
		Log4c::Warn(ROOT_MODULE,"Register the Global Script Timer error!");
	}
	return lTimerID;
}
//注销一个脚本定时器
long CGlobalScriptTimer::UnRegisterTimer(long lUserData,long lTimerID)
{
	long lTimerNum = 0;
	itST it = m_ScriptTimers.begin();
	for(;it != m_ScriptTimers.end();)
	{
		tagScriptTimer &scripttimer = (*it).second;
		if( scripttimer.lUserData == lUserData &&
			(lTimerID == -1 ||  (*it).first == lTimerID) )
		{
			TIMER_CANCEL((*it).first);
			it = m_ScriptTimers.erase(it);
			lTimerNum++;
		}
		else
		{
			it++;
		}
	}
	return lTimerNum;
}

//定制执行wait函数的脚本
void CGlobalScriptTimer::RegisterWaitScript(const CGUID& guScriptID,long lFutureTime)
{
	long lTimerID = TIMER_SCHEDULE(this,(void*)TST_Wait,timeGetTime()+lFutureTime,0);
	if(lTimerID != -1)
	{
		m_WaitScripts[lTimerID]=guScriptID;
	}
	else
	{
		Log4c::Warn(ROOT_MODULE,"Register the wait Script Timer error!!");
	}
}

//清除所有脚本定时器
void CGlobalScriptTimer::ClearScriptTimer()
{
	itST it = m_ScriptTimers.begin();
	for(;it != m_ScriptTimers.end();it++)
	{
		TIMER_CANCEL((*it).first);
	}
	m_ScriptTimers.clear();

	itWS itW = m_WaitScripts.begin();
	for(;itW != m_WaitScripts.end();itW++)
	{
		TIMER_CANCEL((*itW).first);
	}
	m_WaitScripts.clear();
}

//超时响应
long CGlobalScriptTimer::OnTimeOut(DWORD timerid,DWORD curtime,const void* var)
{
	long eSType = (long)var;
	switch(eSType)
	{
	case TST_Timer:
		{
			itST it = m_ScriptTimers.find(timerid);
			if(it != m_ScriptTimers.end())
			{
				//执行脚本
				const tagScriptTimer &scriptTimer = (*it).second;

				Script::RunEnv env;
				GetInst( ScriptSys ).RunScript( env, scriptTimer.strScript.c_str() );
				if( !scriptTimer.bLoop )
				{
					m_ScriptTimers.erase( it );
				}
			}
		}
		break;
	case TST_Wait:
		{
			itWS it = m_WaitScripts.find(timerid);
			if(it != m_WaitScripts.end())
			{
				const CGUID &scriptID = (*it).second;
				GetInst( ScriptSys ).ResumeScript( scriptID );
				m_WaitScripts.erase(it);
			}
		}
		break;
	default:
		Log4c::Warn(ROOT_MODULE,"the scripttimer timeout,the param 'var' error!,");
		break;
	};
	return 0;
}