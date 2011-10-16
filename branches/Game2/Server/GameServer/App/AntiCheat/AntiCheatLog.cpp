#include "StdAfx.h"
#include "../Player.h"
#include "../ServerRegion.h"
#include "AntiCheatCommon.h"

namespace AntiCheat
{
	static LogInfo s_LogInfo;

	const LogInfo *GetLogInfo()
	{
		return &s_LogInfo;
	}

	unsigned long AddAccWrong()
	{
		return ++s_LogInfo.AccWrong;
	}

	unsigned long AddAccRight()
	{
		return ++s_LogInfo.AccRight;
	}

	unsigned long AddAccQues()
	{
		return ++s_LogInfo.AccQues;
	}

	unsigned long AddAccTimeOut()
	{
		return ++s_LogInfo.AccTimeOut; 
	}

	unsigned long AddAccSysError()
	{
		return ++s_LogInfo.AccSysErr;
	}

	void ResetLogInfo()
	{
		memset( &s_LogInfo, 0, sizeof( s_LogInfo ) );
	}
}

