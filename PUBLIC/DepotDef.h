

/*
*	file:		DepotDef.h
*	Brief:		关于仓库的的一些C/S端共用的定义
*	detail:		
*				
*				
*	Authtor:	张科智
*	Datae:		2008-01-22
*/

#pragma once

//!			仓库密码最大字数
#define		DEPOT_PASSWORD_SIZE				PASSWORD_SIZE
#define		DEPOT_PASSWORD_ARRAY_LEN		(DEPOT_PASSWORD_SIZE + 1)

//!			主仓库容积
#define		PRIMARY_SIZE			42U
//!			主仓库2容积
#define		PRIMARY2_SIZE			20U
//!			数量仓库（金币、宝石）数量
#define		AMOUNT_DEPOT_NUM		2U
//!			扩展仓库数量（当前的子背包容器对象的容积为5，若这里的定义超过对象容积，那边也应修改）
#define		DEPOT_SUBPACK_NUM		4U

//!		仓库配置
struct	tagDepotSetup
{
	//!		子背包价格
	long	arr_lSubpackPrice[DEPOT_SUBPACK_NUM];
	//!		一段时间内密码输入错误的最大次数
	long	lMaxPwdErrCount;
	//!		冻结时间，也用于上面成员的限制密码错误次数(秒数)
	long	lFrostTime;
};

//! 仓库容器类型
enum eDepotContainerType
{
	//! 设置一个仓库容器类型比玩家身上的容器类型最大值（PEI_PACK5=10005）更大的起始值，避免重复
	DEPOT_CONTAINER_TYPE_BEGIN_VALUE = 0xFFFF00,

	//! 金币仓库
	eDCT_Gold,
	//! 银币仓库
	eDCT_Silver,

	//! 主仓库
	eDCT_Primary,
	//! 主仓库2
	//eDCT_Primary2,

	//! 扩展包裹容器
	eDCT_Secondary,
	//! 扩展仓库1
	eDCT_Secondary1,
	//! 扩展仓库2
	eDCT_Secondary2,
	//! 扩展仓库3
	eDCT_Secondary3,
	//! 扩展仓库4
	eDCT_Secondary4,
	//! 扩展仓库5
	//eDCT_Secondary5,


	//! 结束值（用于范围判断）
	DEPOT_CONTAINER_TYPE_END_VALUE,
};

#define IS_DEPOT_VOLCONTAINER(type) ((type) < DEPOT_CONTAINER_TYPE_END_VALUE && (type) > DEPOT_CONTAINER_TYPE_BEGIN_VALUE && (type) != eDCT_Gold && (type) != eDCT_Silver)
#define IS_DEPOT_CONTAINER(type) ((type) < DEPOT_CONTAINER_TYPE_END_VALUE && (type) > DEPOT_CONTAINER_TYPE_BEGIN_VALUE)


//! 玩家仓库数据的数据头
struct tagPlayerDepotDataHead
{
	//! 是否有密码
	long lHasPassword;
	//! 解冻时刻
	long lThawDate;

	//! 可用的（已购买的）子仓位
	long lUsableSubbackNum;

	//! 栏位价格表
	long arr_lSubbackPrice[DEPOT_SUBPACK_NUM];


	tagPlayerDepotDataHead(void){memset(this, 0, sizeof(tagPlayerDepotDataHead));}

};

//! 仓库的物品操作消息体
struct tagDepotGoodsMoveMsg
{
	//! 源容器类型
	ULONG lSrcContainerType;
	//! 源容器位置
	ULONG lSrcPos;
	//! 源容器类型
	ULONG lDimContainerType;
	//! 源容器位置
	ULONG lDimPos;
	//! 操作数量
	ULONG lOptAmount;

	tagDepotGoodsMoveMsg(void){memset(this, 0xff, sizeof(tagDepotGoodsMoveMsg));}
};

//! 填充上面消息体的无效值
const ULONG MOVE_MSG_USELESS_VALUE = 0xFFFFFFFFU;

//! 消息代码
enum eDepotMsgCode
{
	eDMC_Failed			= 0,		//! 失败
	eDMC_Succeed		= 1,		//! 成功
};