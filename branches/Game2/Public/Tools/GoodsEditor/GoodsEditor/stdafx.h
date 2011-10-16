// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 项目特定的包含文件

#pragma once

#pragma warning(disable:4996)

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 标头中排除不常使用的资料
#endif

// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用 Windows 95 和 Windows NT 4 或更高版本的特定功能。
#define WINVER 0x0400		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_WINNT		// 允许使用 Windows NT 4 或更高版本的特定功能。
#define _WIN32_WINNT 0x0400		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用 Windows 98 或更高版本的特定功能。
#define _WIN32_WINDOWS 0x0410 //为 Windows Me 及更新版本改变为适当的值。
#endif

#ifndef _WIN32_IE			// 允许使用 IE 4.0 或更高版本的特定功能。
#define _WIN32_IE 0x0400	//为 IE 5.0 及更新版本改变为适当的值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常被安全忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心和标准组件
#include <afxext.h>         // MFC 扩展
#include <afxdisp.h>        // MFC 自动化类
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <shlwapi.h>
#include <stdlib.h>
#include <wchar.h>

using namespace std;



#include <afxdtctl.h>		// Internet Explorer 4 公共控件的 MFC 支持
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows 公共控件的 MFC 支持
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxdhtml.h>
using namespace std;

/*
//《光芒》物品系统不需要修正附加属性
//该结构作废
struct tagModify {
	BYTE	lFixID;		// 修正哪个值（0:Value1 1:Value2）
	long	dwMin;		// 修正最小值
	long	dwMax;		// 修正最大值
	WORD	wOdds;		// 几率(1/10000);	
};*/

struct  tagAddAttr{
	WORD	wType;			// 属性类型
	bool	bEnable;		// 是否有效(为将来组合属性预留)
	bool	bHide;			// 是否是隐藏属性
	long	lValue1;		// 第一个值
	long	lValue2;		// 第二个值
	//不再计算出现几率了
	//编辑添加的属性100%出现
	//WORD	wEnableOdds;    // 出现几率
	//vector<tagModify> vecModify;
};

struct tagSuitAttr
{
	WORD wType;
	long lValue1;
	long lValue2;
	WORD wSuitNum;
};

//add by wang-xx 2009-6-30 begin
//激发属性
struct tagArouseAttr
{
	WORD wType;
	long lValue1;
	long lValue2;
	WORD wArouseCnt;
};
//add by wang-xx 2009-6-30 end

struct tagGoodAttr {
	DWORD	dwIndex;		//	编号
	string  strOrigName;	//	原始名   < 32
	string	strName;		//	中文名称 < 32

	bool	bSave;			//	是否需要存数据库
	DWORD	dwValue;		//	价格
	DWORD   dwSilverValue;	//  银币价格
	//BYTE	lX_Size;		//	物品X方向大小
	//BYTE	lY_Size;		//	物品y方向大小
	DWORD	dwType;			//	物品类型
	//DWORD	dwWeight;		//	重量

	vector<tagAddAttr> vecAddAttr;
	vector<tagSuitAttr> vecSuitAttr;

	//add by wang-xx 2009-6-30 begin
	vector<tagArouseAttr> vecArouseAttr;
	//add by wang-xx 2009-6-30 end

	// 客户端专用属性
	DWORD	dwIconId;		//	界面图形ID
	DWORD	dwGroundId;		//	落地后图形ID
	DWORD	dwEquipID;		//	换装图形ID
	DWORD	dwSound;		//	拾取声音
	DWORD	dwSoundID1;		//	挥动/弱伤害声音
	DWORD	dwSoundID2;		//	特殊攻击/击中声音
	bool	bSoundSwitch;	//	攻击时是否混音
	DWORD	dwWeaponActType;//	武器对应的动作类型
	string	strContent;		//	中文描述 < 64
};