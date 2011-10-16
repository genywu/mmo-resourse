/** 
 *  @file   PhaseDef.h
 *  @author
 *
 *  @brief  提供客户和服务器进行目标跟随的操作定义
 *  @date   2010/03/18
 */

#pragma  once 

/// 跟随状态
enum  Follow_State
{
    Not_Follow_State,                   /// 正常行走状态
    In_Follow_State                     /// 跟随状态
};

/// 跟随请求
enum  TEAM_FOLLOW_TYPE
{
    TF_Team_Leader_Invite=1,            /// 队长邀请队友跟随队长
    TF_Team_Return_TeamFollow,          /// 队员回复是否进入组队跟随状态
    TF_Team_Member_Follow_Leader,       /// 队友请求主动跟随队长
    TF_Team_Stop_TeamFollow,            /// 队友停止跟随
    TF_Team_Member_ChangeRgn,           /// 队员跟随请求切换场景
};

/// 结果返回
enum TEAM_FOLLOW_RESULT
{
    TF_Result_InValid = 0,              /// 禁止跟随
    TF_Result_Not_Team,                 /// 没有队伍不能进行跟随
    TF_Result_Not_SameGS,               /// 不能跨GS寻路
    TF_Result_Not_Follow_Self,          /// 队长不能跟随自身
    TF_Result_Refuse_Follow,            /// 队员拒绝跟随队长
    TF_Result_Enter_Follow,             /// 队员进入组队跟随状态
    TF_Result_Stop_Follow,              /// 队员退出组队跟随状态
    TF_Result_Follow_Flag,              /// 通知客户端进入组队跟随状态的标记
    TF_Result_Too_Far,                  /// 离队长太远了（而不能跟随）
    TF_Error_In_Team_Follow,            /// 已经处于组队跟随状态（而不能做某些操作）
    TF_Errow_Not_In_Follow_Mode,        /// 队伍目前不处于跟随状态
    TF_Result_Follow_Success,           /// 组队跟随成功
};
