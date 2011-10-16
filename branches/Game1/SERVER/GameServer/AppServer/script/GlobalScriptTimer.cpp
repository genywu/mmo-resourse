#include "StdAfx.h"
#include "GlobalScriptTimer.h"

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
		instance = MP_NEW CGlobalScriptTimer;
	}
	return instance;
}

//释放资源
void CGlobalScriptTimer::Release()
{
	if(instance)
		instance->ClearScriptTimer();
	MP_DELETE(instance);
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
		PutDebugString("Register the Global Script Timer error!");
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
		PutDebugString("Register the wait Script Timer error!!");
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
				tagScriptTimer &scripttimer = (*it).second;
				stRunScript sScript;
				sScript.pszFileName = const_cast<char*>(scripttimer.strScript.c_str());
				sScript.pRegion = NULL;
				sScript.srcShape = NULL;
				sScript.desShape = NULL;
				RunScript( &sScript, const_cast<CHAR*>( GetGame()->GetScriptFileData(sScript.pszFileName)) );
				if(!scripttimer.bLoop)
					m_ScriptTimers.erase(it);
			}
		}
		break;
	case TST_Wait:
		{
			itWS it = m_WaitScripts.find(timerid);
			if(it != m_WaitScripts.end())
			{
				CGUID guScriptID = (*it).second;
				m_WaitScripts.erase(it);
				ContinueRunScript(guScriptID, NULL);
			}
		}
		break;
	default:
		PutDebugString("the scripttimer timeout,the param 'var' error!,");
		break;
	};
	return 0;
}