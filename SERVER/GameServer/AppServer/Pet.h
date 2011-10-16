// Fox@20081118----------------------------------------------
// File:    Pet.h
// Brief:   CPet定义了宠物的属性及操作. 现有接口暂未考虑可派生子类.
//			主要涉及宠物初始化及释放, 宠物事件管理及响应.
//
// Author:  Fox (yulefox@gmail.com)
// Date:    Nov.18, 2008
// Fox@20081118----------------------------------------------

#pragma once
#include "moveshape.h"
#include "PetCtrl.h"

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
	long  UpdateAttribute(BOOL bImmediately=0);
	long  OnUpdateAttributeTimeOut();

	virtual void OnSpeedChanged(void);
	virtual void SetPosition(LONG lX, LONG lY);
	virtual void SetSlipPos(LONG lX, LONG lY, float fSpeed, float fDir);
	virtual bool CodeToDataBlock_ForClient(DBWriteSet& setWriteDB,
		bool bExData = true);							// GS==>Client编码
	virtual bool CodeToDataBlock(DBWriteSet& setWriteDB,
		bool bExData = true);							// GS==>WS编码
	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB,
		bool bExData = true);							// WS==>GS解码
	virtual CVariableList* GetVariableList(void)		{ return m_pVariableList; }
	virtual DWORD GetExp()		{ return m_DiffAttrs.lExp; }
	virtual DWORD GetExpDead()	{ return 0; }			// 杀死得到的经验值
	virtual void SetHP(DWORD l) { SetAttr("Hp", l); }
	virtual void SetMP(DWORD l) { SetAttr("Mp", l); }
	virtual DWORD GetHP()		{ return m_DiffAttrs.lHp ? m_DiffAttrs.lHp : (GetState()!=STATE_DIED); }
	virtual WORD GetMP()		{ return m_DiffAttrs.lMp; }
	virtual DWORD GetMaxHP()	{ return m_TempAttrs.lMaxHp + m_BaseAttrs.lMaxHp; }	
	virtual DWORD GetMaxMP()	{ return m_TempAttrs.lMaxMp + m_BaseAttrs.lMaxMp; }	
	virtual WORD GetMinAtk()	{ return 0; }
	virtual WORD GetMaxAtk()	{ return 0; }
	virtual WORD GetHit()		{ return 0; }
	virtual WORD GetDef()		{ return 0; }
	virtual WORD GetAtcSpeed()	{ return 0; }
	virtual WORD GetHpRecoverSpeed()	{ return m_TempAttrs.lHpRecoverSpeed; }
	virtual WORD GetMpRecoverSpeed()	{ return m_TempAttrs.lMpRecoverSpeed; }
	virtual WORD SetHpRecoverSpeed()	{ return m_TempAttrs.lHpRecoverSpeed; }
	virtual WORD SetMpRecoverSpeed()	{ return m_TempAttrs.lMpRecoverSpeed; }

	virtual CMoveShape* GetHost(void);
	virtual long AddActiveSkill(DWORD dwSkillID,
		DWORD dwLevel,
		CMoveShape *pTarget = NULL,
		long lGoodsIndex = 0,
		long lContainerID = 0,
		long lPos = 0);
	virtual void SetSkillCooldown(long lID, DWORD dwTime);
	virtual WORD GetLevel(void) { return m_DiffAttrs.lLevel; }	// 获取宠物等级
	virtual bool IsVisible(OBJECT_TYPE eType,
		const CGUID& rID);								// 是否可被GUID为rID的MoveShape所见

	long InitialSkills(void);							// 创建时初始化技能
	bool UseSkill(tagSkill* pSkill,
		CMoveShape* pTarget = NULL);					// 使用技能
	void IncExp(long lDelta);							// 修正经验
	bool ModifySkill(long lID,
		long lLevel,
		long lPos);										// 修正技能
	tagSkill* GetSkill(long lPos);						// 获取技能
	void SetSkillHotKey(long lPos, long lActPos);		// 设置宠物技能快捷键
	void SetHost(CPlayer* pPlayer);
	long GetPetType(void) { return m_BaseAttrs.lPetType; }
	long GetPetIndex(void) { return m_BaseAttrs.lIndex; }
	void SetLevel(long lLevel);							// 设置宠物等级
	bool ModifyPos(INIT_TYPE lInitType);				// 修正坐标
	void SetMoveType(long lType);						// 设置行走模式
	void SetFightType(long lType);						// 设置攻击模式
	long GetMoveType(void);								// 获取行走模式
	long GetFightType(void);							// 获取攻击模式
	long CanBeSelected(void) { return m_BaseAttrs.lSelAtk & SAT_CAN_BESEL; }
	long CanBeAttacked(void) { return m_BaseAttrs.lSelAtk & SAT_CAN_BEATK; }
	long CanFight(void) { return m_BaseAttrs.lSelAtk & SAT_CAN_FIGHT; }
	bool NeedBeSaved(void)
	{
		return ( m_BaseAttrs.lPetType != PET_TYPE_TRAP &&
			m_BaseAttrs.lPetType != PET_TYPE_ORNA);
	}

	bool IsActive(void)
	{
		return (m_DiffAttrs.lActive > 0) ? true : false;
	}
	bool IsHangUp(void)
	{
		return (GetState() == STATE_HANGUP);
	}
	bool OnTrapSpring(void);							// 陷阱触发
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
		long lInitType);								// 初始化基本属性
	void InitialTempAttrs(void);						// 初始化临时属性
	bool InitialDiffAttrs(long lInitType);				// 初始化宠物的存盘或切服的保存属性
	bool InitialByTarget(CMonster* pTarget);			// 通过怪物初始化激活
	bool InitialByHost(INIT_TYPE lInitType);			// 初始化基本属性
	bool AddToRegion(void);								// 添加到场景中
	void InitNameValueMap(void);						// 初始化属性字段

	// 事件管理
	void RegEvent(PET_EVENT_TYPE eType,
		long lDelayTime = 0,
		long lIntervalTime = 0);						// 注册事件
	bool RegShadeEvent(void);							// 注册渐变事件
	bool RegAutoRenew(void);							// 注册自动恢复HP、MP等
	const CMoveShape* GetTarget(void);					// 返回宠物当前目标

	// 事件响应
	void OnHPRenew(void);								// 1) 战宠, 召唤兽的的HP
	void OnMPRenew(void);								// 2) 精灵, 战宠, 召唤兽的MP
	void OnGorgeRenew(void);							// 3) 精灵, 战宠的饱食度
	void OnGorgeUpdate(long lOld);						// 饱食度升级, 处理对应影响属性
	void OnLoyaltyRenew(void);							// 4) 战宠的忠诚度
	void OnLifeEnd(void);								// 5) 宠物生命终结
	void OnActive(void);								// 激活宠物
	void OnHouse(long lOpType);							// 收藏宠物, bActive为玩家编码时是否激活宠物标志
	void OnLevelUpdate(void);							// 宠物升级

	// 技能
	//bool IsSkillCooldown(long lID);						// 技能是否冷却
	bool HasSkill(long lID);							// 是否拥有技能
	bool ModifySkill(tagSkill& rSkill);					// 修正技能
	long AppendGeniusSkill(void);						// 追加一种天赋技能
	long AddGeniusSkill(void);							// 添加天赋技能
	long AddStudySkill(void);							// 添加插槽技能
	long AddConfigSkill(void);							// 添加配置技能；
	long OpenSkillHole(long lType = 0, long lPos = 0);	// 开启技能槽
	void CloseSkillHole(long lPos);						// 关闭技能槽
	CNewSkill* CanModifySkill(tagSkill& rSkill);		// 检查技能修正条件
	bool UpdateSkill(long lID, long lLevel);			// 更新技能
	tagSkill* AddSkill(tagSkill& rSkill);				// 添加技能
	bool AddSkill(CGoods* pGoods, long lPos);			// 添加技能
	long CanUseSkill(tagSkill* pSkill,
		CMoveShape* pTarget,
		long& lSkillType);								// 检查技能使用条件
	void StudySkillScript(long lID, long lLevel);		// 触发技能学习脚本
	void PetRunScript(const char *szScriptName);		// 宠物运行脚本

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

	long CalcGorgeLevel(void);							// 计算饱食度等级
	long Feed(long lSeconds, long lDelta);				// 喂食
	long GetIndexID(void) { return m_BaseAttrs.lIndex; }
	void SetIndexID(long lIndexID) { m_BaseAttrs.lIndex = m_DiffAttrs.lIndex = lIndexID; }
	void SetPetType(long lType) { m_BaseAttrs.lPetType = lType; }
	void SetActiveAttr(long lActiveType);
	void CancelAllTimer(void);

