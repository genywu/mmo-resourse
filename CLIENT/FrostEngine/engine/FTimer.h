#pragma once

//定时器
#include "fsingleton.h"

#define MAX_TIME_COUNTER  0x20

class FTimer
{
	DECLARE_SINGLETON(FTimer);

	//放入FrostingEngine.Update()中
	//计算当前时间的流逝
	//提供固定的刷新帧数
	
	//消息处理-快速


	//渲染处理

	FTimer(void);
	~FTimer(void);

	//__计时__
	struct _tagCounter
	{
		char  szName[256];
		DWORD dwBegineTime;
		DWORD dwEndTime;
		DWORD dwUsedTime;

		BOOL  bUsed;
	};

	_tagCounter m_pCounterArray[MAX_TIME_COUNTER];

public:
	
	DWORD FrameRate(void);

	void Create();
	void Destroy();

	void BeginCount(const char *pszName,DWORD dwCountIndex);
	void EndCount(DWORD dwCountIndex);

	void OutputCounter();
};
