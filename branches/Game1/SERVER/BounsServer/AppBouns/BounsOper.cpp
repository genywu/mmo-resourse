#include "StdAfx.h"
#include "BounsOper.h"

CBounsOper::CBounsOper(void)
:m_lGSSocketID(0)
{
}

CBounsOper::CBounsOper(eOperType eOt)
:m_lGSSocketID(0)
{
	m_eOperType = eOt;
}

CBounsOper::~CBounsOper(void)
{
}

BOOL CBounsOper::DoOperator()
{
	return TRUE;
}

void CBounsOper::GetCurrentTimeString(std::string& strTime)
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
