#pragma once

//class CMonster;
#include "..\..\PUBLIC\PlayerDef.h"
#include "..\..\PUBLIC\statedef.h"
#include "..\..\public\tinyxml.h"
// 怪物属性和掉落物品全局基本表
const int DROPINDEX_NULL=-1;
const int DROPINDEX_ERRO=-2;

class CGoods;

class CMonsterList
{
public:
	CMonsterList(void);
	virtual ~CMonsterList(void);

//////////////////////////////////////////////////////////////////////////
//	属性
//////////////////////////////////////////////////////////////////////////
public:

	// 技能
	struct tagSkill
	{
		WORD	wID;					//	编号
		WORD	wLevel;					//	等级
		WORD	wOdds;					//	释放的几率
	};
	//技能配置结构
	struct tagSkillInfo
	{
		tagSkillInfo():dwSkillID(0),dwSkillLevel(0),dwCoolDownTime(0),iActModifyTime(0){}
		DWORD dwSkillID;			//技能id
		DWORD dwSkillLevel;			//技能等级
		DWORD dwCoolDownTime;		//冷却时间
		int   iActModifyTime;		//技能修正
	};

	// 怪物属性
	struct tagMonster
	{
		DWORD		dwIndex;
		char		cOriginalName[50];     //原始名
		char		cName[50];             //名字
		BYTE		byBlockType;			//阻挡类型
		BYTE		byIsunbeatable;			//是否是无敌状态
		BYTE		DropFlag;				//掉宝标志：0:默认，1:只受DropList.xml影响
		char		cCollectionName[50];	//对应采集物名字
		char		cTitleName[50];			//称谓名字
		WORD		dwTitlePicID;			//称谓图片id
		WORD		dwTitleColor;			//称谓显示颜色
		BYTE		byCountry;				//国家ID
		eNpcType	eNType;				//Npc类别 (NNPC,Monster,Guard)
		WORD		wBossLevel;				//boss等级
		DWORD		dwPicID;				//图形ID
		DWORD		dwPicLevel;				//图形等级
		DWORD		dwNameColor;			//名字颜色编号,对应到客户端配置]
		DWORD		dwColorOfLevel;         //等级颜色
		DWORD		dwHpBarColor;			//血条颜色，0：表示不显示血条 1：红色 2：黄色
		DWORD		dwSoundID;				//声音ID
    	BYTE		byTamable;				//是否可以驯服
		DWORD		dwMaxTameCount;			//最大尝试驯服次数
		WORD		wFigure;				//体形(0:1x1 1:3*3 2:5*5)*
		WORD		wLevel;				    //等级
		DWORD		dwExp;					//经验值*
		DWORD		dwOccuPoint;			//职业点数
		DWORD		dwMaxHP;                //最大hp
		WORD		wMinAtk;				//最小攻击力		    
		WORD		wMaxAtk;				//最大攻击力
		WORD		wMinMAtk;				//最小法术攻击力
		WORD		wMaxMAtk;				//最大法术攻击力
		WORD		wHit;					//命中能力
		long		lDef;					//防御力
		long		lMDef;					//魔法防御力
		WORD		wBlock;					//格挡
		WORD		wParry;					//招架
		long		lImmunity;				//免伤
		WORD		wPierce;				//穿透
		WORD		wDodge;					//闪避能力
		float		fMoveSpeed;				//移动速度
		float		fFightMoveSpeed;		//战斗移动速度
		float       fTurningSpeed;			//转身速度
		long		lWillDef;				//意志抗性
		long		lElemDef[ET_Max];		//各个元素抗性
		WORD		wHpRecoverSpeed;		//生命恢复速度
		WORD		wMPRecoverSpeed;		//魔法恢复速度
		WORD		lBaseConstiDef;			//体质抗性
		long		lDeBuffDef[DET_Max];	//Debuff抗性

		WORD		wMinStopTimes;			//最少停留次数
		WORD		wMaxStopTimes;			//最多停留次数
		WORD		wMinRunTimes;			//最少移动步数
		WORD		wMaxRunTimes;			//最多移动步数

