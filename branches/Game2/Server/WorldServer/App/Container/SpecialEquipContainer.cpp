/**
*	file:		SpecialEquipContainer.cpp
*	Brief:		法宝装备容器
*	detail:		法宝装备栏位， 天字、地字、人字
*	Authtor:	吴畏
*	Date:		2010-08-24
*/
#include "stdafx.h"
#include "SpecialEquipContainer.h"

CSpecialEquipContainer::CSpecialEquipContainer()
{
    // 天字、地字及人字栏位均为8个
    SetContainerVolume(8);
}

CSpecialEquipContainer::~CSpecialEquipContainer()
{

}

BOOL CSpecialEquipContainer::AddFromDB(CGoods* pObject, DWORD dwPosition /* = 0  */)
{
    BOOL bResult = FALSE;

	if (!pObject) return bResult;

	if (pObject->GetGoodsBaseType()==GBT_TRUMP)
	{
		if (pObject->GetTrumpType() == m_eTrumpType)
		{
			bResult = CVolumeLimitGoodsContainer::AddFromDB(pObject, dwPosition);
		}
	}

    return bResult;
}