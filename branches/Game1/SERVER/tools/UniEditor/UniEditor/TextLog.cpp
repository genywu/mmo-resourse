#include "StdAfx.h"
#include "TextLog.h"

ofstream CTextLog::m_ofs;
SYSTEMTIME CTextLog::m_sysTime;
CTextLog::CTextLog(void)
{
	/*CString str;
	GetLocalTime(&m_sysTime);
	CreateDirectory(_T("Log"), NULL);
	str.Format(_T("Log\\%4u_%02u_%02u@%02u.%02u.%02u.log"),
		m_sysTime.wYear, m_sysTime.wMonth, m_sysTime.wDay,
		m_sysTime.wHour, m_sysTime.wMinute, m_sysTime.wSecond);

	m_ofs.open(str.GetString(), ios_base::app);
	if(m_ofs.is_open())
	{
		m_ofs << _T("________________________________________________________________________________________________________________\n");
		  AddLog(_T("打开... ...\n"));
	}*/
}

CTextLog::~CTextLog(void)
{
	/*AddLog(_T("关闭... ...\n"));
	m_ofs.close();*/
}
bool CTextLog::AddLog(const CString &str)
{
	/*SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	CString strAdd;
	strAdd.Format(_T("\n[%04u-%02u-%02u %02u:%02u:%02u] "),
		sysTime.wYear, sysTime.wMonth, sysTime.wDay,
		sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	strAdd += str;

	m_ofs << strAdd;*/

	return false;
}

bool CTextLog::WriteLog(const CString &str)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	CString strAdd;
	CString strContent;
	CString strTemp="\\log\\";
	strContent.Format(_T("\n[%04u-%02u-%02u %02u:%02u:%02u]"),
		sysTime.wYear, sysTime.wMonth, sysTime.wDay,
		sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	CreateDirectory(_T("Log"), NULL);
	//文件名
	strAdd.Format(_T("Log\\[%04u-%02u-%02u]"),
		sysTime.wYear, sysTime.wMonth, sysTime.wDay);

	strAdd +=".txt";
	strTemp +=strAdd;
	setlocale(LC_ALL,"Chinese-simplified");//设置中文环境
	ofstream file(strAdd,ios_base::app);
	if (!file.is_open())
	{
		return false;
	}
	strContent += str;
	file<<strContent;
	


	return true;
}