#ifndef REGION_PLUG_SETUP_H
#define REGION_PLUG_SETUP_H

class CRegionPlugSetup
{
public:
	CRegionPlugSetup(void){}
	virtual ~CRegionPlugSetup(void){}

	struct tagRegionPlugSetup
	{
		long lID; // 地图ID
		bool bType; // 传送类型 0:回国都传送,1:指定地图传送
		long lRegionID; // 传送地图ID, 国都设为0
		long lX1;
		long lY1;
		long lX2;
		long lY2;
	};

	//////////////////////////////////////////////////////////////////////////
	//	属性
	//////////////////////////////////////////////////////////////////////////
private:
	static	map<long, tagRegionPlugSetup>	m_mapRegionPlugSetup;		// region plug array

public:
	static map<long, tagRegionPlugSetup>& GetRegionPlugSetupMap(void);
	static BOOL LoadRegionPlugSetup(const char* szPath);			// 从目录读取
	static void Release();

	static bool AddToByteArray(vector<BYTE>* pByteArray);		// 添加到CByteArray
	static bool DecordFromByteArray(BYTE* pByte, long& pos);	// 解码
};


#endif//REGION_PLUG_SETUP_H