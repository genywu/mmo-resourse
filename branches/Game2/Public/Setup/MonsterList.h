//======================================================================================
/**
*	@file		MonsterList.h
*	@data		
*	@author		
*/
//======================================================================================
#pragma once

#include "../Common/StateDef.h"
#include "../Common/PlayerDef.h"
#include "../Common/MonsterDef.h"

// 怪物属性和掉落物品全局基本表
const int DROPINDEX_NULL = -1;
const int DROPINDEX_ERRO = -2;

/**
*	@class CMonsterList
*
*	@brief 怪物列表配置类
*/
class CMonsterList
{
public:
	/// 构造
	CMonsterList(void);

	/// 析构
	~CMonsterList(void);

public:

	/// 技能结构
	struct tagSkill
	{
		WORD	wID;					///<	编号
		WORD	wLevel;					///<	等级
		WORD	wOdds;					///<	释放的几率
	};
	/// 技能配置结构
	struct tagSkillInfo
	{
		tagSkillInfo():dwSkillID(0),dwSkillLevel(0),dwCoolDownTime(0),iActModifyTime(0){}
		DWORD dwSkillID;			///< 技能id
		DWORD dwSkillLevel;			///< 技能等级
		DWORD dwCoolDownTime;		///< 冷却时间
		int   iActModifyTime;		///< 技能修正
	};

	/// 怪物属性
	struct tagMonster
	{
		DWORD		dwIndex;				///< 编号
		char		cOriginalName[50];		///< 原始名
		char		cName[50];				///< 名字
		uchar		byBlockType;			///< 阻挡类型
		uchar		byIsunbeatable;			///< 是否是无敌状态
		uchar		DropFlag;				///< 掉宝标志：0:默认，1:只受DropList.xml影响
		char		cCollectionName[50];	///< 对应采集物名字
		char		cTitleName[50];			///< 称谓名字
		WORD		dwTitleColor;			///< 称谓显示颜色
		WORD		dwTitlePicID;			///< 称谓图片id
		uchar		byCountry;				///< 国家ID
		eNpcType	eNType;					///< Npc类别 (NNPC,Monster,Guard)
		WORD		wBossLevel;				///< boss等级
		DWORD		dwPicID;				///< 图形ID
		DWORD		dwPicLevel;				///< 图形等级
		DWORD		dwNameColor;			///< 名字颜色编号,对应到客户端配置
		DWORD		dwColorOfLevel;         ///< 等级颜色
		DWORD		dwHpBarColor;			///< 血条颜色，0：表示不显示血条 1：红色 2：黄色
		DWORD		dwSoundID;				///< 声音ID
    	uchar		byCatchable;			///< 是否可以捕获
		DWORD		dwMaxCatchFastness;		///< 最大捕获抗性
		ulong		dwServantID;			///< 对应侍从ID
		WORD		wFigure;				///< 体形(0:1x1 1:3*3 2:5*5)*
		WORD		wLevel;				    ///< 等级
		DWORD		dwExp;					///< 经验值
		DWORD		dwDExp;					///< 修为经验值
		DWORD		dwOccuPoint;			///< 职业点数
		DWORD		dwMaxHP;                ///< 最大hp
		WORD		wMinAtk;				///< 最小攻击力		    
		WORD		wMaxAtk;				///< 最大攻击力
		WORD		wMinMAtk;				///< 最小法术攻击力
		WORD		wMaxMAtk;				///< 最大法术攻击力
		WORD		wHit;					///< 命中能力
		WORD		wDef;					///< 防御力
		WORD		wMDef;					///< 魔法防御力
		WORD		wBlock;					///< 格挡
		WORD		wParry;					///< 招架
		WORD		wImmunity;				///< 免伤
		WORD		wPierce;				///< 穿透
		WORD		wDodge;					///< 闪避能力
		float		fMoveSpeed;				///< 移动速度
		float		fFightMoveSpeed;		///< 战斗移动速度
		float       fTurningSpeed;			///< 转身速度
		WORD		wWillDef;				///< 意志抗性
		WORD		wElemDef[ET_Max];		///< 各个元素抗性
		WORD		wHpRecoverSpeed;		///< 生命恢复速度
		WORD		wMPRecoverSpeed;		///< 魔法恢复速度
		WORD		wBaseConstiDef;			///< 体质抗性
		WORD		wDeBuffDef[DET_Max];	///< Debuff抗性

