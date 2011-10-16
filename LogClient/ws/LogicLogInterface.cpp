
#include "../StdAfx.h"

#pragma warning(disable:4996)

#include "LogicLogInterface.h"
#include "LogClient.h"


LogicLogInterface::LogicLogInterface(LogClient *pLogClient)
{
	m_pLogClient = pLogClient;
}


//!							得到当前时间字符串（00-00-00 00:00:00格式）
void LogicLogInterface::GetCurrTimeString(string &strTime)
{
	SYSTEMTIME	stTime;
	char		szTime[128] = {0};
	GetLocalTime(&stTime);
	sprintf(szTime,"%d-%d-%d %d:%d:%d" ,
		stTime.wYear,
		stTime.wMonth,
		stTime.wDay,
		stTime.wHour,
		stTime.wMinute,
		stTime.wSecond);

	strTime = string(szTime);
}


//! 注意！
//! 具体的接口定义由逻辑模块自己完成，不要定义在LogicLogInterface.cpp中





