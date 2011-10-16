#include "stdafx.h"
#include "fmemory.h"
//
//void * _cdecl operator new(size_t nSize, const char* lpszFileName, int nLine)
//{
//	return ::operator new(nSize, _NORMAL_BLOCK, lpszFileName, nLine);
//}
//
//void operator delete(void *pVoid, const char*, int /* nLine */)
//{
//	_free_dbg(pVoid, _CLIENT_BLOCK);
//}
//
//
////void EnableDebugNew()
////{
////#ifdef _DEBUG
////	int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
////	tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
////	_CrtSetDbgFlag( tmpFlag );
////#endif
////}