		WORD		wGuardRange;			//警戒范围*
		WORD		wChaseRange;			//追击范围*
		WORD		wPeaceMoveRange;		//和平移动范围
		WORD        wAIModel;               //AI模式
		WORD		wCanTalk;				//可否对话
		eClass		eClass_;				//怪物类型
		eEleType	eEType;				//元素类别
		WORD		wStopTime;				//停留的时间(在一个格子中间停留帧数)
		WORD		wAIHangupType;			//AI挂起方式
		WORD		wClassDam[C_Max];		//对各个种族伤害
		WORD		wElemDam[ET_Max];		//对各个元素伤害
		WORD		wMoveRandomValue;		//产生移动的概率(活力)
		WORD		wCri;					//物理爆击率
		WORD		wCriDamg;				//物理暴击伤害
		long		lCriDef;				//物理爆击抗性
		WORD		wMCri;					//魔法暴击率
		WORD		wMCriDamg;				//魔法暴击伤害
		long		lMCriDef;				//魔法爆击抗性
		//..............add ....5/16.......
		float       fHurtProportion;        //重伤比率
		DWORD       dwGuardTime;            //警戒时间
		float       fEscapePoint;           //逃跑底线（逃跑点）
        WORD        wSkillProportion;       //技能概率,一个小于等于100，大于等于0的整型。
		DWORD       dwSkillSpace;           //技能间隔
		BYTE		byIsFlyToPeace;			//是否在转换为和平的时候飞回去
		BYTE		byIsResumeAll;			//是否在转换为和平的时候完全恢复
		WORD		wReturnRange;			//归位距离
		DWORD		dwChaseTime;			//追击时间，在追击范围外的没受到攻击转化为和平的时间
		DWORD       dwAIReferrence[MONSTER_AI_REFERRENCE_NUM];      //AI参考

		//基本技能
		tagSkillInfo BaseSkillList[MONSTER_SKILL_SUM];
		tagSkillInfo CommonSkillList[MONSTER_SKILL_SUM];
		tagSkillInfo CycleSkillList[MONSTER_SKILL_SUM];

		char		cBornScriptName[MAX_PATH];
		char        cFightScriptName[MAX_PATH];
		char        cGBHScriptName[MAX_PATH];
		char		cDeadScriptName[MAX_PATH];
		char		cCycleScriptName[MAX_PATH];
		char		cSearchEnemyScriptName[MAX_PATH];
		char		cFindTargetScriptName[MAX_PATH];
		char		cKillTargetScriptName[MAX_PATH];

		tagPlayerElementProperty	m_arrElementProperty[MT_Max];	//元素属性
		//end add..........................
	};

	// 怪物对应的掉落物品属性
	struct tagDropGoods
	{
		struct tagDrop
		{
			long lGoodsIndex;			// 物品编号
			long lOdds;					// 掉落几率
			long lMaxOdds;				// 最大值
			long lMinMoney;				// 掉最小钱
			long lMaxMoney;				// 掉最大钱
			long lMinLevel;				// 掉落物品的最小等级  0表示原始物品
			long lMaxLevel;				// 掉落物品的最大等级(闭区间)
			float fLevelAttenuation;		// 等级差衰减 (玩家每高于怪物1级，掉落几率降低一定比例)
			float fLevelAttenuationLimit;	// 等级差衰减上限

			string strName;				// 物品名称
		};
		// add by chenaly 07/6/21
		struct tagOneGoods 
		{
			long	lGoodsIndex;
			long	lProbability;		//权值（概率相关）
			long	lMinLevel;			// 掉落物品的最小等级  0表示原始物品
			long	lMaxLevel;			// 掉落物品的最大等级(闭区间)
			string	strName;			//物品名称
		};
		struct tagFixedDrop 
		{
			INT iMaxDropNum;			//最大掉落数
			INT	iMinDropNum;			//最小掉落数
			long lOdds;					//本组掉落几率
			long lMaxOdds;				//本组掉落最大几率
			float fLevelAttenuation;	//本组等级衰减概率
			float fLevelAttenuationLimit;
			vector<tagOneGoods>	vecOneDropGroup;		//一个掉落组里的物品
		};
		//end change...

