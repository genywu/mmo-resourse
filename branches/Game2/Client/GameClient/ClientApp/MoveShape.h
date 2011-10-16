#pragma once
#include "Shape.h"
#include "../../Net/Message.h"
#include "../ClientApp/Other/FindPath.h"

using namespace std;
class CEffect;
class CSkillXml;

struct tagFaceAction;

class CMoveShape :public CShape
{
public:
	CMoveShape(void);
	virtual ~CMoveShape(void);

	// 初始化变量名到值得影射
	virtual void InitNameValueMap();

	virtual void SetAction(WORD l);			//设定动作
	virtual void SetState(WORD l);

	virtual bool DecordFromByteArray(BYTE* pByte, long& pos, bool bExData = true);	// 解码
	//解压buff
	bool DecordBuffFromByteArray(BYTE* pByte, long& pos);
	//解压debuff
	bool DecordDeBuffFromByteArray(BYTE* pByte, long& pos);
	//解压onoffskill
	bool DecordOnOffSkillFromByteArray(BYTE* pByte, long& pos);
	//解压主动使用技能
	bool DecordActSkillFromByteArray(BYTE* pByte, long& pos);


	virtual bool DecodeFromDataBlock(DBReadSet& setReadDB, bool bExData = true);	// 解码
	//解压buff
	bool DecodeBuffFromDataBlock(DBReadSet& setReadDB);
	//解压debuff
	bool DecodeDeBuffFromDataBlock(DBReadSet& setReadDB);
	//解压onoffskill
	bool DecodeOnOffSkillFromDataBlock(DBReadSet& setReadDB);
	//解压主动使用技能
	bool DecodeActSkillFromDataBlock(DBReadSet& setReadDB);

//////////////////////////////////////////////////////////////////////////
//	附加效果相关定义
//////////////////////////////////////////////////////////////////////////
protected:
	// 附加效果类型
	enum eADDEFFECT
	{
		ADDEFFECT_FLASH = 0,	//角色闪光
		ADDEFFECT_SHAKE			//角色震动
	};

	// 被攻击所造成的附加效果结构体
	struct tagAddEffect
	{
		bool  bPlayState;		//附加效果的播放状态(为真则开始播放,为假则不播放该效果)
		DWORD dwStartTime;		//效果开始时间
		DWORD dwTimeLength;		//效果持续时间
		DWORD dwValue;			//效果的值(不同的效果类型会有不同的意义)
	};

	// 震动相关属性
	float m_fXIncrement;		//当前模型震动X增量
	float m_fYIncrement;		//当前模型震动Y增量
	float m_fShakeMax;			//当前震动位移的最大幅度
	float m_fShakeMin;			//当前震动位移的最小幅度
	bool  m_bRestoration;		//震动复位(为真时表示,需要复位到模型原位置,反之则不需要复位)

	tagAddEffect m_listAddEffect[2];		// 指定索引0为"角色闪光",索引1为"角色震动"效果

	// 寻路
	CFindPath	m_FindPath;

public:
	void SetupFlashEffect(DWORD dwLum, DWORD dwTimeLength);				//设定角色闪光附加效果
	void SetupShakeEffect(DWORD dwExtent, DWORD dwTimeLength);			//设定角色震动附加效果
	float GetXIncrement()			{ return m_fXIncrement; }
	float GetYIncrement()			{ return m_fYIncrement; }

    /// 相位
private:
    CGUID       m_PhaseID;

public:
    void    SetPhaseID(const CGUID& ID)  { m_PhaseID = ID ; }
    const   CGUID&  GetPhaseID()const { return  m_PhaseID ; }

//////////////////////////////////////////////////////////////////////////
//	异常状态
//////////////////////////////////////////////////////////////////////////
public:
	// 异常状态
	struct tagStateEx
	{
		DWORD	dwID;					//	状态编号
		DWORD	dwLevel;				//	新增状态等级
		DWORD	dwAdditionalData;		//	附加属性 (每个技能单独使用)
		DWORD	dwStartTime;			//	开始时间
		DWORD	dwStandingTime;			//	持续时间 (0代表一直持续)
		bool	bEndSelf;				//	结束自己
		long	lCount;					//	技数器，多次使用同一状态时使用

