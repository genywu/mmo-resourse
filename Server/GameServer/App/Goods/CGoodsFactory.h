/** 
*  @file   CGoodsFactory.h
*  @author
*
*  @brief  物品工厂类
*  @date   
*/

#pragma once

class CGoods;
class CPlayer;

class CGoodsFactory 
{
public:	
	static void Release();
	
	CGoodsFactory();
	
	~CGoodsFactory();

	static void RunScriptOnGet(DWORD dwGoodsId,CPlayer* pPlayer);	


	//通过制定编号创建物品
	static CGoods* CreateGoods(DWORD dwID,long lType);

	//创建多个物品	
	static void CreateGoods(DWORD dwID, DWORD dwAmount, vector<CGoods*>& vOut,long lType);

	//创建物品的附加属性
	static void CreateAddonProperties(CGoods* pGoods);
	//重新产生物品的附加属性,保留物品的等级。
	static void ReCreateAddonProperties(CGoods* pGoods);

	//创建物品的套装属性
	static void CreateSuitProperties(CGoods* pGoods);


	//通过ByteArray还原物品
	static CGoods* UnserializeGoods(BYTE* pStream, LONG& lOffset,long lType);

	//通过ByteArray还原物品
	static CGoods* UnserializeGoods(DBReadSet& setReadDB,long lType);

	//回收物品对象内存
	static BOOL GarbageCollect(CGoods** ppGoods,long lFlag, BOOL bCollectMedal = FALSE);

	//计算物品的修理价格
	static DWORD CalculateRepairPrice(CGoods* pGoods);

	//计算物品的升级价格
	static DWORD CalculateUpgradePrice(CGoods* pGoods);

	//计算物品的出售价格
	static DWORD CalculateVendPrice(CGoods *pGoods);

	//计算物品的银币出售价格
	static DWORD CalculateVendSilverPrice(CGoods *pGoods);
	

	//装备消耗
	static BOOL EquipmentWaste(CGoods* pGoods, BOOL bIsSpecialWaste = FALSE);

	//修理装备
	static BOOL RepairEquipment(CGoods* pGoods);

	//升级装备
	static BOOL UpgradeEquipment(CGoods* pGoods, LONG lTargetLevel);

	//! -----------------------------------2008-1-8 Add-----------------------------------


	//! 改变装备等级（在原有等级基础上改变）
	//! 最小改变到0，最大达到设置的等级上限
	static BOOL ChangeEquipmentLevel(CGoods* pEquipment, long lChangeDegree);

	//! 设置装备等级
	//! 最小改变到0，最大达到设置的等级上限
	static BOOL SetEquipmentLevel(CGoods* pEquipment, LONG lLevel);



	//! 得到装备是否是攻击类
	static BOOL IsAttEquipment(CGoods* pEquipment);



	//! 更新装备属性
	static void UpdateEquipmentAttr(CGoods* pEquipment);

	static  eEquipLimitType QueryPlayerEquipLimitType(CPlayer* pPlayer);

	static eBusinessGoodsType GetBusinessGoodsType(DWORD dwGoodsId);


	//! -----------------------------------2008-1-8 End-----------------------------------

	//! 创建同一个类型的所有物品，并放入一个按物品索引为key的map
	static void CreateGoodsListByType(eGoodsBaseType eType, map<LONG, CGoods*> &mapGoods);

	//物品附加值的修正计算
	static BOOL ModifyGoodsAddonProperty(CGoods* pGoods, DWORD dwID, LONG lValue1, LONG lValue2 = 0, BOOL bIsImplicitAttribute = FALSE);
	//获取物品最大叠加数
	//static DWORD QueryGoodsMaxStackNumber( DWORD dwGoodsBasePropertiesID );

private:	

	static map<string,string> s_mapSucceedOrder;
	static map<string,CGUID> s_mapPendingOrder;
	static list<string> s_listSuccedOrder;

	static set<CGoods*> s_mGcGoodsSet;

	static vector<CGoods*> s_vNoDelGoods;

	static map<long,set<CGoods*> >	s_UsingGoods;	

	static BOOL Upgrade(CGoods* pGoods, eGoodsAddonProperties gapSource, eGoodsAddonProperties gapDest, BOOL bUpgrade = TRUE );

public:

	static void AddGoodsToGCSet(CGoods* pGoods);

	static void ReleaseGCGoodsSet();
	static void SetNoDelFlag(BOOL bVal){msNoDelFlag=bVal;}
	static BOOL GetNoDelFlag(){return msNoDelFlag;}

	typedef vector<CGoods*>& NoDelGoodsVec;
	static vector<CGoods*>& GetNoDelGoodsVec(){return s_vNoDelGoods;}
	static void ReleaseNoDelGoods();

	static bool IsOrderExist(const char* szOrder);
	static void PushOrder(const char* szOrder,const char* szContent);
	static void PushPendingOrder(const char* szOrder,const CGUID& guid);
	static CGUID& GetGUIDByOrderId(const char* szOrder);
	static void ErasePendingOrder(const char* szOrder);

	//! 聊天中的物品信息管理
	////////////////////////////////////////////////////////////////////////////////
public:
	//!							添加一个物品信息
	static		void			AddGoodsInfo(const CGUID &Key, const vector<BYTE> &vGoodsInfo);
	//!							取得一个物品信息
	static		BOOL			GetGoodsInfo(const CGUID &Key, vector<BYTE> &vGoodsInfo);

	static		long			OutputUseGoodsInfo(const char *pszFileName);

private:
	//!							清理
	static		void			ClearInfo(LONG lClearNum);


	//!									保存的物品信息
	static	map<CGUID, vector<BYTE>>	m_mapChatGoodsInfo;
	//!									保存的时间先后顺序
	static	list<pair<CGUID, LONG>>		m_listSaveTimeOrder;

	static BOOL msNoDelFlag;

};