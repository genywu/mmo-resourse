/*
*	file:		SpriteDef.h
*	Brief:		小精灵的定义文件
*	detail:		
*	Authtor:	张科智
*	Datae:		2008-05-21
*/

#pragma once

//! 百分之百
const LONG A_Hundred_Percent	= 100;
//! 自动补给的定时器时间间隔
const LONG Auto_Supply_Space	= 500;
//! 自动补给的字段最大长度
const LONG Max_DB_Item_Size		= 2048;
//! 稳定和普通挂机共有定时器时间间隔 3s,单位：ms
const LONG Auto_Common_Space     = 3000;
//! 稳定挂机时间的定时器时间间隔 1s,单位：ms
const LONG Auto_StableTime_Space = 1000;  
//! 稳定挂机时间进入稳定状态保护的时间10分钟,单位：s
const LONG Auto_EnterStableTime  = 600; 
//! 稳定挂机时间最大值30天，单位：s
const DWORD MAX_STABLETIME       = 2592000;    
//! 自动修理的银币下限
const DWORD MAX_REPAIRMONMEY     = 4000000;
//! 稳定挂机中进入保护状态死亡后自动原地复活的时间 ,单位：s
const DWORD Auto_DieReliveTime   = 60;
//! 武器装备耐久为0后回城的时间3分钟，单位：s
const DWORD Auto_ReturnCity      = 180;

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


//! 辅助挂机类型
enum eAutoHookType
{
	//! 自动辅助工具为 稳定辅助 类型
	eAHT_STABLE,
	//! 自动辅助工具为 普通辅助 类型
	eAHT_COMMON,
	//! 攻击技能 类型
	eAHT_FIGHTSKILL,
	//! 辅助技能 类型
	eAHT_ASSISTSKILL,
	//! 辅助道具 类型
	eAHT_ASSISTGOODS,
	//! 自动修理 类型
	eAHT_REPAIR,

	//! 所有类型数量（作数组边界用）
	AutoHookTypeNum,

};


//! 单个自动补给配置对象
struct tagAotoSupplyNode
{
	//!			状态
	BOOL		bActive;
	//!			触发百分比（百分数分子）
	LONG		lSpringScale;
	//!			补给物品索引
	LONG		lGoodsIndex;

	tagAotoSupplyNode(void){memset(this, 0, sizeof(tagAotoSupplyNode));}
};
struct tagAutoHookBuff
{
	string	strSkillName;					// 技能名称(只是为了方便策划配置，要不要后期斟酌)
	string	strOrginName;			        // 物品原始名
	DWORD	dwAutoHookIndex;   				// 技能或者物品 对应的ID号	
	DWORD	dwBuffIndex;					// buff 对应的ID号
	DWORD	Level;				        	// 等级标识符(后期斟酌)

};

//!单个自动打怪配置
struct tagAutoFightNode 
{
	//!      状态
	int     bOpen;
	//!      类型
	LONG     lFightTypeIndex;

	tagAutoFightNode(void) {memset(this, 0, sizeof(tagAutoFightNode));}
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
	//! 地图不支持
	eASET_NotMap,
	//! 技能或道具可以放置
	eASET_CanUse,
	//! 技能或道具不可使用
	eASET_NoCan,
	//! 稳定时间不足
	eASET_NoStableTime,
	//! 银币不足
	eASET_NoMoney,
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