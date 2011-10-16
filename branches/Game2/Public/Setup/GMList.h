#pragma once

/// GM等级
enum eGMLevel
{
	GM_GOD = 200,		//天神
	GM_ADMIN = 100,		//最高级管理员,可以执行系统级操作
	GM_ARCH = 90,		//可以有限制执行管理操作的GM
	GM_WIZARD = 50,		//普通GM,只能执行临时性更改(比如禁言)的GM
	GM_PLAYER = 0		//默认等级(非GM)
};

/// GM信息(名字,等级)
struct tagGMInfo
{
	string		strName;
	eGMLevel	lLevel;
	
	tagGMInfo()
	{
		lLevel = GM_PLAYER;
	}
	tagGMInfo(const char* name, eGMLevel level)
	{
		strName = name;
		lLevel = level;
	}
};


/**
*	@class GMList
*
*	@brief GM列表类
*/
class GMList
{
public:
	/// 构造
	GMList(void);

	/// 析构
	~GMList(void);

private:
	/// GM列表 map<name, tagGMInfo>
	static map<string, tagGMInfo> m_mapGMInfo;

public:
	/// 从文件加载GM列表
	static bool Load(char* filename);

	/// 从GM名字读取其信息
	static tagGMInfo* GetInfoByName(const char* name);

	/// 清理
	static void Clear();

	/// 编码
	static bool AddToByteArray(vector<uchar>* pByteArray);

	/// 解码
	static bool DecordFromByteArray(uchar* pByte, long& pos);

	/// 取得GM列表
	static map<string, tagGMInfo>* GetGMInfoMap()	{return &m_mapGMInfo;}

	/// 口令校正
	static string s_strGodPassport;
};
