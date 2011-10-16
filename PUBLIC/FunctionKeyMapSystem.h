#pragma once
#ifndef _FKMS_
#define _FKMS_

#include "tinyxml.h"

/// 各种可设置热键的功能枚举类型
typedef enum _FKMS
{
	FKMS_UNKNOW = 0,

	/// 移动功能枚举值
	FKMS_MOVEKEY = 100,

	FKMS_MOVEKEY_RUNORWALK,	// 走或跑
	FKMS_MOVEKEY_FORWARD = 122,		// 向前
	FKMS_MOVEKEY_GOBACK,		// 向后
	FKMS_MOVEKEY_GOLEFT,		// 向左
	FKMS_MOVEKEY_GORIGHT,     // 向右

	FKMS_MOVEKEY_UP,			// ↑箭头（向前）
	FKMS_MOVEKEY_DOWN,			// ↓箭头（向后）
	FKMS_MOVEKEY_LEFT,			// ←箭头（向左）
	FKMS_MOVEKEY_RIGHT,		// →箭头（向右）

	/// 界面功能枚举值
	FKMS_INTERFACEKEY = 200,

	FKMS_INTERFACEKEY_ESC,			// 取消打开界面或者开启系统菜单界面
	FKMS_INTERFACEKEY_HIDEINTERFACE,	// 隐藏操作界面
	FKMS_INTERFACEKEY_TALKFACE,		// 开启私聊界面
	FKMS_INTERFACEKEY_QUST = 224,		// 开启任务界面
	FKMS_INTERFACEKEY_SHOWGOODSNAME,	// 标示地面掉落物品名字显示
	FKMS_INTERFACEKEY_FASTREPLY,		// 快速回复
	FKMS_INTERFACEKEY_AUTOSUPPLY,		// 开启药品自动补给界面
	FKMS_INTERFACEKEY_SKILL,			// 开启技能界面
	FKMS_INTERFACEKEY_PRESTIGE,		// 开启人物声望界面
	FKMS_INTERFACEKEY_PACKAGE,		// 开启背包
	FKMS_INTERFACEKEY_FRIEND,			// 开启好友界面
	FKMS_INTERFACEKEY_PRODUCE,		// 开启专业制作界面
	FKMS_INTERFACEKEY_UNDERWORLD,		// 开启帮会界面
	FKMS_INTERFACEKEY_PK,				// 开启PK设置界面
	FKMS_INTERFACEKEY_HELP,			// 开启帮助界面
	FKMS_INTERFACEKEY_INCREASESHOP,	// 开启增殖商店
	FKMS_INTERFACEKEY_NPCLIST,		// 开启NPC列表
	FKMS_INTERFACEKEY_PLAYERPROPERTY,// 开启人物属性界面
	FKMS_INTERFACEKEY_SERCHFRIEND,	// 开启好友搜索界面
	FKMS_INTERFACEKEY_PACKAGE2,		// 开启背包界面
	FKMS_INTERFACEKEY_BIGMAP,			// 开启大地图

	/// 快捷键枚举值
	FKMS_HOTKEY = 300,

	FKMS_HOTKEY_301,	
	FKMS_HOTKEY_302,
	FKMS_HOTKEY_303,
	FKMS_HOTKEY_304,
	FKMS_HOTKEY_305,
	FKMS_HOTKEY_306,
	FKMS_HOTKEY_307,
	FKMS_HOTKEY_308,
	FKMS_HOTKEY_309,
	FKMS_HOTKEY_310,
	FKMS_HOTKEY_311,
	FKMS_HOTKEY_312,
	FKMS_HOTKEY_313,
	FKMS_HOTKEY_314,
	FKMS_HOTKEY_315,
	FKMS_HOTKEY_316,
	FKMS_HOTKEY_317,
	FKMS_HOTKEY_318,
	FKMS_HOTKEY_319,
	FKMS_HOTKEY_320,
	FKMS_HOTKEY_321,
	FKMS_HOTKEY_322,
	FKMS_HOTKEY_323,
	FKMS_HOTKEY_324,

	/// 其他功能热键枚举值
	FKMS_OTHERHOTKEY = 400,

	FKMS_OTHERHOTKEY_LOCKATTACK,		// 锁定目标进行普通攻击
	FKMS_OTHERHOTKEY_AUTOMOVE,		// 自动跟随鼠标移动
	FKMS_OTHERHOTKEY_FITTING,			// 试穿功能
	FKMS_OTHERHOTKEY_AUTOPRINTSCREEN,// 自动截图
	FKMS_OTHERHOTKEY_CHAT,			// 开启聊天框
	FKMS_OTHERHOTKEY_CHANGETARGET = 426,	// 切换攻击目标
	FKMS_OTHERHOTKEY_PICKGOODS,		// 拾取物品
	FKMS_OTHERHOTKEY_AFFIRMTARGET,	// 显示当前选中的目标对象
	FKMS_OTHERHOTKEY_FIRESPELLSELF,	// 与技能快捷键配合使用对自己施放法术
	//FKMS_OTHERHOTKEY_CLOSEDISTANCE,	// 拉近视角距离
	//FKMS_OTHERHOTKEY_FARDISTANCE,		// 拉远视角距离

}FKMS;

