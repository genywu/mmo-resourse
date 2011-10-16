//=============================================================================
/**
*  file: BuffSkillDef.h
*
*  Brief:提取buff,skill模块的基本定义
*
*  Authtor:wangqiao
*	
*	Datae:2007-8-31
*/
//=============================================================================

#ifndef __BUFFF_SKILL_DEF_H__
#define __BUFFF_SKILL_DEF_H__

#pragma once

class CMoveShape;
class CScript;
class CVariableList;
struct tagEvenRegVar;
struct tagProcVar;

//该模块函数返回值定义
enum eModuleRet
{
	MR_No = 0,//未知

	MR_Continue=1,//继续执行
	MR_Return,	//返回
	MR_Proc_Next,//下一个处理工序
	MR_Step_End,//Step结束
	MR_Proc_End,//处理工序结束
	MR_Modu_End,//模块结束
};

//定义模块类型
enum ModuleType
{
	ModuType_Buff,			//buff
	ModuType_DeBuff,		//debuff(持续伤害技能)
	ModuType_Skill_Active,	//主动技能
	ModuType_Skill_Passive,	//被动技能
	ModuType_Skill_OnOff,	//开关(行动)技能	
};
//定义增益减益标志符
enum eAddSub
{
	eNo_Type=0,
	eAdd_Type=1,
	eSub_Type,
};
//攻击标志符
enum eAttack
{
	eNoAttack=0,
	eYesAttack,
};
//目标标志符
enum eTarget
{
	eNoTarget=0,
	eYesTarget,
};
//技能类别
enum eSkillUseType
{
	SKILL_TYPE_NO=0,
	SKILL_TYPE_PHYSICAL=1,//物理类
	SKILL_TYPE_MAGIC,	//魔法类
};


struct stTimerParam : BaseMemObj
{
	//定时器ID
	long lTimerID;
	//预定触发时间
	long lFuture;
	//间隔时间
	long lInter;
	//调用次数
	DWORD nCount;
	//已经调用次数
	DWORD nCalledCount;
	//开始时间
	DWORD nStartTime;
	//标示类型ID
	long lFlag;		
	//该定时器关心的目标
	long lDestType;
	CGUID DestID;

	void GetRemainTime(DWORD &dwFutue,DWORD &dwCount);
};

struct stEventParam : public BaseMemObj
{
	//事件类型
	long lEventType;	
	//标示类型ID
	long lFlag;	
	tagEvenRegVar RegVar;
};

//模块的静态参数
struct stStaticModuParam : public BaseMemObj
{
	//冷却时间
	long lCoolDownTime;
	//消耗的法力
	long lConsMp;
	//最小攻击距离
	long lMinAtkDistance;
	//最大攻击距离
	long lMaxAtkDistance;
	//动作时间
	long lActTime;
	//
	list<long> lBuffID;
	//
	list<long> lNBuffID;
	//脚本变量
	list<long> lScriptParam;
};

//综合参数结构体
struct stModuParam : public BaseMemObj
{
	//类型
	ModuleType eType;
	//唯一id
	DWORD dwGlobleID;
	//ID和等级
	long nID;
	int nLvl;
	//类别
	eSkillUseType eUseType;
	//增益减益
	eAddSub eAddSubType;	
	//攻击标志符
	eAttack eTagAttack;	

	//执行脚本变量列表
	CVariableList *pVariableList;

	//初始化脚本变量名
	string strFileName;
	//制作技能ID
	DWORD dwCreateID;
	//施放者
	int nFirerType;
	CGUID FirerID;
	CMoveShape *pFirer;

	//目标标志符
	eTarget eTagTarget;
	//冷却时间
	long lCoolDownTime;
	//运行标志符
	bool bRun;
	BOOL bTime;
	

	//保存者
	CMoveShape *pUser;
	//鼠标点
	long nMouseX;
	long nMouseY;
	//鼠标点中的shape对象
	int nMouseShapeType;
	CGUID MouseShapeID;
	CMoveShape* pMouseShape;

	//目标坐标
	long lDestX;
	long lDestY;

	//当前触发的定时器id
	long lCurTimerID;
	//当前触发的事件类型
	long lCurEventType;
	//当前触发的事件参数
	tagProcVar *pEventVar;
	//当前执行的procedure序列号
	long nProceNum;
	//阶段耗时
	long lUseTime;
	//触发技能的物品ID
	long lGoodsIndex;
	//物品容器ID
	long lContainerID;
	//位置
	long lPos;
	//物品GUID
	CGUID GoodsGuid;
	//当前procedure作用的目标对象
	list<stDestObj*>	OperObjs;
	//作用类型
	 int	nPosition;
	 //元素类型
	 int nElement;

	//当前procedure正在使用的定时器
	typedef list<stTimerParam*>::iterator itTimer;
	list<stTimerParam*> Timers;

