#pragma once
class CSkillAI;
class CMoveShape;
class CPlayer;
#include "Shape.h"
#include "SkillListXml.h"
#include "SkillAI.h"
#include "SkillDecorator.h"

///技能的装饰类
class CSkillXml : public CShape,public SkillComponent
{
private:
	CSkillXml();
public:
	CSkillXml(SkillComponent* com);
	
	virtual ~CSkillXml(void);
protected:
	SkillComponent* m_com;//指向被装饰类
public:
	virtual void AI(void);
	virtual bool Display(void);
	/////////////////////////////
	//SkillComponent的接口

	///执行技能开始步骤
	bool StepBegin(long lID, long lLevel, 
		long sType, const CGUID& sID,
		long lDestX, long lDestY, float fDir,
		vector<long> vType,
		vector<CGUID> vID);
	/// 执行发送技能消息步骤
	bool virtual StepSend(DWORD dwID,DWORD dwLevel,
		CMoveShape *sShape, CMoveShape *dShape,
		long dX, long dY);
	///执行技能实施步骤
	bool  StepRun(long lID,long lLevel,
		long sType,const CGUID& sID,
		long dX,long dY,float fDir,
		vector<long>vType,vector<CGUID>vID);
	///执行技能结束步骤
	bool  StepEnd(long lID,long lLevel,
		long sType,const CGUID& sID,float fDir);
	///执行技能回收步骤
	bool  StepSummon();
	/// 设置技能键值
	void SetSkillKey(long lSkillKey){m_lSkillKey = lSkillKey;}
	//////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// 工具函数
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// 设置吟唱时间
	void virtual IntonateTime(long lTime);
	/// 取得施放者编号
	virtual const CGUID& GetSourID(){return m_SourID;}
	/// 取得所有召唤特效编号
	virtual vector<long> GetSummonEffect(){return m_vecSummonEffect;}
	/// 取得所有召唤声效
	virtual vector<long> GetSummonSound(){return m_vecSummonSound;}
	///取得召唤盛名周期时间
	virtual DWORD GetSummonLifeTime() { return m_lSummonLifeTime ;}
	/// 取得施放者类型
	virtual long GetSourType(){return m_lSourType;}
	
	/// 取得技能键值
	virtual long GetSkillKey(){return m_lSkillKey;}

	/// 取得施放者shape对象
	virtual CMoveShape* GetSourMoveShape(){return m_pMoveShape;}
	
	/// 取得目标shape对象
	virtual vector<CMoveShape*> GetDestMoveShape(){return m_vecDestMoveShape;}

	/// 取得目标者类型
	virtual long GetDestType(){return m_lDestType;}

	/// 取得目标编号
	virtual const CGUID& GetDestID(){return m_DestID;}

	/// 取得目标者位置x
	virtual long GetDestX(){return m_lDestX;}

	/// 取得目标者位置y
	virtual long GetDestY(){return m_lDestY;}

	/// 取得技能编号
	virtual long GetSkillID(){return m_lSkillID;}	

	/// 取得技能等级
	virtual long GetSkillLevel(){return m_lSkillLevel;}

	/// 取得飞行特效起始位置
	virtual vector<eLOCATOR> GetFlyEffectLoc(){return m_vecFlyEffectPos;}

	/// 取得飞行特效飞行时间
	virtual vector<long> GetFlyTime(){return m_vecFlyTime;}

	/// 取得飞行特效延迟时间
	virtual vector<long> GetFlyDelay(){return m_vecFlyDelay;}

	/// 取得所有的飞行特效编号
	virtual vector<long> GetFlyEffect(){return m_vecFlyEffect;}

	/// 取得所有飞行特效索引
	virtual vector<long> GetFlyEffectIndex(){return m_vecFlyEffectIndex;}
	
	/// 取得飞行特效X轴方向偏移角度队列
	virtual vector<long> GetFlyEffectTurnX(){return m_vecFlyEffectTurnX;}

	/// 取得飞行特效Y轴方向偏移角度队列
	virtual vector<long> GetFlyEffectTurnY(){return m_vecFlyEffectTurnY;}

	/// 取得飞行特效是否循环播放的标志
	virtual bool IsFlyEffectLoop()	{return m_bFlyEffectCycle;}


	/// 取得随机召唤技能X轴范围
	virtual long GetRandomSummonX(){return m_lRandomSummonX;}