		tagStateEx()
		{
			dwID = 0;
			dwLevel = 0;
			dwStartTime = 0;
			dwStandingTime = 0;
			bEndSelf = false;
			lCount = 0;
		}
	};
	//强制移动
	struct tagForceMoveInfo
	{
		float x,y;			// 轨迹运动坐标
		float dx,dy;		// 每次AI间隔x,y坐标移动距离
		float endx,endy;	// 目标位置
		int count;			// 时间计数，需要执行的次数
		CGUID RegionID;		// 场景ID
	};
	// 解码技能队列
	struct tagDecordSkillList
	{
		long				lSkillKey;			// 技能键值
		long				lSkillID;			// 技能编号
		long				lSkillLvl;			// 技能等级
		long				lProceNum;			// 技能施放阶段
		long				lMouseX;			// 目标x位置
		long				lMouseY;			// 目标y位置
		vector<long>		vecType;			// 目标类型
		vector<CGUID>		vecID;				// 目标编号
	};
	list<tagStateEx>*	GetStateExList()	{return &m_listStateEx;}

protected:
	tagForceMoveInfo * m_pForceMove;
	list<tagStateEx>   m_listStateEx;			// 异常状态列表

	map<long, CSkillXml*> m_mapCurSkill;		// 当前对象正在执行的所有技能
	vector<tagDecordSkillList> m_vecDecordSkill;// 从服务器解码所有技能队列
	/////////////XY 2008.2.16//////////////
	//对应采集物名字
	string m_strCollectionName;
	//采集物id
	DWORD	m_dwCollectID;
	//(当前)能被采集的次数
	DWORD	m_dwCollectTimes;
	//是否正在被采集
	bool	m_bIsBeenCollect;
	//是否能被采集
	bool	m_bCanBeenCollect;
	///////////////////////////////////////
	//是否在乘骑
	bool    m_bNotRide;

    ///@by:expter增加技能的直接更改的最新状态
    short                       m_sSelfControl;                 //可以自身控制自己与否
    short						m_sMoveable;                    //是否可移动
    short                       m_sUseSkill;                    //是否可以使用技能
    short                       m_sMoveDamage;                  //受到伤害是否移除
    short                       m_sBeenAttack;                  //是否能被攻击
    short						m_sRandomMove;					//随机跑动

	bool m_bRun;					// 行走方式(false:walk,true:run)
	bool m_bAntiInvisibleable;		// 是否反隐形
	bool m_bInvisible;				// 隐形
	bool m_bPretendDead;			// 装死	
	bool m_bUseRabbSkill;			// 使用法师技能
	bool m_bUseFighterSkill;		// 使用战士技能
	bool m_bUseItem;				// 使用物品
	bool m_bInstruction;			// 使用指令
	bool m_bUnAttack;				// 不被攻击标志符
	bool m_bAddHp;					// hp增加标志符
	bool m_bHide;					// 隐蔽
	bool m_bForceAttact;			// 是否强制攻击

	long m_lChangeGraphicstype;
	long m_lChangeGraphicsID;
	bool m_bInForceMoving;			// 是否出于强制移动状态

	BOOL  m_bStateColorPlay;			//异常状态的颜色动画播放开关
	bool  m_bColorFade;					//异常状态的颜色动画是否采用渐变
	bool  m_bColorFadeFlag;				//异常状态的颜色动画渐进渐出标志
	float m_fColorFadePer;				//异常状态的颜色动画渐进渐出的百分比
	tagStateEx m_CurrentStateExForStateColor;	//当前播放异常状态颜色的异常状态
	int	  m_iLastColorLevel;			//异常状态时的颜色处理最后一次等级

