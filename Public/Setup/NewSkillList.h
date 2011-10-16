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
        string paramvalue;  //字符串参数
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
        string strOper;     //运算方式,主要从玩家身上读取等级配置进行比较
		string strObject;	//检查对象
        string strBufObject;//验证buf存在对象
        string strLevel;    //等级变量名
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
        float value;
		string strValueName;    //值
		string strCmpFun;	    //操作符
		string strRefObj;	    //参照对象
	};
    //几率条件
    struct tagOddsCondition 
    {
        short   value;          //几率值
        string  strCmpFun;      //几率比较函数
        string  strValueName;   //几率值名字
    };
    //驱散条件
    struct tagDispelCondition 
    {
        short  Num;             //驱散数量
        string strDispelNum;    //驱散字符串
        string strDispelType;   //驱散类型
    };

	//事件条件
	struct tagEventCondition 
	{
		DWORD  dwFlag;
		DWORD  dwVar1;
		DWORD  dwVar2;
		DWORD  dwVar3;				//类型
		DWORD  dwVar4;				//小类型
        string dwSkillId;           //用于检测事件的特定特定技能注册
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
		list<tagSkillCondition*>   ltgSkillCondition;
	};

    //几率条件体
    struct tagConOdds 
    {
        list<tagOddsCondition*>    ltgOddsCondition;
    };

    //驱散条件体
    struct tagConDispel
    {
        list<tagDispelCondition*>  ltgDispelCondition;
    };

	//距离条件体
	struct tagConDistance 
	{
		list<tagDistanceCondition*> ltgDistanceCondition;		
	};
	//事件条件体
	struct tagConEvent 
	{
		list<tagEventCondition*>	ltgEventCondition;
	};
	//时间
	struct tagConTimer
	{
		list<DWORD>					ldwConTimer;
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
        BYTE   IsWithIn;                //是否的目标的范围内
		DWORD  dwID;					//ID
		DWORD  dwSkillID;				//技能ID
		DWORD  dwSkillLevel;			//等级
		DWORD  dwConGroupID;			//条件组ID
		DWORD  dwNumObject;				//对象个数
		float  distance;				//距离
		float  radius;				    //半径
        float angle;                    //扇形角度
        float width;                    //矩形宽度
		string strType;					//类型
		string strPosition;				//位置		
		string strCenterObject;			//中心对象		
		string strObject;				//对象
        string NumObjectName;           //对象数量参数名字
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
        string paramname;               //程序参数名
	};
	//改变状态
	struct tagChangeBuff :public tagBaseChange
	{
		DWORD dwID;						//状态ID
		DWORD dwLevel;					//等级		
		DWORD dwCount;					//数量
        string strSIDName;				//ID变量名		
        string strSLvName;				//等级变量名	
        string strSCountName;			//数量变量名
		string strOperator;				//操作符
        string strSkillType;            //技能类型
	};

	//位置修改
	struct tagChangePos :public tagBaseChange
	{		
		string strRefObject;			//相对位置
		string strDir;					//方向
		string strStyle;				//方式
		string strSpeed;					//移动速度
        float dis;
	};

	//冷却时间
	struct tagCoolDownChange :public tagBaseChange
	{
		DWORD  dwValue;					//值	
		string strType;					//类型	
		string strSVarianame;			//脚本变量名
        string paramname;               //参数名
	};
	//注册时间
	struct tagRegisterTimer :public tagBaseChange
	{
		DWORD  dwFlag;					//对象	
		DWORD  dwIntelTime;				//时间		
		DWORD  dwFuture;				//触发时间		
		DWORD  dwCount;					//次数
		string strSIntelName;		    //间隔时间脚本变量
		string strSFutvarname;			//触发时间脚本变量
		string strSCountName;           //定时次数脚本变量
        string strVFuturename;          //间隔时间变量
		string strVIntername;           //触发时间变量
        string strVCountname;           //次数变量
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
        string strDamagename;           //伤害名变量
	};	
	//do 伤害
	struct tagDoDamage :public tagBaseChange
	{
		DWORD dwResult;					//伤害结果
		DWORD dwBlastAttack;			//暴击值
		string strSResVariName;			//伤害结果的脚本变量
		string strSBlastVariName;		//暴击脚本变量
        string strDamagename;           //伤害名变量
        string strBlastname;            //暴击名变量
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
    //关联天赋
    struct tagGeniusChange:public tagBaseChange
    {
        string strGenius;
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
		tagStepConditon					tgCondition;	  //条件
		list<tagDoProcess*>				ltgTrueDoProcess; //真操作
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

	struct tagSkill
	{
		DWORD					 dwTemplateId;					//技能Complateid	
		map<DWORD,tagCondition*> mapCondition;					//条件体 	
		list<tagProcess*>		 ltgExecuteStep;				//阶段	
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
    string                          m_strAttType;               //技能攻击类型
    DWORD                           m_CDControl;                //CD控制时间
	uchar							m_bUseType;					//类别
	string							m_strAddSub;				//增益和减益的标志符Add表示增益Sub表示减益	
	uchar							m_bOnly;					//唯一标志符1表唯一0表非唯一
	DWORD							m_dwOnlyType;				//唯一类型
	uchar							m_bNameOnly;				//同名唯一标志符
	uchar							m_bRoleOnly;				//角色唯一标志符
	uchar							m_bAttack;					//攻击和非攻击技能标志符1表示攻击0表示非攻击
	uchar							m_bTarget;					//目标标志符
	uchar							m_bCooldown;				//冷却保存标志符1是保存0不保存
	uchar							m_bDeadUse;					//能否对死亡目标使用技能标志符0表示不能使用1表示可以使用
	uchar							m_bDeadEnd;					//死亡是否结束状态0表示结束1表示结束
	uchar							m_bBreak;					//中断技能施放标志符(1表示产生中断0表示不中断)
	uchar							m_bPetType;					//宠物技能类型
	uchar							m_bTime;					//状态计时标志符(1表示在离线下状态计时0表示离线下状态不计时)
	uchar							m_bSystem;					//系统状态标志符
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
    void    SetSkillAttType     ( string strAttType)                                    { m_strAttType = strAttType;}           //设置技能攻击类型
    void    SetCDControl        ( DWORD  Time)                                          { m_CDControl  = Time;      }           //CD控制时间
	void	SetUseType			( uchar bUseType)										{ m_bUseType = bUseType; }				//设置模块类别							
	void	SetOnly				( uchar bOnly)											{ m_bOnly = bOnly;}						//设置唯一标志符						
	void	SetOnlyType			( DWORD dwOnlyType)										{ m_dwOnlyType = dwOnlyType;}			//设置唯一类型							
	void	SetNameOnly			( uchar bNameOnly)										{ m_bNameOnly = bNameOnly;}				//名称唯一标志符						
	void	SetRoleOnly			( uchar bRoleOnly)										{ m_bRoleOnly = bRoleOnly;}				//角色唯一标志符						
	void	SetTotalNum			( DWORD dwNum)											{ m_dwTotalNum = dwNum;}				//累加数量																			
	void	SetAddSub			( string str)											{ m_strAddSub = str;}					//增减益标符
	void	SetAttack			( uchar bAttack)										{ m_bAttack = bAttack;}					//攻击技能标志符
	void	SetTarget			( uchar bTarget)										{ m_bTarget = bTarget;}					//目标标志符	
	void	SetCoolDown			( uchar bCooldown)										{ m_bCooldown = bCooldown;}				//冷却时间
	void	SetDeadUse			( uchar bDeadUse)										{ m_bDeadUse = bDeadUse;}				//对死亡目标使用技能标志符
	void	SetDeadEnd			( uchar bDeadEnd)										{ m_bDeadEnd = bDeadEnd;}
	void	SetBreak			( uchar bBreak)											{ m_bBreak	= bBreak;}					//中断标志符
	void	SetPetType			( uchar bType)											{ m_bPetType = bType;}					//宠物技能类型
	void	SetTime				( uchar bTime)											{ m_bTime	= bTime;}					//计时标志符
	void	SetSystem			( uchar bSystem)											{ m_bSystem = bSystem;}					//系统标志符

	void	SetFormat			( DWORD dwID, string& strFileName,string& strContend); //脚本
	void    SetOperateObject	( DWORD dwObjectID,tagOperateObject *ptgOperateObject)	{m_mapOperateObject[dwObjectID] = ptgOperateObject;}							//作用对象

	void    AddNewSkill			( tagSkill *pSkill);
	void	SetCondition		( DWORD dwID,tagCondition *tgCondtition);
	void	SetConGroup			( DWORD dwGroupID,tagConditionGroup *ptgConditionGroup);		
	
	map<DWORD, tagOperateObject*>	GetOperateObject	()  							{return m_mapOperateObject;}												
	map<DWORD,tagExpression>&		GetFormat			()								{return m_mapFormat;}		
	map<DWORD,tagCondition*>		GetCondition		()								{return m_mapCondition;}	 	
	map<DWORD,tagConditionGroup*>	GetConditionGroup	()								{return m_mapConditionGroup;}	
	string							GetSkillType		()								{return m_strType;}
    string                          GetSkillAttType     ()                              {return m_strAttType;}
    DWORD                           GetCDControl        ()                              {return m_CDControl; }
	DWORD							GetNewSkillID		()								{return m_dwID;}
	uchar							GetOnly				()								{return m_bOnly;}
	DWORD							GetOnlyType			()								{return m_dwOnlyType;}	
	//uchar							GetSkillAdd			()								{return m_bSkillAdd;}
	string							GetAddSub			()								{return m_strAddSub;}
	uchar							GetUseType			()								{return m_bUseType;}
	string							GetInitScriptName	()								{return m_strInitSricpt;}
	int								GetElement			()								{return m_dwElement;}
	uchar							GetNameOnly			()								{return m_bNameOnly;}
	uchar							GetRoleOnly			()								{return m_bRoleOnly;}
	DWORD							GetTotalNum			()								{return m_dwTotalNum;}
	uchar							GetAttack			()								{return m_bAttack;}
	uchar							GetTarget			()								{return m_bTarget;}
	uchar							GetCoolDown			()								{return m_bCooldown;}
	uchar							GetDeadUse			()								{return m_bDeadUse;}
	uchar							GetDeadEnd			()								{return m_bDeadEnd;}
	uchar							GetBreak			()								{return m_bBreak;}
	uchar							GetPetType			()								{return m_bPetType;}
	uchar							GetTime				()								{return m_bTime;}
	uchar							GetSystem			()								{return m_bSystem;}

	void	ClearCDo(tagCDo* ptgCDo);																					//释放CDo	
	void	ClearDo(tagBaseChange *ptgBaseChange);
	void	ClearCondition( void *pCondition,string strType);											//释放条件
};

#endif//_INC_NEWSKILLLIST_INCLUDED