	/// 取得随机召唤技能Y轴范围
	virtual long GetRandomSummonY(){return m_lRandomSummonY;}

	/// 取得随机召唤技能召唤物数量
	virtual long GetRandomSummonNum(){return m_lRandomSummonNum;}

	/// 取得技能的吟唱时间
	virtual long GetIntonateTime(){return m_lIntonateTime;}

	/// 取得吟唱动作特效ID
	virtual long GetIntonateEffect(){return m_lIntonateEffect;}

	/// 获得配置的飞行特效的个数
	virtual long GetFlyEffectNum(){return m_lFlyEffectNum;}

	/// 获得配置的技能特效绑定位置
	virtual eLOCATOR GetReleaseEffectPosition()	{return m_eReleaseEffectPosition;}

	/// 设置吟唱阶段主要循环动作
	virtual WORD GetIntonateMainAct()	{return m_wIntonateMainAct;}

	/// 设置随机召唤技能召唤物的延迟播放时间
	virtual long GetRandomSummonDelayTime()	{return m_lRandomSummonDelayTime;}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// 技能函数
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	
	
	/// 判断目标对象
	virtual BOOL SkillObject(long l);
	
	/// 判断强制目标对象
	virtual BOOL SkillForceObject(long l);
	
	/// 判断是否可以对非pvp角色发动技能
	virtual BOOL FirstAtt(long l);
	
	/// 判断要施放技能所需的buff
	virtual BOOL SkillStarBuff(long l);
	
	/// 判断施放技能时玩家必须处于何种游戏状态
	virtual BOOL SkillStarState(long l);
	
	/// 设置要施放技能所需装备的id
	virtual void SkillStarEquipId(long l);
	
	/// 设置要施放技能所需装备需放在何处
	virtual void SkillStarEquipPlace(long l);
	
	///	判断要施放技能必须的装备条件
	virtual BOOL SkillStarEquipUse();

	/// 设置施放技能所需道具id
	virtual void SkillItemId(long l);

	/// 设置施放技能所需道具需要的数量
	virtual void SkillItemNum(long l);

	/// 设置施放技能所需道具需放在何处
	virtual void SkillItemPlace(long l);

	/// 判断施放技能所需道具条件
	virtual BOOL SkillUseItem();

	/// 判断施放技能所需hp
	virtual BOOL SkillHp(long l);

	/// 判断施放技能所需mp
	virtual BOOL SkillMp(long l);

	/// 判断施放技能所需能量
	virtual BOOL SkillEnergy(long l);
	
	/// 判断吟唱最小施放距离
	virtual BOOL IntonateLeastSpace(long l);

	/// 判断吟唱最大施放距离
	virtual BOOL IntonateMostSpace(long l);

	/// 判断施放最小施放距离
	virtual BOOL FireLeastSpace(long l);

	/// 判断施放最大施放距离
	virtual BOOL FireMostSpace(long l);
protected:

	/// 设置吟唱阶段主要循环动作
	void IntonateMainAct(WORD l);
	
	/// 设置吟唱动作ID
	void IntonateAct(long l);

	/// 设置吟唱动作特效ID
	void IntonateEffect(long l);

	/// 设置吟唱特效索引
	void IntonateEffectIndex(long l);

	/// 吟唱特效是否与武器相关
	void IntonateEffectByEquip(long l);

	/// 设置吟唱特效绑定位置
	void IntonateEffectPosition(long l);

	/// 设置吟唱特效是否循环
	void IntonateEffectCycle(long l);
	
	/// 开始吟唱
	void IntonatePlay();

	/// 设置吟唱声效id
	void IntonateSound(long l);

	/// 吟唱音效是否与武器相关
	void IntonateSoundByEquip(long l);

	/// 设置吟唱声效是否循环
	void IntonateSoundCycle(long l);

	/// 开始播放吟唱声效
	void IntonateSoundPlay();

	/// 设置是否可以被打断
	void IntonateBreak(long l);

public:

	/// 执行技能实施步骤
	//virtual bool StepRun(long lID, long lLevel, long sType,const CGUID& sID,long dX, long dY,  float fDir, vector<long> vType, vector<CGUID> vID);

	/// 设置施放动作ID
	virtual void ReleaseAct(long l);

	/// 设置施放后续动作ID
	virtual void ReleaseActEx(long l);

	/// 设置施放动作特效id
	virtual void ReleaseEffect(long l);

