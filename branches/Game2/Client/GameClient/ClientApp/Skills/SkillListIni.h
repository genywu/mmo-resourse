/********************************************************************
created:	2010/09/13
created:	13:9:2010   16:15
filename: 	e:\SVN_P6\trunk\Client\GameClient\ClientApp\Skills\SkillListIni.h
file path:	e:\SVN_P6\trunk\Client\GameClient\ClientApp\Skills
file base:	SkillListIni
file ext:	h
author:		zhaohang(kaba) (kuailekaba@gmail.com)

purpose:	新的技能配表实现
*********************************************************************/
#include "../../../../Public/Common/SkillRelated.h"
class tagSkillBaseAttr;
class tagEffectAttr;
class tagStpAttrSound;
class tagStpAttr;
class tagIntonateStp;
class tagRelEffectAttr;
class tagFlyEffectAttr;
class tagReleaseStp;
class tagEndStp;
class tagSkillLevelAttr;
class tagSkillAttr;
class SkillAttrIniSetup;

/**技能初始定义阶段基本属性*/
class tagSkillBaseAttr{
	///技能ID
	uint mID;
	///技能名称
	char mName[256];
	///技能类型 0.被动 1.瞬发 2.吟唱
	uint mSkillType;
	///是否锁定目标 0.否 1.是
	bool mbLockTarget;
	///是否锁定技能 0.否 1.是
	bool mbLockSkill;
public:
	///构造函数
	tagSkillBaseAttr();
	///析构函数
	~tagSkillBaseAttr();

	///获取技能ID
	uint GetID();
	///设置技能ID
	void SetID(uint id);
	///获取技能名称
	const char* GetName();
	///设置技能名称
	void SetName(const char* name);
	///获得技能类型 0.被动 1.瞬发 2.吟唱
	uint GetSkillType();
	///设置技能类型 0.被动 1.瞬发 2.吟唱
	void SetSkillType(uint);
	///是否锁定目标
	bool IsLockTarget();
	///设置是否锁定目标
	void SetLockTarget(bool bLockTarget);
	///是否锁定技能
	bool IsLockSkill();
	///设置是否锁定技能
	void SetLockSkill(bool bLockSkill);
};

/**一个特效的基本属性*/
class tagEffectAttr{
private:
	///特效ID
	uint mID;
	///特效索引
	uint mIndex;
	///特效绑定位置
	uint mBindPos;
	///特效是否循环播放
	bool mCycle;
	///开始播放
	bool mPlay;
public:
	tagEffectAttr();
	~tagEffectAttr();
	///设置特效ID
	void SetID(uint id);
	///获取特效ID
	uint GetID();
	///设置索引
	void SetIndex(uint index);
	///获取索引
	uint GetIndex();
	void SetBindPos(uint pos);
	uint GetBindPos();
	void SetCycle(bool b);
	bool IsCycle();
	void SetPlay(bool b);
	bool IsPlay();
};
/**一个阶段基本属性里的音效部分*/
class tagStpAttrSound{
private:
	///音效ID -1标识没有音效
	uint mSoundID;
	///音效是否循环播放
	bool mSoundCycle;
	///开始播放音效
	bool mSoundPlay;
public:
	tagStpAttrSound();
	~tagStpAttrSound();
	void SetSoundID(uint id);
	uint GetSoundID();
	void SetSoundCycle(bool b);
	bool IsSoundCycle();
	void SetSoundPlay(bool b);
	bool IsSoundPlay();
};

/**一个阶段的基本属性*/
class tagStpAttr{
private:
	///动作Id -1标识没有动作
	uint mActID;
public:
	tagStpAttr();
	~tagStpAttr();
	void SetActID(uint id);
	uint GetActID();
	//音效属性
	tagStpAttrSound mSoundAttr;
};

/**吟唱阶段属性StepBegin*/
class tagIntonateStp{
private:
	///主要循环动作ID -1标识没有主要循环动作ID
	uint mMainActID;
	///是否可以中断
	bool mBreak;
public:
	///吟唱阶段基本属性
	tagStpAttr mBaseAttr;
	///吟唱特效
	std::vector<tagEffectAttr>mVecEffect;
	tagIntonateStp();
	~tagIntonateStp();
	void SetMainActID(uint id);
	uint GetMainActID();
	void SetBreak(bool b);
	bool IsBreak();
};

/***释放特效属性*/
class tagRelEffectAttr{
private:
	///释放目标 -1标识没有目标
	uint mDest;
public:
	///特效基本属性
	tagEffectAttr mEffect;

	tagRelEffectAttr();
	~tagRelEffectAttr();

	void SetDest(uint dest);
	uint GetDest();

};

/**飞行特效属性*/
class tagFlyEffectAttr{
private:
	///X轴转向角度
	int mTurnX;
	///Y轴转向角度
	int mTurnY;
	///动作保护时间
	uint mActTime;
	///飞行特效速度
	uint mSpeed;
	///飞行特效延迟时间
	uint mDelayTime;

	///音效部分新增一个属性，音效从何处发出,-1标识空
	uint mSoundPos;

public:
	///飞行特效部分基本属性
	tagEffectAttr mBaseAttr;
	///飞行特效音效部分
	tagStpAttrSound mSoundAttr;
	tagFlyEffectAttr();
	~tagFlyEffectAttr();

	void SetTurnX(int x);
	int GetTurnX();
	void SetTurnY(int y);
	int GetTurnY();

	void SetActTime(uint time);
	uint GetActTime();

	void SetSpeed(uint speed);
	uint GetSpeed();

	void SetDelayTime(uint delay);
	uint GetDelayTime();

