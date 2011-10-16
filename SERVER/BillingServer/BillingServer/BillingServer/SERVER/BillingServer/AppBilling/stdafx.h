// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

// TODO: 在此处引用程序要求的附加头文件
#include <Windows.h>
#include <objbase.h>
#include <process.h>

#include <map>
#include <string>
#include <list>
using namespace std;

#include "..\public\debugnew.h"
#include "..\public\packagefile.h"
#include "..\public\RFile.h"
#include "..\public\public.h"
#include "..\public\tools.h"
#include "..\public\readwrite.h"
#include "..\public\ini.h"
#include "..\public\NetSession.h"
#include "..\public\NetSessionManager.h"
#include "..\public\Timer.h"
#include "..\public\Date.h"
#include "..\Server\BillingServer\BillingServer\Game.h"
