


#include "stdafx.h"
#include "LogTools.h"

//#include <fstream>


const DWORD LON_MAX_LENGTH = 1024 * 16;	// log信息最大长度


//! 在本地日志上输出一个信息
VOID LogTools::OutLog(const char *pText, ...)
{
	char szTxt[LON_MAX_LENGTH] = {0};

	//! 生成文本
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	sprintf(szTxt, "[%02d-%02d %02d:%02d:%02d] ", stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

	va_list va;
	va_start(va, pText);
	vsprintf(&szTxt[strlen(szTxt)], pText, va);
	va_end(va);

	strcat(szTxt, "\r\n");

	//! 写文件
	char strFile[MAX_PATH];
	sprintf(strFile, "log/%04d-%02d-%02d.txt", stTime.wYear, stTime.wMonth, stTime.wDay);

	fstream logfile(strFile, ios::app | ios::out);
	if(logfile.is_open())
		logfile << szTxt << endl;

	
#ifdef ALWAYS_OUT_TO_INTERFACE
	OutTextToInterface(szTxt);
#endif

	logfile.close();
}

//! 在本地错误日志上输出一个信息
VOID LogTools::OutErrLog(const char *pText, ...)
{
	char szTxt[LON_MAX_LENGTH] = {0};

	//! 生成文本
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	sprintf(szTxt, "[%02d-%02d %02d:%02d:%02d] ", stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

	va_list va;
	va_start(va, pText);
	vsprintf(&szTxt[strlen(szTxt)], pText, va);
	va_end(va);

	strcat(szTxt, "\r\n");

	//! 写文件
	char strFile[MAX_PATH];
	sprintf(strFile, "log/%04d-%02d-%02d-error.txt", stTime.wYear, stTime.wMonth, stTime.wDay);

	fstream logfile(strFile, ios::app | ios::out);
	if(logfile.is_open())
		logfile << szTxt << endl;

#ifdef ALWAYS_OUT_TO_INTERFACE
	OutTextToInterface(szTxt);
#endif

	logfile.close();

	//! 引发调试异常
	//assert(false);
}

//! 在本地日志和监视框上输出一个信息
VOID LogTools::OutLogAndTxt(const char *pText, ...)
{
	char szTxt[LON_MAX_LENGTH] = {0};

	//! 生成文本
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	sprintf(szTxt, "[%02d-%02d %02d:%02d:%02d] ", stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

	va_list va;
	va_start(va, pText);
	vsprintf(&szTxt[strlen(szTxt)], pText, va);
	va_end(va);

	strcat(szTxt, "\r\n");

	//! 输出信息
	OutTextToInterface(szTxt);

	//! 写文件
	char strFile[MAX_PATH];
	sprintf(strFile, "log/%04d-%02d-%02d.txt", stTime.wYear, stTime.wMonth, stTime.wDay);

	fstream logfile(strFile, ios::app | ios::out);
	if(logfile.is_open())
		logfile << szTxt << endl;

	logfile.close();
}


//! 消息对话框
VOID LogTools::MsgboxInfo(const char *pText, ...)
{
	char szTxt[LON_MAX_LENGTH] = {0};
	va_list va;
	va_start(va, pText);
	vsprintf(&szTxt[strlen(szTxt)], pText, va);
	va_end(va);
	strcat(szTxt, "\r\n");

	MessageBox(NULL, szTxt, "message", MB_OK);
}
//! 错误消息对话框
VOID LogTools::MsgboxErrInfo(const char *pText, ...)
{
	char szTxt[LON_MAX_LENGTH] = {0};
	va_list va;
	va_start(va, pText);
	vsprintf(&szTxt[strlen(szTxt)], pText, va);
	va_end(va);
	strcat(szTxt, "\r\n");

	MessageBox(NULL, szTxt, "error!!", MB_OK);
}
//! 选择对话框
BOOL LogTools::MsgboxYesNo(const char *pText, ...)
{
	char szTxt[LON_MAX_LENGTH] = {0};
	va_list va;
	va_start(va, pText);
	vsprintf(&szTxt[strlen(szTxt)], pText, va);
	va_end(va);
	strcat(szTxt, "\r\n");

	return (IDOK == MessageBox(NULL, szTxt, "select:", MB_OKCANCEL));
}

void LogTools::OutToFile(bool clear,const char* filename,const char*pText,...)
{
	ofstream file;
	char Path[256]="log\\";
	strcat(Path,filename);
	if (clear)
	{
		file.open(Path,ios_base::trunc | ios_base::out);
	}
	else
	{
		file.open(Path,ios_base::app | ios_base::out);
	}
	if (!file.is_open())
	{
		return;
	}
	char szTxt[LON_MAX_LENGTH] = {0};
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	sprintf(szTxt, "[%02d-%02d %02d:%02d:%02d] ", stTime.wMonth, stTime.wDay, stTime.wHour, stTime.wMinute, stTime.wSecond);

	va_list va;
	va_start(va, pText);
	vsprintf(&szTxt[strlen(szTxt)], pText, va);
	va_end(va);
	strcat(szTxt, "\r\n");

	file<<szTxt;
	file.close();
}