	//当前procedure注册的事件
	typedef list<stEventParam*>::iterator itEvent;
	list<stEventParam*> Events;

	//伤害值数据
	typedef map<long,long>::iterator itDamage;
	//目标伤害值列表
	map<long,long> DestDamages;
	//自己伤害值列表
	map<long,long> SelfDamages;
	//目标加血列表
	map<long,long> DestIncHP;
	//自己加血列表
	map<long,long> SelfIncHP;

	

	//发送消息给玩家的列表
	hash_set<CGUID,hash_guid_compare> SendMsgPlayers;

	//根据TimerID找到相应的参数
	stTimerParam* FindTimerParam(long lTimerID);
	//根据设置参数和作用目标寻找定时器参数
	stTimerParam* FindTimerParam(long lFlag,long lDestType,const CGUID& DestID);
	//定时触发
	void OnTimerOut(long lTimerID);
	//获取ID
	long				GetID()				{return nID;}		
	//获取等级	
	int					GetLv()				{return nLvl;}	
	//获取全局唯一标志符
	DWORD				GetGlobleID()		{return dwGlobleID;}  
	//类型
	ModuleType			GetModType()		{return eType;}		
	
	//容器ID
	void				SetContainerID(long lID){lContainerID = lID;}
	long				GetContainerID()		{return lContainerID;}
	//位置
	void				SetPos(long pos)		{lPos = pos;}		
	//物品guid
	void				SetGoodGuid(CGUID Guid){GoodsGuid=Guid;}
	//施放者类型
	int					GetFirerType()		{return nFirerType;}  
	//施放者guid
	CGUID				GetFirerID()		{return FirerID;}		
	//施放者
	CMoveShape			*GetFirer()			{return pFirer;}
	//阶段号
	long				GetProcNum()		{return nProceNum;}		
	
	//类别
	eSkillUseType		GetUseType()		{return eUseType;}	
	//元素类型	
	int					GetElemnet()		{return nElement;}	
	//当前触发的事件参数
	tagProcVar			*GetEventProc()		{return pEventVar;}
	//计时标志
	BOOL				GetTime()			{return bTime;}		


	
	

	DWORD				GetCreateID()		{return dwCreateID;}  //制作ID
	eAddSub				GetAddSub()			{return eAddSubType;}
	eAttack				GetAttack()			{return eTagAttack;}	//攻击标志符
	eTarget				GetTarget()			{return eTagTarget;}	//目标标志符
	bool				GetbRun()			{return bRun;}		//是否启动标志符


	long				GetGoodsIndex()		{return lGoodsIndex;} //物品ID
	
	
	long				GetPos()			{return lPos;}
	
	CVariableList*		GetVariableList()	{return pVariableList;}	//变量列表
	
	void				SetbRun(bool b)		{bRun = b;}			//运行标志符
	
	
	
	void				SetGoodsIndex(long l){lGoodsIndex = l;}

	//设置目标数据
	void SetDesObjs(long lMouseX,long lMouseY,long lMouseShapeType,const CGUID& ShapeID);
	void SetDestObjByEvent(CMoveShape* pShape)		{pMouseShape = pShape; }

	//键值
	void				SetGlobleID(DWORD dwGID)			{dwGlobleID=dwGID;}
	//脚本变量
	long GetScriptParam(int nPos);	
	//重新设置数据
	void Reset(CMoveShape *pOwner,CMoveShape *pFire);
	//设置冷却时间
	void SetCooldownTime();		
	//清除事件和定时器
	void ClearTimerAndEvent();
	//释放对象
	void ClearOperObj();
	//释放接口
	void Clear();

	void ProcNumSub();
	// 添加到CByteArray
	bool AddToByteArray(vector<BYTE>* pByteArray);
	// 添加到CByteArray为发送到客户段
	bool AddToByteArrayForClient(vector<BYTE>* pByteArray,ModuleType eType);
	// 解码
	bool DecordFromByteArray(long lProceNum,list<stTimerParam*>& Timers);

	// 添加到DataBlock
	bool CodeToDataBlock(DBWriteSet& setWriteDB);
	// 到添加到DataBlock为客户端
	bool CodeToDataBlockForClient(DBWriteSet& setWriteDB,ModuleType eType);
	// 解码
	bool DecodeFromDataBlock(long lProceNum,list<stTimerParam*>& lTimers);

};

enum eObj
{
	Obj_Self=0x1,//使用者自己
	Obj_Dest=0x2,//作用对象
	Obj_Mouse=0x3,//作用鼠标点
	Obj_Firer=0x4,//施放者
};

enum eModuleOper
{
	//加操作
	MODOP_Add,
	//减操作
	MODOP_Sub,
	//设置操作
	MODOP_Set,
	//清除
	MODOP_Clear,
	//加血
	MODOP_Inc,
};

typedef list<CMoveShape*>::iterator itObj;
typedef list<stDestObj*>::iterator itDestObj;

#endif
