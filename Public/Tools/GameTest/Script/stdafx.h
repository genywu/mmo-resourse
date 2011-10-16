// stdafx.h : 标准系统包含文件的包含文件，
// 或是常用但不常更改的项目特定的包含文件
//

#pragma once

#pragma warning(disable:4018;disable:4244;disable:4267;disable:4311;disable:4312;disable:4311;disable:4996)

#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料

// TODO: 在此处引用程序要求的附加头文件
#include <afx.h>
//#include "../../public/debugnew.h"
#include "lua.hpp"
#include "../../../DefPublic.h"
#include "../../../ServerPublic/ServerPublic.h"
#include "../../../Common/GoodsDef.h"
#include "../../../Common/PlayerDef.h"
#include "../../../Common/MonsterDef.h"
#include "../../../Common/DepotDef.h"
#include "../../../Common/OrganizingDef.h"
#include "../../../Common/BounsDef.h"
#include "../../../Common/SkillRelated.h"

#include "../Utility/PlayerDef.h"
#include "../Utility/GoodsExDef.h"
#include "../../../Nets/MemServer.h"

// 标准模板库
#include <list>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <deque>
#include <set>
using namespace std;

#include <hash_map>
using namespace stdext;