	/// 设置动作特效索引
	virtual void ReleaseEffectIndex(long l);

	/// 施放特效是否与武器相关
	virtual void ReleaseEffectByEquip(long l);

	/// 设置施放特效目标
	virtual void ReleaseEffectDest(long l);

	/// 设置施放特效绑定位置
	virtual void ReleaseEffectPosition(long l);

	/// 设置施放特效是否循环
	virtual void ReleaseEffectCycle(long l);

	/// 开始施放动作
	virtual void ReleasePlay();

	/// 设置施放声效id
	virtual void ReleaseSound(long l);

	/// 施放音效是否与武器相关
	virtual void ReleaseSoundByEquip(long l);

	/// 设置施放声效是否循环
	virtual void ReleaseSoundCycle(long l);

	/// 设置施放声效目标
	virtual void ReleaseSoundDest(long l);

	/// 播放施放声效
	virtual void ReleaseSoundPlay();
	
	/// 震动频率(百分比)
	virtual void ShockRatio(long l);

	/// 震动间隔时
	virtual void ShockInterval(long l);

	/// 震动时间长度
	virtual void ShockTimeLength(long l);

	/// 开始震动
	virtual void ShockPlay();

	/// 设置飞行特效id
	virtual void FlyEffect(long l);

	/// 设置飞行特效索引
	virtual void FlyEffectIndex(long l);

	/// 飞行特效是否与武器相关
	virtual void FlyEffectByEquip(long l);

	/// 设置飞行特效从何处发出
	virtual void FlyEffectPosition(long l);

	/// 设置飞行特效是否循环播放
	virtual void FlyEffectCycle(long l);

	/// 设置飞行特效X轴方向偏移角度
	virtual void FlyEffectTurnX(long l);

	/// 设置飞行特效Y轴方向偏移角度
	virtual void FlyEffectTurnY(long l);

	/// 开始飞行动作
	virtual void FlyPlay();

	/// 设置飞行音效id
	virtual void FlySound(long l);

	/// 飞行音效是否与武器相关
	virtual void FlySoundByEquip(long l);

	/// 设置飞行音效从何处释放
	virtual void FlySoundPosition(long l);

	/// 设置飞行音效是否循环
	virtual void FlySoundCycle(long l);

	/// 开始播放飞行音效
	virtual void FlySoundPlay();

	/// 设置飞行速度(飞行一个格子所用的时间)
	virtual void FlyTime(long l);

	/// 设置飞行延迟时间
	virtual void FlyDelay(long l);

	/// 飞行特效的个数
	virtual void FlyEffectNum(long l);

	/// 设置召唤技能特效
	virtual void SummonEffect(long l);

	/// 设置召唤声效
	virtual void SummonSound(long l);

	/// 设置召唤生命周期
	virtual void SummonLifeTime(long l);

	/// 设置随机召唤技能X轴范围
	virtual void RandomSummonX(long l);

	/// 设置随机召唤技能Y轴范围
	virtual void RandomSummonY(long l);

	/// 设置随机召唤技能召唤物数量
	virtual void RandomSummonNum(long l);

	/// 设置随机召唤技能召唤物的延迟播放时间
	virtual void RandomSummonDelayTime(long l);

	/// 设置是否显示挥动武器的光影效果
	virtual void WeaponEffect(long l);

	/// 改变指定的属性
	virtual void ChangeProperty(long lvalue, const string& strValue);

	/// 设置当前动作的规定时间
	virtual void SetActTime(DWORD dwTime);

	/// 获得
	virtual queue<WORD>* GetRunStepActExQueue()	{return &m_queueRunStepActEx;}

	/// 设置和获得释放阶段的最后一个后继动作是否是循环播放
	virtual void SetActExIsCycle(BOOL bIsCycle)	{m_bIsLastActExCycle = bIsCycle;}

	virtual BOOL IsActExCycle()					{return m_bIsLastActExCycle;}

protected:

	/// 执行技能结束步骤
	//bool StepEnd(long lID, long lLevel, long sType, const CGUID& sID, float fDir);

	/// 设定删除特效
	void DelLocEffect(long l);

	/// 设定删除特效索引
	void DelLocEffectIndex(long l);

