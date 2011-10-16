/** 
* @file   Pet.h
* @author Fox (yulefox@gmail.com)
* @brief  CPet定义了宠物的属性及操作. 现有接口暂未考虑可派生子类.
			主要涉及宠物初始化及释放, 宠物事件管理及响应.
*
* @date   Nov.18, 2008       
*/

#pragma once

#pragma  warning(disable:4244)
#pragma  warning(disable:4018)
#pragma  warning(disable:4267)

#include "MoveShape.h"
#include "Mover.h"
#include "PetCtrl.h"
#include "../../../Public/Common/BasePetDef.h"

#define UNI_CONF \
	GetInst(CPetCtrl).GetPetConfig()
#define MY_CONF \
	GetInst(CPetCtrl).GetPetsConfig()

class CGoods;
class CPlayer;
class CMonster;

class CPet :
	public CMoveShape
{
public:
	CPet(void);
	~CPet(void);

	friend class CPetCtrl;

	const tagPetDiff& GetDiffPro(void) { return m_DiffAttrs; }	// 差异属性
	const tagPetTemp& GetTempPro(void) { return m_TempAttrs; }	// 临时属性

public:
	// 消息响应
	virtual void OnMessage(CMessage*);					// 网络消息响应
	virtual long OnTimeOut(DWORD timerid,
		DWORD curtime,
		const void* var);								// 超时响应
	virtual long Proc(eEventType type,
		const tagProcVar* pVar);						// 处理实体间消息函数

	void AddMoveMsg(float fNewX, float fNewY);			// 添加一个移动消息
	bool CheckMoveStep(float fCurX, float fCurY);		// 验证每一步的移动
	bool CheckMoveSpeedValidate(void);					// 移动速度验证
	void ResetMoveValiData(bool bAll = false);			// 重设相关数据
	virtual void InitAI(void);							// 初始化AI
	virtual void InitSkills(void);						// 初始化技能
	virtual void OnQuestMoveStep(float fCurX,
		float fCurY,
		float fDestX,
		float fDestY);									// 宠物移动
	virtual void OnQuestStopMove(float fCurX,
		float fCurY,
		short lDir);									// 宠物停止移动
	virtual void OnCannotMove(void);
	virtual long OnBeenAttacked(CMoveShape* pAttacker,
		long lHp);										// 宠物被攻击
	virtual void OnDied(void);							// 宠物死亡
	virtual bool IsAttackAble(CMoveShape* pAttacker);	// 可否攻击
	virtual void UpdateCurrentState(void);				// 更新状态
	virtual void ChangeStateTo(CShape::eSTATE eState);	// 改变状态
	virtual bool SetAttribute(std::string& attrName,
		long value);
	//virtual bool SetAttribute(char* attrName,
	//	long value);
	virtual long GetAttribute(std::string attrName);
	virtual bool ChangeAttribute(std::string& attrName,
		long value);
	virtual long UpdateAttributeToClient(bool bAll);	// 更新属性, true更新周围玩家, false仅更新主人
	/// 向其他客户端更新属性
	bool UpdateAttributeToOtherClient();

	long  UpdateAttribute(BOOL bImmediately=0);
	long  OnUpdateAttributeTimeOut();

	/// 当属性发生改变时候调用
	virtual void OnPropertyValueChanged(const char* strName, LONG lValue);

	virtual void OnSpeedChanged(void);
	virtual void SetPosition(LONG lX, LONG lY);
	virtual void SetSlipPos(LONG lX, LONG lY, float fSpeed, float fDir);
	virtual bool CodeToDataBlock_ForClient(DBWriteSet& setWriteDB,
		bool bExData = true);							// GS==>Client编码
	virtual bool CodeToDataBlock(DBWriteSet& setWriteDB,
		bool bExData = true);							// GS==>WS编码
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,
		bool bExData = true);							// WS==>GS解码

	/// 编码宠物属性给客户端，此属性只编码给宠物主人客户端
	bool CodeAttrToClient(DBWriteSet& setWriteDB);

	virtual DWORD GetExp()		{ return m_DiffAttrs.lExp; }
	virtual DWORD GetExpDead()	{ return 0; }			// 杀死得到的经验值
	virtual void SetHP(DWORD l) { SetAttr("Hp", l); }
	virtual void SetMP(DWORD l) { SetAttr("Mp", l); }
	virtual DWORD GetHP()		{ return m_DiffAttrs.lHp ? m_DiffAttrs.lHp : (GetState()!=STATE_DIED); }
	virtual WORD GetMP()		{ return m_DiffAttrs.lMp; }
	virtual DWORD GetEnergy()	{ return m_DiffAttrs.lEnergy; }
	virtual DWORD GetMaxHP()	{ return m_TempAttrs.lMaxHp; }	
	virtual DWORD GetMaxMP()	{ return m_TempAttrs.lMaxMp; }
	virtual DWORD GetMaxEnergy(){ return m_TempAttrs.lMaxEnergy;}

	long GetLoyalty(void)		{ return m_DiffAttrs.lLoyalty; }
	long GetMaxLoyalty(void)	{ return UNI_CONF.loyaltyMax; }
	long GetLife(void)			{ return m_DiffAttrs.lLife; }
	long GetMaxLife(void)		{ return UNI_CONF.maxLife; }

	/// 取得HP回复速度
	virtual WORD GetHpRecoverSpeed()	{ return m_BaseAttrs.lHpRecoverSpeed; }
	/// 取得MP回复速度
	virtual WORD GetMpRecoverSpeed()	{ return m_BaseAttrs.lMpRecoverSpeed; }
	/// 取得内力回复速度
	virtual WORD GetEnergyRecoverSpeed()	{ return m_BaseAttrs.lEnergyRecoverSpeed; }

	virtual CMoveShape* GetHost(void);
	virtual long AddActiveSkill(DWORD dwSkillID,
		DWORD dwLevel,
		CMoveShape *pTarget = NULL,
		long lGoodsIndex = 0,
		long lContainerID = 0,
		long lPos = 0);
	virtual void SetSkillCooldown(long lID, DWORD dwTime);
	virtual WORD GetLevel(void) { return m_DiffAttrs.lLevel; }	// 获取宠物等级
	virtual WORD GetDLevel(void) {return m_DiffAttrs.ServantAttr.lDLevel;} ///< 获取宠物道行等级
	virtual bool IsVisible(OBJECT_TYPE eType,
		const CGUID& rID);								// 是否可被GUID为rID的MoveShape所见

	long InitialSkills(void);							// 创建时初始化技能
	bool UseSkill(tagSkill* pSkill,
		CMoveShape* pTarget = NULL);					// 使用技能

	bool ModifySkill(long lID,
		long lLevel,
		long lPos);										// 修正技能

	/// 添加技能(由技能物品镶嵌)
	bool AddSkill(CGoods* pGoods, long lPos, long lHoleType);

	/// 获取技能
	tagSkill* GetSkill(long lPos);

	/// 设置宠物技能快捷键
	void SetSkillHotKey(long lPos, long lActPos);

	/// 取得宠物类型
	long GetPetType(void) { return m_BaseAttrs.lPetType; }
	/// 设置宠物类型
	void SetPetType(long lType) { m_BaseAttrs.lPetType = m_TempAttrs.lPetType =  lType; }

	///// 取得宠物图档编号
	//long GetPicID(void) { return m_BaseAttrs.lPicID; }
	///// 设置宠物图档编号
	//void SetPicID(long lPicID) { m_BaseAttrs.lPicID = lPicID; }

	/// 取得宠物索引编号
	long GetIndexID(void) { return m_BaseAttrs.lIndex; }
	/// 设置宠物索引编号
	void SetIndexID(long lIndexID) { m_BaseAttrs.lIndex = m_DiffAttrs.lIndex = lIndexID; }

	/// 取得宠物名称
	const char* GetPetName(void) { return m_DiffAttrs.szName; }
	/// 设置宠物名称
	void SetPetName(const char* name) { strcpy_s(m_DiffAttrs.szName, 32, name); }
	/// 设置主人名称
	void SetHostName(const char* name) { strcpy_s(m_DiffAttrs.szHostName, 32, name); }

	/// 设置主人
	void SetHost(CPlayer* pPlayer);

	void SetLevel(long lLevel);							// 设置宠物等级
	bool ModifyPos(INIT_TYPE lInitType);				// 修正坐标

	/// 设置行走模式
	void SetMoveType(long lType);
	/// 设置攻击模式
	void SetFightType(long lType);
	/// 获取行走模式
	long GetMoveType(void);
	/// 获取攻击模式
	long GetFightType(void);
	/// 设置状态模式
	void SetStateType(long lType);
	/// 获取状态模式
	long GetStateType(void);
	/// 转换状态模式
	void ChangeStateType(long lNewState);

	long CanBeSelected(void) { return m_BaseAttrs.lSelAtk & SAT_CAN_BESEL; }
	long CanBeAttacked(void) { return m_BaseAttrs.lSelAtk & SAT_CAN_BEATK; }
	long CanFight(void) { return m_BaseAttrs.lSelAtk & SAT_CAN_FIGHT; }
	bool NeedBeSaved(void)
	{
		return IsServant();
	}

	/// 是否野生
	bool IsWild(void) { return m_bWild; }
	/// 设置野生标志
	void SetWild(bool b) { m_bWild = b; }
	/// 是否激活状态
	bool IsActive(void)
	{
		return (m_DiffAttrs.lStateType ==  PET_STATE_ACTIVE) ? true : false;
	}
	/// 是否挂起
	bool IsHangUp(void)
	{
		return (GetState() == STATE_HANGUP);
	}

	bool DelFromRegion(long lOpType);					// 從场景中刪除
	void SetTarget(CMoveShape* pTarget = NULL);			// 设置宠物当前目标
	void SaveSkillSetting(long lPos,
		long lActPos,
		bool bAuto);									// 保存客户端技能设置