	bool  m_busesimplemodel;

public:
	//////////XY 2008 2.18有关采集的接口//////////
	string GetColletctionGoodsShowName(){return m_strCollectionName;}
	DWORD  GetCollectGoodsID()			{return m_dwCollectID;}
	DWORD  GetCollectTimes()			{return m_dwCollectTimes;}
	bool   IsBeingCollected()			{return m_bIsBeenCollect;}
	bool   IsCanBeCollected()			{return m_bCanBeenCollect;}

	
	bool   IsRide()						{return !m_bNotRide;}
	void   SetIsRide(bool b)			{ m_bNotRide = b; }
	bool   GetIsRun()					{return m_bRun;}
	void   SetIsRun(bool bRun)			{m_bRun = bRun;}
	void   SetCollectTimes(DWORD dwTimes){ m_dwCollectTimes = dwTimes;}
	void   SetBeingCollectState(bool bState) { m_bIsBeenCollect = bState;}
	void   SetCanBeCollectState(bool bState) { m_bCanBeenCollect = bState;}
	//////////////////////////////////////////////
	void AddCurSkillList(long lSkillKey, CSkillXml* pSkill){m_mapCurSkill[lSkillKey] = pSkill;}		// 添加到当前技能列表中
	CSkillXml* GetCurSkill(long lSkillKey);		// 取得技能列表中某一个正在使用的技能
	void DeleteCurSkill(long lSkillKey);		// 删除技能列表中某一个技能
	void DeleteCurSkillList();					// 删除技能列表
	void SkillDisplay();		// 显示技能
	bool IsHaveSkill(){ return m_mapCurSkill.empty()?false:true; }	// 判断是否有正在使用的技能
	bool IsHaveSkillByIndex(DWORD dwID);			// 察看是否存在指定ID的技能

	void UseCurSkill();					// 使用当前技能

	void AddStateEx(DWORD dwID,DWORD dwLevel, DWORD dwStandingTime, DWORD dwAdditionalData=0,BYTE bFlag = 0);
	void RemoveStateEx(DWORD dwID,DWORD dwLevel);
	void ClearStateEx();		// 清除所有异常状态
	bool IsHaveStateEx(DWORD dwID);	// 是否有某种状态

	bool IsCanUseRabbSkill()		{return m_bUseRabbSkill;}		// 是否能使用魔法技能
	bool IsCanUseFighterSkill()		{return m_bUseFighterSkill;}	// 是否能使用物理技能
	bool IsCanUseItem()				{return m_bUseItem;}			// 是否能使用物品

	bool IsInUnControlState(){return m_bInstruction;}	// 是否处于不可控制的异常状态，包括不能吃药和放技能
	short IsInUnMoveState(){return m_sMoveable;}		// 是否处于不可行走的异常状态

	short IsInDreadState(){return m_sRandomMove;}	// 是否处于恐惧的异常状态（新加）
	bool IsInDefianceState(){return m_bForceAttact;}	// 是否处于被挑衅的异常状态（新加）
	bool IsInDieAwayState();						// 是否处于假死状态（新加）

	bool IsInForceMoving()		{return m_bInForceMoving;}
	void SetInForceMove(bool b)	{m_bInForceMoving=b;}
	bool GetIsConceal()			{return m_bInvisible;}					//取得是否出于隐身状态
	bool GetIsSpy()				{return m_bAntiInvisibleable;}			//取得是否处于侦查状态
	bool GetIsHide()			{return m_bHide;}						//取得是否处于隐蔽状态

    short CanControlSelf()      {return m_sSelfControl;}                //能否自己控制自己
    short CanReMoveDamage()     {return m_sMoveDamage;}                 //伤害是否被移除
    short IsCanUseAllSkill()    {return m_sUseSkill;}                   //能否使用技能
    short IsCanBeenAttack()     {return m_sBeenAttack;}                 //能否被攻击

	void GetCurGraphicTypeID(long &lType,long &lID);

	void UpdataGetLocator();		// 即时更新绑定点位置

	//void SetExRenderState();		//设定异常状态的渲染状态

	void MoveTo( float xDest, float yDest );
	BOOL IsStateColorPlay()		{ return m_bStateColorPlay; }			//得到异常状态颜色是否播放

/////////////////////////////////////////////////////////////////////
// 消息响应 AI
	/////////////////////////////////////////////////////////////////////
public:
	virtual void OnMessage(CMessage*);
	virtual void UpdateProperty(CMessage*);
	virtual	void DecordChangeProperty(BYTE* pByte,long& lPos);
	virtual	void DecodeChangePropertyFromDataBlock(DBReadSet& setReadDB);
	virtual void AI(void);
	virtual void SetSimpleModel(bool bsimplemodel){m_busesimplemodel = bsimplemodel;}
	virtual	bool Display(void);
	virtual	bool DisplayByLocator(CDisplayModel *pParentModel,AnimInfo *pParentAnimInfo,DWORD locatorname);
	virtual void DisplayLayer2D(float pos);
	virtual void RendShadow(bool bsimple = false);

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
	
//////////////////////////////////////////////////////////////////////////
//	行走
//////////////////////////////////////////////////////////////////////////
public:
	// 更新移动时间定时器
	static void UpdateMoveTimeCounter(DWORD dwCurTime){m_dwMoveTimeCounter = dwCurTime;}
	static void UpdateLastMoveTimeCounter(DWORD dwCurTime){m_dwLastMoveTimeCounter = dwCurTime;}

