//======================================================================================
/**
*	@file		HitLevelSetup.h
*	@data		
*	@author		
*/
//======================================================================================
#pragma once

/**
*	@class HitLevelSetup
*
*	@brief 连击等级配置类
*/
class HitLevelSetup
{
public:
	/// 构造
	HitLevelSetup(void);

	/// 析构
	~HitLevelSetup(void);

public:
	/// 连击等级结构
	struct tagHitLevel
	{
		DWORD dwLevel;				// 等级
		DWORD dwHit;				// 连击数
		DWORD dwExp;				// 经验值
	};

private:
	/// 连击等级配置列表
	static vector<tagHitLevel> m_vecHitLevel;

public:
	/// 取得连击等级配置列表
	static vector<tagHitLevel>*	GetHitLevelVec(){return &m_vecHitLevel;}

	/// 从文件加载连击等级配置
	static bool LoadHitLevelSetup(const char* filename);

	/// 编码
	static bool AddToByteArray(vector<uchar>* pByteArray);

	/// 解码
	static bool DecordFromByteArray(uchar* pByte, long& pos);	
};