private:
	// 初始化及释放
	void InitialOrigAttrs(long lOldLevel,
		long lCurLevel);								// 初始化原始属性
	bool InitialBaseAttrs(const tagPetBase& rPet,
		long lInitType,long IsWild);								// 初始化基本属性
	void InitialTempAttrs(void);						///< 初始化临时属性
	bool InitialDiffAttrs(long lInitType, bool IsWild);				// 初始化宠物的存盘或切服的保存属性
	//bool InitialByTarget(CMonster* pTarget);			// 通过怪物初始化激活
	bool InitialByHost(INIT_TYPE lInitType);			// 初始化基本属性
	bool AddToRegion(void);								// 添加到场景中
	void InitNameValueMap(void);						// 初始化属性字段
	void InitialFightProperty(void);					// 设置战斗属性

	// 事件管理
	void RegEvent(PET_EVENT_TYPE eType,
		long lDelayTime = 0,
		long lIntervalTime = 0);						// 注册事件
	bool RegShadeEvent(void);							// 注册渐变事件
	bool RegAutoRenew(void);							// 注册自动恢复HP、MP等
	const CMoveShape* GetTarget(void);					// 返回宠物当前目标

	// 事件响应
	void OnHPRenew(void);								///< 侍从的HP刷新
	void OnMPRenew(void);								///< 侍从的MP刷新
	void OnEnergyRenew(void);							///< 侍从的内力刷新
	void OnLoyaltyRenew(void);							///< 侍从的忠诚度
	void OnLifeRenew(void);								///< 侍从的寿命

	void OnLifeEnd(void);								// 5) 宠物生命终结
	void OnActive(void);								// 激活宠物
	void OnHouse(long lOpType);							// 收藏宠物, bActive为玩家编码时是否激活宠物标志
	/// 创建宠物
	void OnCreate(void);

	// 技能
	/// 是否拥有技能
	bool HasSkill(long lID);

	/// 添加技能
	tagSkill* AddSkill(tagSkill& rSkill);

	/// 技能镶嵌条件判断
	bool CanAddSkill(CGoods* pGoods);

	/// 检查技能使用条件
	long CanUseSkill(tagSkill* pSkill,
		CMoveShape* pTarget,
		long& lSkillType);

	bool ModifySkill(tagSkill& rSkill);					///< 修正技能
	//long AppendGeniusSkill(void);						///< 追加一种天赋技能
	//long AddGeniusSkill(void);							///< 添加天赋技能
	//long AddStudySkill(void);							///< 添加插槽技能
	//long AddConfigSkill(void);							///< 添加配置技能；
	//long OpenSkillHole(long lType = 0, long lPos = 0);	///< 开启技能槽
	//void CloseSkillHole(long lPos);						///< 关闭技能槽
	CNewSkill* CanModifySkill(tagSkill& rSkill);		///< 检查技能修正条件
	//bool UpdateSkill(long lID, long lLevel);			///< 更新技能


	void StudySkillScript(long lID, long lLevel);		///< 触发技能学习脚本
	void PetRunScript(const char *szScriptName);		///< 宠物运行脚本

	long GetAttr(const char* szName);
	long SetAttr(const char* szName, long lVal);
	long ChgAttr(const char* szName, long lDelta);
	void AddDirtyAttr(const char* szName);				// 脏属性数据标记

	void CodeChgAttr(DBWriteSet& setWriteDB,
		short sEnum,
		char cType,
		long lValue);									// 编码属性
	void CodeChgAttrs(DBWriteSet& setWriteDB,
		set<string>& rAttrs);							// 编码脏属性

	/// 
	void SetActiveAttr(long lActiveType);

	/// 注消所有定时器
	void CancelAllTimer(void);

	/// 计算忠诚度等级
	long CalcLoyLevel(void);

	/// 新增侍从操作集
