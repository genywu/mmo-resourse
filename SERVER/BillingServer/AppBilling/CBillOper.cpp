#include "stdafx.h"
#include "CBillOper.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBillOper::CBillOper()
{

}

CBillOper::CBillOper(eBillOperType eType)
{
	m_eOperType=eType;
}

CBillOper::~CBillOper()
{

}

BOOL CBillOper::DoOpertaion()
{
	return TRUE;
}

VOID CBillOper::GetCurrTimeString(string &strTime)
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