	long MoveAI();											// 行走AI
	long GetDestDir(long sx, long sy, long dx, long dy);	// 获取目标方向
	long OnArrive();							// 到达目标点处理
	long OnArriveCell(long x, long y);						// 达到格子中心处理
	//到达了新的格子触发
	DWORD GetEndMoveTime()			{return m_dwEndMoveTime;}
	void SetEndMoveTime(DWORD dwTime) {m_dwEndMoveTime = dwTime;}
	DWORD GetLastUpdatePosTime() {return m_dwLastUpdatePosTime;}
	void SetLastUpdatePosTime(DWORD dwTime);

	float GetDestX()					{return m_fDestX;}
	float GetDestY()					{return m_fDestY;}
	void SetDestXY(float x, float y)	{m_fDestX=x; m_fDestY=y;}
    float GetMoveOffsetX()				{return m_moveOffsetX;}
    float GetMoveOffsetY()				{return m_moveOffsetY;}
    void SetMoveOffset(float x, float y) {m_moveOffsetX=x; m_moveOffsetY=y;}

	void SetIsArrive(bool b)		{m_bIsArrive=b;}
	bool IsArrive()					{return m_bIsArrive;}
	
	long m_lStopCount;				// 停止行走的延迟帧数

protected:
	static DWORD m_dwMoveTimeCounter;//所有moveshape统一使用时间计时器来驱动位置更新
	static DWORD m_dwLastMoveTimeCounter; //上一次移动的时间计时
	DWORD m_dwEndMoveTime;			// 结束移动的时间
	DWORD m_dwLastUpdatePosTime;	// 上一次更新位置时间
	float m_fBeginX;				// 起始点X
	float m_fBeginY;				// 起始点Y
	float m_fDestX;					// 目标点X
	float m_fDestY;					// 目标点Y
    float m_moveOffsetX;			// 单位时间(1ms)偏移X
    float m_moveOffsetY;			// 单位时间(1ms)偏移Y

	static long s_lIdleMinCount;	// 最早进入idle动作的记数
	static long s_lIdleMaxCount;	// 最迟进入ilde动作的记数
	long m_lIdleCount;				// 站立的IDLE记数，当idle到一定的大小时，播放IDLE动作
	bool m_bIdleStart;				// 是否开始IDLE
	bool m_bIsArrive;				// 是否达到目标点
	
//////////////////////////////////////////////////////////////////////////
//	谈话
//////////////////////////////////////////////////////////////////////////
public:
	void SetTalkText(const char* str)	{m_strTalkText=str;}
	const char* GetTalkText()			{return m_strTalkText.c_str();}
	void SetTalkLiveTimer(long l)		{m_lTalkLiveTimer=l;}
	long GetTalkLiveTimer()				{return m_lTalkLiveTimer;}
	
protected:
	string m_strTalkText;
	long m_lTalkLiveTimer;	

//////////////////////////////////////////////////////////////////////////
//	冒血
//////////////////////////////////////////////////////////////////////////
public:
	struct tagBloodText
	{
		string	strText;
		char	cType;		// 攻击类型
		float	fPosX;		// 位置坐标
		float	fPosY;		// 位置坐标
		float	fPos;		// 位置高度
		long	lAlpha;		// 透明度
		bool	bBlastAttack;	// 爆击
		float	fScale;		// 文字大小
		DWORD   dwStartTime; //开始时间
		float   fCurScale;
		float   fCurScale2;
	};

