#include "stdafx.h"
#include "FactionPageOper.h"
#include "ClientRegion.h"
#include "BaseObject.h"
#include "Player.h"
#include "MsgBoxSys/MsgEventManager.h"
#include "../GameClient/Game.h"
#include "../../../ui/GamePage/ChatList/ChatListPage.h"

namespace  uFactionOper
{
    void  ExitFaction(MsgEvent& event,eMsgResult UserId)
    {
        if( UserId  !=  S_Ok ) return ;

        if ( GetGame()->GetFactionManager()->GetMyJob() == -1 )
        {
            GetGame()->GetFactionManager()->SendDisbandMsg();
            return;
        }   
        GetGame()->GetFactionManager()->SendQuitMsg();
    }

    void  JoinFaction(MsgEvent& event,eMsgResult UserId)
    {
        CGUID * guid = static_cast<CGUID*>(event.pData);
        if( UserId  !=  S_Ok ) 
        { 
            SAFE_DELETE( guid );
            return ;
        }

        CMessage msg( MSG_C2S_ORGSYS_JoinFac_Request );
        msg.Add( (long)1 );
        msg.Add( *guid );
        msg.Send();

        SAFE_DELETE( guid );
    }

    void FactionInviteAnswer(MsgEvent& event,eMsgResult UserId)
    {
        FactionOper::SInviterInfo*	pst = static_cast<FactionOper::SInviterInfo*>(event.pData); 
        if ( UserId != S_Ok )
        {
            SAFE_DELETE( pst );
            return;
        }          

        /// 同意加入 


    }

}

FactionOper::FactionOper()
{
    m_pFactionManager = GetGame()->GetFactionManager();
    m_IsOpen = m_Factionlist = false;
}
FactionOper::~FactionOper()
{
    m_Factionlist = false;
}

void FactionOper::Switch()
{
    CPlayer* player = GetGame()->GetMainPlayer();
    if ( !player)
        return ;

    // "您尚未加入家族！"
    if( !m_pFactionManager->AmIJoinedFaction() )
    {
        GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("FA_301")); //您尚未加入行会！
        return ;
    }
    m_IsOpen = !m_IsOpen;
    FireUIEvent("FactionPage",EVENT_SWITCH);
    if ( m_IsOpen )
    {
        m_pFactionManager->SendBaseInfoRequestMsg();
    }        
}

void  FactionOper::RecvCreateMsg(CMessage * pMsg)
{
    // 检测 成功失败
    if( PLAYER_OPT_SUCCEED == pMsg->GetLong() )
    {
        CGUID  facGuid;
        pMsg->GetGUID( facGuid );
        GetGame()->GetMainPlayer()->SetFactionID( facGuid );

        // 设置官阶,会长
        m_pFactionManager->SetMyJob( Exceptive_JobLvl );

        // "帮会创建成功！"
        GetInst(MsgEventManager).PushEvent(Msg_Ok,"创建帮会成功!");
    }
    else
    {
        // 处理错误
        m_pFactionManager->HandleErrorMsg( pMsg );
    }
}

void  FactionOper::ExitFaction()
{
    const char* pStr = NULL;AppFrame::GetText("FA_401"); //您确定要解散行会吗？
    if ( GetGame()->GetFactionManager()->GetMyJob() != -1 )
    {
        pStr = AppFrame::GetText("FA_402");      //您确定要退出行会吗？
    }
    else
    {
        pStr = AppFrame::GetText("FA_eIDS_FAC_AreYouSureQuitFac");
    }

    /// 确定退出帮会
    GetInst(MsgEventManager).PushEvent(Msg_YesNo,pStr,uFactionOper::ExitFaction);
}

void  FactionOper::JoinFactionList(const CGUID& guid)
{   
    CGUID * id = new CGUID(guid.tostring());
    GetInst(MsgEventManager).PushEvent(Msg_YesNo,AppFrame::GetText("FA_409"),uFactionOper::JoinFaction,id);//307
}

void  FactionOper::OpenCountryFaction()
{
     m_Factionlist =!m_Factionlist;
     if (m_Factionlist)
     {
         m_pFactionManager->SendFactionList();
     }
}

