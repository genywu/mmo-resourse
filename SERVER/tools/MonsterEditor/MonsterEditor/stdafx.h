// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once
#pragma   warning(disable:4996)   //全部关掉
#pragma   warning(disable:4244)   //全部关掉
#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 头中排除极少使用的资料
#endif

// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用特定于 Windows XP 或更高版本的功能。
#define WINVER 0x0501		// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用特定于 Windows 98 或更高版本的功能。
#define _WIN32_WINDOWS 0x0410 // 将它更改为适合 Windows Me 或更高版本的相应值。
#endif

#ifndef _WIN32_IE			// 允许使用特定于 IE 6.0 或更高版本的功能。
#define _WIN32_IE 0x0600	// 将此值更改为相应的值，以适用于 IE 的其他版本。值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展
#include <fstream>
#include <string>
#include <iostream>
#include <fstream>
#include<sstream>
#include<vector>
#include <wchar.h>
#include <afxdisp.h>        // MFC 自动化类
#include<windef.h>
#include <shlwapi.h>
#include <stdlib.h>
#include<WinNT.h> 
#include<float.h>
using namespace std;



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

struct tagSkill
{
	string strSkillType;	//技能类型,三种类型　基本技能标识符:basic  普通技能标识符:common  周期技能标识符:cycle
	DWORD dwSkillID;		//技能ID
	WORD wSkillLevel;		//技能等级
	WORD wInterTime;		//间隔时间
};

struct tagMonster
{
	int nNO;				 //自动生成的序号
	DWORD dwIndex;			//索引值
	string strOrigName;		//原始名
	string strName;			//怪物名
	string strBlock;		//是否有阻挡
	string strSuper;		//是否无敌
	WORD   wDropFlag;		//掉宝标志
	string strCollection;	//对应采集物名
	string strTitle;		//称谓
	DWORD dwTitleColor;		//称谓颜色
	DWORD dwTitlePicture;	//称谓图片
	string strCountryID;	//图家ID
	string strNpcSort;		//NPC类别
	WORD   wBossLevel;		//BOSS类别
	DWORD dwPicID;			//图片ID
	DWORD dwPicLevel;		//图片等级
	DWORD dwNameColorID;	//名字颜色编号
	DWORD dwLevelColorID;	//等级颜色编号
	DWORD dwHpColor;		//血条颜色
	DWORD dwSoundID;		//声音ID
	string strTame;			//是否可驯服
	DWORD dwTameTimes;		//最大尝试驯服次数
	string strShape;		//体形
	int nLevel;				//等级
	DWORD dwExp;			//经验值
	DWORD dwWorkPoint;		//职业点数
	DWORD dwMaxHp;			//最大HP
	WORD wMinAttack;		//最小攻击力
	WORD wMaxAttack;		//最大攻击力
	WORD wMinMagicAttack;	//最小法力攻击
	WORD wMaxMagicAttack;	//最大法力攻击
	WORD wHitRating;		//命中能力
	WORD wDefense;			//防御力
	WORD wMagicDefense;		//魔法防御力
	WORD wBlock;			//格挡
	WORD wParry;			//招架
	WORD wImmunity;			//免伤
	WORD wPierce;			//穿透
	WORD wDodge;			//闪避能力
	float fMoveSpd;			//移动速度
	float fBatMoveSpd;		//战斗移动速度
	float fTurningSpeed;	//转身速度
	WORD wWillAnti;			//意志抗性
	WORD wGroundAnti;		//基本地元素抗性
	WORD wWaterAnti;		//基本水元素抗性
	WORD wFireAnti;			//基本火元素抗性
	WORD wWindAnti;			//基本风元素抗性
	WORD wSacreAnti;		//基本神圣元素抗性
	WORD wDarkAnti;			//基本黑暗元素抗性
	WORD wNotElemAnti;		//基本无元素抗性
	float wHpRecoverSpd;		//生命恢复速度
	float wMpRecoverSpd;		//魔法恢复速度
	WORD wBodyAnti;			//体质抗性
	WORD wBloodAnti;		//基本流血抗性
	WORD wDuskAnti;			//基本击晕抗性
	WORD wNumbAnti;			//基本麻痹抗性
	WORD wSleepAnti;		//基本睡眠抗性
	WORD wDrugAnti;			//基本中毒抗性
	WORD wNotMoveAnti;		//基本定身抗性
	WORD wSilenceAnti;		//基本沉默抗性
	WORD wMinStayTimes;		//最小停留次数
	WORD wMaxStayTimes;		//最大停留次数
	WORD wMinWalkTimes;		//最小行走次数
	WORD wMaxWalkTimes;		//最大行走次数
	WORD wAlertRange;		//警戒范围
	WORD wPurRange;			//追击范围
	WORD wPeaMoveRange;		//和平移动范围
	string strAIModel;		//AI模式
	int bTalk;				//可否对话
	string strMonRace;		//怪物种族
	string strElem;			//元素类型
	WORD wStayTime;			//停留的时间
	string strAIWay;		//AI挂起方式
	WORD wHurtHuman;		//对人形类怪物基本伤害
	WORD wHurtElem;			//对元素怪物基本伤害
	WORD wHurtDiff;			//对异族怪物基本伤害
	WORD wHurtAnimal;		//对动物怪物基本伤害
	WORD wHurtDead;			//对亡灵怪物基本伤害
	WORD wHurtDevil;		//对恶魔类怪物基本伤害
	WORD wHurtInsect;		//对昆虫类怪物基本伤害
	WORD wHurtFire;			//对火系怪物基本伤害
	WORD wHurtWater;		//对水系怪物基本伤害
	WORD wHurtGround;		//对土系怪物基本伤害
	WORD wHurtWind;			//对风系怪物基本伤害
	WORD wHurtSacre;		//对神圣系怪物基本伤害
	WORD wHurtDark;			//对黑暗系怪物基本伤害
	WORD wHurtNotElem;		//对无元素系列怪物基本伤害
	WORD wVitality;			//活力
	WORD wStormHit;			//物理暴击率
	WORD wStormHurt;		//物理暴击伤害
	WORD wStormAnti;		//物理暴击抗性
	WORD wMagicStorm;		//魔法暴击率
	WORD wMagicStormHurt;	//魔法暴击伤害
	WORD wMagicStormAnti;	//魔法暴击抗性
	float fInjurRate;		//重伤比率
	WORD wAlertTime;		//警戒时间
	float fEscape;			//逃跑底线							
	WORD wSkillRate;		//技能概率
	DWORD wSkillInter;		//技能间隔
	string strReturn;		//是否飞回
	string strAllRecover;	//是否完全恢复
	WORD wReDistance;		//归位距离
	WORD wReTime;			//归位时间
	string strRefe1;		//ai参考1
	string strRefe2;		//ai参考2
	string strRefe3;		//ai参考3
	string strRefe4;		//ai参考4
	string strRefe5;		//ai参考5
	string strBirthSct;		//出生脚本
	string strBattleSct;	//战斗脚本
	string strInjurSct;		//重伤脚本
	string strDeadSct;		//死亡脚本
	string strCycleSct;		//周期脚本
	string strChoseSkillSct;//选择技能脚本
	string strFindTgtSct;	//寻找目标脚本
	string strKillTgtSct;	//杀死目标脚本
	vector<tagSkill>vecSkill;
};