	void AddBloodText(const char* str, char cType, bool bBlastAttack=false, float fScale=1.0);

protected:
	list<tagBloodText>		m_listBloodText;

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
		AE_CHANGEDIR,			// 改变方向
		AE_CURE,				// 治疗
	};

	enum tagDeathType
	{
		DT_BEEN_KILLED			= 1,		// 被杀死
		DT_SELF_BLASTING		= 2,		// 自杀
		DT_EVANISH				= 3,		// 时间到
	};

	struct tagLost
	{
		char	cLostType;		// 伤害类型(Public/skillrelated.h里面的tagDamageType)
		long	lLostPoint;		// 伤害点数
	};

	// 战斗事件
	struct tagAttackEvent
	{
		char cType;				// 事件类型(0:受伤 1:死亡 2:完全防御)
		long lTotalLostPoint;	// 总的伤害点数(hp+mp+...)
		long lFinalHP;			// 被攻击方剩余的HP
		long lHitNum;			// 连击数
		vector<tagLost>	vtLost;	// 伤害列表
		float fHurtedDir;		// 被攻击的方向
		//bool bCritical;			// 是否是致命一击  （取消）
		bool bBlastAttack;		// 是否是爆击
		tagDeathType eDeathType;// 死亡类型
		DWORD dwSkillID;	    // 受到伤害的技能编号
		BYTE  bySkillLevel;		// 技能等级
		CGUID Guidattracker;       // 攻击者的GUID

		tagAttackEvent() 
		{
			cType = 0;
			lTotalLostPoint = 0;
			lFinalHP = 0;
			lHitNum = 0;
			fHurtedDir = 0.f;
			//bCritical = false;		// 
			bBlastAttack = false;
			eDeathType = DT_BEEN_KILLED;
			dwSkillID = 1;//SKILL_BASE_ATTACK
			bySkillLevel = 1;
			Guidattracker = CGUID::GUID_INVALID;
		}
	};

protected:
	queue<tagAttackEvent>		m_queueAttackEvent;		// 在自己身上的战斗事件

public:
	queue<tagAttackEvent>* GetAttackEventQueue()	{return &m_queueAttackEvent;}
	long OnAttackEvent();		// 处理攻击事件

	virtual	DWORD	GetHp()					{return 0;}
	virtual	void	SetHp(DWORD dw)			{}
	virtual	DWORD	GetMaxHp()    			{return 0;}
	virtual bool	IsDied();		// 是否死亡

public:
	static float		highhurt_monster;
	static float		highhurt_boss;
	static float		highhurt_knight;
	static float		highhurt_bow;
	static float		highhurt_magic;
	static float		mildhurt_monster ;
	static float		mildhurt_boss ;
	static float		mildhurt_knight ;
	static float		mildhurt_bow ;
	static float		mildhurt_magic ;

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
public:
	long Distance(CShape* pShape);			// 求两个Shape间的距离（单位：格子）

	//soft 2004.12.2
	long	m_lMoveSoundCount;				// 行走声音计数器

//////////////////////////////////////////////////////////////////////////
//	自定义动作时间相关
//////////////////////////////////////////////////////////////////////////
private:
	// 以下两个数据成员与CShape::eACTION下的前12个枚举索引一一对应
	float m_fActionCustomSpeedRate[12];
	// liuke 2008.8.14
	// 本对象当前使用技能的规定时间
	DWORD m_dwCurSkillActTime;
	// 本对象当前使用技能的开始时间
	DWORD m_dwCurSkillStartTime;
	// liuke 08.8.21（平滑移位相关属性）
	bool m_bIsSlipState;		// 是否处于平滑移位状态
	float m_fSlipSpeed;			// 平滑移位的速度
	float m_fSlipDestPosX;		// 平滑移位的目标点坐标
	float m_fSlipDestPosY;
	float m_fSlipDir;			// 平滑移动的方向
	DWORD m_dwSlipEndMoveTime;			// 完成平滑移动的时间
	DWORD m_dwSlipLastUpdatePosTime;	// 上一次平滑移位更新位置时间

