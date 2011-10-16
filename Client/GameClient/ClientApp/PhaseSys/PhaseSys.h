/**
 *  @brief 提供客户端相位的消息操作模块
 *  @file  PhaseSys.h
 */

#pragma  once 

#include "../../../../Public/Common/PhaseDef.h"

class  CMessage;
class  CPlayer;
class  PhaseSys
{
public:
    ///
    /// 关于相位消息的处理
    ///
    void  PhaseMsgPro(CMessage* pMsg);

    ///
    /// 关于相位中玩家的主动行为操作。
    /// @param Oper     C2SPhaseOperType
    /// 
    void  PhaseOper(CPlayer* player,C2SPhaseOperType Oper);

    ///
    /// 关于相位中玩家的主动行为操作。
    /// @param first    creator
    /// @param second   Inviteer
    /// @param Oper     C2SPhaseOperType
    /// 
    void  PhaseOper(const CGUID& first,const CGUID& second,C2SPhaseOperType Oper);
};