		//string strMonsterOrginName;		// 怪物原始名
		vector<tagDrop>	vectorDrop;		// 掉落列表
		vector<tagFixedDrop> vecFixedDrop;//固定掉物品队列
	};

private:

	static	map<string, tagMonster>		m_mapMonsterList;		// 原始Monster列表(strOrginName, tagMonster)
	//static	map<string, tagDropGoods>	m_mapDropGoods;			// 掉物品分组(strMonsterOrginName, tagDropGoods)
	//static  vector<vector<POINT>>		m_vecRoadsList;			// 路径表
	struct  tgMonsterDrop
	{
		tgMonsterDrop(){ShareDropIndex=DROPINDEX_NULL;OwnDropIndex=DROPINDEX_NULL;ActivityIndex=DROPINDEX_NULL;}
		INT ShareDropIndex;
		INT OwnDropIndex; 
		INT ActivityIndex;
	};
	static vector<tagDropGoods*>		m_vecDropList;					//drop的管理
	static map<string , tgMonsterDrop>	m_mapMonsterDrop;				//monster掉落
	static map<int, int>				m_mapRegionDrop;				//region掉落
	static vector<int>					m_vecLevelDrop;					//等级掉落
	static int							m_iWordDrop;		//世界掉落（索引）	
public:

//	static BOOL LoadRoadsList(const char * filename);			// 从文件里读路径
//	static BOOL SaveRoadsList(const char * filename);			// 保存到文件
	static BOOL LoadMonsterList(const char* filename);			// 从文件读取原始物品列表
	static BOOL SaveMonsterList(const char* filename);			// 保存到文件
	//static bool LoadDropGoodsList(const char* filename);		// 从文件读取掉落物品列表
	//static bool SaveDropGoodsList(const char* filename);		// 保存到文件
	static BOOL LoadMonsterDrop(const char* filename);
	static BOOL LoadActivityDrop(const char* filename);
	static BOOL LoadRegionDrop(const char* filename, long regionID);
private:
	static tagDropGoods* LoadOneDrop(TiXmlElement* p);
	static BOOL GetTwoNum(const char* str, long &first, long &second);
public:
	static bool LoadSkillList(const char* filename);            // 从文件读取技能列表,*******此函数应该在LoadMonsterList函数之后调用
	static bool SaveSkillList(const char*filename);             // 保存技能列表

	static bool AddToByteArray(vector<BYTE>* pByteArray);		// 添加到CByteArray
	static bool DecordFromByteArray(BYTE* pByte, long& pos);	// 解码

public:
	static tagMonster* GetProperty(DWORD dwPicID);				// 返回指定的NPC属性
	static tagMonster* GetPropertyByName(const char* strName);
	static tagMonster* GetPropertyByOrginName(const char* strName);
	static tagMonster* GetPropertyByOrginIndex(DWORD dwIndex);
	//static bool SetProperty(CMonster* pMonster, const char* strOrginName=NULL);		// 设置指定的NPC属性

	//static tagDropGoods* GetDropGoods(const char* strName);		// 返回指定的掉物品组
	// drop_flag : 1:掉落规则只受DropList.xml限制，0:同一般怪物，获取方式 ：CMonsterList::GetPropertyByOrginName(strOrginName)->DropFlag
	static BOOL CheckDropGoods(string MonsterName, INT RegionIndex, INT Level, INT LevelBalances,vector<CGoods*> &pOut, BYTE drop_flag = 0 );
	static BOOL CheckOneDrop(tagDropGoods* pDrop,int LevelBalance, vector<CGoods*> &pOut);
	static bool RefeashAllMonster();							// 刷新WORLD中的所有Monster属性指针
public:
	static void Release();
};
