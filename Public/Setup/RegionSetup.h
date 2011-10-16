//======================================================================================
/**
*	@file		RegionSetup.h
*	@data		
*	@author		
*/
//======================================================================================
#pragma once


/**
*	@class CRegionSetup
*
*	@brief 场景进入配置类
*/

class CRegionSetup
{
public:
	/// 场景进入结构
	struct tagRegionSetup
	{
		long lID;						// 地图ID
		long lCanEnterLevel;			// 能够进入的等级
		char pszEnterScript[MAX_PATH];	//进入场景执行脚本
		char pszExitScript[MAX_PATH];	//退出场景执行脚本
	};

private:
	/// 场景进入配置
	static map<long,tagRegionSetup> s_mapRegionSetup;

public:
	/// 构造
	CRegionSetup();

	/// 析构
	~CRegionSetup();

	/// 从文件加载场景进入配置
	static bool LoadRegionSetup(const char* filename);

	/// 取得场景进入配置
	static tagRegionSetup*	GetProperty(long lIndex)	{return (s_mapRegionSetup.find(lIndex)!=s_mapRegionSetup.end())?(&s_mapRegionSetup[lIndex]):NULL;}

	/// 取得进入场景的等级条件
	static long GetEnterRegionLvl(long lRegionID);

	/// 取向进入场景执行的脚本
	static const char* GetEnterRegionScript(long lRegionID);

	/// 取得退出场景执行的脚本
	static const char* GetExitRegionScript(long lRegionID);

	/// 编码
	static bool AddToByteArray(vector<uchar>* pByteArray);

	/// 解码
	static bool DecordFromByteArray(uchar* pByte, long& pos);	
};