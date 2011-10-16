#include "StdAfx.h"
#include "LogicLogInterface.h"
#include "LogClient.h"

#pragma warning(disable:4996)


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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