public:
	/// 宠物类型是否是侍从
	bool IsServant(void);

	/// 天资重置算法
	bool OnReSetTalent(void);

	/// 后天资质强化检测
	bool DAfterValCheck(void);

	/// 后天资质强化
	bool OnDAfterValUpGrade(long DVal);

	/// 重置后天资质强化次数
	bool OnResetDAfterCnt(void);

	/// 侍从融合
	bool OnServantSyncretism(void);

	/// 侍从融合算法
	void CopyServantAttr(const CPet* pPet, const CPet* lPet, long lPLev);

	/// 融合计算公式<以后整理>
	void CALC_ELF_SYNCRETISM(const CPet* pPet, const CPet* lPet, long lPLev);

	/// 分配悟性点
	bool  SetSawyPointValue(const tagPetBaseAttr& rtag);

	/// 重置悟性点
	bool OnResetSavvy(void);

	/// 魂石强化条件判断
	bool OnFStoneUpgradeCheck(void);

	/// 魂石强化
	bool OnFStoneUpgrade(long fstone);

	/// 顿悟神位强化条件检测
	bool OnlFUpgradeCheck(void);

	/// 顿悟神位强化 
	bool OnlFUpgrade(long rate);

	/// 根据宠物忠诚度强化成功率的百分比
	int GetDownRate(int drop);

	/// 星等强化条件检测
	bool OnStarLevUpGradeCheck(void);

	/// 星等强化
	bool OnStarLevUpGrade(long rates);

	/// 品质强化算法条件判断
	bool OnCElfPensValueUpGradeCheck(void);

	/// 品质强化算法算法
	bool OnCElfPensValueUpGrade(long val);

	/// 取得品质强化所需虚空精华数目
	long GetLessNice();

	/// 改变忠诚度
	void OnChangeLoy(long lDelta);

	/// 改变寿命值
	void OnChangeLife(long lDelta);

	/// 最大精灵携带数的增加
	long AddServantTakeNum(long num);

	/// 得到Lev需要的升级经验
	inline	long GetALevNeedExp(long  _lev);

	///  获得每次升级基本属性的提升量
	inline long	GetUpGradeUpValue(long _talentval , long _BaseVal);

	/// 召唤精灵, 判断其条件
	bool VacationPet(long lev);

	/// 取得宠物品级
	long GetGrade(void);

	/// 取得宠物后天资质已强化次数
	long GetDAfterCnt(void);

	/// 强化接口
	bool OnServantUpGrade(long uId , long uRate);

	/// 宠物升级
	bool OnUpgradeLev(void);

	/// 宠物提升道行等级
	bool OnUpgradeDLev(void);

	/// 宠物获取经验
	void AddExp(CMonster* pMonster);

	/// 宠物获取修为经验
	void AddDExp(CMonster* pMonster);