	/// 开始删除特效
	void StartDelEffect(long l);

//=================07.12.20刘科===============
// 各个阶段的特效的相关结构
public:
	/// 方向
	enum eSTEP
	{
		STEP_START = 0,
		STEP_RUN,
		STEP_END,
		STEP_NUM,
	};
	/// 某一确定特效的索引结构，可以通过其中的一个值来确定一个特效
	struct tagEffectIndex
	{
		long lEffectIndex;			/// 配置的特效索引
		CGUID guidEffect;			/// 特效的GUID
		tagEffectIndex()
		{
			lEffectIndex = -1;
			guidEffect = CGUID::GUID_INVALID;
		}
	};
	/// 特效相关对象的结构（该对象可能为场景、shape等，通过类型和GUID来区分）
	struct tagObjectEffect
	{
		long lType;			/// 对象类型
		CGUID guid;			/// 对象的GUID
		list<tagEffectIndex> lsEffectIndex; /// 特效列表
		tagObjectEffect()
		{
			lType = 0;
			guid = CGUID::GUID_INVALID;
		}
	};
public:
	/// 取得当前技能中，特定阶段的指定对象的特效列表
	virtual list<tagEffectIndex> *GetObjectEffectList(long lStep, long lType, CGUID guid);
	/// 取得当前技能中，指定阶段的所有特效对象列表
	virtual vector<tagObjectEffect> *GetEffectIndex(long lStep);

	/// 删除当前技能中，特定阶段的指定对象的特效列表
	virtual bool DelObjectEffect(long lStep, long lTypeObj, CGUID guidObj);

	/// 删除当前技能，在指定阶段的指定对象的所有特效
	virtual bool DelAllEffectListByObject(long lStep, long lTypeObj, CGUID guidObj);

	/// 删除当前技能，在指定阶段的所有对象的所有特效
	virtual bool DelAllEffectListByStep(long lStep);

	/// 获得当前技能所在的阶段
	virtual long GetCurStep()	{return m_lStep;}

	/// 设置循环特效延迟删除时间
	virtual void SetCycleEffectDelayTime(long l)	{m_lCycleEffectDelayTime = l;}
///============================================

private:

	/// 执行所有技能步骤
	BOOL ExecuteStep(int iWordKey, long lValue, const string& strValue);

	/// 判断哪些职业可以使用当前技能
	BOOL SkillOccType(CSkillListXml::tagSkill *pSkill, CMoveShape *sShape);

	CMoveShape *m_pLauncher;		/// 技能发送消息阶段的施放者
	CShape *m_pDestShape;		/// 技能发送消息阶段的目标者
	CMoveShape *m_pMoveShape;	/// 所有类型的施放者（怪物、玩家、其他）
	vector<CMoveShape*> m_vecDestMoveShape;/// 所有类型的目标（怪物、玩家、其他）

	vector<tagObjectEffect> m_vecEffectIndex[STEP_NUM];		/// 对象的特效索引列表（分成三个阶段）

	long m_lStep;				/// 该技能当前所处的阶段（0：开始，1：运行，2：结束）

	long m_lSourType;			/// 施放者类型
	CGUID m_SourID;				/// 施放者编号
	long m_lDestType;			/// 目标者类型
	CGUID m_DestID;				/// 目标者编号
	long m_lDestX;				/// 目标x位置
	long m_lDestY;				/// 目标y位置
	long m_lSkillKey;			/// 技能键值
	long m_lSkillID;			/// 技能编号
	long m_lSkillLevel;			/// 技能等级

	CSkillAI *m_pSkillAI;			/// 技能AI类
	float m_fDir;				/// 施放者面对的方向

	vector<long> m_vecStarEquipId;		/// 所需装备id列表
	vector<long> m_vecStarEquipPlace;	/// 所需装备放置的位置列表
	
	long m_lItemId;				/// 所需物品id
	long m_lItemNum;			/// 所需物品数
	long m_lItemPlace;			/// 所需物品放置的位置

	//====================================================================================///

	long m_lIntonateTime;		/// 吟唱时间（07.12.11刘科添加，对应索引为72）
	WORD m_wIntonateMainAct;	/// 吟唱阶段主要动作ID

	long m_lIntonateEffect;		/// 吟唱特效
	long m_lIntonateEffectIndex;/// 吟唱特效索引
	bool m_bIntonateEffectByEquip;	/// 吟唱特效是否与武器相关
	bool m_bIntonateEffectCycle;/// 吟唱是否循环
	eLOCATOR m_eIntonateEffectPosition;	/// 吟唱绑定的位置