void  FactionOper::RecvApplyRestMsg(CMessage* pMsg)
{
    // 检测 成功失败
    if( PLAYER_OPT_SUCCEED == pMsg->GetLong() )
    {
        // 申请还是取消申请
        if( pMsg->GetLong() )
        {
            // 操作的家族GUID
            CGUID	guidFaction;
            pMsg->GetGUID( guidFaction );

            // "您的家族申请已提交！请等待审批。。。"
            GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("FA_313"));
        }
    }
    else
    {
        // 处理错误
        m_pFactionManager->HandleErrorMsg( pMsg );
    }
}
void  FactionOper::RecvQuitMsg( CMessage* pMsg )
{
    // 检测 成功失败
    if( PLAYER_OPT_SUCCEED == pMsg->GetLong() )
    {
    }
    else
    {
        // 处理错误
        m_pFactionManager->HandleErrorMsg( pMsg );
    }
}

void  FactionOper::RecvQuitNotifyMsg(CMessage* pMsg)
{
    // 退出族员的家族 GUID
    CGUID	guidFaction;
    pMsg->GetGUID( guidFaction );

    // 退出族员的 GUID
    CGUID	guidMember;
    pMsg->GetGUID( guidMember );

    // 退出的族员 名字
    TCHAR	nameMember[ MAX_MEMBER_NAME_SIZE ];
    pMsg->GetStr( nameMember, MAX_MEMBER_NAME_SIZE );

    // 自己家族的GUID
    CGUID	guidMyFaction = GetGame()->GetMainPlayer()->GetFactionID();

    // 遍历处理场景内所有的玩家
    std::list<CBaseObject*>::iterator p = GetGame()->GetRegion()->GetObjectList()->begin();
    for( ; p != GetGame()->GetRegion()->GetObjectList()->end(); p++ )
    {
        if( (*p)->GetType() == TYPE_PLAYER )
        {
            CPlayer* pPlayer = dynamic_cast<CPlayer*>(*p);

            // 将退出族员的家族信息清除
            if( pPlayer->GetExID() == guidMember )
            {
                // 清空自己的家族标志信息
                pPlayer->SetFactionID( NULL_GUID );
                pPlayer->SetUnionID( NULL_GUID );
                pPlayer->SetFactionName( std::string() );
                pPlayer->SetFactionTitle( std::string() );

                break;
            }
        }
    }
    // 如果不是自己家族的，则返回
    if( guidMyFaction != guidFaction )
        return;
    // 处理自己
    if( GetGame()->GetMainPlayer()->GetExID() == guidMember )
    {
        FireUIEvent("FactionPage","Closed");

        // "您已经退出家族！"
        //GetGame()->GetAudioList()->Play2DSound("SOUNDS\\interfaces\\i-31.wav");
        // "您的家族申请已提交！请等待审批。。。"
        GetInst(MsgEventManager).PushEvent(Msg_Ok,AppFrame::GetText("FA_307")); //您已经退出帮会！
    }
}

void  FactionOper::PushApplyInfo(const CGUID& guid,BOOL Agree)
{
     m_pFactionManager->SendCheckRequisitionMsg( Agree ,guid );
}

void  FactionOper::RecvCheckRequisitionMsg(CMessage* pMsg)
{
    // 检测 成功失败
    if( PLAYER_OPT_SUCCEED == pMsg->GetLong() )
    {
        // "家族以及批准了你的加入申请！"
        GetInst(MsgEventManager).PushEvent(Msg_Ok,"家族以及批准了你的加入申请");
    }
    else
    {
        // 处理错误
        m_pFactionManager->HandleErrorMsg( pMsg );
    }
}