private:
	/// 修正经验
	void IncExp(long lDelta);
	/// 修正修为经验
	void IncDExp(long lDelta);
	/// 随机天资算法
	long GetBaseTalentRadomVal(void);
	/// 品质随机算法
	long GetBaseQualityRadomVal(void);

private:
	typedef map<string, tagPty>	ATTRS_MAP;
	typedef ATTRS_MAP::iterator ITR_ATTR_MAP; 

	// 移动
	long	m_lAICount;									///< AI时间
	DWORD	m_dwMoveValiStartTime;						///< 移动验证开始的计时
	DWORD	m_dwLastRecvMsgTime;						///< 上次接受到移动消息的时间
	float	m_fMaxPeriodMoveSpeed;						///< 一个时间段里的最大速度
	CGUID	m_guMoveValiStartRgnID;						///< 开始验证的场景ID
	long	m_lMoveValiStartPosX;						///< 开始移动的X位置
	long	m_lMoveValiStartPosY;						///< 开始移动的Y位置
	float	m_fMoveValiStartSpeed;						///< 移动验证开始速度
	float	m_fMoveValiTotalDistance;					///< 移动的总距离
	float	m_fMoveValiRecvMsgCount;					///< 收到的移动消息个数

	GROW_LIST			m_GrowCoefs;					///< 原始属性成长系数
	tagPetBase			m_BaseAttrs;					///< 基本属性
	tagPetDiff			m_DiffAttrs;					///< 差异属性
	tagPetTemp			m_TempAttrs;					///< 临时属性
	ATTRS_MAP			m_Attrs;						///< 属性集
	set<string>			m_DirtyAttrs;					///< 主人刷新的脏属性数据
	set<string>			m_BaseDirtyAttrs;				///< 周围玩家刷新的脏属性数据
	string				m_strOriName;					///< 原始名
	CGUID				m_HostID;						///< 主人
	SKILL_LIST			m_Skills;						///< 技能列表
	CPlayer*			m_pHost;						///< 主人
	map<long, long>		m_Timers;						///< 定时器集合
	bool				m_bWild;						///< 是否野生标志
public:
    Mover &GetMover() { return m_mover; }
private:
    Mover m_mover;
}; 