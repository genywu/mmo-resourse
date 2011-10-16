

/*
*	file:		GoodsTailorSystem.h
*	Brief:		非副职业制作系统
*	detail:		
*	Authtor:	张科智
*	Datae:		2009-1-9
*/



#pragma once

//!-------------------------------------------------------------------------------------------------
//! 一个制作配方（这个类只作一些方法上的封装，不改变基类数据）
class GsOneMakeTable : public OneMakeTable
{
public:
	//!		按这个配方为玩家制作物品
	DWORD	MakeGoods(CPlayer *pPlayer) const;
};



//!-------------------------------------------------------------------------------------------------


//! 制作系统
class GoodsTailorSystem
{
public:
	GoodsTailorSystem(VOID){}
	~GoodsTailorSystem(VOID){}
	static	GoodsTailorSystem&	GetInstance					(VOID){ return c_GoodsTailorSystem; }

public:
	//!							玩家开启一个物品触发的制作
	BOOL						PlayerMakeBeginByGoods		(const CGUID &PlayerGuid, const CGUID &GoodsGuid, LONG lMakeTableID, BOOL bDelGoodsWhenFinish);
	//!							玩家开启一个NPC触发的制作
	BOOL						PlayerMakeBeginByNpc		(const CGUID &PlayerGuid, const CGUID &RegionGuid, LONG lRadius, LONG lMakeTableID);
	//!							玩家请求制作完成
	BOOL						PlayerMakeFinish			(const CGUID &PlayerGuid);
	//!							玩家请求制作取消
	BOOL						PlayerMakeCancel			(const CGUID &PlayerGuid);

private:
	//! 制作信息
	struct tagMakeInfo
	{
		//! 基本验证信息
		LONG					lType;				//! 制作类型
		CGUID					PlayerGuid;			//! 制作者
		const GsOneMakeTable	*pGsOneMakeTable;	//! 制作配方

		//! 扩充验证信息
		CGUID					GoodsGuid;			//! 触发物品
		BOOL					bDelGoodsWhenFinish;//! 制作完成后删除触发物
		CGUID					RegionGuid;			//! 合法地图
		float					fPosX;				//! 合法位置X
		float					fPosY;				//! 合法位置Y
		LONG					lRadius;			//! 有效半径
	};

	//!							进行制作
	BOOL						MakeGoods					(const tagMakeInfo& MakeInfo);

private:
	


	//!							正在制作的玩家
	map<CGUID, tagMakeInfo>		m_mapMakingPlayer;

private:
	static GoodsTailorSystem	c_GoodsTailorSystem;
};