void  FactionOper::RecvCheckRequisitionMsgNotify(CMessage* pMsg)
{
    // 家族GUID
    CGUID	guidFaction;
    pMsg->GetGUID( guidFaction );

    // true添加，false删除
    long	bAdd = pMsg->GetLong();

    if( CFaction* pMyFaction = m_pFactionManager->GetMyFaction() )
    {
        if( bAdd )
        {
            //申请者结构
            tagFacApplyPerson	stFacApplyPerson;
            pMsg->GetEx( &stFacApplyPerson, sizeof(stFacApplyPerson) );

            // 进行添加
            CFacMember::SMemberInfo	stMemberInfo;
            stMemberInfo.guidFac = GetGame()->GetMainPlayer()->GetFactionID();
            stMemberInfo.guidMember = stFacApplyPerson.PlayerGuid;
            stMemberInfo.strName = stFacApplyPerson.szName;
            stMemberInfo.lLevel = stFacApplyPerson.lLvl;
            stMemberInfo.lOccupation = stFacApplyPerson.lOccu;

            pMyFaction->AddApply( &stMemberInfo );
        }
        else
        {
            // 申请者GUID
            CGUID	guidRequisition;
            pMsg->GetGUID( guidRequisition );

            // 删除
            pMyFaction->DelApply( guidRequisition );
        }
    }
    FireUIEvent("FactionPage","UpdateFacAppList");
}

void FactionOper::RecvJoindNotifyMsg(CMessage* pMsg)
{
    // 族员结构
    tagFacMemInfo	stFacMemInfo;
    pMsg->GetEx( &stFacMemInfo, sizeof(stFacMemInfo) );
    CGUID           FactionGuid;
    pMsg->GetGUID( FactionGuid );

    // 添加一个族员
    CFacMember::SMemberInfo	stMemberInfo;
    stMemberInfo.guidFac = FactionGuid;
    stMemberInfo.guidMember = stFacMemInfo.MemberGuid;
    stMemberInfo.strName = stFacMemInfo.szName;
    stMemberInfo.strTitle = stFacMemInfo.szTitle;
    stMemberInfo.lLevel = stFacMemInfo.lLvl;
    stMemberInfo.lJob = stFacMemInfo.lJobLvl;
    stMemberInfo.lOccupation = stFacMemInfo.lOccu;
    stMemberInfo.lContribute = stFacMemInfo.lContribute;
    stMemberInfo.bOnline = stFacMemInfo.bIsOnline != 0;
    stMemberInfo.strRegion = stFacMemInfo.szRegion;

    
    m_pFactionManager->AddMyConfrere( &stMemberInfo );

    // 是否是自己
    if( GetGame()->GetMainPlayer()->GetExID() == stFacMemInfo.MemberGuid )
    {
        GetGame()->GetMainPlayer()->SetFactionID(FactionGuid);

        m_pFactionManager->SetMyJob( stFacMemInfo.lJobLvl );

        // "您已经加入家族： %s"
        TCHAR	szPrompt[ 1024 ];
        wsprintf( szPrompt, AppFrame::GetText("FA_308"), GetGame()->GetMainPlayer()->GetFactionName().c_str() );
        GetInst(MsgEventManager).PushEvent(Msg_Ok,szPrompt);
    }

    FireUIEvent("FactionPage","UpdateFacAppList");
}

void  FactionOper::RecvKickNotifyMsg(CMessage* pMsg)
{
    TCHAR	strPrompt[ 1024 ];

    // 踢出族员的家族 GUID
    CGUID	guidFaction;
    pMsg->GetGUID( guidFaction );

    // 被踢的族员 GUID
    CGUID	guidMember;
    pMsg->GetGUID( guidMember );

    // 被踢的族员 名字
    TCHAR	nameMember[ MAX_MEMBER_NAME_SIZE ];
    pMsg->GetStr( nameMember, MAX_MEMBER_NAME_SIZE );

    // 自己家族的GUID
    CGUID	guidMyFaction = GetGame()->GetMainPlayer()->GetFactionID();

    // 遍历处理场景内所有的玩家
    for( std::list<CBaseObject*>::iterator p = GetGame()->GetRegion()->GetObjectList()->begin(); p != GetGame()->GetRegion()->GetObjectList()->end(); p++ )
    {
        if( (*p)->GetType() == TYPE_PLAYER )
        {
            CPlayer* pPlayer = dynamic_cast<CPlayer*>(*p);

            // 将退出族员的家族信息清除
            if( pPlayer->GetExID() == guidMember )
            {
                // 清空自己的家族标志信息
                pPlayer->SetFactionID( NULL_GUID );
                pPlayer->SetUnionID( NULL_GUID );
                pPlayer->SetFactionName( std::string() );
                pPlayer->SetFactionTitle( std::string() );

                break;
            }
        }
    }

    // 如果不是自己家族的，则返回
    if( guidMyFaction != guidFaction )
        return;

    // 看是否是自己被踢
    if( GetGame()->GetMainPlayer()->GetExID() == guidMember )
    {   
        // 关闭界面
        FireUIEvent("FactionPage","Closed");

        // "您已经被踢出家族！"
        wsprintf( strPrompt, AppFrame::GetText("FA_304"), GetGame()->GetMainPlayer()->GetFactionName().c_str() ); //您已经被踢出帮会！
        GetInst(MsgEventManager).PushEvent(Msg_Ok,strPrompt);

    }
    // 处理会友
    else if( GetGame()->GetMainPlayer()->GetFactionID() == guidFaction )
    {
        // "%s 已经被踢出家族！"
        wsprintf( strPrompt, AppFrame::GetText("FA_111"), nameMember );   //%s 已经被踢出行会！
        //GetMainBar()->AddOtherInfo( strPrompt );
        GetInst(ChatListPage).AddChatWords(strPrompt,0xffffffff,0,0xff000000,"",CHAT_BOARDCAST);

        // 删除
        m_pFactionManager->GetMyFaction()->DelMember( guidMember );
        FireUIEvent("FactionPage","UpdateMem");
    }
}

