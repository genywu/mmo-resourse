///////////////////////////////////////////////////////////////////////////////
///  NewSkillList.h
///
///  PATH: E:\alienbrainWork\Game_Program_Develop\guangmang\SERVER\setup
///
///  CREATED: 08/06/2007 13:49:01 PM by 陈先进
///
///  PURPOSE:   技能类
//////////////////////////////////////////////////////////////////////
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_NEWSKILLLIST_INCLUDED
#define _INC_NEWSKILLLIST_INCLUDED
#include <vector>
#include <list>
#include <string>
#include <map>
using namespace std;

class CNewSkillProperties
{
public:

	//属性条件
	struct tagPropertyCondition 
	{
		DWORD  dwValue;		//属性值	
		string strObject;	//检查对象
		string strName;		//属性名称
		string cmpareOp;	//比较操作符		
	};
	//物品附加属性
	struct tagGoodAddProperty
	{
		DWORD dwAddProType;	//附加属性类型
		DWORD dwValue1;		//值1
		DWORD dwValue2;		//值2	
	};
	//物品条件
	struct tagGoodCondition 
	{
		DWORD  dwValue;		//数量
		DWORD  dwPlace;		//地点
		DWORD  dwPos;		//位置
		DWORD  dwType;		//物品类别
		string strName;		//物品原始		
		string strCmpFun;	//操作符
		string strObject;   //检查对象
		
		list<tagGoodAddProperty*> ltgGoodAddProperty;
	};
	//状态条件
	struct tagBuffCondition
	{	
		DWORD  dwID;		//状态ID
		DWORD  dwLevel;		//等级
		DWORD  dwNum;		//数量
		string strObject;	//检查对象	
	};
	//技能条件
	struct tagSkillCondition 
	{		
		DWORD  dwID;		//技能ID
		DWORD  dwLevel;		//等级	
		string strObject;   //检查对象
	};
	//距离条件
	struct tagDistanceCondition
	{
		DWORD  dwValue;		//值
		string strCmpFun;	//操作符
		string strRefObj;	//参照对象
	};

	//事件条件
	struct tagEventCondition 
	{
		DWORD  dwFlag;
		DWORD  dwVar1;
		DWORD  dwVar2;
		DWORD  dwVar3;				//类型
		DWORD  dwVar4;				//小类型
		string strEventName;	
		string strName;		
	};
	struct tagBlockCondition
	{	
		DWORD  dwBlockType;//阻挡类型
		DWORD  dwShapeType;//shape类型
		string strType;	   //类型
		string strDir;	   //方向
	};
	struct tagFaceCondition 
	{
		string strFaceType;//1表示目标面对自己
	};
	struct tagValidObjectCondition
	{
		string       strType;		 //对象类型（包括PLAYER OR MONSTER）
		list<string> lObjType;		 //具体对象类型
	};

	struct tagRegionCondition 
	{
		string strName;
		DWORD dwType;
	};
	struct tagOccupationCondition
	{
		list<long>	lOccu;
	};


	//属性条件体
	struct tagConProperty
	{	
		list<tagPropertyCondition*> ltgProtertyConditon;	
	};

	//物品条件体
	struct tagConGood 
	{
		list<tagGoodCondition*>	  ltgGoodCondition;			
	};

	//状态条件体
	struct tagConBuff
	{
		list<tagBuffCondition*>	  ltgBuffCondition;			
	};

	//技能条件体
	struct tagConSkill 
	{
		list<tagSkillCondition*>	  ltgSkillCondition;
	};

	//距离条件体
	struct tagConDistance 
	{
		list<tagDistanceCondition*> ltgDistanceCondition;		
	};
	//事件条件体
	struct tagConEvent 
	{
		list<tagEventCondition*>	  ltgEventCondition;
	};
	//时间
	struct tagConTimer
	{
		list<DWORD>					 ldwConTimer;
	};
	//对象条件体
	struct tagConValidObject 
	{	
		list<tagValidObjectCondition*> ltgValidObjectCondition;		
	};
	//职业条件
	struct tagConOccupation
	{
		list<tagOccupationCondition*>	ltgOccupationCondition;
	};
	//阻挡条件
	struct tagConBlock
	{
		list<tagBlockCondition*>	 ltgBlockCondition;
	};
	//面向条件
	struct tagConFace 
	{
		list<tagFaceCondition*>		ltgFaceCondition;
	};
	//场景条件
	struct tagConRegion 
	{
		list<tagRegionCondition*>   ltgRegionCondition;
	};

	//脚本条件体
	struct tagConScript
	{
		string						strScript;
	};


	struct tagCondition 
	{
		DWORD  dwLength;
		string strType;		
		void*  pVoidCondition;
	};


