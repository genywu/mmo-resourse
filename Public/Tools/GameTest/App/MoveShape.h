#pragma once
#include "shape.h"
#include <queue>
//#include "../../nets/netGT/Message.h"
#include "../Utility/FindPath.h"
#include "../Net/Message.h"

class Mover;

class CMoveShape :
	public CShape
{
public:
	CMoveShape(void);
	inline virtual ~CMoveShape(void);

	// 初始化变量名到值得影射
	virtual void InitNameValueMap();
	//动作
	virtual void SetAction(WORD l);
	//状态
	virtual void SetState(WORD l);

	//-------------------------------------------------------------------
	virtual	void DecordChangeProperty(BYTE* pByte,long& lPos);
	// 解码
	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	
	//解压buff
	bool DecordBuffFromByteArray(BYTE* pByte, long& pos);
	//解压debuff
	bool DecordDeBuffFromByteArray(BYTE* pByte, long& pos);
	//解压onoffskill
	bool DecordOnOffSkillFromByteArray(BYTE* pByte, long& pos);
	//解压主动使用技能
	bool DecordActSkillFromByteArray(BYTE* pByte, long& pos);

	virtual bool DecordFromDataBlock(DBReadSet& readDB,bool bExData = true);
	virtual void DecordChangePropertyFromDataBlock(DBReadSet& readDB);
	bool DecordBuffFromDataBlock(DBReadSet& readDB);
	bool DecordDebuffFromDataBlock(DBReadSet& readDB);
	bool DecordOnOffSkillFromDataBlock(DBReadSet& readDB);
	bool DecordActSkillFromDataBlock(DBReadSet& readDB);

	//  [8/11/2009 Square]
	void	IncrUsedTimes()	{ m_dwUsedTimes++; }
	void	ReduUsedTimes()	{ m_dwUsedTimes--; }
	DWORD	GetUsedTimes()	{ return m_dwUsedTimes; }
	void	SetUsedTimes(DWORD t)	{ m_dwUsedTimes = t; }

	/// 相位
private:
	CGUID       m_PhaseID;

protected:
	//	引用计数
	DWORD	m_dwUsedTimes;

public:
	//// 异常状态
	//struct tagStateEx
	//{
	//	DWORD	dwID;					//	状态编号
	//	DWORD	dwLevel;				//	新增状态等级
	//	DWORD	dwAdditionalData;		//	附加属性 (每个技能单独使用)
	//	DWORD	dwStartTime;			//	开始时间
	//	DWORD	dwStandingTime;			//	持续时间 (0代表一直持续)
	//	bool	bEndSelf;				//	结束自己
	//	long	lCount;					//	技数器，多次使用同一状态时使用

	//	tagStateEx()
	//	{
	//		dwID = 0;
	//		dwLevel = 0;
	//		dwStartTime = 0;
	//		dwStandingTime = 0;
	//		bEndSelf = false;
	//		lCount = 0;
	//	}
	//};
	////强制移动
	//struct tagForceMoveInfo
	//{
	//	float x,y;			// 轨迹运动坐标
	//	float dx,dy;		// 每次AI间隔x,y坐标移动距离
	//	float endx,endy;	// 目标位置
	//	int	  count;			// 时间计数，需要执行的次数
	//	CGUID RegionGuid;		// 场景ID
	//};
	// 解码技能队列
	//struct tagDecordSkillList
	//{
	//	long				lSkillKey;			// 技能键值
	//	long				lSkillID;			// 技能编号
	//	long				lSkillLvl;			// 技能等级
	//	long				lProceNum;			// 技能施放阶段
	//	long				lObjectX;			// 目标x位置
	//	long				lObjectY;			// 目标y位置
	//	vector<long>		vecType;			// 目标类型
	//	vector<CGUID>		vecID;				// 目标编号
	//};
	//	list<tagStateEx>*	GetStateExList()	{return &m_listStateEx;}

protected:
	//	tagForceMoveInfo * m_pForceMove;
	//	list<tagStateEx>   m_listStateEx;							// 异常状态列表
	//	vector<tagDecordSkillList> m_vecDecordSkill;				// 从服务器解码所有技能队列

	//采集
protected:
	//	string m_strCollectionName;					//对应采集物名字
	//	DWORD	m_dwCollectID;							//采集物id
	//	DWORD	m_dwCollectTimes;					//(当前)能被采集的次数
	//	bool	m_bIsBeenCollect;					//是否正在被采集
	//	bool	m_bCanBeenCollect;					//是否能被采集
		bool    m_bNotRide;							//是否在乘骑

		bool m_bRun;					// 行走方式(false:walk,true:run)
		//bool m_bAntiInvisibleable;		// 是否反隐形
		//bool m_bInvisible;				// 隐形
		//bool m_bPretendDead;			// 装死	
		//bool m_bUseRabbSkill;			// 使用法师技能
		//bool m_bUseFighterSkill;		// 使用战士技能
		//bool m_bUseItem;				// 使用物品
		//bool m_bInstruction;			// 使用指令
		//bool m_bUnAttack;				// 不被攻击标志符
		//bool m_bAddHp;					// hp增加标志符
		//bool m_bHide;					// 隐蔽
		//bool m_bForceAttact;			// 是否强制攻击

// 		long m_lChangeGraphicstype;
// 		long m_lChangeGraphicsID;
		//bool m_bInForceMoving;			// 是否出于强制移动状态

public:
	//////////XY 2008 2.18有关采集的接口//////////
	//string GetColletctionGoodsShowName(){return m_strCollectionName;}
	//DWORD  GetCollectGoodsID()			{return m_dwCollectID;}
	//DWORD  GetCollectTimes()			{return m_dwCollectTimes;}
	//bool   IsBeingCollected()			{return m_bIsBeenCollect;}
	//bool   IsCanBeCollected()			{return m_bCanBeenCollect;}

	bool   IsRide()						{return !m_bNotRide;}
	bool   GetIsRun()					{return m_bRun;}
	void   SetIsRun(bool bRun)			{m_bRun = bRun;}

	//void   SetCollectTimes(DWORD dwTimes){ m_dwCollectTimes = dwTimes;}
	//void   SetBeingCollectState(bool bState) { m_bIsBeenCollect = bState;}
	//void   SetCanBeCollectState(bool bState) { m_bCanBeenCollect = bState;}

	void DeleteCurSkill(long lSkillKey);						// 删除技能列表中某一个技能
	void DeleteCurSkillList();									// 删除技能列表

	void UseCurSkill();											// 使用当前技能

	void AddStateEx(DWORD dwID,DWORD dwLevel, DWORD dwStandingTime, DWORD dwAdditionalData=0);
	void RemoveStateEx(DWORD dwID);
	void ClearStateEx();										// 清除所有异常状态
	bool IsHaveStateEx(DWORD dwID);								// 是否有某种状态

	//bool IsInUnControlState(){return m_bInstruction;}			// 是否处于不可控制的异常状态，包括不能吃药和放技能
	//bool IsInUnMoveState(){return m_bMoveable;}					// 是否处于不可行走的异常状态

	//bool IsInDreadState(){return m_bRandomMove;}				// 是否处于恐惧的异常状态（新加）
	//bool IsInDefianceState(){return m_bForceAttact;}			// 是否处于被挑衅的异常状态（新加）
	bool IsInDieAwayState();									// 是否处于假死状态（新加）

	//bool IsInForceMoving()		{return m_bInForceMoving;}
	//void SetInForceMove(bool b)	{m_bInForceMoving=b;}
	bool GetIsConceal();										// 取得是否出于隐身状态
	bool GetIsSpy();											// 取得是否处于侦查状态
	//bool IsInHideState();										// 是否处于隐蔽状态

	void UpdataGetLocator();		// 即时更新绑定点位置

	/////////////////////////////////////////////////////////////////////
	// 消息响应 AI
	/////////////////////////////////////////////////////////////////////
protected:
	// 寻路
	CFindPath	m_FindPath;

public:
	long GetDestDir(long sx, long sy, long dx, long dy);		// 获取目标方向
	long OnArrive();										// 到达目标点处理
	long OnArriveCell(long x, long y);							// 达到格子中心处理
	//到达了新的格子触发
	long MoveAI();											// 行走AI
	void AI(void);
	//	DWORD GetBeginMoveTime()			{return m_dwBeginMoveTime;}
	//	void SetBeginMoveTime(DWORD dw)		{m_dwBeginMoveTime=dw;}
	//	DWORD GetEndMoveTime()				{return m_dwEndMoveTime;}
		void SetEndMoveTime(DWORD dwTime)	 {m_dwEndMoveTime = dwTime;}
		DWORD GetLastUpdatePosTime()		 {	return m_dwLastUpdatePosTime;}
		void SetLastUpdatePosTime(DWORD dwTime) { m_dwLastUpdatePosTime = dwTime;}
	float GetBeginX()					{return m_fBeginX;}
	float GetBeginY()					{return m_fBeginY;}
	void SetBeginXY(float x, float y)	{m_fBeginX=x; m_fBeginY=y;}

	long AI_Stand();			// 站AI
	long AI_Walk();				// 走AI
	long AI_Run();				// 跑AI
	long AI_Attack();			// 攻擊AI
	long AI_Perform();			// 施法AI
	long AI_BeAttack();			// 被攻擊AI
	long AI_Died();				// 死亡AI
	long AI_StateEx();			// 异常状态的AI
	void AI_Skill();			// 技能AI
	void AI_SlipMove();			// 平滑移位AI

	float GetDestX()					{return m_fDestX;}
	float GetDestY()					{return m_fDestY;}
	void SetDestXY(float x, float y)	{m_fDestX=x; m_fDestY=y;}

	void SetIsArrive(bool b)		{m_bIsArrive=b;}
	bool IsArrive()					{return m_bIsArrive;}

	float GetMoveOffsetX()				{return m_moveOffsetX;}
	float GetMoveOffsetY()				{return m_moveOffsetY;}
	void SetMoveOffset(float x, float y) {m_moveOffsetX=x; m_moveOffsetY=y;}

	//virtual float GetSpeed()		{ return m_fSpeed; }
	//virtual void SetSpeed(float l)	{ m_fSpeed = l; }
	//	long m_lStopCount;				// 停止行走的延迟帧数
protected:
	//	static DWORD m_dwMoveTimeCounter;//所有moveshape统一使用时间计时器来驱动位置更新
	//	static DWORD m_dwLastMoveTimeCounter; //上一次移动的时间计时
	//	DWORD m_dwBeginMoveTime;		// 开始移动的时间
		DWORD m_dwEndMoveTime;			// 结束移动的时间
		DWORD m_dwLastUpdatePosTime;	// 上一次更新位置时间
	float m_fBeginX;				// 起始点X
	float m_fBeginY;				// 起始点Y
	float m_fDestX;					// 目标点X
	float m_fDestY;					// 目标点Y
	float m_moveOffsetX;			// 单位时间(1ms)偏移X
	float m_moveOffsetY;			// 单位时间(1ms)偏移Y

	//float m_fSpeed;					// 速度
	//	static long s_lIdleMinCount;	// 最早进入idle动作的记数
	//	static long s_lIdleMaxCount;	// 最迟进入ilde动作的记数
	//	long m_lIdleCount;				// 站立的IDLE记数，当idle到一定的大小时，播放IDLE动作
	//	bool m_bIdleStart;				// 是否开始IDLE
	bool m_bIsArrive;				// 是否达到目标点


	//////////////////////////////////////////////////////////////////////////
	//	谈话
	//////////////////////////////////////////////////////////////////////////
public:
	//	void SetTalkText(const char* str)	{m_strTalkText=str;}
	//	const char* GetTalkText()			{return m_strTalkText.c_str();}
	//	void SetTalkLiveTimer(long l)		{m_lTalkLiveTimer=l;}
	//	long GetTalkLiveTimer()				{return m_lTalkLiveTimer;}
protected:
	//	string m_strTalkText;
	//	long m_lTalkLiveTimer;	
	//////////////////////////////////////////////////////////////////////////
	//	冒血
	//////////////////////////////////////////////////////////////////////////
public:
	//struct tagBloodText
	//{
	//	string	strText;
	//	char	cType;		// 攻击类型
	//	float	fPosX;		// 位置坐标
	//	float	fPosY;		// 位置坐标
	//	float	fPos;		// 位置高度
	//	long	lAlpha;		// 透明度
	//	bool	bBlastAttack;	// 爆击
	//	float	fScale;		// 文字大小
	//	DWORD   dwStartTime; //开始时间
	//	float   fCurScale;
	//};

	//	void AddBloodText(const char* str, char cType, bool bBlastAttack=false, float fScale=1.0);

protected:
	//	list<tagBloodText>		m_listBloodText;
	///////////////////////////////////////////////////////////////
	//	战斗事件
	///////////////////////////////////////////////////////////////
public:
	enum eAttackEvent
	{
		AE_DAMAGE,				// 减血伤害
		AE_BREAK,				// 受伤
		AE_DIED,				// 死亡
		AE_FULLMISS,			// 闪避
		AE_PARRY,				// 招架
		AE_BLOCK,				// 格档
		AE_RESIST,				// 抵抗
		AE_REBOUND,				// 反弹
		AE_SUCK,				// 吸收
		AE_LOST,				// 丢失
	};

	enum tagDeathType
	{
		DT_BEEN_KILLED			= 1,		// 被杀死
		DT_SELF_BLASTING		= 2,		// 自杀
		DT_EVANISH				= 3,		// 时间到
	};

	//struct tagLost
	//{
	//	char	cLostType;		// 伤害类型(public\skillrelated.h里面的tagDamageType)
	//	WORD	wLostPoint;		// 伤害点数
	//};

	//// 战斗事件
	//struct tagAttackEvent
	//{
	//	char cType;				// 事件类型(0:受伤 1:死亡 2:完全防御)
	//	long lTotalLostPoint;	// 总的伤害点数(hp+mp+...)
	//	long lFinalHP;			// 被攻击方剩余的HP
	//	long lHitNum;			// 连击数
	//	vector<tagLost>	vtLost;	// 伤害列表
	//	float fHurtedDir;		// 被攻击的方向
	//	//bool bCritical;			// 是否是致命一击  （取消）
	//	bool bBlastAttack;		// 是否是爆击
	//	tagDeathType eDeathType;// 死亡类型
	//	DWORD dwSkillID;	    // 受到伤害的技能编号
	//	BYTE  bySkillLevel;		// 技能等级
	//	CGUID AtkerGuid;

	//	tagAttackEvent() 
	//	{
	//		cType = 0;
	//		lTotalLostPoint = 0;
	//		lFinalHP = 0;
	//		lHitNum = 0;
	//		fHurtedDir = 0.f;
	//		bBlastAttack = false;
	//		eDeathType = DT_BEEN_KILLED;
	//		dwSkillID = 1;//SKILL_BASE_ATTACK
	//		bySkillLevel = 1;
	//		AtkerGuid = CGUID::GUID_INVALID;
	//	}
	//};

protected:
	//	queue<tagAttackEvent>		m_queueAttackEvent;		// 在自己身上的战斗事件

public:
	//	queue<tagAttackEvent>* GetAttackEventQueue()	{return &m_queueAttackEvent;}
	long OnAttackEvent();		// 处理攻击事件

	virtual	DWORD	GetHp()					{return 0;}
	virtual	void	SetHp(DWORD dw)			{}
	virtual	DWORD	GetMaxHp()    			{return 0;}
	virtual bool	IsDied();		// 是否死亡

public:
	//static float		highhurt_monster;
	//static float		highhurt_boss;
	//static float		highhurt_knight;
	//static float		highhurt_bow;
	//static float		highhurt_magic;
	//static float		mildhurt_monster ;
	//static float		mildhurt_boss ;
	//static float		mildhurt_knight ;
	//static float		mildhurt_bow ;
	//static float		mildhurt_magic ;

public:
	long Distance(CShape* pShape);			// 求两个Shape间的距离（单位：格子）

	//////////////////////////////////////////////////////////////////////////
	//	自定义动作时间相关
	//////////////////////////////////////////////////////////////////////////
private:
	// 以下两个数据成员与CShape::eACTION下的前12个枚举索引一一对应
		float m_fActionCustomSpeedRate[12];
	// liuke 2008.8.14
	// 本对象当前使用技能的规定时间
	//	DWORD m_dwCurSkillActTime;
	// 本对象当前使用技能的开始时间
	//	DWORD m_dwCurSkillStartTime;
	// liuke 08.8.21（平滑移位相关属性）
	//	bool m_bIsSlipState;		// 是否处于平滑移位状态
	//	float m_fSlipSpeed;			// 平滑移位的速度
	//	float m_fSlipDestPosX;		// 平滑移位的目标点坐标
	//	float m_fSlipDestPosY;
	//	float m_fSlipDir;			// 平滑移动的方向
	//	DWORD m_dwSlipEndMoveTime;			// 完成平滑移动的时间
	//	DWORD m_dwSlipLastUpdatePosTime;	// 上一次平滑移位更新位置时间
public:
	void SetActionSpeedRate(WORD l, float fTimeRate = 1.0f)			{ m_fActionCustomSpeedRate[l] = fTimeRate; }
	// liuke 2008.8.14
	// 设置本对象当前使用技能的规定时间
	//	void SetCurSkillStartTime(DWORD dwStartTime)	{m_dwCurSkillStartTime = dwStartTime;}
	// 设置本对象当前使用技能的规定时间
	//	void SetCurSkillActTime(DWORD dwTime)	{m_dwCurSkillActTime = dwTime;}
	// 返回本对象当前使用技能的规定时间
	//	DWORD GetCurSkillActTime()	{return m_dwCurSkillActTime;}
	// 是否达到当前使用技能的规定时间
	bool IsCurSkillActTimeFinished();

	// 获得和设置平滑移位状态
	//	void SetIsSlipState(bool bState)		{m_bIsSlipState = bState;}
	//	bool GetIsSlipState()					{return m_bIsSlipState;}
	// 获得和设置平滑移位速度
	//	void SetSlipSpeed(float fSpeed)			{m_fSlipSpeed = fSpeed;}
	//	float GetSlipSpeed()					{return m_fSlipSpeed;}
	// 获得和设置平滑移位坐标
	//	void SetSlipDestPos(float fx,float fy)	{m_fSlipDestPosX = fx;m_fSlipDestPosY = fy;}
	//	float GetSlipDestPosX()					{return m_fSlipDestPosX;}
	//	float GetSlipDestPosY()					{return m_fSlipDestPosY;}
	// 获得和设置平滑移位的方向
	//	void SetSlipDir(float fDir)				{m_fSlipDir = fDir;}
	//	float GetSlipDir()						{return m_fSlipDir;}
	// 获得和设置上一次更新位置时间
	//	void SetSlipEndMoveTime(DWORD dwTime)	{m_dwSlipEndMoveTime = dwTime;}
	//	DWORD GetSlipEndMoveTime()				{return m_dwSlipEndMoveTime;}
	//	void SetSlipLastTime(DWORD dwTime)		{m_dwSlipLastUpdatePosTime = dwTime;}
	//	DWORD GetSlipLastTime()					{return m_dwSlipLastUpdatePosTime;}
	//变量名和地址的映射表
protected:
	//struct tagPty
	//{
	//	BYTE lType;		// 0: byte 1:WORD 2:DWORD	3:float 4:long 5:short 6:bool 7:CGUID
	//	union
	//	{
	//		BYTE	*plValue;
	//		WORD	*pwValue;
	//		DWORD	*pdwValue;
	//		long	*pValue;
	//		float	*pfValue;
	//		short	*psValue;
	//		bool	*pbValue;
	//	};
	//};

	//map<string,tagPty>		m_mapNameValue;
	//typedef map<string,tagPty>::iterator mapNameValueItr;
	//typedef map<DWORD,string>::iterator itProETN;
	//typedef map<DWORD,string>	m_mapPropertyEnumToName;
	//m_mapPropertyEnumToName m_PropertyEnumToName;
public:
	virtual bool SetProperty(const char* strName,long value);
	virtual bool OnPropertyChanged(const char* strName);
	virtual long GetProperty(const char*strName);
	virtual const char* GetPropertyStringValue(const char* pszPropertyName);
	void FormatStringWithProperty(const char* pszIn,char*pszOut,int nOutSize);

public:
    void RequestMove( float destX, float destY );

    bool UpdateMove();

    void StopMove();

    virtual long OnTimeOut(DWORD timerid,DWORD curtime,const void* var);
private:
    Mover *m_mover;
};