void FactionOper::RecvKickMsg(CMessage* pMsg)
{
    // 检测 成功失败
    if( PLAYER_OPT_SUCCEED == pMsg->GetLong() )
    {
        // 被踢的族员 GUID
        CGUID	guidMember;
        pMsg->GetGUID( guidMember );

        //
        if( CFacMember* pMember = m_pFactionManager->GetMyConfrere( guidMember ) )
        {
            // "%s 已经被踢出本家族！"
            TCHAR	strPrompt[ 256 ];
            wsprintf( strPrompt, AppFrame::GetText("FA_107"), pMember->GetName());  //%s 已经被踢出本帮会！
            GetInst(ChatListPage).AddChatWords(strPrompt,0xffffffff,0,0xff000000,"",CHAT_SYSTEM);
        }
    }
    else
    {
        // 处理错误
        m_pFactionManager->HandleErrorMsg( pMsg );
    }
}

void  FactionOper::RecvDisbandMsg(CMessage* pMsg)
{
    // 检测 成功失败
    if( PLAYER_OPT_SUCCEED == pMsg->GetLong() )
    {
        // 关闭界面
        FireUIEvent("FactionPage","Closed");

        // "您所在家族已经解散！"
        const char* pStr =AppFrame::GetText("FA_309") ;
        GetInst(ChatListPage).AddChatWords(pStr,0xffffffff,0,0xff000000,"",CHAT_SYSTEM);

        // 清除该家族的信息
        m_pFactionManager->DelFaction( GetGame()->GetMainPlayer()->GetFactionID() );
    }
    else
    {
        // 处理错误
        m_pFactionManager->HandleErrorMsg( pMsg );
    }
}

void  FactionOper::RecvDisbandNotifyMsg(CMessage* pMsg)
{
    // 解散的家族 GUID
    CGUID	guidFaction;
    pMsg->GetGUID( guidFaction );

    // 处理自己
    if( m_pFactionManager->AmIJoinedFaction( guidFaction ) )
    {
        // 关闭界面
        FireUIEvent("FactionPage","Closed");

        // "您所在的家族已经解散！"
        const char* pStr =AppFrame::GetText("FA_309") ;
        GetInst(ChatListPage).AddChatWords(pStr,0xffffffff,0,0xff000000,"",CHAT_SYSTEM);   
    }

    // 清除该家族的信息
    m_pFactionManager->DelFaction( guidFaction );
}

void  FactionOper::RecvMemberOnlineNotifyMsg(CMessage* pMsg)
{
    // 族员GUID
    CGUID	guidMember;
    pMsg->GetGUID( guidMember );

    bool bIsOnline = pMsg->GetLong() != 0;

    // 保存设置
    if( CFacMember*	pMember = m_pFactionManager->GetMyConfrere( guidMember ) )
    {
        pMember->SetOnline( bIsOnline );
    }
    FireUIEvent("FactionPage","UpdateMem");
}