		WORD		wMinStopTimes;			///< 最少停留次数
		WORD		wMaxStopTimes;			///< 最多停留次数
		WORD		wMinRunTimes;			///< 最少移动步数
		WORD		wMaxRunTimes;			///< 最多移动步数

		DisType		GuardRange;			///< 警戒范围*
		DisType		ChaseRange;			///< 追击范围*
		DisType		PeaceMoveRange;		///< 和平移动范围
		WORD        wAIModel;               ///< AI模式
		WORD		wCanTalk;				///< 可否对话
		eClass		eClass_;				///< 怪物类型
		eEleType	eEType;					///< 元素类别
		WORD		wStopTime;				///< 停留的时间(在一个格子中间停留帧数)
		WORD		wAIHangupType;			///< AI挂起方式
		WORD		wClassDam[C_Max];		///< 对各个种族伤害
		WORD		wElemDam[ET_Max];		///< 对各个元素伤害
		WORD		wMoveRandomValue;		///< 产生移动的概率(活力)
		WORD		wCri;					///< 物理爆击率
		WORD		wCriDamg;				///< 物理暴击伤害
		WORD		wCriDef;				///< 物理爆击抗性
		WORD		wMCri;					///< 魔法暴击率
		WORD		wMCriDamg;				///< 魔法暴击伤害
		WORD		wMCriDef;				///< 魔法爆击抗性
		float       fHurtProportion;        ///< 重伤比率
		DWORD       dwGuardTime;            ///< 警戒时间
		float       fEscapePoint;           ///< 逃跑底线（逃跑点）
        WORD        wSkillProportion;       ///< 技能概率,一个小于等于100，大于等于0的整型。
		DWORD       dwSkillSpace;           ///< 技能间隔
		uchar		byIsFlyToPeace;			///< 是否在转换为和平的时候飞回去
		uchar		byIsResumeAll;			///< 是否在转换为和平的时候完全恢复
		DisType		ReturnRange;			///< 归位距离
		DWORD		dwChaseTime;			///< 追击时间，在追击范围外的没受到攻击转化为和平的时间
		DWORD       dwAIReferrence[MONSTER_AI_REFERRENCE_NUM];      ///< AI参考

		//基本技能
		tagSkillInfo BaseSkillList[MONSTER_SKILL_SUM];
		tagSkillInfo CommonSkillList[MONSTER_SKILL_SUM];
		tagSkillInfo CycleSkillList[MONSTER_SKILL_SUM];

		char		cBornScriptName[MAX_PATH];				///< 出生脚本
		char        cFightScriptName[MAX_PATH];				///< 战斗脚本
		char        cGBHScriptName[MAX_PATH];				///< 重伤脚本
		char		cDeadScriptName[MAX_PATH];				///< 死亡脚本
		char		cCycleScriptName[MAX_PATH];				///< 周期脚本
		char		cSearchEnemyScriptName[MAX_PATH];		///< 选择技能脚本
		char		cFindTargetScriptName[MAX_PATH];		///< 寻找目标脚本
		char		cKillTargetScriptName[MAX_PATH];		///< 杀死目标脚本
		char		cCatchScriptName[MAX_PATH];				///< 捕获脚本

		tagPlayerElementProperty	m_arrElementProperty[MT_Max];	///< 元素属性
        int tauntSize; ///< 仇恨列表最大长度
	};

	/// 怪物对应的掉落物品属性
	struct tagDropGoods
	{
		struct tagDrop
		{
			long lGoodsIndex;			///< 物品编号
			long lOdds;					///< 掉落几率
			long lMaxOdds;				///< 最大值
			long lMinMoney;				///< 掉最小钱
			long lMaxMoney;				///< 掉最大钱
			long lMinLevel;				///< 掉落物品的最小等级  0表示原始物品
			long lMaxLevel;				///< 掉落物品的最大等级(闭区间)
			float fLevelAttenuation;		///< 等级差衰减 (玩家每高于怪物1级，掉落几率降低一定比例)
			float fLevelAttenuationLimit;	///< 等级差衰减上限

