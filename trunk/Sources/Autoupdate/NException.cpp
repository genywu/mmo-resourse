#include "nexception.h"

NException::NException(long ErrorDescription,const char * ThrowFileName,int ThrowLineNumber)
:NBaseException(ThrowFileName,ThrowLineNumber)
{
	ErrorType = ErrorDescription;
	ErrorInfo = ExplainError(ErrorDescription);
}

NException::~NException(void)
{



}




// 取得错误文字解释
string NException::ExplainError(long ErrorID)
{
	return ExplainErrorID(ErrorID);
}
