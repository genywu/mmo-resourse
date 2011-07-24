
#include "nwindowsapiexception.h"
#include<windows.h>
//构造函数，通过GetLastError()取得系统模块错误，并初始化ErrorType,ErrorInfo
NWindowsAPIException::NWindowsAPIException(const char * ThrowFileName, int ThrowLineNumber,const char * ModuleName)
:NBaseException(ThrowFileName,ThrowLineNumber)
{
	ErrorType = GetLastError();

	HMODULE hModule = NULL; // default to system source
	
	DWORD dwFormatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_SYSTEM ;

	hModule = GetModuleHandle(ModuleName); 

	if(hModule != NULL)
	{
		dwFormatFlags |= FORMAT_MESSAGE_FROM_HMODULE;
	}

	LPVOID lpMsgBuf;
	FormatMessageA( 
		dwFormatFlags,
		hModule,
		ErrorType,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)& lpMsgBuf,
		0,
		NULL 
		);
	ErrorInfo = string("系统模块") + string(ModuleName) + string("内部错误:") + string((char *)lpMsgBuf);  

	LocalFree( lpMsgBuf );



}

NWindowsAPIException::~NWindowsAPIException(void)
{
}

void AssertWindowsAPI(int result,char * FileName,int Line,char * ModuleName)
{
	if(result ==NULL)
	{
		throw NWindowsAPIException(FileName,Line,ModuleName);


	}



}