	//作用对象
	struct tagOperateObject 
	{
		DWORD  dwID;					//ID
		DWORD  dwSkillID;				//技能ID
		DWORD  dwSkillLevel;			//等级
		DWORD  dwConGroupID;			//条件组ID
		DWORD  dwNumObject;				//对象个数
		DWORD  dwDistance;				//距离
		DWORD  dwRadius;				//半径
		string strType;					//类型
		string strPosition;				//位置		
		string strCenterObject;			//中心对象		
		string strObject;				//对象
	};

	struct tagBaseChange
	{
		string strChangeType;
		string strObj;
		virtual void GetTyp(){}
	};
	//改变属性
	struct tagChangeProperty :public tagBaseChange
	{
		DWORD  dwValue;					//值
		string strName;					//属性名称
		string strOperator;				//操作符		
		string strScriptVarName;		//脚本变量名	
	};
	//改变状态
	struct tagChangeBuff :public tagBaseChange
	{
		DWORD dwID;						//状态ID
		DWORD dwLevel;					//等级		
		DWORD dwCount;					//数量
		string strSIDName;				//ID脚本变量名		
		string strSLvName;				//等级脚本变量名	
		string strSCountName;			//数量脚本变量名
		string strOperator;				//操作符
	};

	//位置修改
	struct tagChangePos :public tagBaseChange
	{		
		string strRefObject;			//相对位置
		string strDir;					//方向
		string strStyle;				//方式
		string strSpeed;					//移动速度
	};

	//冷却时间
	struct tagCoolDownChange :public tagBaseChange
	{
		DWORD  dwValue;					//值	
		string strType;					//类型	
		string strSVarianame;			//脚本变量名
	};
	//注册时间
	struct tagRegisterTimer :public tagBaseChange
	{
		DWORD  dwFlag;					//对象	
		DWORD  dwIntelTime;				//时间		
		DWORD  dwFuture;				//触发时间		
		DWORD  dwCount;					//次数
		string strSIntelName;		
		string strSFutvarname;			
		string strSCountName;
		
	};

	//发送信息
	struct tagSendMsg :public tagBaseChange
	{
		DWORD  dwValue;					//值
		string strType;					//类型
		string strRange;				//范围				
		string strSVName;				//脚本变量名
		int	   nAdd;					//添加标志符（0为新加，1为更新(主要针对状态而设置)）
	};

	//注册事件
	struct tagRegisterEvent :public tagBaseChange
	{
		DWORD  dwFlag;					//对象
		string strName;					//注册事件名称
	};
	//伤害变化
	struct tagDamageChange :public tagBaseChange
	{
		DWORD  dwValue;					//数值
		string strOperator;				//操作符
		string strType;					//伤害类型
		string strSVarianame;			//伤害值脚本变量			
	};	
	//do 伤害
	struct tagDoDamage :public tagBaseChange
	{
		DWORD dwResult;					//伤害结果
		DWORD dwBlastAttack;			//暴击值
		string strSResVariName;			//伤害结果的脚本变量
		string strSBlastVariName;		//暴击脚本变量
	};
	//物品
	struct tagGoodChange :public tagBaseChange
	{
		DWORD  dwValue;					//数量
		string strOperator;				//操作符
		DWORD  dwPlace;					//容器ID
		DWORD  dwPos;					//栏位
		string strName;					//原始名
		string strSValname;				//数量脚本变量
		DWORD  dwGoodsType;				//物品类别
		DWORD  dwWeaponType;			//武器类别
	};

	//步骤
	struct  tagStepConditon 
	{
		DWORD dwConditionGroup;			//条件组ID
		string strTrueRet;			    //真结果
		string strFalseRet;			    //假结果
	};


	struct tagAttackChangeObject :public tagBaseChange 
	{
		string strAttackChangeObject;
	};

	//目标对象
	struct tagDestObjChange :public tagBaseChange 
	{
		string strDestObjChange;
	};

	//脚本
	struct tagScriptID :public tagBaseChange 
	{
		DWORD dwScriptID;
	};

	//注消时间
	struct tagUnRegisterTimer :public tagBaseChange 
	{
		DWORD dwUnRegisterTimer;
	};

	//注消事件
	struct tagUnRegisterEvent :public tagBaseChange 
	{
		DWORD dwUnRegisterEvent;
	};
	//方向
	struct tagDirection :public tagBaseChange
	{		
		string strDir;
	};
	//技能
	struct tagSkillChange :public tagBaseChange 
	{
		string strType;
		string strOpera;
		long   lID;
		long   lLv;
		string strSIDName;				//ID脚本变量名		
		string strSLvName;				//等级脚本变量名	
	};