	void SetSoundPos(uint pos);
	uint GetSoundPos();

};
/**释放阶段属性StepRun*/
class tagReleaseStp{
private:
	///释放新增释放音效目标
	uint mSoundDest;
	///释放新增释放动作时间,-1标识没有此项
	uint mActTime;
	///是否有飞行阶段
	bool mbFly;
public:
	///释放阶段基本属性
	tagStpAttr mBaseAttr;
	///释放特效
	std::vector<tagRelEffectAttr> mVecEffect;
	///飞行特效
	tagFlyEffectAttr mFlyEffect;
	void SetHasFly(bool);
	bool IsHasFly();

	tagReleaseStp();
	~tagReleaseStp();

	void SetSoundDest(uint dest);
	uint GetSoundDest();

	void SetActTime(uint t);
	uint GetActTime();
};

/**结束阶段属性StepEnd*/
class tagEndStp{
private:
	///命中特效ID -1标识没有
	uint mHitEffectID;
	///命中音效 -1标识没有
	uint mHitSoundID;
	///是否有闪光效果
	bool mFlashEffect;
	///闪光亮度
	uint mFlashEffectLum;
	///闪光效果持续时间
	uint mFlashEffectTime;
	///震动效果
	bool mShakeEffect;
	///震动幅度值
	uint mShakeEffectExtent;
	///震动效果持续时间
	uint mShakeEffectTime;
public:
	tagEndStp();
	~tagEndStp();
	
	void SetHitEffID(uint id);
	uint GetHitEffID();
	
	uint GetHitSoundID();
	void SetHitSoundID(uint id);
	
	void SetFlashEffect(bool b);
	bool IsFlashEffect();

	void SetFlashEffLum(uint num);
	uint GetFlashEffLum();

	void SetFlashEffTime(uint t);
	uint GetFlashEffTime();

	void SetShakeEffect(bool b);
	bool IsShakeEffect();

	void SetShakeEffExtent(uint num);
	uint GetShakeEffExtent();
	
	void SetShakeEffTime(uint t);
	uint GetShakeEffTime();
};

/**召唤特效阶段StepSummon*/
class tagSummonStp{
private:
	///特效id -1标识没有
	uint mID;
	///音效ID -1标识没有
	uint mSoundID; 
	///召唤生命周期
	uint mLifeTime;
	///X轴范围
	uint mRangeX;
	///Y轴范围
	uint mRangeY;
	///随机召唤技能召唤物数量
	uint mRandomNum;
	///随机召唤技能召唤物延迟时间
	uint mRandomDelayTime;
public:
	tagSummonStp();
	~tagSummonStp();

	void SetID(uint id);
	uint GetID();

	void SetSoundID(uint id);
	uint GetSoundID();

	void SetLifeTime(uint t);
	uint GetLifeTime();

	void SetRangeX(uint x);
	uint GetRangeX();

	void SetRangeY(uint y);
	uint GetRangeY();

	void SetRandomNum(uint num);
	uint GetRandomNum();

	void SetRandomDelayTime(uint t);
	uint GetRandomDelayTime();
};

/**技能等级属性*/
class tagSkillLevelAttr{
private:
	///技能等级
	uint mLevel;
	///Ai类型
	uint mAIType;
	///技能使用时的状态 
	uint mStartState;

	///是否有吟唱阶段
	bool mbInto;
	///是否有释放阶段
	bool mbRel;
	///是否有结束阶段
	bool mbEnd;
	///是否有召唤阶段
	bool mbSummon;
public:
	typedef std::vector<uint>VecObject;
	typedef VecObject VecForceObject;
	///释放对象
	VecObject mObjVec;
	///强制释放对象
	VecForceObject mForceObjVec;
	
	///吟唱阶段
	tagIntonateStp		mIntoStp;
	void SetHasIntoStp(bool b);
	bool IsHasIntoStp();

	///释放阶段
	tagReleaseStp		mRelStp;
	void SetHasRelStp(bool);
	bool IsHasRelStp();

	///结束阶段
	tagEndStp			mEndStp;
	void SetHasEndStp(bool);
	bool IsHasEndStp();
	///召唤阶段
	tagSummonStp		mSummonStp;
	void SetHasSummonStp(bool b);
	bool IsHasSummonStp();
	tagSkillLevelAttr();
	~tagSkillLevelAttr();

	void SetLevel(uint level);
	uint GetLevel();
	void SetAiType(uint aitype);
	uint GetAiType();
	void SetStartState(uint state);
	uint GetStartState();
};

/**一条技能属性结构*/
class tagSkillAttr{
private:
	///技能基本属性
	tagSkillBaseAttr mBaseAttr;
	///新增是否有动作,只要有INTONATE_ACT RELEASE_ACT INTONATE_MAINACT RELEASE_ACTEX 就有动作
	bool mbHasAct;
public:
	///技能等级属性
	std::vector<tagSkillLevelAttr> mLevelAttrVec;

	tagSkillAttr();
	~tagSkillAttr();

	tagSkillBaseAttr* GetBaseAttr();
	tagSkillLevelAttr* GetLevelAttr(uint level);
	tagSkillLevelAttr* GetLevelAttrByIndex(uint index);
	bool IsHasAct();
	void SetHasAct(bool b);

};
/*技能属性配置表的读取类*/
class SkillAttrIniSetup{
private:
	typedef std::map<uint,tagSkillAttr>SkillAttrMap;
	///所有技能属性
	static SkillAttrMap m_SkillAttrMap;
	
	friend class CSkillListXml;
public:
	static bool LoadFile(const char* fileName);

};