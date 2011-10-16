#pragma once

enum eGMLevel
{
	GM_GOD = 200,		//天神
	GM_ADMIN = 100,		//最高级管理员,可以执行系统级操作
	GM_ARCH = 90,		//可以有限制执行管理操作的GM
	GM_WIZARD = 50,		//普通GM,只能执行临时性更改(比如禁言)的GM
	GM_PLAYER = 0		//默认等级(非GM)
};				//GM等级


//GM信息
struct tagGMInfo
{
	string		strName;
	eGMLevel	lLevel;
	
	tagGMInfo()
	{
		lLevel=GM_PLAYER;
	}
	tagGMInfo(const char* name, eGMLevel level)
	{
		strName = name;
		lLevel = level;
	}
};	//GM信息(名字,等级)


//GM列表
class CGMList
{
protected:
	static map<string, tagGMInfo>		m_mapGMInfo;	// map<name, tagGMInfo>

public:
	static bool Load(char* filename);	// 读入

	static tagGMInfo* GetInfoByName(const char* name);	//从GM名字读取其信息
	static inline int count()		{return (int)m_mapGMInfo.size();}

	static void Clear();

	static bool AddToByteArray(vector<BYTE>* pByteArray);		// 添加到CByteArray
	static bool DecordFromByteArray(BYTE* pByte, long& pos);	// 解码

	static map<string, tagGMInfo>* GetGMInfoMap()	{return &m_mapGMInfo;}

	CGMList(void);
	~CGMList(void);

	static string s_strGodPassport;		// 口令校正
};