	struct tagCDo;
		//过程
	struct tagDoProcess
	{
		list<tagBaseChange*>			ltgBaseChange;
		list<tagCDo*>					ltgCDo;
	};

	struct tagCDo
	{
		tagStepConditon					tgCondition;	//条件
		list<tagDoProcess*>				ltgTrueDoProcess;//真操作
		list<tagDoProcess*>				ltgFalseDoProcess;//假操作
	};


	//Do操作
	struct tagDo
	{
		DWORD						dwOperobjid;				//作用对象ID
		string						strRet;						//返回结果
		list<tagCDo*>				ltgCDo;
	};



	//执行步骤
	struct tagStep
	{
		string							strName;				//名称
		list<tagDo*>					ltgDo;					//为真操作				
	};



	//技能执行过程
	struct tagProcess 
	{
		string						strName;					//过程名称 	
		list<tagStep*>				ltgStep;					//执行步骤
	};

	//模块的静态参数
	struct stStaticParam
	{
		
		long lCoolDownTime;//冷却时间		
		long lConsMp;//消耗的法力	
		long lMinAtkDistance;	//最小攻击距离		
		long lMaxAtkDistance;//最大攻击距离		
		long lActTime;		//动作时间	
		string strBuffID;	//状态id		
		string strNBuffID;//NoBuffID		
		list<long> lBuffID;//状态id		
		list<long> lNBuffID;//NBuffID		
		string strScriptParam;//脚本变量		
		list<long> lScriptParam;//脚本变量
	};

	//技能主要结构
	struct tagSkill
	{
		DWORD					 dwLevel;						//等级	
		stStaticParam*			 pStaticParam;				    //静态参数
		map<DWORD,tagCondition*> mapCondition;					//条件体 	
		list<tagProcess*>		 ltgExecuteStep;				//阶段
		map<string,string>		 mapVarible;					//变量列表		
	};

	//条件组
	struct tagConditionGroup
	{
		string			strMode;
		list<DWORD>		lConditonGroup;
	};

	//脚本
	struct tagExpression
	{
		string strFileName;//文件名
		string strContent;//单行内容
	};

protected:


	DWORD							m_dwID;						//技能ID	
	string							m_strName;					//技能名称
	string							m_strInitSricpt;			//初始化脚本变量文件名
	DWORD							m_dwElement;				//元素类型
	string							m_strType;					//类型
	BYTE							m_bUseType;					//类别
	string							m_strAddSub;				//增益和减益的标志符Add表示增益Sub表示减益	
	BYTE							m_bOnly;					//唯一标志符1表唯一0表非唯一
	DWORD							m_dwOnlyType;				//唯一类型
	BYTE							m_bNameOnly;				//同名唯一标志符
	BYTE							m_bRoleOnly;				//角色唯一标志符
	BYTE							m_bAttack;					//攻击和非攻击技能标志符1表示攻击0表示非攻击
	BYTE							m_bTarget;					//目标标志符
	BYTE							m_bCooldown;				//冷却保存标志符1是保存0不保存
	BYTE							m_bDeadUse;					//能否对死亡目标使用技能标志符0表示不能使用1表示可以使用
	BYTE							m_bDeadEnd;					//死亡是否结束状态0表示结束1表示结束
	BYTE							m_bBreak;					//中断技能施放标志符(1表示产生中断0表示不中断)
	BYTE							m_bPetType;					//宠物技能类型
	BYTE							m_bTime;					//状态计时标志符(1表示在离线下状态计时0表示离线下状态不计时)
	BYTE							m_bSystem;					//系统状态标志符
	DWORD							m_dwTotalNum;				//同名状态最大叠加个数
	list<tagSkill*>					m_lSkill;					//技能列表
	map<DWORD,tagConditionGroup*>	m_mapConditionGroup;		//条件组
	map<DWORD,tagCondition*>		m_mapCondition;				//条件体 
	map<DWORD,tagExpression>		m_mapFormat;				//计算公式			
	map<DWORD,tagOperateObject*>	m_mapOperateObject;		    //操作对象 
	
public:
	CNewSkillProperties(void);
	CNewSkillProperties(DWORD dwSkillID);
	virtual ~CNewSkillProperties(void);

