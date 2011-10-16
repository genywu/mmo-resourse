/*
*	file:		SpriteDef.h
*	Brief:		小精灵的定义文件
*	detail:		
*	Authtor:	张科智
*	Datae:		2008-05-21
*/

#pragma once

//! 百分之百
const long A_Hundred_Percent	= 100;
//! 自动补给的定时器时间间隔
const long Auto_Supply_Space	= 500;
//! 自动补给的字段最大长度
const long Max_DB_Item_Size		= 2048;

//! 自动补给类型
//! 此项枚举值将作为数组索引，修改时小心
enum eAotoSupplyType
{
	//! 第一种补给HP
	eAST_HP1,
	//! 第二中补给HP
	eAST_HP2,
	//! 补给MP
	eAST_MP,
	//! 补给小精灵MP
	eAST_SpriteMP,
	//! 补给武器能量
	eAST_ArmPower,
	//! 所有类型数量（作数组边界用）
	AotoSupplyTypeNum,
};

//! 单个自动补给配置对象
struct tagAotoSupplyNode
{
	//!			状态
	bool		bActive;
	//!			触发百分比（百分数分子）
	long		lSpringScale;
	//!			补给物品索引
	long		lGoodsIndex;

	tagAotoSupplyNode(void){memset(this, 0, sizeof(tagAotoSupplyNode));}
};

enum eAutoSetupErrorType
{
	//! 成功
	eASET_NotErr,
	//! 补给类型超出范围
	eASET_TypeWithout,
	//! 设置的值不合法
	eASET_ValueErr,
	//! 没有作出改变
	eASET_NotChange,
	//! 未知错误
	eASET_UnknowErr,
};

//! 物品的适用范围
enum eGoodsApplyBound
{
	eGAB_HP		= 0x1,
	eGAB_MP		= 0x2,
	eGAB_SMP	= 0x4,
	eGAB_POWER	= 0x8,
};