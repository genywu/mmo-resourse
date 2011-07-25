#pragma once
#include "targetver.h"
#include "resource.h"
#include <stdio.h>
#include <xstring>
#include <list>
#include <tchar.h>
#include <windows.h>
#include <Psapi.h>


#include "GlobalValue.h"

#ifndef NEW
#   ifndef _DEBUG   //调试版本
#       define NEW  new(_NORMAL_BLOCK, __FILE__, __LINE__)
#   else            //发布版本
#       define NEW  new(std::nothrow)
#   endif
#endif