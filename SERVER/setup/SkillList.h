#pragma once

class CSkill;

class CSkillList
{
public:
	CSkillList(void);
	virtual ~CSkillList(void);

//////////////////////////////////////////////////////////////////////////
//	属性
//////////////////////////////////////////////////////////////////////////
public:
	//struct tagSkill
	//{
	//	DWORD	dwIndex;			//	技能编号
	//	DWORD	dwActionID;			//	动作图档ID
	//	DWORD	dwCastEffectID;		//	施放技能的特效
	//	DWORD	dwHitEffectID;		//	击中特效
	//	DWORD	dwRange;			//	范围
	//	DWORD	dwObject;			//	目标
	//	DWORD	dwCastTimePlus;		//	吟唱时间修正
	//	DWORD	dwCastTimeMul;		//	吟唱时间系数
	//	DWORD	dwDelayTime;		//	僵直时间
	//	DWORD	dwCoolDownTime;		//	冷却时间
	//	DWORD	dwInterrupt;		//	打断
	//	DWORD	dwCostHp;			//	消耗Hp
	//	DWORD	dwCostHpPer;		//	消耗Hp％
	//	DWORD	dwCostMp;			//	消耗Mp
	//	DWORD	dwCostMpPer;		//	消耗Mp％
	//	DWORD	dwCostYao;			//	消耗妖力

	//	//	可变参数
	//	long	lParameter01;		//	参数1
	//	long	lParameter02;		//	参数2
	//	long	lParameter03;		//	参数3
	//	long	lParameter04;		//	参数4
	//	long	lParameter05;		//	参数5
	//	long	lParameter06;		//	参数6
	//	long	lParameter07;		//	参数7
	//	long	lParameter08;		//	参数8
	//	long	lParameter09;		//	参数9
	//	long	lParameter10;		//	参数10

	//	string	strName;			//	技能名称
	//	string	strDescribe;		//	描述
	//};

	// 技能目标
	enum eSkillDestType
	{
		SDT_SELF,				// 自己
		SDT_SHAPE,				// 其他shape
		SDT_TILE,				// 地图格子
	};

	struct tagStep
	{
		WORD	wCommandID;			// 命令编号
		BYTE	bStringParam;		// 是否是字符串变量
		union{
			long	lParam;				// 整形参数
			char	*pstrParam;			// 字符串参数
		};
	};

	struct tagLevel
	{
		WORD	wRange;					//	释放范围
		vector<tagStep>	vectorSteps;	//	步骤列表
	};

	struct tagSkill
	{
		DWORD	dwIndex;				//	技能编号
		string	strName;				//	技能名称
		string	strDescribe;			//	描述
		eSkillDestType	lDestType;		//	目标类型

		vector<tagLevel>	vectorLevels;	//	各个等级的详细资料
	};

private:

	static	map<DWORD, tagSkill>		m_mapSkillList;		// 原始Skill列表(dwIndex, tagSkill)
	static	map<string, WORD>			m_mapStepNameID;	// 步骤名称对应的ID

public:

	static BOOL LoadSkillList(const char* stepfilename, const char* dir_name);			// 从目录读取原始技能列表
	static BOOL LoadOneSkill(const char* filename);				// 读取一个技能
	static void Release();

	static bool AddToByteArray(vector<BYTE>* pByteArray);		// 添加到CByteArray
	static bool DecordFromByteArray(BYTE* pByte, long& pos);	// 解码

public:
	static tagSkill* GetPropertiy(DWORD dwIndex);				// 返回指定的Skill属性
	static tagSkill* GetPropertiyByName(const char* strName);
	static bool SetPropertiy(CSkill* pSkill, const char* strOrginName=NULL);		// 设置指定Skill的属性
};
