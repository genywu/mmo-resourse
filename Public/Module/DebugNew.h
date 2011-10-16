#pragma once

#include <crtdbg.h>
#include "crashdumper.h"

#ifdef _DEBUG

	void* _cdecl operator new(size_t nSize, const char* lpszFileName, int nLine);
	void operator delete(void *pObject, const char*, int /* nLine */);

	#define DEBUG_NEW new(THIS_FILE, __LINE__)

#endif

void EnableDebugNew();