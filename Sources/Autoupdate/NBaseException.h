#pragma once
#include<string>
using namespace std;
class NBaseException
{
public:
	NBaseException(int WhichError,const char * ErrorMessage,const char * ThrowFileName,int ThrowLineNumber);
	virtual ~NBaseException(void);
	
	
	// 取得错误类型
	virtual int GetErrorType(void) const;
protected:
	// 错误类型
	long ErrorType;
	// 字符形式的错误信息
	string  ErrorInfo;
private:
	string  FileName; 
	int     LineNumber;
public:
	// 返回错误消息
	virtual string GetErrorMessage(void);
	// qude
	string GetThrowFileName(void);
	// 取得抛出常
	int GetThrowLineNumber(void);
	// 构造异常，初始化
	NBaseException(const char * ThrowFileName , int ThrowLineNumber);
};
