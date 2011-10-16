/** 
 *  @file   PhaseDef.h
 *  @author
 *
 *  @brief  关于相位功能的客户端与服务器的一些枚举和属性定义
 *  @date   2010/03/18
 */

#pragma  once 

enum   C2SPhaseOperType
{
    C2S_Invite=0,               /// 邀请者发出邀请请求
    C2S_Invitee,                /// 被邀请者同意加入Pk相位
    C2S_Leave,                  /// 离开
    C2S_Escape,                 /// 主动逃跑
    C2S_ReJoin,                 /// 主动归队
    S2C_FollowLeader,           /// 跟随队长请求(锁定)
};

enum   S2CPhaseOperType
{
    S2C_ChangeRgn,              /// 场景切换
    S2C_ChangeState,            /// 状态改变
    S2C_EnterPhase,             /// 进入相位
    S2C_LeavePhase,             /// 离开相位
    S2C_InVitee,                /// 邀请加入相位
    S2C_JoinTimeOut,            /// 加入超时
    
    S2C_ReInvite,               /// 重复邀请
    S2C_TooFar,                 /// 太远
    S2C_RefuseJoin,             /// 拒绝加入相位
};

/// 触发器类型
enum eTrigger
{   
    eUnAct,                     /// 被动触发
    eActive,                    /// 主动触发   
};

/// 相位类型
enum  ePhaseType
{
    eInValid = -1,               
    eFight,                     /// 野战的相位
    ePk                         /// Pk相位
};

/// 相位状态
enum  ePhaseState
{
    eRun,                       /// 运行中
    eEnd,                       /// 相位结束
    eClose                      /// 相位退出
};

/// 相位事件
enum  ePhaseEvent
{
    eExit=1,                   /// 正常结束事件触发，可能是通过杀死某个怪物或者完成某件事情
    eDie,                      /// 正常死亡
    eSecKill,                  /// 秒杀
    eAgonal,                   /// 玩家濒死状态
};


/// 相位怪物死亡事件
enum  ePhaseMOper
{
    eMonsterDie,
    eAddToPhase,
};

/// 相位中存在的基本操作
enum   ePhaseOper
{
    eRemit=1,                 /// 传送
    eLeave,                   /// 离开(离队或者队长驱逐) ，退出游戏都是
    eEscape,                  /// 逃跑
    eReJoin,                  /// 归队操作
    eReceive,                 /// 接受Pk相位的操作
};


/// 计数器类型
enum   eTimerType
{
    eAdd    =1,
    eSub
};

/// 脚本操作类型  
enum   eWScriptType
{
    eSc_Oper = 1,
    eSc_Ttype,
    eSc_Event
};