	long m_lIntonateSound;		/// 吟唱声音
	bool m_bIntonateSoundByEquip; /// 吟唱音效是否与武器相关
	bool m_bIntonateSoundCycle;	/// 吟唱声音是否循环
	int m_IntonateSoundChannel; ///呤唱声音channel

	///====================================================================================///

	queue<WORD>	m_queueRunStepActEx;	/// 释放阶段的后续动作列表
	BOOL		m_bIsLastActExCycle;	/// 最后一个释放阶段后继动作是否循环

	long m_lReleaseEffect;		/// 施放特效
	long m_lReleaseEffectIndex;	/// 施放特效索引
	bool m_bReleaseEffectByEquip;	/// 施放特效是否与武器相关
	long m_lReleaseEffectDest;	/// 施放特效目标
	eLOCATOR m_eReleaseEffectPosition;	/// 施放特效绑定位置
	bool m_bReleaseEffectCycle;	/// 施放特效是否循环

	long m_lReleaseSound;		/// 施放特效声音
	bool m_bReleaseSoundByEquip;	/// 施放音效是否与武器相关
	bool m_bReleaseSoundCycle;	/// 施放特效声音是否循环
	long m_lReleaseSoundDest;	/// 施放声效目标

	float m_fShockRatio;		/// 震动频率
	long m_lShockInterval;		/// 震动间隔
	long m_lShockTimeLength;	/// 震动时间长度

	long m_lFlyEffectNum;		/// 飞行特效的个数
	long m_lFlyEffect;			/// 飞行特效
	long m_lFlyEffectIndex;		/// 飞行特效索引
	bool m_bFlyEffectByEquip;	/// 飞行特效是否与武器相关
	eLOCATOR m_eFlyEffectPosition;	/// 飞行特效从何处发出
	bool m_bFlyEffectCycle;		/// 飞行特效是否循环
	long m_lFlyEffectTurnX;				/// 飞行特效X轴方向偏移角度
	long m_lFlyEffectTurnY;				/// 飞行特效Y轴方向偏移角度
	vector<long> m_vecFlyEffect;		/// 飞行特效队列
	vector<long> m_vecFlyEffectIndex;	/// 飞行特效索引队列
	vector<eLOCATOR> m_vecFlyEffectPos;	/// 飞行特效发出点队列
	vector<long> m_vecFlyTime;			/// 飞行时间
	vector<long> m_vecFlyDelay;			/// 飞行延迟时间
	vector<long> m_vecFlyEffectTurnX;	/// 飞行特效X轴方向偏移角度队列
	vector<long> m_vecFlyEffectTurnY;	/// 飞行特效Y轴方向偏移角度队列
	vector<long> m_vecSummonEffect;		/// 召唤技能特效
	vector<long> m_vecSummonSound;		/// 召唤声效
	long m_lSummonLifeTime;				/// 召唤生命周期
	long m_lRandomSummonX;				/// 随机召唤技能X轴范围
	long m_lRandomSummonY;				/// 随机召唤技能Y轴范围
	long m_lRandomSummonNum;			/// 随机召唤技能召唤物数
	long m_lRandomSummonDelayTime;		/// 随即召唤技能召唤物的延迟播放时间

	long m_lFlySound;			/// 飞行声音
	bool m_bFlySoundByEquip;	/// 飞行音效是否与武器相关
	bool m_bFlySoundCycle;		/// 飞行声音是否循环
	eLOCATOR m_eFlySoundPosition;	/// 飞行音效从何处发出
	
	long m_lDelLocEffect;		/// 删除特效
	long m_lDelLocEffectIndex;	/// 删除特效索引

	long m_lCycleEffectDelayTime;	/// 循环特效的延迟删除时间
    //===========================================================================//
private:
	vector<int> m_VecFlySoundChannel;        ///飞行音效的Channel
	CPlayer *ConvertFromPetOrPlayer(CMoveShape *pTransform);		///根据类型转换 指针
	CPlayer *ConvertFromPetOrPlayer(CShape *pTransform);		///根据类型转换 指针
public:
	virtual void SetFlySoundChannel(int nChannel);
	virtual void StopFlySound();
};

//新的技能装饰实例类
class ConcreteDecSkillXml: public CSkillXml
{
public:
	///构造函数
	ConcreteDecSkillXml(SkillComponent* com);
	///析构函数
	~ConcreteDecSkillXml();
};


