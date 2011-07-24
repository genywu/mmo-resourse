#pragma once
#include "nbaseexception.h"

class NWindowsAPIException :
	public NBaseException
{
public:
	//构造函数，通过GetLastError()取得系统模块错误，并初始化ErrorType,ErrorInfo
	NWindowsAPIException(const char * ThrowFileName,int ThrowLineNumber,const char * ModuleName);
	virtual ~NWindowsAPIException(void);
};
void AssertWindowsAPI(int result,char * FileName,int Line,char * ModuleName);

#define AssertSystemAPI(result) AssertWindowsAPI(result,__FILE__,__LINE__,NULL)
#define AssertModuleAPI(result,ModuleName) AssertWindowsAPI(result,__FILE__,__LINE__,ModuleName)
