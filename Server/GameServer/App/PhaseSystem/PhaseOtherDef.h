#pragma  once 

#ifdef _GAME_SERVER
#define PHASE_LOG_FILE "PhaseSysGS"
#elif defined _WORLD_SERVER_
#define PHASE_LOG_FILE "PhaseSysWS"
#endif

#define  PhaseTimer     600

//#define PRINT_PHASE_TO_CLIENT(__strLog,Host) \
//    GetGame()->SendNotifyMessage(__strLog, Host, NOTIFY_COLOR_RED); \
//    PutoutLog(PHASE_LOG_FILE, LT_ERROR, __strLog)
/***********************************************************************/
/*  zhaohang 2010-11-8 fix 直接将颜色加入到字符串中*/
/***********************************************************************/
#define PRINT_PHASE_TO_CLIENT(__strLog,Host) \
	GetGame()->SendNotifyMessage(__strLog, Host); \
    Log4c::Error(ROOT_MODULE,"%-15s %s",__FUNCTION__,__strLog)

#define DisPlay(s)   Displays(s)
#define Displays(s)  #s

#define   SEND_PHASEMSG(Type, Oper, FirstID )\
    CMessage msg(Type); \
    msg.Add( (long)Oper );  \
    msg.Add( FirstID )

#define   SEND_PHASEMSG_ID(Type, Oper, FirstID, SecondId, Ret) \
    SEND_PHASEMSG(Type,Oper,FirstID);\
    msg.Add( SecondId );\
    msg.Add( (long)Ret)

#define   SendToAddWS(Type, Oper, FirstID, SecondId, Ret ) \
  {\
    SEND_PHASEMSG_ID(Type,Oper,FirstID,SecondId,Ret);\
    msg.SendToSocket( CMessage::GetWSSocket() );\
  } 

#define   SendToRemoveWS(Type, Oper, FirstID )\
  {\
    SEND_PHASEMSG(Type,Oper,FirstID);\
    msg.SendToSocket( CMessage::GetWSSocket() );\
  } 

#define   SendToClientPlayer( Type, Oper, FirstID, Phase, Ret)\
  {\
    SEND_PHASEMSG(Type,Oper,Phase->GetID());\
    msg.Add(Phase->m_Place.x_point);\
    msg.Add(Phase->m_Place.y_point);\
    msg.Add(Phase->m_Place.a_size);\
    msg.Add(Phase->m_Place.a_size);\
    msg.SendToPlayer( FirstID );\
  }
#define   SendToClinetAround( Type, Oper,PhaseID,Main) \
  {\
       SEND_PHASEMSG(Type,Oper,PhaseID);\
       msg.Add(Main->GetExID());\
       msg.SendToAround(Main);\
  }