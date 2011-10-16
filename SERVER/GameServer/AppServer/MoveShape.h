#pragma once
#include "shape.h"
#include "..\..\..\Nets\MySocket.h"
#include <queue>
#include <vector>
#include <set>
#include <list>

#include "ai\baseai.h"
#include "Goods\CGoods.h"
#include "BuffSkill/NewSkillFactory.h"
#include "BuffSkill/AttackPower.h"
#include "Message Packaging\CS2CContainerObjectMove.h"
using namespace std;

class CRegion;
class CPlayer;
class CBaseAI;
class CPlayerAI;
class CBaseModule;
class COpPosition;


class CScript;

class CMoveShape :
	public CShape
{
public:
	CMoveShape(void);
	virtual ~CMoveShape(void);

	virtual void SetAction(WORD l){}

protected:

	CBaseAI*					m_pAI;
public:

    // [070611 AHC]
    virtual bool  SetAttribute(std::string& attrName, long value);
	virtual bool  SetAttribute(char* attrName, long value);
    virtual long  GetAttribute(std::string attrName);
    virtual bool  ChangeAttribute(std::string& attrName, long value);
	virtual void  AddAttributeToDirty(string attrName) {return;}

	virtual long  UpdateAttributeToClient(){return 0;}
	virtual long  UpdateAttribute(BOOL bImmediately=0){return 0;}
	virtual long  OnUpdateAttributeTimeOut(){return 0;}
	virtual long  UpdateHPMPToClient(set<string>& dirtyAttr){return 0;}
	//添加变量名到胀数据
	virtual void  AddAttributeToDirty(string& attrName){return;}


	virtual BOOL  SetBoolAttribute(const std::string& attrName, long value);
	virtual BOOL  SetBoolAttribute(const std::string &attrName, BOOL b);

	long CMoveShape::GetValue(const char* strName);
	long CMoveShape::SetValue(const char* strName, long dwValue);
	//当属性发生改变时候调用
	virtual void OnPropertyValueChanged(const char* strName);
	
	virtual	BYTE		GetCountry(){return 0;}
	virtual DWORD		GetPkValue(){return 0;}
	virtual DWORD		GetPVPValue(){return 0;}
	virtual CMoveShape* GetHost(void) {return NULL;}

	// 初始化
	void InitNameValueMap();
	//////////////////////////////////////////////////////////////////////////
	//	变量名和地址的映射表
	//////////////////////////////////////////////////////////////////////////
protected:
	map<string, tagPty>				m_mapShapeNameValue;	// 变量名字对应的变量地址
	typedef map<string, tagPty>::iterator mapShapeNameValueItr; 

	set<string>  m_mapShapeDirtyAttrData;
	void    PushShapeDirtyAttrData(string attrName) { m_mapShapeDirtyAttrData.insert(attrName); }
	string  PopShapeDirtyAttrData() 
	{ 
		string tmp = *m_mapShapeDirtyAttrData.begin(); 
		m_mapShapeDirtyAttrData.erase(m_mapShapeDirtyAttrData.begin());
		return tmp; 
	}
	long    DirtyShapeAttrDataSize(void) { return m_mapShapeDirtyAttrData.size(); }



	//////////////////////////////////////////////////////////////////////////
	//	受益者
	//////////////////////////////////////////////////////////////////////////
public:
	void SetBeneficiary(long lType,CGUID guidFirst);				//设置受益者
	void RecordBeneficiaryTime(DWORD time=0)			//记录最后受攻击时间
	{
		if (time==0)
		{
			m_dwBeneficiaryTime= timeGetTime();
		}
		else
		{
			m_dwBeneficiaryTime= time;
		}
	}		
	void CleanBeneficiary();							//清空受益者
	long GetBeneficiaryTime(){return m_dwBeneficiaryTime;}
	CGUID GetBeneficiaryID(){return m_BeneficiaryID;}
	OBJECT_TYPE GetBeneficiaryType(){return m_BeneficiaryType;}
	long AddBeneficiaryEvent(long lTime);				//添加受益者事件
	virtual void OnLoseBeneficiary(){}					//丢失受益者事件
	virtual void StartRecordBeneficiary(CGUID guidFirst,OBJECT_TYPE typeFirst,DWORD Time=0){}				//开始记录受益者
private:
	CGUID m_BeneficiaryID;				// 最先攻击自己的玩家的id
	OBJECT_TYPE m_BeneficiaryType;		// 受益者的类型
	DWORD m_dwBeneficiaryTime;			// 最后一次攻击时间
	LONG m_lBeneficiaryTimerID;			// 受益者事件id

//---------------------------------------------------------------------
// 注册消失
//---------------------------------------------------------------------
public:
	void AddDelEvent(DWORD dwTime);		// 注册删除事件
	void LogoutDelEvent();				// 注销删除事件
	void CleanDelEvent();				// 清空删除事件
	virtual void OnDelEvent(){}		// 响应删除事件
	
	//注册更新属性的定时器
	void RegisterUpdateAttrTimer();
	//注销更新属性定时器
	void CancelUpdateAttrTimer();
	//触发属性定时器
	void OnUpdateAttrTimeOut();
	//重置发送消息的时间,如果超时则返回true
	bool ResetSendMoveMsgTime();

private:
	LONG m_lDelEventTimerID;			// 注册消失时间
	long	m_lUpdateAttrTimerID;	//更新实体属性到客户端的定时器
	//用来标记更新远处玩家移动消息的时间
	DWORD	m_dwLastSendMoveSendTime;

//---------------------------------------------------------------------
//  任务保护标志
//---------------------------------------------------------------------
private:
	bool m_bRoleProtect;
public:
	void SetRoleProtect(bool l){m_bRoleProtect= l;}
	bool GetRoleProtect(){return m_bRoleProtect;}

	/////////////////////////////////////////////////////////////////////
	// 消息响应
	/////////////////////////////////////////////////////////////////////
public:
	virtual void OnMessage(CMessage* pMsg)	{return;}
	void OnShapeMessage(CMessage* pMsg);
	virtual long OnTimeOut(DWORD timerid,DWORD curtime,const void* var);
	virtual long Proc(eEventType type,const tagProcVar *pVar);
	VOID UpdateAbnormality();			// 异常状态更新
	//////////////////////////////////////////////////////////////////////////
	//	行走
	//////////////////////////////////////////////////////////////////////////
private:
	bool						m_bRun;							//移动方式是否是跑
	bool						m_bMoveable;
	LONG						m_lMoveableCount;

	bool						m_bUseFighterSkill;				//使用战斗技能标志符
	bool						m_bUseRabbSkill;				//使用法师技能标志符
	bool						m_bUseItem;						//使用道具的标志符
	bool						m_bAntiInvisibleable;			//是否反隐形
	bool						m_bInvisible;					//不可见状态
	bool						m_bHide;						//隐蔽		
	bool						m_bPretendDead;					//装死
	bool						m_bInstruction;					//使用指令标志符
	bool						m_bUnAttack;					//不被攻击标志符
	bool						m_bAddHp;						//Hp不能增加的标志符
	long						m_lChangeModelType;				//变身类型
	long						m_lChangeModelID;				//变身id
	bool						m_bForceAttack;					//强制攻击
	bool						m_bRandomMove;					//随机跑动
	bool						m_bRide;						//非坐骑状态
	CGUID						m_BefoolID;						//愚弄自己的id
private:
	bool						m_bWeakness;					//虚弱状态
	long						m_lWeakCount;					//虚弱计数
public:
	bool						GetWeaknessState(){return m_bWeakness;}
	void						SetWeakNessState(bool bWeak);
	//改变虚弱状态
	virtual void				ChangeWeaknessState(bool l){}
public:
	
	virtual VOID				OnCannotMove();
	virtual VOID				SetPosition( LONG lX, LONG lY );
	virtual VOID				OnMove(float fNewX, float fNewY);
	virtual VOID				OnStopMove();
	virtual VOID				SetSlipPos( LONG lX, LONG lY,float fSpeed,float fDir);

	virtual void OnQuestMoveStep(float fCurX,float fCurY,float fDestX,float fDestY){return;}
	virtual void OnQuestStopMove(float fCurX,float fCurY,short lDir){return;}

	bool						GetIsRide()	{return m_bRide;}
	void						SetIsRide(bool bRide){m_bRide = bRide;}

	bool						GetIsRun()	{return m_bRun;}
	void						SetIsRun(bool bRun){m_bRun = bRun;}

	virtual bool				GetForceAttack(){return m_bForceAttack;}            //强制攻击
	void						SetForceAttack(bool b){m_bForceAttack = b;}

	bool						GetRandomMove(){return m_bRandomMove;}				//随机跑动
	void						SetRandomMove(bool b){m_bRandomMove = b;}	

	virtual const CGUID&		GetBefoolID() {return NULL_GUID;}//return m_BefoolID;}	
	virtual VOID				SetBefoolID(const CGUID& guid){m_BefoolID=guid;}					//设置愚弄自己的id

public:
	void			SetUseFightSkill(bool b);					//设置使用战士技能标志符
	bool			GetUseFightSkill(){return m_bUseFighterSkill;}

	void			SetUseRabbSkill(bool b);					//设置使用法师技能标志符
	bool			GetUseRabbSkill(){return m_bUseRabbSkill;}

	void			SetUseItem(bool b);							//设置使用道具标志符
	bool			GetUseItem(){return m_bUseItem;}

	void			SetUseInstruction(bool b){m_bInstruction = b;}//玩家使用指令标志符
	bool			GetInstruction(){return m_bInstruction;}

	void		    SetUnAttack(bool b){m_bUnAttack = b;}
	bool			GetUnAttack(){return m_bUnAttack;}			 //不能被攻击

	void			SetAddHp(bool b){m_bAddHp = b;}
	bool			GetAddHp(){return m_bAddHp;}

	void			SetChangeModelType(long l){m_lChangeModelType = l;}	//改变模型类型
	long			GetChangeModelType(){return m_lChangeModelType;}

	void			SetChangeModelID(long l){m_lChangeModelID = l;}		//改变模型id
	long			GetChangeModelID(){return m_lChangeModelID;}

	virtual VOID	SetAntiInvisibleable(bool l){m_bAntiInvisibleable=l;}			//设置是否反隐形
	bool			GetAntiInvisibleable(){return m_bAntiInvisibleable;	}

	virtual VOID	SetInvisibleState(bool l){m_bInvisible=l;}			//设置是否不可见状态
	bool			GetInvisibleState(){return m_bInvisible;}

	virtual VOID	SetPretendDeadState(bool l){m_bPretendDead=l;}			//设置是否装死
	bool			GetPretendDeadState(){return m_bPretendDead;}	
	virtual VOID	SetHide(bool l){m_bHide=l;}								//隐蔽
	bool			GetHide(){return m_bHide;}

	//根据目标的状态来判断是否可见
	virtual bool	IsVisible(CMoveShape* pTarget);

	//! 
	//!*************************
	void			GetAllBuffID(list<pair<DWORD, DWORD>>& listBuff);	//! 得到所BUFF的ID
	virtual	void	OnAddBuff(DWORD dwStateID, DWORD dwBuffLevel);
	virtual	void	OnDelBuff(DWORD dwStateID, DWORD dwBuffLevel);
	//!*************************


	

	virtual void	InitSkillInstance(DWORD dwSkillID,DWORD dwLevel,long lGoodsIndex=0,long lContainerID=0,long lPos=0,CGUID GoodsGuid=NULL_GUID,long lType=0);
	virtual void	AddActiveSkill(stModuParam *pSkillParam);

	virtual long	AddActiveSkill(DWORD dwSkillID, DWORD dwLevel,CMoveShape *pTarget=NULL,long lGoodsIndex=0,long lContainerID=0,long lPos=0,CGUID GoodsGuid=NULL_GUID);				//添加主动技能

	virtual stModuParam *AddPassiveSkill(DWORD dwSkillID, DWORD dwLevel);		//添加被动技能
			void	AddPassSkill(DWORD dwSkillID, DWORD dwLevel);
			long	GetAllStateNum();											//获取所有状态个数
			long    GetStateParamByID(DWORD dwID,string strParam);				//通过状态id和参数名称得到状态的相关参数
			long	GetStateParamByIndex(DWORD dwIndex,string strParam);		//通过状态索引和参数名称得到状态的相关参数
	virtual	void	SetPkValue(DWORD lValue){}									//设置PK值
																										  

	//发送变化的速度值到客户端
	void			SendChangedSpeedToClients();
	long			GetDestDir		(long sx, long sy, long dx, long dy);			// 获取目标方向
	virtual void	GetBeAttackedPoint(long lAttackerX,long lAttackerY,long& lTileX,long& lTileY)
	{
		lTileX = GetTileX();lTileY = GetTileY();
	}
	virtual long	GetAttackerDir(long lAttackerX,long lAttackerY,long ldx,long ldy)
	{return GetDestDir(lAttackerX,lAttackerY,ldx,ldy);}
	void SetPosXY(float fX,float fY );

	virtual void	InitAI(){};

	//##强制在dwTime时间内移动到坐标x,y，
	VOID			ForceMove		( LONG lDestX, LONG lDestY, DWORD dwTime );

	virtual long	GetStopFrame	() {return 0;}						// 获取在格子中心停留的桢数
	VOID			SetMoveable		( BOOL bMoveable = TRUE );	

	virtual CBaseAI* GetAI() {return m_pAI;}



	//////////////////////////////////////////////////////////////////////////
	//	战斗
	//////////////////////////////////////////////////////////////////////////
public:
	// 战斗公式计算用的虚函数
	virtual DWORD	GetExp() {return 0;}			//	经验值
	virtual DWORD	GetExpDead() {return 0;}		//	杀死得到的经验值
	virtual DWORD	GetHP() {return 0;}
	virtual void	SetHP(DWORD l) {}
	//设置MP
	virtual WORD	GetMP() {return 0;}
	virtual void	SetMP(DWORD l) {}

	virtual DWORD	GetMaxHP() {return 0;}	

	virtual WORD	GetMinAtk() {return 0;}
	virtual WORD	GetMaxAtk() {return 0;}
	virtual WORD	GetHit(){return 0;}
	virtual WORD	GetDef() {return 0;}
	virtual WORD	GetDodge() {return 0;}
	virtual WORD	GetAtcSpeed() {return 0;}
	virtual WORD	GetElementResistant(){return 0;}
	virtual WORD	GetSoulResistant() {return 0;}
	virtual WORD	GetHpRecoverSpeed() {return 0;}
	virtual WORD	GetMpRecoverSpeed() {return 0;}
	
	virtual WORD	GetLevel()	{return 1;}
	virtual WORD	GetCCH()	{return 0;}
	virtual WORD	GetAddElementAtk() {return 0;}
	virtual WORD	GetAddSoulAtk()	{return 0;}


	virtual bool IsAttackAble(CMoveShape* pAttacker/*攻击者*/) = 0;	// 可否攻击
	//判断是否可以攻击
	virtual bool IsAttackAble() {return true;}
	//判断是否可以被攻击
	virtual bool IsBeAttackAble() {return true;}

	virtual long GetAlertRange() {return 10;}
	virtual long GetTrackRange() {return 16;}
	virtual WORD GetReAnk()	{return 0;}

	virtual	WORD	GetAttackAvoid()			{return 0;}
	virtual	WORD	GetElementAvoid()			{return 0;}
	virtual	WORD	GetFullMiss()				{return 0;}

	
	void God( bool bIsGod = true )		{ m_bIsGod = bIsGod; }
	bool IsGod()						{ return m_bIsGod; }

	virtual bool CanMove()	{return m_bMoveable;}	//是否能移动	

	//!									技能加成开始（返回一个KEY）
	virtual	long						SkillReinforceBegin(bool isMagic, bool isAttack);
	//!									技能加成结束（清空效果缓存）
	virtual	void						SkillReinforceEnd(long lKey);
	//!									获得技能加成值（first：吟唱时间变化比例；second：效果值变化比例）
	virtual	const pair<float, float>&	GetSkillReinforceAmend(long lKey);
	//!
	virtual float						GetReinforceByEnum(long eValuePos,BOOL bMagic){return 0;}
	virtual bool						ExpendPower(BOOL isMagic){return true;}

protected:
	bool	m_bIsGod;				// 是否是无敌模式
	pair<float, float>				m_FixedReinforce;

protected:
	struct tagScriptTimer
	{
		long lUserData;
		string	strScript;
		bool bLoop;//是否循环
	};
	//注册的脚本定时器标量
	typedef map<long,tagScriptTimer>	mapScriptTimers;
	typedef map<long,tagScriptTimer>::iterator itST;
	//被注册的脚本定时器
	mapScriptTimers m_ScriptTimers;

public:
	//定制一个脚本定时器
	long RegisterScriptTimer(long lFutureTime,long lInterTime,
							const char* pszScriptName,long lUserData);
	//注销一个脚本定时器
	long UnRegisterScriptTimer(long lUserData,long lTimerID);
	//脚本定时器触发
	void OnScripttTimeOut(long lTimerID);
	//清除所有脚本定时器
	void ClearScriptTimer();


	virtual CGoods* FindGoods(LONG lContainerId,LONG lPos){return NULL;}
	virtual DWORD FindGoodsByName(LONG lContainerId,const char* strGoodsName){return 0;}
	virtual DWORD FindGoodsByType(LONG lContainerId,int nType){return 0;}
	virtual DWORD FindGoods(LONG lContainerId,LONG lPos,const char* strGoodsName){return 0;}

	//-------------------------------------------------------------
	// 战斗的处理
	//-------------------------------------------------------------
	/**## 加在这个MoveShape身上的异常状态列表 */
protected:
	typedef map<LONG,stModuParam*>::iterator itModu;
	typedef map<LONG,stModuParam*>	mapModules;

	//状态读数器
	map<tagModuleKey,long,tagKeycmp> m_mpStateCount;
	
	//buff动态参数
	mapModules m_BuffParam;
	//debuff动态参数
	mapModules m_DeBuffParam;
	//主动技能动态参数
	mapModules m_ActiveSkillParam;
	//被动技能动态参数
	mapModules m_PassiveSkillParam;
	//正在使用的主动技能的动态参数
	stModuParam* m_pInstanceParam;
	//正在使用的主动技能
	stModuParam *m_pInstanceSkill;
	
	
	//主动技能唯一标志符
	long m_ActiveID;

	friend class CPlayerAI;
	DWORD	m_dwIntonateStartTime;			//吟唱开始时间
	DWORD	m_dwIntonateTime;				//吟唱时间
	DWORD	m_dwActStartTime;				//动作开始时间
	DWORD   m_dwActTime;					//动作时间
	long	m_lAttackerType;				//攻击者类型
	CGUID   m_guidAttacker;					//攻击者guid

	CGUID   m_guidAttackTarget;				//攻击目标GUID
	long	m_lAttackTargetType;			//攻击目标类型

	CGUID   m_guidTarget;					//选择目标GUID
	long	m_lTargetType;					//选择目标类型
	CGUID   m_guidKiller;					//刺杀者guid
	long    m_lKillerType;					//刺杀者类型

	hashsetGUID	m_SelectSelfPlayers;		//选中自己玩家对象

public:
	stModuParam  *InitModuParam(DWORD dwSkillID, DWORD dwLevel, DWORD dwGlobleID);					//创建动态参数
	stModuParam  *GetInstanceSkill(){return m_pInstanceSkill;}										//获取动态参数
	void		 SetInstanceSkill(stModuParam *pInstanceSkill){ m_pInstanceSkill=pInstanceSkill; }	//设置动态动态参数
	void		 EndAllActiveSkill();																//结束清除主动技能
	void		 AddStateCount(stModuParam *pState);												//加状态计数器
    void		 SubStateCount(stModuParam *pState);												//减状态计数器	
	void		 SetTarget(CGUID guid,long lType);													//设置对象参数
	void		 GetTarget(CGUID &guidTarget,long &lTargetType);									//获取对象参数
	void		 SetKillerInfo(CGUID guid,long lType);												//设置刺杀者参数
	void		 GetKillerInfo(CGUID &guidKiller,long &lKillerType);								//获取刺杀者参数
	void		 SetAttackTarget(CGUID guid,long lType);											//设置攻击目标参数
	void		 GetAttacktarget(CGUID &guid,long &lType);											//获取攻击目标参数	
	void		 RemoveState(itModu &it);															//删除状态
	void		 RemoveState(mapModules &mpBaseModule,DWORD dwStateID,DWORD dwLevel,DWORD dwCount); //删除状态
	void		 ClearAllBuff();																	//清除所有的状态
	void		 EndBuffAndDebuff();																//结束所有的状态
	void		 EndDeBuff();																		//结束所有不良状态
	void		 EndState(CBaseModule *pBaseModule);												//结束状态
	void		 EndSkill(CBaseModule *pBaseModule);												//结束技能
	void		 AddNewState(CMoveShape *pUser, DWORD dwStateID, DWORD dwLevel,long lType);			//添加Buff
	void		 StartUpState(CMoveShape *pUser, stModuParam *pModuParam, mapModules &mapState);	//启动状态
	void		 StartUpState(mapModules &mapState);												//启动状态
	void		 ClearState(mapModules &mapState);													//清除状态
	void		 EndState(mapModules &mapState);													//结束状态
	void		 DelNewState(DWORD dwStateID, DWORD dwLevel,DWORD dwCount=0);						//删除状态
	void		 EndActiveSkill(long lID,long lKeyID);												//结束主动技能
	void		 StartAllBuff();																	//重新开始所有的状态
	void		 RemoveActive(CBaseModule* pActiveSkill);											//移除主动技能
	void		 RemoveBaseModule(stModuParam* pModuParam);											//移除动态参数
	void		 ReleaseBaseModule(stModuParam* pModuParam,mapModules &mpBaseModule);				//释放动态参数
	void		 SetAttacker(long lType,const CGUID guid);											//设置攻击者的类型和guid
	bool		 EndPassiveSkill(DWORD dwID,DWORD dwLv);											//结束被动技能
	long		 GetAttackerType() {return m_lAttackerType;}										//获取攻击者类型
	long		 GetStateCount(stModuParam *pState);												//获取状态数量
	CBaseModule *GetActiveSkill(long lActiveID);													//获取主动技能实例
	CGUID		&GetAttackerGuid()   {return m_guidAttacker;}										//获取攻击者guid
	mapModules	 GetBuff()			{return m_BuffParam;}											//获取BUFF的动态参数	
	virtual void EndBuff(DWORD dwStateID);															//根据ID结束状态

	void	AddSelectSelfPlayer(const CGUID& PlayerID);
	void	RemoveSelectSelfPlayer(const CGUID& PlayerID);
	hashsetGUID& GetSelectSelfPlayer()	{return m_SelectSelfPlayers;}	
	//冷却时间处理
	bool		 CooldownPast(long lSkilllD)	{return true;}
	virtual void SetCommonCooldown(long lID, DWORD dwTime){}
	virtual void SetSkillCooldown(long lID,DWORD dwTime){}


	//##初始化最基本的技能信息
	virtual VOID	InitSkills				();
	virtual void	SetIntonateTime(DWORD dwStartTime, DWORD dwTime){m_dwIntonateStartTime=dwStartTime,m_dwIntonateTime=dwTime;}
	virtual void	SetActTime(DWORD dwActStartTime,DWORD dwAtcTime){m_dwActStartTime=dwActStartTime,m_dwActTime=dwAtcTime;}
	virtual BOOL	IsInAct();								//返回TRUE表示动作进行中,FALSE表示动作已过
	virtual long	GetActStartTime(){return m_dwActStartTime;}
	virtual long    GetActTime(){return m_dwActTime;}


	VOID						ClearSkills				();
	VOID						ClearPassiveSkill		();

	BOOL AddSkill( const char* strName, LONG LLevel = 1 );		// 添加技能
	BOOL AddSkill( tagSkillID tID, LONG lLevel = 1 );
							// 删除技能
	virtual BOOL DelSkill( long lSkillID,long lSkillLv);
	BOOL SetSkillLevel(const char* strName, LONG lLevel);		// 设置技能等级
	
	virtual void AddSkill(long id,long lv){}
	//检测buff
	int CheckBuff(int lID,int lLvl)	{return 0;}
	//检测skill



	//##该虚函数实际上是由技能在AI轮询开始的时候调用，其他时候不调用。
	//##返回的布尔值用于判定是否需要停止该技能。TRUE表示允许，FALSE表示不允许。
	virtual BOOL				OnBeginSkill( DWORD dwID,DWORD dwLv );

	//##该虚函数实际上是由技能在AI轮询停止的时候调用，其他时候不调用。
	virtual VOID				OnEndSkill				() {}

	//物品逆
public:
	virtual int	 CheckGoods(const char *goodoriname)	{return 0;}//检查物品是否有该物品，返回物品的个数

public:
	//##攻击者攻击防御者时，将自身的攻击强度和类型等信息传递给
	//##防守方，并且由防御者算出最终的防御结果。
	virtual long				OnBeenAttacked			( CMoveShape *pAttacker,long lHp,long lDamageResult=0);
	virtual void				OnBeenKilled(CMoveShape* pAttacker);

	virtual bool				OnIncreaseHp			(long &lHp);

	//被使用增益技能处理
	virtual VOID				OnBeenAddAppendSkill	(CMoveShape *pAttacker);
	

	virtual VOID				OnChangeStates			();



	VOID						StopAllSkills			();
	VOID						StopPassiveSkills		();

	VOID						StopSkills				(long lID,long lLv,long lType);
	VOID						StopActiveSkill			(long lID,long lLv);
	VOID						StopPassiveSkill		(long lID,long lLv);
	

	//释放主动技能参数
	VOID						ClearActiveSkills			();
	VOID						ClearAllStates			(bool bIsDead = false);

	VOID						StartAllStates			(bool bIsRelive = false);

	//##可能召唤兽会有切换场景的需求.
	virtual VOID				OnEnterRegion			(bool bIsRelive = false);
	virtual VOID				OnExitRegion			(){}

	///////////////////////////////////////////////////////////////
	//	Shape战斗处理
	///////////////////////////////////////////////////////////////
protected:
	LONG						m_lCanFightCount;	//##计数器
	bool						m_bCanFight;		//##能否进攻别人
	bool						m_bAttackAble;		//##可否攻击
	

public:
	virtual void				SetAttackAble	( bool b )			{m_bAttackAble = b;}
	virtual bool				GetAttackAble	()					{ return m_bAttackAble; }
	virtual void				OnRelive(long lType,long lRecoverRatio=50){}

	VOID						SetFightable	( bool b );
	bool						IsFightable		() { return m_bCanFight; }
	


	//##判定自身是否死亡
	BOOL						IsDied			();
	
	//##自身死亡后的处理
	virtual void				OnDied			(){}
	//当位置改变之后触发
	virtual void OnPosChanged(){return;}

	//##这个函数主要是为了判断杀人者
	virtual VOID				OnBeenMurdered	( LONG lMurdererType, const CGUID& MurdererID ) {}
	virtual VOID				OnBeenHurted	( LONG lAttackerType, const CGUID& AttackerID ) {}
	//删除物品
	virtual BOOL DelGoods(LONG lContainerId,LONG lPos){return TRUE;}
	virtual BOOL DelGoods(char* strOriginName,int num){return TRUE;}
	virtual DWORD DeleteGoods(CS2CContainerObjectMove::PLAYER_EXTEND_ID pei,eGoodsBaseType baseType,DWORD dwWeaponType,DWORD dwNum){return 0;}
	//! 删除物品并通知玩家
	virtual void DelGoodsAndSendMsg(long lContainerId, long lPos){}

	//##获取武器的强度对目标的伤害的修正值。
	virtual FLOAT				GetWeaponModifier( LONG lTargetLevel ){ return 1.0f; }
	enum tagDeathType
	{
		DT_BEEN_KILLED			= 1,
		DT_SELF_BLASTING		= 2,
		DT_EVANISH				= 3,
	};

	///////////////////////////////////////////////////////////////
	//	杀死自己的人（砍最后一刀的）
	///////////////////////////////////////////////////////////////
protected:
	tagAttackInformation	m_KilledMeAttackInfo;


public:
	VOID SetKilledMeAttackInfo(LONG lType, CGUID guid);
	const tagAttackInformation& GetKilledMeAttackInfo()const {return m_KilledMeAttackInfo;}

	long GetKilledMeType()			{return m_KilledMeAttackInfo.lAttackerType;}
	const CGUID& GetKilledMeID()			{return m_KilledMeAttackInfo.AttackerID;}


	bool AddBuffToByteArray	(vector<BYTE>* pByteArray);


	virtual bool CodeToDataBlock_ForClient( DBWriteSet& setWriteDB, bool bExData = TRUE );
	bool CodeBuffToDataBlock_ForClient( DBWriteSet& setWriteDB);
	bool CodeDeBuffToDataBlocky_ForClient( DBWriteSet& setWriteDB);
	bool CodeActSkillToDataBlock_ForClient( DBWriteSet& setWriteDB);
	bool CodeOnOffSkillToDataBlock_ForClient( DBWriteSet& setWriteDB);
	bool CodeBuffToDataBlock	(DBWriteSet& setWriteDB);
	bool DecodeBuffFromDataBlock(DBReadSet& setReadDB);


	//-------------------------------------------------------------
	//	一些和战斗相关的状态虚函数
	//-------------------------------------------------------------
	virtual VOID				EnterCriminalState	()	{}
	virtual VOID				EnterResidentState	()  {}

	virtual VOID				ChangeStateTo		(CShape::eSTATE);
	virtual VOID				UpdateCurrentState	()	= 0;

	//-------------------------------------------------------------
	// 其他辅助函数
	//-------------------------------------------------------------

	//##检测2个MoveShape之间是否面对面。
	BOOL						IsFaceTo			( CMoveShape* pShape );	
	//---------------------------------------------------------------
	//  采集相关
	//---------------------------------------------------------------
protected:
	//对应采集物名字
	string m_strCollectionName;
	//采集物id
	DWORD	m_dwCollectionID;
	//能被采集的次数
	DWORD	m_dwCollectTimes;
	//是否正在被采集
	bool	m_bIsBeenCollect;
	//是否能被采集
	bool	m_bCanBeenCollect;
public:
	const string& GetCollectionName()			{return m_strCollectionName;}
	VOID	SetCollectionName(const string & l)	{m_strCollectionName=l;}
	DWORD	GetCollectionID()				{return m_dwCollectionID;}
	VOID	SetCollectionID(DWORD l);
	DWORD	GetCollectionTimes()			{return m_dwCollectTimes;}
	VOID	SetCollectionTimes(DWORD l);
	bool	GetIsBeenCollect()				{return m_bIsBeenCollect;}
	VOID	SetIsBeenCollect(bool l);
	bool	GetCanBeenCollect()				{return m_bCanBeenCollect;}
	VOID	SetCanBeenCollect(bool l);
public:
	BOOL	BeenCollected();
	virtual BOOL	BeenCollectedAll(){return FALSE;}
	virtual void OnDiedEvent(){}

	friend class CBaseAI;
	friend class CMonsterAI;
	friend class CGuardWithSword;
	friend class CCityGuardWithSword;
	friend class CVilCouGuardWithSword;
	friend class COpPosition;
	friend class CColony;

	// 重载接口
	virtual int SetScriptVarValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName, const char* value);
	virtual int SetScriptVarValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName, long lArray=0, double value=0);
	virtual const char* GetScriptVarValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName);
	virtual double GetScriptVarValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName, long lArray);

	virtual int SetScriptGuidValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName, const CGUID& guid);
	virtual const CGUID& GetScriptGuidValue(CScript* scriptPtr/*调用的脚本指针*/, const char* varName);

	virtual CVariableList* GetVariableList(void);


};
