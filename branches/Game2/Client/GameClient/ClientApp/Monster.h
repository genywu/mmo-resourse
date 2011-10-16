#pragma once
#include "MoveShape.h"
#include "../../../Public/Common/MonsterDef.h"
#include "../../../Public/Common/MultiHpBar.h"
class CMonster :
	public CMoveShape
{
public:
	CMonster(void);
	virtual ~CMonster(void);

	// 初始化变量名到值得影射
	void InitNameValueMap();

public:
	// -------------NPC的相关任务状态标识（刘科07.10.11添加）--------------
	// 该枚举状态顺序，标示NPC任务状态显示的优先级（黄问-绿问-蓝问-黄感-绿感-蓝感-灰问-灰感-其他）
	enum eEffectLevel
	{
		YELLOW_QMARK = 0,		// 黄色问号		（已接主线任务[可完成]）
		GREEN_QMARK,			// 绿色问号		（已接支线任务[可完成]）
		BLUE_QMARK,				// 兰色问号		（已接重复任务[可完成]）
		YELLOW_EMARK,			// 黄色感叹号	（可接主线任务）
		GREEN_EMARK,			// 绿色感叹号	（可接支线任务）
		BLUE_EMARK,				// 兰色感叹号	（可接反复任务）
		GRAY_QMARK,				// 灰色问号		（已接任务[未完成]）
		GRAY_EMARK,				// 灰色感叹号	（任务即将可接[等级未足等]）
		EFFECT_LEVEL,
	};
	// 状态类型
	enum eStateByQuest
	{
		STATE_STEP = 0,			// 步骤
		STATE_TRACE,			// 追踪
		STATE_QUEST,			// 任务
		STATE_NUM,				// 任务相关状态的数量
	};

	struct tagQuestInfo
	{
		long lQuestID;			// 任务ID
		long lStepIndex;		// 任务步骤的索引
		long lTraceIndex;		// 该步骤追踪索引
		long lPicOK;			// 图片（完成或可接）
		long lPicNO;			// 图片（未完成或不可接）
		bool bIsOK;				// 是否显示完成图标（或者可接任务图标）
		tagQuestInfo()
		{
			lQuestID = 0;
			lStepIndex = -1;
			lTraceIndex = -1;
			lPicOK = -1;
			lPicNO = -1;
			bIsOK = false;
		}
	};

	// 任务相关状态标识结构，该结构为每种状态计数
	struct tagStateByQuest
	{
		int iState;				// 状态标识
		vector<tagQuestInfo> vec_QuestInfo;		// 该NPC的此种状态对应的任务信息列表
		tagStateByQuest()
		{
			iState = -1;
		}
	};
	// 设置该NPC对象任务信息相关的指定状态
	void SetStateByQuest(int iState, long lPicOK, long lPicNO, bool bIsAdd, long lQuestID, long lStep = -1, long lTraceIndex=-1);
	// 获得当前NPC对象需要显示的任务相关状态索引
	long GetStateByQuest();
	// 获得指定状态应该绘制的标志（完成/未完成，或者可接，将接）
	long GetPicIndexByNpc(int iState);
	// 从任务信息表中移出指定任务ID的任务信息
	void RemoveStateByQuest(int iState, long lQuestID, long lStep = -1, long lTraceIndex = -1);
	// 获得和设置当前任务相关的特效
	void SetCurQuestEffect(long lEffectID);
	long GetCurQuestEffect()				{return m_lCurQuestEffectID;}
	// 获得和设置当前monster的转身速度
	void SetTurnSpeed(float fSpeed)			{m_fTurnSpeed = fSpeed;}
	float GetTurnSpeed()					{return m_fTurnSpeed;}

private:
	tagStateByQuest m_State[STATE_NUM];
	long	m_lEffectType[EFFECT_LEVEL];
	long	m_lCurQuestEffectID;		// 当前的任务相关特效ID
	list<long>	m_lsOtherQuestEffectID;	// 其他的任务特效（固定优先级之外的）
	void DelOtherQuestEffectID(long lID);		// 删除指定ID的其它任务特效
	//---------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
//	属性
//////////////////////////////////////////////////////////////////////////
public:

	// 怪物种类
	enum eMonsterKind
	{
		MK_HUMAN,				// 人类
		MK_MONSTER,				// 野兽
		MK_DEMON,				// 妖魔
		MK_UNDIED,				// 不死
		MK_BOSS,				// 老怪
		MK_GUARD,				// 守卫
	};
	
    // 宠物代码注释 Bugs 2008-9-12
	//作为宝宝的信息
    //struct tagPetInfo
	//{
	//	long lMasterType;
	//	CGUID MasterID;
	//	char strMasterName[20];
	//	DWORD dwLevel;
	//	DWORD dwExp;
	//	tagPetInfo()
	//	{
	//		lMasterType=0;
	//		dwLevel=dwExp=0;
	//		memset(strMasterName,0,sizeof(strMasterName));
	//	}
	//};

private:
	//出现的渐变效果
	BOOL  m_bFade;
	BOOL  m_bFadeIn;//FALSEOUT TRUEIN
	DWORD m_dwFadeStartTime;
	DWORD m_dwFadeTimeLegth;
	float m_fFadeRatio;
	DWORD m_dwLastAction;


	//--------------07.12.6liuke------------------
	//BOOL	m_bIsCanTalk;	// Monster能否对话标记
	string	m_strTitle;		// Monster的称号
	WORD	m_wTitleColor;	// 称号的颜色
	WORD	m_wIconID;		// Monster的图标ID
	string	m_strOrigName;	// 原始名称
	//--------------------------------------------

	//---------------08.7.2liuke------------------
	long	m_lCreatorType;		// 创建关联者的类型
	CGUID	m_CreatorID;		// 创建关联者的GUID
	//--------------------------------------------
	// --------------08.8.21liuke-----------------
	float	m_fTurnSpeed;		// 转身速度


	virtual void SetAction(WORD l);
public:
	//--------------07.12.6liuke------------------
	// 设置和获得Monster的称号
	void SetMonsterTitle(const char *strTitle)	{m_strTitle = strTitle;}
	const char * GetMonsterTitle()				{return m_strTitle.c_str();}
	// 设置和获得Monster称号的颜色
	void SetTitleColor(WORD lColor)			{m_wTitleColor = lColor;}
	LONG GetTitleColor()					{return m_wTitleColor;}
	// 设置和获得Monster的图标ID
	void SetMonsterIconID(WORD lIconID)		{m_wIconID = lIconID;}
	LONG GetMonsterIconID()					{return m_wIconID;}

	//---------07.12.6刘科添加，显示Monster称号----------
	void DisplayMasterTitle(const char* str, long lIconID, D3DCOLOR color, D3DCOLOR bkcolor, float fPos, D3DCOLOR bkBarColor=0,bool bShoWName = false);
	//---------------------------------------------------

	// 获得Monster创建者的类型和ID
	long GetCreatorType()		{return m_lCreatorType;}
	CGUID GetCreatorID()		{return m_CreatorID;}
	// 设置Monster创建者的类型和ID
	void SetCreatorType(long type)		{m_lCreatorType = type;}
	void SetCreatorID(CGUID guid)		{m_CreatorID = guid;}



	// 获得Monster的原始名
	virtual const char* GetOrigName()			{ return (LPCSTR)m_strOrigName.c_str();}
	// 设置位置
	virtual void SetPosXY(float x, float y);

	float GetFadeRatio() const { return m_fFadeRatio;}
	BOOL  IsFade() const { return m_bFade;}
	void  UpdateProperty(CMessage* pMsg);
	void  DecordChangeProperty(BYTE* pByte, long& pos);
	void DecodeChangePropertyFromDataBlock(DBReadSet& setReadDB);
private:
	////////////////////////////新的怪物属性////////////////////////
	tagMPro			m_BaseProperty;				//怪物基本属性
	tagMFPro		m_FightProperty;			//怪物战斗属性
	CMultiHpBar		m_MultiHpBar;				// 怪物的多重血条属性
	/////////////////////////////////////////////////////////////////
	//tagPetInfo		m_tPetInfo;					//宝宝属性
	BOOL			m_bDeadStatus;				//开始死亡了
	DWORD			m_dwDeadStartTime;			// 死亡时间记数(死亡后一定时间从client清除)
	bool			m_bAppearanceSoundPlayed;	// 是否已经播放了出现声音效果
	DWORD			m_dwPetLastRefreshTime;
	DWORD			m_dwPetLastDiedTime;

public:
	//tagPetInfo*		GetPetInfo()	{return &m_tPetInfo;}
//////////////////////////新定义的属性接口////////////////////////////////
////Monster基本属性	
	//种类索引编号
	DWORD	GetIndex()				{return m_BaseProperty.dwIndex;}
	void	SetIndex(DWORD dw)		{m_BaseProperty.dwIndex = dw;}
	//图形ID
	DWORD	GetPicID()				{return m_BaseProperty.dwPicID;}
	void	SetPicID(DWORD dw)		{ m_BaseProperty.dwPicID = dw;}
	//图形等级
	DWORD	GetPicLevel()			{return m_BaseProperty.dwPicLevel;}
	void	SetPicLevel(DWORD dw)	{m_BaseProperty.dwPicLevel = dw;}
	//名字颜色编号,对应到客户端配置
	DWORD	GetNameColor()			{return m_BaseProperty.dwNameColor;}
	void	SetNameColor(DWORD dw)	{m_BaseProperty.dwNameColor = dw;}
	//血条颜色，0：表示不显示血条 1：红色 2：黄色
	DWORD	GetHpBarColor()			{return m_BaseProperty.dwHpBarColor;}
	void	SetHpBarColor(DWORD dw)	{m_BaseProperty.dwHpBarColor = dw;}
	//声音ID
	DWORD	GetSoundID()			{return m_BaseProperty.dwSoundID;}
	void	SetSoundID(DWORD dw)	{m_BaseProperty.dwSoundID = dw;}
	//等级
	WORD	GetLevel()				{return m_BaseProperty.wLevel;}
	void	SetLevel(WORD w)		{m_BaseProperty.wLevel = w;}
	//经验值
	DWORD	GetExp()				{return m_BaseProperty.dwExp;}
	void	SetExp(DWORD dw)		{m_BaseProperty.dwExp = dw;}
	//国家ID
	BYTE	GetCountry()			{return m_BaseProperty.byCountry;}
	void	SetCountry(BYTE b)		{m_BaseProperty.byCountry = b;}
	//是否可以捕获
	BYTE	GetCatchable()			{return m_BaseProperty.byCatchable;}
	void	SetCatchable(BYTE b)	{m_BaseProperty.byCatchable = b;}
	//设置阻挡类型
	BYTE	GetBlockType()			{return m_BaseProperty.byBlockType;}
	void	SetBlockType(BYTE b)	{m_BaseProperty.byBlockType = b;}
	//怪物最大捕获抗性
	DWORD	GetMaxCatchFastness()		{return m_BaseProperty.dwMaxCatchFastness;}
	void	SetMaxCatchFastness(DWORD dw)	{m_BaseProperty.dwMaxCatchFastness = dw;}
	//国家贡献值
	WORD	GetCountryContriValue()	{return m_BaseProperty.wCountryContriValue;}
	void	SetCountryContriValue(WORD w)	{m_BaseProperty.wCountryContriValue = w;}
	//SP贡献值
	WORD	GetSPContriValue()		{return m_BaseProperty.wSPContriValue;}
	void	SetSPContrivalue(WORD w)	{m_BaseProperty.wSPContriValue = w;}
	//元素类别
	eEleType	GeteEleType()		{return m_BaseProperty.eEType;}
	void	SetEleType(eEleType l)	{m_BaseProperty.eEType = l;}
	//Npc类别 (NNPC,Monster,Guard)
	eNpcType	GeteNpcType()		{return m_BaseProperty.eNType;}
	void	SeteNpcType(eNpcType l)	{m_BaseProperty.eNType = l;}
	//种类
	eClass	GetClass()				{return m_BaseProperty.eClass_;}
	void	SetClass(eClass w)		{m_BaseProperty.eClass_ = w;}
	//族群(AI使用)
	WORD	GetRace()				{return m_BaseProperty.wRace;}
	void	SetRace(WORD w)			{m_BaseProperty.wRace = w;}
	//产生移动的概率(活力)
	WORD	GetMoveRandomValue()	{return m_BaseProperty.wMoveRandomValue;}
	void	SetMoveRandomValue(WORD w)	{m_BaseProperty.wMoveRandomValue = w;}
	//停留的时间(在一个格子中间停留帧数)
	WORD	GetStopTime()			{return m_BaseProperty.wStopTime;}
	void	SetStopTime(WORD w)		{m_BaseProperty.wStopTime = w;}
	//AI间隙
	//WORD	GetAIInterval()			{return m_BaseProperty.wAIInterval;}
	//void	SetAIInterval(WORD w)	{m_BaseProperty.wAIInterval = w;}
	//体形(0:1x1 1:3*3 2:5*5)
	BYTE GetFigure(eDIR dir=DIR_UP)	{return (BYTE)m_BaseProperty.wFigure;}
	void	SetFigure(WORD w)		{m_BaseProperty.wFigure = w;}
	//当前生命值
	DWORD	GetHp()					{return m_BaseProperty.dwHp;}
	void	SetHp(DWORD dw)			{m_BaseProperty.dwHp = dw;}
	//AI类型
	DWORD	GetAIType()				{return m_BaseProperty.dwAIType;}
	void	SetAIType(DWORD dw)		{m_BaseProperty.dwAIType = dw;}
	//重伤比率
	float	GetHurtProportion()		{return m_BaseProperty.fHurtProportion;}
	void	SetHurtProportion(float f)	{m_BaseProperty.fHurtProportion = f;}
	//警戒时间
	DWORD   GetGuardTime()			{return m_BaseProperty.dwGuardTime;}
	void	SetGuardTime(DWORD d)	{m_BaseProperty.dwGuardTime = d;}
	//逃跑底线（逃跑点)
	float   GetEscapePoint()		{return m_BaseProperty.fEscapePoint;}
	void	SetEscapePoint(float f)	{m_BaseProperty.fEscapePoint = f;}
	//技能概率,一个小于等于100，大于等于0的整型
	WORD    GetSkillProportion()	{return m_BaseProperty.wSkillProportion;}
	void	SetSkillProportion(WORD w)	{m_BaseProperty.wSkillProportion = w;}
	//技能间隔
	DWORD   GetSkillSpace()			{return m_BaseProperty.dwSkillSpace;}
	void	SetSkillSpace(DWORD dw)	{m_BaseProperty.dwSkillSpace = dw;}
	//AI参考
	DWORD   GetAIReferrence(int index)			{return m_BaseProperty.dwAIReferrence[index];}
	void	SetAIReferrence(int index,DWORD dw)	{m_BaseProperty.dwAIReferrence[index] = dw;}
	//boss级别
	WORD	GetBossLevel()						{return	m_BaseProperty.wBossLevel;}
	void	SetBossLevel(WORD w)				{m_BaseProperty.wBossLevel = w;}

////Monster战斗属性
	//生命上限
	DWORD	GetMaxHp()				{return m_FightProperty.dwMaxHp;}
	void	SetMaxHp(DWORD dw)		{m_FightProperty.dwMaxHp = dw;}
	//生命恢复速度
	WORD	GetHpRecoverSpeed()		{return m_FightProperty.wHpRecoverSpeed;}	
	void	SetHpRecoverSpeed(WORD w)		{m_FightProperty.wHpRecoverSpeed = w;}
	//最小攻击力
	WORD	GetMinAtk()				{return m_FightProperty.wMinAtk;}	
	void	SetMinAtk(WORD w)		{m_FightProperty.wMinAtk = w;}
	//最大攻击力
	WORD	GetMaxAtk()				{return m_FightProperty.wMaxAtk;}
	void	SetMaxAtk(WORD w)		{m_FightProperty.wMaxAtk = w;}
	//最小法术攻击力
	WORD	GetMinMAtk()			{return	m_FightProperty.wMinMAtk;}	
	void	SetMinMAtk(WORD w)		{m_FightProperty.wMinMAtk = w;}
	//最大法术攻击力
	WORD	GetMaxMAtk()			{return	m_FightProperty.wMaxMAtk;}	
	void	SetMaxMAtk(WORD w)		{m_FightProperty.wMaxMAtk = w;}
	//命中能力
	WORD	GetHit()				{return	m_FightProperty.wHit;}		
	void	SetHit(WORD w)			{m_FightProperty.wHit = w;}
	//防御力
	WORD	GetDef()				{return	m_FightProperty.wDef;}
	void	SetDef(WORD w)			{m_FightProperty.wDef = w;}
	//闪避能力
	WORD	GetDodge()				{return	m_FightProperty.wDodge;	}
	void	SetDodge(WORD w)		{m_FightProperty.wDodge = w;}
	//格挡
	WORD	GetFightBlock()			{return	m_FightProperty.wBlock;}
	void	SetFightBlock(WORD w)	{m_FightProperty.wBlock = w;}
	//招架
	WORD	GetParry()				{return m_FightProperty.wParry;}	
	void	SetParry(WORD w)		{m_FightProperty.wParry = w;}
	//攻击速度
	//WORD	GetAtcSpeed()			{return m_FightProperty.wAtcSpeed;}
	//void	SetAtcSpeed(WORD w)		{m_FightProperty.wAtcSpeed = w;}
	//最远攻击距离
	//WORD	GetFarAtkDis()			{return m_FightProperty.wFarAtkDis;}
	//void	SetFarAtkDis(WORD w)	{m_FightProperty.wFarAtkDis = w;}
	////最近攻击距离
	//WORD	GetNeaAtkDis()			{return	m_FightProperty.wNeaAtkDis;}	
	//void	SetNeaAtkDis(WORD w)	{m_FightProperty.wNeaAtkDis = w;}
	////攻击范围(以攻击目标为中心0:1x1 1:3*3 2:5*5)
	////WORD	GetAtkRange()			{return	m_FightProperty.wAtkRange;}	
	////void	SetAtkRange(WORD w)		{m_FightProperty.wAtkRange = w;}
	////警戒范围
	//WORD	GetGuardRange()			{return	m_FightProperty.wGuardRange;}
	//void	SetGuardRange(WORD w)	{m_FightProperty.wGuardRange = w;}
	////追击范围
	//WORD	GetChaseRange()			{return m_FightProperty.wChaseRange;}	
	//void	SetChaseRange(WORD w)	{m_FightProperty.wChaseRange = w;}
	//意志抗性
	WORD	GetWillDef()			{return	m_FightProperty.wWillDef;}	
	void	SetWillDef(WORD w)		{m_FightProperty.wWillDef = w;}
	//体质抗性
	WORD	GetConstiDef()			{return	m_FightProperty.wConstiDef;}
	void	SetConstiDef(WORD w)	{m_FightProperty.wConstiDef = w;}
	//物理爆击率
	WORD	GetCri()				{return m_FightProperty.wCri;}	
	void	SetCri(WORD w)			{m_FightProperty.wCri = w;}
	//物理爆击抗性
	WORD	GetCriDef()				{return	m_FightProperty.wCriDef;}
	void	SetCriDef(WORD w)		{m_FightProperty.wCriDef = w;}
	//魔法爆击抗性
	WORD	GetMCriDef()			{return m_FightProperty.wMCriDef;}	
	void	SetMCriDef(WORD w)		{m_FightProperty.wMCriDef = w;}
	//Debuff抗性
	WORD	GetDeBuffDef(eDefbuffType index)	{return	m_FightProperty.wDeBuffDef[index];}	
	void	SetDeBuffDef(eDefbuffType index,WORD w)	{m_FightProperty.wDeBuffDef[index] = w;}
	//各个元素抗性
	WORD	GetElemDef(eEleType	index)			{return m_FightProperty.wElemDef[index];}
	void	SetElemDef(eEleType	index,WORD w)	{m_FightProperty.wElemDef[index] = w;}
	//对各个种族伤害
	WORD	GetClassDam(eClass	index)			{return m_FightProperty.wClassDam[index];}	
	void	SetClassDam(eClass	index,WORD w)	{m_FightProperty.wClassDam[index] = w;}
	//对各个元素伤害
	WORD	GetElemDam(eEleType index)			{return	m_FightProperty.wElemDam[index];}	
	void	SetElemDam(eEleType	index,WORD w)	{m_FightProperty.wElemDam[index] = w;}

	// 设置和获得Monster能否对话的属性(对话值对应了对话动画图标，0表示不能对话)
	void SetIsCanTalk(WORD wIsTalk)		{m_BaseProperty.wCanTalk = wIsTalk;}
	WORD IsCanTalk()					{return m_BaseProperty.wCanTalk;}

	// 获得怪物的多重血条属性
	CMultiHpBar * GetMultiHpBar()		{return &m_MultiHpBar;}


	//当属性值改变以后出发
	virtual bool  OnPropertyChanged(const char* attrName);
	//得到属性值的字符串表示
	virtual const char* GetPropertyStringValue(const char* pszPropertyName);

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//	接口
//////////////////////////////////////////////////////////////////////////
public:
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,bool bExData = true);

	virtual void AI(void);

	// 退出战斗 特效
	void	RunLeaveFightAI( long xDest, long yDest, long time );
	void	AILeaveFight();
private:
	long	m_xLeaveFightDest;
	long	m_yLeaveFightDest;
	long	m_timeLeaveFight;
	long	m_timeLeaveFightStart;
	bool	m_bRestorColor;
};
