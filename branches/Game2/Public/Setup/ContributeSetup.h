//======================================================================================
/**
*	@file		ContributeSetup.h
*	@data		
*	@author		
*/
//======================================================================================

#pragma once

/**
*	@class ContributeSetup
*
*	@brief 国家贡献配置类
*/

class ContributeSetup
{
public:
	/// 国家贡献物品结构
	struct tagContributeItem
	{
		DWORD	dwLoValue;									// 物品下限
		DWORD	dwHiValue;									// 物品上限
		DWORD	dwNum;										// 物品数量
		string	strName;									// 怪物原始名
	};

	/// 国家贡献参数结构
	struct tagContributePara
	{
		long	lCombatLevel1;								// 国家击杀级别差1	
		long	lCombatLevel2;								// 国家击杀级别差2

		long	lContribute1;								// 国家贡献区间1惩罚扣除(小于30以上)
		long	lContribute2;								// 国家贡献区间2惩罚扣除(20-40之间)

		long	lContributeLevel;							// 国家贡献获得等级修正
		long	lContributeMultiple;						// 国家贡献获得倍率修正

		long	lContributeBaseValue;						// 国家贡献损失基础值
		long	lContributeMax;								// 国家贡献损失基础上限值

		long	lCountryCityContributeModifierLose;			// 国家领地国家贡献系数损失（4.0倍）
		long	lCountryCityContributeModifierGain;			// 国家领地国家贡献系数获取（1.2倍）
		long	lCountryCityContributeItemModifier;			// 国家领地暴出宝箱系数（2.8倍）
	};
public:
	/// 构造
	ContributeSetup(void);

	/// 析构
	~ContributeSetup(void);
private:
	/// 国家贡献参数列表
	static tagContributePara m_ContributePara;

	/// 国家贡献物品列表
	static vector<tagContributeItem> m_vContributeItems;

public:
	/// 加载国家贡献配置
	static bool LoadContributeSetup(const char* filename);

	/// 编码
	static bool AddToByteArray(vector<uchar>* pByteArray);

	/// 解码
	static bool DecordFromByteArray(uchar* pByte, long& pos);

	/// 取得国家贡献参数列表
	static tagContributePara* GetContributeParaList();

	/// 取得贡献物品列表
	static vector<tagContributeItem>* GetContributeItemList();

	/// 清理
	static void Release();
};
