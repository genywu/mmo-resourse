#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

class CEntityGroup;
class CIncrementShopManager
{
private:
	static CIncrementShopManager* m_pInstance;

	map<DWORD,DWORD> m_mapLimitGoodsMap;
private:		
	CIncrementShopManager();	
	~CIncrementShopManager();
public:
	static void Init();
	static void Destroy();

	static CIncrementShopManager& GetSingleton();
	static CIncrementShopManager* GetSingletonPtr();

	map<DWORD,DWORD>& GetLimitGoodsMap(){return m_mapLimitGoodsMap;}

	//添加购买记录
	void AddLimitGoodsRecord(DWORD dwGoodsId,DWORD dwGoodsNum);
	//获取某ID物品已被购买数
	DWORD GetLimitGoodsBuyNum(DWORD dwGoodsId);
	//是否还能购买指定数量的限购物品
	BOOL CanBuyGoods(DWORD dwGoodsId,DWORD dwGoodsNum);
	//清除购买记录
	void ClearLimitGoodsRecord(){m_mapLimitGoodsMap.clear();}

	void DecodeFromEntityGroup( long ret, CEntityGroup *entity );
	
	bool AddToByteArray(vector<BYTE>* pByteArray);
};