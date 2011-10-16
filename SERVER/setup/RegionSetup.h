#pragma once

class CRegionSetup
{
public:
	struct tagRegionSetup
	{
		long lID;					// 地图ID
		long lCanEnterLevel;		// 能够进入的等级
		char pszEnterScript[MAX_PATH];//进入场景执行脚本
		char pszExitScript[MAX_PATH];//退出场景执行脚本
	};

private:
	static map<long,tagRegionSetup> s_mapRegionSetup;

public:
	
	CRegionSetup();
	~CRegionSetup();	 
	static bool LoadRegionSetup(const char* filename);
	static tagRegionSetup*	GetProperty(long lIndex)	{return (s_mapRegionSetup.find(lIndex)!=s_mapRegionSetup.end())?(&s_mapRegionSetup[lIndex]):NULL;}

	//进入场景的等级条件
	static long GetEnterRegionLvl(long lRegionID);
	//进入场景执行的脚本
	static const char* GetEnterRegionScript(long lRegionID);
	//退出场景执行的脚本
	static const char* GetExitRegionScript(long lRegionID);

	static bool AddToByteArray(vector<BYTE>* pByteArray);		// 添加到CByteArray
	static bool DecordFromByteArray(BYTE* pByte, long& pos);	// 解码
};