//********************************************************************************
//	created:	2004/10/12 14:35:58
//	file name: 	d:\Project\DBAccess\Worlddb\DBGoods.h
//	file path:	d:\Project\DBAccess\Worlddb
//	file base:	DBGoods
//	file ext:	h
//	author:		Bugs
//
//	purpose:	存储玩家物品到数据库
//********************************************************************************

#pragma once
#ifndef _INC_DBGOODS
#define _INC_DBGOODS
#include "..\myadobase.h"
#include "..\..\server\dbserver\appworld\dbgood.h"
//#include "..\..\server\dbserver\appworld\goods\CGoodsFactory.h"
//#include "..\..\server\dbserver\appworld\goods\CGoodsBaseProperties.h"
#include "..\..\server\dbserver\appworld\dbplayer.h"

//class CGame;
class CDBGoodsGroup;

class CRSGoods :
	public CMyAdoBase
{
public:

	CRSGoods(void);
	~CRSGoods(void);

	// 删除玩家物品
	bool static DeleteContainerGoods(CDBGoodsGroup* goodsGroup, const CGUID& ownerID, _ConnectionPtr& cn);

	bool static SaveGood(const CGUID& ownerID, CDBGoodsGroup* goodsGroup, _ConnectionPtr& cn);// save call
	bool static SaveGoodsFiled(const CGUID& ownerID, CDBGoodsGroup* goodsGroup, _ConnectionPtr& cn); // save call

	// 装载玩家物品列表
	bool LoadGoods(CDBPlayer* player, CDBGoodsGroup* goodsGroup, _ConnectionPtr& cn);

	bool static DeleteGoods(const CGUID& dwPlayerID, _ConnectionPtr& cn);

	CDBGoods* FindGoodsByPos(CDBPlayer* pPlayer ,long place, long pos);

	//存Buffer
	static bool SaveBufferField(CDBGoods *pGoods, const string& szFiledName, _RecordsetPtr &pRs);
	//读Buffer
	bool LoadBufferField(CDBGoods *pGoods, const string& szFiledName, _RecordsetPtr &pRs);

	bool UpdatePlayerGoldCoins(CDBGoods *pGoods, const CGUID& ownerID, _ConnectionPtr& cn); 
	enum GOODS_PLACE
	{		
		PLACE_PACKET					= 1,	//普通背包
		PLACE_EQUIP,//装备
		//PLACE_HAND,//手上
		PLACE_WALLET,//钱包
		PLACE_SILVERWALLET,//银币
		PLACE_GEM,//! 宝石
		PLACE_YUANBAO,//元宝
		PLACE_JIFEN,//积分
		PLACE_EQUIPMENT_UPGRADING,			//装备升级
		//PLACE_BANK,//银行
		//PLACE_DEPOT, //仓库
		PEI_ENHANCEMENT,//

		PLACE_PACK =10000,
		PLACE_PACK1=10001,
		PLACE_PACK2=10002,
		PLACE_PACK3=10003,
		PLACE_PACK4=10004,
		PLACE_PACK5=10005,

		//! 设置一个仓库容器类型比玩家身上的容器类型最大值（PEI_PACK5=10005）更大的起始值，避免重复
		DEPOT_CONTAINER_TYPE_BEGIN_VALUE = 0xFFFF00,
		//! 金币仓库
		eDCT_Gold,
		//! 银币仓库（以前是宝石仓库，枚举值不变）
		eDCT_Gem,
		//! 主仓库
		eDCT_Primary,
		//! 主仓库2
		eDCT_Primary2,
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

protected:
/*	// 存储物品附加属性
	bool static SaveGoodsProperties(vector<CDBGoods::tagAddonProperty>& vAddonProperties,
		CGoodsBaseProperties *pGoodsBaseProperties, 
		char* GoodsID, _ConnectionPtr cnSave);
*/
private:	
	// 装载物品附加属性
	//bool static LoadGoodsProperties(CDBGoods *pGoods, const char *szID, _ConnectionPtr& cn);

};
#endif // _INC_DBGOODS