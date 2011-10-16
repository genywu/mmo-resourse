



#pragma once

#define  ALWAYS_OUT_TO_INTERFACE

//! 执行程序提供的方法集
class LogTools
{
public:
	//!						在本地日志上输出一个信息
	static		VOID		OutLog(const char *pText, ...);
	//!						在本地错误日志上输出一个信息
	static		VOID		OutErrLog(const char *pText, ...);
	//!						在本地日志和监视框上输出一个信息
	static		VOID		OutLogAndTxt(const char *pText, ...);

public:
	//!						消息对话框
	static		VOID		MsgboxInfo(const char *pText, ...);
	//!						错误消息对话框
	static		VOID		MsgboxErrInfo(const char *pText, ...);
	//!						选择对话框
	static		BOOL		MsgboxYesNo(const char *pText, ...);

	//!						在界面上输出信息
	//!						这个方法必须在引用的模块中实现
	static		VOID		OutTextToInterface(const char *pText);
	
	static		void		OutToFile(bool clear,const char* filename,const char*pText,...);
private:
	LogTools(VOID);
	~LogTools(VOID);		
};