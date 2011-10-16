/**
* @filename:GoodsAttrTypeDef.h
* @date: 2010/2/3
* @author:  zhaohang(kaba) (kuailekaba@gmail.com) 

* @purpose: 从GoodsViewer.sln中Copy过来的，它定义了Goods的数据结构
*/
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _GOODSEDITOR_ATTRTYPEDEF_H_
#define _GOODSEDITOR_ATTRTYPEDEF_H_

#include <vector>
#include <string>
#include <list>

namespace UIData{
	//附加属性
	struct  tagAddAttr{
		WORD			wType;					// 属性类型
		bool			bEnable;				// 是否有效(为将来组合属性预留)
		bool			bHide;					// 是否是隐藏属性
		long			lValue1;				// 第一个值
		long			lValue2;				// 第二个值
	};

	//套装属性
	struct tagSuitAttr{
		WORD			wType;					// 属性类型
		long			lValue1;				// 第一个值
		long			lValue2;				// 第二个值
		WORD			wSuitNum;				// 套装数目
	};

	//物品属性
	struct tagGoodAttr {
		//基本属性
		DWORD			dwIndex;				//	编号
		std::string		strOrigName;			//	原始名   < 32
		std::string		strName;				//	中文名称 < 32 
		bool				bSave;				//	是否需要存数据库
		DWORD		dwValue;					//	金币价格
		DWORD		dwSilverValue;				//  银币价格
		DWORD		dwType;						//	物品类型

		// 客户端专用属性
		DWORD		dwIconId;					//	界面图形ID
		DWORD		dwGroundId;					//	落地后图形ID
		DWORD		dwEquipID;					//	换装图形ID
		DWORD		dwSound;					//	拾取声音
		DWORD		dwSoundID1;					//	挥动/弱伤害声音
		DWORD		dwSoundID2;					//	特殊攻击/击中声音
		bool		bSoundSwitch;				//	攻击时是否混音
		DWORD		dwWeaponActType;			//	武器对应的动作类型
		std::string		strContent;				//	中文描述 < 64

		std::vector<tagAddAttr> vecAddAttr;		//  附加属性List
		std::vector<tagSuitAttr> vecSuitAttr;	//  套装属性List
	};

	typedef std::vector<tagGoodAttr>::iterator iterGoodAttr;
	typedef std::vector<tagAddAttr>::iterator iterAddAttr;
	typedef std::vector<tagSuitAttr>::iterator iterSuitAttr;

	struct tagConvert
	{
		//std::string strSign;				// 标识符 #
		std::string strOrgName;				// 原始名
		std::string strShowName;			// 显示名
		std::string lGraphicID;				// 图形ID
		std::string lGroundID;				// 落地ID
		std::string lEquipID;				// 换装ID
		std::string lPrice;					// 金币
		std::string lSilverPrice;			// 银币价格
		std::string strContent;				// 中文描述 < 64
		std::string lLevelRest;				// 等级限制
		std::string lRunScript;				// 执行脚本
		std::string lColor;					// 显示颜色
		std::string lSound;					// 拾取声音
	};

	struct tagConvertE 
	{
		std::string	OrgName;				// 原始名
		std::string	WeaponStrong;			// 武器强度
		std::string	WeaponStringAdd;		// 武器强度成长
		std::string	MinAtk;					// 最小攻击力
		std::string	MaxAtk;					// 最大攻击力
		std::string	MoGong;					// 魔法攻击
		std::string	WuGongSockPro;			// 物理致命一击率
		std::string	WuBaoJiDamage;			// 物理暴击伤害
		std::string	WuBaoJiFastness;		// 物理暴击抗性
		std::string	MoGongSockPro;			// 魔法致命一击率
		std::string	MoBaoJiDamage;			// 魔法暴击伤害
		std::string	MoBaoJiFastness;		// 魔法暴击抗性
		std::string	WuRecovery;				// 物理防御
		std::string	MoRecovery;				// 魔法防御
		std::string	AsunderValue1;			// 分解价值1
		std::string	GoodsChar;				// 物品品质
		std::string	ForceAdd;				// 力量追加
		std::string	MinJieAdd;				// 敏捷追加
		std::string	TiZhiAdd;				// 体制追加
		std::string	MindAdd;				// 智力追加
		std::string	ZhiHuiAdd;				// 智慧追加
		std::string	EnergyAdd;				// 精神追加
		std::string	MingZhongRevise;		// 命中能力修正
		std::string	DuckRevise;				// 闪避能力修正
		std::string	GeDangRevise;			// 格挡能力修正
		std::string	ZhaoJiaRevise;			// 招架能力修正
		std::string	MaxChongNeng;			// 充能修正	
		std::string	WuImmunity;				// 物理免伤值
		std::string	MoImmunity;				// 魔法免伤值;
		std::string	GoodsUpPrice;			// 物品升级价格
		std::string	BasicAperValue1;		// 基本孔数值1
		std::string	BasicAperValue2;		// 基本孔数值2
		std::string	AsunderValue2;			// 分解价值2
		std::string	LianDuValue2;			// 练度值2
	};

