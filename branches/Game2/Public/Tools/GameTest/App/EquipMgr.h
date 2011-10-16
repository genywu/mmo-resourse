



#ifndef _EquipMgr_H_
#define _EquipMgr_H_
#pragma once
#include "../Utility/gtDef.h"
class CGoods;

class CEquipMgr
{
public:
	CEquipMgr(void);
	~CEquipMgr(void);

	void DecodeFromDataBlock(DBReadSet& dbRead,bool bExData);

	void DecodeFromDataBlockEx(DBReadSet& dbRead);

	CGoods* GetEquip(int index);

	bool GetWeapon(const char* strOrgName);

	void SetEquip(int index,CGoods *pGood);

	CGoods* GetGoodsByGUID(const CGUID& guID,int *pPos,long* lExtendID);

	WORD GetGoodsContainerID(CGoods* pGoods);

	DWORD GetEquipIndex(long l)	{return m_dwEquipIndex[l];}
	void SetEquipIndex(long lIndex, DWORD dwIndex);
	BYTE GetEquipLevel(long l)	{return m_btEquipLevel[l];}
	void SetEquipLevel(long lIndex,BYTE level);

	long GetEquipAmount();

	CGoods* GetEquipInfo()	{ return m_Equip[0];}

private:
	CGoods*	m_Equip[EQUIP_SUM];								//装备数组,保存当前装备的数据结构
	DWORD	m_dwEquipIndex[EQUIP_SUM];		// 装备编号（其他玩家使用）
	BYTE	m_btEquipLevel[EQUIP_SUM];		// 装备等级（其他玩家使用）
};
#endif//_EquipMgr_H_