/**
 *  @file  FactionPageOper.h
 *  @brief about a faction page oper class 
 */

#pragma  once
#include "Faction.h"

class  CMessage;
class  CGUID;
class  FactionOper
{

public:
    // 邀请者信息结构
    struct	SInviterInfo
    {
        CGUID		guidInviter;										// 邀请者GUID
        CHAR		nameInviter[ MAX_MEMBER_NAME_SIZE ];				// 邀请者名字
        CGUID		guidFaction;										// 家族GUID
        TCHAR		nameFaction[ MAXSIZE_FactionName ];					// 家族名字
    };

public:
    FactionOper() ;
    ~FactionOper();

    /// 界面操作
    void  Switch(); 

    void  SetVisible(bool vis=false)   { m_IsOpen = vis; }

    void  ExitFaction();

    void  OpenCountryFaction();

    void  JoinFactionList(const CGUID& guid);

    void  PushApplyInfo(const CGUID& guid,BOOL Agree=FALSE);

    void  CreateFactionUi();

    /// 消息
    void  RecvCreateMsg(CMessage * pMsg);

    void  RecvApplyRestMsg(CMessage* pMsg);

    void  RecvQuitNotifyMsg(CMessage* pMsg);

    void  RecvQuitMsg( CMessage* pMsg);

    void  RecvCheckRequisitionMsg(CMessage* pMsg);

    void  RecvCheckRequisitionMsgNotify(CMessage* pMsg);

    void  RecvJoindNotifyMsg(CMessage* pMsg);

    void  RecvKickNotifyMsg(CMessage* pMsg);

    void  RecvKickMsg(CMessage* pMsg);

    void  RecvDisbandMsg(CMessage* pMsg);

    void  RecvDisbandNotifyMsg(CMessage* pMsg);

    void  RecvMemberOnlineNotifyMsg(CMessage* pMsg);

    void  RecvMemberMapNotifyeMsg(CMessage* pMsg);

    void  RecvSetChairmanMsg(CMessage* pMsg);

    void  RecvSetChairmanNotifyMsg(CMessage* pMsg);

    void  RecvMemberLevelNotifyMsg(CMessage* pMsg);
    
    void  RecvMemberOccuNotifyMsg(CMessage* pMsg);

    void  RecvOpenMsg(CMessage* pMsg);

    void  RecvUpdatePlayerFacInfoMsg(CMessage* pMsg);

    void  RecvInviteRequestMsg(CMessage* pMsg);

    void  RecvInviteAnswerMsg(CMessage* pMsg);

private:
    // 家族逻辑管理对象
    CFactionManager*	m_pFactionManager;
    bool                m_IsOpen;
    bool                m_Factionlist;
};