public:
	void SetActionSpeedRate(WORD l, float fTimeRate = 1.0f)			{ m_fActionCustomSpeedRate[l] = fTimeRate; }
	// liuke 2008.8.14
	// 设置本对象当前使用技能的规定时间
	void SetCurSkillStartTime(DWORD dwStartTime)	{m_dwCurSkillStartTime = dwStartTime;}
	// 设置本对象当前使用技能的规定时间
	void SetCurSkillActTime(DWORD dwTime)	{m_dwCurSkillActTime = dwTime;}
	// 返回本对象当前使用技能的规定时间
	DWORD GetCurSkillActTime()	{return m_dwCurSkillActTime;}
	// 返回本对象当前使用技能的开始时间
	DWORD GetCurSkillBeginTime()	{return m_dwCurSkillStartTime;}
	// 是否达到当前使用技能的规定时间
	bool IsCurSkillActTimeFinished();

	// 获得和设置平滑移位状态
	void SetIsSlipState(bool bState)		{m_bIsSlipState = bState;}
	bool GetIsSlipState()					{return m_bIsSlipState;}
	// 获得和设置平滑移位速度
	void SetSlipSpeed(float fSpeed)			{m_fSlipSpeed = fSpeed;}
	float GetSlipSpeed()					{return m_fSlipSpeed;}
	// 获得和设置平滑移位坐标
	void SetSlipDestPos(float fx,float fy)	{m_fSlipDestPosX = fx;m_fSlipDestPosY = fy;}
	float GetSlipDestPosX()					{return m_fSlipDestPosX;}
	float GetSlipDestPosY()					{return m_fSlipDestPosY;}
	// 获得和设置平滑移位的方向
	void SetSlipDir(float fDir)				{m_fSlipDir = fDir;}
	float GetSlipDir()						{return m_fSlipDir;}
	// 获得和设置上一次更新位置时间
	void SetSlipEndMoveTime(DWORD dwTime)	{m_dwSlipEndMoveTime = dwTime;}
	DWORD GetSlipEndMoveTime()				{return m_dwSlipEndMoveTime;}
	void SetSlipLastTime(DWORD dwTime)		{m_dwSlipLastUpdatePosTime = dwTime;}
	DWORD GetSlipLastTime()					{return m_dwSlipLastUpdatePosTime;}



//////////////////////////////////////////////////////////////////////////
//	变量名和地址的映射表
//////////////////////////////////////////////////////////////////////////
protected:
	struct tagPty
	{
		BYTE lType;		// 0: byte 1:WORD 2:DWORD	3:float 4:long 5:short 6:bool 7:CGUID
		union
		{
			BYTE	*plValue;
			WORD	*pwValue;
			DWORD	*pdwValue;
			long	*pValue;
			float	*pfValue;
			short	*psValue;
			bool	*pbValue;
		};
	};

	map<string, tagPty>				m_mapNameValue;	// 变量名字对应的变量地址
	typedef map<string, tagPty>::iterator mapNameValueItr; 

	typedef map<DWORD,string>	mapProperttEnumToName;
	typedef map<DWORD,string>::iterator itProETN;

	//枚举值到名字的映射
	mapProperttEnumToName m_PropertyEnumToName;

public:
	//得到属性值接口
	virtual bool  SetProperty(const char* attrName, long value);
	//当属性值改变以后出发
	virtual bool  OnPropertyChanged(const char* attrName);
	//得到属性值接口
	virtual long  GetProperty(const char* attrName);
	//得到属性值的字符串表示
	virtual const char* GetPropertyStringValue(const char* pszPropertyName);
	//根据对象属性格式化字符串
	void FormatStringWithProperty(const char* pszIn,char *pszOut,int nOutSize);

	

//////////////////////////////////////////////////////////////////////////
// 仪态相关(即表情动作) add by lpf(2008.04.10)
// - 包含表情和动作两个方面
//////////////////////////////////////////////////////////////////////////
private:
	// 表情结构体
	struct tagFaceAnim
	{
		// 基本信息
		DWORD  dwFaceCount;			//表情循环次数
		DWORD  dwFaceStartIndex;	//表情开始序号
		DWORD  dwFaceEndIndex;		//表情结束序号
		DWORD  dwFaceInterval;		//表情跳帧速度(毫秒)
		DWORD  dwFaceFileHandle;	//表情文件句柄

		// 即时信息
		bool  bPlayState;			//是否在播放(为true表示正在播放)
		DWORD dwFaceLoopCount;		//当前播放循环次数
		DWORD dwFaceCurrentIndex;	//当前播放序号
		DWORD dwStartTime;			//开始播放时间
	};

	// 当前播放的表情信息
	tagFaceAnim m_stCurrentFaceAnim;		//当前播放的表情信息

	// 动作结构体
	struct tagActionAnim
	{
		int iActionIndex;	//动作文件索引

		// 即时信息
		CGUID guidEffect;			//当前播放的特效GUID
		bool bActionPlayState;		//是否在播放动作(为true表示正在播放)
		bool bEffectPlayState;		//绑定特效是否已经播放过
		DWORD dwActionCurrentIndex;	//当前播放分解动作序号
		DWORD dwStartTime;			//开始播放时间
	};

	// 当前播放的动作信息
	tagActionAnim m_stCurrentActionAnim;		//当前播放的动作信息
	int   m_iActionChannel;                     //当前播放动作的音频Channel
	string m_strActionAudioFile;                //当前播放动作的声音文件
	bool  m_bPlayActionAudio;                   //是否播放表情音效