void  FactionOper::RecvMemberMapNotifyeMsg(CMessage *pMsg)
{
    // 被设置官阶的族员GUID
    CGUID	guidMember;
    pMsg->GetGUID( guidMember );

    // 地图
    TCHAR	szRegion[ MAX_MEMBER_NAME_SIZE ];
    pMsg->GetStr( szRegion, MAX_MEMBER_NAME_SIZE );

    // 保存设置
    if( CFacMember*	pMember = m_pFactionManager->GetMyConfrere( guidMember ) )
    {
        pMember->SetRegion( szRegion );
    }
    FireUIEvent("FactionPage","UpdateMem");
}

void  FactionOper::RecvSetChairmanMsg(CMessage* pMsg)
{
    // 检测 成功失败
    if( PLAYER_OPT_SUCCEED == pMsg->GetLong() )
    {
        m_pFactionManager->SetMyJob( 0 );
        FireUIEvent("FactionPage","UpdateMem");
    }
    else
    {
        // 处理错误
        m_pFactionManager->HandleErrorMsg( pMsg );
    }
}

void  FactionOper::RecvSetChairmanNotifyMsg(CMessage* pMsg)
{
    TCHAR	szPrompt[ 256 ];

    // 老族长 名字
    TCHAR	nameOldMaster[ MAX_MEMBER_NAME_SIZE ];
    pMsg->GetStr( nameOldMaster, MAX_MEMBER_NAME_SIZE );

    // 新族长 名字
    TCHAR	nameNewMaster[ MAX_MEMBER_NAME_SIZE ];
    pMsg->GetStr( nameNewMaster, MAX_MEMBER_NAME_SIZE );

    // 如果我是老族长
    if( ! lstrcmp( GetGame()->GetMainPlayer()->GetName(), nameOldMaster ) )
    {
        // "您已经将族长职位转让给 %s！"
        wsprintf( szPrompt, AppFrame::GetText("FA_104") , nameNewMaster );
        GetInst(MsgEventManager).PushEvent(Msg_Ok,szPrompt);

        // 修改官阶
        m_pFactionManager->SetMyJob( 0 );
    }
    // 如果我是新族长
    else if( ! lstrcmp( GetGame()->GetMainPlayer()->GetName(), nameNewMaster ) )
    {
        // "%s 任命您为 %s 的新族长！"
        wsprintf( szPrompt, AppFrame::GetText("FA_302") , nameOldMaster, m_pFactionManager->GetMyFaction()->GetName() );
        GetInst(MsgEventManager).PushEvent(Msg_Ok,szPrompt);

        // 设置作为族长的相关数据
        m_pFactionManager->SetMyJob( Exceptive_JobLvl );
    }
    // 如果是我的会友
    else if( m_pFactionManager->GetMyFaction() )
    {
        // "%s 任命 %s 为 %s 的新族长！"
        wsprintf( szPrompt, AppFrame::GetText("FA_303") , nameOldMaster, m_pFactionManager->GetMyFaction()->GetName() );
        GetInst(ChatListPage).AddChatWords(szPrompt,0xffffffff,0,0xff000000,"",CHAT_SYSTEM);
    }

    // 更新 Member 数据
    if( CFacMember* pMember = m_pFactionManager->GetMyConfrere( nameOldMaster ) )
    {
        pMember->SetJob( 0 );
    }

    if( CFacMember* pMember = m_pFactionManager->GetMyConfrere( nameNewMaster ) )
    {
        pMember->SetJob( Exceptive_JobLvl );
    }

    // 刷新界面
    FireUIEvent("FactionPage","UpdateMem");
}

void  FactionOper::RecvMemberLevelNotifyMsg(CMessage* pMsg)
{
    // 被设置官阶的族员GUID
    CGUID	guidMember;
    pMsg->GetGUID( guidMember );

    long lLevel = pMsg->GetLong();

    // 保存设置
    if( CFacMember*	pMember = m_pFactionManager->GetMyConfrere( guidMember ) )
    {
        pMember->SetLevel( lLevel );
    }

    FireUIEvent("FactionPage","UpdateMem");
}

