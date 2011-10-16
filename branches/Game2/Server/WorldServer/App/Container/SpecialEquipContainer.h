/**
*	file:		SpecialEquipContainer.h
*	Brief:		法宝装备容器
*	detail:		法宝装备栏位， 天字、地字、人字
*	Authtor:	吴畏
*	Date:		2010-08-24
*/
#pragma once
#include "volumelimitgoodscontainer.h"
#include "../Goods/Goods.h"

class CSpecialEquipContainer :
    public CVolumeLimitGoodsContainer
{
public:
    CSpecialEquipContainer(void);
    ~CSpecialEquipContainer(void);

	/// 设置法宝装备栏位类型
	void SetTrumpType(eTrumpType eType) { m_eTrumpType = eType; }
	/// 获取法宝装备栏位类型
	eTrumpType GetTrumpType(void) { return m_eTrumpType; }

    virtual BOOL AddFromDB( CGoods* pObject, DWORD dwPosition = 0 );

private:
	eTrumpType m_eTrumpType;			///< 法宝装备类型
};
