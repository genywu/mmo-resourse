////////////////////////////////////////////////////////////////////////////////
//Timer.h/////////////////////////////////////////////////////////////////////
//时间定时器类:
//function:1,完成某个时间点的定时事件处理2,作一个计时器处理
//Author:Joe
//Create Time:2004.7.28
#pragma once
#include "../../../Module/MulTiThMap.h"

typedef LRESULT (CALLBACK* CallBackFun)(LPARAM lparam);
class CTimer
{
private:
	CTimer(void);
	~CTimer(void);
public:
	//时间计时器
	struct tagTimer
	{
		ULONG	uID;
		bool	bOpen;				//是否打开次计时器
		bool	bUseAITick;			//是否用AITick
		ULONG	uElapseTime;
		ULONG	uStartTime;
		ULONG	uElapseAITick;
		ULONG	uStartAITick;
		LPARAM  lparam;
		CallBackFun pCallBack;
	};
	//时间定时器
	struct tagTimeEvent
	{
		ULONG	uID;
		//bool	bTriger;			//表示该时间事件触发否
		tagTime Time;				//事件触发点
		LPARAM  lparam;
		CallBackFun pCallBack;
	};
private:
	MulTiThMap<ULONG,tagTimer>			m_Timers;		//计时器集合
	MulTiThMap<ULONG,tagTimeEvent>		m_TimeEvents;	//计时事件集合

	typedef	MulTiThMap<ULONG,tagTimer>::iterator itTimer;
	typedef MulTiThMap<ULONG,tagTimeEvent>::iterator itTimeEvent;

	static CTimer* instance;				//全局唯一的实例

private:
	ULONG	GetULONGID()
	{
		static	ULONG uID = 1;
		return uID++;
	}
public:
	static CTimer* getInstance();
	static void	Release();								//释放资源

	bool	SetTimerParam(ULONG uID,LPARAM lparam);
	bool	SetTimerCallBackFun(ULONG uID,CallBackFun pCallBackFun);

	ULONG	SetTimeEvent(tagTime& Time,CallBackFun pCallBackFun,LPARAM  lparam);//设置一个时间事件	
	bool	KillTimeEvent(ULONG uID);
	bool	SetTimeEventParam(ULONG uID,LPARAM lparam);
	bool	SetTimeEventCallBackFun(ULONG uID,CallBackFun pCallBackFun);

	void	Run();
};

CTimer* GetTimer();