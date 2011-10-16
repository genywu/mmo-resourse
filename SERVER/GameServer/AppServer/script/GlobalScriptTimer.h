//=============================================================================
/**
*  file: GlobalScriptTimer.h
*
*  Brief:管理脚本中定制的全局脚本定时器
*
*  Authtor:wangqiao
*	
*	Datae:2003-3-4
*/
//=============================================================================

#ifndef __GLOABAL_SCRIPT_TIMER_H__
#define __GLOABAL_SCRIPT_TIMER_H__
#pragma once

#include "..\public\GameEvent.h"

class CGlobalScriptTimer: public BaseMemObj, public CGameEvent
{
	typedef enum eTimerScriptType
	{
		TST_Wait=0,
		TST_Timer,
	}eTST;
public:
	CGlobalScriptTimer(void);
	~CGlobalScriptTimer(void);

private:
	static CGlobalScriptTimer* instance;

public:
	static CGlobalScriptTimer* GetInstance();
	static void Release();

private:
	struct tagScriptTimer
	{
		long lUserData;
		string	strScript;
		bool bLoop;//是否循环
	};
	//注册的脚本定时器标量
	typedef map<long,tagScriptTimer>	mapScriptTimers;
	typedef map<long,tagScriptTimer>::iterator itST;
	//被注册的脚本定时器
	mapScriptTimers m_ScriptTimers;


	//当脚本执行到wait函数时，定制的定时器
	typedef map<long,CGUID>	mapWaitScripts;
	typedef map<long,CGUID>::iterator itWS;
	//被注册的脚本定时器
	mapWaitScripts m_WaitScripts;

public:
	//定制一个脚本定时器
	long RegisterTimer(long lFutureTime,long lInterTime,
							const char* pszScriptName,long lUserData);
	//注销一个脚本定时器
	long UnRegisterTimer(long lUserData,long lTimerID);

	//定制执行wait函数的脚本
	void RegisterWaitScript(const CGUID& guScriptID,long lFutureTime);
	//清除所有脚本定时器
	void ClearScriptTimer();

	//网络消息响应
	virtual void OnMessage(CMessage*)	{ return; }
	//超时响应
	virtual long OnTimeOut(DWORD timerid,DWORD curtime,const void* var);
	//当定时器被取消的调用
	void OnTimerCancel(DWORD timerid,const void* var){return;}
	//处理实体间消息函数
	virtual long Proc(eEventType type,const tagProcVar *pVar){return 0;}
};

#endif
