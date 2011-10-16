#pragma once

enum timeType
{
	GAPP_LOADRES=0,//载入资源超时
	GAPP_MAINLOOP,//启动时主循环超时
	GAPP_SHUTDOWN,//关闭超时
	GAPP_PING,//心跳计数超时
};

//定时器中(void* var)参数结构
struct tagSMTimerVar
{
	tagSMTimerVar(){lvar=0;lvar1=0;pvar=NULL;}
	timeType TimerType;
	long lvar;
	long lvar1;
	void *pvar;
};