			string strName;				///< 物品名称
		};
		struct tagOneGoods 
		{
			long	lGoodsIndex;
			long	lProbability;		///< 权值（概率相关）
			long	lMinLevel;			///< 掉落物品的最小等级  0表示原始物品
			long	lMaxLevel;			///< 掉落物品的最大等级(闭区间)
			string	strName;			///< 物品名称
		};
		struct tagFixedDrop 
		{
			INT iMaxDropNum;			///< 最大掉落数
			INT	iMinDropNum;			///< 最小掉落数
			long lOdds;					///< 本组掉落几率
			long lMaxOdds;				///< 本组掉落最大几率
			float fLevelAttenuation;	///< 本组等级衰减概率
			float fLevelAttenuationLimit;
			vector<tagOneGoods>	vecOneDropGroup;		///< 一个掉落组里的物品
		};


		vector<tagDrop>	vectorDrop;		///< 掉落列表
		vector<tagFixedDrop> vecFixedDrop;///< 固定掉物品队列
	};

	/// 怪物掉落结构
	struct  tgMonsterDrop
	{
		tgMonsterDrop(){ShareDropIndex=DROPINDEX_NULL;OwnDropIndex=DROPINDEX_NULL;ActivityIndex=DROPINDEX_NULL;}
		INT ShareDropIndex;
		INT OwnDropIndex; 
		INT ActivityIndex;
	};
private:
	/// 原始Monster列表(strOrginName, tagMonster)
	static	map<string, tagMonster>		m_mapMonsterList;

	/// drop的管理
	static vector<tagDropGoods*>		m_vecDropList;

	/// monster掉落
	static map<string , tgMonsterDrop>	m_mapMonsterDrop;

	/// region掉落
	static map<int, int>				m_mapRegionDrop;

	/// 等级掉落
	static vector<int>					m_vecLevelDrop;

	/// 世界掉落（索引）			
	static int							m_iWordDrop;			
public:
	/// 从文件加载怪物列表
	static BOOL LoadMonsterList(const char* filename);

	/// 从文件读取技能列表,*******此函数应该在LoadMonsterList函数之后调用
	static bool LoadSkillList(const char* filename);

	/// 从文件加载怪物掉落配置
	static BOOL LoadMonsterDrop(const char* filename);

	/// 从文件加载活动掉落配置
	static BOOL LoadActivityDrop(const char* filename);

	/// 从文件加载场景掉落配置
	static BOOL LoadRegionDrop(const char* filename, long regionID);

	/// 编码
	static bool AddToByteArray(vector<uchar>* pByteArray);

	/// 解码
	static bool DecordFromByteArray(uchar* pByte, long& pos);

	/// 清理
	static void Release();

private:
	/// 加载一个掉落配置
	static tagDropGoods* LoadOneDrop(TiXmlElement* p);

	/// 
	static BOOL GetTwoNum(const char* str, long &first, long &second);

public:
	/// 根据图形ID返回指定的NPC属性
	static tagMonster* GetProperty(DWORD dwPicID);

	/// 根据名称返回指定的NPC属性
	static tagMonster* GetPropertyByName(const char* strName);

	/// 根据原始名称返回指定的NPC属性
	static tagMonster* GetPropertyByOrginName(const char* strName);

	/// 根据ID返回指定的NPC属性
	static tagMonster* GetPropertyByOrginIndex(DWORD dwIndex);

	/// 取得怪物掉落集合
	static map<string , tgMonsterDrop>* GetMonsterDropList();

	/// 取得Drop管理集合
	static vector<tagDropGoods*>* GetDropGoodsList();

	/// 取得region掉落集合
	static map<int, int>* GetRegionDropList();

	/// 取得等级掉落集合
	static vector<int>* GetLevelDropList();

	/// 取得世界掉落（索引）
	static int GetWorldDropIdx();

};