private:
	typedef map<string, tagPty>	ATTRS_MAP;
	typedef ATTRS_MAP::iterator ITR_ATTR_MAP; 

	// 移动
	long	m_lAICount;									// AI时间
	DWORD	m_dwMoveValiStartTime;						// 移动验证开始的计时
	DWORD	m_dwLastRecvMsgTime;						// 上次接受到移动消息的时间
	float	m_fMaxPeriodMoveSpeed;						// 一个时间段里的最大速度
	CGUID	m_guMoveValiStartRgnID;						// 开始验证的场景ID
	long	m_lMoveValiStartPosX;						// 开始移动的X位置
	long	m_lMoveValiStartPosY;						// 开始移动的Y位置
	float	m_fMoveValiStartSpeed;						// 移动验证开始速度
	float	m_fMoveValiTotalDistance;					// 移动的总距离
	float	m_fMoveValiRecvMsgCount;					// 收到的移动消息个数

	GROW_LIST			m_GrowCoefs;					// 原始属性成长系数
	tagPetBase			m_BaseAttrs;					// 基本属性
	tagPetDiff			m_DiffAttrs;					// 差异属性
	tagPetTemp			m_TempAttrs;					// 临时属性
	ATTRS_MAP			m_Attrs;						// 属性集
	set<string>			m_DirtyAttrs;					// 主人刷新的脏属性数据
	set<string>			m_BaseDirtyAttrs;				// 周围玩家刷新的脏属性数据
	string				m_strOriName;					// 原始名
	CGUID				m_HostID;						// 主人
	SKILL_LIST			m_Skills;						// 技能列表
	CPlayer*			m_pHost;						// 主人
	map<long, long>		m_Timers;						// 定时器集合
	CVariableList*		m_pVariableList;				// 宠物的私有脚本变量列表
}; 