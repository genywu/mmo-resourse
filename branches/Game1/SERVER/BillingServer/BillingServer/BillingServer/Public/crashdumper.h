
//**************************************************************************************
//	日期:	14:11:2003   
//	创建:	tiamo	
//	描述:	异常崩溃记录
//**************************************************************************************

#pragma once

namespace _Sword3
{
	// 异常过滤函数
	int CrashFilter(LPEXCEPTION_POINTERS lpEP,DWORD dwExceptionCode,char* dumpfilename = 0);
}