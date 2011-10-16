//======================================================================================
/**
*	@file		RegionPlugSetup.h
*	@data		
*	@author		
*/
//======================================================================================
#pragma once


/**
*	@class CRegionPlugSetup
*
*	@brief 
*/
class CRegionPlugSetup
{
public:
	/// 构造
	CRegionPlugSetup(void){}

	/// 析构
	~CRegionPlugSetup(void){}

	/// region plug结构
	struct tagRegionPlugSetup
	{
		long lID;				// 地图ID
		bool bType;				// 传送类型 0:回国都传送,1:指定地图传送
		long lRegionID;			// 传送地图ID, 国都设为0
		long lX1;
		long lY1;
		long lX2;
		long lY2;
	};

private:
	/// region plug集合
	static	map<long, tagRegionPlugSetup>	m_mapRegionPlugSetup;

public:
	/// 取得region plug 集合
	static map<long, tagRegionPlugSetup>& GetRegionPlugSetupMap(void);

	/// 从文件加载region plug配置
	static bool LoadRegionPlugSetup(const char* szPath);

	/// 清理
	static void Release();

	/// 编码
	static bool AddToByteArray(vector<uchar>* pByteArray);

	// 解码
	static bool DecordFromByteArray(uchar* pByte, long& pos);	
};
