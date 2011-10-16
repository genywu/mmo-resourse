//======================================================================================
/**
*	@file		SkillList.h
*	@data		
*	@author		
*/
//======================================================================================
#pragma once

/**
*	@class CSkillList
*
*	@brief 技能列表配置类
*/
class CSkillList
{
public:
	/// 构造
	CSkillList(void);

	/// 析构
	~CSkillList(void);

public:
	/// 技能目标
	enum eSkillDestType
	{
		SDT_SELF,				// 自己
		SDT_SHAPE,				// 其他shape
		SDT_TILE,				// 地图格子
	};

	/// 技能步骤
	struct tagStep
	{
		WORD	wCommandID;			// 命令编号
		uchar	bStringParam;		// 是否是字符串变量
		union{
			long	lParam;				// 整形参数
			char	*pstrParam;			// 字符串参数
		};
	};

	/// 技能等级
	struct tagLevel
	{
		WORD	wRange;					//	释放范围
		vector<tagStep>	vectorSteps;	//	步骤列表
	};

	/// 技能结构
	struct tagSkill
	{
		DWORD	dwIndex;				//	技能编号
		string	strName;				//	技能名称
		string	strDescribe;			//	描述
		eSkillDestType	lDestType;		//	目标类型

		vector<tagLevel>	vectorLevels;	//	各个等级的详细资料
	};

private:
	/// 原始Skill列表(dwIndex, tagSkill)
	static map<DWORD, tagSkill> m_mapSkillList;

	/// 步骤名称对应的ID
	static map<string, WORD> m_mapStepNameID;

public:
	// 从文件加载原始技能列表
	static BOOL LoadSkillList(const char* stepfilename, const char* dir_name);

	/// 编码
	static bool AddToByteArray(vector<uchar>* pByteArray);

	/// 解码
	static bool DecordFromByteArray(uchar* pByte, long& pos);

public:
	/// 根据技能编号查询指定的Skill属性
	static tagSkill* GetPropertiy(DWORD dwIndex);

	/// 根据技能名称查询指定的Skill属性
	static tagSkill* GetPropertiyByName(const char* strName);

private:
	/// 读取一个技能
	static BOOL LoadOneSkill(const char* filename);

	/// 释放
	static void Release();
};