	void	SetNewSkillID		( DWORD dwID )											{ m_dwID = dwID;}						//设置ID								
	void	SetNewSkillName		( string strName)										{ m_strName = strName;}					//设置名称								
	void	SetDescription		( string strDescription )								{ m_strInitSricpt = strDescription;}	//设置初始化脚本变量的文件名			
	void	SetElement			( DWORD dwElement)										{ m_dwElement = dwElement;}				//设置元素类型							
	void	SetSkillType		( string strType )										{ m_strType = strType; }				//设置模块类型							
	void	SetUseType			( BYTE bUseType)										{ m_bUseType = bUseType; }				//设置模块类别							
	void	SetOnly				( BYTE bOnly)											{ m_bOnly = bOnly;}						//设置唯一标志符						
	void	SetOnlyType			( DWORD dwOnlyType)										{ m_dwOnlyType = dwOnlyType;}			//设置唯一类型							
	void	SetNameOnly			( BYTE bNameOnly)										{ m_bNameOnly = bNameOnly;}				//名称唯一标志符						
	void	SetRoleOnly			( BYTE bRoleOnly)										{ m_bRoleOnly = bRoleOnly;}				//角色唯一标志符						
	void	SetTotalNum			( DWORD dwNum)											{ m_dwTotalNum = dwNum;}				//累加数量																			
	void	SetAddSub			( string str)											{ m_strAddSub = str;}					//增减益标符
	void	SetAttack			( BYTE bAttack)											{ m_bAttack = bAttack;}					//攻击技能标志符
	void	SetTarget			( BYTE bTarget)											{ m_bTarget = bTarget;}					//目标标志符	
	void	SetCoolDown			( BYTE bCooldown)										{ m_bCooldown = bCooldown;}				//冷却时间
	void	SetDeadUse			( BYTE bDeadUse)										{ m_bDeadUse = bDeadUse;}				//对死亡目标使用技能标志符
	void	SetDeadEnd			( BYTE bDeadEnd)										{ m_bDeadEnd = bDeadEnd;}
	void	SetBreak			( BYTE bBreak)											{ m_bBreak	= bBreak;}					//中断标志符
	void	SetPetType			( BYTE bType)											{ m_bPetType = bType;}					//宠物技能类型
	void	SetTime				( BYTE bTime)											{ m_bTime	= bTime;}					//计时标志符
	void	SetSystem			( BYTE bSystem)											{ m_bSystem = bSystem;}					//系统标志符

	void	SetFormat			( DWORD dwID, string& strFileName,string& strContend); //脚本
	void    SetOperateObject	( DWORD dwObjectID,tagOperateObject *ptgOperateObject)	{m_mapOperateObject[dwObjectID] = ptgOperateObject;}							//作用对象

	void    AddNewSkill			( tagSkill *pSkill);
	void	SetCondition		( DWORD dwID,tagCondition *tgCondtition);
	void	SetConGroup			( DWORD dwGroupID,tagConditionGroup *ptgConditionGroup);		
	
	tagSkill						*GetNewSkill(DWORD dwLevel);
	map<DWORD, tagOperateObject*>	GetOperateObject	()  							{return m_mapOperateObject;}												
	map<DWORD,tagExpression>&		GetFormat			()								{return m_mapFormat;}		
	map<DWORD,tagCondition*>		GetCondition		()								{return m_mapCondition;}	 	
	map<DWORD,tagConditionGroup*>	GetConditionGroup	()								{return m_mapConditionGroup;}	
	string							GetSkillType		()								{return m_strType;}
	DWORD							GetNewSkillID		()								{return m_dwID;}
	BYTE							GetOnly				()								{return m_bOnly;}
	DWORD							GetOnlyType			()								{return m_dwOnlyType;}	
	//BYTE							GetSkillAdd			()								{return m_bSkillAdd;}
	string							GetAddSub			()								{return m_strAddSub;}
	BYTE							GetUseType			()								{return m_bUseType;}
	string							GetInitScriptName	()								{return m_strInitSricpt;}
	int								GetElement			()								{return m_dwElement;}
	BYTE							GetNameOnly			()								{return m_bNameOnly;}
	BYTE							GetRoleOnly			()								{return m_bRoleOnly;}
	DWORD							GetTotalNum			()								{return m_dwTotalNum;}
	BYTE							GetAttack			()								{return m_bAttack;}
	BYTE							GetTarget			()								{return m_bTarget;}
	BYTE							GetCoolDown			()								{return m_bCooldown;}
	BYTE							GetDeadUse			()								{return m_bDeadUse;}
	BYTE							GetDeadEnd			()								{return m_bDeadEnd;}
	BYTE							GetBreak			()								{return m_bBreak;}
	BYTE							GetPetType			()								{return m_bPetType;}
	BYTE							GetTime				()								{return m_bTime;}
	BYTE							GetSystem			()								{return m_bSystem;}
	stStaticParam					*GetStaticParam		(DWORD dwLevel);
	
};

#endif//_INC_NEWSKILLLIST_INCLUDED