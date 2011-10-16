
/*
*	file:		MedalContainer.h
*	Brief:		勋章容器类
*	detail:		勋章容器类包含两部分内容，一部分是勋章相关的全局数据和操作，另一部分是为CPlayer提供的功能接口
*				简单说明：所有物品实例由静态部分进行维护，容器实例本身不对物品进行管理，只操作物品索引(勋章ID
*	Authtor:	张科智
*	Datae:		2008-08-26
*/


#include "../public/GoodsExDef.h"
#pragma once

//! 常量申明
////////////////////////////////////////////////////////////////////////
//! 勋章最大使用数
const LONG MEDAL_MAX_USE_NUM = 3;

//! 勋章容器操作返回值
enum eMCOptReValue
{
	//! 操作成功
	eMCRV_Succeed = 1,
	//! 没有该勋章
	eMCRV_MedalNotBeing,
	//! 重复添加
	eMCRV_RepeatAdd,
	//! 目标不存在
	eMCRV_AimCantFind,
};


//! 勋章容器
////////////////////////////////////////////////////////////////////////
class MedalContainer
{
public:
	MedalContainer(CPlayer *pOwner);
	~MedalContainer(void);


public:
	//!						按索引(勋章ID)添加勋章
	eMCOptReValue			AddMedal(LONG lGoodsBaseIndex);
	//!						按索引(勋章ID)删除勋章
	eMCOptReValue			DelMedal(LONG lGoodsBaseIndex);

	//!						判断是否有指定索引(勋章ID)的的勋章
	BOOL					HasMedal(LONG lGoodsBaseIndex);
	//!						得到可使用的勋章数量
	LONG					GetMedalNum(void);

	//!						使用一个勋章（同时只能使用最多MEDAL_MAX_USE_NUM个勋章）
	CGoods*					UseMedal(LONG lGoodsBaseIndex);
	//!						归还一个使用的勋章
	eMCOptReValue			ReturnMedal(LONG lGoodsBaseIndex);
	//!						用一个正在使用的勋章交换出一个未被使用的勋章
	CGoods*					SwapMedal(LONG lUsingIndex, LONG lNewIndex);


public:
	//!						编码
	void					AddToByteArray(vector<BYTE> &vData);
	//!						编码
	void					Decode(BYTE *pData, LONG &lPos);

	//!						编码
	void					CodeToDataBlock(DBWriteSet& setWriteDB);
	//!						编码
	void					DecodeFromDataBlock(DBReadSet& setReadDB);


private:
	//!						拥有者
	CPlayer					*m_pOwner;
	//!						拥有的勋章(按索引记)
	set<LONG>				m_setHold;
	//!						正在使用的勋章(按索引记)
	set<LONG>				m_setUsing;


#pragma region 静态全局成员

public:
	//!										初始化勋章系统
	static			BOOL					InitializeMedalSystem(void);
	//!										释放资源
	static			void					Release(void);

	//!										找到一个勋章
	static			CGoods*					FindMedia(LONG lGoodsBaseIndex);
private:
	//!										以索引为key的勋章对象列表
	static			map<LONG, CGoods*>		s_mapMedalByIndex;

#pragma endregion


};