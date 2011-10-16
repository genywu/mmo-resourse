//======================================================================================
/**
*	@file		GoodsSetup.h
*	@data		2010-01-05 17:30
*	@author		wing<totti19851101@gmail.com>
*/
//======================================================================================

#pragma once

#include "../ServerPublic/Server/Common/GoodsBaseProperties.h"

/**
*	@class GoodsSetup
*
*	@brief 物品配置加载
*/

class GoodsSetup
{
public:
	/// 构造
	GoodsSetup();

	/// 析构
	~GoodsSetup();

public:
	/// 加载物品配置
	static bool Load(const char* strPath);

	/// 加载物品属性DB存储配置
	static bool LoadAttrDBSetup(const char* strpath);

	/// 编码
	static bool Serialize(vector<uchar>* pStream, bool b = true);

	/// 解码
	static bool Unserialize(uchar* pStream, long& lOffset, bool b = true);

	/// 序列化到DataBlock
	static bool Serialize(DBWriteSet& setWriteDB, bool b = true);

	/// 通过DataBlock还原
	static bool Unserialize(DBReadSet& setReadDB, bool b = true);

	/// 清除
	static void Release();

private:
	/// 初始化附加属性编号
	static void InitAddonNameMap();

public:
	/// 根据ID获取物品基本属性
	static CGoodsBaseProperties* QueryGoodsBaseProperties(ulong dwID);

	/// 根据原始名获取物品基本属性
	static CGoodsBaseProperties* QueryGoodsBasePropertiesByOriginalName(const char* pName);

	/// 根据名称获取物品基本属性
	static CGoodsBaseProperties* QueryGoodsBasePropertiesByName(const char* pName);

	/// 根据ID获取物品原始名
	static const char* QueryGoodsOriginalName(ulong dwID);

	/// 根据ID获取物品名称 
	static const char* QueryGoodsName(ulong dwID);

	/// 根据物品原始名获取物品名称
	static const char* QueryGoodsName(const char* pOriginName);

	/// 根据物品原始名获取物品ID
	static ulong QueryGoodsIDByOriginalName(const char* pName);

	/// 根据ID获取物品最大叠加数
	static ulong GoodsSetup::QueryGoodsMaxStackNumber(ulong dwID);

	/// 根据物品名获取物品ID 
	static ulong QueryGoodsIDByName(const char* pName);

	/// 取得"金钱"的编号
	static ulong GetGoldCoinIndex();

	/// 取得"金钱"的物品原始名
	static const char* GetGoldOrginName();

	/// 取得"金钱"的物品名
	static const char* GetGoldName();

	/// 取得"银币"的编号
	static ulong GetSilverIndex();
	
	/// 取得"银币"的物品原始名
	static const char* GetSilverOrginName();

	/// 取得"银币"的物品名
	static const char* GetSilverName();

	/// 取得"宝石"的编号
	static ulong GetGemIndex();

	/// 取得"元宝"的编号
	static ulong GetYuanBaoIndex();

	/// 根据ID获取物品类型
	static eGoodsBaseType QueryGoodsType(ulong dwID);

	/// 根据ID获取物品价格
	static ulong QueryGoodsPrice(ulong dwID);

	/// 根据ID获取物品描述
	static const char* QueryGoodsDescribe(ulong dwID);

	/// 取得物品附加属性编号
	static ulong GetAddonByName(string str);

	/// 取得物品ID和物品基本属性映射集合
	static map<ulong,CGoodsBaseProperties*>* GetGoodsBasePropertiesList();

public:
	/// 物品属性DB存储
	static uchar s_GoodsAttrDBSetup[GAP_TATAL_NUM][2];

private:
	/// 物品原始名和物品ID映射
	static map<string, ulong> s_mOriginalNameIndex;

	/// 物品名和物品ID映射
	static map<string, ulong> s_mNameIndex;

	/// 物品附加属性个数
	static ulong s_dwAddonPropertyNum;

	/// 物品ID和物品基本属性映射
	static map<ulong,CGoodsBaseProperties*> s_mGoodsBaseProperties;

	/// 附加属性编号映射
	static map<string,ulong> s_mAddonNameMap;
};
