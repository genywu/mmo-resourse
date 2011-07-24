#pragma once
#include "nbaseexception.h"

class NException :
	public NBaseException
{
public:
	NException(long ErrorDescription,const char * ThrowFileName,int ThrowLineNumber);
	virtual ~NException(void);
protected:
	
	
	// 对错误号进行说明，返回对传入的错误号的详细文字解释
	virtual string ExplainErrorID(long ErrorID) const = 0;
private:
	// 取得错误文字解释
	string ExplainError(long ErrorID);
};
