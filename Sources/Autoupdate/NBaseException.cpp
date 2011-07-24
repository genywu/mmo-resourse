
#include "nbaseexception.h"

NBaseException::NBaseException(int WhichError,const char * ErrorMessage,const char * ThrowFileName,int ThrowLineNumber)
:ErrorType(WhichError)
,ErrorInfo(ErrorMessage)
,FileName(ThrowFileName)
,LineNumber(ThrowLineNumber)
{
	
	


}

NBaseException::~NBaseException(void)
{

}



// 取得错误类型
int NBaseException::GetErrorType(void) const
{
	return ErrorType;
}

// 取得错误消息
string NBaseException::GetErrorMessage(void)
{
	return ErrorInfo;
}

// 取得抛出异常的代码文件
string NBaseException::GetThrowFileName(void)
{
	return FileName;
}

// 取得抛出异常的代码行
int NBaseException::GetThrowLineNumber(void)
{
	return LineNumber;
}

// 构造异常，初始化 FileName、LineNumber
NBaseException::NBaseException(const char * ThrowFileName , int ThrowLineNumber)
:FileName(ThrowFileName)
,LineNumber(ThrowLineNumber)
{
	ErrorInfo = "未知错误";
	ErrorInfo = -1;


}