void  FactionOper::RecvMemberOccuNotifyMsg(CMessage* pMsg)
{
    // 被设置官阶的族员GUID
    CGUID	guidMember;
    pMsg->GetGUID( guidMember );

    long lOccu = pMsg->GetLong();

    // 保存设置
    if( CFacMember*	pMember = m_pFactionManager->GetMyConfrere( guidMember ) )
    {
        pMember->SetOccupation( lOccu );
    }

    // 刷新
    FireUIEvent("FactionPage","UpdateMem");
}

void  FactionOper::RecvOpenMsg(CMessage* pMsg)
{

}

void  FactionOper::RecvUpdatePlayerFacInfoMsg(CMessage* pMsg)
{
    // 玩家 GUID
    CGUID guidPlayer;
    pMsg->GetGUID( guidPlayer );

    // 所在家族 GUID
    CGUID guidFaction;
    pMsg->GetGUID( guidFaction );

    // 玩家家族称号
    TCHAR szFacTitle[ MAX_MEMBER_NAME_SIZE ];
    pMsg->GetStr( szFacTitle, MAX_MEMBER_NAME_SIZE );

    // 遍历处理场景内所有的玩家
    for( std::list<CBaseObject*>::iterator p = GetGame()->GetRegion()->GetObjectList()->begin(); p != GetGame()->GetRegion()->GetObjectList()->end(); p++ )
    {
        if( (*p)->GetType() == TYPE_PLAYER )
        {
            CPlayer* pPlayer = dynamic_cast<CPlayer*>(*p);

            // 处理更新的玩家
            if( pPlayer->GetExID() == guidPlayer )
            {
                pPlayer->SetFactionID( guidFaction );
                pPlayer->SetFactionTitle( std::string( szFacTitle ) );
                pPlayer->SetFactionName( std::string( m_pFactionManager->GetFaction( guidFaction )->GetName() ) );
            }
        }
    }

    // 刷新界面
    FireUIEvent("FactionPage","UpdateMem");
}

void  FactionOper::RecvInviteRequestMsg(CMessage* pMsg)
{
    // 
    TCHAR	szPrompt[ 256 ];

    // 检测 成功失败
    if( PLAYER_OPT_SUCCEED == pMsg->GetLong() )
    {
        TCHAR	namePlayer[ MAX_MEMBER_NAME_SIZE ];
        pMsg->GetStr( namePlayer, MAX_MEMBER_NAME_SIZE );

        // "%s 已经同意了您的入会邀请"
        wsprintf( szPrompt, AppFrame::GetText("FA_108" ) ,namePlayer);
        GetInst(ChatListPage).AddChatWords(szPrompt,0xffffffff,0,0xff000000,"",CHAT_SYSTEM);

    }
    else
    {
        // 处理错误
        m_pFactionManager->HandleErrorMsg( pMsg );
    }
}

void  FactionOper::RecvInviteAnswerMsg(CMessage* pMsg)
{
    SInviterInfo*	pstInviterInfo = new SInviterInfo;

    // 邀请者GUID
    pMsg->GetGUID( pstInviterInfo->guidInviter );

    // 邀请者名字
    pMsg->GetStr( pstInviterInfo->nameInviter, MAX_MEMBER_NAME_SIZE );

    // 家族GUID
    pMsg->GetGUID( pstInviterInfo->guidFaction );

    // 家族名字
    pMsg->GetStr( pstInviterInfo->nameFaction, MAXSIZE_FactionName );

    // "%s 邀请您加入家族：%s"
    TCHAR	strPrompt[ 1024 ];
    wsprintf( strPrompt, AppFrame::GetText("FA_106"), pstInviterInfo->nameInviter, pstInviterInfo->nameFaction );
    GetInst(MsgEventManager).PushEvent(Msg_Agree,strPrompt,uFactionOper::FactionInviteAnswer,pstInviterInfo);
}

void  FactionOper::CreateFactionUi()
{
    // "您尚未加入家族！"
    if( !m_pFactionManager->AmIJoinedFaction() )
    {
        FireUIEvent("CreateFaction","Opened");
        return ;
    }
    else
    {

    }
    
}