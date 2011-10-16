#ifndef _INCREMENT_SHOP_MANAGER_H_
#define _INCREMENT_SHOP_MANAGER_H_

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

class CIncrementShopManager
{
private:
	static map<DWORD,DWORD> m_mapLimitGoodsMap;
private:		
	CIncrementShopManager();	
	~CIncrementShopManager();
public:
	//取得限量记录
	static map<DWORD,DWORD>& GetLimitGoodsMap() { return m_mapLimitGoodsMap; }
	//添加购买记录
	static void AddLimitGoodsRecord( DWORD dwGoodsId, DWORD dwGoodsNum );
	//获取某ID物品已被购买数
	static DWORD GetWorldLimitNumByGoodsID( DWORD dwGoodsId );
	//清除购买记录
	static void ClearLimitGoodsRecord() { m_mapLimitGoodsMap.clear(); }
	//从WS解码
	static bool DecordFromByteArray(BYTE* pByte, long& pos);
};

#endif//_INCREMENT_SHOP_MANAGER_H_