public:
	bool GetPlayStateOfActionAnim()		{ return m_stCurrentActionAnim.bActionPlayState; }		//得到当前仪态动作是否正在播放
	int	 GetActionIndexOfActionAnim()	{ return m_stCurrentActionAnim.iActionIndex; }			//得到当前播放的仪态动作索引
	void SetupFaceAnim(tagFaceAction * pFaceAction);		//设定播放表情动画所需要的信息
	void SetupActionAnim(tagFaceAction * pFaceAction);		//设定播放动作动画所需要的信息

	void PlayFaceAnim();			//播放表情动画
	void PlayActionAnim();			//播放动作动画

	void SetActionChannel(int iChannel){ m_iActionChannel=iChannel; } //设置表情动作的音频Channel
	int GetActionChannel(){ return m_iActionChannel; }                //获得表情动作的音频Channel
	void PlayActionSound() ;                                           //根据动作播放动作的音效
	void SetActionAudio(string strFileName){m_strActionAudioFile = strFileName;} //设置表情动作的声音
	string GetActionAudio(){ return m_strActionAudioFile; }           //获得表情动作的声音
	void InitActionAudio(DWORD dwFaceActionID);                       //初始化表情动作音效

/*************************************************************************
 * MoveShape身上的特效队列，加载在里面的特效会按顺序播放(刘科2008.11.21)
 * - 按照一定的时间顺序播放队列里面的特效
 **************************************************************************/
	private:
		list<CEffect *> m_lsEffectByOrder;			// 顺序显示的特效列表
	public:
		// 添加顺序播放的特效
		CEffect* AddEffectToList(long lGraphicsID, CShape* pHoster, eLOCATOR eLocator, 
			bool bLoop=false, float fDir=0.f, long lID = 0,bool bAsyn=true);

		// 删除指定顺序播放的特效
		void DelEffectByOrder(long lGraphicsID, long lID = 0);
		
		// 移出指定顺序播放的特效
		void RemoveEffectByOrder(long lGraphicsID, long lID = 0);

		// 删除全部顺序播放的特效
		void DelAllEffectByOrder();
		
		// 绘制顺序播放的特效
		void DisPlayEffectByOrder();

		// 顺序播放的特效
		void AIEffectByOrder();

		// 查找指定顺序播放的特效
		CEffect * FindEffectByOrder(long lGraphicsID, long lID = 0);
	
	
	private:
		//===============07.12.11刘科添加吟唱技能的吟唱条处理=============
		DWORD m_dwIntonateStartTime;		// 正在吟唱技能的开始时间
		DWORD m_dwIntonateTime;				// 正在吟唱技能的吟唱时间
		DWORD m_wIntonateSkillID;			// 正在吟唱技能的ID
		long m_lIntonateSkillKey;			// 正在吟唱技能的键值
		BYTE m_bIntonateAlpha;				// 吟唱条的alpha值
		bool m_bIsLToR;						// 是否从左往右递增显示进度条
	public:
		// 设置正在吟唱技能的开始时间和吟唱时间
		void SetIsLToR(bool bIsLToR)	{m_bIsLToR = bIsLToR;}
		// 设置正在吟唱技能的开始时间和吟唱时间
		void SetCurIntonateTime(DWORD dwTime,DWORD dwStartTime);
		// 获得当前正在吟唱的技能ID
		DWORD GetIntonateState()	{return m_wIntonateSkillID;}
		// 设置当前正在吟唱的技能
		void SetIntonateSkill(DWORD wID)	{m_wIntonateSkillID=wID;}
		// 获得当前正在吟唱的技能ID
		long GetIntonateKey()	{return m_lIntonateSkillKey;}
		// 设置和获得当前吟唱技能的键值
		void SetIntonateSkillKey(long wKey)	{m_lIntonateSkillKey=wKey;}
		// 更新吟唱条数据
		void UpdataIntonatebar();
		// 绘制吟唱条
		void DisplayIntonate();
		//================================================================
};