	enum eAddAtrrType
	{
		ADDATTR_TYPE,						// 属性类型
		ADDATTR_ENABLE,						// 是否有效(为将来组合属性预留)
		ADDATTR_HIDE,						// 是否是隐藏属性
		ADDATTR_VALUE1,						// 第一个值
		ADDATTR_VALUE2,						// 第二个值

		ADDATTR_LISTITEMNUM					// 附加属性List列表项数(5)
	};

	enum eSuitAtrrType
	{
		SUITATTR_TYPE,						// 属性类型
		SUITATTR_VALUE1,					// 第一个值
		SUITATTR_VALUE2,					// 第二个值
		SUITATTR_NUM,						// 套装数目

		SUITATTR_LISTITEMNUM				// 套装属性List列表项数 (4)
	};

	enum eAllGoodsAttrType
	{
		ALLGOODSATTR_INDEX,					// 编号
		ALLGOODSATTR_ORIGNAME,				// 原始名   < 32
		ALLGOODSATTR_NAME,					// 中文名称 < 32
		ALLGOODSATTR_SAVE,					// 是否需要存数据库
		ALLGOODSATTR_ICONID,				// 界面图形ID
		ALLGOODSATTR_GROUNDID,				// 落地后图形ID
		ALLGOODSATTR_EQUIPID,				// 换装图形ID
		ALLGOODSATTR_VALUE,					// 金币价格
		ALLGOODSATTR_SILVERVALUE,			// 银币价格
		ALLGOODSATTR_TYPE,					// 物品类型
		ALLGOODSATTR_SOUND,					// 拾取声音
		ALLGOODSATTR_CONTENT,				// 中文描述 < 64
		ALLGOODSATTR_SOUNDID1,				// 挥动/弱伤害声音
		ALLGOODSATTR_SOUNDID2,				// 特殊攻击/击中声音
		ALLGOODSATTR_SOUNDSWITCH,			// 攻击时是否混音

		ALLGOODSATTR_LISTITEMNUM			// 所有物品属性List列表项数 (15)
	};

	enum eHotKeyType
	{
		HOTKEY_CTRL_C = 1001,				// Ctrl+C
		HOTKEY_CTRL_LC ,					// Ctrl+c
		HOTKEY_CTRL_V ,						// Ctrl+V
		HOTKEY_CTRL_LV,						// Ctrl+v
		HOTKEY_CTRL_A,						// Ctrl+A
		HOTKEY_CTRL_LA ,					// Ctrl+a
		HOTKEY_CTRL_M,						// Ctrl+M
		HOTKEY_CTRL_LM,						// Ctrl+m
		HOTKEY_CTRL_D,						// Ctrl+D
		HOTKEY_CTRL_LD,						// Ctrl+d
		HOTKEY_CTRL_X,						// Ctrl+X
		HOTKEY_CTRL_LX,						// Ctrl+x
	};

	enum eOperatorHistory
	{
		OPERATOR_TYPE,						// 操作类型（添加/删除/修改/粘帖）
		OPERATOR_KIND,						// 操作属性类别（附加属性/套装属性） 
		OPERATOR_ATTRNAME,					// 操作属性名称
		OPERATOR_REPEAT,					// 属性是否有重复

		OPERATOR_NUM
	};

	struct tagOperatorRec
	{
		WORD		wOperType;				// 操作类型
		WORD		wOperAttrKind;			// 操作属性类别
		WORD		wOperAttrType;			// 操作属性类型
		bool		bIsRepeat;				// 属性是否有重复
	};

	typedef std::vector<tagOperatorRec>::iterator iterOperRec;

	enum eOperatorType
	{
		OPER_TYPE_ADD,						// 添加
		OPER_TYPE_DEL,						// 删除
		OPER_TYPE_MODIFY,					// 修改
		OPER_TYPE_PASTA,					// 粘帖

		OPER_TYPE_NUM
	};

	enum eOperatorKind
	{
		OPER_KIND_ADD,						// 添加
		OPER_KIND_SUIT,						// 删除

		OPER_KIND_NUM
	};

#endif//_GOODSEDITOR_ATTRTYPEDEF_H_
}