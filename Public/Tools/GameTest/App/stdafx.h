// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4018)
#pragma warning(disable:4267)
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4819)

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

// TODO: 在此处引用程序要求的附加头文件
#include <afx.h>
#include <time.h>
#include <mmsystem.h>
#include <process.h>
#include <math.h>

#include "lua.hpp"

//#include "../Utility/BaseDef.h"
//#include "../Utility/public.h"
//#include "../Utility/tools.h"
//#include "../Utility/readwrite.h"
//#include "../Utility/MsgType.h"
//#include "../Utility/GUID.h"
//#include "../Utility/RFile.h"

//for D3DX9
//#pragma comment(lib, "d3d9.lib")
//#pragma comment(lib, "d3dx9.lib")
//#pragma comment(lib, "winmm.lib")
//
////
//#include <d3d9.h>
//#include <d3dx9.h>
//#include <d3dx9math.h>

#include "../../../DefPublic.h"
#include "../../../ServerPublic/ServerPublic.h"
#include "../../../Common/GoodsDef.h"
#include "../../../Common/PlayerDef.h"
#include "../../../Common/MonsterDef.h"
#include "../../../Common/DepotDef.h"
#include "../../../Common/OrganizingDef.h"
#include "../../../Common/BounsDef.h"
#include "../../../Common/SkillRelated.h"
#include "../../../Common/PlayerSetupOnServer.h"
#include "../../../Nets/MemServer.h"

#include "../Utility/PlayerDef.h"
#include "../Utility/GoodsExDef.h"
#include "../Utility/timerdef.h"

// 标准模板库
#include <list>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <queue>
#include <set>
using namespace std;
extern HWND g_hWnd;


//#include "../Utility/DataBlockSetWriteRead.h"
