//======================================================================================
/**
*	@file		GoodsTailorSetup.h
*	@data		2009-1-8
*	@author		张科智
*/
//======================================================================================
#pragma once

#include "../Common/GoodsDef.h"

/// 制作类型申明
enum eMakeType
{
	eMT_FORM_GOODS,
	eMT_FROM_NPC,
};

/// 物品配置信息/消耗物
struct	tagExpend
{
	long lGoodsIndex;
	long lGoodsNum;

	bool operator==(tagExpend& RightObj){ return (this->lGoodsIndex == RightObj.lGoodsIndex); }
};


/// 物品配置信息/产物
struct tagProduction : public tagExpend
{
	long lOutOdds;

	bool operator==(tagExpend& RightObj){ return (this->lGoodsIndex == RightObj.lGoodsIndex); }
};


/**
*	@class OneMakeTable
*
*	@brief 一个配方清单
*/
class OneMakeTable
{
public:
	/// 构造
	OneMakeTable(void);

	/// 析构
	~OneMakeTable(void)
	{
		m_vExpend.clear();
		m_vProduction.clear();
	}

public:
	/// 编码
	void Code(DBWriteSet& setWriteSet) const;

	/// 解码
	void Decode(DBReadSet& setReadDB);

	/// 得到ID
	long GetID (void){ return m_lID; }

	/// 得到名称
	const char*	GetName(void){ return m_szName; }

	/// 得到说明
	const char*	GetNarrate(void){ return m_szNarrate; }

	/// 设置基本信息
	bool SetBaseInfo(long lID, const char* pName, const char* pNarrate);

	/// 添加一个耗费物
	bool AddExpend(tagExpend& Expend);

	/// 添加一个生成物
	bool AddProduction(tagProduction& Production);

	/// 取得耗费物数量
	long GetExpendNum(void){ return (long)m_vExpend.size(); }

	/// 根据ID取得耗费物
	tagExpend* GetExpend(ulong dwIndex) { return &m_vExpend[dwIndex]; }

	/// 取得生成物数量
	long GetProductionNum(void) { return (long)m_vProduction.size(); }

	/// 根据ID取得生成物
	tagProduction* GetProduction(ulong dwIndex)	{ return &m_vProduction[dwIndex]; }

public:
	/// 比较运算
	bool operator==(OneMakeTable& RightObj){ return (this->m_lID == RightObj.m_lID); }

protected:
	/// 配方ID
	long m_lID;

	/// 配方名称
	char m_szName[MAKE_TABLE_NAME_SIZE];

	/// 配方叙述
	char m_szNarrate[MAKE_TABLE_NARRATE_SIZE];

	/// 耗费物列表
	vector<tagExpend>			m_vExpend;

	/// 生成物列表
	vector<tagProduction>		m_vProduction;

private:
	/// 拷贝构造
	OneMakeTable(OneMakeTable&);

	/// 赋值操作符
	OneMakeTable& operator=(OneMakeTable&);
};


/**
*	@class GoodsTailorSetup
*
*	@brief 非副职业制作配方设置
*/

class GoodsTailorSetup
{
public:
	/// 构造
	GoodsTailorSetup(void);

	/// 析构
	~GoodsTailorSetup(void);

public:
	/// 加载非副职业制作配方设置
	static bool LoadGoodsTailorSetup(const char *filename);

	/// 编码
	static void Code(DBWriteSet& setWriteSet);

	/// 解码
	static void Decode(DBReadSet& setReadDB);

public:
	/// 清理所有配方
	static void Release(void);

	/// 添加一个配方
	static bool AddOneMakeTable(OneMakeTable* pOneMakeTable);

	/// 找出一个配方
	static const OneMakeTable* FindOneMakeTable(long lID);
	
private:
	/// 配方列表
	static map<long, OneMakeTable*> m_mapOneMakeTable;
};