/// 存储键值的结构
typedef struct _SETKEY
{
	int mkey;			// 主键
	int askey1;		// 第一辅助键
	int askey2;		// 第二辅助键
	_SETKEY():mkey(0),askey1(0),askey2(0){}

	bool operator==(_SETKEY& keys)
	{
		return (mkey==keys.mkey&&askey1==keys.askey1&&askey2==keys.askey2);
	}

	bool operator!=(_SETKEY& keys) { return !operator==(keys); }

	bool operator==(const _SETKEY& keys) const
	{
		return (mkey==keys.mkey&&askey1==keys.askey1&&askey2==keys.askey2);
	}

	bool operator!=(const _SETKEY& keys) const { return !operator==(keys); }
}SETKEY,*PSETKEY;

/// 存储有热键的功能
typedef struct _MAPKEY
{
	FKMS funcIndex;	// 功能索引
	SETKEY keyArray;	// 功能对应的热键组（最多可以由三个热键组成，也可以用其中一个或者两个）
	BOOL modify;		// 此功能的热键是否可被修改
	_MAPKEY():funcIndex(FKMS_UNKNOW),modify(0){}
}MAPKEY,*PMAPKEY;

typedef map<FKMS,MAPKEY> MAPFKEY;
typedef map<FKMS,MAPKEY>::iterator MAPFKEYITER;

/// 存储每个功能热键所需的全部信息（客户端专用）
typedef struct _INFOFUNC
{
	string strFuncDesc;	// 描述此功能的文字信息
	MAPKEY	key;			// 对应的功能
}INFOFUNC,*PINFOFUNC;

typedef map<FKMS,INFOFUNC> MAPFKEYINFO;
typedef map<FKMS,INFOFUNC>::iterator MAPFKEYINFOITER;
typedef map<FKMS,INFOFUNC>::const_iterator MAPFKEYINFOCITER;

/// 存储各类功能的信息（客户端专用）
typedef struct _INFOTYPE 
{
	WORD  typeId;				// 功能类ID
	string strTypeDesc;		// 功能类的描述性文字
	MAPFKEYINFO FuncKeyInfo;  // 存放各种功能及其对应的热键等数据
}INFOTYPE,*PINFOTYPE;

typedef map<WORD,INFOTYPE> MAPINFOTYPE;
typedef map<WORD,INFOTYPE>::iterator MAPINFOTYPEITER;
typedef map<WORD,INFOTYPE>::const_iterator MAPINFOTYPECITER;

class CFuncKeyMapSystem
{

#ifndef USE_IN_SERVER
	
static MAPINFOTYPE m_DefultFuncKeyInfo;	// 默认的配置信息（通过读配置文件来设置）

public:

static const MAPINFOTYPE& GetDefultFuncKey() { return m_DefultFuncKeyInfo; }
static bool LoadDefaultFuncMapKey(string strFileName);
static bool LoadFuncKeyConfig(TiXmlElement *pElem,INFOTYPE& typeinfo);

//获取整型
static int	GetIntAttribute(TiXmlElement *pElem,string strName);

// 释放数据
static void Release();

// 根据参数dwFuncIndex指定的功能索引值检测此功能的热键是否被触发,bContinuous指定此功能是否可连续，即按住键不放的情况(适用于有指定虚拟键情况使用)
static bool CheckFuncKey(MAPFKEY& mapkey,FKMS FuncIndex,SHORT appointkey,bool bContinuous);

// 根据参数dwFuncIndex指定的功能索引值检测此功能的热键是否被触发,bContinuous指定此功能是否可连续，即按住键不放的情况（适用于按当前键盘扫描码取出的值来使用）
static bool CheckFuncKey(MAPFKEY& mapkey,FKMS FuncIndex,bool bContinuous = false);

static string& GetHotKeyName(string&,const SETKEY&);					// 将热键由虚拟键值转换成可见的字符串

static string& ConvertVirKeyToString(string&,const int virKey);	// 将一个虚拟键值转化为它对应的可视化名字（符合我们习惯的按键名称）

#endif

//bool DefaultToCurrent();			// 将默认的配置信息倒入当前使用的m_FuncKey中

// 服务器和客户端用此成员保存当前各种功能对应的热键
//static MAPFKEY m_FuncKey;

// 获取当前正在使用的功能热键映射表
//static MAPFKEY& GetCurMapKey() { return m_FuncKey; }

public:

// 插入值
static bool InsertKeyToCurMap(MAPFKEY&,FKMS,MAPKEY);

// 检测某个功能当前是否定义了热键值
static bool CheckFunHasAvailableKey(MAPFKEY&,FKMS);

// 删除值
static bool DelKeyToCurMap(MAPFKEY&,FKMS);

// 修改某个功能的热键值
static bool AlterKeyofCurMap(MAPFKEY&,FKMS,MAPKEY);

};

#endif