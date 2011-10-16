/** 
*  @file   GoodsFactory.h
*  @author
*
*  @brief  物品工厂类
*  @date   
*/

#pragma once

class CGoods;

class CGoodsFactory 
{
public:
	/// 构造
	CGoodsFactory();

	/// 析构
	~CGoodsFactory();

	/// 物品附加属性的生成计算
	static void CreateAddonProperties(CGoods* pGoods);

	/// 创建物品
	static CGoods* CreateGoods(ulong dwID,long lType);	
	/// 创建多个物品
	static void CreateGoods(ulong dwID, ulong dwAmount, vector<CGoods*>& vOut,long lType);


	/// 垃圾回收
	static bool GarbageCollect(CGoods** ppGoods);

	/// 解码物品 
	static CGoods* UnserializeGoods(uchar* pStream, long& lOffset,long lType);
	/// 解码物品 
	static CGoods* UnserializeGoods(DBReadSet& setReadDB,long lType);

	/// 添加创建物品日志
	static void AddCreateGoodsLog(CGoods* pGoods,long lType);

	
private:
	/// 使用物品列表
	static map<long,set<CGoods*> >	s_UsingGoods;
	/// 升级
	static bool Upgrade(CGoods* pGoods, eGoodsAddonProperties gapSource, eGoodsAddonProperties gapDest, bool bUpgrade = true );
	/// 升级装备
	static bool	UpgradeEquipment(CGoods* pGoods, long lTargetLevel);


	//! 聊天中的物品信息管理
	////////////////////////////////////////////////////////////////////////////////
public:
	///	添加一个物品信息
	static void	AddGoodsInfo(const CGUID &Key, const vector<uchar> &vGoodsInfo);
	/// 取得一个物品信息
	static bool	GetGoodsInfo(const CGUID &Key, vector<uchar> &vGoodsInfo);
	/// 输出使用物品信息
	static long OutputUseGoodsInfo(const char* pszFileName);

private:
	/// 清理
	static void ClearInfo(long lClearNum);
	/// 保存的物品信息
	static	map<CGUID, vector<uchar>>	m_mapChatGoodsInfo;
	/// 保存的时间先后顺序
	static	list<pair<CGUID, long>>		m_listSaveTimeOrder;

};