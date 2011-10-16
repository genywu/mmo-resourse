#ifndef __GAME_EVENT__
#define __GAME_EVENT__
class CMessage;
#include "../public/GUID.h"

//定时类型
enum eChildObjType
{
	ChildObjType_AI,
	ChildObjType_Buff,
	ChildObjType_DeBuff,
	ChildObjType_ActiveSkill,
	ChildObjType_PassiveSkill,
	ChildObjType_OnOffSkill,
	ChildObjType_GoodsTimeout,	 // 限时物品超时
	ChildObjType_BuyLimitClear,
	ChildObjType_BuySellClear,
	ChildObjType_PriceUpdate,
	ChildObjType_CreateRgnQueue, // 玩家排队创建副本
	ChildObjType_CreatePRgn,	 // 玩家创建个人副本时间间隔
	ChildObjType_CreateTRgn,	 // 玩家创建组队副本时间间隔
	ChildObjType_DeleteRgn,		 // 场景管理器删除副本场景
	ChildObjType_DeleteObj,		 //	删除场景中OBJECT
	ChildObjType_ReBornObj,		//  场景object重生
	ChildObjType_AddObj,		//	加入object到场景
	ChildObjType_AddShapeToClient,//添加对象到客户端的定时器
	ChildObjType_InvestLimit,	 // 投资时间限制的解除
	ChildObjType_BusinessUpdate,
	ChildObjType_WSMsg,			 // WS的消息超时定时器
	ChildObjType_PlayerHint,	 // 提示帮助
	ChildObjType_RunScript,		 // 执行脚本
	ChildObjType_AreaHiberate,
	ChildObjType_DelGoods,
	ChildObjType_PKValue,		 //pk值
	ChildObjType_PvPValue,		 //恶名值
	ChildObjType_AutoRecoverHpMp,//恢复Hp,Mp
	ChildObjType_AutoRecoverEnergy,//恢复能量
	ChildObjType_Beneficiary,	 //受益者事件
	ChildObjType_UpdateMercQuest,//玩家刷新俑兵任务
	ChildObjType_Mail,			//邮件
	ChildObjType_SysMail,		//系统邮件;
	ChildObjType_Weather,		//场景天气
	ChildObjType_CollectionDied,//采集物死亡
	ChildObjType_UpdateAttr,	//更新属性定时器
	ChildObjType_RgnKickPlayerTime,	 // 玩家副本清人时间间隔
	COT_PET_EVENT,				// 宠物事件(子类型在PET中处理)
	ChildObjType_Mail_CheckTime,// 邮件定时检查
	ChildObjType_Fcm_KickPlayerTime,// 防沉迷系统踢玩家下线
	ChildObjType_ReliveTimer,//死亡复活倒计时
	ChildObjType_GoodsExTimeOut, /// 统一玩家物品定时器
	ChildObjType_ChangeMdTimeOut, //武器换装定时器
};

//#define _DEBUG_TIMER_VAR_


//定时器中(void* var)参数结构
struct tagTimerVar
{
private:
#ifdef _DEBUG_TIMER_VAR_
	tagTimerVar(long Flag):DebugFlag(Flag){lvar=0;lvar1=0;pvar=NULL;pvar_size=0;}
	long DebugFlag;
	~tagTimerVar(){}
#else
	tagTimerVar(){lvar=0;lvar1=0;pvar=NULL;pvar_size=0;}
#endif
	friend class CBaseObject;
public:	
	eChildObjType TimerType;
	long lvar;
	long lvar1;
	void *pvar;
	long pvar_size;
	
};

//事件类型
enum eEventType
{
	//移动
	ET_Move,
	//被攻击
	ET_BeAttack,
	//攻击
	ET_Attack,
	//伤害
	ET_Damage,
	//被伤害
	ET_BeenDamage,
	//装备
	ET_EquipMount,
	//卸装备
	ET_EquipUnmount,
	//玩家退出游戏
	ET_ExitGame,
	//离开场景
	ET_ExitRegion,
	//加状态事件
	ET_AddState,
	//使用技能事件
	ET_UseSkill,
	//玩家进入游戏
	ET_EnterGame,
	//转职
	ET_ChangeOccu,	
	//重设基本战斗属性
	ET_ResetBaseFightProperty,
	//交易
	ET_Trade,
	//开店
	ET_OpenShop,
	//杀死目标
	ET_Kill,
	//被杀死
	ET_BeenKilled,
	//爆击目标
	ET_Blast,
	//被爆击
	ET_BeenBlast,
	//格挡目标
	ET_Block,
	//被目标格挡
	ET_BeenBlocked,
	//闪避目标
	ET_Dodge,
	//被目标闪避
	ET_BeenDodged,
	//招架目标
	ET_Parry,
	//被招架
	ET_BeenParried,
	//抵抗目标
	ET_Resist,
	//被抵抗
	ET_BeenResisted,
	//进入战斗状态
	ET_Fighting,
	//进入和平状态
	ET_Peace,
	//发信事件
	ET_SendMail,
	//中断技能
	ET_BreakSkill,
	//状态中断
	ET_StateBreak,
	//事件中断
	ET_EventBreak,
	//进入场景事件
	ET_EnterRegion,
	//技能成功
	ET_SuccessSkill,
	//额外伤害
	ET_DamageEx,
	//额外伤害
	ET_BeenDamageEx,
	//归位事件
	ET_Homing,
	//DOTA伤害
	ET_SAttack,
	//被DOTA伤害
	ET_BeSAttack,
};


//事件注册参数
struct tagEvenRegVar
{
	long lEventID;
	//关心该事件的一级对象(player,monster,regiongood)
	long lType1;
	CGUID ID1;
	//关心该事件的二级对象(buff,skill,playergood)
	long lType2;
	long lID2;
	
	tagEvenRegVar()
		:lEventID(0),lType1(0),ID1(NULL_GUID),lType2(0),lID2(0)
	{
	}
	
	tagEvenRegVar(long EventID,long Type1,const CGUID& guid,long Type2,long ID2)
		:lEventID(EventID),lType1(Type1),ID1(guid),lType2(Type2),lID2(ID2)
	{
	}
};

//处理过程参数
struct tagProcVar
{
	//关心该事件的子对象
	long lType;
	long lID;
	
	//自定义参数
	long lVar1;
	long lVar2;
	long lVar3;
	long lVar4;
	void* pVar;
};


class CGameEvent
{
public:
	//网络消息响应
	virtual void OnMessage(CMessage*)=0;
	//超时响应
	virtual long OnTimeOut(DWORD timerid,DWORD curtime,const void* var)=0;
	//当定时器被取消的调用
	virtual void OnTimerCancel(DWORD timerid,const void* var)=0;
	//处理实体间消息函数
	virtual long Proc(eEventType type,const tagProcVar *pVar)=0;
};

#endif
